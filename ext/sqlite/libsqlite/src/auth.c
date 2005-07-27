/*
** 2003 January 11
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used to implement the sqlite_set_authorizer()
** API.  This facility is an optional feature of the library.  Embedded
** systems that do not need this facility may omit it by recompiling
** the library with -DSQLITE_OMIT_AUTHORIZATION=1
**
** $Id$
*/
#include "sqliteInt.h"

/*
** All of the code in this file may be omitted by defining a single
** macro.
*/
#ifndef SQLITE_OMIT_AUTHORIZATION

/*
** Set or clear the access authorization function.
**
** The access authorization function is be called during the compilation
** phase to verify that the user has read and/or write access permission
** various fields of the database.  The first argument to the auth function
** is a copy of the 3rd argument to this routine.  The second argument
** to the auth function is one of these constants:
**
**        SQLITE_READ_COLUMN
**        SQLITE_WRITE_COLUMN
**        SQLITE_DELETE_ROW
**        SQLITE_INSERT_ROW
**
** The third and fourth arguments to the auth function are the name of
** the table and the column that are being accessed.  The auth function
** should return either SQLITE_OK, SQLITE_DENY, or SQLITE_IGNORE.  If
** SQLITE_OK is returned, it means that access is allowed.  SQLITE_DENY
** means that the SQL statement will never-run - the sqlite_exec() call
** will return with an error.  SQLITE_IGNORE means that the SQL statement
** should run but attempts to read the specified column will return NULL
** and attempts to write the column will be ignored.
**
** Setting the auth function to NULL disables this hook.  The default
** setting of the auth function is NULL.
*/
int sqlite_set_authorizer(
  sqlite *db,
  int (*xAuth)(void*,int,const char*,const char*),
  void *pArg
){
  db->xAuth = xAuth;
  db->pAuthArg = pArg;
  return SQLITE_OK;
}

/*
** Write an error message into pParse->zErrMsg that explains that the
** user-supplied authorization function returned an illegal value.
*/
static void sqliteAuthBadReturnCode(Parse *pParse, int rc){
  char zBuf[20];
  sprintf(zBuf, "(%d)", rc);
  sqliteSetString(&pParse->zErrMsg, "illegal return value ", zBuf,
    " from the authorization function - should be SQLITE_OK, "
    "SQLITE_IGNORE, or SQLITE_DENY", 0);
  pParse->nErr++;
  pParse->rc = SQLITE_MISUSE;
}

/*
** The pExpr should be a TK_COLUMN expression.  The table referred to
** is in pTabList with an offset of base.  Check to see if it is OK to read
** this particular column.
**
** If the auth function returns SQLITE_IGNORE, change the TK_COLUMN 
** instruction into a TK_NULL.  If the auth function returns SQLITE_DENY,
** then generate an error.
*/
void sqliteAuthRead(
  Parse *pParse,        /* The parser context */
  Expr *pExpr,          /* The expression to check authorization on */
  SrcList *pTabList,    /* All table that pExpr might refer to */
  int base              /* Offset of pTabList relative to pExpr */
){
  sqlite *db = pParse->db;
  int rc;
  Table *pTab;
  const char *zCol;
  if( db->xAuth==0 ) return;
  assert( pExpr->op==TK_COLUMN );
  assert( pExpr->iTable>=base && pExpr->iTable<base+pTabList->nSrc );
  pTab = pTabList->a[pExpr->iTable-base].pTab;
  if( pTab==0 ) return;
  if( pExpr->iColumn>=0 ){
    assert( pExpr->iColumn<pTab->nCol );
    zCol = pTab->aCol[pExpr->iColumn].zName;
  }else if( pTab->iPKey>=0 ){
    assert( pTab->iPKey<pTab->nCol );
    zCol = pTab->aCol[pTab->iPKey].zName;
  }else{
    zCol = "ROWID";
  }
  rc = db->xAuth(db->pAuthArg, SQLITE_READ, pTab->zName, zCol);
  if( rc==SQLITE_IGNORE ){
    pExpr->op = TK_NULL;
  }else if( rc==SQLITE_DENY ){
    sqliteSetString(&pParse->zErrMsg,"access to ",
        pTab->zName, ".", zCol, " is prohibited", 0);
    pParse->nErr++;
    pParse->rc = SQLITE_AUTH;
  }else if( rc!=SQLITE_OK ){
    sqliteAuthBadReturnCode(pParse, rc);
  }
}

/*
** Do an authorization check using the code and arguments given.  Return
** either SQLITE_OK (zero) or SQLITE_IGNORE or SQLITE_DENY.  If SQLITE_DENY
** is returned, then the error count and error message in pParse are
** modified appropriately.
*/
int sqliteAuthCheck(
  Parse *pParse,
  int code,
  const char *zArg1,
  const char *zArg2
){
  sqlite *db = pParse->db;
  int rc;
  if( db->xAuth==0 ){
    return SQLITE_OK;
  }
  rc = db->xAuth(db->pAuthArg, code, zArg1, zArg2);
  if( rc==SQLITE_DENY ){
    sqliteSetString(&pParse->zErrMsg, "not authorized", 0);
    pParse->rc = SQLITE_AUTH;
    pParse->nErr++;
  }else if( rc!=SQLITE_OK && rc!=SQLITE_IGNORE ){
    rc = SQLITE_DENY;
    sqliteAuthBadReturnCode(pParse, rc);
  }
  return rc;
}

#endif /* SQLITE_OMIT_AUTHORIZATION */
