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
#include <ctype.h>
#ifdef SQLITE_ENABLE_FTS3
# include "fts3.h"
#endif

/*
** The version of the library
*/
const char sqlite3_version[] = SQLITE_VERSION;
const char *sqlite3_libversion(void){ return sqlite3_version; }
int sqlite3_libversion_number(void){ return SQLITE_VERSION_NUMBER; }
int sqlite3_threadsafe(void){ return SQLITE_THREADSAFE; }

/*
** If the following function pointer is not NULL and if
** SQLITE_ENABLE_IOTRACE is enabled, then messages describing
** I/O active are written using this function.  These messages
** are intended for debugging activity only.
*/
void (*sqlite3_io_trace)(const char*, ...) = 0;

/*
** If the following global variable points to a string which is the
** name of a directory, then that directory will be used to store
** temporary files.
**
** See also the "PRAGMA temp_store_directory" SQL command.
*/
char *sqlite3_temp_directory = 0;


/*
** Return true if the buffer z[0..n-1] contains all spaces.
*/
static int allSpaces(const char *z, int n){
  while( n>0 && z[--n]==' ' ){}
  return n==0;
}

/*
** This is the default collating function named "BINARY" which is always
** available.
**
** If the padFlag argument is not NULL then space padding at the end
** of strings is ignored.  This implements the RTRIM collation.
*/
static int binCollFunc(
  void *padFlag,
  int nKey1, const void *pKey1,
  int nKey2, const void *pKey2
){
  int rc, n;
  n = nKey1<nKey2 ? nKey1 : nKey2;
  rc = memcmp(pKey1, pKey2, n);
  if( rc==0 ){
    if( padFlag
     && allSpaces(((char*)pKey1)+n, nKey1-n)
     && allSpaces(((char*)pKey2)+n, nKey2-n)
    ){
      /* Leave rc unchanged at 0 */
    }else{
      rc = nKey1 - nKey2;
    }
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
  if( !sqlite3SafetyCheckSickOrOk(db) ){
    return SQLITE_MISUSE;
  }
  sqlite3_mutex_enter(db->mutex);

#ifdef SQLITE_SSE
  {
    extern void sqlite3SseCleanup(sqlite3*);
    sqlite3SseCleanup(db);
  }
#endif 

  sqlite3ResetInternalSchema(db, 0);

  /* If a transaction is open, the ResetInternalSchema() call above
  ** will not have called the xDisconnect() method on any virtual
  ** tables in the db->aVTrans[] array. The following sqlite3VtabRollback()
  ** call will do so. We need to do this before the check for active
  ** SQL statements below, as the v-table implementation may be storing
  ** some prepared statements internally.
  */
  sqlite3VtabRollback(db);

  /* If there are any outstanding VMs, return SQLITE_BUSY. */
  if( db->pVdbe ){
    sqlite3Error(db, SQLITE_BUSY, 
        "Unable to close due to unfinalised statements");
    sqlite3_mutex_leave(db->mutex);
    return SQLITE_BUSY;
  }
  assert( sqlite3SafetyCheckSickOrOk(db) );

  for(j=0; j<db->nDb; j++){
    struct Db *pDb = &db->aDb[j];
    if( pDb->pBt ){
      sqlite3BtreeClose(pDb->pBt);
      pDb->pBt = 0;
      if( j!=1 ){
        pDb->pSchema = 0;
      }
    }
  }
  sqlite3ResetInternalSchema(db, 0);
  assert( db->nDb<=2 );
  assert( db->aDb==db->aDbStatic );
  for(i=sqliteHashFirst(&db->aFunc); i; i=sqliteHashNext(i)){
    FuncDef *pFunc, *pNext;
    for(pFunc = (FuncDef*)sqliteHashData(i); pFunc; pFunc=pNext){
      pNext = pFunc->pNext;
      sqlite3_free(pFunc);
    }
  }

  for(i=sqliteHashFirst(&db->aCollSeq); i; i=sqliteHashNext(i)){
    CollSeq *pColl = (CollSeq *)sqliteHashData(i);
    /* Invoke any destructors registered for collation sequence user data. */
    for(j=0; j<3; j++){
      if( pColl[j].xDel ){
        pColl[j].xDel(pColl[j].pUser);
      }
    }
    sqlite3_free(pColl);
  }
  sqlite3HashClear(&db->aCollSeq);
#ifndef SQLITE_OMIT_VIRTUALTABLE
  for(i=sqliteHashFirst(&db->aModule); i; i=sqliteHashNext(i)){
    Module *pMod = (Module *)sqliteHashData(i);
    if( pMod->xDestroy ){
      pMod->xDestroy(pMod->pAux);
    }
    sqlite3_free(pMod);
  }
  sqlite3HashClear(&db->aModule);
#endif

  sqlite3HashClear(&db->aFunc);
  sqlite3Error(db, SQLITE_OK, 0); /* Deallocates any cached error strings. */
  if( db->pErr ){
    sqlite3ValueFree(db->pErr);
  }
  sqlite3CloseExtensions(db);

  db->magic = SQLITE_MAGIC_ERROR;

  /* The temp-database schema is allocated differently from the other schema
  ** objects (using sqliteMalloc() directly, instead of sqlite3BtreeSchema()).
  ** So it needs to be freed here. Todo: Why not roll the temp schema into
  ** the same sqliteMalloc() as the one that allocates the database 
  ** structure?
  */
  sqlite3_free(db->aDb[1].pSchema);
  sqlite3_mutex_leave(db->mutex);
  db->magic = SQLITE_MAGIC_CLOSED;
  sqlite3_mutex_free(db->mutex);
  sqlite3_free(db);
  return SQLITE_OK;
}

/*
** Rollback all database files.
*/
void sqlite3RollbackAll(sqlite3 *db){
  int i;
  int inTrans = 0;
  assert( sqlite3_mutex_held(db->mutex) );
  sqlite3FaultBenign(SQLITE_FAULTINJECTOR_MALLOC, 1);
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].pBt ){
      if( sqlite3BtreeIsInTrans(db->aDb[i].pBt) ){
        inTrans = 1;
      }
      sqlite3BtreeRollback(db->aDb[i].pBt);
      db->aDb[i].inTrans = 0;
    }
  }
  sqlite3VtabRollback(db);
  sqlite3FaultBenign(SQLITE_FAULTINJECTOR_MALLOC, 0);

  if( db->flags&SQLITE_InternChanges ){
    sqlite3ExpirePreparedStatements(db);
    sqlite3ResetInternalSchema(db, 0);
  }

  /* If one has been configured, invoke the rollback-hook callback */
  if( db->xRollbackCallback && (inTrans || !db->autoCommit) ){
    db->xRollbackCallback(db->pRollbackArg);
  }
}

