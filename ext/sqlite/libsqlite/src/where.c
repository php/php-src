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
  int ix[32];     /* Cursor assigned to each bit */
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
** previously invoked sqliteExprResolveIds() on the expression.  See
** the header comment on that routine for additional information.
** The sqliteExprResolveIds() routines looks for column names and
** sets their opcodes to TK_COLUMN and their Expr.iTable fields to
** the VDBE cursor number of the table.
*/
static int exprTableUsage(ExprMaskSet *pMaskSet, Expr *p){
  unsigned int mask = 0;
  if( p==0 ) return 0;
  if( p->op==TK_COLUMN ){
    return getMask(pMaskSet, p->iTable);
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
  switch( op ){
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_EQ:
    case TK_IN:
      return 1;
    default:
      return 0;
  }
}

/*
** The input to this routine is an ExprInfo structure with only the
** "p" field filled in.  The job of this routine is to analyze the
** subexpression and populate all the other fields of the ExprInfo
** structure.
*/
static void exprAnalyze(ExprMaskSet *pMaskSet, ExprInfo *pInfo){
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

  assert( pOrderBy!=0 );
  assert( pOrderBy->nExpr>0 );
  sortOrder = pOrderBy->a[0].sortOrder & SQLITE_SO_DIRMASK;
  for(i=0; i<pOrderBy->nExpr; i++){
    Expr *p;
    if( (pOrderBy->a[i].sortOrder & SQLITE_SO_DIRMASK)!=sortOrder ){
      /* Indices can only be used if all ORDER BY terms are either
      ** DESC or ASC.  Indices cannot be used on a mixture. */
      return 0;
    }
    if( (pOrderBy->a[i].sortOrder & SQLITE_SO_TYPEMASK)!=SQLITE_SO_UNK ){
      /* Do not sort by index if there is a COLLATE clause */
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
      if( pPreferredIdx->aiColumn[i]!=pIdx->aiColumn[i] ) break;
      if( j<nExpr && pOrderBy->a[j].pExpr->iColumn==pIdx->aiColumn[i] ){ j++; }
    }
    if( i<nEqCol ) continue;
    for(i=0; i+j<nExpr; i++){
      if( pOrderBy->a[i+j].pExpr->iColumn!=pIdx->aiColumn[i+nEqCol] ) break;
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
** Generate the beginning of the loop used for WHERE clause processing.
** The return value is a pointer to an (opaque) structure that contains
** information needed to terminate the loop.  Later, the calling routine
** should invoke sqliteWhereEnd() with the return value of this function
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
**        foreach row2 in t2 do      |-- by sqliteWhereBegin()
**          foreach row3 in t3 do   /
**            ...
**          end                     \    Code generated
**        end                        |-- by sqliteWhereEnd()
**      end                         /
**
** There are Btree cursors associated with each table.  t1 uses cursor
** number pTabList->a[0].iCursor.  t2 uses the cursor pTabList->a[1].iCursor.
** And so forth.  This routine generates code to open those VDBE cursors
** and sqliteWhereEnd() generates the code to close them.
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
WhereInfo *sqliteWhereBegin(
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
  int haveKey;         /* True if KEY is on the stack */
  ExprMaskSet maskSet; /* The expression mask set */
  int iDirectEq[32];   /* Term of the form ROWID==X for the N-th table */
  int iDirectLt[32];   /* Term of the form ROWID<X or ROWID<=X */
  int iDirectGt[32];   /* Term of the form ROWID>X or ROWID>=X */
  ExprInfo aExpr[101]; /* The WHERE clause is divided into these expressions */

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
    sqliteErrorMsg(pParse, "WHERE clause too complex - no more "
       "than %d terms allowed", (int)ARRAYSIZE(aExpr)-1);
    return 0;
  }
  
  /* Allocate and initialize the WhereInfo structure that will become the
  ** return value.
  */
  pWInfo = sqliteMalloc( sizeof(WhereInfo) + pTabList->nSrc*sizeof(WhereLevel));
  if( sqlite_malloc_failed ){
    sqliteFree(pWInfo);
    return 0;
  }
  pWInfo->pParse = pParse;
  pWInfo->pTabList = pTabList;
  pWInfo->peakNTab = pWInfo->savedNTab = pParse->nTab;
  pWInfo->iBreak = sqliteVdbeMakeLabel(v);

  /* Special case: a WHERE clause that is constant.  Evaluate the
  ** expression and either jump over all of the code or fall thru.
  */
  if( pWhere && (pTabList->nSrc==0 || sqliteExprIsConstant(pWhere)) ){
    sqliteExprIfFalse(pParse, pWhere, pWInfo->iBreak, 1);
    pWhere = 0;
  }

  /* Analyze all of the subexpressions.
  */
  for(i=0; i<nExpr; i++){
    exprAnalyze(&maskSet, &aExpr[i]);

    /* If we are executing a trigger body, remove all references to
    ** new.* and old.* tables from the prerequisite masks.
    */
    if( pParse->trigStack ){
      int x;
      if( (x = pParse->trigStack->newIdx) >= 0 ){
        int mask = ~getMask(&maskSet, x);
        aExpr[i].prereqRight &= mask;
        aExpr[i].prereqLeft &= mask;
        aExpr[i].prereqAll &= mask;
      }
      if( (x = pParse->trigStack->oldIdx) >= 0 ){
        int mask = ~getMask(&maskSet, x);
        aExpr[i].prereqRight &= mask;
        aExpr[i].prereqLeft &= mask;
        aExpr[i].prereqAll &= mask;
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
    pWInfo->a[i].iCur = -1;
    iDirectEq[i] = -1;
    iDirectLt[i] = -1;
    iDirectGt[i] = -1;
    for(j=0; j<nExpr; j++){
      if( aExpr[j].idxLeft==iCur && aExpr[j].p->pLeft->iColumn<0
            && (aExpr[j].prereqRight & loopMask)==aExpr[j].prereqRight ){
        switch( aExpr[j].p->op ){
          case TK_IN:
          case TK_EQ: iDirectEq[i] = j; break;
          case TK_LE:
          case TK_LT: iDirectLt[i] = j; break;
          case TK_GE:
          case TK_GT: iDirectGt[i] = j;  break;
        }
      }
      if( aExpr[j].idxRight==iCur && aExpr[j].p->pRight->iColumn<0
            && (aExpr[j].prereqLeft & loopMask)==aExpr[j].prereqLeft ){
        switch( aExpr[j].p->op ){
          case TK_EQ: iDirectEq[i] = j;  break;
          case TK_LE:
          case TK_LT: iDirectGt[i] = j;  break;
          case TK_GE:
          case TK_GT: iDirectLt[i] = j;  break;
        }
      }
    }
    if( iDirectEq[i]>=0 ){
      loopMask |= mask;
      pWInfo->a[i].pIdx = 0;
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
      for(j=0; j<nExpr; j++){
        if( aExpr[j].idxLeft==iCur 
             && (aExpr[j].prereqRight & loopMask)==aExpr[j].prereqRight ){
          int iColumn = aExpr[j].p->pLeft->iColumn;
          int k;
          for(k=0; k<pIdx->nColumn; k++){
            if( pIdx->aiColumn[k]==iColumn ){
              switch( aExpr[j].p->op ){
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
        if( aExpr[j].idxRight==iCur 
             && (aExpr[j].prereqLeft & loopMask)==aExpr[j].prereqLeft ){
          int iColumn = aExpr[j].p->pRight->iColumn;
          int k;
          for(k=0; k<pIdx->nColumn; k++){
            if( pIdx->aiColumn[k]==iColumn ){
              switch( aExpr[j].p->op ){
                case TK_EQ: {
                  eqMask |= 1<<k;
                  break;
                }
                case TK_LE:
                case TK_LT: {
                  gtMask |= 1<<k;
                  break;
                }
                case TK_GE:
                case TK_GT: {
                  ltMask |= 1<<k;
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
    pWInfo->a[i].pIdx = pBestIdx;
    pWInfo->a[i].score = bestScore;
    pWInfo->a[i].bRev = 0;
    loopMask |= mask;
    if( pBestIdx ){
      pWInfo->a[i].iCur = pParse->nTab++;
      pWInfo->peakNTab = pParse->nTab;
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
       pSortIdx = findSortingIndex(pTab, pTabList->a[0].iCursor, 
                                   *ppOrderBy, pIdx, nEqCol, &bRev);
     }
     if( pSortIdx && (pIdx==0 || pIdx==pSortIdx) ){
       if( pIdx==0 ){
         pWInfo->a[0].pIdx = pSortIdx;
         pWInfo->a[0].iCur = pParse->nTab++;
         pWInfo->peakNTab = pParse->nTab;
       }
       pWInfo->a[0].bRev = bRev;
       *ppOrderBy = 0;
     }
  }

  /* Open all tables in the pTabList and all indices used by those tables.
  */
  for(i=0; i<pTabList->nSrc; i++){
    Table *pTab;
    Index *pIx;

    pTab = pTabList->a[i].pTab;
    if( pTab->isTransient || pTab->pSelect ) continue;
    sqliteVdbeAddOp(v, OP_Integer, pTab->iDb, 0);
    sqliteVdbeOp3(v, OP_OpenRead, pTabList->a[i].iCursor, pTab->tnum,
                     pTab->zName, P3_STATIC);
    sqliteCodeVerifySchema(pParse, pTab->iDb);
    if( (pIx = pWInfo->a[i].pIdx)!=0 ){
      sqliteVdbeAddOp(v, OP_Integer, pIx->iDb, 0);
      sqliteVdbeOp3(v, OP_OpenRead, pWInfo->a[i].iCur, pIx->tnum, pIx->zName,0);
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
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      sqliteVdbeAddOp(v, OP_MemStore, pLevel->iLeftJoin, 1);
    }

    pIdx = pLevel->pIdx;
    pLevel->inOp = OP_Noop;
    if( i<ARRAYSIZE(iDirectEq) && iDirectEq[i]>=0 ){
      /* Case 1:  We can directly reference a single row using an
      **          equality comparison against the ROWID field.  Or
      **          we reference multiple rows using a "rowid IN (...)"
      **          construct.
      */
      k = iDirectEq[i];
      assert( k<nExpr );
      assert( aExpr[k].p!=0 );
      assert( aExpr[k].idxLeft==iCur || aExpr[k].idxRight==iCur );
      brk = pLevel->brk = sqliteVdbeMakeLabel(v);
      if( aExpr[k].idxLeft==iCur ){
        Expr *pX = aExpr[k].p;
        if( pX->op!=TK_IN ){
          sqliteExprCode(pParse, aExpr[k].p->pRight);
        }else if( pX->pList ){
          sqliteVdbeAddOp(v, OP_SetFirst, pX->iTable, brk);
          pLevel->inOp = OP_SetNext;
          pLevel->inP1 = pX->iTable;
          pLevel->inP2 = sqliteVdbeCurrentAddr(v);
        }else{
          assert( pX->pSelect );
          sqliteVdbeAddOp(v, OP_Rewind, pX->iTable, brk);
          sqliteVdbeAddOp(v, OP_KeyAsData, pX->iTable, 1);
          pLevel->inP2 = sqliteVdbeAddOp(v, OP_FullKey, pX->iTable, 0);
          pLevel->inOp = OP_Next;
          pLevel->inP1 = pX->iTable;
        }
      }else{
        sqliteExprCode(pParse, aExpr[k].p->pLeft);
      }
      aExpr[k].p = 0;
      cont = pLevel->cont = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_MustBeInt, 1, brk);
      haveKey = 0;
      sqliteVdbeAddOp(v, OP_NotExists, iCur, brk);
      pLevel->op = OP_Noop;
    }else if( pIdx!=0 && pLevel->score>0 && pLevel->score%4==0 ){
      /* Case 2:  There is an index and all terms of the WHERE clause that
      **          refer to the index use the "==" or "IN" operators.
      */
      int start;
      int testOp;
      int nColumn = (pLevel->score+4)/8;
      brk = pLevel->brk = sqliteVdbeMakeLabel(v);
      for(j=0; j<nColumn; j++){
        for(k=0; k<nExpr; k++){
          Expr *pX = aExpr[k].p;
          if( pX==0 ) continue;
          if( aExpr[k].idxLeft==iCur
             && (aExpr[k].prereqRight & loopMask)==aExpr[k].prereqRight 
             && pX->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            if( pX->op==TK_EQ ){
              sqliteExprCode(pParse, pX->pRight);
              aExpr[k].p = 0;
              break;
            }
            if( pX->op==TK_IN && nColumn==1 ){
              if( pX->pList ){
                sqliteVdbeAddOp(v, OP_SetFirst, pX->iTable, brk);
                pLevel->inOp = OP_SetNext;
                pLevel->inP1 = pX->iTable;
                pLevel->inP2 = sqliteVdbeCurrentAddr(v);
              }else{
                assert( pX->pSelect );
                sqliteVdbeAddOp(v, OP_Rewind, pX->iTable, brk);
                sqliteVdbeAddOp(v, OP_KeyAsData, pX->iTable, 1);
                pLevel->inP2 = sqliteVdbeAddOp(v, OP_FullKey, pX->iTable, 0);
                pLevel->inOp = OP_Next;
                pLevel->inP1 = pX->iTable;
              }
              aExpr[k].p = 0;
              break;
            }
          }
          if( aExpr[k].idxRight==iCur
             && aExpr[k].p->op==TK_EQ
             && (aExpr[k].prereqLeft & loopMask)==aExpr[k].prereqLeft
             && aExpr[k].p->pRight->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, aExpr[k].p->pLeft);
            aExpr[k].p = 0;
            break;
          }
        }
      }
      pLevel->iMem = pParse->nMem++;
      cont = pLevel->cont = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_NotNull, -nColumn, sqliteVdbeCurrentAddr(v)+3);
      sqliteVdbeAddOp(v, OP_Pop, nColumn, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, brk);
      sqliteVdbeAddOp(v, OP_MakeKey, nColumn, 0);
      sqliteAddIdxKeyType(v, pIdx);
      if( nColumn==pIdx->nColumn || pLevel->bRev ){
        sqliteVdbeAddOp(v, OP_MemStore, pLevel->iMem, 0);
        testOp = OP_IdxGT;
      }else{
        sqliteVdbeAddOp(v, OP_Dup, 0, 0);
        sqliteVdbeAddOp(v, OP_IncrKey, 0, 0);
        sqliteVdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        testOp = OP_IdxGE;
      }
      if( pLevel->bRev ){
        /* Scan in reverse order */
        sqliteVdbeAddOp(v, OP_IncrKey, 0, 0);
        sqliteVdbeAddOp(v, OP_MoveLt, pLevel->iCur, brk);
        start = sqliteVdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqliteVdbeAddOp(v, OP_IdxLT, pLevel->iCur, brk);
        pLevel->op = OP_Prev;
      }else{
        /* Scan in the forward order */
        sqliteVdbeAddOp(v, OP_MoveTo, pLevel->iCur, brk);
        start = sqliteVdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqliteVdbeAddOp(v, testOp, pLevel->iCur, brk);
        pLevel->op = OP_Next;
      }
      sqliteVdbeAddOp(v, OP_RowKey, pLevel->iCur, 0);
      sqliteVdbeAddOp(v, OP_IdxIsNull, nColumn, cont);
      sqliteVdbeAddOp(v, OP_IdxRecno, pLevel->iCur, 0);
      if( i==pTabList->nSrc-1 && pushKey ){
        haveKey = 1;
      }else{
        sqliteVdbeAddOp(v, OP_MoveTo, iCur, 0);
        haveKey = 0;
      }
      pLevel->p1 = pLevel->iCur;
      pLevel->p2 = start;
    }else if( i<ARRAYSIZE(iDirectLt) && (iDirectLt[i]>=0 || iDirectGt[i]>=0) ){
      /* Case 3:  We have an inequality comparison against the ROWID field.
      */
      int testOp = OP_Noop;
      int start;

      brk = pLevel->brk = sqliteVdbeMakeLabel(v);
      cont = pLevel->cont = sqliteVdbeMakeLabel(v);
      if( iDirectGt[i]>=0 ){
        k = iDirectGt[i];
        assert( k<nExpr );
        assert( aExpr[k].p!=0 );
        assert( aExpr[k].idxLeft==iCur || aExpr[k].idxRight==iCur );
        if( aExpr[k].idxLeft==iCur ){
          sqliteExprCode(pParse, aExpr[k].p->pRight);
        }else{
          sqliteExprCode(pParse, aExpr[k].p->pLeft);
        }
        sqliteVdbeAddOp(v, OP_ForceInt,
          aExpr[k].p->op==TK_LT || aExpr[k].p->op==TK_GT, brk);
        sqliteVdbeAddOp(v, OP_MoveTo, iCur, brk);
        aExpr[k].p = 0;
      }else{
        sqliteVdbeAddOp(v, OP_Rewind, iCur, brk);
      }
      if( iDirectLt[i]>=0 ){
        k = iDirectLt[i];
        assert( k<nExpr );
        assert( aExpr[k].p!=0 );
        assert( aExpr[k].idxLeft==iCur || aExpr[k].idxRight==iCur );
        if( aExpr[k].idxLeft==iCur ){
          sqliteExprCode(pParse, aExpr[k].p->pRight);
        }else{
          sqliteExprCode(pParse, aExpr[k].p->pLeft);
        }
        /* sqliteVdbeAddOp(v, OP_MustBeInt, 0, sqliteVdbeCurrentAddr(v)+1); */
        pLevel->iMem = pParse->nMem++;
        sqliteVdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        if( aExpr[k].p->op==TK_LT || aExpr[k].p->op==TK_GT ){
          testOp = OP_Ge;
        }else{
          testOp = OP_Gt;
        }
        aExpr[k].p = 0;
      }
      start = sqliteVdbeCurrentAddr(v);
      pLevel->op = OP_Next;
      pLevel->p1 = iCur;
      pLevel->p2 = start;
      if( testOp!=OP_Noop ){
        sqliteVdbeAddOp(v, OP_Recno, iCur, 0);
        sqliteVdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqliteVdbeAddOp(v, testOp, 0, brk);
      }
      haveKey = 0;
    }else if( pIdx==0 ){
      /* Case 4:  There is no usable index.  We must do a complete
      **          scan of the entire database table.
      */
      int start;

      brk = pLevel->brk = sqliteVdbeMakeLabel(v);
      cont = pLevel->cont = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_Rewind, iCur, brk);
      start = sqliteVdbeCurrentAddr(v);
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
      int leFlag, geFlag;
      int testOp;

      /* Evaluate the equality constraints
      */
      for(j=0; j<nEqColumn; j++){
        for(k=0; k<nExpr; k++){
          if( aExpr[k].p==0 ) continue;
          if( aExpr[k].idxLeft==iCur
             && aExpr[k].p->op==TK_EQ
             && (aExpr[k].prereqRight & loopMask)==aExpr[k].prereqRight 
             && aExpr[k].p->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, aExpr[k].p->pRight);
            aExpr[k].p = 0;
            break;
          }
          if( aExpr[k].idxRight==iCur
             && aExpr[k].p->op==TK_EQ
             && (aExpr[k].prereqLeft & loopMask)==aExpr[k].prereqLeft
             && aExpr[k].p->pRight->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, aExpr[k].p->pLeft);
            aExpr[k].p = 0;
            break;
          }
        }
      }

      /* Duplicate the equality term values because they will all be
      ** used twice: once to make the termination key and once to make the
      ** start key.
      */
      for(j=0; j<nEqColumn; j++){
        sqliteVdbeAddOp(v, OP_Dup, nEqColumn-1, 0);
      }

      /* Labels for the beginning and end of the loop
      */
      cont = pLevel->cont = sqliteVdbeMakeLabel(v);
      brk = pLevel->brk = sqliteVdbeMakeLabel(v);

      /* Generate the termination key.  This is the key value that
      ** will end the search.  There is no termination key if there
      ** are no equality terms and no "X<..." term.
      **
      ** 2002-Dec-04: On a reverse-order scan, the so-called "termination"
      ** key computed here really ends up being the start key.
      */
      if( (score & 1)!=0 ){
        for(k=0; k<nExpr; k++){
          Expr *pExpr = aExpr[k].p;
          if( pExpr==0 ) continue;
          if( aExpr[k].idxLeft==iCur
             && (pExpr->op==TK_LT || pExpr->op==TK_LE)
             && (aExpr[k].prereqRight & loopMask)==aExpr[k].prereqRight 
             && pExpr->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, pExpr->pRight);
            leFlag = pExpr->op==TK_LE;
            aExpr[k].p = 0;
            break;
          }
          if( aExpr[k].idxRight==iCur
             && (pExpr->op==TK_GT || pExpr->op==TK_GE)
             && (aExpr[k].prereqLeft & loopMask)==aExpr[k].prereqLeft
             && pExpr->pRight->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, pExpr->pLeft);
            leFlag = pExpr->op==TK_GE;
            aExpr[k].p = 0;
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
        sqliteVdbeAddOp(v, OP_NotNull, -nCol, sqliteVdbeCurrentAddr(v)+3);
        sqliteVdbeAddOp(v, OP_Pop, nCol, 0);
        sqliteVdbeAddOp(v, OP_Goto, 0, brk);
        sqliteVdbeAddOp(v, OP_MakeKey, nCol, 0);
        sqliteAddIdxKeyType(v, pIdx);
        if( leFlag ){
          sqliteVdbeAddOp(v, OP_IncrKey, 0, 0);
        }
        if( pLevel->bRev ){
          sqliteVdbeAddOp(v, OP_MoveLt, pLevel->iCur, brk);
        }else{
          sqliteVdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        }
      }else if( pLevel->bRev ){
        sqliteVdbeAddOp(v, OP_Last, pLevel->iCur, brk);
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
        for(k=0; k<nExpr; k++){
          Expr *pExpr = aExpr[k].p;
          if( pExpr==0 ) continue;
          if( aExpr[k].idxLeft==iCur
             && (pExpr->op==TK_GT || pExpr->op==TK_GE)
             && (aExpr[k].prereqRight & loopMask)==aExpr[k].prereqRight 
             && pExpr->pLeft->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, pExpr->pRight);
            geFlag = pExpr->op==TK_GE;
            aExpr[k].p = 0;
            break;
          }
          if( aExpr[k].idxRight==iCur
             && (pExpr->op==TK_LT || pExpr->op==TK_LE)
             && (aExpr[k].prereqLeft & loopMask)==aExpr[k].prereqLeft
             && pExpr->pRight->iColumn==pIdx->aiColumn[j]
          ){
            sqliteExprCode(pParse, pExpr->pLeft);
            geFlag = pExpr->op==TK_LE;
            aExpr[k].p = 0;
            break;
          }
        }
      }else{
        geFlag = 1;
      }
      if( nEqColumn>0 || (score&2)!=0 ){
        int nCol = nEqColumn + ((score&2)!=0);
        sqliteVdbeAddOp(v, OP_NotNull, -nCol, sqliteVdbeCurrentAddr(v)+3);
        sqliteVdbeAddOp(v, OP_Pop, nCol, 0);
        sqliteVdbeAddOp(v, OP_Goto, 0, brk);
        sqliteVdbeAddOp(v, OP_MakeKey, nCol, 0);
        sqliteAddIdxKeyType(v, pIdx);
        if( !geFlag ){
          sqliteVdbeAddOp(v, OP_IncrKey, 0, 0);
        }
        if( pLevel->bRev ){
          pLevel->iMem = pParse->nMem++;
          sqliteVdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
          testOp = OP_IdxLT;
        }else{
          sqliteVdbeAddOp(v, OP_MoveTo, pLevel->iCur, brk);
        }
      }else if( pLevel->bRev ){
        testOp = OP_Noop;
      }else{
        sqliteVdbeAddOp(v, OP_Rewind, pLevel->iCur, brk);
      }

      /* Generate the the top of the loop.  If there is a termination
      ** key we have to test for that key and abort at the top of the
      ** loop.
      */
      start = sqliteVdbeCurrentAddr(v);
      if( testOp!=OP_Noop ){
        sqliteVdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqliteVdbeAddOp(v, testOp, pLevel->iCur, brk);
      }
      sqliteVdbeAddOp(v, OP_RowKey, pLevel->iCur, 0);
      sqliteVdbeAddOp(v, OP_IdxIsNull, nEqColumn + (score & 1), cont);
      sqliteVdbeAddOp(v, OP_IdxRecno, pLevel->iCur, 0);
      if( i==pTabList->nSrc-1 && pushKey ){
        haveKey = 1;
      }else{
        sqliteVdbeAddOp(v, OP_MoveTo, iCur, 0);
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
    for(j=0; j<nExpr; j++){
      if( aExpr[j].p==0 ) continue;
      if( (aExpr[j].prereqAll & loopMask)!=aExpr[j].prereqAll ) continue;
      if( pLevel->iLeftJoin && !ExprHasProperty(aExpr[j].p,EP_FromJoin) ){
        continue;
      }
      if( haveKey ){
        haveKey = 0;
        sqliteVdbeAddOp(v, OP_MoveTo, iCur, 0);
      }
      sqliteExprIfFalse(pParse, aExpr[j].p, cont, 1);
      aExpr[j].p = 0;
    }
    brk = cont;

    /* For a LEFT OUTER JOIN, generate code that will record the fact that
    ** at least one row of the right table has matched the left table.  
    */
    if( pLevel->iLeftJoin ){
      pLevel->top = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_Integer, 1, 0);
      sqliteVdbeAddOp(v, OP_MemStore, pLevel->iLeftJoin, 1);
      for(j=0; j<nExpr; j++){
        if( aExpr[j].p==0 ) continue;
        if( (aExpr[j].prereqAll & loopMask)!=aExpr[j].prereqAll ) continue;
        if( haveKey ){
          /* Cannot happen.  "haveKey" can only be true if pushKey is true
          ** an pushKey can only be true for DELETE and UPDATE and there are
          ** no outer joins with DELETE and UPDATE.
          */
          haveKey = 0;
          sqliteVdbeAddOp(v, OP_MoveTo, iCur, 0);
        }
        sqliteExprIfFalse(pParse, aExpr[j].p, cont, 1);
        aExpr[j].p = 0;
      }
    }
  }
  pWInfo->iContinue = cont;
  if( pushKey && !haveKey ){
    sqliteVdbeAddOp(v, OP_Recno, pTabList->a[0].iCursor, 0);
  }
  freeMaskSet(&maskSet);
  return pWInfo;
}

/*
** Generate the end of the WHERE loop.  See comments on 
** sqliteWhereBegin() for additional information.
*/
void sqliteWhereEnd(WhereInfo *pWInfo){
  Vdbe *v = pWInfo->pParse->pVdbe;
  int i;
  WhereLevel *pLevel;
  SrcList *pTabList = pWInfo->pTabList;

  for(i=pTabList->nSrc-1; i>=0; i--){
    pLevel = &pWInfo->a[i];
    sqliteVdbeResolveLabel(v, pLevel->cont);
    if( pLevel->op!=OP_Noop ){
      sqliteVdbeAddOp(v, pLevel->op, pLevel->p1, pLevel->p2);
    }
    sqliteVdbeResolveLabel(v, pLevel->brk);
    if( pLevel->inOp!=OP_Noop ){
      sqliteVdbeAddOp(v, pLevel->inOp, pLevel->inP1, pLevel->inP2);
    }
    if( pLevel->iLeftJoin ){
      int addr;
      addr = sqliteVdbeAddOp(v, OP_MemLoad, pLevel->iLeftJoin, 0);
      sqliteVdbeAddOp(v, OP_NotNull, 1, addr+4 + (pLevel->iCur>=0));
      sqliteVdbeAddOp(v, OP_NullRow, pTabList->a[i].iCursor, 0);
      if( pLevel->iCur>=0 ){
        sqliteVdbeAddOp(v, OP_NullRow, pLevel->iCur, 0);
      }
      sqliteVdbeAddOp(v, OP_Goto, 0, pLevel->top);
    }
  }
  sqliteVdbeResolveLabel(v, pWInfo->iBreak);
  for(i=0; i<pTabList->nSrc; i++){
    Table *pTab = pTabList->a[i].pTab;
    assert( pTab!=0 );
    if( pTab->isTransient || pTab->pSelect ) continue;
    pLevel = &pWInfo->a[i];
    sqliteVdbeAddOp(v, OP_Close, pTabList->a[i].iCursor, 0);
    if( pLevel->pIdx!=0 ){
      sqliteVdbeAddOp(v, OP_Close, pLevel->iCur, 0);
    }
  }
#if 0  /* Never reuse a cursor */
  if( pWInfo->pParse->nTab==pWInfo->peakNTab ){
    pWInfo->pParse->nTab = pWInfo->savedNTab;
  }
#endif
  sqliteFree(pWInfo);
  return;
}
