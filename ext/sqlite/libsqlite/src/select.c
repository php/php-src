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
** Allocate a new Select structure and return a pointer to that
** structure.
*/
Select *sqliteSelectNew(
  ExprList *pEList,     /* which columns to include in the result */
  SrcList *pSrc,        /* the FROM clause -- which tables to scan */
  Expr *pWhere,         /* the WHERE clause */
  ExprList *pGroupBy,   /* the GROUP BY clause */
  Expr *pHaving,        /* the HAVING clause */
  ExprList *pOrderBy,   /* the ORDER BY clause */
  int isDistinct,       /* true if the DISTINCT keyword is present */
  int nLimit,           /* LIMIT value.  -1 means not used */
  int nOffset           /* OFFSET value.  0 means no offset */
){
  Select *pNew;
  pNew = sqliteMalloc( sizeof(*pNew) );
  if( pNew==0 ){
    sqliteExprListDelete(pEList);
    sqliteSrcListDelete(pSrc);
    sqliteExprDelete(pWhere);
    sqliteExprListDelete(pGroupBy);
    sqliteExprDelete(pHaving);
    sqliteExprListDelete(pOrderBy);
  }else{
    if( pEList==0 ){
      pEList = sqliteExprListAppend(0, sqliteExpr(TK_ALL,0,0,0), 0);
    }
    pNew->pEList = pEList;
    pNew->pSrc = pSrc;
    pNew->pWhere = pWhere;
    pNew->pGroupBy = pGroupBy;
    pNew->pHaving = pHaving;
    pNew->pOrderBy = pOrderBy;
    pNew->isDistinct = isDistinct;
    pNew->op = TK_SELECT;
    pNew->nLimit = nLimit;
    pNew->nOffset = nOffset;
    pNew->iLimit = -1;
    pNew->iOffset = -1;
  }
  return pNew;
}

/*
** Given 1 to 3 identifiers preceeding the JOIN keyword, determine the
** type of join.  Return an integer constant that expresses that type
** in terms of the following bit values:
**
**     JT_INNER
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
int sqliteJoinType(Parse *pParse, Token *pA, Token *pB, Token *pC){
  int jointype = 0;
  Token *apAll[3];
  Token *p;
  static struct {
    const char *zKeyword;
    int nChar;
    int code;
  } keywords[] = {
    { "natural", 7, JT_NATURAL },
    { "left",    4, JT_LEFT|JT_OUTER },
    { "right",   5, JT_RIGHT|JT_OUTER },
    { "full",    4, JT_LEFT|JT_RIGHT|JT_OUTER },
    { "outer",   5, JT_OUTER },
    { "inner",   5, JT_INNER },
    { "cross",   5, JT_INNER },
  };
  int i, j;
  apAll[0] = pA;
  apAll[1] = pB;
  apAll[2] = pC;
  for(i=0; i<3 && apAll[i]; i++){
    p = apAll[i];
    for(j=0; j<sizeof(keywords)/sizeof(keywords[0]); j++){
      if( p->n==keywords[j].nChar 
          && sqliteStrNICmp(p->z, keywords[j].zKeyword, p->n)==0 ){
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
    static Token dummy = { 0, 0 };
    char *zSp1 = " ", *zSp2 = " ";
    if( pB==0 ){ pB = &dummy; zSp1 = 0; }
    if( pC==0 ){ pC = &dummy; zSp2 = 0; }
    sqliteSetNString(&pParse->zErrMsg, "unknown or unsupported join type: ", 0,
       pA->z, pA->n, zSp1, 1, pB->z, pB->n, zSp2, 1, pC->z, pC->n, 0);
    pParse->nErr++;
    jointype = JT_INNER;
  }else if( jointype & JT_RIGHT ){
    sqliteErrorMsg(pParse, 
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
    if( sqliteStrICmp(pTab->aCol[i].zName, zCol)==0 ) return i;
  }
  return -1;
}

/*
** Add a term to the WHERE expression in *ppExpr that requires the
** zCol column to be equal in the two tables pTab1 and pTab2.
*/
static void addWhereTerm(
  const char *zCol,        /* Name of the column */
  const Table *pTab1,      /* First table */
  const Table *pTab2,      /* Second table */
  Expr **ppExpr            /* Add the equality term to this expression */
){
  Token dummy;
  Expr *pE1a, *pE1b, *pE1c;
  Expr *pE2a, *pE2b, *pE2c;
  Expr *pE;

  dummy.z = zCol;
  dummy.n = strlen(zCol);
  dummy.dyn = 0;
  pE1a = sqliteExpr(TK_ID, 0, 0, &dummy);
  pE2a = sqliteExpr(TK_ID, 0, 0, &dummy);
  dummy.z = pTab1->zName;
  dummy.n = strlen(dummy.z);
  pE1b = sqliteExpr(TK_ID, 0, 0, &dummy);
  dummy.z = pTab2->zName;
  dummy.n = strlen(dummy.z);
  pE2b = sqliteExpr(TK_ID, 0, 0, &dummy);
  pE1c = sqliteExpr(TK_DOT, pE1b, pE1a, 0);
  pE2c = sqliteExpr(TK_DOT, pE2b, pE2a, 0);
  pE = sqliteExpr(TK_EQ, pE1c, pE2c, 0);
  ExprSetProperty(pE, EP_FromJoin);
  if( *ppExpr ){
    *ppExpr = sqliteExpr(TK_AND, *ppExpr, pE, 0);
  }else{
    *ppExpr = pE;
  }
}

/*
** Set the EP_FromJoin property on all terms of the given expression.
**
** The EP_FromJoin property is used on terms of an expression to tell
** the LEFT OUTER JOIN processing logic that this term is part of the
** join restriction specified in the ON or USING clause and not a part
** of the more general WHERE clause.  These terms are moved over to the
** WHERE clause during join processing but we need to remember that they
** originated in the ON or USING clause.
*/
static void setJoinExpr(Expr *p){
  while( p ){
    ExprSetProperty(p, EP_FromJoin);
    setJoinExpr(p->pLeft);
    p = p->pRight;
  } 
}

/*
** This routine processes the join information for a SELECT statement.
** ON and USING clauses are converted into extra terms of the WHERE clause.
** NATURAL joins also create extra WHERE clause terms.
**
** This routine returns the number of errors encountered.
*/
static int sqliteProcessJoin(Parse *pParse, Select *p){
  SrcList *pSrc;
  int i, j;
  pSrc = p->pSrc;
  for(i=0; i<pSrc->nSrc-1; i++){
    struct SrcList_item *pTerm = &pSrc->a[i];
    struct SrcList_item *pOther = &pSrc->a[i+1];

    if( pTerm->pTab==0 || pOther->pTab==0 ) continue;

    /* When the NATURAL keyword is present, add WHERE clause terms for
    ** every column that the two tables have in common.
    */
    if( pTerm->jointype & JT_NATURAL ){
      Table *pTab;
      if( pTerm->pOn || pTerm->pUsing ){
        sqliteErrorMsg(pParse, "a NATURAL join may not have "
           "an ON or USING clause", 0);
        return 1;
      }
      pTab = pTerm->pTab;
      for(j=0; j<pTab->nCol; j++){
        if( columnIndex(pOther->pTab, pTab->aCol[j].zName)>=0 ){
          addWhereTerm(pTab->aCol[j].zName, pTab, pOther->pTab, &p->pWhere);
        }
      }
    }

    /* Disallow both ON and USING clauses in the same join
    */
    if( pTerm->pOn && pTerm->pUsing ){
      sqliteErrorMsg(pParse, "cannot have both ON and USING "
        "clauses in the same join");
      return 1;
    }

    /* Add the ON clause to the end of the WHERE clause, connected by
    ** and AND operator.
    */
    if( pTerm->pOn ){
      setJoinExpr(pTerm->pOn);
      if( p->pWhere==0 ){
        p->pWhere = pTerm->pOn;
      }else{
        p->pWhere = sqliteExpr(TK_AND, p->pWhere, pTerm->pOn, 0);
      }
      pTerm->pOn = 0;
    }

    /* Create extra terms on the WHERE clause for each column named
    ** in the USING clause.  Example: If the two tables to be joined are 
    ** A and B and the USING clause names X, Y, and Z, then add this
    ** to the WHERE clause:    A.X=B.X AND A.Y=B.Y AND A.Z=B.Z
    ** Report an error if any column mentioned in the USING clause is
    ** not contained in both tables to be joined.
    */
    if( pTerm->pUsing ){
      IdList *pList;
      int j;
      assert( i<pSrc->nSrc-1 );
      pList = pTerm->pUsing;
      for(j=0; j<pList->nId; j++){
        if( columnIndex(pTerm->pTab, pList->a[j].zName)<0 ||
            columnIndex(pOther->pTab, pList->a[j].zName)<0 ){
          sqliteErrorMsg(pParse, "cannot join using column %s - column "
            "not present in both tables", pList->a[j].zName);
          return 1;
        }
        addWhereTerm(pList->a[j].zName, pTerm->pTab, pOther->pTab, &p->pWhere);
      }
    }
  }
  return 0;
}

/*
** Delete the given Select structure and all of its substructures.
*/
void sqliteSelectDelete(Select *p){
  if( p==0 ) return;
  sqliteExprListDelete(p->pEList);
  sqliteSrcListDelete(p->pSrc);
  sqliteExprDelete(p->pWhere);
  sqliteExprListDelete(p->pGroupBy);
  sqliteExprDelete(p->pHaving);
  sqliteExprListDelete(p->pOrderBy);
  sqliteSelectDelete(p->pPrior);
  sqliteFree(p->zSelect);
  sqliteFree(p);
}

