/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains C code routines that are called by the parser
** to handle DELETE FROM statements.
**
** $Id$
*/
#include "sqliteInt.h"


/*
** Given a table name, find the corresponding table and make sure the
** table is writeable.  Generate an error and return NULL if not.  If
** everything checks out, return a pointer to the Table structure.
*/
Table *sqliteTableNameToTable(Parse *pParse, const char *zTab){
  Table *pTab;
  pTab = sqliteFindTable(pParse->db, zTab);
  if( pTab==0 ){
    sqliteSetString(&pParse->zErrMsg, "no such table: ", zTab, 0);
    pParse->nErr++;
    return 0;
  }
  if( pTab->readOnly || pTab->pSelect ){
    sqliteSetString(&pParse->zErrMsg, 
      pTab->pSelect ? "view " : "table ",
      zTab,
      " may not be modified", 0);
    pParse->nErr++;
    return 0;      
  }
  return pTab;
}

/*
** Given a table name, check to make sure the table exists, is writable
** and is not a view.  If everything is OK, construct an SrcList holding
** the table and return a pointer to the SrcList.  The calling function
** is responsible for freeing the SrcList when it has finished with it.
** If there is an error, leave a message on pParse->zErrMsg and return
** NULL.
*/
SrcList *sqliteTableTokenToSrcList(Parse *pParse, Token *pTableName){
  Table *pTab;
  SrcList *pTabList;

  pTabList = sqliteSrcListAppend(0, pTableName);
  if( pTabList==0 ) return 0;
  assert( pTabList->nSrc==1 );
  pTab = sqliteTableNameToTable(pParse, pTabList->a[0].zName);
  if( pTab==0 ){
    sqliteSrcListDelete(pTabList);
    return 0;
  }
  pTabList->a[0].pTab = pTab;
  return pTabList;
}

