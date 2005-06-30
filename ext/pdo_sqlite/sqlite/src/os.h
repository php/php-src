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
** Figure out if we are dealing with Unix, Windows or MacOS.
**
** N.B. MacOS means Mac Classic (or Carbon). Treat Darwin (OS X) as Unix.
**      The MacOS build is designed to use CodeWarrior (tested with v8)
*/
#if !defined(OS_UNIX) && !defined(OS_TEST) && !defined(OS_OTHER)
# define OS_OTHER 0
# ifndef OS_WIN
#   if defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#     define OS_WIN 1
#     define OS_UNIX 0
#   else
#     define OS_WIN 0
#     define OS_UNIX 1
#  endif
# else
#  define OS_UNIX 0
# endif
#else
# ifndef OS_WIN
#  define OS_WIN 0
# endif
#endif

/*
** Invoke the appropriate operating-system specific header file.
*/
#if OS_TEST
# include "os_test.h"
#endif
#if OS_UNIX
# include "os_unix.h"
#endif
#if OS_WIN
# include "os_win.h"
#endif

/* os_other.c and os_other.h are not delivered with SQLite.  These files
** are place-holders that can be filled in by third-party developers to
** implement backends to their on proprietary operating systems.
*/
#if OS_OTHER
# include "os_other.h"
#endif

/* If the SET_FULLSYNC macro is not defined above, then make it
** a no-op
*/
#ifndef SET_FULLSYNC
# define SET_FULLSYNC(x,y)
#endif

/*
** Temporary files are named starting with this prefix followed by 16 random
** alphanumeric characters, and no file extension. They are stored in the
** OS's standard temporary file directory, and are deleted prior to exit.
** If sqlite is being embedded in another program, you may wish to change the
** prefix to reflect your program's name, so that if your program exits
** prematurely, old temporary files can be easily identified. This can be done
** using -DTEMP_FILE_PREFIX=myprefix_ on the compiler command line.
*/
#ifndef TEMP_FILE_PREFIX
# define TEMP_FILE_PREFIX "sqlite_"
#endif

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
** These #defines are available in os.h so that Unix can use the same
** byte ranges for locking.  This leaves open the possiblity of having
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
#define PENDING_BYTE      0x40000000  /* First byte past the 1GB boundary */
/* #define PENDING_BYTE     0x5400   // Page 22 - for testing */
#define RESERVED_BYTE     (PENDING_BYTE+1)
#define SHARED_FIRST      (PENDING_BYTE+2)
#define SHARED_SIZE       510


int sqlite3OsDelete(const char*);
int sqlite3OsFileExists(const char*);
int sqlite3OsOpenReadWrite(const char*, OsFile*, int*);
int sqlite3OsOpenExclusive(const char*, OsFile*, int);
int sqlite3OsOpenReadOnly(const char*, OsFile*);
int sqlite3OsOpenDirectory(const char*, OsFile*);
int sqlite3OsSyncDirectory(const char*);
int sqlite3OsTempFileName(char*);
int sqlite3OsIsDirWritable(char*);
int sqlite3OsClose(OsFile*);
int sqlite3OsRead(OsFile*, void*, int amt);
int sqlite3OsWrite(OsFile*, const void*, int amt);
int sqlite3OsSeek(OsFile*, i64 offset);
int sqlite3OsSync(OsFile*);
int sqlite3OsTruncate(OsFile*, i64 size);
int sqlite3OsFileSize(OsFile*, i64 *pSize);
char *sqlite3OsFullPathname(const char*);
int sqlite3OsLock(OsFile*, int);
int sqlite3OsUnlock(OsFile*, int);
int sqlite3OsCheckReservedLock(OsFile *id);


/* The interface for file I/O is above.  Other miscellaneous functions
** are below */

int sqlite3OsRandomSeed(char*);
int sqlite3OsSleep(int ms);
int sqlite3OsCurrentTime(double*);
void sqlite3OsEnterMutex(void);
void sqlite3OsLeaveMutex(void);

#endif /* _SQLITE_OS_H_ */