/*
** Delete the aggregate information from the parse structure.
*/
static void sqliteAggregateInfoReset(Parse *pParse){
  sqliteFree(pParse->aAgg);
  pParse->aAgg = 0;
  pParse->nAgg = 0;
  pParse->useAgg = 0;
}

/*
** Insert code into "v" that will push the record on the top of the
** stack into the sorter.
*/
static void pushOntoSorter(Parse *pParse, Vdbe *v, ExprList *pOrderBy){
  char *zSortOrder;
  int i;
  zSortOrder = sqliteMalloc( pOrderBy->nExpr + 1 );
  if( zSortOrder==0 ) return;
  for(i=0; i<pOrderBy->nExpr; i++){
    int order = pOrderBy->a[i].sortOrder;
    int type;
    int c;
    if( (order & SQLITE_SO_TYPEMASK)==SQLITE_SO_TEXT ){
      type = SQLITE_SO_TEXT;
    }else if( (order & SQLITE_SO_TYPEMASK)==SQLITE_SO_NUM ){
      type = SQLITE_SO_NUM;
    }else if( pParse->db->file_format>=4 ){
      type = sqliteExprType(pOrderBy->a[i].pExpr);
    }else{
      type = SQLITE_SO_NUM;
    }
    if( (order & SQLITE_SO_DIRMASK)==SQLITE_SO_ASC ){
      c = type==SQLITE_SO_TEXT ? 'A' : '+';
    }else{
      c = type==SQLITE_SO_TEXT ? 'D' : '-';
    }
    zSortOrder[i] = c;
    sqliteExprCode(pParse, pOrderBy->a[i].pExpr);
  }
  zSortOrder[pOrderBy->nExpr] = 0;
  sqliteVdbeOp3(v, OP_SortMakeKey, pOrderBy->nExpr, 0, zSortOrder, P3_DYNAMIC);
  sqliteVdbeAddOp(v, OP_SortPut, 0, 0);
}

/*
** This routine adds a P3 argument to the last VDBE opcode that was
** inserted. The P3 argument added is a string suitable for the 
** OP_MakeKey or OP_MakeIdxKey opcodes.  The string consists of
** characters 't' or 'n' depending on whether or not the various
** fields of the key to be generated should be treated as numeric
** or as text.  See the OP_MakeKey and OP_MakeIdxKey opcode
** documentation for additional information about the P3 string.
** See also the sqliteAddIdxKeyType() routine.
*/
void sqliteAddKeyType(Vdbe *v, ExprList *pEList){
  int nColumn = pEList->nExpr;
  char *zType = sqliteMalloc( nColumn+1 );
  int i;
  if( zType==0 ) return;
  for(i=0; i<nColumn; i++){
    zType[i] = sqliteExprType(pEList->a[i].pExpr)==SQLITE_SO_NUM ? 'n' : 't';
  }
  zType[i] = 0;
  sqliteVdbeChangeP3(v, -1, zType, P3_DYNAMIC);
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
  int iBreak              /* Jump here to break out of the inner loop */
){
  Vdbe *v = pParse->pVdbe;
  int i;

  if( v==0 ) return 0;
  assert( pEList!=0 );

  /* If there was a LIMIT clause on the SELECT statement, then do the check
  ** to see if this row should be output.
  */
  if( pOrderBy==0 ){
    if( p->iOffset>=0 ){
      int addr = sqliteVdbeCurrentAddr(v);
      sqliteVdbeAddOp(v, OP_MemIncr, p->iOffset, addr+2);
      sqliteVdbeAddOp(v, OP_Goto, 0, iContinue);
    }
    if( p->iLimit>=0 ){
      sqliteVdbeAddOp(v, OP_MemIncr, p->iLimit, iBreak);
    }
  }

  /* Pull the requested columns.
  */
  if( nColumn>0 ){
    for(i=0; i<nColumn; i++){
      sqliteVdbeAddOp(v, OP_Column, srcTab, i);
    }
  }else{
    nColumn = pEList->nExpr;
    for(i=0; i<pEList->nExpr; i++){
      sqliteExprCode(pParse, pEList->a[i].pExpr);
    }
  }

  /* If the DISTINCT keyword was present on the SELECT statement
  ** and this row has been seen before, then do not make this row
  ** part of the result.
  */
  if( distinct>=0 && pEList && pEList->nExpr>0 ){
#if NULL_ALWAYS_DISTINCT
    sqliteVdbeAddOp(v, OP_IsNull, -pEList->nExpr, sqliteVdbeCurrentAddr(v)+7);
#endif
    sqliteVdbeAddOp(v, OP_MakeKey, pEList->nExpr, 1);
    if( pParse->db->file_format>=4 ) sqliteAddKeyType(v, pEList);
    sqliteVdbeAddOp(v, OP_Distinct, distinct, sqliteVdbeCurrentAddr(v)+3);
    sqliteVdbeAddOp(v, OP_Pop, pEList->nExpr+1, 0);
    sqliteVdbeAddOp(v, OP_Goto, 0, iContinue);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeAddOp(v, OP_PutStrKey, distinct, 0);
  }

  switch( eDest ){
    /* In this mode, write each query result to the key of the temporary
    ** table iParm.
    */
    case SRT_Union: {
      sqliteVdbeAddOp(v, OP_MakeRecord, nColumn, NULL_ALWAYS_DISTINCT);
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      sqliteVdbeAddOp(v, OP_PutStrKey, iParm, 0);
      break;
    }

    /* Store the result as data using a unique key.
    */
    case SRT_Table:
    case SRT_TempTable: {
      sqliteVdbeAddOp(v, OP_MakeRecord, nColumn, 0);
      if( pOrderBy ){
        pushOntoSorter(pParse, v, pOrderBy);
      }else{
        sqliteVdbeAddOp(v, OP_NewRecno, iParm, 0);
        sqliteVdbeAddOp(v, OP_Pull, 1, 0);
        sqliteVdbeAddOp(v, OP_PutIntKey, iParm, 0);
      }
      break;
    }

    /* Construct a record from the query result, but instead of
    ** saving that record, use it as a key to delete elements from
    ** the temporary table iParm.
    */
    case SRT_Except: {
      int addr;
      addr = sqliteVdbeAddOp(v, OP_MakeRecord, nColumn, NULL_ALWAYS_DISTINCT);
      sqliteVdbeAddOp(v, OP_NotFound, iParm, addr+3);
      sqliteVdbeAddOp(v, OP_Delete, iParm, 0);
      break;
    }

    /* If we are creating a set for an "expr IN (SELECT ...)" construct,
    ** then there should be a single item on the stack.  Write this
    ** item into the set table with bogus data.
    */
    case SRT_Set: {
      int addr1 = sqliteVdbeCurrentAddr(v);
      int addr2;
      assert( nColumn==1 );
      sqliteVdbeAddOp(v, OP_NotNull, -1, addr1+3);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      addr2 = sqliteVdbeAddOp(v, OP_Goto, 0, 0);
      if( pOrderBy ){
        pushOntoSorter(pParse, v, pOrderBy);
      }else{
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        sqliteVdbeAddOp(v, OP_PutStrKey, iParm, 0);
      }
      sqliteVdbeChangeP2(v, addr2, sqliteVdbeCurrentAddr(v));
      break;
    }

    /* If this is a scalar select that is part of an expression, then
    ** store the results in the appropriate memory cell and break out
    ** of the scan loop.
    */
    case SRT_Mem: {
      assert( nColumn==1 );
      if( pOrderBy ){
        pushOntoSorter(pParse, v, pOrderBy);
      }else{
        sqliteVdbeAddOp(v, OP_MemStore, iParm, 1);
        sqliteVdbeAddOp(v, OP_Goto, 0, iBreak);
      }
      break;
    }

    /* Send the data to the callback function.
    */
    case SRT_Callback:
    case SRT_Sorter: {
      if( pOrderBy ){
        sqliteVdbeAddOp(v, OP_SortMakeRec, nColumn, 0);
        pushOntoSorter(pParse, v, pOrderBy);
      }else{
        assert( eDest==SRT_Callback );
        sqliteVdbeAddOp(v, OP_Callback, nColumn, 0);
      }
      break;
    }

    /* Invoke a subroutine to handle the results.  The subroutine itself
    ** is responsible for popping the results off of the stack.
    */
    case SRT_Subroutine: {
      if( pOrderBy ){
        sqliteVdbeAddOp(v, OP_MakeRecord, nColumn, 0);
        pushOntoSorter(pParse, v, pOrderBy);
      }else{
        sqliteVdbeAddOp(v, OP_Gosub, 0, iParm);
      }
      break;
    }

    /* Discard the results.  This is used for SELECT statements inside
    ** the body of a TRIGGER.  The purpose of such selects is to call
    ** user-defined functions that have side effects.  We do not care
    ** about the actual results of the select.
    */
    default: {
      assert( eDest==SRT_Discard );
      sqliteVdbeAddOp(v, OP_Pop, nColumn, 0);
      break;
    }
  }
  return 0;
}

