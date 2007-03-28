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
** Main file for the SQLite library.  The routines in this file
** implement the programmer interface to the library.  Routines in
** other files are for internal use by SQLite and should not be
** accessed by users of the library.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>

/*
** A pointer to this structure is used to communicate information
** from sqliteInit into the sqliteInitCallback.
*/
typedef struct {
  sqlite *db;         /* The database being initialized */
  char **pzErrMsg;    /* Error message stored here */
} InitData;

/*
** Fill the InitData structure with an error message that indicates
** that the database is corrupt.
*/
static void corruptSchema(InitData *pData){
  sqliteSetString(pData->pzErrMsg, "malformed database schema", (char*)0);
}

/*
** This is the callback routine for the code that initializes the
** database.  See sqliteInit() below for additional information.
**
** Each callback contains the following information:
**
**     argv[0] = "file-format" or "schema-cookie" or "table" or "index"
**     argv[1] = table or index name or meta statement type.
**     argv[2] = root page number for table or index.  NULL for meta.
**     argv[3] = SQL text for a CREATE TABLE or CREATE INDEX statement.
**     argv[4] = "1" for temporary files, "0" for main database, "2" or more
**               for auxiliary database files.
**
*/
static
int sqliteInitCallback(void *pInit, int argc, char **argv, char **azColName){
  InitData *pData = (InitData*)pInit;
  Parse sParse;
  int nErr = 0;

  assert( argc==5 );
  if( argv==0 ) return 0;   /* Might happen if EMPTY_RESULT_CALLBACKS are on */
  if( argv[0]==0 ){
    corruptSchema(pData);
    return 1;
  }
  switch( argv[0][0] ){
    case 'v':
    case 'i':
    case 't': {  /* CREATE TABLE, CREATE INDEX, or CREATE VIEW statements */
      if( argv[2]==0 || argv[4]==0 ){
        corruptSchema(pData);
        return 1;
      }
      if( argv[3] && argv[3][0] ){
        /* Call the parser to process a CREATE TABLE, INDEX or VIEW.
        ** But because sParse.initFlag is set to 1, no VDBE code is generated
        ** or executed.  All the parser does is build the internal data
        ** structures that describe the table, index, or view.
        */
        memset(&sParse, 0, sizeof(sParse));
        sParse.db = pData->db;
        sParse.initFlag = 1;
        sParse.iDb = atoi(argv[4]);
        sParse.newTnum = atoi(argv[2]);
        sParse.useCallback = 1;
        sqliteRunParser(&sParse, argv[3], pData->pzErrMsg);
      }else{
        /* If the SQL column is blank it means this is an index that
        ** was created to be the PRIMARY KEY or to fulfill a UNIQUE
        ** constraint for a CREATE TABLE.  The index should have already
        ** been created when we processed the CREATE TABLE.  All we have
        ** to do here is record the root page number for that index.
        */
        int iDb;
        Index *pIndex;

        iDb = atoi(argv[4]);
        assert( iDb>=0 && iDb<pData->db->nDb );
        pIndex = sqliteFindIndex(pData->db, argv[1], pData->db->aDb[iDb].zName);
        if( pIndex==0 || pIndex->tnum!=0 ){
          /* This can occur if there exists an index on a TEMP table which
          ** has the same name as another index on a permanent index.  Since
          ** the permanent table is hidden by the TEMP table, we can also
          ** safely ignore the index on the permanent table.
          */
          /* Do Nothing */;
        }else{
          pIndex->tnum = atoi(argv[2]);
        }
      }
      break;
    }
    default: {
      /* This can not happen! */
      nErr = 1;
      assert( nErr==0 );
    }
  }
  return nErr;
}

/*
** This is a callback procedure used to reconstruct a table.  The
** name of the table to be reconstructed is passed in as argv[0].
**
** This routine is used to automatically upgrade a database from
** format version 1 or 2 to version 3.  The correct operation of
** this routine relys on the fact that no indices are used when
** copying a table out to a temporary file.
*/
static
int upgrade_3_callback(void *pInit, int argc, char **argv, char **NotUsed){
  InitData *pData = (InitData*)pInit;
  int rc;
  Table *pTab;
  Trigger *pTrig;
  char *zErr = 0;

  pTab = sqliteFindTable(pData->db, argv[0], 0);
  assert( pTab!=0 );
  assert( sqliteStrICmp(pTab->zName, argv[0])==0 );
  if( pTab ){
    pTrig = pTab->pTrigger;
    pTab->pTrigger = 0;  /* Disable all triggers before rebuilding the table */
  }
  rc = sqlite_exec_printf(pData->db,
    "CREATE TEMP TABLE sqlite_x AS SELECT * FROM '%q'; "
    "DELETE FROM '%q'; "
    "INSERT INTO '%q' SELECT * FROM sqlite_x; "
    "DROP TABLE sqlite_x;",
    0, 0, &zErr, argv[0], argv[0], argv[0]);
  if( zErr ){
    sqliteSetString(pData->pzErrMsg, zErr, (char*)0);
    sqlite_freemem(zErr);
  }

  /* If an error occurred in the SQL above, then the transaction will
  ** rollback which will delete the internal symbol tables.  This will
  ** cause the structure that pTab points to be deleted.  In case that
  ** happened, we need to refetch pTab.
  */
  pTab = sqliteFindTable(pData->db, argv[0], 0);
  if( pTab ){
    assert( sqliteStrICmp(pTab->zName, argv[0])==0 );
    pTab->pTrigger = pTrig;  /* Re-enable triggers */
  }
  return rc!=SQLITE_OK;
}



