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
** The following constant value is used by the SQLITE_BIGENDIAN and
** SQLITE_LITTLEENDIAN macros.
*/
const int sqlite3one = 1;

/*
** Fill the InitData structure with an error message that indicates
** that the database is corrupt.
*/
static void corruptSchema(InitData *pData, const char *zExtra){
  if( !sqlite3_malloc_failed ){
    sqlite3SetString(pData->pzErrMsg, "malformed database schema",
       zExtra!=0 && zExtra[0]!=0 ? " - " : (char*)0, zExtra, (char*)0);
  }
}

/*
** This is the callback routine for the code that initializes the
** database.  See sqlite3Init() below for additional information.
** This routine is also called from the OP_ParseSchema opcode of the VDBE.
**
** Each callback contains the following information:
**
**     argv[0] = name of thing being created
**     argv[1] = root page number for table or index.  NULL for trigger or view.
**     argv[2] = SQL text for the CREATE statement.
**     argv[3] = "1" for temporary files, "0" for main database, "2" or more
**               for auxiliary database files.
**
*/
int sqlite3InitCallback(void *pInit, int argc, char **argv, char **azColName){
  InitData *pData = (InitData*)pInit;
  sqlite3 *db = pData->db;
  int iDb;

  assert( argc==4 );
  if( argv==0 ) return 0;   /* Might happen if EMPTY_RESULT_CALLBACKS are on */
  if( argv[1]==0 || argv[3]==0 ){
    corruptSchema(pData, 0);
    return 1;
  }
  iDb = atoi(argv[3]);
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
    if( SQLITE_OK!=rc ){
      corruptSchema(pData, zErr);
      sqlite3_free(zErr);
      return rc;
    }
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
  char const *azArg[5];
  char zDbNum[30];
  int meta[10];
  InitData initData;
  char const *zMasterSchema;
  char const *zMasterName;

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
  static const char temp_master_schema[] = 
     "CREATE TEMP TABLE sqlite_temp_master(\n"
     "  type text,\n"
     "  name text,\n"
     "  tbl_name text,\n"
     "  rootpage integer,\n"
     "  sql text\n"
     ")"
  ;

  assert( iDb>=0 && iDb<db->nDb );

  /* zMasterSchema and zInitScript are set to point at the master schema
  ** and initialisation script appropriate for the database being
  ** initialised. zMasterName is the name of the master table.
  */
  if( iDb==1 ){
    zMasterSchema = temp_master_schema;
    zMasterName = TEMP_MASTER_NAME;
  }else{
    zMasterSchema = master_schema;
    zMasterName = MASTER_NAME;
  }

  /* Construct the schema tables.  */
  sqlite3SafetyOff(db);
  azArg[0] = zMasterName;
  azArg[1] = "1";
  azArg[2] = zMasterSchema;
  sprintf(zDbNum, "%d", iDb);
  azArg[3] = zDbNum;
  azArg[4] = 0;
  initData.db = db;
  initData.pzErrMsg = pzErrMsg;
  rc = sqlite3InitCallback(&initData, 4, (char **)azArg, 0);
  if( rc!=SQLITE_OK ){
    sqlite3SafetyOn(db);
    return rc;
  }
  pTab = sqlite3FindTable(db, zMasterName, db->aDb[iDb].zName);
  if( pTab ){
    pTab->readOnly = 1;
  }
  sqlite3SafetyOn(db);

  /* Create a cursor to hold the database open
  */
  if( db->aDb[iDb].pBt==0 ){
    if( iDb==1 ) DbSetProperty(db, 1, DB_SchemaLoaded);
    return SQLITE_OK;
  }
  rc = sqlite3BtreeCursor(db->aDb[iDb].pBt, MASTER_ROOT, 0, 0, 0, &curMain);
  if( rc!=SQLITE_OK && rc!=SQLITE_EMPTY ){
    sqlite3SetString(pzErrMsg, sqlite3ErrStr(rc), (char*)0);
    return rc;
  }

  /* Get the database meta information.
  **
  ** Meta values are as follows:
  **    meta[0]   Schema cookie.  Changes with each schema change.
  **    meta[1]   File format of schema layer.
  **    meta[2]   Size of the page cache.
  **    meta[3]   Use freelist if 0.  Autovacuum if greater than zero.
  **    meta[4]   Db text encoding. 1:UTF-8 3:UTF-16 LE 4:UTF-16 BE
  **    meta[5]
  **    meta[6]   
  **    meta[7]
  **    meta[8]
  **    meta[9]
  **
  ** Note: The hash defined SQLITE_UTF* symbols in sqliteInt.h correspond to
  ** the possible values of meta[4].
  */
  if( rc==SQLITE_OK ){
    int i;
    for(i=0; rc==SQLITE_OK && i<sizeof(meta)/sizeof(meta[0]); i++){
      rc = sqlite3BtreeGetMeta(db->aDb[iDb].pBt, i+1, (u32 *)&meta[i]);
    }
    if( rc ){
      sqlite3SetString(pzErrMsg, sqlite3ErrStr(rc), (char*)0);
      sqlite3BtreeCloseCursor(curMain);
      return rc;
    }
  }else{
    memset(meta, 0, sizeof(meta));
  }
  db->aDb[iDb].schema_cookie = meta[0];

  /* If opening a non-empty database, check the text encoding. For the
  ** main database, set sqlite3.enc to the encoding of the main database.
  ** For an attached db, it is an error if the encoding is not the same
  ** as sqlite3.enc.
  */
  if( meta[4] ){  /* text encoding */
    if( iDb==0 ){
      /* If opening the main database, set db->enc. */
      db->enc = (u8)meta[4];
      db->pDfltColl = sqlite3FindCollSeq(db, db->enc, "BINARY", 6, 0);
    }else{
      /* If opening an attached database, the encoding much match db->enc */
      if( meta[4]!=db->enc ){
        sqlite3BtreeCloseCursor(curMain);
        sqlite3SetString(pzErrMsg, "attached databases must use the same"
            " text encoding as main database", (char*)0);
        return SQLITE_ERROR;
      }
    }
  }

  size = meta[2];
  if( size==0 ){ size = MAX_PAGES; }
  db->aDb[iDb].cache_size = size;

  if( iDb==0 ){
    db->file_format = meta[1];
    if( db->file_format==0 ){
      /* This happens if the database was initially empty */
      db->file_format = 1;
    }
  }

  /*
  **  file_format==1    Version 3.0.0.
  */
  if( meta[1]>1 ){
    sqlite3BtreeCloseCursor(curMain);
    sqlite3SetString(pzErrMsg, "unsupported file format", (char*)0);
    return SQLITE_ERROR;
  }

  sqlite3BtreeSetCacheSize(db->aDb[iDb].pBt, db->aDb[iDb].cache_size);

  /* Read the schema information out of the schema tables
  */
  assert( db->init.busy );
  if( rc==SQLITE_EMPTY ){
    /* For an empty database, there is nothing to read */
    rc = SQLITE_OK;
  }else{
    char *zSql;
    zSql = sqlite3MPrintf(
        "SELECT name, rootpage, sql, %s FROM '%q'.%s",
        zDbNum, db->aDb[iDb].zName, zMasterName);
    sqlite3SafetyOff(db);
    rc = sqlite3_exec(db, zSql, sqlite3InitCallback, &initData, 0);
    sqlite3SafetyOn(db);
    sqliteFree(zSql);
    sqlite3BtreeCloseCursor(curMain);
  }
  if( sqlite3_malloc_failed ){
    sqlite3SetString(pzErrMsg, "out of memory", (char*)0);
    rc = SQLITE_NOMEM;
    sqlite3ResetInternalSchema(db, 0);
  }
  if( rc==SQLITE_OK ){
    DbSetProperty(db, iDb, DB_SchemaLoaded);
  }else{
    sqlite3ResetInternalSchema(db, iDb);
  }
  return rc;
}

