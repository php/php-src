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
** This module contains C code that generates VDBE code used to process
** the WHERE clause of SQL statements.
**
** $Id$
*/
#include "sqliteInt.h"

/*
** The query generator uses an array of instances of this structure to
** help it analyze the subexpressions of the WHERE clause.  Each WHERE
** clause subexpression is separated from the others by an AND operator.
*/
typedef struct ExprInfo ExprInfo;
struct ExprInfo {
  Expr *p;                /* Pointer to the subexpression */
  u8 indexable;           /* True if this subexprssion is usable by an index */
  short int idxLeft;      /* p->pLeft is a column in this table number. -1 if
                          ** p->pLeft is not the column of any table */
  short int idxRight;     /* p->pRight is a column in this table number. -1 if
                          ** p->pRight is not the column of any table */
  unsigned prereqLeft;    /* Bitmask of tables referenced by p->pLeft */
  unsigned prereqRight;   /* Bitmask of tables referenced by p->pRight */
  unsigned prereqAll;     /* Bitmask of tables referenced by p */
};

/*
** An instance of the following structure keeps track of a mapping
** between VDBE cursor numbers and bitmasks.  The VDBE cursor numbers
** are small integers contained in SrcList_item.iCursor and Expr.iTable
** fields.  For any given WHERE clause, we want to track which cursors
** are being used, so we assign a single bit in a 32-bit word to track
** that cursor.  Then a 32-bit integer is able to show the set of all
** cursors being used.
*/
typedef struct ExprMaskSet ExprMaskSet;
struct ExprMaskSet {
  int n;          /* Number of assigned cursor values */
  int ix[31];     /* Cursor assigned to each bit */
};

/*
** Determine the number of elements in an array.
*/
#define ARRAYSIZE(X)  (sizeof(X)/sizeof(X[0]))

/*
** This routine is used to divide the WHERE expression into subexpressions
** separated by the AND operator.
**
** aSlot[] is an array of subexpressions structures.
** There are nSlot spaces left in this array.  This routine attempts to
** split pExpr into subexpressions and fills aSlot[] with those subexpressions.
** The return value is the number of slots filled.
*/
static int exprSplit(int nSlot, ExprInfo *aSlot, Expr *pExpr){
  int cnt = 0;
  if( pExpr==0 || nSlot<1 ) return 0;
  if( nSlot==1 || pExpr->op!=TK_AND ){
    aSlot[0].p = pExpr;
    return 1;
  }
  if( pExpr->pLeft->op!=TK_AND ){
    aSlot[0].p = pExpr->pLeft;
    cnt = 1 + exprSplit(nSlot-1, &aSlot[1], pExpr->pRight);
  }else{
    cnt = exprSplit(nSlot, aSlot, pExpr->pLeft);
    cnt += exprSplit(nSlot-cnt, &aSlot[cnt], pExpr->pRight);
  }
  return cnt;
}

/*
** Initialize an expression mask set
*/
#define initMaskSet(P)  memset(P, 0, sizeof(*P))

/*
** Return the bitmask for the given cursor.  Assign a new bitmask
** if this is the first time the cursor has been seen.
*/
static int getMask(ExprMaskSet *pMaskSet, int iCursor){
  int i;
  for(i=0; i<pMaskSet->n; i++){
    if( pMaskSet->ix[i]==iCursor ) return 1<<i;
  }
  if( i==pMaskSet->n && i<ARRAYSIZE(pMaskSet->ix) ){
    pMaskSet->n++;
    pMaskSet->ix[i] = iCursor;
    return 1<<i;
  }
  return 0;
}

/*
** Destroy an expression mask set
*/
#define freeMaskSet(P)   /* NO-OP */

/*
** This routine walks (recursively) an expression tree and generates
** a bitmask indicating which tables are used in that expression
** tree.
**
** In order for this routine to work, the calling function must have
** previously invoked sqlite3ExprResolveIds() on the expression.  See
** the header comment on that routine for additional information.
** The sqlite3ExprResolveIds() routines looks for column names and
** sets their opcodes to TK_COLUMN and their Expr.iTable fields to
** the VDBE cursor number of the table.
*/
static int exprTableUsage(ExprMaskSet *pMaskSet, Expr *p){
  unsigned int mask = 0;
  if( p==0 ) return 0;
  if( p->op==TK_COLUMN ){
    mask = getMask(pMaskSet, p->iTable);
    if( mask==0 ) mask = -1;
    return mask;
  }
  if( p->pRight ){
    mask = exprTableUsage(pMaskSet, p->pRight);
  }
  if( p->pLeft ){
    mask |= exprTableUsage(pMaskSet, p->pLeft);
  }
  if( p->pList ){
    int i;
    for(i=0; i<p->pList->nExpr; i++){
      mask |= exprTableUsage(pMaskSet, p->pList->a[i].pExpr);
    }
  }
  return mask;
}

/*
** Return TRUE if the given operator is one of the operators that is
** allowed for an indexable WHERE clause.  The allowed operators are
** "=", "<", ">", "<=", ">=", and "IN".
*/
static int allowedOp(int op){
  assert( TK_GT==TK_LE-1 && TK_LE==TK_LT-1 && TK_LT==TK_GE-1 && TK_EQ==TK_GT-1);
  return op==TK_IN || (op>=TK_EQ && op<=TK_GE);
}

/*
** Swap two integers.
*/
#define SWAP(TYPE,A,B) {TYPE t=A; A=B; B=t;}

/*
** Return the index in the SrcList that uses cursor iCur.  If iCur is
** used by the first entry in SrcList return 0.  If iCur is used by
** the second entry return 1.  And so forth.
**
** SrcList is the set of tables in the FROM clause in the order that
** they will be processed.  The value returned here gives us an index
** of which tables will be processed first.
*/
static int tableOrder(SrcList *pList, int iCur){
  int i;
  for(i=0; i<pList->nSrc; i++){
    if( pList->a[i].iCursor==iCur ) return i;
  }
  return -1;
}

