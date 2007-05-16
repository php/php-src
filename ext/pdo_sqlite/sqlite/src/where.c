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
** the WHERE clause of SQL statements.  This module is reponsible for
** generating the code that loops through a table looking for applicable
** rows.  Indices are selected and used to speed the search when doing
** so is applicable.  Because this module is responsible for selecting
** indices, you might also think of this module as the "query optimizer".
**
** $Id$
*/
#include "sqliteInt.h"

/*
** The number of bits in a Bitmask.  "BMS" means "BitMask Size".
*/
#define BMS  (sizeof(Bitmask)*8)

/*
** Trace output macros
*/
#if defined(SQLITE_TEST) || defined(SQLITE_DEBUG)
int sqlite3_where_trace = 0;
# define WHERETRACE(X)  if(sqlite3_where_trace) sqlite3DebugPrintf X
#else
# define WHERETRACE(X)
#endif

/* Forward reference
*/
typedef struct WhereClause WhereClause;
typedef struct ExprMaskSet ExprMaskSet;

/*
** The query generator uses an array of instances of this structure to
** help it analyze the subexpressions of the WHERE clause.  Each WHERE
** clause subexpression is separated from the others by an AND operator.
**
** All WhereTerms are collected into a single WhereClause structure.  
** The following identity holds:
**
**        WhereTerm.pWC->a[WhereTerm.idx] == WhereTerm
**
** When a term is of the form:
**
**              X <op> <expr>
**
** where X is a column name and <op> is one of certain operators,
** then WhereTerm.leftCursor and WhereTerm.leftColumn record the
** cursor number and column number for X.  WhereTerm.operator records
** the <op> using a bitmask encoding defined by WO_xxx below.  The
** use of a bitmask encoding for the operator allows us to search
** quickly for terms that match any of several different operators.
**
** prereqRight and prereqAll record sets of cursor numbers,
** but they do so indirectly.  A single ExprMaskSet structure translates
** cursor number into bits and the translated bit is stored in the prereq
** fields.  The translation is used in order to maximize the number of
** bits that will fit in a Bitmask.  The VDBE cursor numbers might be
** spread out over the non-negative integers.  For example, the cursor
** numbers might be 3, 8, 9, 10, 20, 23, 41, and 45.  The ExprMaskSet
** translates these sparse cursor numbers into consecutive integers
** beginning with 0 in order to make the best possible use of the available
** bits in the Bitmask.  So, in the example above, the cursor numbers
** would be mapped into integers 0 through 7.
*/
typedef struct WhereTerm WhereTerm;
struct WhereTerm {
  Expr *pExpr;            /* Pointer to the subexpression */
  i16 iParent;            /* Disable pWC->a[iParent] when this term disabled */
  i16 leftCursor;         /* Cursor number of X in "X <op> <expr>" */
  i16 leftColumn;         /* Column number of X in "X <op> <expr>" */
  u16 eOperator;          /* A WO_xx value describing <op> */
  u8 flags;               /* Bit flags.  See below */
  u8 nChild;              /* Number of children that must disable us */
  WhereClause *pWC;       /* The clause this term is part of */
  Bitmask prereqRight;    /* Bitmask of tables used by pRight */
  Bitmask prereqAll;      /* Bitmask of tables referenced by p */
};

/*
** Allowed values of WhereTerm.flags
*/
#define TERM_DYNAMIC    0x01   /* Need to call sqlite3ExprDelete(pExpr) */
#define TERM_VIRTUAL    0x02   /* Added by the optimizer.  Do not code */
#define TERM_CODED      0x04   /* This term is already coded */
#define TERM_COPIED     0x08   /* Has a child */
#define TERM_OR_OK      0x10   /* Used during OR-clause processing */

/*
** An instance of the following structure holds all information about a
** WHERE clause.  Mostly this is a container for one or more WhereTerms.
*/
struct WhereClause {
  Parse *pParse;           /* The parser context */
  ExprMaskSet *pMaskSet;   /* Mapping of table indices to bitmasks */
  int nTerm;               /* Number of terms */
  int nSlot;               /* Number of entries in a[] */
  WhereTerm *a;            /* Each a[] describes a term of the WHERE cluase */
  WhereTerm aStatic[10];   /* Initial static space for a[] */
};

/*
** An instance of the following structure keeps track of a mapping
** between VDBE cursor numbers and bits of the bitmasks in WhereTerm.
**
** The VDBE cursor numbers are small integers contained in 
** SrcList_item.iCursor and Expr.iTable fields.  For any given WHERE 
** clause, the cursor numbers might not begin with 0 and they might
** contain gaps in the numbering sequence.  But we want to make maximum
** use of the bits in our bitmasks.  This structure provides a mapping
** from the sparse cursor numbers into consecutive integers beginning
** with 0.
**
** If ExprMaskSet.ix[A]==B it means that The A-th bit of a Bitmask
** corresponds VDBE cursor number B.  The A-th bit of a bitmask is 1<<A.
**
** For example, if the WHERE clause expression used these VDBE
** cursors:  4, 5, 8, 29, 57, 73.  Then the  ExprMaskSet structure
** would map those cursor numbers into bits 0 through 5.
**
** Note that the mapping is not necessarily ordered.  In the example
** above, the mapping might go like this:  4->3, 5->1, 8->2, 29->0,
** 57->5, 73->4.  Or one of 719 other combinations might be used. It
** does not really matter.  What is important is that sparse cursor
** numbers all get mapped into bit numbers that begin with 0 and contain
** no gaps.
*/
struct ExprMaskSet {
  int n;                        /* Number of assigned cursor values */
  int ix[sizeof(Bitmask)*8];    /* Cursor assigned to each bit */
};


/*
** Bitmasks for the operators that indices are able to exploit.  An
** OR-ed combination of these values can be used when searching for
** terms in the where clause.
*/
#define WO_IN     1
#define WO_EQ     2
#define WO_LT     (WO_EQ<<(TK_LT-TK_EQ))
#define WO_LE     (WO_EQ<<(TK_LE-TK_EQ))
#define WO_GT     (WO_EQ<<(TK_GT-TK_EQ))
#define WO_GE     (WO_EQ<<(TK_GE-TK_EQ))
#define WO_MATCH  64
#define WO_ISNULL 128

/*
** Value for flags returned by bestIndex().  
**
** The least significant byte is reserved as a mask for WO_ values above.
** The WhereLevel.flags field is usually set to WO_IN|WO_EQ|WO_ISNULL.
** But if the table is the right table of a left join, WhereLevel.flags
** is set to WO_IN|WO_EQ.  The WhereLevel.flags field can then be used as
** the "op" parameter to findTerm when we are resolving equality constraints.
** ISNULL constraints will then not be used on the right table of a left
** join.  Tickets #2177 and #2189.
*/
#define WHERE_ROWID_EQ     0x000100   /* rowid=EXPR or rowid IN (...) */
#define WHERE_ROWID_RANGE  0x000200   /* rowid<EXPR and/or rowid>EXPR */
#define WHERE_COLUMN_EQ    0x001000   /* x=EXPR or x IN (...) */
#define WHERE_COLUMN_RANGE 0x002000   /* x<EXPR and/or x>EXPR */
#define WHERE_COLUMN_IN    0x004000   /* x IN (...) */
#define WHERE_TOP_LIMIT    0x010000   /* x<EXPR or x<=EXPR constraint */
#define WHERE_BTM_LIMIT    0x020000   /* x>EXPR or x>=EXPR constraint */
#define WHERE_IDX_ONLY     0x080000   /* Use index only - omit table */
#define WHERE_ORDERBY      0x100000   /* Output will appear in correct order */
#define WHERE_REVERSE      0x200000   /* Scan in reverse order */
#define WHERE_UNIQUE       0x400000   /* Selects no more than one row */
#define WHERE_VIRTUALTABLE 0x800000   /* Use virtual-table processing */

/*
** Initialize a preallocated WhereClause structure.
*/
static void whereClauseInit(
  WhereClause *pWC,        /* The WhereClause to be initialized */
  Parse *pParse,           /* The parsing context */
  ExprMaskSet *pMaskSet    /* Mapping from table indices to bitmasks */
){
  pWC->pParse = pParse;
  pWC->pMaskSet = pMaskSet;
  pWC->nTerm = 0;
  pWC->nSlot = ArraySize(pWC->aStatic);
  pWC->a = pWC->aStatic;
}

/*
** Deallocate a WhereClause structure.  The WhereClause structure
** itself is not freed.  This routine is the inverse of whereClauseInit().
*/
static void whereClauseClear(WhereClause *pWC){
  int i;
  WhereTerm *a;
  for(i=pWC->nTerm-1, a=pWC->a; i>=0; i--, a++){
    if( a->flags & TERM_DYNAMIC ){
      sqlite3ExprDelete(a->pExpr);
    }
  }
  if( pWC->a!=pWC->aStatic ){
    sqliteFree(pWC->a);
  }
}

/*
** Add a new entries to the WhereClause structure.  Increase the allocated
** space as necessary.
**
** If the flags argument includes TERM_DYNAMIC, then responsibility
** for freeing the expression p is assumed by the WhereClause object.
**
** WARNING:  This routine might reallocate the space used to store
** WhereTerms.  All pointers to WhereTerms should be invalided after
** calling this routine.  Such pointers may be reinitialized by referencing
** the pWC->a[] array.
*/
static int whereClauseInsert(WhereClause *pWC, Expr *p, int flags){
  WhereTerm *pTerm;
  int idx;
  if( pWC->nTerm>=pWC->nSlot ){
    WhereTerm *pOld = pWC->a;
    pWC->a = sqliteMalloc( sizeof(pWC->a[0])*pWC->nSlot*2 );
    if( pWC->a==0 ){
      if( flags & TERM_DYNAMIC ){
        sqlite3ExprDelete(p);
      }
      return 0;
    }
    memcpy(pWC->a, pOld, sizeof(pWC->a[0])*pWC->nTerm);
    if( pOld!=pWC->aStatic ){
      sqliteFree(pOld);
    }
    pWC->nSlot *= 2;
  }
  pTerm = &pWC->a[idx = pWC->nTerm];
  pWC->nTerm++;
  pTerm->pExpr = p;
  pTerm->flags = flags;
  pTerm->pWC = pWC;
  pTerm->iParent = -1;
  return idx;
}

/*
** This routine identifies subexpressions in the WHERE clause where
** each subexpression is separated by the AND operator or some other
** operator specified in the op parameter.  The WhereClause structure
** is filled with pointers to subexpressions.  For example:
**
**    WHERE  a=='hello' AND coalesce(b,11)<10 AND (c+12!=d OR c==22)
**           \________/     \_______________/     \________________/
**            slot[0]            slot[1]               slot[2]
**
** The original WHERE clause in pExpr is unaltered.  All this routine
** does is make slot[] entries point to substructure within pExpr.
**
** In the previous sentence and in the diagram, "slot[]" refers to
** the WhereClause.a[] array.  This array grows as needed to contain
** all terms of the WHERE clause.
*/
static void whereSplit(WhereClause *pWC, Expr *pExpr, int op){
  if( pExpr==0 ) return;
  if( pExpr->op!=op ){
    whereClauseInsert(pWC, pExpr, 0);
  }else{
    whereSplit(pWC, pExpr->pLeft, op);
    whereSplit(pWC, pExpr->pRight, op);
  }
}

/*
** Initialize an expression mask set
*/
#define initMaskSet(P)  memset(P, 0, sizeof(*P))

/*
** Return the bitmask for the given cursor number.  Return 0 if
** iCursor is not in the set.
*/
static Bitmask getMask(ExprMaskSet *pMaskSet, int iCursor){
  int i;
  for(i=0; i<pMaskSet->n; i++){
    if( pMaskSet->ix[i]==iCursor ){
      return ((Bitmask)1)<<i;
    }
  }
  return 0;
}

/*
** Create a new mask for cursor iCursor.
**
** There is one cursor per table in the FROM clause.  The number of
** tables in the FROM clause is limited by a test early in the
** sqlite3WhereBegin() routine.  So we know that the pMaskSet->ix[]
** array will never overflow.
*/
static void createMask(ExprMaskSet *pMaskSet, int iCursor){
  assert( pMaskSet->n < ArraySize(pMaskSet->ix) );
  pMaskSet->ix[pMaskSet->n++] = iCursor;
}

/*
** This routine walks (recursively) an expression tree and generates
** a bitmask indicating which tables are used in that expression
** tree.
**
** In order for this routine to work, the calling function must have
** previously invoked sqlite3ExprResolveNames() on the expression.  See
** the header comment on that routine for additional information.
** The sqlite3ExprResolveNames() routines looks for column names and
** sets their opcodes to TK_COLUMN and their Expr.iTable fields to
** the VDBE cursor number of the table.  This routine just has to
** translate the cursor numbers into bitmask values and OR all
** the bitmasks together.
*/
static Bitmask exprListTableUsage(ExprMaskSet*, ExprList*);
static Bitmask exprSelectTableUsage(ExprMaskSet*, Select*);
static Bitmask exprTableUsage(ExprMaskSet *pMaskSet, Expr *p){
  Bitmask mask = 0;
  if( p==0 ) return 0;
  if( p->op==TK_COLUMN ){
    mask = getMask(pMaskSet, p->iTable);
    return mask;
  }
  mask = exprTableUsage(pMaskSet, p->pRight);
  mask |= exprTableUsage(pMaskSet, p->pLeft);
  mask |= exprListTableUsage(pMaskSet, p->pList);
  mask |= exprSelectTableUsage(pMaskSet, p->pSelect);
  return mask;
}
static Bitmask exprListTableUsage(ExprMaskSet *pMaskSet, ExprList *pList){
  int i;
  Bitmask mask = 0;
  if( pList ){
    for(i=0; i<pList->nExpr; i++){
      mask |= exprTableUsage(pMaskSet, pList->a[i].pExpr);
    }
  }
  return mask;
}
static Bitmask exprSelectTableUsage(ExprMaskSet *pMaskSet, Select *pS){
  Bitmask mask;
  if( pS==0 ){
    mask = 0;
  }else{
    mask = exprListTableUsage(pMaskSet, pS->pEList);
    mask |= exprListTableUsage(pMaskSet, pS->pGroupBy);
    mask |= exprListTableUsage(pMaskSet, pS->pOrderBy);
    mask |= exprTableUsage(pMaskSet, pS->pWhere);
    mask |= exprTableUsage(pMaskSet, pS->pHaving);
  }
  return mask;
}