/*
** Attempt to read the database schema and initialize internal
** data structures for a single database file.  The index of the
** database file is given by iDb.  iDb==0 is used for the main
** database.  iDb==1 should never be used.  iDb>=2 is used for
** auxiliary databases.  Return one of the SQLITE_ error codes to
** indicate success or failure.
*/
static int sqliteInitOne(sqlite *db, int iDb, char **pzErrMsg){
  int rc;
  BtCursor *curMain;
  int size;
  Table *pTab;
  char *azArg[6];
  char zDbNum[30];
  int meta[SQLITE_N_BTREE_META];
  Parse sParse;
  InitData initData;

  /*
  ** The master database table has a structure like this
  */
  static char master_schema[] = 
     "CREATE TABLE sqlite_master(\n"
     "  type text,\n"
     "  name text,\n"
     "  tbl_name text,\n"
     "  rootpage integer,\n"
     "  sql text\n"
     ")"
  ;
  static char temp_master_schema[] = 
     "CREATE TEMP TABLE sqlite_temp_master(\n"
     "  type text,\n"
     "  name text,\n"
     "  tbl_name text,\n"
     "  rootpage integer,\n"
     "  sql text\n"
     ")"
  ;

  /* The following SQL will read the schema from the master tables.
  ** The first version works with SQLite file formats 2 or greater.
  ** The second version is for format 1 files.
  **
  ** Beginning with file format 2, the rowid for new table entries
  ** (including entries in sqlite_master) is an increasing integer.
  ** So for file format 2 and later, we can play back sqlite_master
  ** and all the CREATE statements will appear in the right order.
  ** But with file format 1, table entries were random and so we
  ** have to make sure the CREATE TABLEs occur before their corresponding
  ** CREATE INDEXs.  (We don't have to deal with CREATE VIEW or
  ** CREATE TRIGGER in file format 1 because those constructs did
  ** not exist then.) 
  */
  static char init_script[] = 
     "SELECT type, name, rootpage, sql, 1 FROM sqlite_temp_master "
     "UNION ALL "
     "SELECT type, name, rootpage, sql, 0 FROM sqlite_master";
  static char older_init_script[] = 
     "SELECT type, name, rootpage, sql, 1 FROM sqlite_temp_master "
     "UNION ALL "
     "SELECT type, name, rootpage, sql, 0 FROM sqlite_master "
     "WHERE type='table' "
     "UNION ALL "
     "SELECT type, name, rootpage, sql, 0 FROM sqlite_master "
     "WHERE type='index'";


  assert( iDb>=0 && iDb!=1 && iDb<db->nDb );

  /* Construct the schema tables: sqlite_master and sqlite_temp_master
  */
  azArg[0] = "table";
  azArg[1] = MASTER_NAME;
  azArg[2] = "2";
  azArg[3] = master_schema;
  sprintf(zDbNum, "%d", iDb);
  azArg[4] = zDbNum;
  azArg[5] = 0;
  initData.db = db;
  initData.pzErrMsg = pzErrMsg;
  sqliteInitCallback(&initData, 5, azArg, 0);
  pTab = sqliteFindTable(db, MASTER_NAME, "main");
  if( pTab ){
    pTab->readOnly = 1;
  }
  if( iDb==0 ){
    azArg[1] = TEMP_MASTER_NAME;
    azArg[3] = temp_master_schema;
    azArg[4] = "1";
    sqliteInitCallback(&initData, 5, azArg, 0);
    pTab = sqliteFindTable(db, TEMP_MASTER_NAME, "temp");
    if( pTab ){
      pTab->readOnly = 1;
    }
  }

  /* Create a cursor to hold the database open
  */
  if( db->aDb[iDb].pBt==0 ) return SQLITE_OK;
  rc = sqliteBtreeCursor(db->aDb[iDb].pBt, 2, 0, &curMain);
  if( rc ){
    sqliteSetString(pzErrMsg, sqlite_error_string(rc), (char*)0);
    return rc;
  }

  /* Get the database meta information
  */
  rc = sqliteBtreeGetMeta(db->aDb[iDb].pBt, meta);
  if( rc ){
    sqliteSetString(pzErrMsg, sqlite_error_string(rc), (char*)0);
    sqliteBtreeCloseCursor(curMain);
    return rc;
  }
  db->aDb[iDb].schema_cookie = meta[1];
  if( iDb==0 ){
    db->next_cookie = meta[1];
    db->file_format = meta[2];
    size = meta[3];
    if( size==0 ){ size = MAX_PAGES; }
    db->cache_size = size;
    db->safety_level = meta[4];
    if( db->safety_level==0 ) db->safety_level = 2;

    /*
    **  file_format==1    Version 2.1.0.
    **  file_format==2    Version 2.2.0. Add support for INTEGER PRIMARY KEY.
    **  file_format==3    Version 2.6.0. Fix empty-string index bug.
    **  file_format==4    Version 2.7.0. Add support for separate numeric and
    **                    text datatypes.
    */
    if( db->file_format==0 ){
      /* This happens if the database was initially empty */
      db->file_format = 4;
    }else if( db->file_format>4 ){
      sqliteBtreeCloseCursor(curMain);
      sqliteSetString(pzErrMsg, "unsupported file format", (char*)0);
      return SQLITE_ERROR;
    }
  }else if( db->file_format!=meta[2] || db->file_format<4 ){
    assert( db->file_format>=4 );
    if( meta[2]==0 ){
      sqliteSetString(pzErrMsg, "cannot attach empty database: ",
         db->aDb[iDb].zName, (char*)0);
    }else{
      sqliteSetString(pzErrMsg, "incompatible file format in auxiliary "
         "database: ", db->aDb[iDb].zName, (char*)0);
    }
    sqliteBtreeClose(db->aDb[iDb].pBt);
    db->aDb[iDb].pBt = 0;
    return SQLITE_FORMAT;
  }
  sqliteBtreeSetCacheSize(db->aDb[iDb].pBt, db->cache_size);
  sqliteBtreeSetSafetyLevel(db->aDb[iDb].pBt, meta[4]==0 ? 2 : meta[4]);

  /* Read the schema information out of the schema tables
  */
  memset(&sParse, 0, sizeof(sParse));
  sParse.db = db;
  sParse.xCallback = sqliteInitCallback;
  sParse.pArg = (void*)&initData;
  sParse.initFlag = 1;
  sParse.useCallback = 1;
  if( iDb==0 ){
    sqliteRunParser(&sParse,
        db->file_format>=2 ? init_script : older_init_script,
        pzErrMsg);
  }else{
    char *zSql = 0;
    sqliteSetString(&zSql, 
       "SELECT type, name, rootpage, sql, ", zDbNum, " FROM \"",
       db->aDb[iDb].zName, "\".sqlite_master", (char*)0);
    sqliteRunParser(&sParse, zSql, pzErrMsg);
    sqliteFree(zSql);
  }
  sqliteBtreeCloseCursor(curMain);
  if( sqlite_malloc_failed ){
    sqliteSetString(pzErrMsg, "out of memory", (char*)0);
    sParse.rc = SQLITE_NOMEM;
    sqliteResetInternalSchema(db, 0);
  }
  if( sParse.rc==SQLITE_OK ){
    DbSetProperty(db, iDb, DB_SchemaLoaded);
    if( iDb==0 ){
      DbSetProperty(db, 1, DB_SchemaLoaded);
    }
  }else{
    sqliteResetInternalSchema(db, iDb);
  }
  return sParse.rc;
}

