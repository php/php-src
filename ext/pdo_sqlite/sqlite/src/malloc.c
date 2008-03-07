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
** Memory allocation functions used throughout sqlite.
**
**
** $Id$
*/
#include "sqliteInt.h"
#include <stdarg.h>
#include <ctype.h>

/*
** This routine runs when the memory allocator sees that the
** total memory allocation is about to exceed the soft heap
** limit.
*/
static void softHeapLimitEnforcer(
  void *NotUsed, 
  sqlite3_int64 inUse,
  int allocSize
){
  sqlite3_release_memory(allocSize);
}

/*
** Set the soft heap-size limit for the current thread. Passing a
** zero or negative value indicates no limit.
*/
void sqlite3_soft_heap_limit(int n){
  sqlite3_uint64 iLimit;
  int overage;
  if( n<0 ){
    iLimit = 0;
  }else{
    iLimit = n;
  }
  if( iLimit>0 ){
    sqlite3_memory_alarm(softHeapLimitEnforcer, 0, iLimit);
  }else{
    sqlite3_memory_alarm(0, 0, 0);
  }
  overage = sqlite3_memory_used() - n;
  if( overage>0 ){
    sqlite3_release_memory(overage);
  }
}

/*
** Release memory held by SQLite instances created by the current thread.
*/
int sqlite3_release_memory(int n){
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  return sqlite3PagerReleaseMemory(n);
#else
  return SQLITE_OK;
#endif
}


/*
** Allocate and zero memory.
*/ 
void *sqlite3MallocZero(unsigned n){
  void *p = sqlite3_malloc(n);
  if( p ){
    memset(p, 0, n);
  }
  return p;
}

/*
** Allocate and zero memory.  If the allocation fails, make
** the mallocFailed flag in the connection pointer.
*/
void *sqlite3DbMallocZero(sqlite3 *db, unsigned n){
  void *p = sqlite3DbMallocRaw(db, n);
  if( p ){
    memset(p, 0, n);
  }
  return p;
}

/*
** Allocate and zero memory.  If the allocation fails, make
** the mallocFailed flag in the connection pointer.
*/
void *sqlite3DbMallocRaw(sqlite3 *db, unsigned n){
  void *p = 0;
  if( !db || db->mallocFailed==0 ){
    p = sqlite3_malloc(n);
    if( !p && db ){
      db->mallocFailed = 1;
    }
  }
  return p;
}

/*
** Resize the block of memory pointed to by p to n bytes. If the
** resize fails, set the mallocFailed flag inthe connection object.
*/
void *sqlite3DbRealloc(sqlite3 *db, void *p, int n){
  void *pNew = 0;
  if( db->mallocFailed==0 ){
    pNew = sqlite3_realloc(p, n);
    if( !pNew ){
      db->mallocFailed = 1;
    }
  }
  return pNew;
}

/*
** Attempt to reallocate p.  If the reallocation fails, then free p
** and set the mallocFailed flag in the database connection.
*/
void *sqlite3DbReallocOrFree(sqlite3 *db, void *p, int n){
  void *pNew;
  pNew = sqlite3DbRealloc(db, p, n);
  if( !pNew ){
    sqlite3_free(p);
  }
  return pNew;
}

/*
** Make a copy of a string in memory obtained from sqliteMalloc(). These 
** functions call sqlite3MallocRaw() directly instead of sqliteMalloc(). This
** is because when memory debugging is turned on, these two functions are 
** called via macros that record the current file and line number in the
** ThreadData structure.
*/
char *sqlite3StrDup(const char *z){
  char *zNew;
  int n;
  if( z==0 ) return 0;
  n = strlen(z)+1;
  zNew = sqlite3_malloc(n);
  if( zNew ) memcpy(zNew, z, n);
  return zNew;
}
char *sqlite3StrNDup(const char *z, int n){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqlite3_malloc(n+1);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
  }
  return zNew;
}

char *sqlite3DbStrDup(sqlite3 *db, const char *z){
  char *zNew = sqlite3StrDup(z);
  if( z && !zNew ){
    db->mallocFailed = 1;
  }
  return zNew;
}
char *sqlite3DbStrNDup(sqlite3 *db, const char *z, int n){
  char *zNew = sqlite3StrNDup(z, n);
  if( z && !zNew ){
    db->mallocFailed = 1;
  }
  return zNew;
}

/*
** Create a string from the 2nd and subsequent arguments (up to the
** first NULL argument), store the string in memory obtained from
** sqliteMalloc() and make the pointer indicated by the 1st argument
** point to that string.  The 1st argument must either be NULL or 
** point to memory obtained from sqliteMalloc().
*/
void sqlite3SetString(char **pz, ...){
  va_list ap;
  int nByte;
  const char *z;
  char *zResult;

  assert( pz!=0 );
  nByte = 1;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    nByte += strlen(z);
  }
  va_end(ap);
  sqlite3_free(*pz);
  *pz = zResult = sqlite3_malloc(nByte);
  if( zResult==0 ){
    return;
  }
  *zResult = 0;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    int n = strlen(z);
    memcpy(zResult, z, n);
    zResult += n;
  }
  zResult[0] = 0;
  va_end(ap);
}


/*
** This function must be called before exiting any API function (i.e. 
** returning control to the user) that has called sqlite3_malloc or
** sqlite3_realloc.
**
** The returned value is normally a copy of the second argument to this
** function. However, if a malloc() failure has occured since the previous
** invocation SQLITE_NOMEM is returned instead. 
**
** If the first argument, db, is not NULL and a malloc() error has occured,
** then the connection error-code (the value returned by sqlite3_errcode())
** is set to SQLITE_NOMEM.
*/
int sqlite3ApiExit(sqlite3* db, int rc){
  /* If the db handle is not NULL, then we must hold the connection handle
  ** mutex here. Otherwise the read (and possible write) of db->mallocFailed 
  ** is unsafe, as is the call to sqlite3Error().
  */
  assert( !db || sqlite3_mutex_held(db->mutex) );
  if( db && db->mallocFailed ){
    sqlite3Error(db, SQLITE_NOMEM, 0);
    db->mallocFailed = 0;
    rc = SQLITE_NOMEM;
  }
  return rc & (db ? db->errMask : 0xff);
}
