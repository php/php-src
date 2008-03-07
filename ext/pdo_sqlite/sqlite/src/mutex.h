/*
** 2007 August 28
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
**
** This file contains the common header for all mutex implementations.
** The sqliteInt.h header #includes this file so that it is available
** to all source files.  We break it out in an effort to keep the code
** better organized.
**
** NOTE:  source files should *not* #include this header file directly.
** Source files should #include the sqliteInt.h file and let that file
** include this one indirectly.
**
** $Id$
*/


#ifdef SQLITE_MUTEX_APPDEF
/*
** If SQLITE_MUTEX_APPDEF is defined, then this whole module is
** omitted and equivalent functionality must be provided by the
** application that links against the SQLite library.
*/
#else
/*
** Figure out what version of the code to use.  The choices are
**
**   SQLITE_MUTEX_NOOP         For single-threaded applications that
**                             do not desire error checking.
**
**   SQLITE_MUTEX_NOOP_DEBUG   For single-threaded applications with
**                             error checking to help verify that mutexes
**                             are being used correctly even though they
**                             are not needed.  Used when SQLITE_DEBUG is
**                             defined on single-threaded builds.
**
**   SQLITE_MUTEX_PTHREADS     For multi-threaded applications on Unix.
**
**   SQLITE_MUTEX_W32          For multi-threaded applications on Win32.
**
**   SQLITE_MUTEX_OS2          For multi-threaded applications on OS/2.
*/
#define SQLITE_MUTEX_NOOP 1   /* The default */
#if defined(SQLITE_DEBUG) && !SQLITE_THREADSAFE
# undef SQLITE_MUTEX_NOOP
# define SQLITE_MUTEX_NOOP_DEBUG
#endif
#if defined(SQLITE_MUTEX_NOOP) && SQLITE_THREADSAFE && OS_UNIX
# undef SQLITE_MUTEX_NOOP
# define SQLITE_MUTEX_PTHREADS
#endif
#if defined(SQLITE_MUTEX_NOOP) && SQLITE_THREADSAFE && OS_WIN
# undef SQLITE_MUTEX_NOOP
# define SQLITE_MUTEX_W32
#endif
#if defined(SQLITE_MUTEX_NOOP) && SQLITE_THREADSAFE && OS_OS2
# undef SQLITE_MUTEX_NOOP
# define SQLITE_MUTEX_OS2
#endif

#ifdef SQLITE_MUTEX_NOOP
/*
** If this is a no-op implementation, implement everything as macros.
*/
#define sqlite3_mutex_alloc(X)    ((sqlite3_mutex*)8)
#define sqlite3_mutex_free(X)
#define sqlite3_mutex_enter(X)
#define sqlite3_mutex_try(X)      SQLITE_OK
#define sqlite3_mutex_leave(X)
#define sqlite3_mutex_held(X)     1
#define sqlite3_mutex_notheld(X)  1
#endif

#endif /* SQLITE_MUTEX_APPDEF */
