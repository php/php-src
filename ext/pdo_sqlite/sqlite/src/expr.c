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
** This file contains routines used for analyzing expressions and
** for generating VDBE code that evaluates expressions in SQLite.
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

/*
** Return the 'affinity' of the expression pExpr if any.
**
** If pExpr is a column, a reference to a column via an 'AS' alias,
** or a sub-select with a column as the return value, then the 
** affinity of that column is returned. Otherwise, 0x00 is returned,
** indicating no affinity for the expression.
**
** i.e. the WHERE clause expresssions in the following statements all
** have an affinity:
**
** CREATE TABLE t1(a);
** SELECT * FROM t1 WHERE a;
** SELECT a AS b FROM t1 WHERE b;
** SELECT * FROM t1 WHERE (select a from t1);
*/
char sqlite3ExprAffinity(Expr *pExpr){
  int op = pExpr->op;
  if( op==TK_AS ){
    return sqlite3ExprAffinity(pExpr->pLeft);
  }
  if( op==TK_SELECT ){
    return sqlite3ExprAffinity(pExpr->pSelect->pEList->a[0].pExpr);
  }
#ifndef SQLITE_OMIT_CAST
  if( op==TK_CAST ){
    return sqlite3AffinityType(&pExpr->token);
  }
#endif
  return pExpr->affinity;
}

/*
** Set the collating sequence for expression pExpr to be the collating
** sequence named by pToken.   Return a pointer to the revised expression.
** The collating sequence is marked as "explicit" using the EP_ExpCollate
** flag.  An explicit collating sequence will override implicit
** collating sequences.
*/
Expr *sqlite3ExprSetColl(Parse *pParse, Expr *pExpr, Token *pName){
  CollSeq *pColl;
  if( pExpr==0 ) return 0;
  pColl = sqlite3LocateCollSeq(pParse, (char*)pName->z, pName->n);
  if( pColl ){
    pExpr->pColl = pColl;
    pExpr->flags |= EP_ExpCollate;
  }
  return pExpr;
}

/*
** Return the default collation sequence for the expression pExpr. If
** there is no default collation type, return 0.
*/
CollSeq *sqlite3ExprCollSeq(Parse *pParse, Expr *pExpr){
  CollSeq *pColl = 0;
  if( pExpr ){
    pColl = pExpr->pColl;
    if( (pExpr->op==TK_AS || pExpr->op==TK_CAST) && !pColl ){
      return sqlite3ExprCollSeq(pParse, pExpr->pLeft);
    }
  }
  if( sqlite3CheckCollSeq(pParse, pColl) ){ 
    pColl = 0;
  }
  return pColl;
}

/*
** pExpr is an operand of a comparison operator.  aff2 is the
** type affinity of the other operand.  This routine returns the
** type affinity that should be used for the comparison operator.
*/
char sqlite3CompareAffinity(Expr *pExpr, char aff2){
  char aff1 = sqlite3ExprAffinity(pExpr);
  if( aff1 && aff2 ){
    /* Both sides of the comparison are columns. If one has numeric
    ** affinity, use that. Otherwise use no affinity.
    */
    if( sqlite3IsNumericAffinity(aff1) || sqlite3IsNumericAffinity(aff2) ){
      return SQLITE_AFF_NUMERIC;
    }else{
      return SQLITE_AFF_NONE;
    }
  }else if( !aff1 && !aff2 ){
    /* Neither side of the comparison is a column.  Compare the
    ** results directly.
    */
    return SQLITE_AFF_NONE;
  }else{
    /* One side is a column, the other is not. Use the columns affinity. */
    assert( aff1==0 || aff2==0 );
    return (aff1 + aff2);
  }
}

/*
** pExpr is a comparison operator.  Return the type affinity that should
** be applied to both operands prior to doing the comparison.
*/
static char comparisonAffinity(Expr *pExpr){
  char aff;
  assert( pExpr->op==TK_EQ || pExpr->op==TK_IN || pExpr->op==TK_LT ||
          pExpr->op==TK_GT || pExpr->op==TK_GE || pExpr->op==TK_LE ||
          pExpr->op==TK_NE );
  assert( pExpr->pLeft );
  aff = sqlite3ExprAffinity(pExpr->pLeft);
  if( pExpr->pRight ){
    aff = sqlite3CompareAffinity(pExpr->pRight, aff);
  }
  else if( pExpr->pSelect ){
    aff = sqlite3CompareAffinity(pExpr->pSelect->pEList->a[0].pExpr, aff);
  }
  else if( !aff ){
    aff = SQLITE_AFF_NONE;
  }
  return aff;
}

/*
** pExpr is a comparison expression, eg. '=', '<', IN(...) etc.
** idx_affinity is the affinity of an indexed column. Return true
** if the index with affinity idx_affinity may be used to implement
** the comparison in pExpr.
*/
int sqlite3IndexAffinityOk(Expr *pExpr, char idx_affinity){
  char aff = comparisonAffinity(pExpr);
  switch( aff ){
    case SQLITE_AFF_NONE:
      return 1;
    case SQLITE_AFF_TEXT:
      return idx_affinity==SQLITE_AFF_TEXT;
    default:
      return sqlite3IsNumericAffinity(idx_affinity);
  }
}

/*
** Return the P1 value that should be used for a binary comparison
** opcode (OP_Eq, OP_Ge etc.) used to compare pExpr1 and pExpr2.
** If jumpIfNull is true, then set the low byte of the returned
** P1 value to tell the opcode to jump if either expression
** evaluates to NULL.
*/
static int binaryCompareP1(Expr *pExpr1, Expr *pExpr2, int jumpIfNull){
  char aff = sqlite3ExprAffinity(pExpr2);
  return ((int)sqlite3CompareAffinity(pExpr1, aff))+(jumpIfNull?0x100:0);
}

/*
** Return a pointer to the collation sequence that should be used by
** a binary comparison operator comparing pLeft and pRight.
**
** If the left hand expression has a collating sequence type, then it is
** used. Otherwise the collation sequence for the right hand expression
** is used, or the default (BINARY) if neither expression has a collating
** type.
*/
static CollSeq* binaryCompareCollSeq(Parse *pParse, Expr *pLeft, Expr *pRight){
  CollSeq *pColl;
  assert( pLeft );
  assert( pRight );
  if( pLeft->flags & EP_ExpCollate ){
    assert( pLeft->pColl );
    pColl = pLeft->pColl;
  }else if( pRight->flags & EP_ExpCollate ){
    assert( pRight->pColl );
    pColl = pRight->pColl;
  }else{
    pColl = sqlite3ExprCollSeq(pParse, pLeft);
    if( !pColl ){
      pColl = sqlite3ExprCollSeq(pParse, pRight);
    }
  }
  return pColl;
}

/*
** Generate code for a comparison operator.
*/
static int codeCompare(
  Parse *pParse,    /* The parsing (and code generating) context */
  Expr *pLeft,      /* The left operand */
  Expr *pRight,     /* The right operand */
  int opcode,       /* The comparison opcode */
  int dest,         /* Jump here if true.  */
  int jumpIfNull    /* If true, jump if either operand is NULL */
){
  int p1 = binaryCompareP1(pLeft, pRight, jumpIfNull);
  CollSeq *p3 = binaryCompareCollSeq(pParse, pLeft, pRight);
  return sqlite3VdbeOp3(pParse->pVdbe, opcode, p1, dest, (void*)p3, P3_COLLSEQ);
}

/*
** Construct a new expression node and return a pointer to it.  Memory
** for this node is obtained from sqliteMalloc().  The calling function
** is responsible for making sure the node eventually gets freed.
*/
Expr *sqlite3Expr(int op, Expr *pLeft, Expr *pRight, const Token *pToken){
  Expr *pNew;
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    /* When malloc fails, delete pLeft and pRight. Expressions passed to 
    ** this function must always be allocated with sqlite3Expr() for this 
    ** reason. 
    */
    sqlite3ExprDelete(pLeft);
    sqlite3ExprDelete(pRight);
    return 0;
  }
  pNew->op = op;
  pNew->pLeft = pLeft;
  pNew->pRight = pRight;
  pNew->iAgg = -1;
  if( pToken ){
    assert( pToken->dyn==0 );
    pNew->span = pNew->token = *pToken;
  }else if( pLeft ){
    if( pRight ){
      sqlite3ExprSpan(pNew, &pLeft->span, &pRight->span);
      if( pRight->flags & EP_ExpCollate ){
        pNew->flags |= EP_ExpCollate;
        pNew->pColl = pRight->pColl;
      }
    }
    if( pLeft->flags & EP_ExpCollate ){
      pNew->flags |= EP_ExpCollate;
      pNew->pColl = pLeft->pColl;
    }
  }
  return pNew;
}

/*
** Works like sqlite3Expr() but frees its pLeft and pRight arguments
** if it fails due to a malloc problem.
*/
Expr *sqlite3ExprOrFree(int op, Expr *pLeft, Expr *pRight, const Token *pToken){
  Expr *pNew = sqlite3Expr(op, pLeft, pRight, pToken);
  if( pNew==0 ){
    sqlite3ExprDelete(pLeft);
    sqlite3ExprDelete(pRight);
  }
  return pNew;
}

/*
** When doing a nested parse, you can include terms in an expression
** that look like this:   #0 #1 #2 ...  These terms refer to elements
** on the stack.  "#0" means the top of the stack.
** "#1" means the next down on the stack.  And so forth.
**
** This routine is called by the parser to deal with on of those terms.
** It immediately generates code to store the value in a memory location.
** The returns an expression that will code to extract the value from
** that memory location as needed.
*/
Expr *sqlite3RegisterExpr(Parse *pParse, Token *pToken){
  Vdbe *v = pParse->pVdbe;
  Expr *p;
  int depth;
  if( pParse->nested==0 ){
    sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", pToken);
    return 0;
  }
  if( v==0 ) return 0;
  p = sqlite3Expr(TK_REGISTER, 0, 0, pToken);
  if( p==0 ){
    return 0;  /* Malloc failed */
  }
  depth = atoi((char*)&pToken->z[1]);
  p->iTable = pParse->nMem++;
  sqlite3VdbeAddOp(v, OP_Dup, depth, 0);
  sqlite3VdbeAddOp(v, OP_MemStore, p->iTable, 1);
  return p;
}

/*
** Join two expressions using an AND operator.  If either expression is
** NULL, then just return the other expression.
*/
Expr *sqlite3ExprAnd(Expr *pLeft, Expr *pRight){
  if( pLeft==0 ){
    return pRight;
  }else if( pRight==0 ){
    return pLeft;
  }else{
    return sqlite3Expr(TK_AND, pLeft, pRight, 0);
  }
}

/*
** Set the Expr.span field of the given expression to span all
** text between the two given tokens.
*/
void sqlite3ExprSpan(Expr *pExpr, Token *pLeft, Token *pRight){
  assert( pRight!=0 );
  assert( pLeft!=0 );
  if( !sqlite3MallocFailed() && pRight->z && pLeft->z ){
    assert( pLeft->dyn==0 || pLeft->z[pLeft->n]==0 );
    if( pLeft->dyn==0 && pRight->dyn==0 ){
      pExpr->span.z = pLeft->z;
      pExpr->span.n = pRight->n + (pRight->z - pLeft->z);
    }else{
      pExpr->span.z = 0;
    }
  }
}