/*
** Initialize all database files - the main database file, the file
** used to store temporary tables, and any additional database files
** created using ATTACH statements.  Return a success code.  If an
** error occurs, write an error message into *pzErrMsg.
**
** After the database is initialized, the SQLITE_Initialized
** bit is set in the flags field of the sqlite structure.  An
** attempt is made to initialize the database as soon as it
** is opened.  If that fails (perhaps because another process
** has the sqlite_master table locked) than another attempt
** is made the first time the database is accessed.
*/
int sqliteInit(sqlite *db, char **pzErrMsg){
  int i, rc;
  
  assert( (db->flags & SQLITE_Initialized)==0 );
  rc = SQLITE_OK;
  for(i=0; rc==SQLITE_OK && i<db->nDb; i++){
    if( DbHasProperty(db, i, DB_SchemaLoaded) ) continue;
    assert( i!=1 );  /* Should have been initialized together with 0 */
    rc = sqliteInitOne(db, i, pzErrMsg);
  }
  if( rc==SQLITE_OK ){
    db->flags |= SQLITE_Initialized;
    sqliteCommitInternalChanges(db);
  }else{
    db->flags &= ~SQLITE_Initialized;
  }
  return rc;
}

/*
** The version of the library
*/
const char rcsid[] = "@(#) \044Id: SQLite version " SQLITE_VERSION " $";
const char sqlite_version[] = SQLITE_VERSION;

