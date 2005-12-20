/*
** 2001 September 16
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
** This file contains code that is specific to particular operating
** systems.  The purpose of this file is to provide a uniform abstraction
** on which the rest of SQLite can operate.
*/
#include "os.h"          /* Must be first to enable large file support */
#include "sqliteInt.h"

#if OS_UNIX
# include <time.h>
# include <errno.h>
# include <unistd.h>
# ifndef O_LARGEFILE
#  define O_LARGEFILE 0
# endif
# ifdef SQLITE_DISABLE_LFS
#  undef O_LARGEFILE
#  define O_LARGEFILE 0
# endif
# ifndef O_NOFOLLOW
#  define O_NOFOLLOW 0
# endif
# ifndef O_BINARY
#  define O_BINARY 0
# endif
#endif


#if OS_WIN
# include <winbase.h>
#endif

#if OS_MAC
# include <extras.h>
# include <path2fss.h>
# include <TextUtils.h>
# include <FinderRegistry.h>
# include <Folders.h>
# include <Timer.h>
# include <OSUtils.h>
#endif

/*
** The DJGPP compiler environment looks mostly like Unix, but it
** lacks the fcntl() system call.  So redefine fcntl() to be something
** that always succeeds.  This means that locking does not occur under
** DJGPP.  But its DOS - what did you expect?
*/
#ifdef __DJGPP__
# define fcntl(A,B,C) 0
#endif

/*
** Macros used to determine whether or not to use threads.  The
** SQLITE_UNIX_THREADS macro is defined if we are synchronizing for
** Posix threads and SQLITE_W32_THREADS is defined if we are
** synchronizing using Win32 threads.
*/
#if OS_UNIX && defined(THREADSAFE) && THREADSAFE
# include <pthread.h>
# define SQLITE_UNIX_THREADS 1
#endif
#if OS_WIN && defined(THREADSAFE) && THREADSAFE
# define SQLITE_W32_THREADS 1
#endif
#if OS_MAC && defined(THREADSAFE) && THREADSAFE
# include <Multiprocessing.h>
# define SQLITE_MACOS_MULTITASKING 1
#endif

/*
** Macros for performance tracing.  Normally turned off
*/
#if 0
static int last_page = 0;
__inline__ unsigned long long int hwtime(void){
  unsigned long long int x;
  __asm__("rdtsc\n\t"
          "mov %%edx, %%ecx\n\t"
          :"=A" (x));
  return x;
}
static unsigned long long int g_start;
static unsigned int elapse;
#define TIMER_START       g_start=hwtime()
#define TIMER_END         elapse=hwtime()-g_start
#define SEEK(X)           last_page=(X)
#define TRACE1(X)         fprintf(stderr,X)
#define TRACE2(X,Y)       fprintf(stderr,X,Y)
#define TRACE3(X,Y,Z)     fprintf(stderr,X,Y,Z)
#define TRACE4(X,Y,Z,A)   fprintf(stderr,X,Y,Z,A)
#define TRACE5(X,Y,Z,A,B) fprintf(stderr,X,Y,Z,A,B)
#else
#define TIMER_START
#define TIMER_END
#define SEEK(X)
#define TRACE1(X)
#define TRACE2(X,Y)
#define TRACE3(X,Y,Z)
#define TRACE4(X,Y,Z,A)
#define TRACE5(X,Y,Z,A,B)
#endif


#if OS_UNIX
/*
** Here is the dirt on POSIX advisory locks:  ANSI STD 1003.1 (1996)
** section 6.5.2.2 lines 483 through 490 specify that when a process
** sets or clears a lock, that operation overrides any prior locks set
** by the same process.  It does not explicitly say so, but this implies
** that it overrides locks set by the same process using a different
** file descriptor.  Consider this test case:
**
**       int fd1 = open("./file1", O_RDWR|O_CREAT, 0644);
**       int fd2 = open("./file2", O_RDWR|O_CREAT, 0644);
**
** Suppose ./file1 and ./file2 are really the same file (because
** one is a hard or symbolic link to the other) then if you set
** an exclusive lock on fd1, then try to get an exclusive lock
** on fd2, it works.  I would have expected the second lock to
** fail since there was already a lock on the file due to fd1.
** But not so.  Since both locks came from the same process, the
** second overrides the first, even though they were on different
** file descriptors opened on different file names.
**
** Bummer.  If you ask me, this is broken.  Badly broken.  It means
** that we cannot use POSIX locks to synchronize file access among
** competing threads of the same process.  POSIX locks will work fine
** to synchronize access for threads in separate processes, but not
** threads within the same process.
**
** To work around the problem, SQLite has to manage file locks internally
** on its own.  Whenever a new database is opened, we have to find the
** specific inode of the database file (the inode is determined by the
** st_dev and st_ino fields of the stat structure that fstat() fills in)
** and check for locks already existing on that inode.  When locks are
** created or removed, we have to look at our own internal record of the
** locks to see if another thread has previously set a lock on that same
** inode.
**
** The OsFile structure for POSIX is no longer just an integer file
** descriptor.  It is now a structure that holds the integer file
** descriptor and a pointer to a structure that describes the internal
** locks on the corresponding inode.  There is one locking structure
** per inode, so if the same inode is opened twice, both OsFile structures
** point to the same locking structure.  The locking structure keeps
** a reference count (so we will know when to delete it) and a "cnt"
** field that tells us its internal lock status.  cnt==0 means the
** file is unlocked.  cnt==-1 means the file has an exclusive lock.
** cnt>0 means there are cnt shared locks on the file.
**
** Any attempt to lock or unlock a file first checks the locking
** structure.  The fcntl() system call is only invoked to set a 
** POSIX lock if the internal lock structure transitions between
** a locked and an unlocked state.
**
** 2004-Jan-11:
** More recent discoveries about POSIX advisory locks.  (The more
** I discover, the more I realize the a POSIX advisory locks are
** an abomination.)
**
** If you close a file descriptor that points to a file that has locks,
** all locks on that file that are owned by the current process are
** released.  To work around this problem, each OsFile structure contains
** a pointer to an openCnt structure.  There is one openCnt structure
** per open inode, which means that multiple OsFiles can point to a single
** openCnt.  When an attempt is made to close an OsFile, if there are
** other OsFiles open on the same inode that are holding locks, the call
** to close() the file descriptor is deferred until all of the locks clear.
** The openCnt structure keeps a list of file descriptors that need to
** be closed and that list is walked (and cleared) when the last lock
** clears.
**
** First, under Linux threads, because each thread has a separate
** process ID, lock operations in one thread do not override locks
** to the same file in other threads.  Linux threads behave like
** separate processes in this respect.  But, if you close a file
** descriptor in linux threads, all locks are cleared, even locks
** on other threads and even though the other threads have different
** process IDs.  Linux threads is inconsistent in this respect.
** (I'm beginning to think that linux threads is an abomination too.)
** The consequence of this all is that the hash table for the lockInfo
** structure has to include the process id as part of its key because
** locks in different threads are treated as distinct.  But the 
** openCnt structure should not include the process id in its
** key because close() clears lock on all threads, not just the current
** thread.  Were it not for this goofiness in linux threads, we could
** combine the lockInfo and openCnt structures into a single structure.
*/

