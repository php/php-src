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
  if( pExpr->op==TK_AS ){
    return sqlite3ExprAffinity(pExpr->pLeft);
  }
  if( pExpr->op==TK_SELECT ){
    return sqlite3ExprAffinity(pExpr->pSelect->pEList->a[0].pExpr);
  }
  return pExpr->affinity;
}

/*
** Return the default collation sequence for the expression pExpr. If
** there is no default collation type, return 0.
*/
CollSeq *sqlite3ExprCollSeq(Parse *pParse, Expr *pExpr){
  CollSeq *pColl = 0;
  if( pExpr ){
    pColl = pExpr->pColl;
    if( pExpr->op==TK_AS && !pColl ){
      return sqlite3ExprCollSeq(pParse, pExpr->pLeft);
    }
  }
  if( sqlite3CheckCollSeq(pParse, pColl) ){ 
    pColl = 0;
  }
  return pColl;
}

/*
** pExpr is the left operand of a comparison operator.  aff2 is the
** type affinity of the right operand.  This routine returns the
** type affinity that should be used for the comparison operator.
*/
char sqlite3CompareAffinity(Expr *pExpr, char aff2){
  char aff1 = sqlite3ExprAffinity(pExpr);
  if( aff1 && aff2 ){
    /* Both sides of the comparison are columns. If one has numeric or
    ** integer affinity, use that. Otherwise use no affinity.
    */
    if( aff1==SQLITE_AFF_INTEGER || aff2==SQLITE_AFF_INTEGER ){
      return SQLITE_AFF_INTEGER;
    }else if( aff1==SQLITE_AFF_NUMERIC || aff2==SQLITE_AFF_NUMERIC ){
      return SQLITE_AFF_NUMERIC;
    }else{
      return SQLITE_AFF_NONE;
    }
  }else if( !aff1 && !aff2 ){
    /* Neither side of the comparison is a column.  Compare the
    ** results directly.
    */
    /* return SQLITE_AFF_NUMERIC;  // Ticket #805 */
    return SQLITE_AFF_NONE;
  }else{
    /* One side is a column, the other is not. Use the columns affinity. */
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
    aff = SQLITE_AFF_NUMERIC;
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
  return 
    (aff==SQLITE_AFF_NONE) ||
    (aff==SQLITE_AFF_NUMERIC && idx_affinity==SQLITE_AFF_INTEGER) ||
    (aff==SQLITE_AFF_INTEGER && idx_affinity==SQLITE_AFF_NUMERIC) ||
    (aff==idx_affinity);
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
  return (((int)sqlite3CompareAffinity(pExpr1, aff))<<8)+(jumpIfNull?1:0);
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
  CollSeq *pColl = sqlite3ExprCollSeq(pParse, pLeft);
  if( !pColl ){
    pColl = sqlite3ExprCollSeq(pParse, pRight);
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
Expr *sqlite3Expr(int op, Expr *pLeft, Expr *pRight, Token *pToken){
  Expr *pNew;
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    /* When malloc fails, we leak memory from pLeft and pRight */
    return 0;
  }
  pNew->op = op;
  pNew->pLeft = pLeft;
  pNew->pRight = pRight;
  if( pToken ){
    assert( pToken->dyn==0 );
    pNew->span = pNew->token = *pToken;
  }else if( pLeft && pRight ){
    sqlite3ExprSpan(pNew, &pLeft->span, &pRight->span);
  }
  return pNew;
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
  if( !sqlite3_malloc_failed && pRight->z && pLeft->z ){
    assert( pLeft->dyn==0 || pLeft->z[pLeft->n]==0 );
    if( pLeft->dyn==0 && pRight->dyn==0 ){
      pExpr->span.z = pLeft->z;
      pExpr->span.n = pRight->n + Addr(pRight->z) - Addr(pLeft->z);
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
  pNew = sqliteMalloc( sizeof(Expr) );
  if( pNew==0 ){
    /* sqlite3ExprListDelete(pList); // Leak pList when malloc fails */
    return 0;
  }
  pNew->op = TK_FUNCTION;
  pNew->pList = pList;
  if( pToken ){
    assert( pToken->dyn==0 );
    pNew->token = *pToken;
  }else{
    pNew->token.z = 0;
  }
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
    pExpr->iTable = i = atoi(&pToken->z[1]);
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
        pParse->apVarExpr = sqliteRealloc(pParse->apVarExpr,
                       pParse->nVarExprAlloc*sizeof(pParse->apVarExpr[0]) );
      }
      if( !sqlite3_malloc_failed ){
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
    pNew->token.z = sqliteStrDup(p->token.z);
    pNew->token.dyn = 1;
  }else{
    assert( pNew->token.z==0 );
  }
  pNew->span.z = 0;
  pNew->pLeft = sqlite3ExprDup(p->pLeft);
  pNew->pRight = sqlite3ExprDup(p->pRight);
  pNew->pList = sqlite3ExprListDup(p->pList);
  pNew->pSelect = sqlite3SelectDup(p->pSelect);
  return pNew;
}
void sqlite3TokenCopy(Token *pTo, Token *pFrom){
  if( pTo->dyn ) sqliteFree((char*)pTo->z);
  if( pFrom->z ){
    pTo->n = pFrom->n;
    pTo->z = sqliteStrNDup(pFrom->z, pFrom->n);
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
            || pOldExpr->span.z==0 || sqlite3_malloc_failed );
    pItem->zName = sqliteStrDup(pOldItem->zName);
    pItem->sortOrder = pOldItem->sortOrder;
    pItem->isAgg = pOldItem->isAgg;
    pItem->done = 0;
  }
  return pNew;
}
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
    pNewItem->zDatabase = sqliteStrDup(pOldItem->zDatabase);
    pNewItem->zName = sqliteStrDup(pOldItem->zName);
    pNewItem->zAlias = sqliteStrDup(pOldItem->zAlias);
    pNewItem->jointype = pOldItem->jointype;
    pNewItem->iCursor = pOldItem->iCursor;
    pNewItem->pTab = 0;
    pNewItem->pSelect = sqlite3SelectDup(pOldItem->pSelect);
    pNewItem->pOn = sqlite3ExprDup(pOldItem->pOn);
    pNewItem->pUsing = sqlite3IdListDup(pOldItem->pUsing);
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
  if( pNew->a==0 ) return 0;
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
  pNew->nLimit = p->nLimit;
  pNew->nOffset = p->nOffset;
  pNew->zSelect = 0;
  pNew->iLimit = -1;
  pNew->iOffset = -1;
  pNew->ppOpenTemp = 0;
  return pNew;
}


/*
** Add a new element to the end of an expression list.  If pList is
** initially NULL, then create a new expression list.
*/
ExprList *sqlite3ExprListAppend(ExprList *pList, Expr *pExpr, Token *pName){
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(ExprList) );
    if( pList==0 ){
      /* sqlite3ExprDelete(pExpr); // Leak memory if malloc fails */
      return 0;
    }
    assert( pList->nAlloc==0 );
  }
  if( pList->nAlloc<=pList->nExpr ){
    pList->nAlloc = pList->nAlloc*2 + 4;
    pList->a = sqliteRealloc(pList->a, pList->nAlloc*sizeof(pList->a[0]));
    if( pList->a==0 ){
      /* sqlite3ExprDelete(pExpr); // Leak memory if malloc fails */
      pList->nExpr = pList->nAlloc = 0;
      return pList;
    }
  }
  assert( pList->a!=0 );
  if( pExpr || pName ){
    struct ExprList_item *pItem = &pList->a[pList->nExpr++];
    memset(pItem, 0, sizeof(*pItem));
    pItem->pExpr = pExpr;
    pItem->zName = sqlite3NameFromToken(pName);
  }
  return pList;
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
** Walk an expression tree.  Return 1 if the expression is constant
** and 0 if it involves variables.
**
** For the purposes of this function, a double-quoted string (ex: "abc")
** is considered a variable but a single-quoted string (ex: 'abc') is
** a constant.
*/
int sqlite3ExprIsConstant(Expr *p){
  switch( p->op ){
    case TK_ID:
    case TK_COLUMN:
    case TK_DOT:
    case TK_FUNCTION:
      return 0;
    case TK_NULL:
    case TK_STRING:
    case TK_BLOB:
    case TK_INTEGER:
    case TK_FLOAT:
    case TK_VARIABLE:
      return 1;
    default: {
      if( p->pLeft && !sqlite3ExprIsConstant(p->pLeft) ) return 0;
      if( p->pRight && !sqlite3ExprIsConstant(p->pRight) ) return 0;
      if( p->pList ){
        int i;
        for(i=0; i<p->pList->nExpr; i++){
          if( !sqlite3ExprIsConstant(p->pList->a[i].pExpr) ) return 0;
        }
      }
      return p->pLeft!=0 || p->pRight!=0 || (p->pList && p->pList->nExpr>0);
    }
  }
  return 0;
}

/*
** If the given expression codes a constant integer that is small enough
** to fit in a 32-bit integer, return 1 and put the value of the integer
** in *pValue.  If the expression is not an integer or if it is too big
** to fit in a signed 32-bit integer, return 0 and leave *pValue unchanged.
*/
int sqlite3ExprIsInteger(Expr *p, int *pValue){
  switch( p->op ){
    case TK_INTEGER: {
      if( sqlite3GetInt32(p->token.z, pValue) ){
        return 1;
      }
      break;
    }
    case TK_STRING: {
      const u8 *z = (u8*)p->token.z;
      int n = p->token.n;
      if( n>0 && z[0]=='-' ){ z++; n--; }
      while( n>0 && *z && isdigit(*z) ){ z++; n--; }
      if( n==0 && sqlite3GetInt32(p->token.z, pValue) ){
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
  Parse *pParse,      /* The parsing context */
  Token *pDbToken,     /* Name of the database containing table, or NULL */
  Token *pTableToken,  /* Name of table containing column, or NULL */
  Token *pColumnToken, /* Name of the column. */
  SrcList *pSrcList,   /* List of tables used to resolve column names */
  ExprList *pEList,    /* List of expressions used to resolve "AS" */
  Expr *pExpr          /* Make this EXPR node point to the selected column */
){
  char *zDb = 0;       /* Name of the database.  The "X" in X.Y.Z */
  char *zTab = 0;      /* Name of the table.  The "Y" in X.Y.Z or Y.Z */
  char *zCol = 0;      /* Name of the column.  The "Z" */
  int i, j;            /* Loop counters */
  int cnt = 0;         /* Number of matching column names */
  int cntTab = 0;      /* Number of matching table names */
  sqlite3 *db = pParse->db;  /* The database */

  assert( pColumnToken && pColumnToken->z ); /* The Z in X.Y.Z cannot be NULL */
  zDb = sqlite3NameFromToken(pDbToken);
  zTab = sqlite3NameFromToken(pTableToken);
  zCol = sqlite3NameFromToken(pColumnToken);
  if( sqlite3_malloc_failed ){
    return 1;  /* Leak memory (zDb and zTab) if malloc fails */
  }
  assert( zTab==0 || pEList==0 );

  pExpr->iTable = -1;
  for(i=0; i<pSrcList->nSrc; i++){
    struct SrcList_item *pItem = &pSrcList->a[i];
    Table *pTab = pItem->pTab;
    Column *pCol;

    if( pTab==0 ) continue;
    assert( pTab->nCol>0 );
    if( zTab ){
      if( pItem->zAlias ){
        char *zTabName = pItem->zAlias;
        if( sqlite3StrICmp(zTabName, zTab)!=0 ) continue;
      }else{
        char *zTabName = pTab->zName;
        if( zTabName==0 || sqlite3StrICmp(zTabName, zTab)!=0 ) continue;
        if( zDb!=0 && sqlite3StrICmp(db->aDb[pTab->iDb].zName, zDb)!=0 ){
          continue;
        }
      }
    }
    if( 0==(cntTab++) ){
      pExpr->iTable = pItem->iCursor;
      pExpr->iDb = pTab->iDb;
    }
    for(j=0, pCol=pTab->aCol; j<pTab->nCol; j++, pCol++){
      if( sqlite3StrICmp(pCol->zName, zCol)==0 ){
        cnt++;
        pExpr->iTable = pItem->iCursor;
        pExpr->iDb = pTab->iDb;
        /* Substitute the rowid (column -1) for the INTEGER PRIMARY KEY */
        pExpr->iColumn = j==pTab->iPKey ? -1 : j;
        pExpr->affinity = pTab->aCol[j].affinity;
        pExpr->pColl = pTab->aCol[j].pColl;
        break;
      }
    }
  }

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
    }else if( pTriggerStack->oldIdx != -1 && sqlite3StrICmp("old", zTab) == 0 ){
      pExpr->iTable = pTriggerStack->oldIdx;
      assert( pTriggerStack->pTab );
      pTab = pTriggerStack->pTab;
    }

    if( pTab ){ 
      int j;
      Column *pCol = pTab->aCol;
      
      pExpr->iDb = pTab->iDb;
      cntTab++;
      for(j=0; j < pTab->nCol; j++, pCol++) {
        if( sqlite3StrICmp(pCol->zName, zCol)==0 ){
          cnt++;
          pExpr->iColumn = j==pTab->iPKey ? -1 : j;
          pExpr->affinity = pTab->aCol[j].affinity;
          pExpr->pColl = pTab->aCol[j].pColl;
          break;
        }
      }
    }
  }

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
  if( cnt==0 && pEList!=0 ){
    for(j=0; j<pEList->nExpr; j++){
      char *zAs = pEList->a[j].zName;
      if( zAs!=0 && sqlite3StrICmp(zAs, zCol)==0 ){
        assert( pExpr->pLeft==0 && pExpr->pRight==0 );
        pExpr->op = TK_AS;
        pExpr->iColumn = j;
        pExpr->pLeft = sqlite3ExprDup(pEList->a[j].pExpr);
        sqliteFree(zCol);
        assert( zTab==0 && zDb==0 );
        return 0;
      }
    } 
  }

  /*
  ** If X and Y are NULL (in other words if only the column name Z is
  ** supplied) and the value of Z is enclosed in double-quotes, then
  ** Z is a string literal if it doesn't match any column names.  In that
  ** case, we need to return right away and not make any changes to
  ** pExpr.
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
      sqlite3SetString(&z, zDb, ".", zTab, ".", zCol, 0);
    }else if( zTab ){
      sqlite3SetString(&z, zTab, ".", zCol, 0);
    }else{
      z = sqliteStrDup(zCol);
    }
    sqlite3ErrorMsg(pParse, zErr, z);
    sqliteFree(z);
  }

  /* Clean up and return
  */
  sqliteFree(zDb);
  sqliteFree(zTab);
  sqliteFree(zCol);
  sqlite3ExprDelete(pExpr->pLeft);
  pExpr->pLeft = 0;
  sqlite3ExprDelete(pExpr->pRight);
  pExpr->pRight = 0;
  pExpr->op = TK_COLUMN;
  sqlite3AuthRead(pParse, pExpr, pSrcList);
  return cnt!=1;
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
** We also check for instances of the IN operator.  IN comes in two
** forms:
**
**           expr IN (exprlist)
** and
**           expr IN (SELECT ...)
**
** The first form is handled by creating a set holding the list
** of allowed values.  The second form causes the SELECT to generate 
** a temporary table.
**
** This routine also looks for scalar SELECTs that are part of an expression.
** If it finds any, it generates code to write the value of that select
** into a memory cell.
**
** Unknown columns or tables provoke an error.  The function returns
** the number of errors seen and leaves an error message on pParse->zErrMsg.
*/
int sqlite3ExprResolveIds(
  Parse *pParse,     /* The parser context */
  SrcList *pSrcList, /* List of tables used to resolve column names */
  ExprList *pEList,  /* List of expressions used to resolve "AS" */
  Expr *pExpr        /* The expression to be analyzed. */
){
  int i;

  if( pExpr==0 || pSrcList==0 ) return 0;
  for(i=0; i<pSrcList->nSrc; i++){
    assert( pSrcList->a[i].iCursor>=0 && pSrcList->a[i].iCursor<pParse->nTab );
  }
  switch( pExpr->op ){
    /* Double-quoted strings (ex: "abc") are used as identifiers if
    ** possible.  Otherwise they remain as strings.  Single-quoted
    ** strings (ex: 'abc') are always string literals.
    */
    case TK_STRING: {
      if( pExpr->token.z[0]=='\'' ) break;
      /* Fall thru into the TK_ID case if this is a double-quoted string */
    }
    /* A lone identifier is the name of a columnd.
    */
    case TK_ID: {
      if( lookupName(pParse, 0, 0, &pExpr->token, pSrcList, pEList, pExpr) ){
        return 1;
      }
      break; 
    }
  
    /* A table name and column name:     ID.ID
    ** Or a database, table and column:  ID.ID.ID
    */
    case TK_DOT: {
      Token *pColumn;
      Token *pTable;
      Token *pDb;
      Expr *pRight;

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
      if( lookupName(pParse, pDb, pTable, pColumn, pSrcList, 0, pExpr) ){
        return 1;
      }
      break;
    }

    case TK_IN: {
      char affinity;
      Vdbe *v = sqlite3GetVdbe(pParse);
      KeyInfo keyInfo;
      int addr;        /* Address of OP_OpenTemp instruction */

      if( v==0 ) return 1;
      if( sqlite3ExprResolveIds(pParse, pSrcList, pEList, pExpr->pLeft) ){
        return 1;
      }
      affinity = sqlite3ExprAffinity(pExpr->pLeft);

      /* Whether this is an 'x IN(SELECT...)' or an 'x IN(<exprlist>)'
      ** expression it is handled the same way. A temporary table is 
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
      addr = sqlite3VdbeAddOp(v, OP_OpenTemp, pExpr->iTable, 0);
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
        sqlite3Select(pParse, pExpr->pSelect, SRT_Set, iParm, 0, 0, 0, 0);
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
        if( !affinity ){
          affinity = SQLITE_AFF_NUMERIC;
        }
        keyInfo.aColl[0] = pExpr->pLeft->pColl;

        /* Loop through each expression in <exprlist>. */
        for(i=0; i<pExpr->pList->nExpr; i++){
          Expr *pE2 = pExpr->pList->a[i].pExpr;

          /* Check that the expression is constant and valid. */
          if( !sqlite3ExprIsConstant(pE2) ){
            sqlite3ErrorMsg(pParse,
              "right-hand side of IN operator must be constant");
            return 1;
          }
          if( sqlite3ExprCheck(pParse, pE2, 0, 0) ){
            return 1;
          }

          /* Evaluate the expression and insert it into the temp table */
          sqlite3ExprCode(pParse, pE2);
          sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &affinity, 1);
          sqlite3VdbeAddOp(v, OP_String8, 0, 0);
          sqlite3VdbeAddOp(v, OP_PutStrKey, pExpr->iTable, 0);
        }
      }
      sqlite3VdbeChangeP3(v, addr, (void *)&keyInfo, P3_KEYINFO);

      break;
    }

    case TK_SELECT: {
      /* This has to be a scalar SELECT.  Generate code to put the
      ** value of this select in a memory cell and record the number
      ** of the memory cell in iColumn.
      */
      pExpr->iColumn = pParse->nMem++;
      if(sqlite3Select(pParse, pExpr->pSelect, SRT_Mem,pExpr->iColumn,0,0,0,0)){
        return 1;
      }
      break;
    }

    /* For all else, just recursively walk the tree */
    default: {
      if( pExpr->pLeft
      && sqlite3ExprResolveIds(pParse, pSrcList, pEList, pExpr->pLeft) ){
        return 1;
      }
      if( pExpr->pRight 
      && sqlite3ExprResolveIds(pParse, pSrcList, pEList, pExpr->pRight) ){
        return 1;
      }
      if( pExpr->pList ){
        int i;
        ExprList *pList = pExpr->pList;
        for(i=0; i<pList->nExpr; i++){
          Expr *pArg = pList->a[i].pExpr;
          if( sqlite3ExprResolveIds(pParse, pSrcList, pEList, pArg) ){
            return 1;
          }
        }
      }
    }
  }
  return 0;
}

