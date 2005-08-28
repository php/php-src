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
** in order to generate code for DELETE FROM statements.
**
** $Id$
*/
#include "sqliteInt.h"

/*
** Look up every table that is named in pSrc.  If any table is not found,
** add an error message to pParse->zErrMsg and return NULL.  If all tables
** are found, return a pointer to the last table.
*/
Table *sqlite3SrcListLookup(Parse *pParse, SrcList *pSrc){
  Table *pTab = 0;
  int i;
  struct SrcList_item *pItem;
  for(i=0, pItem=pSrc->a; i<pSrc->nSrc; i++, pItem++){
    pTab = sqlite3LocateTable(pParse, pItem->zName, pItem->zDatabase);
    sqlite3DeleteTable(pParse->db, pItem->pTab);
    pItem->pTab = pTab;
    if( pTab ){
      pTab->nRef++;
    }
  }
  return pTab;
}

/*
** Check to make sure the given table is writable.  If it is not
** writable, generate an error message and return 1.  If it is
** writable return 0;
*/
int sqlite3IsReadOnly(Parse *pParse, Table *pTab, int viewOk){
  if( pTab->readOnly && (pParse->db->flags & SQLITE_WriteSchema)==0
        && pParse->nested==0 ){
    sqlite3ErrorMsg(pParse, "table %s may not be modified", pTab->zName);
    return 1;
  }
#ifndef SQLITE_OMIT_VIEW
  if( !viewOk && pTab->pSelect ){
    sqlite3ErrorMsg(pParse,"cannot modify %s because it is a view",pTab->zName);
    return 1;
  }
#endif
  return 0;
}

/*
** Generate code that will open a table for reading.
*/
void sqlite3OpenTableForReading(
  Vdbe *v,        /* Generate code into this VDBE */
  int iCur,       /* The cursor number of the table */
  Table *pTab     /* The table to be opened */
){
  sqlite3VdbeAddOp(v, OP_Integer, pTab->iDb, 0);
  VdbeComment((v, "# %s", pTab->zName));
  sqlite3VdbeAddOp(v, OP_OpenRead, iCur, pTab->tnum);
  sqlite3VdbeAddOp(v, OP_SetNumColumns, iCur, pTab->nCol);
}


