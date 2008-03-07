/*
** 2005 May 25
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the implementation of the sqlite3_prepare()
** interface, and routines that contribute to loading the database schema
** from disk.
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

/*
** Fill the InitData structure with an error message that indicates
** that the database is corrupt.
*/
static void corruptSchema(InitData *pData, const char *zExtra){
  if( !pData->db->mallocFailed ){
    sqlite3SetString(pData->pzErrMsg, "malformed database schema",
       zExtra!=0 && zExtra[0]!=0 ? " - " : (char*)0, zExtra, (char*)0);
  }
  pData->rc = SQLITE_CORRUPT;
}

/*
** This is the callback routine for the code that initializes the
** database.  See sqlite3Init() below for additional information.
** This routine is also called from the OP_ParseSchema opcode of the VDBE.
**
** Each callback contains the following information:
**
**     argv[0] = name of thing being created
**     argv[1] = root page number for table or index. 0 for trigger or view.
**     argv[2] = SQL text for the CREATE statement.
**
*/
int sqlite3InitCallback(void *pInit, int argc, char **argv, char **azColName){
  InitData *pData = (InitData*)pInit;
  sqlite3 *db = pData->db;
  int iDb = pData->iDb;

  assert( sqlite3_mutex_held(db->mutex) );
  pData->rc = SQLITE_OK;
  DbClearProperty(db, iDb, DB_Empty);
  if( db->mallocFailed ){
    corruptSchema(pData, 0);
    return SQLITE_NOMEM;
  }

  assert( argc==3 );
  if( argv==0 ) return 0;   /* Might happen if EMPTY_RESULT_CALLBACKS are on */
  if( argv[1]==0 ){
    corruptSchema(pData, 0);
    return 1;
  }
  assert( iDb>=0 && iDb<db->nDb );
  if( argv[2] && argv[2][0] ){
    /* Call the parser to process a CREATE TABLE, INDEX or VIEW.
    ** But because db->init.busy is set to 1, no VDBE code is generated
    ** or executed.  All the parser does is build the internal data
    ** structures that describe the table, index, or view.
    */
    char *zErr;
    int rc;
    assert( db->init.busy );
    db->init.iDb = iDb;
    db->init.newTnum = atoi(argv[1]);
    rc = sqlite3_exec(db, argv[2], 0, 0, &zErr);
    db->init.iDb = 0;
    assert( rc!=SQLITE_OK || zErr==0 );
    if( SQLITE_OK!=rc ){
      pData->rc = rc;
      if( rc==SQLITE_NOMEM ){
        db->mallocFailed = 1;
      }else if( rc!=SQLITE_INTERRUPT ){
        corruptSchema(pData, zErr);
      }
      sqlite3_free(zErr);
      return 1;
    }
  }else if( argv[0]==0 ){
    corruptSchema(pData, 0);
  }else{
    /* If the SQL column is blank it means this is an index that
    ** was created to be the PRIMARY KEY or to fulfill a UNIQUE
    ** constraint for a CREATE TABLE.  The index should have already
    ** been created when we processed the CREATE TABLE.  All we have
    ** to do here is record the root page number for that index.
    */
    Index *pIndex;
    pIndex = sqlite3FindIndex(db, argv[0], db->aDb[iDb].zName);
    if( pIndex==0 || pIndex->tnum!=0 ){
      /* This can occur if there exists an index on a TEMP table which
      ** has the same name as another index on a permanent index.  Since
      ** the permanent table is hidden by the TEMP table, we can also
      ** safely ignore the index on the permanent table.
      */
      /* Do Nothing */;
    }else{
      pIndex->tnum = atoi(argv[1]);
    }
  }
  return 0;
}

