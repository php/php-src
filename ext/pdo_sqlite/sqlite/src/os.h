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
** This header file (together with is companion C source-code file
** "os.c") attempt to abstract the underlying operating system so that
** the SQLite library will work on both POSIX and windows systems.
*/
#ifndef _SQLITE_OS_H_
#define _SQLITE_OS_H_

/*
** Figure out if we are dealing with Unix, Windows, or some other
** operating system.
*/
#if defined(OS_OTHER)
# if OS_OTHER==1
#   undef OS_UNIX
#   define OS_UNIX 0
#   undef OS_WIN
#   define OS_WIN 0
#   undef OS_OS2
#   define OS_OS2 0
# else
#   undef OS_OTHER
# endif
#endif
#if !defined(OS_UNIX) && !defined(OS_OTHER)
# define OS_OTHER 0
# ifndef OS_WIN
#   if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#     define OS_WIN 1
#     define OS_UNIX 0
#     define OS_OS2 0
#   elif defined(_EMX_) || defined(_OS2) || defined(OS2) || defined(_OS2_) || defined(__OS2__)
#     define OS_WIN 0
#     define OS_UNIX 0
#     define OS_OS2 1
#   else
#     define OS_WIN 0
#     define OS_UNIX 1
#     define OS_OS2 0
#  endif
# else
#  define OS_UNIX 0
#  define OS_OS2 0
# endif
#else
# ifndef OS_WIN
#  define OS_WIN 0
# endif
#endif


/*
** Define the maximum size of a temporary filename
*/
#if OS_WIN
# include <windows.h>
# define SQLITE_TEMPNAME_SIZE (MAX_PATH+50)
#elif OS_OS2
# define INCL_DOSDATETIME
# define INCL_DOSFILEMGR
# define INCL_DOSERRORS
# define INCL_DOSMISC
# define INCL_DOSPROCESS
# include <os2.h>
# define SQLITE_TEMPNAME_SIZE (CCHMAXPATHCOMP)
#else
# define SQLITE_TEMPNAME_SIZE 200
#endif

/* If the SET_FULLSYNC macro is not defined above, then make it
** a no-op
*/
#ifndef SET_FULLSYNC
# define SET_FULLSYNC(x,y)
#endif

/*
** The default size of a disk sector
*/
#ifndef SQLITE_DEFAULT_SECTOR_SIZE
# define SQLITE_DEFAULT_SECTOR_SIZE 512
#endif

/*
** Temporary files are named starting with this prefix followed by 16 random
** alphanumeric characters, and no file extension. They are stored in the
** OS's standard temporary file directory, and are deleted prior to exit.
** If sqlite is being embedded in another program, you may wish to change the
** prefix to reflect your program's name, so that if your program exits
** prematurely, old temporary files can be easily identified. This can be done
** using -DTEMP_FILE_PREFIX=myprefix_ on the compiler command line.
**
** 2006-10-31:  The default prefix used to be "sqlite_".  But then
** Mcafee started using SQLite in their anti-virus product and it
** started putting files with the "sqlite" name in the c:/temp folder.
** This annoyed many windows users.  Those users would then do a 
** Google search for "sqlite", find the telephone numbers of the
** developers and call to wake them up at night and complain.
** For this reason, the default name prefix is changed to be "sqlite" 
** spelled backwards.  So the temp files are still identified, but
** anybody smart enough to figure out the code is also likely smart
** enough to know that calling the developer will not help get rid
** of the file.
*/
#ifndef TEMP_FILE_PREFIX
# define TEMP_FILE_PREFIX "etilqs_"
#endif