/*
** Does the library expect data to be encoded as UTF-8 or iso8859?  The
** following global constant always lets us know.
*/
#ifdef SQLITE_UTF8
const char sqlite_encoding[] = "UTF-8";
#else
const char sqlite_encoding[] = "iso8859";
#endif

/*
** Open a new SQLite database.  Construct an "sqlite" structure to define
** the state of this database and return a pointer to that structure.
**
** An attempt is made to initialize the in-memory data structures that
** hold the database schema.  But if this fails (because the schema file
** is locked) then that step is deferred until the first call to
** sqlite_exec().
*/
sqlite *sqlite_open(const char *zFilename, int mode, char **pzErrMsg){
  sqlite *db;
  int rc, i;

  /* Allocate the sqlite data structure */
  db = sqliteMalloc( sizeof(sqlite) );
  if( pzErrMsg ) *pzErrMsg = 0;
  if( db==0 ) goto no_mem_on_open;
  db->onError = OE_Default;
  db->priorNewRowid = 0;
  db->magic = SQLITE_MAGIC_BUSY;
  db->nDb = 2;
  db->aDb = db->aDbStatic;
  sqliteHashInit(&db->aFunc, SQLITE_HASH_STRING, 1);
  for(i=0; i<db->nDb; i++){
    sqliteHashInit(&db->aDb[i].tblHash, SQLITE_HASH_STRING, 0);
    sqliteHashInit(&db->aDb[i].idxHash, SQLITE_HASH_STRING, 0);
    sqliteHashInit(&db->aDb[i].trigHash, SQLITE_HASH_STRING, 0);
    sqliteHashInit(&db->aDb[i].aFKey, SQLITE_HASH_STRING, 1);
  }
  
  /* Open the backend database driver */
  if( zFilename[0]==':' && strcmp(zFilename,":memory:")==0 ){
    db->temp_store = 2;
  }
  rc = sqliteBtreeFactory(db, zFilename, 0, MAX_PAGES, &db->aDb[0].pBt);
  if( rc!=SQLITE_OK ){
    switch( rc ){
      default: {
        sqliteSetString(pzErrMsg, "unable to open database: ",
           zFilename, (char*)0);
      }
    }
    sqliteFree(db);
    sqliteStrRealloc(pzErrMsg);
    return 0;
  }
  db->aDb[0].zName = "main";
  db->aDb[1].zName = "temp";

  /* Attempt to read the schema */
  sqliteRegisterBuiltinFunctions(db);
  rc = sqliteInit(db, pzErrMsg);
  db->magic = SQLITE_MAGIC_OPEN;
  if( sqlite_malloc_failed ){
    sqlite_close(db);
    goto no_mem_on_open;
  }else if( rc!=SQLITE_OK && rc!=SQLITE_BUSY ){
    sqlite_close(db);
    sqliteStrRealloc(pzErrMsg);
    return 0;
  }else if( pzErrMsg ){
    sqliteFree(*pzErrMsg);
    *pzErrMsg = 0;
  }

  /* If the database is in formats 1 or 2, then upgrade it to
  ** version 3.  This will reconstruct all indices.  If the
  ** upgrade fails for any reason (ex: out of disk space, database
  ** is read only, interrupt received, etc.) then refuse to open.
  */
  if( rc==SQLITE_OK && db->file_format<3 ){
    char *zErr = 0;
    InitData initData;
    int meta[SQLITE_N_BTREE_META];

    initData.db = db;
    initData.pzErrMsg = &zErr;
    db->file_format = 3;
    rc = sqlite_exec(db,
      "BEGIN; SELECT name FROM sqlite_master WHERE type='table';",
      upgrade_3_callback,
      &initData,
      &zErr);
    if( rc==SQLITE_OK ){
      sqliteBtreeGetMeta(db->aDb[0].pBt, meta);
      meta[2] = 4;
      sqliteBtreeUpdateMeta(db->aDb[0].pBt, meta);
      sqlite_exec(db, "COMMIT", 0, 0, 0);
    }
    if( rc!=SQLITE_OK ){
      sqliteSetString(pzErrMsg, 
        "unable to upgrade database to the version 2.6 format",
        zErr ? ": " : 0, zErr, (char*)0);
      sqlite_freemem(zErr);
      sqliteStrRealloc(pzErrMsg);
      sqlite_close(db);
      return 0;
    }
    sqlite_freemem(zErr);
  }

  /* Return a pointer to the newly opened database structure */
  return db;

no_mem_on_open:
  sqliteSetString(pzErrMsg, "out of memory", (char*)0);
  sqliteStrRealloc(pzErrMsg);
  return 0;
}