/*
** Construct a new expression node for a function with multiple
** arguments.
*/
Expr *sqlite3ExprFunction(ExprList *pList, Token *pToken){
  Expr *pNew;
  assert( pToken );
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    sqlite3ExprListDelete(pList); /* Avoid leaking memory when malloc fails */
    return 0;
  }
  pNew->op = TK_FUNCTION;
  pNew->pList = pList;
  assert( pToken->dyn==0 );
  pNew->token = *pToken;
  pNew->span = pNew->token;
  return pNew;
}

/*
** Assign a variable number to an expression that encodes a wildcard
** in the original SQL statement.  
**
** Wildcards consisting of a single "?" are assigned the next sequential
** variable number.
**
** Wildcards of the form "?nnn" are assigned the number "nnn".  We make
** sure "nnn" is not too be to avoid a denial of service attack when
** the SQL statement comes from an external source.
**
** Wildcards of the form ":aaa" or "$aaa" are assigned the same number
** as the previous instance of the same wildcard.  Or if this is the first
** instance of the wildcard, the next sequenial variable number is
** assigned.
*/
void sqlite3ExprAssignVarNumber(Parse *pParse, Expr *pExpr){
  Token *pToken;
  if( pExpr==0 ) return;
  pToken = &pExpr->token;
  assert( pToken->n>=1 );
  assert( pToken->z!=0 );
  assert( pToken->z[0]!=0 );
  if( pToken->n==1 ){
    /* Wildcard of the form "?".  Assign the next variable number */
    pExpr->iTable = ++pParse->nVar;
  }else if( pToken->z[0]=='?' ){
    /* Wildcard of the form "?nnn".  Convert "nnn" to an integer and
    ** use it as the variable number */
    int i;
    pExpr->iTable = i = atoi((char*)&pToken->z[1]);
    if( i<1 || i>SQLITE_MAX_VARIABLE_NUMBER ){
      sqlite3ErrorMsg(pParse, "variable number must be between ?1 and ?%d",
          SQLITE_MAX_VARIABLE_NUMBER);
    }
    if( i>pParse->nVar ){
      pParse->nVar = i;
    }
  }else{
    /* Wildcards of the form ":aaa" or "$aaa".  Reuse the same variable
    ** number as the prior appearance of the same name, or if the name
    ** has never appeared before, reuse the same variable number
    */
    int i, n;
    n = pToken->n;
    for(i=0; i<pParse->nVarExpr; i++){
      Expr *pE;
      if( (pE = pParse->apVarExpr[i])!=0
          && pE->token.n==n
          && memcmp(pE->token.z, pToken->z, n)==0 ){
        pExpr->iTable = pE->iTable;
        break;
      }
    }
    if( i>=pParse->nVarExpr ){
      pExpr->iTable = ++pParse->nVar;
      if( pParse->nVarExpr>=pParse->nVarExprAlloc-1 ){
        pParse->nVarExprAlloc += pParse->nVarExprAlloc + 10;
        pParse->apVarExpr = sqliteReallocOrFree(pParse->apVarExpr,
                       pParse->nVarExprAlloc*sizeof(pParse->apVarExpr[0]) );
      }
      if( !sqlite3MallocFailed() ){
        assert( pParse->apVarExpr!=0 );
        pParse->apVarExpr[pParse->nVarExpr++] = pExpr;
      }
    }
  } 
}

/*
** Recursively delete an expression tree.
*/
void sqlite3ExprDelete(Expr *p){
  if( p==0 ) return;
  if( p->span.dyn ) sqliteFree((char*)p->span.z);
  if( p->token.dyn ) sqliteFree((char*)p->token.z);
  sqlite3ExprDelete(p->pLeft);
  sqlite3ExprDelete(p->pRight);
  sqlite3ExprListDelete(p->pList);
  sqlite3SelectDelete(p->pSelect);
  sqliteFree(p);
}

/*
** The Expr.token field might be a string literal that is quoted.
** If so, remove the quotation marks.
*/
void sqlite3DequoteExpr(Expr *p){
  if( ExprHasAnyProperty(p, EP_Dequoted) ){
    return;
  }
  ExprSetProperty(p, EP_Dequoted);
  if( p->token.dyn==0 ){
    sqlite3TokenCopy(&p->token, &p->token);
  }
  sqlite3Dequote((char*)p->token.z);
}


/*
** The following group of routines make deep copies of expressions,
** expression lists, ID lists, and select statements.  The copies can
** be deleted (by being passed to their respective ...Delete() routines)
** without effecting the originals.
**
** The expression list, ID, and source lists return by sqlite3ExprListDup(),
** sqlite3IdListDup(), and sqlite3SrcListDup() can not be further expanded 
** by subsequent calls to sqlite*ListAppend() routines.
**
** Any tables that the SrcList might point to are not duplicated.
*/
Expr *sqlite3ExprDup(Expr *p){
  Expr *pNew;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*p) );
  if( pNew==0 ) return 0;
  memcpy(pNew, p, sizeof(*pNew));
  if( p->token.z!=0 ){
    pNew->token.z = (u8*)sqliteStrNDup((char*)p->token.z, p->token.n);
    pNew->token.dyn = 1;
  }else{
    assert( pNew->token.z==0 );
  }
  pNew->span.z = 0;
  pNew->pLeft = sqlite3ExprDup(p->pLeft);
  pNew->pRight = sqlite3ExprDup(p->pRight);
  pNew->pList = sqlite3ExprListDup(p->pList);
  pNew->pSelect = sqlite3SelectDup(p->pSelect);
  pNew->pTab = p->pTab;
  return pNew;
}
void sqlite3TokenCopy(Token *pTo, Token *pFrom){
  if( pTo->dyn ) sqliteFree((char*)pTo->z);
  if( pFrom->z ){
    pTo->n = pFrom->n;
    pTo->z = (u8*)sqliteStrNDup((char*)pFrom->z, pFrom->n);
    pTo->dyn = 1;
  }else{
    pTo->z = 0;
  }
}
ExprList *sqlite3ExprListDup(ExprList *p){
  ExprList *pNew;
  struct ExprList_item *pItem, *pOldItem;
  int i;
  if( p==0 ) return 0;
  pNew = sqliteMalloc( sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nExpr = pNew->nAlloc = p->nExpr;
  pNew->a = pItem = sqliteMalloc( p->nExpr*sizeof(p->a[0]) );
  if( pItem==0 ){
    sqliteFree(pNew);
    return 0;
  } 
  pOldItem = p->a;
  for(i=0; i<p->nExpr; i++, pItem++, pOldItem++){
    Expr *pNewExpr, *pOldExpr;
    pItem->pExpr = pNewExpr = sqlite3ExprDup(pOldExpr = pOldItem->pExpr);
    if( pOldExpr->span.z!=0 && pNewExpr ){
      /* Always make a copy of the span for top-level expressions in the
      ** expression list.  The logic in SELECT processing that determines
      ** the names of columns in the result set needs this information */
      sqlite3TokenCopy(&pNewExpr->span, &pOldExpr->span);
    }
    assert( pNewExpr==0 || pNewExpr->span.z!=0 
            || pOldExpr->span.z==0
            || sqlite3MallocFailed() );
    pItem->zName = sqliteStrDup(pOldItem->zName);
    pItem->sortOrder = pOldItem->sortOrder;
    pItem->isAgg = pOldItem->isAgg;
    pItem->done = 0;
  }
  return pNew;
}

/*
** If cursors, triggers, views and subqueries are all omitted from
** the build, then none of the following routines, except for 
** sqlite3SelectDup(), can be called. sqlite3SelectDup() is sometimes
** called with a NULL argument.
*/
#if !defined(SQLITE_OMIT_VIEW) || !defined(SQLITE_OMIT_TRIGGER) \
 || !defined(SQLITE_OMIT_SUBQUERY)
SrcList *sqlite3SrcListDup(SrcList *p){
  SrcList *pNew;
  int i;
  int nByte;
  if( p==0 ) return 0;
  nByte = sizeof(*p) + (p->nSrc>0 ? sizeof(p->a[0]) * (p->nSrc-1) : 0);
  pNew = sqliteMallocRaw( nByte );
  if( pNew==0 ) return 0;
  pNew->nSrc = pNew->nAlloc = p->nSrc;
  for(i=0; i<p->nSrc; i++){
    struct SrcList_item *pNewItem = &pNew->a[i];
    struct SrcList_item *pOldItem = &p->a[i];
    Table *pTab;
    pNewItem->zDatabase = sqliteStrDup(pOldItem->zDatabase);
    pNewItem->zName = sqliteStrDup(pOldItem->zName);
    pNewItem->zAlias = sqliteStrDup(pOldItem->zAlias);
    pNewItem->jointype = pOldItem->jointype;
    pNewItem->iCursor = pOldItem->iCursor;
    pNewItem->isPopulated = pOldItem->isPopulated;
    pTab = pNewItem->pTab = pOldItem->pTab;
    if( pTab ){
      pTab->nRef++;
    }
    pNewItem->pSelect = sqlite3SelectDup(pOldItem->pSelect);
    pNewItem->pOn = sqlite3ExprDup(pOldItem->pOn);
    pNewItem->pUsing = sqlite3IdListDup(pOldItem->pUsing);
    pNewItem->colUsed = pOldItem->colUsed;
  }
  return pNew;
}
IdList *sqlite3IdListDup(IdList *p){
  IdList *pNew;
  int i;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nId = pNew->nAlloc = p->nId;
  pNew->a = sqliteMallocRaw( p->nId*sizeof(p->a[0]) );
  if( pNew->a==0 ){
    sqliteFree(pNew);
    return 0;
  }
  for(i=0; i<p->nId; i++){
    struct IdList_item *pNewItem = &pNew->a[i];
    struct IdList_item *pOldItem = &p->a[i];
    pNewItem->zName = sqliteStrDup(pOldItem->zName);
    pNewItem->idx = pOldItem->idx;
  }
  return pNew;
}
Select *sqlite3SelectDup(Select *p){
  Select *pNew;
  if( p==0 ) return 0;
  pNew = sqliteMallocRaw( sizeof(*p) );
  if( pNew==0 ) return 0;
  pNew->isDistinct = p->isDistinct;
  pNew->pEList = sqlite3ExprListDup(p->pEList);
  pNew->pSrc = sqlite3SrcListDup(p->pSrc);
  pNew->pWhere = sqlite3ExprDup(p->pWhere);
  pNew->pGroupBy = sqlite3ExprListDup(p->pGroupBy);
  pNew->pHaving = sqlite3ExprDup(p->pHaving);
  pNew->pOrderBy = sqlite3ExprListDup(p->pOrderBy);
  pNew->op = p->op;
  pNew->pPrior = sqlite3SelectDup(p->pPrior);
  pNew->pLimit = sqlite3ExprDup(p->pLimit);
  pNew->pOffset = sqlite3ExprDup(p->pOffset);
  pNew->iLimit = -1;
  pNew->iOffset = -1;
  pNew->isResolved = p->isResolved;
  pNew->isAgg = p->isAgg;
  pNew->usesEphm = 0;
  pNew->disallowOrderBy = 0;
  pNew->pRightmost = 0;
  pNew->addrOpenEphm[0] = -1;
  pNew->addrOpenEphm[1] = -1;
  pNew->addrOpenEphm[2] = -1;
  return pNew;
}
#else
Select *sqlite3SelectDup(Select *p){
  assert( p==0 );
  return 0;
}
#endif


/*
** Add a new element to the end of an expression list.  If pList is
** initially NULL, then create a new expression list.
*/
ExprList *sqlite3ExprListAppend(ExprList *pList, Expr *pExpr, Token *pName){
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(ExprList) );
    if( pList==0 ){
      goto no_mem;
    }
    assert( pList->nAlloc==0 );
  }
  if( pList->nAlloc<=pList->nExpr ){
    struct ExprList_item *a;
    int n = pList->nAlloc*2 + 4;
    a = sqliteRealloc(pList->a, n*sizeof(pList->a[0]));
    if( a==0 ){
      goto no_mem;
    }
    pList->a = a;
    pList->nAlloc = n;
  }
  assert( pList->a!=0 );
  if( pExpr || pName ){
    struct ExprList_item *pItem = &pList->a[pList->nExpr++];
    memset(pItem, 0, sizeof(*pItem));
    pItem->zName = sqlite3NameFromToken(pName);
    pItem->pExpr = pExpr;
  }
  return pList;