/*
** Define the interfaces for Unix, Windows, and OS/2.
*/
#if OS_UNIX
#define sqlite3OsOpenReadWrite      sqlite3UnixOpenReadWrite
#define sqlite3OsOpenExclusive      sqlite3UnixOpenExclusive
#define sqlite3OsOpenReadOnly       sqlite3UnixOpenReadOnly
#define sqlite3OsDelete             sqlite3UnixDelete
#define sqlite3OsFileExists         sqlite3UnixFileExists
#define sqlite3OsFullPathname       sqlite3UnixFullPathname
#define sqlite3OsIsDirWritable      sqlite3UnixIsDirWritable
#define sqlite3OsSyncDirectory      sqlite3UnixSyncDirectory
#define sqlite3OsTempFileName       sqlite3UnixTempFileName
#define sqlite3OsRandomSeed         sqlite3UnixRandomSeed
#define sqlite3OsSleep              sqlite3UnixSleep
#define sqlite3OsCurrentTime        sqlite3UnixCurrentTime
#define sqlite3OsEnterMutex         sqlite3UnixEnterMutex
#define sqlite3OsLeaveMutex         sqlite3UnixLeaveMutex
#define sqlite3OsInMutex            sqlite3UnixInMutex
#define sqlite3OsThreadSpecificData sqlite3UnixThreadSpecificData
#define sqlite3OsMalloc             sqlite3GenericMalloc
#define sqlite3OsRealloc            sqlite3GenericRealloc
#define sqlite3OsFree               sqlite3GenericFree
#define sqlite3OsAllocationSize     sqlite3GenericAllocationSize
#define sqlite3OsDlopen             sqlite3UnixDlopen
#define sqlite3OsDlsym              sqlite3UnixDlsym
#define sqlite3OsDlclose            sqlite3UnixDlclose
#endif
#if OS_WIN
#define sqlite3OsOpenReadWrite      sqlite3WinOpenReadWrite
#define sqlite3OsOpenExclusive      sqlite3WinOpenExclusive
#define sqlite3OsOpenReadOnly       sqlite3WinOpenReadOnly
#define sqlite3OsDelete             sqlite3WinDelete
#define sqlite3OsFileExists         sqlite3WinFileExists
#define sqlite3OsFullPathname       sqlite3WinFullPathname
#define sqlite3OsIsDirWritable      sqlite3WinIsDirWritable
#define sqlite3OsSyncDirectory      sqlite3WinSyncDirectory
#define sqlite3OsTempFileName       sqlite3WinTempFileName
#define sqlite3OsRandomSeed         sqlite3WinRandomSeed
#define sqlite3OsSleep              sqlite3WinSleep
#define sqlite3OsCurrentTime        sqlite3WinCurrentTime
#define sqlite3OsEnterMutex         sqlite3WinEnterMutex
#define sqlite3OsLeaveMutex         sqlite3WinLeaveMutex
#define sqlite3OsInMutex            sqlite3WinInMutex
#define sqlite3OsThreadSpecificData sqlite3WinThreadSpecificData
#define sqlite3OsMalloc             sqlite3GenericMalloc
#define sqlite3OsRealloc            sqlite3GenericRealloc
#define sqlite3OsFree               sqlite3GenericFree
#define sqlite3OsAllocationSize     sqlite3GenericAllocationSize
#define sqlite3OsDlopen             sqlite3WinDlopen
#define sqlite3OsDlsym              sqlite3WinDlsym
#define sqlite3OsDlclose            sqlite3WinDlclose
#endif
#if OS_OS2
#define sqlite3OsOpenReadWrite      sqlite3Os2OpenReadWrite
#define sqlite3OsOpenExclusive      sqlite3Os2OpenExclusive
#define sqlite3OsOpenReadOnly       sqlite3Os2OpenReadOnly
#define sqlite3OsDelete             sqlite3Os2Delete
#define sqlite3OsFileExists         sqlite3Os2FileExists
#define sqlite3OsFullPathname       sqlite3Os2FullPathname
#define sqlite3OsIsDirWritable      sqlite3Os2IsDirWritable
#define sqlite3OsSyncDirectory      sqlite3Os2SyncDirectory
#define sqlite3OsTempFileName       sqlite3Os2TempFileName
#define sqlite3OsRandomSeed         sqlite3Os2RandomSeed
#define sqlite3OsSleep              sqlite3Os2Sleep
#define sqlite3OsCurrentTime        sqlite3Os2CurrentTime
#define sqlite3OsEnterMutex         sqlite3Os2EnterMutex
#define sqlite3OsLeaveMutex         sqlite3Os2LeaveMutex
#define sqlite3OsInMutex            sqlite3Os2InMutex
#define sqlite3OsThreadSpecificData sqlite3Os2ThreadSpecificData
#define sqlite3OsMalloc             sqlite3GenericMalloc
#define sqlite3OsRealloc            sqlite3GenericRealloc
#define sqlite3OsFree               sqlite3GenericFree
#define sqlite3OsAllocationSize     sqlite3GenericAllocationSize
#define sqlite3OsDlopen             sqlite3Os2Dlopen
#define sqlite3OsDlsym              sqlite3Os2Dlsym
#define sqlite3OsDlclose            sqlite3Os2Dlclose
#endif




/*
** If using an alternative OS interface, then we must have an "os_other.h"
** header file available for that interface.  Presumably the "os_other.h"
** header file contains #defines similar to those above.
*/
#if OS_OTHER
# include "os_other.h"
#endif



/*
** Forward declarations
*/
typedef struct OsFile OsFile;
typedef struct IoMethod IoMethod;

/*
** An instance of the following structure contains pointers to all
** methods on an OsFile object.
*/
struct IoMethod {
  int (*xClose)(OsFile**);
  int (*xOpenDirectory)(OsFile*, const char*);
  int (*xRead)(OsFile*, void*, int amt);
  int (*xWrite)(OsFile*, const void*, int amt);
  int (*xSeek)(OsFile*, i64 offset);
  int (*xTruncate)(OsFile*, i64 size);
  int (*xSync)(OsFile*, int);
  void (*xSetFullSync)(OsFile *id, int setting);
  int (*xFileHandle)(OsFile *id);
  int (*xFileSize)(OsFile*, i64 *pSize);
  int (*xLock)(OsFile*, int);
  int (*xUnlock)(OsFile*, int);
  int (*xLockState)(OsFile *id);
  int (*xCheckReservedLock)(OsFile *id);
  int (*xSectorSize)(OsFile *id);
};

/*
** The OsFile object describes an open disk file in an OS-dependent way.
** The version of OsFile defined here is a generic version.  Each OS
** implementation defines its own subclass of this structure that contains
** additional information needed to handle file I/O.  But the pMethod
** entry (pointing to the virtual function table) always occurs first
** so that we can always find the appropriate methods.
*/
struct OsFile {
  IoMethod const *pMethod;
};

/*
** The following values may be passed as the second argument to
** sqlite3OsLock(). The various locks exhibit the following semantics:
**
** SHARED:    Any number of processes may hold a SHARED lock simultaneously.
** RESERVED:  A single process may hold a RESERVED lock on a file at
**            any time. Other processes may hold and obtain new SHARED locks.
** PENDING:   A single process may hold a PENDING lock on a file at
**            any one time. Existing SHARED locks may persist, but no new
**            SHARED locks may be obtained by other processes.
** EXCLUSIVE: An EXCLUSIVE lock precludes all other locks.
**
** PENDING_LOCK may not be passed directly to sqlite3OsLock(). Instead, a
** process that requests an EXCLUSIVE lock may actually obtain a PENDING
** lock. This can be upgraded to an EXCLUSIVE lock by a subsequent call to
** sqlite3OsLock().
*/
#define NO_LOCK         0
#define SHARED_LOCK     1
#define RESERVED_LOCK   2
#define PENDING_LOCK    3
#define EXCLUSIVE_LOCK  4