/*
** Return a static string that describes the kind of error specified in the
** argument.
*/
const char *sqlite3ErrStr(int rc){
  const char *z;
  switch( rc & 0xff ){
    case SQLITE_ROW:
    case SQLITE_DONE:
    case SQLITE_OK:         z = "not an error";                          break;
    case SQLITE_ERROR:      z = "SQL logic error or missing database";   break;
    case SQLITE_PERM:       z = "access permission denied";              break;
    case SQLITE_ABORT:      z = "callback requested query abort";        break;
    case SQLITE_BUSY:       z = "database is locked";                    break;
    case SQLITE_LOCKED:     z = "database table is locked";              break;
    case SQLITE_NOMEM:      z = "out of memory";                         break;
    case SQLITE_READONLY:   z = "attempt to write a readonly database";  break;
    case SQLITE_INTERRUPT:  z = "interrupted";                           break;
    case SQLITE_IOERR:      z = "disk I/O error";                        break;
    case SQLITE_CORRUPT:    z = "database disk image is malformed";      break;
    case SQLITE_FULL:       z = "database or disk is full";              break;
    case SQLITE_CANTOPEN:   z = "unable to open database file";          break;
    case SQLITE_EMPTY:      z = "table contains no data";                break;
    case SQLITE_SCHEMA:     z = "database schema has changed";           break;
    case SQLITE_TOOBIG:     z = "String or BLOB exceeded size limit";    break;
    case SQLITE_CONSTRAINT: z = "constraint failed";                     break;
    case SQLITE_MISMATCH:   z = "datatype mismatch";                     break;
    case SQLITE_MISUSE:     z = "library routine called out of sequence";break;
    case SQLITE_NOLFS:      z = "kernel lacks large file support";       break;
    case SQLITE_AUTH:       z = "authorization denied";                  break;
    case SQLITE_FORMAT:     z = "auxiliary database format error";       break;
    case SQLITE_RANGE:      z = "bind or column index out of range";     break;
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
 void *ptr,               /* Database connection */
 int count                /* Number of times table has been busy */
){
#if OS_WIN || (defined(HAVE_USLEEP) && HAVE_USLEEP)
  static const u8 delays[] =
     { 1, 2, 5, 10, 15, 20, 25, 25,  25,  50,  50, 100 };
  static const u8 totals[] =
     { 0, 1, 3,  8, 18, 33, 53, 78, 103, 128, 178, 228 };
# define NDELAY (sizeof(delays)/sizeof(delays[0]))
  sqlite3 *db = (sqlite3 *)ptr;
  int timeout = db->busyTimeout;
  int delay, prior;

  assert( count>=0 );
  if( count < NDELAY ){
    delay = delays[count];
    prior = totals[count];
  }else{
    delay = delays[NDELAY-1];
    prior = totals[NDELAY-1] + delay*(count-(NDELAY-1));
  }
  if( prior + delay > timeout ){
    delay = timeout - prior;
    if( delay<=0 ) return 0;
  }
  sqlite3OsSleep(db->pVfs, delay*1000);
  return 1;
#else
  sqlite3 *db = (sqlite3 *)ptr;
  int timeout = ((sqlite3 *)ptr)->busyTimeout;
  if( (count+1)*1000 > timeout ){
    return 0;
  }
  sqlite3OsSleep(db->pVfs, 1000000);
  return 1;
#endif
}

/*
** Invoke the given busy handler.
**
** This routine is called when an operation failed with a lock.
** If this routine returns non-zero, the lock is retried.  If it
** returns 0, the operation aborts with an SQLITE_BUSY error.
*/
int sqlite3InvokeBusyHandler(BusyHandler *p){
  int rc;
  if( p==0 || p->xFunc==0 || p->nBusy<0 ) return 0;
  rc = p->xFunc(p->pArg, p->nBusy);
  if( rc==0 ){
    p->nBusy = -1;
  }else{
    p->nBusy++;
  }
  return rc; 
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
  sqlite3_mutex_enter(db->mutex);
  db->busyHandler.xFunc = xBusy;
  db->busyHandler.pArg = pArg;
  db->busyHandler.nBusy = 0;
  sqlite3_mutex_leave(db->mutex);
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
  if( sqlite3SafetyCheckOk(db) ){
    sqlite3_mutex_enter(db->mutex);
    if( nOps>0 ){
      db->xProgress = xProgress;
      db->nProgressOps = nOps;
      db->pProgressArg = pArg;
    }else{
      db->xProgress = 0;
      db->nProgressOps = 0;
      db->pProgressArg = 0;
    }
    sqlite3_mutex_leave(db->mutex);
  }
}
#endif


/*
** This routine installs a default busy handler that waits for the
** specified number of milliseconds before returning 0.
*/
int sqlite3_busy_timeout(sqlite3 *db, int ms){
  if( ms>0 ){
    db->busyTimeout = ms;
    sqlite3_busy_handler(db, sqliteDefaultBusyCallback, (void*)db);
  }else{
    sqlite3_busy_handler(db, 0, 0);
  }
  return SQLITE_OK;
}

/*
** Cause any pending operation to stop at its earliest opportunity.
*/
void sqlite3_interrupt(sqlite3 *db){
  if( sqlite3SafetyCheckOk(db) ){
    db->u1.isInterrupted = 1;
  }
}


/*
** This function is exactly the same as sqlite3_create_function(), except
** that it is designed to be called by internal code. The difference is
** that if a malloc() fails in sqlite3_create_function(), an error code
** is returned and the mallocFailed flag cleared. 
*/
int sqlite3CreateFunc(
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

  assert( sqlite3_mutex_held(db->mutex) );
  if( zFunctionName==0 ||
      (xFunc && (xFinal || xStep)) || 
      (!xFunc && (xFinal && !xStep)) ||
      (!xFunc && (!xFinal && xStep)) ||
      (nArg<-1 || nArg>127) ||
      (255<(nName = strlen(zFunctionName))) ){
    sqlite3Error(db, SQLITE_ERROR, "bad parameters");
    return SQLITE_ERROR;
  }
  
#ifndef SQLITE_OMIT_UTF16
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
    rc = sqlite3CreateFunc(db, zFunctionName, nArg, SQLITE_UTF8,
         pUserData, xFunc, xStep, xFinal);
    if( rc==SQLITE_OK ){
      rc = sqlite3CreateFunc(db, zFunctionName, nArg, SQLITE_UTF16LE,
          pUserData, xFunc, xStep, xFinal);
    }
    if( rc!=SQLITE_OK ){
      return rc;
    }
    enc = SQLITE_UTF16BE;
  }
#else
  enc = SQLITE_UTF8;
#endif
  
  /* Check if an existing function is being overridden or deleted. If so,
  ** and there are active VMs, then return SQLITE_BUSY. If a function
  ** is being overridden/deleted but there are no active VMs, allow the
  ** operation to continue but invalidate all precompiled statements.
  */
  p = sqlite3FindFunction(db, zFunctionName, nName, nArg, enc, 0);
  if( p && p->iPrefEnc==enc && p->nArg==nArg ){
    if( db->activeVdbeCnt ){
      sqlite3Error(db, SQLITE_BUSY, 
        "Unable to delete/modify user-function due to active statements");
      assert( !db->mallocFailed );
      return SQLITE_BUSY;
    }else{
      sqlite3ExpirePreparedStatements(db);
    }
  }

  p = sqlite3FindFunction(db, zFunctionName, nName, nArg, enc, 1);
  assert(p || db->mallocFailed);
  if( !p ){
    return SQLITE_NOMEM;
  }
  p->flags = 0;
  p->xFunc = xFunc;
  p->xStep = xStep;
  p->xFinalize = xFinal;
  p->pUserData = pUserData;
  p->nArg = nArg;
  return SQLITE_OK;
}