no_mem:     
  /* Avoid leaking memory if malloc has failed. */
  sqlite3ExprDelete(pExpr);
  sqlite3ExprListDelete(pList);
  return 0;
}

/*
** Delete an entire expression list.
*/
void sqlite3ExprListDelete(ExprList *pList){
  int i;
  struct ExprList_item *pItem;
  if( pList==0 ) return;
  assert( pList->a!=0 || (pList->nExpr==0 && pList->nAlloc==0) );
  assert( pList->nExpr<=pList->nAlloc );
  for(pItem=pList->a, i=0; i<pList->nExpr; i++, pItem++){
    sqlite3ExprDelete(pItem->pExpr);
    sqliteFree(pItem->zName);
  }
  sqliteFree(pList->a);
  sqliteFree(pList);
}

/*
** Walk an expression tree.  Call xFunc for each node visited.
**
** The return value from xFunc determines whether the tree walk continues.
** 0 means continue walking the tree.  1 means do not walk children
** of the current node but continue with siblings.  2 means abandon
** the tree walk completely.
**
** The return value from this routine is 1 to abandon the tree walk
** and 0 to continue.
**
** NOTICE:  This routine does *not* descend into subqueries.
*/
static int walkExprList(ExprList *, int (*)(void *, Expr*), void *);
static int walkExprTree(Expr *pExpr, int (*xFunc)(void*,Expr*), void *pArg){
  int rc;
  if( pExpr==0 ) return 0;
  rc = (*xFunc)(pArg, pExpr);
  if( rc==0 ){
    if( walkExprTree(pExpr->pLeft, xFunc, pArg) ) return 1;
    if( walkExprTree(pExpr->pRight, xFunc, pArg) ) return 1;
    if( walkExprList(pExpr->pList, xFunc, pArg) ) return 1;
  }
  return rc>1;
}

/*
** Call walkExprTree() for every expression in list p.
*/
static int walkExprList(ExprList *p, int (*xFunc)(void *, Expr*), void *pArg){
  int i;
  struct ExprList_item *pItem;
  if( !p ) return 0;
  for(i=p->nExpr, pItem=p->a; i>0; i--, pItem++){
    if( walkExprTree(pItem->pExpr, xFunc, pArg) ) return 1;
  }
  return 0;
}

/*
** Call walkExprTree() for every expression in Select p, not including
** expressions that are part of sub-selects in any FROM clause or the LIMIT
** or OFFSET expressions..
*/
static int walkSelectExpr(Select *p, int (*xFunc)(void *, Expr*), void *pArg){
  walkExprList(p->pEList, xFunc, pArg);
  walkExprTree(p->pWhere, xFunc, pArg);
  walkExprList(p->pGroupBy, xFunc, pArg);
  walkExprTree(p->pHaving, xFunc, pArg);
  walkExprList(p->pOrderBy, xFunc, pArg);
  return 0;
}


/*
** This routine is designed as an xFunc for walkExprTree().
**
** pArg is really a pointer to an integer.  If we can tell by looking
** at pExpr that the expression that contains pExpr is not a constant
** expression, then set *pArg to 0 and return 2 to abandon the tree walk.
** If pExpr does does not disqualify the expression from being a constant
** then do nothing.
**
** After walking the whole tree, if no nodes are found that disqualify
** the expression as constant, then we assume the whole expression
** is constant.  See sqlite3ExprIsConstant() for additional information.
*/
static int exprNodeIsConstant(void *pArg, Expr *pExpr){
  switch( pExpr->op ){
    /* Consider functions to be constant if all their arguments are constant
    ** and *pArg==2 */
    case TK_FUNCTION:
      if( *((int*)pArg)==2 ) return 0;
      /* Fall through */
    case TK_ID:
    case TK_COLUMN:
    case TK_DOT:
    case TK_AGG_FUNCTION:
    case TK_AGG_COLUMN:
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_SELECT:
    case TK_EXISTS:
#endif
      *((int*)pArg) = 0;
      return 2;
    case TK_IN:
      if( pExpr->pSelect ){
        *((int*)pArg) = 0;
        return 2;
      }
    default:
      return 0;
  }
}

/*
** Walk an expression tree.  Return 1 if the expression is constant
** and 0 if it involves variables or function calls.
**
** For the purposes of this function, a double-quoted string (ex: "abc")
** is considered a variable but a single-quoted string (ex: 'abc') is
** a constant.
*/
int sqlite3ExprIsConstant(Expr *p){
  int isConst = 1;
  walkExprTree(p, exprNodeIsConstant, &isConst);
  return isConst;
}

/*
** Walk an expression tree.  Return 1 if the expression is constant
** or a function call with constant arguments.  Return and 0 if there
** are any variables.
**
** For the purposes of this function, a double-quoted string (ex: "abc")
** is considered a variable but a single-quoted string (ex: 'abc') is
** a constant.
*/
int sqlite3ExprIsConstantOrFunction(Expr *p){
  int isConst = 2;
  walkExprTree(p, exprNodeIsConstant, &isConst);
  return isConst!=0;
}

/*
** If the expression p codes a constant integer that is small enough
** to fit in a 32-bit integer, return 1 and put the value of the integer
** in *pValue.  If the expression is not an integer or if it is too big
** to fit in a signed 32-bit integer, return 0 and leave *pValue unchanged.
*/
int sqlite3ExprIsInteger(Expr *p, int *pValue){
  switch( p->op ){
    case TK_INTEGER: {
      if( sqlite3GetInt32((char*)p->token.z, pValue) ){
        return 1;
      }
      break;
    }
    case TK_UPLUS: {
      return sqlite3ExprIsInteger(p->pLeft, pValue);
    }
    case TK_UMINUS: {
      int v;
      if( sqlite3ExprIsInteger(p->pLeft, &v) ){
        *pValue = -v;
        return 1;
      }
      break;
    }
    default: break;
  }
  return 0;
}

/*
** Return TRUE if the given string is a row-id column name.
*/
int sqlite3IsRowid(const char *z){
  if( sqlite3StrICmp(z, "_ROWID_")==0 ) return 1;
  if( sqlite3StrICmp(z, "ROWID")==0 ) return 1;
  if( sqlite3StrICmp(z, "OID")==0 ) return 1;
  return 0;
}

/*
** Given the name of a column of the form X.Y.Z or Y.Z or just Z, look up
** that name in the set of source tables in pSrcList and make the pExpr 
** expression node refer back to that source column.  The following changes
** are made to pExpr:
**
**    pExpr->iDb           Set the index in db->aDb[] of the database holding
**                         the table.
**    pExpr->iTable        Set to the cursor number for the table obtained
**                         from pSrcList.
**    pExpr->iColumn       Set to the column number within the table.
**    pExpr->op            Set to TK_COLUMN.
**    pExpr->pLeft         Any expression this points to is deleted
**    pExpr->pRight        Any expression this points to is deleted.
**
** The pDbToken is the name of the database (the "X").  This value may be
** NULL meaning that name is of the form Y.Z or Z.  Any available database
** can be used.  The pTableToken is the name of the table (the "Y").  This
** value can be NULL if pDbToken is also NULL.  If pTableToken is NULL it
** means that the form of the name is Z and that columns from any table
** can be used.
**
** If the name cannot be resolved unambiguously, leave an error message
** in pParse and return non-zero.  Return zero on success.
*/
static int lookupName(
  Parse *pParse,       /* The parsing context */
  Token *pDbToken,     /* Name of the database containing table, or NULL */
  Token *pTableToken,  /* Name of table containing column, or NULL */
  Token *pColumnToken, /* Name of the column. */
  NameContext *pNC,    /* The name context used to resolve the name */
  Expr *pExpr          /* Make this EXPR node point to the selected column */
){
  char *zDb = 0;       /* Name of the database.  The "X" in X.Y.Z */
  char *zTab = 0;      /* Name of the table.  The "Y" in X.Y.Z or Y.Z */
  char *zCol = 0;      /* Name of the column.  The "Z" */
  int i, j;            /* Loop counters */
  int cnt = 0;         /* Number of matching column names */
  int cntTab = 0;      /* Number of matching table names */
  sqlite3 *db = pParse->db;  /* The database */
  struct SrcList_item *pItem;       /* Use for looping over pSrcList items */
  struct SrcList_item *pMatch = 0;  /* The matching pSrcList item */
  NameContext *pTopNC = pNC;        /* First namecontext in the list */

  assert( pColumnToken && pColumnToken->z ); /* The Z in X.Y.Z cannot be NULL */
  zDb = sqlite3NameFromToken(pDbToken);
  zTab = sqlite3NameFromToken(pTableToken);
  zCol = sqlite3NameFromToken(pColumnToken);
  if( sqlite3MallocFailed() ){
    goto lookupname_end;
  }

  pExpr->iTable = -1;
  while( pNC && cnt==0 ){
    ExprList *pEList;
    SrcList *pSrcList = pNC->pSrcList;

    if( pSrcList ){
      for(i=0, pItem=pSrcList->a; i<pSrcList->nSrc; i++, pItem++){
        Table *pTab;
        int iDb;
        Column *pCol;
  
        pTab = pItem->pTab;
        assert( pTab!=0 );
        iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
        assert( pTab->nCol>0 );
        if( zTab ){
          if( pItem->zAlias ){
            char *zTabName = pItem->zAlias;
            if( sqlite3StrICmp(zTabName, zTab)!=0 ) continue;
          }else{
            char *zTabName = pTab->zName;
            if( zTabName==0 || sqlite3StrICmp(zTabName, zTab)!=0 ) continue;
            if( zDb!=0 && sqlite3StrICmp(db->aDb[iDb].zName, zDb)!=0 ){
              continue;
            }
          }
        }
        if( 0==(cntTab++) ){
          pExpr->iTable = pItem->iCursor;
          pExpr->pSchema = pTab->pSchema;
          pMatch = pItem;
        }
        for(j=0, pCol=pTab->aCol; j<pTab->nCol; j++, pCol++){
          if( sqlite3StrICmp(pCol->zName, zCol)==0 ){
            const char *zColl = pTab->aCol[j].zColl;
            IdList *pUsing;
            cnt++;
            pExpr->iTable = pItem->iCursor;
            pMatch = pItem;
            pExpr->pSchema = pTab->pSchema;
            /* Substitute the rowid (column -1) for the INTEGER PRIMARY KEY */
            pExpr->iColumn = j==pTab->iPKey ? -1 : j;
            pExpr->affinity = pTab->aCol[j].affinity;
            if( (pExpr->flags & EP_ExpCollate)==0 ){
              pExpr->pColl = sqlite3FindCollSeq(db, ENC(db), zColl,-1, 0);
            }
            if( i<pSrcList->nSrc-1 ){
              if( pItem[1].jointype & JT_NATURAL ){
                /* If this match occurred in the left table of a natural join,
                ** then skip the right table to avoid a duplicate match */
                pItem++;
                i++;
              }else if( (pUsing = pItem[1].pUsing)!=0 ){
                /* If this match occurs on a column that is in the USING clause
                ** of a join, skip the search of the right table of the join
                ** to avoid a duplicate match there. */
                int k;
                for(k=0; k<pUsing->nId; k++){
                  if( sqlite3StrICmp(pUsing->a[k].zName, zCol)==0 ){
                    pItem++;
                    i++;
                    break;
                  }
                }
              }
            }
            break;
          }
        }
      }
    }

#ifndef SQLITE_OMIT_TRIGGER
    /* If we have not already resolved the name, then maybe 
    ** it is a new.* or old.* trigger argument reference
    */
    if( zDb==0 && zTab!=0 && cnt==0 && pParse->trigStack!=0 ){
      TriggerStack *pTriggerStack = pParse->trigStack;
      Table *pTab = 0;
      if( pTriggerStack->newIdx != -1 && sqlite3StrICmp("new", zTab) == 0 ){
        pExpr->iTable = pTriggerStack->newIdx;
        assert( pTriggerStack->pTab );
        pTab = pTriggerStack->pTab;
      }else if( pTriggerStack->oldIdx != -1 && sqlite3StrICmp("old", zTab)==0 ){
        pExpr->iTable = pTriggerStack->oldIdx;
        assert( pTriggerStack->pTab );
        pTab = pTriggerStack->pTab;
      }

      if( pTab ){ 
        int iCol;
        Column *pCol = pTab->aCol;

        pExpr->pSchema = pTab->pSchema;
        cntTab++;
        for(iCol=0; iCol < pTab->nCol; iCol++, pCol++) {
          if( sqlite3StrICmp(pCol->zName, zCol)==0 ){
            const char *zColl = pTab->aCol[iCol].zColl;
            cnt++;
            pExpr->iColumn = iCol==pTab->iPKey ? -1 : iCol;
            pExpr->affinity = pTab->aCol[iCol].affinity;
            if( (pExpr->flags & EP_ExpCollate)==0 ){
              pExpr->pColl = sqlite3FindCollSeq(db, ENC(db), zColl,-1, 0);
            }
            pExpr->pTab = pTab;
            break;
          }
        }
      }
    }
#endif /* !defined(SQLITE_OMIT_TRIGGER) */

    /*
    ** Perhaps the name is a reference to the ROWID
    */
    if( cnt==0 && cntTab==1 && sqlite3IsRowid(zCol) ){
      cnt = 1;
      pExpr->iColumn = -1;
      pExpr->affinity = SQLITE_AFF_INTEGER;
    }

    /*
    ** If the input is of the form Z (not Y.Z or X.Y.Z) then the name Z
    ** might refer to an result-set alias.  This happens, for example, when
    ** we are resolving names in the WHERE clause of the following command:
    **
    **     SELECT a+b AS x FROM table WHERE x<10;
    **
    ** In cases like this, replace pExpr with a copy of the expression that
    ** forms the result set entry ("a+b" in the example) and return immediately.
    ** Note that the expression in the result set should have already been
    ** resolved by the time the WHERE clause is resolved.
    */
    if( cnt==0 && (pEList = pNC->pEList)!=0 && zTab==0 ){
      for(j=0; j<pEList->nExpr; j++){
        char *zAs = pEList->a[j].zName;
        if( zAs!=0 && sqlite3StrICmp(zAs, zCol)==0 ){
          assert( pExpr->pLeft==0 && pExpr->pRight==0 );
          pExpr->op = TK_AS;
          pExpr->iColumn = j;
          pExpr->pLeft = sqlite3ExprDup(pEList->a[j].pExpr);
          cnt = 1;
          assert( zTab==0 && zDb==0 );
          goto lookupname_end_2;
        }
      } 
    }

    /* Advance to the next name context.  The loop will exit when either
    ** we have a match (cnt>0) or when we run out of name contexts.
    */
    if( cnt==0 ){
      pNC = pNC->pNext;
    }
  }

  /*
  ** If X and Y are NULL (in other words if only the column name Z is
  ** supplied) and the value of Z is enclosed in double-quotes, then
  ** Z is a string literal if it doesn't match any column names.  In that
  ** case, we need to return right away and not make any changes to
  ** pExpr.
  **
  ** Because no reference was made to outer contexts, the pNC->nRef
  ** fields are not changed in any context.
  */
  if( cnt==0 && zTab==0 && pColumnToken->z[0]=='"' ){
    sqliteFree(zCol);
    return 0;
  }

  /*
  ** cnt==0 means there was not match.  cnt>1 means there were two or
  ** more matches.  Either way, we have an error.
  */
  if( cnt!=1 ){
    char *z = 0;
    char *zErr;
    zErr = cnt==0 ? "no such column: %s" : "ambiguous column name: %s";
    if( zDb ){
      sqlite3SetString(&z, zDb, ".", zTab, ".", zCol, (char*)0);
    }else if( zTab ){
      sqlite3SetString(&z, zTab, ".", zCol, (char*)0);
    }else{
      z = sqliteStrDup(zCol);
    }
    sqlite3ErrorMsg(pParse, zErr, z);
    sqliteFree(z);
    pTopNC->nErr++;
  }

  /* If a column from a table in pSrcList is referenced, then record
  ** this fact in the pSrcList.a[].colUsed bitmask.  Column 0 causes
  ** bit 0 to be set.  Column 1 sets bit 1.  And so forth.  If the
  ** column number is greater than the number of bits in the bitmask
  ** then set the high-order bit of the bitmask.
  */
  if( pExpr->iColumn>=0 && pMatch!=0 ){
    int n = pExpr->iColumn;
    if( n>=sizeof(Bitmask)*8 ){
      n = sizeof(Bitmask)*8-1;
    }
    assert( pMatch->iCursor==pExpr->iTable );
    pMatch->colUsed |= ((Bitmask)1)<<n;
  }

lookupname_end:
  /* Clean up and return
  */
  sqliteFree(zDb);
  sqliteFree(zTab);
  sqlite3ExprDelete(pExpr->pLeft);
  pExpr->pLeft = 0;
  sqlite3ExprDelete(pExpr->pRight);
  pExpr->pRight = 0;
  pExpr->op = TK_COLUMN;
lookupname_end_2:
  sqliteFree(zCol);
  if( cnt==1 ){
    assert( pNC!=0 );
    sqlite3AuthRead(pParse, pExpr, pNC->pSrcList);
    if( pMatch && !pMatch->pSelect ){
      pExpr->pTab = pMatch->pTab;
    }
    /* Increment the nRef value on all name contexts from TopNC up to
    ** the point where the name matched. */
    for(;;){
      assert( pTopNC!=0 );
      pTopNC->nRef++;
      if( pTopNC==pNC ) break;
      pTopNC = pTopNC->pNext;
    }
    return 0;
  } else {
    return 1;
  }
}

/*
** This routine is designed as an xFunc for walkExprTree().
**
** Resolve symbolic names into TK_COLUMN operators for the current
** node in the expression tree.  Return 0 to continue the search down
** the tree or 2 to abort the tree walk.
**
** This routine also does error checking and name resolution for
** function names.  The operator for aggregate functions is changed
** to TK_AGG_FUNCTION.
*/
static int nameResolverStep(void *pArg, Expr *pExpr){
  NameContext *pNC = (NameContext*)pArg;
  Parse *pParse;

  if( pExpr==0 ) return 1;
  assert( pNC!=0 );
  pParse = pNC->pParse;

  if( ExprHasAnyProperty(pExpr, EP_Resolved) ) return 1;
  ExprSetProperty(pExpr, EP_Resolved);
#ifndef NDEBUG
  if( pNC->pSrcList && pNC->pSrcList->nAlloc>0 ){
    SrcList *pSrcList = pNC->pSrcList;
    int i;
    for(i=0; i<pNC->pSrcList->nSrc; i++){
      assert( pSrcList->a[i].iCursor>=0 && pSrcList->a[i].iCursor<pParse->nTab);
    }
  }
#endif
  switch( pExpr->op ){
    /* Double-quoted strings (ex: "abc") are used as identifiers if
    ** possible.  Otherwise they remain as strings.  Single-quoted
    ** strings (ex: 'abc') are always string literals.
    */
    case TK_STRING: {
      if( pExpr->token.z[0]=='\'' ) break;
      /* Fall thru into the TK_ID case if this is a double-quoted string */
    }
    /* A lone identifier is the name of a column.
    */
    case TK_ID: {
      lookupName(pParse, 0, 0, &pExpr->token, pNC, pExpr);
      return 1;
    }
  
    /* A table name and column name:     ID.ID
    ** Or a database, table and column:  ID.ID.ID
    */
    case TK_DOT: {
      Token *pColumn;
      Token *pTable;
      Token *pDb;
      Expr *pRight;

      /* if( pSrcList==0 ) break; */
      pRight = pExpr->pRight;
      if( pRight->op==TK_ID ){
        pDb = 0;
        pTable = &pExpr->pLeft->token;
        pColumn = &pRight->token;
      }else{
        assert( pRight->op==TK_DOT );
        pDb = &pExpr->pLeft->token;
        pTable = &pRight->pLeft->token;
        pColumn = &pRight->pRight->token;
      }
      lookupName(pParse, pDb, pTable, pColumn, pNC, pExpr);
      return 1;
    }

    /* Resolve function names
    */
    case TK_CONST_FUNC:
    case TK_FUNCTION: {
      ExprList *pList = pExpr->pList;    /* The argument list */
      int n = pList ? pList->nExpr : 0;  /* Number of arguments */
      int no_such_func = 0;       /* True if no such function exists */
      int wrong_num_args = 0;     /* True if wrong number of arguments */
      int is_agg = 0;             /* True if is an aggregate function */
      int i;
      int auth;                   /* Authorization to use the function */
      int nId;                    /* Number of characters in function name */
      const char *zId;            /* The function name. */
      FuncDef *pDef;              /* Information about the function */
      int enc = ENC(pParse->db);  /* The database encoding */

      zId = (char*)pExpr->token.z;
      nId = pExpr->token.n;
      pDef = sqlite3FindFunction(pParse->db, zId, nId, n, enc, 0);
      if( pDef==0 ){
        pDef = sqlite3FindFunction(pParse->db, zId, nId, -1, enc, 0);
        if( pDef==0 ){
          no_such_func = 1;
        }else{
          wrong_num_args = 1;
        }
      }else{
        is_agg = pDef->xFunc==0;
      }
#ifndef SQLITE_OMIT_AUTHORIZATION
      if( pDef ){
        auth = sqlite3AuthCheck(pParse, SQLITE_FUNCTION, 0, pDef->zName, 0);
        if( auth!=SQLITE_OK ){
          if( auth==SQLITE_DENY ){
            sqlite3ErrorMsg(pParse, "not authorized to use function: %s",
                                    pDef->zName);
            pNC->nErr++;
          }
          pExpr->op = TK_NULL;
          return 1;
        }
      }
#endif
      if( is_agg && !pNC->allowAgg ){
        sqlite3ErrorMsg(pParse, "misuse of aggregate function %.*s()", nId,zId);
        pNC->nErr++;
        is_agg = 0;
      }else if( no_such_func ){
        sqlite3ErrorMsg(pParse, "no such function: %.*s", nId, zId);
        pNC->nErr++;
      }else if( wrong_num_args ){
        sqlite3ErrorMsg(pParse,"wrong number of arguments to function %.*s()",
             nId, zId);
        pNC->nErr++;
      }
      if( is_agg ){
        pExpr->op = TK_AGG_FUNCTION;
        pNC->hasAgg = 1;
      }
      if( is_agg ) pNC->allowAgg = 0;
      for(i=0; pNC->nErr==0 && i<n; i++){
        walkExprTree(pList->a[i].pExpr, nameResolverStep, pNC);
      }
      if( is_agg ) pNC->allowAgg = 1;
      /* FIX ME:  Compute pExpr->affinity based on the expected return
      ** type of the function 
      */
      return is_agg;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_SELECT:
    case TK_EXISTS:
#endif
    case TK_IN: {
      if( pExpr->pSelect ){
        int nRef = pNC->nRef;
#ifndef SQLITE_OMIT_CHECK
        if( pNC->isCheck ){
          sqlite3ErrorMsg(pParse,"subqueries prohibited in CHECK constraints");
        }
#endif
        sqlite3SelectResolve(pParse, pExpr->pSelect, pNC);
        assert( pNC->nRef>=nRef );
        if( nRef!=pNC->nRef ){
          ExprSetProperty(pExpr, EP_VarSelect);
        }
      }
      break;
    }
#ifndef SQLITE_OMIT_CHECK
    case TK_VARIABLE: {
      if( pNC->isCheck ){
        sqlite3ErrorMsg(pParse,"parameters prohibited in CHECK constraints");
      }
      break;
    }
#endif
  }
  return 0;
}

/*
** This routine walks an expression tree and resolves references to
** table columns.  Nodes of the form ID.ID or ID resolve into an
** index to the table in the table list and a column offset.  The 
** Expr.opcode for such nodes is changed to TK_COLUMN.  The Expr.iTable
** value is changed to the index of the referenced table in pTabList
** plus the "base" value.  The base value will ultimately become the
** VDBE cursor number for a cursor that is pointing into the referenced
** table.  The Expr.iColumn value is changed to the index of the column 
** of the referenced table.  The Expr.iColumn value for the special
** ROWID column is -1.  Any INTEGER PRIMARY KEY column is tried as an
** alias for ROWID.
**
** Also resolve function names and check the functions for proper
** usage.  Make sure all function names are recognized and all functions
** have the correct number of arguments.  Leave an error message
** in pParse->zErrMsg if anything is amiss.  Return the number of errors.
**
** If the expression contains aggregate functions then set the EP_Agg
** property on the expression.
*/
int sqlite3ExprResolveNames(
  NameContext *pNC,       /* Namespace to resolve expressions in. */
  Expr *pExpr             /* The expression to be analyzed. */
){
  int savedHasAgg;
  if( pExpr==0 ) return 0;
  savedHasAgg = pNC->hasAgg;
  pNC->hasAgg = 0;
  walkExprTree(pExpr, nameResolverStep, pNC);
  if( pNC->nErr>0 ){
    ExprSetProperty(pExpr, EP_Error);
  }
  if( pNC->hasAgg ){
    ExprSetProperty(pExpr, EP_Agg);
  }else if( savedHasAgg ){
    pNC->hasAgg = 1;
  }
  return ExprHasProperty(pExpr, EP_Error);
}

/*
** A pointer instance of this structure is used to pass information
** through walkExprTree into codeSubqueryStep().
*/
typedef struct QueryCoder QueryCoder;
struct QueryCoder {
  Parse *pParse;       /* The parsing context */
  NameContext *pNC;    /* Namespace of first enclosing query */
};


/*
** Generate code for scalar subqueries used as an expression
** and IN operators.  Examples:
**
**     (SELECT a FROM b)          -- subquery
**     EXISTS (SELECT a FROM b)   -- EXISTS subquery
**     x IN (4,5,11)              -- IN operator with list on right-hand side
**     x IN (SELECT a FROM b)     -- IN operator with subquery on the right
**
** The pExpr parameter describes the expression that contains the IN
** operator or subquery.
*/
#ifndef SQLITE_OMIT_SUBQUERY
void sqlite3CodeSubselect(Parse *pParse, Expr *pExpr){
  int testAddr = 0;                       /* One-time test address */
  Vdbe *v = sqlite3GetVdbe(pParse);
  if( v==0 ) return;

  /* This code must be run in its entirety every time it is encountered
  ** if any of the following is true:
  **
  **    *  The right-hand side is a correlated subquery
  **    *  The right-hand side is an expression list containing variables
  **    *  We are inside a trigger
  **
  ** If all of the above are false, then we can run this code just once
  ** save the results, and reuse the same result on subsequent invocations.
  */
  if( !ExprHasAnyProperty(pExpr, EP_VarSelect) && !pParse->trigStack ){
    int mem = pParse->nMem++;
    sqlite3VdbeAddOp(v, OP_MemLoad, mem, 0);
    testAddr = sqlite3VdbeAddOp(v, OP_If, 0, 0);
    assert( testAddr>0 || sqlite3MallocFailed() );
    sqlite3VdbeAddOp(v, OP_MemInt, 1, mem);
  }

  switch( pExpr->op ){
    case TK_IN: {
      char affinity;
      KeyInfo keyInfo;
      int addr;        /* Address of OP_OpenEphemeral instruction */

      affinity = sqlite3ExprAffinity(pExpr->pLeft);

      /* Whether this is an 'x IN(SELECT...)' or an 'x IN(<exprlist>)'
      ** expression it is handled the same way. A virtual table is 
      ** filled with single-field index keys representing the results
      ** from the SELECT or the <exprlist>.
      **
      ** If the 'x' expression is a column value, or the SELECT...
      ** statement returns a column value, then the affinity of that
      ** column is used to build the index keys. If both 'x' and the
      ** SELECT... statement are columns, then numeric affinity is used
      ** if either column has NUMERIC or INTEGER affinity. If neither
      ** 'x' nor the SELECT... statement are columns, then numeric affinity
      ** is used.
      */
      pExpr->iTable = pParse->nTab++;
      addr = sqlite3VdbeAddOp(v, OP_OpenEphemeral, pExpr->iTable, 0);
      memset(&keyInfo, 0, sizeof(keyInfo));
      keyInfo.nField = 1;
      sqlite3VdbeAddOp(v, OP_SetNumColumns, pExpr->iTable, 1);

      if( pExpr->pSelect ){
        /* Case 1:     expr IN (SELECT ...)
        **
        ** Generate code to write the results of the select into the temporary
        ** table allocated and opened above.
        */
        int iParm = pExpr->iTable +  (((int)affinity)<<16);
        ExprList *pEList;
        assert( (pExpr->iTable&0x0000FFFF)==pExpr->iTable );
        if( sqlite3Select(pParse, pExpr->pSelect, SRT_Set, iParm, 0, 0, 0, 0) ){
          return;
        }
        pEList = pExpr->pSelect->pEList;
        if( pEList && pEList->nExpr>0 ){ 
          keyInfo.aColl[0] = binaryCompareCollSeq(pParse, pExpr->pLeft,
              pEList->a[0].pExpr);
        }
      }else if( pExpr->pList ){
        /* Case 2:     expr IN (exprlist)
        **
	** For each expression, build an index key from the evaluation and
        ** store it in the temporary table. If <expr> is a column, then use
        ** that columns affinity when building index keys. If <expr> is not
        ** a column, use numeric affinity.
        */
        int i;
        ExprList *pList = pExpr->pList;
        struct ExprList_item *pItem;

        if( !affinity ){
          affinity = SQLITE_AFF_NONE;
        }
        keyInfo.aColl[0] = pExpr->pLeft->pColl;

        /* Loop through each expression in <exprlist>. */
        for(i=pList->nExpr, pItem=pList->a; i>0; i--, pItem++){
          Expr *pE2 = pItem->pExpr;

          /* If the expression is not constant then we will need to
          ** disable the test that was generated above that makes sure
          ** this code only executes once.  Because for a non-constant
          ** expression we need to rerun this code each time.
          */
          if( testAddr>0 && !sqlite3ExprIsConstant(pE2) ){
            sqlite3VdbeChangeToNoop(v, testAddr-1, 3);
            testAddr = 0;
          }

          /* Evaluate the expression and insert it into the temp table */
          sqlite3ExprCode(pParse, pE2);
          sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &affinity, 1);
          sqlite3VdbeAddOp(v, OP_IdxInsert, pExpr->iTable, 0);
        }
      }
      sqlite3VdbeChangeP3(v, addr, (void *)&keyInfo, P3_KEYINFO);
      break;
    }

    case TK_EXISTS:
    case TK_SELECT: {
      /* This has to be a scalar SELECT.  Generate code to put the
      ** value of this select in a memory cell and record the number
      ** of the memory cell in iColumn.
      */
      static const Token one = { (u8*)"1", 0, 1 };
      Select *pSel;
      int iMem;
      int sop;

      pExpr->iColumn = iMem = pParse->nMem++;
      pSel = pExpr->pSelect;
      if( pExpr->op==TK_SELECT ){
        sop = SRT_Mem;
        sqlite3VdbeAddOp(v, OP_MemNull, iMem, 0);
        VdbeComment((v, "# Init subquery result"));
      }else{
        sop = SRT_Exists;
        sqlite3VdbeAddOp(v, OP_MemInt, 0, iMem);
        VdbeComment((v, "# Init EXISTS result"));
      }
      sqlite3ExprDelete(pSel->pLimit);
      pSel->pLimit = sqlite3Expr(TK_INTEGER, 0, 0, &one);
      if( sqlite3Select(pParse, pSel, sop, iMem, 0, 0, 0, 0) ){
        return;
      }
      break;
    }
  }

  if( testAddr ){
    sqlite3VdbeJumpHere(v, testAddr);
  }
  return;
}
#endif /* SQLITE_OMIT_SUBQUERY */

/*
** Generate an instruction that will put the integer describe by
** text z[0..n-1] on the stack.
*/
static void codeInteger(Vdbe *v, const char *z, int n){
  int i;
  if( sqlite3GetInt32(z, &i) ){
    sqlite3VdbeAddOp(v, OP_Integer, i, 0);
  }else if( sqlite3FitsIn64Bits(z) ){
    sqlite3VdbeOp3(v, OP_Int64, 0, 0, z, n);
  }else{
    sqlite3VdbeOp3(v, OP_Real, 0, 0, z, n);
  }
}


/*
** Generate code that will extract the iColumn-th column from
** table pTab and push that column value on the stack.  There
** is an open cursor to pTab in iTable.  If iColumn<0 then
** code is generated that extracts the rowid.
*/
void sqlite3ExprCodeGetColumn(Vdbe *v, Table *pTab, int iColumn, int iTable){
  if( iColumn<0 ){
    int op = (pTab && IsVirtual(pTab)) ? OP_VRowid : OP_Rowid;
    sqlite3VdbeAddOp(v, op, iTable, 0);
  }else if( pTab==0 ){
    sqlite3VdbeAddOp(v, OP_Column, iTable, iColumn);
  }else{
    int op = IsVirtual(pTab) ? OP_VColumn : OP_Column;
    sqlite3VdbeAddOp(v, op, iTable, iColumn);
    sqlite3ColumnDefault(v, pTab, iColumn);
#ifndef SQLITE_OMIT_FLOATING_POINT
    if( pTab->aCol[iColumn].affinity==SQLITE_AFF_REAL ){
      sqlite3VdbeAddOp(v, OP_RealAffinity, 0, 0);
    }
#endif
  }
}

/*
** Generate code into the current Vdbe to evaluate the given
** expression and leave the result on the top of stack.
**
** This code depends on the fact that certain token values (ex: TK_EQ)
** are the same as opcode values (ex: OP_Eq) that implement the corresponding
** operation.  Special comments in vdbe.c and the mkopcodeh.awk script in
** the make process cause these values to align.  Assert()s in the code
** below verify that the numbers are aligned correctly.
*/
void sqlite3ExprCode(Parse *pParse, Expr *pExpr){
  Vdbe *v = pParse->pVdbe;
  int op;
  int stackChng = 1;    /* Amount of change to stack depth */

  if( v==0 ) return;
  if( pExpr==0 ){
    sqlite3VdbeAddOp(v, OP_Null, 0, 0);
    return;
  }
  op = pExpr->op;
  switch( op ){
    case TK_AGG_COLUMN: {
      AggInfo *pAggInfo = pExpr->pAggInfo;
      struct AggInfo_col *pCol = &pAggInfo->aCol[pExpr->iAgg];
      if( !pAggInfo->directMode ){
        sqlite3VdbeAddOp(v, OP_MemLoad, pCol->iMem, 0);
        break;
      }else if( pAggInfo->useSortingIdx ){
        sqlite3VdbeAddOp(v, OP_Column, pAggInfo->sortingIdx,
                              pCol->iSorterColumn);
        break;
      }
      /* Otherwise, fall thru into the TK_COLUMN case */
    }
    case TK_COLUMN: {
      if( pExpr->iTable<0 ){
        /* This only happens when coding check constraints */
        assert( pParse->ckOffset>0 );
        sqlite3VdbeAddOp(v, OP_Dup, pParse->ckOffset-pExpr->iColumn-1, 1);
      }else{
        sqlite3ExprCodeGetColumn(v, pExpr->pTab, pExpr->iColumn, pExpr->iTable);
      }
      break;
    }
    case TK_INTEGER: {
      codeInteger(v, (char*)pExpr->token.z, pExpr->token.n);
      break;
    }
    case TK_FLOAT:
    case TK_STRING: {
      assert( TK_FLOAT==OP_Real );
      assert( TK_STRING==OP_String8 );
      sqlite3DequoteExpr(pExpr);
      sqlite3VdbeOp3(v, op, 0, 0, (char*)pExpr->token.z, pExpr->token.n);
      break;
    }
    case TK_NULL: {
      sqlite3VdbeAddOp(v, OP_Null, 0, 0);
      break;
    }
#ifndef SQLITE_OMIT_BLOB_LITERAL
    case TK_BLOB: {
      int n;
      const char *z;
      assert( TK_BLOB==OP_HexBlob );
      n = pExpr->token.n - 3;
      z = (char*)pExpr->token.z + 2;
      assert( n>=0 );
      if( n==0 ){
        z = "";
      }
      sqlite3VdbeOp3(v, op, 0, 0, z, n);
      break;
    }
#endif
    case TK_VARIABLE: {
      sqlite3VdbeAddOp(v, OP_Variable, pExpr->iTable, 0);
      if( pExpr->token.n>1 ){
        sqlite3VdbeChangeP3(v, -1, (char*)pExpr->token.z, pExpr->token.n);
      }
      break;
    }
    case TK_REGISTER: {
      sqlite3VdbeAddOp(v, OP_MemLoad, pExpr->iTable, 0);
      break;
    }
#ifndef SQLITE_OMIT_CAST
    case TK_CAST: {
      /* Expressions of the form:   CAST(pLeft AS token) */
      int aff, to_op;
      sqlite3ExprCode(pParse, pExpr->pLeft);
      aff = sqlite3AffinityType(&pExpr->token);
      to_op = aff - SQLITE_AFF_TEXT + OP_ToText;
      assert( to_op==OP_ToText    || aff!=SQLITE_AFF_TEXT    );
      assert( to_op==OP_ToBlob    || aff!=SQLITE_AFF_NONE    );
      assert( to_op==OP_ToNumeric || aff!=SQLITE_AFF_NUMERIC );
      assert( to_op==OP_ToInt     || aff!=SQLITE_AFF_INTEGER );
      assert( to_op==OP_ToReal    || aff!=SQLITE_AFF_REAL    );
      sqlite3VdbeAddOp(v, to_op, 0, 0);
      stackChng = 0;
      break;
    }
#endif /* SQLITE_OMIT_CAST */
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      assert( TK_LT==OP_Lt );
      assert( TK_LE==OP_Le );
      assert( TK_GT==OP_Gt );
      assert( TK_GE==OP_Ge );
      assert( TK_EQ==OP_Eq );
      assert( TK_NE==OP_Ne );
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3ExprCode(pParse, pExpr->pRight);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op, 0, 0);
      stackChng = -1;
      break;
    }
    case TK_AND:
    case TK_OR:
    case TK_PLUS:
    case TK_STAR:
    case TK_MINUS:
    case TK_REM:
    case TK_BITAND:
    case TK_BITOR:
    case TK_SLASH:
    case TK_LSHIFT:
    case TK_RSHIFT: 
    case TK_CONCAT: {
      assert( TK_AND==OP_And );
      assert( TK_OR==OP_Or );
      assert( TK_PLUS==OP_Add );
      assert( TK_MINUS==OP_Subtract );
      assert( TK_REM==OP_Remainder );
      assert( TK_BITAND==OP_BitAnd );
      assert( TK_BITOR==OP_BitOr );
      assert( TK_SLASH==OP_Divide );
      assert( TK_LSHIFT==OP_ShiftLeft );
      assert( TK_RSHIFT==OP_ShiftRight );
      assert( TK_CONCAT==OP_Concat );
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3ExprCode(pParse, pExpr->pRight);
      sqlite3VdbeAddOp(v, op, 0, 0);
      stackChng = -1;
      break;
    }
    case TK_UMINUS: {
      Expr *pLeft = pExpr->pLeft;
      assert( pLeft );
      if( pLeft->op==TK_FLOAT || pLeft->op==TK_INTEGER ){
        Token *p = &pLeft->token;
        char *z = sqlite3MPrintf("-%.*s", p->n, p->z);
        if( pLeft->op==TK_FLOAT ){
          sqlite3VdbeOp3(v, OP_Real, 0, 0, z, p->n+1);
        }else{
          codeInteger(v, z, p->n+1);
        }
        sqliteFree(z);
        break;
      }
      /* Fall through into TK_NOT */
    }
    case TK_BITNOT:
    case TK_NOT: {
      assert( TK_BITNOT==OP_BitNot );
      assert( TK_NOT==OP_Not );
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3VdbeAddOp(v, op, 0, 0);
      stackChng = 0;
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      int dest;
      assert( TK_ISNULL==OP_IsNull );
      assert( TK_NOTNULL==OP_NotNull );
      sqlite3VdbeAddOp(v, OP_Integer, 1, 0);
      sqlite3ExprCode(pParse, pExpr->pLeft);
      dest = sqlite3VdbeCurrentAddr(v) + 2;
      sqlite3VdbeAddOp(v, op, 1, dest);
      sqlite3VdbeAddOp(v, OP_AddImm, -1, 0);
      stackChng = 0;
      break;
    }
    case TK_AGG_FUNCTION: {
      AggInfo *pInfo = pExpr->pAggInfo;
      if( pInfo==0 ){
        sqlite3ErrorMsg(pParse, "misuse of aggregate: %T",
            &pExpr->span);
      }else{
        sqlite3VdbeAddOp(v, OP_MemLoad, pInfo->aFunc[pExpr->iAgg].iMem, 0);
      }
      break;
    }
    case TK_CONST_FUNC:
    case TK_FUNCTION: {
      ExprList *pList = pExpr->pList;
      int nExpr = pList ? pList->nExpr : 0;
      FuncDef *pDef;
      int nId;
      const char *zId;
      int constMask = 0;
      int i;
      u8 enc = ENC(pParse->db);
      CollSeq *pColl = 0;
      zId = (char*)pExpr->token.z;
      nId = pExpr->token.n;
      pDef = sqlite3FindFunction(pParse->db, zId, nId, nExpr, enc, 0);
      assert( pDef!=0 );
      nExpr = sqlite3ExprCodeExprList(pParse, pList);
#ifndef SQLITE_OMIT_VIRTUALTABLE
      /* Possibly overload the function if the first argument is
      ** a virtual table column.
      **
      ** For infix functions (LIKE, GLOB, REGEXP, and MATCH) use the
      ** second argument, not the first, as the argument to test to
      ** see if it is a column in a virtual table.  This is done because
      ** the left operand of infix functions (the operand we want to
      ** control overloading) ends up as the second argument to the
      ** function.  The expression "A glob B" is equivalent to 
      ** "glob(B,A).  We want to use the A in "A glob B" to test
      ** for function overloading.  But we use the B term in "glob(B,A)".
      */
      if( nExpr>=2 && (pExpr->flags & EP_InfixFunc) ){
        pDef = sqlite3VtabOverloadFunction(pDef, nExpr, pList->a[1].pExpr);
      }else if( nExpr>0 ){
        pDef = sqlite3VtabOverloadFunction(pDef, nExpr, pList->a[0].pExpr);
      }
#endif
      for(i=0; i<nExpr && i<32; i++){
        if( sqlite3ExprIsConstant(pList->a[i].pExpr) ){
          constMask |= (1<<i);
        }
        if( pDef->needCollSeq && !pColl ){
          pColl = sqlite3ExprCollSeq(pParse, pList->a[i].pExpr);
        }
      }
      if( pDef->needCollSeq ){
        if( !pColl ) pColl = pParse->db->pDfltColl; 
        sqlite3VdbeOp3(v, OP_CollSeq, 0, 0, (char *)pColl, P3_COLLSEQ);
      }
      sqlite3VdbeOp3(v, OP_Function, constMask, nExpr, (char*)pDef, P3_FUNCDEF);
      stackChng = 1-nExpr;
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_EXISTS:
    case TK_SELECT: {
      if( pExpr->iColumn==0 ){
        sqlite3CodeSubselect(pParse, pExpr);
      }
      sqlite3VdbeAddOp(v, OP_MemLoad, pExpr->iColumn, 0);
      VdbeComment((v, "# load subquery result"));
      break;
    }
    case TK_IN: {
      int addr;
      char affinity;
      int ckOffset = pParse->ckOffset;
      sqlite3CodeSubselect(pParse, pExpr);

      /* Figure out the affinity to use to create a key from the results
      ** of the expression. affinityStr stores a static string suitable for
      ** P3 of OP_MakeRecord.
      */
      affinity = comparisonAffinity(pExpr);

      sqlite3VdbeAddOp(v, OP_Integer, 1, 0);
      pParse->ckOffset = ckOffset+1;

      /* Code the <expr> from "<expr> IN (...)". The temporary table
      ** pExpr->iTable contains the values that make up the (...) set.
      */
      sqlite3ExprCode(pParse, pExpr->pLeft);
      addr = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_NotNull, -1, addr+4);            /* addr + 0 */
      sqlite3VdbeAddOp(v, OP_Pop, 2, 0);
      sqlite3VdbeAddOp(v, OP_Null, 0, 0);
      sqlite3VdbeAddOp(v, OP_Goto, 0, addr+7);
      sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &affinity, 1);   /* addr + 4 */
      sqlite3VdbeAddOp(v, OP_Found, pExpr->iTable, addr+7);
      sqlite3VdbeAddOp(v, OP_AddImm, -1, 0);                  /* addr + 6 */

      break;
    }
#endif
    case TK_BETWEEN: {
      Expr *pLeft = pExpr->pLeft;
      struct ExprList_item *pLItem = pExpr->pList->a;
      Expr *pRight = pLItem->pExpr;
      sqlite3ExprCode(pParse, pLeft);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3ExprCode(pParse, pRight);
      codeCompare(pParse, pLeft, pRight, OP_Ge, 0, 0);
      sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
      pLItem++;
      pRight = pLItem->pExpr;
      sqlite3ExprCode(pParse, pRight);
      codeCompare(pParse, pLeft, pRight, OP_Le, 0, 0);
      sqlite3VdbeAddOp(v, OP_And, 0, 0);
      break;
    }
    case TK_UPLUS:
    case TK_AS: {
      sqlite3ExprCode(pParse, pExpr->pLeft);
      stackChng = 0;
      break;
    }
    case TK_CASE: {
      int expr_end_label;
      int jumpInst;
      int nExpr;
      int i;
      ExprList *pEList;
      struct ExprList_item *aListelem;

      assert(pExpr->pList);
      assert((pExpr->pList->nExpr % 2) == 0);
      assert(pExpr->pList->nExpr > 0);
      pEList = pExpr->pList;
      aListelem = pEList->a;
      nExpr = pEList->nExpr;
      expr_end_label = sqlite3VdbeMakeLabel(v);
      if( pExpr->pLeft ){
        sqlite3ExprCode(pParse, pExpr->pLeft);
      }
      for(i=0; i<nExpr; i=i+2){
        sqlite3ExprCode(pParse, aListelem[i].pExpr);
        if( pExpr->pLeft ){
          sqlite3VdbeAddOp(v, OP_Dup, 1, 1);
          jumpInst = codeCompare(pParse, pExpr->pLeft, aListelem[i].pExpr,
                                 OP_Ne, 0, 1);
          sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
        }else{
          jumpInst = sqlite3VdbeAddOp(v, OP_IfNot, 1, 0);
        }
        sqlite3ExprCode(pParse, aListelem[i+1].pExpr);
        sqlite3VdbeAddOp(v, OP_Goto, 0, expr_end_label);
        sqlite3VdbeJumpHere(v, jumpInst);
      }
      if( pExpr->pLeft ){
        sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      }
      if( pExpr->pRight ){
        sqlite3ExprCode(pParse, pExpr->pRight);
      }else{
        sqlite3VdbeAddOp(v, OP_Null, 0, 0);
      }
      sqlite3VdbeResolveLabel(v, expr_end_label);
      break;
    }
#ifndef SQLITE_OMIT_TRIGGER
    case TK_RAISE: {
      if( !pParse->trigStack ){
        sqlite3ErrorMsg(pParse,
                       "RAISE() may only be used within a trigger-program");
	return;
      }
      if( pExpr->iColumn!=OE_Ignore ){
         assert( pExpr->iColumn==OE_Rollback ||
                 pExpr->iColumn == OE_Abort ||
                 pExpr->iColumn == OE_Fail );
         sqlite3DequoteExpr(pExpr);
         sqlite3VdbeOp3(v, OP_Halt, SQLITE_CONSTRAINT, pExpr->iColumn,
                        (char*)pExpr->token.z, pExpr->token.n);
      } else {
         assert( pExpr->iColumn == OE_Ignore );
         sqlite3VdbeAddOp(v, OP_ContextPop, 0, 0);
         sqlite3VdbeAddOp(v, OP_Goto, 0, pParse->trigStack->ignoreJump);
         VdbeComment((v, "# raise(IGNORE)"));
      }
      stackChng = 0;
      break;
    }
#endif
  }

  if( pParse->ckOffset ){
    pParse->ckOffset += stackChng;
    assert( pParse->ckOffset );
  }
}

#ifndef SQLITE_OMIT_TRIGGER
/*
** Generate code that evalutes the given expression and leaves the result
** on the stack.  See also sqlite3ExprCode().
**
** This routine might also cache the result and modify the pExpr tree
** so that it will make use of the cached result on subsequent evaluations
** rather than evaluate the whole expression again.  Trivial expressions are
** not cached.  If the expression is cached, its result is stored in a 
** memory location.
*/
void sqlite3ExprCodeAndCache(Parse *pParse, Expr *pExpr){
  Vdbe *v = pParse->pVdbe;
  int iMem;
  int addr1, addr2;
  if( v==0 ) return;
  addr1 = sqlite3VdbeCurrentAddr(v);
  sqlite3ExprCode(pParse, pExpr);
  addr2 = sqlite3VdbeCurrentAddr(v);
  if( addr2>addr1+1 || sqlite3VdbeGetOp(v, addr1)->opcode==OP_Function ){
    iMem = pExpr->iTable = pParse->nMem++;
    sqlite3VdbeAddOp(v, OP_MemStore, iMem, 0);
    pExpr->op = TK_REGISTER;
  }
}
#endif

/*
** Generate code that pushes the value of every element of the given
** expression list onto the stack.
**
** Return the number of elements pushed onto the stack.
*/
int sqlite3ExprCodeExprList(
  Parse *pParse,     /* Parsing context */
  ExprList *pList    /* The expression list to be coded */
){
  struct ExprList_item *pItem;
  int i, n;
  if( pList==0 ) return 0;
  n = pList->nExpr;
  for(pItem=pList->a, i=n; i>0; i--, pItem++){
    sqlite3ExprCode(pParse, pItem->pExpr);
  }
  return n;
}

/*
** Generate code for a boolean expression such that a jump is made
** to the label "dest" if the expression is true but execution
** continues straight thru if the expression is false.
**
** If the expression evaluates to NULL (neither true nor false), then
** take the jump if the jumpIfNull flag is true.
**
** This code depends on the fact that certain token values (ex: TK_EQ)
** are the same as opcode values (ex: OP_Eq) that implement the corresponding
** operation.  Special comments in vdbe.c and the mkopcodeh.awk script in
** the make process cause these values to align.  Assert()s in the code
** below verify that the numbers are aligned correctly.
*/
void sqlite3ExprIfTrue(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  int ckOffset = pParse->ckOffset;
  if( v==0 || pExpr==0 ) return;
  op = pExpr->op;
  switch( op ){
    case TK_AND: {
      int d2 = sqlite3VdbeMakeLabel(v);
      sqlite3ExprIfFalse(pParse, pExpr->pLeft, d2, !jumpIfNull);
      sqlite3ExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
      sqlite3VdbeResolveLabel(v, d2);
      break;
    }
    case TK_OR: {
      sqlite3ExprIfTrue(pParse, pExpr->pLeft, dest, jumpIfNull);
      sqlite3ExprIfTrue(pParse, pExpr->pRight, dest, jumpIfNull);
      break;
    }
    case TK_NOT: {
      sqlite3ExprIfFalse(pParse, pExpr->pLeft, dest, jumpIfNull);
      break;
    }
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      assert( TK_LT==OP_Lt );
      assert( TK_LE==OP_Le );
      assert( TK_GT==OP_Gt );
      assert( TK_GE==OP_Ge );
      assert( TK_EQ==OP_Eq );
      assert( TK_NE==OP_Ne );
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3ExprCode(pParse, pExpr->pRight);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op, dest, jumpIfNull);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      assert( TK_ISNULL==OP_IsNull );
      assert( TK_NOTNULL==OP_NotNull );
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3VdbeAddOp(v, op, 1, dest);
      break;
    }
    case TK_BETWEEN: {
      /* The expression "x BETWEEN y AND z" is implemented as:
      **
      ** 1 IF (x < y) GOTO 3
      ** 2 IF (x <= z) GOTO <dest>
      ** 3 ...
      */
      int addr;
      Expr *pLeft = pExpr->pLeft;
      Expr *pRight = pExpr->pList->a[0].pExpr;
      sqlite3ExprCode(pParse, pLeft);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3ExprCode(pParse, pRight);
      addr = codeCompare(pParse, pLeft, pRight, OP_Lt, 0, !jumpIfNull);

      pRight = pExpr->pList->a[1].pExpr;
      sqlite3ExprCode(pParse, pRight);
      codeCompare(pParse, pLeft, pRight, OP_Le, dest, jumpIfNull);

      sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
      sqlite3VdbeJumpHere(v, addr);
      sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      break;
    }
    default: {
      sqlite3ExprCode(pParse, pExpr);
      sqlite3VdbeAddOp(v, OP_If, jumpIfNull, dest);
      break;
    }
  }
  pParse->ckOffset = ckOffset;
}