/*
** Process a DELETE FROM statement.
*/
void sqliteDeleteFrom(
  Parse *pParse,         /* The parser context */
  Token *pTableName,     /* The table from which we should delete things */
  Expr *pWhere           /* The WHERE clause.  May be null */
){
  Vdbe *v;               /* The virtual database engine */
  Table *pTab;           /* The table from which records will be deleted */
  char *zTab;            /* Name of the table from which we are deleting */
  SrcList *pTabList;     /* A fake FROM clause holding just pTab */
  int end, addr;         /* A couple addresses of generated code */
  int i;                 /* Loop counter */
  WhereInfo *pWInfo;     /* Information about the WHERE clause */
  Index *pIdx;           /* For looping over indices of the table */
  int base;              /* Index of the first available table cursor */
  sqlite *db;            /* Main database structure */
  int openOp;            /* Opcode used to open a cursor to the table */

  int row_triggers_exist = 0;
  int oldIdx = -1;

  if( pParse->nErr || sqlite_malloc_failed ){
    pTabList = 0;
    goto delete_from_cleanup;
  }
  db = pParse->db;

  /* Check for the special case of a VIEW with one or more ON DELETE triggers 
  ** defined 
  */
  zTab = sqliteTableNameFromToken(pTableName);
  if( zTab != 0 ){
    pTab = sqliteFindTable(pParse->db, zTab);
    if( pTab ){
      row_triggers_exist = 
        sqliteTriggersExist(pParse, pTab->pTrigger, 
            TK_DELETE, TK_BEFORE, TK_ROW, 0) ||
        sqliteTriggersExist(pParse, pTab->pTrigger, 
            TK_DELETE, TK_AFTER, TK_ROW, 0);
    }
    sqliteFree(zTab);
    if( row_triggers_exist &&  pTab->pSelect ){
      /* Just fire VIEW triggers */
      sqliteViewTriggers(pParse, pTab, pWhere, OE_Replace, 0);
      return;
    }
  }

  /* Locate the table which we want to delete.  This table has to be
  ** put in an SrcList structure because some of the subroutines we
  ** will be calling are designed to work with multiple tables and expect
  ** an SrcList* parameter instead of just a Table* parameter.
  */
  pTabList = sqliteTableTokenToSrcList(pParse, pTableName);
  if( pTabList==0 ) goto delete_from_cleanup;
  assert( pTabList->nSrc==1 );
  pTab = pTabList->a[0].pTab;
  assert( pTab->pSelect==0 );  /* This table is not a view */
  if( sqliteAuthCheck(pParse, SQLITE_DELETE, pTab->zName, 0) ){
    goto delete_from_cleanup;
  }

  /* Allocate a cursor used to store the old.* data for a trigger.
  */
  if( row_triggers_exist ){ 
    oldIdx = pParse->nTab++;
  }

  /* Resolve the column names in all the expressions.
  */
  base = pParse->nTab++;
  if( pWhere ){
    if( sqliteExprResolveIds(pParse, base, pTabList, 0, pWhere) ){
      goto delete_from_cleanup;
    }
    if( sqliteExprCheck(pParse, pWhere, 0, 0) ){
      goto delete_from_cleanup;
    }
  }

  /* Begin generating code.
  */
  v = sqliteGetVdbe(pParse);
  if( v==0 ){
    goto delete_from_cleanup;
  }
  sqliteBeginWriteOperation(pParse, row_triggers_exist,
       !row_triggers_exist && pTab->isTemp);

  /* Initialize the counter of the number of rows deleted, if
  ** we are counting rows.
  */
  if( db->flags & SQLITE_CountRows ){
    sqliteVdbeAddOp(v, OP_Integer, 0, 0);
  }

  /* Special case: A DELETE without a WHERE clause deletes everything.
  ** It is easier just to erase the whole table.  Note, however, that
  ** this means that the row change count will be incorrect.
  */
  if( pWhere==0 && !row_triggers_exist ){
    if( db->flags & SQLITE_CountRows ){
      /* If counting rows deleted, just count the total number of
      ** entries in the table. */
      int endOfLoop = sqliteVdbeMakeLabel(v);
      int addr;
      openOp = pTab->isTemp ? OP_OpenAux : OP_Open;
      sqliteVdbeAddOp(v, openOp, base, pTab->tnum);
      sqliteVdbeAddOp(v, OP_Rewind, base, sqliteVdbeCurrentAddr(v)+2);
      addr = sqliteVdbeAddOp(v, OP_AddImm, 1, 0);
      sqliteVdbeAddOp(v, OP_Next, base, addr);
      sqliteVdbeResolveLabel(v, endOfLoop);
      sqliteVdbeAddOp(v, OP_Close, base, 0);
    }
    sqliteVdbeAddOp(v, OP_Clear, pTab->tnum, pTab->isTemp);
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      sqliteVdbeAddOp(v, OP_Clear, pIdx->tnum, pTab->isTemp);
    }
  }

  /* The usual case: There is a WHERE clause so we have to scan through
  ** the table an pick which records to delete.
  */
  else{
    /* Begin the database scan
    */
    pWInfo = sqliteWhereBegin(pParse, base, pTabList, pWhere, 1, 0);
    if( pWInfo==0 ) goto delete_from_cleanup;

    /* Remember the key of every item to be deleted.
    */
    sqliteVdbeAddOp(v, OP_ListWrite, 0, 0);
    if( db->flags & SQLITE_CountRows ){
      sqliteVdbeAddOp(v, OP_AddImm, 1, 0);
    }

    /* End the database scan loop.
    */
    sqliteWhereEnd(pWInfo);

    /* Delete every item whose key was written to the list during the
    ** database scan.  We have to delete items after the scan is complete
    ** because deleting an item can change the scan order.
    */
    sqliteVdbeAddOp(v, OP_ListRewind, 0, 0);
    end = sqliteVdbeMakeLabel(v);

    /* This is the beginning of the delete loop when there are
    ** row triggers.
    */
    if( row_triggers_exist ){
      addr = sqliteVdbeAddOp(v, OP_ListRead, 0, end);
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);

      openOp = pTab->isTemp ? OP_OpenAux : OP_Open;
      sqliteVdbeAddOp(v, openOp, base, pTab->tnum);
      sqliteVdbeAddOp(v, OP_MoveTo, base, 0);
      sqliteVdbeAddOp(v, OP_OpenTemp, oldIdx, 0);

      sqliteVdbeAddOp(v, OP_Integer, 13, 0);
      for(i = 0; i<pTab->nCol; i++){
        if( i==pTab->iPKey ){ 
          sqliteVdbeAddOp(v, OP_Recno, base, 0);
	} else {
          sqliteVdbeAddOp(v, OP_Column, base, i);
	}
      }
      sqliteVdbeAddOp(v, OP_MakeRecord, pTab->nCol, 0);
      sqliteVdbeAddOp(v, OP_PutIntKey, oldIdx, 0);
      sqliteVdbeAddOp(v, OP_Close, base, 0);
      sqliteVdbeAddOp(v, OP_Rewind, oldIdx, 0);

      sqliteCodeRowTrigger(pParse, TK_DELETE, 0, TK_BEFORE, pTab, -1, 
          oldIdx, (pParse->trigStack)?pParse->trigStack->orconf:OE_Default,
	  addr);
    }

    /* Open cursors for the table we are deleting from and all its
    ** indices.  If there are row triggers, this happens inside the
    ** OP_ListRead loop because the cursor have to all be closed
    ** before the trigger fires.  If there are no row triggers, the
    ** cursors are opened only once on the outside the loop.
    */
    pParse->nTab = base + 1;
    openOp = pTab->isTemp ? OP_OpenWrAux : OP_OpenWrite;
    sqliteVdbeAddOp(v, openOp, base, pTab->tnum);
    for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
      sqliteVdbeAddOp(v, openOp, pParse->nTab++, pIdx->tnum);
    }

    /* This is the beginning of the delete loop when there are no
    ** row triggers */
    if( !row_triggers_exist ){ 
      addr = sqliteVdbeAddOp(v, OP_ListRead, 0, end);
    }

    /* Delete the row */
    sqliteGenerateRowDelete(db, v, pTab, base, pParse->trigStack==0);

    /* If there are row triggers, close all cursors then invoke
    ** the AFTER triggers
    */
    if( row_triggers_exist ){
      for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
        sqliteVdbeAddOp(v, OP_Close, base + i, pIdx->tnum);
      }
      sqliteVdbeAddOp(v, OP_Close, base, 0);
      sqliteCodeRowTrigger(pParse, TK_DELETE, 0, TK_AFTER, pTab, -1, 
          oldIdx, (pParse->trigStack)?pParse->trigStack->orconf:OE_Default,
	  addr);
    }

    /* End of the delete loop */
    sqliteVdbeAddOp(v, OP_Goto, 0, addr);
    sqliteVdbeResolveLabel(v, end);
    sqliteVdbeAddOp(v, OP_ListReset, 0, 0);

    /* Close the cursors after the loop if there are no row triggers */
    if( !row_triggers_exist ){
      for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
        sqliteVdbeAddOp(v, OP_Close, base + i, pIdx->tnum);
      }
      sqliteVdbeAddOp(v, OP_Close, base, 0);
      pParse->nTab = base;
    }
  }
  sqliteEndWriteOperation(pParse);

  /*
  ** Return the number of rows that were deleted.
  */
  if( db->flags & SQLITE_CountRows ){
    sqliteVdbeAddOp(v, OP_ColumnName, 0, 0);
    sqliteVdbeChangeP3(v, -1, "rows deleted", P3_STATIC);
    sqliteVdbeAddOp(v, OP_Callback, 1, 0);
  }