/*
** Return TRUE if the given operator is one of the operators that is
** allowed for an indexable WHERE clause term.  The allowed operators are
** "=", "<", ">", "<=", ">=", and "IN".
*/
static int allowedOp(int op){
  assert( TK_GT>TK_EQ && TK_GT<TK_GE );
  assert( TK_LT>TK_EQ && TK_LT<TK_GE );
  assert( TK_LE>TK_EQ && TK_LE<TK_GE );
  assert( TK_GE==TK_EQ+4 );
  return op==TK_IN || (op>=TK_EQ && op<=TK_GE) || op==TK_ISNULL;
}

/*
** Swap two objects of type T.
*/
#define SWAP(TYPE,A,B) {TYPE t=A; A=B; B=t;}

/*
** Commute a comparision operator.  Expressions of the form "X op Y"
** are converted into "Y op X".
*/
static void exprCommute(Expr *pExpr){
  assert( allowedOp(pExpr->op) && pExpr->op!=TK_IN );
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
}

/*
** Translate from TK_xx operator to WO_xx bitmask.
*/
static int operatorMask(int op){
  int c;
  assert( allowedOp(op) );
  if( op==TK_IN ){
    c = WO_IN;
  }else if( op==TK_ISNULL ){
    c = WO_ISNULL;
  }else{
    c = WO_EQ<<(op-TK_EQ);
  }
  assert( op!=TK_ISNULL || c==WO_ISNULL );
  assert( op!=TK_IN || c==WO_IN );
  assert( op!=TK_EQ || c==WO_EQ );
  assert( op!=TK_LT || c==WO_LT );
  assert( op!=TK_LE || c==WO_LE );
  assert( op!=TK_GT || c==WO_GT );
  assert( op!=TK_GE || c==WO_GE );
  return c;
}

/*
** Search for a term in the WHERE clause that is of the form "X <op> <expr>"
** where X is a reference to the iColumn of table iCur and <op> is one of
** the WO_xx operator codes specified by the op parameter.
** Return a pointer to the term.  Return 0 if not found.
*/
static WhereTerm *findTerm(
  WhereClause *pWC,     /* The WHERE clause to be searched */
  int iCur,             /* Cursor number of LHS */
  int iColumn,          /* Column number of LHS */
  Bitmask notReady,     /* RHS must not overlap with this mask */
  u16 op,               /* Mask of WO_xx values describing operator */
  Index *pIdx           /* Must be compatible with this index, if not NULL */
){
  WhereTerm *pTerm;
  int k;
  for(pTerm=pWC->a, k=pWC->nTerm; k; k--, pTerm++){
    if( pTerm->leftCursor==iCur
       && (pTerm->prereqRight & notReady)==0
       && pTerm->leftColumn==iColumn
       && (pTerm->eOperator & op)!=0
    ){
      if( iCur>=0 && pIdx && pTerm->eOperator!=WO_ISNULL ){
        Expr *pX = pTerm->pExpr;
        CollSeq *pColl;
        char idxaff;
        int j;
        Parse *pParse = pWC->pParse;

        idxaff = pIdx->pTable->aCol[iColumn].affinity;
        if( !sqlite3IndexAffinityOk(pX, idxaff) ) continue;
        pColl = sqlite3ExprCollSeq(pParse, pX->pLeft);
        if( !pColl ){
          if( pX->pRight ){
            pColl = sqlite3ExprCollSeq(pParse, pX->pRight);
          }
          if( !pColl ){
            pColl = pParse->db->pDfltColl;
          }
        }
        for(j=0; j<pIdx->nColumn && pIdx->aiColumn[j]!=iColumn; j++){}
        assert( j<pIdx->nColumn );
        if( sqlite3StrICmp(pColl->zName, pIdx->azColl[j]) ) continue;
      }
      return pTerm;
    }
  }
  return 0;
}

/* Forward reference */
static void exprAnalyze(SrcList*, WhereClause*, int);

/*
** Call exprAnalyze on all terms in a WHERE clause.  
**
**
*/
static void exprAnalyzeAll(
  SrcList *pTabList,       /* the FROM clause */
  WhereClause *pWC         /* the WHERE clause to be analyzed */
){
  int i;
  for(i=pWC->nTerm-1; i>=0; i--){
    exprAnalyze(pTabList, pWC, i);
  }
}

#ifndef SQLITE_OMIT_LIKE_OPTIMIZATION
/*
** Check to see if the given expression is a LIKE or GLOB operator that
** can be optimized using inequality constraints.  Return TRUE if it is
** so and false if not.
**
** In order for the operator to be optimizible, the RHS must be a string
** literal that does not begin with a wildcard.  
*/
static int isLikeOrGlob(
  sqlite3 *db,      /* The database */
  Expr *pExpr,      /* Test this expression */
  int *pnPattern,   /* Number of non-wildcard prefix characters */
  int *pisComplete  /* True if the only wildcard is % in the last character */
){
  const char *z;
  Expr *pRight, *pLeft;
  ExprList *pList;
  int c, cnt;
  int noCase;
  char wc[3];
  CollSeq *pColl;

  if( !sqlite3IsLikeFunction(db, pExpr, &noCase, wc) ){
    return 0;
  }
  pList = pExpr->pList;
  pRight = pList->a[0].pExpr;
  if( pRight->op!=TK_STRING ){
    return 0;
  }
  pLeft = pList->a[1].pExpr;
  if( pLeft->op!=TK_COLUMN ){
    return 0;
  }
  pColl = pLeft->pColl;
  if( pColl==0 ){
    /* TODO: Coverage testing doesn't get this case. Is it actually possible
    ** for an expression of type TK_COLUMN to not have an assigned collation 
    ** sequence at this point?
    */
    pColl = db->pDfltColl;
  }
  if( (pColl->type!=SQLITE_COLL_BINARY || noCase) &&
      (pColl->type!=SQLITE_COLL_NOCASE || !noCase) ){
    return 0;
  }
  sqlite3DequoteExpr(pRight);
  z = (char *)pRight->token.z;
  for(cnt=0; (c=z[cnt])!=0 && c!=wc[0] && c!=wc[1] && c!=wc[2]; cnt++){}
  if( cnt==0 || 255==(u8)z[cnt] ){
    return 0;
  }
  *pisComplete = z[cnt]==wc[0] && z[cnt+1]==0;
  *pnPattern = cnt;
  return 1;
}
#endif /* SQLITE_OMIT_LIKE_OPTIMIZATION */