/*
** File Locking Notes:  (Mostly about windows but also some info for Unix)
**
** We cannot use LockFileEx() or UnlockFileEx() on Win95/98/ME because
** those functions are not available.  So we use only LockFile() and
** UnlockFile().
**
** LockFile() prevents not just writing but also reading by other processes.
** A SHARED_LOCK is obtained by locking a single randomly-chosen 
** byte out of a specific range of bytes. The lock byte is obtained at 
** random so two separate readers can probably access the file at the 
** same time, unless they are unlucky and choose the same lock byte.
** An EXCLUSIVE_LOCK is obtained by locking all bytes in the range.
** There can only be one writer.  A RESERVED_LOCK is obtained by locking
** a single byte of the file that is designated as the reserved lock byte.
** A PENDING_LOCK is obtained by locking a designated byte different from
** the RESERVED_LOCK byte.
**
** On WinNT/2K/XP systems, LockFileEx() and UnlockFileEx() are available,
** which means we can use reader/writer locks.  When reader/writer locks
** are used, the lock is placed on the same range of bytes that is used
** for probabilistic locking in Win95/98/ME.  Hence, the locking scheme
** will support two or more Win95 readers or two or more WinNT readers.
** But a single Win95 reader will lock out all WinNT readers and a single
** WinNT reader will lock out all other Win95 readers.
**
** The following #defines specify the range of bytes used for locking.
** SHARED_SIZE is the number of bytes available in the pool from which
** a random byte is selected for a shared lock.  The pool of bytes for
** shared locks begins at SHARED_FIRST. 
**
** These #defines are available in sqlite_aux.h so that adaptors for
** connecting SQLite to other operating systems can use the same byte
** ranges for locking.  In particular, the same locking strategy and
** byte ranges are used for Unix.  This leaves open the possiblity of having
** clients on win95, winNT, and unix all talking to the same shared file
** and all locking correctly.  To do so would require that samba (or whatever
** tool is being used for file sharing) implements locks correctly between
** windows and unix.  I'm guessing that isn't likely to happen, but by
** using the same locking range we are at least open to the possibility.
**
** Locking in windows is manditory.  For this reason, we cannot store
** actual data in the bytes used for locking.  The pager never allocates
** the pages involved in locking therefore.  SHARED_SIZE is selected so
** that all locks will fit on a single page even at the minimum page size.
** PENDING_BYTE defines the beginning of the locks.  By default PENDING_BYTE
** is set high so that we don't have to allocate an unused page except
** for very large databases.  But one should test the page skipping logic 
** by setting PENDING_BYTE low and running the entire regression suite.
**
** Changing the value of PENDING_BYTE results in a subtly incompatible
** file format.  Depending on how it is changed, you might not notice
** the incompatibility right away, even running a full regression test.
** The default location of PENDING_BYTE is the first byte past the
** 1GB boundary.
**
*/
#ifndef SQLITE_TEST
#define PENDING_BYTE      0x40000000  /* First byte past the 1GB boundary */
#else
extern unsigned int sqlite3_pending_byte;
#define PENDING_BYTE sqlite3_pending_byte
#endif

#define RESERVED_BYTE     (PENDING_BYTE+1)
#define SHARED_FIRST      (PENDING_BYTE+2)
#define SHARED_SIZE       510

