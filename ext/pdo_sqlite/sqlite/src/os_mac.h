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
** This header file defines OS-specific features of classic Mac.
** OS X uses the os_unix.h file, not this one.
*/
#ifndef _SQLITE_OS_MAC_H_
#define _SQLITE_OS_MAC_H_


#include <unistd.h>
#include <Files.h>
#define SQLITE_TEMPNAME_SIZE _MAX_PATH
#define SQLITE_MIN_SLEEP_MS 17

/*
** The OsFile structure is a operating-system independing representation
** of an open file handle.  It is defined differently for each architecture.
**
** This is the definition for class Mac.
*/
typedef struct OsFile OsFile;
struct OsFile {
  SInt16 refNum;           /* Data fork/file reference number */
  SInt16 refNumRF;         /* Resource fork reference number (for locking) */
  int locked;              /* 0: unlocked, <0: write lock, >0: read lock */
  int delOnClose;          /* True if file is to be deleted on close */
  char *pathToDel;         /* Name of file to delete on close */
};


#endif /* _SQLITE_OS_MAC_H_ */