#ifndef SQLITE_OMIT_VIRTUALTABLE
/*
** Check to see if the given expression is of the form
**
**         column MATCH expr
**
** If it is then return TRUE.  If not, return FALSE.
*/
static int isMatchOfColumn(
  Expr *pExpr      /* Test this expression */
){
  ExprList *pList;

  if( pExpr->op!=TK_FUNCTION ){
    return 0;
  }
  if( pExpr->token.n!=5 ||
       sqlite3StrNICmp((const char*)pExpr->token.z,"match",5)!=0 ){
    return 0;
  }
  pList = pExpr->pList;
  if( pList->nExpr!=2 ){
    return 0;
  }
  if( pList->a[1].pExpr->op != TK_COLUMN ){
    return 0;
  }
  return 1;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

/*
** If the pBase expression originated in the ON or USING clause of
** a join, then transfer the appropriate markings over to derived.
*/
static void transferJoinMarkings(Expr *pDerived, Expr *pBase){
  pDerived->flags |= pBase->flags & EP_FromJoin;
  pDerived->iRightJoinTable = pBase->iRightJoinTable;
}

#if !defined(SQLITE_OMIT_OR_OPTIMIZATION) && !defined(SQLITE_OMIT_SUBQUERY)
/*
** Return TRUE if the given term of an OR clause can be converted
** into an IN clause.  The iCursor and iColumn define the left-hand
** side of the IN clause.
**
** The context is that we have multiple OR-connected equality terms
** like this:
**
**           a=<expr1> OR  a=<expr2> OR b=<expr3>  OR ...
**
** The pOrTerm input to this routine corresponds to a single term of
** this OR clause.  In order for the term to be a condidate for
** conversion to an IN operator, the following must be true:
**
**     *  The left-hand side of the term must be the column which
**        is identified by iCursor and iColumn.
**
**     *  If the right-hand side is also a column, then the affinities
**        of both right and left sides must be such that no type
**        conversions are required on the right.  (Ticket #2249)
**
** If both of these conditions are true, then return true.  Otherwise
** return false.
*/
static int orTermIsOptCandidate(WhereTerm *pOrTerm, int iCursor, int iColumn){
  int affLeft, affRight;
  assert( pOrTerm->eOperator==WO_EQ );
  if( pOrTerm->leftCursor!=iCursor ){
    return 0;
  }
  if( pOrTerm->leftColumn!=iColumn ){
    return 0;
  }
  affRight = sqlite3ExprAffinity(pOrTerm->pExpr->pRight);
  if( affRight==0 ){
    return 1;
  }
  affLeft = sqlite3ExprAffinity(pOrTerm->pExpr->pLeft);
  if( affRight!=affLeft ){
    return 0;
  }
  return 1;
}

/*
** Return true if the given term of an OR clause can be ignored during
** a check to make sure all OR terms are candidates for optimization.
** In other words, return true if a call to the orTermIsOptCandidate()
** above returned false but it is not necessary to disqualify the
** optimization.
**
** Suppose the original OR phrase was this:
**
**           a=4  OR  a=11  OR  a=b
**
** During analysis, the third term gets flipped around and duplicate
** so that we are left with this:
**
**           a=4  OR  a=11  OR  a=b  OR  b=a
**
** Since the last two terms are duplicates, only one of them
** has to qualify in order for the whole phrase to qualify.  When
** this routine is called, we know that pOrTerm did not qualify.
** This routine merely checks to see if pOrTerm has a duplicate that
** might qualify.  If there is a duplicate that has not yet been
** disqualified, then return true.  If there are no duplicates, or
** the duplicate has also been disqualifed, return false.
*/
static int orTermHasOkDuplicate(WhereClause *pOr, WhereTerm *pOrTerm){
  if( pOrTerm->flags & TERM_COPIED ){
    /* This is the original term.  The duplicate is to the left had
    ** has not yet been analyzed and thus has not yet been disqualified. */
    return 1;
  }
  if( (pOrTerm->flags & TERM_VIRTUAL)!=0
     && (pOr->a[pOrTerm->iParent].flags & TERM_OR_OK)!=0 ){
    /* This is a duplicate term.  The original qualified so this one
    ** does not have to. */
    return 1;
  }
  /* This is either a singleton term or else it is a duplicate for
  ** which the original did not qualify.  Either way we are done for. */
  return 0;
}
#endif /* !SQLITE_OMIT_OR_OPTIMIZATION && !SQLITE_OMIT_SUBQUERY */

/*
** The input to this routine is an WhereTerm structure with only the
** "pExpr" field filled in.  The job of this routine is to analyze the
** subexpression and populate all the other fields of the WhereTerm
** structure.
**
** If the expression is of the form "<expr> <op> X" it gets commuted
** to the standard form of "X <op> <expr>".  If the expression is of
** the form "X <op> Y" where both X and Y are columns, then the original
** expression is unchanged and a new virtual expression of the form
** "Y <op> X" is added to the WHERE clause and analyzed separately.
*/
static void exprAnalyze(
  SrcList *pSrc,            /* the FROM clause */
  WhereClause *pWC,         /* the WHERE clause */
  int idxTerm               /* Index of the term to be analyzed */
){
  WhereTerm *pTerm = &pWC->a[idxTerm];
  ExprMaskSet *pMaskSet = pWC->pMaskSet;
  Expr *pExpr = pTerm->pExpr;
  Bitmask prereqLeft;
  Bitmask prereqAll;
  int nPattern;
  int isComplete;
  int op;

  if( sqlite3MallocFailed() ) return;
  prereqLeft = exprTableUsage(pMaskSet, pExpr->pLeft);
  op = pExpr->op;
  if( op==TK_IN ){
    assert( pExpr->pRight==0 );
    pTerm->prereqRight = exprListTableUsage(pMaskSet, pExpr->pList)
                          | exprSelectTableUsage(pMaskSet, pExpr->pSelect);
  }else if( op==TK_ISNULL ){
    pTerm->prereqRight = 0;
  }else{
    pTerm->prereqRight = exprTableUsage(pMaskSet, pExpr->pRight);
  }
  prereqAll = exprTableUsage(pMaskSet, pExpr);
  if( ExprHasProperty(pExpr, EP_FromJoin) ){
    prereqAll |= getMask(pMaskSet, pExpr->iRightJoinTable);
  }
  pTerm->prereqAll = prereqAll;
  pTerm->leftCursor = -1;
  pTerm->iParent = -1;
  pTerm->eOperator = 0;
  if( allowedOp(op) && (pTerm->prereqRight & prereqLeft)==0 ){
    Expr *pLeft = pExpr->pLeft;
    Expr *pRight = pExpr->pRight;
    if( pLeft->op==TK_COLUMN ){
      pTerm->leftCursor = pLeft->iTable;
      pTerm->leftColumn = pLeft->iColumn;
      pTerm->eOperator = operatorMask(op);
    }
    if( pRight && pRight->op==TK_COLUMN ){
      WhereTerm *pNew;
      Expr *pDup;
      if( pTerm->leftCursor>=0 ){
        int idxNew;
        pDup = sqlite3ExprDup(pExpr);
        if( sqlite3MallocFailed() ){
          sqlite3ExprDelete(pDup);
          return;
        }
        idxNew = whereClauseInsert(pWC, pDup, TERM_VIRTUAL|TERM_DYNAMIC);
        if( idxNew==0 ) return;
        pNew = &pWC->a[idxNew];
        pNew->iParent = idxTerm;
        pTerm = &pWC->a[idxTerm];
        pTerm->nChild = 1;
        pTerm->flags |= TERM_COPIED;
      }else{
        pDup = pExpr;
        pNew = pTerm;
      }
      exprCommute(pDup);
      pLeft = pDup->pLeft;
      pNew->leftCursor = pLeft->iTable;
      pNew->leftColumn = pLeft->iColumn;
      pNew->prereqRight = prereqLeft;
      pNew->prereqAll = prereqAll;
      pNew->eOperator = operatorMask(pDup->op);
    }
  }

#ifndef SQLITE_OMIT_BETWEEN_OPTIMIZATION
  /* If a term is the BETWEEN operator, create two new virtual terms
  ** that define the range that the BETWEEN implements.
  */
  else if( pExpr->op==TK_BETWEEN ){
    ExprList *pList = pExpr->pList;
    int i;
    static const u8 ops[] = {TK_GE, TK_LE};
    assert( pList!=0 );
    assert( pList->nExpr==2 );
    for(i=0; i<2; i++){
      Expr *pNewExpr;
      int idxNew;
      pNewExpr = sqlite3Expr(ops[i], sqlite3ExprDup(pExpr->pLeft),
                             sqlite3ExprDup(pList->a[i].pExpr), 0);
      idxNew = whereClauseInsert(pWC, pNewExpr, TERM_VIRTUAL|TERM_DYNAMIC);
      exprAnalyze(pSrc, pWC, idxNew);
      pTerm = &pWC->a[idxTerm];
      pWC->a[idxNew].iParent = idxTerm;
    }
    pTerm->nChild = 2;
  }
#endif /* SQLITE_OMIT_BETWEEN_OPTIMIZATION */

#if !defined(SQLITE_OMIT_OR_OPTIMIZATION) && !defined(SQLITE_OMIT_SUBQUERY)
  /* Attempt to convert OR-connected terms into an IN operator so that
  ** they can make use of indices.  Example:
  **
  **      x = expr1  OR  expr2 = x  OR  x = expr3
  **
  ** is converted into
  **
  **      x IN (expr1,expr2,expr3)
  **
  ** This optimization must be omitted if OMIT_SUBQUERY is defined because
  ** the compiler for the the IN operator is part of sub-queries.
  */
  else if( pExpr->op==TK_OR ){
    int ok;
    int i, j;
    int iColumn, iCursor;
    WhereClause sOr;
    WhereTerm *pOrTerm;

    assert( (pTerm->flags & TERM_DYNAMIC)==0 );
    whereClauseInit(&sOr, pWC->pParse, pMaskSet);
    whereSplit(&sOr, pExpr, TK_OR);
    exprAnalyzeAll(pSrc, &sOr);
    assert( sOr.nTerm>=2 );
    j = 0;
    do{
      assert( j<sOr.nTerm );
      iColumn = sOr.a[j].leftColumn;
      iCursor = sOr.a[j].leftCursor;
      ok = iCursor>=0;
      for(i=sOr.nTerm-1, pOrTerm=sOr.a; i>=0 && ok; i--, pOrTerm++){
        if( pOrTerm->eOperator!=WO_EQ ){
          goto or_not_possible;
        }
        if( orTermIsOptCandidate(pOrTerm, iCursor, iColumn) ){
          pOrTerm->flags |= TERM_OR_OK;
        }else if( orTermHasOkDuplicate(&sOr, pOrTerm) ){
          pOrTerm->flags &= ~TERM_OR_OK;
        }else{
          ok = 0;
        }
      }
    }while( !ok && (sOr.a[j++].flags & TERM_COPIED)!=0 && j<2 );
    if( ok ){
      ExprList *pList = 0;
      Expr *pNew, *pDup;
      Expr *pLeft = 0;
      for(i=sOr.nTerm-1, pOrTerm=sOr.a; i>=0 && ok; i--, pOrTerm++){
        if( (pOrTerm->flags & TERM_OR_OK)==0 ) continue;
        pDup = sqlite3ExprDup(pOrTerm->pExpr->pRight);
        pList = sqlite3ExprListAppend(pList, pDup, 0);
        pLeft = pOrTerm->pExpr->pLeft;
      }
      assert( pLeft!=0 );
      pDup = sqlite3ExprDup(pLeft);
      pNew = sqlite3Expr(TK_IN, pDup, 0, 0);
      if( pNew ){
        int idxNew;
        transferJoinMarkings(pNew, pExpr);
        pNew->pList = pList;
        idxNew = whereClauseInsert(pWC, pNew, TERM_VIRTUAL|TERM_DYNAMIC);
        exprAnalyze(pSrc, pWC, idxNew);
        pTerm = &pWC->a[idxTerm];
        pWC->a[idxNew].iParent = idxTerm;
        pTerm->nChild = 1;
      }else{
        sqlite3ExprListDelete(pList);
      }
    }
or_not_possible:
    whereClauseClear(&sOr);
  }
#endif /* SQLITE_OMIT_OR_OPTIMIZATION */

#ifndef SQLITE_OMIT_LIKE_OPTIMIZATION
  /* Add constraints to reduce the search space on a LIKE or GLOB
  ** operator.
  */
  if( isLikeOrGlob(pWC->pParse->db, pExpr, &nPattern, &isComplete) ){
    Expr *pLeft, *pRight;
    Expr *pStr1, *pStr2;
    Expr *pNewExpr1, *pNewExpr2;
    int idxNew1, idxNew2;

    pLeft = pExpr->pList->a[1].pExpr;
    pRight = pExpr->pList->a[0].pExpr;
    pStr1 = sqlite3Expr(TK_STRING, 0, 0, 0);
    if( pStr1 ){
      sqlite3TokenCopy(&pStr1->token, &pRight->token);
      pStr1->token.n = nPattern;
    }
    pStr2 = sqlite3ExprDup(pStr1);
    if( pStr2 ){
      assert( pStr2->token.dyn );
      ++*(u8*)&pStr2->token.z[nPattern-1];
    }
    pNewExpr1 = sqlite3Expr(TK_GE, sqlite3ExprDup(pLeft), pStr1, 0);
    idxNew1 = whereClauseInsert(pWC, pNewExpr1, TERM_VIRTUAL|TERM_DYNAMIC);
    exprAnalyze(pSrc, pWC, idxNew1);
    pNewExpr2 = sqlite3Expr(TK_LT, sqlite3ExprDup(pLeft), pStr2, 0);
    idxNew2 = whereClauseInsert(pWC, pNewExpr2, TERM_VIRTUAL|TERM_DYNAMIC);
    exprAnalyze(pSrc, pWC, idxNew2);
    pTerm = &pWC->a[idxTerm];
    if( isComplete ){
      pWC->a[idxNew1].iParent = idxTerm;
      pWC->a[idxNew2].iParent = idxTerm;
      pTerm->nChild = 2;
    }
  }
#endif /* SQLITE_OMIT_LIKE_OPTIMIZATION */

#ifndef SQLITE_OMIT_VIRTUALTABLE
  /* Add a WO_MATCH auxiliary term to the constraint set if the
  ** current expression is of the form:  column MATCH expr.
  ** This information is used by the xBestIndex methods of
  ** virtual tables.  The native query optimizer does not attempt
  ** to do anything with MATCH functions.
  */
  if( isMatchOfColumn(pExpr) ){
    int idxNew;
    Expr *pRight, *pLeft;
    WhereTerm *pNewTerm;
    Bitmask prereqColumn, prereqExpr;

    pRight = pExpr->pList->a[0].pExpr;
    pLeft = pExpr->pList->a[1].pExpr;
    prereqExpr = exprTableUsage(pMaskSet, pRight);
    prereqColumn = exprTableUsage(pMaskSet, pLeft);
    if( (prereqExpr & prereqColumn)==0 ){
      Expr *pNewExpr;
      pNewExpr = sqlite3Expr(TK_MATCH, 0, sqlite3ExprDup(pRight), 0);
      idxNew = whereClauseInsert(pWC, pNewExpr, TERM_VIRTUAL|TERM_DYNAMIC);
      pNewTerm = &pWC->a[idxNew];
      pNewTerm->prereqRight = prereqExpr;
      pNewTerm->leftCursor = pLeft->iTable;
      pNewTerm->leftColumn = pLeft->iColumn;
      pNewTerm->eOperator = WO_MATCH;
      pNewTerm->iParent = idxTerm;
      pTerm = &pWC->a[idxTerm];
      pTerm->nChild = 1;
      pTerm->flags |= TERM_COPIED;
      pNewTerm->prereqAll = pTerm->prereqAll;
    }
  }
#endif /* SQLITE_OMIT_VIRTUALTABLE */
}

/*
** Return TRUE if any of the expressions in pList->a[iFirst...] contain
** a reference to any table other than the iBase table.
*/
static int referencesOtherTables(
  ExprList *pList,          /* Search expressions in ths list */
  ExprMaskSet *pMaskSet,    /* Mapping from tables to bitmaps */
  int iFirst,               /* Be searching with the iFirst-th expression */
  int iBase                 /* Ignore references to this table */
){
  Bitmask allowed = ~getMask(pMaskSet, iBase);
  while( iFirst<pList->nExpr ){
    if( (exprTableUsage(pMaskSet, pList->a[iFirst++].pExpr)&allowed)!=0 ){
      return 1;
    }
  }
  return 0;
}


/*
** This routine decides if pIdx can be used to satisfy the ORDER BY
** clause.  If it can, it returns 1.  If pIdx cannot satisfy the
** ORDER BY clause, this routine returns 0.
**
** pOrderBy is an ORDER BY clause from a SELECT statement.  pTab is the
** left-most table in the FROM clause of that same SELECT statement and
** the table has a cursor number of "base".  pIdx is an index on pTab.
**
** nEqCol is the number of columns of pIdx that are used as equality
** constraints.  Any of these columns may be missing from the ORDER BY
** clause and the match can still be a success.
**
** All terms of the ORDER BY that match against the index must be either
** ASC or DESC.  (Terms of the ORDER BY clause past the end of a UNIQUE
** index do not need to satisfy this constraint.)  The *pbRev value is
** set to 1 if the ORDER BY clause is all DESC and it is set to 0 if
** the ORDER BY clause is all ASC.
*/
static int isSortingIndex(
  Parse *pParse,          /* Parsing context */
  ExprMaskSet *pMaskSet,  /* Mapping from table indices to bitmaps */
  Index *pIdx,            /* The index we are testing */
  int base,               /* Cursor number for the table to be sorted */
  ExprList *pOrderBy,     /* The ORDER BY clause */
  int nEqCol,             /* Number of index columns with == constraints */
  int *pbRev              /* Set to 1 if ORDER BY is DESC */
){
  int i, j;                       /* Loop counters */
  int sortOrder = 0;              /* XOR of index and ORDER BY sort direction */
  int nTerm;                      /* Number of ORDER BY terms */
  struct ExprList_item *pTerm;    /* A term of the ORDER BY clause */
  sqlite3 *db = pParse->db;

  assert( pOrderBy!=0 );
  nTerm = pOrderBy->nExpr;
  assert( nTerm>0 );

  /* Match terms of the ORDER BY clause against columns of
  ** the index.
  **
  ** Note that indices have pIdx->nColumn regular columns plus
  ** one additional column containing the rowid.  The rowid column
  ** of the index is also allowed to match against the ORDER BY
  ** clause.
  */
  for(i=j=0, pTerm=pOrderBy->a; j<nTerm && i<=pIdx->nColumn; i++){
    Expr *pExpr;       /* The expression of the ORDER BY pTerm */
    CollSeq *pColl;    /* The collating sequence of pExpr */
    int termSortOrder; /* Sort order for this term */
    int iColumn;       /* The i-th column of the index.  -1 for rowid */
    int iSortOrder;    /* 1 for DESC, 0 for ASC on the i-th index term */
    const char *zColl; /* Name of the collating sequence for i-th index term */

    pExpr = pTerm->pExpr;
    if( pExpr->op!=TK_COLUMN || pExpr->iTable!=base ){
      /* Can not use an index sort on anything that is not a column in the
      ** left-most table of the FROM clause */
      break;
    }
    pColl = sqlite3ExprCollSeq(pParse, pExpr);
    if( !pColl ){
      pColl = db->pDfltColl;
    }
    if( i<pIdx->nColumn ){
      iColumn = pIdx->aiColumn[i];
      if( iColumn==pIdx->pTable->iPKey ){
        iColumn = -1;
      }
      iSortOrder = pIdx->aSortOrder[i];
      zColl = pIdx->azColl[i];
    }else{
      iColumn = -1;
      iSortOrder = 0;
      zColl = pColl->zName;
    }
    if( pExpr->iColumn!=iColumn || sqlite3StrICmp(pColl->zName, zColl) ){
      /* Term j of the ORDER BY clause does not match column i of the index */
      if( i<nEqCol ){
        /* If an index column that is constrained by == fails to match an
        ** ORDER BY term, that is OK.  Just ignore that column of the index
        */
        continue;
      }else{
        /* If an index column fails to match and is not constrained by ==
        ** then the index cannot satisfy the ORDER BY constraint.
        */
        return 0;
      }
    }
    assert( pIdx->aSortOrder!=0 );
    assert( pTerm->sortOrder==0 || pTerm->sortOrder==1 );
    assert( iSortOrder==0 || iSortOrder==1 );
    termSortOrder = iSortOrder ^ pTerm->sortOrder;
    if( i>nEqCol ){
      if( termSortOrder!=sortOrder ){
        /* Indices can only be used if all ORDER BY terms past the
        ** equality constraints are all either DESC or ASC. */
        return 0;
      }
    }else{
      sortOrder = termSortOrder;
    }
    j++;
    pTerm++;
    if( iColumn<0 && !referencesOtherTables(pOrderBy, pMaskSet, j, base) ){
      /* If the indexed column is the primary key and everything matches
      ** so far and none of the ORDER BY terms to the right reference other
      ** tables in the join, then we are assured that the index can be used 
      ** to sort because the primary key is unique and so none of the other
      ** columns will make any difference
      */
      j = nTerm;
    }
  }

  *pbRev = sortOrder!=0;
  if( j>=nTerm ){
    /* All terms of the ORDER BY clause are covered by this index so
    ** this index can be used for sorting. */
    return 1;
  }
  if( pIdx->onError!=OE_None && i==pIdx->nColumn
      && !referencesOtherTables(pOrderBy, pMaskSet, j, base) ){
    /* All terms of this index match some prefix of the ORDER BY clause
    ** and the index is UNIQUE and no terms on the tail of the ORDER BY
    ** clause reference other tables in a join.  If this is all true then
    ** the order by clause is superfluous. */
    return 1;
  }
  return 0;
}

/*
** Check table to see if the ORDER BY clause in pOrderBy can be satisfied
** by sorting in order of ROWID.  Return true if so and set *pbRev to be
** true for reverse ROWID and false for forward ROWID order.
*/
static int sortableByRowid(
  int base,               /* Cursor number for table to be sorted */
  ExprList *pOrderBy,     /* The ORDER BY clause */
  ExprMaskSet *pMaskSet,  /* Mapping from tables to bitmaps */
  int *pbRev              /* Set to 1 if ORDER BY is DESC */
){
  Expr *p;

  assert( pOrderBy!=0 );
  assert( pOrderBy->nExpr>0 );
  p = pOrderBy->a[0].pExpr;
  if( p->op==TK_COLUMN && p->iTable==base && p->iColumn==-1
    && !referencesOtherTables(pOrderBy, pMaskSet, 1, base) ){
    *pbRev = pOrderBy->a[0].sortOrder;
    return 1;
  }
  return 0;
}

/*
** Prepare a crude estimate of the logarithm of the input value.
** The results need not be exact.  This is only used for estimating
** the total cost of performing operatings with O(logN) or O(NlogN)
** complexity.  Because N is just a guess, it is no great tragedy if
** logN is a little off.
*/
static double estLog(double N){
  double logN = 1;
  double x = 10;
  while( N>x ){
    logN += 1;
    x *= 10;
  }
  return logN;
}

/*
** Two routines for printing the content of an sqlite3_index_info
** structure.  Used for testing and debugging only.  If neither
** SQLITE_TEST or SQLITE_DEBUG are defined, then these routines
** are no-ops.
*/
#if !defined(SQLITE_OMIT_VIRTUALTABLE) && defined(SQLITE_DEBUG)
static void TRACE_IDX_INPUTS(sqlite3_index_info *p){
  int i;
  if( !sqlite3_where_trace ) return;
  for(i=0; i<p->nConstraint; i++){
    sqlite3DebugPrintf("  constraint[%d]: col=%d termid=%d op=%d usabled=%d\n",
       i,
       p->aConstraint[i].iColumn,
       p->aConstraint[i].iTermOffset,
       p->aConstraint[i].op,
       p->aConstraint[i].usable);
  }
  for(i=0; i<p->nOrderBy; i++){
    sqlite3DebugPrintf("  orderby[%d]: col=%d desc=%d\n",
       i,
       p->aOrderBy[i].iColumn,
       p->aOrderBy[i].desc);
  }
}
static void TRACE_IDX_OUTPUTS(sqlite3_index_info *p){
  int i;
  if( !sqlite3_where_trace ) return;
  for(i=0; i<p->nConstraint; i++){
    sqlite3DebugPrintf("  usage[%d]: argvIdx=%d omit=%d\n",
       i,
       p->aConstraintUsage[i].argvIndex,
       p->aConstraintUsage[i].omit);
  }
  sqlite3DebugPrintf("  idxNum=%d\n", p->idxNum);
  sqlite3DebugPrintf("  idxStr=%s\n", p->idxStr);
  sqlite3DebugPrintf("  orderByConsumed=%d\n", p->orderByConsumed);
  sqlite3DebugPrintf("  estimatedCost=%g\n", p->estimatedCost);
}
#else
#define TRACE_IDX_INPUTS(A)
#define TRACE_IDX_OUTPUTS(A)
#endif

#ifndef SQLITE_OMIT_VIRTUALTABLE
/*
** Compute the best index for a virtual table.
**
** The best index is computed by the xBestIndex method of the virtual
** table module.  This routine is really just a wrapper that sets up
** the sqlite3_index_info structure that is used to communicate with
** xBestIndex.
**
** In a join, this routine might be called multiple times for the
** same virtual table.  The sqlite3_index_info structure is created
** and initialized on the first invocation and reused on all subsequent
** invocations.  The sqlite3_index_info structure is also used when
** code is generated to access the virtual table.  The whereInfoDelete() 
** routine takes care of freeing the sqlite3_index_info structure after
** everybody has finished with it.
*/
static double bestVirtualIndex(
  Parse *pParse,                 /* The parsing context */
  WhereClause *pWC,              /* The WHERE clause */
  struct SrcList_item *pSrc,     /* The FROM clause term to search */
  Bitmask notReady,              /* Mask of cursors that are not available */
  ExprList *pOrderBy,            /* The order by clause */
  int orderByUsable,             /* True if we can potential sort */
  sqlite3_index_info **ppIdxInfo /* Index information passed to xBestIndex */
){
  Table *pTab = pSrc->pTab;
  sqlite3_index_info *pIdxInfo;
  struct sqlite3_index_constraint *pIdxCons;
  struct sqlite3_index_orderby *pIdxOrderBy;
  struct sqlite3_index_constraint_usage *pUsage;
  WhereTerm *pTerm;
  int i, j;
  int nOrderBy;
  int rc;

  /* If the sqlite3_index_info structure has not been previously
  ** allocated and initialized for this virtual table, then allocate
  ** and initialize it now
  */
  pIdxInfo = *ppIdxInfo;
  if( pIdxInfo==0 ){
    WhereTerm *pTerm;
    int nTerm;
    WHERETRACE(("Recomputing index info for %s...\n", pTab->zName));

    /* Count the number of possible WHERE clause constraints referring
    ** to this virtual table */
    for(i=nTerm=0, pTerm=pWC->a; i<pWC->nTerm; i++, pTerm++){
      if( pTerm->leftCursor != pSrc->iCursor ) continue;
      if( pTerm->eOperator==WO_IN ) continue;
      nTerm++;
    }

    /* If the ORDER BY clause contains only columns in the current 
    ** virtual table then allocate space for the aOrderBy part of
    ** the sqlite3_index_info structure.
    */
    nOrderBy = 0;
    if( pOrderBy ){
      for(i=0; i<pOrderBy->nExpr; i++){
        Expr *pExpr = pOrderBy->a[i].pExpr;
        if( pExpr->op!=TK_COLUMN || pExpr->iTable!=pSrc->iCursor ) break;
      }
      if( i==pOrderBy->nExpr ){
        nOrderBy = pOrderBy->nExpr;
      }
    }

    /* Allocate the sqlite3_index_info structure
    */
    pIdxInfo = sqliteMalloc( sizeof(*pIdxInfo)
                             + (sizeof(*pIdxCons) + sizeof(*pUsage))*nTerm
                             + sizeof(*pIdxOrderBy)*nOrderBy );
    if( pIdxInfo==0 ){
      sqlite3ErrorMsg(pParse, "out of memory");
      return 0.0;
    }
    *ppIdxInfo = pIdxInfo;

    /* Initialize the structure.  The sqlite3_index_info structure contains
    ** many fields that are declared "const" to prevent xBestIndex from
    ** changing them.  We have to do some funky casting in order to
    ** initialize those fields.
    */
    pIdxCons = (struct sqlite3_index_constraint*)&pIdxInfo[1];
    pIdxOrderBy = (struct sqlite3_index_orderby*)&pIdxCons[nTerm];
    pUsage = (struct sqlite3_index_constraint_usage*)&pIdxOrderBy[nOrderBy];
    *(int*)&pIdxInfo->nConstraint = nTerm;
    *(int*)&pIdxInfo->nOrderBy = nOrderBy;
    *(struct sqlite3_index_constraint**)&pIdxInfo->aConstraint = pIdxCons;
    *(struct sqlite3_index_orderby**)&pIdxInfo->aOrderBy = pIdxOrderBy;
    *(struct sqlite3_index_constraint_usage**)&pIdxInfo->aConstraintUsage =
                                                                     pUsage;

    for(i=j=0, pTerm=pWC->a; i<pWC->nTerm; i++, pTerm++){
      if( pTerm->leftCursor != pSrc->iCursor ) continue;
      if( pTerm->eOperator==WO_IN ) continue;
      pIdxCons[j].iColumn = pTerm->leftColumn;
      pIdxCons[j].iTermOffset = i;
      pIdxCons[j].op = pTerm->eOperator;
      /* The direct assignment in the previous line is possible only because
      ** the WO_ and SQLITE_INDEX_CONSTRAINT_ codes are identical.  The
      ** following asserts verify this fact. */
      assert( WO_EQ==SQLITE_INDEX_CONSTRAINT_EQ );
      assert( WO_LT==SQLITE_INDEX_CONSTRAINT_LT );
      assert( WO_LE==SQLITE_INDEX_CONSTRAINT_LE );
      assert( WO_GT==SQLITE_INDEX_CONSTRAINT_GT );
      assert( WO_GE==SQLITE_INDEX_CONSTRAINT_GE );
      assert( WO_MATCH==SQLITE_INDEX_CONSTRAINT_MATCH );
      assert( pTerm->eOperator & (WO_EQ|WO_LT|WO_LE|WO_GT|WO_GE|WO_MATCH) );
      j++;
    }
    for(i=0; i<nOrderBy; i++){
      Expr *pExpr = pOrderBy->a[i].pExpr;
      pIdxOrderBy[i].iColumn = pExpr->iColumn;
      pIdxOrderBy[i].desc = pOrderBy->a[i].sortOrder;
    }
  }

  /* At this point, the sqlite3_index_info structure that pIdxInfo points
  ** to will have been initialized, either during the current invocation or
  ** during some prior invocation.  Now we just have to customize the
  ** details of pIdxInfo for the current invocation and pass it to
  ** xBestIndex.
  */

  /* The module name must be defined. Also, by this point there must
  ** be a pointer to an sqlite3_vtab structure. Otherwise
  ** sqlite3ViewGetColumnNames() would have picked up the error. 
  */
  assert( pTab->azModuleArg && pTab->azModuleArg[0] );
  assert( pTab->pVtab );
#if 0
  if( pTab->pVtab==0 ){
    sqlite3ErrorMsg(pParse, "undefined module %s for table %s",
        pTab->azModuleArg[0], pTab->zName);
    return 0.0;
  }
#endif

  /* Set the aConstraint[].usable fields and initialize all 
  ** output variables to zero.
  **
  ** aConstraint[].usable is true for constraints where the right-hand
  ** side contains only references to tables to the left of the current
  ** table.  In other words, if the constraint is of the form:
  **
  **           column = expr
  **
  ** and we are evaluating a join, then the constraint on column is 
  ** only valid if all tables referenced in expr occur to the left
  ** of the table containing column.
  **
  ** The aConstraints[] array contains entries for all constraints
  ** on the current table.  That way we only have to compute it once
  ** even though we might try to pick the best index multiple times.
  ** For each attempt at picking an index, the order of tables in the
  ** join might be different so we have to recompute the usable flag
  ** each time.
  */
  pIdxCons = *(struct sqlite3_index_constraint**)&pIdxInfo->aConstraint;
  pUsage = pIdxInfo->aConstraintUsage;
  for(i=0; i<pIdxInfo->nConstraint; i++, pIdxCons++){
    j = pIdxCons->iTermOffset;
    pTerm = &pWC->a[j];
    pIdxCons->usable =  (pTerm->prereqRight & notReady)==0;
  }
  memset(pUsage, 0, sizeof(pUsage[0])*pIdxInfo->nConstraint);
  if( pIdxInfo->needToFreeIdxStr ){
    sqlite3_free(pIdxInfo->idxStr);
  }
  pIdxInfo->idxStr = 0;
  pIdxInfo->idxNum = 0;
  pIdxInfo->needToFreeIdxStr = 0;
  pIdxInfo->orderByConsumed = 0;
  pIdxInfo->estimatedCost = SQLITE_BIG_DBL / 2.0;
  nOrderBy = pIdxInfo->nOrderBy;
  if( pIdxInfo->nOrderBy && !orderByUsable ){
    *(int*)&pIdxInfo->nOrderBy = 0;
  }

  sqlite3SafetyOff(pParse->db);
  WHERETRACE(("xBestIndex for %s\n", pTab->zName));
  TRACE_IDX_INPUTS(pIdxInfo);
  rc = pTab->pVtab->pModule->xBestIndex(pTab->pVtab, pIdxInfo);
  TRACE_IDX_OUTPUTS(pIdxInfo);
  if( rc!=SQLITE_OK ){
    if( rc==SQLITE_NOMEM ){
      sqlite3FailedMalloc();
    }else {
      sqlite3ErrorMsg(pParse, "%s", sqlite3ErrStr(rc));
    }
    sqlite3SafetyOn(pParse->db);
  }else{
    rc = sqlite3SafetyOn(pParse->db);
  }
  *(int*)&pIdxInfo->nOrderBy = nOrderBy;

  return pIdxInfo->estimatedCost;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

/*
** Find the best index for accessing a particular table.  Return a pointer
** to the index, flags that describe how the index should be used, the
** number of equality constraints, and the "cost" for this index.
**
** The lowest cost index wins.  The cost is an estimate of the amount of
** CPU and disk I/O need to process the request using the selected index.
** Factors that influence cost include:
**
**    *  The estimated number of rows that will be retrieved.  (The
**       fewer the better.)
**
**    *  Whether or not sorting must occur.
**
**    *  Whether or not there must be separate lookups in the
**       index and in the main table.
**
*/
static double bestIndex(
  Parse *pParse,              /* The parsing context */
  WhereClause *pWC,           /* The WHERE clause */
  struct SrcList_item *pSrc,  /* The FROM clause term to search */
  Bitmask notReady,           /* Mask of cursors that are not available */
  ExprList *pOrderBy,         /* The order by clause */
  Index **ppIndex,            /* Make *ppIndex point to the best index */
  int *pFlags,                /* Put flags describing this choice in *pFlags */
  int *pnEq                   /* Put the number of == or IN constraints here */
){
  WhereTerm *pTerm;
  Index *bestIdx = 0;         /* Index that gives the lowest cost */
  double lowestCost;          /* The cost of using bestIdx */
  int bestFlags = 0;          /* Flags associated with bestIdx */
  int bestNEq = 0;            /* Best value for nEq */
  int iCur = pSrc->iCursor;   /* The cursor of the table to be accessed */
  Index *pProbe;              /* An index we are evaluating */
  int rev;                    /* True to scan in reverse order */
  int flags;                  /* Flags associated with pProbe */
  int nEq;                    /* Number of == or IN constraints */
  int eqTermMask;             /* Mask of valid equality operators */
  double cost;                /* Cost of using pProbe */

  WHERETRACE(("bestIndex: tbl=%s notReady=%x\n", pSrc->pTab->zName, notReady));
  lowestCost = SQLITE_BIG_DBL;
  pProbe = pSrc->pTab->pIndex;

  /* If the table has no indices and there are no terms in the where
  ** clause that refer to the ROWID, then we will never be able to do
  ** anything other than a full table scan on this table.  We might as
  ** well put it first in the join order.  That way, perhaps it can be
  ** referenced by other tables in the join.
  */
  if( pProbe==0 &&
     findTerm(pWC, iCur, -1, 0, WO_EQ|WO_IN|WO_LT|WO_LE|WO_GT|WO_GE,0)==0 &&
     (pOrderBy==0 || !sortableByRowid(iCur, pOrderBy, pWC->pMaskSet, &rev)) ){
    *pFlags = 0;
    *ppIndex = 0;
    *pnEq = 0;
    return 0.0;
  }

  /* Check for a rowid=EXPR or rowid IN (...) constraints
  */
  pTerm = findTerm(pWC, iCur, -1, notReady, WO_EQ|WO_IN, 0);
  if( pTerm ){
    Expr *pExpr;
    *ppIndex = 0;
    bestFlags = WHERE_ROWID_EQ;
    if( pTerm->eOperator & WO_EQ ){
      /* Rowid== is always the best pick.  Look no further.  Because only
      ** a single row is generated, output is always in sorted order */
      *pFlags = WHERE_ROWID_EQ | WHERE_UNIQUE;
      *pnEq = 1;
      WHERETRACE(("... best is rowid\n"));
      return 0.0;
    }else if( (pExpr = pTerm->pExpr)->pList!=0 ){
      /* Rowid IN (LIST): cost is NlogN where N is the number of list
      ** elements.  */
      lowestCost = pExpr->pList->nExpr;
      lowestCost *= estLog(lowestCost);
    }else{
      /* Rowid IN (SELECT): cost is NlogN where N is the number of rows
      ** in the result of the inner select.  We have no way to estimate
      ** that value so make a wild guess. */
      lowestCost = 200;
    }
    WHERETRACE(("... rowid IN cost: %.9g\n", lowestCost));
  }

  /* Estimate the cost of a table scan.  If we do not know how many
  ** entries are in the table, use 1 million as a guess.
  */
  cost = pProbe ? pProbe->aiRowEst[0] : 1000000;
  WHERETRACE(("... table scan base cost: %.9g\n", cost));
  flags = WHERE_ROWID_RANGE;

  /* Check for constraints on a range of rowids in a table scan.
  */
  pTerm = findTerm(pWC, iCur, -1, notReady, WO_LT|WO_LE|WO_GT|WO_GE, 0);
  if( pTerm ){
    if( findTerm(pWC, iCur, -1, notReady, WO_LT|WO_LE, 0) ){
      flags |= WHERE_TOP_LIMIT;
      cost /= 3;  /* Guess that rowid<EXPR eliminates two-thirds or rows */
    }
    if( findTerm(pWC, iCur, -1, notReady, WO_GT|WO_GE, 0) ){
      flags |= WHERE_BTM_LIMIT;
      cost /= 3;  /* Guess that rowid>EXPR eliminates two-thirds of rows */
    }
    WHERETRACE(("... rowid range reduces cost to %.9g\n", cost));
  }else{
    flags = 0;
  }

  /* If the table scan does not satisfy the ORDER BY clause, increase
  ** the cost by NlogN to cover the expense of sorting. */
  if( pOrderBy ){
    if( sortableByRowid(iCur, pOrderBy, pWC->pMaskSet, &rev) ){
      flags |= WHERE_ORDERBY|WHERE_ROWID_RANGE;
      if( rev ){
        flags |= WHERE_REVERSE;
      }
    }else{
      cost += cost*estLog(cost);
      WHERETRACE(("... sorting increases cost to %.9g\n", cost));
    }
  }
  if( cost<lowestCost ){
    lowestCost = cost;
    bestFlags = flags;
  }

  /* If the pSrc table is the right table of a LEFT JOIN then we may not
  ** use an index to satisfy IS NULL constraints on that table.  This is
  ** because columns might end up being NULL if the table does not match -
  ** a circumstance which the index cannot help us discover.  Ticket #2177.
  */
  if( (pSrc->jointype & JT_LEFT)!=0 ){
    eqTermMask = WO_EQ|WO_IN;
  }else{
    eqTermMask = WO_EQ|WO_IN|WO_ISNULL;
  }

  /* Look at each index.
  */
  for(; pProbe; pProbe=pProbe->pNext){
    int i;                       /* Loop counter */
    double inMultiplier = 1;

    WHERETRACE(("... index %s:\n", pProbe->zName));

    /* Count the number of columns in the index that are satisfied
    ** by x=EXPR constraints or x IN (...) constraints.
    */
    flags = 0;
    for(i=0; i<pProbe->nColumn; i++){
      int j = pProbe->aiColumn[i];
      pTerm = findTerm(pWC, iCur, j, notReady, eqTermMask, pProbe);
      if( pTerm==0 ) break;
      flags |= WHERE_COLUMN_EQ;
      if( pTerm->eOperator & WO_IN ){
        Expr *pExpr = pTerm->pExpr;
        flags |= WHERE_COLUMN_IN;
        if( pExpr->pSelect!=0 ){
          inMultiplier *= 25;
        }else if( pExpr->pList!=0 ){
          inMultiplier *= pExpr->pList->nExpr + 1;
        }
      }
    }
    cost = pProbe->aiRowEst[i] * inMultiplier * estLog(inMultiplier);
    nEq = i;
    if( pProbe->onError!=OE_None && (flags & WHERE_COLUMN_IN)==0
         && nEq==pProbe->nColumn ){
      flags |= WHERE_UNIQUE;
    }
    WHERETRACE(("...... nEq=%d inMult=%.9g cost=%.9g\n", nEq, inMultiplier, cost));

    /* Look for range constraints
    */
    if( nEq<pProbe->nColumn ){
      int j = pProbe->aiColumn[nEq];
      pTerm = findTerm(pWC, iCur, j, notReady, WO_LT|WO_LE|WO_GT|WO_GE, pProbe);
      if( pTerm ){
        flags |= WHERE_COLUMN_RANGE;
        if( findTerm(pWC, iCur, j, notReady, WO_LT|WO_LE, pProbe) ){
          flags |= WHERE_TOP_LIMIT;
          cost /= 3;
        }
        if( findTerm(pWC, iCur, j, notReady, WO_GT|WO_GE, pProbe) ){
          flags |= WHERE_BTM_LIMIT;
          cost /= 3;
        }
        WHERETRACE(("...... range reduces cost to %.9g\n", cost));
      }
    }

    /* Add the additional cost of sorting if that is a factor.
    */
    if( pOrderBy ){
      if( (flags & WHERE_COLUMN_IN)==0 &&
           isSortingIndex(pParse,pWC->pMaskSet,pProbe,iCur,pOrderBy,nEq,&rev) ){
        if( flags==0 ){
          flags = WHERE_COLUMN_RANGE;
        }
        flags |= WHERE_ORDERBY;
        if( rev ){
          flags |= WHERE_REVERSE;
        }
      }else{
        cost += cost*estLog(cost);
        WHERETRACE(("...... orderby increases cost to %.9g\n", cost));
      }
    }

    /* Check to see if we can get away with using just the index without
    ** ever reading the table.  If that is the case, then halve the
    ** cost of this index.
    */
    if( flags && pSrc->colUsed < (((Bitmask)1)<<(BMS-1)) ){
      Bitmask m = pSrc->colUsed;
      int j;
      for(j=0; j<pProbe->nColumn; j++){
        int x = pProbe->aiColumn[j];
        if( x<BMS-1 ){
          m &= ~(((Bitmask)1)<<x);
        }
      }
      if( m==0 ){
        flags |= WHERE_IDX_ONLY;
        cost /= 2;
        WHERETRACE(("...... idx-only reduces cost to %.9g\n", cost));
      }
    }

    /* If this index has achieved the lowest cost so far, then use it.
    */
    if( cost < lowestCost ){
      bestIdx = pProbe;
      lowestCost = cost;
      assert( flags!=0 );
      bestFlags = flags;
      bestNEq = nEq;
    }
  }

  /* Report the best result
  */
  *ppIndex = bestIdx;
  WHERETRACE(("best index is %s, cost=%.9g, flags=%x, nEq=%d\n",
        bestIdx ? bestIdx->zName : "(none)", lowestCost, bestFlags, bestNEq));
  *pFlags = bestFlags | eqTermMask;
  *pnEq = bestNEq;
  return lowestCost;
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
** The t2.z='ok' is disabled in the in (2) because it originates
** in the ON clause.  The term is disabled in (3) because it is not part
** of a LEFT OUTER JOIN.  In (1), the term is not disabled.
**
** Disabling a term causes that term to not be tested in the inner loop
** of the join.  Disabling is an optimization.  When terms are satisfied
** by indices, we disable them to prevent redundant tests in the inner
** loop.  We would get the correct results if nothing were ever disabled,
** but joins might run a little slower.  The trick is to disable as much
** as we can without disabling too much.  If we disabled in (1), we'd get
** the wrong answer.  See ticket #813.
*/
static void disableTerm(WhereLevel *pLevel, WhereTerm *pTerm){
  if( pTerm
      && (pTerm->flags & TERM_CODED)==0
      && (pLevel->iLeftJoin==0 || ExprHasProperty(pTerm->pExpr, EP_FromJoin))
  ){
    pTerm->flags |= TERM_CODED;
    if( pTerm->iParent>=0 ){
      WhereTerm *pOther = &pTerm->pWC->a[pTerm->iParent];
      if( (--pOther->nChild)==0 ){
        disableTerm(pLevel, pOther);
      }
    }
  }
}

/*
** Generate code that builds a probe for an index.
**
** There should be nColumn values on the stack.  The index
** to be probed is pIdx.  Pop the values from the stack and
** replace them all with a single record that is the index
** problem.
*/
static void buildIndexProbe(
  Vdbe *v,        /* Generate code into this VM */
  int nColumn,    /* The number of columns to check for NULL */
  Index *pIdx     /* Index that we will be searching */
){
  sqlite3VdbeAddOp(v, OP_MakeRecord, nColumn, 0);
  sqlite3IndexAffinityStr(v, pIdx);
}


/*
** Generate code for a single equality term of the WHERE clause.  An equality
** term can be either X=expr or X IN (...).   pTerm is the term to be 
** coded.
**
** The current value for the constraint is left on the top of the stack.
**
** For a constraint of the form X=expr, the expression is evaluated and its
** result is left on the stack.  For constraints of the form X IN (...)
** this routine sets up a loop that will iterate over all values of X.
*/
static void codeEqualityTerm(
  Parse *pParse,      /* The parsing context */
  WhereTerm *pTerm,   /* The term of the WHERE clause to be coded */
  WhereLevel *pLevel  /* When level of the FROM clause we are working on */
){
  Expr *pX = pTerm->pExpr;
  Vdbe *v = pParse->pVdbe;
  if( pX->op==TK_EQ ){
    sqlite3ExprCode(pParse, pX->pRight);
  }else if( pX->op==TK_ISNULL ){
    sqlite3VdbeAddOp(v, OP_Null, 0, 0);
#ifndef SQLITE_OMIT_SUBQUERY
  }else{
    int iTab;
    struct InLoop *pIn;

    assert( pX->op==TK_IN );
    sqlite3CodeSubselect(pParse, pX);
    iTab = pX->iTable;
    sqlite3VdbeAddOp(v, OP_Rewind, iTab, 0);
    VdbeComment((v, "# %.*s", pX->span.n, pX->span.z));
    if( pLevel->nIn==0 ){
      pLevel->nxt = sqlite3VdbeMakeLabel(v);
    }
    pLevel->nIn++;
    pLevel->aInLoop = sqliteReallocOrFree(pLevel->aInLoop,
                                    sizeof(pLevel->aInLoop[0])*pLevel->nIn);
    pIn = pLevel->aInLoop;
    if( pIn ){
      pIn += pLevel->nIn - 1;
      pIn->iCur = iTab;
      pIn->topAddr = sqlite3VdbeAddOp(v, OP_Column, iTab, 0);
      sqlite3VdbeAddOp(v, OP_IsNull, -1, 0);
    }else{
      pLevel->nIn = 0;
    }
#endif
  }
  disableTerm(pLevel, pTerm);
}

/*
** Generate code that will evaluate all == and IN constraints for an
** index.  The values for all constraints are left on the stack.
**
** For example, consider table t1(a,b,c,d,e,f) with index i1(a,b,c).
** Suppose the WHERE clause is this:  a==5 AND b IN (1,2,3) AND c>5 AND c<10
** The index has as many as three equality constraints, but in this
** example, the third "c" value is an inequality.  So only two 
** constraints are coded.  This routine will generate code to evaluate
** a==5 and b IN (1,2,3).  The current values for a and b will be left
** on the stack - a is the deepest and b the shallowest.
**
** In the example above nEq==2.  But this subroutine works for any value
** of nEq including 0.  If nEq==0, this routine is nearly a no-op.
** The only thing it does is allocate the pLevel->iMem memory cell.
**
** This routine always allocates at least one memory cell and puts
** the address of that memory cell in pLevel->iMem.  The code that
** calls this routine will use pLevel->iMem to store the termination
** key value of the loop.  If one or more IN operators appear, then
** this routine allocates an additional nEq memory cells for internal
** use.
*/
static void codeAllEqualityTerms(
  Parse *pParse,        /* Parsing context */
  WhereLevel *pLevel,   /* Which nested loop of the FROM we are coding */
  WhereClause *pWC,     /* The WHERE clause */
  Bitmask notReady      /* Which parts of FROM have not yet been coded */
){
  int nEq = pLevel->nEq;        /* The number of == or IN constraints to code */
  int termsInMem = 0;           /* If true, store value in mem[] cells */
  Vdbe *v = pParse->pVdbe;      /* The virtual machine under construction */
  Index *pIdx = pLevel->pIdx;   /* The index being used for this loop */
  int iCur = pLevel->iTabCur;   /* The cursor of the table */
  WhereTerm *pTerm;             /* A single constraint term */
  int j;                        /* Loop counter */

  /* Figure out how many memory cells we will need then allocate them.
  ** We always need at least one used to store the loop terminator
  ** value.  If there are IN operators we'll need one for each == or
  ** IN constraint.
  */
  pLevel->iMem = pParse->nMem++;
  if( pLevel->flags & WHERE_COLUMN_IN ){
    pParse->nMem += pLevel->nEq;
    termsInMem = 1;
  }

  /* Evaluate the equality constraints
  */
  assert( pIdx->nColumn>=nEq );
  for(j=0; j<nEq; j++){
    int k = pIdx->aiColumn[j];
    pTerm = findTerm(pWC, iCur, k, notReady, pLevel->flags, pIdx);
    if( pTerm==0 ) break;
    assert( (pTerm->flags & TERM_CODED)==0 );
    codeEqualityTerm(pParse, pTerm, pLevel);
    if( (pTerm->eOperator & (WO_ISNULL|WO_IN))==0 ){
      sqlite3VdbeAddOp(v, OP_IsNull, termsInMem ? -1 : -(j+1), pLevel->brk);
    }
    if( termsInMem ){
      sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem+j+1, 1);
    }
  }

  /* Make sure all the constraint values are on the top of the stack
  */
  if( termsInMem ){
    for(j=0; j<nEq; j++){
      sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem+j+1, 0);
    }
  }
}

#if defined(SQLITE_TEST)
/*
** The following variable holds a text description of query plan generated
** by the most recent call to sqlite3WhereBegin().  Each call to WhereBegin
** overwrites the previous.  This information is used for testing and
** analysis only.
*/
char sqlite3_query_plan[BMS*2*40];  /* Text of the join */
static int nQPlan = 0;              /* Next free slow in _query_plan[] */

#endif /* SQLITE_TEST */


/*
** Free a WhereInfo structure
*/
static void whereInfoFree(WhereInfo *pWInfo){
  if( pWInfo ){
    int i;
    for(i=0; i<pWInfo->nLevel; i++){
      sqlite3_index_info *pInfo = pWInfo->a[i].pIdxInfo;
      if( pInfo ){
        if( pInfo->needToFreeIdxStr ){
          /* Coverage: Don't think this can be reached. By the time this
          ** function is called, the index-strings have been passed
          ** to the vdbe layer for deletion.
          */
          sqlite3_free(pInfo->idxStr);
        }
        sqliteFree(pInfo);
      }
    }
    sqliteFree(pWInfo);
  }
}


/*
** Generate the beginning of the loop used for WHERE clause processing.
** The return value is a pointer to an opaque structure that contains
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
** Note that the loops might not be nested in the order in which they
** appear in the FROM clause if a different order is better able to make
** use of indices.  Note also that when the IN operator appears in
** the WHERE clause, it might result in additional nested loops for
** scanning through all values on the right-hand side of the IN.
**
** There are Btree cursors associated with each table.  t1 uses cursor
** number pTabList->a[0].iCursor.  t2 uses the cursor pTabList->a[1].iCursor.
** And so forth.  This routine generates code to open those VDBE cursors
** and sqlite3WhereEnd() generates the code to close them.
**
** The code that sqlite3WhereBegin() generates leaves the cursors named
** in pTabList pointing at their appropriate entries.  The [...] code
** can use OP_Column and OP_Rowid opcodes on these cursors to extract
** data from the various tables of the loop.
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
  Parse *pParse,        /* The parser context */
  SrcList *pTabList,    /* A list of all tables to be scanned */
  Expr *pWhere,         /* The WHERE clause */
  ExprList **ppOrderBy  /* An ORDER BY clause, or NULL */
){
  int i;                     /* Loop counter */
  WhereInfo *pWInfo;         /* Will become the return value of this function */
  Vdbe *v = pParse->pVdbe;   /* The virtual database engine */
  int brk, cont = 0;         /* Addresses used during code generation */
  Bitmask notReady;          /* Cursors that are not yet positioned */
  WhereTerm *pTerm;          /* A single term in the WHERE clause */
  ExprMaskSet maskSet;       /* The expression mask set */
  WhereClause wc;            /* The WHERE clause is divided into these terms */
  struct SrcList_item *pTabItem;  /* A single entry from pTabList */
  WhereLevel *pLevel;             /* A single level in the pWInfo list */
  int iFrom;                      /* First unused FROM clause element */
  int andFlags;              /* AND-ed combination of all wc.a[].flags */

  /* The number of tables in the FROM clause is limited by the number of
  ** bits in a Bitmask 
  */
  if( pTabList->nSrc>BMS ){
    sqlite3ErrorMsg(pParse, "at most %d tables in a join", BMS);
    return 0;
  }

  /* Split the WHERE clause into separate subexpressions where each
  ** subexpression is separated by an AND operator.
  */
  initMaskSet(&maskSet);
  whereClauseInit(&wc, pParse, &maskSet);
  whereSplit(&wc, pWhere, TK_AND);
    
  /* Allocate and initialize the WhereInfo structure that will become the
  ** return value.
  */
  pWInfo = sqliteMalloc( sizeof(WhereInfo) + pTabList->nSrc*sizeof(WhereLevel));
  if( sqlite3MallocFailed() ){
    goto whereBeginNoMem;
  }
  pWInfo->nLevel = pTabList->nSrc;
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

  /* Analyze all of the subexpressions.  Note that exprAnalyze() might
  ** add new virtual terms onto the end of the WHERE clause.  We do not
  ** want to analyze these virtual terms, so start analyzing at the end
  ** and work forward so that the added virtual terms are never processed.
  */
  for(i=0; i<pTabList->nSrc; i++){
    createMask(&maskSet, pTabList->a[i].iCursor);
  }
  exprAnalyzeAll(pTabList, &wc);
  if( sqlite3MallocFailed() ){
    goto whereBeginNoMem;
  }

  /* Chose the best index to use for each table in the FROM clause.
  **
  ** This loop fills in the following fields:
  **
  **   pWInfo->a[].pIdx      The index to use for this level of the loop.
  **   pWInfo->a[].flags     WHERE_xxx flags associated with pIdx
  **   pWInfo->a[].nEq       The number of == and IN constraints
  **   pWInfo->a[].iFrom     When term of the FROM clause is being coded
  **   pWInfo->a[].iTabCur   The VDBE cursor for the database table
  **   pWInfo->a[].iIdxCur   The VDBE cursor for the index
  **
  ** This loop also figures out the nesting order of tables in the FROM
  ** clause.
  */
  notReady = ~(Bitmask)0;
  pTabItem = pTabList->a;
  pLevel = pWInfo->a;
  andFlags = ~0;
  WHERETRACE(("*** Optimizer Start ***\n"));
  for(i=iFrom=0, pLevel=pWInfo->a; i<pTabList->nSrc; i++, pLevel++){
    Index *pIdx;                /* Index for FROM table at pTabItem */
    int flags;                  /* Flags asssociated with pIdx */
    int nEq;                    /* Number of == or IN constraints */
    double cost;                /* The cost for pIdx */
    int j;                      /* For looping over FROM tables */
    Index *pBest = 0;           /* The best index seen so far */
    int bestFlags = 0;          /* Flags associated with pBest */
    int bestNEq = 0;            /* nEq associated with pBest */
    double lowestCost;          /* Cost of the pBest */
    int bestJ = 0;              /* The value of j */
    Bitmask m;                  /* Bitmask value for j or bestJ */
    int once = 0;               /* True when first table is seen */
    sqlite3_index_info *pIndex; /* Current virtual index */

    lowestCost = SQLITE_BIG_DBL;
    for(j=iFrom, pTabItem=&pTabList->a[j]; j<pTabList->nSrc; j++, pTabItem++){
      int doNotReorder;  /* True if this table should not be reordered */

      doNotReorder =  (pTabItem->jointype & (JT_LEFT|JT_CROSS))!=0;
      if( once && doNotReorder ) break;
      m = getMask(&maskSet, pTabItem->iCursor);
      if( (m & notReady)==0 ){
        if( j==iFrom ) iFrom++;
        continue;
      }
      assert( pTabItem->pTab );
#ifndef SQLITE_OMIT_VIRTUALTABLE
      if( IsVirtual(pTabItem->pTab) ){
        sqlite3_index_info **ppIdxInfo = &pWInfo->a[j].pIdxInfo;
        cost = bestVirtualIndex(pParse, &wc, pTabItem, notReady,
                                ppOrderBy ? *ppOrderBy : 0, i==0,
                                ppIdxInfo);
        flags = WHERE_VIRTUALTABLE;
        pIndex = *ppIdxInfo;
        if( pIndex && pIndex->orderByConsumed ){
          flags = WHERE_VIRTUALTABLE | WHERE_ORDERBY;
        }
        pIdx = 0;
        nEq = 0;
        if( (SQLITE_BIG_DBL/2.0)<cost ){
          /* The cost is not allowed to be larger than SQLITE_BIG_DBL (the
          ** inital value of lowestCost in this loop. If it is, then
          ** the (cost<lowestCost) test below will never be true and
          ** pLevel->pBestIdx never set.
          */ 
          cost = (SQLITE_BIG_DBL/2.0);
        }
      }else 
#endif
      {
        cost = bestIndex(pParse, &wc, pTabItem, notReady,
                         (i==0 && ppOrderBy) ? *ppOrderBy : 0,
                         &pIdx, &flags, &nEq);
        pIndex = 0;
      }
      if( cost<lowestCost ){
        once = 1;
        lowestCost = cost;
        pBest = pIdx;
        bestFlags = flags;
        bestNEq = nEq;
        bestJ = j;
        pLevel->pBestIdx = pIndex;
      }
      if( doNotReorder ) break;
    }
    WHERETRACE(("*** Optimizer choose table %d for loop %d\n", bestJ,
           pLevel-pWInfo->a));
    if( (bestFlags & WHERE_ORDERBY)!=0 ){
      *ppOrderBy = 0;
    }
    andFlags &= bestFlags;
    pLevel->flags = bestFlags;
    pLevel->pIdx = pBest;
    pLevel->nEq = bestNEq;
    pLevel->aInLoop = 0;
    pLevel->nIn = 0;
    if( pBest ){
      pLevel->iIdxCur = pParse->nTab++;
    }else{
      pLevel->iIdxCur = -1;
    }
    notReady &= ~getMask(&maskSet, pTabList->a[bestJ].iCursor);
    pLevel->iFrom = bestJ;
  }
  WHERETRACE(("*** Optimizer Finished ***\n"));

  /* If the total query only selects a single row, then the ORDER BY
  ** clause is irrelevant.
  */
  if( (andFlags & WHERE_UNIQUE)!=0 && ppOrderBy ){
    *ppOrderBy = 0;
  }

  /* Open all tables in the pTabList and any indices selected for
  ** searching those tables.
  */
  sqlite3CodeVerifySchema(pParse, -1); /* Insert the cookie verifier Goto */
  for(i=0, pLevel=pWInfo->a; i<pTabList->nSrc; i++, pLevel++){
    Table *pTab;     /* Table to open */
    Index *pIx;      /* Index used to access pTab (if any) */
    int iDb;         /* Index of database containing table/index */
    int iIdxCur = pLevel->iIdxCur;

#ifndef SQLITE_OMIT_EXPLAIN
    if( pParse->explain==2 ){
      char *zMsg;
      struct SrcList_item *pItem = &pTabList->a[pLevel->iFrom];
      zMsg = sqlite3MPrintf("TABLE %s", pItem->zName);
      if( pItem->zAlias ){
        zMsg = sqlite3MPrintf("%z AS %s", zMsg, pItem->zAlias);
      }
      if( (pIx = pLevel->pIdx)!=0 ){
        zMsg = sqlite3MPrintf("%z WITH INDEX %s", zMsg, pIx->zName);
      }else if( pLevel->flags & (WHERE_ROWID_EQ|WHERE_ROWID_RANGE) ){
        zMsg = sqlite3MPrintf("%z USING PRIMARY KEY", zMsg);
      }
#ifndef SQLITE_OMIT_VIRTUALTABLE
      else if( pLevel->pBestIdx ){
        sqlite3_index_info *pBestIdx = pLevel->pBestIdx;
        zMsg = sqlite3MPrintf("%z VIRTUAL TABLE INDEX %d:%s", zMsg,
                    pBestIdx->idxNum, pBestIdx->idxStr);
      }
#endif
      if( pLevel->flags & WHERE_ORDERBY ){
        zMsg = sqlite3MPrintf("%z ORDER BY", zMsg);
      }
      sqlite3VdbeOp3(v, OP_Explain, i, pLevel->iFrom, zMsg, P3_DYNAMIC);
    }
#endif /* SQLITE_OMIT_EXPLAIN */
    pTabItem = &pTabList->a[pLevel->iFrom];
    pTab = pTabItem->pTab;
    iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
    if( pTab->isEphem || pTab->pSelect ) continue;
#ifndef SQLITE_OMIT_VIRTUALTABLE
    if( pLevel->pBestIdx ){
      int iCur = pTabItem->iCursor;
      sqlite3VdbeOp3(v, OP_VOpen, iCur, 0, (const char*)pTab->pVtab, P3_VTAB);
    }else
#endif
    if( (pLevel->flags & WHERE_IDX_ONLY)==0 ){
      sqlite3OpenTable(pParse, pTabItem->iCursor, iDb, pTab, OP_OpenRead);
      if( pTab->nCol<(sizeof(Bitmask)*8) ){
        Bitmask b = pTabItem->colUsed;
        int n = 0;
        for(; b; b=b>>1, n++){}
        sqlite3VdbeChangeP2(v, sqlite3VdbeCurrentAddr(v)-1, n);
        assert( n<=pTab->nCol );
      }
    }else{
      sqlite3TableLock(pParse, iDb, pTab->tnum, 0, pTab->zName);
    }
    pLevel->iTabCur = pTabItem->iCursor;
    if( (pIx = pLevel->pIdx)!=0 ){
      KeyInfo *pKey = sqlite3IndexKeyinfo(pParse, pIx);
      assert( pIx->pSchema==pTab->pSchema );
      sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
      VdbeComment((v, "# %s", pIx->zName));
      sqlite3VdbeOp3(v, OP_OpenRead, iIdxCur, pIx->tnum,
                     (char*)pKey, P3_KEYINFO_HANDOFF);
    }
    if( (pLevel->flags & (WHERE_IDX_ONLY|WHERE_COLUMN_RANGE))!=0 ){
      /* Only call OP_SetNumColumns on the index if we might later use
      ** OP_Column on the index. */
      sqlite3VdbeAddOp(v, OP_SetNumColumns, iIdxCur, pIx->nColumn+1);
    }
    sqlite3CodeVerifySchema(pParse, iDb);
  }
  pWInfo->iTop = sqlite3VdbeCurrentAddr(v);

  /* Generate the code to do the search.  Each iteration of the for
  ** loop below generates code for a single nested loop of the VM
  ** program.
  */
  notReady = ~(Bitmask)0;
  for(i=0, pLevel=pWInfo->a; i<pTabList->nSrc; i++, pLevel++){
    int j;
    int iCur = pTabItem->iCursor;  /* The VDBE cursor for the table */
    Index *pIdx;       /* The index we will be using */
    int nxt;           /* Where to jump to continue with the next IN case */
    int iIdxCur;       /* The VDBE cursor for the index */
    int omitTable;     /* True if we use the index only */
    int bRev;          /* True if we need to scan in reverse order */

    pTabItem = &pTabList->a[pLevel->iFrom];
    iCur = pTabItem->iCursor;
    pIdx = pLevel->pIdx;
    iIdxCur = pLevel->iIdxCur;
    bRev = (pLevel->flags & WHERE_REVERSE)!=0;
    omitTable = (pLevel->flags & WHERE_IDX_ONLY)!=0;

    /* Create labels for the "break" and "continue" instructions
    ** for the current loop.  Jump to brk to break out of a loop.
    ** Jump to cont to go immediately to the next iteration of the
    ** loop.
    **
    ** When there is an IN operator, we also have a "nxt" label that
    ** means to continue with the next IN value combination.  When
    ** there are no IN operators in the constraints, the "nxt" label
    ** is the same as "brk".
    */
    brk = pLevel->brk = pLevel->nxt = sqlite3VdbeMakeLabel(v);
    cont = pLevel->cont = sqlite3VdbeMakeLabel(v);

    /* If this is the right table of a LEFT OUTER JOIN, allocate and
    ** initialize a memory cell that records if this table matches any
    ** row of the left table of the join.
    */
    if( pLevel->iFrom>0 && (pTabItem[0].jointype & JT_LEFT)!=0 ){
      if( !pParse->nMem ) pParse->nMem++;
      pLevel->iLeftJoin = pParse->nMem++;
      sqlite3VdbeAddOp(v, OP_MemInt, 0, pLevel->iLeftJoin);
      VdbeComment((v, "# init LEFT JOIN no-match flag"));
    }

#ifndef SQLITE_OMIT_VIRTUALTABLE
    if( pLevel->pBestIdx ){
      /* Case 0:  The table is a virtual-table.  Use the VFilter and VNext
      **          to access the data.
      */
      int j;
      sqlite3_index_info *pBestIdx = pLevel->pBestIdx;
      int nConstraint = pBestIdx->nConstraint;
      struct sqlite3_index_constraint_usage *aUsage =
                                                  pBestIdx->aConstraintUsage;
      const struct sqlite3_index_constraint *aConstraint =
                                                  pBestIdx->aConstraint;

      for(j=1; j<=nConstraint; j++){
        int k;
        for(k=0; k<nConstraint; k++){
          if( aUsage[k].argvIndex==j ){
            int iTerm = aConstraint[k].iTermOffset;
            sqlite3ExprCode(pParse, wc.a[iTerm].pExpr->pRight);
            break;
          }
        }
        if( k==nConstraint ) break;
      }
      sqlite3VdbeAddOp(v, OP_Integer, j-1, 0);
      sqlite3VdbeAddOp(v, OP_Integer, pBestIdx->idxNum, 0);
      sqlite3VdbeOp3(v, OP_VFilter, iCur, brk, pBestIdx->idxStr,
                      pBestIdx->needToFreeIdxStr ? P3_MPRINTF : P3_STATIC);
      pBestIdx->needToFreeIdxStr = 0;
      for(j=0; j<pBestIdx->nConstraint; j++){
        if( aUsage[j].omit ){
          int iTerm = aConstraint[j].iTermOffset;
          disableTerm(pLevel, &wc.a[iTerm]);
        }
      }
      pLevel->op = OP_VNext;
      pLevel->p1 = iCur;
      pLevel->p2 = sqlite3VdbeCurrentAddr(v);
    }else
#endif /* SQLITE_OMIT_VIRTUALTABLE */

    if( pLevel->flags & WHERE_ROWID_EQ ){
      /* Case 1:  We can directly reference a single row using an
      **          equality comparison against the ROWID field.  Or
      **          we reference multiple rows using a "rowid IN (...)"
      **          construct.
      */
      pTerm = findTerm(&wc, iCur, -1, notReady, WO_EQ|WO_IN, 0);
      assert( pTerm!=0 );
      assert( pTerm->pExpr!=0 );
      assert( pTerm->leftCursor==iCur );
      assert( omitTable==0 );
      codeEqualityTerm(pParse, pTerm, pLevel);
      nxt = pLevel->nxt;
      sqlite3VdbeAddOp(v, OP_MustBeInt, 1, nxt);
      sqlite3VdbeAddOp(v, OP_NotExists, iCur, nxt);
      VdbeComment((v, "pk"));
      pLevel->op = OP_Noop;
    }else if( pLevel->flags & WHERE_ROWID_RANGE ){
      /* Case 2:  We have an inequality comparison against the ROWID field.
      */
      int testOp = OP_Noop;
      int start;
      WhereTerm *pStart, *pEnd;

      assert( omitTable==0 );
      pStart = findTerm(&wc, iCur, -1, notReady, WO_GT|WO_GE, 0);
      pEnd = findTerm(&wc, iCur, -1, notReady, WO_LT|WO_LE, 0);
      if( bRev ){
        pTerm = pStart;
        pStart = pEnd;
        pEnd = pTerm;
      }
      if( pStart ){
        Expr *pX;
        pX = pStart->pExpr;
        assert( pX!=0 );
        assert( pStart->leftCursor==iCur );
        sqlite3ExprCode(pParse, pX->pRight);
        sqlite3VdbeAddOp(v, OP_ForceInt, pX->op==TK_LE || pX->op==TK_GT, brk);
        sqlite3VdbeAddOp(v, bRev ? OP_MoveLt : OP_MoveGe, iCur, brk);
        VdbeComment((v, "pk"));
        disableTerm(pLevel, pStart);
      }else{
        sqlite3VdbeAddOp(v, bRev ? OP_Last : OP_Rewind, iCur, brk);
      }
      if( pEnd ){
        Expr *pX;
        pX = pEnd->pExpr;
        assert( pX!=0 );
        assert( pEnd->leftCursor==iCur );
        sqlite3ExprCode(pParse, pX->pRight);
        pLevel->iMem = pParse->nMem++;
        sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        if( pX->op==TK_LT || pX->op==TK_GT ){
          testOp = bRev ? OP_Le : OP_Ge;
        }else{
          testOp = bRev ? OP_Lt : OP_Gt;
        }
        disableTerm(pLevel, pEnd);
      }
      start = sqlite3VdbeCurrentAddr(v);
      pLevel->op = bRev ? OP_Prev : OP_Next;
      pLevel->p1 = iCur;
      pLevel->p2 = start;
      if( testOp!=OP_Noop ){
        sqlite3VdbeAddOp(v, OP_Rowid, iCur, 0);
        sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, testOp, SQLITE_AFF_NUMERIC, brk);
      }
    }else if( pLevel->flags & WHERE_COLUMN_RANGE ){
      /* Case 3: The WHERE clause term that refers to the right-most
      **         column of the index is an inequality.  For example, if
      **         the index is on (x,y,z) and the WHERE clause is of the
      **         form "x=5 AND y<10" then this case is used.  Only the
      **         right-most column can be an inequality - the rest must
      **         use the "==" and "IN" operators.
      **
      **         This case is also used when there are no WHERE clause
      **         constraints but an index is selected anyway, in order
      **         to force the output order to conform to an ORDER BY.
      */
      int start;
      int nEq = pLevel->nEq;
      int topEq=0;        /* True if top limit uses ==. False is strictly < */
      int btmEq=0;        /* True if btm limit uses ==. False if strictly > */
      int topOp, btmOp;   /* Operators for the top and bottom search bounds */
      int testOp;
      int topLimit = (pLevel->flags & WHERE_TOP_LIMIT)!=0;
      int btmLimit = (pLevel->flags & WHERE_BTM_LIMIT)!=0;

      /* Generate code to evaluate all constraint terms using == or IN
      ** and level the values of those terms on the stack.
      */
      codeAllEqualityTerms(pParse, pLevel, &wc, notReady);

      /* Duplicate the equality term values because they will all be
      ** used twice: once to make the termination key and once to make the
      ** start key.
      */
      for(j=0; j<nEq; j++){
        sqlite3VdbeAddOp(v, OP_Dup, nEq-1, 0);
      }

      /* Figure out what comparison operators to use for top and bottom 
      ** search bounds. For an ascending index, the bottom bound is a > or >=
      ** operator and the top bound is a < or <= operator.  For a descending
      ** index the operators are reversed.
      */
      if( pIdx->aSortOrder[nEq]==SQLITE_SO_ASC ){
        topOp = WO_LT|WO_LE;
        btmOp = WO_GT|WO_GE;
      }else{
        topOp = WO_GT|WO_GE;
        btmOp = WO_LT|WO_LE;
        SWAP(int, topLimit, btmLimit);
      }

      /* Generate the termination key.  This is the key value that
      ** will end the search.  There is no termination key if there
      ** are no equality terms and no "X<..." term.
      **
      ** 2002-Dec-04: On a reverse-order scan, the so-called "termination"
      ** key computed here really ends up being the start key.
      */
      nxt = pLevel->nxt;
      if( topLimit ){
        Expr *pX;
        int k = pIdx->aiColumn[j];
        pTerm = findTerm(&wc, iCur, k, notReady, topOp, pIdx);
        assert( pTerm!=0 );
        pX = pTerm->pExpr;
        assert( (pTerm->flags & TERM_CODED)==0 );
        sqlite3ExprCode(pParse, pX->pRight);
        sqlite3VdbeAddOp(v, OP_IsNull, -(nEq+1), nxt);
        topEq = pTerm->eOperator & (WO_LE|WO_GE);
        disableTerm(pLevel, pTerm);
        testOp = OP_IdxGE;
      }else{
        testOp = nEq>0 ? OP_IdxGE : OP_Noop;
        topEq = 1;
      }
      if( testOp!=OP_Noop ){
        int nCol = nEq + topLimit;
        pLevel->iMem = pParse->nMem++;
        buildIndexProbe(v, nCol, pIdx);
        if( bRev ){
          int op = topEq ? OP_MoveLe : OP_MoveLt;
          sqlite3VdbeAddOp(v, op, iIdxCur, nxt);
        }else{
          sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
        }
      }else if( bRev ){
        sqlite3VdbeAddOp(v, OP_Last, iIdxCur, brk);
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
      if( btmLimit ){
        Expr *pX;
        int k = pIdx->aiColumn[j];
        pTerm = findTerm(&wc, iCur, k, notReady, btmOp, pIdx);
        assert( pTerm!=0 );
        pX = pTerm->pExpr;
        assert( (pTerm->flags & TERM_CODED)==0 );
        sqlite3ExprCode(pParse, pX->pRight);
        sqlite3VdbeAddOp(v, OP_IsNull, -(nEq+1), nxt);
        btmEq = pTerm->eOperator & (WO_LE|WO_GE);
        disableTerm(pLevel, pTerm);
      }else{
        btmEq = 1;
      }
      if( nEq>0 || btmLimit ){
        int nCol = nEq + btmLimit;
        buildIndexProbe(v, nCol, pIdx);
        if( bRev ){
          pLevel->iMem = pParse->nMem++;
          sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 1);
          testOp = OP_IdxLT;
        }else{
          int op = btmEq ? OP_MoveGe : OP_MoveGt;
          sqlite3VdbeAddOp(v, op, iIdxCur, nxt);
        }
      }else if( bRev ){
        testOp = OP_Noop;
      }else{
        sqlite3VdbeAddOp(v, OP_Rewind, iIdxCur, brk);
      }

      /* Generate the the top of the loop.  If there is a termination
      ** key we have to test for that key and abort at the top of the
      ** loop.
      */
      start = sqlite3VdbeCurrentAddr(v);
      if( testOp!=OP_Noop ){
        sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, testOp, iIdxCur, nxt);
        if( (topEq && !bRev) || (!btmEq && bRev) ){
          sqlite3VdbeChangeP3(v, -1, "+", P3_STATIC);
        }
      }
      if( topLimit | btmLimit ){
        sqlite3VdbeAddOp(v, OP_Column, iIdxCur, nEq);
        sqlite3VdbeAddOp(v, OP_IsNull, 1, cont);
      }
      if( !omitTable ){
        sqlite3VdbeAddOp(v, OP_IdxRowid, iIdxCur, 0);
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
      }

      /* Record the instruction used to terminate the loop.
      */
      pLevel->op = bRev ? OP_Prev : OP_Next;
      pLevel->p1 = iIdxCur;
      pLevel->p2 = start;
    }else if( pLevel->flags & WHERE_COLUMN_EQ ){
      /* Case 4:  There is an index and all terms of the WHERE clause that
      **          refer to the index using the "==" or "IN" operators.
      */
      int start;
      int nEq = pLevel->nEq;

      /* Generate code to evaluate all constraint terms using == or IN
      ** and leave the values of those terms on the stack.
      */
      codeAllEqualityTerms(pParse, pLevel, &wc, notReady);
      nxt = pLevel->nxt;

      /* Generate a single key that will be used to both start and terminate
      ** the search
      */
      buildIndexProbe(v, nEq, pIdx);
      sqlite3VdbeAddOp(v, OP_MemStore, pLevel->iMem, 0);

      /* Generate code (1) to move to the first matching element of the table.
      ** Then generate code (2) that jumps to "nxt" after the cursor is past
      ** the last matching element of the table.  The code (1) is executed
      ** once to initialize the search, the code (2) is executed before each
      ** iteration of the scan to see if the scan has finished. */
      if( bRev ){
        /* Scan in reverse order */
        sqlite3VdbeAddOp(v, OP_MoveLe, iIdxCur, nxt);
        start = sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeAddOp(v, OP_IdxLT, iIdxCur, nxt);
        pLevel->op = OP_Prev;
      }else{
        /* Scan in the forward order */
        sqlite3VdbeAddOp(v, OP_MoveGe, iIdxCur, nxt);
        start = sqlite3VdbeAddOp(v, OP_MemLoad, pLevel->iMem, 0);
        sqlite3VdbeOp3(v, OP_IdxGE, iIdxCur, nxt, "+", P3_STATIC);
        pLevel->op = OP_Next;
      }
      if( !omitTable ){
        sqlite3VdbeAddOp(v, OP_IdxRowid, iIdxCur, 0);
        sqlite3VdbeAddOp(v, OP_MoveGe, iCur, 0);
      }
      pLevel->p1 = iIdxCur;
      pLevel->p2 = start;
    }else{
      /* Case 5:  There is no usable index.  We must do a complete
      **          scan of the entire table.
      */
      assert( omitTable==0 );
      assert( bRev==0 );
      pLevel->op = OP_Next;
      pLevel->p1 = iCur;
      pLevel->p2 = 1 + sqlite3VdbeAddOp(v, OP_Rewind, iCur, brk);
    }
    notReady &= ~getMask(&maskSet, iCur);

    /* Insert code to test every subexpression that can be completely
    ** computed using the current set of tables.
    */
    for(pTerm=wc.a, j=wc.nTerm; j>0; j--, pTerm++){
      Expr *pE;
      if( pTerm->flags & (TERM_VIRTUAL|TERM_CODED) ) continue;
      if( (pTerm->prereqAll & notReady)!=0 ) continue;
      pE = pTerm->pExpr;
      assert( pE!=0 );
      if( pLevel->iLeftJoin && !ExprHasProperty(pE, EP_FromJoin) ){
        continue;
      }
      sqlite3ExprIfFalse(pParse, pE, cont, 1);
      pTerm->flags |= TERM_CODED;
    }

    /* For a LEFT OUTER JOIN, generate code that will record the fact that
    ** at least one row of the right table has matched the left table.  
    */
    if( pLevel->iLeftJoin ){
      pLevel->top = sqlite3VdbeCurrentAddr(v);
      sqlite3VdbeAddOp(v, OP_MemInt, 1, pLevel->iLeftJoin);
      VdbeComment((v, "# record LEFT JOIN hit"));
      for(pTerm=wc.a, j=0; j<wc.nTerm; j++, pTerm++){
        if( pTerm->flags & (TERM_VIRTUAL|TERM_CODED) ) continue;
        if( (pTerm->prereqAll & notReady)!=0 ) continue;
        assert( pTerm->pExpr );
        sqlite3ExprIfFalse(pParse, pTerm->pExpr, cont, 1);
        pTerm->flags |= TERM_CODED;
      }
    }
  }

