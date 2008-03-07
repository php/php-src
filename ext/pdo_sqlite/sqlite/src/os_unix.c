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
#if SQLITE_THREADSAFE
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
** Maximum supported path-length.
*/
#define MAX_PATHNAME 512


/*
** The unixFile structure is subclass of sqlite3_file specific for the unix
** protability layer.
*/
typedef struct unixFile unixFile;
struct unixFile {
  sqlite3_io_methods const *pMethod;  /* Always the first entry */
#ifdef SQLITE_TEST
  /* In test mode, increase the size of this structure a bit so that 
  ** it is larger than the struct CrashFile defined in test6.c.
  */
  char aPadding[32];
#endif
  struct openCnt *pOpen;    /* Info about all open fd's on this inode */
  struct lockInfo *pLock;   /* Info about locks on this inode */
#ifdef SQLITE_ENABLE_LOCKING_STYLE
  void *lockingContext;     /* Locking style specific state */
#endif /* SQLITE_ENABLE_LOCKING_STYLE */
  int h;                    /* The file descriptor */
  unsigned char locktype;   /* The type of lock held on this fd */
  int dirfd;                /* File descriptor for the directory */
#if SQLITE_THREADSAFE
  pthread_t tid;            /* The thread that "owns" this unixFile */
#endif
};

/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

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
** DJGPP.  But it is DOS - what did you expect?
*/
#ifdef __DJGPP__
# define fcntl(A,B,C) 0
#endif

/*
** The threadid macro resolves to the thread-id or to 0.  Used for
** testing and debugging only.
*/
#if SQLITE_THREADSAFE
#define threadid pthread_self()
#else
#define threadid 0
#endif

/*
** Set or check the unixFile.tid field.  This field is set when an unixFile
** is first opened.  All subsequent uses of the unixFile verify that the
** same thread is operating on the unixFile.  Some operating systems do
** not allow locks to be overridden by other threads and that restriction
** means that sqlite3* database handles cannot be moved from one thread
** to another.  This logic makes sure a user does not try to do that
** by mistake.
**
** Version 3.3.1 (2006-01-15):  unixFile can be moved from one thread to
** another as long as we are running on a system that supports threads
** overriding each others locks (which now the most common behavior)
** or if no locks are held.  But the unixFile.pLock field needs to be
** recomputed because its key includes the thread-id.  See the 
** transferOwnership() function below for additional information
*/
#if SQLITE_THREADSAFE
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
** The sqlite3_file structure for POSIX is no longer just an integer file
** descriptor.  It is now a structure that holds the integer file
** descriptor and a pointer to a structure that describes the internal
** locks on the corresponding inode.  There is one locking structure
** per inode, so if the same inode is opened twice, both unixFile structures
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
** released.  To work around this problem, each unixFile structure contains
** a pointer to an openCnt structure.  There is one openCnt structure
** per open inode, which means that multiple unixFile can point to a single
** openCnt.  When an attempt is made to close an unixFile, if there are
** other unixFile open on the same inode that are holding locks, the call
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
#if SQLITE_THREADSAFE
  pthread_t tid;   /* Thread ID or zero if threads can override each other */
#endif
};

/*
** An instance of the following structure is allocated for each open
** inode on each thread with a different process ID.  (Threads have
** different process IDs on linux, but not on most other unixes.)
**
** A single inode can have multiple file descriptors, so each unixFile
** structure contains a pointer to an instance of this object and this
** object keeps a count of the number of unixFile pointing to it.
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
static Hash lockHash = {SQLITE_HASH_BINARY, 0, 0, 0, 0, 0};
static Hash openHash = {SQLITE_HASH_BINARY, 0, 0, 0, 0, 0};

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

/*
** Helper functions to obtain and relinquish the global mutex.
*/
static void enterMutex(){
  sqlite3_mutex_enter(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MASTER));
}
static void leaveMutex(){
  sqlite3_mutex_leave(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MASTER));
}

#if SQLITE_THREADSAFE
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
#endif /* SQLITE_THREADSAFE */

/*
** Release a lockInfo structure previously allocated by findLockInfo().
*/
static void releaseLockInfo(struct lockInfo *pLock){
  if (pLock == NULL)
    return;
  pLock->nRef--;
  if( pLock->nRef==0 ){
    sqlite3HashInsert(&lockHash, &pLock->key, sizeof(pLock->key), 0);
    sqlite3_free(pLock);
  }
}

/*
** Release a openCnt structure previously allocated by findLockInfo().
*/
static void releaseOpenCnt(struct openCnt *pOpen){
  if (pOpen == NULL)
    return;
  pOpen->nRef--;
  if( pOpen->nRef==0 ){
    sqlite3HashInsert(&openHash, &pOpen->key, sizeof(pOpen->key), 0);
    free(pOpen->aPending);
    sqlite3_free(pOpen);
  }
}