/*
** Return the ROWID of the most recent insert
*/
int sqlite_last_insert_rowid(sqlite *db){
  return db->lastRowid;
}

/*
** Return the number of changes in the most recent call to sqlite_exec().
*/
int sqlite_changes(sqlite *db){
  return db->nChange;
}

/*
** Close an existing SQLite database
*/
void sqlite_close(sqlite *db){
  HashElem *i;
  int j;
  db->want_to_close = 1;
  if( sqliteSafetyCheck(db) || sqliteSafetyOn(db) ){
    /* printf("DID NOT CLOSE\n"); fflush(stdout); */
    return;
  }
  db->magic = SQLITE_MAGIC_CLOSED;
  for(j=0; j<db->nDb; j++){
    if( db->aDb[j].pBt ){
      sqliteBtreeClose(db->aDb[j].pBt);
      db->aDb[j].pBt = 0;
    }
    if( j>=2 ){
      sqliteFree(db->aDb[j].zName);
      db->aDb[j].zName = 0;
    }
  }
  sqliteResetInternalSchema(db, 0);
  assert( db->nDb<=2 );
  assert( db->aDb==db->aDbStatic );
  for(i=sqliteHashFirst(&db->aFunc); i; i=sqliteHashNext(i)){
    FuncDef *pFunc, *pNext;
    for(pFunc = (FuncDef*)sqliteHashData(i); pFunc; pFunc=pNext){
      pNext = pFunc->pNext;
      sqliteFree(pFunc);
    }
  }
  sqliteHashClear(&db->aFunc);
  sqliteFree(db);
}

/*
** Rollback all database files.
*/
void sqliteRollbackAll(sqlite *db){
  int i;
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].pBt ){
      sqliteBtreeRollback(db->aDb[i].pBt);
      db->aDb[i].inTrans = 0;
    }
  }
  sqliteRollbackInternalChanges(db);
}

/*
** This routine does the work of either sqlite_exec() or sqlite_compile().
** It works like sqlite_exec() if pVm==NULL and it works like sqlite_compile()
** otherwise.
*/
static int sqliteMain(
  sqlite *db,                 /* The database on which the SQL executes */
  const char *zSql,           /* The SQL to be executed */
  sqlite_callback xCallback,  /* Invoke this callback routine */
  void *pArg,                 /* First argument to xCallback() */
  const char **pzTail,        /* OUT: Next statement after the first */
  sqlite_vm **ppVm,           /* OUT: The virtual machine */
  char **pzErrMsg             /* OUT: Write error messages here */
){
  Parse sParse;

  if( pzErrMsg ) *pzErrMsg = 0;
  if( sqliteSafetyOn(db) ) goto exec_misuse;
  if( (db->flags & SQLITE_Initialized)==0 ){
    int rc, cnt = 1;
    while( (rc = sqliteInit(db, pzErrMsg))==SQLITE_BUSY
       && db->xBusyCallback && db->xBusyCallback(db->pBusyArg, "", cnt++)!=0 ){}
    if( rc!=SQLITE_OK ){
      sqliteStrRealloc(pzErrMsg);
      sqliteSafetyOff(db);
      return rc;
    }
    if( pzErrMsg ){
      sqliteFree(*pzErrMsg);
      *pzErrMsg = 0;
    }
  }
  if( db->file_format<3 ){
    sqliteSafetyOff(db);
    sqliteSetString(pzErrMsg, "obsolete database file format", (char*)0);
    return SQLITE_ERROR;
  }
  if( db->pVdbe==0 ){ db->nChange = 0; }
  memset(&sParse, 0, sizeof(sParse));
  sParse.db = db;
  sParse.xCallback = xCallback;
  sParse.pArg = pArg;
  sParse.useCallback = ppVm==0;
  if( db->xTrace ) db->xTrace(db->pTraceArg, zSql);
  sqliteRunParser(&sParse, zSql, pzErrMsg);
  if( sqlite_malloc_failed ){
    sqliteSetString(pzErrMsg, "out of memory", (char*)0);
    sParse.rc = SQLITE_NOMEM;
    sqliteRollbackAll(db);
    sqliteResetInternalSchema(db, 0);
    db->flags &= ~SQLITE_InTrans;
  }
  if( sParse.rc==SQLITE_DONE ) sParse.rc = SQLITE_OK;
  if( sParse.rc!=SQLITE_OK && pzErrMsg && *pzErrMsg==0 ){
    sqliteSetString(pzErrMsg, sqlite_error_string(sParse.rc), (char*)0);
  }
  sqliteStrRealloc(pzErrMsg);
  if( sParse.rc==SQLITE_SCHEMA ){
    sqliteResetInternalSchema(db, 0);
  }
  if( sParse.useCallback==0 ){
    assert( ppVm );
    *ppVm = (sqlite_vm*)sParse.pVdbe;
    if( pzTail ) *pzTail = sParse.zTail;
  }
  if( sqliteSafetyOff(db) ) goto exec_misuse;
  return sParse.rc;

exec_misuse:
  if( pzErrMsg ){
    *pzErrMsg = 0;
    sqliteSetString(pzErrMsg, sqlite_error_string(SQLITE_MISUSE), (char*)0);
    sqliteStrRealloc(pzErrMsg);
  }
  return SQLITE_MISUSE;
}