/*
** Generate code for a boolean expression such that a jump is made
** to the label "dest" if the expression is false but execution
** continues straight thru if the expression is true.
**
** If the expression evaluates to NULL (neither true nor false) then
** jump if jumpIfNull is true or fall through if jumpIfNull is false.
*/
void sqlite3ExprIfFalse(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  int ckOffset = pParse->ckOffset;
  if( v==0 || pExpr==0 ) return;

  /* The value of pExpr->op and op are related as follows:
  **
  **       pExpr->op            op
  **       ---------          ----------
  **       TK_ISNULL          OP_NotNull
  **       TK_NOTNULL         OP_IsNull
  **       TK_NE              OP_Eq
  **       TK_EQ              OP_Ne
  **       TK_GT              OP_Le
  **       TK_LE              OP_Gt
  **       TK_GE              OP_Lt
  **       TK_LT              OP_Ge
  **
  ** For other values of pExpr->op, op is undefined and unused.
  ** The value of TK_ and OP_ constants are arranged such that we
  ** can compute the mapping above using the following expression.
  ** Assert()s verify that the computation is correct.
  */
  op = ((pExpr->op+(TK_ISNULL&1))^1)-(TK_ISNULL&1);

  /* Verify correct alignment of TK_ and OP_ constants
  */
  assert( pExpr->op!=TK_ISNULL || op==OP_NotNull );
  assert( pExpr->op!=TK_NOTNULL || op==OP_IsNull );
  assert( pExpr->op!=TK_NE || op==OP_Eq );
  assert( pExpr->op!=TK_EQ || op==OP_Ne );
  assert( pExpr->op!=TK_LT || op==OP_Ge );
  assert( pExpr->op!=TK_LE || op==OP_Gt );
  assert( pExpr->op!=TK_GT || op==OP_Le );
  assert( pExpr->op!=TK_GE || op==OP_Lt );

  switch( pExpr->op ){
    case TK_AND: {
      sqlite3ExprIfFalse(pParse, pExpr->pLeft, dest, jumpIfNull);
      sqlite3ExprIfFalse(pParse, pExpr->pRight, dest, jumpIfNull);
      break;
    }
    case TK_OR: {
      int d2 = sqlite3VdbeMakeLabel(v);
      sqlite3ExprIfTrue(pParse, pExpr->pLeft, d2, !jumpIfNull);
      sqlite3ExprIfFalse(pParse, pExpr->pRight, dest, jumpIfNull);
      sqlite3VdbeResolveLabel(v, d2);
      break;
    }
    case TK_NOT: {
      sqlite3ExprIfTrue(pParse, pExpr->pLeft, dest, jumpIfNull);
      break;
    }
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
    case TK_NE:
    case TK_EQ: {
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3ExprCode(pParse, pExpr->pRight);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op, dest, jumpIfNull);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      sqlite3ExprCode(pParse, pExpr->pLeft);
      sqlite3VdbeAddOp(v, op, 1, dest);
      break;
    }
    case TK_BETWEEN: {
      /* The expression is "x BETWEEN y AND z". It is implemented as:
      **
      ** 1 IF (x >= y) GOTO 3
      ** 2 GOTO <dest>
      ** 3 IF (x > z) GOTO <dest>
      */
      int addr;
      Expr *pLeft = pExpr->pLeft;
      Expr *pRight = pExpr->pList->a[0].pExpr;
      sqlite3ExprCode(pParse, pLeft);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3ExprCode(pParse, pRight);
      addr = sqlite3VdbeCurrentAddr(v);
      codeCompare(pParse, pLeft, pRight, OP_Ge, addr+3, !jumpIfNull);

      sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      sqlite3VdbeAddOp(v, OP_Goto, 0, dest);
      pRight = pExpr->pList->a[1].pExpr;
      sqlite3ExprCode(pParse, pRight);
      codeCompare(pParse, pLeft, pRight, OP_Gt, dest, jumpIfNull);
      break;
    }
    default: {
      sqlite3ExprCode(pParse, pExpr);
      sqlite3VdbeAddOp(v, OP_IfNot, jumpIfNull, dest);
      break;
    }
  }
  pParse->ckOffset = ckOffset;
}

/*
** Do a deep comparison of two expression trees.  Return TRUE (non-zero)
** if they are identical and return FALSE if they differ in any way.
**
** Sometimes this routine will return FALSE even if the two expressions
** really are equivalent.  If we cannot prove that the expressions are
** identical, we return FALSE just to be safe.  So if this routine
** returns false, then you do not really know for certain if the two
** expressions are the same.  But if you get a TRUE return, then you
** can be sure the expressions are the same.  In the places where
** this routine is used, it does not hurt to get an extra FALSE - that
** just might result in some slightly slower code.  But returning
** an incorrect TRUE could lead to a malfunction.
*/
int sqlite3ExprCompare(Expr *pA, Expr *pB){
  int i;
  if( pA==0||pB==0 ){
    return pB==pA;
  }
  if( pA->op!=pB->op ) return 0;
  if( (pA->flags & EP_Distinct)!=(pB->flags & EP_Distinct) ) return 0;
  if( !sqlite3ExprCompare(pA->pLeft, pB->pLeft) ) return 0;
  if( !sqlite3ExprCompare(pA->pRight, pB->pRight) ) return 0;
  if( pA->pList ){
    if( pB->pList==0 ) return 0;
    if( pA->pList->nExpr!=pB->pList->nExpr ) return 0;
    for(i=0; i<pA->pList->nExpr; i++){
      if( !sqlite3ExprCompare(pA->pList->a[i].pExpr, pB->pList->a[i].pExpr) ){
        return 0;
      }
    }
  }else if( pB->pList ){
    return 0;
  }
  if( pA->pSelect || pB->pSelect ) return 0;
  if( pA->iTable!=pB->iTable || pA->iColumn!=pB->iColumn ) return 0;
  if( pA->op!=TK_COLUMN && pA->token.z ){
    if( pB->token.z==0 ) return 0;
    if( pB->token.n!=pA->token.n ) return 0;
    if( sqlite3StrNICmp((char*)pA->token.z,(char*)pB->token.z,pB->token.n)!=0 ){
      return 0;
    }
  }
  return 1;
}


/*
** Add a new element to the pAggInfo->aCol[] array.  Return the index of
** the new element.  Return a negative number if malloc fails.
*/
static int addAggInfoColumn(AggInfo *pInfo){
  int i;
  pInfo->aCol = sqlite3ArrayAllocate(
       pInfo->aCol,
       sizeof(pInfo->aCol[0]),
       3,
       &pInfo->nColumn,
       &pInfo->nColumnAlloc,
       &i
  );
  return i;
}    

/*
** Add a new element to the pAggInfo->aFunc[] array.  Return the index of
** the new element.  Return a negative number if malloc fails.
*/
static int addAggInfoFunc(AggInfo *pInfo){
  int i;
  pInfo->aFunc = sqlite3ArrayAllocate(
       pInfo->aFunc,
       sizeof(pInfo->aFunc[0]),
       3,
       &pInfo->nFunc,
       &pInfo->nFuncAlloc,
       &i
  );
  return i;
}    

/*
** This is an xFunc for walkExprTree() used to implement 
** sqlite3ExprAnalyzeAggregates().  See sqlite3ExprAnalyzeAggregates
** for additional information.
**
** This routine analyzes the aggregate function at pExpr.
*/
static int analyzeAggregate(void *pArg, Expr *pExpr){
  int i;
  NameContext *pNC = (NameContext *)pArg;
  Parse *pParse = pNC->pParse;
  SrcList *pSrcList = pNC->pSrcList;
  AggInfo *pAggInfo = pNC->pAggInfo;
  

  switch( pExpr->op ){
    case TK_AGG_COLUMN:
    case TK_COLUMN: {
      /* Check to see if the column is in one of the tables in the FROM
      ** clause of the aggregate query */
      if( pSrcList ){
        struct SrcList_item *pItem = pSrcList->a;
        for(i=0; i<pSrcList->nSrc; i++, pItem++){
          struct AggInfo_col *pCol;
          if( pExpr->iTable==pItem->iCursor ){
            /* If we reach this point, it means that pExpr refers to a table
            ** that is in the FROM clause of the aggregate query.  
            **
            ** Make an entry for the column in pAggInfo->aCol[] if there
            ** is not an entry there already.
            */
            int k;
            pCol = pAggInfo->aCol;
            for(k=0; k<pAggInfo->nColumn; k++, pCol++){
              if( pCol->iTable==pExpr->iTable &&
                  pCol->iColumn==pExpr->iColumn ){
                break;
              }
            }
            if( k>=pAggInfo->nColumn && (k = addAggInfoColumn(pAggInfo))>=0 ){
              pCol = &pAggInfo->aCol[k];
              pCol->pTab = pExpr->pTab;
              pCol->iTable = pExpr->iTable;
              pCol->iColumn = pExpr->iColumn;
              pCol->iMem = pParse->nMem++;
              pCol->iSorterColumn = -1;
              pCol->pExpr = pExpr;
              if( pAggInfo->pGroupBy ){
                int j, n;
                ExprList *pGB = pAggInfo->pGroupBy;
                struct ExprList_item *pTerm = pGB->a;
                n = pGB->nExpr;
                for(j=0; j<n; j++, pTerm++){
                  Expr *pE = pTerm->pExpr;
                  if( pE->op==TK_COLUMN && pE->iTable==pExpr->iTable &&
                      pE->iColumn==pExpr->iColumn ){
                    pCol->iSorterColumn = j;
                    break;
                  }
                }
              }
              if( pCol->iSorterColumn<0 ){
                pCol->iSorterColumn = pAggInfo->nSortingColumn++;
              }
            }
            /* There is now an entry for pExpr in pAggInfo->aCol[] (either
            ** because it was there before or because we just created it).
            ** Convert the pExpr to be a TK_AGG_COLUMN referring to that
            ** pAggInfo->aCol[] entry.
            */
            pExpr->pAggInfo = pAggInfo;
            pExpr->op = TK_AGG_COLUMN;
            pExpr->iAgg = k;
            break;
          } /* endif pExpr->iTable==pItem->iCursor */
        } /* end loop over pSrcList */
      }
      return 1;
    }
    case TK_AGG_FUNCTION: {
      /* The pNC->nDepth==0 test causes aggregate functions in subqueries
      ** to be ignored */
      if( pNC->nDepth==0 ){
        /* Check to see if pExpr is a duplicate of another aggregate 
        ** function that is already in the pAggInfo structure
        */
        struct AggInfo_func *pItem = pAggInfo->aFunc;
        for(i=0; i<pAggInfo->nFunc; i++, pItem++){
          if( sqlite3ExprCompare(pItem->pExpr, pExpr) ){
            break;
          }
        }
        if( i>=pAggInfo->nFunc ){
          /* pExpr is original.  Make a new entry in pAggInfo->aFunc[]
          */
          u8 enc = ENC(pParse->db);
          i = addAggInfoFunc(pAggInfo);
          if( i>=0 ){
            pItem = &pAggInfo->aFunc[i];
            pItem->pExpr = pExpr;
            pItem->iMem = pParse->nMem++;
            pItem->pFunc = sqlite3FindFunction(pParse->db,
                   (char*)pExpr->token.z, pExpr->token.n,
                   pExpr->pList ? pExpr->pList->nExpr : 0, enc, 0);
            if( pExpr->flags & EP_Distinct ){
              pItem->iDistinct = pParse->nTab++;
            }else{
              pItem->iDistinct = -1;
            }
          }
        }
        /* Make pExpr point to the appropriate pAggInfo->aFunc[] entry
        */
        pExpr->iAgg = i;
        pExpr->pAggInfo = pAggInfo;
        return 1;
      }
    }
  }

  /* Recursively walk subqueries looking for TK_COLUMN nodes that need
  ** to be changed to TK_AGG_COLUMN.  But increment nDepth so that
  ** TK_AGG_FUNCTION nodes in subqueries will be unchanged.
  */
  if( pExpr->pSelect ){
    pNC->nDepth++;
    walkSelectExpr(pExpr->pSelect, analyzeAggregate, pNC);
    pNC->nDepth--;
  }
  return 0;
}

/*
** Analyze the given expression looking for aggregate functions and
** for variables that need to be added to the pParse->aAgg[] array.
** Make additional entries to the pParse->aAgg[] array as necessary.
**
** This routine should only be called after the expression has been
** analyzed by sqlite3ExprResolveNames().
**
** If errors are seen, leave an error message in zErrMsg and return
** the number of errors.
*/
int sqlite3ExprAnalyzeAggregates(NameContext *pNC, Expr *pExpr){
  int nErr = pNC->pParse->nErr;
  walkExprTree(pExpr, analyzeAggregate, pNC);
  return pNC->pParse->nErr - nErr;
}

/*
** Call sqlite3ExprAnalyzeAggregates() for every expression in an
** expression list.  Return the number of errors.
**
** If an error is found, the analysis is cut short.
*/
int sqlite3ExprAnalyzeAggList(NameContext *pNC, ExprList *pList){
  struct ExprList_item *pItem;
  int i;
  int nErr = 0;
  if( pList ){
    for(pItem=pList->a, i=0; nErr==0 && i<pList->nExpr; i++, pItem++){
      nErr += sqlite3ExprAnalyzeAggregates(pNC, pItem->pExpr);
    }
  }
  return nErr;
}
