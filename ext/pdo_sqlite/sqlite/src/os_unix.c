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


#include <time.h>
#include <errno.h>
#include <unistd.h>
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
#if defined(THREADSAFE) && THREADSAFE
# include <pthread.h>
# define SQLITE_UNIX_THREADS 1
#endif


/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

#if defined(THREADSAFE) && THREADSAFE && defined(__linux__)
#define getpid pthread_self
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
** each others locks then tid is always set to zero.  tid is also
** set to zero if we compile without threading support.
*/
struct lockKey {
  dev_t dev;       /* Device number */
  ino_t ino;       /* Inode number */
#ifdef SQLITE_UNIX_THREADS
  pthread_t tid;   /* Thread ID or zero if threads cannot override each other */
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
** These hash table maps inodes and process IDs into lockInfo and openCnt
** structures.  Access to these hash tables must be protected by a mutex.
*/
static Hash lockHash = { SQLITE_HASH_BINARY, 0, 0, 0, 0, 0 };
static Hash openHash = { SQLITE_HASH_BINARY, 0, 0, 0, 0, 0 };


#ifdef SQLITE_UNIX_THREADS
/*
** This variable records whether or not threads can override each others
** locks.
**
**    0:  No.  Threads cannot override each others locks.
**    1:  Yes.  Threads can override each others locks.
**   -1:  We don't know yet.
*/
static int threadsOverrideEachOthersLocks = -1;

/*
** This structure holds information passed into individual test
** threads by the testThreadLockingBehavior() routine.
*/
struct threadTestData {
  int fd;                /* File to be locked */
  struct flock lock;     /* The locking operation */
  int result;            /* Result of the locking operation */
};

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
static void testThreadLockingBehavior(fd_orig){
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
  pLock->nRef--;
  if( pLock->nRef==0 ){
    sqlite3HashInsert(&lockHash, &pLock->key, sizeof(pLock->key), 0);
    sqliteFree(pLock);
  }
}

/*
** Release a openCnt structure previously allocated by findLockInfo().
*/
static void releaseOpenCnt(struct openCnt *pOpen){
  pOpen->nRef--;
  if( pOpen->nRef==0 ){
    sqlite3HashInsert(&openHash, &pOpen->key, sizeof(pOpen->key), 0);
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
    pLock = sqliteMallocRaw( sizeof(*pLock) );
    if( pLock==0 ) return 1;
    pLock->key = key1;
    pLock->nRef = 1;
    pLock->cnt = 0;
    pLock->locktype = 0;
    pOld = sqlite3HashInsert(&lockHash, &pLock->key, sizeof(key1), pLock);
    if( pOld!=0 ){
      assert( pOld==pLock );
      sqliteFree(pLock);
      return 1;
    }
  }else{
    pLock->nRef++;
  }
  *ppLock = pLock;
  pOpen = (struct openCnt*)sqlite3HashFind(&openHash, &key2, sizeof(key2));
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
    pOld = sqlite3HashInsert(&openHash, &pOpen->key, sizeof(key2), pOpen);
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

/*
** Delete the named file
*/
int sqlite3OsDelete(const char *zFilename){
  unlink(zFilename);
  return SQLITE_OK;
}

/*
** Return TRUE if the named file exists.
*/
int sqlite3OsFileExists(const char *zFilename){
  return access(zFilename, 0)==0;
}

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
int sqlite3OsOpenReadWrite(
  const char *zFilename,
  OsFile *id,
  int *pReadonly
){
  int rc;
  assert( !id->isOpen );
  id->dirfd = -1;
  id->h = open(zFilename, O_RDWR|O_CREAT|O_LARGEFILE|O_BINARY, 0644);
  if( id->h<0 ){
#ifdef EISDIR
    if( errno==EISDIR ){
      return SQLITE_CANTOPEN;
    }
#endif
    id->h = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
    if( id->h<0 ){
      return SQLITE_CANTOPEN; 
    }
    *pReadonly = 1;
  }else{
    *pReadonly = 0;
  }
  sqlite3OsEnterMutex();
  rc = findLockInfo(id->h, &id->pLock, &id->pOpen);
  sqlite3OsLeaveMutex();
  if( rc ){
    close(id->h);
    return SQLITE_NOMEM;
  }
  id->locktype = 0;
  id->isOpen = 1;
  TRACE3("OPEN    %-3d %s\n", id->h, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
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
int sqlite3OsOpenExclusive(const char *zFilename, OsFile *id, int delFlag){
  int rc;
  assert( !id->isOpen );
  if( access(zFilename, 0)==0 ){
    return SQLITE_CANTOPEN;
  }
  id->dirfd = -1;
  id->h = open(zFilename,
                O_RDWR|O_CREAT|O_EXCL|O_NOFOLLOW|O_LARGEFILE|O_BINARY, 0600);
  if( id->h<0 ){
    return SQLITE_CANTOPEN;
  }
  sqlite3OsEnterMutex();
  rc = findLockInfo(id->h, &id->pLock, &id->pOpen);
  sqlite3OsLeaveMutex();
  if( rc ){
    close(id->h);
    unlink(zFilename);
    return SQLITE_NOMEM;
  }
  id->locktype = 0;
  id->isOpen = 1;
  if( delFlag ){
    unlink(zFilename);
  }
  TRACE3("OPEN-EX %-3d %s\n", id->h, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
}

/*
** Attempt to open a new file for read-only access.
**
** On success, write the file handle into *id and return SQLITE_OK.
**
** On failure, return SQLITE_CANTOPEN.
*/
int sqlite3OsOpenReadOnly(const char *zFilename, OsFile *id){
  int rc;
  assert( !id->isOpen );
  id->dirfd = -1;
  id->h = open(zFilename, O_RDONLY|O_LARGEFILE|O_BINARY);
  if( id->h<0 ){
    return SQLITE_CANTOPEN;
  }
  sqlite3OsEnterMutex();
  rc = findLockInfo(id->h, &id->pLock, &id->pOpen);
  sqlite3OsLeaveMutex();
  if( rc ){
    close(id->h);
    return SQLITE_NOMEM;
  }
  id->locktype = 0;
  id->isOpen = 1;
  TRACE3("OPEN-RO %-3d %s\n", id->h, zFilename);
  OpenCounter(+1);
  return SQLITE_OK;
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
int sqlite3OsOpenDirectory(
  const char *zDirname,
  OsFile *id
){
  if( !id->isOpen ){
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
  return SQLITE_OK;
}

/*
** If the following global variable points to a string which is the
** name of a directory, then that directory will be used to store
** temporary files.
*/
const char *sqlite3_temp_directory = 0;

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at least SQLITE_TEMPNAME_SIZE characters.
*/
int sqlite3OsTempFileName(char *zBuf){
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
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
int sqlite3OsRead(OsFile *id, void *pBuf, int amt){
  int got;
  assert( id->isOpen );
  SimulateIOError(SQLITE_IOERR);
  TIMER_START;
  got = read(id->h, pBuf, amt);
  TIMER_END;
  TRACE4("READ    %-3d %7d %d\n", id->h, last_page, TIMER_ELAPSED);
  SEEK(0);
  /* if( got<0 ) got = 0; */
  if( got==amt ){
    return SQLITE_OK;
  }else{
    return SQLITE_IOERR;
  }
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
int sqlite3OsWrite(OsFile *id, const void *pBuf, int amt){
  int wrote = 0;
  assert( id->isOpen );
  SimulateIOError(SQLITE_IOERR);
  SimulateDiskfullError;
  TIMER_START;
  while( amt>0 && (wrote = write(id->h, pBuf, amt))>0 ){
    amt -= wrote;
    pBuf = &((char*)pBuf)[wrote];
  }
  TIMER_END;
  TRACE4("WRITE   %-3d %7d %d\n", id->h, last_page, TIMER_ELAPSED);
  SEEK(0);
  if( amt>0 ){
    return SQLITE_FULL;
  }
  return SQLITE_OK;
}

/*
** Move the read/write pointer in a file.
*/
int sqlite3OsSeek(OsFile *id, i64 offset){
  assert( id->isOpen );
  SEEK(offset/1024 + 1);
  lseek(id->h, offset, SEEK_SET);
  return SQLITE_OK;
}

/*
** The fsync() system call does not work as advertised on many
** unix systems.  The following procedure is an attempt to make
** it work better.
*/
static int full_fsync(int fd){
  int rc;
#ifdef F_FULLFSYNC
  rc = fcntl(fd, F_FULLFSYNC, 0);
  if( rc ) rc = fsync(fd);
#else
  rc = fsync(fd);
#endif
  return rc;
}

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
int sqlite3OsSync(OsFile *id){
  assert( id->isOpen );
  SimulateIOError(SQLITE_IOERR);
  TRACE2("SYNC    %-3d\n", id->h);
  if( full_fsync(id->h) ){
    return SQLITE_IOERR;
  }
  if( id->dirfd>=0 ){
    TRACE2("DIRSYNC %-3d\n", id->dirfd);
    full_fsync(id->dirfd);
    close(id->dirfd);  /* Only need to sync once, so close the directory */
    id->dirfd = -1;    /* when we are done. */
  }
  return SQLITE_OK;
}

/*
** Sync the directory zDirname. This is a no-op on operating systems other
** than UNIX.
*/
int sqlite3OsSyncDirectory(const char *zDirname){
  int fd;
  int r;
  SimulateIOError(SQLITE_IOERR);
  fd = open(zDirname, O_RDONLY|O_BINARY, 0644);
  TRACE3("DIRSYNC %-3d (%s)\n", fd, zDirname);
  if( fd<0 ){
    return SQLITE_CANTOPEN; 
  }
  r = fsync(fd);
  close(fd);
  return ((r==0)?SQLITE_OK:SQLITE_IOERR);
}

/*
** Truncate an open file to a specified size
*/
int sqlite3OsTruncate(OsFile *id, i64 nByte){
  assert( id->isOpen );
  SimulateIOError(SQLITE_IOERR);
  return ftruncate(id->h, nByte)==0 ? SQLITE_OK : SQLITE_IOERR;
}

/*
** Determine the current size of a file in bytes
*/
int sqlite3OsFileSize(OsFile *id, i64 *pSize){
  struct stat buf;
  assert( id->isOpen );
  SimulateIOError(SQLITE_IOERR);
  if( fstat(id->h, &buf)!=0 ){
    return SQLITE_IOERR;
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
int sqlite3OsCheckReservedLock(OsFile *id){
  int r = 0;

  assert( id->isOpen );
  sqlite3OsEnterMutex(); /* Needed because id->pLock is shared across threads */

  /* Check if a thread in this process holds such a lock */
  if( id->pLock->locktype>SHARED_LOCK ){
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
    fcntl(id->h, F_GETLK, &lock);
    if( lock.l_type!=F_UNLCK ){
      r = 1;
    }
  }
  
  sqlite3OsLeaveMutex();
  TRACE3("TEST WR-LOCK %d %d\n", id->h, r);

  return r;
}

#ifdef SQLITE_DEBUG
/*
** Helper function for printing out trace information from debugging
** binaries. This returns the string represetation of the supplied
** integer lock-type.
*/
static const char * locktypeName(int locktype){
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
int sqlite3OsLock(OsFile *id, int locktype){
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
  struct lockInfo *pLock = id->pLock;
  struct flock lock;
  int s;

  assert( id->isOpen );
  TRACE7("LOCK %d %s was %s(%s,%d) pid=%d\n", id->h, locktypeName(locktype), 
      locktypeName(id->locktype), locktypeName(pLock->locktype), pLock->cnt
      ,getpid() );

  /* If there is already a lock of this type or more restrictive on the
  ** OsFile, do nothing. Don't use the end_lock: exit path, as
  ** sqlite3OsEnterMutex() hasn't been called yet.
  */
  if( id->locktype>=locktype ){
    TRACE3("LOCK %d %s ok (already held)\n", id->h, locktypeName(locktype));
    return SQLITE_OK;
  }

  /* Make sure the locking sequence is correct
  */
  assert( id->locktype!=NO_LOCK || locktype==SHARED_LOCK );
  assert( locktype!=PENDING_LOCK );
  assert( locktype!=RESERVED_LOCK || id->locktype==SHARED_LOCK );

  /* This mutex is needed because id->pLock is shared across threads
  */
  sqlite3OsEnterMutex();

  /* If some thread using this PID has a lock via a different OsFile*
  ** handle that precludes the requested lock, return BUSY.
  */
  if( (id->locktype!=pLock->locktype && 
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
    assert( id->locktype==0 );
    assert( pLock->cnt>0 );
    id->locktype = SHARED_LOCK;
    pLock->cnt++;
    id->pOpen->nLock++;
    goto end_lock;
  }

  lock.l_len = 1L;
  lock.l_whence = SEEK_SET;

  /* A PENDING lock is needed before acquiring a SHARED lock and before
  ** acquiring an EXCLUSIVE lock.  For the SHARED lock, the PENDING will
  ** be released.
  */
  if( locktype==SHARED_LOCK 
      || (locktype==EXCLUSIVE_LOCK && id->locktype<PENDING_LOCK)
  ){
    lock.l_type = (locktype==SHARED_LOCK?F_RDLCK:F_WRLCK);
    lock.l_start = PENDING_BYTE;
    s = fcntl(id->h, F_SETLK, &lock);
    if( s ){
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
    s = fcntl(id->h, F_SETLK, &lock);

    /* Drop the temporary PENDING lock */
    lock.l_start = PENDING_BYTE;
    lock.l_len = 1L;
    lock.l_type = F_UNLCK;
    fcntl(id->h, F_SETLK, &lock);
    if( s ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }else{
      id->locktype = SHARED_LOCK;
      id->pOpen->nLock++;
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
    assert( 0!=id->locktype );
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
    s = fcntl(id->h, F_SETLK, &lock);
    if( s ){
      rc = (errno==EINVAL) ? SQLITE_NOLFS : SQLITE_BUSY;
    }
  }
  
  if( rc==SQLITE_OK ){
    id->locktype = locktype;
    pLock->locktype = locktype;
  }else if( locktype==EXCLUSIVE_LOCK ){
    id->locktype = PENDING_LOCK;
    pLock->locktype = PENDING_LOCK;
  }

end_lock:
  sqlite3OsLeaveMutex();
  TRACE4("LOCK %d %s %s\n", id->h, locktypeName(locktype), 
      rc==SQLITE_OK ? "ok" : "failed");
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
** is NO_LOCK.  If the second argument is SHARED_LOCK, this routine
** might return SQLITE_IOERR instead of SQLITE_OK.
*/
int sqlite3OsUnlock(OsFile *id, int locktype){
  struct lockInfo *pLock;
  struct flock lock;
  int rc = SQLITE_OK;

  assert( id->isOpen );
  TRACE7("UNLOCK %d %d was %d(%d,%d) pid=%d\n", id->h, locktype, id->locktype, 
      id->pLock->locktype, id->pLock->cnt, getpid());

  assert( locktype<=SHARED_LOCK );
  if( id->locktype<=locktype ){
    return SQLITE_OK;
  }
  sqlite3OsEnterMutex();
  pLock = id->pLock;
  assert( pLock->cnt!=0 );
  if( id->locktype>SHARED_LOCK ){
    assert( pLock->locktype==id->locktype );
    if( locktype==SHARED_LOCK ){
      lock.l_type = F_RDLCK;
      lock.l_whence = SEEK_SET;
      lock.l_start = SHARED_FIRST;
      lock.l_len = SHARED_SIZE;
      if( fcntl(id->h, F_SETLK, &lock)!=0 ){
        /* This should never happen */
        rc = SQLITE_IOERR;
      }
    }
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = PENDING_BYTE;
    lock.l_len = 2L;  assert( PENDING_BYTE+1==RESERVED_BYTE );
    fcntl(id->h, F_SETLK, &lock);
    pLock->locktype = SHARED_LOCK;
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
      fcntl(id->h, F_SETLK, &lock);
      pLock->locktype = NO_LOCK;
    }

    /* Decrement the count of locks against this same file.  When the
    ** count reaches zero, close any other file descriptors whose close
    ** was deferred because of outstanding locks.
    */
    pOpen = id->pOpen;
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
  sqlite3OsLeaveMutex();
  id->locktype = locktype;
  return rc;
}

/*
** Close a file.
*/
int sqlite3OsClose(OsFile *id){
  if( !id->isOpen ) return SQLITE_OK;
  sqlite3OsUnlock(id, NO_LOCK);
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
    pOpen->nPending++;
    aNew = sqliteRealloc( pOpen->aPending, pOpen->nPending*sizeof(int) );
    if( aNew==0 ){
      /* If a malloc fails, just leak the file descriptor */
    }else{
      pOpen->aPending = aNew;
      pOpen->aPending[pOpen->nPending-1] = id->h;
    }
  }else{
    /* There are no outstanding locks so we can close the file immediately */
    close(id->h);
  }
  releaseLockInfo(id->pLock);
  releaseOpenCnt(id->pOpen);
  sqlite3OsLeaveMutex();
  id->isOpen = 0;
  TRACE2("CLOSE   %-3d\n", id->h);
  OpenCounter(-1);
  return SQLITE_OK;
}

/*
** Get information to seed the random number generator.  The seed
** is written into the buffer zBuf[256].  The calling function must
** supply a sufficiently large buffer.
*/
int sqlite3OsRandomSeed(char *zBuf){
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
#if !defined(SQLITE_TEST)
  {
    int pid;
    time((time_t*)zBuf);
    pid = getpid();
    memcpy(&zBuf[sizeof(time_t)], &pid, sizeof(pid));
  }
#endif
  return SQLITE_OK;
}

/*
** Sleep for a little while.  Return the amount of time slept.
*/
int sqlite3OsSleep(int ms){
#if defined(HAVE_USLEEP) && HAVE_USLEEP
  usleep(ms*1000);
  return ms;
#else
  sleep((ms+999)/1000);
  return 1000*((ms+999)/1000);
#endif
}

/*
** Static variables used for thread synchronization
*/
static int inMutex = 0;
#ifdef SQLITE_UNIX_THREADS
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

/*
** The following pair of routine implement mutual exclusion for
** multi-threaded processes.  Only a single thread is allowed to
** executed code that is surrounded by EnterMutex() and LeaveMutex().
**
** SQLite uses only a single Mutex.  There is not much critical
** code and what little there is executes quickly and without blocking.
*/
void sqlite3OsEnterMutex(){
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_lock(&mutex);
#endif
  assert( !inMutex );
  inMutex = 1;
}
void sqlite3OsLeaveMutex(){
  assert( inMutex );
  inMutex = 0;
#ifdef SQLITE_UNIX_THREADS
  pthread_mutex_unlock(&mutex);
#endif
}

/*
** Turn a relative pathname into a full pathname.  Return a pointer
** to the full pathname stored in space obtained from sqliteMalloc().
** The calling function is responsible for freeing this space once it
** is no longer needed.
*/
char *sqlite3OsFullPathname(const char *zRelative){
  char *zFull = 0;
  if( zRelative[0]=='/' ){
    sqlite3SetString(&zFull, zRelative, (char*)0);
  }else{
    char zBuf[5000];
    sqlite3SetString(&zFull, getcwd(zBuf, sizeof(zBuf)), "/", zRelative,
                    (char*)0);
  }
  return zFull;
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
int sqlite3OsCurrentTime(double *prNow){
  time_t t;
  time(&t);
  *prNow = t/86400.0 + 2440587.5;
#ifdef SQLITE_TEST
  if( sqlite3_current_time ){
    *prNow = sqlite3_current_time/86400.0 + 2440587.5;
  }
#endif
  return 0;
}

#if 0 /* NOT USED */
/*
** Find the time that the file was last modified.  Write the
** modification time and date as a Julian Day number into *prNow and
** return SQLITE_OK.  Return SQLITE_ERROR if the modification
** time cannot be found.
*/
int sqlite3OsFileModTime(OsFile *id, double *prNow){
  int rc;
  struct stat statbuf;
  if( fstat(id->h, &statbuf)==0 ){
    *prNow = statbuf.st_mtime/86400.0 + 2440587.5;
    rc = SQLITE_OK;
  }else{
    rc = SQLITE_ERROR;
  }
  return rc;
}
#endif /* NOT USED */

#endif /* OS_UNIX */