#ifdef SQLITE_TEST  /* For testing and debugging use only */
  /* Record in the query plan information about the current table
  ** and the index used to access it (if any).  If the table itself
  ** is not used, its name is just '{}'.  If no index is used
  ** the index is listed as "{}".  If the primary key is used the
  ** index name is '*'.
  */
  for(i=0; i<pTabList->nSrc; i++){
    char *z;
    int n;
    pLevel = &pWInfo->a[i];
    pTabItem = &pTabList->a[pLevel->iFrom];
    z = pTabItem->zAlias;
    if( z==0 ) z = pTabItem->pTab->zName;
    n = strlen(z);
    if( n+nQPlan < sizeof(sqlite3_query_plan)-10 ){
      if( pLevel->flags & WHERE_IDX_ONLY ){
        strcpy(&sqlite3_query_plan[nQPlan], "{}");
        nQPlan += 2;
      }else{
        strcpy(&sqlite3_query_plan[nQPlan], z);
        nQPlan += n;
      }
      sqlite3_query_plan[nQPlan++] = ' ';
    }
    if( pLevel->flags & (WHERE_ROWID_EQ|WHERE_ROWID_RANGE) ){
      strcpy(&sqlite3_query_plan[nQPlan], "* ");
      nQPlan += 2;
    }else if( pLevel->pIdx==0 ){
      strcpy(&sqlite3_query_plan[nQPlan], "{} ");
      nQPlan += 3;
    }else{
      n = strlen(pLevel->pIdx->zName);
      if( n+nQPlan < sizeof(sqlite3_query_plan)-2 ){
        strcpy(&sqlite3_query_plan[nQPlan], pLevel->pIdx->zName);
        nQPlan += n;
        sqlite3_query_plan[nQPlan++] = ' ';
      }
    }
  }
  while( nQPlan>0 && sqlite3_query_plan[nQPlan-1]==' ' ){
    sqlite3_query_plan[--nQPlan] = 0;
  }
  sqlite3_query_plan[nQPlan] = 0;
  nQPlan = 0;