/*
** Initialize all database files - the main database file, the file
** used to store temporary tables, and any additional database files
** created using ATTACH statements.  Return a success code.  If an
** error occurs, write an error message into *pzErrMsg.
**
** After the database is initialized, the SQLITE_Initialized
** bit is set in the flags field of the sqlite structure. 
*/
int sqlite3Init(sqlite3 *db, char **pzErrMsg){
  int i, rc;
  
  if( db->init.busy ) return SQLITE_OK;
  assert( (db->flags & SQLITE_Initialized)==0 );
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
  if( rc==SQLITE_OK && db->nDb>1 && !DbHasProperty(db, 1, DB_SchemaLoaded) ){
    rc = sqlite3InitOne(db, 1, pzErrMsg);
    if( rc ){
      sqlite3ResetInternalSchema(db, 1);
    }
  }

  db->init.busy = 0;
  if( rc==SQLITE_OK ){
    db->flags |= SQLITE_Initialized;
    sqlite3CommitInternalChanges(db);
  }

  if( rc!=SQLITE_OK ){
    db->flags &= ~SQLITE_Initialized;
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
  if( !db->init.busy ){
    if( (db->flags & SQLITE_Initialized)==0 ){
      rc = sqlite3Init(db, &pParse->zErrMsg);
    }
  }
  assert( rc!=SQLITE_OK || (db->flags & SQLITE_Initialized)||db->init.busy );
  if( rc!=SQLITE_OK ){
    pParse->rc = rc;
    pParse->nErr++;
  }
  return rc;
}

/*
** The version of the library
*/
const char rcsid3[] = "@(#) \044Id: SQLite version " SQLITE_VERSION " $";
const char sqlite3_version[] = SQLITE_VERSION;
const char *sqlite3_libversion(void){ return sqlite3_version; }

/*
** This is the default collating function named "BINARY" which is always
** available.
*/
static int binaryCollatingFunc(
  void *NotUsed,
  int nKey1, const void *pKey1,
  int nKey2, const void *pKey2
){
  int rc, n;
  n = nKey1<nKey2 ? nKey1 : nKey2;
  rc = memcmp(pKey1, pKey2, n);
  if( rc==0 ){
    rc = nKey1 - nKey2;
  }
  return rc;
}

/*
** Another built-in collating sequence: NOCASE. 
**
** This collating sequence is intended to be used for "case independant
** comparison". SQLite's knowledge of upper and lower case equivalents
** extends only to the 26 characters used in the English language.
**
** At the moment there is only a UTF-8 implementation.
*/
static int nocaseCollatingFunc(
  void *NotUsed,
  int nKey1, const void *pKey1,
  int nKey2, const void *pKey2
){
  int r = sqlite3StrNICmp(
      (const char *)pKey1, (const char *)pKey2, (nKey1<nKey2)?nKey1:nKey2);
  if( 0==r ){
    r = nKey1-nKey2;
  }
  return r;
}

/*
** Return the ROWID of the most recent insert
*/
sqlite_int64 sqlite3_last_insert_rowid(sqlite3 *db){
  return db->lastRowid;
}

/*
** Return the number of changes in the most recent call to sqlite3_exec().
*/
int sqlite3_changes(sqlite3 *db){
  return db->nChange;
}

/*
** Return the number of changes since the database handle was opened.
*/
int sqlite3_total_changes(sqlite3 *db){
  return db->nTotalChange;
}

/*
** Close an existing SQLite database
*/
int sqlite3_close(sqlite3 *db){
  HashElem *i;
  int j;

  if( !db ){
    return SQLITE_OK;
  }
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }

  /* If there are any outstanding VMs, return SQLITE_BUSY. */
  if( db->pVdbe ){
    sqlite3Error(db, SQLITE_BUSY, 
        "Unable to close due to unfinalised statements");
    return SQLITE_BUSY;
  }
  assert( !sqlite3SafetyCheck(db) );

  /* FIX ME: db->magic may be set to SQLITE_MAGIC_CLOSED if the database
  ** cannot be opened for some reason. So this routine needs to run in
  ** that case. But maybe there should be an extra magic value for the
  ** "failed to open" state.
  */
  if( db->magic!=SQLITE_MAGIC_CLOSED && sqlite3SafetyOn(db) ){
    /* printf("DID NOT CLOSE\n"); fflush(stdout); */
    return SQLITE_ERROR;
  }

  for(j=0; j<db->nDb; j++){
    struct Db *pDb = &db->aDb[j];
    if( pDb->pBt ){
      sqlite3BtreeClose(pDb->pBt);
      pDb->pBt = 0;
    }
  }
  sqlite3ResetInternalSchema(db, 0);
  assert( db->nDb<=2 );
  assert( db->aDb==db->aDbStatic );
  for(i=sqliteHashFirst(&db->aFunc); i; i=sqliteHashNext(i)){
    FuncDef *pFunc, *pNext;
    for(pFunc = (FuncDef*)sqliteHashData(i); pFunc; pFunc=pNext){
      pNext = pFunc->pNext;
      sqliteFree(pFunc);
    }
  }

  for(i=sqliteHashFirst(&db->aCollSeq); i; i=sqliteHashNext(i)){
    CollSeq *pColl = (CollSeq *)sqliteHashData(i);
    sqliteFree(pColl);
  }
  sqlite3HashClear(&db->aCollSeq);

  sqlite3HashClear(&db->aFunc);
  sqlite3Error(db, SQLITE_OK, 0); /* Deallocates any cached error strings. */
  if( db->pValue ){
    sqlite3ValueFree(db->pValue);
  }
  if( db->pErr ){
    sqlite3ValueFree(db->pErr);
  }

  db->magic = SQLITE_MAGIC_ERROR;
  sqliteFree(db);
  return SQLITE_OK;
}