/*
** Generate code for a DELETE FROM statement.
**
**     DELETE FROM table_wxyz WHERE a<5 AND b NOT NULL;
**                 \________/       \________________/
**                  pTabList              pWhere
*/
void sqlite3DeleteFrom(
  Parse *pParse,         /* The parser context */
  SrcList *pTabList,     /* The table from which we should delete things */
  Expr *pWhere           /* The WHERE clause.  May be null */
){
  Vdbe *v;               /* The virtual database engine */
  Table *pTab;           /* The table from which records will be deleted */
  const char *zDb;       /* Name of database holding pTab */
  int end, addr = 0;     /* A couple addresses of generated code */
  int i;                 /* Loop counter */
  WhereInfo *pWInfo;     /* Information about the WHERE clause */
  Index *pIdx;           /* For looping over indices of the table */
  int iCur;              /* VDBE Cursor number for pTab */
  sqlite3 *db;           /* Main database structure */
  AuthContext sContext;  /* Authorization context */
  int oldIdx = -1;       /* Cursor for the OLD table of AFTER triggers */
  NameContext sNC;       /* Name context to resolve expressions in */

#ifndef SQLITE_OMIT_TRIGGER
  int isView;                  /* True if attempting to delete from a view */
  int triggers_exist = 0;      /* True if any triggers exist */
#endif

  sContext.pParse = 0;
  if( pParse->nErr || sqlite3_malloc_failed ){
    goto delete_from_cleanup;
  }
  db = pParse->db;
  assert( pTabList->nSrc==1 );

  /* Locate the table which we want to delete.  This table has to be
  ** put in an SrcList structure because some of the subroutines we
  ** will be calling are designed to work with multiple tables and expect
  ** an SrcList* parameter instead of just a Table* parameter.
  */
  pTab = sqlite3SrcListLookup(pParse, pTabList);
  if( pTab==0 )  goto delete_from_cleanup;

  /* Figure out if we have any triggers and if the table being
  ** deleted from is a view
  */
#ifndef SQLITE_OMIT_TRIGGER
  triggers_exist = sqlite3TriggersExist(pParse, pTab, TK_DELETE, 0);
  isView = pTab->pSelect!=0;
#else
# define triggers_exist 0
# define isView 0
#endif
#ifdef SQLITE_OMIT_VIEW
# undef isView
# define isView 0
#endif

  if( sqlite3IsReadOnly(pParse, pTab, triggers_exist) ){
    goto delete_from_cleanup;
  }
  assert( pTab->iDb<db->nDb );
  zDb = db->aDb[pTab->iDb].zName;
  if( sqlite3AuthCheck(pParse, SQLITE_DELETE, pTab->zName, 0, zDb) ){
    goto delete_from_cleanup;
  }

  /* If pTab is really a view, make sure it has been initialized.
  */
  if( isView && sqlite3ViewGetColumnNames(pParse, pTab) ){
    goto delete_from_cleanup;
  }

  /* Allocate a cursor used to store the old.* data for a trigger.
  */
  if( triggers_exist ){ 
    oldIdx = pParse->nTab++;
  }

  /* Resolve the column names in the WHERE clause.
  */
  assert( pTabList->nSrc==1 );
  iCur = pTabList->a[0].iCursor = pParse->nTab++;
  memset(&sNC, 0, sizeof(sNC));
  sNC.pParse = pParse;
  sNC.pSrcList = pTabList;
  if( sqlite3ExprResolveNames(&sNC, pWhere) ){
    goto delete_from_cleanup;
  }

  /* Start the view context
  */
  if( isView ){
    sqlite3AuthContextPush(pParse, &sContext, pTab->zName);
  }

  /* Begin generating code.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ){
    goto delete_from_cleanup;
  }
  if( pParse->nested==0 ) sqlite3VdbeCountChanges(v);
  sqlite3BeginWriteOperation(pParse, triggers_exist, pTab->iDb);

  /* If we are trying to delete from a view, construct that view into
  ** a temporary table.
  */
  if( isView ){
    Select *pView = sqlite3SelectDup(pTab->pSelect);
    sqlite3Select(pParse, pView, SRT_TempTable, iCur, 0, 0, 0, 0);
    sqlite3SelectDelete(pView);
  }

  /* Initialize the counter of the number of rows deleted, if
  ** we are counting rows.
  */
  if( db->flags & SQLITE_CountRows ){
    sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
  }

  /* Special case: A DELETE without a WHERE clause deletes everything.
  ** It is easier just to erase the whole table.  Note, however, that
  ** this means that the row change count will be incorrect.
  */
  if( pWhere==0 && !triggers_exist ){
    if( db->flags & SQLITE_CountRows ){
      /* If counting rows deleted, just count the total number of
      ** entries in the table. */
      int endOfLoop = sqlite3VdbeMakeLabel(v);
      int addr;
      if( !isView ){
        sqlite3OpenTableForReading(v, iCur, pTab);
      }
      sqlite3VdbeAddOp(v, OP_Rewind, iCur, sqlite3VdbeCurrentAddr(v)+2);
      addr = sqlite3VdbeAddOp(v, OP_AddImm, 1, 0);
      sqlite3VdbeAddOp(v, OP_Next, iCur, addr);
      sqlite3VdbeResolveLabel(v, endOfLoop);
      sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
    }
    if( !isView ){
      sqlite3VdbeAddOp(v, OP_Clear, pTab->tnum, pTab->iDb);
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        sqlite3VdbeAddOp(v, OP_Clear, pIdx->tnum, pIdx->iDb);
      }
    }
  }

  /* The usual case: There is a WHERE clause so we have to scan through
  ** the table and pick which records to delete.
  */
  else{
    /* Ensure all required collation sequences are available. */
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      if( sqlite3CheckIndexCollSeq(pParse, pIdx) ){
        goto delete_from_cleanup;
      }
    }

    /* Begin the database scan
    */
    pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, 0);
    if( pWInfo==0 ) goto delete_from_cleanup;

    /* Remember the rowid of every item to be deleted.
    */
    sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
    sqlite3VdbeAddOp(v, OP_FifoWrite, 0, 0);
    if( db->flags & SQLITE_CountRows ){
      sqlite3VdbeAddOp(v, OP_AddImm, 1, 0);
    }

    /* End the database scan loop.
    */
    sqlite3WhereEnd(pWInfo);

    /* Open the pseudo-table used to store OLD if there are triggers.
    */
    if( triggers_exist ){
      sqlite3VdbeAddOp(v, OP_OpenPseudo, oldIdx, 0);
      sqlite3VdbeAddOp(v, OP_SetNumColumns, oldIdx, pTab->nCol);
    }

    /* Delete every item whose key was written to the list during the
    ** database scan.  We have to delete items after the scan is complete
    ** because deleting an item can change the scan order.
    */
    end = sqlite3VdbeMakeLabel(v);

    /* This is the beginning of the delete loop when there are
    ** row triggers.
    */
    if( triggers_exist ){
      addr = sqlite3VdbeAddOp(v, OP_FifoRead, 0, end);
      if( !isView ){
        sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
        sqlite3OpenTableForReading(v, iCur, pTab);
      }
      sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
      sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
      sqlite3VdbeAddOp(v, OP_RowData, iCur, 0);
      sqlite3VdbeAddOp(v, OP_Insert, oldIdx, 0);
      if( !isView ){
        sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
      }

      (void)sqlite3CodeRowTrigger(pParse, TK_DELETE, 0, TRIGGER_BEFORE, pTab,
          -1, oldIdx, (pParse->trigStack)?pParse->trigStack->orconf:OE_Default,
          addr);
    }

    if( !isView ){
      /* Open cursors for the table we are deleting from and all its
      ** indices.  If there are row triggers, this happens inside the
      ** OP_FifoRead loop because the cursor have to all be closed
      ** before the trigger fires.  If there are no row triggers, the
      ** cursors are opened only once on the outside the loop.
      */
      sqlite3OpenTableAndIndices(pParse, pTab, iCur, OP_OpenWrite);

      /* This is the beginning of the delete loop when there are no
      ** row triggers */
      if( !triggers_exist ){ 
        addr = sqlite3VdbeAddOp(v, OP_FifoRead, 0, end);
      }

      /* Delete the row */
      sqlite3GenerateRowDelete(db, v, pTab, iCur, pParse->nested==0);
    }

    /* If there are row triggers, close all cursors then invoke
    ** the AFTER triggers
    */
    if( triggers_exist ){
      if( !isView ){
        for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
          sqlite3VdbeAddOp(v, OP_Close, iCur + i, pIdx->tnum);
        }
        sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
      }
      (void)sqlite3CodeRowTrigger(pParse, TK_DELETE, 0, TRIGGER_AFTER, pTab, -1,
          oldIdx, (pParse->trigStack)?pParse->trigStack->orconf:OE_Default,
          addr);
    }

    /* End of the delete loop */
    sqlite3VdbeAddOp(v, OP_Goto, 0, addr);
    sqlite3VdbeResolveLabel(v, end);

    /* Close the cursors after the loop if there are no row triggers */
    if( !triggers_exist ){
      for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
        sqlite3VdbeAddOp(v, OP_Close, iCur + i, pIdx->tnum);
      }
      sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
    }
  }

  /*
  ** Return the number of rows that were deleted. If this routine is 
  ** generating code because of a call to sqlite3NestedParse(), do not
  ** invoke the callback function.
  */
  if( db->flags & SQLITE_CountRows && pParse->nested==0 && !pParse->trigStack ){
    sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, "rows deleted", P3_STATIC);
  }