/*
** An instance of the following structure serves as the key used
** to locate a particular lockInfo structure given its inode.  Note
** that we have to include the process ID as part of the key.  On some
** threading implementations (ex: linux), each thread has a separate
** process ID.
*/
struct lockKey {
  dev_t dev;   /* Device number */
  ino_t ino;   /* Inode number */
  pid_t pid;   /* Process ID */
};

/*
** An instance of the following structure is allocated for each open
** inode on each thread with a different process ID.  (Threads have
** different process IDs on linux, but not on most other unixes.)
**
** A single inode can have multiple file descriptors, so each OsFile
** structure contains a pointer to an instance of this object and this
** object keeps a count of the number of OsFiles pointing to it.
*/
struct lockInfo {
  struct lockKey key;  /* The lookup key */
  int cnt;             /* 0: unlocked.  -1: write lock.  1...: read lock. */
  int nRef;            /* Number of pointers to this structure */
};

/*
** An instance of the following structure serves as the key used
** to locate a particular openCnt structure given its inode.  This
** is the same as the lockKey except that the process ID is omitted.
*/
struct openKey {
  dev_t dev;   /* Device number */
  ino_t ino;   /* Inode number */
};

/*
** An instance of the following structure is allocated for each open
** inode.  This structure keeps track of the number of locks on that
** inode.  If a close is attempted against an inode that is holding
** locks, the close is deferred until all locks clear by adding the
** file descriptor to be closed to the pending list.
*/
struct openCnt {
  struct openKey key;   /* The lookup key */
  int nRef;             /* Number of pointers to this structure */
  int nLock;            /* Number of outstanding locks */
  int nPending;         /* Number of pending close() operations */
  int *aPending;        /* Malloced space holding fd's awaiting a close() */
};

/* 
** These hash table maps inodes and process IDs into lockInfo and openCnt
** structures.  Access to these hash tables must be protected by a mutex.
*/
static Hash lockHash = { SQLITE_HASH_BINARY, 0, 0, 0, 0, 0 };
static Hash openHash = { SQLITE_HASH_BINARY, 0, 0, 0, 0, 0 };

/*
** Release a lockInfo structure previously allocated by findLockInfo().
*/
static void releaseLockInfo(struct lockInfo *pLock){
  pLock->nRef--;
  if( pLock->nRef==0 ){
    sqliteHashInsert(&lockHash, &pLock->key, sizeof(pLock->key), 0);
    sqliteFree(pLock);
  }
}

/*
** Release a openCnt structure previously allocated by findLockInfo().
*/
static void releaseOpenCnt(struct openCnt *pOpen){
  pOpen->nRef--;
  if( pOpen->nRef==0 ){
    sqliteHashInsert(&openHash, &pOpen->key, sizeof(pOpen->key), 0);
    sqliteFree(pOpen->aPending);
    sqliteFree(pOpen);
  }
}

/*
** Given a file descriptor, locate lockInfo and openCnt structures that
** describes that file descriptor.  Create a new ones if necessary.  The
** return values might be unset if an error occurs.
**
** Return the number of errors.
*/
int findLockInfo(
  int fd,                      /* The file descriptor used in the key */
  struct lockInfo **ppLock,    /* Return the lockInfo structure here */
  struct openCnt **ppOpen   /* Return the openCnt structure here */
){
  int rc;
  struct lockKey key1;
  struct openKey key2;
  struct stat statbuf;
  struct lockInfo *pLock;
  struct openCnt *pOpen;
  rc = fstat(fd, &statbuf);
  if( rc!=0 ) return 1;
  memset(&key1, 0, sizeof(key1));
  key1.dev = statbuf.st_dev;
  key1.ino = statbuf.st_ino;
  key1.pid = getpid();
  memset(&key2, 0, sizeof(key2));
  key2.dev = statbuf.st_dev;
  key2.ino = statbuf.st_ino;
  pLock = (struct lockInfo*)sqliteHashFind(&lockHash, &key1, sizeof(key1));
  if( pLock==0 ){
    struct lockInfo *pOld;
    pLock = sqliteMallocRaw( sizeof(*pLock) );
    if( pLock==0 ) return 1;
    pLock->key = key1;
    pLock->nRef = 1;
    pLock->cnt = 0;
    pOld = sqliteHashInsert(&lockHash, &pLock->key, sizeof(key1), pLock);
    if( pOld!=0 ){
      assert( pOld==pLock );
      sqliteFree(pLock);
      return 1;
    }
  }else{
    pLock->nRef++;
  }
  *ppLock = pLock;
  pOpen = (struct openCnt*)sqliteHashFind(&openHash, &key2, sizeof(key2));
  if( pOpen==0 ){
    struct openCnt *pOld;
    pOpen = sqliteMallocRaw( sizeof(*pOpen) );
    if( pOpen==0 ){
      releaseLockInfo(pLock);
      return 1;
    }
    pOpen->key = key2;
    pOpen->nRef = 1;
    pOpen->nLock = 0;
    pOpen->nPending = 0;
    pOpen->aPending = 0;
    pOld = sqliteHashInsert(&openHash, &pOpen->key, sizeof(key2), pOpen);
    if( pOld!=0 ){
      assert( pOld==pOpen );
      sqliteFree(pOpen);
      releaseLockInfo(pLock);
      return 1;
    }
  }else{
    pOpen->nRef++;
  }
  *ppOpen = pOpen;
  return 0;
}

#endif  /** POSIX advisory lock work-around **/

/*
** If we compile with the SQLITE_TEST macro set, then the following block
** of code will give us the ability to simulate a disk I/O error.  This
** is used for testing the I/O recovery logic.
*/
#ifdef SQLITE_TEST
int sqlite_io_error_pending = 0;
#define SimulateIOError(A)  \
   if( sqlite_io_error_pending ) \
     if( sqlite_io_error_pending-- == 1 ){ local_ioerr(); return A; }
static void local_ioerr(){
  sqlite_io_error_pending = 0;  /* Really just a place to set a breakpoint */
}
#else
#define SimulateIOError(A)
#endif

/*
** When testing, keep a count of the number of open files.
*/
#ifdef SQLITE_TEST
int sqlite_open_file_count = 0;
#define OpenCounter(X)  sqlite_open_file_count+=(X)
#else
#define OpenCounter(X)
#endif


/*
** Delete the named file
*/
int sqliteOsDelete(const char *zFilename){
#if OS_UNIX
  unlink(zFilename);
#endif
#if OS_WIN
  DeleteFile(zFilename);
#endif
#if OS_MAC
  unlink(zFilename);
#endif
  return SQLITE_OK;
}

/*
** Return TRUE if the named file exists.
*/
int sqliteOsFileExists(const char *zFilename){
#if OS_UNIX
  return access(zFilename, 0)==0;
#endif
#if OS_WIN
  return GetFileAttributes(zFilename) != 0xffffffff;
#endif
#if OS_MAC
  return access(zFilename, 0)==0;
#endif
}


