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
** This file contains SQLite's grammar for SQL.  Process this file
** using the lemon parser generator to generate C code that runs
** the parser.  Lemon will also generate a header file containing
** numeric codes for all of the tokens.
**
** @(#) $Id$
*/
%token_prefix TK_
%token_type {Token}
%default_type {Token}
%extra_argument {Parse *pParse}
%syntax_error {
  if( pParse->zErrMsg==0 ){
    if( TOKEN.z[0] ){
      sqliteErrorMsg(pParse, "near \"%T\": syntax error", &TOKEN);
    }else{
      sqliteErrorMsg(pParse, "incomplete SQL statement");
    }
  }
}
%name sqliteParser
%include {
#include "sqliteInt.h"
#include "parse.h"

/*
** An instance of this structure holds information about the
** LIMIT clause of a SELECT statement.
*/
struct LimitVal {
  int limit;    /* The LIMIT value.  -1 if there is no limit */
  int offset;   /* The OFFSET.  0 if there is none */
};

/*
** An instance of the following structure describes the event of a
** TRIGGER.  "a" is the event type, one of TK_UPDATE, TK_INSERT,
** TK_DELETE, or TK_INSTEAD.  If the event is of the form
**
**      UPDATE ON (a,b,c)
**
** Then the "b" IdList records the list "a,b,c".
*/
struct TrigEvent { int a; IdList * b; };

} // end %include

// These are extra tokens used by the lexer but never seen by the
// parser.  We put them in a rule so that the parser generator will
// add them to the parse.h output file.
//
%nonassoc END_OF_FILE ILLEGAL SPACE UNCLOSED_STRING COMMENT FUNCTION
          COLUMN AGG_FUNCTION.

// Input is a single SQL command
input ::= cmdlist.
cmdlist ::= cmdlist ecmd.
cmdlist ::= ecmd.
ecmd ::= explain cmdx SEMI.
ecmd ::= SEMI.
cmdx ::= cmd.           { sqliteExec(pParse); }
explain ::= EXPLAIN.    { sqliteBeginParse(pParse, 1); }
explain ::= .           { sqliteBeginParse(pParse, 0); }

///////////////////// Begin and end transactions. ////////////////////////////
//

cmd ::= BEGIN trans_opt onconf(R).  {sqliteBeginTransaction(pParse,R);}
trans_opt ::= .
trans_opt ::= TRANSACTION.
trans_opt ::= TRANSACTION nm.
cmd ::= COMMIT trans_opt.      {sqliteCommitTransaction(pParse);}
cmd ::= END trans_opt.         {sqliteCommitTransaction(pParse);}
cmd ::= ROLLBACK trans_opt.    {sqliteRollbackTransaction(pParse);}

///////////////////// The CREATE TABLE statement ////////////////////////////
//
cmd ::= create_table create_table_args.
create_table ::= CREATE(X) temp(T) TABLE nm(Y). {
   sqliteStartTable(pParse,&X,&Y,T,0);
}
%type temp {int}
temp(A) ::= TEMP.  {A = 1;}
temp(A) ::= .      {A = 0;}
create_table_args ::= LP columnlist conslist_opt RP(X). {
  sqliteEndTable(pParse,&X,0);
}
create_table_args ::= AS select(S). {
  sqliteEndTable(pParse,0,S);
  sqliteSelectDelete(S);
}
columnlist ::= columnlist COMMA column.
columnlist ::= column.

// About the only information used for a column is the name of the
// column.  The type is always just "text".  But the code will accept
// an elaborate typename.  Perhaps someday we'll do something with it.
//
column ::= columnid type carglist. 
columnid ::= nm(X).                {sqliteAddColumn(pParse,&X);}

// An IDENTIFIER can be a generic identifier, or one of several
// keywords.  Any non-standard keyword can also be an identifier.
//
%type id {Token}
id(A) ::= ID(X).         {A = X;}

// The following directive causes tokens ABORT, AFTER, ASC, etc. to
// fallback to ID if they will not parse as their original value.
// This obviates the need for the "id" nonterminal.
//
%fallback ID
  ABORT AFTER ASC ATTACH BEFORE BEGIN CASCADE CLUSTER CONFLICT
  COPY DATABASE DEFERRED DELIMITERS DESC DETACH EACH END EXPLAIN FAIL FOR
  GLOB IGNORE IMMEDIATE INITIALLY INSTEAD LIKE MATCH KEY
  OF OFFSET PRAGMA RAISE REPLACE RESTRICT ROW STATEMENT
  TEMP TRIGGER VACUUM VIEW.

// Define operator precedence early so that this is the first occurance
// of the operator tokens in the grammer.  Keeping the operators together
// causes them to be assigned integer values that are close together,
// which keeps parser tables smaller.
//
%left OR.
%left AND.
%right NOT.
%left EQ NE ISNULL NOTNULL IS LIKE GLOB BETWEEN IN.
%left GT GE LT LE.
%left BITAND BITOR LSHIFT RSHIFT.
%left PLUS MINUS.
%left STAR SLASH REM.
%left CONCAT.
%right UMINUS UPLUS BITNOT.

// And "ids" is an identifer-or-string.
//
%type ids {Token}
ids(A) ::= ID(X).        {A = X;}
ids(A) ::= STRING(X).    {A = X;}

// The name of a column or table can be any of the following:
//
%type nm {Token}
nm(A) ::= ID(X).         {A = X;}
nm(A) ::= STRING(X).     {A = X;}
nm(A) ::= JOIN_KW(X).    {A = X;}

type ::= .
type ::= typename(X).                    {sqliteAddColumnType(pParse,&X,&X);}
type ::= typename(X) LP signed RP(Y).    {sqliteAddColumnType(pParse,&X,&Y);}
type ::= typename(X) LP signed COMMA signed RP(Y).
                                         {sqliteAddColumnType(pParse,&X,&Y);}
%type typename {Token}
typename(A) ::= ids(X).           {A = X;}
typename(A) ::= typename(X) ids.  {A = X;}
%type signed {int}
signed(A) ::= INTEGER(X).         { A = atoi(X.z); }
signed(A) ::= PLUS INTEGER(X).    { A = atoi(X.z); }
signed(A) ::= MINUS INTEGER(X).   { A = -atoi(X.z); }
carglist ::= carglist carg.
carglist ::= .
carg ::= CONSTRAINT nm ccons.
carg ::= ccons.
carg ::= DEFAULT STRING(X).          {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT ID(X).              {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT INTEGER(X).         {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT PLUS INTEGER(X).    {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT MINUS INTEGER(X).   {sqliteAddDefaultValue(pParse,&X,1);}
carg ::= DEFAULT FLOAT(X).           {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT PLUS FLOAT(X).      {sqliteAddDefaultValue(pParse,&X,0);}
carg ::= DEFAULT MINUS FLOAT(X).     {sqliteAddDefaultValue(pParse,&X,1);}
carg ::= DEFAULT NULL. 

// In addition to the type name, we also care about the primary key and
// UNIQUE constraints.
//
ccons ::= NULL onconf.
ccons ::= NOT NULL onconf(R).               {sqliteAddNotNull(pParse, R);}
ccons ::= PRIMARY KEY sortorder onconf(R).  {sqliteAddPrimaryKey(pParse,0,R);}
ccons ::= UNIQUE onconf(R).           {sqliteCreateIndex(pParse,0,0,0,R,0,0);}
ccons ::= CHECK LP expr RP onconf.
ccons ::= REFERENCES nm(T) idxlist_opt(TA) refargs(R).
                                {sqliteCreateForeignKey(pParse,0,&T,TA,R);}
ccons ::= defer_subclause(D).   {sqliteDeferForeignKey(pParse,D);}
ccons ::= COLLATE id(C).  {
   sqliteAddCollateType(pParse, sqliteCollateType(C.z, C.n));
}

// The next group of rules parses the arguments to a REFERENCES clause
// that determine if the referential integrity checking is deferred or
// or immediate and which determine what action to take if a ref-integ
// check fails.
//
%type refargs {int}
refargs(A) ::= .                     { A = OE_Restrict * 0x010101; }
refargs(A) ::= refargs(X) refarg(Y). { A = (X & Y.mask) | Y.value; }
%type refarg {struct {int value; int mask;}}
refarg(A) ::= MATCH nm.              { A.value = 0;     A.mask = 0x000000; }
refarg(A) ::= ON DELETE refact(X).   { A.value = X;     A.mask = 0x0000ff; }
refarg(A) ::= ON UPDATE refact(X).   { A.value = X<<8;  A.mask = 0x00ff00; }
refarg(A) ::= ON INSERT refact(X).   { A.value = X<<16; A.mask = 0xff0000; }
%type refact {int}
refact(A) ::= SET NULL.              { A = OE_SetNull; }
refact(A) ::= SET DEFAULT.           { A = OE_SetDflt; }
refact(A) ::= CASCADE.               { A = OE_Cascade; }
refact(A) ::= RESTRICT.              { A = OE_Restrict; }
%type defer_subclause {int}
defer_subclause(A) ::= NOT DEFERRABLE init_deferred_pred_opt(X).  {A = X;}
defer_subclause(A) ::= DEFERRABLE init_deferred_pred_opt(X).      {A = X;}
%type init_deferred_pred_opt {int}
init_deferred_pred_opt(A) ::= .                       {A = 0;}
init_deferred_pred_opt(A) ::= INITIALLY DEFERRED.     {A = 1;}
init_deferred_pred_opt(A) ::= INITIALLY IMMEDIATE.    {A = 0;}

// For the time being, the only constraint we care about is the primary
// key and UNIQUE.  Both create indices.
//
conslist_opt ::= .
conslist_opt ::= COMMA conslist.
conslist ::= conslist COMMA tcons.
conslist ::= conslist tcons.
conslist ::= tcons.
tcons ::= CONSTRAINT nm.
tcons ::= PRIMARY KEY LP idxlist(X) RP onconf(R).
                                             {sqliteAddPrimaryKey(pParse,X,R);}
tcons ::= UNIQUE LP idxlist(X) RP onconf(R).
                                       {sqliteCreateIndex(pParse,0,0,X,R,0,0);}
tcons ::= CHECK expr onconf.
tcons ::= FOREIGN KEY LP idxlist(FA) RP
          REFERENCES nm(T) idxlist_opt(TA) refargs(R) defer_subclause_opt(D). {
    sqliteCreateForeignKey(pParse, FA, &T, TA, R);
    sqliteDeferForeignKey(pParse, D);
}
%type defer_subclause_opt {int}
defer_subclause_opt(A) ::= .                    {A = 0;}
defer_subclause_opt(A) ::= defer_subclause(X).  {A = X;}

// The following is a non-standard extension that allows us to declare the
// default behavior when there is a constraint conflict.
//
%type onconf {int}
%type orconf {int}
%type resolvetype {int}
onconf(A) ::= .                              { A = OE_Default; }
onconf(A) ::= ON CONFLICT resolvetype(X).    { A = X; }
orconf(A) ::= .                              { A = OE_Default; }
orconf(A) ::= OR resolvetype(X).             { A = X; }
resolvetype(A) ::= ROLLBACK.                 { A = OE_Rollback; }
resolvetype(A) ::= ABORT.                    { A = OE_Abort; }
resolvetype(A) ::= FAIL.                     { A = OE_Fail; }
resolvetype(A) ::= IGNORE.                   { A = OE_Ignore; }
resolvetype(A) ::= REPLACE.                  { A = OE_Replace; }

////////////////////////// The DROP TABLE /////////////////////////////////////
//
cmd ::= DROP TABLE nm(X).          {sqliteDropTable(pParse,&X,0);}

///////////////////// The CREATE VIEW statement /////////////////////////////
//
cmd ::= CREATE(X) temp(T) VIEW nm(Y) AS select(S). {
  sqliteCreateView(pParse, &X, &Y, S, T);
}
cmd ::= DROP VIEW nm(X). {
  sqliteDropTable(pParse, &X, 1);
}

//////////////////////// The SELECT statement /////////////////////////////////
//
cmd ::= select(X).  {
  sqliteSelect(pParse, X, SRT_Callback, 0, 0, 0, 0);
  sqliteSelectDelete(X);
}

%type select {Select*}
%destructor select {sqliteSelectDelete($$);}
%type oneselect {Select*}
%destructor oneselect {sqliteSelectDelete($$);}

select(A) ::= oneselect(X).                      {A = X;}
select(A) ::= select(X) multiselect_op(Y) oneselect(Z).  {
  if( Z ){
    Z->op = Y;
    Z->pPrior = X;
  }
  A = Z;
}
%type multiselect_op {int}
multiselect_op(A) ::= UNION.      {A = TK_UNION;}
multiselect_op(A) ::= UNION ALL.  {A = TK_ALL;}
multiselect_op(A) ::= INTERSECT.  {A = TK_INTERSECT;}
multiselect_op(A) ::= EXCEPT.     {A = TK_EXCEPT;}
oneselect(A) ::= SELECT distinct(D) selcollist(W) from(X) where_opt(Y)
                 groupby_opt(P) having_opt(Q) orderby_opt(Z) limit_opt(L). {
  A = sqliteSelectNew(W,X,Y,P,Q,Z,D,L.limit,L.offset);
}

// The "distinct" nonterminal is true (1) if the DISTINCT keyword is
// present and false (0) if it is not.
//
%type distinct {int}
distinct(A) ::= DISTINCT.   {A = 1;}
distinct(A) ::= ALL.        {A = 0;}
distinct(A) ::= .           {A = 0;}

// selcollist is a list of expressions that are to become the return
// values of the SELECT statement.  The "*" in statements like
// "SELECT * FROM ..." is encoded as a special expression with an
// opcode of TK_ALL.
//
%type selcollist {ExprList*}
%destructor selcollist {sqliteExprListDelete($$);}
%type sclp {ExprList*}
%destructor sclp {sqliteExprListDelete($$);}
sclp(A) ::= selcollist(X) COMMA.             {A = X;}
sclp(A) ::= .                                {A = 0;}
selcollist(A) ::= sclp(P) expr(X) as(Y).     {
   A = sqliteExprListAppend(P,X,Y.n?&Y:0);
}
selcollist(A) ::= sclp(P) STAR. {
  A = sqliteExprListAppend(P, sqliteExpr(TK_ALL, 0, 0, 0), 0);
}
selcollist(A) ::= sclp(P) nm(X) DOT STAR. {
  Expr *pRight = sqliteExpr(TK_ALL, 0, 0, 0);
  Expr *pLeft = sqliteExpr(TK_ID, 0, 0, &X);
  A = sqliteExprListAppend(P, sqliteExpr(TK_DOT, pLeft, pRight, 0), 0);
}

// An option "AS <id>" phrase that can follow one of the expressions that
// define the result set, or one of the tables in the FROM clause.
//
%type as {Token}
as(X) ::= AS nm(Y).    { X = Y; }
as(X) ::= ids(Y).      { X = Y; }
as(X) ::= .            { X.n = 0; }


%type seltablist {SrcList*}
%destructor seltablist {sqliteSrcListDelete($$);}
%type stl_prefix {SrcList*}
%destructor stl_prefix {sqliteSrcListDelete($$);}
%type from {SrcList*}
%destructor from {sqliteSrcListDelete($$);}

// A complete FROM clause.
//
from(A) ::= .                                 {A = sqliteMalloc(sizeof(*A));}
from(A) ::= FROM seltablist(X).               {A = X;}

// "seltablist" is a "Select Table List" - the content of the FROM clause
// in a SELECT statement.  "stl_prefix" is a prefix of this list.
//
stl_prefix(A) ::= seltablist(X) joinop(Y).    {
   A = X;
   if( A && A->nSrc>0 ) A->a[A->nSrc-1].jointype = Y;
}
stl_prefix(A) ::= .                           {A = 0;}
seltablist(A) ::= stl_prefix(X) nm(Y) dbnm(D) as(Z) on_opt(N) using_opt(U). {
  A = sqliteSrcListAppend(X,&Y,&D);
  if( Z.n ) sqliteSrcListAddAlias(A,&Z);
  if( N ){
    if( A && A->nSrc>1 ){ A->a[A->nSrc-2].pOn = N; }
    else { sqliteExprDelete(N); }
  }
  if( U ){
    if( A && A->nSrc>1 ){ A->a[A->nSrc-2].pUsing = U; }
    else { sqliteIdListDelete(U); }
  }
}
seltablist(A) ::= stl_prefix(X) LP seltablist_paren(S) RP
                  as(Z) on_opt(N) using_opt(U). {
  A = sqliteSrcListAppend(X,0,0);
  A->a[A->nSrc-1].pSelect = S;
  if( Z.n ) sqliteSrcListAddAlias(A,&Z);
  if( N ){
    if( A && A->nSrc>1 ){ A->a[A->nSrc-2].pOn = N; }
    else { sqliteExprDelete(N); }
  }
  if( U ){
    if( A && A->nSrc>1 ){ A->a[A->nSrc-2].pUsing = U; }
    else { sqliteIdListDelete(U); }
  }
}

// A seltablist_paren nonterminal represents anything in a FROM that
// is contained inside parentheses.  This can be either a subquery or
// a grouping of table and subqueries.
//
%type seltablist_paren {Select*}
%destructor seltablist_paren {sqliteSelectDelete($$);}
seltablist_paren(A) ::= select(S).      {A = S;}
seltablist_paren(A) ::= seltablist(F).  {
   A = sqliteSelectNew(0,F,0,0,0,0,0,-1,0);
}

%type dbnm {Token}
dbnm(A) ::= .          {A.z=0; A.n=0;}
dbnm(A) ::= DOT nm(X). {A = X;}

%type joinop {int}
%type joinop2 {int}
joinop(X) ::= COMMA.                   { X = JT_INNER; }
joinop(X) ::= JOIN.                    { X = JT_INNER; }
joinop(X) ::= JOIN_KW(A) JOIN.         { X = sqliteJoinType(pParse,&A,0,0); }
joinop(X) ::= JOIN_KW(A) nm(B) JOIN.   { X = sqliteJoinType(pParse,&A,&B,0); }
joinop(X) ::= JOIN_KW(A) nm(B) nm(C) JOIN.
                                       { X = sqliteJoinType(pParse,&A,&B,&C); }

%type on_opt {Expr*}
%destructor on_opt {sqliteExprDelete($$);}
on_opt(N) ::= ON expr(E).   {N = E;}
on_opt(N) ::= .             {N = 0;}

%type using_opt {IdList*}
%destructor using_opt {sqliteIdListDelete($$);}
using_opt(U) ::= USING LP idxlist(L) RP.  {U = L;}
using_opt(U) ::= .                        {U = 0;}


%type orderby_opt {ExprList*}
%destructor orderby_opt {sqliteExprListDelete($$);}
%type sortlist {ExprList*}
%destructor sortlist {sqliteExprListDelete($$);}
%type sortitem {Expr*}
%destructor sortitem {sqliteExprDelete($$);}

orderby_opt(A) ::= .                          {A = 0;}
orderby_opt(A) ::= ORDER BY sortlist(X).      {A = X;}
sortlist(A) ::= sortlist(X) COMMA sortitem(Y) collate(C) sortorder(Z). {
  A = sqliteExprListAppend(X,Y,0);
  if( A ) A->a[A->nExpr-1].sortOrder = C+Z;
}
sortlist(A) ::= sortitem(Y) collate(C) sortorder(Z). {
  A = sqliteExprListAppend(0,Y,0);
  if( A ) A->a[0].sortOrder = C+Z;
}
sortitem(A) ::= expr(X).   {A = X;}

%type sortorder {int}
%type collate {int}

sortorder(A) ::= ASC.           {A = SQLITE_SO_ASC;}
sortorder(A) ::= DESC.          {A = SQLITE_SO_DESC;}
sortorder(A) ::= .              {A = SQLITE_SO_ASC;}
collate(C) ::= .                {C = SQLITE_SO_UNK;}
collate(C) ::= COLLATE id(X).   {C = sqliteCollateType(X.z, X.n);}

%type groupby_opt {ExprList*}
%destructor groupby_opt {sqliteExprListDelete($$);}
groupby_opt(A) ::= .                      {A = 0;}
groupby_opt(A) ::= GROUP BY exprlist(X).  {A = X;}

%type having_opt {Expr*}
%destructor having_opt {sqliteExprDelete($$);}
having_opt(A) ::= .                {A = 0;}
having_opt(A) ::= HAVING expr(X).  {A = X;}

%type limit_opt {struct LimitVal}
limit_opt(A) ::= .                     {A.limit = -1; A.offset = 0;}
limit_opt(A) ::= LIMIT signed(X).      {A.limit = X; A.offset = 0;}
limit_opt(A) ::= LIMIT signed(X) OFFSET signed(Y). 
                                       {A.limit = X; A.offset = Y;}
limit_opt(A) ::= LIMIT signed(X) COMMA signed(Y). 
                                       {A.limit = Y; A.offset = X;}

/////////////////////////// The DELETE statement /////////////////////////////
//
cmd ::= DELETE FROM nm(X) dbnm(D) where_opt(Y). {
   sqliteDeleteFrom(pParse, sqliteSrcListAppend(0,&X,&D), Y);
}

%type where_opt {Expr*}
%destructor where_opt {sqliteExprDelete($$);}

where_opt(A) ::= .                    {A = 0;}
where_opt(A) ::= WHERE expr(X).       {A = X;}

%type setlist {ExprList*}
%destructor setlist {sqliteExprListDelete($$);}

////////////////////////// The UPDATE command ////////////////////////////////
//
cmd ::= UPDATE orconf(R) nm(X) dbnm(D) SET setlist(Y) where_opt(Z).
    {sqliteUpdate(pParse,sqliteSrcListAppend(0,&X,&D),Y,Z,R);}

setlist(A) ::= setlist(Z) COMMA nm(X) EQ expr(Y).
    {A = sqliteExprListAppend(Z,Y,&X);}
setlist(A) ::= nm(X) EQ expr(Y).   {A = sqliteExprListAppend(0,Y,&X);}

////////////////////////// The INSERT command /////////////////////////////////
//
cmd ::= insert_cmd(R) INTO nm(X) dbnm(D) inscollist_opt(F) 
        VALUES LP itemlist(Y) RP.
            {sqliteInsert(pParse, sqliteSrcListAppend(0,&X,&D), Y, 0, F, R);}
cmd ::= insert_cmd(R) INTO nm(X) dbnm(D) inscollist_opt(F) select(S).
            {sqliteInsert(pParse, sqliteSrcListAppend(0,&X,&D), 0, S, F, R);}

%type insert_cmd {int}
insert_cmd(A) ::= INSERT orconf(R).   {A = R;}
insert_cmd(A) ::= REPLACE.            {A = OE_Replace;}


%type itemlist {ExprList*}
%destructor itemlist {sqliteExprListDelete($$);}

itemlist(A) ::= itemlist(X) COMMA expr(Y).  {A = sqliteExprListAppend(X,Y,0);}
itemlist(A) ::= expr(X).                    {A = sqliteExprListAppend(0,X,0);}

%type inscollist_opt {IdList*}
%destructor inscollist_opt {sqliteIdListDelete($$);}
%type inscollist {IdList*}
%destructor inscollist {sqliteIdListDelete($$);}

inscollist_opt(A) ::= .                       {A = 0;}
inscollist_opt(A) ::= LP inscollist(X) RP.    {A = X;}
inscollist(A) ::= inscollist(X) COMMA nm(Y).  {A = sqliteIdListAppend(X,&Y);}
inscollist(A) ::= nm(Y).                      {A = sqliteIdListAppend(0,&Y);}

/////////////////////////// Expression Processing /////////////////////////////
//

%type expr {Expr*}
%destructor expr {sqliteExprDelete($$);}

expr(A) ::= LP(B) expr(X) RP(E). {A = X; sqliteExprSpan(A,&B,&E); }
expr(A) ::= NULL(X).             {A = sqliteExpr(TK_NULL, 0, 0, &X);}
expr(A) ::= ID(X).               {A = sqliteExpr(TK_ID, 0, 0, &X);}
expr(A) ::= JOIN_KW(X).          {A = sqliteExpr(TK_ID, 0, 0, &X);}
expr(A) ::= nm(X) DOT nm(Y). {
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &X);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &Y);
  A = sqliteExpr(TK_DOT, temp1, temp2, 0);
}
expr(A) ::= nm(X) DOT nm(Y) DOT nm(Z). {
  Expr *temp1 = sqliteExpr(TK_ID, 0, 0, &X);
  Expr *temp2 = sqliteExpr(TK_ID, 0, 0, &Y);
  Expr *temp3 = sqliteExpr(TK_ID, 0, 0, &Z);
  Expr *temp4 = sqliteExpr(TK_DOT, temp2, temp3, 0);
  A = sqliteExpr(TK_DOT, temp1, temp4, 0);
}
expr(A) ::= INTEGER(X).      {A = sqliteExpr(TK_INTEGER, 0, 0, &X);}
expr(A) ::= FLOAT(X).        {A = sqliteExpr(TK_FLOAT, 0, 0, &X);}
expr(A) ::= STRING(X).       {A = sqliteExpr(TK_STRING, 0, 0, &X);}
expr(A) ::= VARIABLE(X).     {
  A = sqliteExpr(TK_VARIABLE, 0, 0, &X);
  if( A ) A->iTable = ++pParse->nVar;
}
expr(A) ::= ID(X) LP exprlist(Y) RP(E). {
  A = sqliteExprFunction(Y, &X);
  sqliteExprSpan(A,&X,&E);
}
expr(A) ::= ID(X) LP STAR RP(E). {
  A = sqliteExprFunction(0, &X);
  sqliteExprSpan(A,&X,&E);
}
expr(A) ::= expr(X) AND expr(Y).   {A = sqliteExpr(TK_AND, X, Y, 0);}
expr(A) ::= expr(X) OR expr(Y).    {A = sqliteExpr(TK_OR, X, Y, 0);}
expr(A) ::= expr(X) LT expr(Y).    {A = sqliteExpr(TK_LT, X, Y, 0);}
expr(A) ::= expr(X) GT expr(Y).    {A = sqliteExpr(TK_GT, X, Y, 0);}
expr(A) ::= expr(X) LE expr(Y).    {A = sqliteExpr(TK_LE, X, Y, 0);}
expr(A) ::= expr(X) GE expr(Y).    {A = sqliteExpr(TK_GE, X, Y, 0);}
expr(A) ::= expr(X) NE expr(Y).    {A = sqliteExpr(TK_NE, X, Y, 0);}
expr(A) ::= expr(X) EQ expr(Y).    {A = sqliteExpr(TK_EQ, X, Y, 0);}
expr(A) ::= expr(X) BITAND expr(Y). {A = sqliteExpr(TK_BITAND, X, Y, 0);}
expr(A) ::= expr(X) BITOR expr(Y).  {A = sqliteExpr(TK_BITOR, X, Y, 0);}
expr(A) ::= expr(X) LSHIFT expr(Y). {A = sqliteExpr(TK_LSHIFT, X, Y, 0);}
expr(A) ::= expr(X) RSHIFT expr(Y). {A = sqliteExpr(TK_RSHIFT, X, Y, 0);}
expr(A) ::= expr(X) likeop(OP) expr(Y).  [LIKE]  {
  ExprList *pList = sqliteExprListAppend(0, Y, 0);
  pList = sqliteExprListAppend(pList, X, 0);
  A = sqliteExprFunction(pList, 0);
  if( A ) A->op = OP;
  sqliteExprSpan(A, &X->span, &Y->span);
}
expr(A) ::= expr(X) NOT likeop(OP) expr(Y). [LIKE] {
  ExprList *pList = sqliteExprListAppend(0, Y, 0);
  pList = sqliteExprListAppend(pList, X, 0);
  A = sqliteExprFunction(pList, 0);
  if( A ) A->op = OP;
  A = sqliteExpr(TK_NOT, A, 0, 0);
  sqliteExprSpan(A,&X->span,&Y->span);
}
%type likeop {int}
likeop(A) ::= LIKE. {A = TK_LIKE;}
likeop(A) ::= GLOB. {A = TK_GLOB;}
expr(A) ::= expr(X) PLUS expr(Y).  {A = sqliteExpr(TK_PLUS, X, Y, 0);}
expr(A) ::= expr(X) MINUS expr(Y). {A = sqliteExpr(TK_MINUS, X, Y, 0);}
expr(A) ::= expr(X) STAR expr(Y).  {A = sqliteExpr(TK_STAR, X, Y, 0);}
expr(A) ::= expr(X) SLASH expr(Y). {A = sqliteExpr(TK_SLASH, X, Y, 0);}
expr(A) ::= expr(X) REM expr(Y).   {A = sqliteExpr(TK_REM, X, Y, 0);}
expr(A) ::= expr(X) CONCAT expr(Y). {A = sqliteExpr(TK_CONCAT, X, Y, 0);}
expr(A) ::= expr(X) ISNULL(E). {
  A = sqliteExpr(TK_ISNULL, X, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) IS NULL(E). {
  A = sqliteExpr(TK_ISNULL, X, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) NOTNULL(E). {
  A = sqliteExpr(TK_NOTNULL, X, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) NOT NULL(E). {
  A = sqliteExpr(TK_NOTNULL, X, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) IS NOT NULL(E). {
  A = sqliteExpr(TK_NOTNULL, X, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= NOT(B) expr(X). {
  A = sqliteExpr(TK_NOT, X, 0, 0);
  sqliteExprSpan(A,&B,&X->span);
}
expr(A) ::= BITNOT(B) expr(X). {
  A = sqliteExpr(TK_BITNOT, X, 0, 0);
  sqliteExprSpan(A,&B,&X->span);
}
expr(A) ::= MINUS(B) expr(X). [UMINUS] {
  A = sqliteExpr(TK_UMINUS, X, 0, 0);
  sqliteExprSpan(A,&B,&X->span);
}
expr(A) ::= PLUS(B) expr(X). [UPLUS] {
  A = sqliteExpr(TK_UPLUS, X, 0, 0);
  sqliteExprSpan(A,&B,&X->span);
}
expr(A) ::= LP(B) select(X) RP(E). {
  A = sqliteExpr(TK_SELECT, 0, 0, 0);
  if( A ) A->pSelect = X;
  sqliteExprSpan(A,&B,&E);
}
expr(A) ::= expr(W) BETWEEN expr(X) AND expr(Y). {
  ExprList *pList = sqliteExprListAppend(0, X, 0);
  pList = sqliteExprListAppend(pList, Y, 0);
  A = sqliteExpr(TK_BETWEEN, W, 0, 0);
  if( A ) A->pList = pList;
  sqliteExprSpan(A,&W->span,&Y->span);
}
expr(A) ::= expr(W) NOT BETWEEN expr(X) AND expr(Y). {
  ExprList *pList = sqliteExprListAppend(0, X, 0);
  pList = sqliteExprListAppend(pList, Y, 0);
  A = sqliteExpr(TK_BETWEEN, W, 0, 0);
  if( A ) A->pList = pList;
  A = sqliteExpr(TK_NOT, A, 0, 0);
  sqliteExprSpan(A,&W->span,&Y->span);
}
expr(A) ::= expr(X) IN LP exprlist(Y) RP(E).  {
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pList = Y;
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) IN LP select(Y) RP(E).  {
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pSelect = Y;
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) NOT IN LP exprlist(Y) RP(E).  {
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pList = Y;
  A = sqliteExpr(TK_NOT, A, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) NOT IN LP select(Y) RP(E).  {
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pSelect = Y;
  A = sqliteExpr(TK_NOT, A, 0, 0);
  sqliteExprSpan(A,&X->span,&E);
}
expr(A) ::= expr(X) IN nm(Y) dbnm(D). {
  SrcList *pSrc = sqliteSrcListAppend(0, &Y, &D);
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  sqliteExprSpan(A,&X->span,D.z?&D:&Y);
}
expr(A) ::= expr(X) NOT IN nm(Y) dbnm(D). {
  SrcList *pSrc = sqliteSrcListAppend(0, &Y, &D);
  A = sqliteExpr(TK_IN, X, 0, 0);
  if( A ) A->pSelect = sqliteSelectNew(0,pSrc,0,0,0,0,0,-1,0);
  A = sqliteExpr(TK_NOT, A, 0, 0);
  sqliteExprSpan(A,&X->span,D.z?&D:&Y);
}


/* CASE expressions */
expr(A) ::= CASE(C) case_operand(X) case_exprlist(Y) case_else(Z) END(E). {
  A = sqliteExpr(TK_CASE, X, Z, 0);
  if( A ) A->pList = Y;
  sqliteExprSpan(A, &C, &E);
}
%type case_exprlist {ExprList*}
%destructor case_exprlist {sqliteExprListDelete($$);}
case_exprlist(A) ::= case_exprlist(X) WHEN expr(Y) THEN expr(Z). {
  A = sqliteExprListAppend(X, Y, 0);
  A = sqliteExprListAppend(A, Z, 0);
}
case_exprlist(A) ::= WHEN expr(Y) THEN expr(Z). {
  A = sqliteExprListAppend(0, Y, 0);
  A = sqliteExprListAppend(A, Z, 0);
}
%type case_else {Expr*}
case_else(A) ::=  ELSE expr(X).         {A = X;}
case_else(A) ::=  .                     {A = 0;} 
%type case_operand {Expr*}
case_operand(A) ::= expr(X).            {A = X;} 
case_operand(A) ::= .                   {A = 0;} 

%type exprlist {ExprList*}
%destructor exprlist {sqliteExprListDelete($$);}
%type expritem {Expr*}
%destructor expritem {sqliteExprDelete($$);}

exprlist(A) ::= exprlist(X) COMMA expritem(Y). 
   {A = sqliteExprListAppend(X,Y,0);}
exprlist(A) ::= expritem(X).            {A = sqliteExprListAppend(0,X,0);}
expritem(A) ::= expr(X).                {A = X;}
expritem(A) ::= .                       {A = 0;}

///////////////////////////// The CREATE INDEX command ///////////////////////
//
cmd ::= CREATE(S) uniqueflag(U) INDEX nm(X)
        ON nm(Y) dbnm(D) LP idxlist(Z) RP(E) onconf(R). {
  SrcList *pSrc = sqliteSrcListAppend(0, &Y, &D);
  if( U!=OE_None ) U = R;
  if( U==OE_Default) U = OE_Abort;
  sqliteCreateIndex(pParse, &X, pSrc, Z, U, &S, &E);
}

%type uniqueflag {int}
uniqueflag(A) ::= UNIQUE.  { A = OE_Abort; }
uniqueflag(A) ::= .        { A = OE_None; }

%type idxlist {IdList*}
%destructor idxlist {sqliteIdListDelete($$);}
%type idxlist_opt {IdList*}
%destructor idxlist_opt {sqliteIdListDelete($$);}
%type idxitem {Token}

idxlist_opt(A) ::= .                         {A = 0;}
idxlist_opt(A) ::= LP idxlist(X) RP.         {A = X;}
idxlist(A) ::= idxlist(X) COMMA idxitem(Y).  {A = sqliteIdListAppend(X,&Y);}
idxlist(A) ::= idxitem(Y).                   {A = sqliteIdListAppend(0,&Y);}
idxitem(A) ::= nm(X) sortorder.              {A = X;}

///////////////////////////// The DROP INDEX command /////////////////////////
//

cmd ::= DROP INDEX nm(X) dbnm(Y).   {
  sqliteDropIndex(pParse, sqliteSrcListAppend(0,&X,&Y));
}


///////////////////////////// The COPY command ///////////////////////////////
//
cmd ::= COPY orconf(R) nm(X) dbnm(D) FROM nm(Y) USING DELIMITERS STRING(Z).
    {sqliteCopy(pParse,sqliteSrcListAppend(0,&X,&D),&Y,&Z,R);}
cmd ::= COPY orconf(R) nm(X) dbnm(D) FROM nm(Y).
    {sqliteCopy(pParse,sqliteSrcListAppend(0,&X,&D),&Y,0,R);}

///////////////////////////// The VACUUM command /////////////////////////////
//
cmd ::= VACUUM.                {sqliteVacuum(pParse,0);}
cmd ::= VACUUM nm(X).         {sqliteVacuum(pParse,&X);}

///////////////////////////// The PRAGMA command /////////////////////////////
//
cmd ::= PRAGMA ids(X) EQ nm(Y).         {sqlitePragma(pParse,&X,&Y,0);}
cmd ::= PRAGMA ids(X) EQ ON(Y).          {sqlitePragma(pParse,&X,&Y,0);}
cmd ::= PRAGMA ids(X) EQ plus_num(Y).    {sqlitePragma(pParse,&X,&Y,0);}
cmd ::= PRAGMA ids(X) EQ minus_num(Y).   {sqlitePragma(pParse,&X,&Y,1);}
cmd ::= PRAGMA ids(X) LP nm(Y) RP.      {sqlitePragma(pParse,&X,&Y,0);}
cmd ::= PRAGMA ids(X).                   {sqlitePragma(pParse,&X,&X,0);}
plus_num(A) ::= plus_opt number(X).   {A = X;}
minus_num(A) ::= MINUS number(X).     {A = X;}
number(A) ::= INTEGER(X).  {A = X;}
number(A) ::= FLOAT(X).    {A = X;}
plus_opt ::= PLUS.
plus_opt ::= .

//////////////////////////// The CREATE TRIGGER command /////////////////////

cmd ::= CREATE(A) trigger_decl BEGIN trigger_cmd_list(S) END(Z). {
  Token all;
  all.z = A.z;
  all.n = (Z.z - A.z) + Z.n;
  sqliteFinishTrigger(pParse, S, &all);
}

trigger_decl ::= temp(T) TRIGGER nm(B) trigger_time(C) trigger_event(D)
                 ON nm(E) dbnm(DB) foreach_clause(F) when_clause(G). {
  SrcList *pTab = sqliteSrcListAppend(0, &E, &DB);
  sqliteBeginTrigger(pParse, &B, C, D.a, D.b, pTab, F, G, T);
}

%type trigger_time  {int}
trigger_time(A) ::= BEFORE.      { A = TK_BEFORE; }
trigger_time(A) ::= AFTER.       { A = TK_AFTER;  }
trigger_time(A) ::= INSTEAD OF.  { A = TK_INSTEAD;}
trigger_time(A) ::= .            { A = TK_BEFORE; }

%type trigger_event {struct TrigEvent}
%destructor trigger_event {sqliteIdListDelete($$.b);}
trigger_event(A) ::= DELETE. { A.a = TK_DELETE; A.b = 0; }
trigger_event(A) ::= INSERT. { A.a = TK_INSERT; A.b = 0; }
trigger_event(A) ::= UPDATE. { A.a = TK_UPDATE; A.b = 0;}
trigger_event(A) ::= UPDATE OF inscollist(X). {A.a = TK_UPDATE; A.b = X; }

%type foreach_clause {int}
foreach_clause(A) ::= .                   { A = TK_ROW; }
foreach_clause(A) ::= FOR EACH ROW.       { A = TK_ROW; }
foreach_clause(A) ::= FOR EACH STATEMENT. { A = TK_STATEMENT; }

%type when_clause {Expr *}
when_clause(A) ::= .             { A = 0; }
when_clause(A) ::= WHEN expr(X). { A = X; }

%type trigger_cmd_list {TriggerStep *}
%destructor trigger_cmd_list {sqliteDeleteTriggerStep($$);}
trigger_cmd_list(A) ::= trigger_cmd(X) SEMI trigger_cmd_list(Y). {
  X->pNext = Y;
  A = X;
}
trigger_cmd_list(A) ::= . { A = 0; }

%type trigger_cmd {TriggerStep *}
%destructor trigger_cmd {sqliteDeleteTriggerStep($$);}
// UPDATE 
trigger_cmd(A) ::= UPDATE orconf(R) nm(X) SET setlist(Y) where_opt(Z).  
               { A = sqliteTriggerUpdateStep(&X, Y, Z, R); }

// INSERT
trigger_cmd(A) ::= insert_cmd(R) INTO nm(X) inscollist_opt(F) 
  VALUES LP itemlist(Y) RP.  
{A = sqliteTriggerInsertStep(&X, F, Y, 0, R);}

trigger_cmd(A) ::= insert_cmd(R) INTO nm(X) inscollist_opt(F) select(S).
               {A = sqliteTriggerInsertStep(&X, F, 0, S, R);}

// DELETE
trigger_cmd(A) ::= DELETE FROM nm(X) where_opt(Y).
               {A = sqliteTriggerDeleteStep(&X, Y);}

// SELECT
trigger_cmd(A) ::= select(X).  {A = sqliteTriggerSelectStep(X); }

// The special RAISE expression that may occur in trigger programs
expr(A) ::= RAISE(X) LP IGNORE RP(Y).  {
  A = sqliteExpr(TK_RAISE, 0, 0, 0); 
  A->iColumn = OE_Ignore;
  sqliteExprSpan(A, &X, &Y);
}
expr(A) ::= RAISE(X) LP ROLLBACK COMMA nm(Z) RP(Y).  {
  A = sqliteExpr(TK_RAISE, 0, 0, &Z); 
  A->iColumn = OE_Rollback;
  sqliteExprSpan(A, &X, &Y);
}
expr(A) ::= RAISE(X) LP ABORT COMMA nm(Z) RP(Y).  {
  A = sqliteExpr(TK_RAISE, 0, 0, &Z); 
  A->iColumn = OE_Abort;
  sqliteExprSpan(A, &X, &Y);
}
expr(A) ::= RAISE(X) LP FAIL COMMA nm(Z) RP(Y).  {
  A = sqliteExpr(TK_RAISE, 0, 0, &Z); 
  A->iColumn = OE_Fail;
  sqliteExprSpan(A, &X, &Y);
}

////////////////////////  DROP TRIGGER statement //////////////////////////////
cmd ::= DROP TRIGGER nm(X) dbnm(D). {
  sqliteDropTrigger(pParse,sqliteSrcListAppend(0,&X,&D));
}

//////////////////////// ATTACH DATABASE file AS name /////////////////////////
cmd ::= ATTACH database_kw_opt ids(F) AS nm(D) key_opt(K). {
  sqliteAttach(pParse, &F, &D, &K);
}
%type key_opt {Token}
key_opt(A) ::= USING ids(X).  { A = X; }
key_opt(A) ::= .              { A.z = 0; A.n = 0; }

database_kw_opt ::= DATABASE.
database_kw_opt ::= .

//////////////////////// DETACH DATABASE name /////////////////////////////////
cmd ::= DETACH database_kw_opt nm(D). {
  sqliteDetach(pParse, &D);
}