/*
** Execute SQL code.  Return one of the SQLITE_ success/failure
** codes.  Also write an error message into memory obtained from
** malloc() and make *pzErrMsg point to that message.
**
** If the SQL is a query, then for each row in the query result
** the xCallback() function is called.  pArg becomes the first
** argument to xCallback().  If xCallback=NULL then no callback
** is invoked, even for queries.
*/
int sqlite_exec(
  sqlite *db,                 /* The database on which the SQL executes */
  const char *zSql,           /* The SQL to be executed */
  sqlite_callback xCallback,  /* Invoke this callback routine */
  void *pArg,                 /* First argument to xCallback() */
  char **pzErrMsg             /* Write error messages here */
){
  return sqliteMain(db, zSql, xCallback, pArg, 0, 0, pzErrMsg);
}

/*
** Compile a single statement of SQL into a virtual machine.  Return one
** of the SQLITE_ success/failure codes.  Also write an error message into
** memory obtained from malloc() and make *pzErrMsg point to that message.
*/
int sqlite_compile(
  sqlite *db,                 /* The database on which the SQL executes */
  const char *zSql,           /* The SQL to be executed */
  const char **pzTail,        /* OUT: Next statement after the first */
  sqlite_vm **ppVm,           /* OUT: The virtual machine */
  char **pzErrMsg             /* OUT: Write error messages here */
){
  return sqliteMain(db, zSql, 0, 0, pzTail, ppVm, pzErrMsg);
}


/*
** The following routine destroys a virtual machine that is created by
** the sqlite_compile() routine.
**
** The integer returned is an SQLITE_ success/failure code that describes
** the result of executing the virtual machine.  An error message is
** written into memory obtained from malloc and *pzErrMsg is made to
** point to that error if pzErrMsg is not NULL.  The calling routine
** should use sqlite_freemem() to delete the message when it has finished
** with it.
*/
int sqlite_finalize(
  sqlite_vm *pVm,            /* The virtual machine to be destroyed */
  char **pzErrMsg            /* OUT: Write error messages here */
){
  int rc = sqliteVdbeFinalize((Vdbe*)pVm, pzErrMsg);
  sqliteStrRealloc(pzErrMsg);
  return rc;
}

/*
** Terminate the current execution of a virtual machine then
** reset the virtual machine back to its starting state so that it
** can be reused.  Any error message resulting from the prior execution
** is written into *pzErrMsg.  A success code from the prior execution
** is returned.
*/
int sqlite_reset(
  sqlite_vm *pVm,            /* The virtual machine to be destroyed */
  char **pzErrMsg            /* OUT: Write error messages here */
){
  int rc = sqliteVdbeReset((Vdbe*)pVm, pzErrMsg);
  sqliteVdbeMakeReady((Vdbe*)pVm, -1, 0, 0, 0);
  sqliteStrRealloc(pzErrMsg);
  return rc;
}

/*
** Return a static string that describes the kind of error specified in the
** argument.
*/
const char *sqlite_error_string(int rc){
  const char *z;
  switch( rc ){
    case SQLITE_OK:         z = "not an error";                          break;
    case SQLITE_ERROR:      z = "SQL logic error or missing database";   break;
    case SQLITE_INTERNAL:   z = "internal SQLite implementation flaw";   break;
    case SQLITE_PERM:       z = "access permission denied";              break;
    case SQLITE_ABORT:      z = "callback requested query abort";        break;
    case SQLITE_BUSY:       z = "database is locked";                    break;
    case SQLITE_LOCKED:     z = "database table is locked";              break;
    case SQLITE_NOMEM:      z = "out of memory";                         break;
    case SQLITE_READONLY:   z = "attempt to write a readonly database";  break;
    case SQLITE_INTERRUPT:  z = "interrupted";                           break;
    case SQLITE_IOERR:      z = "disk I/O error";                        break;
    case SQLITE_CORRUPT:    z = "database disk image is malformed";      break;
    case SQLITE_NOTFOUND:   z = "table or record not found";             break;
    case SQLITE_FULL:       z = "database is full";                      break;
    case SQLITE_CANTOPEN:   z = "unable to open database file";          break;
    case SQLITE_PROTOCOL:   z = "database locking protocol failure";     break;
    case SQLITE_EMPTY:      z = "table contains no data";                break;
    case SQLITE_SCHEMA:     z = "database schema has changed";           break;
    case SQLITE_TOOBIG:     z = "too much data for one table row";       break;
    case SQLITE_CONSTRAINT: z = "constraint failed";                     break;
    case SQLITE_MISMATCH:   z = "datatype mismatch";                     break;
    case SQLITE_MISUSE:     z = "library routine called out of sequence";break;
    case SQLITE_NOLFS:      z = "kernel lacks large file support";       break;
    case SQLITE_AUTH:       z = "authorization denied";                  break;
    case SQLITE_FORMAT:     z = "auxiliary database format error";       break;
    case SQLITE_RANGE:      z = "bind index out of range";               break;
    default:                z = "unknown error";                         break;
  }
  return z;
}

