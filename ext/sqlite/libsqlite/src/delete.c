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
** Look up every table that is named in pSrc.  If any table is not found,
** add an error message to pParse->zErrMsg and return NULL.  If all tables
** are found, return a pointer to the last table.
*/
Table *sqliteSrcListLookup(Parse *pParse, SrcList *pSrc){
  Table *pTab = 0;
  int i;
  for(i=0; i<pSrc->nSrc; i++){
    const char *zTab = pSrc->a[i].zName;
    const char *zDb = pSrc->a[i].zDatabase;
    pTab = sqliteLocateTable(pParse, zTab, zDb);
    pSrc->a[i].pTab = pTab;
  }
  return pTab;
}

/*
** Check to make sure the given table is writable.  If it is not
** writable, generate an error message and return 1.  If it is
** writable return 0;
*/
int sqliteIsReadOnly(Parse *pParse, Table *pTab, int viewOk){
  if( pTab->readOnly ){
    sqliteErrorMsg(pParse, "table %s may not be modified", pTab->zName);
    return 1;
  }
  if( !viewOk && pTab->pSelect ){
    sqliteErrorMsg(pParse, "cannot modify %s because it is a view",pTab->zName);
    return 1;
  }
  return 0;
}

/*
** Process a DELETE FROM statement.
*/
void sqliteDeleteFrom(
  Parse *pParse,         /* The parser context */
  SrcList *pTabList,     /* The table from which we should delete things */
  Expr *pWhere           /* The WHERE clause.  May be null */
){
  Vdbe *v;               /* The virtual database engine */
  Table *pTab;           /* The table from which records will be deleted */
  const char *zDb;       /* Name of database holding pTab */
  int end, addr;         /* A couple addresses of generated code */
  int i;                 /* Loop counter */
  WhereInfo *pWInfo;     /* Information about the WHERE clause */
  Index *pIdx;           /* For looping over indices of the table */
  int iCur;              /* VDBE Cursor number for pTab */
  sqlite *db;            /* Main database structure */
  int isView;            /* True if attempting to delete from a view */
  AuthContext sContext;  /* Authorization context */

  int row_triggers_exist = 0;  /* True if any triggers exist */
  int before_triggers;         /* True if there are BEFORE triggers */
  int after_triggers;          /* True if there are AFTER triggers */
  int oldIdx = -1;             /* Cursor for the OLD table of AFTER triggers */

  sContext.pParse = 0;
  if( pParse->nErr || sqlite_malloc_failed ){
    pTabList = 0;
    goto delete_from_cleanup;
  }
  db = pParse->db;
  assert( pTabList->nSrc==1 );

  /* Locate the table which we want to delete.  This table has to be
  ** put in an SrcList structure because some of the subroutines we
  ** will be calling are designed to work with multiple tables and expect
  ** an SrcList* parameter instead of just a Table* parameter.
  */
  pTab = sqliteSrcListLookup(pParse, pTabList);
  if( pTab==0 )  goto delete_from_cleanup;
  before_triggers = sqliteTriggersExist(pParse, pTab->pTrigger, 
                         TK_DELETE, TK_BEFORE, TK_ROW, 0);
  after_triggers = sqliteTriggersExist(pParse, pTab->pTrigger, 
                         TK_DELETE, TK_AFTER, TK_ROW, 0);
  row_triggers_exist = before_triggers || after_triggers;
  isView = pTab->pSelect!=0;
  if( sqliteIsReadOnly(pParse, pTab, before_triggers) ){
    goto delete_from_cleanup;
  }
  assert( pTab->iDb<db->nDb );
  zDb = db->aDb[pTab->iDb].zName;
  if( sqliteAuthCheck(pParse, SQLITE_DELETE, pTab->zName, 0, zDb) ){
    goto delete_from_cleanup;
  }

  /* If pTab is really a view, make sure it has been initialized.
  */
  if( isView && sqliteViewGetColumnNames(pParse, pTab) ){
    goto delete_from_cleanup;
  }

  /* Allocate a cursor used to store the old.* data for a trigger.
  */
  if( row_triggers_exist ){ 
    oldIdx = pParse->nTab++;
  }

  /* Resolve the column names in all the expressions.
  */
  assert( pTabList->nSrc==1 );
  iCur = pTabList->a[0].iCursor = pParse->nTab++;
  if( pWhere ){
    if( sqliteExprResolveIds(pParse, pTabList, 0, pWhere) ){
      goto delete_from_cleanup;
    }
    if( sqliteExprCheck(pParse, pWhere, 0, 0) ){
      goto delete_from_cleanup;
    }
  }

  /* Start the view context
  */
  if( isView ){
    sqliteAuthContextPush(pParse, &sContext, pTab->zName);
  }

  /* Begin generating code.
  */
  v = sqliteGetVdbe(pParse);
  if( v==0 ){
    goto delete_from_cleanup;
  }
  sqliteBeginWriteOperation(pParse, row_triggers_exist, pTab->iDb);

  /* If we are trying to delete from a view, construct that view into
  ** a temporary table.
  */
  if( isView ){
    Select *pView = sqliteSelectDup(pTab->pSelect);
    sqliteSelect(pParse, pView, SRT_TempTable, iCur, 0, 0, 0);
    sqliteSelectDelete(pView);
  }

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
      if( !isView ){
        sqliteVdbeAddOp(v, OP_Integer, pTab->iDb, 0);
        sqliteVdbeAddOp(v, OP_OpenRead, iCur, pTab->tnum);
      }
      sqliteVdbeAddOp(v, OP_Rewind, iCur, sqliteVdbeCurrentAddr(v)+2);
      addr = sqliteVdbeAddOp(v, OP_AddImm, 1, 0);
      sqliteVdbeAddOp(v, OP_Next, iCur, addr);
      sqliteVdbeResolveLabel(v, endOfLoop);
      sqliteVdbeAddOp(v, OP_Close, iCur, 0);
    }
    if( !isView ){
      sqliteVdbeAddOp(v, OP_Clear, pTab->tnum, pTab->iDb);
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        sqliteVdbeAddOp(v, OP_Clear, pIdx->tnum, pIdx->iDb);
      }
    }
  }

  /* The usual case: There is a WHERE clause so we have to scan through
  ** the table and pick which records to delete.
  */
  else{
    /* Begin the database scan
    */
    pWInfo = sqliteWhereBegin(pParse, pTabList, pWhere, 1, 0);
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

    /* Open the pseudo-table used to store OLD if there are triggers.
    */
    if( row_triggers_exist ){
      sqliteVdbeAddOp(v, OP_OpenPseudo, oldIdx, 0);
    }

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
      if( !isView ){
        sqliteVdbeAddOp(v, OP_Integer, pTab->iDb, 0);
        sqliteVdbeAddOp(v, OP_OpenRead, iCur, pTab->tnum);
      }
      sqliteVdbeAddOp(v, OP_MoveTo, iCur, 0);

      sqliteVdbeAddOp(v, OP_Recno, iCur, 0);
      sqliteVdbeAddOp(v, OP_RowData, iCur, 0);
      sqliteVdbeAddOp(v, OP_PutIntKey, oldIdx, 0);
      if( !isView ){
        sqliteVdbeAddOp(v, OP_Close, iCur, 0);
      }

      sqliteCodeRowTrigger(pParse, TK_DELETE, 0, TK_BEFORE, pTab, -1, 
          oldIdx, (pParse->trigStack)?pParse->trigStack->orconf:OE_Default,
	  addr);
    }

    if( !isView ){
      /* Open cursors for the table we are deleting from and all its
      ** indices.  If there are row triggers, this happens inside the
      ** OP_ListRead loop because the cursor have to all be closed
      ** before the trigger fires.  If there are no row triggers, the
      ** cursors are opened only once on the outside the loop.
      */
      pParse->nTab = iCur + 1;
      sqliteOpenTableAndIndices(pParse, pTab, iCur);

      /* This is the beginning of the delete loop when there are no
      ** row triggers */
      if( !row_triggers_exist ){ 
        addr = sqliteVdbeAddOp(v, OP_ListRead, 0, end);
      }

      /* Delete the row */
      sqliteGenerateRowDelete(db, v, pTab, iCur, pParse->trigStack==0);
    }

    /* If there are row triggers, close all cursors then invoke
    ** the AFTER triggers
    */
    if( row_triggers_exist ){
      if( !isView ){
        for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
          sqliteVdbeAddOp(v, OP_Close, iCur + i, pIdx->tnum);
        }
        sqliteVdbeAddOp(v, OP_Close, iCur, 0);
      }
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
        sqliteVdbeAddOp(v, OP_Close, iCur + i, pIdx->tnum);
      }
      sqliteVdbeAddOp(v, OP_Close, iCur, 0);
      pParse->nTab = iCur;
    }
  }
  sqliteVdbeAddOp(v, OP_SetCounts, 0, 0);
  sqliteEndWriteOperation(pParse);

  /*
  ** Return the number of rows that were deleted.
  */
  if( db->flags & SQLITE_CountRows ){
    sqliteVdbeAddOp(v, OP_ColumnName, 0, 1);
    sqliteVdbeChangeP3(v, -1, "rows deleted", P3_STATIC);
    sqliteVdbeAddOp(v, OP_Callback, 1, 0);
  }

