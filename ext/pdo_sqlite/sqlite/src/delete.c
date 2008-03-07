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
    pTab = sqlite3LocateTable(pParse, 0, pItem->zName, pItem->zDatabase);
    sqlite3DeleteTable(pItem->pTab);
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
  if( (pTab->readOnly && (pParse->db->flags & SQLITE_WriteSchema)==0
        && pParse->nested==0) 
#ifndef SQLITE_OMIT_VIRTUALTABLE
      || (pTab->pMod && pTab->pMod->pModule->xUpdate==0)
#endif
  ){
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
void sqlite3OpenTable(
  Parse *p,       /* Generate code into this VDBE */
  int iCur,       /* The cursor number of the table */
  int iDb,        /* The database index in sqlite3.aDb[] */
  Table *pTab,    /* The table to be opened */
  int opcode      /* OP_OpenRead or OP_OpenWrite */
){
  Vdbe *v;
  if( IsVirtual(pTab) ) return;
  v = sqlite3GetVdbe(p);
  assert( opcode==OP_OpenWrite || opcode==OP_OpenRead );
  sqlite3TableLock(p, iDb, pTab->tnum, (opcode==OP_OpenWrite), pTab->zName);
  sqlite3VdbeAddOp3(v, opcode, iCur, pTab->tnum, iDb);
  VdbeComment((v, "%s", pTab->zName));
  sqlite3VdbeAddOp2(v, OP_SetNumColumns, iCur, pTab->nCol);
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
  int iDb;               /* Database number */
  int memCnt = 0;        /* Memory cell used for change counting */

#ifndef SQLITE_OMIT_TRIGGER
  int isView;                  /* True if attempting to delete from a view */
  int triggers_exist = 0;      /* True if any triggers exist */
#endif
  int iBeginAfterTrigger;      /* Address of after trigger program */
  int iEndAfterTrigger;        /* Exit of after trigger program */
  int iBeginBeforeTrigger;     /* Address of before trigger program */
  int iEndBeforeTrigger;       /* Exit of before trigger program */
  u32 old_col_mask = 0;        /* Mask of OLD.* columns in use */

  sContext.pParse = 0;
  db = pParse->db;
  if( pParse->nErr || db->mallocFailed ){
    goto delete_from_cleanup;
  }
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
  iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
  assert( iDb<db->nDb );
  zDb = db->aDb[iDb].zName;
  if( sqlite3AuthCheck(pParse, SQLITE_DELETE, pTab->zName, 0, zDb) ){
    goto delete_from_cleanup;
  }

  /* If pTab is really a view, make sure it has been initialized.
  */
  if( sqlite3ViewGetColumnNames(pParse, pTab) ){
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
  for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
    pParse->nTab++;
  }
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
  sqlite3BeginWriteOperation(pParse, triggers_exist, iDb);

  if( triggers_exist ){
    int orconf = ((pParse->trigStack)?pParse->trigStack->orconf:OE_Default);
    int iGoto = sqlite3VdbeAddOp0(v, OP_Goto);
    addr = sqlite3VdbeMakeLabel(v);

    iBeginBeforeTrigger = sqlite3VdbeCurrentAddr(v);
    (void)sqlite3CodeRowTrigger(pParse, TK_DELETE, 0, TRIGGER_BEFORE, pTab,
        -1, oldIdx, orconf, addr, &old_col_mask, 0);
    iEndBeforeTrigger = sqlite3VdbeAddOp0(v, OP_Goto);

    iBeginAfterTrigger = sqlite3VdbeCurrentAddr(v);
    (void)sqlite3CodeRowTrigger(pParse, TK_DELETE, 0, TRIGGER_AFTER, pTab, -1,
        oldIdx, orconf, addr, &old_col_mask, 0);
    iEndAfterTrigger = sqlite3VdbeAddOp0(v, OP_Goto);

    sqlite3VdbeJumpHere(v, iGoto);
  }

  /* If we are trying to delete from a view, realize that view into
  ** a ephemeral table.
  */
  if( isView ){
    SelectDest dest;
    Select *pView;

    pView = sqlite3SelectDup(db, pTab->pSelect);
    sqlite3SelectMask(pParse, pView, old_col_mask);
    sqlite3SelectDestInit(&dest, SRT_EphemTab, iCur);
    sqlite3Select(pParse, pView, &dest, 0, 0, 0, 0);
    sqlite3SelectDelete(pView);
  }

  /* Initialize the counter of the number of rows deleted, if
  ** we are counting rows.
  */
  if( db->flags & SQLITE_CountRows ){
    memCnt = ++pParse->nMem;
    sqlite3VdbeAddOp2(v, OP_Integer, 0, memCnt);
  }

  /* Special case: A DELETE without a WHERE clause deletes everything.
  ** It is easier just to erase the whole table.  Note, however, that
  ** this means that the row change count will be incorrect.
  */
  if( pWhere==0 && !triggers_exist && !IsVirtual(pTab) ){
    if( db->flags & SQLITE_CountRows ){
      /* If counting rows deleted, just count the total number of
      ** entries in the table. */
      int addr2;
      if( !isView ){
        sqlite3OpenTable(pParse, iCur, iDb, pTab, OP_OpenRead);
      }
      sqlite3VdbeAddOp2(v, OP_Rewind, iCur, sqlite3VdbeCurrentAddr(v)+2);
      addr2 = sqlite3VdbeAddOp2(v, OP_AddImm, memCnt, 1);
      sqlite3VdbeAddOp2(v, OP_Next, iCur, addr2);
      sqlite3VdbeAddOp1(v, OP_Close, iCur);
    }
    if( !isView ){
      sqlite3VdbeAddOp2(v, OP_Clear, pTab->tnum, iDb);
      if( !pParse->nested ){
        sqlite3VdbeChangeP4(v, -1, pTab->zName, P4_STATIC);
      }
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        assert( pIdx->pSchema==pTab->pSchema );
        sqlite3VdbeAddOp2(v, OP_Clear, pIdx->tnum, iDb);
      }
    }
  } 
  /* The usual case: There is a WHERE clause so we have to scan through
  ** the table and pick which records to delete.
  */
  else{
    int iRowid = ++pParse->nMem;    /* Used for storing rowid values. */

    /* Begin the database scan
    */
    pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, 0, 0);
    if( pWInfo==0 ) goto delete_from_cleanup;

    /* Remember the rowid of every item to be deleted.
    */
    sqlite3VdbeAddOp2(v, IsVirtual(pTab) ? OP_VRowid : OP_Rowid, iCur, iRowid);
    sqlite3VdbeAddOp1(v, OP_FifoWrite, iRowid);
    if( db->flags & SQLITE_CountRows ){
      sqlite3VdbeAddOp2(v, OP_AddImm, memCnt, 1);
    }

    /* End the database scan loop.
    */
    sqlite3WhereEnd(pWInfo);

    /* Open the pseudo-table used to store OLD if there are triggers.
    */
    if( triggers_exist ){
      sqlite3VdbeAddOp1(v, OP_OpenPseudo, oldIdx);
      sqlite3VdbeAddOp2(v, OP_SetNumColumns, oldIdx, pTab->nCol);
    }

    /* Delete every item whose key was written to the list during the
    ** database scan.  We have to delete items after the scan is complete
    ** because deleting an item can change the scan order.
    */
    end = sqlite3VdbeMakeLabel(v);

    if( !isView ){
      /* Open cursors for the table we are deleting from and 
      ** all its indices.
      */
      sqlite3OpenTableAndIndices(pParse, pTab, iCur, OP_OpenWrite);
    }

    /* This is the beginning of the delete loop. If a trigger encounters
    ** an IGNORE constraint, it jumps back to here.
    */
    if( triggers_exist ){
      sqlite3VdbeResolveLabel(v, addr);
    }
    addr = sqlite3VdbeAddOp2(v, OP_FifoRead, iRowid, end);

    if( triggers_exist ){
      int iData = ++pParse->nMem;   /* For storing row data of OLD table */

      /* If the record is no longer present in the table, jump to the
      ** next iteration of the loop through the contents of the fifo.
      */
      sqlite3VdbeAddOp3(v, OP_NotExists, iCur, addr, iRowid);

      /* Populate the OLD.* pseudo-table */
      if( old_col_mask ){
        sqlite3VdbeAddOp2(v, OP_RowData, iCur, iData);
      }else{
        sqlite3VdbeAddOp2(v, OP_Null, 0, iData);
      }
      sqlite3VdbeAddOp3(v, OP_Insert, oldIdx, iData, iRowid);

      /* Jump back and run the BEFORE triggers */
      sqlite3VdbeAddOp2(v, OP_Goto, 0, iBeginBeforeTrigger);
      sqlite3VdbeJumpHere(v, iEndBeforeTrigger);
    }

    if( !isView ){
      /* Delete the row */
#ifndef SQLITE_OMIT_VIRTUALTABLE
      if( IsVirtual(pTab) ){
        const char *pVtab = (const char *)pTab->pVtab;
        pParse->pVirtualLock = pTab;
        sqlite3VdbeAddOp4(v, OP_VUpdate, 0, 1, iRowid, pVtab, P4_VTAB);
      }else
#endif
      {
        sqlite3GenerateRowDelete(pParse, pTab, iCur, iRowid, pParse->nested==0);
      }
    }

    /* If there are row triggers, close all cursors then invoke
    ** the AFTER triggers
    */
    if( triggers_exist ){
      /* Jump back and run the AFTER triggers */
      sqlite3VdbeAddOp2(v, OP_Goto, 0, iBeginAfterTrigger);
      sqlite3VdbeJumpHere(v, iEndAfterTrigger);
    }

    /* End of the delete loop */
    sqlite3VdbeAddOp2(v, OP_Goto, 0, addr);
    sqlite3VdbeResolveLabel(v, end);

    /* Close the cursors after the loop if there are no row triggers */
    if( !isView  && !IsVirtual(pTab) ){
      for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
        sqlite3VdbeAddOp2(v, OP_Close, iCur + i, pIdx->tnum);
      }
      sqlite3VdbeAddOp1(v, OP_Close, iCur);
    }
  }

  /*
  ** Return the number of rows that were deleted. If this routine is 
  ** generating code because of a call to sqlite3NestedParse(), do not
  ** invoke the callback function.
  */
  if( db->flags & SQLITE_CountRows && pParse->nested==0 && !pParse->trigStack ){
    sqlite3VdbeAddOp2(v, OP_ResultRow, memCnt, 1);
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "rows deleted", P4_STATIC);
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
**   3.  The record number of the row to be deleted must be stored in
**       memory cell iRowid.
**
** This routine pops the top of the stack to remove the record number
** and then generates code to remove both the table record and all index
** entries that point to that record.
*/
void sqlite3GenerateRowDelete(
  Parse *pParse,     /* Parsing context */
  Table *pTab,       /* Table containing the row to be deleted */
  int iCur,          /* Cursor number for the table */
  int iRowid,        /* Memory cell that contains the rowid to delete */
  int count          /* Increment the row change counter */
){
  int addr;
  Vdbe *v;

  v = pParse->pVdbe;
  addr = sqlite3VdbeAddOp3(v, OP_NotExists, iCur, 0, iRowid);
  sqlite3GenerateRowIndexDelete(pParse, pTab, iCur, 0);
  sqlite3VdbeAddOp2(v, OP_Delete, iCur, (count?OPFLAG_NCHANGE:0));
  if( count ){
    sqlite3VdbeChangeP4(v, -1, pTab->zName, P4_STATIC);
  }
  sqlite3VdbeJumpHere(v, addr);
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
  Parse *pParse,     /* Parsing and code generating context */
  Table *pTab,       /* Table containing the row to be deleted */
  int iCur,          /* Cursor number for the table */
  int *aRegIdx       /* Only delete if aRegIdx!=0 && aRegIdx[i]>0 */
){
  int i;
  Index *pIdx;
  int r1;

  r1 = sqlite3GetTempReg(pParse);
  for(i=1, pIdx=pTab->pIndex; pIdx; i++, pIdx=pIdx->pNext){
    if( aRegIdx!=0 && aRegIdx[i-1]==0 ) continue;
    sqlite3GenerateIndexKey(pParse, pIdx, iCur, r1);
    sqlite3VdbeAddOp2(pParse->pVdbe, OP_IdxDelete, iCur+i, r1);
  }
  sqlite3ReleaseTempReg(pParse, r1);
}

