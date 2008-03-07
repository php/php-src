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
  char *zColl = 0;            /* Dequoted name of collation sequence */
  CollSeq *pColl;
  zColl = sqlite3NameFromToken(pParse->db, pName);
  if( pExpr && zColl ){
    pColl = sqlite3LocateCollSeq(pParse, zColl, -1);
    if( pColl ){
      pExpr->pColl = pColl;
      pExpr->flags |= EP_ExpCollate;
    }
  }
  sqlite3_free(zColl);
  return pExpr;
}

/*
** Return the default collation sequence for the expression pExpr. If
** there is no default collation type, return 0.
*/
CollSeq *sqlite3ExprCollSeq(Parse *pParse, Expr *pExpr){
  CollSeq *pColl = 0;
  if( pExpr ){
    int op;
    pColl = pExpr->pColl;
    op = pExpr->op;
    if( (op==TK_CAST || op==TK_UPLUS) && !pColl ){
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
** Return the P5 value that should be used for a binary comparison
** opcode (OP_Eq, OP_Ge etc.) used to compare pExpr1 and pExpr2.
*/
static u8 binaryCompareP5(Expr *pExpr1, Expr *pExpr2, int jumpIfNull){
  u8 aff = (char)sqlite3ExprAffinity(pExpr2);
  aff = sqlite3CompareAffinity(pExpr1, aff) | jumpIfNull;
  return aff;
}

/*
** Return a pointer to the collation sequence that should be used by
** a binary comparison operator comparing pLeft and pRight.
**
** If the left hand expression has a collating sequence type, then it is
** used. Otherwise the collation sequence for the right hand expression
** is used, or the default (BINARY) if neither expression has a collating
** type.
**
** Argument pRight (but not pLeft) may be a null pointer. In this case,
** it is not considered.
*/
CollSeq *sqlite3BinaryCompareCollSeq(
  Parse *pParse, 
  Expr *pLeft, 
  Expr *pRight
){
  CollSeq *pColl;
  assert( pLeft );
  if( pLeft->flags & EP_ExpCollate ){
    assert( pLeft->pColl );
    pColl = pLeft->pColl;
  }else if( pRight && pRight->flags & EP_ExpCollate ){
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
  int in1, int in2, /* Register holding operands */
  int dest,         /* Jump here if true.  */
  int jumpIfNull    /* If true, jump if either operand is NULL */
){
  int p5;
  int addr;
  CollSeq *p4;

  p4 = sqlite3BinaryCompareCollSeq(pParse, pLeft, pRight);
  p5 = binaryCompareP5(pLeft, pRight, jumpIfNull);
  addr = sqlite3VdbeAddOp4(pParse->pVdbe, opcode, in2, dest, in1,
                           (void*)p4, P4_COLLSEQ);
  sqlite3VdbeChangeP5(pParse->pVdbe, p5);
  return addr;
}

/*
** Construct a new expression node and return a pointer to it.  Memory
** for this node is obtained from sqlite3_malloc().  The calling function
** is responsible for making sure the node eventually gets freed.
*/
Expr *sqlite3Expr(
  sqlite3 *db,            /* Handle for sqlite3DbMallocZero() (may be null) */
  int op,                 /* Expression opcode */
  Expr *pLeft,            /* Left operand */
  Expr *pRight,           /* Right operand */
  const Token *pToken     /* Argument token */
){
  Expr *pNew;
  pNew = sqlite3DbMallocZero(db, sizeof(Expr));
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

  sqlite3ExprSetHeight(pNew);
  return pNew;
}

/*
** Works like sqlite3Expr() except that it takes an extra Parse*
** argument and notifies the associated connection object if malloc fails.
*/
Expr *sqlite3PExpr(
  Parse *pParse,          /* Parsing context */
  int op,                 /* Expression opcode */
  Expr *pLeft,            /* Left operand */
  Expr *pRight,           /* Right operand */
  const Token *pToken     /* Argument token */
){
  return sqlite3Expr(pParse->db, op, pLeft, pRight, pToken);
}

/*
** When doing a nested parse, you can include terms in an expression
** that look like this:   #1 #2 ...  These terms refer to registers
** in the virtual machine.  #N is the N-th register.
**
** This routine is called by the parser to deal with on of those terms.
** It immediately generates code to store the value in a memory location.
** The returns an expression that will code to extract the value from
** that memory location as needed.
*/
Expr *sqlite3RegisterExpr(Parse *pParse, Token *pToken){
  Vdbe *v = pParse->pVdbe;
  Expr *p;
  if( pParse->nested==0 ){
    sqlite3ErrorMsg(pParse, "near \"%T\": syntax error", pToken);
    return sqlite3PExpr(pParse, TK_NULL, 0, 0, 0);
  }
  if( v==0 ) return 0;
  p = sqlite3PExpr(pParse, TK_REGISTER, 0, 0, pToken);
  if( p==0 ){
    return 0;  /* Malloc failed */
  }
  p->iTable = atoi((char*)&pToken->z[1]);
  return p;
}

/*
** Join two expressions using an AND operator.  If either expression is
** NULL, then just return the other expression.
*/
Expr *sqlite3ExprAnd(sqlite3 *db, Expr *pLeft, Expr *pRight){
  if( pLeft==0 ){
    return pRight;
  }else if( pRight==0 ){
    return pLeft;
  }else{
    return sqlite3Expr(db, TK_AND, pLeft, pRight, 0);
  }
}

/*
** Set the Expr.span field of the given expression to span all
** text between the two given tokens.
*/
void sqlite3ExprSpan(Expr *pExpr, Token *pLeft, Token *pRight){
  assert( pRight!=0 );
  assert( pLeft!=0 );
  if( pExpr && pRight->z && pLeft->z ){
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
Expr *sqlite3ExprFunction(Parse *pParse, ExprList *pList, Token *pToken){
  Expr *pNew;
  assert( pToken );
  pNew = sqlite3DbMallocZero(pParse->db, sizeof(Expr) );
  if( pNew==0 ){
    sqlite3ExprListDelete(pList); /* Avoid leaking memory when malloc fails */
    return 0;
  }
  pNew->op = TK_FUNCTION;
  pNew->pList = pList;
  assert( pToken->dyn==0 );
  pNew->token = *pToken;
  pNew->span = pNew->token;

  sqlite3ExprSetHeight(pNew);
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
  sqlite3 *db = pParse->db;

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
        pParse->apVarExpr =
            sqlite3DbReallocOrFree(
              db,
              pParse->apVarExpr,
              pParse->nVarExprAlloc*sizeof(pParse->apVarExpr[0])
            );
      }
      if( !db->mallocFailed ){
        assert( pParse->apVarExpr!=0 );
        pParse->apVarExpr[pParse->nVarExpr++] = pExpr;
      }
    }
  } 
  if( !pParse->nErr && pParse->nVar>SQLITE_MAX_VARIABLE_NUMBER ){
    sqlite3ErrorMsg(pParse, "too many SQL variables");
  }
}

/*
** Recursively delete an expression tree.
*/
void sqlite3ExprDelete(Expr *p){
  if( p==0 ) return;
  if( p->span.dyn ) sqlite3_free((char*)p->span.z);
  if( p->token.dyn ) sqlite3_free((char*)p->token.z);
  sqlite3ExprDelete(p->pLeft);
  sqlite3ExprDelete(p->pRight);
  sqlite3ExprListDelete(p->pList);
  sqlite3SelectDelete(p->pSelect);
  sqlite3_free(p);
}

/*
** The Expr.token field might be a string literal that is quoted.
** If so, remove the quotation marks.
*/
void sqlite3DequoteExpr(sqlite3 *db, Expr *p){
  if( ExprHasAnyProperty(p, EP_Dequoted) ){
    return;
  }
  ExprSetProperty(p, EP_Dequoted);
  if( p->token.dyn==0 ){
    sqlite3TokenCopy(db, &p->token, &p->token);
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
Expr *sqlite3ExprDup(sqlite3 *db, Expr *p){
  Expr *pNew;
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRaw(db, sizeof(*p) );
  if( pNew==0 ) return 0;
  memcpy(pNew, p, sizeof(*pNew));
  if( p->token.z!=0 ){
    pNew->token.z = (u8*)sqlite3DbStrNDup(db, (char*)p->token.z, p->token.n);
    pNew->token.dyn = 1;
  }else{
    assert( pNew->token.z==0 );
  }
  pNew->span.z = 0;
  pNew->pLeft = sqlite3ExprDup(db, p->pLeft);
  pNew->pRight = sqlite3ExprDup(db, p->pRight);
  pNew->pList = sqlite3ExprListDup(db, p->pList);
  pNew->pSelect = sqlite3SelectDup(db, p->pSelect);
  return pNew;
}
void sqlite3TokenCopy(sqlite3 *db, Token *pTo, Token *pFrom){
  if( pTo->dyn ) sqlite3_free((char*)pTo->z);
  if( pFrom->z ){
    pTo->n = pFrom->n;
    pTo->z = (u8*)sqlite3DbStrNDup(db, (char*)pFrom->z, pFrom->n);
    pTo->dyn = 1;
  }else{
    pTo->z = 0;
  }
}
ExprList *sqlite3ExprListDup(sqlite3 *db, ExprList *p){
  ExprList *pNew;
  struct ExprList_item *pItem, *pOldItem;
  int i;
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRaw(db, sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->iECursor = 0;
  pNew->nExpr = pNew->nAlloc = p->nExpr;
  pNew->a = pItem = sqlite3DbMallocRaw(db,  p->nExpr*sizeof(p->a[0]) );
  if( pItem==0 ){
    sqlite3_free(pNew);
    return 0;
  } 
  pOldItem = p->a;
  for(i=0; i<p->nExpr; i++, pItem++, pOldItem++){
    Expr *pNewExpr, *pOldExpr;
    pItem->pExpr = pNewExpr = sqlite3ExprDup(db, pOldExpr = pOldItem->pExpr);
    if( pOldExpr->span.z!=0 && pNewExpr ){
      /* Always make a copy of the span for top-level expressions in the
      ** expression list.  The logic in SELECT processing that determines
      ** the names of columns in the result set needs this information */
      sqlite3TokenCopy(db, &pNewExpr->span, &pOldExpr->span);
    }
    assert( pNewExpr==0 || pNewExpr->span.z!=0 
            || pOldExpr->span.z==0
            || db->mallocFailed );
    pItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
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
SrcList *sqlite3SrcListDup(sqlite3 *db, SrcList *p){
  SrcList *pNew;
  int i;
  int nByte;
  if( p==0 ) return 0;
  nByte = sizeof(*p) + (p->nSrc>0 ? sizeof(p->a[0]) * (p->nSrc-1) : 0);
  pNew = sqlite3DbMallocRaw(db, nByte );
  if( pNew==0 ) return 0;
  pNew->nSrc = pNew->nAlloc = p->nSrc;
  for(i=0; i<p->nSrc; i++){
    struct SrcList_item *pNewItem = &pNew->a[i];
    struct SrcList_item *pOldItem = &p->a[i];
    Table *pTab;
    pNewItem->zDatabase = sqlite3DbStrDup(db, pOldItem->zDatabase);
    pNewItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
    pNewItem->zAlias = sqlite3DbStrDup(db, pOldItem->zAlias);
    pNewItem->jointype = pOldItem->jointype;
    pNewItem->iCursor = pOldItem->iCursor;
    pNewItem->isPopulated = pOldItem->isPopulated;
    pTab = pNewItem->pTab = pOldItem->pTab;
    if( pTab ){
      pTab->nRef++;
    }
    pNewItem->pSelect = sqlite3SelectDup(db, pOldItem->pSelect);
    pNewItem->pOn = sqlite3ExprDup(db, pOldItem->pOn);
    pNewItem->pUsing = sqlite3IdListDup(db, pOldItem->pUsing);
    pNewItem->colUsed = pOldItem->colUsed;
  }
  return pNew;
}
IdList *sqlite3IdListDup(sqlite3 *db, IdList *p){
  IdList *pNew;
  int i;
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRaw(db, sizeof(*pNew) );
  if( pNew==0 ) return 0;
  pNew->nId = pNew->nAlloc = p->nId;
  pNew->a = sqlite3DbMallocRaw(db, p->nId*sizeof(p->a[0]) );
  if( pNew->a==0 ){
    sqlite3_free(pNew);
    return 0;
  }
  for(i=0; i<p->nId; i++){
    struct IdList_item *pNewItem = &pNew->a[i];
    struct IdList_item *pOldItem = &p->a[i];
    pNewItem->zName = sqlite3DbStrDup(db, pOldItem->zName);
    pNewItem->idx = pOldItem->idx;
  }
  return pNew;
}
Select *sqlite3SelectDup(sqlite3 *db, Select *p){
  Select *pNew;
  if( p==0 ) return 0;
  pNew = sqlite3DbMallocRaw(db, sizeof(*p) );
  if( pNew==0 ) return 0;
  pNew->isDistinct = p->isDistinct;
  pNew->pEList = sqlite3ExprListDup(db, p->pEList);
  pNew->pSrc = sqlite3SrcListDup(db, p->pSrc);
  pNew->pWhere = sqlite3ExprDup(db, p->pWhere);
  pNew->pGroupBy = sqlite3ExprListDup(db, p->pGroupBy);
  pNew->pHaving = sqlite3ExprDup(db, p->pHaving);
  pNew->pOrderBy = sqlite3ExprListDup(db, p->pOrderBy);
  pNew->op = p->op;
  pNew->pPrior = sqlite3SelectDup(db, p->pPrior);
  pNew->pLimit = sqlite3ExprDup(db, p->pLimit);
  pNew->pOffset = sqlite3ExprDup(db, p->pOffset);
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
Select *sqlite3SelectDup(sqlite3 *db, Select *p){
  assert( p==0 );
  return 0;
}
#endif


/*
** Add a new element to the end of an expression list.  If pList is
** initially NULL, then create a new expression list.
*/
ExprList *sqlite3ExprListAppend(
  Parse *pParse,          /* Parsing context */
  ExprList *pList,        /* List to which to append. Might be NULL */
  Expr *pExpr,            /* Expression to be appended */
  Token *pName            /* AS keyword for the expression */
){
  sqlite3 *db = pParse->db;
  if( pList==0 ){
    pList = sqlite3DbMallocZero(db, sizeof(ExprList) );
    if( pList==0 ){
      goto no_mem;
    }
    assert( pList->nAlloc==0 );
  }
  if( pList->nAlloc<=pList->nExpr ){
    struct ExprList_item *a;
    int n = pList->nAlloc*2 + 4;
    a = sqlite3DbRealloc(db, pList->a, n*sizeof(pList->a[0]));
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
    pItem->zName = sqlite3NameFromToken(db, pName);
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
** If the expression list pEList contains more than iLimit elements,
** leave an error message in pParse.
*/
void sqlite3ExprListCheckLength(
  Parse *pParse,
  ExprList *pEList,
  int iLimit,
  const char *zObject
){
  if( pEList && pEList->nExpr>iLimit ){
    sqlite3ErrorMsg(pParse, "too many columns in %s", zObject);
  }
}


#if defined(SQLITE_TEST) || SQLITE_MAX_EXPR_DEPTH>0
/* The following three functions, heightOfExpr(), heightOfExprList()
** and heightOfSelect(), are used to determine the maximum height
** of any expression tree referenced by the structure passed as the
** first argument.
**
** If this maximum height is greater than the current value pointed
** to by pnHeight, the second parameter, then set *pnHeight to that
** value.
*/
static void heightOfExpr(Expr *p, int *pnHeight){
  if( p ){
    if( p->nHeight>*pnHeight ){
      *pnHeight = p->nHeight;
    }
  }
}
static void heightOfExprList(ExprList *p, int *pnHeight){
  if( p ){
    int i;
    for(i=0; i<p->nExpr; i++){
      heightOfExpr(p->a[i].pExpr, pnHeight);
    }
  }
}
static void heightOfSelect(Select *p, int *pnHeight){
  if( p ){
    heightOfExpr(p->pWhere, pnHeight);
    heightOfExpr(p->pHaving, pnHeight);
    heightOfExpr(p->pLimit, pnHeight);
    heightOfExpr(p->pOffset, pnHeight);
    heightOfExprList(p->pEList, pnHeight);
    heightOfExprList(p->pGroupBy, pnHeight);
    heightOfExprList(p->pOrderBy, pnHeight);
    heightOfSelect(p->pPrior, pnHeight);
  }
}

/*
** Set the Expr.nHeight variable in the structure passed as an 
** argument. An expression with no children, Expr.pList or 
** Expr.pSelect member has a height of 1. Any other expression
** has a height equal to the maximum height of any other 
** referenced Expr plus one.
*/
void sqlite3ExprSetHeight(Expr *p){
  int nHeight = 0;
  heightOfExpr(p->pLeft, &nHeight);
  heightOfExpr(p->pRight, &nHeight);
  heightOfExprList(p->pList, &nHeight);
  heightOfSelect(p->pSelect, &nHeight);
  p->nHeight = nHeight + 1;
}

/*
** Return the maximum height of any expression tree referenced
** by the select statement passed as an argument.
*/
int sqlite3SelectExprHeight(Select *p){
  int nHeight = 0;
  heightOfSelect(p, &nHeight);
  return nHeight;
}
#endif

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
    sqlite3_free(pItem->zName);
  }
  sqlite3_free(pList->a);
  sqlite3_free(pList);
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
  if( p->pPrior ){
    walkSelectExpr(p->pPrior, xFunc, pArg);
  }
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
  int *pN = (int*)pArg;

  /* If *pArg is 3 then any term of the expression that comes from
  ** the ON or USING clauses of a join disqualifies the expression
  ** from being considered constant. */
  if( (*pN)==3 && ExprHasAnyProperty(pExpr, EP_FromJoin) ){
    *pN = 0;
    return 2;
  }

  switch( pExpr->op ){
    /* Consider functions to be constant if all their arguments are constant
    ** and *pArg==2 */
    case TK_FUNCTION:
      if( (*pN)==2 ) return 0;
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
      *pN = 0;
      return 2;
    case TK_IN:
      if( pExpr->pSelect ){
        *pN = 0;
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
** that does no originate from the ON or USING clauses of a join.
** Return 0 if it involves variables or function calls or terms from
** an ON or USING clause.
*/
int sqlite3ExprIsConstantNotJoin(Expr *p){
  int isConst = 3;
  walkExprTree(p, exprNodeIsConstant, &isConst);
  return isConst!=0;
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
  Schema *pSchema = 0;              /* Schema of the expression */

  assert( pColumnToken && pColumnToken->z ); /* The Z in X.Y.Z cannot be NULL */
  zDb = sqlite3NameFromToken(db, pDbToken);
  zTab = sqlite3NameFromToken(db, pTableToken);
  zCol = sqlite3NameFromToken(db, pColumnToken);
  if( db->mallocFailed ){
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
          pSchema = pTab->pSchema;
          pMatch = pItem;
        }
        for(j=0, pCol=pTab->aCol; j<pTab->nCol; j++, pCol++){
          if( sqlite3StrICmp(pCol->zName, zCol)==0 ){
            const char *zColl = pTab->aCol[j].zColl;
            IdList *pUsing;
            cnt++;
            pExpr->iTable = pItem->iCursor;
            pMatch = pItem;
            pSchema = pTab->pSchema;
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
      u32 *piColMask;
      if( pTriggerStack->newIdx != -1 && sqlite3StrICmp("new", zTab) == 0 ){
        pExpr->iTable = pTriggerStack->newIdx;
        assert( pTriggerStack->pTab );
        pTab = pTriggerStack->pTab;
        piColMask = &(pTriggerStack->newColMask);
      }else if( pTriggerStack->oldIdx != -1 && sqlite3StrICmp("old", zTab)==0 ){
        pExpr->iTable = pTriggerStack->oldIdx;
        assert( pTriggerStack->pTab );
        pTab = pTriggerStack->pTab;
        piColMask = &(pTriggerStack->oldColMask);
      }

      if( pTab ){ 
        int iCol;
        Column *pCol = pTab->aCol;

        pSchema = pTab->pSchema;
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
            if( iCol>=0 ){
              *piColMask |= ((u32)1<<iCol) | (iCol>=32?0xffffffff:0);
            }
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
          Expr *pDup, *pOrig;
          assert( pExpr->pLeft==0 && pExpr->pRight==0 );
          assert( pExpr->pList==0 );
          assert( pExpr->pSelect==0 );
          pOrig = pEList->a[j].pExpr;
          if( !pNC->allowAgg && ExprHasProperty(pOrig, EP_Agg) ){
            sqlite3ErrorMsg(pParse, "misuse of aliased aggregate %s", zAs);
            sqlite3_free(zCol);
            return 2;
          }
          pDup = sqlite3ExprDup(db, pOrig);
          if( pExpr->flags & EP_ExpCollate ){
            pDup->pColl = pExpr->pColl;
            pDup->flags |= EP_ExpCollate;
          }
          if( pExpr->span.dyn ) sqlite3_free((char*)pExpr->span.z);
          if( pExpr->token.dyn ) sqlite3_free((char*)pExpr->token.z);
          memcpy(pExpr, pDup, sizeof(*pExpr));
          sqlite3_free(pDup);
          cnt = 1;
          pMatch = 0;
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
    sqlite3_free(zCol);
    return 0;
  }

  /*
  ** cnt==0 means there was not match.  cnt>1 means there were two or
  ** more matches.  Either way, we have an error.
  */
  if( cnt!=1 ){
    const char *zErr;
    zErr = cnt==0 ? "no such column" : "ambiguous column name";
    if( zDb ){
      sqlite3ErrorMsg(pParse, "%s: %s.%s.%s", zErr, zDb, zTab, zCol);
    }else if( zTab ){
      sqlite3ErrorMsg(pParse, "%s: %s.%s", zErr, zTab, zCol);
    }else{
      sqlite3ErrorMsg(pParse, "%s: %s", zErr, zCol);
    }
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
  sqlite3_free(zDb);
  sqlite3_free(zTab);
  sqlite3ExprDelete(pExpr->pLeft);
  pExpr->pLeft = 0;
  sqlite3ExprDelete(pExpr->pRight);
  pExpr->pRight = 0;
  pExpr->op = TK_COLUMN;
lookupname_end_2:
  sqlite3_free(zCol);
  if( cnt==1 ){
    assert( pNC!=0 );
    sqlite3AuthRead(pParse, pExpr, pSchema, pNC->pSrcList);
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
#if defined(SQLITE_TEST) || SQLITE_MAX_EXPR_DEPTH>0
  if( (pExpr->nHeight+pNC->pParse->nHeight)>SQLITE_MAX_EXPR_DEPTH ){
    sqlite3ErrorMsg(pNC->pParse, 
       "Expression tree is too large (maximum depth %d)",
       SQLITE_MAX_EXPR_DEPTH
    );
    return 1;
  }
  pNC->pParse->nHeight += pExpr->nHeight;
#endif
  savedHasAgg = pNC->hasAgg;
  pNC->hasAgg = 0;
  walkExprTree(pExpr, nameResolverStep, pNC);
#if defined(SQLITE_TEST) || SQLITE_MAX_EXPR_DEPTH>0
  pNC->pParse->nHeight -= pExpr->nHeight;
#endif
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

#ifdef SQLITE_TEST
  int sqlite3_enable_in_opt = 1;
#else
  #define sqlite3_enable_in_opt 1
#endif

/*
** This function is used by the implementation of the IN (...) operator.
** It's job is to find or create a b-tree structure that may be used
** either to test for membership of the (...) set or to iterate through
** its members, skipping duplicates.
**
** The cursor opened on the structure (database table, database index 
** or ephermal table) is stored in pX->iTable before this function returns.
** The returned value indicates the structure type, as follows:
**
**   IN_INDEX_ROWID - The cursor was opened on a database table.
**   IN_INDEX_INDEX - The cursor was opened on a database index.
**   IN_INDEX_EPH -   The cursor was opened on a specially created and
**                    populated epheremal table.
**
** An existing structure may only be used if the SELECT is of the simple
** form:
**
**     SELECT <column> FROM <table>
**
** If the mustBeUnique parameter is false, the structure will be used 
** for fast set membership tests. In this case an epheremal table must 
** be used unless <column> is an INTEGER PRIMARY KEY or an index can 
** be found with <column> as its left-most column.
**
** If mustBeUnique is true, then the structure will be used to iterate
** through the set members, skipping any duplicates. In this case an
** epheremal table must be used unless the selected <column> is guaranteed
** to be unique - either because it is an INTEGER PRIMARY KEY or it
** is unique by virtue of a constraint or implicit index.
*/
#ifndef SQLITE_OMIT_SUBQUERY
int sqlite3FindInIndex(Parse *pParse, Expr *pX, int mustBeUnique){
  Select *p;
  int eType = 0;
  int iTab = pParse->nTab++;

  /* The follwing if(...) expression is true if the SELECT is of the 
  ** simple form:
  **
  **     SELECT <column> FROM <table>
  **
  ** If this is the case, it may be possible to use an existing table
  ** or index instead of generating an epheremal table.
  */
  if( sqlite3_enable_in_opt
   && (p=pX->pSelect) && !p->pPrior
   && !p->isDistinct && !p->isAgg && !p->pGroupBy
   && p->pSrc && p->pSrc->nSrc==1 && !p->pSrc->a[0].pSelect
   && !p->pSrc->a[0].pTab->pSelect                                  
   && p->pEList->nExpr==1 && p->pEList->a[0].pExpr->op==TK_COLUMN
   && !p->pLimit && !p->pOffset && !p->pWhere
  ){
    sqlite3 *db = pParse->db;
    Index *pIdx;
    Expr *pExpr = p->pEList->a[0].pExpr;
    int iCol = pExpr->iColumn;
    Vdbe *v = sqlite3GetVdbe(pParse);

    /* This function is only called from two places. In both cases the vdbe
    ** has already been allocated. So assume sqlite3GetVdbe() is always
    ** successful here.
    */
    assert(v);
    if( iCol<0 ){
      int iMem = ++pParse->nMem;
      int iAddr;
      Table *pTab = p->pSrc->a[0].pTab;
      int iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
      sqlite3VdbeUsesBtree(v, iDb);

      iAddr = sqlite3VdbeAddOp1(v, OP_If, iMem);
      sqlite3VdbeAddOp2(v, OP_Integer, 1, iMem);

      sqlite3OpenTable(pParse, iTab, iDb, pTab, OP_OpenRead);
      eType = IN_INDEX_ROWID;

      sqlite3VdbeJumpHere(v, iAddr);
    }else{
      /* The collation sequence used by the comparison. If an index is to 
      ** be used in place of a temp-table, it must be ordered according
      ** to this collation sequence.
      */
      CollSeq *pReq = sqlite3BinaryCompareCollSeq(pParse, pX->pLeft, pExpr);

      /* Check that the affinity that will be used to perform the 
      ** comparison is the same as the affinity of the column. If
      ** it is not, it is not possible to use any index.
      */
      Table *pTab = p->pSrc->a[0].pTab;
      char aff = comparisonAffinity(pX);
      int affinity_ok = (pTab->aCol[iCol].affinity==aff||aff==SQLITE_AFF_NONE);

      for(pIdx=pTab->pIndex; pIdx && eType==0 && affinity_ok; pIdx=pIdx->pNext){
        if( (pIdx->aiColumn[0]==iCol)
         && (pReq==sqlite3FindCollSeq(db, ENC(db), pIdx->azColl[0], -1, 0))
         && (!mustBeUnique || (pIdx->nColumn==1 && pIdx->onError!=OE_None))
        ){
          int iDb;
          int iMem = ++pParse->nMem;
          int iAddr;
          char *pKey;
  
          pKey = (char *)sqlite3IndexKeyinfo(pParse, pIdx);
          iDb = sqlite3SchemaToIndex(db, pIdx->pSchema);
          sqlite3VdbeUsesBtree(v, iDb);

          iAddr = sqlite3VdbeAddOp1(v, OP_If, iMem);
          sqlite3VdbeAddOp2(v, OP_Integer, 1, iMem);
  
          sqlite3VdbeAddOp4(v, OP_OpenRead, iTab, pIdx->tnum, iDb,
                               pKey,P4_KEYINFO_HANDOFF);
          VdbeComment((v, "%s", pIdx->zName));
          eType = IN_INDEX_INDEX;
          sqlite3VdbeAddOp2(v, OP_SetNumColumns, iTab, pIdx->nColumn);

          sqlite3VdbeJumpHere(v, iAddr);
        }
      }
    }
  }

  if( eType==0 ){
    sqlite3CodeSubselect(pParse, pX);
    eType = IN_INDEX_EPH;
  }else{
    pX->iTable = iTab;
  }
  return eType;
}
#endif

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
    int mem = ++pParse->nMem;
    sqlite3VdbeAddOp1(v, OP_If, mem);
    testAddr = sqlite3VdbeAddOp2(v, OP_Integer, 1, mem);
    assert( testAddr>0 || pParse->db->mallocFailed );
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
      addr = sqlite3VdbeAddOp1(v, OP_OpenEphemeral, pExpr->iTable);
      memset(&keyInfo, 0, sizeof(keyInfo));
      keyInfo.nField = 1;
      sqlite3VdbeAddOp2(v, OP_SetNumColumns, pExpr->iTable, 1);

      if( pExpr->pSelect ){
        /* Case 1:     expr IN (SELECT ...)
        **
        ** Generate code to write the results of the select into the temporary
        ** table allocated and opened above.
        */
        SelectDest dest;
        ExprList *pEList;

        sqlite3SelectDestInit(&dest, SRT_Set, pExpr->iTable);
        dest.affinity = (int)affinity;
        assert( (pExpr->iTable&0x0000FFFF)==pExpr->iTable );
        if( sqlite3Select(pParse, pExpr->pSelect, &dest, 0, 0, 0, 0) ){
          return;
        }
        pEList = pExpr->pSelect->pEList;
        if( pEList && pEList->nExpr>0 ){ 
          keyInfo.aColl[0] = sqlite3BinaryCompareCollSeq(pParse, pExpr->pLeft,
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
        int r1, r2;

        if( !affinity ){
          affinity = SQLITE_AFF_NONE;
        }
        keyInfo.aColl[0] = pExpr->pLeft->pColl;

        /* Loop through each expression in <exprlist>. */
        r1 = sqlite3GetTempReg(pParse);
        r2 = sqlite3GetTempReg(pParse);
        for(i=pList->nExpr, pItem=pList->a; i>0; i--, pItem++){
          Expr *pE2 = pItem->pExpr;

          /* If the expression is not constant then we will need to
          ** disable the test that was generated above that makes sure
          ** this code only executes once.  Because for a non-constant
          ** expression we need to rerun this code each time.
          */
          if( testAddr && !sqlite3ExprIsConstant(pE2) ){
            sqlite3VdbeChangeToNoop(v, testAddr-1, 2);
            testAddr = 0;
          }

          /* Evaluate the expression and insert it into the temp table */
          sqlite3ExprCode(pParse, pE2, r1);
          sqlite3VdbeAddOp4(v, OP_MakeRecord, r1, 1, r2, &affinity, 1);
          sqlite3VdbeAddOp2(v, OP_IdxInsert, pExpr->iTable, r2);
        }
        sqlite3ReleaseTempReg(pParse, r1);
        sqlite3ReleaseTempReg(pParse, r2);
      }
      sqlite3VdbeChangeP4(v, addr, (void *)&keyInfo, P4_KEYINFO);
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
      SelectDest dest;

      pSel = pExpr->pSelect;
      sqlite3SelectDestInit(&dest, 0, ++pParse->nMem);
      if( pExpr->op==TK_SELECT ){
        dest.eDest = SRT_Mem;
        sqlite3VdbeAddOp2(v, OP_Null, 0, dest.iParm);
        VdbeComment((v, "Init subquery result"));
      }else{
        dest.eDest = SRT_Exists;
        sqlite3VdbeAddOp2(v, OP_Integer, 0, dest.iParm);
        VdbeComment((v, "Init EXISTS result"));
      }
      sqlite3ExprDelete(pSel->pLimit);
      pSel->pLimit = sqlite3PExpr(pParse, TK_INTEGER, 0, 0, &one);
      if( sqlite3Select(pParse, pSel, &dest, 0, 0, 0, 0) ){
        return;
      }
      pExpr->iColumn = dest.iParm;
      break;
    }
  }

  if( testAddr ){
    sqlite3VdbeJumpHere(v, testAddr-1);
  }

  return;
}
#endif /* SQLITE_OMIT_SUBQUERY */

/*
** Duplicate an 8-byte value
*/
static char *dup8bytes(Vdbe *v, const char *in){
  char *out = sqlite3DbMallocRaw(sqlite3VdbeDb(v), 8);
  if( out ){
    memcpy(out, in, 8);
  }
  return out;
}

/*
** Generate an instruction that will put the floating point
** value described by z[0..n-1] into register iMem.
**
** The z[] string will probably not be zero-terminated.  But the 
** z[n] character is guaranteed to be something that does not look
** like the continuation of the number.
*/
static void codeReal(Vdbe *v, const char *z, int n, int negateFlag, int iMem){
  assert( z || v==0 || sqlite3VdbeDb(v)->mallocFailed );
  if( z ){
    double value;
    char *zV;
    assert( !isdigit(z[n]) );
    sqlite3AtoF(z, &value);
    if( negateFlag ) value = -value;
    zV = dup8bytes(v, (char*)&value);
    sqlite3VdbeAddOp4(v, OP_Real, 0, iMem, 0, zV, P4_REAL);
  }
}


/*
** Generate an instruction that will put the integer describe by
** text z[0..n-1] into register iMem.
**
** The z[] string will probably not be zero-terminated.  But the 
** z[n] character is guaranteed to be something that does not look
** like the continuation of the number.
*/
static void codeInteger(Vdbe *v, const char *z, int n, int negFlag, int iMem){
  assert( z || v==0 || sqlite3VdbeDb(v)->mallocFailed );
  if( z ){
    int i;
    assert( !isdigit(z[n]) );
    if( sqlite3GetInt32(z, &i) ){
      if( negFlag ) i = -i;
      sqlite3VdbeAddOp2(v, OP_Integer, i, iMem);
    }else if( sqlite3FitsIn64Bits(z, negFlag) ){
      i64 value;
      char *zV;
      sqlite3Atoi64(z, &value);
      if( negFlag ) value = -value;
      zV = dup8bytes(v, (char*)&value);
      sqlite3VdbeAddOp4(v, OP_Int64, 0, iMem, 0, zV, P4_INT64);
    }else{
      codeReal(v, z, n, negFlag, iMem);
    }
  }
}


/*
** Generate code that will extract the iColumn-th column from
** table pTab and store the column value in register iReg.
** There is an open cursor to pTab in 
** iTable.  If iColumn<0 then code is generated that extracts the rowid.
*/
void sqlite3ExprCodeGetColumn(
  Vdbe *v,         /* The VM being created */
  Table *pTab,     /* Description of the table we are reading from */
  int iColumn,     /* Index of the table column */
  int iTable,      /* The cursor pointing to the table */
  int iReg         /* Store results here */
){
  if( iColumn<0 ){
    int op = (pTab && IsVirtual(pTab)) ? OP_VRowid : OP_Rowid;
    sqlite3VdbeAddOp2(v, op, iTable, iReg);
  }else if( pTab==0 ){
    sqlite3VdbeAddOp3(v, OP_Column, iTable, iColumn, iReg);
  }else{
    int op = IsVirtual(pTab) ? OP_VColumn : OP_Column;
    sqlite3VdbeAddOp3(v, op, iTable, iColumn, iReg);
    sqlite3ColumnDefault(v, pTab, iColumn);
#ifndef SQLITE_OMIT_FLOATING_POINT
    if( pTab->aCol[iColumn].affinity==SQLITE_AFF_REAL ){
      sqlite3VdbeAddOp1(v, OP_RealAffinity, iReg);
    }
#endif
  }
}

/*
** Generate code into the current Vdbe to evaluate the given
** expression.  Attempt to store the results in register "target".
** Return the register where results are stored.
**
** With this routine, there is no guaranteed that results will
** be stored in target.  The result might be stored in some other
** register if it is convenient to do so.  The calling function
** must check the return code and move the results to the desired
** register.
*/
static int sqlite3ExprCodeTarget(Parse *pParse, Expr *pExpr, int target){
  Vdbe *v = pParse->pVdbe;  /* The VM under construction */
  int op;                   /* The opcode being coded */
  int inReg = target;       /* Results stored in register inReg */
  int regFree1 = 0;         /* If non-zero free this temporary register */
  int regFree2 = 0;         /* If non-zero free this temporary register */
  int r1, r2, r3;           /* Various register numbers */

  assert( v!=0 || pParse->db->mallocFailed );
  assert( target>0 && target<=pParse->nMem );
  if( v==0 ) return 0;

  if( pExpr==0 ){
    op = TK_NULL;
  }else{
    op = pExpr->op;
  }
  switch( op ){
    case TK_AGG_COLUMN: {
      AggInfo *pAggInfo = pExpr->pAggInfo;
      struct AggInfo_col *pCol = &pAggInfo->aCol[pExpr->iAgg];
      if( !pAggInfo->directMode ){
        assert( pCol->iMem>0 );
        inReg = pCol->iMem;
        break;
      }else if( pAggInfo->useSortingIdx ){
        sqlite3VdbeAddOp3(v, OP_Column, pAggInfo->sortingIdx,
                              pCol->iSorterColumn, target);
        break;
      }
      /* Otherwise, fall thru into the TK_COLUMN case */
    }
    case TK_COLUMN: {
      if( pExpr->iTable<0 ){
        /* This only happens when coding check constraints */
        assert( pParse->ckBase>0 );
        inReg = pExpr->iColumn + pParse->ckBase;
      }else{
        sqlite3ExprCodeGetColumn(v, pExpr->pTab,
                                 pExpr->iColumn, pExpr->iTable, target);
      }
      break;
    }
    case TK_INTEGER: {
      codeInteger(v, (char*)pExpr->token.z, pExpr->token.n, 0, target);
      break;
    }
    case TK_FLOAT: {
      codeReal(v, (char*)pExpr->token.z, pExpr->token.n, 0, target);
      break;
    }
    case TK_STRING: {
      sqlite3DequoteExpr(pParse->db, pExpr);
      sqlite3VdbeAddOp4(v,OP_String8, 0, target, 0,
                        (char*)pExpr->token.z, pExpr->token.n);
      break;
    }
    case TK_NULL: {
      sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      break;
    }
#ifndef SQLITE_OMIT_BLOB_LITERAL
    case TK_BLOB: {
      int n;
      const char *z;
      char *zBlob;
      assert( pExpr->token.n>=3 );
      assert( pExpr->token.z[0]=='x' || pExpr->token.z[0]=='X' );
      assert( pExpr->token.z[1]=='\'' );
      assert( pExpr->token.z[pExpr->token.n-1]=='\'' );
      n = pExpr->token.n - 3;
      z = (char*)pExpr->token.z + 2;
      zBlob = sqlite3HexToBlob(sqlite3VdbeDb(v), z, n);
      sqlite3VdbeAddOp4(v, OP_Blob, n/2, target, 0, zBlob, P4_DYNAMIC);
      break;
    }
#endif
    case TK_VARIABLE: {
      sqlite3VdbeAddOp2(v, OP_Variable, pExpr->iTable, target);
      if( pExpr->token.n>1 ){
        sqlite3VdbeChangeP4(v, -1, (char*)pExpr->token.z, pExpr->token.n);
      }
      break;
    }
    case TK_REGISTER: {
      inReg = pExpr->iTable;
      break;
    }
#ifndef SQLITE_OMIT_CAST
    case TK_CAST: {
      /* Expressions of the form:   CAST(pLeft AS token) */
      int aff, to_op;
      inReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
      aff = sqlite3AffinityType(&pExpr->token);
      to_op = aff - SQLITE_AFF_TEXT + OP_ToText;
      assert( to_op==OP_ToText    || aff!=SQLITE_AFF_TEXT    );
      assert( to_op==OP_ToBlob    || aff!=SQLITE_AFF_NONE    );
      assert( to_op==OP_ToNumeric || aff!=SQLITE_AFF_NUMERIC );
      assert( to_op==OP_ToInt     || aff!=SQLITE_AFF_INTEGER );
      assert( to_op==OP_ToReal    || aff!=SQLITE_AFF_REAL    );
      sqlite3VdbeAddOp1(v, to_op, inReg);
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
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op,
                  r1, r2, inReg, SQLITE_STOREP2);
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
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      sqlite3VdbeAddOp3(v, op, r2, r1, target);
      break;
    }
    case TK_UMINUS: {
      Expr *pLeft = pExpr->pLeft;
      assert( pLeft );
      if( pLeft->op==TK_FLOAT || pLeft->op==TK_INTEGER ){
        Token *p = &pLeft->token;
        if( pLeft->op==TK_FLOAT ){
          codeReal(v, (char*)p->z, p->n, 1, target);
        }else{
          codeInteger(v, (char*)p->z, p->n, 1, target);
        }
      }else{
        regFree1 = r1 = sqlite3GetTempReg(pParse);
        sqlite3VdbeAddOp2(v, OP_Integer, 0, r1);
        r2 = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
        sqlite3VdbeAddOp3(v, OP_Subtract, r2, r1, target);
      }
      inReg = target;
      break;
    }
    case TK_BITNOT:
    case TK_NOT: {
      assert( TK_BITNOT==OP_BitNot );
      assert( TK_NOT==OP_Not );
      inReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
      sqlite3VdbeAddOp1(v, op, inReg);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      int addr;
      assert( TK_ISNULL==OP_IsNull );
      assert( TK_NOTNULL==OP_NotNull );
      sqlite3VdbeAddOp2(v, OP_Integer, 1, target);
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      addr = sqlite3VdbeAddOp1(v, op, r1);
      sqlite3VdbeAddOp2(v, OP_AddImm, target, -1);
      sqlite3VdbeJumpHere(v, addr);
      break;
    }
    case TK_AGG_FUNCTION: {
      AggInfo *pInfo = pExpr->pAggInfo;
      if( pInfo==0 ){
        sqlite3ErrorMsg(pParse, "misuse of aggregate: %T",
            &pExpr->span);
      }else{
        inReg = pInfo->aFunc[pExpr->iAgg].iMem;
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
      sqlite3 *db = pParse->db;
      u8 enc = ENC(db);
      CollSeq *pColl = 0;

      zId = (char*)pExpr->token.z;
      nId = pExpr->token.n;
      pDef = sqlite3FindFunction(pParse->db, zId, nId, nExpr, enc, 0);
      assert( pDef!=0 );
      if( pList ){
        nExpr = pList->nExpr;
        r1 = sqlite3GetTempRange(pParse, nExpr);
        sqlite3ExprCodeExprList(pParse, pList, r1);
      }else{
        nExpr = r1 = 0;
      }
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
        pDef = sqlite3VtabOverloadFunction(db, pDef, nExpr, pList->a[1].pExpr);
      }else if( nExpr>0 ){
        pDef = sqlite3VtabOverloadFunction(db, pDef, nExpr, pList->a[0].pExpr);
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
        sqlite3VdbeAddOp4(v, OP_CollSeq, 0, 0, 0, (char *)pColl, P4_COLLSEQ);
      }
      sqlite3VdbeAddOp4(v, OP_Function, constMask, r1, target,
                        (char*)pDef, P4_FUNCDEF);
      sqlite3VdbeChangeP5(v, nExpr);
      if( nExpr ){
        sqlite3ReleaseTempRange(pParse, r1, nExpr);
      }
      break;
    }
#ifndef SQLITE_OMIT_SUBQUERY
    case TK_EXISTS:
    case TK_SELECT: {
      if( pExpr->iColumn==0 ){
        sqlite3CodeSubselect(pParse, pExpr);
      }
      inReg = pExpr->iColumn;
      break;
    }
    case TK_IN: {
      int j1, j2, j3, j4, j5;
      char affinity;
      int eType;

      eType = sqlite3FindInIndex(pParse, pExpr, 0);

      /* Figure out the affinity to use to create a key from the results
      ** of the expression. affinityStr stores a static string suitable for
      ** P4 of OP_MakeRecord.
      */
      affinity = comparisonAffinity(pExpr);

      sqlite3VdbeAddOp2(v, OP_Integer, 1, target);

      /* Code the <expr> from "<expr> IN (...)". The temporary table
      ** pExpr->iTable contains the values that make up the (...) set.
      */
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      j1 = sqlite3VdbeAddOp1(v, OP_NotNull, r1);
      sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      j2  = sqlite3VdbeAddOp0(v, OP_Goto);
      sqlite3VdbeJumpHere(v, j1);
      if( eType==IN_INDEX_ROWID ){
        j3 = sqlite3VdbeAddOp3(v, OP_MustBeInt, r1, 0, 1);
        j4 = sqlite3VdbeAddOp3(v, OP_NotExists, pExpr->iTable, 0, r1);
        j5 = sqlite3VdbeAddOp0(v, OP_Goto);
        sqlite3VdbeJumpHere(v, j3);
        sqlite3VdbeJumpHere(v, j4);
      }else{
        r2 = regFree2 = sqlite3GetTempReg(pParse);
        sqlite3VdbeAddOp4(v, OP_MakeRecord, r1, 1, r2, &affinity, 1);
        j5 = sqlite3VdbeAddOp3(v, OP_Found, pExpr->iTable, 0, r2);
      }
      sqlite3VdbeAddOp2(v, OP_AddImm, target, -1);
      sqlite3VdbeJumpHere(v, j2);
      sqlite3VdbeJumpHere(v, j5);
      break;
    }
#endif
    /*
    **    x BETWEEN y AND z
    **
    ** This is equivalent to
    **
    **    x>=y AND x<=z
    **
    ** X is stored in pExpr->pLeft.
    ** Y is stored in pExpr->pList->a[0].pExpr.
    ** Z is stored in pExpr->pList->a[1].pExpr.
    */
    case TK_BETWEEN: {
      Expr *pLeft = pExpr->pLeft;
      struct ExprList_item *pLItem = pExpr->pList->a;
      Expr *pRight = pLItem->pExpr;

      r1 = sqlite3ExprCodeTemp(pParse, pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pRight, &regFree2);
      r3 = sqlite3GetTempReg(pParse);
      codeCompare(pParse, pLeft, pRight, OP_Ge,
                  r1, r2, r3, SQLITE_STOREP2);
      pLItem++;
      pRight = pLItem->pExpr;
      sqlite3ReleaseTempReg(pParse, regFree2);
      r2 = sqlite3ExprCodeTemp(pParse, pRight, &regFree2);
      codeCompare(pParse, pLeft, pRight, OP_Le, r1, r2, r2, SQLITE_STOREP2);
      sqlite3VdbeAddOp3(v, OP_And, r3, r2, target);
      sqlite3ReleaseTempReg(pParse, r3);
      break;
    }
    case TK_UPLUS: {
      inReg = sqlite3ExprCodeTarget(pParse, pExpr->pLeft, target);
      break;
    }

    /*
    ** Form A:
    **   CASE x WHEN e1 THEN r1 WHEN e2 THEN r2 ... WHEN eN THEN rN ELSE y END
    **
    ** Form B:
    **   CASE WHEN e1 THEN r1 WHEN e2 THEN r2 ... WHEN eN THEN rN ELSE y END
    **
    ** Form A is can be transformed into the equivalent form B as follows:
    **   CASE WHEN x=e1 THEN r1 WHEN x=e2 THEN r2 ...
    **        WHEN x=eN THEN rN ELSE y END
    **
    ** X (if it exists) is in pExpr->pLeft.
    ** Y is in pExpr->pRight.  The Y is also optional.  If there is no
    ** ELSE clause and no other term matches, then the result of the
    ** exprssion is NULL.
    ** Ei is in pExpr->pList->a[i*2] and Ri is pExpr->pList->a[i*2+1].
    **
    ** The result of the expression is the Ri for the first matching Ei,
    ** or if there is no matching Ei, the ELSE term Y, or if there is
    ** no ELSE term, NULL.
    */
    case TK_CASE: {
      int endLabel;                     /* GOTO label for end of CASE stmt */
      int nextCase;                     /* GOTO label for next WHEN clause */
      int nExpr;                        /* 2x number of WHEN terms */
      int i;                            /* Loop counter */
      ExprList *pEList;                 /* List of WHEN terms */
      struct ExprList_item *aListelem;  /* Array of WHEN terms */
      Expr opCompare;                   /* The X==Ei expression */
      Expr cacheX;                      /* Cached expression X */
      Expr *pX;                         /* The X expression */
      Expr *pTest;                      /* X==Ei (form A) or just Ei (form B) */

      assert(pExpr->pList);
      assert((pExpr->pList->nExpr % 2) == 0);
      assert(pExpr->pList->nExpr > 0);
      pEList = pExpr->pList;
      aListelem = pEList->a;
      nExpr = pEList->nExpr;
      endLabel = sqlite3VdbeMakeLabel(v);
      if( (pX = pExpr->pLeft)!=0 ){
        cacheX = *pX;
        cacheX.iTable = sqlite3ExprCodeTemp(pParse, pX, &regFree1);
        cacheX.op = TK_REGISTER;
        opCompare.op = TK_EQ;
        opCompare.pLeft = &cacheX;
        pTest = &opCompare;
      }
      for(i=0; i<nExpr; i=i+2){
        if( pX ){
          opCompare.pRight = aListelem[i].pExpr;
        }else{
          pTest = aListelem[i].pExpr;
        }
        nextCase = sqlite3VdbeMakeLabel(v);
        sqlite3ExprIfFalse(pParse, pTest, nextCase, SQLITE_JUMPIFNULL);
        sqlite3ExprCode(pParse, aListelem[i+1].pExpr, target);
        sqlite3VdbeAddOp2(v, OP_Goto, 0, endLabel);
        sqlite3VdbeResolveLabel(v, nextCase);
      }
      if( pExpr->pRight ){
        sqlite3ExprCode(pParse, pExpr->pRight, target);
      }else{
        sqlite3VdbeAddOp2(v, OP_Null, 0, target);
      }
      sqlite3VdbeResolveLabel(v, endLabel);
      break;
    }
#ifndef SQLITE_OMIT_TRIGGER
    case TK_RAISE: {
      if( !pParse->trigStack ){
        sqlite3ErrorMsg(pParse,
                       "RAISE() may only be used within a trigger-program");
        return 0;
      }
      if( pExpr->iColumn!=OE_Ignore ){
         assert( pExpr->iColumn==OE_Rollback ||
                 pExpr->iColumn == OE_Abort ||
                 pExpr->iColumn == OE_Fail );
         sqlite3DequoteExpr(pParse->db, pExpr);
         sqlite3VdbeAddOp4(v, OP_Halt, SQLITE_CONSTRAINT, pExpr->iColumn, 0,
                        (char*)pExpr->token.z, pExpr->token.n);
      } else {
         assert( pExpr->iColumn == OE_Ignore );
         sqlite3VdbeAddOp2(v, OP_ContextPop, 0, 0);
         sqlite3VdbeAddOp2(v, OP_Goto, 0, pParse->trigStack->ignoreJump);
         VdbeComment((v, "raise(IGNORE)"));
      }
      break;
    }
#endif
  }
  sqlite3ReleaseTempReg(pParse, regFree1);
  sqlite3ReleaseTempReg(pParse, regFree2);
  return inReg;
}

/*
** Generate code to evaluate an expression and store the results
** into a register.  Return the register number where the results
** are stored.
**
** If the register is a temporary register that can be deallocated,
** then write its number into *pReg.  If the result register is no
** a temporary, then set *pReg to zero.
*/
int sqlite3ExprCodeTemp(Parse *pParse, Expr *pExpr, int *pReg){
  int r1 = sqlite3GetTempReg(pParse);
  int r2 = sqlite3ExprCodeTarget(pParse, pExpr, r1);
  if( r2==r1 ){
    *pReg = r1;
  }else{
    sqlite3ReleaseTempReg(pParse, r1);
    *pReg = 0;
  }
  return r2;
}

/*
** Generate code that will evaluate expression pExpr and store the
** results in register target.  The results are guaranteed to appear
** in register target.
*/
int sqlite3ExprCode(Parse *pParse, Expr *pExpr, int target){
  int inReg;

  assert( target>0 && target<=pParse->nMem );
  inReg = sqlite3ExprCodeTarget(pParse, pExpr, target);
  assert( pParse->pVdbe || pParse->db->mallocFailed );
  if( inReg!=target && pParse->pVdbe ){
    sqlite3VdbeAddOp2(pParse->pVdbe, OP_SCopy, inReg, target);
  }
  return target;
}

/*
** Generate code that evalutes the given expression and puts the result
** in register target.
**
** Also make a copy of the expression results into another "cache" register
** and modify the expression so that the next time it is evaluated,
** the result is a copy of the cache register.
**
** This routine is used for expressions that are used multiple 
** times.  They are evaluated once and the results of the expression
** are reused.
*/
int sqlite3ExprCodeAndCache(Parse *pParse, Expr *pExpr, int target){
  Vdbe *v = pParse->pVdbe;
  int inReg;
  inReg = sqlite3ExprCode(pParse, pExpr, target);
  assert( target>0 );
  if( pExpr->op!=TK_REGISTER ){  
    int iMem;
    iMem = ++pParse->nMem;
    sqlite3VdbeAddOp2(v, OP_Copy, inReg, iMem);
    pExpr->iTable = iMem;
    pExpr->op = TK_REGISTER;
  }
  return inReg;
}


/*
** Generate code that pushes the value of every element of the given
** expression list into a sequence of registers beginning at target.
**
** Return the number of elements evaluated.
*/
int sqlite3ExprCodeExprList(
  Parse *pParse,     /* Parsing context */
  ExprList *pList,   /* The expression list to be coded */
  int target         /* Where to write results */
){
  struct ExprList_item *pItem;
  int i, n;
  assert( pList!=0 || pParse->db->mallocFailed );
  if( pList==0 ){
    return 0;
  }
  assert( target>0 );
  n = pList->nExpr;
  for(pItem=pList->a, i=n; i>0; i--, pItem++){
    sqlite3ExprCode(pParse, pItem->pExpr, target);
    target++;
  }
  return n;
}

/*
** Generate code for a boolean expression such that a jump is made
** to the label "dest" if the expression is true but execution
** continues straight thru if the expression is false.
**
** If the expression evaluates to NULL (neither true nor false), then
** take the jump if the jumpIfNull flag is SQLITE_JUMPIFNULL.
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
  int regFree1 = 0;
  int regFree2 = 0;
  int r1, r2;

  assert( jumpIfNull==SQLITE_JUMPIFNULL || jumpIfNull==0 );
  if( v==0 || pExpr==0 ) return;
  op = pExpr->op;
  switch( op ){
    case TK_AND: {
      int d2 = sqlite3VdbeMakeLabel(v);
      sqlite3ExprIfFalse(pParse, pExpr->pLeft, d2,jumpIfNull^SQLITE_JUMPIFNULL);
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
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op,
                  r1, r2, dest, jumpIfNull);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      assert( TK_ISNULL==OP_IsNull );
      assert( TK_NOTNULL==OP_NotNull );
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      sqlite3VdbeAddOp2(v, op, r1, dest);
      break;
    }
    case TK_BETWEEN: {
      /*    x BETWEEN y AND z
      **
      ** Is equivalent to 
      **
      **    x>=y AND x<=z
      **
      ** Code it as such, taking care to do the common subexpression
      ** elementation of x.
      */
      Expr exprAnd;
      Expr compLeft;
      Expr compRight;
      Expr exprX;

      exprX = *pExpr->pLeft;
      exprAnd.op = TK_AND;
      exprAnd.pLeft = &compLeft;
      exprAnd.pRight = &compRight;
      compLeft.op = TK_GE;
      compLeft.pLeft = &exprX;
      compLeft.pRight = pExpr->pList->a[0].pExpr;
      compRight.op = TK_LE;
      compRight.pLeft = &exprX;
      compRight.pRight = pExpr->pList->a[1].pExpr;
      exprX.iTable = sqlite3ExprCodeTemp(pParse, &exprX, &regFree1);
      exprX.op = TK_REGISTER;
      sqlite3ExprIfTrue(pParse, &exprAnd, dest, jumpIfNull);
      break;
    }
    default: {
      r1 = sqlite3ExprCodeTemp(pParse, pExpr, &regFree1);
      sqlite3VdbeAddOp3(v, OP_If, r1, dest, jumpIfNull!=0);
      break;
    }
  }
  sqlite3ReleaseTempReg(pParse, regFree1);
  sqlite3ReleaseTempReg(pParse, regFree2);  
}

/*
** Generate code for a boolean expression such that a jump is made
** to the label "dest" if the expression is false but execution
** continues straight thru if the expression is true.
**
** If the expression evaluates to NULL (neither true nor false) then
** jump if jumpIfNull is SQLITE_JUMPIFNULL or fall through if jumpIfNull
** is 0.
*/
void sqlite3ExprIfFalse(Parse *pParse, Expr *pExpr, int dest, int jumpIfNull){
  Vdbe *v = pParse->pVdbe;
  int op = 0;
  int regFree1 = 0;
  int regFree2 = 0;
  int r1, r2;

  assert( jumpIfNull==SQLITE_JUMPIFNULL || jumpIfNull==0 );
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
      sqlite3ExprIfTrue(pParse, pExpr->pLeft, d2, jumpIfNull^SQLITE_JUMPIFNULL);
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
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      r2 = sqlite3ExprCodeTemp(pParse, pExpr->pRight, &regFree2);
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op,
                  r1, r2, dest, jumpIfNull);
      break;
    }
    case TK_ISNULL:
    case TK_NOTNULL: {
      r1 = sqlite3ExprCodeTemp(pParse, pExpr->pLeft, &regFree1);
      sqlite3VdbeAddOp2(v, op, r1, dest);
      break;
    }
    case TK_BETWEEN: {
      /*    x BETWEEN y AND z
      **
      ** Is equivalent to 
      **
      **    x>=y AND x<=z
      **
      ** Code it as such, taking care to do the common subexpression
      ** elementation of x.
      */
      Expr exprAnd;
      Expr compLeft;
      Expr compRight;
      Expr exprX;

      exprX = *pExpr->pLeft;
      exprAnd.op = TK_AND;
      exprAnd.pLeft = &compLeft;
      exprAnd.pRight = &compRight;
      compLeft.op = TK_GE;
      compLeft.pLeft = &exprX;
      compLeft.pRight = pExpr->pList->a[0].pExpr;
      compRight.op = TK_LE;
      compRight.pLeft = &exprX;
      compRight.pRight = pExpr->pList->a[1].pExpr;
      exprX.iTable = sqlite3ExprCodeTemp(pParse, &exprX, &regFree1);
      exprX.op = TK_REGISTER;
      sqlite3ExprIfFalse(pParse, &exprAnd, dest, jumpIfNull);
      break;
    }
    default: {
      r1 = sqlite3ExprCodeTemp(pParse, pExpr, &regFree1);
      sqlite3VdbeAddOp3(v, OP_IfNot, r1, dest, jumpIfNull!=0);
      break;
    }
  }
  sqlite3ReleaseTempReg(pParse, regFree1);
  sqlite3ReleaseTempReg(pParse, regFree2);
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
static int addAggInfoColumn(sqlite3 *db, AggInfo *pInfo){
  int i;
  pInfo->aCol = sqlite3ArrayAllocate(
       db,
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
static int addAggInfoFunc(sqlite3 *db, AggInfo *pInfo){
  int i;
  pInfo->aFunc = sqlite3ArrayAllocate(
       db, 
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
            if( (k>=pAggInfo->nColumn)
             && (k = addAggInfoColumn(pParse->db, pAggInfo))>=0 
            ){
              pCol = &pAggInfo->aCol[k];
              pCol->pTab = pExpr->pTab;
              pCol->iTable = pExpr->iTable;
              pCol->iColumn = pExpr->iColumn;
              pCol->iMem = ++pParse->nMem;
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
          i = addAggInfoFunc(pParse->db, pAggInfo);
          if( i>=0 ){
            pItem = &pAggInfo->aFunc[i];
            pItem->pExpr = pExpr;
            pItem->iMem = ++pParse->nMem;
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
*/
void sqlite3ExprAnalyzeAggregates(NameContext *pNC, Expr *pExpr){
  walkExprTree(pExpr, analyzeAggregate, pNC);
}

/*
** Call sqlite3ExprAnalyzeAggregates() for every expression in an
** expression list.  Return the number of errors.
**
** If an error is found, the analysis is cut short.
*/
void sqlite3ExprAnalyzeAggList(NameContext *pNC, ExprList *pList){
  struct ExprList_item *pItem;
  int i;
  if( pList ){
    for(pItem=pList->a, i=0; i<pList->nExpr; i++, pItem++){
      sqlite3ExprAnalyzeAggregates(pNC, pItem->pExpr);
    }
  }
}

/*
** Allocate or deallocate temporary use registers during code generation.
*/
int sqlite3GetTempReg(Parse *pParse){
  if( pParse->nTempReg ){
    return pParse->aTempReg[--pParse->nTempReg];
  }else{
    return ++pParse->nMem;
  }
}
void sqlite3ReleaseTempReg(Parse *pParse, int iReg){
  if( iReg && pParse->nTempReg<ArraySize(pParse->aTempReg) ){
    assert( iReg>0 );
    pParse->aTempReg[pParse->nTempReg++] = iReg;
  }
}

/*
** Allocate or deallocate a block of nReg consecutive registers
*/
int sqlite3GetTempRange(Parse *pParse, int nReg){
  int i;
  if( nReg<=pParse->nRangeReg ){
    i  = pParse->iRangeReg;
    pParse->iRangeReg += nReg;
    pParse->nRangeReg -= nReg;
  }else{
    i = pParse->nMem+1;
    pParse->nMem += nReg;
  }
  return i;
}
void sqlite3ReleaseTempRange(Parse *pParse, int iReg, int nReg){
  if( nReg>pParse->nRangeReg ){
    pParse->nRangeReg = nReg;
    pParse->iRangeReg = iReg;
  }
}
