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
** This file contains code that is specific to Unix systems.
*/
#include "sqliteInt.h"
#include "os.h"
#if OS_UNIX              /* This file is used on unix only */

/* #define SQLITE_ENABLE_LOCKING_STYLE 0 */

/*
** These #defines should enable >2GB file support on Posix if the
** underlying operating system supports it.  If the OS lacks
** large file support, these should be no-ops.
**
** Large file support can be disabled using the -DSQLITE_DISABLE_LFS switch
** on the compiler command line.  This is necessary if you are compiling
** on a recent machine (ex: RedHat 7.2) but you want your code to work
** on an older machine (ex: RedHat 6.0).  If you compile on RedHat 7.2
** without this option, LFS is enable.  But LFS does not exist in the kernel
** in RedHat 6.0, so the code won't work.  Hence, for maximum binary
** portability you should omit LFS.
*/
#ifndef SQLITE_DISABLE_LFS
# define _LARGE_FILE       1
# ifndef _FILE_OFFSET_BITS
#   define _FILE_OFFSET_BITS 64
# endif
# define _LARGEFILE_SOURCE 1
#endif

/*
** standard include files.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#ifdef SQLITE_ENABLE_LOCKING_STYLE
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/mount.h>
#endif /* SQLITE_ENABLE_LOCKING_STYLE */

/*
** If we are to be thread-safe, include the pthreads header and define
** the SQLITE_UNIX_THREADS macro.
*/
#ifndef THREADSAFE
# define THREADSAFE 1
#endif
#if THREADSAFE
# include <pthread.h>
# define SQLITE_UNIX_THREADS 1
#endif

/*
** Default permissions when creating a new file
*/
#ifndef SQLITE_DEFAULT_FILE_PERMISSIONS
# define SQLITE_DEFAULT_FILE_PERMISSIONS 0644
#endif



/*
** The unixFile structure is subclass of OsFile specific for the unix
** protability layer.
*/
typedef struct unixFile unixFile;
struct unixFile {
  IoMethod const *pMethod;  /* Always the first entry */
  struct openCnt *pOpen;    /* Info about all open fd's on this inode */
  struct lockInfo *pLock;   /* Info about locks on this inode */
#ifdef SQLITE_ENABLE_LOCKING_STYLE
  void *lockingContext;     /* Locking style specific state */
#endif /* SQLITE_ENABLE_LOCKING_STYLE */
  int h;                    /* The file descriptor */
  unsigned char locktype;   /* The type of lock held on this fd */
  unsigned char isOpen;     /* True if needs to be closed */
  unsigned char fullSync;   /* Use F_FULLSYNC if available */
  int dirfd;                /* File descriptor for the directory */
  i64 offset;               /* Seek offset */
#ifdef SQLITE_UNIX_THREADS
  pthread_t tid;            /* The thread that "owns" this OsFile */
#endif
};

/*
** Provide the ability to override some OS-layer functions during
** testing.  This is used to simulate OS crashes to verify that 
** commits are atomic even in the event of an OS crash.
*/
#ifdef SQLITE_CRASH_TEST
  extern int sqlite3CrashTestEnable;
  extern int sqlite3CrashOpenReadWrite(const char*, OsFile**, int*);
  extern int sqlite3CrashOpenExclusive(const char*, OsFile**, int);
  extern int sqlite3CrashOpenReadOnly(const char*, OsFile**, int);
# define CRASH_TEST_OVERRIDE(X,A,B,C) \
    if(sqlite3CrashTestEnable){ return X(A,B,C); }
#else
# define CRASH_TEST_OVERRIDE(X,A,B,C)  /* no-op */
#endif


/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

/*
** Do not include any of the File I/O interface procedures if the
** SQLITE_OMIT_DISKIO macro is defined (indicating that the database
** will be in-memory only)
*/
#ifndef SQLITE_OMIT_DISKIO


/*
** Define various macros that are missing from some systems.
*/
#ifndef O_LARGEFILE
# define O_LARGEFILE 0
#endif
#ifdef SQLITE_DISABLE_LFS
# undef O_LARGEFILE
# define O_LARGEFILE 0
#endif
#ifndef O_NOFOLLOW
# define O_NOFOLLOW 0
#endif
#ifndef O_BINARY
# define O_BINARY 0
#endif

/*
** The DJGPP compiler environment looks mostly like Unix, but it
** lacks the fcntl() system call.  So redefine fcntl() to be something
** that always succeeds.  This means that locking does not occur under
** DJGPP.  But it's DOS - what did you expect?
*/
#ifdef __DJGPP__
# define fcntl(A,B,C) 0
#endif

/*
** The threadid macro resolves to the thread-id or to 0.  Used for
** testing and debugging only.
*/
#ifdef SQLITE_UNIX_THREADS
#define threadid pthread_self()
#else
#define threadid 0
#endif

/*
** Set or check the OsFile.tid field.  This field is set when an OsFile
** is first opened.  All subsequent uses of the OsFile verify that the
** same thread is operating on the OsFile.  Some operating systems do
** not allow locks to be overridden by other threads and that restriction
** means that sqlite3* database handles cannot be moved from one thread
** to another.  This logic makes sure a user does not try to do that
** by mistake.
**
** Version 3.3.1 (2006-01-15):  OsFiles can be moved from one thread to
** another as long as we are running on a system that supports threads
** overriding each others locks (which now the most common behavior)
** or if no locks are held.  But the OsFile.pLock field needs to be
** recomputed because its key includes the thread-id.  See the 
** transferOwnership() function below for additional information
*/
#if defined(SQLITE_UNIX_THREADS)
# define SET_THREADID(X)   (X)->tid = pthread_self()
# define CHECK_THREADID(X) (threadsOverrideEachOthersLocks==0 && \
                            !pthread_equal((X)->tid, pthread_self()))
#else
# define SET_THREADID(X)
# define CHECK_THREADID(X) 0
#endif

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
**
** 2004-Jun-28:
** On some versions of linux, threads can override each others locks.
** On others not.  Sometimes you can change the behavior on the same
** system by setting the LD_ASSUME_KERNEL environment variable.  The
** POSIX standard is silent as to which behavior is correct, as far
** as I can tell, so other versions of unix might show the same
** inconsistency.  There is no little doubt in my mind that posix
** advisory locks and linux threads are profoundly broken.
**
** To work around the inconsistencies, we have to test at runtime 
** whether or not threads can override each others locks.  This test
** is run once, the first time any lock is attempted.  A static 
** variable is set to record the results of this test for future
** use.
*/

/*
** An instance of the following structure serves as the key used
** to locate a particular lockInfo structure given its inode.
**
** If threads cannot override each others locks, then we set the
** lockKey.tid field to the thread ID.  If threads can override
** each others locks then tid is always set to zero.  tid is omitted
** if we compile without threading support.
*/
struct lockKey {
  dev_t dev;       /* Device number */
  ino_t ino;       /* Inode number */
#ifdef SQLITE_UNIX_THREADS
  pthread_t tid;   /* Thread ID or zero if threads can override each other */
#endif
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
  int cnt;             /* Number of SHARED locks held */
  int locktype;        /* One of SHARED_LOCK, RESERVED_LOCK etc. */
  int nRef;            /* Number of pointers to this structure */
};

/*
** An instance of the following structure serves as the key used
** to locate a particular openCnt structure given its inode.  This
** is the same as the lockKey except that the thread ID is omitted.
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
** These hash tables map inodes and file descriptors (really, lockKey and
** openKey structures) into lockInfo and openCnt structures.  Access to 
** these hash tables must be protected by a mutex.
*/
static Hash lockHash = {SQLITE_HASH_BINARY, 0, 0, 0, 
    sqlite3ThreadSafeMalloc, sqlite3ThreadSafeFree, 0, 0};
static Hash openHash = {SQLITE_HASH_BINARY, 0, 0, 0, 
    sqlite3ThreadSafeMalloc, sqlite3ThreadSafeFree, 0, 0};

#ifdef SQLITE_ENABLE_LOCKING_STYLE
/*
** The locking styles are associated with the different file locking
** capabilities supported by different file systems.  
**
** POSIX locking style fully supports shared and exclusive byte-range locks 
** ADP locking only supports exclusive byte-range locks
** FLOCK only supports a single file-global exclusive lock
** DOTLOCK isn't a true locking style, it refers to the use of a special
**   file named the same as the database file with a '.lock' extension, this
**   can be used on file systems that do not offer any reliable file locking
** NO locking means that no locking will be attempted, this is only used for
**   read-only file systems currently
** UNSUPPORTED means that no locking will be attempted, this is only used for
**   file systems that are known to be unsupported
*/
typedef enum {
	posixLockingStyle = 0,       /* standard posix-advisory locks */
	afpLockingStyle,             /* use afp locks */
	flockLockingStyle,           /* use flock() */
	dotlockLockingStyle,         /* use <file>.lock files */
	noLockingStyle,              /* useful for read-only file system */
	unsupportedLockingStyle      /* indicates unsupported file system */
} sqlite3LockingStyle;
#endif /* SQLITE_ENABLE_LOCKING_STYLE */

#ifdef SQLITE_UNIX_THREADS
/*
** This variable records whether or not threads can override each others
** locks.
**
**    0:  No.  Threads cannot override each others locks.
**    1:  Yes.  Threads can override each others locks.
**   -1:  We don't know yet.
**
** On some systems, we know at compile-time if threads can override each
** others locks.  On those systems, the SQLITE_THREAD_OVERRIDE_LOCK macro
** will be set appropriately.  On other systems, we have to check at
** runtime.  On these latter systems, SQLTIE_THREAD_OVERRIDE_LOCK is
** undefined.
**
** This variable normally has file scope only.  But during testing, we make
** it a global so that the test code can change its value in order to verify
** that the right stuff happens in either case.
*/
#ifndef SQLITE_THREAD_OVERRIDE_LOCK
# define SQLITE_THREAD_OVERRIDE_LOCK -1
#endif
#ifdef SQLITE_TEST
int threadsOverrideEachOthersLocks = SQLITE_THREAD_OVERRIDE_LOCK;
#else
static int threadsOverrideEachOthersLocks = SQLITE_THREAD_OVERRIDE_LOCK;
#endif

/*
** This structure holds information passed into individual test
** threads by the testThreadLockingBehavior() routine.
*/
struct threadTestData {
  int fd;                /* File to be locked */
  struct flock lock;     /* The locking operation */
  int result;            /* Result of the locking operation */
};

