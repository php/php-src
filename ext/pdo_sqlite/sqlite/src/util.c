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
** Utility functions used throughout sqlite.
**
** This file contains functions for allocating memory, comparing
** strings, and stuff like that.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"
#include <stdarg.h>
#include <ctype.h>

/*
** MALLOC WRAPPER ARCHITECTURE
**
** The sqlite code accesses dynamic memory allocation/deallocation by invoking
** the following six APIs (which may be implemented as macros).
**
**     sqlite3Malloc()
**     sqlite3MallocRaw()
**     sqlite3Realloc()
**     sqlite3ReallocOrFree()
**     sqlite3Free()
**     sqlite3AllocSize()
**
** The function sqlite3FreeX performs the same task as sqlite3Free and is
** guaranteed to be a real function. The same holds for sqlite3MallocX
**
** The above APIs are implemented in terms of the functions provided in the
** operating-system interface. The OS interface is never accessed directly
** by code outside of this file.
**
**     sqlite3OsMalloc()
**     sqlite3OsRealloc()
**     sqlite3OsFree()
**     sqlite3OsAllocationSize()
**
** Functions sqlite3MallocRaw() and sqlite3Realloc() may invoke 
** sqlite3_release_memory() if a call to sqlite3OsMalloc() or
** sqlite3OsRealloc() fails (or if the soft-heap-limit for the thread is
** exceeded). Function sqlite3Malloc() usually invokes
** sqlite3MallocRaw().
**
** MALLOC TEST WRAPPER ARCHITECTURE
**
** The test wrapper provides extra test facilities to ensure the library 
** does not leak memory and handles the failure of the underlying OS level
** allocation system correctly. It is only present if the library is 
** compiled with the SQLITE_MEMDEBUG macro set.
**
**     * Guardposts to detect overwrites.
**     * Ability to cause a specific Malloc() or Realloc() to fail.
**     * Audit outstanding memory allocations (i.e check for leaks).
*/

#define MAX(x,y) ((x)>(y)?(x):(y))

#if defined(SQLITE_ENABLE_MEMORY_MANAGEMENT) && !defined(SQLITE_OMIT_DISKIO)
/*
** Set the soft heap-size limit for the current thread. Passing a negative
** value indicates no limit.
*/
void sqlite3_soft_heap_limit(int n){
  ThreadData *pTd = sqlite3ThreadData();
  if( pTd ){
    pTd->nSoftHeapLimit = n;
  }
  sqlite3ReleaseThreadData();
}

/*
** Release memory held by SQLite instances created by the current thread.
*/
int sqlite3_release_memory(int n){
  return sqlite3PagerReleaseMemory(n);
}
#else
/* If SQLITE_ENABLE_MEMORY_MANAGEMENT is not defined, then define a version
** of sqlite3_release_memory() to be used by other code in this file.
** This is done for no better reason than to reduce the number of 
** pre-processor #ifndef statements.
*/
#define sqlite3_release_memory(x) 0    /* 0 == no memory freed */
#endif

#ifdef SQLITE_MEMDEBUG
/*--------------------------------------------------------------------------
** Begin code for memory allocation system test layer.
**
** Memory debugging is turned on by defining the SQLITE_MEMDEBUG macro.
**
** SQLITE_MEMDEBUG==1    -> Fence-posting only (thread safe) 
** SQLITE_MEMDEBUG==2    -> Fence-posting + linked list of allocations (not ts)
** SQLITE_MEMDEBUG==3    -> Above + backtraces (not thread safe, req. glibc)
*/

/* Figure out whether or not to store backtrace() information for each malloc.
** The backtrace() function is only used if SQLITE_MEMDEBUG is set to 2 or 
** greater and glibc is in use. If we don't want to use backtrace(), then just
** define it as an empty macro and set the amount of space reserved to 0.
*/
#if defined(__GLIBC__) && SQLITE_MEMDEBUG>2
  extern int backtrace(void **, int);
  #define TESTALLOC_STACKSIZE 128
  #define TESTALLOC_STACKFRAMES ((TESTALLOC_STACKSIZE-8)/sizeof(void*))
#else
  #define backtrace(x, y)
  #define TESTALLOC_STACKSIZE 0
  #define TESTALLOC_STACKFRAMES 0
#endif

/*
** Number of 32-bit guard words.  This should probably be a multiple of
** 2 since on 64-bit machines we want the value returned by sqliteMalloc()
** to be 8-byte aligned.
*/
#ifndef TESTALLOC_NGUARD
# define TESTALLOC_NGUARD 2
#endif

/*
** Size reserved for storing file-name along with each malloc()ed blob.
*/
#define TESTALLOC_FILESIZE 64

/*
** Size reserved for storing the user string. Each time a Malloc() or Realloc()
** call succeeds, up to TESTALLOC_USERSIZE bytes of the string pointed to by
** sqlite3_malloc_id are stored along with the other test system metadata.
*/
#define TESTALLOC_USERSIZE 64
const char *sqlite3_malloc_id = 0;

/*
** Blocks used by the test layer have the following format:
**
**        <sizeof(void *) pNext pointer>
**        <sizeof(void *) pPrev pointer>
**        <TESTALLOC_NGUARD 32-bit guard words>
**            <The application level allocation>
**        <TESTALLOC_NGUARD 32-bit guard words>
**        <32-bit line number>
**        <TESTALLOC_FILESIZE bytes containing null-terminated file name>
**        <TESTALLOC_STACKSIZE bytes of backtrace() output>
*/ 

#define TESTALLOC_OFFSET_GUARD1(p)    (sizeof(void *) * 2)
#define TESTALLOC_OFFSET_DATA(p) ( \
  TESTALLOC_OFFSET_GUARD1(p) + sizeof(u32) * TESTALLOC_NGUARD \
)
#define TESTALLOC_OFFSET_GUARD2(p) ( \
  TESTALLOC_OFFSET_DATA(p) + sqlite3OsAllocationSize(p) - TESTALLOC_OVERHEAD \
)
#define TESTALLOC_OFFSET_LINENUMBER(p) ( \
  TESTALLOC_OFFSET_GUARD2(p) + sizeof(u32) * TESTALLOC_NGUARD \
)
#define TESTALLOC_OFFSET_FILENAME(p) ( \
  TESTALLOC_OFFSET_LINENUMBER(p) + sizeof(u32) \
)
#define TESTALLOC_OFFSET_USER(p) ( \
  TESTALLOC_OFFSET_FILENAME(p) + TESTALLOC_FILESIZE \
)
#define TESTALLOC_OFFSET_STACK(p) ( \
  TESTALLOC_OFFSET_USER(p) + TESTALLOC_USERSIZE + 8 - \
  (TESTALLOC_OFFSET_USER(p) % 8) \
)

