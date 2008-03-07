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
** to handle UPDATE statements.
**
** $Id$
*/
#include "sqliteInt.h"

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Forward declaration */
static void updateVirtualTable(
  Parse *pParse,       /* The parsing context */
  SrcList *pSrc,       /* The virtual table to be modified */
  Table *pTab,         /* The virtual table */
  ExprList *pChanges,  /* The columns to change in the UPDATE statement */
  Expr *pRowidExpr,    /* Expression used to recompute the rowid */
  int *aXRef,          /* Mapping from columns of pTab to entries in pChanges */
  Expr *pWhere         /* WHERE clause of the UPDATE statement */
);
#endif /* SQLITE_OMIT_VIRTUALTABLE */

/*
** The most recently coded instruction was an OP_Column to retrieve the
** i-th column of table pTab. This routine sets the P4 parameter of the 
** OP_Column to the default value, if any.
**
** The default value of a column is specified by a DEFAULT clause in the 
** column definition. This was either supplied by the user when the table
** was created, or added later to the table definition by an ALTER TABLE
** command. If the latter, then the row-records in the table btree on disk
** may not contain a value for the column and the default value, taken
** from the P4 parameter of the OP_Column instruction, is returned instead.
** If the former, then all row-records are guaranteed to include a value
** for the column and the P4 value is not required.
**
** Column definitions created by an ALTER TABLE command may only have 
** literal default values specified: a number, null or a string. (If a more
** complicated default expression value was provided, it is evaluated 
** when the ALTER TABLE is executed and one of the literal values written
** into the sqlite_master table.)
**
** Therefore, the P4 parameter is only required if the default value for
** the column is a literal number, string or null. The sqlite3ValueFromExpr()
** function is capable of transforming these types of expressions into
** sqlite3_value objects.
*/
void sqlite3ColumnDefault(Vdbe *v, Table *pTab, int i){
  if( pTab && !pTab->pSelect ){
    sqlite3_value *pValue;
    u8 enc = ENC(sqlite3VdbeDb(v));
    Column *pCol = &pTab->aCol[i];
    VdbeComment((v, "%s.%s", pTab->zName, pCol->zName));
    assert( i<pTab->nCol );
    sqlite3ValueFromExpr(sqlite3VdbeDb(v), pCol->pDflt, enc, 
                         pCol->affinity, &pValue);
    if( pValue ){
      sqlite3VdbeChangeP4(v, -1, (const char *)pValue, P4_MEM);
    }
  }
}

/*
** Process an UPDATE statement.
**
**   UPDATE OR IGNORE table_wxyz SET a=b, c=d WHERE e<5 AND f NOT NULL;
**          \_______/ \________/     \______/       \________________/
*            onError   pTabList      pChanges             pWhere
*/
void sqlite3Update(
  Parse *pParse,         /* The parser context */
  SrcList *pTabList,     /* The table in which we should change things */
  ExprList *pChanges,    /* Things to be changed */
  Expr *pWhere,          /* The WHERE clause.  May be null */
  int onError            /* How to handle constraint errors */
){
  int i, j;              /* Loop counters */
  Table *pTab;           /* The table to be updated */
  int addr = 0;          /* VDBE instruction address of the start of the loop */
  WhereInfo *pWInfo;     /* Information about the WHERE clause */
  Vdbe *v;               /* The virtual database engine */
  Index *pIdx;           /* For looping over indices */
  int nIdx;              /* Number of indices that need updating */
  int iCur;              /* VDBE Cursor number of pTab */
  sqlite3 *db;           /* The database structure */
  int *aRegIdx = 0;      /* One register assigned to each index to be updated */
  int *aXRef = 0;        /* aXRef[i] is the index in pChanges->a[] of the
                         ** an expression for the i-th column of the table.
                         ** aXRef[i]==-1 if the i-th column is not changed. */
  int chngRowid;         /* True if the record number is being changed */
  Expr *pRowidExpr = 0;  /* Expression defining the new record number */
  int openAll = 0;       /* True if all indices need to be opened */
  AuthContext sContext;  /* The authorization context */
  NameContext sNC;       /* The name-context to resolve expressions in */
  int iDb;               /* Database containing the table being updated */
  int j1;                /* Addresses of jump instructions */

#ifndef SQLITE_OMIT_TRIGGER
  int isView;                  /* Trying to update a view */
  int triggers_exist = 0;      /* True if any row triggers exist */
#endif
  int iBeginAfterTrigger;      /* Address of after trigger program */
  int iEndAfterTrigger;        /* Exit of after trigger program */
  int iBeginBeforeTrigger;     /* Address of before trigger program */
  int iEndBeforeTrigger;       /* Exit of before trigger program */
  u32 old_col_mask = 0;        /* Mask of OLD.* columns in use */
  u32 new_col_mask = 0;        /* Mask of NEW.* columns in use */

  int newIdx      = -1;  /* index of trigger "new" temp table       */
  int oldIdx      = -1;  /* index of trigger "old" temp table       */

  /* Register Allocations */
  int regRowCount = 0;   /* A count of rows changed */
  int regOldRowid;       /* The old rowid */
  int regNewRowid;       /* The new rowid */
  int regData;           /* New data for the row */

  sContext.pParse = 0;
  db = pParse->db;
  if( pParse->nErr || db->mallocFailed ){
    goto update_cleanup;
  }
  assert( pTabList->nSrc==1 );

  /* Locate the table which we want to update. 
  */
  pTab = sqlite3SrcListLookup(pParse, pTabList);
  if( pTab==0 ) goto update_cleanup;
  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);

  /* Figure out if we have any triggers and if the table being
  ** updated is a view
  */