delete_from_cleanup:
  sqliteAuthContextPop(&sContext);
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
  int iCur,          /* Cursor number for the table */
  int count          /* Increment the row change counter */
){
  int addr;
  addr = sqliteVdbeAddOp(v, OP_NotExists, iCur, 0);
  sqliteGenerateRowIndexDelete(db, v, pTab, iCur, 0);
  sqliteVdbeAddOp(v, OP_Delete, iCur,
    (count?OPFLAG_NCHANGE:0) | OPFLAG_CSCHANGE);
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
**       to be deleted, must be opened as cursor number "iCur".
**
**   2.  Read/write cursors for all indices of pTab must be open as
**       cursor number iCur+i for the i-th index.
**
**   3.  The "iCur" cursor must be pointing to the row that is to be
**       deleted.
*/
void sqliteGenerateRowIndexDelete(
  sqlite *db,        /* The database containing the index */
  Vdbe *v,           /* Generate code into this VDBE */
  Table *pTab,       /* Table containing the row to be deleted */
  int iCur,          /* Cursor number for the table */
  char *aIdxUsed     /* Only delete if aIdxUsed!=0 && aIdxUsed[i]!=0 */
){
  int i;
  Index *pIdx;

  for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
    int j;
    if( aIdxUsed!=0 && aIdxUsed[i-1]==0 ) continue;
    sqliteVdbeAddOp(v, OP_Recno, iCur, 0);
    for(j=0; j<pIdx->nColumn; j++){
      int idx = pIdx->aiColumn[j];
      if( idx==pTab->iPKey ){
        sqliteVdbeAddOp(v, OP_Dup, j, 0);
      }else{
        sqliteVdbeAddOp(v, OP_Column, iCur, idx);
      }
    }
    sqliteVdbeAddOp(v, OP_MakeIdxKey, pIdx->nColumn, 0);
    if( db->file_format>=4 ) sqliteAddIdxKeyType(v, pIdx);
    sqliteVdbeAddOp(v, OP_IdxDelete, iCur+i, 0);
  }
}