/*
** The input to this routine is an ExprInfo structure with only the
** "p" field filled in.  The job of this routine is to analyze the
** subexpression and populate all the other fields of the ExprInfo
** structure.
*/
static void exprAnalyze(SrcList *pSrc, ExprMaskSet *pMaskSet, ExprInfo *pInfo){
  Expr *pExpr = pInfo->p;
  pInfo->prereqLeft = exprTableUsage(pMaskSet, pExpr->pLeft);
  pInfo->prereqRight = exprTableUsage(pMaskSet, pExpr->pRight);
  pInfo->prereqAll = exprTableUsage(pMaskSet, pExpr);
  pInfo->indexable = 0;
  pInfo->idxLeft = -1;
  pInfo->idxRight = -1;
  if( allowedOp(pExpr->op) && (pInfo->prereqRight & pInfo->prereqLeft)==0 ){
    if( pExpr->pRight && pExpr->pRight->op==TK_COLUMN ){
      pInfo->idxRight = pExpr->pRight->iTable;
      pInfo->indexable = 1;
    }
    if( pExpr->pLeft->op==TK_COLUMN ){
      pInfo->idxLeft = pExpr->pLeft->iTable;
      pInfo->indexable = 1;
    }
  }
  if( pInfo->indexable ){
    assert( pInfo->idxLeft!=pInfo->idxRight );

    /* We want the expression to be of the form "X = expr", not "expr = X".
    ** So flip it over if necessary.  If the expression is "X = Y", then
    ** we want Y to come from an earlier table than X.
    **
    ** The collating sequence rule is to always choose the left expression.
    ** So if we do a flip, we also have to move the collating sequence.
    */
    if( tableOrder(pSrc,pInfo->idxLeft)<tableOrder(pSrc,pInfo->idxRight) ){
      assert( pExpr->op!=TK_IN );
      SWAP(CollSeq*,pExpr->pRight->pColl,pExpr->pLeft->pColl);
      SWAP(Expr*,pExpr->pRight,pExpr->pLeft);
      if( pExpr->op>=TK_GT ){
        assert( TK_LT==TK_GT+2 );
        assert( TK_GE==TK_LE+2 );
        assert( TK_GT>TK_EQ );
        assert( TK_GT<TK_LE );
        assert( pExpr->op>=TK_GT && pExpr->op<=TK_GE );
        pExpr->op = ((pExpr->op-TK_GT)^2)+TK_GT;
      }
      SWAP(unsigned, pInfo->prereqLeft, pInfo->prereqRight);
      SWAP(short int, pInfo->idxLeft, pInfo->idxRight);
    }
  }      

}

/*
** pOrderBy is an ORDER BY clause from a SELECT statement.  pTab is the
** left-most table in the FROM clause of that same SELECT statement and
** the table has a cursor number of "base".
**
** This routine attempts to find an index for pTab that generates the
** correct record sequence for the given ORDER BY clause.  The return value
** is a pointer to an index that does the job.  NULL is returned if the
** table has no index that will generate the correct sort order.
**
** If there are two or more indices that generate the correct sort order
** and pPreferredIdx is one of those indices, then return pPreferredIdx.
**
** nEqCol is the number of columns of pPreferredIdx that are used as
** equality constraints.  Any index returned must have exactly this same
** set of columns.  The ORDER BY clause only matches index columns beyond the
** the first nEqCol columns.
**
** All terms of the ORDER BY clause must be either ASC or DESC.  The
** *pbRev value is set to 1 if the ORDER BY clause is all DESC and it is
** set to 0 if the ORDER BY clause is all ASC.
*/
static Index *findSortingIndex(
  Parse *pParse,
  Table *pTab,            /* The table to be sorted */
  int base,               /* Cursor number for pTab */
  ExprList *pOrderBy,     /* The ORDER BY clause */
  Index *pPreferredIdx,   /* Use this index, if possible and not NULL */
  int nEqCol,             /* Number of index columns used with == constraints */
  int *pbRev              /* Set to 1 if ORDER BY is DESC */
){
  int i, j;
  Index *pMatch;
  Index *pIdx;
  int sortOrder;
  sqlite3 *db = pParse->db;

  assert( pOrderBy!=0 );
  assert( pOrderBy->nExpr>0 );
  sortOrder = pOrderBy->a[0].sortOrder;
  for(i=0; i<pOrderBy->nExpr; i++){
    Expr *p;
    if( pOrderBy->a[i].sortOrder!=sortOrder ){
      /* Indices can only be used if all ORDER BY terms are either
      ** DESC or ASC.  Indices cannot be used on a mixture. */
      return 0;
    }
    p = pOrderBy->a[i].pExpr;
    if( p->op!=TK_COLUMN || p->iTable!=base ){
      /* Can not use an index sort on anything that is not a column in the
      ** left-most table of the FROM clause */
      return 0;
    }
  }

  /* If we get this far, it means the ORDER BY clause consists only of
  ** ascending columns in the left-most table of the FROM clause.  Now
  ** check for a matching index.
  */
  pMatch = 0;
  for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
    int nExpr = pOrderBy->nExpr;
    if( pIdx->nColumn < nEqCol || pIdx->nColumn < nExpr ) continue;
    for(i=j=0; i<nEqCol; i++){
      CollSeq *pColl = sqlite3ExprCollSeq(pParse, pOrderBy->a[j].pExpr);
      if( !pColl ) pColl = db->pDfltColl;
      if( pPreferredIdx->aiColumn[i]!=pIdx->aiColumn[i] ) break;
      if( pPreferredIdx->keyInfo.aColl[i]!=pIdx->keyInfo.aColl[i] ) break;
      if( j<nExpr && 
          pOrderBy->a[j].pExpr->iColumn==pIdx->aiColumn[i] &&
          pColl==pIdx->keyInfo.aColl[i]
      ){ 
        j++; 
      }
    }
    if( i<nEqCol ) continue;
    for(i=0; i+j<nExpr; i++){
      CollSeq *pColl = sqlite3ExprCollSeq(pParse, pOrderBy->a[i+j].pExpr);
      if( !pColl ) pColl = db->pDfltColl;
      if( pOrderBy->a[i+j].pExpr->iColumn!=pIdx->aiColumn[i+nEqCol] ||
          pColl!=pIdx->keyInfo.aColl[i+nEqCol] ) break;
    }
    if( i+j>=nExpr ){
      pMatch = pIdx;
      if( pIdx==pPreferredIdx ) break;
    }
  }
  if( pMatch && pbRev ){
    *pbRev = sortOrder==SQLITE_SO_DESC;
  }
  return pMatch;
}

/*
** Disable a term in the WHERE clause.  Except, do not disable the term
** if it controls a LEFT OUTER JOIN and it did not originate in the ON
** or USING clause of that join.
**
** Consider the term t2.z='ok' in the following queries:
**
**   (1)  SELECT * FROM t1 LEFT JOIN t2 ON t1.a=t2.x WHERE t2.z='ok'
**   (2)  SELECT * FROM t1 LEFT JOIN t2 ON t1.a=t2.x AND t2.z='ok'
**   (3)  SELECT * FROM t1, t2 WHERE t1.a=t2.x AND t2.z='ok'
**
** The t2.z='ok' is disabled in the in (2) because it did not originate
** in the ON clause.  The term is disabled in (3) because it is not part
** of a LEFT OUTER JOIN.  In (1), the term is not disabled.
**
** Disabling a term causes that term to not be tested in the inner loop
** of the join.  Disabling is an optimization.  We would get the correct
** results if nothing were ever disabled, but joins might run a little
** slower.  The trick is to disable as much as we can without disabling
** too much.  If we disabled in (1), we'd get the wrong answer.
** See ticket #813.
*/
static void disableTerm(WhereLevel *pLevel, Expr **ppExpr){
  Expr *pExpr = *ppExpr;
  if( pLevel->iLeftJoin==0 || ExprHasProperty(pExpr, EP_FromJoin) ){
    *ppExpr = 0;
  }
}

/*
** Generate code that builds a probe for an index.  Details:
**
**    *  Check the top nColumn entries on the stack.  If any
**       of those entries are NULL, jump immediately to brk,
**       which is the loop exit, since no index entry will match
**       if any part of the key is NULL.
**
**    *  Construct a probe entry from the top nColumn entries in
**       the stack with affinities appropriate for index pIdx.
*/
static void buildIndexProbe(Vdbe *v, int nColumn, int brk, Index *pIdx){
  sqlite3VdbeAddOp(v, OP_NotNull, -nColumn, sqlite3VdbeCurrentAddr(v)+3);
  sqlite3VdbeAddOp(v, OP_Pop, nColumn, 0);
  sqlite3VdbeAddOp(v, OP_Goto, 0, brk);
  sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
  sqlite3IndexAffinityStr(v, pIdx);
}

/*
** Generate code for an equality term of the WHERE clause.  An equality
** term can be either X=expr  or X IN (...).   pTerm is the X.  
*/
static void codeEqualityTerm(
  Parse *pParse,      /* The parsing context */
  ExprInfo *pTerm,    /* The term of the WHERE clause to be coded */
  int brk,            /* Jump here to abandon the loop */
  WhereLevel *pLevel  /* When level of the FROM clause we are working on */
){
  Expr *pX = pTerm->p;
  if( pX->op!=TK_IN ){
    assert( pX->op==TK_EQ );
    sqlite3ExprCode(pParse, pX->pRight);
  }else{
    int iTab = pX->iTable;
    Vdbe *v = pParse->pVdbe;
    sqlite3VdbeAddOp(v, OP_Rewind, iTab, brk);
    sqlite3VdbeAddOp(v, OP_KeyAsData, iTab, 1);
    pLevel->inP2 = sqlite3VdbeAddOp(v, OP_IdxColumn, iTab, 0);
    pLevel->inOp = OP_Next;
    pLevel->inP1 = iTab;
  }
  disableTerm(pLevel, &pTerm->p);
}


/*
** Generate the beginning of the loop used for WHERE clause processing.
** The return value is a pointer to an (opaque) structure that contains
** information needed to terminate the loop.  Later, the calling routine
** should invoke sqlite3WhereEnd() with the return value of this function
** in order to complete the WHERE clause processing.
**
** If an error occurs, this routine returns NULL.
**
** The basic idea is to do a nested loop, one loop for each table in
** the FROM clause of a select.  (INSERT and UPDATE statements are the
** same as a SELECT with only a single table in the FROM clause.)  For
** example, if the SQL is this:
**
**       SELECT * FROM t1, t2, t3 WHERE ...;
**
** Then the code generated is conceptually like the following:
**
**      foreach row1 in t1 do       \    Code generated
**        foreach row2 in t2 do      |-- by sqlite3WhereBegin()
**          foreach row3 in t3 do   /
**            ...
**          end                     \    Code generated
**        end                        |-- by sqlite3WhereEnd()
**      end                         /
**
** There are Btree cursors associated with each table.  t1 uses cursor
** number pTabList->a[0].iCursor.  t2 uses the cursor pTabList->a[1].iCursor.
** And so forth.  This routine generates code to open those VDBE cursors
** and sqlite3WhereEnd() generates the code to close them.
**
** If the WHERE clause is empty, the foreach loops must each scan their
** entire tables.  Thus a three-way join is an O(N^3) operation.  But if
** the tables have indices and there are terms in the WHERE clause that
** refer to those indices, a complete table scan can be avoided and the
** code will run much faster.  Most of the work of this routine is checking
** to see if there are indices that can be used to speed up the loop.
**
** Terms of the WHERE clause are also used to limit which rows actually
** make it to the "..." in the middle of the loop.  After each "foreach",
** terms of the WHERE clause that use only terms in that loop and outer
** loops are evaluated and if false a jump is made around all subsequent
** inner loops (or around the "..." if the test occurs within the inner-
** most loop)
**
** OUTER JOINS
**
** An outer join of tables t1 and t2 is conceptally coded as follows:
**
**    foreach row1 in t1 do
**      flag = 0
**      foreach row2 in t2 do
**        start:
**          ...
**          flag = 1
**      end
**      if flag==0 then
**        move the row2 cursor to a null row
**        goto start
**      fi
**    end
**
** ORDER BY CLAUSE PROCESSING
**
** *ppOrderBy is a pointer to the ORDER BY clause of a SELECT statement,
** if there is one.  If there is no ORDER BY clause or if this routine
** is called from an UPDATE or DELETE statement, then ppOrderBy is NULL.
**
** If an index can be used so that the natural output order of the table
** scan is correct for the ORDER BY clause, then that index is used and
** *ppOrderBy is set to NULL.  This is an optimization that prevents an
** unnecessary sort of the result set if an index appropriate for the
** ORDER BY clause already exists.
**
** If the where clause loops cannot be arranged to provide the correct
** output order, then the *ppOrderBy is unchanged.
*/
WhereInfo *sqlite3WhereBegin(
  Parse *pParse,       /* The parser context */
  SrcList *pTabList,   /* A list of all tables to be scanned */
  Expr *pWhere,        /* The WHERE clause */
  int pushKey,         /* If TRUE, leave the table key on the stack */
  ExprList **ppOrderBy /* An ORDER BY clause, or NULL */
){
  int i;                     /* Loop counter */
  WhereInfo *pWInfo;         /* Will become the return value of this function */
  Vdbe *v = pParse->pVdbe;   /* The virtual database engine */
  int brk, cont = 0;         /* Addresses used during code generation */
  int nExpr;           /* Number of subexpressions in the WHERE clause */
  int loopMask;        /* One bit set for each outer loop */
  int haveKey = 0;     /* True if KEY is on the stack */
  ExprInfo *pTerm;     /* A single term in the WHERE clause; ptr to aExpr[] */
  ExprMaskSet maskSet; /* The expression mask set */
  int iDirectEq[32];   /* Term of the form ROWID==X for the N-th table */
  int iDirectLt[32];   /* Term of the form ROWID<X or ROWID<=X */
  int iDirectGt[32];   /* Term of the form ROWID>X or ROWID>=X */
  ExprInfo aExpr[101]; /* The WHERE clause is divided into these terms */

  /* pushKey is only allowed if there is a single table (as in an INSERT or
  ** UPDATE statement)
  */
  assert( pushKey==0 || pTabList->nSrc==1 );

  /* Split the WHERE clause into separate subexpressions where each
  ** subexpression is separated by an AND operator.  If the aExpr[]
  ** array fills up, the last entry might point to an expression which
  ** contains additional unfactored AND operators.
  */
  initMaskSet(&maskSet);
  memset(aExpr, 0, sizeof(aExpr));
  nExpr = exprSplit(ARRAYSIZE(aExpr), aExpr, pWhere);
  if( nExpr==ARRAYSIZE(aExpr) ){
    sqlite3ErrorMsg(pParse, "WHERE clause too complex - no more "
       "than %d terms allowed", (int)ARRAYSIZE(aExpr)-1);
    return 0;
  }
  
  /* Allocate and initialize the WhereInfo structure that will become the
  ** return value.
  */
  pWInfo = sqliteMalloc( sizeof(WhereInfo) + pTabList->nSrc*sizeof(WhereLevel));
  if( sqlite3_malloc_failed ){
    /* sqliteFree(pWInfo); // Leak memory when malloc fails */
    return 0;
  }
  pWInfo->pParse = pParse;
  pWInfo->pTabList = pTabList;
  pWInfo->iBreak = sqlite3VdbeMakeLabel(v);

  /* Special case: a WHERE clause that is constant.  Evaluate the
  ** expression and either jump over all of the code or fall thru.
  */
  if( pWhere && (pTabList->nSrc==0 || sqlite3ExprIsConstant(pWhere)) ){
    sqlite3ExprIfFalse(pParse, pWhere, pWInfo->iBreak, 1);
    pWhere = 0;
  }

  /* Analyze all of the subexpressions.
  */
  for(pTerm=aExpr, i=0; i<nExpr; i++, pTerm++){
    TriggerStack *pStack;
    exprAnalyze(pTabList, &maskSet, pTerm);

    /* If we are executing a trigger body, remove all references to
    ** new.* and old.* tables from the prerequisite masks.
    */
    if( (pStack = pParse->trigStack)!=0 ){
      int x;
      if( (x=pStack->newIdx) >= 0 ){
        int mask = ~getMask(&maskSet, x);
        pTerm->prereqRight &= mask;
        pTerm->prereqLeft &= mask;
        pTerm->prereqAll &= mask;
      }
      if( (x=pStack->oldIdx) >= 0 ){
        int mask = ~getMask(&maskSet, x);
        pTerm->prereqRight &= mask;
        pTerm->prereqLeft &= mask;
        pTerm->prereqAll &= mask;
      }
    }
  }

  /* Figure out what index to use (if any) for each nested loop.
  ** Make pWInfo->a[i].pIdx point to the index to use for the i-th nested
  ** loop where i==0 is the outer loop and i==pTabList->nSrc-1 is the inner
  ** loop. 
  **
  ** If terms exist that use the ROWID of any table, then set the
  ** iDirectEq[], iDirectLt[], or iDirectGt[] elements for that table
  ** to the index of the term containing the ROWID.  We always prefer
  ** to use a ROWID which can directly access a table rather than an
  ** index which requires reading an index first to get the rowid then
  ** doing a second read of the actual database table.
  **
  ** Actually, if there are more than 32 tables in the join, only the
  ** first 32 tables are candidates for indices.  This is (again) due
  ** to the limit of 32 bits in an integer bitmask.
  */
  loopMask = 0;
  for(i=0; i<pTabList->nSrc && i<ARRAYSIZE(iDirectEq); i++){
    int j;
    WhereLevel *pLevel = &pWInfo->a[i];
    int iCur = pTabList->a[i].iCursor;    /* The cursor for this table */
    int mask = getMask(&maskSet, iCur);   /* Cursor mask for this table */
    Table *pTab = pTabList->a[i].pTab;
    Index *pIdx;
    Index *pBestIdx = 0;
    int bestScore = 0;

    /* Check to see if there is an expression that uses only the
    ** ROWID field of this table.  For terms of the form ROWID==expr
    ** set iDirectEq[i] to the index of the term.  For terms of the
    ** form ROWID<expr or ROWID<=expr set iDirectLt[i] to the term index.
    ** For terms like ROWID>expr or ROWID>=expr set iDirectGt[i].
    **
    ** (Added:) Treat ROWID IN expr like ROWID=expr.
    */
    pLevel->iCur = -1;
    iDirectEq[i] = -1;
    iDirectLt[i] = -1;
    iDirectGt[i] = -1;
    for(pTerm=aExpr, j=0; j<nExpr; j++, pTerm++){
      Expr *pX = pTerm->p;
      if( pTerm->idxLeft==iCur && pX->pLeft->iColumn<0
            && (pTerm->prereqRight & loopMask)==pTerm->prereqRight ){
        switch( pX->op ){
          case TK_IN:
          case TK_EQ: iDirectEq[i] = j; break;
          case TK_LE:
          case TK_LT: iDirectLt[i] = j; break;
          case TK_GE:
          case TK_GT: iDirectGt[i] = j;  break;
        }
      }
    }
    if( iDirectEq[i]>=0 ){
      loopMask |= mask;
      pLevel->pIdx = 0;
      continue;
    }

    /* Do a search for usable indices.  Leave pBestIdx pointing to
    ** the "best" index.  pBestIdx is left set to NULL if no indices
    ** are usable.
    **
    ** The best index is determined as follows.  For each of the
    ** left-most terms that is fixed by an equality operator, add
    ** 8 to the score.  The right-most term of the index may be
    ** constrained by an inequality.  Add 1 if for an "x<..." constraint
    ** and add 2 for an "x>..." constraint.  Chose the index that
    ** gives the best score.
    **
    ** This scoring system is designed so that the score can later be
    ** used to determine how the index is used.  If the score&7 is 0
    ** then all constraints are equalities.  If score&1 is not 0 then
    ** there is an inequality used as a termination key.  (ex: "x<...")
    ** If score&2 is not 0 then there is an inequality used as the
    ** start key.  (ex: "x>...").  A score or 4 is the special case
    ** of an IN operator constraint.  (ex:  "x IN ...").
    **
    ** The IN operator (as in "<expr> IN (...)") is treated the same as
    ** an equality comparison except that it can only be used on the
    ** left-most column of an index and other terms of the WHERE clause
    ** cannot be used in conjunction with the IN operator to help satisfy
    ** other columns of the index.
    */
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      int eqMask = 0;  /* Index columns covered by an x=... term */
      int ltMask = 0;  /* Index columns covered by an x<... term */
      int gtMask = 0;  /* Index columns covered by an x>... term */
      int inMask = 0;  /* Index columns covered by an x IN .. term */
      int nEq, m, score;

      if( pIdx->nColumn>32 ) continue;  /* Ignore indices too many columns */
      for(pTerm=aExpr, j=0; j<nExpr; j++, pTerm++){
        Expr *pX = pTerm->p;
        CollSeq *pColl = sqlite3ExprCollSeq(pParse, pX->pLeft);
        if( !pColl && pX->pRight ){
          pColl = sqlite3ExprCollSeq(pParse, pX->pRight);
        }
        if( !pColl ){
          pColl = pParse->db->pDfltColl;
        }
        if( pTerm->idxLeft==iCur 
             && (pTerm->prereqRight & loopMask)==pTerm->prereqRight ){
          int iColumn = pX->pLeft->iColumn;
          int k;
          char idxaff = pIdx->pTable->aCol[iColumn].affinity; 
          for(k=0; k<pIdx->nColumn; k++){
            /* If the collating sequences or affinities don't match, 
            ** ignore this index.  */
            if( pColl!=pIdx->keyInfo.aColl[k] ) continue;
            if( !sqlite3IndexAffinityOk(pX, idxaff) ) continue;
            if( pIdx->aiColumn[k]==iColumn ){
              switch( pX->op ){
                case TK_IN: {
                  if( k==0 ) inMask |= 1;
                  break;
                }
                case TK_EQ: {
                  eqMask |= 1<<k;
                  break;
                }
                case TK_LE:
                case TK_LT: {
                  ltMask |= 1<<k;
                  break;
                }
                case TK_GE:
                case TK_GT: {
                  gtMask |= 1<<k;
                  break;
                }
                default: {
                  /* CANT_HAPPEN */
                  assert( 0 );
                  break;
                }
              }
              break;
            }
          }
        }
      }

      /* The following loop ends with nEq set to the number of columns
      ** on the left of the index with == constraints.
      */
      for(nEq=0; nEq<pIdx->nColumn; nEq++){
        m = (1<<(nEq+1))-1;
        if( (m & eqMask)!=m ) break;
      }
      score = nEq*8;   /* Base score is 8 times number of == constraints */
      m = 1<<nEq;
      if( m & ltMask ) score++;    /* Increase score for a < constraint */
      if( m & gtMask ) score+=2;   /* Increase score for a > constraint */
      if( score==0 && inMask ) score = 4;  /* Default score for IN constraint */
      if( score>bestScore ){
        pBestIdx = pIdx;
        bestScore = score;
      }
    }
    pLevel->pIdx = pBestIdx;
    pLevel->score = bestScore;
    pLevel->bRev = 0;
    loopMask |= mask;
    if( pBestIdx ){
      pLevel->iCur = pParse->nTab++;
    }
  }

  /* Check to see if the ORDER BY clause is or can be satisfied by the
  ** use of an index on the first table.
  */
  if( ppOrderBy && *ppOrderBy && pTabList->nSrc>0 ){
     Index *pSortIdx;
     Index *pIdx;
     Table *pTab;
     int bRev = 0;

     pTab = pTabList->a[0].pTab;
     pIdx = pWInfo->a[0].pIdx;
     if( pIdx && pWInfo->a[0].score==4 ){
       /* If there is already an IN index on the left-most table,
       ** it will not give the correct sort order.
       ** So, pretend that no suitable index is found.
       */
       pSortIdx = 0;
     }else if( iDirectEq[0]>=0 || iDirectLt[0]>=0 || iDirectGt[0]>=0 ){
       /* If the left-most column is accessed using its ROWID, then do
       ** not try to sort by index.
       */
       pSortIdx = 0;
     }else{
       int nEqCol = (pWInfo->a[0].score+4)/8;
       pSortIdx = findSortingIndex(pParse, pTab, pTabList->a[0].iCursor, 
                                   *ppOrderBy, pIdx, nEqCol, &bRev);
     }
     if( pSortIdx && (pIdx==0 || pIdx==pSortIdx) ){
       if( pIdx==0 ){
         pWInfo->a[0].pIdx = pSortIdx;
         pWInfo->a[0].iCur = pParse->nTab++;
       }
       pWInfo->a[0].bRev = bRev;
       *ppOrderBy = 0;
     }
  }

  /* Open all tables in the pTabList and all indices used by those tables.
  */
  sqlite3CodeVerifySchema(pParse, -1); /* Insert the cookie verifier Goto */
  for(i=0; i<pTabList->nSrc; i++){
    Table *pTab;
    Index *pIx;

    pTab = pTabList->a[i].pTab;
    if( pTab->isTransient || pTab->pSelect ) continue;
    sqlite3OpenTableForReading(v, pTabList->a[i].iCursor, pTab);
    sqlite3CodeVerifySchema(pParse, pTab->iDb);
    if( (pIx = pWInfo->a[i].pIdx)!=0 ){
      sqlite3VdbeAddOp(v, OP_Integer, pIx->iDb, 0);
      sqlite3VdbeOp3(v, OP_OpenRead, pWInfo->a[i].iCur, pIx->tnum,
                     (char*)&pIx->keyInfo, P3_KEYINFO);
    }
  }

  /* Generate the code to do the search
  */
  loopMask = 0;
  for(i=0; i<pTabList->nSrc; i++){
    int j, k;
    int iCur = pTabList->a[i].iCursor;
    Index *pIdx;
    WhereLevel *pLevel = &pWInfo->a[i];

    /* If this is the right table of a LEFT OUTER JOIN, allocate and
    ** initialize a memory cell that records if this table matches any
    ** row of the left table of the join.
    */
    if( i>0 && (pTabList->a[i-1].jointype & JT_LEFT)!=0 ){
      if( !pParse->nMem ) pParse->nMem++;
      pLevel->iLeftJoin = pParse->nMem++;
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iLeftJoin, 1);
      VdbeComment((v, "# init LEFT JOIN no-match flag"));
    }

    pIdx = pLevel->pIdx;
    pLevel->inOp = OP_Noop;
    if( i<ARRAYSIZE(iDirectEq) && (k = iDirectEq[i])>=0 ){
      /* Case 1:  We can directly reference a single row using an
      **          equality comparison against the ROWID field.  Or
      **          we reference multiple rows using a "rowid IN (...)"
      **          construct.
      */
      assert( k<nExpr );
      pTerm = &aExpr[k];
      assert( pTerm->p!=0 );
      assert( pTerm->idxLeft==iCur );
      brk = pLevel->brk = sqlite3VdbeMakeLabel(v);
      codeEqualityTerm(pParse, pTerm, brk, pLevel);
      cont = pLevel->cont = sqlite3VdbeMakeLabel(v);
      sqlite3VdbeAddOp(v, OP_MustBeInt, 1, brk);
      haveKey = 0;
      sqlite3VdbeAddOp(v, OP_NotExists, iCur, brk);
      pLevel->op = OP_Noop;
    }else if( pIdx!=0 && pLevel->score>0 && pLevel->score%4==0 ){
      /* Case 2:  There is an index and all terms of the WHERE clause that
      **          refer to the index use the "==" or "IN" operators.
      */
      int start;
      int nColumn = (pLevel->score+4)/8;
      brk = pLevel->brk = sqlite3VdbeMakeLabel(v);

      /* For each column of the index, find the term of the WHERE clause that
      ** constraints that column.  If the WHERE clause term is X=expr, then
      ** evaluation expr and leave the result on the stack */
      for(j=0; j<nColumn; j++){
        for(pTerm=aExpr, k=0; k<nExpr; k++, pTerm++){
          Expr *pX = pTerm->p;
          if( pX==0 ) continue;
          if( pTerm->idxLeft==iCur
             && (pTerm->prereqRight & loopMask)==pTerm->prereqRight 
             && pX->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            char idxaff = pIdx->pTable->aCol[pX->pLeft->iColumn].affinity;
            if( sqlite3IndexAffinityOk(pX, idxaff) ){
              codeEqualityTerm(pParse, pTerm, brk, pLevel);
              break;
            }
          }
        }
      }
      pLevel->iMem = pParse->nMem++;
      cont = pLevel->cont = sqlite3VdbeMakeLabel(v);
      buildIndexProbe(v, nColumn, brk, pIdx);
      sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 0);

      /* Generate code (1) to move to the first matching element of the table.
      ** Then generate code (2) that jumps to "brk" after the cursor is past
      ** the last matching element of the table.  The code (1) is executed
      ** once to initialize the search, the code (2) is executed before each
      ** iteration of the scan to see if the scan has finished. */
      if( pLevel->bRev ){
        /* Scan in reverse order */
        sqlite3VdbeAddOp(v, OP_MoveLe, pLevel->iCur, brk);
        start = sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, OP_IdxLT, pLevel->iCur, brk);
        pLevel->op = OP_Prev;
      }else{
        /* Scan in the forward order */
        sqlite3VdbeAddOp(v, OP_MoveGe, pLevel->iCur, brk);
        start = sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeOp3(v, OP_IdxGE, pLevel->iCur, brk, "+", P3_STATIC);
        pLevel->op = OP_Next;
      }
      sqlite3VdbeAddOp(v, OP_RowKey, pLevel->iCur, 0);
      sqlite3VdbeAddOp(v, OP_IdxIsNull, nColumn, cont);
      sqlite3VdbeAddOp(v, OP_IdxRecno, pLevel->iCur, 0);
      if( i==pTabList->nSrc-1 && pushKey ){
        haveKey = 1;
      }else{
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
        haveKey = 0;
      }
      pLevel->p1 = pLevel->iCur;
      pLevel->p2 = start;
    }else if( i<ARRAYSIZE(iDirectLt) && (iDirectLt[i]>=0 || iDirectGt[i]>=0) ){
      /* Case 3:  We have an inequality comparison against the ROWID field.
      */
      int testOp = OP_Noop;
      int start;

      brk = pLevel->brk = sqlite3VdbeMakeLabel(v);
      cont = pLevel->cont = sqlite3VdbeMakeLabel(v);
      if( iDirectGt[i]>=0 ){
        Expr *pX;
        k = iDirectGt[i];
        assert( k<nExpr );
        pTerm = &aExpr[k];
        pX = pTerm->p;
        assert( pX!=0 );
        assert( pTerm->idxLeft==iCur );
        sqlite3ExprCode(pParse, pX->pRight);
        sqlite3VdbeAddOp(v, OP_ForceInt, pX->op==TK_LT || pX->op==TK_GT, brk);
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, brk);
        disableTerm(pLevel, &pTerm->p);
      }else{
        sqlite3VdbeAddOp(v, OP_Rewind, iCur, brk);
      }
      if( iDirectLt[i]>=0 ){
        Expr *pX;
        k = iDirectLt[i];
        assert( k<nExpr );
        pTerm = &aExpr[k];
        pX = pTerm->p;
        assert( pX!=0 );
        assert( pTerm->idxLeft==iCur );
        sqlite3ExprCode(pParse, pX->pRight);
        pLevel->iMem = pParse->nMem++;
        sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        if( pX->op==TK_LT || pX->op==TK_GT ){
          testOp = OP_Ge;
        }else{
          testOp = OP_Gt;
        }
        disableTerm(pLevel, &pTerm->p);
      }
      start = sqlite3VdbeCurrentAddr(v);
      pLevel->op = OP_Next;
      pLevel->p1 = iCur;
      pLevel->p2 = start;
      if( testOp!=OP_Noop ){
        sqlite3VdbeAddOp(v, OP_Recno, iCur, 0);
        sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, testOp, 0, brk);
      }
      haveKey = 0;
    }else if( pIdx==0 ){
      /* Case 4:  There is no usable index.  We must do a complete
      **          scan of the entire database table.
      */
      int start;

      brk = pLevel->brk = sqlite3VdbeMakeLabel(v);
      cont = pLevel->cont = sqlite3VdbeMakeLabel(v);
      sqlite3VdbeAddOp(v, OP_Rewind, iCur, brk);
      start = sqlite3VdbeCurrentAddr(v);
      pLevel->op = OP_Next;
      pLevel->p1 = iCur;
      pLevel->p2 = start;
      haveKey = 0;
    }else{
      /* Case 5: The WHERE clause term that refers to the right-most
      **         column of the index is an inequality.  For example, if
      **         the index is on (x,y,z) and the WHERE clause is of the
      **         form "x=5 AND y<10" then this case is used.  Only the
      **         right-most column can be an inequality - the rest must
      **         use the "==" operator.
      **
      **         This case is also used when there are no WHERE clause
      **         constraints but an index is selected anyway, in order
      **         to force the output order to conform to an ORDER BY.
      */
      int score = pLevel->score;
      int nEqColumn = score/8;
      int start;
      int leFlag=0, geFlag=0;
      int testOp;

      /* Evaluate the equality constraints
      */
      for(j=0; j<nEqColumn; j++){
        int iIdxCol = pIdx->aiColumn[j];
        for(pTerm=aExpr, k=0; k<nExpr; k++, pTerm++){
          Expr *pX = pTerm->p;
          if( pX==0 ) continue;
          if( pTerm->idxLeft==iCur
             && pX->op==TK_EQ
             && (pTerm->prereqRight & loopMask)==pTerm->prereqRight 
             && pX->pLeft->iColumn==iIdxCol
          ){
            sqlite3ExprCode(pParse, pX->pRight);
            disableTerm(pLevel, &pTerm->p);
            break;
          }
        }
      }

      /* Duplicate the equality term values because they will all be
      ** used twice: once to make the termination key and once to make the
      ** start key.
      */
      for(j=0; j<nEqColumn; j++){
        sqlite3VdbeAddOp(v, OP_Dup, nEqColumn-1, 0);
      }

      /* Labels for the beginning and end of the loop
      */
      cont = pLevel->cont = sqlite3VdbeMakeLabel(v);
      brk = pLevel->brk = sqlite3VdbeMakeLabel(v);

      /* Generate the termination key.  This is the key value that
      ** will end the search.  There is no termination key if there
      ** are no equality terms and no "X<..." term.
      **
      ** 2002-Dec-04: On a reverse-order scan, the so-called "termination"
      ** key computed here really ends up being the start key.
      */
      if( (score & 1)!=0 ){
        for(pTerm=aExpr, k=0; k<nExpr; k++, pTerm++){
          Expr *pX = pTerm->p;
          if( pX==0 ) continue;
          if( pTerm->idxLeft==iCur
             && (pX->op==TK_LT || pX->op==TK_LE)
             && (pTerm->prereqRight & loopMask)==pTerm->prereqRight 
             && pX->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            sqlite3ExprCode(pParse, pX->pRight);
            leFlag = pX->op==TK_LE;
            disableTerm(pLevel, &pTerm->p);
            break;
          }
        }
        testOp = OP_IdxGE;
      }else{
        testOp = nEqColumn>0 ? OP_IdxGE : OP_Noop;
        leFlag = 1;
      }
      if( testOp!=OP_Noop ){
        int nCol = nEqColumn + (score & 1);
        pLevel->iMem = pParse->nMem++;
        buildIndexProbe(v, nCol, brk, pIdx);
        if( pLevel->bRev ){
          int op = leFlag ? OP_MoveLe : OP_MoveLt;
          sqlite3VdbeAddOp(v, op, pLevel->iCur, brk);
        }else{
          sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        }
      }else if( pLevel->bRev ){
        sqlite3VdbeAddOp(v, OP_Last, pLevel->iCur, brk);
      }

      /* Generate the start key.  This is the key that defines the lower
      ** bound on the search.  There is no start key if there are no
      ** equality terms and if there is no "X>..." term.  In
      ** that case, generate a "Rewind" instruction in place of the
      ** start key search.
      **
      ** 2002-Dec-04: In the case of a reverse-order search, the so-called
      ** "start" key really ends up being used as the termination key.
      */
      if( (score & 2)!=0 ){
        for(pTerm=aExpr, k=0; k<nExpr; k++, pTerm++){
          Expr *pX = pTerm->p;
          if( pX==0 ) continue;
          if( pTerm->idxLeft==iCur
             && (pX->op==TK_GT || pX->op==TK_GE)
             && (pTerm->prereqRight & loopMask)==pTerm->prereqRight 
             && pX->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            sqlite3ExprCode(pParse, pX->pRight);
            geFlag = pX->op==TK_GE;
            disableTerm(pLevel, &pTerm->p);
            break;
          }
        }
      }else{
        geFlag = 1;
      }
      if( nEqColumn>0 || (score&2)!=0 ){
        int nCol = nEqColumn + ((score&2)!=0);
        buildIndexProbe(v, nCol, brk, pIdx);
        if( pLevel->bRev ){
          pLevel->iMem = pParse->nMem++;
          sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
          testOp = OP_IdxLT;
        }else{
          int op = geFlag ? OP_MoveGe : OP_MoveGt;
          sqlite3VdbeAddOp(v, op, pLevel->iCur, brk);
        }
      }else if( pLevel->bRev ){
        testOp = OP_Noop;
      }else{
        sqlite3VdbeAddOp(v, OP_Rewind, pLevel->iCur, brk);
      }

      /* Generate the the top of the loop.  If there is a termination
      ** key we have to test for that key and abort at the top of the
      ** loop.
      */
      start = sqlite3VdbeCurrentAddr(v);
      if( testOp!=OP_Noop ){
        sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, testOp, pLevel->iCur, brk);
        if( (leFlag && !pLevel->bRev) || (!geFlag && pLevel->bRev) ){
          sqlite3VdbeChangeP3(v, -1, "+", P3_STATIC);
        }
      }
      sqlite3VdbeAddOp(v, OP_RowKey, pLevel->iCur, 0);
      sqlite3VdbeAddOp(v, OP_IdxIsNull, nEqColumn + (score & 1), cont);
      sqlite3VdbeAddOp(v, OP_IdxRecno, pLevel->iCur, 0);
      if( i==pTabList->nSrc-1 && pushKey ){
        haveKey = 1;
      }else{
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
        haveKey = 0;
      }

      /* Record the instruction used to terminate the loop.
      */
      pLevel->op = pLevel->bRev ? OP_Prev : OP_Next;
      pLevel->p1 = pLevel->iCur;
      pLevel->p2 = start;
    }
    loopMask |= getMask(&maskSet, iCur);

    /* Insert code to test every subexpression that can be completely
    ** computed using the current set of tables.
    */
    for(pTerm=aExpr, j=0; j<nExpr; j++, pTerm++){
      if( pTerm->p==0 ) continue;
      if( (pTerm->prereqAll & loopMask)!=pTerm->prereqAll ) continue;
      if( pLevel->iLeftJoin && !ExprHasProperty(pTerm->p,EP_FromJoin) ){
        continue;
      }
      if( haveKey ){
        haveKey = 0;
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
      }
      sqlite3ExprIfFalse(pParse, pTerm->p, cont, 1);
      pTerm->p = 0;
    }
    brk = cont;

    /* For a LEFT OUTER JOIN, generate code that will record the fact that
    ** at least one row of the right table has matched the left table.  
    */
    if( pLevel->iLeftJoin ){
      pLevel->top = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_Integer, 1, 0);
      sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iLeftJoin, 1);
      VdbeComment((v, "# record LEFT JOIN hit"));
      for(pTerm=aExpr, j=0; j<nExpr; j++, pTerm++){
        if( pTerm->p==0 ) continue;
        if( (pTerm->prereqAll & loopMask)!=pTerm->prereqAll ) continue;
        if( haveKey ){
          /* Cannot happen.  "haveKey" can only be true if pushKey is true
          ** an pushKey can only be true for DELETE and UPDATE and there are
          ** no outer joins with DELETE and UPDATE.
          */
          haveKey = 0;
          sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
        }
        sqlite3ExprIfFalse(pParse, pTerm->p, cont, 1);
        pTerm->p = 0;
      }
    }
  }
  pWInfo->iContinue = cont;
  if( pushKey && !haveKey ){
    sqlite3VdbeAddOp(v, OP_Recno, pTabList->a[0].iCursor, 0);
  }
  freeMaskSet(&maskSet);
  return pWInfo;
}

