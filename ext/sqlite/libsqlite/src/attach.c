/*
** 2003 April 6
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used to implement the ATTACH and DETACH commands.
**
** $Id$
*/
#include "sqliteInt.h"

/*
** This routine is called by the parser to process an ATTACH statement:
**
**     ATTACH DATABASE filename AS dbname
**
** The pFilename and pDbname arguments are the tokens that define the
** filename and dbname in the ATTACH statement.
*/
void sqliteAttach(Parse *pParse, Token *pFilename, Token *pDbname, Token *pKey){
  Db *aNew;
  int rc, i;
  char *zFile, *zName;
  sqlite *db;
  Vdbe *v;

  v = sqliteGetVdbe(pParse);
  sqliteVdbeAddOp(v, OP_Halt, 0, 0);
  if( pParse->explain ) return;
  db = pParse->db;
  if( db->file_format<4 ){
    sqliteErrorMsg(pParse, "cannot attach auxiliary databases to an "
       "older format master database", 0);
    pParse->rc = SQLITE_ERROR;
    return;
  }
  if( db->nDb>=MAX_ATTACHED+2 ){
    sqliteErrorMsg(pParse, "too many attached databases - max %d", 
       MAX_ATTACHED);
    pParse->rc = SQLITE_ERROR;
    return;
  }

  zFile = 0;
  sqliteSetNString(&zFile, pFilename->z, pFilename->n, 0);
  if( zFile==0 ) return;
  sqliteDequote(zFile);
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqliteAuthCheck(pParse, SQLITE_ATTACH, zFile, 0, 0)!=SQLITE_OK ){
    sqliteFree(zFile);
    return;
  }
#endif /* SQLITE_OMIT_AUTHORIZATION */

  zName = 0;
  sqliteSetNString(&zName, pDbname->z, pDbname->n, 0);
  if( zName==0 ) return;
  sqliteDequote(zName);
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].zName && sqliteStrICmp(db->aDb[i].zName, zName)==0 ){
      sqliteErrorMsg(pParse, "database %z is already in use", zName);
      pParse->rc = SQLITE_ERROR;
      sqliteFree(zFile);
      return;
    }
  }

  if( db->aDb==db->aDbStatic ){
    aNew = sqliteMalloc( sizeof(db->aDb[0])*3 );
    if( aNew==0 ) return;
    memcpy(aNew, db->aDb, sizeof(db->aDb[0])*2);
  }else{
    aNew = sqliteRealloc(db->aDb, sizeof(db->aDb[0])*(db->nDb+1) );
    if( aNew==0 ) return;
  }
  db->aDb = aNew;
  aNew = &db->aDb[db->nDb++];
  memset(aNew, 0, sizeof(*aNew));
  sqliteHashInit(&aNew->tblHash, SQLITE_HASH_STRING, 0);
  sqliteHashInit(&aNew->idxHash, SQLITE_HASH_STRING, 0);
  sqliteHashInit(&aNew->trigHash, SQLITE_HASH_STRING, 0);
  sqliteHashInit(&aNew->aFKey, SQLITE_HASH_STRING, 1);
  aNew->zName = zName;
  rc = sqliteBtreeFactory(db, zFile, 0, MAX_PAGES, &aNew->pBt);
  if( rc ){
    sqliteErrorMsg(pParse, "unable to open database: %s", zFile);
  }
#if SQLITE_HAS_CODEC
  {
    extern int sqliteCodecAttach(sqlite*, int, void*, int);
    char *zKey = 0;
    int nKey;
    if( pKey && pKey->z && pKey->n ){
      sqliteSetNString(&zKey, pKey->z, pKey->n, 0);
      sqliteDequote(zKey);
      nKey = strlen(zKey);
    }else{
      zKey = 0;
      nKey = 0;
    }
    sqliteCodecAttach(db, db->nDb-1, zKey, nKey);
  }
#endif
  sqliteFree(zFile);
  db->flags &= ~SQLITE_Initialized;
  if( pParse->nErr ) return;
  if( rc==SQLITE_OK ){
    rc = sqliteInit(pParse->db, &pParse->zErrMsg);
  }
  if( rc ){
    int i = db->nDb - 1;
    assert( i>=2 );
    if( db->aDb[i].pBt ){
      sqliteBtreeClose(db->aDb[i].pBt);
      db->aDb[i].pBt = 0;
    }
    sqliteResetInternalSchema(db, 0);
    pParse->nErr++;
    pParse->rc = SQLITE_ERROR;
  }
}

/*
** This routine is called by the parser to process a DETACH statement:
**
**    DETACH DATABASE dbname
**
** The pDbname argument is the name of the database in the DETACH statement.
*/
void sqliteDetach(Parse *pParse, Token *pDbname){
  int i;
  sqlite *db;
  Vdbe *v;
  Db *pDb;

  v = sqliteGetVdbe(pParse);
  sqliteVdbeAddOp(v, OP_Halt, 0, 0);
  if( pParse->explain ) return;
  db = pParse->db;
  for(i=0; i<db->nDb; i++){
    pDb = &db->aDb[i];
    if( pDb->pBt==0 || pDb->zName==0 ) continue;
    if( strlen(pDb->zName)!=pDbname->n ) continue;
    if( sqliteStrNICmp(pDb->zName, pDbname->z, pDbname->n)==0 ) break;
  }
  if( i>=db->nDb ){
    sqliteErrorMsg(pParse, "no such database: %T", pDbname);
    return;
  }
  if( i<2 ){
    sqliteErrorMsg(pParse, "cannot detach database %T", pDbname);
    return;
  }
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqliteAuthCheck(pParse,SQLITE_DETACH,db->aDb[i].zName,0,0)!=SQLITE_OK ){
    return;
  }