/*
** pExpr is a node that defines a function of some kind.  It might
** be a syntactic function like "count(x)" or it might be a function
** that implements an operator, like "a LIKE b".  
**
** This routine makes *pzName point to the name of the function and 
** *pnName hold the number of characters in the function name.
*/
static void getFunctionName(Expr *pExpr, const char **pzName, int *pnName){
  switch( pExpr->op ){
    case TK_FUNCTION: {
      *pzName = pExpr->token.z;
      *pnName = pExpr->token.n;
      break;
    }
    case TK_LIKE: {
      *pzName = "like";
      *pnName = 4;
      break;
    }
    case TK_GLOB: {
      *pzName = "glob";
      *pnName = 4;
      break;
    }
    default: {
      *pzName = "can't happen";
      *pnName = 12;
      break;
    }
  }
}

/*
** Error check the functions in an expression.  Make sure all
** function names are recognized and all functions have the correct
** number of arguments.  Leave an error message in pParse->zErrMsg
** if anything is amiss.  Return the number of errors.
**
** if pIsAgg is not null and this expression is an aggregate function
** (like count(*) or max(value)) then write a 1 into *pIsAgg.
*/
int sqlite3ExprCheck(Parse *pParse, Expr *pExpr, int allowAgg, int *pIsAgg){
  int nErr = 0;
  if( pExpr==0 ) return 0;
  switch( pExpr->op ){
    case TK_GLOB:
    case TK_LIKE:
    case TK_FUNCTION: {
      int n = pExpr->pList ? pExpr->pList->nExpr : 0;  /* Number of arguments */
      int no_such_func = 0;       /* True if no such function exists */
      int wrong_num_args = 0;     /* True if wrong number of arguments */
      int is_agg = 0;             /* True if is an aggregate function */
      int i;
      int nId;                    /* Number of characters in function name */
      const char *zId;            /* The function name. */
      FuncDef *pDef;
      int enc = pParse->db->enc;

      getFunctionName(pExpr, &zId, &nId);
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
      if( is_agg && !allowAgg ){
        sqlite3ErrorMsg(pParse, "misuse of aggregate function %.*s()", nId, zId);
        nErr++;
        is_agg = 0;
      }else if( no_such_func ){
        sqlite3ErrorMsg(pParse, "no such function: %.*s", nId, zId);
        nErr++;
      }else if( wrong_num_args ){
        sqlite3ErrorMsg(pParse,"wrong number of arguments to function %.*s()",
             nId, zId);
        nErr++;
      }
      if( is_agg ){
        pExpr->op = TK_AGG_FUNCTION;
        if( pIsAgg ) *pIsAgg = 1;
      }
      for(i=0; nErr==0 && i<n; i++){
        nErr = sqlite3ExprCheck(pParse, pExpr->pList->a[i].pExpr,
                               allowAgg && !is_agg, pIsAgg);
      }
      /* FIX ME:  Compute pExpr->affinity based on the expected return
      ** type of the function 
      */
    }
    default: {
      if( pExpr->pLeft ){
        nErr = sqlite3ExprCheck(pParse, pExpr->pLeft, allowAgg, pIsAgg);
      }
      if( nErr==0 && pExpr->pRight ){
        nErr = sqlite3ExprCheck(pParse, pExpr->pRight, allowAgg, pIsAgg);
      }
      if( nErr==0 && pExpr->pList ){
        int n = pExpr->pList->nExpr;
        int i;
        for(i=0; nErr==0 && i<n; i++){
          Expr *pE2 = pExpr->pList->a[i].pExpr;
          nErr = sqlite3ExprCheck(pParse, pE2, allowAgg, pIsAgg);
        }
      }
      break;
    }
  }
  return nErr;
}