/*
** Create new user functions.
*/
int sqlite3_create_function(
  sqlite3 *db,
  const char *zFunctionName,
  int nArg,
  int enc,
  void *p,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value **),
  void (*xStep)(sqlite3_context*,int,sqlite3_value **),
  void (*xFinal)(sqlite3_context*)
){
  int rc;
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  rc = sqlite3CreateFunc(db, zFunctionName, nArg, enc, p, xFunc, xStep, xFinal);
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

#ifndef SQLITE_OMIT_UTF16
int sqlite3_create_function16(
  sqlite3 *db,
  const void *zFunctionName,
  int nArg,
  int eTextRep,
  void *p,
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
  void (*xStep)(sqlite3_context*,int,sqlite3_value**),
  void (*xFinal)(sqlite3_context*)
){
  int rc;
  char *zFunc8;
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  zFunc8 = sqlite3Utf16to8(db, zFunctionName, -1);
  rc = sqlite3CreateFunc(db, zFunc8, nArg, eTextRep, p, xFunc, xStep, xFinal);
  sqlite3_free(zFunc8);
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}
#endif


/*
** Declare that a function has been overloaded by a virtual table.
**
** If the function already exists as a regular global function, then
** this routine is a no-op.  If the function does not exist, then create
** a new one that always throws a run-time error.  
**
** When virtual tables intend to provide an overloaded function, they
** should call this routine to make sure the global function exists.
** A global function must exist in order for name resolution to work
** properly.
*/
int sqlite3_overload_function(
  sqlite3 *db,
  const char *zName,
  int nArg
){
  int nName = strlen(zName);
  int rc;
  sqlite3_mutex_enter(db->mutex);
  if( sqlite3FindFunction(db, zName, nName, nArg, SQLITE_UTF8, 0)==0 ){
    sqlite3CreateFunc(db, zName, nArg, SQLITE_UTF8,
                      0, sqlite3InvalidFunction, 0, 0);
  }
  rc = sqlite3ApiExit(db, SQLITE_OK);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

#ifndef SQLITE_OMIT_TRACE
/*
** Register a trace function.  The pArg from the previously registered trace
** is returned.  
**
** A NULL trace function means that no tracing is executes.  A non-NULL
** trace is a pointer to a function that is invoked at the start of each
** SQL statement.
*/
void *sqlite3_trace(sqlite3 *db, void (*xTrace)(void*,const char*), void *pArg){
  void *pOld;
  sqlite3_mutex_enter(db->mutex);
  pOld = db->pTraceArg;
  db->xTrace = xTrace;
  db->pTraceArg = pArg;
  sqlite3_mutex_leave(db->mutex);
  return pOld;
}
/*
** Register a profile function.  The pArg from the previously registered 
** profile function is returned.  
**
** A NULL profile function means that no profiling is executes.  A non-NULL
** profile is a pointer to a function that is invoked at the conclusion of
** each SQL statement that is run.
*/
void *sqlite3_profile(
  sqlite3 *db,
  void (*xProfile)(void*,const char*,sqlite_uint64),
  void *pArg
){
  void *pOld;
  sqlite3_mutex_enter(db->mutex);
  pOld = db->pProfileArg;
  db->xProfile = xProfile;
  db->pProfileArg = pArg;
  sqlite3_mutex_leave(db->mutex);
  return pOld;
}
#endif /* SQLITE_OMIT_TRACE */

/*** EXPERIMENTAL ***
**
** Register a function to be invoked when a transaction comments.
** If the invoked function returns non-zero, then the commit becomes a
** rollback.
*/
void *sqlite3_commit_hook(
  sqlite3 *db,              /* Attach the hook to this database */
  int (*xCallback)(void*),  /* Function to invoke on each commit */
  void *pArg                /* Argument to the function */
){
  void *pOld;
  sqlite3_mutex_enter(db->mutex);
  pOld = db->pCommitArg;
  db->xCommitCallback = xCallback;
  db->pCommitArg = pArg;
  sqlite3_mutex_leave(db->mutex);
  return pOld;
}

/*
** Register a callback to be invoked each time a row is updated,
** inserted or deleted using this database connection.
*/
void *sqlite3_update_hook(
  sqlite3 *db,              /* Attach the hook to this database */
  void (*xCallback)(void*,int,char const *,char const *,sqlite_int64),
  void *pArg                /* Argument to the function */
){
  void *pRet;
  sqlite3_mutex_enter(db->mutex);
  pRet = db->pUpdateArg;
  db->xUpdateCallback = xCallback;
  db->pUpdateArg = pArg;
  sqlite3_mutex_leave(db->mutex);
  return pRet;
}

/*
** Register a callback to be invoked each time a transaction is rolled
** back by this database connection.
*/
void *sqlite3_rollback_hook(
  sqlite3 *db,              /* Attach the hook to this database */
  void (*xCallback)(void*), /* Callback function */
  void *pArg                /* Argument to the function */
){
  void *pRet;
  sqlite3_mutex_enter(db->mutex);
  pRet = db->pRollbackArg;
  db->xRollbackCallback = xCallback;
  db->pRollbackArg = pArg;
  sqlite3_mutex_leave(db->mutex);
  return pRet;
}

/*
** This routine is called to create a connection to a database BTree
** driver.  If zFilename is the name of a file, then that file is
** opened and used.  If zFilename is the magic name ":memory:" then
** the database is stored in memory (and is thus forgotten as soon as
** the connection is closed.)  If zFilename is NULL then the database
** is a "virtual" database for transient use only and is deleted as
** soon as the connection is closed.
**
** A virtual database can be either a disk file (that is automatically
** deleted when the file is closed) or it an be held entirely in memory,
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
  int vfsFlags,             /* Flags passed through to vfsOpen */
  Btree **ppBtree           /* Pointer to new Btree object written here */
){
  int btFlags = 0;
  int rc;
  
  assert( sqlite3_mutex_held(db->mutex) );
  assert( ppBtree != 0);
  if( omitJournal ){
    btFlags |= BTREE_OMIT_JOURNAL;
  }
  if( db->flags & SQLITE_NoReadlock ){
    btFlags |= BTREE_NO_READLOCK;
  }
  if( zFilename==0 ){
#if TEMP_STORE==0
    /* Do nothing */
#endif
#ifndef SQLITE_OMIT_MEMORYDB
#if TEMP_STORE==1
    if( db->temp_store==2 ) zFilename = ":memory:";
#endif
#if TEMP_STORE==2
    if( db->temp_store!=1 ) zFilename = ":memory:";
#endif
#if TEMP_STORE==3
    zFilename = ":memory:";
#endif
#endif /* SQLITE_OMIT_MEMORYDB */
  }

  if( (vfsFlags & SQLITE_OPEN_MAIN_DB)!=0 && (zFilename==0 || *zFilename==0) ){
    vfsFlags = (vfsFlags & ~SQLITE_OPEN_MAIN_DB) | SQLITE_OPEN_TEMP_DB;
  }
  rc = sqlite3BtreeOpen(zFilename, (sqlite3 *)db, ppBtree, btFlags, vfsFlags);
  if( rc==SQLITE_OK ){
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
  if( !db ){
    return sqlite3ErrStr(SQLITE_NOMEM);
  }
  if( !sqlite3SafetyCheckSickOrOk(db) || db->errCode==SQLITE_MISUSE ){
    return sqlite3ErrStr(SQLITE_MISUSE);
  }
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  z = (char*)sqlite3_value_text(db->pErr);
  if( z==0 ){
    z = sqlite3ErrStr(db->errCode);
  }
  sqlite3_mutex_leave(db->mutex);
  return z;
}

#ifndef SQLITE_OMIT_UTF16
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
  if( !db ){
    return (void *)(&outOfMemBe[SQLITE_UTF16NATIVE==SQLITE_UTF16LE?1:0]);
  }
  if( !sqlite3SafetyCheckSickOrOk(db) || db->errCode==SQLITE_MISUSE ){
    return (void *)(&misuseBe[SQLITE_UTF16NATIVE==SQLITE_UTF16LE?1:0]);
  }
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  z = sqlite3_value_text16(db->pErr);
  if( z==0 ){
    sqlite3ValueSetStr(db->pErr, -1, sqlite3ErrStr(db->errCode),
         SQLITE_UTF8, SQLITE_STATIC);
    z = sqlite3_value_text16(db->pErr);
  }
  sqlite3ApiExit(0, 0);
  sqlite3_mutex_leave(db->mutex);
  return z;
}
#endif /* SQLITE_OMIT_UTF16 */

/*
** Return the most recent error code generated by an SQLite routine. If NULL is
** passed to this function, we assume a malloc() failed during sqlite3_open().
*/
int sqlite3_errcode(sqlite3 *db){
  if( db && !sqlite3SafetyCheckSickOrOk(db) ){
    return SQLITE_MISUSE;
  }
  if( !db || db->mallocFailed ){
    return SQLITE_NOMEM;
  }
  return db->errCode & db->errMask;
}

/*
** Create a new collating function for database "db".  The name is zName
** and the encoding is enc.
*/
static int createCollation(
  sqlite3* db, 
  const char *zName, 
  int enc, 
  void* pCtx,
  int(*xCompare)(void*,int,const void*,int,const void*),
  void(*xDel)(void*)
){
  CollSeq *pColl;
  int enc2;
  
  assert( sqlite3_mutex_held(db->mutex) );

  /* If SQLITE_UTF16 is specified as the encoding type, transform this
  ** to one of SQLITE_UTF16LE or SQLITE_UTF16BE using the
  ** SQLITE_UTF16NATIVE macro. SQLITE_UTF16 is not used internally.
  */
  enc2 = enc & ~SQLITE_UTF16_ALIGNED;
  if( enc2==SQLITE_UTF16 ){
    enc2 = SQLITE_UTF16NATIVE;
  }

  if( (enc2&~3)!=0 ){
    sqlite3Error(db, SQLITE_ERROR, "unknown encoding");
    return SQLITE_ERROR;
  }

  /* Check if this call is removing or replacing an existing collation 
  ** sequence. If so, and there are active VMs, return busy. If there
  ** are no active VMs, invalidate any pre-compiled statements.
  */
  pColl = sqlite3FindCollSeq(db, (u8)enc2, zName, strlen(zName), 0);
  if( pColl && pColl->xCmp ){
    if( db->activeVdbeCnt ){
      sqlite3Error(db, SQLITE_BUSY, 
        "Unable to delete/modify collation sequence due to active statements");
      return SQLITE_BUSY;
    }
    sqlite3ExpirePreparedStatements(db);

    /* If collation sequence pColl was created directly by a call to
    ** sqlite3_create_collation, and not generated by synthCollSeq(),
    ** then any copies made by synthCollSeq() need to be invalidated.
    ** Also, collation destructor - CollSeq.xDel() - function may need
    ** to be called.
    */ 
    if( (pColl->enc & ~SQLITE_UTF16_ALIGNED)==enc2 ){
      CollSeq *aColl = sqlite3HashFind(&db->aCollSeq, zName, strlen(zName));
      int j;
      for(j=0; j<3; j++){
        CollSeq *p = &aColl[j];
        if( p->enc==pColl->enc ){
          if( p->xDel ){
            p->xDel(p->pUser);
          }
          p->xCmp = 0;
        }
      }
    }
  }

  pColl = sqlite3FindCollSeq(db, (u8)enc2, zName, strlen(zName), 1);
  if( pColl ){
    pColl->xCmp = xCompare;
    pColl->pUser = pCtx;
    pColl->xDel = xDel;
    pColl->enc = enc2 | (enc & SQLITE_UTF16_ALIGNED);
  }
  sqlite3Error(db, SQLITE_OK, 0);
  return SQLITE_OK;
}


/*
** This routine does the work of opening a database on behalf of
** sqlite3_open() and sqlite3_open16(). The database filename "zFilename"  
** is UTF-8 encoded.
*/
static int openDatabase(
  const char *zFilename, /* Database filename UTF-8 encoded */
  sqlite3 **ppDb,        /* OUT: Returned database handle */
  unsigned flags,        /* Operational flags */
  const char *zVfs       /* Name of the VFS to use */
){
  sqlite3 *db;
  int rc;
  CollSeq *pColl;

  /* Allocate the sqlite data structure */
  db = sqlite3MallocZero( sizeof(sqlite3) );
  if( db==0 ) goto opendb_out;
  db->mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_RECURSIVE);
  if( db->mutex==0 ){
    sqlite3_free(db);
    db = 0;
    goto opendb_out;
  }
  sqlite3_mutex_enter(db->mutex);
  db->errMask = 0xff;
  db->priorNewRowid = 0;
  db->nDb = 2;
  db->magic = SQLITE_MAGIC_BUSY;
  db->aDb = db->aDbStatic;
  db->autoCommit = 1;
  db->nextAutovac = -1;
  db->flags |= SQLITE_ShortColNames
#if SQLITE_DEFAULT_FILE_FORMAT<4
                 | SQLITE_LegacyFileFmt
#endif
#ifdef SQLITE_ENABLE_LOAD_EXTENSION
                 | SQLITE_LoadExtension
#endif
      ;
  sqlite3HashInit(&db->aFunc, SQLITE_HASH_STRING, 0);
  sqlite3HashInit(&db->aCollSeq, SQLITE_HASH_STRING, 0);
#ifndef SQLITE_OMIT_VIRTUALTABLE
  sqlite3HashInit(&db->aModule, SQLITE_HASH_STRING, 0);
#endif

  db->pVfs = sqlite3_vfs_find(zVfs);
  if( !db->pVfs ){
    rc = SQLITE_ERROR;
    db->magic = SQLITE_MAGIC_SICK;
    sqlite3Error(db, rc, "no such vfs: %s", (zVfs?zVfs:"(null)"));
    goto opendb_out;
  }

  /* Add the default collation sequence BINARY. BINARY works for both UTF-8
  ** and UTF-16, so add a version for each to avoid any unnecessary
  ** conversions. The only error that can occur here is a malloc() failure.
  */
  if( createCollation(db, "BINARY", SQLITE_UTF8, 0, binCollFunc, 0) ||
      createCollation(db, "BINARY", SQLITE_UTF16BE, 0, binCollFunc, 0) ||
      createCollation(db, "BINARY", SQLITE_UTF16LE, 0, binCollFunc, 0) ||
      createCollation(db, "RTRIM", SQLITE_UTF8, (void*)1, binCollFunc, 0) ||
      (db->pDfltColl = sqlite3FindCollSeq(db, SQLITE_UTF8, "BINARY", 6, 0))==0 
  ){
    assert( db->mallocFailed );
    db->magic = SQLITE_MAGIC_SICK;
    goto opendb_out;
  }

  /* Also add a UTF-8 case-insensitive collation sequence. */
  createCollation(db, "NOCASE", SQLITE_UTF8, 0, nocaseCollatingFunc, 0);

  /* Set flags on the built-in collating sequences */
  db->pDfltColl->type = SQLITE_COLL_BINARY;
  pColl = sqlite3FindCollSeq(db, SQLITE_UTF8, "NOCASE", 6, 0);
  if( pColl ){
    pColl->type = SQLITE_COLL_NOCASE;
  }

  /* Open the backend database driver */
  db->openFlags = flags;
  rc = sqlite3BtreeFactory(db, zFilename, 0, SQLITE_DEFAULT_CACHE_SIZE, 
                           flags | SQLITE_OPEN_MAIN_DB,
                           &db->aDb[0].pBt);
  if( rc!=SQLITE_OK ){
    sqlite3Error(db, rc, 0);
    db->magic = SQLITE_MAGIC_SICK;
    goto opendb_out;
  }
  db->aDb[0].pSchema = sqlite3SchemaGet(db, db->aDb[0].pBt);
  db->aDb[1].pSchema = sqlite3SchemaGet(db, 0);


  /* The default safety_level for the main database is 'full'; for the temp
  ** database it is 'NONE'. This matches the pager layer defaults.  
  */
  db->aDb[0].zName = "main";
  db->aDb[0].safety_level = 3;
#ifndef SQLITE_OMIT_TEMPDB
  db->aDb[1].zName = "temp";
  db->aDb[1].safety_level = 1;
#endif

  db->magic = SQLITE_MAGIC_OPEN;
  if( db->mallocFailed ){
    goto opendb_out;
  }

  /* Register all built-in functions, but do not attempt to read the
  ** database schema yet. This is delayed until the first time the database
  ** is accessed.
  */
  sqlite3Error(db, SQLITE_OK, 0);
  sqlite3RegisterBuiltinFunctions(db);

  /* Load automatic extensions - extensions that have been registered
  ** using the sqlite3_automatic_extension() API.
  */
  (void)sqlite3AutoLoadExtensions(db);
  if( sqlite3_errcode(db)!=SQLITE_OK ){
    goto opendb_out;
  }

#ifdef SQLITE_ENABLE_FTS1
  if( !db->mallocFailed ){
    extern int sqlite3Fts1Init(sqlite3*);
    rc = sqlite3Fts1Init(db);
  }
#endif

#ifdef SQLITE_ENABLE_FTS2
  if( !db->mallocFailed && rc==SQLITE_OK ){
    extern int sqlite3Fts2Init(sqlite3*);
    rc = sqlite3Fts2Init(db);
  }
#endif

#ifdef SQLITE_ENABLE_FTS3
  if( !db->mallocFailed && rc==SQLITE_OK ){
    rc = sqlite3Fts3Init(db);
  }
#endif

#ifdef SQLITE_ENABLE_ICU
  if( !db->mallocFailed && rc==SQLITE_OK ){
    extern int sqlite3IcuInit(sqlite3*);
    rc = sqlite3IcuInit(db);
  }
#endif
  sqlite3Error(db, rc, 0);

  /* -DSQLITE_DEFAULT_LOCKING_MODE=1 makes EXCLUSIVE the default locking
  ** mode.  -DSQLITE_DEFAULT_LOCKING_MODE=0 make NORMAL the default locking
  ** mode.  Doing nothing at all also makes NORMAL the default.
  */
#ifdef SQLITE_DEFAULT_LOCKING_MODE
  db->dfltLockMode = SQLITE_DEFAULT_LOCKING_MODE;
  sqlite3PagerLockingMode(sqlite3BtreePager(db->aDb[0].pBt),
                          SQLITE_DEFAULT_LOCKING_MODE);
#endif

opendb_out:
  if( db && db->mutex ){
    sqlite3_mutex_leave(db->mutex);
  }
  if( SQLITE_NOMEM==(rc = sqlite3_errcode(db)) ){
    sqlite3_close(db);
    db = 0;
  }
  *ppDb = db;
  return sqlite3ApiExit(0, rc);
}

