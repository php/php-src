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
** to handle SELECT statements in SQLite.
**
** $Id$
*/
#include "sqliteInt.h"

/*
** Delete all the content of a Select structure but do not deallocate
** the select structure itself.
*/
static void clearSelect(Select *p){
  sqlite3ExprListDelete(p->pEList);
  sqlite3SrcListDelete(p->pSrc);
  sqlite3ExprDelete(p->pWhere);
  sqlite3ExprListDelete(p->pGroupBy);
  sqlite3ExprDelete(p->pHaving);
  sqlite3ExprListDelete(p->pOrderBy);
  sqlite3SelectDelete(p->pPrior);
  sqlite3ExprDelete(p->pLimit);
  sqlite3ExprDelete(p->pOffset);
}

/*
** Allocate a new Select structure and return a pointer to that
** structure.
*/
Select *sqlite3SelectNew(
  ExprList *pEList,     /* which columns to include in the result */
  SrcList *pSrc,        /* the FROM clause -- which tables to scan */
  Expr *pWhere,         /* the WHERE clause */
  ExprList *pGroupBy,   /* the GROUP BY clause */
  Expr *pHaving,        /* the HAVING clause */
  ExprList *pOrderBy,   /* the ORDER BY clause */
  int isDistinct,       /* true if the DISTINCT keyword is present */
  Expr *pLimit,         /* LIMIT value.  NULL means not used */
  Expr *pOffset         /* OFFSET value.  NULL means no offset */
){
  Select *pNew;
  Select standin;
  pNew = sqliteMalloc( sizeof(*pNew) );
  assert( !pOffset || pLimit );   /* Can't have OFFSET without LIMIT. */
  if( pNew==0 ){
    pNew = &standin;
    memset(pNew, 0, sizeof(*pNew));
  }
  if( pEList==0 ){
    pEList = sqlite3ExprListAppend(0, sqlite3Expr(TK_ALL,0,0,0), 0);
  }
  pNew->pEList = pEList;
  pNew->pSrc = pSrc;
  pNew->pWhere = pWhere;
  pNew->pGroupBy = pGroupBy;
  pNew->pHaving = pHaving;
  pNew->pOrderBy = pOrderBy;
  pNew->isDistinct = isDistinct;
  pNew->op = TK_SELECT;
  assert( pOffset==0 || pLimit!=0 );
  pNew->pLimit = pLimit;
  pNew->pOffset = pOffset;
  pNew->iLimit = -1;
  pNew->iOffset = -1;
  pNew->addrOpenEphm[0] = -1;
  pNew->addrOpenEphm[1] = -1;
  pNew->addrOpenEphm[2] = -1;
  if( pNew==&standin) {
    clearSelect(pNew);
    pNew = 0;
  }
  return pNew;
}

/*
** Delete the given Select structure and all of its substructures.
*/
void sqlite3SelectDelete(Select *p){
  if( p ){
    clearSelect(p);
    sqliteFree(p);
  }
}

/*
** Given 1 to 3 identifiers preceeding the JOIN keyword, determine the
** type of join.  Return an integer constant that expresses that type
** in terms of the following bit values:
**
**     JT_INNER
**     JT_CROSS
**     JT_OUTER
**     JT_NATURAL
**     JT_LEFT
**     JT_RIGHT
**
** A full outer join is the combination of JT_LEFT and JT_RIGHT.
**
** If an illegal or unsupported join type is seen, then still return
** a join type, but put an error in the pParse structure.
*/
int sqlite3JoinType(Parse *pParse, Token *pA, Token *pB, Token *pC){
  int jointype = 0;
  Token *apAll[3];
  Token *p;
  static const struct {
    const char zKeyword[8];
    u8 nChar;
    u8 code;
  } keywords[] = {
    { "natural", 7, JT_NATURAL },
    { "left",    4, JT_LEFT|JT_OUTER },
    { "right",   5, JT_RIGHT|JT_OUTER },
    { "full",    4, JT_LEFT|JT_RIGHT|JT_OUTER },
    { "outer",   5, JT_OUTER },
    { "inner",   5, JT_INNER },
    { "cross",   5, JT_INNER|JT_CROSS },
  };
  int i, j;
  apAll[0] = pA;
  apAll[1] = pB;
  apAll[2] = pC;
  for(i=0; i<3 && apAll[i]; i++){
    p = apAll[i];
    for(j=0; j<sizeof(keywords)/sizeof(keywords[0]); j++){
      if( p->n==keywords[j].nChar 
          && sqlite3StrNICmp((char*)p->z, keywords[j].zKeyword, p->n)==0 ){
        jointype |= keywords[j].code;
        break;
      }
    }
    if( j>=sizeof(keywords)/sizeof(keywords[0]) ){
      jointype |= JT_ERROR;
      break;
    }
  }
  if(
     (jointype & (JT_INNER|JT_OUTER))==(JT_INNER|JT_OUTER) ||
     (jointype & JT_ERROR)!=0
  ){
    const char *zSp1 = " ";
    const char *zSp2 = " ";
    if( pB==0 ){ zSp1++; }
    if( pC==0 ){ zSp2++; }
    sqlite3ErrorMsg(pParse, "unknown or unsupported join type: "
       "%T%s%T%s%T", pA, zSp1, pB, zSp2, pC);
    jointype = JT_INNER;
  }else if( jointype & JT_RIGHT ){
    sqlite3ErrorMsg(pParse, 
      "RIGHT and FULL OUTER JOINs are not currently supported");
    jointype = JT_INNER;
  }
  return jointype;
}

/*
** Return the index of a column in a table.  Return -1 if the column
** is not contained in the table.
*/
static int columnIndex(Table *pTab, const char *zCol){
  int i;
  for(i=0; i<pTab->nCol; i++){
    if( sqlite3StrICmp(pTab->aCol[i].zName, zCol)==0 ) return i;
  }
  return -1;
}

/*
** Set the value of a token to a '\000'-terminated string.
*/
static void setToken(Token *p, const char *z){
  p->z = (u8*)z;
  p->n = z ? strlen(z) : 0;
  p->dyn = 0;
}

/*
** Create an expression node for an identifier with the name of zName
*/
Expr *sqlite3CreateIdExpr(const char *zName){
  Token dummy;
  setToken(&dummy, zName);
  return sqlite3Expr(TK_ID, 0, 0, &dummy);
}


/*
** Add a term to the WHERE expression in *ppExpr that requires the
** zCol column to be equal in the two tables pTab1 and pTab2.
*/
static void addWhereTerm(
  const char *zCol,        /* Name of the column */
  const Table *pTab1,      /* First table */
  const char *zAlias1,     /* Alias for first table.  May be NULL */
  const Table *pTab2,      /* Second table */
  const char *zAlias2,     /* Alias for second table.  May be NULL */
  int iRightJoinTable,     /* VDBE cursor for the right table */
  Expr **ppExpr            /* Add the equality term to this expression */
){
  Expr *pE1a, *pE1b, *pE1c;
  Expr *pE2a, *pE2b, *pE2c;
  Expr *pE;

  pE1a = sqlite3CreateIdExpr(zCol);
  pE2a = sqlite3CreateIdExpr(zCol);
  if( zAlias1==0 ){
    zAlias1 = pTab1->zName;
  }
  pE1b = sqlite3CreateIdExpr(zAlias1);
  if( zAlias2==0 ){
    zAlias2 = pTab2->zName;
  }
  pE2b = sqlite3CreateIdExpr(zAlias2);
  pE1c = sqlite3ExprOrFree(TK_DOT, pE1b, pE1a, 0);
  pE2c = sqlite3ExprOrFree(TK_DOT, pE2b, pE2a, 0);
  pE = sqlite3ExprOrFree(TK_EQ, pE1c, pE2c, 0);
  if( pE ){
    ExprSetProperty(pE, EP_FromJoin);
    pE->iRightJoinTable = iRightJoinTable;
  }
  pE = sqlite3ExprAnd(*ppExpr, pE);
  if( pE ){
    *ppExpr = pE;
  }
}

/*
** Set the EP_FromJoin property on all terms of the given expression.
** And set the Expr.iRightJoinTable to iTable for every term in the
** expression.
**
** The EP_FromJoin property is used on terms of an expression to tell
** the LEFT OUTER JOIN processing logic that this term is part of the
** join restriction specified in the ON or USING clause and not a part
** of the more general WHERE clause.  These terms are moved over to the
** WHERE clause during join processing but we need to remember that they
** originated in the ON or USING clause.
**
** The Expr.iRightJoinTable tells the WHERE clause processing that the
** expression depends on table iRightJoinTable even if that table is not
** explicitly mentioned in the expression.  That information is needed
** for cases like this:
**
**    SELECT * FROM t1 LEFT JOIN t2 ON t1.a=t2.b AND t1.x=5
**
** The where clause needs to defer the handling of the t1.x=5
** term until after the t2 loop of the join.  In that way, a
** NULL t2 row will be inserted whenever t1.x!=5.  If we do not
** defer the handling of t1.x=5, it will be processed immediately
** after the t1 loop and rows with t1.x!=5 will never appear in
** the output, which is incorrect.
*/
static void setJoinExpr(Expr *p, int iTable){
  while( p ){
    ExprSetProperty(p, EP_FromJoin);
    p->iRightJoinTable = iTable;
    setJoinExpr(p->pLeft, iTable);
    p = p->pRight;
  } 
}

/*
** This routine processes the join information for a SELECT statement.
** ON and USING clauses are converted into extra terms of the WHERE clause.
** NATURAL joins also create extra WHERE clause terms.
**
** The terms of a FROM clause are contained in the Select.pSrc structure.
** The left most table is the first entry in Select.pSrc.  The right-most
** table is the last entry.  The join operator is held in the entry to
** the left.  Thus entry 0 contains the join operator for the join between
** entries 0 and 1.  Any ON or USING clauses associated with the join are
** also attached to the left entry.
**
** This routine returns the number of errors encountered.
*/
static int sqliteProcessJoin(Parse *pParse, Select *p){
  SrcList *pSrc;                  /* All tables in the FROM clause */
  int i, j;                       /* Loop counters */
  struct SrcList_item *pLeft;     /* Left table being joined */
  struct SrcList_item *pRight;    /* Right table being joined */

  pSrc = p->pSrc;
  pLeft = &pSrc->a[0];
  pRight = &pLeft[1];
  for(i=0; i<pSrc->nSrc-1; i++, pRight++, pLeft++){
    Table *pLeftTab = pLeft->pTab;
    Table *pRightTab = pRight->pTab;

    if( pLeftTab==0 || pRightTab==0 ) continue;

    /* When the NATURAL keyword is present, add WHERE clause terms for
    ** every column that the two tables have in common.
    */
    if( pRight->jointype & JT_NATURAL ){
      if( pRight->pOn || pRight->pUsing ){
        sqlite3ErrorMsg(pParse, "a NATURAL join may not have "
           "an ON or USING clause", 0);
        return 1;
      }
      for(j=0; j<pLeftTab->nCol; j++){
        char *zName = pLeftTab->aCol[j].zName;
        if( columnIndex(pRightTab, zName)>=0 ){
          addWhereTerm(zName, pLeftTab, pLeft->zAlias, 
                              pRightTab, pRight->zAlias,
                              pRight->iCursor, &p->pWhere);
          
        }
      }
    }

    /* Disallow both ON and USING clauses in the same join
    */
    if( pRight->pOn && pRight->pUsing ){
      sqlite3ErrorMsg(pParse, "cannot have both ON and USING "
        "clauses in the same join");
      return 1;
    }

    /* Add the ON clause to the end of the WHERE clause, connected by
    ** an AND operator.
    */
    if( pRight->pOn ){
      setJoinExpr(pRight->pOn, pRight->iCursor);
      p->pWhere = sqlite3ExprAnd(p->pWhere, pRight->pOn);
      pRight->pOn = 0;
    }

    /* Create extra terms on the WHERE clause for each column named
    ** in the USING clause.  Example: If the two tables to be joined are 
    ** A and B and the USING clause names X, Y, and Z, then add this
    ** to the WHERE clause:    A.X=B.X AND A.Y=B.Y AND A.Z=B.Z
    ** Report an error if any column mentioned in the USING clause is
    ** not contained in both tables to be joined.
    */
    if( pRight->pUsing ){
      IdList *pList = pRight->pUsing;
      for(j=0; j<pList->nId; j++){
        char *zName = pList->a[j].zName;
        if( columnIndex(pLeftTab, zName)<0 || columnIndex(pRightTab, zName)<0 ){
          sqlite3ErrorMsg(pParse, "cannot join using column %s - column "
            "not present in both tables", zName);
          return 1;
        }
        addWhereTerm(zName, pLeftTab, pLeft->zAlias, 
                            pRightTab, pRight->zAlias,
                            pRight->iCursor, &p->pWhere);
      }
    }
  }
  return 0;
}

/*
** Insert code into "v" that will push the record on the top of the
** stack into the sorter.
*/
static void pushOntoSorter(
  Parse *pParse,         /* Parser context */
  ExprList *pOrderBy,    /* The ORDER BY clause */
  Select *pSelect        /* The whole SELECT statement */
){
  Vdbe *v = pParse->pVdbe;
  sqlite3ExprCodeExprList(pParse, pOrderBy);
  sqlite3VdbeAddOp(v, OP_Sequence, pOrderBy->iECursor, 0);
  sqlite3VdbeAddOp(v, OP_Pull, pOrderBy->nExpr + 1, 0);
  sqlite3VdbeAddOp(v, OP_MakeRecord, pOrderBy->nExpr + 2, 0);
  sqlite3VdbeAddOp(v, OP_IdxInsert, pOrderBy->iECursor, 0);
  if( pSelect->iLimit>=0 ){
    int addr1, addr2;
    addr1 = sqlite3VdbeAddOp(v, OP_IfMemZero, pSelect->iLimit+1, 0);
    sqlite3VdbeAddOp(v, OP_MemIncr, -1, pSelect->iLimit+1);
    addr2 = sqlite3VdbeAddOp(v, OP_Goto, 0, 0);
    sqlite3VdbeJumpHere(v, addr1);
    sqlite3VdbeAddOp(v, OP_Last, pOrderBy->iECursor, 0);
    sqlite3VdbeAddOp(v, OP_Delete, pOrderBy->iECursor, 0);
    sqlite3VdbeJumpHere(v, addr2);
    pSelect->iLimit = -1;
  }
}

/*
** Add code to implement the OFFSET
*/
static void codeOffset(
  Vdbe *v,          /* Generate code into this VM */
  Select *p,        /* The SELECT statement being coded */
  int iContinue,    /* Jump here to skip the current record */
  int nPop          /* Number of times to pop stack when jumping */
){
  if( p->iOffset>=0 && iContinue!=0 ){
    int addr;
    sqlite3VdbeAddOp(v, OP_MemIncr, -1, p->iOffset);
    addr = sqlite3VdbeAddOp(v, OP_IfMemNeg, p->iOffset, 0);
    if( nPop>0 ){
      sqlite3VdbeAddOp(v, OP_Pop, nPop, 0);
    }
    sqlite3VdbeAddOp(v, OP_Goto, 0, iContinue);
    VdbeComment((v, "# skip OFFSET records"));
    sqlite3VdbeJumpHere(v, addr);
  }
}

/*
** Add code that will check to make sure the top N elements of the
** stack are distinct.  iTab is a sorting index that holds previously
** seen combinations of the N values.  A new entry is made in iTab
** if the current N values are new.
**
** A jump to addrRepeat is made and the N+1 values are popped from the
** stack if the top N elements are not distinct.
*/
static void codeDistinct(
  Vdbe *v,           /* Generate code into this VM */
  int iTab,          /* A sorting index used to test for distinctness */
  int addrRepeat,    /* Jump to here if not distinct */
  int N              /* The top N elements of the stack must be distinct */
){
  sqlite3VdbeAddOp(v, OP_MakeRecord, -N, 0);
  sqlite3VdbeAddOp(v, OP_Distinct, iTab, sqlite3VdbeCurrentAddr(v)+3);
  sqlite3VdbeAddOp(v, OP_Pop, N+1, 0);
  sqlite3VdbeAddOp(v, OP_Goto, 0, addrRepeat);
  VdbeComment((v, "# skip indistinct records"));
  sqlite3VdbeAddOp(v, OP_IdxInsert, iTab, 0);
}