/*
** This routine implements a busy callback that sleeps and tries
** again until a timeout value is reached.  The timeout value is
** an integer number of milliseconds passed in as the first
** argument.
*/
static int sqliteDefaultBusyCallback(
 void *Timeout,           /* Maximum amount of time to wait */
 const char *NotUsed,     /* The name of the table that is busy */
 int count                /* Number of times table has been busy */
){
#if SQLITE_MIN_SLEEP_MS==1
  int delay = 10;
  int prior_delay = 0;
  int timeout = (int)(long)Timeout;
  int i;

  for(i=1; i<count; i++){ 
    prior_delay += delay;
    delay = delay*2;
    if( delay>=1000 ){
      delay = 1000;
      prior_delay += 1000*(count - i - 1);
      break;
    }
  }
  if( prior_delay + delay > timeout ){
    delay = timeout - prior_delay;
    if( delay<=0 ) return 0;
  }
  sqliteOsSleep(delay);
  return 1;
#else
  int timeout = (int)(long)Timeout;
  if( (count+1)*1000 > timeout ){
    return 0;
  }
  sqliteOsSleep(1000);
  return 1;
#endif
}

/*
** This routine sets the busy callback for an Sqlite database to the
** given callback function with the given argument.
*/
void sqlite_busy_handler(
  sqlite *db,
  int (*xBusy)(void*,const char*,int),
  void *pArg
){
  db->xBusyCallback = xBusy;
  db->pBusyArg = pArg;
}

#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
/*
** This routine sets the progress callback for an Sqlite database to the
** given callback function with the given argument. The progress callback will
** be invoked every nOps opcodes.
*/
void sqlite_progress_handler(
  sqlite *db, 
  int nOps,
  int (*xProgress)(void*), 
  void *pArg
){
  if( nOps>0 ){
    db->xProgress = xProgress;
    db->nProgressOps = nOps;
    db->pProgressArg = pArg;
  }else{
    db->xProgress = 0;
    db->nProgressOps = 0;
    db->pProgressArg = 0;
  }
}
#endif


/*
** This routine installs a default busy handler that waits for the
** specified number of milliseconds before returning 0.
*/
void sqlite_busy_timeout(sqlite *db, int ms){
  if( ms>0 ){
    sqlite_busy_handler(db, sqliteDefaultBusyCallback, (void*)ms);
  }else{
    sqlite_busy_handler(db, 0, 0);
  }
}

/*
** Cause any pending operation to stop at its earliest opportunity.
*/
void sqlite_interrupt(sqlite *db){
  db->flags |= SQLITE_Interrupt;
}

/*
** Windows systems should call this routine to free memory that
** is returned in the in the errmsg parameter of sqlite_open() when
** SQLite is a DLL.  For some reason, it does not work to call free()
** directly.
**
** Note that we need to call free() not sqliteFree() here, since every
** string that is exported from SQLite should have already passed through
** sqliteStrRealloc().
*/
void sqlite_freemem(void *p){ free(p); }

/*
** Windows systems need functions to call to return the sqlite_version
** and sqlite_encoding strings since they are unable to access constants
** within DLLs.
*/
const char *sqlite_libversion(void){ return sqlite_version; }
const char *sqlite_libencoding(void){ return sqlite_encoding; }