/*
** Rollback all database files.
*/
void sqlite3RollbackAll(sqlite3 *db){
  int i;
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].pBt ){
      sqlite3BtreeRollback(db->aDb[i].pBt);
      db->aDb[i].inTrans = 0;
    }
  }
  sqlite3ResetInternalSchema(db, 0);
}

/*
** Return a static string that describes the kind of error specified in the
** argument.
*/
const char *sqlite3ErrStr(int rc){
  const char *z;
  switch( rc ){
    case SQLITE_ROW:
    case SQLITE_DONE:
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
    case SQLITE_NOTADB:     z = "file is encrypted or is not a database";break;
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
 int count                /* Number of times table has been busy */
){
#if SQLITE_MIN_SLEEP_MS==1
  static const char delays[] =
     { 1, 2, 5, 10, 15, 20, 25, 25,  25,  50,  50,  50, 100};
  static const short int totals[] =
     { 0, 1, 3,  8, 18, 33, 53, 78, 103, 128, 178, 228, 287};
# define NDELAY (sizeof(delays)/sizeof(delays[0]))
  ptr timeout = (ptr)Timeout;
  ptr delay, prior;

  if( count <= NDELAY ){
    delay = delays[count-1];
    prior = totals[count-1];
  }else{
    delay = delays[NDELAY-1];
    prior = totals[NDELAY-1] + delay*(count-NDELAY-1);
  }
  if( prior + delay > timeout ){
    delay = timeout - prior;
    if( delay<=0 ) return 0;
  }
  sqlite3OsSleep(delay);
  return 1;
#else
  int timeout = (int)Timeout;
  if( (count+1)*1000 > timeout ){
    return 0;
  }
  sqlite3OsSleep(1000);
  return 1;
#endif
}

/*
** This routine sets the busy callback for an Sqlite database to the
** given callback function with the given argument.
*/
int sqlite3_busy_handler(
  sqlite3 *db,
  int (*xBusy)(void*,int),
  void *pArg
){
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->busyHandler.xFunc = xBusy;
  db->busyHandler.pArg = pArg;
  return SQLITE_OK;
}

#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
/*
** This routine sets the progress callback for an Sqlite database to the
** given callback function with the given argument. The progress callback will
** be invoked every nOps opcodes.
*/
void sqlite3_progress_handler(
  sqlite3 *db, 
  int nOps,
  int (*xProgress)(void*), 
  void *pArg
){
  if( !sqlite3SafetyCheck(db) ){
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
}
#endif


/*
** This routine installs a default busy handler that waits for the
** specified number of milliseconds before returning 0.
*/
int sqlite3_busy_timeout(sqlite3 *db, int ms){
  if( ms>0 ){
    sqlite3_busy_handler(db, sqliteDefaultBusyCallback, (void*)(ptr)ms);
  }else{
    sqlite3_busy_handler(db, 0, 0);
  }
  return SQLITE_OK;
}

/*
** Cause any pending operation to stop at its earliest opportunity.
*/
void sqlite3_interrupt(sqlite3 *db){
  if( !sqlite3SafetyCheck(db) ){
    db->flags |= SQLITE_Interrupt;
  }
}

/*
** Windows systems should call this routine to free memory that
** is returned in the in the errmsg parameter of sqlite3_open() when
** SQLite is a DLL.  For some reason, it does not work to call free()
** directly.
**
** Note that we need to call free() not sqliteFree() here.
*/
void sqlite3_free(char *p){ free(p); }

/*
** Create new user functions.
*/
int sqlite3_create_function(
  sqlite3 *db,
  const char *zFunctionName,
  int nArg,
  int enc,
  void *pUserData,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value **),
  void (*xStep)(sqlite3_context*,int,sqlite3_value **),
  void (*xFinal)(sqlite3_context*)
){
  FuncDef *p;
  int nName;

  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  if( zFunctionName==0 ||
      (xFunc && (xFinal || xStep)) || 
      (!xFunc && (xFinal && !xStep)) ||
      (!xFunc && (!xFinal && xStep)) ||
      (nArg<-1 || nArg>127) ||
      (255<(nName = strlen(zFunctionName))) ){
    return SQLITE_ERROR;
  }
  
  /* If SQLITE_UTF16 is specified as the encoding type, transform this
  ** to one of SQLITE_UTF16LE or SQLITE_UTF16BE using the
  ** SQLITE_UTF16NATIVE macro. SQLITE_UTF16 is not used internally.
  **
  ** If SQLITE_ANY is specified, add three versions of the function
  ** to the hash table.
  */
  if( enc==SQLITE_UTF16 ){
    enc = SQLITE_UTF16NATIVE;
  }else if( enc==SQLITE_ANY ){
    int rc;
    rc = sqlite3_create_function(db, zFunctionName, nArg, SQLITE_UTF8,
         pUserData, xFunc, xStep, xFinal);
    if( rc!=SQLITE_OK ) return rc;
    rc = sqlite3_create_function(db, zFunctionName, nArg, SQLITE_UTF16LE,
        pUserData, xFunc, xStep, xFinal);
    if( rc!=SQLITE_OK ) return rc;
    enc = SQLITE_UTF16BE;
  }

  p = sqlite3FindFunction(db, zFunctionName, nName, nArg, enc, 1);
  if( p==0 ) return SQLITE_NOMEM;
  p->xFunc = xFunc;
  p->xStep = xStep;
  p->xFinalize = xFinal;
  p->pUserData = pUserData;
  return SQLITE_OK;
}
int sqlite3_create_function16(
  sqlite3 *db,
  const void *zFunctionName,
  int nArg,
  int eTextRep,
  void *pUserData,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
){
  int rc;
  char const *zFunc8;
  sqlite3_value *pTmp;

  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  pTmp = sqlite3GetTransientValue(db);
  sqlite3ValueSetStr(pTmp, -1, zFunctionName, SQLITE_UTF16NATIVE,SQLITE_STATIC);
  zFunc8 = sqlite3ValueText(pTmp, SQLITE_UTF8);

  if( !zFunc8 ){
    return SQLITE_NOMEM;
  }
  rc = sqlite3_create_function(db, zFunc8, nArg, eTextRep, 
      pUserData, xFunc, xStep, xFinal);
  return rc;
}

/*
** Register a trace function.  The pArg from the previously registered trace
** is returned.  
**
** A NULL trace function means that no tracing is executes.  A non-NULL
** trace is a pointer to a function that is invoked at the start of each
** sqlite3_exec().
*/
void *sqlite3_trace(sqlite3 *db, void (*xTrace)(void*,const char*), void *pArg){
  void *pOld = db->pTraceArg;
  db->xTrace = xTrace;
  db->pTraceArg = pArg;
  return pOld;
}

/*** EXPERIMENTAL ***
**
** Register a function to be invoked when a transaction comments.
** If either function returns non-zero, then the commit becomes a
** rollback.
*/
void *sqlite3_commit_hook(
  sqlite3 *db,              /* Attach the hook to this database */
  int (*xCallback)(void*),  /* Function to invoke on each commit */
  void *pArg                /* Argument to the function */
){
  void *pOld = db->pCommitArg;
  db->xCommitCallback = xCallback;
  db->pCommitArg = pArg;
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
int sqlite3BtreeFactory(
  const sqlite3 *db,        /* Main database when opening aux otherwise 0 */
  const char *zFilename,    /* Name of the file containing the BTree database */
  int omitJournal,          /* if TRUE then do not journal this file */
  int nCache,               /* How many pages in the page cache */
  Btree **ppBtree           /* Pointer to new Btree object written here */
){
  int btree_flags = 0;
  int rc;
  
  assert( ppBtree != 0);
  if( omitJournal ){
    btree_flags |= BTREE_OMIT_JOURNAL;
  }
  if( zFilename==0 ){
#ifndef TEMP_STORE
# define TEMP_STORE 1
#endif
#if TEMP_STORE==0
    /* Do nothing */
#endif
#if TEMP_STORE==1
    if( db->temp_store==2 ) zFilename = ":memory:";
#endif
#if TEMP_STORE==2
    if( db->temp_store!=1 ) zFilename = ":memory:";
#endif
#if TEMP_STORE==3
    zFilename = ":memory:";
#endif
  }

  rc = sqlite3BtreeOpen(zFilename, ppBtree, btree_flags);
  if( rc==SQLITE_OK ){
    sqlite3BtreeSetBusyHandler(*ppBtree, (void*)&db->busyHandler);
    sqlite3BtreeSetCacheSize(*ppBtree, nCache);
  }
  return rc;
}

/*
** Return UTF-8 encoded English language explanation of the most recent
** error.
*/
const char *sqlite3_errmsg(sqlite3 *db){
  const char *z;
  if( sqlite3_malloc_failed ){
    return sqlite3ErrStr(SQLITE_NOMEM);
  }
  if( sqlite3SafetyCheck(db) || db->errCode==SQLITE_MISUSE ){
    return sqlite3ErrStr(SQLITE_MISUSE);
  }
  z = sqlite3_value_text(db->pErr);
  if( z==0 ){
    z = sqlite3ErrStr(db->errCode);
  }
  return z;
}

/*
** Return UTF-16 encoded English language explanation of the most recent
** error.
*/
const void *sqlite3_errmsg16(sqlite3 *db){
  /* Because all the characters in the string are in the unicode
  ** range 0x00-0xFF, if we pad the big-endian string with a 
  ** zero byte, we can obtain the little-endian string with
  ** &big_endian[1].
  */
  static const char outOfMemBe[] = {
    0, 'o', 0, 'u', 0, 't', 0, ' ', 
    0, 'o', 0, 'f', 0, ' ', 
    0, 'm', 0, 'e', 0, 'm', 0, 'o', 0, 'r', 0, 'y', 0, 0, 0
  };
  static const char misuseBe [] = {
    0, 'l', 0, 'i', 0, 'b', 0, 'r', 0, 'a', 0, 'r', 0, 'y', 0, ' ', 
    0, 'r', 0, 'o', 0, 'u', 0, 't', 0, 'i', 0, 'n', 0, 'e', 0, ' ', 
    0, 'c', 0, 'a', 0, 'l', 0, 'l', 0, 'e', 0, 'd', 0, ' ', 
    0, 'o', 0, 'u', 0, 't', 0, ' ', 
    0, 'o', 0, 'f', 0, ' ', 
    0, 's', 0, 'e', 0, 'q', 0, 'u', 0, 'e', 0, 'n', 0, 'c', 0, 'e', 0, 0, 0
  };

  const void *z;
  if( sqlite3_malloc_failed ){
    return (void *)(&outOfMemBe[SQLITE_UTF16NATIVE==SQLITE_UTF16LE?1:0]);
  }
  if( sqlite3SafetyCheck(db) || db->errCode==SQLITE_MISUSE ){
    return (void *)(&misuseBe[SQLITE_UTF16NATIVE==SQLITE_UTF16LE?1:0]);
  }
  z = sqlite3_value_text16(db->pErr);
  if( z==0 ){
    sqlite3ValueSetStr(db->pErr, -1, sqlite3ErrStr(db->errCode),
         SQLITE_UTF8, SQLITE_STATIC);
    z = sqlite3_value_text16(db->pErr);
  }
  return z;
}

/*
** Return the most recent error code generated by an SQLite routine.
*/
int sqlite3_errcode(sqlite3 *db){
  if( sqlite3_malloc_failed ){
    return SQLITE_NOMEM;
  }
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  return db->errCode;
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

  for(iDb=0; allOk && iDb<db->nDb; iDb++){
    Btree *pBt;
    pBt = db->aDb[iDb].pBt;
    if( pBt==0 ) continue;
    rc = sqlite3BtreeCursor(pBt, MASTER_ROOT, 0, 0, 0, &curTemp);
    if( rc==SQLITE_OK ){
      rc = sqlite3BtreeGetMeta(pBt, 1, (u32 *)&cookie);
      if( rc==SQLITE_OK && cookie!=db->aDb[iDb].schema_cookie ){
        allOk = 0;
      }
      sqlite3BtreeCloseCursor(curTemp);
    }
  }
  return allOk;
}

/*
** Compile the UTF-8 encoded SQL statement zSql into a statement handle.
*/
int sqlite3_prepare(
  sqlite3 *db,              /* Database handle. */
  const char *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const char** pzTail       /* OUT: End of parsed string */
){
  Parse sParse;
  char *zErrMsg = 0;
  int rc = SQLITE_OK;

  if( sqlite3_malloc_failed ){
    return SQLITE_NOMEM;
  }

  assert( ppStmt );
  *ppStmt = 0;
  if( sqlite3SafetyOn(db) ){
    return SQLITE_MISUSE;
  }

  memset(&sParse, 0, sizeof(sParse));
  sParse.db = db;
  sqlite3RunParser(&sParse, zSql, &zErrMsg);

  if( sqlite3_malloc_failed ){
    rc = SQLITE_NOMEM;
    sqlite3RollbackAll(db);
    sqlite3ResetInternalSchema(db, 0);
    db->flags &= ~SQLITE_InTrans;
    goto prepare_out;
  }
  if( sParse.rc==SQLITE_DONE ) sParse.rc = SQLITE_OK;
  if( sParse.rc!=SQLITE_OK && sParse.checkSchema && !schemaIsValid(db) ){
    sParse.rc = SQLITE_SCHEMA;
  }
  if( sParse.rc==SQLITE_SCHEMA ){
    sqlite3ResetInternalSchema(db, 0);
  }
  if( pzTail ) *pzTail = sParse.zTail;
  rc = sParse.rc;

  if( rc==SQLITE_OK && sParse.pVdbe && sParse.explain ){
    sqlite3VdbeSetNumCols(sParse.pVdbe, 5);
    sqlite3VdbeSetColName(sParse.pVdbe, 0, "addr", P3_STATIC);
    sqlite3VdbeSetColName(sParse.pVdbe, 1, "opcode", P3_STATIC);
    sqlite3VdbeSetColName(sParse.pVdbe, 2, "p1", P3_STATIC);
    sqlite3VdbeSetColName(sParse.pVdbe, 3, "p2", P3_STATIC);
    sqlite3VdbeSetColName(sParse.pVdbe, 4, "p3", P3_STATIC);
  } 

prepare_out:
  if( sqlite3SafetyOff(db) ){
    rc = SQLITE_MISUSE;
  }
  if( rc==SQLITE_OK ){
    *ppStmt = (sqlite3_stmt*)sParse.pVdbe;
  }else if( sParse.pVdbe ){
    sqlite3_finalize((sqlite3_stmt*)sParse.pVdbe);
  }

  if( zErrMsg ){
    sqlite3Error(db, rc, "%s", zErrMsg);
    sqliteFree(zErrMsg);
  }else{
    sqlite3Error(db, rc, 0);
  }
  return rc;
}

/*
** Compile the UTF-16 encoded SQL statement zSql into a statement handle.
*/
int sqlite3_prepare16(
  sqlite3 *db,              /* Database handle. */ 
  const void *zSql,         /* UTF-8 encoded SQL statement. */
  int nBytes,               /* Length of zSql in bytes. */
  sqlite3_stmt **ppStmt,    /* OUT: A pointer to the prepared statement */
  const void **pzTail       /* OUT: End of parsed string */
){
  /* This function currently works by first transforming the UTF-16
  ** encoded string to UTF-8, then invoking sqlite3_prepare(). The
  ** tricky bit is figuring out the pointer to return in *pzTail.
  */
  char const *zSql8 = 0;
  char const *zTail8 = 0;
  int rc;
  sqlite3_value *pTmp;

  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  pTmp = sqlite3GetTransientValue(db);
  sqlite3ValueSetStr(pTmp, -1, zSql, SQLITE_UTF16NATIVE, SQLITE_STATIC);
  zSql8 = sqlite3ValueText(pTmp, SQLITE_UTF8);
  if( !zSql8 ){
    sqlite3Error(db, SQLITE_NOMEM, 0);
    return SQLITE_NOMEM;
  }
  rc = sqlite3_prepare(db, zSql8, -1, ppStmt, &zTail8);

  if( zTail8 && pzTail ){
    /* If sqlite3_prepare returns a tail pointer, we calculate the
    ** equivalent pointer into the UTF-16 string by counting the unicode
    ** characters between zSql8 and zTail8, and then returning a pointer
    ** the same number of characters into the UTF-16 string.
    */
    int chars_parsed = sqlite3utf8CharLen(zSql8, zTail8-zSql8);
    *pzTail = (u8 *)zSql + sqlite3utf16ByteLen(zSql, chars_parsed);
  }
 
  return rc;
}

/*
** This routine does the work of opening a database on behalf of
** sqlite3_open() and sqlite3_open16(). The database filename "zFilename"  
** is UTF-8 encoded. The fourth argument, "def_enc" is one of the TEXT_*
** macros from sqliteInt.h. If we end up creating a new database file
** (not opening an existing one), the text encoding of the database
** will be set to this value.
*/
static int openDatabase(
  const char *zFilename, /* Database filename UTF-8 encoded */
  sqlite3 **ppDb         /* OUT: Returned database handle */
){
  sqlite3 *db;
  int rc, i;
  char *zErrMsg = 0;

  /* Allocate the sqlite data structure */
  db = sqliteMalloc( sizeof(sqlite3) );
  if( db==0 ) goto opendb_out;
  db->priorNewRowid = 0;
  db->magic = SQLITE_MAGIC_BUSY;
  db->nDb = 2;
  db->aDb = db->aDbStatic;
  db->enc = SQLITE_UTF8;
  db->autoCommit = 1;
  /* db->flags |= SQLITE_ShortColNames; */
  sqlite3HashInit(&db->aFunc, SQLITE_HASH_STRING, 0);
  sqlite3HashInit(&db->aCollSeq, SQLITE_HASH_STRING, 0);
  for(i=0; i<db->nDb; i++){
    sqlite3HashInit(&db->aDb[i].tblHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&db->aDb[i].idxHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&db->aDb[i].trigHash, SQLITE_HASH_STRING, 0);
    sqlite3HashInit(&db->aDb[i].aFKey, SQLITE_HASH_STRING, 1);
  }
  
  /* Add the default collation sequence BINARY. BINARY works for both UTF-8
  ** and UTF-16, so add a version for each to avoid any unnecessary
  ** conversions. The only error that can occur here is a malloc() failure.
  */
  sqlite3_create_collation(db, "BINARY", SQLITE_UTF8, 0,binaryCollatingFunc);
  sqlite3_create_collation(db, "BINARY", SQLITE_UTF16LE, 0,binaryCollatingFunc);
  sqlite3_create_collation(db, "BINARY", SQLITE_UTF16BE, 0,binaryCollatingFunc);
  db->pDfltColl = sqlite3FindCollSeq(db, db->enc, "BINARY", 6, 0);
  if( !db->pDfltColl ){
    rc = db->errCode;
    assert( rc!=SQLITE_OK );
    db->magic = SQLITE_MAGIC_CLOSED;
    goto opendb_out;
  }

  /* Also add a UTF-8 case-insensitive collation sequence. */
  sqlite3_create_collation(db, "NOCASE", SQLITE_UTF8, 0, nocaseCollatingFunc);

  /* Open the backend database driver */
  rc = sqlite3BtreeFactory(db, zFilename, 0, MAX_PAGES, &db->aDb[0].pBt);
  if( rc!=SQLITE_OK ){
    sqlite3Error(db, rc, 0);
    db->magic = SQLITE_MAGIC_CLOSED;
    goto opendb_out;
  }
  db->aDb[0].zName = "main";
  db->aDb[1].zName = "temp";

  /* The default safety_level for the main database is 'full' for the temp
  ** database it is 'NONE'. This matches the pager layer defaults.  */
  db->aDb[0].safety_level = 3;
  db->aDb[1].safety_level = 1;

  /* Register all built-in functions, but do not attempt to read the
  ** database schema yet. This is delayed until the first time the database
  ** is accessed.
  */
  sqlite3RegisterBuiltinFunctions(db);
  if( rc==SQLITE_OK ){
    sqlite3Error(db, SQLITE_OK, 0);
    db->magic = SQLITE_MAGIC_OPEN;
  }else{
    sqlite3Error(db, rc, "%s", zErrMsg, 0);
    if( zErrMsg ) sqliteFree(zErrMsg);
    db->magic = SQLITE_MAGIC_CLOSED;
  }

opendb_out:
  if( sqlite3_errcode(db)==SQLITE_OK && sqlite3_malloc_failed ){
    sqlite3Error(db, SQLITE_NOMEM, 0);
  }
  *ppDb = db;
  return sqlite3_errcode(db);
}

/*
** Open a new database handle.
*/
int sqlite3_open(
  const char *zFilename, 
  sqlite3 **ppDb 
){
  return openDatabase(zFilename, ppDb);
}

/*
** Open a new database handle.
*/
int sqlite3_open16(
  const void *zFilename, 
  sqlite3 **ppDb
){
  char const *zFilename8;   /* zFilename encoded in UTF-8 instead of UTF-16 */
  int rc = SQLITE_NOMEM;
  sqlite3_value *pVal;

  assert( ppDb );
  *ppDb = 0;
  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, -1, zFilename, SQLITE_UTF16NATIVE, SQLITE_STATIC);
  zFilename8 = sqlite3ValueText(pVal, SQLITE_UTF8);
  if( zFilename8 ){
    rc = openDatabase(zFilename8, ppDb);
    if( rc==SQLITE_OK && *ppDb ){
      sqlite3_exec(*ppDb, "PRAGMA encoding = 'UTF-16'", 0, 0, 0);
    }
  }
  if( pVal ){
    sqlite3ValueFree(pVal);
  }

  return rc;
}

/*
** The following routine destroys a virtual machine that is created by
** the sqlite3_compile() routine. The integer returned is an SQLITE_
** success/failure code that describes the result of executing the virtual
** machine.
**
** This routine sets the error code and string returned by
** sqlite3_errcode(), sqlite3_errmsg() and sqlite3_errmsg16().
*/
int sqlite3_finalize(sqlite3_stmt *pStmt){
  int rc;
  if( pStmt==0 ){
    rc = SQLITE_OK;
  }else{
    rc = sqlite3VdbeFinalize((Vdbe*)pStmt);
  }
  return rc;
}

/*
** Terminate the current execution of an SQL statement and reset it
** back to its starting state so that it can be reused. A success code from
** the prior execution is returned.
**
** This routine sets the error code and string returned by
** sqlite3_errcode(), sqlite3_errmsg() and sqlite3_errmsg16().
*/
int sqlite3_reset(sqlite3_stmt *pStmt){
  int rc;
  if( pStmt==0 ){
    rc = SQLITE_OK;
  }else{
    rc = sqlite3VdbeReset((Vdbe*)pStmt);
    sqlite3VdbeMakeReady((Vdbe*)pStmt, -1, 0, 0, 0);
  }
  return rc;
}

/*
** Register a new collation sequence with the database handle db.
*/
int sqlite3_create_collation(
  sqlite3* db, 
  const char *zName, 
  int enc, 
  void* pCtx,
  int(*xCompare)(void*,int,const void*,int,const void*)
){
  CollSeq *pColl;
  int rc = SQLITE_OK;
  
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }

  /* If SQLITE_UTF16 is specified as the encoding type, transform this
  ** to one of SQLITE_UTF16LE or SQLITE_UTF16BE using the
  ** SQLITE_UTF16NATIVE macro. SQLITE_UTF16 is not used internally.
  */
  if( enc==SQLITE_UTF16 ){
    enc = SQLITE_UTF16NATIVE;
  }

  if( enc!=SQLITE_UTF8 && enc!=SQLITE_UTF16LE && enc!=SQLITE_UTF16BE ){
    sqlite3Error(db, SQLITE_ERROR, 
        "Param 3 to sqlite3_create_collation() must be one of "
        "SQLITE_UTF8, SQLITE_UTF16, SQLITE_UTF16LE or SQLITE_UTF16BE"
    );
    return SQLITE_ERROR;
  }
  pColl = sqlite3FindCollSeq(db, (u8)enc, zName, strlen(zName), 1);
  if( 0==pColl ){
   rc = SQLITE_NOMEM;
  }else{
    pColl->xCmp = xCompare;
    pColl->pUser = pCtx;
    pColl->enc = enc;
  }
  sqlite3Error(db, rc, 0);
  return rc;
}