/*
** This routine generates the code for the inside of the inner loop
** of a SELECT.
**
** If srcTab and nColumn are both zero, then the pEList expressions
** are evaluated in order to get the data for this row.  If nColumn>0
** then data is pulled from srcTab and pEList is used only to get the
** datatypes for each column.
*/
static int selectInnerLoop(
  Parse *pParse,          /* The parser context */
  Select *p,              /* The complete select statement being coded */
  ExprList *pEList,       /* List of values being extracted */
  int srcTab,             /* Pull data from this table */
  int nColumn,            /* Number of columns in the source table */
  ExprList *pOrderBy,     /* If not NULL, sort results using this key */
  int distinct,           /* If >=0, make sure results are distinct */
  int eDest,              /* How to dispose of the results */
  int iParm,              /* An argument to the disposal method */
  int iContinue,          /* Jump here to continue with next row */
  int iBreak,             /* Jump here to break out of the inner loop */
  char *aff               /* affinity string if eDest is SRT_Union */
){
  Vdbe *v = pParse->pVdbe;
  int i;
  int hasDistinct;        /* True if the DISTINCT keyword is present */

  if( v==0 ) return 0;
  assert( pEList!=0 );

  /* If there was a LIMIT clause on the SELECT statement, then do the check
  ** to see if this row should be output.
  */
  hasDistinct = distinct>=0 && pEList->nExpr>0;
  if( pOrderBy==0 && !hasDistinct ){
    codeOffset(v, p, iContinue, 0);
  }

  /* Pull the requested columns.
  */
  if( nColumn>0 ){
    for(i=0; i<nColumn; i++){
      sqlite3VdbeAddOp(v, OP_Column, srcTab, i);
    }
  }else{
    nColumn = pEList->nExpr;
    sqlite3ExprCodeExprList(pParse, pEList);
  }

  /* If the DISTINCT keyword was present on the SELECT statement
  ** and this row has been seen before, then do not make this row
  ** part of the result.
  */
  if( hasDistinct ){
    assert( pEList!=0 );
    assert( pEList->nExpr==nColumn );
    codeDistinct(v, distinct, iContinue, nColumn);
    if( pOrderBy==0 ){
      codeOffset(v, p, iContinue, nColumn);
    }
  }

  switch( eDest ){
    /* In this mode, write each query result to the key of the temporary
    ** table iParm.
    */
#ifndef SQLITE_OMIT_COMPOUND_SELECT
    case SRT_Union: {
      sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
      if( aff ){
        sqlite3VdbeChangeP3(v, -1, aff, P3_STATIC);
      }
      sqlite3VdbeAddOp(v, OP_IdxInsert, iParm, 0);
      break;
    }

    /* Construct a record from the query result, but instead of
    ** saving that record, use it as a key to delete elements from
    ** the temporary table iParm.
    */
    case SRT_Except: {
      int addr;
      addr = sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
      sqlite3VdbeChangeP3(v, -1, aff, P3_STATIC);
      sqlite3VdbeAddOp(v, OP_NotFound, iParm, addr+3);
      sqlite3VdbeAddOp(v, OP_Delete, iParm, 0);
      break;
    }
#endif

    /* Store the result as data using a unique key.
    */
    case SRT_Table:
    case SRT_EphemTab: {
      sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
      if( pOrderBy ){
        pushOntoSorter(pParse, pOrderBy, p);
      }else{
        sqlite3VdbeAddOp(v, OP_NewRowid, iParm, 0);
        sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
        sqlite3VdbeAddOp(v, OP_Insert, iParm, OPFLAG_APPEND);
      }
      break;
    }

#ifndef SQLITE_OMIT_SUBQUERY
    /* If we are creating a set for an "expr IN (SELECT ...)" construct,
    ** then there should be a single item on the stack.  Write this
    ** item into the set table with bogus data.
    */
    case SRT_Set: {
      int addr1 = sqlite3VdbeCurrentAddr(v);
      int addr2;

      assert( nColumn==1 );
      sqlite3VdbeAddOp(v, OP_NotNull, -1, addr1+3);
      sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      addr2 = sqlite3VdbeAddOp(v, OP_Goto, 0, 0);
      p->affinity = sqlite3CompareAffinity(pEList->a[0].pExpr,(iParm>>16)&0xff);
      if( pOrderBy ){
        /* At first glance you would think we could optimize out the
        ** ORDER BY in this case since the order of entries in the set
        ** does not matter.  But there might be a LIMIT clause, in which
        ** case the order does matter */
        pushOntoSorter(pParse, pOrderBy, p);
      }else{
        sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &p->affinity, 1);
        sqlite3VdbeAddOp(v, OP_IdxInsert, (iParm&0x0000FFFF), 0);
      }
      sqlite3VdbeJumpHere(v, addr2);
      break;
    }

    /* If any row exist in the result set, record that fact and abort.
    */
    case SRT_Exists: {
      sqlite3VdbeAddOp(v, OP_MemInt, 1, iParm);
      sqlite3VdbeAddOp(v, OP_Pop, nColumn, 0);
      /* The LIMIT clause will terminate the loop for us */
      break;
    }

    /* If this is a scalar select that is part of an expression, then
    ** store the results in the appropriate memory cell and break out
    ** of the scan loop.
    */
    case SRT_Mem: {
      assert( nColumn==1 );
      if( pOrderBy ){
        pushOntoSorter(pParse, pOrderBy, p);
      }else{
        sqlite3VdbeAddOp(v, OP_MemStore, iParm, 1);
        /* The LIMIT clause will jump out of the loop for us */
      }
      break;
    }
#endif /* #ifndef SQLITE_OMIT_SUBQUERY */

    /* Send the data to the callback function or to a subroutine.  In the
    ** case of a subroutine, the subroutine itself is responsible for
    ** popping the data from the stack.
    */
    case SRT_Subroutine:
    case SRT_Callback: {
      if( pOrderBy ){
        sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
        pushOntoSorter(pParse, pOrderBy, p);
      }else if( eDest==SRT_Subroutine ){
        sqlite3VdbeAddOp(v, OP_Gosub, 0, iParm);
      }else{
        sqlite3VdbeAddOp(v, OP_Callback, nColumn, 0);
      }
      break;
    }

#if !defined(SQLITE_OMIT_TRIGGER)
    /* Discard the results.  This is used for SELECT statements inside
    ** the body of a TRIGGER.  The purpose of such selects is to call
    ** user-defined functions that have side effects.  We do not care
    ** about the actual results of the select.
    */
    default: {
      assert( eDest==SRT_Discard );
      sqlite3VdbeAddOp(v, OP_Pop, nColumn, 0);
      break;
    }
#endif
  }

  /* Jump to the end of the loop if the LIMIT is reached.
  */
  if( p->iLimit>=0 && pOrderBy==0 ){
    sqlite3VdbeAddOp(v, OP_MemIncr, -1, p->iLimit);
    sqlite3VdbeAddOp(v, OP_IfMemZero, p->iLimit, iBreak);
  }
  return 0;
}

/*
** Given an expression list, generate a KeyInfo structure that records
** the collating sequence for each expression in that expression list.
**
** If the ExprList is an ORDER BY or GROUP BY clause then the resulting
** KeyInfo structure is appropriate for initializing a virtual index to
** implement that clause.  If the ExprList is the result set of a SELECT
** then the KeyInfo structure is appropriate for initializing a virtual
** index to implement a DISTINCT test.
**
** Space to hold the KeyInfo structure is obtain from malloc.  The calling
** function is responsible for seeing that this structure is eventually
** freed.  Add the KeyInfo structure to the P3 field of an opcode using
** P3_KEYINFO_HANDOFF is the usual way of dealing with this.
*/
static KeyInfo *keyInfoFromExprList(Parse *pParse, ExprList *pList){
  sqlite3 *db = pParse->db;
  int nExpr;
  KeyInfo *pInfo;
  struct ExprList_item *pItem;
  int i;

  nExpr = pList->nExpr;
  pInfo = sqliteMalloc( sizeof(*pInfo) + nExpr*(sizeof(CollSeq*)+1) );
  if( pInfo ){
    pInfo->aSortOrder = (u8*)&pInfo->aColl[nExpr];
    pInfo->nField = nExpr;
    pInfo->enc = ENC(db);
    for(i=0, pItem=pList->a; i<nExpr; i++, pItem++){
      CollSeq *pColl;
      pColl = sqlite3ExprCollSeq(pParse, pItem->pExpr);
      if( !pColl ){
        pColl = db->pDfltColl;
      }
      pInfo->aColl[i] = pColl;
      pInfo->aSortOrder[i] = pItem->sortOrder;
    }
  }
  return pInfo;
}


/*
** If the inner loop was generated using a non-null pOrderBy argument,
** then the results were placed in a sorter.  After the loop is terminated
** we need to run the sorter and output the results.  The following
** routine generates the code needed to do that.
*/
static void generateSortTail(
  Parse *pParse,   /* Parsing context */
  Select *p,       /* The SELECT statement */
  Vdbe *v,         /* Generate code into this VDBE */
  int nColumn,     /* Number of columns of data */
  int eDest,       /* Write the sorted results here */
  int iParm        /* Optional parameter associated with eDest */
){
  int brk = sqlite3VdbeMakeLabel(v);
  int cont = sqlite3VdbeMakeLabel(v);
  int addr;
  int iTab;
  int pseudoTab = 0;
  ExprList *pOrderBy = p->pOrderBy;

  iTab = pOrderBy->iECursor;
  if( eDest==SRT_Callback || eDest==SRT_Subroutine ){
    pseudoTab = pParse->nTab++;
    sqlite3VdbeAddOp(v, OP_OpenPseudo, pseudoTab, 0);
    sqlite3VdbeAddOp(v, OP_SetNumColumns, pseudoTab, nColumn);
  }
  addr = 1 + sqlite3VdbeAddOp(v, OP_Sort, iTab, brk);
  codeOffset(v, p, cont, 0);
  if( eDest==SRT_Callback || eDest==SRT_Subroutine ){
    sqlite3VdbeAddOp(v, OP_Integer, 1, 0);
  }
  sqlite3VdbeAddOp(v, OP_Column, iTab, pOrderBy->nExpr + 1);
  switch( eDest ){
    case SRT_Table:
    case SRT_EphemTab: {
      sqlite3VdbeAddOp(v, OP_NewRowid, iParm, 0);
      sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
      sqlite3VdbeAddOp(v, OP_Insert, iParm, OPFLAG_APPEND);
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case SRT_Set: {
      assert( nColumn==1 );
      sqlite3VdbeAddOp(v, OP_NotNull, -1, sqlite3VdbeCurrentAddr(v)+3);
      sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      sqlite3VdbeAddOp(v, OP_Goto, 0, sqlite3VdbeCurrentAddr(v)+3);
      sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &p->affinity, 1);
      sqlite3VdbeAddOp(v, OP_IdxInsert, (iParm&0x0000FFFF), 0);
      break;
    }
    case SRT_Mem: {
      assert( nColumn==1 );
      sqlite3VdbeAddOp(v, OP_MemStore, iParm, 1);
      /* The LIMIT clause will terminate the loop for us */
      break;
    }
#endif
    case SRT_Callback:
    case SRT_Subroutine: {
      int i;
      sqlite3VdbeAddOp(v, OP_Insert, pseudoTab, 0);
      for(i=0; i<nColumn; i++){
        sqlite3VdbeAddOp(v, OP_Column, pseudoTab, i);
      }
      if( eDest==SRT_Callback ){
        sqlite3VdbeAddOp(v, OP_Callback, nColumn, 0);
      }else{
        sqlite3VdbeAddOp(v, OP_Gosub, 0, iParm);
      }
      break;
    }
    default: {
      /* Do nothing */
      break;
    }
  }

  /* Jump to the end of the loop when the LIMIT is reached
  */
  if( p->iLimit>=0 ){
    sqlite3VdbeAddOp(v, OP_MemIncr, -1, p->iLimit);
    sqlite3VdbeAddOp(v, OP_IfMemZero, p->iLimit, brk);
  }

  /* The bottom of the loop
  */
  sqlite3VdbeResolveLabel(v, cont);
  sqlite3VdbeAddOp(v, OP_Next, iTab, addr);
  sqlite3VdbeResolveLabel(v, brk);
  if( eDest==SRT_Callback || eDest==SRT_Subroutine ){
    sqlite3VdbeAddOp(v, OP_Close, pseudoTab, 0);
  }

}