#ifndef SQLITE_OMIT_TRIGGER
  triggers_exist = sqlite3TriggersExist(pParse, pTab, TK_UPDATE, pChanges);
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
    goto update_cleanup;
  }
  if( sqlite3ViewGetColumnNames(pParse, pTab) ){
    goto update_cleanup;
  }
  aXRef = sqlite3DbMallocRaw(db, sizeof(int) * pTab->nCol );
  if( aXRef==0 ) goto update_cleanup;
  for(i=0; i<pTab->nCol; i++) aXRef[i] = -1;

  /* If there are FOR EACH ROW triggers, allocate cursors for the
  ** special OLD and NEW tables
  */
  if( triggers_exist ){
    newIdx = pParse->nTab++;
    oldIdx = pParse->nTab++;
  }

  /* Allocate a cursors for the main database table and for all indices.
  ** The index cursors might not be used, but if they are used they
  ** need to occur right after the database cursor.  So go ahead and
  ** allocate enough space, just in case.
  */
  pTabList->a[0].iCursor = iCur = pParse->nTab++;
  for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
    pParse->nTab++;
  }

  /* Initialize the name-context */
  memset(&sNC, 0, sizeof(sNC));
  sNC.pParse = pParse;
  sNC.pSrcList = pTabList;

  /* Resolve the column names in all the expressions of the
  ** of the UPDATE statement.  Also find the column index
  ** for each column to be updated in the pChanges array.  For each
  ** column to be updated, make sure we have authorization to change
  ** that column.
  */
  chngRowid = 0;
  for(i=0; i<pChanges->nExpr; i++){
    if( sqlite3ExprResolveNames(&sNC, pChanges->a[i].pExpr) ){
      goto update_cleanup;
    }
    for(j=0; j<pTab->nCol; j++){
      if( sqlite3StrICmp(pTab->aCol[j].zName, pChanges->a[i].zName)==0 ){
        if( j==pTab->iPKey ){
          chngRowid = 1;
          pRowidExpr = pChanges->a[i].pExpr;
        }
        aXRef[j] = i;
        break;
      }
    }
    if( j>=pTab->nCol ){
      if( sqlite3IsRowid(pChanges->a[i].zName) ){
        chngRowid = 1;
        pRowidExpr = pChanges->a[i].pExpr;
      }else{
        sqlite3ErrorMsg(pParse, "no such column: %s", pChanges->a[i].zName);
        goto update_cleanup;
      }
    }
#ifndef SQLITE_OMIT_AUTHORIZATION
    {
      int rc;
      rc = sqlite3AuthCheck(pParse, SQLITE_UPDATE, pTab->zName,
                           pTab->aCol[j].zName, db->aDb[iDb].zName);
      if( rc==SQLITE_DENY ){
        goto update_cleanup;
      }else if( rc==SQLITE_IGNORE ){
        aXRef[j] = -1;
      }
    }
#endif
  }

  /* Allocate memory for the array aRegIdx[].  There is one entry in the
  ** array for each index associated with table being updated.  Fill in
  ** the value with a register number for indices that are to be used
  ** and with zero for unused indices.
  */
  for(nIdx=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, nIdx++){}
  if( nIdx>0 ){
    aRegIdx = sqlite3DbMallocRaw(db, sizeof(Index*) * nIdx );
    if( aRegIdx==0 ) goto update_cleanup;
  }
  for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
    int reg;
    if( chngRowid ){
      reg = ++pParse->nMem;
    }else{
      reg = 0;
      for(i=0; i<pIdx->nColumn; i++){
        if( aXRef[pIdx->aiColumn[i]]>=0 ){
          reg = ++pParse->nMem;
          break;
        }
      }
    }
    aRegIdx[j] = reg;
  }

  /* Allocate a block of register used to store the change record
  ** sent to sqlite3GenerateConstraintChecks().  There are either
  ** one or two registers for holding the rowid.  One rowid register
  ** is used if chngRowid is false and two are used if chngRowid is
  ** true.  Following these are pTab->nCol register holding column
  ** data.
  */
  regOldRowid = regNewRowid = pParse->nMem + 1;
  pParse->nMem += pTab->nCol + 1;
  if( chngRowid ){
    regNewRowid++;
    pParse->nMem++;
  }
  regData = regNewRowid+1;
 

  /* Begin generating code.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ) goto update_cleanup;
  if( pParse->nested==0 ) sqlite3VdbeCountChanges(v);
  sqlite3BeginWriteOperation(pParse, 1, iDb);

#ifndef SQLITE_OMIT_VIRTUALTABLE
  /* Virtual tables must be handled separately */
  if( IsVirtual(pTab) ){
    updateVirtualTable(pParse, pTabList, pTab, pChanges, pRowidExpr, aXRef,
                       pWhere);
    pWhere = 0;
    pTabList = 0;
    goto update_cleanup;
  }