#ifdef SQLITE_LOCK_TRACE
/*
** Print out information about all locking operations.
**
** This routine is used for troubleshooting locks on multithreaded
** platforms.  Enable by compiling with the -DSQLITE_LOCK_TRACE
** command-line option on the compiler.  This code is normally
** turned off.
*/
static int lockTrace(int fd, int op, struct flock *p){
  char *zOpName, *zType;
  int s;
  int savedErrno;
  if( op==F_GETLK ){
    zOpName = "GETLK";
  }else if( op==F_SETLK ){
    zOpName = "SETLK";
  }else{
    s = fcntl(fd, op, p);
    sqlite3DebugPrintf("fcntl unknown %d %d %d\n", fd, op, s);
    return s;
  }
  if( p->l_type==F_RDLCK ){
    zType = "RDLCK";
  }else if( p->l_type==F_WRLCK ){
    zType = "WRLCK";
  }else if( p->l_type==F_UNLCK ){
    zType = "UNLCK";
  }else{
    assert( 0 );
  }
  assert( p->l_whence==SEEK_SET );
  s = fcntl(fd, op, p);
  savedErrno = errno;
  sqlite3DebugPrintf("fcntl %d %d %s %s %d %d %d %d\n",
     threadid, fd, zOpName, zType, (int)p->l_start, (int)p->l_len,
     (int)p->l_pid, s);
  if( s==(-1) && op==F_SETLK && (p->l_type==F_RDLCK || p->l_type==F_WRLCK) ){
    struct flock l2;
    l2 = *p;
    fcntl(fd, F_GETLK, &l2);
    if( l2.l_type==F_RDLCK ){
      zType = "RDLCK";
    }else if( l2.l_type==F_WRLCK ){
      zType = "WRLCK";
    }else if( l2.l_type==F_UNLCK ){
      zType = "UNLCK";
    }else{
      assert( 0 );
    }
    sqlite3DebugPrintf("fcntl-failure-reason: %s %d %d %d\n",
       zType, (int)l2.l_start, (int)l2.l_len, (int)l2.l_pid);
  }
  errno = savedErrno;
  return s;
}
#define fcntl lockTrace
#endif /* SQLITE_LOCK_TRACE */

/*
** The testThreadLockingBehavior() routine launches two separate
** threads on this routine.  This routine attempts to lock a file
** descriptor then returns.  The success or failure of that attempt
** allows the testThreadLockingBehavior() procedure to determine
** whether or not threads can override each others locks.
*/
static void *threadLockingTest(void *pArg){
  struct threadTestData *pData = (struct threadTestData*)pArg;
  pData->result = fcntl(pData->fd, F_SETLK, &pData->lock);
  return pArg;
}

/*
** This procedure attempts to determine whether or not threads
** can override each others locks then sets the 
** threadsOverrideEachOthersLocks variable appropriately.
*/
static void testThreadLockingBehavior(int fd_orig){
  int fd;
  struct threadTestData d[2];
  pthread_t t[2];

  fd = dup(fd_orig);
  if( fd<0 ) return;
  memset(d, 0, sizeof(d));
  d[0].fd = fd;
  d[0].lock.l_type = F_RDLCK;
  d[0].lock.l_len = 1;
  d[0].lock.l_start = 0;
  d[0].lock.l_whence = SEEK_SET;
  d[1] = d[0];
  d[1].lock.l_type = F_WRLCK;
  pthread_create(&t[0], 0, threadLockingTest, &d[0]);
  pthread_create(&t[1], 0, threadLockingTest, &d[1]);
  pthread_join(t[0], 0);
  pthread_join(t[1], 0);
  close(fd);
  threadsOverrideEachOthersLocks =  d[0].result==0 && d[1].result==0;
}
#endif /* SQLITE_UNIX_THREADS */

/*
** Release a lockInfo structure previously allocated by findLockInfo().
*/
static void releaseLockInfo(struct lockInfo *pLock){
  assert( sqlite3OsInMutex(1) );
  if (pLock == NULL)
    return;
  pLock->nRef--;
  if( pLock->nRef==0 ){
    sqlite3HashInsert(&lockHash, &pLock->key, sizeof(pLock->key), 0);
    sqlite3ThreadSafeFree(pLock);
  }
}

/*
** Release a openCnt structure previously allocated by findLockInfo().
*/
static void releaseOpenCnt(struct openCnt *pOpen){
  assert( sqlite3OsInMutex(1) );
  if (pOpen == NULL)
    return;
  pOpen->nRef--;
  if( pOpen->nRef==0 ){
    sqlite3HashInsert(&openHash, &pOpen->key, sizeof(pOpen->key), 0);
    free(pOpen->aPending);
    sqlite3ThreadSafeFree(pOpen);
  }
}

#ifdef SQLITE_ENABLE_LOCKING_STYLE
/*
** Tests a byte-range locking query to see if byte range locks are 
** supported, if not we fall back to dotlockLockingStyle.
*/
static sqlite3LockingStyle sqlite3TestLockingStyle(const char *filePath, 
  int fd) {
  /* test byte-range lock using fcntl */
  struct flock lockInfo;
  
  lockInfo.l_len = 1;
  lockInfo.l_start = 0;
  lockInfo.l_whence = SEEK_SET;
  lockInfo.l_type = F_RDLCK;
  
  if (fcntl(fd, F_GETLK, &lockInfo) != -1) {
    return posixLockingStyle;
  } 
  
  /* testing for flock can give false positives.  So if if the above test
  ** fails, then we fall back to using dot-lock style locking.
  */  
  return dotlockLockingStyle;
}

/* 
** Examines the f_fstypename entry in the statfs structure as returned by 
** stat() for the file system hosting the database file, assigns the 
** appropriate locking style based on it's value.  These values and 
** assignments are based on Darwin/OSX behavior and have not been tested on 
** other systems.
*/
static sqlite3LockingStyle sqlite3DetectLockingStyle(const char *filePath, 
  int fd) {

#ifdef SQLITE_FIXED_LOCKING_STYLE
  return (sqlite3LockingStyle)SQLITE_FIXED_LOCKING_STYLE;
#else
  struct statfs fsInfo;

  if (statfs(filePath, &fsInfo) == -1)
    return sqlite3TestLockingStyle(filePath, fd);
  
  if (fsInfo.f_flags & MNT_RDONLY)
    return noLockingStyle;
  
  if( (!strcmp(fsInfo.f_fstypename, "hfs")) ||
    (!strcmp(fsInfo.f_fstypename, "ufs")) )
		return posixLockingStyle;
  
  if(!strcmp(fsInfo.f_fstypename, "afpfs"))
    return afpLockingStyle;
  
  if(!strcmp(fsInfo.f_fstypename, "nfs")) 
    return sqlite3TestLockingStyle(filePath, fd);
  
  if(!strcmp(fsInfo.f_fstypename, "smbfs"))
    return flockLockingStyle;
  
  if(!strcmp(fsInfo.f_fstypename, "msdos"))
    return dotlockLockingStyle;
  
  if(!strcmp(fsInfo.f_fstypename, "webdav"))
    return unsupportedLockingStyle;
  
  return sqlite3TestLockingStyle(filePath, fd);  
#endif // SQLITE_FIXED_LOCKING_STYLE
}

#endif /* SQLITE_ENABLE_LOCKING_STYLE */

/*
** Given a file descriptor, locate lockInfo and openCnt structures that
** describes that file descriptor.  Create new ones if necessary.  The
** return values might be uninitialized if an error occurs.
**
** Return the number of errors.
*/
static int findLockInfo(
  int fd,                      /* The file descriptor used in the key */
  struct lockInfo **ppLock,    /* Return the lockInfo structure here */
  struct openCnt **ppOpen      /* Return the openCnt structure here */
){
  int rc;
  struct lockKey key1;
  struct openKey key2;
  struct stat statbuf;
  struct lockInfo *pLock;
  struct openCnt *pOpen;
  rc = fstat(fd, &statbuf);
  if( rc!=0 ) return 1;

  assert( sqlite3OsInMutex(1) );
  memset(&key1, 0, sizeof(key1));
  key1.dev = statbuf.st_dev;
  key1.ino = statbuf.st_ino;
#ifdef SQLITE_UNIX_THREADS
  if( threadsOverrideEachOthersLocks<0 ){
    testThreadLockingBehavior(fd);
  }
  key1.tid = threadsOverrideEachOthersLocks ? 0 : pthread_self();
#endif
  memset(&key2, 0, sizeof(key2));
  key2.dev = statbuf.st_dev;
  key2.ino = statbuf.st_ino;
  pLock = (struct lockInfo*)sqlite3HashFind(&lockHash, &key1, sizeof(key1));
  if( pLock==0 ){
    struct lockInfo *pOld;
    pLock = sqlite3ThreadSafeMalloc( sizeof(*pLock) );
    if( pLock==0 ){
      rc = 1;
      goto exit_findlockinfo;
    }
    pLock->key = key1;
    pLock->nRef = 1;
    pLock->cnt = 0;
    pLock->locktype = 0;
    pOld = sqlite3HashInsert(&lockHash, &pLock->key, sizeof(key1), pLock);
    if( pOld!=0 ){
      assert( pOld==pLock );
      sqlite3ThreadSafeFree(pLock);
      rc = 1;
      goto exit_findlockinfo;
    }
  }else{
    pLock->nRef++;
  }
  *ppLock = pLock;
  if( ppOpen!=0 ){
    pOpen = (struct openCnt*)sqlite3HashFind(&openHash, &key2, sizeof(key2));
    if( pOpen==0 ){
      struct openCnt *pOld;
      pOpen = sqlite3ThreadSafeMalloc( sizeof(*pOpen) );
      if( pOpen==0 ){
        releaseLockInfo(pLock);
        rc = 1;
        goto exit_findlockinfo;
      }
      pOpen->key = key2;
      pOpen->nRef = 1;
      pOpen->nLock = 0;
      pOpen->nPending = 0;
      pOpen->aPending = 0;
      pOld = sqlite3HashInsert(&openHash, &pOpen->key, sizeof(key2), pOpen);
      if( pOld!=0 ){
        assert( pOld==pOpen );
        sqlite3ThreadSafeFree(pOpen);
        releaseLockInfo(pLock);
        rc = 1;
        goto exit_findlockinfo;
      }
    }else{
      pOpen->nRef++;
    }
    *ppOpen = pOpen;
  }

exit_findlockinfo:
  return rc;
}

#ifdef SQLITE_DEBUG
/*
** Helper function for printing out trace information from debugging
** binaries. This returns the string represetation of the supplied
** integer lock-type.
*/
static const char *locktypeName(int locktype){
  switch( locktype ){
  case NO_LOCK: return "NONE";
  case SHARED_LOCK: return "SHARED";
  case RESERVED_LOCK: return "RESERVED";
  case PENDING_LOCK: return "PENDING";
  case EXCLUSIVE_LOCK: return "EXCLUSIVE";
  }
  return "ERROR";
}
#endif