#define TESTALLOC_OVERHEAD ( \
  sizeof(void *)*2 +                   /* pPrev and pNext pointers */   \
  TESTALLOC_NGUARD*sizeof(u32)*2 +              /* Guard words */       \
  sizeof(u32) + TESTALLOC_FILESIZE +   /* File and line number */       \
  TESTALLOC_USERSIZE +                 /* User string */                \
  TESTALLOC_STACKSIZE                  /* backtrace() stack */          \
)


/*
** For keeping track of the number of mallocs and frees.   This
** is used to check for memory leaks.  The iMallocFail and iMallocReset
** values are used to simulate malloc() failures during testing in 
** order to verify that the library correctly handles an out-of-memory
** condition.
*/
int sqlite3_nMalloc;         /* Number of sqliteMalloc() calls */
int sqlite3_nFree;           /* Number of sqliteFree() calls */
int sqlite3_memUsed;         /* TODO Total memory obtained from malloc */
int sqlite3_memMax;          /* TODO Mem usage high-water mark */
int sqlite3_iMallocFail;     /* Fail sqliteMalloc() after this many calls */
int sqlite3_iMallocReset = -1; /* When iMallocFail reaches 0, set to this */

void *sqlite3_pFirst = 0;         /* Pointer to linked list of allocations */
int sqlite3_nMaxAlloc = 0;        /* High water mark of ThreadData.nAlloc */
int sqlite3_mallocDisallowed = 0; /* assert() in sqlite3Malloc() if set */
int sqlite3_isFail = 0;           /* True if all malloc calls should fail */
const char *sqlite3_zFile = 0;    /* Filename to associate debug info with */
int sqlite3_iLine = 0;            /* Line number for debug info */

/*
** Check for a simulated memory allocation failure.  Return true if
** the failure should be simulated.  Return false to proceed as normal.
*/
int sqlite3TestMallocFail(){
  if( sqlite3_isFail ){
    return 1;
  }
  if( sqlite3_iMallocFail>=0 ){
    sqlite3_iMallocFail--;
    if( sqlite3_iMallocFail==0 ){
      sqlite3_iMallocFail = sqlite3_iMallocReset;
      sqlite3_isFail = 1;
      return 1;
    }
  }
  return 0;
}

/*
** The argument is a pointer returned by sqlite3OsMalloc() or xRealloc().
** assert() that the first and last (TESTALLOC_NGUARD*4) bytes are set to the
** values set by the applyGuards() function.
*/
static void checkGuards(u32 *p)
{
  int i;
  char *zAlloc = (char *)p;
  char *z;

  /* First set of guard words */
  z = &zAlloc[TESTALLOC_OFFSET_GUARD1(p)];
  for(i=0; i<TESTALLOC_NGUARD; i++){
    assert(((u32 *)z)[i]==0xdead1122);
  }

  /* Second set of guard words */
  z = &zAlloc[TESTALLOC_OFFSET_GUARD2(p)];
  for(i=0; i<TESTALLOC_NGUARD; i++){
    u32 guard = 0;
    memcpy(&guard, &z[i*sizeof(u32)], sizeof(u32));
    assert(guard==0xdead3344);
  }
}

/*
** The argument is a pointer returned by sqlite3OsMalloc() or Realloc(). The
** first and last (TESTALLOC_NGUARD*4) bytes are set to known values for use as 
** guard-posts.
*/
static void applyGuards(u32 *p)
{
  int i;
  char *z;
  char *zAlloc = (char *)p;

  /* First set of guard words */
  z = &zAlloc[TESTALLOC_OFFSET_GUARD1(p)];
  for(i=0; i<TESTALLOC_NGUARD; i++){
    ((u32 *)z)[i] = 0xdead1122;
  }

  /* Second set of guard words */
  z = &zAlloc[TESTALLOC_OFFSET_GUARD2(p)];
  for(i=0; i<TESTALLOC_NGUARD; i++){
    static const int guard = 0xdead3344;
    memcpy(&z[i*sizeof(u32)], &guard, sizeof(u32));
  }

  /* Line number */
  z = &((char *)z)[TESTALLOC_NGUARD*sizeof(u32)];             /* Guard words */
  z = &zAlloc[TESTALLOC_OFFSET_LINENUMBER(p)];
  memcpy(z, &sqlite3_iLine, sizeof(u32));

  /* File name */
  z = &zAlloc[TESTALLOC_OFFSET_FILENAME(p)];
  strncpy(z, sqlite3_zFile, TESTALLOC_FILESIZE);
  z[TESTALLOC_FILESIZE - 1] = '\0';

  /* User string */
  z = &zAlloc[TESTALLOC_OFFSET_USER(p)];
  z[0] = 0;
  if( sqlite3_malloc_id ){
    strncpy(z, sqlite3_malloc_id, TESTALLOC_USERSIZE);
    z[TESTALLOC_USERSIZE-1] = 0;
  }

  /* backtrace() stack */
  z = &zAlloc[TESTALLOC_OFFSET_STACK(p)];
  backtrace((void **)z, TESTALLOC_STACKFRAMES);

  /* Sanity check to make sure checkGuards() is working */
  checkGuards(p);
}

/*
** The argument is a malloc()ed pointer as returned by the test-wrapper.
** Return a pointer to the Os level allocation.
*/
static void *getOsPointer(void *p)
{
  char *z = (char *)p;
  return (void *)(&z[-1 * TESTALLOC_OFFSET_DATA(p)]);
}


#if SQLITE_MEMDEBUG>1
/*
** The argument points to an Os level allocation. Link it into the threads list
** of allocations.
*/
static void linkAlloc(void *p){
  void **pp = (void **)p;
  pp[0] = 0;
  pp[1] = sqlite3_pFirst;
  if( sqlite3_pFirst ){
    ((void **)sqlite3_pFirst)[0] = p;
  }
  sqlite3_pFirst = p;
}