#endif

  /* Resolve the column names in all the expressions in the
  ** WHERE clause.
  */
  if( sqlite3ExprResolveNames(&sNC, pWhere) ){
    goto update_cleanup;
  }

  /* Start the view context
  */
  if( isView ){
    sqlite3AuthContextPush(pParse, &sContext, pTab->zName);
  }

  /* Generate the code for triggers.
  */
  if( triggers_exist ){
    int iGoto;

    /* Create pseudo-tables for NEW and OLD
    */
    sqlite3VdbeAddOp2(v, OP_OpenPseudo, oldIdx, 0);
    sqlite3VdbeAddOp2(v, OP_SetNumColumns, oldIdx, pTab->nCol);
    sqlite3VdbeAddOp2(v, OP_OpenPseudo, newIdx, 0);
    sqlite3VdbeAddOp2(v, OP_SetNumColumns, newIdx, pTab->nCol);

    iGoto = sqlite3VdbeAddOp2(v, OP_Goto, 0, 0);
    addr = sqlite3VdbeMakeLabel(v);
    iBeginBeforeTrigger = sqlite3VdbeCurrentAddr(v);
    if( sqlite3CodeRowTrigger(pParse, TK_UPDATE, pChanges, TRIGGER_BEFORE, pTab,
          newIdx, oldIdx, onError, addr, &old_col_mask, &new_col_mask) ){
      goto update_cleanup;
    }
    iEndBeforeTrigger = sqlite3VdbeAddOp2(v, OP_Goto, 0, 0);
    iBeginAfterTrigger = sqlite3VdbeCurrentAddr(v);
    if( sqlite3CodeRowTrigger(pParse, TK_UPDATE, pChanges, TRIGGER_AFTER, pTab, 
          newIdx, oldIdx, onError, addr, &old_col_mask, &new_col_mask) ){
      goto update_cleanup;
    }
    iEndAfterTrigger = sqlite3VdbeAddOp2(v, OP_Goto, 0, 0);
    sqlite3VdbeJumpHere(v, iGoto);
  }

  /* If we are trying to update a view, realize that view into
  ** a ephemeral table.
  */
  if( isView ){
    Select *pView;
    SelectDest dest;

    pView = sqlite3SelectDup(db, pTab->pSelect);
    sqlite3SelectMask(pParse, pView, old_col_mask|new_col_mask);
    sqlite3SelectDestInit(&dest, SRT_EphemTab, iCur);
    sqlite3Select(pParse, pView, &dest, 0, 0, 0, 0);
    sqlite3SelectDelete(pView);
  }

  /* Begin the database scan
  */
  pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, 0, 0);
  if( pWInfo==0 ) goto update_cleanup;

  /* Remember the rowid of every item to be updated.
  */
  sqlite3VdbeAddOp2(v, IsVirtual(pTab) ? OP_VRowid : OP_Rowid,iCur,regOldRowid);
  sqlite3VdbeAddOp2(v, OP_FifoWrite, regOldRowid, 0);

  /* End the database scan loop.
  */
  sqlite3WhereEnd(pWInfo);

  /* Initialize the count of updated rows
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack ){
    regRowCount = ++pParse->nMem;
    sqlite3VdbeAddOp2(v, OP_Integer, 0, regRowCount);
  }

  if( !isView && !IsVirtual(pTab) ){
    /* 
    ** Open every index that needs updating.  Note that if any
    ** index could potentially invoke a REPLACE conflict resolution 
    ** action, then we need to open all indices because we might need
    ** to be deleting some records.
    */
    sqlite3OpenTable(pParse, iCur, iDb, pTab, OP_OpenWrite); 
    if( onError==OE_Replace ){
      openAll = 1;
    }else{
      openAll = 0;
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        if( pIdx->onError==OE_Replace ){
          openAll = 1;
          break;
        }
      }
    }
    for(i=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, i++){
      if( openAll || aRegIdx[i]>0 ){
        KeyInfo *pKey = sqlite3IndexKeyinfo(pParse, pIdx);
        sqlite3VdbeAddOp4(v, OP_OpenWrite, iCur+i+1, pIdx->tnum, iDb,
                       (char*)pKey, P4_KEYINFO_HANDOFF);
        assert( pParse->nTab>iCur+i+1 );
      }
    }
  }
  
  /* Jump back to this point if a trigger encounters an IGNORE constraint. */
  if( triggers_exist ){
    sqlite3VdbeResolveLabel(v, addr);
  }

  /* Top of the update loop */
  addr = sqlite3VdbeAddOp2(v, OP_FifoRead, regOldRowid, 0);

  if( triggers_exist ){
    int regRowid;
    int regRow;
    int regCols;

    /* Make cursor iCur point to the record that is being updated.
    */
    sqlite3VdbeAddOp3(v, OP_NotExists, iCur, addr, regOldRowid);

    /* Generate the OLD table
    */
    regRowid = sqlite3GetTempReg(pParse);
    regRow = sqlite3GetTempReg(pParse);
    sqlite3VdbeAddOp2(v, OP_Rowid, iCur, regRowid);
    if( !old_col_mask ){
      sqlite3VdbeAddOp2(v, OP_Null, 0, regRow);
    }else{
      sqlite3VdbeAddOp2(v, OP_RowData, iCur, regRow);
    }
    sqlite3VdbeAddOp3(v, OP_Insert, oldIdx, regRow, regRowid);

    /* Generate the NEW table
    */
    if( chngRowid ){
      sqlite3ExprCodeAndCache(pParse, pRowidExpr, regRowid);
    }else{
      sqlite3VdbeAddOp2(v, OP_Rowid, iCur, regRowid);
    }
    regCols = sqlite3GetTempRange(pParse, pTab->nCol);
    for(i=0; i<pTab->nCol; i++){
      if( i==pTab->iPKey ){
        sqlite3VdbeAddOp2(v, OP_Null, 0, regCols+i);
        continue;
      }
      j = aXRef[i];
      if( new_col_mask&((u32)1<<i) || new_col_mask==0xffffffff ){
        if( j<0 ){
          sqlite3VdbeAddOp3(v, OP_Column, iCur, i, regCols+i);
          sqlite3ColumnDefault(v, pTab, i);
        }else{
          sqlite3ExprCodeAndCache(pParse, pChanges->a[j].pExpr, regCols+i);
        }
      }else{
        sqlite3VdbeAddOp2(v, OP_Null, 0, regCols+i);
      }
    }
    sqlite3VdbeAddOp3(v, OP_MakeRecord, regCols, pTab->nCol, regRow);
    if( !isView ){
      sqlite3TableAffinityStr(v, pTab);
    }
    sqlite3ReleaseTempRange(pParse, regCols, pTab->nCol);
    if( pParse->nErr ) goto update_cleanup;
    sqlite3VdbeAddOp3(v, OP_Insert, newIdx, regRow, regRowid);
    sqlite3ReleaseTempReg(pParse, regRowid);
    sqlite3ReleaseTempReg(pParse, regRow);

    sqlite3VdbeAddOp2(v, OP_Goto, 0, iBeginBeforeTrigger);
    sqlite3VdbeJumpHere(v, iEndBeforeTrigger);
  }

  if( !isView && !IsVirtual(pTab) ){
    /* Loop over every record that needs updating.  We have to load
    ** the old data for each record to be updated because some columns
    ** might not change and we will need to copy the old value.
    ** Also, the old data is needed to delete the old index entries.
    ** So make the cursor point at the old record.
    */
    sqlite3VdbeAddOp3(v, OP_NotExists, iCur, addr, regOldRowid);

    /* If the record number will change, push the record number as it
    ** will be after the update. (The old record number is currently
    ** on top of the stack.)
    */
    if( chngRowid ){
      sqlite3ExprCode(pParse, pRowidExpr, regNewRowid);
      sqlite3VdbeAddOp1(v, OP_MustBeInt, regNewRowid);
    }

    /* Compute new data for this record.  
    */
    for(i=0; i<pTab->nCol; i++){
      if( i==pTab->iPKey ){
        sqlite3VdbeAddOp2(v, OP_Null, 0, regData+i);
        continue;
      }
      j = aXRef[i];
      if( j<0 ){
        sqlite3VdbeAddOp3(v, OP_Column, iCur, i, regData+i);
        sqlite3ColumnDefault(v, pTab, i);
      }else{
        sqlite3ExprCode(pParse, pChanges->a[j].pExpr, regData+i);
      }
    }

    /* Do constraint checks
    */
    sqlite3GenerateConstraintChecks(pParse, pTab, iCur, regNewRowid,
                                    aRegIdx, chngRowid, 1,
                                    onError, addr);

    /* Delete the old indices for the current record.
    */
    j1 = sqlite3VdbeAddOp3(v, OP_NotExists, iCur, 0, regOldRowid);
    sqlite3GenerateRowIndexDelete(pParse, pTab, iCur, aRegIdx);

    /* If changing the record number, delete the old record.
    */
    if( chngRowid ){
      sqlite3VdbeAddOp2(v, OP_Delete, iCur, 0);
    }
    sqlite3VdbeJumpHere(v, j1);

    /* Create the new index entries and the new record.
    */
    sqlite3CompleteInsertion(pParse, pTab, iCur, regNewRowid, 
                             aRegIdx, chngRowid, 1, -1, 0);
  }

  /* Increment the row counter 
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack){
    sqlite3VdbeAddOp2(v, OP_AddImm, regRowCount, 1);
  }

  /* If there are triggers, close all the cursors after each iteration
  ** through the loop.  The fire the after triggers.
  */
  if( triggers_exist ){
    sqlite3VdbeAddOp2(v, OP_Goto, 0, iBeginAfterTrigger);
    sqlite3VdbeJumpHere(v, iEndAfterTrigger);
  }

  /* Repeat the above with the next record to be updated, until
  ** all record selected by the WHERE clause have been updated.
  */
  sqlite3VdbeAddOp2(v, OP_Goto, 0, addr);
  sqlite3VdbeJumpHere(v, addr);

  /* Close all tables */
  for(i=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, i++){
    if( openAll || aRegIdx[i]>0 ){
      sqlite3VdbeAddOp2(v, OP_Close, iCur+i+1, 0);
    }
  }
  sqlite3VdbeAddOp2(v, OP_Close, iCur, 0);
  if( triggers_exist ){
    sqlite3VdbeAddOp2(v, OP_Close, newIdx, 0);
    sqlite3VdbeAddOp2(v, OP_Close, oldIdx, 0);
  }

  /*
  ** Return the number of rows that were changed. If this routine is 
  ** generating code because of a call to sqlite3NestedParse(), do not
  ** invoke the callback function.
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack && pParse->nested==0 ){
    sqlite3VdbeAddOp2(v, OP_ResultRow, regRowCount, 1);
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "rows updated", P4_STATIC);
  }

update_cleanup:
  sqlite3AuthContextPop(&sContext);
  sqlite3_free(aRegIdx);
  sqlite3_free(aXRef);
  sqlite3SrcListDelete(pTabList);
  sqlite3ExprListDelete(pChanges);
  sqlite3ExprDelete(pWhere);
  return;
}

#ifndef SQLITE_OMIT_VIRTUALTABLE
/*
** Generate code for an UPDATE of a virtual table.
**
** The strategy is that we create an ephemerial table that contains
** for each row to be changed:
**
**   (A)  The original rowid of that row.
**   (B)  The revised rowid for the row. (note1)
**   (C)  The content of every column in the row.
**
** Then we loop over this ephemeral table and for each row in
** the ephermeral table call VUpdate.
**
** When finished, drop the ephemeral table.
**
** (note1) Actually, if we know in advance that (A) is always the same
** as (B) we only store (A), then duplicate (A) when pulling
** it out of the ephemeral table before calling VUpdate.
*/
static void updateVirtualTable(
  Parse *pParse,       /* The parsing context */
  SrcList *pSrc,       /* The virtual table to be modified */
  Table *pTab,         /* The virtual table */
  ExprList *pChanges,  /* The columns to change in the UPDATE statement */
  Expr *pRowid,        /* Expression used to recompute the rowid */
  int *aXRef,          /* Mapping from columns of pTab to entries in pChanges */
  Expr *pWhere         /* WHERE clause of the UPDATE statement */
){
  Vdbe *v = pParse->pVdbe;  /* Virtual machine under construction */
  ExprList *pEList = 0;     /* The result set of the SELECT statement */
  Select *pSelect = 0;      /* The SELECT statement */
  Expr *pExpr;              /* Temporary expression */
  int ephemTab;             /* Table holding the result of the SELECT */
  int i;                    /* Loop counter */
  int addr;                 /* Address of top of loop */
  int iReg;                 /* First register in set passed to OP_VUpdate */
  sqlite3 *db = pParse->db; /* Database connection */
  const char *pVtab = (const char*)pTab->pVtab;
  SelectDest dest;

  /* Construct the SELECT statement that will find the new values for
  ** all updated rows. 
  */
  pEList = sqlite3ExprListAppend(pParse, 0, 
                                 sqlite3CreateIdExpr(pParse, "_rowid_"), 0);
  if( pRowid ){
    pEList = sqlite3ExprListAppend(pParse, pEList,
                                   sqlite3ExprDup(db, pRowid), 0);
  }
  assert( pTab->iPKey<0 );
  for(i=0; i<pTab->nCol; i++){
    if( aXRef[i]>=0 ){
      pExpr = sqlite3ExprDup(db, pChanges->a[aXRef[i]].pExpr);
    }else{
      pExpr = sqlite3CreateIdExpr(pParse, pTab->aCol[i].zName);
    }
    pEList = sqlite3ExprListAppend(pParse, pEList, pExpr, 0);
  }
  pSelect = sqlite3SelectNew(pParse, pEList, pSrc, pWhere, 0, 0, 0, 0, 0, 0);
  
  /* Create the ephemeral table into which the update results will
  ** be stored.
  */
  assert( v );
  ephemTab = pParse->nTab++;
  sqlite3VdbeAddOp2(v, OP_OpenEphemeral, ephemTab, pTab->nCol+1+(pRowid!=0));

  /* fill the ephemeral table 
  */
  sqlite3SelectDestInit(&dest, SRT_Table, ephemTab);
  sqlite3Select(pParse, pSelect, &dest, 0, 0, 0, 0);

  /* Generate code to scan the ephemeral table and call VUpdate. */
  iReg = ++pParse->nMem;
  pParse->nMem += pTab->nCol+1;
  sqlite3VdbeAddOp2(v, OP_Rewind, ephemTab, 0);
  addr = sqlite3VdbeCurrentAddr(v);
  sqlite3VdbeAddOp3(v, OP_Column,  ephemTab, 0, iReg);
  sqlite3VdbeAddOp3(v, OP_Column, ephemTab, (pRowid?1:0), iReg+1);
  for(i=0; i<pTab->nCol; i++){
    sqlite3VdbeAddOp3(v, OP_Column, ephemTab, i+1+(pRowid!=0), iReg+2+i);
  }
  pParse->pVirtualLock = pTab;
  sqlite3VdbeAddOp4(v, OP_VUpdate, 0, pTab->nCol+2, iReg, pVtab, P4_VTAB);
  sqlite3VdbeAddOp2(v, OP_Next, ephemTab, addr);
  sqlite3VdbeJumpHere(v, addr-1);
  sqlite3VdbeAddOp2(v, OP_Close, ephemTab, 0);

  /* Cleanup */
  sqlite3SelectDelete(pSelect);  
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */
