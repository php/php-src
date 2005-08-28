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
** This header file defined OS-specific features for Unix.
*/
#ifndef _SQLITE_OS_UNIX_H_
#define _SQLITE_OS_UNIX_H_

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
** standard include files.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
** The OsFile structure is a operating-system independing representation
** of an open file handle.  It is defined differently for each architecture.
**
** This is the definition for Unix.
**
** OsFile.locktype takes one of the values SHARED_LOCK, RESERVED_LOCK,
** PENDING_LOCK or EXCLUSIVE_LOCK.
*/
typedef struct OsFile OsFile;
struct OsFile {
  struct Pager *pPager;     /* The pager that owns this OsFile.  Might be 0 */
  struct openCnt *pOpen;    /* Info about all open fd's on this inode */
  struct lockInfo *pLock;   /* Info about locks on this inode */
  int h;                    /* The file descriptor */
  unsigned char locktype;   /* The type of lock held on this fd */
  unsigned char isOpen;     /* True if needs to be closed */
  unsigned char fullSync;   /* Use F_FULLSYNC if available */
  int dirfd;                /* File descriptor for the directory */
#ifdef SQLITE_UNIX_THREADS
  pthread_t tid;            /* The thread authorized to use this OsFile */
#endif
};

/*
** A macro to set the OsFile.fullSync flag, if it exists.
*/
#define SET_FULLSYNC(x,y)  ((x).fullSync = (y))

/*
** Maximum number of characters in a temporary file name
*/
#define SQLITE_TEMPNAME_SIZE 200

/*
** Minimum interval supported by sqlite3OsSleep().
*/
#if defined(HAVE_USLEEP) && HAVE_USLEEP
# define SQLITE_MIN_SLEEP_MS 1
#else
# define SQLITE_MIN_SLEEP_MS 1000
#endif

/*
** Default permissions when creating a new file
*/
#ifndef SQLITE_DEFAULT_FILE_PERMISSIONS
# define SQLITE_DEFAULT_FILE_PERMISSIONS 0644
#endif


#endif /* _SQLITE_OS_UNIX_H_ */