/*
** The argument points to an Os level allocation. Unlinke it from the threads
** list of allocations.
*/
static void unlinkAlloc(void *p)
{
  void **pp = (void **)p;
  if( p==sqlite3_pFirst ){
    assert(!pp[0]);
    assert(!pp[1] || ((void **)(pp[1]))[0]==p);
    sqlite3_pFirst = pp[1];
    if( sqlite3_pFirst ){
      ((void **)sqlite3_pFirst)[0] = 0;
    }
  }else{
    void **pprev = pp[0];
    void **pnext = pp[1];
    assert(pprev);
    assert(pprev[1]==p);
    pprev[1] = (void *)pnext;
    if( pnext ){
      assert(pnext[0]==p);
      pnext[0] = (void *)pprev;
    }
  }
}

/*
** Pointer p is a pointer to an OS level allocation that has just been
** realloc()ed. Set the list pointers that point to this entry to it's new
** location.
*/
static void relinkAlloc(void *p)
{
  void **pp = (void **)p;
  if( pp[0] ){
    ((void **)(pp[0]))[1] = p;
  }else{
    sqlite3_pFirst = p;
  }
  if( pp[1] ){
    ((void **)(pp[1]))[0] = p;
  }
}
#else
#define linkAlloc(x)
#define relinkAlloc(x)
#define unlinkAlloc(x)
#endif

/*
** This function sets the result of the Tcl interpreter passed as an argument
** to a list containing an entry for each currently outstanding call made to 
** sqliteMalloc and friends by the current thread. Each list entry is itself a
** list, consisting of the following (in order):
**
**     * The number of bytes allocated
**     * The __FILE__ macro at the time of the sqliteMalloc() call.
**     * The __LINE__ macro ...
**     * The value of the sqlite3_malloc_id variable ...
**     * The output of backtrace() (if available) ...
**
** Todo: We could have a version of this function that outputs to stdout, 
** to debug memory leaks when Tcl is not available.
*/
#if defined(TCLSH) && defined(SQLITE_DEBUG) && SQLITE_MEMDEBUG>1
#include <tcl.h>
int sqlite3OutstandingMallocs(Tcl_Interp *interp){
  void *p;
  Tcl_Obj *pRes = Tcl_NewObj();
  Tcl_IncrRefCount(pRes);


  for(p=sqlite3_pFirst; p; p=((void **)p)[1]){
    Tcl_Obj *pEntry = Tcl_NewObj();
    Tcl_Obj *pStack = Tcl_NewObj();
    char *z;
    u32 iLine;
    int nBytes = sqlite3OsAllocationSize(p) - TESTALLOC_OVERHEAD;
    char *zAlloc = (char *)p;
    int i;

    Tcl_ListObjAppendElement(0, pEntry, Tcl_NewIntObj(nBytes));

    z = &zAlloc[TESTALLOC_OFFSET_FILENAME(p)];
    Tcl_ListObjAppendElement(0, pEntry, Tcl_NewStringObj(z, -1));

    z = &zAlloc[TESTALLOC_OFFSET_LINENUMBER(p)];
    memcpy(&iLine, z, sizeof(u32));
    Tcl_ListObjAppendElement(0, pEntry, Tcl_NewIntObj(iLine));

    z = &zAlloc[TESTALLOC_OFFSET_USER(p)];
    Tcl_ListObjAppendElement(0, pEntry, Tcl_NewStringObj(z, -1));

    z = &zAlloc[TESTALLOC_OFFSET_STACK(p)];
    for(i=0; i<TESTALLOC_STACKFRAMES; i++){
      char zHex[128];
      sprintf(zHex, "%p", ((void **)z)[i]);
      Tcl_ListObjAppendElement(0, pStack, Tcl_NewStringObj(zHex, -1));
    }

    Tcl_ListObjAppendElement(0, pEntry, pStack);
    Tcl_ListObjAppendElement(0, pRes, pEntry);
  }

  Tcl_ResetResult(interp);
  Tcl_SetObjResult(interp, pRes);
  Tcl_DecrRefCount(pRes);
  return TCL_OK;
}
#endif

/*
** This is the test layer's wrapper around sqlite3OsMalloc().
*/
static void * OSMALLOC(int n){
  sqlite3OsEnterMutex();
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  sqlite3_nMaxAlloc = 
      MAX(sqlite3_nMaxAlloc, sqlite3ThreadDataReadOnly()->nAlloc);
#endif
  assert( !sqlite3_mallocDisallowed );
  if( !sqlite3TestMallocFail() ){
    u32 *p;
    p = (u32 *)sqlite3OsMalloc(n + TESTALLOC_OVERHEAD);
    assert(p);
    sqlite3_nMalloc++;
    applyGuards(p);
    linkAlloc(p);
    sqlite3OsLeaveMutex();
    return (void *)(&p[TESTALLOC_NGUARD + 2*sizeof(void *)/sizeof(u32)]);
  }
  sqlite3OsLeaveMutex();
  return 0;
}

static int OSSIZEOF(void *p){
  if( p ){
    u32 *pOs = (u32 *)getOsPointer(p);
    return sqlite3OsAllocationSize(pOs) - TESTALLOC_OVERHEAD;
  }
  return 0;
}

/*
** This is the test layer's wrapper around sqlite3OsFree(). The argument is a
** pointer to the space allocated for the application to use.
*/
static void OSFREE(void *pFree){
  u32 *p;         /* Pointer to the OS-layer allocation */
  sqlite3OsEnterMutex();
  p = (u32 *)getOsPointer(pFree);
  checkGuards(p);
  unlinkAlloc(p);
  memset(pFree, 0x55, OSSIZEOF(pFree));
  sqlite3OsFree(p);
  sqlite3_nFree++;
  sqlite3OsLeaveMutex();
}

/*
** This is the test layer's wrapper around sqlite3OsRealloc().
*/
static void * OSREALLOC(void *pRealloc, int n){
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  sqlite3_nMaxAlloc = 
      MAX(sqlite3_nMaxAlloc, sqlite3ThreadDataReadOnly()->nAlloc);
#endif
  assert( !sqlite3_mallocDisallowed );
  if( !sqlite3TestMallocFail() ){
    u32 *p = (u32 *)getOsPointer(pRealloc);
    checkGuards(p);
    p = sqlite3OsRealloc(p, n + TESTALLOC_OVERHEAD);
    applyGuards(p);
    relinkAlloc(p);
    return (void *)(&p[TESTALLOC_NGUARD + 2*sizeof(void *)/sizeof(u32)]);
  }
  return 0;
}