/*
** If we are currently in a different thread than the thread that the
** unixFile argument belongs to, then transfer ownership of the unixFile
** over to the current thread.
**
** A unixFile is only owned by a thread on systems where one thread is
** unable to override locks created by a different thread.  RedHat9 is
** an example of such a system.
**
** Ownership transfer is only allowed if the unixFile is currently unlocked.
** If the unixFile is locked and an ownership is wrong, then return
** SQLITE_MISUSE.  SQLITE_OK is returned if everything works.
*/
#ifdef SQLITE_UNIX_THREADS
static int transferOwnership(unixFile *pFile){
  int rc;
  pthread_t hSelf;
  if( threadsOverrideEachOthersLocks ){
    /* Ownership transfers not needed on this system */
    return SQLITE_OK;
  }
  hSelf = pthread_self();
  if( pthread_equal(pFile->tid, hSelf) ){
    /* We are still in the same thread */
    OSTRACE1("No-transfer, same thread\n");
    return SQLITE_OK;
  }
  if( pFile->locktype!=NO_LOCK ){
    /* We cannot change ownership while we are holding a lock! */
    return SQLITE_MISUSE;
  }
  OSTRACE4("Transfer ownership of %d from %d to %d\n",
            pFile->h, pFile->tid, hSelf);
  pFile->tid = hSelf;
  if (pFile->pLock != NULL) {
    releaseLockInfo(pFile->pLock);
    rc = findLockInfo(pFile->h, &pFile->pLock, 0);
    OSTRACE5("LOCK    %d is now %s(%s,%d)\n", pFile->h,
           locktypeName(pFile->locktype),
           locktypeName(pFile->pLock->locktype), pFile->pLock->cnt);
    return rc;
  } else {
    return SQLITE_OK;
  }
}
#else
  /* On single-threaded builds, ownership transfer is a no-op */
# define transferOwnership(X) SQLITE_OK
#endif

/*
** Delete the named file
*/
int sqlite3UnixDelete(const char *zFilename){
  SimulateIOError(return SQLITE_IOERR_DELETE);
  unlink(zFilename);
  return SQLITE_OK;
}

/*
** Return TRUE if the named file exists.
*/
int sqlite3UnixFileExists(const char *zFilename){
  return access(zFilename, 0)==0;
}

