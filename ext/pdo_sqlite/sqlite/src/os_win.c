/*
** 2004 May 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This file contains code that is specific to windows.
*/
#include "sqliteInt.h"
#if OS_WIN               /* This file is used for windows only */


/*
** A Note About Memory Allocation:
**
** This driver uses malloc()/free() directly rather than going through
** the SQLite-wrappers sqlite3_malloc()/sqlite3_free().  Those wrappers
** are designed for use on embedded systems where memory is scarce and
** malloc failures happen frequently.  Win32 does not typically run on
** embedded systems, and when it does the developers normally have bigger
** problems to worry about than running out of memory.  So there is not
** a compelling need to use the wrappers.
**
** But there is a good reason to not use the wrappers.  If we use the
** wrappers then we will get simulated malloc() failures within this
** driver.  And that causes all kinds of problems for our tests.  We
** could enhance SQLite to deal with simulated malloc failures within
** the OS driver, but the code to deal with those failure would not
** be exercised on Linux (which does not need to malloc() in the driver)
** and so we would have difficulty writing coverage tests for that
** code.  Better to leave the code out, we think.
**
** The point of this discussion is as follows:  When creating a new
** OS layer for an embedded system, if you use this file as an example,
** avoid the use of malloc()/free().  Those routines work ok on windows
** desktops but not so well in embedded systems.
*/

#include <winbase.h>

#ifdef __CYGWIN__
# include <sys/cygwin.h>
#endif

/*
** Macros used to determine whether or not to use threads.
*/
#if defined(THREADSAFE) && THREADSAFE
# define SQLITE_W32_THREADS 1
#endif

/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

/*
** Determine if we are dealing with WindowsCE - which has a much
** reduced API.
*/
#if defined(_WIN32_WCE)
# define OS_WINCE 1
# define AreFileApisANSI() 1
#else
# define OS_WINCE 0
#endif

/*
** WinCE lacks native support for file locking so we have to fake it
** with some code of our own.
*/
#if OS_WINCE
typedef struct winceLock {
  int nReaders;       /* Number of reader locks obtained */
  BOOL bPending;      /* Indicates a pending lock has been obtained */
  BOOL bReserved;     /* Indicates a reserved lock has been obtained */
  BOOL bExclusive;    /* Indicates an exclusive lock has been obtained */
} winceLock;
#endif

/*
** The winFile structure is a subclass of sqlite3_file* specific to the win32
** portability layer.
*/
typedef struct winFile winFile;
struct winFile {
  const sqlite3_io_methods *pMethod;/* Must be first */
  HANDLE h;               /* Handle for accessing the file */
  unsigned char locktype; /* Type of lock currently held on this file */
  short sharedLockByte;   /* Randomly chosen byte used as a shared lock */
#if OS_WINCE
  WCHAR *zDeleteOnClose;  /* Name of file to delete when closing */
  HANDLE hMutex;          /* Mutex used to control access to shared lock */  
  HANDLE hShared;         /* Shared memory segment used for locking */
  winceLock local;        /* Locks obtained by this instance of winFile */
  winceLock *shared;      /* Global shared lock memory for the file  */
#endif
};


/*
** The following variable is (normally) set once and never changes
** thereafter.  It records whether the operating system is Win95
** or WinNT.
**
** 0:   Operating system unknown.
** 1:   Operating system is Win95.
** 2:   Operating system is WinNT.
**
** In order to facilitate testing on a WinNT system, the test fixture
** can manually set this value to 1 to emulate Win98 behavior.
*/
#ifdef SQLITE_TEST
int sqlite3_os_type = 0;
#else
static int sqlite3_os_type = 0;
#endif

/*
** Return true (non-zero) if we are running under WinNT, Win2K, WinXP,
** or WinCE.  Return false (zero) for Win95, Win98, or WinME.
**
** Here is an interesting observation:  Win95, Win98, and WinME lack
** the LockFileEx() API.  But we can still statically link against that
** API as long as we don't call it win running Win95/98/ME.  A call to
** this routine is used to determine if the host is Win95/98/ME or
** WinNT/2K/XP so that we will know whether or not we can safely call
** the LockFileEx() API.
*/
#if OS_WINCE
# define isNT()  (1)
#else
  static int isNT(void){
    if( sqlite3_os_type==0 ){
      OSVERSIONINFO sInfo;
      sInfo.dwOSVersionInfoSize = sizeof(sInfo);
      GetVersionEx(&sInfo);
      sqlite3_os_type = sInfo.dwPlatformId==VER_PLATFORM_WIN32_NT ? 2 : 1;
    }
    return sqlite3_os_type==2;
  }
#endif /* OS_WINCE */

/*
** Convert a UTF-8 string to microsoft unicode (UTF-16?). 
**
** Space to hold the returned string is obtained from malloc.
*/
static WCHAR *utf8ToUnicode(const char *zFilename){
  int nChar;
  WCHAR *zWideFilename;

  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, NULL, 0);
  zWideFilename = malloc( nChar*sizeof(zWideFilename[0]) );
  if( zWideFilename==0 ){
    return 0;
  }
  nChar = MultiByteToWideChar(CP_UTF8, 0, zFilename, -1, zWideFilename, nChar);
  if( nChar==0 ){
    free(zWideFilename);
    zWideFilename = 0;
  }
  return zWideFilename;
}

/*
** Convert microsoft unicode to UTF-8.  Space to hold the returned string is
** obtained from malloc().
*/
static char *unicodeToUtf8(const WCHAR *zWideFilename){
  int nByte;
  char *zFilename;

  nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, 0, 0, 0, 0);
  zFilename = malloc( nByte );
  if( zFilename==0 ){
    return 0;
  }
  nByte = WideCharToMultiByte(CP_UTF8, 0, zWideFilename, -1, zFilename, nByte,
                              0, 0);
  if( nByte == 0 ){
    free(zFilename);
    zFilename = 0;
  }
  return zFilename;
}

/*
** Convert an ansi string to microsoft unicode, based on the
** current codepage settings for file apis.
** 
** Space to hold the returned string is obtained
** from malloc.
*/
static WCHAR *mbcsToUnicode(const char *zFilename){
  int nByte;
  WCHAR *zMbcsFilename;
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, NULL,0)*sizeof(WCHAR);
  zMbcsFilename = malloc( nByte*sizeof(zMbcsFilename[0]) );
  if( zMbcsFilename==0 ){
    return 0;
  }
  nByte = MultiByteToWideChar(codepage, 0, zFilename, -1, zMbcsFilename, nByte);
  if( nByte==0 ){
    free(zMbcsFilename);
    zMbcsFilename = 0;
  }
  return zMbcsFilename;
}

