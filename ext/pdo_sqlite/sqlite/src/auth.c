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
** This file contains code used to implement the sqlite3_set_authorizer()
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
** phase to verify that the user has read and/or write access permission on
** various fields of the database.  The first argument to the auth function
** is a copy of the 3rd argument to this routine.  The second argument
** to the auth function is one of these constants:
**
**       SQLITE_CREATE_INDEX
**       SQLITE_CREATE_TABLE
**       SQLITE_CREATE_TEMP_INDEX
**       SQLITE_CREATE_TEMP_TABLE
**       SQLITE_CREATE_TEMP_TRIGGER
**       SQLITE_CREATE_TEMP_VIEW
**       SQLITE_CREATE_TRIGGER
**       SQLITE_CREATE_VIEW
**       SQLITE_DELETE
**       SQLITE_DROP_INDEX
**       SQLITE_DROP_TABLE
**       SQLITE_DROP_TEMP_INDEX
**       SQLITE_DROP_TEMP_TABLE
**       SQLITE_DROP_TEMP_TRIGGER
**       SQLITE_DROP_TEMP_VIEW
**       SQLITE_DROP_TRIGGER
**       SQLITE_DROP_VIEW
**       SQLITE_INSERT
**       SQLITE_PRAGMA
**       SQLITE_READ
**       SQLITE_SELECT
**       SQLITE_TRANSACTION
**       SQLITE_UPDATE
**
** The third and fourth arguments to the auth function are the name of
** the table and the column that are being accessed.  The auth function
** should return either SQLITE_OK, SQLITE_DENY, or SQLITE_IGNORE.  If
** SQLITE_OK is returned, it means that access is allowed.  SQLITE_DENY
** means that the SQL statement will never-run - the sqlite3_exec() call
** will return with an error.  SQLITE_IGNORE means that the SQL statement
** should run but attempts to read the specified column will return NULL
** and attempts to write the column will be ignored.
**
** Setting the auth function to NULL disables this hook.  The default
** setting of the auth function is NULL.
*/
int sqlite3_set_authorizer(
  sqlite3 *db,
  int (*xAuth)(void*,int,const char*,const char*,const char*,const char*),
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
  sqlite3ErrorMsg(pParse, "illegal return value (%d) from the "
    "authorization function - should be SQLITE_OK, SQLITE_IGNORE, "
    "or SQLITE_DENY", rc);
  pParse->rc = SQLITE_ERROR;
}

/*
** The pExpr should be a TK_COLUMN expression.  The table referred to
** is in pTabList or else it is the NEW or OLD table of a trigger.  
** Check to see if it is OK to read this particular column.
**
** If the auth function returns SQLITE_IGNORE, change the TK_COLUMN 
** instruction into a TK_NULL.  If the auth function returns SQLITE_DENY,
** then generate an error.
*/
void sqlite3AuthRead(
  Parse *pParse,        /* The parser context */
  Expr *pExpr,          /* The expression to check authorization on */
  SrcList *pTabList     /* All table that pExpr might refer to */
){
  sqlite3 *db = pParse->db;
  int rc;
  Table *pTab;          /* The table being read */
  const char *zCol;     /* Name of the column of the table */
  int iSrc;             /* Index in pTabList->a[] of table being read */
  const char *zDBase;   /* Name of database being accessed */
  TriggerStack *pStack; /* The stack of current triggers */

  if( db->xAuth==0 ) return;
  assert( pExpr->op==TK_COLUMN );
  for(iSrc=0; iSrc<pTabList->nSrc; iSrc++){
    if( pExpr->iTable==pTabList->a[iSrc].iCursor ) break;
  }
  if( iSrc>=0 && iSrc<pTabList->nSrc ){
    pTab = pTabList->a[iSrc].pTab;
  }else if( (pStack = pParse->trigStack)!=0 ){
    /* This must be an attempt to read the NEW or OLD pseudo-tables
    ** of a trigger.
    */
    assert( pExpr->iTable==pStack->newIdx || pExpr->iTable==pStack->oldIdx );
    pTab = pStack->pTab;
  }else{
    return;
  }
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
  assert( pExpr->iDb<db->nDb );
  zDBase = db->aDb[pExpr->iDb].zName;
  rc = db->xAuth(db->pAuthArg, SQLITE_READ, pTab->zName, zCol, zDBase, 
                 pParse->zAuthContext);
  if( rc==SQLITE_IGNORE ){
    pExpr->op = TK_NULL;
  }else if( rc==SQLITE_DENY ){
    if( db->nDb>2 || pExpr->iDb!=0 ){
      sqlite3ErrorMsg(pParse, "access to %s.%s.%s is prohibited", 
         zDBase, pTab->zName, zCol);
    }else{
      sqlite3ErrorMsg(pParse, "access to %s.%s is prohibited",pTab->zName,zCol);
    }
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
int sqlite3AuthCheck(
  Parse *pParse,
  int code,
  const char *zArg1,
  const char *zArg2,
  const char *zArg3
){
  sqlite3 *db = pParse->db;
  int rc;

  /* Don't do any authorization checks if the database is initialising. */
  if( db->init.busy ){
    return SQLITE_OK;
  }

  if( db->xAuth==0 ){
    return SQLITE_OK;
  }
  rc = db->xAuth(db->pAuthArg, code, zArg1, zArg2, zArg3, pParse->zAuthContext);
  if( rc==SQLITE_DENY ){
    sqlite3ErrorMsg(pParse, "not authorized");
    pParse->rc = SQLITE_AUTH;
  }else if( rc!=SQLITE_OK && rc!=SQLITE_IGNORE ){
    rc = SQLITE_DENY;
    sqliteAuthBadReturnCode(pParse, rc);
  }
  return rc;
}

/*
** Push an authorization context.  After this routine is called, the
** zArg3 argument to authorization callbacks will be zContext until
** popped.  Or if pParse==0, this routine is a no-op.
*/
void sqlite3AuthContextPush(
  Parse *pParse,
  AuthContext *pContext, 
  const char *zContext
){
  pContext->pParse = pParse;
  if( pParse ){
    pContext->zAuthContext = pParse->zAuthContext;
    pParse->zAuthContext = zContext;
  }
}

/*
** Pop an authorization context that was previously pushed
** by sqlite3AuthContextPush
*/
void sqlite3AuthContextPop(AuthContext *pContext){
  if( pContext->pParse ){
    pContext->pParse->zAuthContext = pContext->zAuthContext;
    pContext->pParse = 0;
  }
}

#endif /* SQLITE_OMIT_AUTHORIZATION */