#endif /* SQLITE_TEST // Testing and debugging use only */

  /* Record the continuation address in the WhereInfo structure.  Then
  ** clean up and return.
  */
  pWInfo->iContinue = cont;
  whereClauseClear(&wc);
  return pWInfo;

  /* Jump here if malloc fails */
whereBeginNoMem:
  whereClauseClear(&wc);
  whereInfoFree(pWInfo);
  return 0;
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

  /* Generate loop termination code.
  */
  for(i=pTabList->nSrc-1; i>=0; i--){
    pLevel = &pWInfo->a[i];
    sqlite3VdbeResolveLabel(v, pLevel->cont);
    if( pLevel->op!=OP_Noop ){
      sqlite3VdbeAddOp(v, pLevel->op, pLevel->p1, pLevel->p2);
    }
    if( pLevel->nIn ){
      struct InLoop *pIn;
      int j;
      sqlite3VdbeResolveLabel(v, pLevel->nxt);
      for(j=pLevel->nIn, pIn=&pLevel->aInLoop[j-1]; j>0; j--, pIn--){
        sqlite3VdbeJumpHere(v, pIn->topAddr+1);
        sqlite3VdbeAddOp(v, OP_Next, pIn->iCur, pIn->topAddr);
        sqlite3VdbeJumpHere(v, pIn->topAddr-1);
      }
      sqliteFree(pLevel->aInLoop);
    }
    sqlite3VdbeResolveLabel(v, pLevel->brk);
    if( pLevel->iLeftJoin ){
      int addr;
      addr = sqlite3VdbeAddOp(v, OP_IfMemPos, pLevel->iLeftJoin, 0);
      sqlite3VdbeAddOp(v, OP_NullRow, pTabList->a[i].iCursor, 0);
      if( pLevel->iIdxCur>=0 ){
        sqlite3VdbeAddOp(v, OP_NullRow, pLevel->iIdxCur, 0);
      }
      sqlite3VdbeAddOp(v, OP_Goto, 0, pLevel->top);
      sqlite3VdbeJumpHere(v, addr);
    }
  }

  /* The "break" point is here, just past the end of the outer loop.
  ** Set it.
  */
  sqlite3VdbeResolveLabel(v, pWInfo->iBreak);

  /* Close all of the cursors that were opened by sqlite3WhereBegin.
  */
  for(i=0, pLevel=pWInfo->a; i<pTabList->nSrc; i++, pLevel++){
    struct SrcList_item *pTabItem = &pTabList->a[pLevel->iFrom];
    Table *pTab = pTabItem->pTab;
    assert( pTab!=0 );
    if( pTab->isEphem || pTab->pSelect ) continue;
    if( (pLevel->flags & WHERE_IDX_ONLY)==0 ){
      sqlite3VdbeAddOp(v, OP_Close, pTabItem->iCursor, 0);
    }
    if( pLevel->pIdx!=0 ){
      sqlite3VdbeAddOp(v, OP_Close, pLevel->iIdxCur, 0);
    }

    /* Make cursor substitutions for cases where we want to use
    ** just the index and never reference the table.
    ** 
    ** Calls to the code generator in between sqlite3WhereBegin and
    ** sqlite3WhereEnd will have created code that references the table
    ** directly.  This loop scans all that code looking for opcodes
    ** that reference the table and converts them into opcodes that
    ** reference the index.
    */
    if( pLevel->flags & WHERE_IDX_ONLY ){
      int k, j, last;
      VdbeOp *pOp;
      Index *pIdx = pLevel->pIdx;

      assert( pIdx!=0 );
      pOp = sqlite3VdbeGetOp(v, pWInfo->iTop);
      last = sqlite3VdbeCurrentAddr(v);
      for(k=pWInfo->iTop; k<last; k++, pOp++){
        if( pOp->p1!=pLevel->iTabCur ) continue;
        if( pOp->opcode==OP_Column ){
          pOp->p1 = pLevel->iIdxCur;
          for(j=0; j<pIdx->nColumn; j++){
            if( pOp->p2==pIdx->aiColumn[j] ){
              pOp->p2 = j;
              break;
            }
          }
        }else if( pOp->opcode==OP_Rowid ){
          pOp->p1 = pLevel->iIdxCur;
          pOp->opcode = OP_IdxRowid;
        }else if( pOp->opcode==OP_NullRow ){
          pOp->opcode = OP_Noop;
        }
      }
    }
  }

  /* Final cleanup
  */
  whereInfoFree(pWInfo);
  return;
}