#endif /* SQLITE_OMIT_AUTHORIZATION */
  sqliteBtreeClose(pDb->pBt);
  pDb->pBt = 0;
  sqliteFree(pDb->zName);
  sqliteResetInternalSchema(db, i);
  if( pDb->pAux && pDb->xFreeAux ) pDb->xFreeAux(pDb->pAux);
  db->nDb--;
  if( i<db->nDb ){
    db->aDb[i] = db->aDb[db->nDb];
    memset(&db->aDb[db->nDb], 0, sizeof(db->aDb[0]));
    sqliteResetInternalSchema(db, i);
  }
}

/*
** Initialize a DbFixer structure.  This routine must be called prior
** to passing the structure to one of the sqliteFixAAAA() routines below.
**
** The return value indicates whether or not fixation is required.  TRUE
** means we do need to fix the database references, FALSE means we do not.
*/
int sqliteFixInit(
  DbFixer *pFix,      /* The fixer to be initialized */
  Parse *pParse,      /* Error messages will be written here */
  int iDb,            /* This is the database that must must be used */
  const char *zType,  /* "view", "trigger", or "index" */
  const Token *pName  /* Name of the view, trigger, or index */
){
  sqlite *db;

  if( iDb<0 || iDb==1 ) return 0;
  db = pParse->db;
  assert( db->nDb>iDb );
  pFix->pParse = pParse;
  pFix->zDb = db->aDb[iDb].zName;
  pFix->zType = zType;
  pFix->pName = pName;
  return 1;
}

/*
** The following set of routines walk through the parse tree and assign
** a specific database to all table references where the database name
** was left unspecified in the original SQL statement.  The pFix structure
** must have been initialized by a prior call to sqliteFixInit().
**
** These routines are used to make sure that an index, trigger, or
** view in one database does not refer to objects in a different database.
** (Exception: indices, triggers, and views in the TEMP database are
** allowed to refer to anything.)  If a reference is explicitly made
** to an object in a different database, an error message is added to
** pParse->zErrMsg and these routines return non-zero.  If everything
** checks out, these routines return 0.
*/
int sqliteFixSrcList(
  DbFixer *pFix,       /* Context of the fixation */
  SrcList *pList       /* The Source list to check and modify */
){
  int i;
  const char *zDb;

  if( pList==0 ) return 0;
  zDb = pFix->zDb;
  for(i=0; i<pList->nSrc; i++){
    if( pList->a[i].zDatabase==0 ){
      pList->a[i].zDatabase = sqliteStrDup(zDb);
    }else if( sqliteStrICmp(pList->a[i].zDatabase,zDb)!=0 ){
      sqliteErrorMsg(pFix->pParse,
         "%s %z cannot reference objects in database %s",
         pFix->zType, sqliteStrNDup(pFix->pName->z, pFix->pName->n),
         pList->a[i].zDatabase);
      return 1;
    }
    if( sqliteFixSelect(pFix, pList->a[i].pSelect) ) return 1;
    if( sqliteFixExpr(pFix, pList->a[i].pOn) ) return 1;
  }
  return 0;
}
int sqliteFixSelect(
  DbFixer *pFix,       /* Context of the fixation */
  Select *pSelect      /* The SELECT statement to be fixed to one database */
){
  while( pSelect ){
    if( sqliteFixExprList(pFix, pSelect->pEList) ){
      return 1;
    }
    if( sqliteFixSrcList(pFix, pSelect->pSrc) ){
      return 1;
    }
    if( sqliteFixExpr(pFix, pSelect->pWhere) ){
      return 1;
    }
    if( sqliteFixExpr(pFix, pSelect->pHaving) ){
      return 1;
    }
    pSelect = pSelect->pPrior;
  }
  return 0;
}
int sqliteFixExpr(
  DbFixer *pFix,     /* Context of the fixation */
  Expr *pExpr        /* The expression to be fixed to one database */
){
  while( pExpr ){
    if( sqliteFixSelect(pFix, pExpr->pSelect) ){
      return 1;
    }
    if( sqliteFixExprList(pFix, pExpr->pList) ){
      return 1;
    }
    if( sqliteFixExpr(pFix, pExpr->pRight) ){
      return 1;
    }
    pExpr = pExpr->pLeft;
  }
  return 0;
}
int sqliteFixExprList(
  DbFixer *pFix,     /* Context of the fixation */
  ExprList *pList    /* The expression to be fixed to one database */
){
  int i;
  if( pList==0 ) return 0;
  for(i=0; i<pList->nExpr; i++){
    if( sqliteFixExpr(pFix, pList->a[i].pExpr) ){
      return 1;
    }
  }
  return 0;
}
int sqliteFixTriggerStep(
  DbFixer *pFix,     /* Context of the fixation */
  TriggerStep *pStep /* The trigger step be fixed to one database */
){
  while( pStep ){
    if( sqliteFixSelect(pFix, pStep->pSelect) ){
      return 1;
    }
    if( sqliteFixExpr(pFix, pStep->pWhere) ){
      return 1;
    }
    if( sqliteFixExprList(pFix, pStep->pExprList) ){
      return 1;
    }
    pStep = pStep->pNext;
  }
  return 0;
}