/*
** Call sqlite3ExprResolveIds() followed by sqlite3ExprCheck().
**
** This routine is provided as a convenience since it is very common
** to call ResolveIds() and Check() back to back.
*/
int sqlite3ExprResolveAndCheck(
  Parse *pParse,     /* The parser context */
  SrcList *pSrcList, /* List of tables used to resolve column names */
  ExprList *pEList,  /* List of expressions used to resolve "AS" */
  Expr *pExpr,       /* The expression to be analyzed. */
  int allowAgg,      /* True to allow aggregate expressions */
  int *pIsAgg        /* Set to TRUE if aggregates are found */
){
  if( pExpr==0 ) return 0;
  if( sqlite3ExprResolveIds(pParse,pSrcList,pEList,pExpr) ){
    return 1;
  }
  return sqlite3ExprCheck(pParse, pExpr, allowAgg, pIsAgg);
}

/*
** Generate an instruction that will put the integer describe by
** text z[0..n-1] on the stack.
*/
static void codeInteger(Vdbe *v, const char *z, int n){
  int i;
  if( sqlite3GetInt32(z, &i) ){
    sqlite3VdbeAddOp(v, OP_Integer, i, 0);
  }else if( sqlite3FitsIn64Bits(z) ){
    sqlite3VdbeOp3(v, OP_Integer, 0, 0, z, n);
  }else{
    sqlite3VdbeOp3(v, OP_Real, 0, 0, z, n);
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
  if( v==0 || pExpr==0 ) return;
  op = pExpr->op;
  switch( op ){
    case TK_COLUMN: {
      if( pParse->useAgg ){
        sqlite3VdbeAddOp(v, OP_AggGet, 0, pExpr->iAgg);
      }else if( pExpr->iColumn>=0 ){
        sqlite3VdbeAddOp(v, OP_Column, pExpr->iTable, pExpr->iColumn);
#ifndef NDEBUG
        if( pExpr->span.z && pExpr->span.n>0 && pExpr->span.n<100 ){
          VdbeComment((v, "# %T", &pExpr->span));
        }
#endif
      }else{
        sqlite3VdbeAddOp(v, OP_Recno, pExpr->iTable, 0);
      }
      break;
    }
    case TK_INTEGER: {
      codeInteger(v, pExpr->token.z, pExpr->token.n);
      break;
    }
    case TK_FLOAT:
    case TK_STRING: {
      assert( TK_FLOAT==OP_Real );
      assert( TK_STRING==OP_String8 );
      sqlite3VdbeOp3(v, op, 0, 0, pExpr->token.z, pExpr->token.n);
      sqlite3VdbeDequoteP3(v, -1);
      break;
    }
    case TK_BLOB: {
      assert( TK_BLOB==OP_HexBlob );
      sqlite3VdbeOp3(v, op, 0, 0, pExpr->token.z+1, pExpr->token.n-1);
      sqlite3VdbeDequoteP3(v, -1);
      break;
    }
    case TK_NULL: {
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      break;
    }
    case TK_VARIABLE: {
      sqlite3VdbeAddOp(v, OP_Variable, pExpr->iTable, 0);
      if( pExpr->token.n>1 ){
        sqlite3VdbeChangeP3(v, -1, pExpr->token.z, pExpr->token.n);
      }
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
      codeCompare(pParse, pExpr->pLeft, pExpr->pRight, op, 0, 0);
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
      break;
    }
    case TK_UMINUS: {
      Expr *pLeft = pExpr->pLeft;
      assert( pLeft );
      if( pLeft->op==TK_FLOAT || pLeft->op==TK_INTEGER ){
        Token *p = &pLeft->token;
        char *z = sqliteMalloc( p->n + 2 );
        sprintf(z, "-%.*s", p->n, p->z);
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
      break;
    }
    case TK_AGG_FUNCTION: {
      sqlite3VdbeAddOp(v, OP_AggGet, 0, pExpr->iAgg);
      break;
    }
    case TK_GLOB:
    case TK_LIKE:
    case TK_FUNCTION: {
      ExprList *pList = pExpr->pList;
      int nExpr = pList ? pList->nExpr : 0;
      FuncDef *pDef;
      int nId;
      const char *zId;
      int p2 = 0;
      int i;
      u8 enc = pParse->db->enc;
      CollSeq *pColl = 0;
      getFunctionName(pExpr, &zId, &nId);
      pDef = sqlite3FindFunction(pParse->db, zId, nId, nExpr, enc, 0);
      assert( pDef!=0 );
      nExpr = sqlite3ExprCodeExprList(pParse, pList);
      for(i=0; i<nExpr && i<32; i++){
        if( sqlite3ExprIsConstant(pList->a[i].pExpr) ){
          p2 |= (1<<i);
        }
        if( pDef->needCollSeq && !pColl ){
          pColl = sqlite3ExprCollSeq(pParse, pList->a[i].pExpr);
        }
      }
      if( pDef->needCollSeq ){
        if( !pColl ) pColl = pParse->db->pDfltColl; 
        sqlite3VdbeOp3(v, OP_CollSeq, 0, 0, (char *)pColl, P3_COLLSEQ);
      }
      sqlite3VdbeOp3(v, OP_Function, nExpr, p2, (char*)pDef, P3_FUNCDEF);
      break;
    }
    case TK_SELECT: {
      sqlite3VdbeAddOp(v, OP_MemLoad, pExpr->iColumn, 0);
      VdbeComment((v, "# load subquery result"));
      break;
    }
    case TK_IN: {
      int addr;
      char affinity;

      /* Figure out the affinity to use to create a key from the results
      ** of the expression. affinityStr stores a static string suitable for
      ** P3 of OP_MakeRecord.
      */
      affinity = comparisonAffinity(pExpr);

      sqlite3VdbeAddOp(v, OP_Integer, 1, 0);

      /* Code the <expr> from "<expr> IN (...)". The temporary table
      ** pExpr->iTable contains the values that make up the (...) set.
      */
      sqlite3ExprCode(pParse, pExpr->pLeft);
      addr = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_NotNull, -1, addr+4);            /* addr + 0 */
      sqlite3VdbeAddOp(v, OP_Pop, 2, 0);
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      sqlite3VdbeAddOp(v, OP_Goto, 0, addr+7);
      sqlite3VdbeOp3(v, OP_MakeRecord, 1, 0, &affinity, 1);   /* addr + 4 */
      sqlite3VdbeAddOp(v, OP_Found, pExpr->iTable, addr+7);
      sqlite3VdbeAddOp(v, OP_AddImm, -1, 0);                  /* addr + 6 */

      break;
    }
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
      break;
    }
    case TK_CASE: {
      int expr_end_label;
      int jumpInst;
      int addr;
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
        addr = sqlite3VdbeCurrentAddr(v);
        sqlite3VdbeChangeP2(v, jumpInst, addr);
      }
      if( pExpr->pLeft ){
        sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      }
      if( pExpr->pRight ){
        sqlite3ExprCode(pParse, pExpr->pRight);
      }else{
        sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      }
      sqlite3VdbeResolveLabel(v, expr_end_label);
      break;
    }
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
         sqlite3VdbeOp3(v, OP_Halt, SQLITE_CONSTRAINT, pExpr->iColumn,
                        pExpr->token.z, pExpr->token.n);
         sqlite3VdbeDequoteP3(v, -1);
      } else {
         assert( pExpr->iColumn == OE_Ignore );
         sqlite3VdbeAddOp(v, OP_ContextPop, 0, 0);
         sqlite3VdbeAddOp(v, OP_Goto, 0, pParse->trigStack->ignoreJump);
         VdbeComment((v, "# raise(IGNORE)"));
      }
    }
    break;
  }
}

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
  Vdbe *v;
  if( pList==0 ) return 0;
  v = sqlite3GetVdbe(pParse);
  n = pList->nExpr;
  for(pItem=pList->a, i=0; i<n; i++, pItem++){
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
      sqlite3VdbeChangeP2(v, addr, sqlite3VdbeCurrentAddr(v));
      sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
      break;
    }
    default: {
      sqlite3ExprCode(pParse, pExpr);
      sqlite3VdbeAddOp(v, OP_If, jumpIfNull, dest);
      break;
    }
  }
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
}