/*
** Return a pointer to a string containing the 'declaration type' of the
** expression pExpr. The string may be treated as static by the caller.
**
** The declaration type is the exact datatype definition extracted from the
** original CREATE TABLE statement if the expression is a column. The
** declaration type for a ROWID field is INTEGER. Exactly when an expression
** is considered a column can be complex in the presence of subqueries. The
** result-set expression in all of the following SELECT statements is 
** considered a column by this function.
**
**   SELECT col FROM tbl;
**   SELECT (SELECT col FROM tbl;
**   SELECT (SELECT col FROM tbl);
**   SELECT abc FROM (SELECT col AS abc FROM tbl);
** 
** The declaration type for any expression other than a column is NULL.
*/
static const char *columnType(
  NameContext *pNC, 
  Expr *pExpr,
  const char **pzOriginDb,
  const char **pzOriginTab,
  const char **pzOriginCol
){
  char const *zType = 0;
  char const *zOriginDb = 0;
  char const *zOriginTab = 0;
  char const *zOriginCol = 0;
  int j;
  if( pExpr==0 || pNC->pSrcList==0 ) return 0;

  /* The TK_AS operator can only occur in ORDER BY, GROUP BY, HAVING,
  ** and LIMIT clauses.  But pExpr originates in the result set of a
  ** SELECT.  So pExpr can never contain an AS operator.
  */
  assert( pExpr->op!=TK_AS );

  switch( pExpr->op ){
    case TK_AGG_COLUMN:
    case TK_COLUMN: {
      /* The expression is a column. Locate the table the column is being
      ** extracted from in NameContext.pSrcList. This table may be real
      ** database table or a subquery.
      */
      Table *pTab = 0;            /* Table structure column is extracted from */
      Select *pS = 0;             /* Select the column is extracted from */
      int iCol = pExpr->iColumn;  /* Index of column in pTab */
      while( pNC && !pTab ){
        SrcList *pTabList = pNC->pSrcList;
        for(j=0;j<pTabList->nSrc && pTabList->a[j].iCursor!=pExpr->iTable;j++);
        if( j<pTabList->nSrc ){
          pTab = pTabList->a[j].pTab;
          pS = pTabList->a[j].pSelect;
        }else{
          pNC = pNC->pNext;
        }
      }

      if( pTab==0 ){
        /* FIX ME:
        ** This can occurs if you have something like "SELECT new.x;" inside
        ** a trigger.  In other words, if you reference the special "new"
        ** table in the result set of a select.  We do not have a good way
        ** to find the actual table type, so call it "TEXT".  This is really
        ** something of a bug, but I do not know how to fix it.
        **
        ** This code does not produce the correct answer - it just prevents
        ** a segfault.  See ticket #1229.
        */
        zType = "TEXT";
        break;
      }

      assert( pTab );
      if( pS ){
        /* The "table" is actually a sub-select or a view in the FROM clause
        ** of the SELECT statement. Return the declaration type and origin
        ** data for the result-set column of the sub-select.
        */
        if( iCol>=0 && iCol<pS->pEList->nExpr ){
          /* If iCol is less than zero, then the expression requests the
          ** rowid of the sub-select or view. This expression is legal (see 
          ** test case misc2.2.2) - it always evaluates to NULL.
          */
          NameContext sNC;
          Expr *p = pS->pEList->a[iCol].pExpr;
          sNC.pSrcList = pS->pSrc;
          sNC.pNext = 0;
          sNC.pParse = pNC->pParse;
          zType = columnType(&sNC, p, &zOriginDb, &zOriginTab, &zOriginCol); 
        }
      }else if( pTab->pSchema ){
        /* A real table */
        assert( !pS );
        if( iCol<0 ) iCol = pTab->iPKey;
        assert( iCol==-1 || (iCol>=0 && iCol<pTab->nCol) );
        if( iCol<0 ){
          zType = "INTEGER";
          zOriginCol = "rowid";
        }else{
          zType = pTab->aCol[iCol].zType;
          zOriginCol = pTab->aCol[iCol].zName;
        }
        zOriginTab = pTab->zName;
        if( pNC->pParse ){
          int iDb = sqlite3SchemaToIndex(pNC->pParse->db, pTab->pSchema);
          zOriginDb = pNC->pParse->db->aDb[iDb].zName;
        }
      }
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_SELECT: {
      /* The expression is a sub-select. Return the declaration type and
      ** origin info for the single column in the result set of the SELECT
      ** statement.
      */
      NameContext sNC;
      Select *pS = pExpr->pSelect;
      Expr *p = pS->pEList->a[0].pExpr;
      sNC.pSrcList = pS->pSrc;
      sNC.pNext = pNC;
      sNC.pParse = pNC->pParse;
      zType = columnType(&sNC, p, &zOriginDb, &zOriginTab, &zOriginCol); 
      break;
    }
#endif
  }
  
  if( pzOriginDb ){
    assert( pzOriginTab && pzOriginCol );
    *pzOriginDb = zOriginDb;
    *pzOriginTab = zOriginTab;
    *pzOriginCol = zOriginCol;
  }
  return zType;
}

/*
** Generate code that will tell the VDBE the declaration types of columns
** in the result set.
*/
static void generateColumnTypes(
  Parse *pParse,      /* Parser context */
  SrcList *pTabList,  /* List of tables */
  ExprList *pEList    /* Expressions defining the result set */
){
  Vdbe *v = pParse->pVdbe;
  int i;
  NameContext sNC;
  sNC.pSrcList = pTabList;
  sNC.pParse = pParse;
  for(i=0; i<pEList->nExpr; i++){
    Expr *p = pEList->a[i].pExpr;
    const char *zOrigDb = 0;
    const char *zOrigTab = 0;
    const char *zOrigCol = 0;
    const char *zType = columnType(&sNC, p, &zOrigDb, &zOrigTab, &zOrigCol);

    /* The vdbe must make it's own copy of the column-type and other 
    ** column specific strings, in case the schema is reset before this
    ** virtual machine is deleted.
    */
    sqlite3VdbeSetColName(v, i, COLNAME_DECLTYPE, zType, P3_TRANSIENT);
    sqlite3VdbeSetColName(v, i, COLNAME_DATABASE, zOrigDb, P3_TRANSIENT);
    sqlite3VdbeSetColName(v, i, COLNAME_TABLE, zOrigTab, P3_TRANSIENT);
    sqlite3VdbeSetColName(v, i, COLNAME_COLUMN, zOrigCol, P3_TRANSIENT);
  }
}

/*
** Generate code that will tell the VDBE the names of columns
** in the result set.  This information is used to provide the
** azCol[] values in the callback.
*/
static void generateColumnNames(
  Parse *pParse,      /* Parser context */
  SrcList *pTabList,  /* List of tables */
  ExprList *pEList    /* Expressions defining the result set */
){
  Vdbe *v = pParse->pVdbe;
  int i, j;
  sqlite3 *db = pParse->db;
  int fullNames, shortNames;

#ifndef SQLITE_OMIT_EXPLAIN
  /* If this is an EXPLAIN, skip this step */
  if( pParse->explain ){
    return;
  }
#endif

  assert( v!=0 );
  if( pParse->colNamesSet || v==0 || sqlite3MallocFailed() ) return;
  pParse->colNamesSet = 1;
  fullNames = (db->flags & SQLITE_FullColNames)!=0;
  shortNames = (db->flags & SQLITE_ShortColNames)!=0;
  sqlite3VdbeSetNumCols(v, pEList->nExpr);
  for(i=0; i<pEList->nExpr; i++){
    Expr *p;
    p = pEList->a[i].pExpr;
    if( p==0 ) continue;
    if( pEList->a[i].zName ){
      char *zName = pEList->a[i].zName;
      sqlite3VdbeSetColName(v, i, COLNAME_NAME, zName, strlen(zName));
      continue;
    }
    if( p->op==TK_COLUMN && pTabList ){
      Table *pTab;
      char *zCol;
      int iCol = p->iColumn;
      for(j=0; j<pTabList->nSrc && pTabList->a[j].iCursor!=p->iTable; j++){}
      assert( j<pTabList->nSrc );
      pTab = pTabList->a[j].pTab;
      if( iCol<0 ) iCol = pTab->iPKey;
      assert( iCol==-1 || (iCol>=0 && iCol<pTab->nCol) );
      if( iCol<0 ){
        zCol = "rowid";
      }else{
        zCol = pTab->aCol[iCol].zName;
      }
      if( !shortNames && !fullNames && p->span.z && p->span.z[0] ){
        sqlite3VdbeSetColName(v, i, COLNAME_NAME, (char*)p->span.z, p->span.n);
      }else if( fullNames || (!shortNames && pTabList->nSrc>1) ){
        char *zName = 0;
        char *zTab;
 
        zTab = pTabList->a[j].zAlias;
        if( fullNames || zTab==0 ) zTab = pTab->zName;
        sqlite3SetString(&zName, zTab, ".", zCol, (char*)0);
        sqlite3VdbeSetColName(v, i, COLNAME_NAME, zName, P3_DYNAMIC);
      }else{
        sqlite3VdbeSetColName(v, i, COLNAME_NAME, zCol, strlen(zCol));
      }
    }else if( p->span.z && p->span.z[0] ){
      sqlite3VdbeSetColName(v, i, COLNAME_NAME, (char*)p->span.z, p->span.n);
      /* sqlite3VdbeCompressSpace(v, addr); */
    }else{
      char zName[30];
      assert( p->op!=TK_COLUMN || pTabList==0 );
      sprintf(zName, "column%d", i+1);
      sqlite3VdbeSetColName(v, i, COLNAME_NAME, zName, 0);
    }
  }
  generateColumnTypes(pParse, pTabList, pEList);
}

#ifndef SQLITE_OMIT_COMPOUND_SELECT
/*
** Name of the connection operator, used for error messages.
*/
static const char *selectOpName(int id){
  char *z;
  switch( id ){
    case TK_ALL:       z = "UNION ALL";   break;
    case TK_INTERSECT: z = "INTERSECT";   break;
    case TK_EXCEPT:    z = "EXCEPT";      break;
    default:           z = "UNION";       break;
  }
  return z;
}
#endif /* SQLITE_OMIT_COMPOUND_SELECT */

/*
** Forward declaration
*/
static int prepSelectStmt(Parse*, Select*);

/*
** Given a SELECT statement, generate a Table structure that describes
** the result set of that SELECT.
*/
Table *sqlite3ResultSetOfSelect(Parse *pParse, char *zTabName, Select *pSelect){
  Table *pTab;
  int i, j;
  ExprList *pEList;
  Column *aCol, *pCol;

  while( pSelect->pPrior ) pSelect = pSelect->pPrior;
  if( prepSelectStmt(pParse, pSelect) ){
    return 0;
  }
  if( sqlite3SelectResolve(pParse, pSelect, 0) ){
    return 0;
  }
  pTab = sqliteMalloc( sizeof(Table) );
  if( pTab==0 ){
    return 0;
  }
  pTab->nRef = 1;
  pTab->zName = zTabName ? sqliteStrDup(zTabName) : 0;
  pEList = pSelect->pEList;
  pTab->nCol = pEList->nExpr;
  assert( pTab->nCol>0 );
  pTab->aCol = aCol = sqliteMalloc( sizeof(pTab->aCol[0])*pTab->nCol );
  for(i=0, pCol=aCol; i<pTab->nCol; i++, pCol++){
    Expr *p, *pR;
    char *zType;
    char *zName;
    int nName;
    CollSeq *pColl;
    int cnt;
    NameContext sNC;
    
    /* Get an appropriate name for the column
    */
    p = pEList->a[i].pExpr;
    assert( p->pRight==0 || p->pRight->token.z==0 || p->pRight->token.z[0]!=0 );
    if( (zName = pEList->a[i].zName)!=0 ){
      /* If the column contains an "AS <name>" phrase, use <name> as the name */
      zName = sqliteStrDup(zName);
    }else if( p->op==TK_DOT 
              && (pR=p->pRight)!=0 && pR->token.z && pR->token.z[0] ){
      /* For columns of the from A.B use B as the name */
      zName = sqlite3MPrintf("%T", &pR->token);
    }else if( p->span.z && p->span.z[0] ){
      /* Use the original text of the column expression as its name */
      zName = sqlite3MPrintf("%T", &p->span);
    }else{
      /* If all else fails, make up a name */
      zName = sqlite3MPrintf("column%d", i+1);
    }
    sqlite3Dequote(zName);
    if( sqlite3MallocFailed() ){
      sqliteFree(zName);
      sqlite3DeleteTable(pTab);
      return 0;
    }

    /* Make sure the column name is unique.  If the name is not unique,
    ** append a integer to the name so that it becomes unique.
    */
    nName = strlen(zName);
    for(j=cnt=0; j<i; j++){
      if( sqlite3StrICmp(aCol[j].zName, zName)==0 ){
        zName[nName] = 0;
        zName = sqlite3MPrintf("%z:%d", zName, ++cnt);
        j = -1;
        if( zName==0 ) break;
      }
    }
    pCol->zName = zName;

    /* Get the typename, type affinity, and collating sequence for the
    ** column.
    */
    memset(&sNC, 0, sizeof(sNC));
    sNC.pSrcList = pSelect->pSrc;
    zType = sqliteStrDup(columnType(&sNC, p, 0, 0, 0));
    pCol->zType = zType;
    pCol->affinity = sqlite3ExprAffinity(p);
    pColl = sqlite3ExprCollSeq(pParse, p);
    if( pColl ){
      pCol->zColl = sqliteStrDup(pColl->zName);
    }
  }
  pTab->iPKey = -1;
  return pTab;
}

/*
** Prepare a SELECT statement for processing by doing the following
** things:
**
**    (1)  Make sure VDBE cursor numbers have been assigned to every
**         element of the FROM clause.
**
**    (2)  Fill in the pTabList->a[].pTab fields in the SrcList that 
**         defines FROM clause.  When views appear in the FROM clause,
**         fill pTabList->a[].pSelect with a copy of the SELECT statement
**         that implements the view.  A copy is made of the view's SELECT
**         statement so that we can freely modify or delete that statement
**         without worrying about messing up the presistent representation
**         of the view.
**
**    (3)  Add terms to the WHERE clause to accomodate the NATURAL keyword
**         on joins and the ON and USING clause of joins.
**
**    (4)  Scan the list of columns in the result set (pEList) looking
**         for instances of the "*" operator or the TABLE.* operator.
**         If found, expand each "*" to be every column in every table
**         and TABLE.* to be every column in TABLE.
**
** Return 0 on success.  If there are problems, leave an error message
** in pParse and return non-zero.
*/
static int prepSelectStmt(Parse *pParse, Select *p){
  int i, j, k, rc;
  SrcList *pTabList;
  ExprList *pEList;
  struct SrcList_item *pFrom;

  if( p==0 || p->pSrc==0 || sqlite3MallocFailed() ){
    return 1;
  }
  pTabList = p->pSrc;
  pEList = p->pEList;

  /* Make sure cursor numbers have been assigned to all entries in
  ** the FROM clause of the SELECT statement.
  */
  sqlite3SrcListAssignCursors(pParse, p->pSrc);

  /* Look up every table named in the FROM clause of the select.  If
  ** an entry of the FROM clause is a subquery instead of a table or view,
  ** then create a transient table structure to describe the subquery.
  */
  for(i=0, pFrom=pTabList->a; i<pTabList->nSrc; i++, pFrom++){
    Table *pTab;
    if( pFrom->pTab!=0 ){
      /* This statement has already been prepared.  There is no need
      ** to go further. */
      assert( i==0 );
      return 0;
    }
    if( pFrom->zName==0 ){
#ifndef SQLITE_OMIT_SUBQUERY
      /* A sub-query in the FROM clause of a SELECT */
      assert( pFrom->pSelect!=0 );
      if( pFrom->zAlias==0 ){
        pFrom->zAlias =
          sqlite3MPrintf("sqlite_subquery_%p_", (void*)pFrom->pSelect);
      }
      assert( pFrom->pTab==0 );
      pFrom->pTab = pTab = 
        sqlite3ResultSetOfSelect(pParse, pFrom->zAlias, pFrom->pSelect);
      if( pTab==0 ){
        return 1;
      }
      /* The isEphem flag indicates that the Table structure has been
      ** dynamically allocated and may be freed at any time.  In other words,
      ** pTab is not pointing to a persistent table structure that defines
      ** part of the schema. */
      pTab->isEphem = 1;
#endif
    }else{
      /* An ordinary table or view name in the FROM clause */
      assert( pFrom->pTab==0 );
      pFrom->pTab = pTab = 
        sqlite3LocateTable(pParse,pFrom->zName,pFrom->zDatabase);
      if( pTab==0 ){
        return 1;
      }
      pTab->nRef++;
#if !defined(SQLITE_OMIT_VIEW) || !defined (SQLITE_OMIT_VIRTUALTABLE)
      if( pTab->pSelect || IsVirtual(pTab) ){
        /* We reach here if the named table is a really a view */
        if( sqlite3ViewGetColumnNames(pParse, pTab) ){
          return 1;
        }
        /* If pFrom->pSelect!=0 it means we are dealing with a
        ** view within a view.  The SELECT structure has already been
        ** copied by the outer view so we can skip the copy step here
        ** in the inner view.
        */
        if( pFrom->pSelect==0 ){
          pFrom->pSelect = sqlite3SelectDup(pTab->pSelect);
        }
      }
#endif
    }
  }

  /* Process NATURAL keywords, and ON and USING clauses of joins.
  */
  if( sqliteProcessJoin(pParse, p) ) return 1;

  /* For every "*" that occurs in the column list, insert the names of
  ** all columns in all tables.  And for every TABLE.* insert the names
  ** of all columns in TABLE.  The parser inserted a special expression
  ** with the TK_ALL operator for each "*" that it found in the column list.
  ** The following code just has to locate the TK_ALL expressions and expand
  ** each one to the list of all columns in all tables.
  **
  ** The first loop just checks to see if there are any "*" operators
  ** that need expanding.
  */
  for(k=0; k<pEList->nExpr; k++){
    Expr *pE = pEList->a[k].pExpr;
    if( pE->op==TK_ALL ) break;
    if( pE->op==TK_DOT && pE->pRight && pE->pRight->op==TK_ALL
         && pE->pLeft && pE->pLeft->op==TK_ID ) break;
  }
  rc = 0;
  if( k<pEList->nExpr ){
    /*
    ** If we get here it means the result set contains one or more "*"
    ** operators that need to be expanded.  Loop through each expression
    ** in the result set and expand them one by one.
    */
    struct ExprList_item *a = pEList->a;
    ExprList *pNew = 0;
    int flags = pParse->db->flags;
    int longNames = (flags & SQLITE_FullColNames)!=0 &&
                      (flags & SQLITE_ShortColNames)==0;

    for(k=0; k<pEList->nExpr; k++){
      Expr *pE = a[k].pExpr;
      if( pE->op!=TK_ALL &&
           (pE->op!=TK_DOT || pE->pRight==0 || pE->pRight->op!=TK_ALL) ){
        /* This particular expression does not need to be expanded.
        */
        pNew = sqlite3ExprListAppend(pNew, a[k].pExpr, 0);
        if( pNew ){
          pNew->a[pNew->nExpr-1].zName = a[k].zName;
        }else{
          rc = 1;
        }
        a[k].pExpr = 0;
        a[k].zName = 0;
      }else{
        /* This expression is a "*" or a "TABLE.*" and needs to be
        ** expanded. */
        int tableSeen = 0;      /* Set to 1 when TABLE matches */
        char *zTName;            /* text of name of TABLE */
        if( pE->op==TK_DOT && pE->pLeft ){
          zTName = sqlite3NameFromToken(&pE->pLeft->token);
        }else{
          zTName = 0;
        }
        for(i=0, pFrom=pTabList->a; i<pTabList->nSrc; i++, pFrom++){
          Table *pTab = pFrom->pTab;
          char *zTabName = pFrom->zAlias;
          if( zTabName==0 || zTabName[0]==0 ){ 
            zTabName = pTab->zName;
          }
          if( zTName && (zTabName==0 || zTabName[0]==0 || 
                 sqlite3StrICmp(zTName, zTabName)!=0) ){
            continue;
          }
          tableSeen = 1;
          for(j=0; j<pTab->nCol; j++){
            Expr *pExpr, *pRight;
            char *zName = pTab->aCol[j].zName;

            if( i>0 ){
              struct SrcList_item *pLeft = &pTabList->a[i-1];
              if( (pLeft[1].jointype & JT_NATURAL)!=0 &&
                        columnIndex(pLeft->pTab, zName)>=0 ){
                /* In a NATURAL join, omit the join columns from the 
                ** table on the right */
                continue;
              }
              if( sqlite3IdListIndex(pLeft[1].pUsing, zName)>=0 ){
                /* In a join with a USING clause, omit columns in the
                ** using clause from the table on the right. */
                continue;
              }
            }
            pRight = sqlite3Expr(TK_ID, 0, 0, 0);
            if( pRight==0 ) break;
            setToken(&pRight->token, zName);
            if( zTabName && (longNames || pTabList->nSrc>1) ){
              Expr *pLeft = sqlite3Expr(TK_ID, 0, 0, 0);
              pExpr = sqlite3Expr(TK_DOT, pLeft, pRight, 0);
              if( pExpr==0 ) break;
              setToken(&pLeft->token, zTabName);
              setToken(&pExpr->span, sqlite3MPrintf("%s.%s", zTabName, zName));
              pExpr->span.dyn = 1;
              pExpr->token.z = 0;
              pExpr->token.n = 0;
              pExpr->token.dyn = 0;
            }else{
              pExpr = pRight;
              pExpr->span = pExpr->token;
            }
            if( longNames ){
              pNew = sqlite3ExprListAppend(pNew, pExpr, &pExpr->span);
            }else{
              pNew = sqlite3ExprListAppend(pNew, pExpr, &pRight->token);
            }
          }
        }
        if( !tableSeen ){
          if( zTName ){
            sqlite3ErrorMsg(pParse, "no such table: %s", zTName);
          }else{
            sqlite3ErrorMsg(pParse, "no tables specified");
          }
          rc = 1;
        }
        sqliteFree(zTName);
      }
    }
    sqlite3ExprListDelete(pEList);
    p->pEList = pNew;
  }
  return rc;
}

#ifndef SQLITE_OMIT_COMPOUND_SELECT
/*
** This routine associates entries in an ORDER BY expression list with
** columns in a result.  For each ORDER BY expression, the opcode of
** the top-level node is changed to TK_COLUMN and the iColumn value of
** the top-level node is filled in with column number and the iTable
** value of the top-level node is filled with iTable parameter.
**
** If there are prior SELECT clauses, they are processed first.  A match
** in an earlier SELECT takes precedence over a later SELECT.
**
** Any entry that does not match is flagged as an error.  The number
** of errors is returned.
*/
static int matchOrderbyToColumn(
  Parse *pParse,          /* A place to leave error messages */
  Select *pSelect,        /* Match to result columns of this SELECT */
  ExprList *pOrderBy,     /* The ORDER BY values to match against columns */
  int iTable,             /* Insert this value in iTable */
  int mustComplete        /* If TRUE all ORDER BYs must match */
){
  int nErr = 0;
  int i, j;
  ExprList *pEList;

  if( pSelect==0 || pOrderBy==0 ) return 1;
  if( mustComplete ){
    for(i=0; i<pOrderBy->nExpr; i++){ pOrderBy->a[i].done = 0; }
  }
  if( prepSelectStmt(pParse, pSelect) ){
    return 1;
  }
  if( pSelect->pPrior ){
    if( matchOrderbyToColumn(pParse, pSelect->pPrior, pOrderBy, iTable, 0) ){
      return 1;
    }
  }
  pEList = pSelect->pEList;
  for(i=0; i<pOrderBy->nExpr; i++){
    struct ExprList_item *pItem;
    Expr *pE = pOrderBy->a[i].pExpr;
    int iCol = -1;
    char *zLabel;

    if( pOrderBy->a[i].done ) continue;
    if( sqlite3ExprIsInteger(pE, &iCol) ){
      if( iCol<=0 || iCol>pEList->nExpr ){
        sqlite3ErrorMsg(pParse,
          "ORDER BY position %d should be between 1 and %d",
          iCol, pEList->nExpr);
        nErr++;
        break;
      }
      if( !mustComplete ) continue;
      iCol--;
    }
    if( iCol<0 && (zLabel = sqlite3NameFromToken(&pE->token))!=0 ){
      for(j=0, pItem=pEList->a; j<pEList->nExpr; j++, pItem++){
        char *zName;
        int isMatch;
        if( pItem->zName ){
          zName = sqlite3StrDup(pItem->zName);
        }else{
          zName = sqlite3NameFromToken(&pItem->pExpr->token);
        }
        isMatch = zName && sqlite3StrICmp(zName, zLabel)==0;
        sqliteFree(zName);
        if( isMatch ){
          iCol = j;
          break;
        }
      }
      sqliteFree(zLabel);
    }
    if( iCol>=0 ){
      pE->op = TK_COLUMN;
      pE->iColumn = iCol;
      pE->iTable = iTable;
      pE->iAgg = -1;
      pOrderBy->a[i].done = 1;
    }else if( mustComplete ){
      sqlite3ErrorMsg(pParse,
        "ORDER BY term number %d does not match any result column", i+1);
      nErr++;
      break;
    }
  }
  return nErr;  
}
#endif /* #ifndef SQLITE_OMIT_COMPOUND_SELECT */

/*
** Get a VDBE for the given parser context.  Create a new one if necessary.
** If an error occurs, return NULL and leave a message in pParse.
*/
Vdbe *sqlite3GetVdbe(Parse *pParse){
  Vdbe *v = pParse->pVdbe;
  if( v==0 ){
    v = pParse->pVdbe = sqlite3VdbeCreate(pParse->db);
  }
  return v;
}


/*
** Compute the iLimit and iOffset fields of the SELECT based on the
** pLimit and pOffset expressions.  pLimit and pOffset hold the expressions
** that appear in the original SQL statement after the LIMIT and OFFSET
** keywords.  Or NULL if those keywords are omitted. iLimit and iOffset 
** are the integer memory register numbers for counters used to compute 
** the limit and offset.  If there is no limit and/or offset, then 
** iLimit and iOffset are negative.
**
** This routine changes the values of iLimit and iOffset only if
** a limit or offset is defined by pLimit and pOffset.  iLimit and
** iOffset should have been preset to appropriate default values
** (usually but not always -1) prior to calling this routine.
** Only if pLimit!=0 or pOffset!=0 do the limit registers get
** redefined.  The UNION ALL operator uses this property to force
** the reuse of the same limit and offset registers across multiple
** SELECT statements.
*/
static void computeLimitRegisters(Parse *pParse, Select *p, int iBreak){
  Vdbe *v = 0;
  int iLimit = 0;
  int iOffset;
  int addr1, addr2;

  /* 
  ** "LIMIT -1" always shows all rows.  There is some
  ** contraversy about what the correct behavior should be.
  ** The current implementation interprets "LIMIT 0" to mean
  ** no rows.
  */
  if( p->pLimit ){
    p->iLimit = iLimit = pParse->nMem;
    pParse->nMem += 2;
    v = sqlite3GetVdbe(pParse);
    if( v==0 ) return;
    sqlite3ExprCode(pParse, p->pLimit);
    sqlite3VdbeAddOp(v, OP_MustBeInt, 0, 0);
    sqlite3VdbeAddOp(v, OP_MemStore, iLimit, 0);
    VdbeComment((v, "# LIMIT counter"));
    sqlite3VdbeAddOp(v, OP_IfMemZero, iLimit, iBreak);
  }
  if( p->pOffset ){
    p->iOffset = iOffset = pParse->nMem++;
    v = sqlite3GetVdbe(pParse);
    if( v==0 ) return;
    sqlite3ExprCode(pParse, p->pOffset);
    sqlite3VdbeAddOp(v, OP_MustBeInt, 0, 0);
    sqlite3VdbeAddOp(v, OP_MemStore, iOffset, p->pLimit==0);
    VdbeComment((v, "# OFFSET counter"));
    addr1 = sqlite3VdbeAddOp(v, OP_IfMemPos, iOffset, 0);
    sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
    sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
    sqlite3VdbeJumpHere(v, addr1);
    if( p->pLimit ){
      sqlite3VdbeAddOp(v, OP_Add, 0, 0);
    }
  }
  if( p->pLimit ){
    addr1 = sqlite3VdbeAddOp(v, OP_IfMemPos, iLimit, 0);
    sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
    sqlite3VdbeAddOp(v, OP_MemInt, -1, iLimit+1);
    addr2 = sqlite3VdbeAddOp(v, OP_Goto, 0, 0);
    sqlite3VdbeJumpHere(v, addr1);
    sqlite3VdbeAddOp(v, OP_MemStore, iLimit+1, 1);
    VdbeComment((v, "# LIMIT+OFFSET"));
    sqlite3VdbeJumpHere(v, addr2);
  }
}

/*
** Allocate a virtual index to use for sorting.
*/
static void createSortingIndex(Parse *pParse, Select *p, ExprList *pOrderBy){
  if( pOrderBy ){
    int addr;
    assert( pOrderBy->iECursor==0 );
    pOrderBy->iECursor = pParse->nTab++;
    addr = sqlite3VdbeAddOp(pParse->pVdbe, OP_OpenEphemeral,
                            pOrderBy->iECursor, pOrderBy->nExpr+1);
    assert( p->addrOpenEphm[2] == -1 );
    p->addrOpenEphm[2] = addr;
  }
}

#ifndef SQLITE_OMIT_COMPOUND_SELECT
/*
** Return the appropriate collating sequence for the iCol-th column of
** the result set for the compound-select statement "p".  Return NULL if
** the column has no default collating sequence.
**
** The collating sequence for the compound select is taken from the
** left-most term of the select that has a collating sequence.
*/
static CollSeq *multiSelectCollSeq(Parse *pParse, Select *p, int iCol){
  CollSeq *pRet;
  if( p->pPrior ){
    pRet = multiSelectCollSeq(pParse, p->pPrior, iCol);
  }else{
    pRet = 0;
  }
  if( pRet==0 ){
    pRet = sqlite3ExprCollSeq(pParse, p->pEList->a[iCol].pExpr);
  }
  return pRet;
}
#endif /* SQLITE_OMIT_COMPOUND_SELECT */

#ifndef SQLITE_OMIT_COMPOUND_SELECT
/*
** This routine is called to process a query that is really the union
** or intersection of two or more separate queries.
**
** "p" points to the right-most of the two queries.  the query on the
** left is p->pPrior.  The left query could also be a compound query
** in which case this routine will be called recursively. 
**
** The results of the total query are to be written into a destination
** of type eDest with parameter iParm.
**
** Example 1:  Consider a three-way compound SQL statement.
**
**     SELECT a FROM t1 UNION SELECT b FROM t2 UNION SELECT c FROM t3
**
** This statement is parsed up as follows:
**
**     SELECT c FROM t3
**      |
**      `----->  SELECT b FROM t2
**                |
**                `------>  SELECT a FROM t1
**
** The arrows in the diagram above represent the Select.pPrior pointer.
** So if this routine is called with p equal to the t3 query, then
** pPrior will be the t2 query.  p->op will be TK_UNION in this case.
**
** Notice that because of the way SQLite parses compound SELECTs, the
** individual selects always group from left to right.
*/
static int multiSelect(
  Parse *pParse,        /* Parsing context */
  Select *p,            /* The right-most of SELECTs to be coded */
  int eDest,            /* \___  Store query results as specified */
  int iParm,            /* /     by these two parameters.         */
  char *aff             /* If eDest is SRT_Union, the affinity string */
){
  int rc = SQLITE_OK;   /* Success code from a subroutine */
  Select *pPrior;       /* Another SELECT immediately to our left */
  Vdbe *v;              /* Generate code to this VDBE */
  int nCol;             /* Number of columns in the result set */
  ExprList *pOrderBy;   /* The ORDER BY clause on p */
  int aSetP2[2];        /* Set P2 value of these op to number of columns */
  int nSetP2 = 0;       /* Number of slots in aSetP2[] used */

  /* Make sure there is no ORDER BY or LIMIT clause on prior SELECTs.  Only
  ** the last (right-most) SELECT in the series may have an ORDER BY or LIMIT.
  */
  if( p==0 || p->pPrior==0 ){
    rc = 1;
    goto multi_select_end;
  }
  pPrior = p->pPrior;
  assert( pPrior->pRightmost!=pPrior );
  assert( pPrior->pRightmost==p->pRightmost );
  if( pPrior->pOrderBy ){
    sqlite3ErrorMsg(pParse,"ORDER BY clause should come after %s not before",
      selectOpName(p->op));
    rc = 1;
    goto multi_select_end;
  }
  if( pPrior->pLimit ){
    sqlite3ErrorMsg(pParse,"LIMIT clause should come after %s not before",
      selectOpName(p->op));
    rc = 1;
    goto multi_select_end;
  }

  /* Make sure we have a valid query engine.  If not, create a new one.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ){
    rc = 1;
    goto multi_select_end;
  }

  /* Create the destination temporary table if necessary
  */
  if( eDest==SRT_EphemTab ){
    assert( p->pEList );
    assert( nSetP2<sizeof(aSetP2)/sizeof(aSetP2[0]) );
    aSetP2[nSetP2++] = sqlite3VdbeAddOp(v, OP_OpenEphemeral, iParm, 0);
    eDest = SRT_Table;
  }

  /* Generate code for the left and right SELECT statements.
  */
  pOrderBy = p->pOrderBy;
  switch( p->op ){
    case TK_ALL: {
      if( pOrderBy==0 ){
        int addr = 0;
        assert( !pPrior->pLimit );
        pPrior->pLimit = p->pLimit;
        pPrior->pOffset = p->pOffset;
        rc = sqlite3Select(pParse, pPrior, eDest, iParm, 0, 0, 0, aff);
        p->pLimit = 0;
        p->pOffset = 0;
        if( rc ){
          goto multi_select_end;
        }
        p->pPrior = 0;
        p->iLimit = pPrior->iLimit;
        p->iOffset = pPrior->iOffset;
        if( p->iLimit>=0 ){
          addr = sqlite3VdbeAddOp(v, OP_IfMemZero, p->iLimit, 0);
          VdbeComment((v, "# Jump ahead if LIMIT reached"));
        }
        rc = sqlite3Select(pParse, p, eDest, iParm, 0, 0, 0, aff);
        p->pPrior = pPrior;
        if( rc ){
          goto multi_select_end;
        }
        if( addr ){
          sqlite3VdbeJumpHere(v, addr);
        }
        break;
      }
      /* For UNION ALL ... ORDER BY fall through to the next case */
    }
    case TK_EXCEPT:
    case TK_UNION: {
      int unionTab;    /* Cursor number of the temporary table holding result */
      int op = 0;      /* One of the SRT_ operations to apply to self */
      int priorOp;     /* The SRT_ operation to apply to prior selects */
      Expr *pLimit, *pOffset; /* Saved values of p->nLimit and p->nOffset */
      int addr;

      priorOp = p->op==TK_ALL ? SRT_Table : SRT_Union;
      if( eDest==priorOp && pOrderBy==0 && !p->pLimit && !p->pOffset ){
        /* We can reuse a temporary table generated by a SELECT to our
        ** right.
        */
        unionTab = iParm;
      }else{
        /* We will need to create our own temporary table to hold the
        ** intermediate results.
        */
        unionTab = pParse->nTab++;
        if( pOrderBy && matchOrderbyToColumn(pParse, p, pOrderBy, unionTab,1) ){
          rc = 1;
          goto multi_select_end;
        }
        addr = sqlite3VdbeAddOp(v, OP_OpenEphemeral, unionTab, 0);
        if( priorOp==SRT_Table ){
          assert( nSetP2<sizeof(aSetP2)/sizeof(aSetP2[0]) );
          aSetP2[nSetP2++] = addr;
        }else{
          assert( p->addrOpenEphm[0] == -1 );
          p->addrOpenEphm[0] = addr;
          p->pRightmost->usesEphm = 1;
        }
        createSortingIndex(pParse, p, pOrderBy);
        assert( p->pEList );
      }

      /* Code the SELECT statements to our left
      */
      assert( !pPrior->pOrderBy );
      rc = sqlite3Select(pParse, pPrior, priorOp, unionTab, 0, 0, 0, aff);
      if( rc ){
        goto multi_select_end;
      }

      /* Code the current SELECT statement
      */
      switch( p->op ){
         case TK_EXCEPT:  op = SRT_Except;   break;
         case TK_UNION:   op = SRT_Union;    break;
         case TK_ALL:     op = SRT_Table;    break;
      }
      p->pPrior = 0;
      p->pOrderBy = 0;
      p->disallowOrderBy = pOrderBy!=0;
      pLimit = p->pLimit;
      p->pLimit = 0;
      pOffset = p->pOffset;
      p->pOffset = 0;
      rc = sqlite3Select(pParse, p, op, unionTab, 0, 0, 0, aff);
      p->pPrior = pPrior;
      p->pOrderBy = pOrderBy;
      sqlite3ExprDelete(p->pLimit);
      p->pLimit = pLimit;
      p->pOffset = pOffset;
      p->iLimit = -1;
      p->iOffset = -1;
      if( rc ){
        goto multi_select_end;
      }


      /* Convert the data in the temporary table into whatever form
      ** it is that we currently need.
      */      
      if( eDest!=priorOp || unionTab!=iParm ){
        int iCont, iBreak, iStart;
        assert( p->pEList );
        if( eDest==SRT_Callback ){
          Select *pFirst = p;
          while( pFirst->pPrior ) pFirst = pFirst->pPrior;
          generateColumnNames(pParse, 0, pFirst->pEList);
        }
        iBreak = sqlite3VdbeMakeLabel(v);
        iCont = sqlite3VdbeMakeLabel(v);
        computeLimitRegisters(pParse, p, iBreak);
        sqlite3VdbeAddOp(v, OP_Rewind, unionTab, iBreak);
        iStart = sqlite3VdbeCurrentAddr(v);
        rc = selectInnerLoop(pParse, p, p->pEList, unionTab, p->pEList->nExpr,
                             pOrderBy, -1, eDest, iParm, 
                             iCont, iBreak, 0);
        if( rc ){
          rc = 1;
          goto multi_select_end;
        }
        sqlite3VdbeResolveLabel(v, iCont);
        sqlite3VdbeAddOp(v, OP_Next, unionTab, iStart);
        sqlite3VdbeResolveLabel(v, iBreak);
        sqlite3VdbeAddOp(v, OP_Close, unionTab, 0);
      }
      break;
    }
    case TK_INTERSECT: {
      int tab1, tab2;
      int iCont, iBreak, iStart;
      Expr *pLimit, *pOffset;
      int addr;

      /* INTERSECT is different from the others since it requires
      ** two temporary tables.  Hence it has its own case.  Begin
      ** by allocating the tables we will need.
      */
      tab1 = pParse->nTab++;
      tab2 = pParse->nTab++;
      if( pOrderBy && matchOrderbyToColumn(pParse,p,pOrderBy,tab1,1) ){
        rc = 1;
        goto multi_select_end;
      }
      createSortingIndex(pParse, p, pOrderBy);

      addr = sqlite3VdbeAddOp(v, OP_OpenEphemeral, tab1, 0);
      assert( p->addrOpenEphm[0] == -1 );
      p->addrOpenEphm[0] = addr;
      p->pRightmost->usesEphm = 1;
      assert( p->pEList );

      /* Code the SELECTs to our left into temporary table "tab1".
      */
      rc = sqlite3Select(pParse, pPrior, SRT_Union, tab1, 0, 0, 0, aff);
      if( rc ){
        goto multi_select_end;
      }

      /* Code the current SELECT into temporary table "tab2"
      */
      addr = sqlite3VdbeAddOp(v, OP_OpenEphemeral, tab2, 0);
      assert( p->addrOpenEphm[1] == -1 );
      p->addrOpenEphm[1] = addr;
      p->pPrior = 0;
      pLimit = p->pLimit;
      p->pLimit = 0;
      pOffset = p->pOffset;
      p->pOffset = 0;
      rc = sqlite3Select(pParse, p, SRT_Union, tab2, 0, 0, 0, aff);
      p->pPrior = pPrior;
      sqlite3ExprDelete(p->pLimit);
      p->pLimit = pLimit;
      p->pOffset = pOffset;
      if( rc ){
        goto multi_select_end;
      }

      /* Generate code to take the intersection of the two temporary
      ** tables.
      */
      assert( p->pEList );
      if( eDest==SRT_Callback ){
        Select *pFirst = p;
        while( pFirst->pPrior ) pFirst = pFirst->pPrior;
        generateColumnNames(pParse, 0, pFirst->pEList);
      }
      iBreak = sqlite3VdbeMakeLabel(v);
      iCont = sqlite3VdbeMakeLabel(v);
      computeLimitRegisters(pParse, p, iBreak);
      sqlite3VdbeAddOp(v, OP_Rewind, tab1, iBreak);
      iStart = sqlite3VdbeAddOp(v, OP_RowKey, tab1, 0);
      sqlite3VdbeAddOp(v, OP_NotFound, tab2, iCont);
      rc = selectInnerLoop(pParse, p, p->pEList, tab1, p->pEList->nExpr,
                             pOrderBy, -1, eDest, iParm, 
                             iCont, iBreak, 0);
      if( rc ){
        rc = 1;
        goto multi_select_end;
      }
      sqlite3VdbeResolveLabel(v, iCont);
      sqlite3VdbeAddOp(v, OP_Next, tab1, iStart);
      sqlite3VdbeResolveLabel(v, iBreak);
      sqlite3VdbeAddOp(v, OP_Close, tab2, 0);
      sqlite3VdbeAddOp(v, OP_Close, tab1, 0);
      break;
    }
  }

  /* Make sure all SELECTs in the statement have the same number of elements
  ** in their result sets.
  */
  assert( p->pEList && pPrior->pEList );
  if( p->pEList->nExpr!=pPrior->pEList->nExpr ){
    sqlite3ErrorMsg(pParse, "SELECTs to the left and right of %s"
      " do not have the same number of result columns", selectOpName(p->op));
    rc = 1;
    goto multi_select_end;
  }

  /* Set the number of columns in temporary tables
  */
  nCol = p->pEList->nExpr;
  while( nSetP2 ){
    sqlite3VdbeChangeP2(v, aSetP2[--nSetP2], nCol);
  }

  /* Compute collating sequences used by either the ORDER BY clause or
  ** by any temporary tables needed to implement the compound select.
  ** Attach the KeyInfo structure to all temporary tables.  Invoke the
  ** ORDER BY processing if there is an ORDER BY clause.
  **
  ** This section is run by the right-most SELECT statement only.
  ** SELECT statements to the left always skip this part.  The right-most
  ** SELECT might also skip this part if it has no ORDER BY clause and
  ** no temp tables are required.
  */
  if( pOrderBy || p->usesEphm ){
    int i;                        /* Loop counter */
    KeyInfo *pKeyInfo;            /* Collating sequence for the result set */
    Select *pLoop;                /* For looping through SELECT statements */
    int nKeyCol;                  /* Number of entries in pKeyInfo->aCol[] */
    CollSeq **apColl;
    CollSeq **aCopy;

    assert( p->pRightmost==p );
    nKeyCol = nCol + (pOrderBy ? pOrderBy->nExpr : 0);
    pKeyInfo = sqliteMalloc(sizeof(*pKeyInfo)+nKeyCol*(sizeof(CollSeq*) + 1));
    if( !pKeyInfo ){
      rc = SQLITE_NOMEM;
      goto multi_select_end;
    }

    pKeyInfo->enc = ENC(pParse->db);
    pKeyInfo->nField = nCol;

    for(i=0, apColl=pKeyInfo->aColl; i<nCol; i++, apColl++){
      *apColl = multiSelectCollSeq(pParse, p, i);
      if( 0==*apColl ){
        *apColl = pParse->db->pDfltColl;
      }
    }

    for(pLoop=p; pLoop; pLoop=pLoop->pPrior){
      for(i=0; i<2; i++){
        int addr = pLoop->addrOpenEphm[i];
        if( addr<0 ){
          /* If [0] is unused then [1] is also unused.  So we can
          ** always safely abort as soon as the first unused slot is found */
          assert( pLoop->addrOpenEphm[1]<0 );
          break;
        }
        sqlite3VdbeChangeP2(v, addr, nCol);
        sqlite3VdbeChangeP3(v, addr, (char*)pKeyInfo, P3_KEYINFO);
        pLoop->addrOpenEphm[i] = -1;
      }
    }

    if( pOrderBy ){
      struct ExprList_item *pOTerm = pOrderBy->a;
      int nOrderByExpr = pOrderBy->nExpr;
      int addr;
      u8 *pSortOrder;

      aCopy = &pKeyInfo->aColl[nOrderByExpr];
      pSortOrder = pKeyInfo->aSortOrder = (u8*)&aCopy[nCol];
      memcpy(aCopy, pKeyInfo->aColl, nCol*sizeof(CollSeq*));
      apColl = pKeyInfo->aColl;
      for(i=0; i<nOrderByExpr; i++, pOTerm++, apColl++, pSortOrder++){
        Expr *pExpr = pOTerm->pExpr;
        if( (pExpr->flags & EP_ExpCollate) ){
          assert( pExpr->pColl!=0 );
          *apColl = pExpr->pColl;
        }else{
          *apColl = aCopy[pExpr->iColumn];
        }
        *pSortOrder = pOTerm->sortOrder;
      }
      assert( p->pRightmost==p );
      assert( p->addrOpenEphm[2]>=0 );
      addr = p->addrOpenEphm[2];
      sqlite3VdbeChangeP2(v, addr, p->pEList->nExpr+2);
      pKeyInfo->nField = nOrderByExpr;
      sqlite3VdbeChangeP3(v, addr, (char*)pKeyInfo, P3_KEYINFO_HANDOFF);
      pKeyInfo = 0;
      generateSortTail(pParse, p, v, p->pEList->nExpr, eDest, iParm);
    }

    sqliteFree(pKeyInfo);
  }

multi_select_end:
  return rc;
}
#endif /* SQLITE_OMIT_COMPOUND_SELECT */

#ifndef SQLITE_OMIT_VIEW
/*
** Scan through the expression pExpr.  Replace every reference to
** a column in table number iTable with a copy of the iColumn-th
** entry in pEList.  (But leave references to the ROWID column 
** unchanged.)
**
** This routine is part of the flattening procedure.  A subquery
** whose result set is defined by pEList appears as entry in the
** FROM clause of a SELECT such that the VDBE cursor assigned to that
** FORM clause entry is iTable.  This routine make the necessary 
** changes to pExpr so that it refers directly to the source table
** of the subquery rather the result set of the subquery.
*/
static void substExprList(ExprList*,int,ExprList*);  /* Forward Decl */
static void substSelect(Select *, int, ExprList *);  /* Forward Decl */
static void substExpr(Expr *pExpr, int iTable, ExprList *pEList){
  if( pExpr==0 ) return;
  if( pExpr->op==TK_COLUMN && pExpr->iTable==iTable ){
    if( pExpr->iColumn<0 ){
      pExpr->op = TK_NULL;
    }else{
      Expr *pNew;
      assert( pEList!=0 && pExpr->iColumn<pEList->nExpr );
      assert( pExpr->pLeft==0 && pExpr->pRight==0 && pExpr->pList==0 );
      pNew = pEList->a[pExpr->iColumn].pExpr;
      assert( pNew!=0 );
      pExpr->op = pNew->op;
      assert( pExpr->pLeft==0 );
      pExpr->pLeft = sqlite3ExprDup(pNew->pLeft);
      assert( pExpr->pRight==0 );
      pExpr->pRight = sqlite3ExprDup(pNew->pRight);
      assert( pExpr->pList==0 );
      pExpr->pList = sqlite3ExprListDup(pNew->pList);
      pExpr->iTable = pNew->iTable;
      pExpr->pTab = pNew->pTab;
      pExpr->iColumn = pNew->iColumn;
      pExpr->iAgg = pNew->iAgg;
      sqlite3TokenCopy(&pExpr->token, &pNew->token);
      sqlite3TokenCopy(&pExpr->span, &pNew->span);
      pExpr->pSelect = sqlite3SelectDup(pNew->pSelect);
      pExpr->flags = pNew->flags;
    }
  }else{
    substExpr(pExpr->pLeft, iTable, pEList);
    substExpr(pExpr->pRight, iTable, pEList);
    substSelect(pExpr->pSelect, iTable, pEList);
    substExprList(pExpr->pList, iTable, pEList);
  }
}
static void substExprList(ExprList *pList, int iTable, ExprList *pEList){
  int i;
  if( pList==0 ) return;
  for(i=0; i<pList->nExpr; i++){
    substExpr(pList->a[i].pExpr, iTable, pEList);
  }
}
static void substSelect(Select *p, int iTable, ExprList *pEList){
  if( !p ) return;
  substExprList(p->pEList, iTable, pEList);
  substExprList(p->pGroupBy, iTable, pEList);
  substExprList(p->pOrderBy, iTable, pEList);
  substExpr(p->pHaving, iTable, pEList);
  substExpr(p->pWhere, iTable, pEList);
}
#endif /* !defined(SQLITE_OMIT_VIEW) */

#ifndef SQLITE_OMIT_VIEW
/*
** This routine attempts to flatten subqueries in order to speed
** execution.  It returns 1 if it makes changes and 0 if no flattening
** occurs.
**
** To understand the concept of flattening, consider the following
** query:
**
**     SELECT a FROM (SELECT x+y AS a FROM t1 WHERE z<100) WHERE a>5
**
** The default way of implementing this query is to execute the
** subquery first and store the results in a temporary table, then
** run the outer query on that temporary table.  This requires two
** passes over the data.  Furthermore, because the temporary table
** has no indices, the WHERE clause on the outer query cannot be
** optimized.
**
** This routine attempts to rewrite queries such as the above into
** a single flat select, like this:
**
**     SELECT x+y AS a FROM t1 WHERE z<100 AND a>5
**
** The code generated for this simpification gives the same result
** but only has to scan the data once.  And because indices might 
** exist on the table t1, a complete scan of the data might be
** avoided.
**
** Flattening is only attempted if all of the following are true:
**
**   (1)  The subquery and the outer query do not both use aggregates.
**
**   (2)  The subquery is not an aggregate or the outer query is not a join.
**
**   (3)  The subquery is not the right operand of a left outer join, or
**        the subquery is not itself a join.  (Ticket #306)
**
**   (4)  The subquery is not DISTINCT or the outer query is not a join.
**
**   (5)  The subquery is not DISTINCT or the outer query does not use
**        aggregates.
**
**   (6)  The subquery does not use aggregates or the outer query is not
**        DISTINCT.
**
**   (7)  The subquery has a FROM clause.
**
**   (8)  The subquery does not use LIMIT or the outer query is not a join.
**
**   (9)  The subquery does not use LIMIT or the outer query does not use
**        aggregates.
**
**  (10)  The subquery does not use aggregates or the outer query does not
**        use LIMIT.
**
**  (11)  The subquery and the outer query do not both have ORDER BY clauses.
**
**  (12)  The subquery is not the right term of a LEFT OUTER JOIN or the
**        subquery has no WHERE clause.  (added by ticket #350)
**
**  (13)  The subquery and outer query do not both use LIMIT
**
**  (14)  The subquery does not use OFFSET
**
** In this routine, the "p" parameter is a pointer to the outer query.
** The subquery is p->pSrc->a[iFrom].  isAgg is true if the outer query
** uses aggregates and subqueryIsAgg is true if the subquery uses aggregates.
**
** If flattening is not attempted, this routine is a no-op and returns 0.
** If flattening is attempted this routine returns 1.
**
** All of the expression analysis must occur on both the outer query and
** the subquery before this routine runs.
*/
static int flattenSubquery(
  Select *p,           /* The parent or outer SELECT statement */
  int iFrom,           /* Index in p->pSrc->a[] of the inner subquery */
  int isAgg,           /* True if outer SELECT uses aggregate functions */
  int subqueryIsAgg    /* True if the subquery uses aggregate functions */
){
  Select *pSub;       /* The inner query or "subquery" */
  SrcList *pSrc;      /* The FROM clause of the outer query */
  SrcList *pSubSrc;   /* The FROM clause of the subquery */
  ExprList *pList;    /* The result set of the outer query */
  int iParent;        /* VDBE cursor number of the pSub result set temp table */
  int i;              /* Loop counter */
  Expr *pWhere;                    /* The WHERE clause */
  struct SrcList_item *pSubitem;   /* The subquery */

  /* Check to see if flattening is permitted.  Return 0 if not.
  */
  if( p==0 ) return 0;
  pSrc = p->pSrc;
  assert( pSrc && iFrom>=0 && iFrom<pSrc->nSrc );
  pSubitem = &pSrc->a[iFrom];
  pSub = pSubitem->pSelect;
  assert( pSub!=0 );
  if( isAgg && subqueryIsAgg ) return 0;                 /* Restriction (1)  */
  if( subqueryIsAgg && pSrc->nSrc>1 ) return 0;          /* Restriction (2)  */
  pSubSrc = pSub->pSrc;
  assert( pSubSrc );
  /* Prior to version 3.1.2, when LIMIT and OFFSET had to be simple constants,
  ** not arbitrary expresssions, we allowed some combining of LIMIT and OFFSET
  ** because they could be computed at compile-time.  But when LIMIT and OFFSET
  ** became arbitrary expressions, we were forced to add restrictions (13)
  ** and (14). */
  if( pSub->pLimit && p->pLimit ) return 0;              /* Restriction (13) */
  if( pSub->pOffset ) return 0;                          /* Restriction (14) */
  if( pSubSrc->nSrc==0 ) return 0;                       /* Restriction (7)  */
  if( (pSub->isDistinct || pSub->pLimit) 
         && (pSrc->nSrc>1 || isAgg) ){          /* Restrictions (4)(5)(8)(9) */
     return 0;       
  }
  if( p->isDistinct && subqueryIsAgg ) return 0;         /* Restriction (6)  */
  if( (p->disallowOrderBy || p->pOrderBy) && pSub->pOrderBy ){
     return 0;                                           /* Restriction (11) */
  }

  /* Restriction 3:  If the subquery is a join, make sure the subquery is 
  ** not used as the right operand of an outer join.  Examples of why this
  ** is not allowed:
  **
  **         t1 LEFT OUTER JOIN (t2 JOIN t3)
  **
  ** If we flatten the above, we would get
  **
  **         (t1 LEFT OUTER JOIN t2) JOIN t3
  **
  ** which is not at all the same thing.
  */
  if( pSubSrc->nSrc>1 && (pSubitem->jointype & JT_OUTER)!=0 ){
    return 0;
  }

  /* Restriction 12:  If the subquery is the right operand of a left outer
  ** join, make sure the subquery has no WHERE clause.
  ** An examples of why this is not allowed:
  **
  **         t1 LEFT OUTER JOIN (SELECT * FROM t2 WHERE t2.x>0)
  **
  ** If we flatten the above, we would get
  **
  **         (t1 LEFT OUTER JOIN t2) WHERE t2.x>0
  **
  ** But the t2.x>0 test will always fail on a NULL row of t2, which
  ** effectively converts the OUTER JOIN into an INNER JOIN.
  */
  if( (pSubitem->jointype & JT_OUTER)!=0 && pSub->pWhere!=0 ){
    return 0;
  }

  /* If we reach this point, it means flattening is permitted for the
  ** iFrom-th entry of the FROM clause in the outer query.
  */

  /* Move all of the FROM elements of the subquery into the
  ** the FROM clause of the outer query.  Before doing this, remember
  ** the cursor number for the original outer query FROM element in
  ** iParent.  The iParent cursor will never be used.  Subsequent code
  ** will scan expressions looking for iParent references and replace
  ** those references with expressions that resolve to the subquery FROM
  ** elements we are now copying in.
  */
  iParent = pSubitem->iCursor;
  {
    int nSubSrc = pSubSrc->nSrc;
    int jointype = pSubitem->jointype;

    sqlite3DeleteTable(pSubitem->pTab);
    sqliteFree(pSubitem->zDatabase);
    sqliteFree(pSubitem->zName);
    sqliteFree(pSubitem->zAlias);
    if( nSubSrc>1 ){
      int extra = nSubSrc - 1;
      for(i=1; i<nSubSrc; i++){
        pSrc = sqlite3SrcListAppend(pSrc, 0, 0);
      }
      p->pSrc = pSrc;
      for(i=pSrc->nSrc-1; i-extra>=iFrom; i--){
        pSrc->a[i] = pSrc->a[i-extra];
      }
    }
    for(i=0; i<nSubSrc; i++){
      pSrc->a[i+iFrom] = pSubSrc->a[i];
      memset(&pSubSrc->a[i], 0, sizeof(pSubSrc->a[i]));
    }
    pSrc->a[iFrom].jointype = jointype;
  }

  /* Now begin substituting subquery result set expressions for 
  ** references to the iParent in the outer query.
  ** 
  ** Example:
  **
  **   SELECT a+5, b*10 FROM (SELECT x*3 AS a, y+10 AS b FROM t1) WHERE a>b;
  **   \                     \_____________ subquery __________/          /
  **    \_____________________ outer query ______________________________/
  **
  ** We look at every expression in the outer query and every place we see
  ** "a" we substitute "x*3" and every place we see "b" we substitute "y+10".
  */
  pList = p->pEList;
  for(i=0; i<pList->nExpr; i++){
    Expr *pExpr;
    if( pList->a[i].zName==0 && (pExpr = pList->a[i].pExpr)->span.z!=0 ){
      pList->a[i].zName = sqliteStrNDup((char*)pExpr->span.z, pExpr->span.n);
    }
  }
  substExprList(p->pEList, iParent, pSub->pEList);
  if( isAgg ){
    substExprList(p->pGroupBy, iParent, pSub->pEList);
    substExpr(p->pHaving, iParent, pSub->pEList);
  }
  if( pSub->pOrderBy ){
    assert( p->pOrderBy==0 );
    p->pOrderBy = pSub->pOrderBy;
    pSub->pOrderBy = 0;
  }else if( p->pOrderBy ){
    substExprList(p->pOrderBy, iParent, pSub->pEList);
  }
  if( pSub->pWhere ){
    pWhere = sqlite3ExprDup(pSub->pWhere);
  }else{
    pWhere = 0;
  }
  if( subqueryIsAgg ){
    assert( p->pHaving==0 );
    p->pHaving = p->pWhere;
    p->pWhere = pWhere;
    substExpr(p->pHaving, iParent, pSub->pEList);
    p->pHaving = sqlite3ExprAnd(p->pHaving, sqlite3ExprDup(pSub->pHaving));
    assert( p->pGroupBy==0 );
    p->pGroupBy = sqlite3ExprListDup(pSub->pGroupBy);
  }else{
    substExpr(p->pWhere, iParent, pSub->pEList);
    p->pWhere = sqlite3ExprAnd(p->pWhere, pWhere);
  }

  /* The flattened query is distinct if either the inner or the
  ** outer query is distinct. 
  */
  p->isDistinct = p->isDistinct || pSub->isDistinct;

  /*
  ** SELECT ... FROM (SELECT ... LIMIT a OFFSET b) LIMIT x OFFSET y;
  **
  ** One is tempted to try to add a and b to combine the limits.  But this
  ** does not work if either limit is negative.
  */
  if( pSub->pLimit ){
    p->pLimit = pSub->pLimit;
    pSub->pLimit = 0;
  }

  /* Finially, delete what is left of the subquery and return
  ** success.
  */
  sqlite3SelectDelete(pSub);
  return 1;
}
#endif /* SQLITE_OMIT_VIEW */

/*
** Analyze the SELECT statement passed in as an argument to see if it
** is a simple min() or max() query.  If it is and this query can be
** satisfied using a single seek to the beginning or end of an index,
** then generate the code for this SELECT and return 1.  If this is not a 
** simple min() or max() query, then return 0;
**
** A simply min() or max() query looks like this:
**
**    SELECT min(a) FROM table;
**    SELECT max(a) FROM table;
**
** The query may have only a single table in its FROM argument.  There
** can be no GROUP BY or HAVING or WHERE clauses.  The result set must
** be the min() or max() of a single column of the table.  The column
** in the min() or max() function must be indexed.
**
** The parameters to this routine are the same as for sqlite3Select().
** See the header comment on that routine for additional information.
*/
static int simpleMinMaxQuery(Parse *pParse, Select *p, int eDest, int iParm){
  Expr *pExpr;
  int iCol;
  Table *pTab;
  Index *pIdx;
  int base;
  Vdbe *v;
  int seekOp;
  ExprList *pEList, *pList, eList;
  struct ExprList_item eListItem;
  SrcList *pSrc;
  int brk;
  int iDb;

  /* Check to see if this query is a simple min() or max() query.  Return
  ** zero if it is  not.
  */
  if( p->pGroupBy || p->pHaving || p->pWhere ) return 0;
  pSrc = p->pSrc;
  if( pSrc->nSrc!=1 ) return 0;
  pEList = p->pEList;
  if( pEList->nExpr!=1 ) return 0;
  pExpr = pEList->a[0].pExpr;
  if( pExpr->op!=TK_AGG_FUNCTION ) return 0;
  pList = pExpr->pList;
  if( pList==0 || pList->nExpr!=1 ) return 0;
  if( pExpr->token.n!=3 ) return 0;
  if( sqlite3StrNICmp((char*)pExpr->token.z,"min",3)==0 ){
    seekOp = OP_Rewind;
  }else if( sqlite3StrNICmp((char*)pExpr->token.z,"max",3)==0 ){
    seekOp = OP_Last;
  }else{
    return 0;
  }
  pExpr = pList->a[0].pExpr;
  if( pExpr->op!=TK_COLUMN ) return 0;
  iCol = pExpr->iColumn;
  pTab = pSrc->a[0].pTab;

  /* This optimization cannot be used with virtual tables. */
  if( IsVirtual(pTab) ) return 0;

  /* If we get to here, it means the query is of the correct form.
  ** Check to make sure we have an index and make pIdx point to the
  ** appropriate index.  If the min() or max() is on an INTEGER PRIMARY
  ** key column, no index is necessary so set pIdx to NULL.  If no
  ** usable index is found, return 0.
  */
  if( iCol<0 ){
    pIdx = 0;
  }else{
    CollSeq *pColl = sqlite3ExprCollSeq(pParse, pExpr);
    if( pColl==0 ) return 0;
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      assert( pIdx->nColumn>=1 );
      if( pIdx->aiColumn[0]==iCol && 
          0==sqlite3StrICmp(pIdx->azColl[0], pColl->zName) ){
        break;
      }
    }
    if( pIdx==0 ) return 0;
  }

  /* Identify column types if we will be using the callback.  This
  ** step is skipped if the output is going to a table or a memory cell.
  ** The column names have already been generated in the calling function.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ) return 0;

  /* If the output is destined for a temporary table, open that table.
  */
  if( eDest==SRT_EphemTab ){
    sqlite3VdbeAddOp(v, OP_OpenEphemeral, iParm, 1);
  }

  /* Generating code to find the min or the max.  Basically all we have
  ** to do is find the first or the last entry in the chosen index.  If
  ** the min() or max() is on the INTEGER PRIMARY KEY, then find the first
  ** or last entry in the main table.
  */
  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
  assert( iDb>=0 || pTab->isEphem );
  sqlite3CodeVerifySchema(pParse, iDb);
  sqlite3TableLock(pParse, iDb, pTab->tnum, 0, pTab->zName);
  base = pSrc->a[0].iCursor;
  brk = sqlite3VdbeMakeLabel(v);
  computeLimitRegisters(pParse, p, brk);
  if( pSrc->a[0].pSelect==0 ){
    sqlite3OpenTable(pParse, base, iDb, pTab, OP_OpenRead);
  }
  if( pIdx==0 ){
    sqlite3VdbeAddOp(v, seekOp, base, 0);
  }else{
    /* Even though the cursor used to open the index here is closed
    ** as soon as a single value has been read from it, allocate it
    ** using (pParse->nTab++) to prevent the cursor id from being 
    ** reused. This is important for statements of the form 
    ** "INSERT INTO x SELECT max() FROM x".
    */
    int iIdx;
    KeyInfo *pKey = sqlite3IndexKeyinfo(pParse, pIdx);
    iIdx = pParse->nTab++;
    assert( pIdx->pSchema==pTab->pSchema );
    sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
    sqlite3VdbeOp3(v, OP_OpenRead, iIdx, pIdx->tnum, 
        (char*)pKey, P3_KEYINFO_HANDOFF);
    if( seekOp==OP_Rewind ){
      sqlite3VdbeAddOp(v, OP_Null, 0, 0);
      sqlite3VdbeAddOp(v, OP_MakeRecord, 1, 0);
      seekOp = OP_MoveGt;
    }
    sqlite3VdbeAddOp(v, seekOp, iIdx, 0);
    sqlite3VdbeAddOp(v, OP_IdxRowid, iIdx, 0);
    sqlite3VdbeAddOp(v, OP_Close, iIdx, 0);
    sqlite3VdbeAddOp(v, OP_MoveGe, base, 0);
  }
  eList.nExpr = 1;
  memset(&eListItem, 0, sizeof(eListItem));
  eList.a = &eListItem;
  eList.a[0].pExpr = pExpr;
  selectInnerLoop(pParse, p, &eList, 0, 0, 0, -1, eDest, iParm, brk, brk, 0);
  sqlite3VdbeResolveLabel(v, brk);
  sqlite3VdbeAddOp(v, OP_Close, base, 0);
  
  return 1;
}