/*
** Convert microsoft unicode to multibyte character string, based on the
** user's Ansi codepage.
**
** Space to hold the returned string is obtained from
** malloc().
*/
static char *unicodeToMbcs(const WCHAR *zWideFilename){
  int nByte;
  char *zFilename;
  int codepage = AreFileApisANSI() ? CP_ACP : CP_OEMCP;

  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, 0, 0, 0, 0);
  zFilename = malloc( nByte );
  if( zFilename==0 ){
    return 0;
  }
  nByte = WideCharToMultiByte(codepage, 0, zWideFilename, -1, zFilename, nByte,
                              0, 0);
  if( nByte == 0 ){
    free(zFilename);
    zFilename = 0;
  }
  return zFilename;
}

/*
** Convert multibyte character string to UTF-8.  Space to hold the
** returned string is obtained from malloc().
*/
static char *mbcsToUtf8(const char *zFilename){
  char *zFilenameUtf8;
  WCHAR *zTmpWide;

  zTmpWide = mbcsToUnicode(zFilename);
  if( zTmpWide==0 ){
    return 0;
  }
  zFilenameUtf8 = unicodeToUtf8(zTmpWide);
  free(zTmpWide);
  return zFilenameUtf8;
}

/*
** Convert UTF-8 to multibyte character string.  Space to hold the 
** returned string is obtained from malloc().
*/
static char *utf8ToMbcs(const char *zFilename){
  char *zFilenameMbcs;
  WCHAR *zTmpWide;

  zTmpWide = utf8ToUnicode(zFilename);
  if( zTmpWide==0 ){
    return 0;
  }
  zFilenameMbcs = unicodeToMbcs(zTmpWide);
  free(zTmpWide);
  return zFilenameMbcs;
}

#if OS_WINCE
/*************************************************************************
** This section contains code for WinCE only.
*/
/*
** WindowsCE does not have a localtime() function.  So create a
** substitute.
*/
#include <time.h>
struct tm *__cdecl localtime(const time_t *t)
{
  static struct tm y;
  FILETIME uTm, lTm;
  SYSTEMTIME pTm;
  sqlite3_int64 t64;
  t64 = *t;
  t64 = (t64 + 11644473600)*10000000;
  uTm.dwLowDateTime = t64 & 0xFFFFFFFF;
  uTm.dwHighDateTime= t64 >> 32;
  FileTimeToLocalFileTime(&uTm,&lTm);
  FileTimeToSystemTime(&lTm,&pTm);
  y.tm_year = pTm.wYear - 1900;
  y.tm_mon = pTm.wMonth - 1;
  y.tm_wday = pTm.wDayOfWeek;
  y.tm_mday = pTm.wDay;
  y.tm_hour = pTm.wHour;
  y.tm_min = pTm.wMinute;
  y.tm_sec = pTm.wSecond;
  return &y;
}

/* This will never be called, but defined to make the code compile */
#define GetTempPathA(a,b)

#define LockFile(a,b,c,d,e)       winceLockFile(&a, b, c, d, e)
#define UnlockFile(a,b,c,d,e)     winceUnlockFile(&a, b, c, d, e)
#define LockFileEx(a,b,c,d,e,f)   winceLockFileEx(&a, b, c, d, e, f)

#define HANDLE_TO_WINFILE(a) (winFile*)&((char*)a)[-offsetof(winFile,h)]

/*
** Acquire a lock on the handle h
*/
static void winceMutexAcquire(HANDLE h){
   DWORD dwErr;
   do {
     dwErr = WaitForSingleObject(h, INFINITE);
   } while (dwErr != WAIT_OBJECT_0 && dwErr != WAIT_ABANDONED);
}
/*
** Release a lock acquired by winceMutexAcquire()
*/
#define winceMutexRelease(h) ReleaseMutex(h)

/*
** Create the mutex and shared memory used for locking in the file
** descriptor pFile
*/
static BOOL winceCreateLock(const char *zFilename, winFile *pFile){
  WCHAR *zTok;
  WCHAR *zName = utf8ToUnicode(zFilename);
  BOOL bInit = TRUE;

  /* Initialize the local lockdata */
  ZeroMemory(&pFile->local, sizeof(pFile->local));

  /* Replace the backslashes from the filename and lowercase it
  ** to derive a mutex name. */
  zTok = CharLowerW(zName);
  for (;*zTok;zTok++){
    if (*zTok == '\\') *zTok = '_';
  }

  /* Create/open the named mutex */
  pFile->hMutex = CreateMutexW(NULL, FALSE, zName);
  if (!pFile->hMutex){
    free(zName);
    return FALSE;
  }

  /* Acquire the mutex before continuing */
  winceMutexAcquire(pFile->hMutex);
  
  /* Since the names of named mutexes, semaphores, file mappings etc are 
  ** case-sensitive, take advantage of that by uppercasing the mutex name
  ** and using that as the shared filemapping name.
  */
  CharUpperW(zName);
  pFile->hShared = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL,
                                       PAGE_READWRITE, 0, sizeof(winceLock),
                                       zName);  

  /* Set a flag that indicates we're the first to create the memory so it 
  ** must be zero-initialized */
  if (GetLastError() == ERROR_ALREADY_EXISTS){
    bInit = FALSE;
  }

  free(zName);

  /* If we succeeded in making the shared memory handle, map it. */
  if (pFile->hShared){
    pFile->shared = (winceLock*)MapViewOfFile(pFile->hShared, 
             FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, sizeof(winceLock));
    /* If mapping failed, close the shared memory handle and erase it */
    if (!pFile->shared){
      CloseHandle(pFile->hShared);
      pFile->hShared = NULL;
    }
  }

  /* If shared memory could not be created, then close the mutex and fail */
  if (pFile->hShared == NULL){
    winceMutexRelease(pFile->hMutex);
    CloseHandle(pFile->hMutex);
    pFile->hMutex = NULL;
    return FALSE;
  }
  
  /* Initialize the shared memory if we're supposed to */
  if (bInit) {
    ZeroMemory(pFile->shared, sizeof(winceLock));
  }

  winceMutexRelease(pFile->hMutex);
  return TRUE;
}

