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

#ifndef SQLITE_OMIT_GLOBALRECOVER
/*
** Linked list of all open database handles. This is used by the 
** sqlite3_global_recover() function. Entries are added to the list
** by openDatabase() and removed by sqlite3_close().
*/
static sqlite3 *pDbList = 0;
#endif

#ifndef SQLITE_OMIT_UTF16
/* 
** Return the transient sqlite3_value object used for encoding conversions
** during SQL compilation.
*/
sqlite3_value *sqlite3GetTransientValue(sqlite3 *db){
  if( !db->pValue ){
    db->pValue = sqlite3ValueNew();
  }
  return db->pValue;
}
#endif

/*
** The version of the library
*/
const char rcsid3[] = "@(#) \044Id: SQLite version " SQLITE_VERSION " $";
const char sqlite3_version[] = SQLITE_VERSION;
const char *sqlite3_libversion(void){ return sqlite3_version; }
int sqlite3_libversion_number(void){ return SQLITE_VERSION_NUMBER; }

/*
** This is the default collating function named "BINARY" which is always
** available.
*/
static int binCollFunc(
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

#ifdef SQLITE_SSE
  sqlite3_finalize(db->pFetch);
#endif 

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

#ifndef SQLITE_OMIT_GLOBALRECOVER
  {
    sqlite3 *pPrev;
    sqlite3OsEnterMutex();
    pPrev = pDbList;
    while( pPrev && pPrev->pNext!=db ){
      pPrev = pPrev->pNext;
    }
    if( pPrev ){
      pPrev->pNext = db->pNext;
    }else{
      assert( pDbList==db );
      pDbList = db->pNext;
    }
    sqlite3OsLeaveMutex();
  }
#endif

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
    case SQLITE_PROTOCOL:   z = "database locking protocol failure";     break;
    case SQLITE_EMPTY:      z = "table contains no data";                break;
    case SQLITE_SCHEMA:     z = "database schema has changed";           break;
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
#if SQLITE_MIN_SLEEP_MS==1
  static const u8 delays[] =
     { 1, 2, 5, 10, 15, 20, 25, 25,  25,  50,  50, 100 };
  static const u8 totals[] =
     { 0, 1, 3,  8, 18, 33, 53, 78, 103, 128, 178, 228 };
# define NDELAY (sizeof(delays)/sizeof(delays[0]))
  int timeout = ((sqlite3 *)ptr)->busyTimeout;
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
  sqlite3OsSleep(delay);
  return 1;
#else
  int timeout = ((sqlite3 *)ptr)->busyTimeout;
  if( (count+1)*1000 > timeout ){
    return 0;
  }
  sqlite3OsSleep(1000);
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
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->busyHandler.xFunc = xBusy;
  db->busyHandler.pArg = pArg;
  db->busyHandler.nBusy = 0;
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
    rc = sqlite3_create_function(db, zFunctionName, nArg, SQLITE_UTF8,
         pUserData, xFunc, xStep, xFinal);
    if( rc!=SQLITE_OK ) return rc;
    rc = sqlite3_create_function(db, zFunctionName, nArg, SQLITE_UTF16LE,
        pUserData, xFunc, xStep, xFinal);
    if( rc!=SQLITE_OK ) return rc;
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
      return SQLITE_BUSY;
    }else{
      sqlite3ExpirePreparedStatements(db);
    }
  }

  p = sqlite3FindFunction(db, zFunctionName, nName, nArg, enc, 1);
  if( p==0 ) return SQLITE_NOMEM;
  p->flags = 0;
  p->xFunc = xFunc;
  p->xStep = xStep;
  p->xFinalize = xFinal;
  p->pUserData = pUserData;
  return SQLITE_OK;
}
#ifndef SQLITE_OMIT_UTF16
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
#endif

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
  void *pOld = db->pTraceArg;
  db->xTrace = xTrace;
  db->pTraceArg = pArg;
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
  void *pOld = db->pProfileArg;
  db->xProfile = xProfile;
  db->pProfileArg = pArg;
  return pOld;
}
#endif /* SQLITE_OMIT_TRACE */

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
  Btree **ppBtree           /* Pointer to new Btree object written here */
){
  int btree_flags = 0;
  int rc;
  
  assert( ppBtree != 0);
  if( omitJournal ){
    btree_flags |= BTREE_OMIT_JOURNAL;
  }
  if( db->flags & SQLITE_NoReadlock ){
    btree_flags |= BTREE_NO_READLOCK;
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
#endif /* SQLITE_OMIT_UTF16 */

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
** This routine does the work of opening a database on behalf of
** sqlite3_open() and sqlite3_open16(). The database filename "zFilename"  
** is UTF-8 encoded.
*/
static int openDatabase(
  const char *zFilename, /* Database filename UTF-8 encoded */
  sqlite3 **ppDb         /* OUT: Returned database handle */
){
  sqlite3 *db;
  int rc, i;
  CollSeq *pColl;

  /* Allocate the sqlite data structure */
  db = sqliteMalloc( sizeof(sqlite3) );
  if( db==0 ) goto opendb_out;
  db->priorNewRowid = 0;
  db->magic = SQLITE_MAGIC_BUSY;
  db->nDb = 2;
  db->aDb = db->aDbStatic;
  db->enc = SQLITE_UTF8;
  db->autoCommit = 1;
  db->flags |= SQLITE_ShortColNames;
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
  if( sqlite3_create_collation(db, "BINARY", SQLITE_UTF8, 0,binCollFunc) ||
      sqlite3_create_collation(db, "BINARY", SQLITE_UTF16, 0,binCollFunc) ||
      !(db->pDfltColl = sqlite3FindCollSeq(db, db->enc, "BINARY", 6, 0)) ){
    rc = db->errCode;
    assert( rc!=SQLITE_OK );
    db->magic = SQLITE_MAGIC_CLOSED;
    goto opendb_out;
  }

  /* Also add a UTF-8 case-insensitive collation sequence. */
  sqlite3_create_collation(db, "NOCASE", SQLITE_UTF8, 0, nocaseCollatingFunc);

  /* Set flags on the built-in collating sequences */
  db->pDfltColl->type = SQLITE_COLL_BINARY;
  pColl = sqlite3FindCollSeq(db, SQLITE_UTF8, "NOCASE", 6, 0);
  if( pColl ){
    pColl->type = SQLITE_COLL_NOCASE;
  }

  /* Open the backend database driver */
  rc = sqlite3BtreeFactory(db, zFilename, 0, MAX_PAGES, &db->aDb[0].pBt);
  if( rc!=SQLITE_OK ){
    sqlite3Error(db, rc, 0);
    db->magic = SQLITE_MAGIC_CLOSED;
    goto opendb_out;
  }

  /* The default safety_level for the main database is 'full'; for the temp
  ** database it is 'NONE'. This matches the pager layer defaults.  
  */
  db->aDb[0].zName = "main";
  db->aDb[0].safety_level = 3;
#ifndef SQLITE_OMIT_TEMPDB
  db->aDb[1].zName = "temp";
  db->aDb[1].safety_level = 1;
#endif


  /* Register all built-in functions, but do not attempt to read the
  ** database schema yet. This is delayed until the first time the database
  ** is accessed.
  */
  sqlite3RegisterBuiltinFunctions(db);
  sqlite3Error(db, SQLITE_OK, 0);
  db->magic = SQLITE_MAGIC_OPEN;

opendb_out:
  if( sqlite3_errcode(db)==SQLITE_OK && sqlite3_malloc_failed ){
    sqlite3Error(db, SQLITE_NOMEM, 0);
  }
  *ppDb = db;
#ifndef SQLITE_OMIT_GLOBALRECOVER
  if( db ){
    sqlite3OsEnterMutex();
    db->pNext = pDbList;
    pDbList = db;
    sqlite3OsLeaveMutex();
  }
#endif
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

#ifndef SQLITE_OMIT_UTF16
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
#endif /* SQLITE_OMIT_UTF16 */

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

  /* Check if this call is removing or replacing an existing collation 
  ** sequence. If so, and there are active VMs, return busy. If there
  ** are no active VMs, invalidate any pre-compiled statements.
  */
  pColl = sqlite3FindCollSeq(db, (u8)enc, zName, strlen(zName), 0);
  if( pColl && pColl->xCmp ){
    if( db->activeVdbeCnt ){
      sqlite3Error(db, SQLITE_BUSY, 
        "Unable to delete/modify collation sequence due to active statements");
      return SQLITE_BUSY;
    }
    sqlite3ExpirePreparedStatements(db);
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
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->xCollNeeded = xCollNeeded;
  db->xCollNeeded16 = 0;
  db->pCollNeededArg = pCollNeededArg;
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
  if( sqlite3SafetyCheck(db) ){
    return SQLITE_MISUSE;
  }
  db->xCollNeeded = 0;
  db->xCollNeeded16 = xCollNeeded16;
  db->pCollNeededArg = pCollNeededArg;
  return SQLITE_OK;
}
#endif /* SQLITE_OMIT_UTF16 */

#ifndef SQLITE_OMIT_GLOBALRECOVER
/*
** This function is called to recover from a malloc failure that occured
** within SQLite. 
**
** This function is *not* threadsafe. Calling this from within a threaded
** application when threads other than the caller have used SQLite is 
** dangerous and will almost certainly result in malfunctions.
*/
int sqlite3_global_recover(){
  int rc = SQLITE_OK;

  if( sqlite3_malloc_failed ){
    sqlite3 *db;
    int i;
    sqlite3_malloc_failed = 0;
    for(db=pDbList; db; db=db->pNext ){
      sqlite3ExpirePreparedStatements(db);
      for(i=0; i<db->nDb; i++){
        Btree *pBt = db->aDb[i].pBt;
        if( pBt && (rc=sqlite3BtreeReset(pBt)) ){
          goto recover_out;
        }
      } 
      db->autoCommit = 1;
    }
  }

recover_out:
  if( rc!=SQLITE_OK ){
    sqlite3_malloc_failed = 1;
  }
  return rc;
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
