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
void sqliteAttach(Parse *pParse, Token *pFilename, Token *pDbname){
  Db *aNew;
  int rc, i;
  char *zFile, *zName;
  sqlite *db;

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
  
  zName = 0;
  sqliteSetNString(&zName, pDbname->z, pDbname->n, 0);
  if( zName==0 ) return;
  sqliteDequote(zName);
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].zName && sqliteStrICmp(db->aDb[i].zName, zName)==0 ){
      sqliteErrorMsg(pParse, "database %z is already in use", zName);
      db->nDb--;
      pParse->rc = SQLITE_ERROR;
      return;
    }
  }
  aNew->zName = zName;
  zFile = 0;
  sqliteSetNString(&zFile, pFilename->z, pFilename->n, 0);
  if( zFile==0 ) return;
  sqliteDequote(zFile);
  rc = sqliteBtreeFactory(db, zFile, 0, MAX_PAGES, &aNew->pBt);
  if( rc ){
    sqliteErrorMsg(pParse, "unable to open database: %s", zFile);
  }
  sqliteFree(zFile);
  db->flags &= ~SQLITE_Initialized;
  if( pParse->nErr ) return;
  rc = sqliteInit(pParse->db, &pParse->zErrMsg);
  if( rc ){
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

  if( pParse->explain ) return;
  db = pParse->db;
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].pBt==0 || db->aDb[i].zName==0 ) continue;
    if( strlen(db->aDb[i].zName)!=pDbname->n ) continue;
    if( sqliteStrNICmp(db->aDb[i].zName, pDbname->z, pDbname->n)==0 ) break;
  }
  if( i>=db->nDb ){
    sqliteErrorMsg(pParse, "no such database: %T", pDbname);
    return;
  }
  if( i<2 ){
    sqliteErrorMsg(pParse, "cannot detach database %T", pDbname);
    return;
  }
  sqliteBtreeClose(db->aDb[i].pBt);
  db->aDb[i].pBt = 0;
  sqliteFree(db->aDb[i].zName);
  sqliteResetInternalSchema(db, i);
  db->nDb--;
  if( i<db->nDb ){
    db->aDb[i] = db->aDb[db->nDb];
    memset(&db->aDb[db->nDb], 0, sizeof(db->aDb[0]));
    sqliteResetInternalSchema(db, i);
  }
}