delete_from_cleanup:
  sqliteSrcListDelete(pTabList);
  sqliteExprDelete(pWhere);
  return;
}

/*
** This routine generates VDBE code that causes a single row of a
** single table to be deleted.
**
** The VDBE must be in a particular state when this routine is called.
** These are the requirements:
**
**   1.  A read/write cursor pointing to pTab, the table containing the row
**       to be deleted, must be opened as cursor number "base".
**
**   2.  Read/write cursors for all indices of pTab must be open as
**       cursor number base+i for the i-th index.
**
**   3.  The record number of the row to be deleted must be on the top
**       of the stack.
**
** This routine pops the top of the stack to remove the record number
** and then generates code to remove both the table record and all index
** entries that point to that record.
*/
void sqliteGenerateRowDelete(
  sqlite *db,        /* The database containing the index */
  Vdbe *v,           /* Generate code into this VDBE */
  Table *pTab,       /* Table containing the row to be deleted */
  int base,          /* Cursor number for the table */
  int count          /* Increment the row change counter */
){
  int addr;
  addr = sqliteVdbeAddOp(v, OP_NotExists, base, 0);
  sqliteGenerateRowIndexDelete(db, v, pTab, base, 0);
  sqliteVdbeAddOp(v, OP_Delete, base, count);
  sqliteVdbeChangeP2(v, addr, sqliteVdbeCurrentAddr(v));
}

/*
** This routine generates VDBE code that causes the deletion of all
** index entries associated with a single row of a single table.
**
** The VDBE must be in a particular state when this routine is called.
** These are the requirements:
**
**   1.  A read/write cursor pointing to pTab, the table containing the row
**       to be deleted, must be opened as cursor number "base".
**
**   2.  Read/write cursors for all indices of pTab must be open as
**       cursor number base+i for the i-th index.
**
**   3.  The "base" cursor must be pointing to the row that is to be
**       deleted.
*/
void sqliteGenerateRowIndexDelete(
  sqlite *db,        /* The database containing the index */
  Vdbe *v,           /* Generate code into this VDBE */
  Table *pTab,       /* Table containing the row to be deleted */
  int base,          /* Cursor number for the table */
  char *aIdxUsed     /* Only delete if aIdxUsed!=0 && aIdxUsed[i]!=0 */
){
  int i;
  Index *pIdx;

  for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
    int j;
    if( aIdxUsed!=0 && aIdxUsed[i-1]==0 ) continue;
    sqliteVdbeAddOp(v, OP_Recno, base, 0);
    for(j=0; j<pIdx->nColumn; j++){
      int idx = pIdx->aiColumn[j];
      if( idx==pTab->iPKey ){
        sqliteVdbeAddOp(v, OP_Dup, j, 0);
      }else{
        sqliteVdbeAddOp(v, OP_Column, base, idx);
      }
    }
    sqliteVdbeAddOp(v, OP_MakeIdxKey, pIdx->nColumn, 0);
    if( db->file_format>=4 ) sqliteAddIdxKeyType(v, pIdx);
    sqliteVdbeAddOp(v, OP_IdxDelete, base+i, 0);
  }
}