/*
** Prototypes for operating system interface routines.
*/
int sqlite3OsClose(OsFile**);
int sqlite3OsOpenDirectory(OsFile*, const char*);
int sqlite3OsRead(OsFile*, void*, int amt);
int sqlite3OsWrite(OsFile*, const void*, int amt);
int sqlite3OsSeek(OsFile*, i64 offset);
int sqlite3OsTruncate(OsFile*, i64 size);
int sqlite3OsSync(OsFile*, int);
void sqlite3OsSetFullSync(OsFile *id, int setting);
int sqlite3OsFileHandle(OsFile *id);
int sqlite3OsFileSize(OsFile*, i64 *pSize);
int sqlite3OsLock(OsFile*, int);
int sqlite3OsUnlock(OsFile*, int);
int sqlite3OsLockState(OsFile *id);
int sqlite3OsCheckReservedLock(OsFile *id);
int sqlite3OsOpenReadWrite(const char*, OsFile**, int*);
int sqlite3OsOpenExclusive(const char*, OsFile**, int);
int sqlite3OsOpenReadOnly(const char*, OsFile**);
int sqlite3OsDelete(const char*);
int sqlite3OsFileExists(const char*);
char *sqlite3OsFullPathname(const char*);
int sqlite3OsIsDirWritable(char*);
int sqlite3OsSyncDirectory(const char*);
int sqlite3OsSectorSize(OsFile *id);
int sqlite3OsTempFileName(char*);
int sqlite3OsRandomSeed(char*);
int sqlite3OsSleep(int ms);
int sqlite3OsCurrentTime(double*);
void sqlite3OsEnterMutex(void);
void sqlite3OsLeaveMutex(void);
int sqlite3OsInMutex(int);
ThreadData *sqlite3OsThreadSpecificData(int);
void *sqlite3OsMalloc(int);
void *sqlite3OsRealloc(void *, int);
void sqlite3OsFree(void *);
int sqlite3OsAllocationSize(void *);
void *sqlite3OsDlopen(const char*);
void *sqlite3OsDlsym(void*, const char*);
int sqlite3OsDlclose(void*);

/*
** If the SQLITE_ENABLE_REDEF_IO macro is defined, then the OS-layer
** interface routines are not called directly but are invoked using
** pointers to functions.  This allows the implementation of various
** OS-layer interface routines to be modified at run-time.  There are
** obscure but legitimate reasons for wanting to do this.  But for
** most users, a direct call to the underlying interface is preferable
** so the the redefinable I/O interface is turned off by default.
*/
#ifdef SQLITE_ENABLE_REDEF_IO

/*
** When redefinable I/O is enabled, a single global instance of the
** following structure holds pointers to the routines that SQLite 
** uses to talk with the underlying operating system.  Modify this
** structure (before using any SQLite API!) to accomodate perculiar
** operating system interfaces or behaviors.
*/
struct sqlite3OsVtbl {
  int (*xOpenReadWrite)(const char*, OsFile**, int*);
  int (*xOpenExclusive)(const char*, OsFile**, int);
  int (*xOpenReadOnly)(const char*, OsFile**);

  int (*xDelete)(const char*);
  int (*xFileExists)(const char*);
  char *(*xFullPathname)(const char*);
  int (*xIsDirWritable)(char*);
  int (*xSyncDirectory)(const char*);
  int (*xTempFileName)(char*);

  int (*xRandomSeed)(char*);
  int (*xSleep)(int ms);
  int (*xCurrentTime)(double*);

  void (*xEnterMutex)(void);
  void (*xLeaveMutex)(void);
  int (*xInMutex)(int);
  ThreadData *(*xThreadSpecificData)(int);

  void *(*xMalloc)(int);
  void *(*xRealloc)(void *, int);
  void (*xFree)(void *);
  int (*xAllocationSize)(void *);

  void *(*xDlopen)(const char*);
  void *(*xDlsym)(void*, const char*);
  int (*xDlclose)(void*);
};

/* Macro used to comment out routines that do not exists when there is
** no disk I/O or extension loading
*/
#ifdef SQLITE_OMIT_DISKIO
# define IF_DISKIO(X)  0
#else
# define IF_DISKIO(X)  X
#endif
#ifdef SQLITE_OMIT_LOAD_EXTENSION
# define IF_DLOPEN(X)  0
#else
# define IF_DLOPEN(X)  X
#endif


