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
** This header file defines OS-specific features for Win32
*/
#ifndef _SQLITE_OS_WIN_H_
#define _SQLITE_OS_WIN_H_

#include <windows.h>
#include <winbase.h>

/*
** The OsFile structure is a operating-system independing representation
** of an open file handle.  It is defined differently for each architecture.
**
** This is the definition for Win32.
*/
typedef struct OsFile OsFile;
struct OsFile {
  HANDLE h;               /* Handle for accessing the file */
  unsigned char locktype; /* Type of lock currently held on this file */
  unsigned char isOpen;   /* True if needs to be closed */
  short sharedLockByte;   /* Randomly chosen byte used as a shared lock */
};


#define SQLITE_TEMPNAME_SIZE (MAX_PATH+50)
#define SQLITE_MIN_SLEEP_MS 1


#endif /* _SQLITE_OS_WIN_H_ */