/*
** Do a deep comparison of two expression trees.  Return TRUE (non-zero)
** if they are identical and return FALSE if they differ in any way.
*/
int sqlite3ExprCompare(Expr *pA, Expr *pB){
  int i;
  if( pA==0 ){
    return pB==0;
  }else if( pB==0 ){
    return 0;
  }
  if( pA->op!=pB->op ) return 0;
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
  if( pA->token.z ){
    if( pB->token.z==0 ) return 0;
    if( pB->token.n!=pA->token.n ) return 0;
    if( sqlite3StrNICmp(pA->token.z, pB->token.z, pB->token.n)!=0 ) return 0;
  }
  return 1;
}

/*
** Add a new element to the pParse->aAgg[] array and return its index.
*/
static int appendAggInfo(Parse *pParse){
  if( (pParse->nAgg & 0x7)==0 ){
    int amt = pParse->nAgg + 8;
    AggExpr *aAgg = sqliteRealloc(pParse->aAgg, amt*sizeof(pParse->aAgg[0]));
    if( aAgg==0 ){
      return -1;
    }
    pParse->aAgg = aAgg;
  }
  memset(&pParse->aAgg[pParse->nAgg], 0, sizeof(pParse->aAgg[0]));
  return pParse->nAgg++;
}

/*
** Analyze the given expression looking for aggregate functions and
** for variables that need to be added to the pParse->aAgg[] array.
** Make additional entries to the pParse->aAgg[] array as necessary.
**
** This routine should only be called after the expression has been
** analyzed by sqlite3ExprResolveIds() and sqlite3ExprCheck().
**
** If errors are seen, leave an error message in zErrMsg and return
** the number of errors.
*/
int sqlite3ExprAnalyzeAggregates(Parse *pParse, Expr *pExpr){
  int i;
  AggExpr *aAgg;
  int nErr = 0;

  if( pExpr==0 ) return 0;
  switch( pExpr->op ){
    case TK_COLUMN: {
      aAgg = pParse->aAgg;
      for(i=0; i<pParse->nAgg; i++){
        if( aAgg[i].isAgg ) continue;
        if( aAgg[i].pExpr->iTable==pExpr->iTable
         && aAgg[i].pExpr->iColumn==pExpr->iColumn ){
          break;
        }
      }
      if( i>=pParse->nAgg ){
        i = appendAggInfo(pParse);
        if( i<0 ) return 1;
        pParse->aAgg[i].isAgg = 0;
        pParse->aAgg[i].pExpr = pExpr;
      }
      pExpr->iAgg = i;
      break;
    }
    case TK_AGG_FUNCTION: {
      aAgg = pParse->aAgg;
      for(i=0; i<pParse->nAgg; i++){
        if( !aAgg[i].isAgg ) continue;
        if( sqlite3ExprCompare(aAgg[i].pExpr, pExpr) ){
          break;
        }
      }
      if( i>=pParse->nAgg ){
        u8 enc = pParse->db->enc;
        i = appendAggInfo(pParse);
        if( i<0 ) return 1;
        pParse->aAgg[i].isAgg = 1;
        pParse->aAgg[i].pExpr = pExpr;
        pParse->aAgg[i].pFunc = sqlite3FindFunction(pParse->db,
             pExpr->token.z, pExpr->token.n,
             pExpr->pList ? pExpr->pList->nExpr : 0, enc, 0);
      }
      pExpr->iAgg = i;
      break;
    }
    default: {
      if( pExpr->pLeft ){
        nErr = sqlite3ExprAnalyzeAggregates(pParse, pExpr->pLeft);
      }
      if( nErr==0 && pExpr->pRight ){
        nErr = sqlite3ExprAnalyzeAggregates(pParse, pExpr->pRight);
      }
      if( nErr==0 && pExpr->pList ){
        int n = pExpr->pList->nExpr;
        int i;
        for(i=0; nErr==0 && i<n; i++){
          nErr = sqlite3ExprAnalyzeAggregates(pParse, pExpr->pList->a[i].pExpr);
        }
      }
      break;
    }
  }
  return nErr;
}