#ifdef SQLITE_ENABLE_LOCKING_STYLE
/*
** Tests a byte-range locking query to see if byte range locks are 
** supported, if not we fall back to dotlockLockingStyle.
*/
static sqlite3LockingStyle sqlite3TestLockingStyle(
  const char *filePath, 
  int fd
){
  /* test byte-range lock using fcntl */
  struct flock lockInfo;
  
  lockInfo.l_len = 1;
  lockInfo.l_start = 0;
  lockInfo.l_whence = SEEK_SET;
  lockInfo.l_type = F_RDLCK;
  
  if( fcntl(fd, F_GETLK, &lockInfo)!=-1 ) {
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
** appropriate locking style based on its value.  These values and 
** assignments are based on Darwin/OSX behavior and have not been tested on 
** other systems.
*/
static sqlite3LockingStyle sqlite3DetectLockingStyle(
  const char *filePath, 
  int fd
){

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
#endif /* SQLITE_FIXED_LOCKING_STYLE */
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

  memset(&key1, 0, sizeof(key1));
  key1.dev = statbuf.st_dev;
  key1.ino = statbuf.st_ino;
#if SQLITE_THREADSAFE
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
    pLock = sqlite3_malloc( sizeof(*pLock) );
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
      sqlite3_free(pLock);
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
      pOpen = sqlite3_malloc( sizeof(*pOpen) );
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
        sqlite3_free(pOpen);
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
#if SQLITE_THREADSAFE
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
** Seek to the offset passed as the second argument, then read cnt 
** bytes into pBuf. Return the number of bytes actually read.
**
** NB:  If you define USE_PREAD or USE_PREAD64, then it might also
** be necessary to define _XOPEN_SOURCE to be 500.  This varies from
** one system to another.  Since SQLite does not define USE_PREAD
** any any form by default, we will not attempt to define _XOPEN_SOURCE.
** See tickets #2741 and #2681.
*/
static int seekAndRead(unixFile *id, sqlite3_int64 offset, void *pBuf, int cnt){
  int got;
  i64 newOffset;
  TIMER_START;
#if defined(USE_PREAD)
  got = pread(id->h, pBuf, cnt, offset);
  SimulateIOError( got = -1 );
#elif defined(USE_PREAD64)
  got = pread64(id->h, pBuf, cnt, offset);
  SimulateIOError( got = -1 );
#else
  newOffset = lseek(id->h, offset, SEEK_SET);
  SimulateIOError( newOffset-- );
  if( newOffset!=offset ){
    return -1;
  }
  got = read(id->h, pBuf, cnt);
#endif
  TIMER_END;
  OSTRACE5("READ    %-3d %5d %7lld %d\n", id->h, got, offset, TIMER_ELAPSED);
  return got;
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
static int unixRead(
  sqlite3_file *id, 
  void *pBuf, 
  int amt,
  sqlite3_int64 offset
){
  int got;
  assert( id );
  got = seekAndRead((unixFile*)id, offset, pBuf, amt);
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
static int seekAndWrite(unixFile *id, i64 offset, const void *pBuf, int cnt){
  int got;
  i64 newOffset;
  TIMER_START;
#if defined(USE_PREAD)
  got = pwrite(id->h, pBuf, cnt, offset);
#elif defined(USE_PREAD64)
  got = pwrite64(id->h, pBuf, cnt, offset);
#else
  newOffset = lseek(id->h, offset, SEEK_SET);
  if( newOffset!=offset ){
    return -1;
  }
  got = write(id->h, pBuf, cnt);
#endif
  TIMER_END;
  OSTRACE5("WRITE   %-3d %5d %7lld %d\n", id->h, got, offset, TIMER_ELAPSED);
  return got;
}


/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
static int unixWrite(
  sqlite3_file *id, 
  const void *pBuf, 
  int amt,
  sqlite3_int64 offset 
){
  int wrote = 0;
  assert( id );
  assert( amt>0 );
  while( amt>0 && (wrote = seekAndWrite((unixFile*)id, offset, pBuf, amt))>0 ){
    amt -= wrote;
    offset += wrote;
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
static int unixSync(sqlite3_file *id, int flags){
  int rc;
  unixFile *pFile = (unixFile*)id;

  int isDataOnly = (flags&SQLITE_SYNC_DATAONLY);
  int isFullsync = (flags&0x0F)==SQLITE_SYNC_FULL;

  /* Check that one of SQLITE_SYNC_NORMAL or FULL was passed */
  assert((flags&0x0F)==SQLITE_SYNC_NORMAL
      || (flags&0x0F)==SQLITE_SYNC_FULL
  );

  assert( pFile );
  OSTRACE2("SYNC    %-3d\n", pFile->h);
  rc = full_fsync(pFile->h, isFullsync, isDataOnly);
  SimulateIOError( rc=1 );
  if( rc ){
    return SQLITE_IOERR_FSYNC;
  }
  if( pFile->dirfd>=0 ){
    OSTRACE4("DIRSYNC %-3d (have_fullfsync=%d fullsync=%d)\n", pFile->dirfd,
            HAVE_FULLFSYNC, isFullsync);
#ifndef SQLITE_DISABLE_DIRSYNC
    /* The directory sync is only attempted if full_fsync is
    ** turned off or unavailable.  If a full_fsync occurred above,
    ** then the directory sync is superfluous.
    */
    if( (!HAVE_FULLFSYNC || !isFullsync) && full_fsync(pFile->dirfd,0,0) ){
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
** Truncate an open file to a specified size
*/
static int unixTruncate(sqlite3_file *id, i64 nByte){
  int rc;
  assert( id );
  SimulateIOError( return SQLITE_IOERR_TRUNCATE );
  rc = ftruncate(((unixFile*)id)->h, (off_t)nByte);
  if( rc ){
    return SQLITE_IOERR_TRUNCATE;
  }else{
    return SQLITE_OK;
  }
}

/*
** Determine the current size of a file in bytes
*/
static int unixFileSize(sqlite3_file *id, i64 *pSize){
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
static int unixCheckReservedLock(sqlite3_file *id){
  int r = 0;
  unixFile *pFile = (unixFile*)id;

  assert( pFile );
  enterMutex(); /* Because pFile->pLock is shared across threads */

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
  
  leaveMutex();
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
static int unixLock(sqlite3_file *id, int locktype){
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
  ** unixFile, do nothing. Don't use the end_lock: exit path, as
  ** enterMutex() hasn't been called yet.
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
  enterMutex();

  /* Make sure the current thread owns the pFile.
  */
  rc = transferOwnership(pFile);
  if( rc!=SQLITE_OK ){
    leaveMutex();
    return rc;
  }
  pLock = pFile->pLock;

  /* If some thread using this PID has a lock via a different unixFile*
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
  leaveMutex();
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
static int unixUnlock(sqlite3_file *id, int locktype){
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
  enterMutex();
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
  leaveMutex();
  pFile->locktype = locktype;
  return rc;
}

/*
** Close a file.
*/
static int unixClose(sqlite3_file *id){
  unixFile *pFile = (unixFile *)id;
  if( !pFile ) return SQLITE_OK;
  unixUnlock(id, NO_LOCK);
  if( pFile->dirfd>=0 ) close(pFile->dirfd);
  pFile->dirfd = -1;
  enterMutex();

  if( pFile->pOpen->nLock ){
    /* If there are outstanding locks, do not actually close the file just
    ** yet because that would clear those locks.  Instead, add the file
    ** descriptor to pOpen->aPending.  It will be automatically closed when
    ** the last lock is cleared.
    */
    int *aNew;
    struct openCnt *pOpen = pFile->pOpen;
    aNew = realloc( pOpen->aPending, (pOpen->nPending+1)*sizeof(int) );
    if( aNew==0 ){
      /* If a malloc fails, just leak the file descriptor */
    }else{
      pOpen->aPending = aNew;
      pOpen->aPending[pOpen->nPending] = pFile->h;
      pOpen->nPending++;
    }
  }else{
    /* There are no outstanding locks so we can close the file immediately */
    close(pFile->h);
  }
  releaseLockInfo(pFile->pLock);
  releaseOpenCnt(pFile->pOpen);

  leaveMutex();
  OSTRACE2("CLOSE   %-3d\n", pFile->h);
  OpenCounter(-1);
  memset(pFile, 0, sizeof(unixFile));
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

#define afpfsByteRangeLock2FSCTL        _IOWR('z', 23, struct ByteRangeLockPB2)

/* 
** Return 0 on success, 1 on failure.  To match the behavior of the 
** normal posix file locking (used in unixLock for example), we should 
** provide 'richer' return codes - specifically to differentiate between
** 'file busy' and 'file system error' results.
*/
static int _AFPFSSetLock(
  const char *path, 
  int fd, 
  unsigned long long offset, 
  unsigned long long length, 
  int setLockFlag
){
  struct ByteRangeLockPB2       pb;
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
    return 1; /* error */
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
static int afpUnixCheckReservedLock(sqlite3_file *id){
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
    /* lock the byte */
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
static int afpUnixLock(sqlite3_file *id, int locktype)
{
  int rc = SQLITE_OK;
  unixFile *pFile = (unixFile*)id;
  afpLockingContext *context = (afpLockingContext *) pFile->lockingContext;
  int gotPendingLock = 0;
  
  assert( pFile );
  OSTRACE5("LOCK    %d %s was %s pid=%d\n", pFile->h,
         locktypeName(locktype), locktypeName(pFile->locktype), getpid());  
  /* If there is already a lock of this type or more restrictive on the
    ** unixFile, do nothing. Don't use the afp_end_lock: exit path, as
    ** enterMutex() hasn't been called yet.
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
  enterMutex();

  /* Make sure the current thread owns the pFile.
    */
  rc = transferOwnership(pFile);
  if( rc!=SQLITE_OK ){
    leaveMutex();
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
    leaveMutex();
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
static int afpUnixUnlock(sqlite3_file *id, int locktype) {
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
  enterMutex();
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
  leaveMutex();
  return rc;
}

/*
 ** Close a file & cleanup AFP specific locking context 
 */
static int afpUnixClose(sqlite3_file *id) {
  unixFile *pFile = (unixFile*)pId;

  if( !pFile ) return SQLITE_OK;
  afpUnixUnlock(*pId, NO_LOCK);
  /* free the AFP locking structure */
  if (pFile->lockingContext != NULL) {
    if (((afpLockingContext *)pFile->lockingContext)->filePath != NULL)
      sqlite3_free(((afpLockingContext*)pFile->lockingContext)->filePath);
    sqlite3_free(pFile->lockingContext);
  }

  if( pFile->dirfd>=0 ) close(pFile->dirfd);
  pFile->dirfd = -1;
  close(pFile->h);
  OSTRACE2("CLOSE   %-3d\n", pFile->h);
  OpenCounter(-1);
  return SQLITE_OK;
}


#pragma mark flock() style locking

/*
 ** The flockLockingContext is not used
 */
typedef void flockLockingContext;

static int flockUnixCheckReservedLock(sqlite3_file *id) {
  unixFile *pFile = (unixFile*)id;
  
  if (pFile->locktype == RESERVED_LOCK) {
    return 1; /* already have a reserved lock */
  } else {
    /* attempt to get the lock */
    int rc = flock(pFile->h, LOCK_EX | LOCK_NB);
    if (!rc) {
      /* got the lock, unlock it */
      flock(pFile->h, LOCK_UN);
      return 0;  /* no one has it reserved */
    }
    return 1; /* someone else might have it reserved */
  }
}

static int flockUnixLock(sqlite3_file *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  
  /* if we already have a lock, it is exclusive.  
  ** Just adjust level and punt on outta here. */
  if (pFile->locktype > NO_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  /* grab an exclusive lock */
  int rc = flock(pFile->h, LOCK_EX | LOCK_NB);
  if (rc) {
    /* didn't get, must be busy */
    return SQLITE_BUSY;
  } else {
    /* got it, set the type and return ok */
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
}

static int flockUnixUnlock(sqlite3_file *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  
  assert( locktype<=SHARED_LOCK );
  
  /* no-op if possible */
  if( pFile->locktype==locktype ){
    return SQLITE_OK;
  }
  
  /* shared can just be set because we always have an exclusive */
  if (locktype==SHARED_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  /* no, really, unlock. */
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
static int flockUnixClose(sqlite3_file *pId) {
  unixFile *pFile = (unixFile*)*pId;
  
  if( !pFile ) return SQLITE_OK;
  flockUnixUnlock(*pId, NO_LOCK);
  
  if( pFile->dirfd>=0 ) close(pFile->dirfd);
  pFile->dirfd = -1;
  enterMutex();
  
  close(pFile->h);  
  leaveMutex();
  OSTRACE2("CLOSE   %-3d\n", pFile->h);
  OpenCounter(-1);
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


static int dotlockUnixCheckReservedLock(sqlite3_file *id) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  if (pFile->locktype == RESERVED_LOCK) {
    return 1; /* already have a reserved lock */
  } else {
    struct stat statBuf;
    if (lstat(context->lockPath,&statBuf) == 0)
      /* file exists, someone else has the lock */
      return 1;
    else
      /* file does not exist, we could have it if we want it */
      return 0;
  }
}

static int dotlockUnixLock(sqlite3_file *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  /* if we already have a lock, it is exclusive.  
  ** Just adjust level and punt on outta here. */
  if (pFile->locktype > NO_LOCK) {
    pFile->locktype = locktype;
    
    /* Always update the timestamp on the old file */
    utimes(context->lockPath,NULL);
    return SQLITE_OK;
  }
  
  /* check to see if lock file already exists */
  struct stat statBuf;
  if (lstat(context->lockPath,&statBuf) == 0){
    return SQLITE_BUSY; /* it does, busy */
  }
  
  /* grab an exclusive lock */
  int fd = open(context->lockPath,O_RDONLY|O_CREAT|O_EXCL,0600);
  if (fd < 0) {
    /* failed to open/create the file, someone else may have stolen the lock */
    return SQLITE_BUSY; 
  }
  close(fd);
  
  /* got it, set the type and return ok */
  pFile->locktype = locktype;
  return SQLITE_OK;
}

static int dotlockUnixUnlock(sqlite3_file *id, int locktype) {
  unixFile *pFile = (unixFile*)id;
  dotlockLockingContext *context = 
    (dotlockLockingContext *) pFile->lockingContext;
  
  assert( locktype<=SHARED_LOCK );
  
  /* no-op if possible */
  if( pFile->locktype==locktype ){
    return SQLITE_OK;
  }
  
  /* shared can just be set because we always have an exclusive */
  if (locktype==SHARED_LOCK) {
    pFile->locktype = locktype;
    return SQLITE_OK;
  }
  
  /* no, really, unlock. */
  unlink(context->lockPath);
  pFile->locktype = NO_LOCK;
  return SQLITE_OK;
}

/*
 ** Close a file.
 */
static int dotlockUnixClose(sqlite3_file *id) {
  unixFile *pFile = (unixFile*)id;
  
  if( !pFile ) return SQLITE_OK;
  dotlockUnixUnlock(*pId, NO_LOCK);
  /* free the dotlock locking structure */
  if (pFile->lockingContext != NULL) {
    if (((dotlockLockingContext *)pFile->lockingContext)->lockPath != NULL)
      sqlite3_free( ( (dotlockLockingContext *)
        pFile->lockingContext)->lockPath);
    sqlite3_free(pFile->lockingContext);
  }
  
  if( pFile->dirfd>=0 ) close(pFile->dirfd);
  pFile->dirfd = -1;
  enterMutex();
  
  close(pFile->h);
  
  leaveMutex();
  OSTRACE2("CLOSE   %-3d\n", pFile->h);
  OpenCounter(-1);
  return SQLITE_OK;
}


#pragma mark No locking

/*
 ** The nolockLockingContext is void
 */
typedef void nolockLockingContext;

static int nolockUnixCheckReservedLock(sqlite3_file *id) {
  return 0;
}

static int nolockUnixLock(sqlite3_file *id, int locktype) {
  return SQLITE_OK;
}

static int nolockUnixUnlock(sqlite3_file *id, int locktype) {
  return SQLITE_OK;
}

/*
 ** Close a file.
 */
static int nolockUnixClose(sqlite3_file *id) {
  unixFile *pFile = (unixFile*)id;
  
  if( !pFile ) return SQLITE_OK;
  if( pFile->dirfd>=0 ) close(pFile->dirfd);
  pFile->dirfd = -1;
  enterMutex();
  
  close(pFile->h);
  
  leaveMutex();
  OSTRACE2("CLOSE   %-3d\n", pFile->h);
  OpenCounter(-1);
  return SQLITE_OK;
}

#endif /* SQLITE_ENABLE_LOCKING_STYLE */


/*
** Information and control of an open file handle.
*/
static int unixFileControl(sqlite3_file *id, int op, void *pArg){
  switch( op ){
    case SQLITE_FCNTL_LOCKSTATE: {
      *(int*)pArg = ((unixFile*)id)->locktype;
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
static int unixSectorSize(sqlite3_file *id){
  return SQLITE_DEFAULT_SECTOR_SIZE;
}

/*
** Return the device characteristics for the file. This is always 0.
*/
static int unixDeviceCharacteristics(sqlite3_file *id){
  return 0;
}

/*
** This vector defines all the methods that can operate on an sqlite3_file
** for unix.
*/
static const sqlite3_io_methods sqlite3UnixIoMethod = {
  1,                        /* iVersion */
  unixClose,
  unixRead,
  unixWrite,
  unixTruncate,
  unixSync,
  unixFileSize,
  unixLock,
  unixUnlock,
  unixCheckReservedLock,
  unixFileControl,
  unixSectorSize,
  unixDeviceCharacteristics
};

#ifdef SQLITE_ENABLE_LOCKING_STYLE
/*
** This vector defines all the methods that can operate on an sqlite3_file
** for unix with AFP style file locking.
*/
static const sqlite3_io_methods sqlite3AFPLockingUnixIoMethod = {
  1,                        /* iVersion */
  unixClose,
  unixRead,
  unixWrite,
  unixTruncate,
  unixSync,
  unixFileSize,
  afpUnixLock,
  afpUnixUnlock,
  afpUnixCheckReservedLock,
  unixFileControl,
  unixSectorSize,
  unixDeviceCharacteristics
};

/*
** This vector defines all the methods that can operate on an sqlite3_file
** for unix with flock() style file locking.
*/
static const sqlite3_io_methods sqlite3FlockLockingUnixIoMethod = {
  1,                        /* iVersion */
  flockUnixClose,
  unixRead,
  unixWrite,
  unixTruncate,
  unixSync,
  unixFileSize,
  flockUnixLock,
  flockUnixUnlock,
  flockUnixCheckReservedLock,
  unixFileControl,
  unixSectorSize,
  unixDeviceCharacteristics
};

/*
** This vector defines all the methods that can operate on an sqlite3_file
** for unix with dotlock style file locking.
*/
static const sqlite3_io_methods sqlite3DotlockLockingUnixIoMethod = {
  1,                        /* iVersion */
  dotlockUnixClose,
  unixRead,
  unixWrite,
  unixTruncate,
  unixSync,
  unixFileSize,
  dotlockUnixLock,
  dotlockUnixUnlock,
  dotlockUnixCheckReservedLock,
  unixFileControl,
  unixSectorSize,
  unixDeviceCharacteristics
};

/*
** This vector defines all the methods that can operate on an sqlite3_file
** for unix with dotlock style file locking.
*/
static const sqlite3_io_methods sqlite3NolockLockingUnixIoMethod = {
  1,                        /* iVersion */
  nolockUnixClose,
  unixRead,
  unixWrite,
  unixTruncate,
  unixSync,
  unixFileSize,
  nolockUnixLock,
  nolockUnixUnlock,
  nolockUnixCheckReservedLock,
  unixFileControl,
  unixSectorSize,
  unixDeviceCharacteristics
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
static int fillInUnixFile(
  int h,                  /* Open file descriptor of file being opened */
  int dirfd,              /* Directory file descriptor */
  sqlite3_file *pId,      /* Write completed initialization here */
  const char *zFilename,  /* Name of the file being opened */
){
  sqlite3LockingStyle lockingStyle;
  unixFile *pNew = (unixFile *)pId;
  int rc;

  memset(pNew, 0, sizeof(unixFile));
  lockingStyle = sqlite3DetectLockingStyle(zFilename, h);
  if ( lockingStyle == posixLockingStyle ) {
    enterMutex();
    rc = findLockInfo(h, &pNew->pLock, &pNew->pOpen);
    leaveMutex();
    if( rc ){
      close(h);
      unlink(zFilename);
      return SQLITE_NOMEM;
    }
  } else {
    /*  pLock and pOpen are only used for posix advisory locking */
    pNew->pLock = NULL;
    pNew->pOpen = NULL;
  }
  pNew->dirfd = -1;
  pNew->h = h;
  SET_THREADID(pNew);
  pNew = sqlite3_malloc( sizeof(unixFile) );
  if( pNew==0 ){
    close(h);
    enterMutex();
    releaseLockInfo(pNew->pLock);
    releaseOpenCnt(pNew->pOpen);
    leaveMutex();
    return SQLITE_NOMEM;
  }else{
    switch(lockingStyle) {
      case afpLockingStyle: {
        /* afp locking uses the file path so it needs to be included in
        ** the afpLockingContext */
        int nFilename;
        pNew->pMethod = &sqlite3AFPLockingUnixIoMethod;
        pNew->lockingContext = 
          sqlite3_malloc(sizeof(afpLockingContext));
        nFilename = strlen(zFilename)+1;
        ((afpLockingContext *)pNew->lockingContext)->filePath = 
          sqlite3_malloc(nFilename);
        memcpy(((afpLockingContext *)pNew->lockingContext)->filePath, 
               zFilename, nFilename);
        srandomdev();
        break;
      }
      case flockLockingStyle:
        /* flock locking doesn't need additional lockingContext information */
        pNew->pMethod = &sqlite3FlockLockingUnixIoMethod;
        break;
      case dotlockLockingStyle: {
        /* dotlock locking uses the file path so it needs to be included in
         ** the dotlockLockingContext */
        int nFilename;
        pNew->pMethod = &sqlite3DotlockLockingUnixIoMethod;
        pNew->lockingContext = sqlite3_malloc(
          sizeof(dotlockLockingContext));
        nFilename = strlen(zFilename) + 6;
        ((dotlockLockingContext *)pNew->lockingContext)->lockPath = 
            sqlite3_malloc( nFilename );
        sqlite3_snprintf(nFilename, 
                ((dotlockLockingContext *)pNew->lockingContext)->lockPath, 
                "%s.lock", zFilename);
        break;
      }
      case posixLockingStyle:
        /* posix locking doesn't need additional lockingContext information */
        pNew->pMethod = &sqlite3UnixIoMethod;
        break;
      case noLockingStyle:
      case unsupportedLockingStyle:
      default: 
        pNew->pMethod = &sqlite3NolockLockingUnixIoMethod;
    }
    OpenCounter(+1);
    return SQLITE_OK;
  }
}
#else /* SQLITE_ENABLE_LOCKING_STYLE */
static int fillInUnixFile(
  int h,                 /* Open file descriptor on file being opened */
  int dirfd,
  sqlite3_file *pId,     /* Write to the unixFile structure here */
  const char *zFilename  /* Name of the file being opened */
){
  unixFile *pNew = (unixFile *)pId;
  int rc;

#ifdef FD_CLOEXEC
  fcntl(h, F_SETFD, fcntl(h, F_GETFD, 0) | FD_CLOEXEC);
#endif

  enterMutex();
  rc = findLockInfo(h, &pNew->pLock, &pNew->pOpen);
  leaveMutex();
  if( rc ){
    if( dirfd>=0 ) close(dirfd);
    close(h);
    return SQLITE_NOMEM;
  }

  OSTRACE3("OPEN    %-3d %s\n", h, zFilename);
  pNew->dirfd = -1;
  pNew->h = h;
  pNew->dirfd = dirfd;
  SET_THREADID(pNew);

  pNew->pMethod = &sqlite3UnixIoMethod;
  OpenCounter(+1);
  return SQLITE_OK;
}
#endif /* SQLITE_ENABLE_LOCKING_STYLE */

/*
** Open a file descriptor to the directory containing file zFilename.
** If successful, *pFd is set to the opened file descriptor and
** SQLITE_OK is returned. If an error occurs, either SQLITE_NOMEM
** or SQLITE_CANTOPEN is returned and *pFd is set to an undefined
** value.
**
** If SQLITE_OK is returned, the caller is responsible for closing
** the file descriptor *pFd using close().
*/
static int openDirectory(const char *zFilename, int *pFd){
  int ii;
  int fd = -1;
  char zDirname[MAX_PATHNAME+1];

  sqlite3_snprintf(MAX_PATHNAME, zDirname, "%s", zFilename);
  for(ii=strlen(zDirname); ii>=0 && zDirname[ii]!='/'; ii--);
  if( ii>0 ){
    zDirname[ii] = '\0';
    fd = open(zDirname, O_RDONLY|O_BINARY, 0);
    if( fd>=0 ){
#ifdef FD_CLOEXEC
      fcntl(fd, F_SETFD, fcntl(fd, F_GETFD, 0) | FD_CLOEXEC);
#endif
      OSTRACE3("OPENDIR %-3d %s\n", fd, zDirname);
    }
  }
  *pFd = fd;
  return (fd>=0?SQLITE_OK:SQLITE_CANTOPEN);
}

/*
** Open the file zPath.
** 
** Previously, the SQLite OS layer used three functions in place of this
** one:
**
**     sqlite3OsOpenReadWrite();
**     sqlite3OsOpenReadOnly();
**     sqlite3OsOpenExclusive();
**
** These calls correspond to the following combinations of flags:
**
**     ReadWrite() ->     (READWRITE | CREATE)
**     ReadOnly()  ->     (READONLY) 
**     OpenExclusive() -> (READWRITE | CREATE | EXCLUSIVE)
**
** The old OpenExclusive() accepted a boolean argument - "delFlag". If
** true, the file was configured to be automatically deleted when the
** file handle closed. To achieve the same effect using this new 
** interface, add the DELETEONCLOSE flag to those specified above for 
** OpenExclusive().
*/
static int unixOpen(
  sqlite3_vfs *pVfs, 
  const char *zPath, 
  sqlite3_file *pFile,
  int flags,
  int *pOutFlags
){
  int fd = 0;                    /* File descriptor returned by open() */
  int dirfd = -1;                /* Directory file descriptor */
  int oflags = 0;                /* Flags to pass to open() */
  int eType = flags&0xFFFFFF00;  /* Type of file to open */

  int isExclusive  = (flags & SQLITE_OPEN_EXCLUSIVE);
  int isDelete     = (flags & SQLITE_OPEN_DELETEONCLOSE);
  int isCreate     = (flags & SQLITE_OPEN_CREATE);
  int isReadonly   = (flags & SQLITE_OPEN_READONLY);
  int isReadWrite  = (flags & SQLITE_OPEN_READWRITE);

  /* If creating a master or main-file journal, this function will open
  ** a file-descriptor on the directory too. The first time unixSync()
  ** is called the directory file descriptor will be fsync()ed and close()d.
  */
  int isOpenDirectory = (isCreate && 
      (eType==SQLITE_OPEN_MASTER_JOURNAL || eType==SQLITE_OPEN_MAIN_JOURNAL)
  );

  /* Check the following statements are true: 
  **
  **   (a) Exactly one of the READWRITE and READONLY flags must be set, and 
  **   (b) if CREATE is set, then READWRITE must also be set, and
  **   (c) if EXCLUSIVE is set, then CREATE must also be set.
  **   (d) if DELETEONCLOSE is set, then CREATE must also be set.
  */
  assert((isReadonly==0 || isReadWrite==0) && (isReadWrite || isReadonly));
  assert(isCreate==0 || isReadWrite);
  assert(isExclusive==0 || isCreate);
  assert(isDelete==0 || isCreate);


  /* The main DB, main journal, and master journal are never automatically
  ** deleted
  */
  assert( eType!=SQLITE_OPEN_MAIN_DB || !isDelete );
  assert( eType!=SQLITE_OPEN_MAIN_JOURNAL || !isDelete );
  assert( eType!=SQLITE_OPEN_MASTER_JOURNAL || !isDelete );

  /* Assert that the upper layer has set one of the "file-type" flags. */
  assert( eType==SQLITE_OPEN_MAIN_DB      || eType==SQLITE_OPEN_TEMP_DB 
       || eType==SQLITE_OPEN_MAIN_JOURNAL || eType==SQLITE_OPEN_TEMP_JOURNAL 
       || eType==SQLITE_OPEN_SUBJOURNAL   || eType==SQLITE_OPEN_MASTER_JOURNAL 
       || eType==SQLITE_OPEN_TRANSIENT_DB
  );

  if( isReadonly )  oflags |= O_RDONLY;
  if( isReadWrite ) oflags |= O_RDWR;
  if( isCreate )    oflags |= O_CREAT;
  if( isExclusive ) oflags |= (O_EXCL|O_NOFOLLOW);
  oflags |= (O_LARGEFILE|O_BINARY);

  memset(pFile, 0, sizeof(unixFile));
  fd = open(zPath, oflags, isDelete?0600:SQLITE_DEFAULT_FILE_PERMISSIONS);
  if( fd<0 && errno!=EISDIR && isReadWrite && !isExclusive ){
    /* Failed to open the file for read/write access. Try read-only. */
    flags &= ~(SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
    flags |= SQLITE_OPEN_READONLY;
    return unixOpen(pVfs, zPath, pFile, flags, pOutFlags);
  }
  if( fd<0 ){
    return SQLITE_CANTOPEN;
  }
  if( isDelete ){
    unlink(zPath);
  }
  if( pOutFlags ){
    *pOutFlags = flags;
  }

  assert(fd!=0);
  if( isOpenDirectory ){
    int rc = openDirectory(zPath, &dirfd);
    if( rc!=SQLITE_OK ){
      close(fd);
      return rc;
    }
  }
  return fillInUnixFile(fd, dirfd, pFile, zPath);
}

/*
** Delete the file at zPath. If the dirSync argument is true, fsync()
** the directory after deleting the file.
*/
static int unixDelete(sqlite3_vfs *pVfs, const char *zPath, int dirSync){
  int rc = SQLITE_OK;
  SimulateIOError(return SQLITE_IOERR_DELETE);
  unlink(zPath);
  if( dirSync ){
    int fd;
    rc = openDirectory(zPath, &fd);
    if( rc==SQLITE_OK ){
      if( fsync(fd) ){
        rc = SQLITE_IOERR_DIR_FSYNC;
      }
      close(fd);
    }
  }
  return rc;
}

/*
** Test the existance of or access permissions of file zPath. The
** test performed depends on the value of flags:
**
**     SQLITE_ACCESS_EXISTS: Return 1 if the file exists
**     SQLITE_ACCESS_READWRITE: Return 1 if the file is read and writable.
**     SQLITE_ACCESS_READONLY: Return 1 if the file is readable.
**
** Otherwise return 0.
*/
static int unixAccess(sqlite3_vfs *pVfs, const char *zPath, int flags){
  int amode = 0;
  switch( flags ){
    case SQLITE_ACCESS_EXISTS:
      amode = F_OK;
      break;
    case SQLITE_ACCESS_READWRITE:
      amode = W_OK|R_OK;
      break;
    case SQLITE_ACCESS_READ:
      amode = R_OK;
      break;

    default:
      assert(!"Invalid flags argument");
  }
  return (access(zPath, amode)==0);
}

/*
** Create a temporary file name in zBuf.  zBuf must be allocated
** by the calling process and must be big enough to hold at least
** pVfs->mxPathname bytes.
*/
static int unixGetTempname(sqlite3_vfs *pVfs, int nBuf, char *zBuf){
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

  /* It's odd to simulate an io-error here, but really this is just
  ** using the io-error infrastructure to test that SQLite handles this
  ** function failing. 
  */
  SimulateIOError( return SQLITE_ERROR );

  azDirs[0] = sqlite3_temp_directory;
  for(i=0; i<sizeof(azDirs)/sizeof(azDirs[0]); i++){
    if( azDirs[i]==0 ) continue;
    if( stat(azDirs[i], &buf) ) continue;
    if( !S_ISDIR(buf.st_mode) ) continue;
    if( access(azDirs[i], 07) ) continue;
    zDir = azDirs[i];
    break;
  }
  if( strlen(zDir) - sizeof(SQLITE_TEMP_FILE_PREFIX) - 17 <=0 ){
    return SQLITE_ERROR;
  }
  do{
    assert( pVfs->mxPathname==MAX_PATHNAME );
    sqlite3_snprintf(nBuf-17, zBuf, "%s/"SQLITE_TEMP_FILE_PREFIX, zDir);
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
** Turn a relative pathname into a full pathname. The relative path
** is stored as a nul-terminated string in the buffer pointed to by
** zPath. 
**
** zOut points to a buffer of at least sqlite3_vfs.mxPathname bytes 
** (in this case, MAX_PATHNAME bytes). The full-path is written to
** this buffer before returning.
*/
static int unixFullPathname(
  sqlite3_vfs *pVfs,            /* Pointer to vfs object */
  const char *zPath,            /* Possibly relative input path */
  int nOut,                     /* Size of output buffer in bytes */
  char *zOut                    /* Output buffer */
){

  /* It's odd to simulate an io-error here, but really this is just
  ** using the io-error infrastructure to test that SQLite handles this
  ** function failing. This function could fail if, for example, the
  ** current working directly has been unlinked.
  */
  SimulateIOError( return SQLITE_ERROR );

  assert( pVfs->mxPathname==MAX_PATHNAME );
  zOut[nOut-1] = '\0';
  if( zPath[0]=='/' ){
    sqlite3_snprintf(nOut, zOut, "%s", zPath);
  }else{
    int nCwd;
    if( getcwd(zOut, nOut-1)==0 ){
      return SQLITE_CANTOPEN;
    }
    nCwd = strlen(zOut);
    sqlite3_snprintf(nOut-nCwd, &zOut[nCwd], "/%s", zPath);
  }
  return SQLITE_OK;

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
}


#ifndef SQLITE_OMIT_LOAD_EXTENSION
/*
** Interfaces for opening a shared library, finding entry points
** within the shared library, and closing the shared library.
*/
#include <dlfcn.h>
static void *unixDlOpen(sqlite3_vfs *pVfs, const char *zFilename){
  return dlopen(zFilename, RTLD_NOW | RTLD_GLOBAL);
}

/*
** SQLite calls this function immediately after a call to unixDlSym() or
** unixDlOpen() fails (returns a null pointer). If a more detailed error
** message is available, it is written to zBufOut. If no error message
** is available, zBufOut is left unmodified and SQLite uses a default
** error message.
*/
static void unixDlError(sqlite3_vfs *pVfs, int nBuf, char *zBufOut){
  char *zErr;
  enterMutex();
  zErr = dlerror();
  if( zErr ){
    sqlite3_snprintf(nBuf, zBufOut, "%s", zErr);
  }
  leaveMutex();
}
static void *unixDlSym(sqlite3_vfs *pVfs, void *pHandle, const char *zSymbol){
  return dlsym(pHandle, zSymbol);
}
static void unixDlClose(sqlite3_vfs *pVfs, void *pHandle){
  dlclose(pHandle);
}
#else /* if SQLITE_OMIT_LOAD_EXTENSION is defined: */
  #define unixDlOpen  0
  #define unixDlError 0
  #define unixDlSym   0
  #define unixDlClose 0
#endif

/*
** Write nBuf bytes of random data to the supplied buffer zBuf.
*/
static int unixRandomness(sqlite3_vfs *pVfs, int nBuf, char *zBuf){

  assert(nBuf>=(sizeof(time_t)+sizeof(int)));

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
  memset(zBuf, 0, nBuf);
#if !defined(SQLITE_TEST)
  {
    int pid, fd;
    fd = open("/dev/urandom", O_RDONLY);
    if( fd<0 ){
      time_t t;
      time(&t);
      memcpy(zBuf, &t, sizeof(t));
      pid = getpid();
      memcpy(&zBuf[sizeof(t)], &pid, sizeof(pid));
    }else{
      read(fd, zBuf, nBuf);
      close(fd);
    }
  }
#endif
  return SQLITE_OK;
}


/*
** Sleep for a little while.  Return the amount of time slept.
** The argument is the number of microseconds we want to sleep.
** The return value is the number of microseconds of sleep actually
** requested from the underlying operating system, a number which
** might be greater than or equal to the argument, but not less
** than the argument.
*/
static int unixSleep(sqlite3_vfs *pVfs, int microseconds){
#if defined(HAVE_USLEEP) && HAVE_USLEEP
  usleep(microseconds);
  return microseconds;
#else
  int seconds = (microseconds+999999)/1000000;
  sleep(seconds);
  return seconds*1000000;
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
static int unixCurrentTime(sqlite3_vfs *pVfs, double *prNow){
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

/*
** Return a pointer to the sqlite3DefaultVfs structure.   We use
** a function rather than give the structure global scope because
** some compilers (MSVC) do not allow forward declarations of
** initialized structures.
*/
sqlite3_vfs *sqlite3OsDefaultVfs(void){
  static sqlite3_vfs unixVfs = {
    1,                  /* iVersion */
    sizeof(unixFile),   /* szOsFile */
    MAX_PATHNAME,       /* mxPathname */
    0,                  /* pNext */
    "unix",             /* zName */
    0,                  /* pAppData */
  
    unixOpen,           /* xOpen */
    unixDelete,         /* xDelete */
    unixAccess,         /* xAccess */
    unixGetTempname,    /* xGetTempName */
    unixFullPathname,   /* xFullPathname */
    unixDlOpen,         /* xDlOpen */
    unixDlError,        /* xDlError */
    unixDlSym,          /* xDlSym */
    unixDlClose,        /* xDlClose */
    unixRandomness,     /* xRandomness */
    unixSleep,          /* xSleep */
    unixCurrentTime     /* xCurrentTime */
  };
  
  return &unixVfs;
}
 
#endif /* OS_UNIX */