#if 0 /* NOT USED */
/*
** Change the name of an existing file.
*/
int sqliteOsFileRename(const char *zOldName, const char *zNewName){
#if OS_UNIX
  if( link(zOldName, zNewName) ){
    return SQLITE_ERROR;
  }
  unlink(zOldName);
  return SQLITE_OK;
#endif
#if OS_WIN
  if( !MoveFile(zOldName, zNewName) ){
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
#endif
#if OS_MAC
  /**** FIX ME ***/
  return SQLITE_ERROR;
#endif
}
#endif /* NOT USED */

/*
** Attempt to open a file for both reading and writing.  If that
** fails, try opening it read-only.  If the file does not exist,
** try to create it.
**
** On success, a handle for the open file is written to *id
** and *pReadonly is set to 0 if the file was opened for reading and
** writing or 1 if the file was opened read-only.  The function returns
** SQLITE_OK.
**
** On failure, the function returns SQLITE_CANTOPEN and leaves
** *id and *pReadonly unchanged.
*/
int sqliteOsOpenReadWrite(
  const char *zFilename,
  OsFile *id,
  int *pReadonly
){
#if OS_UNIX
  int rc;
  id->dirfd = -1;
  id->fd = open(zFilename, O_RDWR|O_CREAT|O_LARGEFILE|O_BINARY, 0644);
  if( id->fd<0 ){
#ifdef EISDIR
    if( errno==EISDIR ){
      return SQLITE_CANTOPEN;
    }
#endif
    id->fd = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
    if( id->fd<0 ){
      return SQLITE_CANTOPEN; 
    }
    *pReadonly = 1;
  }else{
    *pReadonly = 0;
  }
  sqliteOsEnterMutex();
  rc = findLockInfo(id->fd, &id->pLock, &id->pOpen);
  sqliteOsLeaveMutex();
  if( rc ){
    close(id->fd);
    return SQLITE_NOMEM;
  }
  id->locked = 0;
  TRACE3("OPEN    %-3d %s\n", id->fd, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_WIN
  HANDLE h = CreateFile(zFilename,
     GENERIC_READ | GENERIC_WRITE,
     FILE_SHARE_READ | FILE_SHARE_WRITE,
     NULL,
     OPEN_ALWAYS,
     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
     NULL
  );
  if( h==INVALID_HANDLE_VALUE ){
    h = CreateFile(zFilename,
       GENERIC_READ,
       FILE_SHARE_READ,
       NULL,
       OPEN_ALWAYS,
       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
       NULL
    );
    if( h==INVALID_HANDLE_VALUE ){
      return SQLITE_CANTOPEN;
    }
    *pReadonly = 1;
  }else{
    *pReadonly = 0;
  }
  id->h = h;
  id->locked = 0;
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_MAC
  FSSpec fsSpec;
# ifdef _LARGE_FILE
  HFSUniStr255 dfName;
  FSRef fsRef;
  if( __path2fss(zFilename, &fsSpec) != noErr ){
    if( HCreate(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, 'SQLI', cDocumentFile) != noErr )
      return SQLITE_CANTOPEN;
  }
  if( FSpMakeFSRef(&fsSpec, &fsRef) != noErr )
    return SQLITE_CANTOPEN;
  FSGetDataForkName(&dfName);
  if( FSOpenFork(&fsRef, dfName.length, dfName.unicode,
                 fsRdWrShPerm, &(id->refNum)) != noErr ){
    if( FSOpenFork(&fsRef, dfName.length, dfName.unicode,
                   fsRdWrPerm, &(id->refNum)) != noErr ){
      if (FSOpenFork(&fsRef, dfName.length, dfName.unicode,
                   fsRdPerm, &(id->refNum)) != noErr )
        return SQLITE_CANTOPEN;
      else
        *pReadonly = 1;
    } else
      *pReadonly = 0;
  } else
    *pReadonly = 0;
# else
  __path2fss(zFilename, &fsSpec);
  if( !sqliteOsFileExists(zFilename) ){
    if( HCreate(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, 'SQLI', cDocumentFile) != noErr )
      return SQLITE_CANTOPEN;
  }
  if( HOpenDF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdWrShPerm, &(id->refNum)) != noErr ){
    if( HOpenDF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdWrPerm, &(id->refNum)) != noErr ){
      if( HOpenDF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdPerm, &(id->refNum)) != noErr )
        return SQLITE_CANTOPEN;
      else
        *pReadonly = 1;
    } else
      *pReadonly = 0;
  } else
    *pReadonly = 0;
# endif
  if( HOpenRF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdWrShPerm, &(id->refNumRF)) != noErr){
    id->refNumRF = -1;
  }
  id->locked = 0;
  id->delOnClose = 0;
  OpenCounter(+1);
  return SQLITE_OK;
#endif
}