/*
** Generate code that will assemble an index key and put it on the top
** of the tack.  The key with be for index pIdx which is an index on pTab.
** iCur is the index of a cursor open on the pTab table and pointing to
** the entry that needs indexing.
**
** Return a register number which is the first in a block of
** registers that holds the elements of the index key.  The
** block of registers has already been deallocated by the time
** this routine returns.
*/
int sqlite3GenerateIndexKey(
  Parse *pParse,     /* Parsing context */
  Index *pIdx,       /* The index for which to generate a key */
  int iCur,          /* Cursor number for the pIdx->pTable table */
  int regOut         /* Write the new index key to this register */
){
  Vdbe *v = pParse->pVdbe;
  int j;
  Table *pTab = pIdx->pTable;
  int regBase;
  int nCol;

  nCol = pIdx->nColumn;
  regBase = sqlite3GetTempRange(pParse, nCol+1);
  sqlite3VdbeAddOp2(v, OP_Rowid, iCur, regBase+nCol);
  for(j=0; j<nCol; j++){
    int idx = pIdx->aiColumn[j];
    if( idx==pTab->iPKey ){
      sqlite3VdbeAddOp2(v, OP_SCopy, regBase+nCol, regBase+j);
    }else{
      sqlite3VdbeAddOp3(v, OP_Column, iCur, idx, regBase+j);
      sqlite3ColumnDefault(v, pTab, idx);
    }
  }
  sqlite3VdbeAddOp3(v, OP_MakeRecord, regBase, nCol+1, regOut);
  sqlite3IndexAffinityStr(v, pIdx);
  sqlite3ReleaseTempRange(pParse, regBase, nCol+1);
  return regBase;
}