/*
** Attempt to read the database schema and initialize internal
** data structures for a single database file.  The index of the
** database file is given by iDb.  iDb==0 is used for the main
** database.  iDb==1 should never be used.  iDb>=2 is used for
** auxiliary databases.  Return one of the SQLITE_ error codes to
** indicate success or failure.
*/
static int sqlite3InitOne(sqlite3 *db, int iDb, char **pzErrMsg){
  int rc;
  BtCursor *curMain;
  int size;
  Table *pTab;
  Db *pDb;
  char const *azArg[4];
  int meta[10];
  InitData initData;
  char const *zMasterSchema;
  char const *zMasterName = SCHEMA_TABLE(iDb);

  /*
  ** The master database table has a structure like this
  */
  static const char master_schema[] = 
     "CREATE TABLE sqlite_master(\n"
     "  type text,\n"
     "  name text,\n"
     "  tbl_name text,\n"
     "  rootpage integer,\n"
     "  sql text\n"
     ")"
  ;
#ifndef SQLITE_OMIT_TEMPDB
  static const char temp_master_schema[] = 
     "CREATE TEMP TABLE sqlite_temp_master(\n"
     "  type text,\n"
     "  name text,\n"
     "  tbl_name text,\n"
     "  rootpage integer,\n"
     "  sql text\n"
     ")"
  ;
#else
  #define temp_master_schema 0
#endif

  assert( iDb>=0 && iDb<db->nDb );
  assert( db->aDb[iDb].pSchema );
  assert( sqlite3_mutex_held(db->mutex) );
  assert( iDb==1 || sqlite3BtreeHoldsMutex(db->aDb[iDb].pBt) );

  /* zMasterSchema and zInitScript are set to point at the master schema
  ** and initialisation script appropriate for the database being
  ** initialised. zMasterName is the name of the master table.
  */
  if( !OMIT_TEMPDB && iDb==1 ){
    zMasterSchema = temp_master_schema;
  }else{
    zMasterSchema = master_schema;
  }
  zMasterName = SCHEMA_TABLE(iDb);

  /* Construct the schema tables.  */
  azArg[0] = zMasterName;
  azArg[1] = "1";
  azArg[2] = zMasterSchema;
  azArg[3] = 0;
  initData.db = db;
  initData.iDb = iDb;
  initData.pzErrMsg = pzErrMsg;
  (void)sqlite3SafetyOff(db);
  rc = sqlite3InitCallback(&initData, 3, (char **)azArg, 0);
  (void)sqlite3SafetyOn(db);
  if( rc ){
    rc = initData.rc;
    goto error_out;
  }
  pTab = sqlite3FindTable(db, zMasterName, db->aDb[iDb].zName);
  if( pTab ){
    pTab->readOnly = 1;
  }

  /* Create a cursor to hold the database open
  */
  pDb = &db->aDb[iDb];
  if( pDb->pBt==0 ){
    if( !OMIT_TEMPDB && iDb==1 ){
      DbSetProperty(db, 1, DB_SchemaLoaded);
    }
    return SQLITE_OK;
  }
  sqlite3BtreeEnter(pDb->pBt);
  rc = sqlite3BtreeCursor(pDb->pBt, MASTER_ROOT, 0, 0, 0, &curMain);
  if( rc!=SQLITE_OK && rc!=SQLITE_EMPTY ){
    sqlite3SetString(pzErrMsg, sqlite3ErrStr(rc), (char*)0);
    sqlite3BtreeLeave(pDb->pBt);
    goto error_out;
  }

  /* Get the database meta information.
  **
  ** Meta values are as follows:
  **    meta[0]   Schema cookie.  Changes with each schema change.
  **    meta[1]   File format of schema layer.
  **    meta[2]   Size of the page cache.
  **    meta[3]   Use freelist if 0.  Autovacuum if greater than zero.
  **    meta[4]   Db text encoding. 1:UTF-8 2:UTF-16LE 3:UTF-16BE
  **    meta[5]   The user cookie. Used by the application.
  **    meta[6]   Incremental-vacuum flag.
  **    meta[7]
  **    meta[8]
  **    meta[9]
  **
  ** Note: The #defined SQLITE_UTF* symbols in sqliteInt.h correspond to
  ** the possible values of meta[4].
  */
  if( rc==SQLITE_OK ){
    int i;
    for(i=0; rc==SQLITE_OK && i<sizeof(meta)/sizeof(meta[0]); i++){
      rc = sqlite3BtreeGetMeta(pDb->pBt, i+1, (u32 *)&meta[i]);
    }
    if( rc ){
      sqlite3SetString(pzErrMsg, sqlite3ErrStr(rc), (char*)0);
      sqlite3BtreeCloseCursor(curMain);
      sqlite3BtreeLeave(pDb->pBt);
      goto error_out;
    }
  }else{
    memset(meta, 0, sizeof(meta));
  }
  pDb->pSchema->schema_cookie = meta[0];

  /* If opening a non-empty database, check the text encoding. For the
  ** main database, set sqlite3.enc to the encoding of the main database.
  ** For an attached db, it is an error if the encoding is not the same
  ** as sqlite3.enc.
  */
  if( meta[4] ){  /* text encoding */
    if( iDb==0 ){
      /* If opening the main database, set ENC(db). */
      ENC(db) = (u8)meta[4];
      db->pDfltColl = sqlite3FindCollSeq(db, SQLITE_UTF8, "BINARY", 6, 0);
    }else{
      /* If opening an attached database, the encoding much match ENC(db) */
      if( meta[4]!=ENC(db) ){
        sqlite3BtreeCloseCursor(curMain);
        sqlite3SetString(pzErrMsg, "attached databases must use the same"
            " text encoding as main database", (char*)0);
        sqlite3BtreeLeave(pDb->pBt);
        return SQLITE_ERROR;
      }
    }
  }else{
    DbSetProperty(db, iDb, DB_Empty);
  }
  pDb->pSchema->enc = ENC(db);

  size = meta[2];
  if( size==0 ){ size = SQLITE_DEFAULT_CACHE_SIZE; }
  if( size<0 ) size = -size;
  pDb->pSchema->cache_size = size;
  sqlite3BtreeSetCacheSize(pDb->pBt, pDb->pSchema->cache_size);

  /*
  ** file_format==1    Version 3.0.0.
  ** file_format==2    Version 3.1.3.  // ALTER TABLE ADD COLUMN
  ** file_format==3    Version 3.1.4.  // ditto but with non-NULL defaults
  ** file_format==4    Version 3.3.0.  // DESC indices.  Boolean constants
  */
  pDb->pSchema->file_format = meta[1];
  if( pDb->pSchema->file_format==0 ){
    pDb->pSchema->file_format = 1;
  }
  if( pDb->pSchema->file_format>SQLITE_MAX_FILE_FORMAT ){
    sqlite3BtreeCloseCursor(curMain);
    sqlite3SetString(pzErrMsg, "unsupported file format", (char*)0);
    sqlite3BtreeLeave(pDb->pBt);
    return SQLITE_ERROR;
  }

  /* Ticket #2804:  When we open a database in the newer file format,
  ** clear the legacy_file_format pragma flag so that a VACUUM will
  ** not downgrade the database and thus invalidate any descending
  ** indices that the user might have created.
  */
  if( iDb==0 && meta[1]>=4 ){
    db->flags &= ~SQLITE_LegacyFileFmt;
  }

  /* Read the schema information out of the schema tables
  */
  assert( db->init.busy );
  if( rc==SQLITE_EMPTY ){
    /* For an empty database, there is nothing to read */
    rc = SQLITE_OK;
  }else{
    char *zSql;
    zSql = sqlite3MPrintf(db, 
        "SELECT name, rootpage, sql FROM '%q'.%s",
        db->aDb[iDb].zName, zMasterName);
    (void)sqlite3SafetyOff(db);
#ifndef SQLITE_OMIT_AUTHORIZATION
    {
      int (*xAuth)(void*,int,const char*,const char*,const char*,const char*);
      xAuth = db->xAuth;
      db->xAuth = 0;
#endif
      rc = sqlite3_exec(db, zSql, sqlite3InitCallback, &initData, 0);
#ifndef SQLITE_OMIT_AUTHORIZATION
      db->xAuth = xAuth;
    }
#endif
    if( rc==SQLITE_ABORT ) rc = initData.rc;
    (void)sqlite3SafetyOn(db);
    sqlite3_free(zSql);
#ifndef SQLITE_OMIT_ANALYZE
    if( rc==SQLITE_OK ){
      sqlite3AnalysisLoad(db, iDb);
    }
#endif
    sqlite3BtreeCloseCursor(curMain);
  }
  if( db->mallocFailed ){
    /* sqlite3SetString(pzErrMsg, "out of memory", (char*)0); */
    rc = SQLITE_NOMEM;
    sqlite3ResetInternalSchema(db, 0);
  }
  if( rc==SQLITE_OK || (db->flags&SQLITE_RecoveryMode)){
    /* Black magic: If the SQLITE_RecoveryMode flag is set, then consider
    ** the schema loaded, even if errors occured. In this situation the 
    ** current sqlite3_prepare() operation will fail, but the following one
    ** will attempt to compile the supplied statement against whatever subset
    ** of the schema was loaded before the error occured. The primary
    ** purpose of this is to allow access to the sqlite_master table
    ** even when its contents have been corrupted.
    */
    DbSetProperty(db, iDb, DB_SchemaLoaded);
    rc = SQLITE_OK;
  }
  sqlite3BtreeLeave(pDb->pBt);

error_out:
  if( rc==SQLITE_NOMEM || rc==SQLITE_IOERR_NOMEM ){
    db->mallocFailed = 1;
  }
  return rc;
}