#if defined(_SQLITE_OS_C_) || defined(SQLITE_AMALGAMATION)
  /*
  ** The os.c file implements the global virtual function table.
  ** We have to put this file here because the initializers
  ** (ex: sqlite3OsRandomSeed) are macros that are about to be
  ** redefined.
  */
  struct sqlite3OsVtbl sqlite3Os = {
    IF_DISKIO( sqlite3OsOpenReadWrite ),
    IF_DISKIO( sqlite3OsOpenExclusive ),
    IF_DISKIO( sqlite3OsOpenReadOnly ),
    IF_DISKIO( sqlite3OsDelete ),
    IF_DISKIO( sqlite3OsFileExists ),
    IF_DISKIO( sqlite3OsFullPathname ),
    IF_DISKIO( sqlite3OsIsDirWritable ),
    IF_DISKIO( sqlite3OsSyncDirectory ),
    IF_DISKIO( sqlite3OsTempFileName ),
    sqlite3OsRandomSeed,
    sqlite3OsSleep,
    sqlite3OsCurrentTime,
    sqlite3OsEnterMutex,
    sqlite3OsLeaveMutex,
    sqlite3OsInMutex,
    sqlite3OsThreadSpecificData,
    sqlite3OsMalloc,
    sqlite3OsRealloc,
    sqlite3OsFree,
    sqlite3OsAllocationSize,
    IF_DLOPEN( sqlite3OsDlopen ),
    IF_DLOPEN( sqlite3OsDlsym ),
    IF_DLOPEN( sqlite3OsDlclose ),
  };
#else
  /*
  ** Files other than os.c just reference the global virtual function table. 
  */
  extern struct sqlite3OsVtbl sqlite3Os;
#endif /* _SQLITE_OS_C_ */


/* This additional API routine is available with redefinable I/O */
struct sqlite3OsVtbl *sqlite3_os_switch(void);


/*
** Redefine the OS interface to go through the virtual function table
** rather than calling routines directly.
*/
#undef sqlite3OsOpenReadWrite
#undef sqlite3OsOpenExclusive
#undef sqlite3OsOpenReadOnly
#undef sqlite3OsDelete
#undef sqlite3OsFileExists
#undef sqlite3OsFullPathname
#undef sqlite3OsIsDirWritable
#undef sqlite3OsSyncDirectory
#undef sqlite3OsTempFileName
#undef sqlite3OsRandomSeed
#undef sqlite3OsSleep
#undef sqlite3OsCurrentTime
#undef sqlite3OsEnterMutex
#undef sqlite3OsLeaveMutex
#undef sqlite3OsInMutex
#undef sqlite3OsThreadSpecificData
#undef sqlite3OsMalloc
#undef sqlite3OsRealloc
#undef sqlite3OsFree
#undef sqlite3OsAllocationSize
#define sqlite3OsOpenReadWrite      sqlite3Os.xOpenReadWrite
#define sqlite3OsOpenExclusive      sqlite3Os.xOpenExclusive
#define sqlite3OsOpenReadOnly       sqlite3Os.xOpenReadOnly
#define sqlite3OsDelete             sqlite3Os.xDelete
#define sqlite3OsFileExists         sqlite3Os.xFileExists
#define sqlite3OsFullPathname       sqlite3Os.xFullPathname
#define sqlite3OsIsDirWritable      sqlite3Os.xIsDirWritable
#define sqlite3OsSyncDirectory      sqlite3Os.xSyncDirectory
#define sqlite3OsTempFileName       sqlite3Os.xTempFileName
#define sqlite3OsRandomSeed         sqlite3Os.xRandomSeed
#define sqlite3OsSleep              sqlite3Os.xSleep
#define sqlite3OsCurrentTime        sqlite3Os.xCurrentTime
#define sqlite3OsEnterMutex         sqlite3Os.xEnterMutex
#define sqlite3OsLeaveMutex         sqlite3Os.xLeaveMutex
#define sqlite3OsInMutex            sqlite3Os.xInMutex
#define sqlite3OsThreadSpecificData sqlite3Os.xThreadSpecificData
#define sqlite3OsMalloc             sqlite3Os.xMalloc
#define sqlite3OsRealloc            sqlite3Os.xRealloc
#define sqlite3OsFree               sqlite3Os.xFree
#define sqlite3OsAllocationSize     sqlite3Os.xAllocationSize

#endif /* SQLITE_ENABLE_REDEF_IO */

#endif /* _SQLITE_OS_H_ */