/*
** Generate the end of the WHERE loop.  See comments on 
** sqlite3WhereBegin() for additional information.
*/
void sqlite3WhereEnd(WhereInfo *pWInfo){
  Vdbe *v = pWInfo->pParse->pVdbe;
  int i;
  WhereLevel *pLevel;
  SrcList *pTabList = pWInfo->pTabList;

  for(i=pTabList->nSrc-1; i>=0; i--){
    pLevel = &pWInfo->a[i];
    sqlite3VdbeResolveLabel(v, pLevel->cont);
    if( pLevel->op!=OP_Noop ){
      sqlite3VdbeAddOp(v, pLevel->op, pLevel->p1, pLevel->p2);
    }
    sqlite3VdbeResolveLabel(v, pLevel->brk);
    if( pLevel->inOp!=OP_Noop ){
      sqlite3VdbeAddOp(v, pLevel->inOp, pLevel->inP1, pLevel->inP2);
    }
    if( pLevel->iLeftJoin ){
      int addr;
      addr = sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iLeftJoin, 0);
      sqlite3VdbeAddOp(v, OP_NotNull, 1, addr+4 + (pLevel->iCur>=0));
      sqlite3VdbeAddOp(v, OP_NullRow, pTabList->a[i].iCursor, 0);
      if( pLevel->iCur>=0 ){
        sqlite3VdbeAddOp(v, OP_NullRow, pLevel->iCur, 0);
      }
      sqlite3VdbeAddOp(v, OP_Goto, 0, pLevel->top);
    }
  }
  sqlite3VdbeResolveLabel(v, pWInfo->iBreak);
  for(i=0; i<pTabList->nSrc; i++){
    Table *pTab = pTabList->a[i].pTab;
    assert( pTab!=0 );
    if( pTab->isTransient || pTab->pSelect ) continue;
    pLevel = &pWInfo->a[i];
    sqlite3VdbeAddOp(v, OP_Close, pTabList->a[i].iCursor, 0);
    if( pLevel->pIdx!=0 ){
      sqlite3VdbeAddOp(v, OP_Close, pLevel->iCur, 0);
    }
  }
  sqliteFree(pWInfo);
  return;
}