/*
** Initialize all database files - the main database file, the file
** used to store temporary tables, and any additional database files
** created using ATTACH statements.  Return a success code.  If an
** error occurs, write an error message into *pzErrMsg.
**
** After a database is initialized, the DB_SchemaLoaded bit is set
** bit is set in the flags field of the Db structure. If the database
** file was of zero-length, then the DB_Empty flag is also set.
*/
int sqlite3Init(sqlite3 *db, char **pzErrMsg){
  int i, rc;
  int commit_internal = !(db->flags&SQLITE_InternChanges);
  
  assert( sqlite3_mutex_held(db->mutex) );
  if( db->init.busy ) return SQLITE_OK;
  rc = SQLITE_OK;
  db->init.busy = 1;
  for(i=0; rc==SQLITE_OK && i<db->nDb; i++){
    if( DbHasProperty(db, i, DB_SchemaLoaded) || i==1 ) continue;
    rc = sqlite3InitOne(db, i, pzErrMsg);
    if( rc ){
      sqlite3ResetInternalSchema(db, i);
    }
  }

  /* Once all the other databases have been initialised, load the schema
  ** for the TEMP database. This is loaded last, as the TEMP database
  ** schema may contain references to objects in other databases.
  */
#ifndef SQLITE_OMIT_TEMPDB
  if( rc==SQLITE_OK && db->nDb>1 && !DbHasProperty(db, 1, DB_SchemaLoaded) ){
    rc = sqlite3InitOne(db, 1, pzErrMsg);
    if( rc ){
      sqlite3ResetInternalSchema(db, 1);
    }
  }
#endif

  db->init.busy = 0;
  if( rc==SQLITE_OK && commit_internal ){
    sqlite3CommitInternalChanges(db);
  }

  return rc; 
}