/*
** If the inner loop was generated using a non-null pOrderBy argument,
** then the results were placed in a sorter.  After the loop is terminated
** we need to run the sorter and output the results.  The following
** routine generates the code needed to do that.
*/
static void generateSortTail(
  Select *p,       /* The SELECT statement */
  Vdbe *v,         /* Generate code into this VDBE */
  int nColumn,     /* Number of columns of data */
  int eDest,       /* Write the sorted results here */
  int iParm        /* Optional parameter associated with eDest */
){
  int end1 = sqliteVdbeMakeLabel(v);
  int end2 = sqliteVdbeMakeLabel(v);
  int addr;
  if( eDest==SRT_Sorter ) return;
  sqliteVdbeAddOp(v, OP_Sort, 0, 0);
  addr = sqliteVdbeAddOp(v, OP_SortNext, 0, end1);
  if( p->iOffset>=0 ){
    sqliteVdbeAddOp(v, OP_MemIncr, p->iOffset, addr+4);
    sqliteVdbeAddOp(v, OP_Pop, 1, 0);
    sqliteVdbeAddOp(v, OP_Goto, 0, addr);
  }
  if( p->iLimit>=0 ){
    sqliteVdbeAddOp(v, OP_MemIncr, p->iLimit, end2);
  }
  switch( eDest ){
    case SRT_Callback: {
      sqliteVdbeAddOp(v, OP_SortCallback, nColumn, 0);
      break;
    }
    case SRT_Table:
    case SRT_TempTable: {
      sqliteVdbeAddOp(v, OP_NewRecno, iParm, 0);
      sqliteVdbeAddOp(v, OP_Pull, 1, 0);
      sqliteVdbeAddOp(v, OP_PutIntKey, iParm, 0);
      break;
    }
    case SRT_Set: {
      assert( nColumn==1 );
      sqliteVdbeAddOp(v, OP_NotNull, -1, sqliteVdbeCurrentAddr(v)+3);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      sqliteVdbeAddOp(v, OP_Goto, 0, sqliteVdbeCurrentAddr(v)+3);
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      sqliteVdbeAddOp(v, OP_PutStrKey, iParm, 0);
      break;
    }
    case SRT_Mem: {
      assert( nColumn==1 );
      sqliteVdbeAddOp(v, OP_MemStore, iParm, 1);
      sqliteVdbeAddOp(v, OP_Goto, 0, end1);
      break;
    }
    case SRT_Subroutine: {
      int i;
      for(i=0; i<nColumn; i++){
        sqliteVdbeAddOp(v, OP_Column, -1-i, i);
      }
      sqliteVdbeAddOp(v, OP_Gosub, 0, iParm);
      sqliteVdbeAddOp(v, OP_Pop, 1, 0);
      break;
    }
    default: {
      /* Do nothing */
      break;
    }
  }
  sqliteVdbeAddOp(v, OP_Goto, 0, addr);
  sqliteVdbeResolveLabel(v, end2);
  sqliteVdbeAddOp(v, OP_Pop, 1, 0);
  sqliteVdbeResolveLabel(v, end1);
  sqliteVdbeAddOp(v, OP_SortReset, 0, 0);
}

/*
** Generate code that will tell the VDBE the datatypes of
** columns in the result set.
**
** This routine only generates code if the "PRAGMA show_datatypes=on"
** has been executed.  The datatypes are reported out in the azCol
** parameter to the callback function.  The first N azCol[] entries
** are the names of the columns, and the second N entries are the
** datatypes for the columns.
**
** The "datatype" for a result that is a column of a type is the
** datatype definition extracted from the CREATE TABLE statement.
** The datatype for an expression is either TEXT or NUMERIC.  The
** datatype for a ROWID field is INTEGER.
*/
static void generateColumnTypes(
  Parse *pParse,      /* Parser context */
  SrcList *pTabList,  /* List of tables */
  ExprList *pEList    /* Expressions defining the result set */
){
  Vdbe *v = pParse->pVdbe;
  int i, j;
  for(i=0; i<pEList->nExpr; i++){
    Expr *p = pEList->a[i].pExpr;
    char *zType = 0;
    if( p==0 ) continue;
    if( p->op==TK_COLUMN && pTabList ){
      Table *pTab;
      int iCol = p->iColumn;
      for(j=0; j<pTabList->nSrc && pTabList->a[j].iCursor!=p->iTable; j++){}
      assert( j<pTabList->nSrc );
      pTab = pTabList->a[j].pTab;
      if( iCol<0 ) iCol = pTab->iPKey;
      assert( iCol==-1 || (iCol>=0 && iCol<pTab->nCol) );
      if( iCol<0 ){
        zType = "INTEGER";
      }else{
        zType = pTab->aCol[iCol].zType;
      }
    }else{
      if( sqliteExprType(p)==SQLITE_SO_TEXT ){
        zType = "TEXT";
      }else{
        zType = "NUMERIC";
      }
    }
    sqliteVdbeOp3(v, OP_ColumnName, i + pEList->nExpr, 0, zType, 0);
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
  sqlite *db = pParse->db;
  int fullNames, shortNames;

  assert( v!=0 );
  if( pParse->colNamesSet || v==0 || sqlite_malloc_failed ) return;
  pParse->colNamesSet = 1;
  fullNames = (db->flags & SQLITE_FullColNames)!=0;
  shortNames = (db->flags & SQLITE_ShortColNames)!=0;
  for(i=0; i<pEList->nExpr; i++){
    Expr *p;
    int p2 = i==pEList->nExpr-1;
    p = pEList->a[i].pExpr;
    if( p==0 ) continue;
    if( pEList->a[i].zName ){
      char *zName = pEList->a[i].zName;
      sqliteVdbeOp3(v, OP_ColumnName, i, p2, zName, 0);
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
        zCol = "_ROWID_";
      }else{
        zCol = pTab->aCol[iCol].zName;
      }
      if( !shortNames && !fullNames && p->span.z && p->span.z[0] ){
        int addr = sqliteVdbeOp3(v,OP_ColumnName, i, p2, p->span.z, p->span.n);
        sqliteVdbeCompressSpace(v, addr);
      }else if( fullNames || (!shortNames && pTabList->nSrc>1) ){
        char *zName = 0;
        char *zTab;
 
        zTab = pTabList->a[j].zAlias;
        if( fullNames || zTab==0 ) zTab = pTab->zName;
        sqliteSetString(&zName, zTab, ".", zCol, 0);
        sqliteVdbeOp3(v, OP_ColumnName, i, p2, zName, P3_DYNAMIC);
      }else{
        sqliteVdbeOp3(v, OP_ColumnName, i, p2, zCol, 0);
      }
    }else if( p->span.z && p->span.z[0] ){
      int addr = sqliteVdbeOp3(v,OP_ColumnName, i, p2, p->span.z, p->span.n);
      sqliteVdbeCompressSpace(v, addr);
    }else{
      char zName[30];
      assert( p->op!=TK_COLUMN || pTabList==0 );
      sprintf(zName, "column%d", i+1);
      sqliteVdbeOp3(v, OP_ColumnName, i, p2, zName, 0);
    }
  }
}

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

/*
** Forward declaration
*/
static int fillInColumnList(Parse*, Select*);

/*
** Given a SELECT statement, generate a Table structure that describes
** the result set of that SELECT.
*/
Table *sqliteResultSetOfSelect(Parse *pParse, char *zTabName, Select *pSelect){
  Table *pTab;
  int i, j;
  ExprList *pEList;
  Column *aCol;

  if( fillInColumnList(pParse, pSelect) ){
    return 0;
  }
  pTab = sqliteMalloc( sizeof(Table) );
  if( pTab==0 ){
    return 0;
  }
  pTab->zName = zTabName ? sqliteStrDup(zTabName) : 0;
  pEList = pSelect->pEList;
  pTab->nCol = pEList->nExpr;
  assert( pTab->nCol>0 );
  pTab->aCol = aCol = sqliteMalloc( sizeof(pTab->aCol[0])*pTab->nCol );
  for(i=0; i<pTab->nCol; i++){
    Expr *p, *pR;
    if( pEList->a[i].zName ){
      aCol[i].zName = sqliteStrDup(pEList->a[i].zName);
    }else if( (p=pEList->a[i].pExpr)->op==TK_DOT 
               && (pR=p->pRight)!=0 && pR->token.z && pR->token.z[0] ){
      int cnt;
      sqliteSetNString(&aCol[i].zName, pR->token.z, pR->token.n, 0);
      for(j=cnt=0; j<i; j++){
        if( sqliteStrICmp(aCol[j].zName, aCol[i].zName)==0 ){
          int n;
          char zBuf[30];
          sprintf(zBuf,"_%d",++cnt);
          n = strlen(zBuf);
          sqliteSetNString(&aCol[i].zName, pR->token.z, pR->token.n, zBuf, n,0);
          j = -1;
        }
      }
    }else if( p->span.z && p->span.z[0] ){
      sqliteSetNString(&pTab->aCol[i].zName, p->span.z, p->span.n, 0);
    }else{
      char zBuf[30];
      sprintf(zBuf, "column%d", i+1);
      pTab->aCol[i].zName = sqliteStrDup(zBuf);
    }
  }
  pTab->iPKey = -1;
  return pTab;
}