/*
** Analyze and ORDER BY or GROUP BY clause in a SELECT statement.  Return
** the number of errors seen.
**
** An ORDER BY or GROUP BY is a list of expressions.  If any expression
** is an integer constant, then that expression is replaced by the
** corresponding entry in the result set.
*/
static int processOrderGroupBy(
  NameContext *pNC,     /* Name context of the SELECT statement. */
  ExprList *pOrderBy,   /* The ORDER BY or GROUP BY clause to be processed */
  const char *zType     /* Either "ORDER" or "GROUP", as appropriate */
){
  int i;
  ExprList *pEList = pNC->pEList;     /* The result set of the SELECT */
  Parse *pParse = pNC->pParse;     /* The result set of the SELECT */
  assert( pEList );

  if( pOrderBy==0 ) return 0;
  for(i=0; i<pOrderBy->nExpr; i++){
    int iCol;
    Expr *pE = pOrderBy->a[i].pExpr;
    if( sqlite3ExprIsInteger(pE, &iCol) ){
      if( iCol>0 && iCol<=pEList->nExpr ){
        CollSeq *pColl = pE->pColl;
        int flags = pE->flags & EP_ExpCollate;
        sqlite3ExprDelete(pE);
        pE = pOrderBy->a[i].pExpr = sqlite3ExprDup(pEList->a[iCol-1].pExpr);
        if( pColl && flags ){
          pE->pColl = pColl;
          pE->flags |= flags;
        }
      }else{
        sqlite3ErrorMsg(pParse, 
           "%s BY column number %d out of range - should be "
           "between 1 and %d", zType, iCol, pEList->nExpr);
        return 1;
      }
    }
    if( sqlite3ExprResolveNames(pNC, pE) ){
      return 1;
    }
  }
  return 0;
}