static void OSMALLOC_FAILED(){
  sqlite3_isFail = 0;
}

#else
/* Define macros to call the sqlite3OsXXX interface directly if 
** the SQLITE_MEMDEBUG macro is not defined.
*/
#define OSMALLOC(x)        sqlite3OsMalloc(x)
#define OSREALLOC(x,y)     sqlite3OsRealloc(x,y)
#define OSFREE(x)          sqlite3OsFree(x)
#define OSSIZEOF(x)        sqlite3OsAllocationSize(x)
#define OSMALLOC_FAILED()

#endif  /* SQLITE_MEMDEBUG */
/*
** End code for memory allocation system test layer.
**--------------------------------------------------------------------------*/

/*
** This routine is called when we are about to allocate n additional bytes
** of memory.  If the new allocation will put is over the soft allocation
** limit, then invoke sqlite3_release_memory() to try to release some
** memory before continuing with the allocation.
**
** This routine also makes sure that the thread-specific-data (TSD) has
** be allocated.  If it has not and can not be allocated, then return
** false.  The updateMemoryUsedCount() routine below will deallocate
** the TSD if it ought to be.
**
** If SQLITE_ENABLE_MEMORY_MANAGEMENT is not defined, this routine is
** a no-op
*/ 
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
static int enforceSoftLimit(int n){
  ThreadData *pTsd = sqlite3ThreadData();
  if( pTsd==0 ){
    return 0;
  }
  assert( pTsd->nAlloc>=0 );
  if( n>0 && pTsd->nSoftHeapLimit>0 ){
    while( pTsd->nAlloc+n>pTsd->nSoftHeapLimit && sqlite3_release_memory(n) ){}
  }
  return 1;
}
#else
# define enforceSoftLimit(X)  1
#endif

/*
** Update the count of total outstanding memory that is held in
** thread-specific-data (TSD).  If after this update the TSD is
** no longer being used, then deallocate it.
**
** If SQLITE_ENABLE_MEMORY_MANAGEMENT is not defined, this routine is
** a no-op
*/
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
static void updateMemoryUsedCount(int n){
  ThreadData *pTsd = sqlite3ThreadData();
  if( pTsd ){
    pTsd->nAlloc += n;
    assert( pTsd->nAlloc>=0 );
    if( pTsd->nAlloc==0 && pTsd->nSoftHeapLimit==0 ){
      sqlite3ReleaseThreadData();
    }
  }
}
#else
#define updateMemoryUsedCount(x)  /* no-op */
#endif

/*
** Allocate and return N bytes of uninitialised memory by calling
** sqlite3OsMalloc(). If the Malloc() call fails, attempt to free memory 
** by calling sqlite3_release_memory().
*/
void *sqlite3MallocRaw(int n, int doMemManage){
  void *p = 0;
  if( n>0 && !sqlite3MallocFailed() && (!doMemManage || enforceSoftLimit(n)) ){
    while( (p = OSMALLOC(n))==0 && sqlite3_release_memory(n) ){}
    if( !p ){
      sqlite3FailedMalloc();
      OSMALLOC_FAILED();
    }else if( doMemManage ){
      updateMemoryUsedCount(OSSIZEOF(p));
    }
  }
  return p;
}

/*
** Resize the allocation at p to n bytes by calling sqlite3OsRealloc(). The
** pointer to the new allocation is returned.  If the Realloc() call fails,
** attempt to free memory by calling sqlite3_release_memory().
*/
void *sqlite3Realloc(void *p, int n){
  if( sqlite3MallocFailed() ){
    return 0;
  }

  if( !p ){
    return sqlite3Malloc(n, 1);
  }else{
    void *np = 0;
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
    int origSize = OSSIZEOF(p);
#endif
    if( enforceSoftLimit(n - origSize) ){
      while( (np = OSREALLOC(p, n))==0 && sqlite3_release_memory(n) ){}
      if( !np ){
        sqlite3FailedMalloc();
        OSMALLOC_FAILED();
      }else{
        updateMemoryUsedCount(OSSIZEOF(np) - origSize);
      }
    }
    return np;
  }
}

/*
** Free the memory pointed to by p. p must be either a NULL pointer or a 
** value returned by a previous call to sqlite3Malloc() or sqlite3Realloc().
*/
void sqlite3FreeX(void *p){
  if( p ){
    updateMemoryUsedCount(0 - OSSIZEOF(p));
    OSFREE(p);
  }
}

/*
** A version of sqliteMalloc() that is always a function, not a macro.
** Currently, this is used only to alloc to allocate the parser engine.
*/
void *sqlite3MallocX(int n){
  return sqliteMalloc(n);
}

/*
** sqlite3Malloc
** sqlite3ReallocOrFree
**
** These two are implemented as wrappers around sqlite3MallocRaw(), 
** sqlite3Realloc() and sqlite3Free().
*/ 
void *sqlite3Malloc(int n, int doMemManage){
  void *p = sqlite3MallocRaw(n, doMemManage);
  if( p ){
    memset(p, 0, n);
  }
  return p;
}
void *sqlite3ReallocOrFree(void *p, int n){
  void *pNew;
  pNew = sqlite3Realloc(p, n);
  if( !pNew ){
    sqlite3FreeX(p);
  }
  return pNew;
}

/*
** sqlite3ThreadSafeMalloc() and sqlite3ThreadSafeFree() are used in those
** rare scenarios where sqlite may allocate memory in one thread and free
** it in another. They are exactly the same as sqlite3Malloc() and 
** sqlite3Free() except that:
**
**   * The allocated memory is not included in any calculations with 
**     respect to the soft-heap-limit, and
**
**   * sqlite3ThreadSafeMalloc() must be matched with ThreadSafeFree(),
**     not sqlite3Free(). Calling sqlite3Free() on memory obtained from
**     ThreadSafeMalloc() will cause an error somewhere down the line.
*/
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
void *sqlite3ThreadSafeMalloc(int n){
  (void)ENTER_MALLOC;
  return sqlite3Malloc(n, 0);
}
void sqlite3ThreadSafeFree(void *p){
  (void)ENTER_MALLOC;
  if( p ){
    OSFREE(p);
  }
}
#endif