/*
** Open a new database handle.
*/
int sqlite3_open(
  const char *zFilename, 
  sqlite3 **ppDb 
){
  return openDatabase(zFilename, ppDb,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
}
int sqlite3_open_v2(
  const char *filename,   /* Database filename (UTF-8) */
  sqlite3 **ppDb,         /* OUT: SQLite db handle */
  int flags,              /* Flags */
  const char *zVfs        /* Name of VFS module to use */
){
  return openDatabase(filename, ppDb, flags, zVfs);
}

#ifndef SQLITE_OMIT_UTF16
/*
** Open a new database handle.
*/
int sqlite3_open16(
  const void *zFilename, 
  sqlite3 **ppDb
){
  char const *zFilename8;   /* zFilename encoded in UTF-8 instead of UTF-16 */
  sqlite3_value *pVal;
  int rc = SQLITE_NOMEM;

  assert( zFilename );
  assert( ppDb );
  *ppDb = 0;
  pVal = sqlite3ValueNew(0);
  sqlite3ValueSetStr(pVal, -1, zFilename, SQLITE_UTF16NATIVE, SQLITE_STATIC);
  zFilename8 = sqlite3ValueText(pVal, SQLITE_UTF8);
  if( zFilename8 ){
    rc = openDatabase(zFilename8, ppDb,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);
    if( rc==SQLITE_OK && *ppDb ){
      rc = sqlite3_exec(*ppDb, "PRAGMA encoding = 'UTF-16'", 0, 0, 0);
      if( rc!=SQLITE_OK ){
        sqlite3_close(*ppDb);
        *ppDb = 0;
      }
    }
  }
  sqlite3ValueFree(pVal);

  return sqlite3ApiExit(0, rc);
}
#endif /* SQLITE_OMIT_UTF16 */

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
  int rc;
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  rc = createCollation(db, zName, enc, pCtx, xCompare, 0);
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** Register a new collation sequence with the database handle db.
*/
int sqlite3_create_collation_v2(
  sqlite3* db, 
  const char *zName, 
  int enc, 
  void* pCtx,
  int(*xCompare)(void*,int,const void*,int,const void*),
  void(*xDel)(void*)
){
  int rc;
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  rc = createCollation(db, zName, enc, pCtx, xCompare, xDel);
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

#ifndef SQLITE_OMIT_UTF16
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
  int rc = SQLITE_OK;
  char *zName8;
  sqlite3_mutex_enter(db->mutex);
  assert( !db->mallocFailed );
  zName8 = sqlite3Utf16to8(db, zName, -1);
  if( zName8 ){
    rc = createCollation(db, zName8, enc, pCtx, xCompare, 0);
    sqlite3_free(zName8);
  }
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}
#endif /* SQLITE_OMIT_UTF16 */

/*
** Register a collation sequence factory callback with the database handle
** db. Replace any previously installed collation sequence factory.
*/
int sqlite3_collation_needed(
  sqlite3 *db, 
  void *pCollNeededArg, 
  void(*xCollNeeded)(void*,sqlite3*,int eTextRep,const char*)
){
  sqlite3_mutex_enter(db->mutex);
  db->xCollNeeded = xCollNeeded;
  db->xCollNeeded16 = 0;
  db->pCollNeededArg = pCollNeededArg;
  sqlite3_mutex_leave(db->mutex);
  return SQLITE_OK;
}

#ifndef SQLITE_OMIT_UTF16
/*
** Register a collation sequence factory callback with the database handle
** db. Replace any previously installed collation sequence factory.
*/
int sqlite3_collation_needed16(
  sqlite3 *db, 
  void *pCollNeededArg, 
  void(*xCollNeeded16)(void*,sqlite3*,int eTextRep,const void*)
){
  sqlite3_mutex_enter(db->mutex);
  db->xCollNeeded = 0;
  db->xCollNeeded16 = xCollNeeded16;
  db->pCollNeededArg = pCollNeededArg;
  sqlite3_mutex_leave(db->mutex);
  return SQLITE_OK;
}
#endif /* SQLITE_OMIT_UTF16 */

#ifndef SQLITE_OMIT_GLOBALRECOVER
/*
** This function is now an anachronism. It used to be used to recover from a
** malloc() failure, but SQLite now does this automatically.
*/
int sqlite3_global_recover(void){
  return SQLITE_OK;
}
#endif

/*
** Test to see whether or not the database connection is in autocommit
** mode.  Return TRUE if it is and FALSE if not.  Autocommit mode is on
** by default.  Autocommit is disabled by a BEGIN statement and reenabled
** by the next COMMIT or ROLLBACK.
**
******* THIS IS AN EXPERIMENTAL API AND IS SUBJECT TO CHANGE ******
*/
int sqlite3_get_autocommit(sqlite3 *db){
  return db->autoCommit;
}

#ifdef SQLITE_DEBUG
/*
** The following routine is subtituted for constant SQLITE_CORRUPT in
** debugging builds.  This provides a way to set a breakpoint for when
** corruption is first detected.
*/
int sqlite3Corrupt(void){
  return SQLITE_CORRUPT;
}
#endif

/*
** This is a convenience routine that makes sure that all thread-specific
** data for this thread has been deallocated.
**
** SQLite no longer uses thread-specific data so this routine is now a
** no-op.  It is retained for historical compatibility.
*/
void sqlite3_thread_cleanup(void){
}

/*
** Return meta information about a specific column of a database table.
** See comment in sqlite3.h (sqlite.h.in) for details.
*/
#ifdef SQLITE_ENABLE_COLUMN_METADATA
int sqlite3_table_column_metadata(
  sqlite3 *db,                /* Connection handle */
  const char *zDbName,        /* Database name or NULL */
  const char *zTableName,     /* Table name */
  const char *zColumnName,    /* Column name */
  char const **pzDataType,    /* OUTPUT: Declared data type */
  char const **pzCollSeq,     /* OUTPUT: Collation sequence name */
  int *pNotNull,              /* OUTPUT: True if NOT NULL constraint exists */
  int *pPrimaryKey,           /* OUTPUT: True if column part of PK */
  int *pAutoinc               /* OUTPUT: True if colums is auto-increment */
){
  int rc;
  char *zErrMsg = 0;
  Table *pTab = 0;
  Column *pCol = 0;
  int iCol;

  char const *zDataType = 0;
  char const *zCollSeq = 0;
  int notnull = 0;
  int primarykey = 0;
  int autoinc = 0;

  /* Ensure the database schema has been loaded */
  (void)sqlite3SafetyOn(db);
  sqlite3_mutex_enter(db->mutex);
  rc = sqlite3Init(db, &zErrMsg);
  if( SQLITE_OK!=rc ){
    goto error_out;
  }

  /* Locate the table in question */
  pTab = sqlite3FindTable(db, zTableName, zDbName);
  if( !pTab || pTab->pSelect ){
    pTab = 0;
    goto error_out;
  }

  /* Find the column for which info is requested */
  if( sqlite3IsRowid(zColumnName) ){
    iCol = pTab->iPKey;
    if( iCol>=0 ){
      pCol = &pTab->aCol[iCol];
    }
  }else{
    for(iCol=0; iCol<pTab->nCol; iCol++){
      pCol = &pTab->aCol[iCol];
      if( 0==sqlite3StrICmp(pCol->zName, zColumnName) ){
        break;
      }
    }
    if( iCol==pTab->nCol ){
      pTab = 0;
      goto error_out;
    }
  }

  /* The following block stores the meta information that will be returned
  ** to the caller in local variables zDataType, zCollSeq, notnull, primarykey
  ** and autoinc. At this point there are two possibilities:
  ** 
  **     1. The specified column name was rowid", "oid" or "_rowid_" 
  **        and there is no explicitly declared IPK column. 
  **
  **     2. The table is not a view and the column name identified an 
  **        explicitly declared column. Copy meta information from *pCol.
  */ 
  if( pCol ){
    zDataType = pCol->zType;
    zCollSeq = pCol->zColl;
    notnull = (pCol->notNull?1:0);
    primarykey  = (pCol->isPrimKey?1:0);
    autoinc = ((pTab->iPKey==iCol && pTab->autoInc)?1:0);
  }else{
    zDataType = "INTEGER";
    primarykey = 1;
  }
  if( !zCollSeq ){
    zCollSeq = "BINARY";
  }

error_out:
  (void)sqlite3SafetyOff(db);

  /* Whether the function call succeeded or failed, set the output parameters
  ** to whatever their local counterparts contain. If an error did occur,
  ** this has the effect of zeroing all output parameters.
  */
  if( pzDataType ) *pzDataType = zDataType;
  if( pzCollSeq ) *pzCollSeq = zCollSeq;
  if( pNotNull ) *pNotNull = notnull;
  if( pPrimaryKey ) *pPrimaryKey = primarykey;
  if( pAutoinc ) *pAutoinc = autoinc;

  if( SQLITE_OK==rc && !pTab ){
    sqlite3SetString(&zErrMsg, "no such table column: ", zTableName, ".", 
        zColumnName, 0);
    rc = SQLITE_ERROR;
  }
  sqlite3Error(db, rc, (zErrMsg?"%s":0), zErrMsg);
  sqlite3_free(zErrMsg);
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}
#endif

/*
** Sleep for a little while.  Return the amount of time slept.
*/
int sqlite3_sleep(int ms){
  sqlite3_vfs *pVfs;
  int rc;
  pVfs = sqlite3_vfs_find(0);

  /* This function works in milliseconds, but the underlying OsSleep() 
  ** API uses microseconds. Hence the 1000's.
  */
  rc = (sqlite3OsSleep(pVfs, 1000*ms)/1000);
  return rc;
}

/*
** Enable or disable the extended result codes.
*/
int sqlite3_extended_result_codes(sqlite3 *db, int onoff){
  sqlite3_mutex_enter(db->mutex);
  db->errMask = onoff ? 0xffffffff : 0xff;
  sqlite3_mutex_leave(db->mutex);
  return SQLITE_OK;
}

/*
** Invoke the xFileControl method on a particular database.
*/
int sqlite3_file_control(sqlite3 *db, const char *zDbName, int op, void *pArg){
  int rc = SQLITE_ERROR;
  int iDb;
  sqlite3_mutex_enter(db->mutex);
  if( zDbName==0 ){
    iDb = 0;
  }else{
    for(iDb=0; iDb<db->nDb; iDb++){
      if( strcmp(db->aDb[iDb].zName, zDbName)==0 ) break;
    }
  }
  if( iDb<db->nDb ){
    Btree *pBtree = db->aDb[iDb].pBt;
    if( pBtree ){
      Pager *pPager;
      sqlite3_file *fd;
      sqlite3BtreeEnter(pBtree);
      pPager = sqlite3BtreePager(pBtree);
      assert( pPager!=0 );
      fd = sqlite3PagerFile(pPager);
      assert( fd!=0 );
      if( fd->pMethods ){
        rc = sqlite3OsFileControl(fd, op, pArg);
      }
      sqlite3BtreeLeave(pBtree);
    }
  }
  sqlite3_mutex_leave(db->mutex);
  return rc;   
}

/*
** Interface to the testing logic.
*/
int sqlite3_test_control(int op, ...){
  va_list ap;
  int rc = 0;
  va_start(ap, op);
  switch( op ){
#ifndef SQLITE_OMIT_FAULTINJECTOR
    case SQLITE_TESTCTRL_FAULT_CONFIG: {
      int id = va_arg(ap, int);
      int nDelay = va_arg(ap, int);
      int nRepeat = va_arg(ap, int);
      sqlite3FaultConfig(id, nDelay, nRepeat);
      break;
    }
    case SQLITE_TESTCTRL_FAULT_FAILURES: {
      int id = va_arg(ap, int);
      rc = sqlite3FaultFailures(id);
      break;
    }
    case SQLITE_TESTCTRL_FAULT_BENIGN_FAILURES: {
      int id = va_arg(ap, int);
      rc = sqlite3FaultBenignFailures(id);
      break;
    }
    case SQLITE_TESTCTRL_FAULT_PENDING: {
      int id = va_arg(ap, int);
      rc = sqlite3FaultPending(id);
      break;
    }
#endif /* SQLITE_OMIT_FAULTINJECTOR */
  }
  va_end(ap);
  return rc;
}