/*
** This routine resolves any names used in the result set of the
** supplied SELECT statement. If the SELECT statement being resolved
** is a sub-select, then pOuterNC is a pointer to the NameContext 
** of the parent SELECT.
*/
int sqlite3SelectResolve(
  Parse *pParse,         /* The parser context */
  Select *p,             /* The SELECT statement being coded. */
  NameContext *pOuterNC  /* The outer name context. May be NULL. */
){
  ExprList *pEList;          /* Result set. */
  int i;                     /* For-loop variable used in multiple places */
  NameContext sNC;           /* Local name-context */
  ExprList *pGroupBy;        /* The group by clause */

  /* If this routine has run before, return immediately. */
  if( p->isResolved ){
    assert( !pOuterNC );
    return SQLITE_OK;
  }
  p->isResolved = 1;

  /* If there have already been errors, do nothing. */
  if( pParse->nErr>0 ){
    return SQLITE_ERROR;
  }

  /* Prepare the select statement. This call will allocate all cursors
  ** required to handle the tables and subqueries in the FROM clause.
  */
  if( prepSelectStmt(pParse, p) ){
    return SQLITE_ERROR;
  }

  /* Resolve the expressions in the LIMIT and OFFSET clauses. These
  ** are not allowed to refer to any names, so pass an empty NameContext.
  */
  memset(&sNC, 0, sizeof(sNC));
  sNC.pParse = pParse;
  if( sqlite3ExprResolveNames(&sNC, p->pLimit) ||
      sqlite3ExprResolveNames(&sNC, p->pOffset) ){
    return SQLITE_ERROR;
  }

  /* Set up the local name-context to pass to ExprResolveNames() to
  ** resolve the expression-list.
  */
  sNC.allowAgg = 1;
  sNC.pSrcList = p->pSrc;
  sNC.pNext = pOuterNC;

  /* Resolve names in the result set. */
  pEList = p->pEList;
  if( !pEList ) return SQLITE_ERROR;
  for(i=0; i<pEList->nExpr; i++){
    Expr *pX = pEList->a[i].pExpr;
    if( sqlite3ExprResolveNames(&sNC, pX) ){
      return SQLITE_ERROR;
    }
  }

  /* If there are no aggregate functions in the result-set, and no GROUP BY 
  ** expression, do not allow aggregates in any of the other expressions.
  */
  assert( !p->isAgg );
  pGroupBy = p->pGroupBy;
  if( pGroupBy || sNC.hasAgg ){
    p->isAgg = 1;
  }else{
    sNC.allowAgg = 0;
  }

  /* If a HAVING clause is present, then there must be a GROUP BY clause.
  */
  if( p->pHaving && !pGroupBy ){
    sqlite3ErrorMsg(pParse, "a GROUP BY clause is required before HAVING");
    return SQLITE_ERROR;
  }

  /* Add the expression list to the name-context before parsing the
  ** other expressions in the SELECT statement. This is so that
  ** expressions in the WHERE clause (etc.) can refer to expressions by
  ** aliases in the result set.
  **
  ** Minor point: If this is the case, then the expression will be
  ** re-evaluated for each reference to it.
  */
  sNC.pEList = p->pEList;
  if( sqlite3ExprResolveNames(&sNC, p->pWhere) ||
     sqlite3ExprResolveNames(&sNC, p->pHaving) ){
    return SQLITE_ERROR;
  }
  if( p->pPrior==0 ){
    if( processOrderGroupBy(&sNC, p->pOrderBy, "ORDER") ||
        processOrderGroupBy(&sNC, pGroupBy, "GROUP") ){
      return SQLITE_ERROR;
    }
  }

  /* Make sure the GROUP BY clause does not contain aggregate functions.
  */
  if( pGroupBy ){
    struct ExprList_item *pItem;
  
    for(i=0, pItem=pGroupBy->a; i<pGroupBy->nExpr; i++, pItem++){
      if( ExprHasProperty(pItem->pExpr, EP_Agg) ){
        sqlite3ErrorMsg(pParse, "aggregate functions are not allowed in "
            "the GROUP BY clause");
        return SQLITE_ERROR;
      }
    }
  }

  /* If this is one SELECT of a compound, be sure to resolve names
  ** in the other SELECTs.
  */
  if( p->pPrior ){
    return sqlite3SelectResolve(pParse, p->pPrior, pOuterNC);
  }else{
    return SQLITE_OK;
  }
}