/*
** Destroy the part of winFile that deals with wince locks
*/
static void winceDestroyLock(winFile *pFile){
  if (pFile->hMutex){
    /* Acquire the mutex */
    winceMutexAcquire(pFile->hMutex);

    /* The following blocks should probably assert in debug mode, but they
       are to cleanup in case any locks remained open */
    if (pFile->local.nReaders){
      pFile->shared->nReaders --;
    }
    if (pFile->local.bReserved){
      pFile->shared->bReserved = FALSE;
    }
    if (pFile->local.bPending){
      pFile->shared->bPending = FALSE;
    }
    if (pFile->local.bExclusive){
      pFile->shared->bExclusive = FALSE;
    }

    /* De-reference and close our copy of the shared memory handle */
    UnmapViewOfFile(pFile->shared);
    CloseHandle(pFile->hShared);

    /* Done with the mutex */
    winceMutexRelease(pFile->hMutex);    
    CloseHandle(pFile->hMutex);
    pFile->hMutex = NULL;
  }
}

/* 
** An implementation of the LockFile() API of windows for wince
*/
static BOOL winceLockFile(
  HANDLE *phFile,
  DWORD dwFileOffsetLow,
  DWORD dwFileOffsetHigh,
  DWORD nNumberOfBytesToLockLow,
  DWORD nNumberOfBytesToLockHigh
){
  winFile *pFile = HANDLE_TO_WINFILE(phFile);
  BOOL bReturn = FALSE;

  if (!pFile->hMutex) return TRUE;
  winceMutexAcquire(pFile->hMutex);

  /* Wanting an exclusive lock? */
  if (dwFileOffsetLow == SHARED_FIRST
       && nNumberOfBytesToLockLow == SHARED_SIZE){
    if (pFile->shared->nReaders == 0 && pFile->shared->bExclusive == 0){
       pFile->shared->bExclusive = TRUE;
       pFile->local.bExclusive = TRUE;
       bReturn = TRUE;
    }
  }

  /* Want a read-only lock? */
  else if ((dwFileOffsetLow >= SHARED_FIRST &&
            dwFileOffsetLow < SHARED_FIRST + SHARED_SIZE) &&
            nNumberOfBytesToLockLow == 1){
    if (pFile->shared->bExclusive == 0){
      pFile->local.nReaders ++;
      if (pFile->local.nReaders == 1){
        pFile->shared->nReaders ++;
      }
      bReturn = TRUE;
    }
  }

  /* Want a pending lock? */
  else if (dwFileOffsetLow == PENDING_BYTE && nNumberOfBytesToLockLow == 1){
    /* If no pending lock has been acquired, then acquire it */
    if (pFile->shared->bPending == 0) {
      pFile->shared->bPending = TRUE;
      pFile->local.bPending = TRUE;
      bReturn = TRUE;
    }
  }
  /* Want a reserved lock? */
  else if (dwFileOffsetLow == RESERVED_BYTE && nNumberOfBytesToLockLow == 1){
    if (pFile->shared->bReserved == 0) {
      pFile->shared->bReserved = TRUE;
      pFile->local.bReserved = TRUE;
      bReturn = TRUE;
    }
  }

  winceMutexRelease(pFile->hMutex);
  return bReturn;
}

/*
** An implementation of the UnlockFile API of windows for wince
*/
static BOOL winceUnlockFile(
  HANDLE *phFile,
  DWORD dwFileOffsetLow,
  DWORD dwFileOffsetHigh,
  DWORD nNumberOfBytesToUnlockLow,
  DWORD nNumberOfBytesToUnlockHigh
){
  winFile *pFile = HANDLE_TO_WINFILE(phFile);
  BOOL bReturn = FALSE;

  if (!pFile->hMutex) return TRUE;
  winceMutexAcquire(pFile->hMutex);

  /* Releasing a reader lock or an exclusive lock */
  if (dwFileOffsetLow >= SHARED_FIRST &&
       dwFileOffsetLow < SHARED_FIRST + SHARED_SIZE){
    /* Did we have an exclusive lock? */
    if (pFile->local.bExclusive){
      pFile->local.bExclusive = FALSE;
      pFile->shared->bExclusive = FALSE;
      bReturn = TRUE;
    }

    /* Did we just have a reader lock? */
    else if (pFile->local.nReaders){
      pFile->local.nReaders --;
      if (pFile->local.nReaders == 0)
      {
        pFile->shared->nReaders --;
      }
      bReturn = TRUE;
    }
  }

  /* Releasing a pending lock */
  else if (dwFileOffsetLow == PENDING_BYTE && nNumberOfBytesToUnlockLow == 1){
    if (pFile->local.bPending){
      pFile->local.bPending = FALSE;
      pFile->shared->bPending = FALSE;
      bReturn = TRUE;
    }
  }
  /* Releasing a reserved lock */
  else if (dwFileOffsetLow == RESERVED_BYTE && nNumberOfBytesToUnlockLow == 1){
    if (pFile->local.bReserved) {
      pFile->local.bReserved = FALSE;
      pFile->shared->bReserved = FALSE;
      bReturn = TRUE;
    }
  }

  winceMutexRelease(pFile->hMutex);
  return bReturn;
}

/*
** An implementation of the LockFileEx() API of windows for wince
*/
static BOOL winceLockFileEx(
  HANDLE *phFile,
  DWORD dwFlags,
  DWORD dwReserved,
  DWORD nNumberOfBytesToLockLow,
  DWORD nNumberOfBytesToLockHigh,
  LPOVERLAPPED lpOverlapped
){
  /* If the caller wants a shared read lock, forward this call
  ** to winceLockFile */
  if (lpOverlapped->Offset == SHARED_FIRST &&
      dwFlags == 1 &&
      nNumberOfBytesToLockLow == SHARED_SIZE){
    return winceLockFile(phFile, SHARED_FIRST, 0, 1, 0);
  }
  return FALSE;
}
/*
** End of the special code for wince
*****************************************************************************/
#endif /* OS_WINCE */

/*****************************************************************************
** The next group of routines implement the I/O methods specified
** by the sqlite3_io_methods object.
******************************************************************************/