/*
** Attempt to open a new file for exclusive access by this process.
** The file will be opened for both reading and writing.  To avoid
** a potential security problem, we do not allow the file to have
** previously existed.  Nor do we allow the file to be a symbolic
** link.
**
** If delFlag is true, then make arrangements to automatically delete
** the file when it is closed.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
*/
int sqliteOsOpenExclusive(const char *zFilename, OsFile *id, int delFlag){
#if OS_UNIX
  int rc;
  if( access(zFilename, 0)==0 ){
    return SQLITE_CANTOPEN;
  }
  id->dirfd = -1;
  id->fd = open(zFilename,
                O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_LARGEFILE|O_BINARY, 0600);
  if( id->fd<0 ){
    return SQLITE_CANTOPEN;
  }
  sqliteOsEnterMutex();
  rc = findLockInfo(id->fd, &id->pLock, &id->pOpen);
  sqliteOsLeaveMutex();
  if( rc ){
    close(id->fd);
    unlink(zFilename);
    return SQLITE_NOMEM;
  }
  id->locked = 0;
  if( delFlag ){
    unlink(zFilename);
  }
  TRACE3("OPEN-EX %-3d %s\n", id->fd, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_WIN
  HANDLE h;
  int fileflags;
  if( delFlag ){
    fileflags = FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_RANDOM_ACCESS 
                     | FILE_FLAG_DELETE_ON_CLOSE;
  }else{
    fileflags = FILE_FLAG_RANDOM_ACCESS;
  }
  h = CreateFile(zFilename,
     GENERIC_READ | GENERIC_WRITE,
     0,
     NULL,
     CREATE_ALWAYS,
     fileflags,
     NULL
  );
  if( h==INVALID_HANDLE_VALUE ){
    return SQLITE_CANTOPEN;
  }
  id->h = h;
  id->locked = 0;
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_MAC
  FSSpec fsSpec;
# ifdef _LARGE_FILE
  HFSUniStr255 dfName;
  FSRef fsRef;
  __path2fss(zFilename, &fsSpec);
  if( HCreate(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, 'SQLI', cDocumentFile) != noErr )
    return SQLITE_CANTOPEN;
  if( FSpMakeFSRef(&fsSpec, &fsRef) != noErr )
    return SQLITE_CANTOPEN;
  FSGetDataForkName(&dfName);
  if( FSOpenFork(&fsRef, dfName.length, dfName.unicode,
                 fsRdWrPerm, &(id->refNum)) != noErr )
    return SQLITE_CANTOPEN;
# else
  __path2fss(zFilename, &fsSpec);
  if( HCreate(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, 'SQLI', cDocumentFile) != noErr )
    return SQLITE_CANTOPEN;
  if( HOpenDF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdWrPerm, &(id->refNum)) != noErr )
    return SQLITE_CANTOPEN;
# endif
  id->refNumRF = -1;
  id->locked = 0;
  id->delOnClose = delFlag;
  if (delFlag)
    id->pathToDel = sqliteOsFullPathname(zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
#endif
}

/*
** Attempt to open a new file for read-only access.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
*/
int sqliteOsOpenReadOnly(const char *zFilename, OsFile *id){
#if OS_UNIX
  int rc;
  id->dirfd = -1;
  id->fd = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
  if( id->fd<0 ){
    return SQLITE_CANTOPEN;
  }
  sqliteOsEnterMutex();
  rc = findLockInfo(id->fd, &id->pLock, &id->pOpen);
  sqliteOsLeaveMutex();
  if( rc ){
    close(id->fd);
    return SQLITE_NOMEM;
  }
  id->locked = 0;
  TRACE3("OPEN-RO %-3d %s\n", id->fd, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_WIN
  HANDLE h = CreateFile(zFilename,
     GENERIC_READ,
     0,
     NULL,
     OPEN_EXISTING,
     FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS,
     NULL
  );
  if( h==INVALID_HANDLE_VALUE ){
    return SQLITE_CANTOPEN;
  }
  id->h = h;
  id->locked = 0;
  OpenCounter(+1);
  return SQLITE_OK;
#endif
#if OS_MAC
  FSSpec fsSpec;
# ifdef _LARGE_FILE
  HFSUniStr255 dfName;
  FSRef fsRef;
  if( __path2fss(zFilename, &fsSpec) != noErr )
    return SQLITE_CANTOPEN;
  if( FSpMakeFSRef(&fsSpec, &fsRef) != noErr )
    return SQLITE_CANTOPEN;
  FSGetDataForkName(&dfName);
  if( FSOpenFork(&fsRef, dfName.length, dfName.unicode,
                 fsRdPerm, &(id->refNum)) != noErr )
    return SQLITE_CANTOPEN;
# else
  __path2fss(zFilename, &fsSpec);
  if( HOpenDF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdPerm, &(id->refNum)) != noErr )
    return SQLITE_CANTOPEN;
# endif
  if( HOpenRF(fsSpec.vRefNum, fsSpec.parID, fsSpec.name, fsRdWrShPerm, &(id->refNumRF)) != noErr){
    id->refNumRF = -1;
  }
  id->locked = 0;
  id->delOnClose = 0;
  OpenCounter(+1);
  return SQLITE_OK;
#endif
}

/*
** Attempt to open a file descriptor for the directory that contains a
** file.  This file descriptor can be used to fsync() the directory
** in order to make sure the creation of a new file is actually written
** to disk.
**
** This routine is only meaningful for Unix.  It is a no-op under
** windows since windows does not support hard links.
**
** On success, a handle for a previously open file is at *id is
** updated with the new directory file descriptor and SQLITE_OK is
** returned.
**
** On failure, the function returns SQLITE_CANTOPEN and leaves
** *id unchanged.
*/
int sqliteOsOpenDirectory(
  const char *zDirname,
  OsFile *id
){
#if OS_UNIX
  if( id->fd<0 ){
    /* Do not open the directory if the corresponding file is not already
    ** open. */
    return SQLITE_CANTOPEN;
  }
  assert( id->dirfd<0 );
  id->dirfd = open(zDirname, O_RDONLY|O_BINARY, 0644);
  if( id->dirfd<0 ){
    return SQLITE_CANTOPEN; 
  }
  TRACE3("OPENDIR %-3d %s\n", id->dirfd, zDirname);
#endif
  return SQLITE_OK;
}

/*
** If the following global variable points to a string which is the
** name of a directory, then that directory will be used to store
** temporary files.
*/
const char *sqlite_temp_directory = 0;

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at least SQLITE_TEMPNAME_SIZE characters.
*/
int sqliteOsTempFileName(char *zBuf){
#if OS_UNIX
  static const char *azDirs[] = {
     0,
     "/var/tmp",
     "/usr/tmp",
     "/tmp",
     ".",
  };
  static unsigned char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  struct stat buf;
  const char *zDir = ".";
  azDirs[0] = sqlite_temp_directory;
  for(i=0; i<sizeof(azDirs)/sizeof(azDirs[0]); i++){
    if( azDirs[i]==0 ) continue;
    if( stat(azDirs[i], &buf) ) continue;
    if( !S_ISDIR(buf.st_mode) ) continue;
    if( access(azDirs[i], 07) ) continue;
    zDir = azDirs[i];
    break;
  }
  do{
    sprintf(zBuf, "%s/"TEMP_FILE_PREFIX, zDir);
    j = strlen(zBuf);
    sqliteRandomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
  }while( access(zBuf,0)==0 );
#endif
#if OS_WIN
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  const char *zDir;
  char zTempPath[SQLITE_TEMPNAME_SIZE];
  if( sqlite_temp_directory==0 ){
    GetTempPath(SQLITE_TEMPNAME_SIZE-30, zTempPath);
    for(i=strlen(zTempPath); i>0 && zTempPath[i-1]=='\\'; i--){}
    zTempPath[i] = 0;
    zDir = zTempPath;
  }else{
    zDir = sqlite_temp_directory;
  }
  for(;;){
    sprintf(zBuf, "%s\\"TEMP_FILE_PREFIX, zDir);
    j = strlen(zBuf);
    sqliteRandomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
    if( !sqliteOsFileExists(zBuf) ) break;
  }
#endif
#if OS_MAC
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  char *zDir;
  char zTempPath[SQLITE_TEMPNAME_SIZE];
  char zdirName[32];
  CInfoPBRec infoRec;
  Str31 dirName;
  memset(&infoRec, 0, sizeof(infoRec));
  memset(zTempPath, 0, SQLITE_TEMPNAME_SIZE);
  if( sqlite_temp_directory!=0 ){
    zDir = sqlite_temp_directory;
  }else if( FindFolder(kOnSystemDisk, kTemporaryFolderType,  kCreateFolder,
       &(infoRec.dirInfo.ioVRefNum), &(infoRec.dirInfo.ioDrParID)) == noErr ){
    infoRec.dirInfo.ioNamePtr = dirName;
    do{
      infoRec.dirInfo.ioFDirIndex = -1;
      infoRec.dirInfo.ioDrDirID = infoRec.dirInfo.ioDrParID;
      if( PBGetCatInfoSync(&infoRec) == noErr ){
        CopyPascalStringToC(dirName, zdirName);
        i = strlen(zdirName);
        memmove(&(zTempPath[i+1]), zTempPath, strlen(zTempPath));
        strcpy(zTempPath, zdirName);
        zTempPath[i] = ':';
      }else{
        *zTempPath = 0;
        break;
      }
    } while( infoRec.dirInfo.ioDrDirID != fsRtDirID );
    zDir = zTempPath;
  }
  if( zDir[0]==0 ){
    getcwd(zTempPath, SQLITE_TEMPNAME_SIZE-24);
    zDir = zTempPath;
  }
  for(;;){
    sprintf(zBuf, "%s"TEMP_FILE_PREFIX, zDir);
    j = strlen(zBuf);
    sqliteRandomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
    if( !sqliteOsFileExists(zBuf) ) break;
  }
#endif
  return SQLITE_OK; 
}

/*
** Close a file.
*/
int sqliteOsClose(OsFile *id){
#if OS_UNIX
  sqliteOsUnlock(id);
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  sqliteOsEnterMutex();
  if( id->pOpen->nLock ){
    /* If there are outstanding locks, do not actually close the file just
    ** yet because that would clear those locks.  Instead, add the file
    ** descriptor to pOpen->aPending.  It will be automatically closed when
    ** the last lock is cleared.
    */
    int *aNew;
    struct openCnt *pOpen = id->pOpen;
    pOpen->nPending++;
    aNew = sqliteRealloc( pOpen->aPending, pOpen->nPending*sizeof(int) );
    if( aNew==0 ){
      /* If a malloc fails, just leak the file descriptor */
    }else{
      pOpen->aPending = aNew;
      pOpen->aPending[pOpen->nPending-1] = id->fd;
    }
  }else{
    /* There are no outstanding locks so we can close the file immediately */
    close(id->fd);
  }
  releaseLockInfo(id->pLock);
  releaseOpenCnt(id->pOpen);
  sqliteOsLeaveMutex();
  TRACE2("CLOSE   %-3d\n", id->fd);
  OpenCounter(-1);
  return SQLITE_OK;
#endif
#if OS_WIN
  CloseHandle(id->h);
  OpenCounter(-1);
  return SQLITE_OK;
#endif
#if OS_MAC
  if( id->refNumRF!=-1 )
    FSClose(id->refNumRF);
# ifdef _LARGE_FILE
  FSCloseFork(id->refNum);
# else
  FSClose(id->refNum);
# endif
  if( id->delOnClose ){
    unlink(id->pathToDel);
    sqliteFree(id->pathToDel);
  }
  OpenCounter(-1);
  return SQLITE_OK;
#endif
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
int sqliteOsRead(OsFile *id, void *pBuf, int amt){
#if OS_UNIX
  int got;
  SimulateIOError(SQLITE_IOERR);
  TIMER_START;
  got = read(id->fd, pBuf, amt);
  TIMER_END;
  TRACE4("READ    %-3d %7d %d\n", id->fd, last_page, elapse);
  SEEK(0);
  /* if( got<0 ) got = 0; */
  if( got==amt ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
#endif
#if OS_WIN
  DWORD got;
  SimulateIOError(SQLITE_IOERR);
  TRACE2("READ %d\n", last_page);
  if( !ReadFile(id->h, pBuf, amt, &got, 0) ){
    got = 0;
  }
  if( got==(DWORD)amt ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
#endif
#if OS_MAC
  int got;
  SimulateIOError(SQLITE_IOERR);
  TRACE2("READ %d\n", last_page);
# ifdef _LARGE_FILE
  FSReadFork(id->refNum, fsAtMark, 0, (ByteCount)amt, pBuf, (ByteCount*)&got);
# else
  got = amt;
  FSRead(id->refNum, &got, pBuf);
# endif
  if( got==amt ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
#endif
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
int sqliteOsWrite(OsFile *id, const void *pBuf, int amt){
#if OS_UNIX
  int wrote = 0;
  SimulateIOError(SQLITE_IOERR);
  TIMER_START;
  while( amt>0 && (wrote = write(id->fd, pBuf, amt))>0 ){
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  TIMER_END;
  TRACE4("WRITE   %-3d %7d %d\n", id->fd, last_page, elapse);
  SEEK(0);
  if( amt>0 ){
    return SQLITE_FULL;
  }
  return SQLITE_OK;
#endif
#if OS_WIN
  int rc;
  DWORD wrote;
  SimulateIOError(SQLITE_IOERR);
  TRACE2("WRITE %d\n", last_page);
  while( amt>0 && (rc = WriteFile(id->h, pBuf, amt, &wrote, 0))!=0 && wrote>0 ){
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  if( !rc || amt>(int)wrote ){
    return SQLITE_FULL;
  }
  return SQLITE_OK;
#endif
#if OS_MAC
  OSErr oserr;
  int wrote = 0;
  SimulateIOError(SQLITE_IOERR);
  TRACE2("WRITE %d\n", last_page);
  while( amt>0 ){
# ifdef _LARGE_FILE
    oserr = FSWriteFork(id->refNum, fsAtMark, 0,
                        (ByteCount)amt, pBuf, (ByteCount*)&wrote);
# else
    wrote = amt;
    oserr = FSWrite(id->refNum, &wrote, pBuf);
# endif
    if( wrote == 0 || oserr != noErr)
      break;
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  if( oserr != noErr || amt>wrote ){
    return SQLITE_FULL;
  }
  return SQLITE_OK;
#endif
}

/*
** Move the read/write pointer in a file.
*/
int sqliteOsSeek(OsFile *id, off_t offset){
  SEEK(offset/1024 + 1);
#if OS_UNIX
  lseek(id->fd, offset, SEEK_SET);
  return SQLITE_OK;
#endif
#if OS_WIN
  {
    LONG upperBits = offset>>32;
    LONG lowerBits = offset & 0xffffffff;
    DWORD rc;
    rc = SetFilePointer(id->h, lowerBits, &upperBits, FILE_BEGIN);
    /* TRACE3("SEEK rc=0x%x upper=0x%x\n", rc, upperBits); */
  }
  return SQLITE_OK;
#endif
#if OS_MAC
  {
    off_t curSize;
    if( sqliteOsFileSize(id, &curSize) != SQLITE_OK ){
      return SQLITE_IOERR;
    }
    if( offset >= curSize ){
      if( sqliteOsTruncate(id, offset+1) != SQLITE_OK ){
        return SQLITE_IOERR;
      }
    }
# ifdef _LARGE_FILE
    if( FSSetForkPosition(id->refNum, fsFromStart, offset) != noErr ){
# else
    if( SetFPos(id->refNum, fsFromStart, offset) != noErr ){
# endif
      return SQLITE_IOERR;
    }else{
      return SQLITE_OK;
    }
  }
#endif
}

#ifdef SQLITE_NOSYNC
# define fsync(X) 0
#endif

/*
** Make sure all writes to a particular file are committed to disk.
**
** Under Unix, also make sure that the directory entry for the file
** has been created by fsync-ing the directory that contains the file.
** If we do not do this and we encounter a power failure, the directory
** entry for the journal might not exist after we reboot.  The next
** SQLite to access the file will not know that the journal exists (because
** the directory entry for the journal was never created) and the transaction
** will not roll back - possibly leading to database corruption.
*/
int sqliteOsSync(OsFile *id){
#if OS_UNIX
  SimulateIOError(SQLITE_IOERR);
  TRACE2("SYNC    %-3d\n", id->fd);
  if( fsync(id->fd) ){
    return SQLITE_IOERR;
  }else{
    if( id->dirfd>=0 ){
      TRACE2("DIRSYNC %-3d\n", id->dirfd);
      fsync(id->dirfd);
      close(id->dirfd);  /* Only need to sync once, so close the directory */
      id->dirfd = -1;    /* when we are done. */
    }
    return SQLITE_OK;
  }
#endif
#if OS_WIN
  if( FlushFileBuffers(id->h) ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
#endif
#if OS_MAC
# ifdef _LARGE_FILE
  if( FSFlushFork(id->refNum) != noErr ){
# else
  ParamBlockRec params;
  memset(&params, 0, sizeof(ParamBlockRec));
  params.ioParam.ioRefNum = id->refNum;
  if( PBFlushFileSync(&params) != noErr ){
# endif
    return SQLITE_IOERR;
  }else{
    return SQLITE_OK;
  }
#endif
}

/*
** Truncate an open file to a specified size
*/
int sqliteOsTruncate(OsFile *id, off_t nByte){
  SimulateIOError(SQLITE_IOERR);
#if OS_UNIX
  return ftruncate(id->fd, nByte)==0 ? SQLITE_OK : SQLITE_IOERR;
#endif
#if OS_WIN
  {
    LONG upperBits = nByte>>32;
    SetFilePointer(id->h, nByte, &upperBits, FILE_BEGIN);
    SetEndOfFile(id->h);
  }
  return SQLITE_OK;
#endif
#if OS_MAC
# ifdef _LARGE_FILE
  if( FSSetForkSize(id->refNum, fsFromStart, nByte) != noErr){
# else
  if( SetEOF(id->refNum, nByte) != noErr ){
# endif
    return SQLITE_IOERR;
  }else{
    return SQLITE_OK;
  }
#endif
}

/*
** Determine the current size of a file in bytes
*/
int sqliteOsFileSize(OsFile *id, off_t *pSize){
#if OS_UNIX
  struct stat buf;
  SimulateIOError(SQLITE_IOERR);
  if( fstat(id->fd, &buf)!=0 ){
    return SQLITE_IOERR;
  }
  *pSize = buf.st_size;
  return SQLITE_OK;
#endif
#if OS_WIN
  DWORD upperBits, lowerBits;
  SimulateIOError(SQLITE_IOERR);
  lowerBits = GetFileSize(id->h, &upperBits);
  *pSize = (((off_t)upperBits)<<32) + lowerBits;
  return SQLITE_OK;
#endif
#if OS_MAC
# ifdef _LARGE_FILE
  if( FSGetForkSize(id->refNum, pSize) != noErr){
# else
  if( GetEOF(id->refNum, pSize) != noErr ){
# endif
    return SQLITE_IOERR;
  }else{
    return SQLITE_OK;
  }
#endif
}

#if OS_WIN
/*
** Return true (non-zero) if we are running under WinNT, Win2K or WinXP.
** Return false (zero) for Win95, Win98, or WinME.
**
** Here is an interesting observation:  Win95, Win98, and WinME lack
** the LockFileEx() API.  But we can still statically link against that
** API as long as we don't call it win running Win95/98/ME.  A call to
** this routine is used to determine if the host is Win95/98/ME or
** WinNT/2K/XP so that we will know whether or not we can safely call
** the LockFileEx() API.
*/
int isNT(void){
  static int osType = 0;   /* 0=unknown 1=win95 2=winNT */
  if( osType==0 ){
    OSVERSIONINFO sInfo;
    sInfo.dwOSVersionInfoSize = sizeof(sInfo);
    GetVersionEx(&sInfo);
    osType = sInfo.dwPlatformId==VER_PLATFORM_WIN32_NT ? 2 : 1;
  }
  return osType==2;
}
#endif

/*
** Windows file locking notes:  [similar issues apply to MacOS]
**
** We cannot use LockFileEx() or UnlockFileEx() on Win95/98/ME because
** those functions are not available.  So we use only LockFile() and
** UnlockFile().
**
** LockFile() prevents not just writing but also reading by other processes.
** (This is a design error on the part of Windows, but there is nothing
** we can do about that.)  So the region used for locking is at the
** end of the file where it is unlikely to ever interfere with an
** actual read attempt.
**
** A database read lock is obtained by locking a single randomly-chosen 
** byte out of a specific range of bytes. The lock byte is obtained at 
** random so two separate readers can probably access the file at the 
** same time, unless they are unlucky and choose the same lock byte.
** A database write lock is obtained by locking all bytes in the range.
** There can only be one writer.
**
** A lock is obtained on the first byte of the lock range before acquiring
** either a read lock or a write lock.  This prevents two processes from
** attempting to get a lock at a same time.  The semantics of 
** sqliteOsReadLock() require that if there is already a write lock, that
** lock is converted into a read lock atomically.  The lock on the first
** byte allows us to drop the old write lock and get the read lock without
** another process jumping into the middle and messing us up.  The same
** argument applies to sqliteOsWriteLock().
**
** On WinNT/2K/XP systems, LockFileEx() and UnlockFileEx() are available,
** which means we can use reader/writer locks.  When reader writer locks
** are used, the lock is placed on the same range of bytes that is used
** for probabilistic locking in Win95/98/ME.  Hence, the locking scheme
** will support two or more Win95 readers or two or more WinNT readers.
** But a single Win95 reader will lock out all WinNT readers and a single
** WinNT reader will lock out all other Win95 readers.
**
** Note: On MacOS we use the resource fork for locking.
**
** The following #defines specify the range of bytes used for locking.
** N_LOCKBYTE is the number of bytes available for doing the locking.
** The first byte used to hold the lock while the lock is changing does
** not count toward this number.  FIRST_LOCKBYTE is the address of
** the first byte in the range of bytes used for locking.
*/
#define N_LOCKBYTE       10239
#if OS_MAC
# define FIRST_LOCKBYTE   (0x000fffff - N_LOCKBYTE)
#else
# define FIRST_LOCKBYTE   (0xffffffff - N_LOCKBYTE)
#endif

/*
** Change the status of the lock on the file "id" to be a readlock.
** If the file was write locked, then this reduces the lock to a read.
** If the file was read locked, then this acquires a new read lock.
**
** Return SQLITE_OK on success and SQLITE_BUSY on failure.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqliteOsReadLock(OsFile *id){
#if OS_UNIX
  int rc;
  sqliteOsEnterMutex();
  if( id->pLock->cnt>0 ){
    if( !id->locked ){
      id->pLock->cnt++;
      id->locked = 1;
      id->pOpen->nLock++;
    }
    rc = SQLITE_OK;
  }else if( id->locked || id->pLock->cnt==0 ){
    struct flock lock;
    int s;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0L;
    s = fcntl(id->fd, F_SETLK, &lock);
    if( s!=0 ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }else{
      rc = SQLITE_OK;
      if( !id->locked ){
        id->pOpen->nLock++;
        id->locked = 1;
      }
      id->pLock->cnt = 1;
    }
  }else{
    rc = SQLITE_BUSY;
  }
  sqliteOsLeaveMutex();
  return rc;
#endif
#if OS_WIN
  int rc;
  if( id->locked>0 ){
    rc = SQLITE_OK;
  }else{
    int lk;
    int res;
    int cnt = 100;
    sqliteRandomness(sizeof(lk), &lk);
    lk = (lk & 0x7fffffff)%N_LOCKBYTE + 1;
    while( cnt-->0 && (res = LockFile(id->h, FIRST_LOCKBYTE, 0, 1, 0))==0 ){
      Sleep(1);
    }
    if( res ){
      UnlockFile(id->h, FIRST_LOCKBYTE+1, 0, N_LOCKBYTE, 0);
      if( isNT() ){
        OVERLAPPED ovlp;
        ovlp.Offset = FIRST_LOCKBYTE+1;
        ovlp.OffsetHigh = 0;
        ovlp.hEvent = 0;
        res = LockFileEx(id->h, LOCKFILE_FAIL_IMMEDIATELY, 
                          0, N_LOCKBYTE, 0, &ovlp);
      }else{
        res = LockFile(id->h, FIRST_LOCKBYTE+lk, 0, 1, 0);
      }
      UnlockFile(id->h, FIRST_LOCKBYTE, 0, 1, 0);
    }
    if( res ){
      id->locked = lk;
      rc = SQLITE_OK;
    }else{
      rc = SQLITE_BUSY;
    }
  }
  return rc;
#endif
#if OS_MAC
  int rc;
  if( id->locked>0 || id->refNumRF == -1 ){
    rc = SQLITE_OK;
  }else{
    int lk;
    OSErr res;
    int cnt = 5;
    ParamBlockRec params;
    sqliteRandomness(sizeof(lk), &lk);
    lk = (lk & 0x7fffffff)%N_LOCKBYTE + 1;
    memset(&params, 0, sizeof(params));
    params.ioParam.ioRefNum = id->refNumRF;
    params.ioParam.ioPosMode = fsFromStart;
    params.ioParam.ioPosOffset = FIRST_LOCKBYTE;
    params.ioParam.ioReqCount = 1;
    while( cnt-->0 && (res = PBLockRangeSync(&params))!=noErr ){
      UInt32 finalTicks;
      Delay(1, &finalTicks); /* 1/60 sec */
    }
    if( res == noErr ){
      params.ioParam.ioPosOffset = FIRST_LOCKBYTE+1;
      params.ioParam.ioReqCount = N_LOCKBYTE;
      PBUnlockRangeSync(&params);
      params.ioParam.ioPosOffset = FIRST_LOCKBYTE+lk;
      params.ioParam.ioReqCount = 1;
      res = PBLockRangeSync(&params);
      params.ioParam.ioPosOffset = FIRST_LOCKBYTE;
      params.ioParam.ioReqCount = 1;
      PBUnlockRangeSync(&params);
    }
    if( res == noErr ){
      id->locked = lk;
      rc = SQLITE_OK;
    }else{
      rc = SQLITE_BUSY;
    }
  }
  return rc;
#endif
}

/*
** Change the lock status to be an exclusive or write lock.  Return
** SQLITE_OK on success and SQLITE_BUSY on a failure.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqliteOsWriteLock(OsFile *id){
#if OS_UNIX
  int rc;
  sqliteOsEnterMutex();
  if( id->pLock->cnt==0 || (id->pLock->cnt==1 && id->locked==1) ){
    struct flock lock;
    int s;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0L;
    s = fcntl(id->fd, F_SETLK, &lock);
    if( s!=0 ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }else{
      rc = SQLITE_OK;
      if( !id->locked ){
        id->pOpen->nLock++;
        id->locked = 1;
      }
      id->pLock->cnt = -1;
    }
  }else{
    rc = SQLITE_BUSY;
  }
  sqliteOsLeaveMutex();
  return rc;
#endif
#if OS_WIN
  int rc;
  if( id->locked<0 ){
    rc = SQLITE_OK;
  }else{
    int res;
    int cnt = 100;
    while( cnt-->0 && (res = LockFile(id->h, FIRST_LOCKBYTE, 0, 1, 0))==0 ){
      Sleep(1);
    }
    if( res ){
      if( id->locked>0 ){
        if( isNT() ){
          UnlockFile(id->h, FIRST_LOCKBYTE+1, 0, N_LOCKBYTE, 0);
        }else{
          res = UnlockFile(id->h, FIRST_LOCKBYTE + id->locked, 0, 1, 0);
        }
      }
      if( res ){
        res = LockFile(id->h, FIRST_LOCKBYTE+1, 0, N_LOCKBYTE, 0);
      }else{
        res = 0;
      }
      UnlockFile(id->h, FIRST_LOCKBYTE, 0, 1, 0);
    }
    if( res ){
      id->locked = -1;
      rc = SQLITE_OK;
    }else{
      rc = SQLITE_BUSY;
    }
  }
  return rc;
#endif
#if OS_MAC
  int rc;
  if( id->locked<0 || id->refNumRF == -1 ){
    rc = SQLITE_OK;
  }else{
    OSErr res;
    int cnt = 5;
    ParamBlockRec params;
    memset(&params, 0, sizeof(params));
    params.ioParam.ioRefNum = id->refNumRF;
    params.ioParam.ioPosMode = fsFromStart;
    params.ioParam.ioPosOffset = FIRST_LOCKBYTE;
    params.ioParam.ioReqCount = 1;
    while( cnt-->0 && (res = PBLockRangeSync(&params))!=noErr ){
      UInt32 finalTicks;
      Delay(1, &finalTicks); /* 1/60 sec */
    }
    if( res == noErr ){
      params.ioParam.ioPosOffset = FIRST_LOCKBYTE + id->locked;
      params.ioParam.ioReqCount = 1;
      if( id->locked==0 
            || PBUnlockRangeSync(&params)==noErr ){
        params.ioParam.ioPosOffset = FIRST_LOCKBYTE+1;
        params.ioParam.ioReqCount = N_LOCKBYTE;
        res = PBLockRangeSync(&params);
      }else{
        res = afpRangeNotLocked;
      }
      params.ioParam.ioPosOffset = FIRST_LOCKBYTE;
      params.ioParam.ioReqCount = 1;
      PBUnlockRangeSync(&params);
    }
    if( res == noErr ){
      id->locked = -1;
      rc = SQLITE_OK;
    }else{
      rc = SQLITE_BUSY;
    }
  }
  return rc;
#endif
}

/*
** Unlock the given file descriptor.  If the file descriptor was
** not previously locked, then this routine is a no-op.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqliteOsUnlock(OsFile *id){
#if OS_UNIX
  int rc;
  if( !id->locked ) return SQLITE_OK;
  sqliteOsEnterMutex();
  assert( id->pLock->cnt!=0 );
  if( id->pLock->cnt>1 ){
    id->pLock->cnt--;
    rc = SQLITE_OK;
  }else{
    struct flock lock;
    int s;
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = lock.l_len = 0L;
    s = fcntl(id->fd, F_SETLK, &lock);
    if( s!=0 ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }else{
      rc = SQLITE_OK;
      id->pLock->cnt = 0;
    }
  }
  if( rc==SQLITE_OK ){
    /* Decrement the count of locks against this same file.  When the
    ** count reaches zero, close any other file descriptors whose close
    ** was deferred because of outstanding locks.
    */
    struct openCnt *pOpen = id->pOpen;
    pOpen->nLock--;
    assert( pOpen->nLock>=0 );
    if( pOpen->nLock==0 && pOpen->nPending>0 ){
      int i;
      for(i=0; i<pOpen->nPending; i++){
        close(pOpen->aPending[i]);
      }
      sqliteFree(pOpen->aPending);
      pOpen->nPending = 0;
      pOpen->aPending = 0;
    }
  }
  sqliteOsLeaveMutex();
  id->locked = 0;
  return rc;
#endif
#if OS_WIN
  int rc;
  if( id->locked==0 ){
    rc = SQLITE_OK;
  }else if( isNT() || id->locked<0 ){
    UnlockFile(id->h, FIRST_LOCKBYTE+1, 0, N_LOCKBYTE, 0);
    rc = SQLITE_OK;
    id->locked = 0;
  }else{
    UnlockFile(id->h, FIRST_LOCKBYTE+id->locked, 0, 1, 0);
    rc = SQLITE_OK;
    id->locked = 0;
  }
  return rc;
#endif
#if OS_MAC
  int rc;
  ParamBlockRec params;
  memset(&params, 0, sizeof(params));
  params.ioParam.ioRefNum = id->refNumRF;
  params.ioParam.ioPosMode = fsFromStart;
  if( id->locked==0 || id->refNumRF == -1 ){
    rc = SQLITE_OK;
  }else if( id->locked<0 ){
    params.ioParam.ioPosOffset = FIRST_LOCKBYTE+1;
    params.ioParam.ioReqCount = N_LOCKBYTE;
    PBUnlockRangeSync(&params);
    rc = SQLITE_OK;
    id->locked = 0;
  }else{
    params.ioParam.ioPosOffset = FIRST_LOCKBYTE+id->locked;
    params.ioParam.ioReqCount = 1;
    PBUnlockRangeSync(&params);
    rc = SQLITE_OK;
    id->locked = 0;
  }
  return rc;
#endif
}

/*
** Get information to seed the random number generator.  The seed
** is written into the buffer zBuf[256].  The calling function must
** supply a sufficiently large buffer.
*/
int sqliteOsRandomSeed(char *zBuf){
  /* We have to initialize zBuf to prevent valgrind from reporting
  ** errors.  The reports issued by valgrind are incorrect - we would
  ** prefer that the randomness be increased by making use of the
  ** uninitialized space in zBuf - but valgrind errors tend to worry
  ** some users.  Rather than argue, it seems easier just to initialize
  ** the whole array and silence valgrind, even if that means less randomness
  ** in the random seed.
  **
  ** When testing, initializing zBuf[] to zero is all we do.  That means
  ** that we always use the same random number sequence.* This makes the
  ** tests repeatable.
  */
  memset(zBuf, 0, 256);
#if OS_UNIX && !defined(SQLITE_TEST)
  {
    int pid;
    time((time_t*)zBuf);
    pid = getpid();
    memcpy(&zBuf[sizeof(time_t)], &pid, sizeof(pid));
  }
#endif
#if OS_WIN && !defined(SQLITE_TEST)
  GetSystemTime((LPSYSTEMTIME)zBuf);
#endif
#if OS_MAC
  {
    int pid;
    Microseconds((UnsignedWide*)zBuf);
    pid = getpid();
    memcpy(&zBuf[sizeof(UnsignedWide)], &pid, sizeof(pid));
  }
#endif
  return SQLITE_OK;
}

/*
** Sleep for a little while.  Return the amount of time slept.
*/
int sqliteOsSleep(int ms){
#if OS_UNIX
#if defined(HAVE_USLEEP) && HAVE_USLEEP
  usleep(ms*1000);
  return ms;
#else
  sleep((ms+999)/1000);
  return 1000*((ms+999)/1000);
#endif
#endif
#if OS_WIN
  Sleep(ms);
  return ms;
#endif
#if OS_MAC
  UInt32 finalTicks;
  UInt32 ticks = (((UInt32)ms+16)*3)/50;  /* 1/60 sec per tick */
  Delay(ticks, &finalTicks);
  return (int)((ticks*50)/3);
#endif
}

/*
** Static variables used for thread synchronization
*/
static int inMutex = 0;
#ifdef SQLITE_UNIX_THREADS
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
#ifdef SQLITE_W32_THREADS
  static CRITICAL_SECTION cs;
#endif
#ifdef SQLITE_MACOS_MULTITASKING
  static MPCriticalRegionID criticalRegion;
#endif

/*
** The following pair of routine implement mutual exclusion for
** multi-threaded processes.  Only a single thread is allowed to
** executed code that is surrounded by EnterMutex() and LeaveMutex().
**
** SQLite uses only a single Mutex.  There is not much critical
** code and what little there is executes quickly and without blocking.
*/
void sqliteOsEnterMutex(){
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_lock(&mutex);
#endif
#ifdef SQLITE_W32_THREADS
  static int isInit = 0;
  while( !isInit ){
    static long lock = 0;
    if( InterlockedIncrement(&lock)==1 ){
      InitializeCriticalSection(&cs);
      isInit = 1;
    }else{
      Sleep(1);
    }
  }
  EnterCriticalSection(&cs);
#endif
#ifdef SQLITE_MACOS_MULTITASKING
  static volatile int notInit = 1;
  if( notInit ){
    if( notInit == 2 ) /* as close as you can get to thread safe init */
      MPYield();
    else{
      notInit = 2;
      MPCreateCriticalRegion(&criticalRegion);
      notInit = 0;
    }
  }
  MPEnterCriticalRegion(criticalRegion, kDurationForever);
#endif
  assert( !inMutex );
  inMutex = 1;
}
void sqliteOsLeaveMutex(){
  assert( inMutex );
  inMutex = 0;
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_unlock(&mutex);
#endif
#ifdef SQLITE_W32_THREADS
  LeaveCriticalSection(&cs);
#endif
#ifdef SQLITE_MACOS_MULTITASKING
  MPExitCriticalRegion(criticalRegion);
#endif
}

/*
** Turn a relative pathname into a full pathname.  Return a pointer
** to the full pathname stored in space obtained from sqliteMalloc().
** The calling function is responsible for freeing this space once it
** is no longer needed.
*/
char *sqliteOsFullPathname(const char *zRelative){
#if OS_UNIX
  char *zFull = 0;
  if( zRelative[0]=='/' ){
    sqliteSetString(&zFull, zRelative, (char*)0);
  }else{
    char zBuf[5000];
    zBuf[0] = 0;
    sqliteSetString(&zFull, getcwd(zBuf, sizeof(zBuf)), "/", zRelative,
                    (char*)0);
  }
  return zFull;
#endif
#if OS_WIN
  char *zNotUsed;
  char *zFull;
  int nByte;
  nByte = GetFullPathName(zRelative, 0, 0, &zNotUsed) + 1;
  zFull = sqliteMalloc( nByte );
  if( zFull==0 ) return 0;
  GetFullPathName(zRelative, nByte, zFull, &zNotUsed);
  return zFull;
#endif
#if OS_MAC
  char *zFull = 0;
  if( zRelative[0]==':' ){
    char zBuf[_MAX_PATH+1];
    sqliteSetString(&zFull, getcwd(zBuf, sizeof(zBuf)), &(zRelative[1]),
                    (char*)0);
  }else{
    if( strchr(zRelative, ':') ){
      sqliteSetString(&zFull, zRelative, (char*)0);
    }else{
    char zBuf[_MAX_PATH+1];
      sqliteSetString(&zFull, getcwd(zBuf, sizeof(zBuf)), zRelative, (char*)0);
    }
  }
  return zFull;
#endif
}

/*
** The following variable, if set to a non-zero value, becomes the result
** returned from sqliteOsCurrentTime().  This is used for testing.
*/
#ifdef SQLITE_TEST
int sqlite_current_time = 0;
#endif

/*
** Find the current time (in Universal Coordinated Time).  Write the
** current time and date as a Julian Day number into *prNow and
** return 0.  Return 1 if the time and date cannot be found.
*/
int sqliteOsCurrentTime(double *prNow){
#if OS_UNIX
  time_t t;
  time(&t);
  *prNow = t/86400.0 + 2440587.5;
#endif
#if OS_WIN
  FILETIME ft;
  /* FILETIME structure is a 64-bit value representing the number of 
     100-nanosecond intervals since January 1, 1601 (= JD 2305813.5). 
  */
  double now;
  GetSystemTimeAsFileTime( &ft );
  now = ((double)ft.dwHighDateTime) * 4294967296.0; 
  *prNow = (now + ft.dwLowDateTime)/864000000000.0 + 2305813.5;
#endif
#ifdef SQLITE_TEST
  if( sqlite_current_time ){
    *prNow = sqlite_current_time/86400.0 + 2440587.5;
  }
#endif
  return 0;
}