/*
** Reset the aggregate accumulator.
**
** The aggregate accumulator is a set of memory cells that hold
** intermediate results while calculating an aggregate.  This
** routine simply stores NULLs in all of those memory cells.
*/
static void resetAccumulator(Parse *pParse, AggInfo *pAggInfo){
  Vdbe *v = pParse->pVdbe;
  int i;
  struct AggInfo_func *pFunc;
  if( pAggInfo->nFunc+pAggInfo->nColumn==0 ){
    return;
  }
  for(i=0; i<pAggInfo->nColumn; i++){
    sqlite3VdbeAddOp(v, OP_MemNull, pAggInfo->aCol[i].iMem, 0);
  }
  for(pFunc=pAggInfo->aFunc, i=0; i<pAggInfo->nFunc; i++, pFunc++){
    sqlite3VdbeAddOp(v, OP_MemNull, pFunc->iMem, 0);
    if( pFunc->iDistinct>=0 ){
      Expr *pE = pFunc->pExpr;
      if( pE->pList==0 || pE->pList->nExpr!=1 ){
        sqlite3ErrorMsg(pParse, "DISTINCT in aggregate must be followed "
           "by an expression");
        pFunc->iDistinct = -1;
      }else{
        KeyInfo *pKeyInfo = keyInfoFromExprList(pParse, pE->pList);
        sqlite3VdbeOp3(v, OP_OpenEphemeral, pFunc->iDistinct, 0, 
                          (char*)pKeyInfo, P3_KEYINFO_HANDOFF);
      }
    }
  }
}