delete_from_cleanup:
  sqlite3AuthContextPop(&sContext);
  sqlite3SrcListDelete(pTabList);
  sqlite3ExprDelete(pWhere);
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
void sqlite3GenerateRowDelete(
  sqlite3 *db,       /* The database containing the index */
  Vdbe *v,           /* Generate code into this VDBE */
  Table *pTab,       /* Table containing the row to be deleted */
  int iCur,          /* Cursor number for the table */
  int count          /* Increment the row change counter */
){
  int addr;
  addr = sqlite3VdbeAddOp(v, OP_NotExists, iCur, 0);
  sqlite3GenerateRowIndexDelete(db, v, pTab, iCur, 0);
  sqlite3VdbeAddOp(v, OP_Delete, iCur, (count?OPFLAG_NCHANGE:0));
  sqlite3VdbeChangeP2(v, addr, sqlite3VdbeCurrentAddr(v));
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
void sqlite3GenerateRowIndexDelete(
  sqlite3 *db,       /* The database containing the index */
  Vdbe *v,           /* Generate code into this VDBE */
  Table *pTab,       /* Table containing the row to be deleted */
  int iCur,          /* Cursor number for the table */
  char *aIdxUsed     /* Only delete if aIdxUsed!=0 && aIdxUsed[i]!=0 */
){
  int i;
  Index *pIdx;

  for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
    if( aIdxUsed!=0 && aIdxUsed[i-1]==0 ) continue;
    sqlite3GenerateIndexKey(v, pIdx, iCur);
    sqlite3VdbeAddOp(v, OP_IdxDelete, iCur+i, 0);
  }
}

/*
** Generate code that will assemble an index key and put it on the top
** of the tack.  The key with be for index pIdx which is an index on pTab.
** iCur is the index of a cursor open on the pTab table and pointing to
** the entry that needs indexing.
*/
void sqlite3GenerateIndexKey(
  Vdbe *v,           /* Generate code into this VDBE */
  Index *pIdx,       /* The index for which to generate a key */
  int iCur           /* Cursor number for the pIdx->pTable table */
){
  int j;
  Table *pTab = pIdx->pTable;

  sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
  for(j=0; j<pIdx->nColumn; j++){
    int idx = pIdx->aiColumn[j];
    if( idx==pTab->iPKey ){
      sqlite3VdbeAddOp(v, OP_Dup, j, 0);
    }else{
      sqlite3VdbeAddOp(v, OP_Column, iCur, idx);
      sqlite3ColumnDefault(v, pTab, idx);
    }
  }
  sqlite3VdbeAddOp(v, OP_MakeIdxRec, pIdx->nColumn, 0);
  sqlite3IndexAffinityStr(v, pIdx);
}