/*
** For the given SELECT statement, do three things.
**
**    (1)  Fill in the pTabList->a[].pTab fields in the SrcList that 
**         defines the set of tables that should be scanned.  For views,
**         fill pTabList->a[].pSelect with a copy of the SELECT statement
**         that implements the view.  A copy is made of the view's SELECT
**         statement so that we can freely modify or delete that statement
**         without worrying about messing up the presistent representation
**         of the view.
**
**    (2)  Add terms to the WHERE clause to accomodate the NATURAL keyword
**         on joins and the ON and USING clause of joins.
**
**    (3)  Scan the list of columns in the result set (pEList) looking
**         for instances of the "*" operator or the TABLE.* operator.
**         If found, expand each "*" to be every column in every table
**         and TABLE.* to be every column in TABLE.
**
** Return 0 on success.  If there are problems, leave an error message
** in pParse and return non-zero.
*/
static int fillInColumnList(Parse *pParse, Select *p){
  int i, j, k, rc;
  SrcList *pTabList;
  ExprList *pEList;
  Table *pTab;

  if( p==0 || p->pSrc==0 ) return 1;
  pTabList = p->pSrc;
  pEList = p->pEList;

  /* Look up every table in the table list.
  */
  for(i=0; i<pTabList->nSrc; i++){
    if( pTabList->a[i].pTab ){
      /* This routine has run before!  No need to continue */
      return 0;
    }
    if( pTabList->a[i].zName==0 ){
      /* A sub-query in the FROM clause of a SELECT */
      assert( pTabList->a[i].pSelect!=0 );
      if( pTabList->a[i].zAlias==0 ){
        char zFakeName[60];
        sprintf(zFakeName, "sqlite_subquery_%p_",
           (void*)pTabList->a[i].pSelect);
        sqliteSetString(&pTabList->a[i].zAlias, zFakeName, 0);
      }
      pTabList->a[i].pTab = pTab = 
        sqliteResultSetOfSelect(pParse, pTabList->a[i].zAlias,
                                        pTabList->a[i].pSelect);
      if( pTab==0 ){
        return 1;
      }
      /* The isTransient flag indicates that the Table structure has been
      ** dynamically allocated and may be freed at any time.  In other words,
      ** pTab is not pointing to a persistent table structure that defines
      ** part of the schema. */
      pTab->isTransient = 1;
    }else{
      /* An ordinary table or view name in the FROM clause */
      pTabList->a[i].pTab = pTab = 
        sqliteLocateTable(pParse,pTabList->a[i].zName,pTabList->a[i].zDatabase);
      if( pTab==0 ){
        return 1;
      }
      if( pTab->pSelect ){
        /* We reach here if the named table is a really a view */
        if( sqliteViewGetColumnNames(pParse, pTab) ){
          return 1;
        }
        /* If pTabList->a[i].pSelect!=0 it means we are dealing with a
        ** view within a view.  The SELECT structure has already been
        ** copied by the outer view so we can skip the copy step here
        ** in the inner view.
        */
        if( pTabList->a[i].pSelect==0 ){
          pTabList->a[i].pSelect = sqliteSelectDup(pTab->pSelect);
        }
      }
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
    for(k=0; k<pEList->nExpr; k++){
      Expr *pE = a[k].pExpr;
      if( pE->op!=TK_ALL &&
           (pE->op!=TK_DOT || pE->pRight==0 || pE->pRight->op!=TK_ALL) ){
        /* This particular expression does not need to be expanded.
        */
        pNew = sqliteExprListAppend(pNew, a[k].pExpr, 0);
        pNew->a[pNew->nExpr-1].zName = a[k].zName;
        a[k].pExpr = 0;
        a[k].zName = 0;
      }else{
        /* This expression is a "*" or a "TABLE.*" and needs to be
        ** expanded. */
        int tableSeen = 0;      /* Set to 1 when TABLE matches */
        Token *pName;           /* text of name of TABLE */
        if( pE->op==TK_DOT && pE->pLeft ){
          pName = &pE->pLeft->token;
        }else{
          pName = 0;
        }
        for(i=0; i<pTabList->nSrc; i++){
          Table *pTab = pTabList->a[i].pTab;
          char *zTabName = pTabList->a[i].zAlias;
          if( zTabName==0 || zTabName[0]==0 ){ 
            zTabName = pTab->zName;
          }
          if( pName && (zTabName==0 || zTabName[0]==0 || 
                 sqliteStrNICmp(pName->z, zTabName, pName->n)!=0 ||
                 zTabName[pName->n]!=0) ){
            continue;
          }
          tableSeen = 1;
          for(j=0; j<pTab->nCol; j++){
            Expr *pExpr, *pLeft, *pRight;
            char *zName = pTab->aCol[j].zName;

            if( i>0 && (pTabList->a[i-1].jointype & JT_NATURAL)!=0 &&
                columnIndex(pTabList->a[i-1].pTab, zName)>=0 ){
              /* In a NATURAL join, omit the join columns from the 
              ** table on the right */
              continue;
            }
            if( i>0 && sqliteIdListIndex(pTabList->a[i-1].pUsing, zName)>=0 ){
              /* In a join with a USING clause, omit columns in the
              ** using clause from the table on the right. */
              continue;
            }
            pRight = sqliteExpr(TK_ID, 0, 0, 0);
            if( pRight==0 ) break;
            pRight->token.z = zName;
            pRight->token.n = strlen(zName);
            pRight->token.dyn = 0;
            if( zTabName && pTabList->nSrc>1 ){
              pLeft = sqliteExpr(TK_ID, 0, 0, 0);
              pExpr = sqliteExpr(TK_DOT, pLeft, pRight, 0);
              if( pExpr==0 ) break;
              pLeft->token.z = zTabName;
              pLeft->token.n = strlen(zTabName);
              pLeft->token.dyn = 0;
              sqliteSetString((char**)&pExpr->span.z, zTabName, ".", zName, 0);
              pExpr->span.n = strlen(pExpr->span.z);
              pExpr->span.dyn = 1;
              pExpr->token.z = 0;
              pExpr->token.n = 0;
              pExpr->token.dyn = 0;
            }else{
              pExpr = pRight;
              pExpr->span = pExpr->token;
            }
            pNew = sqliteExprListAppend(pNew, pExpr, 0);
          }
        }
        if( !tableSeen ){
          if( pName ){
            sqliteErrorMsg(pParse, "no such table: %T", pName);
          }else{
            sqliteErrorMsg(pParse, "no tables specified");
          }
          rc = 1;
        }
      }
    }
    sqliteExprListDelete(pEList);
    p->pEList = pNew;
  }
  return rc;
}

/*
** This routine recursively unlinks the Select.pSrc.a[].pTab pointers
** in a select structure.  It just sets the pointers to NULL.  This
** routine is recursive in the sense that if the Select.pSrc.a[].pSelect
** pointer is not NULL, this routine is called recursively on that pointer.
**
** This routine is called on the Select structure that defines a
** VIEW in order to undo any bindings to tables.  This is necessary
** because those tables might be DROPed by a subsequent SQL command.
** If the bindings are not removed, then the Select.pSrc->a[].pTab field
** will be left pointing to a deallocated Table structure after the
** DROP and a coredump will occur the next time the VIEW is used.
*/
void sqliteSelectUnbind(Select *p){
  int i;
  SrcList *pSrc = p->pSrc;
  Table *pTab;
  if( p==0 ) return;
  for(i=0; i<pSrc->nSrc; i++){
    if( (pTab = pSrc->a[i].pTab)!=0 ){
      if( pTab->isTransient ){
        sqliteDeleteTable(0, pTab);
      }
      pSrc->a[i].pTab = 0;
      if( pSrc->a[i].pSelect ){
        sqliteSelectUnbind(pSrc->a[i].pSelect);
      }
    }
  }
}

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
**
** This routine does NOT correctly initialize the Expr.dataType  field
** of the ORDER BY expressions.  The multiSelectSortOrder() routine
** must be called to do that after the individual select statements
** have all been analyzed.  This routine is unable to compute Expr.dataType
** because it must be called before the individual select statements
** have been analyzed.
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
  if( fillInColumnList(pParse, pSelect) ){
    return 1;
  }
  if( pSelect->pPrior ){
    if( matchOrderbyToColumn(pParse, pSelect->pPrior, pOrderBy, iTable, 0) ){
      return 1;
    }
  }
  pEList = pSelect->pEList;
  for(i=0; i<pOrderBy->nExpr; i++){
    Expr *pE = pOrderBy->a[i].pExpr;
    int iCol = -1;
    if( pOrderBy->a[i].done ) continue;
    if( sqliteExprIsInteger(pE, &iCol) ){
      if( iCol<=0 || iCol>pEList->nExpr ){
        sqliteErrorMsg(pParse,
          "ORDER BY position %d should be between 1 and %d",
          iCol, pEList->nExpr);
        nErr++;
        break;
      }
      if( !mustComplete ) continue;
      iCol--;
    }
    for(j=0; iCol<0 && j<pEList->nExpr; j++){
      if( pEList->a[j].zName && (pE->op==TK_ID || pE->op==TK_STRING) ){
        char *zName, *zLabel;
        zName = pEList->a[j].zName;
        assert( pE->token.z );
        zLabel = sqliteStrNDup(pE->token.z, pE->token.n);
        sqliteDequote(zLabel);
        if( sqliteStrICmp(zName, zLabel)==0 ){ 
          iCol = j;
        }
        sqliteFree(zLabel);
      }
      if( iCol<0 && sqliteExprCompare(pE, pEList->a[j].pExpr) ){
        iCol = j;
      }
    }
    if( iCol>=0 ){
      pE->op = TK_COLUMN;
      pE->iColumn = iCol;
      pE->iTable = iTable;
      pOrderBy->a[i].done = 1;
    }
    if( iCol<0 && mustComplete ){
      sqliteErrorMsg(pParse,
        "ORDER BY term number %d does not match any result column", i+1);
      nErr++;
      break;
    }
  }
  return nErr;  
}