/*
** Create new user-defined functions.  The sqlite_create_function()
** routine creates a regular function and sqlite_create_aggregate()
** creates an aggregate function.
**
** Passing a NULL xFunc argument or NULL xStep and xFinalize arguments
** disables the function.  Calling sqlite_create_function() with the
** same name and number of arguments as a prior call to
** sqlite_create_aggregate() disables the prior call to
** sqlite_create_aggregate(), and vice versa.
**
** If nArg is -1 it means that this function will accept any number
** of arguments, including 0.
*/
int sqlite_create_function(
  sqlite *db,          /* Add the function to this database connection */
  const char *zName,   /* Name of the function to add */
  int nArg,            /* Number of arguments */
  void (*xFunc)(sqlite_func*,int,const char**),  /* The implementation */
  void *pUserData      /* User data */
){
  FuncDef *p;
  int nName;
  if( db==0 || zName==0 || sqliteSafetyCheck(db) ) return 1;
  nName = strlen(zName);
  if( nName>255 ) return 1;
  p = sqliteFindFunction(db, zName, nName, nArg, 1);
  if( p==0 ) return 1;
  p->xFunc = xFunc;
  p->xStep = 0;
  p->xFinalize = 0;
  p->pUserData = pUserData;
  return 0;
}
int sqlite_create_aggregate(
  sqlite *db,          /* Add the function to this database connection */
  const char *zName,   /* Name of the function to add */
  int nArg,            /* Number of arguments */
  void (*xStep)(sqlite_func*,int,const char**), /* The step function */
  void (*xFinalize)(sqlite_func*),              /* The finalizer */
  void *pUserData      /* User data */
){
  FuncDef *p;
  int nName;
  if( db==0 || zName==0 || sqliteSafetyCheck(db) ) return 1;
  nName = strlen(zName);
  if( nName>255 ) return 1;
  p = sqliteFindFunction(db, zName, nName, nArg, 1);
  if( p==0 ) return 1;
  p->xFunc = 0;
  p->xStep = xStep;
  p->xFinalize = xFinalize;
  p->pUserData = pUserData;
  return 0;
}

/*
** Change the datatype for all functions with a given name.  See the
** header comment for the prototype of this function in sqlite.h for
** additional information.
*/
int sqlite_function_type(sqlite *db, const char *zName, int dataType){
  FuncDef *p = (FuncDef*)sqliteHashFind(&db->aFunc, zName, strlen(zName));
  while( p ){
    p->dataType = dataType; 
    p = p->pNext;
  }
  return SQLITE_OK;
}

/*
** Register a trace function.  The pArg from the previously registered trace
** is returned.  
**
** A NULL trace function means that no tracing is executes.  A non-NULL
** trace is a pointer to a function that is invoked at the start of each
** sqlite_exec().
*/
void *sqlite_trace(sqlite *db, void (*xTrace)(void*,const char*), void *pArg){
  void *pOld = db->pTraceArg;
  db->xTrace = xTrace;
  db->pTraceArg = pArg;
  return pOld;
}

/*
** This routine is called to create a connection to a database BTree
** driver.  If zFilename is the name of a file, then that file is
** opened and used.  If zFilename is the magic name ":memory:" then
** the database is stored in memory (and is thus forgotten as soon as
** the connection is closed.)  If zFilename is NULL then the database
** is for temporary use only and is deleted as soon as the connection
** is closed.
**
** A temporary database can be either a disk file (that is automatically
** deleted when the file is closed) or a set of red-black trees held in memory,
** depending on the values of the TEMP_STORE compile-time macro and the
** db->temp_store variable, according to the following chart:
**
**       TEMP_STORE     db->temp_store     Location of temporary database
**       ----------     --------------     ------------------------------
**           0               any             file
**           1                1              file
**           1                2              memory
**           1                0              file
**           2                1              file
**           2                2              memory
**           2                0              memory
**           3               any             memory
*/
int sqliteBtreeFactory(
  const sqlite *db,	    /* Main database when opening aux otherwise 0 */
  const char *zFilename,    /* Name of the file containing the BTree database */
  int omitJournal,          /* if TRUE then do not journal this file */
  int nCache,               /* How many pages in the page cache */
  Btree **ppBtree){         /* Pointer to new Btree object written here */

  assert( ppBtree != 0);

#ifndef SQLITE_OMIT_INMEMORYDB
  if( zFilename==0 ){
    if (TEMP_STORE == 0) {
      /* Always use file based temporary DB */
      return sqliteBtreeOpen(0, omitJournal, nCache, ppBtree);
    } else if (TEMP_STORE == 1 || TEMP_STORE == 2) {
      /* Switch depending on compile-time and/or runtime settings. */
      int location = db->temp_store==0 ? TEMP_STORE : db->temp_store;

      if (location == 1) {
        return sqliteBtreeOpen(zFilename, omitJournal, nCache, ppBtree);
      } else {
        return sqliteRbtreeOpen(0, 0, 0, ppBtree);
      }
    } else {
      /* Always use in-core DB */
      return sqliteRbtreeOpen(0, 0, 0, ppBtree);
    }
  }else if( zFilename[0]==':' && strcmp(zFilename,":memory:")==0 ){
    return sqliteRbtreeOpen(0, 0, 0, ppBtree);
  }else
#endif
  {
    return sqliteBtreeOpen(zFilename, omitJournal, nCache, ppBtree);
  }
}