/*
** Invoke the OP_AggFinalize opcode for every aggregate function
** in the AggInfo structure.
*/
static void finalizeAggFunctions(Parse *pParse, AggInfo *pAggInfo){
  Vdbe *v = pParse->pVdbe;
  int i;
  struct AggInfo_func *pF;
  for(i=0, pF=pAggInfo->aFunc; i<pAggInfo->nFunc; i++, pF++){
    ExprList *pList = pF->pExpr->pList;
    sqlite3VdbeOp3(v, OP_AggFinal, pF->iMem, pList ? pList->nExpr : 0,
                      (void*)pF->pFunc, P3_FUNCDEF);
  }
}

/*
** Update the accumulator memory cells for an aggregate based on
** the current cursor position.
*/
static void updateAccumulator(Parse *pParse, AggInfo *pAggInfo){
  Vdbe *v = pParse->pVdbe;
  int i;
  struct AggInfo_func *pF;
  struct AggInfo_col *pC;

  pAggInfo->directMode = 1;
  for(i=0, pF=pAggInfo->aFunc; i<pAggInfo->nFunc; i++, pF++){
    int nArg;
    int addrNext = 0;
    ExprList *pList = pF->pExpr->pList;
    if( pList ){
      nArg = pList->nExpr;
      sqlite3ExprCodeExprList(pParse, pList);
    }else{
      nArg = 0;
    }
    if( pF->iDistinct>=0 ){
      addrNext = sqlite3VdbeMakeLabel(v);
      assert( nArg==1 );
      codeDistinct(v, pF->iDistinct, addrNext, 1);
    }
    if( pF->pFunc->needCollSeq ){
      CollSeq *pColl = 0;
      struct ExprList_item *pItem;
      int j;
      assert( pList!=0 );  /* pList!=0 if pF->pFunc->needCollSeq is true */
      for(j=0, pItem=pList->a; !pColl && j<nArg; j++, pItem++){
        pColl = sqlite3ExprCollSeq(pParse, pItem->pExpr);
      }
      if( !pColl ){
        pColl = pParse->db->pDfltColl;
      }
      sqlite3VdbeOp3(v, OP_CollSeq, 0, 0, (char *)pColl, P3_COLLSEQ);
    }
    sqlite3VdbeOp3(v, OP_AggStep, pF->iMem, nArg, (void*)pF->pFunc, P3_FUNCDEF);
    if( addrNext ){
      sqlite3VdbeResolveLabel(v, addrNext);
    }
  }
  for(i=0, pC=pAggInfo->aCol; i<pAggInfo->nAccumulator; i++, pC++){
    sqlite3ExprCode(pParse, pC->pExpr);
    sqlite3VdbeAddOp(v, OP_MemStore, pC->iMem, 1);
  }
  pAggInfo->directMode = 0;
}