/*
** Get a VDBE for the given parser context.  Create a new one if necessary.
** If an error occurs, return NULL and leave a message in pParse.
*/
Vdbe *sqliteGetVdbe(Parse *pParse){
  Vdbe *v = pParse->pVdbe;
  if( v==0 ){
    v = pParse->pVdbe = sqliteVdbeCreate(pParse->db);
  }
  return v;
}

/*
** This routine sets the Expr.dataType field on all elements of
** the pOrderBy expression list.  The pOrderBy list will have been
** set up by matchOrderbyToColumn().  Hence each expression has
** a TK_COLUMN as its root node.  The Expr.iColumn refers to a 
** column in the result set.   The datatype is set to SQLITE_SO_TEXT
** if the corresponding column in p and every SELECT to the left of
** p has a datatype of SQLITE_SO_TEXT.  If the cooressponding column
** in p or any of the left SELECTs is SQLITE_SO_NUM, then the datatype
** of the order-by expression is set to SQLITE_SO_NUM.
**
** Examples:
**
**     CREATE TABLE one(a INTEGER, b TEXT);
**     CREATE TABLE two(c VARCHAR(5), d FLOAT);
**
**     SELECT b, b FROM one UNION SELECT d, c FROM two ORDER BY 1, 2;
**
** The primary sort key will use SQLITE_SO_NUM because the "d" in
** the second SELECT is numeric.  The 1st column of the first SELECT
** is text but that does not matter because a numeric always overrides
** a text.
**
** The secondary key will use the SQLITE_SO_TEXT sort order because
** both the (second) "b" in the first SELECT and the "c" in the second
** SELECT have a datatype of text.
*/ 
static void multiSelectSortOrder(Select *p, ExprList *pOrderBy){
  int i;
  ExprList *pEList;
  if( pOrderBy==0 ) return;
  if( p==0 ){
    for(i=0; i<pOrderBy->nExpr; i++){
      pOrderBy->a[i].pExpr->dataType = SQLITE_SO_TEXT;
    }
    return;
  }
  multiSelectSortOrder(p->pPrior, pOrderBy);
  pEList = p->pEList;
  for(i=0; i<pOrderBy->nExpr; i++){
    Expr *pE = pOrderBy->a[i].pExpr;
    if( pE->dataType==SQLITE_SO_NUM ) continue;
    assert( pE->iColumn>=0 );
    if( pEList->nExpr>pE->iColumn ){
      pE->dataType = sqliteExprType(pEList->a[pE->iColumn].pExpr);
    }
  }
}

