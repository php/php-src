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
** This file contains code that is specific classic mac.  Mac OS X
** uses the os_unix.c file, not this one.
*/
#include "sqliteInt.h"
#include "os.h"
#if OS_MAC               /* This file used on classic mac only */

#include <extras.h>
#include <path2fss.h>
#include <TextUtils.h>
#include <FinderRegistry.h>
#include <Folders.h>
#include <Timer.h>
#include <OSUtils.h>

/*
** Macros used to determine whether or not to use threads.
*/
#if defined(THREADSAFE) && THREADSAFE
# include <Multiprocessing.h>
# define SQLITE_MACOS_MULTITASKING 1
#endif

/*
** Include code that is common to all os_*.c files
*/
#include "os_common.h"

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
  if( !sqlite3OsFileExists(zFilename) ){
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
    id->pathToDel = sqlite3OsFullPathname(zFilename);
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
  return SQLITE_OK;
}

/*
** Create a temporary file name in zBuf.  zBuf must be big enough to
** hold at least SQLITE_TEMPNAME_SIZE characters.
*/
int sqlite3OsTempFileName(char *zBuf){
  static char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "0123456789";
  int i, j;
  char zTempPath[SQLITE_TEMPNAME_SIZE];
  char zdirName[32];
  CInfoPBRec infoRec;
  Str31 dirName;
  memset(&infoRec, 0, sizeof(infoRec));
  memset(zTempPath, 0, SQLITE_TEMPNAME_SIZE);
  if( FindFolder(kOnSystemDisk, kTemporaryFolderType,  kCreateFolder,
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
  }
  if( *zTempPath == 0 )
    getcwd(zTempPath, SQLITE_TEMPNAME_SIZE-24);
  for(;;){
    sprintf(zBuf, "%s"TEMP_FILE_PREFIX, zTempPath);
    j = strlen(zBuf);
    sqlite3Randomness(15, &zBuf[j]);
    for(i=0; i<15; i++, j++){
      zBuf[j] = (char)zChars[ ((unsigned char)zBuf[j])%(sizeof(zChars)-1) ];
    }
    zBuf[j] = 0;
    if( !sqlite3OsFileExists(zBuf) ) break;
  }
  return SQLITE_OK; 
}

/*
** Close a file.
*/
int sqlite3OsClose(OsFile *id){
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
}

/*
** Read data from a file into a buffer.  Return SQLITE_OK if all
** bytes were read successfully and SQLITE_IOERR if anything goes
** wrong.
*/
int sqlite3OsRead(OsFile *id, void *pBuf, int amt){
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
}

/*
** Write data from a buffer into a file.  Return SQLITE_OK on success
** or some other error code on failure.
*/
int sqlite3OsWrite(OsFile *id, const void *pBuf, int amt){
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
}

/*
** Move the read/write pointer in a file.
*/
int sqlite3OsSeek(OsFile *id, off_t offset){
  off_t curSize;
  SEEK(offset/1024 + 1);
  if( sqlite3OsFileSize(id, &curSize) != SQLITE_OK ){
    return SQLITE_IOERR;
  }
  if( offset >= curSize ){
    if( sqlite3OsTruncate(id, offset+1) != SQLITE_OK ){
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
}

/*
** Sync the directory zDirname. This is a no-op on operating systems other
** than UNIX.
*/
int sqlite3OsSyncDirectory(const char *zDirname){
  SimulateIOError(SQLITE_IOERR);
  return SQLITE_OK;
}

/*
** Truncate an open file to a specified size
*/
int sqlite3OsTruncate(OsFile *id, off_t nByte){
  SimulateIOError(SQLITE_IOERR);
# ifdef _LARGE_FILE
  if( FSSetForkSize(id->refNum, fsFromStart, nByte) != noErr){
# else
  if( SetEOF(id->refNum, nByte) != noErr ){
# endif
    return SQLITE_IOERR;
  }else{
    return SQLITE_OK;
  }
}

/*
** Determine the current size of a file in bytes
*/
int sqlite3OsFileSize(OsFile *id, off_t *pSize){
# ifdef _LARGE_FILE
  if( FSGetForkSize(id->refNum, pSize) != noErr){
# else
  if( GetEOF(id->refNum, pSize) != noErr ){
# endif
    return SQLITE_IOERR;
  }else{
    return SQLITE_OK;
  }
}

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
** sqlite3OsReadLock() require that if there is already a write lock, that
** lock is converted into a read lock atomically.  The lock on the first
** byte allows us to drop the old write lock and get the read lock without
** another process jumping into the middle and messing us up.  The same
** argument applies to sqlite3OsWriteLock().
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
#define FIRST_LOCKBYTE   (0x000fffff - N_LOCKBYTE)

/*
** Change the status of the lock on the file "id" to be a readlock.
** If the file was write locked, then this reduces the lock to a read.
** If the file was read locked, then this acquires a new read lock.
**
** Return SQLITE_OK on success and SQLITE_BUSY on failure.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqlite3OsReadLock(OsFile *id){
  int rc;
  if( id->locked>0 || id->refNumRF == -1 ){
    rc = SQLITE_OK;
  }else{
    int lk;
    OSErr res;
    int cnt = 5;
    ParamBlockRec params;
    sqlite3Randomness(sizeof(lk), &lk);
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
}

/*
** Change the lock status to be an exclusive or write lock.  Return
** SQLITE_OK on success and SQLITE_BUSY on a failure.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqlite3OsWriteLock(OsFile *id){
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
}

/*
** Unlock the given file descriptor.  If the file descriptor was
** not previously locked, then this routine is a no-op.  If this
** library was compiled with large file support (LFS) but LFS is not
** available on the host, then an SQLITE_NOLFS is returned.
*/
int sqlite3OsUnlock(OsFile *id){
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
int sqlite3OsSleep(int ms){
  UInt32 finalTicks;
  UInt32 ticks = (((UInt32)ms+16)*3)/50;  /* 1/60 sec per tick */
  Delay(ticks, &finalTicks);
  return (int)((ticks*50)/3);
}

/*
** Static variables used for thread synchronization
*/
static int inMutex = 0;
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
void sqlite3OsEnterMutex(){
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
void sqlite3OsLeaveMutex(){
  assert( inMutex );
  inMutex = 0;
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
char *sqlite3OsFullPathname(const char *zRelative){
  char *zFull = 0;
  if( zRelative[0]==':' ){
    char zBuf[_MAX_PATH+1];
    sqlite3SetString(&zFull, getcwd(zBuf, sizeof(zBuf)), &(zRelative[1]),
                    (char*)0);
  }else{
    if( strchr(zRelative, ':') ){
      sqlite3SetString(&zFull, zRelative, (char*)0);
    }else{
    char zBuf[_MAX_PATH+1];
      sqlite3SetString(&zFull, getcwd(zBuf, sizeof(zBuf)), zRelative, (char*)0);
    }
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
  *prNow = 0.0;   /**** FIX ME *****/
#ifdef SQLITE_TEST
  if( sqlite3_current_time ){
    *prNow = sqlite3_current_time/86400.0 + 2440587.5;
  }
#endif
  return 0;
}

#endif /* OS_MAC */