/*
** This routine is a no-op if the database schema is already initialised.
** Otherwise, the schema is loaded. An error code is returned.
*/
int sqlite3ReadSchema(Parse *pParse){
  int rc = SQLITE_OK;
  sqlite3 *db = pParse->db;
  assert( sqlite3_mutex_held(db->mutex) );
  if( !db->init.busy ){
    rc = sqlite3Init(db, &pParse->zErrMsg);
  }
  if( rc!=SQLITE_OK ){
    pParse->rc = rc;
    pParse->nErr++;
  }
  return rc;
}


/*
** Check schema cookies in all databases.  If any cookie is out
** of date, return 0.  If all schema cookies are current, return 1.
*/
static int schemaIsValid(sqlite3 *db){
  int iDb;
  int rc;
  BtCursor *curTemp;
  int cookie;
  int allOk = 1;

  assert( sqlite3_mutex_held(db->mutex) );
  for(iDb=0; allOk && iDb<db->nDb; iDb++){
    Btree *pBt;
    pBt = db->aDb[iDb].pBt;
    if( pBt==0 ) continue;
    rc = sqlite3BtreeCursor(pBt, MASTER_ROOT, 0, 0, 0, &curTemp);
    if( rc==SQLITE_OK ){
      rc = sqlite3BtreeGetMeta(pBt, 1, (u32 *)&cookie);
      if( rc==SQLITE_OK && cookie!=db->aDb[iDb].pSchema->schema_cookie ){
        allOk = 0;
      }
      sqlite3BtreeCloseCursor(curTemp);
    }
    if( rc==SQLITE_NOMEM || rc==SQLITE_IOERR_NOMEM ){
      db->mallocFailed = 1;
    }
  }
  return allOk;
}