/*
** Compute the iLimit and iOffset fields of the SELECT based on the
** nLimit and nOffset fields.  nLimit and nOffset hold the integers
** that appear in the original SQL statement after the LIMIT and OFFSET
** keywords.  Or that hold -1 and 0 if those keywords are omitted.
** iLimit and iOffset are the integer memory register numbers for
** counters used to compute the limit and offset.  If there is no
** limit and/or offset, then iLimit and iOffset are negative.
**
** This routine changes the values if iLimit and iOffset only if
** a limit or offset is defined by nLimit and nOffset.  iLimit and
** iOffset should have been preset to appropriate default values
** (usually but not always -1) prior to calling this routine.
** Only if nLimit>=0 or nOffset>0 do the limit registers get
** redefined.  The UNION ALL operator uses this property to force
** the reuse of the same limit and offset registers across multiple
** SELECT statements.
*/
static void computeLimitRegisters(Parse *pParse, Select *p){
  /* 
  ** If the comparison is p->nLimit>0 then "LIMIT 0" shows
  ** all rows.  It is the same as no limit. If the comparision is
  ** p->nLimit>=0 then "LIMIT 0" show no rows at all.
  ** "LIMIT -1" always shows all rows.  There is some
  ** contraversy about what the correct behavior should be.
  ** The current implementation interprets "LIMIT 0" to mean
  ** no rows.
  */
  if( p->nLimit>=0 ){
    int iMem = pParse->nMem++;
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    sqliteVdbeAddOp(v, OP_Integer, -p->nLimit, 0);
    sqliteVdbeAddOp(v, OP_MemStore, iMem, 1);
    p->iLimit = iMem;
  }
  if( p->nOffset>0 ){
    int iMem = pParse->nMem++;
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    sqliteVdbeAddOp(v, OP_Integer, -p->nOffset, 0);
    sqliteVdbeAddOp(v, OP_MemStore, iMem, 1);
    p->iOffset = iMem;
  }
}

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
static int multiSelect(Parse *pParse, Select *p, int eDest, int iParm){
  int rc;             /* Success code from a subroutine */
  Select *pPrior;     /* Another SELECT immediately to our left */
  Vdbe *v;            /* Generate code to this VDBE */

  /* Make sure there is no ORDER BY or LIMIT clause on prior SELECTs.  Only
  ** the last SELECT in the series may have an ORDER BY or LIMIT.
  */
  if( p==0 || p->pPrior==0 ) return 1;
  pPrior = p->pPrior;
  if( pPrior->pOrderBy ){
    sqliteErrorMsg(pParse,"ORDER BY clause should come after %s not before",
      selectOpName(p->op));
    return 1;
  }
  if( pPrior->nLimit>=0 || pPrior->nOffset>0 ){
    sqliteErrorMsg(pParse,"LIMIT clause should come after %s not before",
      selectOpName(p->op));
    return 1;
  }

  /* Make sure we have a valid query engine.  If not, create a new one.
  */
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return 1;

  /* Create the destination temporary table if necessary
  */
  if( eDest==SRT_TempTable ){
    sqliteVdbeAddOp(v, OP_OpenTemp, iParm, 0);
    eDest = SRT_Table;
  }

  /* Generate code for the left and right SELECT statements.
  */
  switch( p->op ){
    case TK_ALL: {
      if( p->pOrderBy==0 ){
        pPrior->nLimit = p->nLimit;
        pPrior->nOffset = p->nOffset;
        rc = sqliteSelect(pParse, pPrior, eDest, iParm, 0, 0, 0);
        if( rc ) return rc;
        p->pPrior = 0;
        p->iLimit = pPrior->iLimit;
        p->iOffset = pPrior->iOffset;
        p->nLimit = -1;
        p->nOffset = 0;
        rc = sqliteSelect(pParse, p, eDest, iParm, 0, 0, 0);
        p->pPrior = pPrior;
        if( rc ) return rc;
        break;
      }
      /* For UNION ALL ... ORDER BY fall through to the next case */
    }
    case TK_EXCEPT:
    case TK_UNION: {
      int unionTab;    /* Cursor number of the temporary table holding result */
      int op;          /* One of the SRT_ operations to apply to self */
      int priorOp;     /* The SRT_ operation to apply to prior selects */
      int nLimit, nOffset; /* Saved values of p->nLimit and p->nOffset */
      ExprList *pOrderBy;  /* The ORDER BY clause for the right SELECT */

      priorOp = p->op==TK_ALL ? SRT_Table : SRT_Union;
      if( eDest==priorOp && p->pOrderBy==0 && p->nLimit<0 && p->nOffset==0 ){
        /* We can reuse a temporary table generated by a SELECT to our
        ** right.
        */
        unionTab = iParm;
      }else{
        /* We will need to create our own temporary table to hold the
        ** intermediate results.
        */
        unionTab = pParse->nTab++;
        if( p->pOrderBy 
        && matchOrderbyToColumn(pParse, p, p->pOrderBy, unionTab, 1) ){
          return 1;
        }
        if( p->op!=TK_ALL ){
          sqliteVdbeAddOp(v, OP_OpenTemp, unionTab, 1);
          sqliteVdbeAddOp(v, OP_KeyAsData, unionTab, 1);
        }else{
          sqliteVdbeAddOp(v, OP_OpenTemp, unionTab, 0);
        }
      }

      /* Code the SELECT statements to our left
      */
      rc = sqliteSelect(pParse, pPrior, priorOp, unionTab, 0, 0, 0);
      if( rc ) return rc;

      /* Code the current SELECT statement
      */
      switch( p->op ){
         case TK_EXCEPT:  op = SRT_Except;   break;
         case TK_UNION:   op = SRT_Union;    break;
         case TK_ALL:     op = SRT_Table;    break;
      }
      p->pPrior = 0;
      pOrderBy = p->pOrderBy;
      p->pOrderBy = 0;
      nLimit = p->nLimit;
      p->nLimit = -1;
      nOffset = p->nOffset;
      p->nOffset = 0;
      rc = sqliteSelect(pParse, p, op, unionTab, 0, 0, 0);
      p->pPrior = pPrior;
      p->pOrderBy = pOrderBy;
      p->nLimit = nLimit;
      p->nOffset = nOffset;
      if( rc ) return rc;

      /* Convert the data in the temporary table into whatever form
      ** it is that we currently need.
      */      
      if( eDest!=priorOp || unionTab!=iParm ){
        int iCont, iBreak, iStart;
        assert( p->pEList );
        if( eDest==SRT_Callback ){
          generateColumnNames(pParse, 0, p->pEList);
          generateColumnTypes(pParse, p->pSrc, p->pEList);
        }
        iBreak = sqliteVdbeMakeLabel(v);
        iCont = sqliteVdbeMakeLabel(v);
        sqliteVdbeAddOp(v, OP_Rewind, unionTab, iBreak);
        computeLimitRegisters(pParse, p);
        iStart = sqliteVdbeCurrentAddr(v);
        multiSelectSortOrder(p, p->pOrderBy);
        rc = selectInnerLoop(pParse, p, p->pEList, unionTab, p->pEList->nExpr,
                             p->pOrderBy, -1, eDest, iParm, 
                             iCont, iBreak);
        if( rc ) return 1;
        sqliteVdbeResolveLabel(v, iCont);
        sqliteVdbeAddOp(v, OP_Next, unionTab, iStart);
        sqliteVdbeResolveLabel(v, iBreak);
        sqliteVdbeAddOp(v, OP_Close, unionTab, 0);
        if( p->pOrderBy ){
          generateSortTail(p, v, p->pEList->nExpr, eDest, iParm);
        }
      }
      break;
    }
    case TK_INTERSECT: {
      int tab1, tab2;
      int iCont, iBreak, iStart;
      int nLimit, nOffset;

      /* INTERSECT is different from the others since it requires
      ** two temporary tables.  Hence it has its own case.  Begin
      ** by allocating the tables we will need.
      */
      tab1 = pParse->nTab++;
      tab2 = pParse->nTab++;
      if( p->pOrderBy && matchOrderbyToColumn(pParse,p,p->pOrderBy,tab1,1) ){
        return 1;
      }
      sqliteVdbeAddOp(v, OP_OpenTemp, tab1, 1);
      sqliteVdbeAddOp(v, OP_KeyAsData, tab1, 1);

      /* Code the SELECTs to our left into temporary table "tab1".
      */
      rc = sqliteSelect(pParse, pPrior, SRT_Union, tab1, 0, 0, 0);
      if( rc ) return rc;

      /* Code the current SELECT into temporary table "tab2"
      */
      sqliteVdbeAddOp(v, OP_OpenTemp, tab2, 1);
      sqliteVdbeAddOp(v, OP_KeyAsData, tab2, 1);
      p->pPrior = 0;
      nLimit = p->nLimit;
      p->nLimit = -1;
      nOffset = p->nOffset;
      p->nOffset = 0;
      rc = sqliteSelect(pParse, p, SRT_Union, tab2, 0, 0, 0);
      p->pPrior = pPrior;
      p->nLimit = nLimit;
      p->nOffset = nOffset;
      if( rc ) return rc;

      /* Generate code to take the intersection of the two temporary
      ** tables.
      */
      assert( p->pEList );
      if( eDest==SRT_Callback ){
        generateColumnNames(pParse, 0, p->pEList);
        generateColumnTypes(pParse, p->pSrc, p->pEList);
      }
      iBreak = sqliteVdbeMakeLabel(v);
      iCont = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_Rewind, tab1, iBreak);
      computeLimitRegisters(pParse, p);
      iStart = sqliteVdbeAddOp(v, OP_FullKey, tab1, 0);
      sqliteVdbeAddOp(v, OP_NotFound, tab2, iCont);
      multiSelectSortOrder(p, p->pOrderBy);
      rc = selectInnerLoop(pParse, p, p->pEList, tab1, p->pEList->nExpr,
                             p->pOrderBy, -1, eDest, iParm, 
                             iCont, iBreak);
      if( rc ) return 1;
      sqliteVdbeResolveLabel(v, iCont);
      sqliteVdbeAddOp(v, OP_Next, tab1, iStart);
      sqliteVdbeResolveLabel(v, iBreak);
      sqliteVdbeAddOp(v, OP_Close, tab2, 0);
      sqliteVdbeAddOp(v, OP_Close, tab1, 0);
      if( p->pOrderBy ){
        generateSortTail(p, v, p->pEList->nExpr, eDest, iParm);
      }
      break;
    }
  }
  assert( p->pEList && pPrior->pEList );
  if( p->pEList->nExpr!=pPrior->pEList->nExpr ){
    sqliteErrorMsg(pParse, "SELECTs to the left and right of %s"
      " do not have the same number of result columns", selectOpName(p->op));
    return 1;
  }
  return 0;
}

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
      pExpr->dataType = pNew->dataType;
      assert( pExpr->pLeft==0 );
      pExpr->pLeft = sqliteExprDup(pNew->pLeft);
      assert( pExpr->pRight==0 );
      pExpr->pRight = sqliteExprDup(pNew->pRight);
      assert( pExpr->pList==0 );
      pExpr->pList = sqliteExprListDup(pNew->pList);
      pExpr->iTable = pNew->iTable;
      pExpr->iColumn = pNew->iColumn;
      pExpr->iAgg = pNew->iAgg;
      sqliteTokenCopy(&pExpr->token, &pNew->token);
      sqliteTokenCopy(&pExpr->span, &pNew->span);
    }
  }else{
    substExpr(pExpr->pLeft, iTable, pEList);
    substExpr(pExpr->pRight, iTable, pEList);
    substExprList(pExpr->pList, iTable, pEList);
  }
}
static void 
substExprList(ExprList *pList, int iTable, ExprList *pEList){
  int i;
  if( pList==0 ) return;
  for(i=0; i<pList->nExpr; i++){
    substExpr(pList->a[i].pExpr, iTable, pEList);
  }
}

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
  Parse *pParse,       /* The parsing context */
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
  int i;
  Expr *pWhere;

  /* Check to see if flattening is permitted.  Return 0 if not.
  */
  if( p==0 ) return 0;
  pSrc = p->pSrc;
  assert( pSrc && iFrom>=0 && iFrom<pSrc->nSrc );
  pSub = pSrc->a[iFrom].pSelect;
  assert( pSub!=0 );
  if( isAgg && subqueryIsAgg ) return 0;
  if( subqueryIsAgg && pSrc->nSrc>1 ) return 0;
  pSubSrc = pSub->pSrc;
  assert( pSubSrc );
  if( pSubSrc->nSrc==0 ) return 0;
  if( (pSub->isDistinct || pSub->nLimit>=0) &&  (pSrc->nSrc>1 || isAgg) ){
     return 0;
  }
  if( (p->isDistinct || p->nLimit>=0) && subqueryIsAgg ) return 0;
  if( p->pOrderBy && pSub->pOrderBy ) return 0;

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
  if( pSubSrc->nSrc>1 && iFrom>0 && (pSrc->a[iFrom-1].jointype & JT_OUTER)!=0 ){
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
  if( iFrom>0 && (pSrc->a[iFrom-1].jointype & JT_OUTER)!=0 
      && pSub->pWhere!=0 ){
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
  iParent = pSrc->a[iFrom].iCursor;
  {
    int nSubSrc = pSubSrc->nSrc;
    int jointype = pSrc->a[iFrom].jointype;

    if( pSrc->a[iFrom].pTab && pSrc->a[iFrom].pTab->isTransient ){
      sqliteDeleteTable(0, pSrc->a[iFrom].pTab);
    }
    sqliteFree(pSrc->a[iFrom].zDatabase);
    sqliteFree(pSrc->a[iFrom].zName);
    sqliteFree(pSrc->a[iFrom].zAlias);
    if( nSubSrc>1 ){
      int extra = nSubSrc - 1;
      for(i=1; i<nSubSrc; i++){
        pSrc = sqliteSrcListAppend(pSrc, 0, 0);
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
    pSrc->a[iFrom+nSubSrc-1].jointype = jointype;
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
  substExprList(p->pEList, iParent, pSub->pEList);
  pList = p->pEList;
  for(i=0; i<pList->nExpr; i++){
    Expr *pExpr;
    if( pList->a[i].zName==0 && (pExpr = pList->a[i].pExpr)->span.z!=0 ){
      pList->a[i].zName = sqliteStrNDup(pExpr->span.z, pExpr->span.n);
    }
  }
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
    pWhere = sqliteExprDup(pSub->pWhere);
  }else{
    pWhere = 0;
  }
  if( subqueryIsAgg ){
    assert( p->pHaving==0 );
    p->pHaving = p->pWhere;
    p->pWhere = pWhere;
    substExpr(p->pHaving, iParent, pSub->pEList);
    if( pSub->pHaving ){
      Expr *pHaving = sqliteExprDup(pSub->pHaving);
      if( p->pHaving ){
        p->pHaving = sqliteExpr(TK_AND, p->pHaving, pHaving, 0);
      }else{
        p->pHaving = pHaving;
      }
    }
    assert( p->pGroupBy==0 );
    p->pGroupBy = sqliteExprListDup(pSub->pGroupBy);
  }else if( p->pWhere==0 ){
    p->pWhere = pWhere;
  }else{
    substExpr(p->pWhere, iParent, pSub->pEList);
    if( pWhere ){
      p->pWhere = sqliteExpr(TK_AND, p->pWhere, pWhere, 0);
    }
  }

  /* The flattened query is distinct if either the inner or the
  ** outer query is distinct. 
  */
  p->isDistinct = p->isDistinct || pSub->isDistinct;

  /* Transfer the limit expression from the subquery to the outer
  ** query.
  */
  if( pSub->nLimit>=0 ){
    if( p->nLimit<0 ){
      p->nLimit = pSub->nLimit;
    }else if( p->nLimit+p->nOffset > pSub->nLimit+pSub->nOffset ){
      p->nLimit = pSub->nLimit + pSub->nOffset - p->nOffset;
    }
  }
  p->nOffset += pSub->nOffset;

  /* Finially, delete what is left of the subquery and return
  ** success.
  */
  sqliteSelectDelete(pSub);
  return 1;
}

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
** The parameters to this routine are the same as for sqliteSelect().
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
  int cont;
  ExprList *pEList, *pList, eList;
  struct ExprList_item eListItem;
  SrcList *pSrc;
  

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
  if( sqliteStrNICmp(pExpr->token.z,"min",3)==0 ){
    seekOp = OP_Rewind;
  }else if( sqliteStrNICmp(pExpr->token.z,"max",3)==0 ){
    seekOp = OP_Last;
  }else{
    return 0;
  }
  pExpr = pList->a[0].pExpr;
  if( pExpr->op!=TK_COLUMN ) return 0;
  iCol = pExpr->iColumn;
  pTab = pSrc->a[0].pTab;

  /* If we get to here, it means the query is of the correct form.
  ** Check to make sure we have an index and make pIdx point to the
  ** appropriate index.  If the min() or max() is on an INTEGER PRIMARY
  ** key column, no index is necessary so set pIdx to NULL.  If no
  ** usable index is found, return 0.
  */
  if( iCol<0 ){
    pIdx = 0;
  }else{
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      assert( pIdx->nColumn>=1 );
      if( pIdx->aiColumn[0]==iCol ) break;
    }
    if( pIdx==0 ) return 0;
  }

  /* Identify column types if we will be using the callback.  This
  ** step is skipped if the output is going to a table or a memory cell.
  ** The column names have already been generated in the calling function.
  */
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return 0;
  if( eDest==SRT_Callback ){
    generateColumnTypes(pParse, p->pSrc, p->pEList);
  }

  /* If the output is destined for a temporary table, open that table.
  */
  if( eDest==SRT_TempTable ){
    sqliteVdbeAddOp(v, OP_OpenTemp, iParm, 0);
  }

  /* Generating code to find the min or the max.  Basically all we have
  ** to do is find the first or the last entry in the chosen index.  If
  ** the min() or max() is on the INTEGER PRIMARY KEY, then find the first
  ** or last entry in the main table.
  */
  sqliteCodeVerifySchema(pParse, pTab->iDb);
  base = pSrc->a[0].iCursor;
  computeLimitRegisters(pParse, p);
  if( pSrc->a[0].pSelect==0 ){
    sqliteVdbeAddOp(v, OP_Integer, pTab->iDb, 0);
    sqliteVdbeOp3(v, OP_OpenRead, base, pTab->tnum, pTab->zName, 0);
  }
  cont = sqliteVdbeMakeLabel(v);
  if( pIdx==0 ){
    sqliteVdbeAddOp(v, seekOp, base, 0);
  }else{
    sqliteVdbeAddOp(v, OP_Integer, pIdx->iDb, 0);
    sqliteVdbeOp3(v, OP_OpenRead, base+1, pIdx->tnum, pIdx->zName, P3_STATIC);
    sqliteVdbeAddOp(v, seekOp, base+1, 0);
    sqliteVdbeAddOp(v, OP_IdxRecno, base+1, 0);
    sqliteVdbeAddOp(v, OP_Close, base+1, 0);
    sqliteVdbeAddOp(v, OP_MoveTo, base, 0);
  }
  eList.nExpr = 1;
  memset(&eListItem, 0, sizeof(eListItem));
  eList.a = &eListItem;
  eList.a[0].pExpr = pExpr;
  selectInnerLoop(pParse, p, &eList, 0, 0, 0, -1, eDest, iParm, cont, cont);
  sqliteVdbeResolveLabel(v, cont);
  sqliteVdbeAddOp(v, OP_Close, base, 0);
  
  return 1;
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
**     SRT_Set         Store results as keys of a table with cursor iParm
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
int sqliteSelect(
  Parse *pParse,         /* The parser context */
  Select *p,             /* The SELECT statement being coded. */
  int eDest,             /* How to dispose of the results */
  int iParm,             /* A parameter used by the eDest disposal method */
  Select *pParent,       /* Another SELECT for which this is a sub-query */
  int parentTab,         /* Index in pParent->pSrc of this query */
  int *pParentAgg        /* True if pParent uses aggregate functions */
){
  int i;
  WhereInfo *pWInfo;
  Vdbe *v;
  int isAgg = 0;         /* True for select lists like "count(*)" */
  ExprList *pEList;      /* List of columns to extract. */
  SrcList *pTabList;     /* List of tables to select from */
  Expr *pWhere;          /* The WHERE clause.  May be NULL */
  ExprList *pOrderBy;    /* The ORDER BY clause.  May be NULL */
  ExprList *pGroupBy;    /* The GROUP BY clause.  May be NULL */
  Expr *pHaving;         /* The HAVING clause.  May be NULL */
  int isDistinct;        /* True if the DISTINCT keyword is present */
  int distinct;          /* Table to use for the distinct set */
  int rc = 1;            /* Value to return from this function */

  if( sqlite_malloc_failed || pParse->nErr || p==0 ) return 1;
  if( sqliteAuthCheck(pParse, SQLITE_SELECT, 0, 0, 0) ) return 1;

  /* If there is are a sequence of queries, do the earlier ones first.
  */
  if( p->pPrior ){
    return multiSelect(pParse, p, eDest, iParm);
  }

  /* Make local copies of the parameters for this query.
  */
  pTabList = p->pSrc;
  pWhere = p->pWhere;
  pOrderBy = p->pOrderBy;
  pGroupBy = p->pGroupBy;
  pHaving = p->pHaving;
  isDistinct = p->isDistinct;

  /* Allocate VDBE cursors for each table in the FROM clause
  */
  sqliteSrcListAssignCursors(pParse, pTabList);

  /* 
  ** Do not even attempt to generate any code if we have already seen
  ** errors before this routine starts.
  */
  if( pParse->nErr>0 ) goto select_end;

  /* Expand any "*" terms in the result set.  (For example the "*" in
  ** "SELECT * FROM t1")  The fillInColumnlist() routine also does some
  ** other housekeeping - see the header comment for details.
  */
  if( fillInColumnList(pParse, p) ){
    goto select_end;
  }
  pWhere = p->pWhere;
  pEList = p->pEList;
  if( pEList==0 ) goto select_end;

  /* If writing to memory or generating a set
  ** only a single column may be output.
  */
  if( (eDest==SRT_Mem || eDest==SRT_Set) && pEList->nExpr>1 ){
    sqliteErrorMsg(pParse, "only a single result allowed for "
       "a SELECT that is part of an expression");
    goto select_end;
  }

  /* ORDER BY is ignored for some destinations.
  */
  switch( eDest ){
    case SRT_Union:
    case SRT_Except:
    case SRT_Discard:
      pOrderBy = 0;
      break;
    default:
      break;
  }

  /* At this point, we should have allocated all the cursors that we
  ** need to handle subquerys and temporary tables.  
  **
  ** Resolve the column names and do a semantics check on all the expressions.
  */
  for(i=0; i<pEList->nExpr; i++){
    if( sqliteExprResolveIds(pParse, pTabList, 0, pEList->a[i].pExpr) ){
      goto select_end;
    }
    if( sqliteExprCheck(pParse, pEList->a[i].pExpr, 1, &isAgg) ){
      goto select_end;
    }
  }
  if( pWhere ){
    if( sqliteExprResolveIds(pParse, pTabList, pEList, pWhere) ){
      goto select_end;
    }
    if( sqliteExprCheck(pParse, pWhere, 0, 0) ){
      goto select_end;
    }
  }
  if( pHaving ){
    if( pGroupBy==0 ){
      sqliteErrorMsg(pParse, "a GROUP BY clause is required before HAVING");
      goto select_end;
    }
    if( sqliteExprResolveIds(pParse, pTabList, pEList, pHaving) ){
      goto select_end;
    }
    if( sqliteExprCheck(pParse, pHaving, 1, &isAgg) ){
      goto select_end;
    }
  }
  if( pOrderBy ){
    for(i=0; i<pOrderBy->nExpr; i++){
      int iCol;
      Expr *pE = pOrderBy->a[i].pExpr;
      if( sqliteExprIsInteger(pE, &iCol) && iCol>0 && iCol<=pEList->nExpr ){
        sqliteExprDelete(pE);
        pE = pOrderBy->a[i].pExpr = sqliteExprDup(pEList->a[iCol-1].pExpr);
      }
      if( sqliteExprResolveIds(pParse, pTabList, pEList, pE) ){
        goto select_end;
      }
      if( sqliteExprCheck(pParse, pE, isAgg, 0) ){
        goto select_end;
      }
      if( sqliteExprIsConstant(pE) ){
        if( sqliteExprIsInteger(pE, &iCol)==0 ){
          sqliteErrorMsg(pParse,
             "ORDER BY terms must not be non-integer constants");
          goto select_end;
        }else if( iCol<=0 || iCol>pEList->nExpr ){
          sqliteErrorMsg(pParse, 
             "ORDER BY column number %d out of range - should be "
             "between 1 and %d", iCol, pEList->nExpr);
          goto select_end;
        }
      }
    }
  }
  if( pGroupBy ){
    for(i=0; i<pGroupBy->nExpr; i++){
      int iCol;
      Expr *pE = pGroupBy->a[i].pExpr;
      if( sqliteExprIsInteger(pE, &iCol) && iCol>0 && iCol<=pEList->nExpr ){
        sqliteExprDelete(pE);
        pE = pGroupBy->a[i].pExpr = sqliteExprDup(pEList->a[iCol-1].pExpr);
      }
      if( sqliteExprResolveIds(pParse, pTabList, pEList, pE) ){
        goto select_end;
      }
      if( sqliteExprCheck(pParse, pE, isAgg, 0) ){
        goto select_end;
      }
      if( sqliteExprIsConstant(pE) ){
        if( sqliteExprIsInteger(pE, &iCol)==0 ){
          sqliteErrorMsg(pParse,
            "GROUP BY terms must not be non-integer constants");
          goto select_end;
        }else if( iCol<=0 || iCol>pEList->nExpr ){
          sqliteErrorMsg(pParse,
             "GROUP BY column number %d out of range - should be "
             "between 1 and %d", iCol, pEList->nExpr);
          goto select_end;
        }
      }
    }
  }

  /* Begin generating code.
  */
  v = sqliteGetVdbe(pParse);
  if( v==0 ) goto select_end;

  /* Identify column names if we will be using them in a callback.  This
  ** step is skipped if the output is going to some other destination.
  */
  if( eDest==SRT_Callback ){
    generateColumnNames(pParse, pTabList, pEList);
  }

  /* Generate code for all sub-queries in the FROM clause
  */
  for(i=0; i<pTabList->nSrc; i++){
    const char *zSavedAuthContext;
    int needRestoreContext;

    if( pTabList->a[i].pSelect==0 ) continue;
    if( pTabList->a[i].zName!=0 ){
      zSavedAuthContext = pParse->zAuthContext;
      pParse->zAuthContext = pTabList->a[i].zName;
      needRestoreContext = 1;
    }else{
      needRestoreContext = 0;
    }
    sqliteSelect(pParse, pTabList->a[i].pSelect, SRT_TempTable, 
                 pTabList->a[i].iCursor, p, i, &isAgg);
    if( needRestoreContext ){
      pParse->zAuthContext = zSavedAuthContext;
    }
    pTabList = p->pSrc;
    pWhere = p->pWhere;
    if( eDest!=SRT_Union && eDest!=SRT_Except && eDest!=SRT_Discard ){
      pOrderBy = p->pOrderBy;
    }
    pGroupBy = p->pGroupBy;
    pHaving = p->pHaving;
    isDistinct = p->isDistinct;
  }

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
  if( pParent && pParentAgg &&
      flattenSubquery(pParse, pParent, parentTab, *pParentAgg, isAgg) ){
    if( isAgg ) *pParentAgg = 1;
    return rc;
  }

  /* Set the limiter.
  */
  computeLimitRegisters(pParse, p);

  /* Identify column types if we will be using a callback.  This
  ** step is skipped if the output is going to a destination other
  ** than a callback.
  **
  ** We have to do this separately from the creation of column names
  ** above because if the pTabList contains views then they will not
  ** have been resolved and we will not know the column types until
  ** now.
  */
  if( eDest==SRT_Callback ){
    generateColumnTypes(pParse, pTabList, pEList);
  }

  /* If the output is destined for a temporary table, open that table.
  */
  if( eDest==SRT_TempTable ){
    sqliteVdbeAddOp(v, OP_OpenTemp, iParm, 0);
  }

  /* Do an analysis of aggregate expressions.
  */
  sqliteAggregateInfoReset(pParse);
  if( isAgg || pGroupBy ){
    assert( pParse->nAgg==0 );
    isAgg = 1;
    for(i=0; i<pEList->nExpr; i++){
      if( sqliteExprAnalyzeAggregates(pParse, pEList->a[i].pExpr) ){
        goto select_end;
      }
    }
    if( pGroupBy ){
      for(i=0; i<pGroupBy->nExpr; i++){
        if( sqliteExprAnalyzeAggregates(pParse, pGroupBy->a[i].pExpr) ){
          goto select_end;
        }
      }
    }
    if( pHaving && sqliteExprAnalyzeAggregates(pParse, pHaving) ){
      goto select_end;
    }
    if( pOrderBy ){
      for(i=0; i<pOrderBy->nExpr; i++){
        if( sqliteExprAnalyzeAggregates(pParse, pOrderBy->a[i].pExpr) ){
          goto select_end;
        }
      }
    }
  }

  /* Reset the aggregator
  */
  if( isAgg ){
    sqliteVdbeAddOp(v, OP_AggReset, 0, pParse->nAgg);
    for(i=0; i<pParse->nAgg; i++){
      FuncDef *pFunc;
      if( (pFunc = pParse->aAgg[i].pFunc)!=0 && pFunc->xFinalize!=0 ){
        sqliteVdbeOp3(v, OP_AggInit, 0, i, (char*)pFunc, P3_POINTER);
      }
    }
    if( pGroupBy==0 ){
      sqliteVdbeAddOp(v, OP_String, 0, 0);
      sqliteVdbeAddOp(v, OP_AggFocus, 0, 0);
    }
  }

  /* Initialize the memory cell to NULL
  */
  if( eDest==SRT_Mem ){
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeAddOp(v, OP_MemStore, iParm, 1);
  }

  /* Open a temporary table to use for the distinct set.
  */
  if( isDistinct ){
    distinct = pParse->nTab++;
    sqliteVdbeAddOp(v, OP_OpenTemp, distinct, 1);
  }else{
    distinct = -1;
  }

  /* Begin the database scan
  */
  pWInfo = sqliteWhereBegin(pParse, pTabList, pWhere, 0, 
                            pGroupBy ? 0 : &pOrderBy);
  if( pWInfo==0 ) goto select_end;

  /* Use the standard inner loop if we are not dealing with
  ** aggregates
  */
  if( !isAgg ){
    if( selectInnerLoop(pParse, p, pEList, 0, 0, pOrderBy, distinct, eDest,
                    iParm, pWInfo->iContinue, pWInfo->iBreak) ){
       goto select_end;
    }
  }

  /* If we are dealing with aggregates, then do the special aggregate
  ** processing.  
  */
  else{
    AggExpr *pAgg;
    if( pGroupBy ){
      int lbl1;
      for(i=0; i<pGroupBy->nExpr; i++){
        sqliteExprCode(pParse, pGroupBy->a[i].pExpr);
      }
      sqliteVdbeAddOp(v, OP_MakeKey, pGroupBy->nExpr, 0);
      if( pParse->db->file_format>=4 ) sqliteAddKeyType(v, pGroupBy);
      lbl1 = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_AggFocus, 0, lbl1);
      for(i=0, pAgg=pParse->aAgg; i<pParse->nAgg; i++, pAgg++){
        if( pAgg->isAgg ) continue;
        sqliteExprCode(pParse, pAgg->pExpr);
        sqliteVdbeAddOp(v, OP_AggSet, 0, i);
      }
      sqliteVdbeResolveLabel(v, lbl1);
    }
    for(i=0, pAgg=pParse->aAgg; i<pParse->nAgg; i++, pAgg++){
      Expr *pE;
      int nExpr;
      FuncDef *pDef;
      if( !pAgg->isAgg ) continue;
      assert( pAgg->pFunc!=0 );
      assert( pAgg->pFunc->xStep!=0 );
      pDef = pAgg->pFunc;
      pE = pAgg->pExpr;
      assert( pE!=0 );
      assert( pE->op==TK_AGG_FUNCTION );
      nExpr = sqliteExprCodeExprList(pParse, pE->pList, pDef->includeTypes);
      sqliteVdbeAddOp(v, OP_Integer, i, 0);
      sqliteVdbeOp3(v, OP_AggFunc, 0, nExpr, (char*)pDef, P3_POINTER);
    }
  }

  /* End the database scan loop.
  */
  sqliteWhereEnd(pWInfo);

  /* If we are processing aggregates, we need to set up a second loop
  ** over all of the aggregate values and process them.
  */
  if( isAgg ){
    int endagg = sqliteVdbeMakeLabel(v);
    int startagg;
    startagg = sqliteVdbeAddOp(v, OP_AggNext, 0, endagg);
    pParse->useAgg = 1;
    if( pHaving ){
      sqliteExprIfFalse(pParse, pHaving, startagg, 1);
    }
    if( selectInnerLoop(pParse, p, pEList, 0, 0, pOrderBy, distinct, eDest,
                    iParm, startagg, endagg) ){
      goto select_end;
    }
    sqliteVdbeAddOp(v, OP_Goto, 0, startagg);
    sqliteVdbeResolveLabel(v, endagg);
    sqliteVdbeAddOp(v, OP_Noop, 0, 0);
    pParse->useAgg = 0;
  }

  /* If there is an ORDER BY clause, then we need to sort the results
  ** and send them to the callback one by one.
  */
  if( pOrderBy ){
    generateSortTail(p, v, pEList->nExpr, eDest, iParm);
  }

  /* If this was a subquery, we have now converted the subquery into a
  ** temporary table.  So delete the subquery structure from the parent
  ** to prevent this subquery from being evaluated again and to force the
  ** the use of the temporary table.
  */
  if( pParent ){
    assert( pParent->pSrc->nSrc>parentTab );
    assert( pParent->pSrc->a[parentTab].pSelect==p );
    sqliteSelectDelete(p);
    pParent->pSrc->a[parentTab].pSelect = 0;
  }

  /* The SELECT was successfully coded.   Set the return code to 0
  ** to indicate no errors.
  */
  rc = 0;

  /* Control jumps to here if an error is encountered above, or upon
  ** successful coding of the SELECT.
  */
select_end:
  sqliteAggregateInfoReset(pParse);
  return rc;
}