/* Forward declaration */
static int allocateUnixFile(
  int h,                    /* File descriptor of the open file */
  OsFile **pId,             /* Write the real file descriptor here */
  const char *zFilename,    /* Name of the file being opened */
  int delFlag               /* If true, make sure the file deletes on close */
);

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
int sqlite3UnixOpenReadWrite(
  const char *zFilename,
  OsFile **pId,
  int *pReadonly
){
  int h;
  
  CRASH_TEST_OVERRIDE(sqlite3CrashOpenReadWrite, zFilename, pId, pReadonly);
  assert( 0==*pId );
  h = open(zFilename, O_RDWR|O_CREAT|O_LARGEFILE|O_BINARY,
                        SQLITE_DEFAULT_FILE_PERMISSIONS);
  if( h<0 ){
#ifdef EISDIR
    if( errno==EISDIR ){
      return SQLITE_CANTOPEN;
    }
#endif
    h = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
    if( h<0 ){
      return SQLITE_CANTOPEN; 
    }
    *pReadonly = 1;
  }else{
    *pReadonly = 0;
  }
  return allocateUnixFile(h, pId, zFilename, 0);
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
int sqlite3UnixOpenExclusive(const char *zFilename, OsFile **pId, int delFlag){
  int h;

  CRASH_TEST_OVERRIDE(sqlite3CrashOpenExclusive, zFilename, pId, delFlag);
  assert( 0==*pId );
  h = open(zFilename,
                O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_LARGEFILE|O_BINARY,
                delFlag ? 0600 : SQLITE_DEFAULT_FILE_PERMISSIONS);
  if( h<0 ){
    return SQLITE_CANTOPEN;
  }
  return allocateUnixFile(h, pId, zFilename, delFlag);
}

/*
** Attempt to open a new file for read-only access.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
*/
int sqlite3UnixOpenReadOnly(const char *zFilename, OsFile **pId){
  int h;
  
  CRASH_TEST_OVERRIDE(sqlite3CrashOpenReadOnly, zFilename, pId, 0);
  assert( 0==*pId );
  h = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
  if( h<0 ){
    return SQLITE_CANTOPEN;
  }
  return allocateUnixFile(h, pId, zFilename, 0);
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
** If FULL_FSYNC is enabled, this function is not longer useful, 
** a FULL_FSYNC sync applies to all pending disk operations.
**
** On success, a handle for a previously open file at *id is
** updated with the new directory file descriptor and SQLITE_OK is
** returned.
**
** On failure, the function returns SQLITE_CANTOPEN and leaves
** *id unchanged.
*/
static int unixOpenDirectory(
  OsFile *id,
  const char *zDirname
){
  unixFile *pFile = (unixFile*)id;
  assert( pFile!=0 );
  SET_THREADID(pFile);
  assert( pFile->dirfd<0 );
  pFile->dirfd = open(zDirname, O_RDONLY|O_BINARY, 0);
  if( pFile->dirfd<0 ){
    return SQLITE_CANTOPEN; 
  }
  OSTRACE3("OPENDIR %-3d %s\n", pFile->dirfd, zDirname);
  return SQLITE_OK;
}

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at least SQLITE_TEMPNAME_SIZE characters.
*/
int sqlite3UnixTempFileName(char *zBuf){
  static const char *azDirs[] = {
     0,
     "/var/tmp",
     "/usr/tmp",
     "/tmp",
     ".",
  };
  static const unsigned char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  struct stat buf;
  const char *zDir = ".";
  azDirs[0] = sqlite3_temp_directory;
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
    sqlite3Randomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
  }while( access(zBuf,0)==0 );
  return SQLITE_OK; 
}

/*
** Check that a given pathname is a directory and is writable 
**
*/
int sqlite3UnixIsDirWritable(char *zBuf){
#ifndef SQLITE_OMIT_PAGER_PRAGMAS
  struct stat buf;
  if( zBuf==0 ) return 0;
  if( zBuf[0]==0 ) return 0;
  if( stat(zBuf, &buf) ) return 0;
  if( !S_ISDIR(buf.st_mode) ) return 0;
  if( access(zBuf, 07) ) return 0;
#endif /* SQLITE_OMIT_PAGER_PRAGMAS */
  return 1;
}

/*
** Seek to the offset in id->offset then read cnt bytes into pBuf.
** Return the number of bytes actually read.  Update the offset.
*/
static int seekAndRead(unixFile *id, void *pBuf, int cnt){
  int got;
  i64 newOffset;
  TIMER_START;
#if defined(USE_PREAD)
  got = pread(id->h, pBuf, cnt, id->offset);
  SimulateIOError( got = -1 );
#elif defined(USE_PREAD64)
  got = pread64(id->h, pBuf, cnt, id->offset);
  SimulateIOError( got = -1 );
#else
  newOffset = lseek(id->h, id->offset, SEEK_SET);
  SimulateIOError( newOffset-- );
  if( newOffset!=id->offset ){
    return -1;
  }
  got = read(id->h, pBuf, cnt);
#endif
  TIMER_END;
  OSTRACE5("READ    %-3d %5d %7lld %d\n", id->h, got, id->offset, TIMER_ELAPSED);
  if( got>0 ){
    id->offset += got;
  }
  return got;
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int unixRead(OsFile *id, void *pBuf, int amt){
  int got;
  assert( id );
  got = seekAndRead((unixFile*)id, pBuf, amt);
  if( got==amt ){
    return SQLITE_OK;
  }else if( got<0 ){
    return SQLITE_IOERR_READ;
  }else{
    memset(&((char*)pBuf)[got], 0, amt-got);
    return SQLITE_IOERR_SHORT_READ;
  }
}

/*
** Seek to the offset in id->offset then read cnt bytes into pBuf.
** Return the number of bytes actually read.  Update the offset.
*/
static int seekAndWrite(unixFile *id, const void *pBuf, int cnt){
  int got;
  i64 newOffset;
  TIMER_START;
#if defined(USE_PREAD)
  got = pwrite(id->h, pBuf, cnt, id->offset);
#elif defined(USE_PREAD64)
  got = pwrite64(id->h, pBuf, cnt, id->offset);
#else
  newOffset = lseek(id->h, id->offset, SEEK_SET);
  if( newOffset!=id->offset ){
    return -1;
  }
  got = write(id->h, pBuf, cnt);
#endif
  TIMER_END;
  OSTRACE5("WRITE   %-3d %5d %7lld %d\n", id->h, got, id->offset, TIMER_ELAPSED);
  if( got>0 ){
    id->offset += got;
  }
  return got;
}


/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int unixWrite(OsFile *id, const void *pBuf, int amt){
  int wrote = 0;
  assert( id );
  assert( amt>0 );
  while( amt>0 && (wrote = seekAndWrite((unixFile*)id, pBuf, amt))>0 ){
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  SimulateIOError(( wrote=(-1), amt=1 ));
  SimulateDiskfullError(( wrote=0, amt=1 ));
  if( amt>0 ){
    if( wrote<0 ){
      return SQLITE_IOERR_WRITE;
    }else{
      return SQLITE_FULL;
    }
  }
  return SQLITE_OK;
}

/*
** Move the read/write pointer in a file.
*/
static int unixSeek(OsFile *id, i64 offset){
  assert( id );
#ifdef SQLITE_TEST
  if( offset ) SimulateDiskfullError(return SQLITE_FULL);
#endif
  ((unixFile*)id)->offset = offset;
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
** Use the fdatasync() API only if the HAVE_FDATASYNC macro is defined.
** Otherwise use fsync() in its place.
*/
#ifndef HAVE_FDATASYNC
# define fdatasync fsync
#endif

/*
** Define HAVE_FULLFSYNC to 0 or 1 depending on whether or not
** the F_FULLFSYNC macro is defined.  F_FULLFSYNC is currently
** only available on Mac OS X.  But that could change.
*/
#ifdef F_FULLFSYNC
# define HAVE_FULLFSYNC 1
#else
# define HAVE_FULLFSYNC 0
#endif


/*
** The fsync() system call does not work as advertised on many
** unix systems.  The following procedure is an attempt to make
** it work better.
**
** The SQLITE_NO_SYNC macro disables all fsync()s.  This is useful
** for testing when we want to run through the test suite quickly.
** You are strongly advised *not* to deploy with SQLITE_NO_SYNC
** enabled, however, since with SQLITE_NO_SYNC enabled, an OS crash
** or power failure will likely corrupt the database file.
*/
static int full_fsync(int fd, int fullSync, int dataOnly){
  int rc;

  /* Record the number of times that we do a normal fsync() and 
  ** FULLSYNC.  This is used during testing to verify that this procedure
  ** gets called with the correct arguments.
  */
#ifdef SQLITE_TEST
  if( fullSync ) sqlite3_fullsync_count++;
  sqlite3_sync_count++;
#endif

  /* If we compiled with the SQLITE_NO_SYNC flag, then syncing is a
  ** no-op
  */
#ifdef SQLITE_NO_SYNC
  rc = SQLITE_OK;
#else

#if HAVE_FULLFSYNC
  if( fullSync ){
    rc = fcntl(fd, F_FULLFSYNC, 0);
  }else{
    rc = 1;
  }
  /* If the FULLFSYNC failed, fall back to attempting an fsync().
   * It shouldn't be possible for fullfsync to fail on the local 
   * file system (on OSX), so failure indicates that FULLFSYNC
   * isn't supported for this file system. So, attempt an fsync 
   * and (for now) ignore the overhead of a superfluous fcntl call.  
   * It'd be better to detect fullfsync support once and avoid 
   * the fcntl call every time sync is called.
   */
  if( rc ) rc = fsync(fd);

#else 
  if( dataOnly ){
    rc = fdatasync(fd);
  }else{
    rc = fsync(fd);
  }
#endif /* HAVE_FULLFSYNC */
#endif /* defined(SQLITE_NO_SYNC) */

  return rc;
}

/*
** Make sure all writes to a particular file are committed to disk.
**
** If dataOnly==0 then both the file itself and its metadata (file
** size, access time, etc) are synced.  If dataOnly!=0 then only the
** file data is synced.
**
** Under Unix, also make sure that the directory entry for the file
** has been created by fsync-ing the directory that contains the file.
** If we do not do this and we encounter a power failure, the directory
** entry for the journal might not exist after we reboot.  The next
** SQLite to access the file will not know that the journal exists (because
** the directory entry for the journal was never created) and the transaction
** will not roll back - possibly leading to database corruption.
*/
static int unixSync(OsFile *id, int dataOnly){
  int rc;
  unixFile *pFile = (unixFile*)id;
  assert( pFile );
  OSTRACE2("SYNC    %-3d\n", pFile->h);
  rc = full_fsync(pFile->h, pFile->fullSync, dataOnly);
  SimulateIOError( rc=1 );
  if( rc ){
    return SQLITE_IOERR_FSYNC;
  }
  if( pFile->dirfd>=0 ){
    OSTRACE4("DIRSYNC %-3d (have_fullfsync=%d fullsync=%d)\n", pFile->dirfd,
            HAVE_FULLFSYNC, pFile->fullSync);
#ifndef SQLITE_DISABLE_DIRSYNC
    /* The directory sync is only attempted if full_fsync is
    ** turned off or unavailable.  If a full_fsync occurred above,
    ** then the directory sync is superfluous.
    */
    if( (!HAVE_FULLFSYNC || !pFile->fullSync) && full_fsync(pFile->dirfd,0,0) ){
       /*
       ** We have received multiple reports of fsync() returning
       ** errors when applied to directories on certain file systems.
       ** A failed directory sync is not a big deal.  So it seems
       ** better to ignore the error.  Ticket #1657
       */
       /* return SQLITE_IOERR; */
    }
#endif
    close(pFile->dirfd);  /* Only need to sync once, so close the directory */
    pFile->dirfd = -1;    /* when we are done. */
  }
  return SQLITE_OK;
}

/*
** Sync the directory zDirname. This is a no-op on operating systems other
** than UNIX.
**
** This is used to make sure the master journal file has truely been deleted
** before making changes to individual journals on a multi-database commit.
** The F_FULLFSYNC option is not needed here.
*/
int sqlite3UnixSyncDirectory(const char *zDirname){
#ifdef SQLITE_DISABLE_DIRSYNC
  return SQLITE_OK;
#else
  int fd;
  int r;
  fd = open(zDirname, O_RDONLY|O_BINARY, 0);
  OSTRACE3("DIRSYNC %-3d (%s)\n", fd, zDirname);
  if( fd<0 ){
    return SQLITE_CANTOPEN; 
  }
  r = fsync(fd);
  close(fd);
  SimulateIOError( r=1 );
  if( r ){
    return SQLITE_IOERR_DIR_FSYNC;
  }else{
    return SQLITE_OK;
  }
#endif
}

/*
** Truncate an open file to a specified size
*/
static int unixTruncate(OsFile *id, i64 nByte){
  int rc;
  assert( id );
  rc = ftruncate(((unixFile*)id)->h, nByte);
  SimulateIOError( rc=1 );
  if( rc ){
    return SQLITE_IOERR_TRUNCATE;
  }else{
    return SQLITE_OK;
  }
}

/*
** Determine the current size of a file in bytes
*/
static int unixFileSize(OsFile *id, i64 *pSize){
  int rc;
  struct stat buf;
  assert( id );
  rc = fstat(((unixFile*)id)->h, &buf);
  SimulateIOError( rc=1 );
  if( rc!=0 ){
    return SQLITE_IOERR_FSTAT;
  }
  *pSize = buf.st_size;
  return SQLITE_OK;
}

/*
** This routine checks if there is a RESERVED lock held on the specified
** file by this or any other process. If such a lock is held, return
** non-zero.  If the file is unlocked or holds only SHARED locks, then
** return zero.
*/
static int unixCheckReservedLock(OsFile *id){
  int r = 0;
  unixFile *pFile = (unixFile*)id;

  assert( pFile );
  sqlite3OsEnterMutex(); /* Because pFile->pLock is shared across threads */

  /* Check if a thread in this process holds such a lock */
  if( pFile->pLock->locktype>SHARED_LOCK ){
    r = 1;
  }

  /* Otherwise see if some other process holds it.
  */
  if( !r ){
    struct flock lock;
    lock.l_whence = SEEK_SET;
    lock.l_start = RESERVED_BYTE;
    lock.l_len = 1;
    lock.l_type = F_WRLCK;
    fcntl(pFile->h, F_GETLK, &lock);
    if( lock.l_type!=F_UNLCK ){
      r = 1;
    }
  }
  
  sqlite3OsLeaveMutex();
  OSTRACE3("TEST WR-LOCK %d %d\n", pFile->h, r);

  return r;
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
** This routine will only increase a lock.  Use the sqlite3OsUnlock()
** routine to lower a locking level.
*/
static int unixLock(OsFile *id, int locktype){
  /* The following describes the implementation of the various locks and
  ** lock transitions in terms of the POSIX advisory shared and exclusive
  ** lock primitives (called read-locks and write-locks below, to avoid
  ** confusion with SQLite lock names). The algorithms are complicated
  ** slightly in order to be compatible with windows systems simultaneously
  ** accessing the same database file, in case that is ever required.
  **
  ** Symbols defined in os.h indentify the 'pending byte' and the 'reserved
  ** byte', each single bytes at well known offsets, and the 'shared byte
  ** range', a range of 510 bytes at a well known offset.
  **
  ** To obtain a SHARED lock, a read-lock is obtained on the 'pending
  ** byte'.  If this is successful, a random byte from the 'shared byte
  ** range' is read-locked and the lock on the 'pending byte' released.
  **
  ** A process may only obtain a RESERVED lock after it has a SHARED lock.
  ** A RESERVED lock is implemented by grabbing a write-lock on the
  ** 'reserved byte'. 
  **
  ** A process may only obtain a PENDING lock after it has obtained a
  ** SHARED lock. A PENDING lock is implemented by obtaining a write-lock
  ** on the 'pending byte'. This ensures that no new SHARED locks can be
  ** obtained, but existing SHARED locks are allowed to persist. A process
  ** does not have to obtain a RESERVED lock on the way to a PENDING lock.
  ** This property is used by the algorithm for rolling back a journal file
  ** after a crash.
  **
  ** An EXCLUSIVE lock, obtained after a PENDING lock is held, is
  ** implemented by obtaining a write-lock on the entire 'shared byte
  ** range'. Since all other locks require a read-lock on one of the bytes
  ** within this range, this ensures that no other locks are held on the
  ** database. 
  **
  ** The reason a single byte cannot be used instead of the 'shared byte
  ** range' is that some versions of windows do not support read-locks. By
  ** locking a random byte from a range, concurrent SHARED locks may exist
  ** even if the locking primitive used is always a write-lock.
  */
  int rc = SQLITE_OK;
  unixFile *pFile = (unixFile*)id;
  struct lockInfo *pLock = pFile->pLock;
  struct flock lock;
  int s;

  assert( pFile );
  OSTRACE7("LOCK    %d %s was %s(%s,%d) pid=%d\n", pFile->h,
      locktypeName(locktype), locktypeName(pFile->locktype),
      locktypeName(pLock->locktype), pLock->cnt , getpid());

  /* If there is already a lock of this type or more restrictive on the
  ** OsFile, do nothing. Don't use the end_lock: exit path, as
  ** sqlite3OsEnterMutex() hasn't been called yet.
  */
  if( pFile->locktype>=locktype ){
    OSTRACE3("LOCK    %d %s ok (already held)\n", pFile->h,
            locktypeName(locktype));
    return SQLITE_OK;
  }

  /* Make sure the locking sequence is correct
  */
  assert( pFile->locktype!=NO_LOCK || locktype==SHARED_LOCK );
  assert( locktype!=PENDING_LOCK );
  assert( locktype!=RESERVED_LOCK || pFile->locktype==SHARED_LOCK );

  /* This mutex is needed because pFile->pLock is shared across threads
  */
  sqlite3OsEnterMutex();

  /* Make sure the current thread owns the pFile.
  */
  rc = transferOwnership(pFile);
  if( rc!=SQLITE_OK ){
    sqlite3OsLeaveMutex();
    return rc;
  }
  pLock = pFile->pLock;

  /* If some thread using this PID has a lock via a different OsFile*
  ** handle that precludes the requested lock, return BUSY.
  */
  if( (pFile->locktype!=pLock->locktype && 
          (pLock->locktype>=PENDING_LOCK || locktype>SHARED_LOCK))
  ){
    rc = SQLITE_BUSY;
    goto end_lock;
  }

  /* If a SHARED lock is requested, and some thread using this PID already
  ** has a SHARED or RESERVED lock, then increment reference counts and
  ** return SQLITE_OK.
  */
  if( locktype==SHARED_LOCK && 
      (pLock->locktype==SHARED_LOCK || pLock->locktype==RESERVED_LOCK) ){
    assert( locktype==SHARED_LOCK );
    assert( pFile->locktype==0 );
    assert( pLock->cnt>0 );
    pFile->locktype = SHARED_LOCK;
    pLock->cnt++;
    pFile->pOpen->nLock++;
    goto end_lock;
  }

  lock.l_len = 1L;

  lock.l_whence = SEEK_SET;

  /* A PENDING lock is needed before acquiring a SHARED lock and before
  ** acquiring an EXCLUSIVE lock.  For the SHARED lock, the PENDING will
  ** be released.
  */
  if( locktype==SHARED_LOCK 
      || (locktype==EXCLUSIVE_LOCK && pFile->locktype<PENDING_LOCK)
  ){
    lock.l_type = (locktype==SHARED_LOCK?F_RDLCK:F_WRLCK);
    lock.l_start = PENDING_BYTE;
    s = fcntl(pFile->h, F_SETLK, &lock);
    if( s==(-1) ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
      goto end_lock;
    }
  }


  /* If control gets to this point, then actually go ahead and make
  ** operating system calls for the specified lock.
  */
  if( locktype==SHARED_LOCK ){
    assert( pLock->cnt==0 );
    assert( pLock->locktype==0 );

    /* Now get the read-lock */
    lock.l_start = SHARED_FIRST;
    lock.l_len = SHARED_SIZE;
    s = fcntl(pFile->h, F_SETLK, &lock);

    /* Drop the temporary PENDING lock */
    lock.l_start = PENDING_BYTE;
    lock.l_len = 1L;
    lock.l_type = F_UNLCK;
    if( fcntl(pFile->h, F_SETLK, &lock)!=0 ){
      rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
      goto end_lock;
    }
    if( s==(-1) ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }else{
      pFile->locktype = SHARED_LOCK;
      pFile->pOpen->nLock++;
      pLock->cnt = 1;
    }
  }else if( locktype==EXCLUSIVE_LOCK && pLock->cnt>1 ){
    /* We are trying for an exclusive lock but another thread in this
    ** same process is still holding a shared lock. */
    rc = SQLITE_BUSY;
  }else{
    /* The request was for a RESERVED or EXCLUSIVE lock.  It is
    ** assumed that there is a SHARED or greater lock on the file
    ** already.
    */
    assert( 0!=pFile->locktype );
    lock.l_type = F_WRLCK;
    switch( locktype ){
      case RESERVED_LOCK:
        lock.l_start = RESERVED_BYTE;
        break;
      case EXCLUSIVE_LOCK:
        lock.l_start = SHARED_FIRST;
        lock.l_len = SHARED_SIZE;
        break;
      default:
        assert(0);
    }
    s = fcntl(pFile->h, F_SETLK, &lock);
    if( s==(-1) ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }
  }
  
  if( rc==SQLITE_OK ){
    pFile->locktype = locktype;
    pLock->locktype = locktype;
  }else if( locktype==EXCLUSIVE_LOCK ){
    pFile->locktype = PENDING_LOCK;
    pLock->locktype = PENDING_LOCK;
  }

end_lock:
  sqlite3OsLeaveMutex();
  OSTRACE4("LOCK    %d %s %s\n", pFile->h, locktypeName(locktype), 
      rc==SQLITE_OK ? "ok" : "failed");
  return rc;
}

/*
** Lower the locking level on file descriptor pFile to locktype.  locktype
** must be either NO_LOCK or SHARED_LOCK.
**
** If the locking level of the file descriptor is already at or below
** the requested locking level, this routine is a no-op.
*/
static int unixUnlock(OsFile *id, int locktype){
  struct lockInfo *pLock;
  struct flock lock;
  int rc = SQLITE_OK;
  unixFile *pFile = (unixFile*)id;

  assert( pFile );
  OSTRACE7("UNLOCK  %d %d was %d(%d,%d) pid=%d\n", pFile->h, locktype,
      pFile->locktype, pFile->pLock->locktype, pFile->pLock->cnt, getpid());

  assert( locktype<=SHARED_LOCK );
  if( pFile->locktype<=locktype ){
    return SQLITE_OK;
  }
  if( CHECK_THREADID(pFile) ){
    return SQLITE_MISUSE;
  }
  sqlite3OsEnterMutex();
  pLock = pFile->pLock;
  assert( pLock->cnt!=0 );
  if( pFile->locktype>SHARED_LOCK ){
    assert( pLock->locktype==pFile->locktype );
    if( locktype==SHARED_LOCK ){
      lock.l_type = F_RDLCK;
      lock.l_whence = SEEK_SET;
      lock.l_start = SHARED_FIRST;
      lock.l_len = SHARED_SIZE;
      if( fcntl(pFile->h, F_SETLK, &lock)==(-1) ){
        /* This should never happen */
        rc = SQLITE_IOERR_RDLOCK;
      }
    }
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = PENDING_BYTE;
    lock.l_len = 2L;  assert( PENDING_BYTE+1==RESERVED_BYTE );
    if( fcntl(pFile->h, F_SETLK, &lock)!=(-1) ){
      pLock->locktype = SHARED_LOCK;
    }else{
      rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
    }
  }
  if( locktype==NO_LOCK ){
    struct openCnt *pOpen;

    /* Decrement the shared lock counter.  Release the lock using an
    ** OS call only when all threads in this same process have released
    ** the lock.
    */
    pLock->cnt--;
    if( pLock->cnt==0 ){
      lock.l_type = F_UNLCK;
      lock.l_whence = SEEK_SET;
      lock.l_start = lock.l_len = 0L;
      if( fcntl(pFile->h, F_SETLK, &lock)!=(-1) ){
        pLock->locktype = NO_LOCK;
      }else{
        rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
      }
    }

    /* Decrement the count of locks against this same file.  When the
    ** count reaches zero, close any other file descriptors whose close
    ** was deferred because of outstanding locks.
    */
    pOpen = pFile->pOpen;
    pOpen->nLock--;
    assert( pOpen->nLock>=0 );
    if( pOpen->nLock==0 && pOpen->nPending>0 ){
      int i;
      for(i=0; i<pOpen->nPending; i++){
        close(pOpen->aPending[i]);
      }
      free(pOpen->aPending);
      pOpen->nPending = 0;
      pOpen->aPending = 0;
    }
  }
  sqlite3OsLeaveMutex();
  pFile->locktype = locktype;
  return rc;
}

/*
** Close a file.
*/
static int unixClose(OsFile **pId){
  unixFile *id = (unixFile*)*pId;

  if( !id ) return SQLITE_OK;
  unixUnlock(*pId, NO_LOCK);
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  sqlite3OsEnterMutex();

  if( id->pOpen->nLock ){
    /* If there are outstanding locks, do not actually close the file just
    ** yet because that would clear those locks.  Instead, add the file
    ** descriptor to pOpen->aPending.  It will be automatically closed when
    ** the last lock is cleared.
    */
    int *aNew;
    struct openCnt *pOpen = id->pOpen;
    aNew = realloc( pOpen->aPending, (pOpen->nPending+1)*sizeof(int) );
    if( aNew==0 ){
      /* If a malloc fails, just leak the file descriptor */
    }else{
      pOpen->aPending = aNew;
      pOpen->aPending[pOpen->nPending] = id->h;
      pOpen->nPending++;
    }
  }else{
    /* There are no outstanding locks so we can close the file immediately */
    close(id->h);
  }
  releaseLockInfo(id->pLock);
  releaseOpenCnt(id->pOpen);

  sqlite3OsLeaveMutex();
  id->isOpen = 0;
  OSTRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  sqlite3ThreadSafeFree(id);
  *pId = 0;
  return SQLITE_OK;
}


#ifdef SQLITE_ENABLE_LOCKING_STYLE
#pragma mark AFP Support

/*
 ** The afpLockingContext structure contains all afp lock specific state
 */
typedef struct afpLockingContext afpLockingContext;
struct afpLockingContext {
  unsigned long long sharedLockByte;
  char *filePath;
};

struct ByteRangeLockPB2
{
  unsigned long long offset;        /* offset to first byte to lock */
  unsigned long long length;        /* nbr of bytes to lock */
  unsigned long long retRangeStart; /* nbr of 1st byte locked if successful */
  unsigned char unLockFlag;         /* 1 = unlock, 0 = lock */
  unsigned char startEndFlag;       /* 1=rel to end of fork, 0=rel to start */
  int fd;                           /* file desc to assoc this lock with */
};

#define afpfsByteRangeLock2FSCTL	_IOWR('z', 23, struct ByteRangeLockPB2)

/* return 0 on success, 1 on failure.  To match the behavior of the 
  normal posix file locking (used in unixLock for example), we should 
  provide 'richer' return codes - specifically to differentiate between
  'file busy' and 'file system error' results */
static int _AFPFSSetLock(const char *path, int fd, unsigned long long offset, 
                         unsigned long long length, int setLockFlag)
{
  struct ByteRangeLockPB2	pb;
  int                     err;
  
  pb.unLockFlag = setLockFlag ? 0 : 1;
  pb.startEndFlag = 0;
  pb.offset = offset;
  pb.length = length; 
  pb.fd = fd;
  OSTRACE5("AFPLOCK setting lock %s for %d in range %llx:%llx\n", 
    (setLockFlag?"ON":"OFF"), fd, offset, length);
  err = fsctl(path, afpfsByteRangeLock2FSCTL, &pb, 0);
  if ( err==-1 ) {
    OSTRACE4("AFPLOCK failed to fsctl() '%s' %d %s\n", path, errno, 
      strerror(errno));
    return 1; // error
  } else {
    return 0;
  }
}

/*
 ** This routine checks if there is a RESERVED lock held on the specified
 ** file by this or any other process. If such a lock is held, return
 ** non-zero.  If the file is unlocked or holds only SHARED locks, then
 ** return zero.
 */
static int afpUnixCheckReservedLock(OsFile *id){
  int r = 0;
  unixFile *pFile = (unixFile*)id;
  
  assert( pFile ); 
  afpLockingContext *context = (afpLockingContext *) pFile->lockingContext;
  
  /* Check if a thread in this process holds such a lock */
  if( pFile->locktype>SHARED_LOCK ){
    r = 1;
  }
  
  /* Otherwise see if some other process holds it.
   */
  if ( !r ) {
    // lock the byte
    int failed = _AFPFSSetLock(context->filePath, pFile->h, RESERVED_BYTE, 1,1);  
    if (failed) {
      /* if we failed to get the lock then someone else must have it */
      r = 1;
    } else {
      /* if we succeeded in taking the reserved lock, unlock it to restore
      ** the original state */
      _AFPFSSetLock(context->filePath, pFile->h, RESERVED_BYTE, 1, 0);
    }
  }
  OSTRACE3("TEST WR-LOCK %d %d\n", pFile->h, r);
  
  return r;
}

/* AFP-style locking following the behavior of unixLock, see the unixLock 
** function comments for details of lock management. */
static int afpUnixLock(OsFile *id, int locktype)
{
  int rc = SQLITE_OK;
  unixFile *pFile = (unixFile*)id;
  afpLockingContext *context = (afpLockingContext *) pFile->lockingContext;
  int gotPendingLock = 0;
  
  assert( pFile );
  OSTRACE5("LOCK    %d %s was %s pid=%d\n", pFile->h,
         locktypeName(locktype), locktypeName(pFile->locktype), getpid());  
  /* If there is already a lock of this type or more restrictive on the
    ** OsFile, do nothing. Don't use the afp_end_lock: exit path, as
    ** sqlite3OsEnterMutex() hasn't been called yet.
    */
  if( pFile->locktype>=locktype ){
    OSTRACE3("LOCK    %d %s ok (already held)\n", pFile->h,
           locktypeName(locktype));
    return SQLITE_OK;
  }

  /* Make sure the locking sequence is correct
    */
  assert( pFile->locktype!=NO_LOCK || locktype==SHARED_LOCK );
  assert( locktype!=PENDING_LOCK );
  assert( locktype!=RESERVED_LOCK || pFile->locktype==SHARED_LOCK );
  
  /* This mutex is needed because pFile->pLock is shared across threads
    */
  sqlite3OsEnterMutex();

  /* Make sure the current thread owns the pFile.
    */
  rc = transferOwnership(pFile);
  if( rc!=SQLITE_OK ){
    sqlite3OsLeaveMutex();
    return rc;
  }
    
  /* A PENDING lock is needed before acquiring a SHARED lock and before
    ** acquiring an EXCLUSIVE lock.  For the SHARED lock, the PENDING will
    ** be released.
    */
  if( locktype==SHARED_LOCK 
      || (locktype==EXCLUSIVE_LOCK && pFile->locktype<PENDING_LOCK)
      ){
    int failed = _AFPFSSetLock(context->filePath, pFile->h, 
      PENDING_BYTE, 1, 1);
    if (failed) {
      rc = SQLITE_BUSY;
      goto afp_end_lock;
    }
  }
  
  /* If control gets to this point, then actually go ahead and make
    ** operating system calls for the specified lock.
    */
  if( locktype==SHARED_LOCK ){
    int lk, failed;
    int tries = 0;
    
    /* Now get the read-lock */
    /* note that the quality of the randomness doesn't matter that much */
    lk = random(); 
    context->sharedLockByte = (lk & 0x7fffffff)%(SHARED_SIZE - 1);
    failed = _AFPFSSetLock(context->filePath, pFile->h, 
      SHARED_FIRST+context->sharedLockByte, 1, 1);
    
    /* Drop the temporary PENDING lock */
    if (_AFPFSSetLock(context->filePath, pFile->h, PENDING_BYTE, 1, 0)) {
      rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
      goto afp_end_lock;
    }
    
    if( failed ){
      rc = SQLITE_BUSY;
    } else {
      pFile->locktype = SHARED_LOCK;
    }
  }else{
    /* The request was for a RESERVED or EXCLUSIVE lock.  It is
    ** assumed that there is a SHARED or greater lock on the file
    ** already.
    */
    int failed = 0;
    assert( 0!=pFile->locktype );
    if (locktype >= RESERVED_LOCK && pFile->locktype < RESERVED_LOCK) {
        /* Acquire a RESERVED lock */
        failed = _AFPFSSetLock(context->filePath, pFile->h, RESERVED_BYTE, 1,1);
    }
    if (!failed && locktype == EXCLUSIVE_LOCK) {
      /* Acquire an EXCLUSIVE lock */
        
      /* Remove the shared lock before trying the range.  we'll need to 
      ** reestablish the shared lock if we can't get the  afpUnixUnlock
      */
      if (!_AFPFSSetLock(context->filePath, pFile->h, SHARED_FIRST +
                         context->sharedLockByte, 1, 0)) {
        /* now attemmpt to get the exclusive lock range */
        failed = _AFPFSSetLock(context->filePath, pFile->h, SHARED_FIRST, 
                               SHARED_SIZE, 1);
        if (failed && _AFPFSSetLock(context->filePath, pFile->h, SHARED_FIRST +
                                    context->sharedLockByte, 1, 1)) {
          rc = SQLITE_IOERR_RDLOCK; /* this should never happen */
        }
      } else {
        /* */
        rc = SQLITE_IOERR_UNLOCK; /* this should never happen */
      }
    }
    if( failed && rc == SQLITE_OK){
      rc = SQLITE_BUSY;
    }
  }
  
  if( rc==SQLITE_OK ){
    pFile->locktype = locktype;
  }else if( locktype==EXCLUSIVE_LOCK ){
    pFile->locktype = PENDING_LOCK;
  }
  
afp_end_lock:
    sqlite3OsLeaveMutex();
  OSTRACE4("LOCK    %d %s %s\n", pFile->h, locktypeName(locktype), 
         rc==SQLITE_OK ? "ok" : "failed");
  return rc;
}

/*
 ** Lower the locking level on file descriptor pFile to locktype.  locktype
 ** must be either NO_LOCK or SHARED_LOCK.
 **
 ** If the locking level of the file descriptor is already at or below
 ** the requested locking level, this routine is a no-op.
 */
static int afpUnixUnlock(OsFile *id, int locktype) {
  struct flock lock;
  int rc = SQLITE_OK;
  unixFile *pFile = (unixFile*)id;
  afpLockingContext *context = (afpLockingContext *) pFile->lockingContext;

  assert( pFile );
  OSTRACE5("UNLOCK  %d %d was %d pid=%d\n", pFile->h, locktype,
         pFile->locktype, getpid());
  
  assert( locktype<=SHARED_LOCK );
  if( pFile->locktype<=locktype ){
    return SQLITE_OK;
  }
  if( CHECK_THREADID(pFile) ){
    return SQLITE_MISUSE;
  }
  sqlite3OsEnterMutex();
  if( pFile->locktype>SHARED_LOCK ){
    if( locktype==SHARED_LOCK ){
      int failed = 0;

      /* unlock the exclusive range - then re-establish the shared lock */
      if (pFile->locktype==EXCLUSIVE_LOCK) {
        failed = _AFPFSSetLock(context->filePath, pFile->h, SHARED_FIRST, 
                                 SHARED_SIZE, 0);
        if (!failed) {
          /* successfully removed the exclusive lock */
          if (_AFPFSSetLock(context->filePath, pFile->h, SHARED_FIRST+
                            context->sharedLockByte, 1, 1)) {
            /* failed to re-establish our shared lock */
            rc = SQLITE_IOERR_RDLOCK; /* This should never happen */
          }
        } else {
          /* This should never happen - failed to unlock the exclusive range */
          rc = SQLITE_IOERR_UNLOCK;
        } 
      }
    }
    if (rc == SQLITE_OK && pFile->locktype>=PENDING_LOCK) {
      if (_AFPFSSetLock(context->filePath, pFile->h, PENDING_BYTE, 1, 0)){
        /* failed to release the pending lock */
        rc = SQLITE_IOERR_UNLOCK; /* This should never happen */
      }
    } 
    if (rc == SQLITE_OK && pFile->locktype>=RESERVED_LOCK) {
      if (_AFPFSSetLock(context->filePath, pFile->h, RESERVED_BYTE, 1, 0)) {
        /* failed to release the reserved lock */
        rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
      }
    } 
  }
  if( locktype==NO_LOCK ){
    int failed = _AFPFSSetLock(context->filePath, pFile->h, 
                               SHARED_FIRST + context->sharedLockByte, 1, 0);
    if (failed) {
      rc = SQLITE_IOERR_UNLOCK;  /* This should never happen */
    }
  }
  if (rc == SQLITE_OK)
    pFile->locktype = locktype;
  sqlite3OsLeaveMutex();
  return rc;
}

/*
 ** Close a file & cleanup AFP specific locking context 
 */
static int afpUnixClose(OsFile **pId) {
  unixFile *id = (unixFile*)*pId;
  
  if( !id ) return SQLITE_OK;
  afpUnixUnlock(*pId, NO_LOCK);
  /* free the AFP locking structure */
  if (id->lockingContext != NULL) {
    if (((afpLockingContext *)id->lockingContext)->filePath != NULL)
      sqlite3ThreadSafeFree(((afpLockingContext*)id->lockingContext)->filePath);
    sqlite3ThreadSafeFree(id->lockingContext);
  }
  
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  close(id->h);
  id->isOpen = 0;
  OSTRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  sqlite3ThreadSafeFree(id);
  *pId = 0;
  return SQLITE_OK;
}


#pragma mark flock() style locking

/*
 ** The flockLockingContext is not used
 */
typedef void flockLockingContext;

static int flockUnixCheckReservedLock(OsFile *id) {
  unixFile *pFile = (unixFile*)id;
  
  if (pFile->locktype == RESERVED_LOCK) {
    return 1; // already have a reserved lock
  } else {
    // attempt to get the lock
    int rc = flock(pFile->h, LOCK_EX | LOCK_NB);
    if (!rc) {
      // got the lock, unlock it
      flock(pFile->h, LOCK_UN);
      return 0;  // no one has it reserved
    }
    return 1; // someone else might have it reserved
  }
}

static int flockUnixLock(OsFile *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  
  // if we already have a lock, it is exclusive.  
  // Just adjust level and punt on outta here.
  if (pFile->locktype > NO_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  // grab an exclusive lock
  int rc = flock(pFile->h, LOCK_EX | LOCK_NB);
  if (rc) {
    // didn't get, must be busy
    return SQLITE_BUSY;
  } else {
    // got it, set the type and return ok
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
}

static int flockUnixUnlock(OsFile *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  
  assert( locktype<=SHARED_LOCK );
  
  // no-op if possible
  if( pFile->locktype==locktype ){
    return SQLITE_OK;
  }
  
  // shared can just be set because we always have an exclusive
  if (locktype==SHARED_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  // no, really, unlock.
  int rc = flock(pFile->h, LOCK_UN);
  if (rc)
    return SQLITE_IOERR_UNLOCK;
  else {
    pFile->locktype = NO_LOCK;
    return SQLITE_OK;
  }
}

/*
 ** Close a file.
 */
static int flockUnixClose(OsFile **pId) {
  unixFile *id = (unixFile*)*pId;
  
  if( !id ) return SQLITE_OK;
  flockUnixUnlock(*pId, NO_LOCK);
  
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  sqlite3OsEnterMutex();
  
  close(id->h);  
  sqlite3OsLeaveMutex();
  id->isOpen = 0;
  OSTRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  sqlite3ThreadSafeFree(id);
  *pId = 0;
  return SQLITE_OK;
}

#pragma mark Old-School .lock file based locking

/*
 ** The dotlockLockingContext structure contains all dotlock (.lock) lock
 ** specific state
 */
typedef struct dotlockLockingContext dotlockLockingContext;
struct dotlockLockingContext {
  char *lockPath;
};


static int dotlockUnixCheckReservedLock(OsFile *id) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  if (pFile->locktype == RESERVED_LOCK) {
    return 1; // already have a reserved lock
  } else {
    struct stat statBuf;
    if (lstat(context->lockPath,&statBuf) == 0)
      // file exists, someone else has the lock
      return 1;
    else
      // file does not exist, we could have it if we want it
      return 0;
  }
}

static int dotlockUnixLock(OsFile *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  // if we already have a lock, it is exclusive.  
  // Just adjust level and punt on outta here.
  if (pFile->locktype > NO_LOCK) {
    pFile->locktype = locktype;
    
    /* Always update the timestamp on the old file */
    utimes(context->lockPath,NULL);
    return SQLITE_OK;
  }
  
  // check to see if lock file already exists
  struct stat statBuf;
  if (lstat(context->lockPath,&statBuf) == 0){
    return SQLITE_BUSY; // it does, busy
  }
  
  // grab an exclusive lock
  int fd = open(context->lockPath,O_RDONLY|O_CREAT|O_EXCL,0600);
  if (fd < 0) {
    // failed to open/create the file, someone else may have stolen the lock
    return SQLITE_BUSY; 
  }
  close(fd);
  
  // got it, set the type and return ok
  pFile->locktype = locktype;
  return SQLITE_OK;
}

static int dotlockUnixUnlock(OsFile *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  assert( locktype<=SHARED_LOCK );
  
  // no-op if possible
  if( pFile->locktype==locktype ){
    return SQLITE_OK;
  }
  
  // shared can just be set because we always have an exclusive
  if (locktype==SHARED_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  // no, really, unlock.
  unlink(context->lockPath);
  pFile->locktype = NO_LOCK;
  return SQLITE_OK;
}

/*
 ** Close a file.
 */
static int dotlockUnixClose(OsFile **pId) {
  unixFile *id = (unixFile*)*pId;
  
  if( !id ) return SQLITE_OK;
  dotlockUnixUnlock(*pId, NO_LOCK);
  /* free the dotlock locking structure */
  if (id->lockingContext != NULL) {
    if (((dotlockLockingContext *)id->lockingContext)->lockPath != NULL)
      sqlite3ThreadSafeFree( ( (dotlockLockingContext *)
        id->lockingContext)->lockPath);
    sqlite3ThreadSafeFree(id->lockingContext);
  }
  
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  sqlite3OsEnterMutex();
  
  close(id->h);
  
  sqlite3OsLeaveMutex();
  id->isOpen = 0;
  OSTRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  sqlite3ThreadSafeFree(id);
  *pId = 0;
  return SQLITE_OK;
}


#pragma mark No locking

/*
 ** The nolockLockingContext is void
 */
typedef void nolockLockingContext;

static int nolockUnixCheckReservedLock(OsFile *id) {
  return 0;
}

static int nolockUnixLock(OsFile *id, int locktype) {
  return SQLITE_OK;
}

static int nolockUnixUnlock(OsFile *id, int locktype) {
  return SQLITE_OK;
}

/*
 ** Close a file.
 */
static int nolockUnixClose(OsFile **pId) {
  unixFile *id = (unixFile*)*pId;
  
  if( !id ) return SQLITE_OK;
  if( id->dirfd>=0 ) close(id->dirfd);
  id->dirfd = -1;
  sqlite3OsEnterMutex();
  
  close(id->h);
  
  sqlite3OsLeaveMutex();
  id->isOpen = 0;
  OSTRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  sqlite3ThreadSafeFree(id);
  *pId = 0;
  return SQLITE_OK;
}

#endif /* SQLITE_ENABLE_LOCKING_STYLE */

/*
** Turn a relative pathname into a full pathname.  Return a pointer
** to the full pathname stored in space obtained from sqliteMalloc().
** The calling function is responsible for freeing this space once it
** is no longer needed.
*/
char *sqlite3UnixFullPathname(const char *zRelative){
  char *zFull = 0;
  if( zRelative[0]=='/' ){
    sqlite3SetString(&zFull, zRelative, (char*)0);
  }else{
    char *zBuf = sqliteMalloc(5000);
    if( zBuf==0 ){
      return 0;
    }
    zBuf[0] = 0;
    sqlite3SetString(&zFull, getcwd(zBuf, 5000), "/", zRelative,
                    (char*)0);
    sqliteFree(zBuf);
  }

#if 0
  /*
  ** Remove "/./" path elements and convert "/A/./" path elements
  ** to just "/".
  */
  if( zFull ){
    int i, j;
    for(i=j=0; zFull[i]; i++){
      if( zFull[i]=='/' ){
        if( zFull[i+1]=='/' ) continue;
        if( zFull[i+1]=='.' && zFull[i+2]=='/' ){
          i += 1;
          continue;
        }
        if( zFull[i+1]=='.' && zFull[i+2]=='.' && zFull[i+3]=='/' ){
          while( j>0 && zFull[j-1]!='/' ){ j--; }
          i += 3;
          continue;
        }
      }
      zFull[j++] = zFull[i];
    }
    zFull[j] = 0;
  }
#endif

  return zFull;
}

/*
** Change the value of the fullsync flag in the given file descriptor.
*/
static void unixSetFullSync(OsFile *id, int v){
  ((unixFile*)id)->fullSync = v;
}

/*
** Return the underlying file handle for an OsFile
*/
static int unixFileHandle(OsFile *id){
  return ((unixFile*)id)->h;
}

/*
** Return an integer that indices the type of lock currently held
** by this handle.  (Used for testing and analysis only.)
*/
static int unixLockState(OsFile *id){
  return ((unixFile*)id)->locktype;
}

/*
** Return the sector size in bytes of the underlying block device for
** the specified file. This is almost always 512 bytes, but may be
** larger for some devices.
**
** SQLite code assumes this function cannot fail. It also assumes that
** if two files are created in the same file-system directory (i.e.
** a database and it's journal file) that the sector size will be the
** same for both.
*/
static int unixSectorSize(OsFile *id){
  return SQLITE_DEFAULT_SECTOR_SIZE;
}

/*
** This vector defines all the methods that can operate on an OsFile
** for unix.
*/
static const IoMethod sqlite3UnixIoMethod = {
  unixClose,
  unixOpenDirectory,
  unixRead,
  unixWrite,
  unixSeek,
  unixTruncate,
  unixSync,
  unixSetFullSync,
  unixFileHandle,
  unixFileSize,
  unixLock,
  unixUnlock,
  unixLockState,
  unixCheckReservedLock,
  unixSectorSize,
};

#ifdef SQLITE_ENABLE_LOCKING_STYLE
/*
 ** This vector defines all the methods that can operate on an OsFile
 ** for unix with AFP style file locking.
 */
static const IoMethod sqlite3AFPLockingUnixIoMethod = {
    afpUnixClose,
    unixOpenDirectory,
    unixRead,
    unixWrite,
    unixSeek,
    unixTruncate,
    unixSync,
    unixSetFullSync,
    unixFileHandle,
    unixFileSize,
    afpUnixLock,
    afpUnixUnlock,
    unixLockState,
    afpUnixCheckReservedLock,
    unixSectorSize,
};

/*
 ** This vector defines all the methods that can operate on an OsFile
 ** for unix with flock() style file locking.
 */
static const IoMethod sqlite3FlockLockingUnixIoMethod = {
    flockUnixClose,
    unixOpenDirectory,
    unixRead,
    unixWrite,
    unixSeek,
    unixTruncate,
    unixSync,
    unixSetFullSync,
    unixFileHandle,
    unixFileSize,
    flockUnixLock,
    flockUnixUnlock,
    unixLockState,
    flockUnixCheckReservedLock,
    unixSectorSize,
};

/*
 ** This vector defines all the methods that can operate on an OsFile
 ** for unix with dotlock style file locking.
 */
static const IoMethod sqlite3DotlockLockingUnixIoMethod = {
    dotlockUnixClose,
    unixOpenDirectory,
    unixRead,
    unixWrite,
    unixSeek,
    unixTruncate,
    unixSync,
    unixSetFullSync,
    unixFileHandle,
    unixFileSize,
    dotlockUnixLock,
    dotlockUnixUnlock,
    unixLockState,
    dotlockUnixCheckReservedLock,
    unixSectorSize,
};

/*
 ** This vector defines all the methods that can operate on an OsFile
 ** for unix with dotlock style file locking.
 */
static const IoMethod sqlite3NolockLockingUnixIoMethod = {
  nolockUnixClose,
  unixOpenDirectory,
  unixRead,
  unixWrite,
  unixSeek,
  unixTruncate,
  unixSync,
  unixSetFullSync,
  unixFileHandle,
  unixFileSize,
  nolockUnixLock,
  nolockUnixUnlock,
  unixLockState,
  nolockUnixCheckReservedLock,
  unixSectorSize,
};

#endif /* SQLITE_ENABLE_LOCKING_STYLE */

/*
** Allocate memory for a new unixFile and initialize that unixFile.
** Write a pointer to the new unixFile into *pId.
** If we run out of memory, close the file and return an error.
*/
#ifdef SQLITE_ENABLE_LOCKING_STYLE
/* 
 ** When locking extensions are enabled, the filepath and locking style 
 ** are needed to determine the unixFile pMethod to use for locking operations.
 ** The locking-style specific lockingContext data structure is created 
 ** and assigned here also.
 */
static int allocateUnixFile(
  int h,                  /* Open file descriptor of file being opened */
  OsFile **pId,           /* Write completed initialization here */
  const char *zFilename,  /* Name of the file being opened */
  int delFlag             /* Delete-on-or-before-close flag */
){
  sqlite3LockingStyle lockingStyle;
  unixFile *pNew;
  unixFile f;
  int rc;

  memset(&f, 0, sizeof(f));
  lockingStyle = sqlite3DetectLockingStyle(zFilename, h);
  if ( lockingStyle == posixLockingStyle ) {
    sqlite3OsEnterMutex();
    rc = findLockInfo(h, &f.pLock, &f.pOpen);
    sqlite3OsLeaveMutex();
    if( rc ){
      close(h);
      unlink(zFilename);
      return SQLITE_NOMEM;
    }
  } else {
    //  pLock and pOpen are only used for posix advisory locking 
    f.pLock = NULL;
    f.pOpen = NULL;
  }
  if( delFlag ){
    unlink(zFilename);
  }
  f.dirfd = -1;
  f.h = h;
  SET_THREADID(&f);
  pNew = sqlite3ThreadSafeMalloc( sizeof(unixFile) );
  if( pNew==0 ){
    close(h);
    sqlite3OsEnterMutex();
    releaseLockInfo(f.pLock);
    releaseOpenCnt(f.pOpen);
    sqlite3OsLeaveMutex();
    *pId = 0;
    return SQLITE_NOMEM;
  }else{
    *pNew = f;
    switch(lockingStyle) {
      case afpLockingStyle:
        /* afp locking uses the file path so it needs to be included in
        ** the afpLockingContext */
        pNew->pMethod = &sqlite3AFPLockingUnixIoMethod;
        pNew->lockingContext = 
          sqlite3ThreadSafeMalloc(sizeof(afpLockingContext));
        ((afpLockingContext *)pNew->lockingContext)->filePath = 
          sqlite3ThreadSafeMalloc(strlen(zFilename) + 1);
        strcpy(((afpLockingContext *)pNew->lockingContext)->filePath, 
               zFilename);
        srandomdev();
        break;
      case flockLockingStyle:
        /* flock locking doesn't need additional lockingContext information */
        pNew->pMethod = &sqlite3FlockLockingUnixIoMethod;
        break;
      case dotlockLockingStyle:
        /* dotlock locking uses the file path so it needs to be included in
         ** the dotlockLockingContext */
        pNew->pMethod = &sqlite3DotlockLockingUnixIoMethod;
        pNew->lockingContext = sqlite3ThreadSafeMalloc(
          sizeof(dotlockLockingContext));
        ((dotlockLockingContext *)pNew->lockingContext)->lockPath = 
            sqlite3ThreadSafeMalloc(strlen(zFilename) + strlen(".lock") + 1);
        sprintf(((dotlockLockingContext *)pNew->lockingContext)->lockPath, 
                "%s.lock", zFilename);
        break;
      case posixLockingStyle:
        /* posix locking doesn't need additional lockingContext information */
        pNew->pMethod = &sqlite3UnixIoMethod;
        break;
      case noLockingStyle:
      case unsupportedLockingStyle:
      default: 
        pNew->pMethod = &sqlite3NolockLockingUnixIoMethod;
    }
    *pId = (OsFile*)pNew;
    OpenCounter(+1);
    return SQLITE_OK;
  }
}
#else /* SQLITE_ENABLE_LOCKING_STYLE */
static int allocateUnixFile(
  int h,                 /* Open file descriptor on file being opened */
  OsFile **pId,          /* Write the resul unixFile structure here */
  const char *zFilename, /* Name of the file being opened */
  int delFlag            /* If true, delete the file on or before closing */
){
  unixFile *pNew;
  unixFile f;
  int rc;

  memset(&f, 0, sizeof(f));
  sqlite3OsEnterMutex();
  rc = findLockInfo(h, &f.pLock, &f.pOpen);
  sqlite3OsLeaveMutex();
  if( delFlag ){
    unlink(zFilename);
  }
  if( rc ){
    close(h);
    return SQLITE_NOMEM;
  }
  OSTRACE3("OPEN    %-3d %s\n", h, zFilename);
  f.dirfd = -1;
  f.h = h;
  SET_THREADID(&f);
  pNew = sqlite3ThreadSafeMalloc( sizeof(unixFile) );
  if( pNew==0 ){
    close(h);
    sqlite3OsEnterMutex();
    releaseLockInfo(f.pLock);
    releaseOpenCnt(f.pOpen);
    sqlite3OsLeaveMutex();
    *pId = 0;
    return SQLITE_NOMEM;
  }else{
    *pNew = f;
    pNew->pMethod = &sqlite3UnixIoMethod;
    *pId = (OsFile*)pNew;
    OpenCounter(+1);
    return SQLITE_OK;
  }
}
#endif /* SQLITE_ENABLE_LOCKING_STYLE */

#endif /* SQLITE_OMIT_DISKIO */
/***************************************************************************
** Everything above deals with file I/O.  Everything that follows deals
** with other miscellanous aspects of the operating system interface
****************************************************************************/


#ifndef SQLITE_OMIT_LOAD_EXTENSION
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
#include <dlfcn.h>
void *sqlite3UnixDlopen(const char *zFilename){
  return dlopen(zFilename, RTLD_NOW | RTLD_GLOBAL);
}
void *sqlite3UnixDlsym(void *pHandle, const char *zSymbol){
  return dlsym(pHandle, zSymbol);
}
int sqlite3UnixDlclose(void *pHandle){
  return dlclose(pHandle);
}
#endif /* SQLITE_OMIT_LOAD_EXTENSION */

/*
** Get information to seed the random number generator.  The seed
** is written into the buffer zBuf[256].  The calling function must
** supply a sufficiently large buffer.
*/
int sqlite3UnixRandomSeed(char *zBuf){
  /* We have to initialize zBuf to prevent valgrind from reporting
  ** errors.  The reports issued by valgrind are incorrect - we would
  ** prefer that the randomness be increased by making use of the
  ** uninitialized space in zBuf - but valgrind errors tend to worry
  ** some users.  Rather than argue, it seems easier just to initialize
  ** the whole array and silence valgrind, even if that means less randomness
  ** in the random seed.
  **
  ** When testing, initializing zBuf[] to zero is all we do.  That means
  ** that we always use the same random number sequence.  This makes the
  ** tests repeatable.
  */
  memset(zBuf, 0, 256);
#if !defined(SQLITE_TEST)
  {
    int pid, fd;
    fd = open("/dev/urandom", O_RDONLY);
    if( fd<0 ){
      time_t t;
      time(&t);
      memcpy(zBuf, &t, sizeof(t));
      pid = getpid();
      memcpy(&zBuf[sizeof(time_t)], &pid, sizeof(pid));
    }else{
      read(fd, zBuf, 256);
      close(fd);
    }
  }
#endif
  return SQLITE_OK;
}

/*
** Sleep for a little while.  Return the amount of time slept.
** The argument is the number of milliseconds we want to sleep.
*/
int sqlite3UnixSleep(int ms){
#if defined(HAVE_USLEEP) && HAVE_USLEEP
  usleep(ms*1000);
  return ms;
#else
  sleep((ms+999)/1000);
  return 1000*((ms+999)/1000);
#endif
}

/*
** Static variables used for thread synchronization.
**
** inMutex      the nesting depth of the recursive mutex.  The thread
**              holding mutexMain can read this variable at any time.
**              But is must hold mutexAux to change this variable.  Other
**              threads must hold mutexAux to read the variable and can
**              never write.
**
** mutexOwner   The thread id of the thread holding mutexMain.  Same
**              access rules as for inMutex.
**
** mutexOwnerValid   True if the value in mutexOwner is valid.  The same
**                   access rules apply as for inMutex.
**
** mutexMain    The main mutex.  Hold this mutex in order to get exclusive
**              access to SQLite data structures.
**
** mutexAux     An auxiliary mutex needed to access variables defined above.
**
** Mutexes are always acquired in this order: mutexMain mutexAux.   It
** is not necessary to acquire mutexMain in order to get mutexAux - just
** do not attempt to acquire them in the reverse order: mutexAux mutexMain.
** Either get the mutexes with mutexMain first or get mutexAux only.
**
** When running on a platform where the three variables inMutex, mutexOwner,
** and mutexOwnerValid can be set atomically, the mutexAux is not required.
** On many systems, all three are 32-bit integers and writing to a 32-bit
** integer is atomic.  I think.  But there are no guarantees.  So it seems
** safer to protect them using mutexAux.
*/
static int inMutex = 0;
#ifdef SQLITE_UNIX_THREADS
static pthread_t mutexOwner;          /* Thread holding mutexMain */
static int mutexOwnerValid = 0;       /* True if mutexOwner is valid */
static pthread_mutex_t mutexMain = PTHREAD_MUTEX_INITIALIZER; /* The mutex */
static pthread_mutex_t mutexAux = PTHREAD_MUTEX_INITIALIZER;  /* Aux mutex */
#endif

/*
** The following pair of routine implement mutual exclusion for
** multi-threaded processes.  Only a single thread is allowed to
** executed code that is surrounded by EnterMutex() and LeaveMutex().
**
** SQLite uses only a single Mutex.  There is not much critical
** code and what little there is executes quickly and without blocking.
**
** As of version 3.3.2, this mutex must be recursive.
*/
void sqlite3UnixEnterMutex(){
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_lock(&mutexAux);
  if( !mutexOwnerValid || !pthread_equal(mutexOwner, pthread_self()) ){
    pthread_mutex_unlock(&mutexAux);
    pthread_mutex_lock(&mutexMain);
    assert( inMutex==0 );
    assert( !mutexOwnerValid );
    pthread_mutex_lock(&mutexAux);
    mutexOwner = pthread_self();
    mutexOwnerValid = 1;
  }
  inMutex++;
  pthread_mutex_unlock(&mutexAux);
#else
  inMutex++;
#endif
}
void sqlite3UnixLeaveMutex(){
  assert( inMutex>0 );
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_lock(&mutexAux);
  inMutex--;
  assert( pthread_equal(mutexOwner, pthread_self()) );
  if( inMutex==0 ){
    assert( mutexOwnerValid );
    mutexOwnerValid = 0;
    pthread_mutex_unlock(&mutexMain);
  }
  pthread_mutex_unlock(&mutexAux);
#else
  inMutex--;
#endif
}

/*
** Return TRUE if the mutex is currently held.
**
** If the thisThrd parameter is true, return true only if the
** calling thread holds the mutex.  If the parameter is false, return
** true if any thread holds the mutex.
*/
int sqlite3UnixInMutex(int thisThrd){
#ifdef SQLITE_UNIX_THREADS
  int rc;
  pthread_mutex_lock(&mutexAux);
  rc = inMutex>0 && (thisThrd==0 || pthread_equal(mutexOwner,pthread_self()));
  pthread_mutex_unlock(&mutexAux);
  return rc;
#else
  return inMutex>0;
#endif
}

/*
** Remember the number of thread-specific-data blocks allocated.
** Use this to verify that we are not leaking thread-specific-data.
** Ticket #1601
*/
#ifdef SQLITE_TEST
int sqlite3_tsd_count = 0;
# ifdef SQLITE_UNIX_THREADS
    static pthread_mutex_t tsd_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
#   define TSD_COUNTER(N) \
             pthread_mutex_lock(&tsd_counter_mutex); \
             sqlite3_tsd_count += N; \
             pthread_mutex_unlock(&tsd_counter_mutex);
# else
#   define TSD_COUNTER(N)  sqlite3_tsd_count += N
# endif
#else
# define TSD_COUNTER(N)  /* no-op */
#endif

/*
** If called with allocateFlag>0, then return a pointer to thread
** specific data for the current thread.  Allocate and zero the
** thread-specific data if it does not already exist.
**
** If called with allocateFlag==0, then check the current thread
** specific data.  Return it if it exists.  If it does not exist,
** then return NULL.
**
** If called with allocateFlag<0, check to see if the thread specific
** data is allocated and is all zero.  If it is then deallocate it.
** Return a pointer to the thread specific data or NULL if it is
** unallocated or gets deallocated.
*/
ThreadData *sqlite3UnixThreadSpecificData(int allocateFlag){
  static const ThreadData zeroData = {0};  /* Initializer to silence warnings
                                           ** from broken compilers */
#ifdef SQLITE_UNIX_THREADS
  static pthread_key_t key;
  static int keyInit = 0;
  ThreadData *pTsd;

  if( !keyInit ){
    sqlite3OsEnterMutex();
    if( !keyInit ){
      int rc;
      rc = pthread_key_create(&key, 0);
      if( rc ){
        sqlite3OsLeaveMutex();
        return 0;
      }
      keyInit = 1;
    }
    sqlite3OsLeaveMutex();
  }

  pTsd = pthread_getspecific(key);
  if( allocateFlag>0 ){
    if( pTsd==0 ){
      if( !sqlite3TestMallocFail() ){
        pTsd = sqlite3OsMalloc(sizeof(zeroData));
      }
#ifdef SQLITE_MEMDEBUG
      sqlite3_isFail = 0;
#endif
      if( pTsd ){
        *pTsd = zeroData;
        pthread_setspecific(key, pTsd);
        TSD_COUNTER(+1);
      }
    }
  }else if( pTsd!=0 && allocateFlag<0 
            && memcmp(pTsd, &zeroData, sizeof(ThreadData))==0 ){
    sqlite3OsFree(pTsd);
    pthread_setspecific(key, 0);
    TSD_COUNTER(-1);
    pTsd = 0;
  }
  return pTsd;
#else
  static ThreadData *pTsd = 0;
  if( allocateFlag>0 ){
    if( pTsd==0 ){
      if( !sqlite3TestMallocFail() ){
        pTsd = sqlite3OsMalloc( sizeof(zeroData) );
      }
#ifdef SQLITE_MEMDEBUG
      sqlite3_isFail = 0;
#endif
      if( pTsd ){
        *pTsd = zeroData;
        TSD_COUNTER(+1);
      }
    }
  }else if( pTsd!=0 && allocateFlag<0
            && memcmp(pTsd, &zeroData, sizeof(ThreadData))==0 ){
    sqlite3OsFree(pTsd);
    TSD_COUNTER(-1);
    pTsd = 0;
  }
  return pTsd;
#endif
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
int sqlite3UnixCurrentTime(double *prNow){
#ifdef NO_GETTOD
  time_t t;
  time(&t);
  *prNow = t/86400.0 + 2440587.5;
#else
  struct timeval sNow;
  gettimeofday(&sNow, 0);
  *prNow = 2440587.5 + sNow.tv_sec/86400.0 + sNow.tv_usec/86400000000.0;
#endif
#ifdef SQLITE_TEST
  if( sqlite3_current_time ){
    *prNow = sqlite3_current_time/86400.0 + 2440587.5;
  }
#endif
  return 0;
}

#endif /* OS_UNIX */
