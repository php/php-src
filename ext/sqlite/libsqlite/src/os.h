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
** Helpful hint:  To get this to compile on HP/UX, add -D_INCLUDE_POSIX_SOURCE
** to the compiler command line.
*/

/*
** These #defines should enable >2GB file support on Posix if the
** underlying operating system supports it.  If the OS lacks
** large file support, or if the OS is windows, these should be no-ops.
**
** Large file support can be disabled using the -DSQLITE_DISABLE_LFS switch
** on the compiler command line.  This is necessary if you are compiling
** on a recent machine (ex: RedHat 7.2) but you want your code to work
** on an older machine (ex: RedHat 6.0).  If you compile on RedHat 7.2
** without this option, LFS is enable.  But LFS does not exist in the kernel
** in RedHat 6.0, so the code won't work.  Hence, for maximum binary
** portability you should omit LFS.
**
** Similar is true for MacOS.  LFS is only supported on MacOS 9 and later.
*/
#ifndef SQLITE_DISABLE_LFS
# define _LARGE_FILE       1
# ifndef _FILE_OFFSET_BITS
#   define _FILE_OFFSET_BITS 64
# endif
# define _LARGEFILE_SOURCE 1
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
** Figure out if we are dealing with Unix, Windows or MacOS.
**
** N.B. MacOS means Mac Classic (or Carbon). Treat Darwin (OS X) as Unix.
**      The MacOS build is designed to use CodeWarrior (tested with v8)
*/
#ifndef OS_UNIX
# ifndef OS_WIN
#  ifndef OS_MAC
#    if defined(__MACOS__)
#      define OS_MAC 1
#      define OS_WIN 0
#      define OS_UNIX 0
#    elif defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__BORLANDC__)
#      define OS_MAC 0
#      define OS_WIN 1
#      define OS_UNIX 0
#    else
#      define OS_MAC 0
#      define OS_WIN 0
#      define OS_UNIX 1
#    endif
#  else
#    define OS_WIN 0
#    define OS_UNIX 0
#  endif
# else
#  define OS_MAC 0
#  define OS_UNIX 0
# endif
#else
# define OS_MAC 0
# ifndef OS_WIN
#  define OS_WIN 0
# endif
#endif

/*
** A handle for an open file is stored in an OsFile object.
*/
#if OS_UNIX
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
  typedef struct OsFile OsFile;
  struct OsFile {
    struct openCnt *pOpen;    /* Info about all open fd's on this inode */
    struct lockInfo *pLock;   /* Info about locks on this inode */
    int fd;                   /* The file descriptor */
    int locked;               /* True if this instance holds the lock */
    int dirfd;                /* File descriptor for the directory */
  };
# define SQLITE_TEMPNAME_SIZE 200
# if defined(HAVE_USLEEP) && HAVE_USLEEP
#  define SQLITE_MIN_SLEEP_MS 1
# else
#  define SQLITE_MIN_SLEEP_MS 1000
# endif
#endif

#if OS_WIN
#include <windows.h>
#include <winbase.h>
  typedef struct OsFile OsFile;
  struct OsFile {
    HANDLE h;               /* Handle for accessing the file */
    int locked;             /* 0: unlocked, <0: write lock, >0: read lock */
  };
# if defined(_MSC_VER) || defined(__BORLANDC__)
    typedef __int64 off_t;
# else
#  if !defined(_CYGWIN_TYPES_H)
     typedef long long off_t;
#    if defined(__MINGW32__)
#      define	_OFF_T_
#    endif
#  endif
# endif
# define SQLITE_TEMPNAME_SIZE (MAX_PATH+50)
# define SQLITE_MIN_SLEEP_MS 1
#endif

#if OS_MAC
# include <unistd.h>
# include <Files.h>
  typedef struct OsFile OsFile;
  struct OsFile {
    SInt16 refNum;           /* Data fork/file reference number */
    SInt16 refNumRF;         /* Resource fork reference number (for locking) */
    int locked;              /* 0: unlocked, <0: write lock, >0: read lock */
    int delOnClose;          /* True if file is to be deleted on close */
    char *pathToDel;         /* Name of file to delete on close */
  };
# ifdef _LARGE_FILE
    typedef SInt64 off_t;
# else
    typedef SInt32 off_t;
# endif
# define SQLITE_TEMPNAME_SIZE _MAX_PATH
# define SQLITE_MIN_SLEEP_MS 17
#endif

int sqliteOsDelete(const char*);
int sqliteOsFileExists(const char*);
int sqliteOsFileRename(const char*, const char*);
int sqliteOsOpenReadWrite(const char*, OsFile*, int*);
int sqliteOsOpenExclusive(const char*, OsFile*, int);
int sqliteOsOpenReadOnly(const char*, OsFile*);
int sqliteOsOpenDirectory(const char*, OsFile*);
int sqliteOsTempFileName(char*);
int sqliteOsClose(OsFile*);
int sqliteOsRead(OsFile*, void*, int amt);
int sqliteOsWrite(OsFile*, const void*, int amt);
int sqliteOsSeek(OsFile*, off_t offset);
int sqliteOsSync(OsFile*);
int sqliteOsTruncate(OsFile*, off_t size);
int sqliteOsFileSize(OsFile*, off_t *pSize);
int sqliteOsReadLock(OsFile*);
int sqliteOsWriteLock(OsFile*);
int sqliteOsUnlock(OsFile*);
int sqliteOsRandomSeed(char*);
int sqliteOsSleep(int ms);
int sqliteOsCurrentTime(double*);
void sqliteOsEnterMutex(void);
void sqliteOsLeaveMutex(void);
char *sqliteOsFullPathname(const char*);



#endif /* _SQLITE_OS_H_ */