/*
** Return the number of bytes allocated at location p. p must be either 
** a NULL pointer (in which case 0 is returned) or a pointer returned by 
** sqlite3Malloc(), sqlite3Realloc() or sqlite3ReallocOrFree().
**
** The number of bytes allocated does not include any overhead inserted by 
** any malloc() wrapper functions that may be called. So the value returned
** is the number of bytes that were available to SQLite using pointer p, 
** regardless of how much memory was actually allocated.
*/
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
int sqlite3AllocSize(void *p){
  return OSSIZEOF(p);
}
#endif

/*
** Make a copy of a string in memory obtained from sqliteMalloc(). These 
** functions call sqlite3MallocRaw() directly instead of sqliteMalloc(). This
** is because when memory debugging is turned on, these two functions are 
** called via macros that record the current file and line number in the
** ThreadData structure.
*/
char *sqlite3StrDup(const char *z){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqlite3MallocRaw(strlen(z)+1, 1);
  if( zNew ) strcpy(zNew, z);
  return zNew;
}
char *sqlite3StrNDup(const char *z, int n){
  char *zNew;
  if( z==0 ) return 0;
  zNew = sqlite3MallocRaw(n+1, 1);
  if( zNew ){
    memcpy(zNew, z, n);
    zNew[n] = 0;
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
  sqliteFree(*pz);
  *pz = zResult = sqliteMallocRaw( nByte );
  if( zResult==0 ){
    return;
  }
  *zResult = 0;
  va_start(ap, pz);
  while( (z = va_arg(ap, const char*))!=0 ){
    strcpy(zResult, z);
    zResult += strlen(zResult);
  }
  va_end(ap);
}

/*
** Set the most recent error code and error string for the sqlite
** handle "db". The error code is set to "err_code".
**
** If it is not NULL, string zFormat specifies the format of the
** error string in the style of the printf functions: The following
** format characters are allowed:
**
**      %s      Insert a string
**      %z      A string that should be freed after use
**      %d      Insert an integer
**      %T      Insert a token
**      %S      Insert the first element of a SrcList
**
** zFormat and any string tokens that follow it are assumed to be
** encoded in UTF-8.
**
** To clear the most recent error for sqlite handle "db", sqlite3Error
** should be called with err_code set to SQLITE_OK and zFormat set
** to NULL.
*/
void sqlite3Error(sqlite3 *db, int err_code, const char *zFormat, ...){
  if( db && (db->pErr || (db->pErr = sqlite3ValueNew())!=0) ){
    db->errCode = err_code;
    if( zFormat ){
      char *z;
      va_list ap;
      va_start(ap, zFormat);
      z = sqlite3VMPrintf(zFormat, ap);
      va_end(ap);
      sqlite3ValueSetStr(db->pErr, -1, z, SQLITE_UTF8, sqlite3FreeX);
    }else{
      sqlite3ValueSetStr(db->pErr, 0, 0, SQLITE_UTF8, SQLITE_STATIC);
    }
  }
}

/*
** Add an error message to pParse->zErrMsg and increment pParse->nErr.
** The following formatting characters are allowed:
**
**      %s      Insert a string
**      %z      A string that should be freed after use
**      %d      Insert an integer
**      %T      Insert a token
**      %S      Insert the first element of a SrcList
**
** This function should be used to report any error that occurs whilst
** compiling an SQL statement (i.e. within sqlite3_prepare()). The
** last thing the sqlite3_prepare() function does is copy the error
** stored by this function into the database handle using sqlite3Error().
** Function sqlite3Error() should be used during statement execution
** (sqlite3_step() etc.).
*/
void sqlite3ErrorMsg(Parse *pParse, const char *zFormat, ...){
  va_list ap;
  pParse->nErr++;
  sqliteFree(pParse->zErrMsg);
  va_start(ap, zFormat);
  pParse->zErrMsg = sqlite3VMPrintf(zFormat, ap);
  va_end(ap);
}

/*
** Clear the error message in pParse, if any
*/
void sqlite3ErrorClear(Parse *pParse){
  sqliteFree(pParse->zErrMsg);
  pParse->zErrMsg = 0;
  pParse->nErr = 0;
}

/*
** Convert an SQL-style quoted string into a normal string by removing
** the quote characters.  The conversion is done in-place.  If the
** input does not begin with a quote character, then this routine
** is a no-op.
**
** 2002-Feb-14: This routine is extended to remove MS-Access style
** brackets from around identifers.  For example:  "[a-b-c]" becomes
** "a-b-c".
*/
void sqlite3Dequote(char *z){
  int quote;
  int i, j;
  if( z==0 ) return;
  quote = z[0];
  switch( quote ){
    case '\'':  break;
    case '"':   break;
    case '`':   break;                /* For MySQL compatibility */
    case '[':   quote = ']';  break;  /* For MS SqlServer compatibility */
    default:    return;
  }
  for(i=1, j=0; z[i]; i++){
    if( z[i]==quote ){
      if( z[i+1]==quote ){
        z[j++] = quote;
        i++;
      }else{
        z[j++] = 0;
        break;
      }
    }else{
      z[j++] = z[i];
    }
  }
}

/* An array to map all upper-case characters into their corresponding
** lower-case character. 
*/
const unsigned char sqlite3UpperToLower[] = {
#ifdef SQLITE_ASCII
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
     18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
     36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
     54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
    104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
    122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
    108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
    126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
    162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
    180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
    198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
    216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
    234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
    252,253,254,255
#endif
#ifdef SQLITE_EBCDIC
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, /* 0x */
     16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, /* 1x */
     32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, /* 2x */
     48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, /* 3x */
     64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, /* 4x */
     80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, /* 5x */
     96, 97, 66, 67, 68, 69, 70, 71, 72, 73,106,107,108,109,110,111, /* 6x */
    112, 81, 82, 83, 84, 85, 86, 87, 88, 89,122,123,124,125,126,127, /* 7x */
    128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143, /* 8x */
    144,145,146,147,148,149,150,151,152,153,154,155,156,157,156,159, /* 9x */
    160,161,162,163,164,165,166,167,168,169,170,171,140,141,142,175, /* Ax */
    176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191, /* Bx */
    192,129,130,131,132,133,134,135,136,137,202,203,204,205,206,207, /* Cx */
    208,145,146,147,148,149,150,151,152,153,218,219,220,221,222,223, /* Dx */
    224,225,162,163,164,165,166,167,168,169,232,203,204,205,206,207, /* Ex */
    239,240,241,242,243,244,245,246,247,248,249,219,220,221,222,255, /* Fx */
#endif
};
#define UpperToLower sqlite3UpperToLower

/*
** Some systems have stricmp().  Others have strcasecmp().  Because
** there is no consistency, we will define our own.
*/
int sqlite3StrICmp(const char *zLeft, const char *zRight){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return UpperToLower[*a] - UpperToLower[*b];
}
int sqlite3StrNICmp(const char *zLeft, const char *zRight, int N){
  register unsigned char *a, *b;
  a = (unsigned char *)zLeft;
  b = (unsigned char *)zRight;
  while( N-- > 0 && *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
  return N<0 ? 0 : UpperToLower[*a] - UpperToLower[*b];
}

/*
** Return TRUE if z is a pure numeric string.  Return FALSE if the
** string contains any character which is not part of a number. If
** the string is numeric and contains the '.' character, set *realnum
** to TRUE (otherwise FALSE).
**
** An empty string is considered non-numeric.
*/
int sqlite3IsNumber(const char *z, int *realnum, u8 enc){
  int incr = (enc==SQLITE_UTF8?1:2);
  if( enc==SQLITE_UTF16BE ) z++;
  if( *z=='-' || *z=='+' ) z += incr;
  if( !isdigit(*(u8*)z) ){
    return 0;
  }
  z += incr;
  if( realnum ) *realnum = 0;
  while( isdigit(*(u8*)z) ){ z += incr; }
  if( *z=='.' ){
    z += incr;
    if( !isdigit(*(u8*)z) ) return 0;
    while( isdigit(*(u8*)z) ){ z += incr; }
    if( realnum ) *realnum = 1;
  }
  if( *z=='e' || *z=='E' ){
    z += incr;
    if( *z=='+' || *z=='-' ) z += incr;
    if( !isdigit(*(u8*)z) ) return 0;
    while( isdigit(*(u8*)z) ){ z += incr; }
    if( realnum ) *realnum = 1;
  }
  return *z==0;
}

/*
** The string z[] is an ascii representation of a real number.
** Convert this string to a double.
**
** This routine assumes that z[] really is a valid number.  If it
** is not, the result is undefined.
**
** This routine is used instead of the library atof() function because
** the library atof() might want to use "," as the decimal point instead
** of "." depending on how locale is set.  But that would cause problems
** for SQL.  So this routine always uses "." regardless of locale.
*/
int sqlite3AtoF(const char *z, double *pResult){
#ifndef SQLITE_OMIT_FLOATING_POINT
  int sign = 1;
  const char *zBegin = z;
  LONGDOUBLE_TYPE v1 = 0.0;
  while( isspace(*z) ) z++;
  if( *z=='-' ){
    sign = -1;
    z++;
  }else if( *z=='+' ){
    z++;
  }
  while( isdigit(*(u8*)z) ){
    v1 = v1*10.0 + (*z - '0');
    z++;
  }
  if( *z=='.' ){
    LONGDOUBLE_TYPE divisor = 1.0;
    z++;
    while( isdigit(*(u8*)z) ){
      v1 = v1*10.0 + (*z - '0');
      divisor *= 10.0;
      z++;
    }
    v1 /= divisor;
  }
  if( *z=='e' || *z=='E' ){
    int esign = 1;
    int eval = 0;
    LONGDOUBLE_TYPE scale = 1.0;
    z++;
    if( *z=='-' ){
      esign = -1;
      z++;
    }else if( *z=='+' ){
      z++;
    }
    while( isdigit(*(u8*)z) ){
      eval = eval*10 + *z - '0';
      z++;
    }
    while( eval>=64 ){ scale *= 1.0e+64; eval -= 64; }
    while( eval>=16 ){ scale *= 1.0e+16; eval -= 16; }
    while( eval>=4 ){ scale *= 1.0e+4; eval -= 4; }
    while( eval>=1 ){ scale *= 1.0e+1; eval -= 1; }
    if( esign<0 ){
      v1 /= scale;
    }else{
      v1 *= scale;
    }
  }
  *pResult = sign<0 ? -v1 : v1;
  return z - zBegin;
#else
  return sqlite3atoi64(z, pResult);
#endif /* SQLITE_OMIT_FLOATING_POINT */
}

/*
** Return TRUE if zNum is a 64-bit signed integer and write
** the value of the integer into *pNum.  If zNum is not an integer
** or is an integer that is too large to be expressed with 64 bits,
** then return false.  If n>0 and the integer is string is not
** exactly n bytes long, return false.
**
** When this routine was originally written it dealt with only
** 32-bit numbers.  At that time, it was much faster than the
** atoi() library routine in RedHat 7.2.
*/
int sqlite3atoi64(const char *zNum, i64 *pNum){
  i64 v = 0;
  int neg;
  int i, c;
  while( isspace(*zNum) ) zNum++;
  if( *zNum=='-' ){
    neg = 1;
    zNum++;
  }else if( *zNum=='+' ){
    neg = 0;
    zNum++;
  }else{
    neg = 0;
  }
  for(i=0; (c=zNum[i])>='0' && c<='9'; i++){
    v = v*10 + c - '0';
  }
  *pNum = neg ? -v : v;
  return c==0 && i>0 && 
      (i<19 || (i==19 && memcmp(zNum,"9223372036854775807",19)<=0));
}

/*
** The string zNum represents an integer.  There might be some other
** information following the integer too, but that part is ignored.
** If the integer that the prefix of zNum represents will fit in a
** 32-bit signed integer, return TRUE.  Otherwise return FALSE.
**
** This routine returns FALSE for the string -2147483648 even that
** that number will in fact fit in a 32-bit integer.  But positive
** 2147483648 will not fit in 32 bits.  So it seems safer to return
** false.
*/
static int sqlite3FitsIn32Bits(const char *zNum){
  int i, c;
  if( *zNum=='-' || *zNum=='+' ) zNum++;
  for(i=0; (c=zNum[i])>='0' && c<='9'; i++){}
  return i<10 || (i==10 && memcmp(zNum,"2147483647",10)<=0);
}

/*
** If zNum represents an integer that will fit in 32-bits, then set
** *pValue to that integer and return true.  Otherwise return false.
*/
int sqlite3GetInt32(const char *zNum, int *pValue){
  if( sqlite3FitsIn32Bits(zNum) ){
    *pValue = atoi(zNum);
    return 1;
  }
  return 0;
}

/*
** The string zNum represents an integer.  There might be some other
** information following the integer too, but that part is ignored.
** If the integer that the prefix of zNum represents will fit in a
** 64-bit signed integer, return TRUE.  Otherwise return FALSE.
**
** This routine returns FALSE for the string -9223372036854775808 even that
** that number will, in theory fit in a 64-bit integer.  Positive
** 9223373036854775808 will not fit in 64 bits.  So it seems safer to return
** false.
*/
int sqlite3FitsIn64Bits(const char *zNum){
  int i, c;
  if( *zNum=='-' || *zNum=='+' ) zNum++;
  for(i=0; (c=zNum[i])>='0' && c<='9'; i++){}
  return i<19 || (i==19 && memcmp(zNum,"9223372036854775807",19)<=0);
}


/*
** Change the sqlite.magic from SQLITE_MAGIC_OPEN to SQLITE_MAGIC_BUSY.
** Return an error (non-zero) if the magic was not SQLITE_MAGIC_OPEN
** when this routine is called.
**
** This routine is called when entering an SQLite API.  The SQLITE_MAGIC_OPEN
** value indicates that the database connection passed into the API is
** open and is not being used by another thread.  By changing the value
** to SQLITE_MAGIC_BUSY we indicate that the connection is in use.
** sqlite3SafetyOff() below will change the value back to SQLITE_MAGIC_OPEN
** when the API exits. 
**
** This routine is a attempt to detect if two threads use the
** same sqlite* pointer at the same time.  There is a race 
** condition so it is possible that the error is not detected.
** But usually the problem will be seen.  The result will be an
** error which can be used to debug the application that is
** using SQLite incorrectly.
**
** Ticket #202:  If db->magic is not a valid open value, take care not
** to modify the db structure at all.  It could be that db is a stale
** pointer.  In other words, it could be that there has been a prior
** call to sqlite3_close(db) and db has been deallocated.  And we do
** not want to write into deallocated memory.
*/
int sqlite3SafetyOn(sqlite3 *db){
  if( db->magic==SQLITE_MAGIC_OPEN ){
    db->magic = SQLITE_MAGIC_BUSY;
    return 0;
  }else if( db->magic==SQLITE_MAGIC_BUSY ){
    db->magic = SQLITE_MAGIC_ERROR;
    db->u1.isInterrupted = 1;
  }
  return 1;
}

/*
** Change the magic from SQLITE_MAGIC_BUSY to SQLITE_MAGIC_OPEN.
** Return an error (non-zero) if the magic was not SQLITE_MAGIC_BUSY
** when this routine is called.
*/
int sqlite3SafetyOff(sqlite3 *db){
  if( db->magic==SQLITE_MAGIC_BUSY ){
    db->magic = SQLITE_MAGIC_OPEN;
    return 0;
  }else {
    db->magic = SQLITE_MAGIC_ERROR;
    db->u1.isInterrupted = 1;
    return 1;
  }
}

/*
** Check to make sure we have a valid db pointer.  This test is not
** foolproof but it does provide some measure of protection against
** misuse of the interface such as passing in db pointers that are
** NULL or which have been previously closed.  If this routine returns
** TRUE it means that the db pointer is invalid and should not be
** dereferenced for any reason.  The calling function should invoke
** SQLITE_MISUSE immediately.
*/
int sqlite3SafetyCheck(sqlite3 *db){
  int magic;
  if( db==0 ) return 1;
  magic = db->magic;
  if( magic!=SQLITE_MAGIC_CLOSED &&
         magic!=SQLITE_MAGIC_OPEN &&
         magic!=SQLITE_MAGIC_BUSY ) return 1;
  return 0;
}

/*
** The variable-length integer encoding is as follows:
**
** KEY:
**         A = 0xxxxxxx    7 bits of data and one flag bit
**         B = 1xxxxxxx    7 bits of data and one flag bit
**         C = xxxxxxxx    8 bits of data
**
**  7 bits - A
** 14 bits - BA
** 21 bits - BBA
** 28 bits - BBBA
** 35 bits - BBBBA
** 42 bits - BBBBBA
** 49 bits - BBBBBBA
** 56 bits - BBBBBBBA
** 64 bits - BBBBBBBBC
*/

/*
** Write a 64-bit variable-length integer to memory starting at p[0].
** The length of data write will be between 1 and 9 bytes.  The number
** of bytes written is returned.
**
** A variable-length integer consists of the lower 7 bits of each byte
** for all bytes that have the 8th bit set and one byte with the 8th
** bit clear.  Except, if we get to the 9th byte, it stores the full
** 8 bits and is the last byte.
*/
int sqlite3PutVarint(unsigned char *p, u64 v){
  int i, j, n;
  u8 buf[10];
  if( v & (((u64)0xff000000)<<32) ){
    p[8] = v;
    v >>= 8;
    for(i=7; i>=0; i--){
      p[i] = (v & 0x7f) | 0x80;
      v >>= 7;
    }
    return 9;
  }    
  n = 0;
  do{
    buf[n++] = (v & 0x7f) | 0x80;
    v >>= 7;
  }while( v!=0 );
  buf[0] &= 0x7f;
  assert( n<=9 );
  for(i=0, j=n-1; j>=0; j--, i++){
    p[i] = buf[j];
  }
  return n;
}

/*
** Read a 64-bit variable-length integer from memory starting at p[0].
** Return the number of bytes read.  The value is stored in *v.
*/
int sqlite3GetVarint(const unsigned char *p, u64 *v){
  u32 x;
  u64 x64;
  int n;
  unsigned char c;
  if( ((c = p[0]) & 0x80)==0 ){
    *v = c;
    return 1;
  }
  x = c & 0x7f;
  if( ((c = p[1]) & 0x80)==0 ){
    *v = (x<<7) | c;
    return 2;
  }
  x = (x<<7) | (c&0x7f);
  if( ((c = p[2]) & 0x80)==0 ){
    *v = (x<<7) | c;
    return 3;
  }
  x = (x<<7) | (c&0x7f);
  if( ((c = p[3]) & 0x80)==0 ){
    *v = (x<<7) | c;
    return 4;
  }
  x64 = (x<<7) | (c&0x7f);
  n = 4;
  do{
    c = p[n++];
    if( n==9 ){
      x64 = (x64<<8) | c;
      break;
    }
    x64 = (x64<<7) | (c&0x7f);
  }while( (c & 0x80)!=0 );
  *v = x64;
  return n;
}

/*
** Read a 32-bit variable-length integer from memory starting at p[0].
** Return the number of bytes read.  The value is stored in *v.
*/
int sqlite3GetVarint32(const unsigned char *p, u32 *v){
  u32 x;
  int n;
  unsigned char c;
  if( ((signed char*)p)[0]>=0 ){
    *v = p[0];
    return 1;
  }
  x = p[0] & 0x7f;
  if( ((signed char*)p)[1]>=0 ){
    *v = (x<<7) | p[1];
    return 2;
  }
  x = (x<<7) | (p[1] & 0x7f);
  n = 2;
  do{
    x = (x<<7) | ((c = p[n++])&0x7f);
  }while( (c & 0x80)!=0 && n<9 );
  *v = x;
  return n;
}

/*
** Return the number of bytes that will be needed to store the given
** 64-bit integer.
*/
int sqlite3VarintLen(u64 v){
  int i = 0;
  do{
    i++;
    v >>= 7;
  }while( v!=0 && i<9 );
  return i;
}

#if !defined(SQLITE_OMIT_BLOB_LITERAL) || defined(SQLITE_HAS_CODEC) \
    || defined(SQLITE_TEST)
/*
** Translate a single byte of Hex into an integer.
*/
static int hexToInt(int h){
  if( h>='0' && h<='9' ){
    return h - '0';
  }else if( h>='a' && h<='f' ){
    return h - 'a' + 10;
  }else{
    assert( h>='A' && h<='F' );
    return h - 'A' + 10;
  }
}
#endif /* !SQLITE_OMIT_BLOB_LITERAL || SQLITE_HAS_CODEC || SQLITE_TEST */

#if !defined(SQLITE_OMIT_BLOB_LITERAL) || defined(SQLITE_HAS_CODEC)
/*
** Convert a BLOB literal of the form "x'hhhhhh'" into its binary
** value.  Return a pointer to its binary value.  Space to hold the
** binary value has been obtained from malloc and must be freed by
** the calling routine.
*/
void *sqlite3HexToBlob(const char *z){
  char *zBlob;
  int i;
  int n = strlen(z);
  if( n%2 ) return 0;

  zBlob = (char *)sqliteMalloc(n/2);
  if( zBlob ){
    for(i=0; i<n; i+=2){
      zBlob[i/2] = (hexToInt(z[i])<<4) | hexToInt(z[i+1]);
    }
  }
  return zBlob;
}
#endif /* !SQLITE_OMIT_BLOB_LITERAL || SQLITE_HAS_CODEC */

#if defined(SQLITE_TEST)
/*
** Convert text generated by the "%p" conversion format back into
** a pointer.
*/
void *sqlite3TextToPtr(const char *z){
  void *p;
  u64 v;
  u32 v2;
  if( z[0]=='0' && z[1]=='x' ){
    z += 2;
  }
  v = 0;
  while( *z ){
    v = (v<<4) + hexToInt(*z);
    z++;
  }
  if( sizeof(p)==sizeof(v) ){
    memcpy(&p, &v, sizeof(p));
  }else{
    assert( sizeof(p)==sizeof(v2) );
    v2 = (u32)v;
    memcpy(&p, &v2, sizeof(p));
  }
  return p;
}
#endif

/*
** Return a pointer to the ThreadData associated with the calling thread.
*/
ThreadData *sqlite3ThreadData(){
  ThreadData *p = (ThreadData*)sqlite3OsThreadSpecificData(1);
  if( !p ){
    sqlite3FailedMalloc();
  }
  return p;
}

/*
** Return a pointer to the ThreadData associated with the calling thread.
** If no ThreadData has been allocated to this thread yet, return a pointer
** to a substitute ThreadData structure that is all zeros. 
*/
const ThreadData *sqlite3ThreadDataReadOnly(){
  static const ThreadData zeroData = {0};  /* Initializer to silence warnings
                                           ** from broken compilers */
  const ThreadData *pTd = sqlite3OsThreadSpecificData(0);
  return pTd ? pTd : &zeroData;
}

/*
** Check to see if the ThreadData for this thread is all zero.  If it
** is, then deallocate it. 
*/
void sqlite3ReleaseThreadData(){
  sqlite3OsThreadSpecificData(-1);
}

/*
** This function must be called before exiting any API function (i.e. 
** returning control to the user) that has called sqlite3Malloc or
** sqlite3Realloc.
**
** The returned value is normally a copy of the second argument to this
** function. However, if a malloc() failure has occured since the previous
** invocation SQLITE_NOMEM is returned instead. 
**
** If the first argument, db, is not NULL and a malloc() error has occured,
** then the connection error-code (the value returned by sqlite3_errcode())
** is set to SQLITE_NOMEM.
*/
static int mallocHasFailed = 0;
int sqlite3ApiExit(sqlite3* db, int rc){
  if( sqlite3MallocFailed() ){
    mallocHasFailed = 0;
    sqlite3OsLeaveMutex();
    sqlite3Error(db, SQLITE_NOMEM, 0);
    rc = SQLITE_NOMEM;
  }
  return rc & (db ? db->errMask : 0xff);
}

/* 
** Return true is a malloc has failed in this thread since the last call
** to sqlite3ApiExit(), or false otherwise.
*/
int sqlite3MallocFailed(){
  return (mallocHasFailed && sqlite3OsInMutex(1));
}

/* 
** Set the "malloc has failed" condition to true for this thread.
*/
void sqlite3FailedMalloc(){
  if( !sqlite3MallocFailed() ){
    sqlite3OsEnterMutex();
    assert( mallocHasFailed==0 );
    mallocHasFailed = 1;
  }
}

#ifdef SQLITE_MEMDEBUG
/*
** This function sets a flag in the thread-specific-data structure that will
** cause an assert to fail if sqliteMalloc() or sqliteRealloc() is called.
*/
void sqlite3MallocDisallow(){
  assert( sqlite3_mallocDisallowed>=0 );
  sqlite3_mallocDisallowed++;
}

/*
** This function clears the flag set in the thread-specific-data structure set
** by sqlite3MallocDisallow().
*/
void sqlite3MallocAllow(){
  assert( sqlite3_mallocDisallowed>0 );
  sqlite3_mallocDisallowed--;
}
#endif