/*
** Locate a user function given a name, a number of arguments and a flag
** indicating whether the function prefers UTF-16 over UTF-8.  Return a
** pointer to the FuncDef structure that defines that function, or return
** NULL if the function does not exist.
**
** If the createFlag argument is true, then a new (blank) FuncDef
** structure is created and liked into the "db" structure if a
** no matching function previously existed.  When createFlag is true
** and the nArg parameter is -1, then only a function that accepts
** any number of arguments will be returned.
**
** If createFlag is false and nArg is -1, then the first valid
** function found is returned.  A function is valid if either xFunc
** or xStep is non-zero.
**
** If createFlag is false, then a function with the required name and
** number of arguments may be returned even if the eTextRep flag does not
** match that requested.
*/
FuncDef *sqlite3FindFunction(
  sqlite3 *db,       /* An open database */
  const char *zName, /* Name of the function.  Not null-terminated */
  int nName,         /* Number of characters in the name */
  int nArg,          /* Number of arguments.  -1 means any number */
  u8 enc,            /* Preferred text encoding */
  int createFlag     /* Create new entry if true and does not otherwise exist */
){
  FuncDef *p;         /* Iterator variable */
  FuncDef *pFirst;    /* First function with this name */
  FuncDef *pBest = 0; /* Best match found so far */
  int bestmatch = 0;  


  assert( enc==SQLITE_UTF8 || enc==SQLITE_UTF16LE || enc==SQLITE_UTF16BE );
  if( nArg<-1 ) nArg = -1;

  pFirst = (FuncDef*)sqlite3HashFind(&db->aFunc, zName, nName);
  for(p=pFirst; p; p=p->pNext){
    /* During the search for the best function definition, bestmatch is set
    ** as follows to indicate the quality of the match with the definition
    ** pointed to by pBest:
    **
    ** 0: pBest is NULL. No match has been found.
    ** 1: A variable arguments function that prefers UTF-8 when a UTF-16
    **    encoding is requested, or vice versa.
    ** 2: A variable arguments function that uses UTF-16BE when UTF-16LE is
    **    requested, or vice versa.
    ** 3: A variable arguments function using the same text encoding.
    ** 4: A function with the exact number of arguments requested that
    **    prefers UTF-8 when a UTF-16 encoding is requested, or vice versa.
    ** 5: A function with the exact number of arguments requested that
    **    prefers UTF-16LE when UTF-16BE is requested, or vice versa.
    ** 6: An exact match.
    **
    ** A larger value of 'matchqual' indicates a more desirable match.
    */
    if( p->nArg==-1 || p->nArg==nArg || nArg==-1 ){
      int match = 1;          /* Quality of this match */
      if( p->nArg==nArg || nArg==-1 ){
        match = 4;
      }
      if( enc==p->iPrefEnc ){
        match += 2;
      }
      else if( (enc==SQLITE_UTF16LE && p->iPrefEnc==SQLITE_UTF16BE) ||
               (enc==SQLITE_UTF16BE && p->iPrefEnc==SQLITE_UTF16LE) ){
        match += 1;
      }

      if( match>bestmatch ){
        pBest = p;
        bestmatch = match;
      }
    }
  }

  /* If the createFlag parameter is true, and the seach did not reveal an
  ** exact match for the name, number of arguments and encoding, then add a
  ** new entry to the hash table and return it.
  */
  if( createFlag && bestmatch<6 && 
      (pBest = sqliteMalloc(sizeof(*pBest)+nName+1)) ){
    pBest->nArg = nArg;
    pBest->pNext = pFirst;
    pBest->zName = (char*)&pBest[1];
    pBest->iPrefEnc = enc;
    memcpy(pBest->zName, zName, nName);
    pBest->zName[nName] = 0;
    sqlite3HashInsert(&db->aFunc, pBest->zName, nName, (void*)pBest);
  }

  if( pBest && (pBest->xStep || pBest->xFunc || createFlag) ){
    return pBest;
  }
  return 0;
}
