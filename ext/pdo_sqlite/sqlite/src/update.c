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

/*
** The most recently coded instruction was an OP_Column to retrieve column
** 'i' of table pTab. This routine sets the P3 parameter of the 
** OP_Column to the default value, if any.
**
** The default value of a column is specified by a DEFAULT clause in the 
** column definition. This was either supplied by the user when the table
** was created, or added later to the table definition by an ALTER TABLE
** command. If the latter, then the row-records in the table btree on disk
** may not contain a value for the column and the default value, taken
** from the P3 parameter of the OP_Column instruction, is returned instead.
** If the former, then all row-records are guaranteed to include a value
** for the column and the P3 value is not required.
**
** Column definitions created by an ALTER TABLE command may only have 
** literal default values specified: a number, null or a string. (If a more
** complicated default expression value was provided, it is evaluated 
** when the ALTER TABLE is executed and one of the literal values written
** into the sqlite_master table.)
**
** Therefore, the P3 parameter is only required if the default value for
** the column is a literal number, string or null. The sqlite3ValueFromExpr()
** function is capable of transforming these types of expressions into
** sqlite3_value objects.
*/
void sqlite3ColumnDefault(Vdbe *v, Table *pTab, int i){
  if( pTab && !pTab->pSelect ){
    sqlite3_value *pValue;
    u8 enc = sqlite3VdbeDb(v)->enc;
    Column *pCol = &pTab->aCol[i];
    sqlite3ValueFromExpr(pCol->pDflt, enc, pCol->affinity, &pValue);
    if( pValue ){
      sqlite3VdbeChangeP3(v, -1, (const char *)pValue, P3_MEM);
    }else{
      VdbeComment((v, "# %s.%s", pTab->zName, pCol->zName));
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
  int nIdxTotal;         /* Total number of indices */
  int iCur;              /* VDBE Cursor number of pTab */
  sqlite3 *db;           /* The database structure */
  Index **apIdx = 0;     /* An array of indices that need updating too */
  char *aIdxUsed = 0;    /* aIdxUsed[i]==1 if the i-th index is used */
  int *aXRef = 0;        /* aXRef[i] is the index in pChanges->a[] of the
                         ** an expression for the i-th column of the table.
                         ** aXRef[i]==-1 if the i-th column is not changed. */
  int chngRowid;         /* True if the record number is being changed */
  Expr *pRowidExpr = 0;  /* Expression defining the new record number */
  int openAll = 0;       /* True if all indices need to be opened */
  AuthContext sContext;  /* The authorization context */
  NameContext sNC;       /* The name-context to resolve expressions in */

#ifndef SQLITE_OMIT_TRIGGER
  int isView;                  /* Trying to update a view */
  int triggers_exist = 0;      /* True if any row triggers exist */
#endif

  int newIdx      = -1;  /* index of trigger "new" temp table       */
  int oldIdx      = -1;  /* index of trigger "old" temp table       */

  sContext.pParse = 0;
  if( pParse->nErr || sqlite3_malloc_failed ) goto update_cleanup;
  db = pParse->db;
  assert( pTabList->nSrc==1 );

  /* Locate the table which we want to update. 
  */
  pTab = sqlite3SrcListLookup(pParse, pTabList);
  if( pTab==0 ) goto update_cleanup;

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
  if( isView ){
    if( sqlite3ViewGetColumnNames(pParse, pTab) ){
      goto update_cleanup;
    }
  }
  aXRef = sqliteMallocRaw( sizeof(int) * pTab->nCol );
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
                           pTab->aCol[j].zName, db->aDb[pTab->iDb].zName);
      if( rc==SQLITE_DENY ){
        goto update_cleanup;
      }else if( rc==SQLITE_IGNORE ){
        aXRef[j] = -1;
      }
    }
#endif
  }

  /* Allocate memory for the array apIdx[] and fill it with pointers to every
  ** index that needs to be updated.  Indices only need updating if their
  ** key includes one of the columns named in pChanges or if the record
  ** number of the original table entry is changing.
  */
  for(nIdx=nIdxTotal=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, nIdxTotal++){
    if( chngRowid ){
      i = 0;
    }else {
      for(i=0; i<pIdx->nColumn; i++){
        if( aXRef[pIdx->aiColumn[i]]>=0 ) break;
      }
    }
    if( i<pIdx->nColumn ) nIdx++;
  }
  if( nIdxTotal>0 ){
    apIdx = sqliteMallocRaw( sizeof(Index*) * nIdx + nIdxTotal );
    if( apIdx==0 ) goto update_cleanup;
    aIdxUsed = (char*)&apIdx[nIdx];
  }
  for(nIdx=j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
    if( chngRowid ){
      i = 0;
    }else{
      for(i=0; i<pIdx->nColumn; i++){
        if( aXRef[pIdx->aiColumn[i]]>=0 ) break;
      }
    }
    if( i<pIdx->nColumn ){
      if( sqlite3CheckIndexCollSeq(pParse, pIdx) ) goto update_cleanup;
      apIdx[nIdx++] = pIdx;
      aIdxUsed[j] = 1;
    }else{
      aIdxUsed[j] = 0;
    }
  }

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

  /* Begin generating code.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ) goto update_cleanup;
  if( pParse->nested==0 ) sqlite3VdbeCountChanges(v);
  sqlite3BeginWriteOperation(pParse, 1, pTab->iDb);

  /* If we are trying to update a view, construct that view into
  ** a temporary table.
  */
  if( isView ){
    Select *pView;
    pView = sqlite3SelectDup(pTab->pSelect);
    sqlite3Select(pParse, pView, SRT_TempTable, iCur, 0, 0, 0, 0);
    sqlite3SelectDelete(pView);
  }

  /* Begin the database scan
  */
  pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, 0);
  if( pWInfo==0 ) goto update_cleanup;

  /* Remember the index of every item to be updated.
  */
  sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
  sqlite3VdbeAddOp(v, OP_FifoWrite, 0, 0);

  /* End the database scan loop.
  */
  sqlite3WhereEnd(pWInfo);

  /* Initialize the count of updated rows
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack ){
    sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
  }

  if( triggers_exist ){
    /* Create pseudo-tables for NEW and OLD
    */
    sqlite3VdbeAddOp(v, OP_OpenPseudo, oldIdx, 0);
    sqlite3VdbeAddOp(v, OP_SetNumColumns, oldIdx, pTab->nCol);
    sqlite3VdbeAddOp(v, OP_OpenPseudo, newIdx, 0);
    sqlite3VdbeAddOp(v, OP_SetNumColumns, newIdx, pTab->nCol);

    /* The top of the update loop for when there are triggers.
    */
    addr = sqlite3VdbeAddOp(v, OP_FifoRead, 0, 0);

    if( !isView ){
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      /* Open a cursor and make it point to the record that is
      ** being updated.
      */
      sqlite3OpenTableForReading(v, iCur, pTab);
    }
    sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);

    /* Generate the OLD table
    */
    sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
    sqlite3VdbeAddOp(v, OP_RowData, iCur, 0);
    sqlite3VdbeAddOp(v, OP_Insert, oldIdx, 0);

    /* Generate the NEW table
    */
    if( chngRowid ){
      sqlite3ExprCodeAndCache(pParse, pRowidExpr);
    }else{
      sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
    }
    for(i=0; i<pTab->nCol; i++){
      if( i==pTab->iPKey ){
        sqlite3VdbeAddOp(v, OP_Null, 0, 0);
        continue;
      }
      j = aXRef[i];
      if( j<0 ){
        sqlite3VdbeAddOp(v, OP_Column, iCur, i);
        sqlite3ColumnDefault(v, pTab, i);
      }else{
        sqlite3ExprCodeAndCache(pParse, pChanges->a[j].pExpr);
      }
    }
    sqlite3VdbeAddOp(v, OP_MakeRecord, pTab->nCol, 0);
    if( !isView ){
      sqlite3TableAffinityStr(v, pTab);
    }
    if( pParse->nErr ) goto update_cleanup;
    sqlite3VdbeAddOp(v, OP_Insert, newIdx, 0);
    if( !isView ){
      sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
    }

    /* Fire the BEFORE and INSTEAD OF triggers
    */
    if( sqlite3CodeRowTrigger(pParse, TK_UPDATE, pChanges, TRIGGER_BEFORE, pTab,
          newIdx, oldIdx, onError, addr) ){
      goto update_cleanup;
    }
  }

  if( !isView ){
    /* 
    ** Open every index that needs updating.  Note that if any
    ** index could potentially invoke a REPLACE conflict resolution 
    ** action, then we need to open all indices because we might need
    ** to be deleting some records.
    */
    sqlite3VdbeAddOp(v, OP_Integer, pTab->iDb, 0);
    sqlite3VdbeAddOp(v, OP_OpenWrite, iCur, pTab->tnum);
    sqlite3VdbeAddOp(v, OP_SetNumColumns, iCur, pTab->nCol);
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
      if( openAll || aIdxUsed[i] ){
        sqlite3VdbeAddOp(v, OP_Integer, pIdx->iDb, 0);
        sqlite3VdbeOp3(v, OP_OpenWrite, iCur+i+1, pIdx->tnum,
                       (char*)&pIdx->keyInfo, P3_KEYINFO);
        assert( pParse->nTab>iCur+i+1 );
      }
    }

    /* Loop over every record that needs updating.  We have to load
    ** the old data for each record to be updated because some columns
    ** might not change and we will need to copy the old value.
    ** Also, the old data is needed to delete the old index entires.
    ** So make the cursor point at the old record.
    */
    if( !triggers_exist ){
      addr = sqlite3VdbeAddOp(v, OP_FifoRead, 0, 0);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
    }
    sqlite3VdbeAddOp(v, OP_NotExists, iCur, addr);

    /* If the record number will change, push the record number as it
    ** will be after the update. (The old record number is currently
    ** on top of the stack.)
    */
    if( chngRowid ){
      sqlite3ExprCode(pParse, pRowidExpr);
      sqlite3VdbeAddOp(v, OP_MustBeInt, 0, 0);
    }

    /* Compute new data for this record.  
    */
    for(i=0; i<pTab->nCol; i++){
      if( i==pTab->iPKey ){
        sqlite3VdbeAddOp(v, OP_Null, 0, 0);
        continue;
      }
      j = aXRef[i];
      if( j<0 ){
        sqlite3VdbeAddOp(v, OP_Column, iCur, i);
        sqlite3ColumnDefault(v, pTab, i);
      }else{
        sqlite3ExprCode(pParse, pChanges->a[j].pExpr);
      }
    }

    /* Do constraint checks
    */
    sqlite3GenerateConstraintChecks(pParse, pTab, iCur, aIdxUsed, chngRowid, 1,
                                   onError, addr);

    /* Delete the old indices for the current record.
    */
    sqlite3GenerateRowIndexDelete(db, v, pTab, iCur, aIdxUsed);

    /* If changing the record number, delete the old record.
    */
    if( chngRowid ){
      sqlite3VdbeAddOp(v, OP_Delete, iCur, 0);
    }

    /* Create the new index entries and the new record.
    */
    sqlite3CompleteInsertion(pParse, pTab, iCur, aIdxUsed, chngRowid, 1, -1);
  }

  /* Increment the row counter 
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack){
    sqlite3VdbeAddOp(v, OP_AddImm, 1, 0);
  }

  /* If there are triggers, close all the cursors after each iteration
  ** through the loop.  The fire the after triggers.
  */
  if( triggers_exist ){
    if( !isView ){
      for(i=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, i++){
        if( openAll || aIdxUsed[i] )
          sqlite3VdbeAddOp(v, OP_Close, iCur+i+1, 0);
      }
      sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
    }
    if( sqlite3CodeRowTrigger(pParse, TK_UPDATE, pChanges, TRIGGER_AFTER, pTab, 
          newIdx, oldIdx, onError, addr) ){
      goto update_cleanup;
    }
  }

  /* Repeat the above with the next record to be updated, until
  ** all record selected by the WHERE clause have been updated.
  */
  sqlite3VdbeAddOp(v, OP_Goto, 0, addr);
  sqlite3VdbeChangeP2(v, addr, sqlite3VdbeCurrentAddr(v));

  /* Close all tables if there were no FOR EACH ROW triggers */
  if( !triggers_exist ){
    for(i=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, i++){
      if( openAll || aIdxUsed[i] ){
        sqlite3VdbeAddOp(v, OP_Close, iCur+i+1, 0);
      }
    }
    sqlite3VdbeAddOp(v, OP_Close, iCur, 0);
  }else{
    sqlite3VdbeAddOp(v, OP_Close, newIdx, 0);
    sqlite3VdbeAddOp(v, OP_Close, oldIdx, 0);
  }

  /*
  ** Return the number of rows that were changed. If this routine is 
  ** generating code because of a call to sqlite3NestedParse(), do not
  ** invoke the callback function.
  */
  if( db->flags & SQLITE_CountRows && !pParse->trigStack && pParse->nested==0 ){
    sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, "rows updated", P3_STATIC);
  }

update_cleanup:
  sqlite3AuthContextPop(&sContext);
  sqliteFree(apIdx);
  sqliteFree(aXRef);
  sqlite3SrcListDelete(pTabList);
  sqlite3ExprListDelete(pChanges);
  sqlite3ExprDelete(pWhere);
  return;
}