/*
** Close a file.
**
** It is reported that an attempt to close a handle might sometimes
** fail.  This is a very unreasonable result, but windows is notorious
** for being unreasonable so I do not doubt that it might happen.  If
** the close fails, we pause for 100 milliseconds and try again.  As
** many as MX_CLOSE_ATTEMPT attempts to close the handle are made before
** giving up and returning an error.
*/
#define MX_CLOSE_ATTEMPT 3
static int winClose(sqlite3_file *id){
  int rc, cnt = 0;
  winFile *pFile = (winFile*)id;
  OSTRACE2("CLOSE %d\n", pFile->h);
  do{
    rc = CloseHandle(pFile->h);
  }while( rc==0 && cnt++ < MX_CLOSE_ATTEMPT && (Sleep(100), 1) );
#if OS_WINCE
  winceDestroyLock(pFile);
  if( pFile->zDeleteOnClose ){
    DeleteFileW(pFile->zDeleteOnClose);
    free(pFile->zDeleteOnClose);
  }
#endif
  OpenCounter(-1);
  return rc ? SQLITE_OK : SQLITE_IOERR;
}

/*
** Some microsoft compilers lack this definition.
*/
#ifndef INVALID_SET_FILE_POINTER
# define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int winRead(
  sqlite3_file *id,          /* File to read from */
  void *pBuf,                /* Write content into this buffer */
  int amt,                   /* Number of bytes to read */
  sqlite3_int64 offset       /* Begin reading at this offset */
){
  LONG upperBits = (offset>>32) & 0x7fffffff;
  LONG lowerBits = offset & 0xffffffff;
  DWORD rc;
  DWORD got;
  winFile *pFile = (winFile*)id;
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_READ);
  OSTRACE3("READ %d lock=%d\n", pFile->h, pFile->locktype);
  rc = SetFilePointer(pFile->h, lowerBits, &upperBits, FILE_BEGIN);
  if( rc==INVALID_SET_FILE_POINTER && GetLastError()!=NO_ERROR ){
    return SQLITE_FULL;
  }
  if( !ReadFile(pFile->h, pBuf, amt, &got, 0) ){
    return SQLITE_IOERR_READ;
  }
  if( got==(DWORD)amt ){
    return SQLITE_OK;
  }else{
    memset(&((char*)pBuf)[got], 0, amt-got);
    return SQLITE_IOERR_SHORT_READ;
  }
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int winWrite(
  sqlite3_file *id,         /* File to write into */
  const void *pBuf,         /* The bytes to be written */
  int amt,                  /* Number of bytes to write */
  sqlite3_int64 offset      /* Offset into the file to begin writing at */
){
  LONG upperBits = (offset>>32) & 0x7fffffff;
  LONG lowerBits = offset & 0xffffffff;
  DWORD rc;
  DWORD wrote;
  winFile *pFile = (winFile*)id;
  assert( id!=0 );
  SimulateIOError(return SQLITE_IOERR_WRITE);
  SimulateDiskfullError(return SQLITE_FULL);
  OSTRACE3("WRITE %d lock=%d\n", pFile->h, pFile->locktype);
  rc = SetFilePointer(pFile->h, lowerBits, &upperBits, FILE_BEGIN);
  if( rc==INVALID_SET_FILE_POINTER && GetLastError()!=NO_ERROR ){
    return SQLITE_FULL;
  }
  assert( amt>0 );
  while(
     amt>0
     && (rc = WriteFile(pFile->h, pBuf, amt, &wrote, 0))!=0
     && wrote>0
  ){
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  if( !rc || amt>(int)wrote ){
    return SQLITE_FULL;
  }
  return SQLITE_OK;
}

/*
** Truncate an open file to a specified size
*/
static int winTruncate(sqlite3_file *id, sqlite3_int64 nByte){
  LONG upperBits = (nByte>>32) & 0x7fffffff;
  LONG lowerBits = nByte & 0xffffffff;
  winFile *pFile = (winFile*)id;
  OSTRACE3("TRUNCATE %d %lld\n", pFile->h, nByte);
  SimulateIOError(return SQLITE_IOERR_TRUNCATE);
  SetFilePointer(pFile->h, lowerBits, &upperBits, FILE_BEGIN);
  SetEndOfFile(pFile->h);
  return SQLITE_OK;
}

#ifdef SQLITE_TEST
/*
** Count the number of fullsyncs and normal syncs.  This is used to test
** that syncs and fullsyncs are occuring at the right times.
*/
int sqlite3_sync_count = 0;
int sqlite3_fullsync_count = 0;
#endif

/*
** Make sure all writes to a particular file are committed to disk.
*/
static int winSync(sqlite3_file *id, int flags){
  winFile *pFile = (winFile*)id;
  OSTRACE3("SYNC %d lock=%d\n", pFile->h, pFile->locktype);
#ifdef SQLITE_TEST
  if( flags & SQLITE_SYNC_FULL ){
    sqlite3_fullsync_count++;
  }
  sqlite3_sync_count++;
#endif
  if( FlushFileBuffers(pFile->h) ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
}

/*
** Determine the current size of a file in bytes
*/
static int winFileSize(sqlite3_file *id, sqlite3_int64 *pSize){
  winFile *pFile = (winFile*)id;
  DWORD upperBits, lowerBits;
  SimulateIOError(return SQLITE_IOERR_FSTAT);
  lowerBits = GetFileSize(pFile->h, &upperBits);
  *pSize = (((sqlite3_int64)upperBits)<<32) + lowerBits;
  return SQLITE_OK;
}

/*
** LOCKFILE_FAIL_IMMEDIATELY is undefined on some Windows systems.
*/
#ifndef LOCKFILE_FAIL_IMMEDIATELY
# define LOCKFILE_FAIL_IMMEDIATELY 1
#endif

/*
** Acquire a reader lock.
** Different API routines are called depending on whether or not this
** is Win95 or WinNT.
*/
static int getReadLock(winFile *pFile){
  int res;
  if( isNT() ){
    OVERLAPPED ovlp;
    ovlp.Offset = SHARED_FIRST;
    ovlp.OffsetHigh = 0;
    ovlp.hEvent = 0;
    res = LockFileEx(pFile->h, LOCKFILE_FAIL_IMMEDIATELY,
                     0, SHARED_SIZE, 0, &ovlp);
  }else{
    int lk;
    sqlite3Randomness(sizeof(lk), &lk);
    pFile->sharedLockByte = (lk & 0x7fffffff)%(SHARED_SIZE - 1);
    res = LockFile(pFile->h, SHARED_FIRST+pFile->sharedLockByte, 0, 1, 0);
  }
  return res;
}

/*
** Undo a readlock
*/
static int unlockReadLock(winFile *pFile){
  int res;
  if( isNT() ){
    res = UnlockFile(pFile->h, SHARED_FIRST, 0, SHARED_SIZE, 0);
  }else{
    res = UnlockFile(pFile->h, SHARED_FIRST + pFile->sharedLockByte, 0, 1, 0);
  }
  return res;
}

/*
** Lock the file with the lock specified by parameter locktype - one
** of the following:
**
**     (1) SHARED_LOCK
**     (2) RESERVED_LOCK
**     (3) PENDING_LOCK
**     (4) EXCLUSIVE_LOCK
**
** Sometimes when requesting one lock state, additional lock states
** are inserted in between.  The locking might fail on one of the later
** transitions leaving the lock state different from what it started but
** still short of its goal.  The following chart shows the allowed
** transitions and the inserted intermediate states:
**
**    UNLOCKED -> SHARED
**    SHARED -> RESERVED
**    SHARED -> (PENDING) -> EXCLUSIVE
**    RESERVED -> (PENDING) -> EXCLUSIVE
**    PENDING -> EXCLUSIVE
**
** This routine will only increase a lock.  The winUnlock() routine
** erases all locks at once and returns us immediately to locking level 0.
** It is not possible to lower the locking level one step at a time.  You
** must go straight to locking level 0.
*/
static int winLock(sqlite3_file *id, int locktype){
  int rc = SQLITE_OK;    /* Return code from subroutines */
  int res = 1;           /* Result of a windows lock call */
  int newLocktype;       /* Set pFile->locktype to this value before exiting */
  int gotPendingLock = 0;/* True if we acquired a PENDING lock this time */
  winFile *pFile = (winFile*)id;

  assert( pFile!=0 );
  OSTRACE5("LOCK %d %d was %d(%d)\n",
          pFile->h, locktype, pFile->locktype, pFile->sharedLockByte);

  /* If there is already a lock of this type or more restrictive on the
  ** OsFile, do nothing. Don't use the end_lock: exit path, as
  ** sqlite3OsEnterMutex() hasn't been called yet.
  */
  if( pFile->locktype>=locktype ){
    return SQLITE_OK;
  }

  /* Make sure the locking sequence is correct
  */
  assert( pFile->locktype!=NO_LOCK || locktype==SHARED_LOCK );
  assert( locktype!=PENDING_LOCK );
  assert( locktype!=RESERVED_LOCK || pFile->locktype==SHARED_LOCK );

  /* Lock the PENDING_LOCK byte if we need to acquire a PENDING lock or
  ** a SHARED lock.  If we are acquiring a SHARED lock, the acquisition of
  ** the PENDING_LOCK byte is temporary.
  */
  newLocktype = pFile->locktype;
  if( pFile->locktype==NO_LOCK
   || (locktype==EXCLUSIVE_LOCK && pFile->locktype==RESERVED_LOCK)
  ){
    int cnt = 3;
    while( cnt-->0 && (res = LockFile(pFile->h, PENDING_BYTE, 0, 1, 0))==0 ){
      /* Try 3 times to get the pending lock.  The pending lock might be
      ** held by another reader process who will release it momentarily.
      */
      OSTRACE2("could not get a PENDING lock. cnt=%d\n", cnt);
      Sleep(1);
    }
    gotPendingLock = res;
  }

  /* Acquire a shared lock
  */
  if( locktype==SHARED_LOCK && res ){
    assert( pFile->locktype==NO_LOCK );
    res = getReadLock(pFile);
    if( res ){
      newLocktype = SHARED_LOCK;
    }
  }

  /* Acquire a RESERVED lock
  */
  if( locktype==RESERVED_LOCK && res ){
    assert( pFile->locktype==SHARED_LOCK );
    res = LockFile(pFile->h, RESERVED_BYTE, 0, 1, 0);
    if( res ){
      newLocktype = RESERVED_LOCK;
    }
  }

  /* Acquire a PENDING lock
  */
  if( locktype==EXCLUSIVE_LOCK && res ){
    newLocktype = PENDING_LOCK;
    gotPendingLock = 0;
  }

  /* Acquire an EXCLUSIVE lock
  */
  if( locktype==EXCLUSIVE_LOCK && res ){
    assert( pFile->locktype>=SHARED_LOCK );
    res = unlockReadLock(pFile);
    OSTRACE2("unreadlock = %d\n", res);
    res = LockFile(pFile->h, SHARED_FIRST, 0, SHARED_SIZE, 0);
    if( res ){
      newLocktype = EXCLUSIVE_LOCK;
    }else{
      OSTRACE2("error-code = %d\n", GetLastError());
      getReadLock(pFile);
    }
  }

  /* If we are holding a PENDING lock that ought to be released, then
  ** release it now.
  */
  if( gotPendingLock && locktype==SHARED_LOCK ){
    UnlockFile(pFile->h, PENDING_BYTE, 0, 1, 0);
  }

  /* Update the state of the lock has held in the file descriptor then
  ** return the appropriate result code.
  */
  if( res ){
    rc = SQLITE_OK;
  }else{
    OSTRACE4("LOCK FAILED %d trying for %d but got %d\n", pFile->h,
           locktype, newLocktype);
    rc = SQLITE_BUSY;
  }
  pFile->locktype = newLocktype;
  return rc;
}

/*
** This routine checks if there is a RESERVED lock held on the specified
** file by this or any other process. If such a lock is held, return
** non-zero, otherwise zero.
*/
static int winCheckReservedLock(sqlite3_file *id){
  int rc;
  winFile *pFile = (winFile*)id;
  assert( pFile!=0 );
  if( pFile->locktype>=RESERVED_LOCK ){
    rc = 1;
    OSTRACE3("TEST WR-LOCK %d %d (local)\n", pFile->h, rc);
  }else{
    rc = LockFile(pFile->h, RESERVED_BYTE, 0, 1, 0);
    if( rc ){
      UnlockFile(pFile->h, RESERVED_BYTE, 0, 1, 0);
    }
    rc = !rc;
    OSTRACE3("TEST WR-LOCK %d %d (remote)\n", pFile->h, rc);
  }
  return rc;
}

/*
** Lower the locking level on file descriptor id to locktype.  locktype
** must be either NO_LOCK or SHARED_LOCK.
**
** If the locking level of the file descriptor is already at or below
** the requested locking level, this routine is a no-op.
**
** It is not possible for this routine to fail if the second argument
** is NO_LOCK.  If the second argument is SHARED_LOCK then this routine
** might return SQLITE_IOERR;
*/
static int winUnlock(sqlite3_file *id, int locktype){
  int type;
  winFile *pFile = (winFile*)id;
  int rc = SQLITE_OK;
  assert( pFile!=0 );
  assert( locktype<=SHARED_LOCK );
  OSTRACE5("UNLOCK %d to %d was %d(%d)\n", pFile->h, locktype,
          pFile->locktype, pFile->sharedLockByte);
  type = pFile->locktype;
  if( type>=EXCLUSIVE_LOCK ){
    UnlockFile(pFile->h, SHARED_FIRST, 0, SHARED_SIZE, 0);
    if( locktype==SHARED_LOCK && !getReadLock(pFile) ){
      /* This should never happen.  We should always be able to
      ** reacquire the read lock */
      rc = SQLITE_IOERR_UNLOCK;
    }
  }
  if( type>=RESERVED_LOCK ){
    UnlockFile(pFile->h, RESERVED_BYTE, 0, 1, 0);
  }
  if( locktype==NO_LOCK && type>=SHARED_LOCK ){
    unlockReadLock(pFile);
  }
  if( type>=PENDING_LOCK ){
    UnlockFile(pFile->h, PENDING_BYTE, 0, 1, 0);
  }
  pFile->locktype = locktype;
  return rc;
}

/*
** Control and query of the open file handle.
*/
static int winFileControl(sqlite3_file *id, int op, void *pArg){
  switch( op ){
    case SQLITE_FCNTL_LOCKSTATE: {
      *(int*)pArg = ((winFile*)id)->locktype;
      return SQLITE_OK;
    }
  }
  return SQLITE_ERROR;
}

/*
** Return the sector size in bytes of the underlying block device for
** the specified file. This is almost always 512 bytes, but may be
** larger for some devices.
**
** SQLite code assumes this function cannot fail. It also assumes that
** if two files are created in the same file-system directory (i.e.
** a database and its journal file) that the sector size will be the
** same for both.
*/
static int winSectorSize(sqlite3_file *id){
  return SQLITE_DEFAULT_SECTOR_SIZE;
}

/*
** Return a vector of device characteristics.
*/
static int winDeviceCharacteristics(sqlite3_file *id){
  return 0;
}

/*
** This vector defines all the methods that can operate on an
** sqlite3_file for win32.
*/
static const sqlite3_io_methods winIoMethod = {
  1,                        /* iVersion */
  winClose,
  winRead,
  winWrite,
  winTruncate,
  winSync,
  winFileSize,
  winLock,
  winUnlock,
  winCheckReservedLock,
  winFileControl,
  winSectorSize,
  winDeviceCharacteristics
};

/***************************************************************************
** Here ends the I/O methods that form the sqlite3_io_methods object.
**
** The next block of code implements the VFS methods.
****************************************************************************/

/*
** Convert a UTF-8 filename into whatever form the underlying
** operating system wants filenames in.  Space to hold the result
** is obtained from malloc and must be freed by the calling
** function.
*/
static void *convertUtf8Filename(const char *zFilename){
  void *zConverted = 0;
  if( isNT() ){
    zConverted = utf8ToUnicode(zFilename);
  }else{
    zConverted = utf8ToMbcs(zFilename);
  }
  /* caller will handle out of memory */
  return zConverted;
}

/*
** Open a file.
*/
static int winOpen(
  sqlite3_vfs *pVfs,        /* Not used */
  const char *zName,        /* Name of the file (UTF-8) */
  sqlite3_file *id,         /* Write the SQLite file handle here */
  int flags,                /* Open mode flags */
  int *pOutFlags            /* Status return flags */
){
  HANDLE h;
  DWORD dwDesiredAccess;
  DWORD dwShareMode;
  DWORD dwCreationDisposition;
  DWORD dwFlagsAndAttributes = 0;
  int isTemp;
  winFile *pFile = (winFile*)id;
  void *zConverted = convertUtf8Filename(zName);
  if( zConverted==0 ){
    return SQLITE_NOMEM;
  }

  if( flags & SQLITE_OPEN_READWRITE ){
    dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
  }else{
    dwDesiredAccess = GENERIC_READ;
  }
  if( flags & SQLITE_OPEN_CREATE ){
    dwCreationDisposition = OPEN_ALWAYS;
  }else{
    dwCreationDisposition = OPEN_EXISTING;
  }
  if( flags & SQLITE_OPEN_MAIN_DB ){
    dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
  }else{
    dwShareMode = 0;
  }
  if( flags & SQLITE_OPEN_DELETEONCLOSE ){
#if OS_WINCE
    dwFlagsAndAttributes = FILE_ATTRIBUTE_HIDDEN;
#else
    dwFlagsAndAttributes = FILE_ATTRIBUTE_TEMPORARY
                               | FILE_ATTRIBUTE_HIDDEN
                               | FILE_FLAG_DELETE_ON_CLOSE;
#endif
    isTemp = 1;
  }else{
    dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
    isTemp = 0;
  }
  /* Reports from the internet are that performance is always
  ** better if FILE_FLAG_RANDOM_ACCESS is used.  Ticket #2699. */
  dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;
  if( isNT() ){
    h = CreateFileW((WCHAR*)zConverted,
       dwDesiredAccess,
       dwShareMode,
       NULL,
       dwCreationDisposition,
       dwFlagsAndAttributes,
       NULL
    );
  }else{
#if OS_WINCE
    return SQLITE_NOMEM;
#else
    h = CreateFileA((char*)zConverted,
       dwDesiredAccess,
       dwShareMode,
       NULL,
       dwCreationDisposition,
       dwFlagsAndAttributes,
       NULL
    );
#endif
  }
  if( h==INVALID_HANDLE_VALUE ){
    free(zConverted);
    if( flags & SQLITE_OPEN_READWRITE ){
      return winOpen(0, zName, id, 
             ((flags|SQLITE_OPEN_READONLY)&~SQLITE_OPEN_READWRITE), pOutFlags);
    }else{
      return SQLITE_CANTOPEN;
    }
  }
  if( pOutFlags ){
    if( flags & SQLITE_OPEN_READWRITE ){
      *pOutFlags = SQLITE_OPEN_READWRITE;
    }else{
      *pOutFlags = SQLITE_OPEN_READONLY;
    }
  }
  memset(pFile, 0, sizeof(*pFile));
  pFile->pMethod = &winIoMethod;
  pFile->h = h;
#if OS_WINCE
  if( (flags & (SQLITE_OPEN_READWRITE|SQLITE_OPEN_MAIN_DB)) ==
               (SQLITE_OPEN_READWRITE|SQLITE_OPEN_MAIN_DB)
       && !winceCreateLock(zName, pFile)
  ){
    CloseHandle(h);
    free(zConverted);
    return SQLITE_CANTOPEN;
  }
  if( isTemp ){
    pFile->zDeleteOnClose = zConverted;
  }else
#endif
  {
    free(zConverted);
  }
  OpenCounter(+1);
  return SQLITE_OK;
}

/*
** Delete the named file.
**
** Note that windows does not allow a file to be deleted if some other
** process has it open.  Sometimes a virus scanner or indexing program
** will open a journal file shortly after it is created in order to do
** whatever does.  While this other process is holding the
** file open, we will be unable to delete it.  To work around this
** problem, we delay 100 milliseconds and try to delete again.  Up
** to MX_DELETION_ATTEMPTs deletion attempts are run before giving
** up and returning an error.
*/
#define MX_DELETION_ATTEMPTS 5
static int winDelete(
  sqlite3_vfs *pVfs,          /* Not used on win32 */
  const char *zFilename,      /* Name of file to delete */
  int syncDir                 /* Not used on win32 */
){
  int cnt = 0;
  int rc;
  void *zConverted = convertUtf8Filename(zFilename);
  if( zConverted==0 ){
    return SQLITE_NOMEM;
  }
  SimulateIOError(return SQLITE_IOERR_DELETE);
  if( isNT() ){
    do{
      DeleteFileW(zConverted);
    }while( (rc = GetFileAttributesW(zConverted))!=0xffffffff 
            && cnt++ < MX_DELETION_ATTEMPTS && (Sleep(100), 1) );
  }else{
#if OS_WINCE
    return SQLITE_NOMEM;
#else
    do{
      DeleteFileA(zConverted);
    }while( (rc = GetFileAttributesA(zConverted))!=0xffffffff
            && cnt++ < MX_DELETION_ATTEMPTS && (Sleep(100), 1) );
#endif
  }
  free(zConverted);
  OSTRACE2("DELETE \"%s\"\n", zFilename);
  return rc==0xffffffff ? SQLITE_OK : SQLITE_IOERR_DELETE;
}

/*
** Check the existance and status of a file.
*/
static int winAccess(
  sqlite3_vfs *pVfs,         /* Not used on win32 */
  const char *zFilename,     /* Name of file to check */
  int flags                  /* Type of test to make on this file */
){
  DWORD attr;
  int rc;
  void *zConverted = convertUtf8Filename(zFilename);
  if( zConverted==0 ){
    return SQLITE_NOMEM;
  }
  if( isNT() ){
    attr = GetFileAttributesW((WCHAR*)zConverted);
  }else{
#if OS_WINCE
    return SQLITE_NOMEM;
#else
    attr = GetFileAttributesA((char*)zConverted);
#endif
  }
  free(zConverted);
  switch( flags ){
    case SQLITE_ACCESS_READ:
    case SQLITE_ACCESS_EXISTS:
      rc = attr!=0xffffffff;
      break;
    case SQLITE_ACCESS_READWRITE:
      rc = (attr & FILE_ATTRIBUTE_READONLY)==0;
      break;
    default:
      assert(!"Invalid flags argument");
  }
  return rc;
}


/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at pVfs->mxPathname characters.
*/
static int winGetTempname(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  char zTempPath[MAX_PATH+1];
  if( sqlite3_temp_directory ){
    sqlite3_snprintf(MAX_PATH-30, zTempPath, "%s", sqlite3_temp_directory);
  }else if( isNT() ){
    char *zMulti;
    WCHAR zWidePath[MAX_PATH];
    GetTempPathW(MAX_PATH-30, zWidePath);
    zMulti = unicodeToUtf8(zWidePath);
    if( zMulti ){
      sqlite3_snprintf(MAX_PATH-30, zTempPath, "%s", zMulti);
      free(zMulti);
    }else{
      return SQLITE_NOMEM;
    }
  }else{
    char *zUtf8;
    char zMbcsPath[MAX_PATH];
    GetTempPathA(MAX_PATH-30, zMbcsPath);
    zUtf8 = mbcsToUtf8(zMbcsPath);
    if( zUtf8 ){
      sqlite3_snprintf(MAX_PATH-30, zTempPath, "%s", zUtf8);
      free(zUtf8);
    }else{
      return SQLITE_NOMEM;
    }
  }
  for(i=strlen(zTempPath); i>0 && zTempPath[i-1]=='\\'; i--){}
  zTempPath[i] = 0;
  sqlite3_snprintf(nBuf-30, zBuf,
                   "%s\\"SQLITE_TEMP_FILE_PREFIX, zTempPath);
  j = strlen(zBuf);
  sqlite3Randomness(20, &zBuf[j]);
  for(i=0; i<20; i++, j++){
    zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
  }
  zBuf[j] = 0;
  OSTRACE2("TEMP FILENAME: %s\n", zBuf);
  return SQLITE_OK; 
}

/*
** Turn a relative pathname into a full pathname.  Write the full
** pathname into zOut[].  zOut[] will be at least pVfs->mxPathname
** bytes in size.
*/
static int winFullPathname(
  sqlite3_vfs *pVfs,            /* Pointer to vfs object */
  const char *zRelative,        /* Possibly relative input path */
  int nFull,                    /* Size of output buffer in bytes */
  char *zFull                   /* Output buffer */
){

#if defined(__CYGWIN__)
  cygwin_conv_to_full_win32_path(zRelative, zFull);
  return SQLITE_OK;
#endif

#if OS_WINCE
  /* WinCE has no concept of a relative pathname, or so I am told. */
  sqlite3_snprintf(pVfs->mxPathname, zFull, "%s", zRelative);
  return SQLITE_OK;
#endif

#if !OS_WINCE && !defined(__CYGWIN__)
  int nByte;
  void *zConverted;
  char *zOut;
  zConverted = convertUtf8Filename(zRelative);
  if( isNT() ){
    WCHAR *zTemp;
    nByte = GetFullPathNameW((WCHAR*)zConverted, 0, 0, 0) + 3;
    zTemp = malloc( nByte*sizeof(zTemp[0]) );
    if( zTemp==0 ){
      free(zConverted);
      return SQLITE_NOMEM;
    }
    GetFullPathNameW((WCHAR*)zConverted, nByte, zTemp, 0);
    free(zConverted);
    zOut = unicodeToUtf8(zTemp);
    free(zTemp);
  }else{
    char *zTemp;
    nByte = GetFullPathNameA((char*)zConverted, 0, 0, 0) + 3;
    zTemp = malloc( nByte*sizeof(zTemp[0]) );
    if( zTemp==0 ){
      free(zConverted);
      return SQLITE_NOMEM;
    }
    GetFullPathNameA((char*)zConverted, nByte, zTemp, 0);
    free(zConverted);
    zOut = mbcsToUtf8(zTemp);
    free(zTemp);
  }
  if( zOut ){
    sqlite3_snprintf(pVfs->mxPathname, zFull, "%s", zOut);
    free(zOut);
    return SQLITE_OK;
  }else{
    return SQLITE_NOMEM;
  }
#endif
}

#ifndef SQLITE_OMIT_LOAD_EXTENSION
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
static void *winDlOpen(sqlite3_vfs *pVfs, const char *zFilename){
  HANDLE h;
  void *zConverted = convertUtf8Filename(zFilename);
  if( zConverted==0 ){
    return 0;
  }
  if( isNT() ){
    h = LoadLibraryW((WCHAR*)zConverted);
  }else{
#if OS_WINCE
    return 0;
#else
    h = LoadLibraryA((char*)zConverted);
#endif
  }
  free(zConverted);
  return (void*)h;
}
static void winDlError(sqlite3_vfs *pVfs, int nBuf, char *zBufOut){
#if OS_WINCE
  int error = GetLastError();
  if( error>0x7FFFFFF ){
    sqlite3_snprintf(nBuf, zBufOut, "OsError 0x%x", error);
  }else{
    sqlite3_snprintf(nBuf, zBufOut, "OsError %d", error);
  }
#else
  FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    0,
    zBufOut,
    nBuf-1,
    0
  );
#endif
}
void *winDlSym(sqlite3_vfs *pVfs, void *pHandle, const char *zSymbol){
#if OS_WINCE
  /* The GetProcAddressA() routine is only available on wince. */
  return GetProcAddressA((HANDLE)pHandle, zSymbol);
#else
  /* All other windows platforms expect GetProcAddress() to take
  ** an Ansi string regardless of the _UNICODE setting */
  return GetProcAddress((HANDLE)pHandle, zSymbol);
#endif
}
void winDlClose(sqlite3_vfs *pVfs, void *pHandle){
  FreeLibrary((HANDLE)pHandle);
}
#else /* if SQLITE_OMIT_LOAD_EXTENSION is defined: */
  #define winDlOpen  0
  #define winDlError 0
  #define winDlSym   0
  #define winDlClose 0
#endif


/*
** Write up to nBuf bytes of randomness into zBuf.
*/
static int winRandomness(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
  int n = 0;
  if( sizeof(SYSTEMTIME)<=nBuf-n ){
    SYSTEMTIME x;
    GetSystemTime(&x);
    memcpy(&zBuf[n], &x, sizeof(x));
    n += sizeof(x);
  }
  if( sizeof(DWORD)<=nBuf-n ){
    DWORD pid = GetCurrentProcessId();
    memcpy(&zBuf[n], &pid, sizeof(pid));
    n += sizeof(pid);
  }
  if( sizeof(DWORD)<=nBuf-n ){
    DWORD cnt = GetTickCount();
    memcpy(&zBuf[n], &cnt, sizeof(cnt));
    n += sizeof(cnt);
  }
  if( sizeof(LARGE_INTEGER)<=nBuf-n ){
    LARGE_INTEGER i;
    QueryPerformanceCounter(&i);
    memcpy(&zBuf[n], &i, sizeof(i));
    n += sizeof(i);
  }
  return n;
}


/*
** Sleep for a little while.  Return the amount of time slept.
*/
static int winSleep(sqlite3_vfs *pVfs, int microsec){
  Sleep((microsec+999)/1000);
  return ((microsec+999)/1000)*1000;
}

/*
** The following variable, if set to a non-zero value, becomes the result
** returned from sqlite3OsCurrentTime().  This is used for testing.
*/
#ifdef SQLITE_TEST
int sqlite3_current_time = 0;
#endif

/*
** Find the current time (in Universal Coordinated Time).  Write the
** current time and date as a Julian Day number into *prNow and
** return 0.  Return 1 if the time and date cannot be found.
*/
int winCurrentTime(sqlite3_vfs *pVfs, double *prNow){
  FILETIME ft;
  /* FILETIME structure is a 64-bit value representing the number of 
     100-nanosecond intervals since January 1, 1601 (= JD 2305813.5). 
  */
  double now;
#if OS_WINCE
  SYSTEMTIME time;
  GetSystemTime(&time);
  SystemTimeToFileTime(&time,&ft);
#else
  GetSystemTimeAsFileTime( &ft );
#endif
  now = ((double)ft.dwHighDateTime) * 4294967296.0; 
  *prNow = (now + ft.dwLowDateTime)/864000000000.0 + 2305813.5;
#ifdef SQLITE_TEST
  if( sqlite3_current_time ){
    *prNow = sqlite3_current_time/86400.0 + 2440587.5;
  }
#endif
  return 0;
}


/*
** Return a pointer to the sqlite3DefaultVfs structure.   We use
** a function rather than give the structure global scope because
** some compilers (MSVC) do not allow forward declarations of
** initialized structures.
*/
sqlite3_vfs *sqlite3OsDefaultVfs(void){
  static sqlite3_vfs winVfs = {
    1,                 /* iVersion */
    sizeof(winFile),   /* szOsFile */
    MAX_PATH,          /* mxPathname */
    0,                 /* pNext */
    "win32",           /* zName */
    0,                 /* pAppData */
  
    winOpen,           /* xOpen */
    winDelete,         /* xDelete */
    winAccess,         /* xAccess */
    winGetTempname,    /* xGetTempName */
    winFullPathname,   /* xFullPathname */
    winDlOpen,         /* xDlOpen */
    winDlError,        /* xDlError */
    winDlSym,          /* xDlSym */
    winDlClose,        /* xDlClose */
    winRandomness,     /* xRandomness */
    winSleep,          /* xSleep */
    winCurrentTime     /* xCurrentTime */
  };
  
  return &winVfs;
}

#endif /* OS_WIN */