/*
** Generate code for the given SELECT statement.
**
** The results are distributed in various ways depending on the
** value of eDest and iParm.
**
**     eDest Value       Result
**     ------------    -------------------------------------------
**     SRT_Callback    Invoke the callback for each row of the result.
**
**     SRT_Mem         Store first result in memory cell iParm
**
**     SRT_Set         Store results as keys of table iParm.
**
**     SRT_Union       Store results as a key in a temporary table iParm
**
**     SRT_Except      Remove results from the temporary table iParm.
**
**     SRT_Table       Store results in temporary table iParm
**
** The table above is incomplete.  Additional eDist value have be added
** since this comment was written.  See the selectInnerLoop() function for
** a complete listing of the allowed values of eDest and their meanings.
**
** This routine returns the number of errors.  If any errors are
** encountered, then an appropriate error message is left in
** pParse->zErrMsg.
**
** This routine does NOT free the Select structure passed in.  The
** calling function needs to do that.
**
** The pParent, parentTab, and *pParentAgg fields are filled in if this
** SELECT is a subquery.  This routine may try to combine this SELECT
** with its parent to form a single flat query.  In so doing, it might
** change the parent query from a non-aggregate to an aggregate query.
** For that reason, the pParentAgg flag is passed as a pointer, so it
** can be changed.
**
** Example 1:   The meaning of the pParent parameter.
**
**    SELECT * FROM t1 JOIN (SELECT x, count(*) FROM t2) JOIN t3;
**    \                      \_______ subquery _______/        /
**     \                                                      /
**      \____________________ outer query ___________________/
**
** This routine is called for the outer query first.   For that call,
** pParent will be NULL.  During the processing of the outer query, this 
** routine is called recursively to handle the subquery.  For the recursive
** call, pParent will point to the outer query.  Because the subquery is
** the second element in a three-way join, the parentTab parameter will
** be 1 (the 2nd value of a 0-indexed array.)
*/
int sqlite3Select(
  Parse *pParse,         /* The parser context */
  Select *p,             /* The SELECT statement being coded. */
  int eDest,             /* How to dispose of the results */
  int iParm,             /* A parameter used by the eDest disposal method */
  Select *pParent,       /* Another SELECT for which this is a sub-query */
  int parentTab,         /* Index in pParent->pSrc of this query */
  int *pParentAgg,       /* True if pParent uses aggregate functions */
  char *aff              /* If eDest is SRT_Union, the affinity string */
){
  int i, j;              /* Loop counters */
  WhereInfo *pWInfo;     /* Return from sqlite3WhereBegin() */
  Vdbe *v;               /* The virtual machine under construction */
  int isAgg;             /* True for select lists like "count(*)" */
  ExprList *pEList;      /* List of columns to extract. */
  SrcList *pTabList;     /* List of tables to select from */
  Expr *pWhere;          /* The WHERE clause.  May be NULL */
  ExprList *pOrderBy;    /* The ORDER BY clause.  May be NULL */
  ExprList *pGroupBy;    /* The GROUP BY clause.  May be NULL */
  Expr *pHaving;         /* The HAVING clause.  May be NULL */
  int isDistinct;        /* True if the DISTINCT keyword is present */
  int distinct;          /* Table to use for the distinct set */
  int rc = 1;            /* Value to return from this function */
  int addrSortIndex;     /* Address of an OP_OpenEphemeral instruction */
  AggInfo sAggInfo;      /* Information used by aggregate queries */
  int iEnd;              /* Address of the end of the query */

  if( p==0 || sqlite3MallocFailed() || pParse->nErr ){
    return 1;
  }
  if( sqlite3AuthCheck(pParse, SQLITE_SELECT, 0, 0, 0) ) return 1;
  memset(&sAggInfo, 0, sizeof(sAggInfo));

#ifndef SQLITE_OMIT_COMPOUND_SELECT
  /* If there is are a sequence of queries, do the earlier ones first.
  */
  if( p->pPrior ){
    if( p->pRightmost==0 ){
      Select *pLoop;
      for(pLoop=p; pLoop; pLoop=pLoop->pPrior){
        pLoop->pRightmost = p;
      }
    }
    return multiSelect(pParse, p, eDest, iParm, aff);
  }
#endif

  pOrderBy = p->pOrderBy;
  if( IgnorableOrderby(eDest) ){
    p->pOrderBy = 0;
  }
  if( sqlite3SelectResolve(pParse, p, 0) ){
    goto select_end;
  }
  p->pOrderBy = pOrderBy;

  /* Make local copies of the parameters for this query.
  */
  pTabList = p->pSrc;
  pWhere = p->pWhere;
  pGroupBy = p->pGroupBy;
  pHaving = p->pHaving;
  isAgg = p->isAgg;
  isDistinct = p->isDistinct;
  pEList = p->pEList;
  if( pEList==0 ) goto select_end;

  /* 
  ** Do not even attempt to generate any code if we have already seen
  ** errors before this routine starts.
  */
  if( pParse->nErr>0 ) goto select_end;

  /* If writing to memory or generating a set
  ** only a single column may be output.
  */
#ifndef SQLITE_OMIT_SUBQUERY
  if( (eDest==SRT_Mem || eDest==SRT_Set) && pEList->nExpr>1 ){
    sqlite3ErrorMsg(pParse, "only a single result allowed for "
       "a SELECT that is part of an expression");
    goto select_end;
  }
#endif

  /* ORDER BY is ignored for some destinations.
  */
  if( IgnorableOrderby(eDest) ){
    pOrderBy = 0;
  }

  /* Begin generating code.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ) goto select_end;

  /* Generate code for all sub-queries in the FROM clause
  */
#if !defined(SQLITE_OMIT_SUBQUERY) || !defined(SQLITE_OMIT_VIEW)
  for(i=0; i<pTabList->nSrc; i++){
    const char *zSavedAuthContext = 0;
    int needRestoreContext;
    struct SrcList_item *pItem = &pTabList->a[i];

    if( pItem->pSelect==0 || pItem->isPopulated ) continue;
    if( pItem->zName!=0 ){
      zSavedAuthContext = pParse->zAuthContext;
      pParse->zAuthContext = pItem->zName;
      needRestoreContext = 1;
    }else{
      needRestoreContext = 0;
    }
    sqlite3Select(pParse, pItem->pSelect, SRT_EphemTab, 
                 pItem->iCursor, p, i, &isAgg, 0);
    if( needRestoreContext ){
      pParse->zAuthContext = zSavedAuthContext;
    }
    pTabList = p->pSrc;
    pWhere = p->pWhere;
    if( !IgnorableOrderby(eDest) ){
      pOrderBy = p->pOrderBy;
    }
    pGroupBy = p->pGroupBy;
    pHaving = p->pHaving;
    isDistinct = p->isDistinct;
  }
#endif

  /* Check for the special case of a min() or max() function by itself
  ** in the result set.
  */
  if( simpleMinMaxQuery(pParse, p, eDest, iParm) ){
    rc = 0;
    goto select_end;
  }

  /* Check to see if this is a subquery that can be "flattened" into its parent.
  ** If flattening is a possiblity, do so and return immediately.  
  */
#ifndef SQLITE_OMIT_VIEW
  if( pParent && pParentAgg &&
      flattenSubquery(pParent, parentTab, *pParentAgg, isAgg) ){
    if( isAgg ) *pParentAgg = 1;
    goto select_end;
  }
#endif

  /* If there is an ORDER BY clause, then this sorting
  ** index might end up being unused if the data can be 
  ** extracted in pre-sorted order.  If that is the case, then the
  ** OP_OpenEphemeral instruction will be changed to an OP_Noop once
  ** we figure out that the sorting index is not needed.  The addrSortIndex
  ** variable is used to facilitate that change.
  */
  if( pOrderBy ){
    KeyInfo *pKeyInfo;
    if( pParse->nErr ){
      goto select_end;
    }
    pKeyInfo = keyInfoFromExprList(pParse, pOrderBy);
    pOrderBy->iECursor = pParse->nTab++;
    p->addrOpenEphm[2] = addrSortIndex =
      sqlite3VdbeOp3(v, OP_OpenEphemeral, pOrderBy->iECursor, pOrderBy->nExpr+2,                     (char*)pKeyInfo, P3_KEYINFO_HANDOFF);
  }else{
    addrSortIndex = -1;
  }

  /* If the output is destined for a temporary table, open that table.
  */
  if( eDest==SRT_EphemTab ){
    sqlite3VdbeAddOp(v, OP_OpenEphemeral, iParm, pEList->nExpr);
  }

  /* Set the limiter.
  */
  iEnd = sqlite3VdbeMakeLabel(v);
  computeLimitRegisters(pParse, p, iEnd);

  /* Open a virtual index to use for the distinct set.
  */
  if( isDistinct ){
    KeyInfo *pKeyInfo;
    distinct = pParse->nTab++;
    pKeyInfo = keyInfoFromExprList(pParse, p->pEList);
    sqlite3VdbeOp3(v, OP_OpenEphemeral, distinct, 0, 
                        (char*)pKeyInfo, P3_KEYINFO_HANDOFF);
  }else{
    distinct = -1;
  }

  /* Aggregate and non-aggregate queries are handled differently */
  if( !isAgg && pGroupBy==0 ){
    /* This case is for non-aggregate queries
    ** Begin the database scan
    */
    pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, &pOrderBy);
    if( pWInfo==0 ) goto select_end;

    /* If sorting index that was created by a prior OP_OpenEphemeral 
    ** instruction ended up not being needed, then change the OP_OpenEphemeral
    ** into an OP_Noop.
    */
    if( addrSortIndex>=0 && pOrderBy==0 ){
      sqlite3VdbeChangeToNoop(v, addrSortIndex, 1);
      p->addrOpenEphm[2] = -1;
    }

    /* Use the standard inner loop
    */
    if( selectInnerLoop(pParse, p, pEList, 0, 0, pOrderBy, distinct, eDest,
                    iParm, pWInfo->iContinue, pWInfo->iBreak, aff) ){
       goto select_end;
    }

    /* End the database scan loop.
    */
    sqlite3WhereEnd(pWInfo);
  }else{
    /* This is the processing for aggregate queries */
    NameContext sNC;    /* Name context for processing aggregate information */
    int iAMem;          /* First Mem address for storing current GROUP BY */
    int iBMem;          /* First Mem address for previous GROUP BY */
    int iUseFlag;       /* Mem address holding flag indicating that at least
                        ** one row of the input to the aggregator has been
                        ** processed */
    int iAbortFlag;     /* Mem address which causes query abort if positive */
    int groupBySort;    /* Rows come from source in GROUP BY order */


    /* The following variables hold addresses or labels for parts of the
    ** virtual machine program we are putting together */
    int addrOutputRow;      /* Start of subroutine that outputs a result row */
    int addrSetAbort;       /* Set the abort flag and return */
    int addrInitializeLoop; /* Start of code that initializes the input loop */
    int addrTopOfLoop;      /* Top of the input loop */
    int addrGroupByChange;  /* Code that runs when any GROUP BY term changes */
    int addrProcessRow;     /* Code to process a single input row */
    int addrEnd;            /* End of all processing */
    int addrSortingIdx;     /* The OP_OpenEphemeral for the sorting index */
    int addrReset;          /* Subroutine for resetting the accumulator */

    addrEnd = sqlite3VdbeMakeLabel(v);

    /* Convert TK_COLUMN nodes into TK_AGG_COLUMN and make entries in
    ** sAggInfo for all TK_AGG_FUNCTION nodes in expressions of the
    ** SELECT statement.
    */
    memset(&sNC, 0, sizeof(sNC));
    sNC.pParse = pParse;
    sNC.pSrcList = pTabList;
    sNC.pAggInfo = &sAggInfo;
    sAggInfo.nSortingColumn = pGroupBy ? pGroupBy->nExpr+1 : 0;
    sAggInfo.pGroupBy = pGroupBy;
    if( sqlite3ExprAnalyzeAggList(&sNC, pEList) ){
      goto select_end;
    }
    if( sqlite3ExprAnalyzeAggList(&sNC, pOrderBy) ){
      goto select_end;
    }
    if( pHaving && sqlite3ExprAnalyzeAggregates(&sNC, pHaving) ){
      goto select_end;
    }
    sAggInfo.nAccumulator = sAggInfo.nColumn;
    for(i=0; i<sAggInfo.nFunc; i++){
      if( sqlite3ExprAnalyzeAggList(&sNC, sAggInfo.aFunc[i].pExpr->pList) ){
        goto select_end;
      }
    }
    if( sqlite3MallocFailed() ) goto select_end;

    /* Processing for aggregates with GROUP BY is very different and
    ** much more complex tha aggregates without a GROUP BY.
    */
    if( pGroupBy ){
      KeyInfo *pKeyInfo;  /* Keying information for the group by clause */

      /* Create labels that we will be needing
      */
     
      addrInitializeLoop = sqlite3VdbeMakeLabel(v);
      addrGroupByChange = sqlite3VdbeMakeLabel(v);
      addrProcessRow = sqlite3VdbeMakeLabel(v);

      /* If there is a GROUP BY clause we might need a sorting index to
      ** implement it.  Allocate that sorting index now.  If it turns out
      ** that we do not need it after all, the OpenEphemeral instruction
      ** will be converted into a Noop.  
      */
      sAggInfo.sortingIdx = pParse->nTab++;
      pKeyInfo = keyInfoFromExprList(pParse, pGroupBy);
      addrSortingIdx =
          sqlite3VdbeOp3(v, OP_OpenEphemeral, sAggInfo.sortingIdx,
                         sAggInfo.nSortingColumn,
                         (char*)pKeyInfo, P3_KEYINFO_HANDOFF);

      /* Initialize memory locations used by GROUP BY aggregate processing
      */
      iUseFlag = pParse->nMem++;
      iAbortFlag = pParse->nMem++;
      iAMem = pParse->nMem;
      pParse->nMem += pGroupBy->nExpr;
      iBMem = pParse->nMem;
      pParse->nMem += pGroupBy->nExpr;
      sqlite3VdbeAddOp(v, OP_MemInt, 0, iAbortFlag);
      VdbeComment((v, "# clear abort flag"));
      sqlite3VdbeAddOp(v, OP_MemInt, 0, iUseFlag);
      VdbeComment((v, "# indicate accumulator empty"));
      sqlite3VdbeAddOp(v, OP_Goto, 0, addrInitializeLoop);

      /* Generate a subroutine that outputs a single row of the result
      ** set.  This subroutine first looks at the iUseFlag.  If iUseFlag
      ** is less than or equal to zero, the subroutine is a no-op.  If
      ** the processing calls for the query to abort, this subroutine
      ** increments the iAbortFlag memory location before returning in
      ** order to signal the caller to abort.
      */
      addrSetAbort = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_MemInt, 1, iAbortFlag);
      VdbeComment((v, "# set abort flag"));
      sqlite3VdbeAddOp(v, OP_Return, 0, 0);
      addrOutputRow = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_IfMemPos, iUseFlag, addrOutputRow+2);
      VdbeComment((v, "# Groupby result generator entry point"));
      sqlite3VdbeAddOp(v, OP_Return, 0, 0);
      finalizeAggFunctions(pParse, &sAggInfo);
      if( pHaving ){
        sqlite3ExprIfFalse(pParse, pHaving, addrOutputRow+1, 1);
      }
      rc = selectInnerLoop(pParse, p, p->pEList, 0, 0, pOrderBy,
                           distinct, eDest, iParm, 
                           addrOutputRow+1, addrSetAbort, aff);
      if( rc ){
        goto select_end;
      }
      sqlite3VdbeAddOp(v, OP_Return, 0, 0);
      VdbeComment((v, "# end groupby result generator"));

      /* Generate a subroutine that will reset the group-by accumulator
      */
      addrReset = sqlite3VdbeCurrentAddr(v);
      resetAccumulator(pParse, &sAggInfo);
      sqlite3VdbeAddOp(v, OP_Return, 0, 0);

      /* Begin a loop that will extract all source rows in GROUP BY order.
      ** This might involve two separate loops with an OP_Sort in between, or
      ** it might be a single loop that uses an index to extract information
      ** in the right order to begin with.
      */
      sqlite3VdbeResolveLabel(v, addrInitializeLoop);
      sqlite3VdbeAddOp(v, OP_Gosub, 0, addrReset);
      pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, &pGroupBy);
      if( pWInfo==0 ) goto select_end;
      if( pGroupBy==0 ){
        /* The optimizer is able to deliver rows in group by order so
        ** we do not have to sort.  The OP_OpenEphemeral table will be
        ** cancelled later because we still need to use the pKeyInfo
        */
        pGroupBy = p->pGroupBy;
        groupBySort = 0;
      }else{
        /* Rows are coming out in undetermined order.  We have to push
        ** each row into a sorting index, terminate the first loop,
        ** then loop over the sorting index in order to get the output
        ** in sorted order
        */
        groupBySort = 1;
        sqlite3ExprCodeExprList(pParse, pGroupBy);
        sqlite3VdbeAddOp(v, OP_Sequence, sAggInfo.sortingIdx, 0);
        j = pGroupBy->nExpr+1;
        for(i=0; i<sAggInfo.nColumn; i++){
          struct AggInfo_col *pCol = &sAggInfo.aCol[i];
          if( pCol->iSorterColumn<j ) continue;
          sqlite3ExprCodeGetColumn(v, pCol->pTab, pCol->iColumn, pCol->iTable);
          j++;
        }
        sqlite3VdbeAddOp(v, OP_MakeRecord, j, 0);
        sqlite3VdbeAddOp(v, OP_IdxInsert, sAggInfo.sortingIdx, 0);
        sqlite3WhereEnd(pWInfo);
        sqlite3VdbeAddOp(v, OP_Sort, sAggInfo.sortingIdx, addrEnd);
        VdbeComment((v, "# GROUP BY sort"));
        sAggInfo.useSortingIdx = 1;
      }

      /* Evaluate the current GROUP BY terms and store in b0, b1, b2...
      ** (b0 is memory location iBMem+0, b1 is iBMem+1, and so forth)
      ** Then compare the current GROUP BY terms against the GROUP BY terms
      ** from the previous row currently stored in a0, a1, a2...
      */
      addrTopOfLoop = sqlite3VdbeCurrentAddr(v);
      for(j=0; j<pGroupBy->nExpr; j++){
        if( groupBySort ){
          sqlite3VdbeAddOp(v, OP_Column, sAggInfo.sortingIdx, j);
        }else{
          sAggInfo.directMode = 1;
          sqlite3ExprCode(pParse, pGroupBy->a[j].pExpr);
        }
        sqlite3VdbeAddOp(v, OP_MemStore, iBMem+j, j<pGroupBy->nExpr-1);
      }
      for(j=pGroupBy->nExpr-1; j>=0; j--){
        if( j<pGroupBy->nExpr-1 ){
          sqlite3VdbeAddOp(v, OP_MemLoad, iBMem+j, 0);
        }
        sqlite3VdbeAddOp(v, OP_MemLoad, iAMem+j, 0);
        if( j==0 ){
          sqlite3VdbeAddOp(v, OP_Eq, 0x200, addrProcessRow);
        }else{
          sqlite3VdbeAddOp(v, OP_Ne, 0x200, addrGroupByChange);
        }
        sqlite3VdbeChangeP3(v, -1, (void*)pKeyInfo->aColl[j], P3_COLLSEQ);
      }

      /* Generate code that runs whenever the GROUP BY changes.
      ** Change in the GROUP BY are detected by the previous code
      ** block.  If there were no changes, this block is skipped.
      **
      ** This code copies current group by terms in b0,b1,b2,...
      ** over to a0,a1,a2.  It then calls the output subroutine
      ** and resets the aggregate accumulator registers in preparation
      ** for the next GROUP BY batch.
      */
      sqlite3VdbeResolveLabel(v, addrGroupByChange);
      for(j=0; j<pGroupBy->nExpr; j++){
        sqlite3VdbeAddOp(v, OP_MemMove, iAMem+j, iBMem+j);
      }
      sqlite3VdbeAddOp(v, OP_Gosub, 0, addrOutputRow);
      VdbeComment((v, "# output one row"));
      sqlite3VdbeAddOp(v, OP_IfMemPos, iAbortFlag, addrEnd);
      VdbeComment((v, "# check abort flag"));
      sqlite3VdbeAddOp(v, OP_Gosub, 0, addrReset);
      VdbeComment((v, "# reset accumulator"));

      /* Update the aggregate accumulators based on the content of
      ** the current row
      */
      sqlite3VdbeResolveLabel(v, addrProcessRow);
      updateAccumulator(pParse, &sAggInfo);
      sqlite3VdbeAddOp(v, OP_MemInt, 1, iUseFlag);
      VdbeComment((v, "# indicate data in accumulator"));

      /* End of the loop
      */
      if( groupBySort ){
        sqlite3VdbeAddOp(v, OP_Next, sAggInfo.sortingIdx, addrTopOfLoop);
      }else{
        sqlite3WhereEnd(pWInfo);
        sqlite3VdbeChangeToNoop(v, addrSortingIdx, 1);
      }

      /* Output the final row of result
      */
      sqlite3VdbeAddOp(v, OP_Gosub, 0, addrOutputRow);
      VdbeComment((v, "# output final row"));
      
    } /* endif pGroupBy */
    else {
      /* This case runs if the aggregate has no GROUP BY clause.  The
      ** processing is much simpler since there is only a single row
      ** of output.
      */
      resetAccumulator(pParse, &sAggInfo);
      pWInfo = sqlite3WhereBegin(pParse, pTabList, pWhere, 0);
      if( pWInfo==0 ) goto select_end;
      updateAccumulator(pParse, &sAggInfo);
      sqlite3WhereEnd(pWInfo);
      finalizeAggFunctions(pParse, &sAggInfo);
      pOrderBy = 0;
      if( pHaving ){
        sqlite3ExprIfFalse(pParse, pHaving, addrEnd, 1);
      }
      selectInnerLoop(pParse, p, p->pEList, 0, 0, 0, -1, 
                      eDest, iParm, addrEnd, addrEnd, aff);
    }
    sqlite3VdbeResolveLabel(v, addrEnd);
    
  } /* endif aggregate query */

  /* If there is an ORDER BY clause, then we need to sort the results
  ** and send them to the callback one by one.
  */
  if( pOrderBy ){
    generateSortTail(pParse, p, v, pEList->nExpr, eDest, iParm);
  }

#ifndef SQLITE_OMIT_SUBQUERY
  /* If this was a subquery, we have now converted the subquery into a
  ** temporary table.  So set the SrcList_item.isPopulated flag to prevent
  ** this subquery from being evaluated again and to force the use of
  ** the temporary table.
  */
  if( pParent ){
    assert( pParent->pSrc->nSrc>parentTab );
    assert( pParent->pSrc->a[parentTab].pSelect==p );
    pParent->pSrc->a[parentTab].isPopulated = 1;
  }
#endif

  /* Jump here to skip this query
  */
  sqlite3VdbeResolveLabel(v, iEnd);

  /* The SELECT was successfully coded.   Set the return code to 0
  ** to indicate no errors.
  */
  rc = 0;

  /* Control jumps to here if an error is encountered above, or upon
  ** successful coding of the SELECT.
  */
select_end:

  /* Identify column names if we will be using them in a callback.  This
  ** step is skipped if the output is going to some other destination.
  */
  if( rc==SQLITE_OK && eDest==SRT_Callback ){
    generateColumnNames(pParse, pTabList, pEList);
  }

  sqliteFree(sAggInfo.aCol);
  sqliteFree(sAggInfo.aFunc);
  return rc;
}

#if defined(SQLITE_DEBUG)
/*
*******************************************************************************
** The following code is used for testing and debugging only.  The code
** that follows does not appear in normal builds.
**
** These routines are used to print out the content of all or part of a 
** parse structures such as Select or Expr.  Such printouts are useful
** for helping to understand what is happening inside the code generator
** during the execution of complex SELECT statements.
**
** These routine are not called anywhere from within the normal
** code base.  Then are intended to be called from within the debugger
** or from temporary "printf" statements inserted for debugging.
*/
void sqlite3PrintExpr(Expr *p){
  if( p->token.z && p->token.n>0 ){
    sqlite3DebugPrintf("(%.*s", p->token.n, p->token.z);
  }else{
    sqlite3DebugPrintf("(%d", p->op);
  }
  if( p->pLeft ){
    sqlite3DebugPrintf(" ");
    sqlite3PrintExpr(p->pLeft);
  }
  if( p->pRight ){
    sqlite3DebugPrintf(" ");
    sqlite3PrintExpr(p->pRight);
  }
  sqlite3DebugPrintf(")");
}
void sqlite3PrintExprList(ExprList *pList){
  int i;
  for(i=0; i<pList->nExpr; i++){
    sqlite3PrintExpr(pList->a[i].pExpr);
    if( i<pList->nExpr-1 ){
      sqlite3DebugPrintf(", ");
    }
  }
}
void sqlite3PrintSelect(Select *p, int indent){
  sqlite3DebugPrintf("%*sSELECT(%p) ", indent, "", p);
  sqlite3PrintExprList(p->pEList);
  sqlite3DebugPrintf("\n");
  if( p->pSrc ){
    char *zPrefix;
    int i;
    zPrefix = "FROM";
    for(i=0; i<p->pSrc->nSrc; i++){
      struct SrcList_item *pItem = &p->pSrc->a[i];
      sqlite3DebugPrintf("%*s ", indent+6, zPrefix);
      zPrefix = "";
      if( pItem->pSelect ){
        sqlite3DebugPrintf("(\n");
        sqlite3PrintSelect(pItem->pSelect, indent+10);
        sqlite3DebugPrintf("%*s)", indent+8, "");
      }else if( pItem->zName ){
        sqlite3DebugPrintf("%s", pItem->zName);
      }
      if( pItem->pTab ){
        sqlite3DebugPrintf("(table: %s)", pItem->pTab->zName);
      }
      if( pItem->zAlias ){
        sqlite3DebugPrintf(" AS %s", pItem->zAlias);
      }
      if( i<p->pSrc->nSrc-1 ){
        sqlite3DebugPrintf(",");
      }
      sqlite3DebugPrintf("\n");
    }
  }
  if( p->pWhere ){
    sqlite3DebugPrintf("%*s WHERE ", indent, "");
    sqlite3PrintExpr(p->pWhere);
    sqlite3DebugPrintf("\n");
  }
  if( p->pGroupBy ){
    sqlite3DebugPrintf("%*s GROUP BY ", indent, "");
    sqlite3PrintExprList(p->pGroupBy);
    sqlite3DebugPrintf("\n");
  }
  if( p->pHaving ){
    sqlite3DebugPrintf("%*s HAVING ", indent, "");
    sqlite3PrintExpr(p->pHaving);
    sqlite3DebugPrintf("\n");
  }
  if( p->pOrderBy ){
    sqlite3DebugPrintf("%*s ORDER BY ", indent, "");
    sqlite3PrintExprList(p->pOrderBy);
    sqlite3DebugPrintf("\n");
  }
}
/* End of the structure debug printing code
*****************************************************************************/
#endif /* defined(SQLITE_TEST) || defined(SQLITE_DEBUG) */