/*
** Register a new collation sequence with the database handle db.
*/
int sqlite3_create_collation16(
  sqlite3* db, 
  const char *zName, 
  int enc, 
  void* pCtx,
  int(*xCompare)(void*,int,const void*,int,const void*)
){
  char const *zName8;
  sqlite3_value *pTmp;
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  pTmp = sqlite3GetTransientValue(db);
  sqlite3ValueSetStr(pTmp, -1, zName, SQLITE_UTF16NATIVE, SQLITE_STATIC);
  zName8 = sqlite3ValueText(pTmp, SQLITE_UTF8);
  return sqlite3_create_collation(db, zName8, enc, pCtx, xCompare);
}

/*
** Register a collation sequence factory callback with the database handle
** db. Replace any previously installed collation sequence factory.
*/
int sqlite3_collation_needed(
  sqlite3 *db, 
  void *pCollNeededArg, 
  void(*xCollNeeded)(void*,sqlite3*,int eTextRep,const char*)
){
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->xCollNeeded = xCollNeeded;
  db->xCollNeeded16 = 0;
  db->pCollNeededArg = pCollNeededArg;
  return SQLITE_OK;
}

/*
** Register a collation sequence factory callback with the database handle
** db. Replace any previously installed collation sequence factory.
*/
int sqlite3_collation_needed16(
  sqlite3 *db, 
  void *pCollNeededArg, 
  void(*xCollNeeded16)(void*,sqlite3*,int eTextRep,const void*)
){
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->xCollNeeded = 0;
  db->xCollNeeded16 = xCollNeeded16;
  db->pCollNeededArg = pCollNeededArg;
  return SQLITE_OK;
}