/*
** Convert a schema pointer into the iDb index that indicates
** which database file in db->aDb[] the schema refers to.
**
** If the same database is attached more than once, the first
** attached database is returned.
*/
int sqlite3SchemaToIndex(sqlite3 *db, Schema *pSchema){
  int i = -1000000;

  /* If pSchema is NULL, then return -1000000. This happens when code in 
  ** expr.c is trying to resolve a reference to a transient table (i.e. one
  ** created by a sub-select). In this case the return value of this 
  ** function should never be used.
  **
  ** We return -1000000 instead of the more usual -1 simply because using
  ** -1000000 as incorrectly using -1000000 index into db->aDb[] is much 
  ** more likely to cause a segfault than -1 (of course there are assert()
  ** statements too, but it never hurts to play the odds).
  */
  assert( sqlite3_mutex_held(db->mutex) );
  if( pSchema ){
    for(i=0; i<db->nDb; i++){
      if( db->aDb[i].pSchema==pSchema ){
        break;
      }
    }
    assert( i>=0 &&i>=0 &&  i<db->nDb );
  }
  return i;
}

/*
** Compile the UTF-8 encoded SQL statement zSql into a statement handle.
*/
int sqlite3Prepare(
  sqlite3 *db,              /* Database handle. */
  const char *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  int saveSqlFlag,          /* True to copy SQL text into the sqlite3_stmt */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const char **pzTail       /* OUT: End of parsed string */
){
  Parse sParse;
  char *zErrMsg = 0;
  int rc = SQLITE_OK;
  int i;

  assert( ppStmt );
  *ppStmt = 0;
  if( sqlite3SafetyOn(db) ){
    return SQLITE_MISUSE;
  }
  assert( !db->mallocFailed );
  assert( sqlite3_mutex_held(db->mutex) );

  /* If any attached database schemas are locked, do not proceed with
  ** compilation. Instead return SQLITE_LOCKED immediately.
  */
  for(i=0; i<db->nDb; i++) {
    Btree *pBt = db->aDb[i].pBt;
    if( pBt ){
      int rc;
      rc = sqlite3BtreeSchemaLocked(pBt);
      if( rc ){
        const char *zDb = db->aDb[i].zName;
        sqlite3Error(db, SQLITE_LOCKED, "database schema is locked: %s", zDb);
        (void)sqlite3SafetyOff(db);
        return SQLITE_LOCKED;
      }
    }
  }
  
  memset(&sParse, 0, sizeof(sParse));
  sParse.db = db;
  if( nBytes>=0 && zSql[nBytes]!=0 ){
    char *zSqlCopy;
    if( SQLITE_MAX_SQL_LENGTH>0 && nBytes>SQLITE_MAX_SQL_LENGTH ){
      sqlite3Error(db, SQLITE_TOOBIG, "statement too long");
      (void)sqlite3SafetyOff(db);
      return SQLITE_TOOBIG;
    }
    zSqlCopy = sqlite3DbStrNDup(db, zSql, nBytes);
    if( zSqlCopy ){
      sqlite3RunParser(&sParse, zSqlCopy, &zErrMsg);
      sqlite3_free(zSqlCopy);
    }
    sParse.zTail = &zSql[nBytes];
  }else{
    sqlite3RunParser(&sParse, zSql, &zErrMsg);
  }

  if( db->mallocFailed ){
    sParse.rc = SQLITE_NOMEM;
  }
  if( sParse.rc==SQLITE_DONE ) sParse.rc = SQLITE_OK;
  if( sParse.checkSchema && !schemaIsValid(db) ){
    sParse.rc = SQLITE_SCHEMA;
  }
  if( sParse.rc==SQLITE_SCHEMA ){
    sqlite3ResetInternalSchema(db, 0);
  }
  if( db->mallocFailed ){
    sParse.rc = SQLITE_NOMEM;
  }
  if( pzTail ){
    *pzTail = sParse.zTail;
  }
  rc = sParse.rc;

#ifndef SQLITE_OMIT_EXPLAIN
  if( rc==SQLITE_OK && sParse.pVdbe && sParse.explain ){
    if( sParse.explain==2 ){
      sqlite3VdbeSetNumCols(sParse.pVdbe, 3);
      sqlite3VdbeSetColName(sParse.pVdbe, 0, COLNAME_NAME, "order", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 1, COLNAME_NAME, "from", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 2, COLNAME_NAME, "detail", P4_STATIC);
    }else{
      sqlite3VdbeSetNumCols(sParse.pVdbe, 8);
      sqlite3VdbeSetColName(sParse.pVdbe, 0, COLNAME_NAME, "addr", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 1, COLNAME_NAME, "opcode", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 2, COLNAME_NAME, "p1", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 3, COLNAME_NAME, "p2", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 4, COLNAME_NAME, "p3", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 5, COLNAME_NAME, "p4", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 6, COLNAME_NAME, "p5", P4_STATIC);
      sqlite3VdbeSetColName(sParse.pVdbe, 7, COLNAME_NAME, "comment",P4_STATIC);
    }
  }
#endif

  if( sqlite3SafetyOff(db) ){
    rc = SQLITE_MISUSE;
  }

  if( saveSqlFlag ){
    sqlite3VdbeSetSql(sParse.pVdbe, zSql, sParse.zTail - zSql);
  }
  if( rc!=SQLITE_OK || db->mallocFailed ){
    sqlite3_finalize((sqlite3_stmt*)sParse.pVdbe);
    assert(!(*ppStmt));
  }else{
    *ppStmt = (sqlite3_stmt*)sParse.pVdbe;
  }

  if( zErrMsg ){
    sqlite3Error(db, rc, "%s", zErrMsg);
    sqlite3_free(zErrMsg);
  }else{
    sqlite3Error(db, rc, 0);
  }

  rc = sqlite3ApiExit(db, rc);
  assert( (rc&db->errMask)==rc );
  return rc;
}
static int sqlite3LockAndPrepare(
  sqlite3 *db,              /* Database handle. */
  const char *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  int saveSqlFlag,          /* True to copy SQL text into the sqlite3_stmt */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const char **pzTail       /* OUT: End of parsed string */
){
  int rc;
  if( !sqlite3SafetyCheckOk(db) ){
    return SQLITE_MISUSE;
  }
  sqlite3_mutex_enter(db->mutex);
  sqlite3BtreeEnterAll(db);
  rc = sqlite3Prepare(db, zSql, nBytes, saveSqlFlag, ppStmt, pzTail);
  sqlite3BtreeLeaveAll(db);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** Rerun the compilation of a statement after a schema change.
** Return true if the statement was recompiled successfully.
** Return false if there is an error of some kind.
*/
int sqlite3Reprepare(Vdbe *p){
  int rc;
  sqlite3_stmt *pNew;
  const char *zSql;
  sqlite3 *db;

  assert( sqlite3_mutex_held(sqlite3VdbeDb(p)->mutex) );
  zSql = sqlite3_sql((sqlite3_stmt *)p);
  assert( zSql!=0 );  /* Reprepare only called for prepare_v2() statements */
  db = sqlite3VdbeDb(p);
  assert( sqlite3_mutex_held(db->mutex) );
  rc = sqlite3LockAndPrepare(db, zSql, -1, 0, &pNew, 0);
  if( rc ){
    if( rc==SQLITE_NOMEM ){
      db->mallocFailed = 1;
    }
    assert( pNew==0 );
    return 0;
  }else{
    assert( pNew!=0 );
  }
  sqlite3VdbeSwap((Vdbe*)pNew, p);
  sqlite3_transfer_bindings(pNew, (sqlite3_stmt*)p);
  sqlite3VdbeResetStepResult((Vdbe*)pNew);
  sqlite3VdbeFinalize((Vdbe*)pNew);
  return 1;
}


/*
** Two versions of the official API.  Legacy and new use.  In the legacy
** version, the original SQL text is not saved in the prepared statement
** and so if a schema change occurs, SQLITE_SCHEMA is returned by
** sqlite3_step().  In the new version, the original SQL text is retained
** and the statement is automatically recompiled if an schema change
** occurs.
*/
int sqlite3_prepare(
  sqlite3 *db,              /* Database handle. */
  const char *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const char **pzTail       /* OUT: End of parsed string */
){
  return sqlite3LockAndPrepare(db,zSql,nBytes,0,ppStmt,pzTail);
}
int sqlite3_prepare_v2(
  sqlite3 *db,              /* Database handle. */
  const char *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const char **pzTail       /* OUT: End of parsed string */
){
  return sqlite3LockAndPrepare(db,zSql,nBytes,1,ppStmt,pzTail);
}


#ifndef SQLITE_OMIT_UTF16
/*
** Compile the UTF-16 encoded SQL statement zSql into a statement handle.
*/
static int sqlite3Prepare16(
  sqlite3 *db,              /* Database handle. */ 
  const void *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  int saveSqlFlag,          /* True to save SQL text into the sqlite3_stmt */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const void **pzTail       /* OUT: End of parsed string */
){
  /* This function currently works by first transforming the UTF-16
  ** encoded string to UTF-8, then invoking sqlite3_prepare(). The
  ** tricky bit is figuring out the pointer to return in *pzTail.
  */
  char *zSql8;
  const char *zTail8 = 0;
  int rc = SQLITE_OK;

  if( !sqlite3SafetyCheckOk(db) ){
    return SQLITE_MISUSE;
  }
  sqlite3_mutex_enter(db->mutex);
  zSql8 = sqlite3Utf16to8(db, zSql, nBytes);
  if( zSql8 ){
    rc = sqlite3LockAndPrepare(db, zSql8, -1, saveSqlFlag, ppStmt, &zTail8);
  }

  if( zTail8 && pzTail ){
    /* If sqlite3_prepare returns a tail pointer, we calculate the
    ** equivalent pointer into the UTF-16 string by counting the unicode
    ** characters between zSql8 and zTail8, and then returning a pointer
    ** the same number of characters into the UTF-16 string.
    */
    int chars_parsed = sqlite3Utf8CharLen(zSql8, zTail8-zSql8);
    *pzTail = (u8 *)zSql + sqlite3Utf16ByteLen(zSql, chars_parsed);
  }
  sqlite3_free(zSql8); 
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** Two versions of the official API.  Legacy and new use.  In the legacy
** version, the original SQL text is not saved in the prepared statement
** and so if a schema change occurs, SQLITE_SCHEMA is returned by
** sqlite3_step().  In the new version, the original SQL text is retained
** and the statement is automatically recompiled if an schema change
** occurs.
*/
int sqlite3_prepare16(
  sqlite3 *db,              /* Database handle. */ 
  const void *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const void **pzTail       /* OUT: End of parsed string */
){
  return sqlite3Prepare16(db,zSql,nBytes,0,ppStmt,pzTail);
}
int sqlite3_prepare16_v2(
  sqlite3 *db,              /* Database handle. */ 
  const void *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const void **pzTail       /* OUT: End of parsed string */
){
  return sqlite3Prepare16(db,zSql,nBytes,1,ppStmt,pzTail);
}

#endif /* SQLITE_OMIT_UTF16 */
