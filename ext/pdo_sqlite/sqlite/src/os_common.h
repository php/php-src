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
** This file contains macros and a little bit of code that is common to
** all of the platform-specific files (os_*.c) and is #included into those
** files.
**
** This file should be #included by the os_*.c files only.  It is not a
** general purpose header file.
*/

/*
** At least two bugs have slipped in because we changed the MEMORY_DEBUG
** macro to SQLITE_DEBUG and some older makefiles have not yet made the
** switch.  The following code should catch this problem at compile-time.
*/
#ifdef MEMORY_DEBUG
# error "The MEMORY_DEBUG macro is obsolete.  Use SQLITE_DEBUG instead."
#endif


int sqlite3_os_trace = 0;
#ifdef SQLITE_DEBUG
static int last_page = 0;
#define SEEK(X)           last_page=(X)
#define TRACE1(X)         if( sqlite3_os_trace ) sqlite3DebugPrintf(X)
#define TRACE2(X,Y)       if( sqlite3_os_trace ) sqlite3DebugPrintf(X,Y)
#define TRACE3(X,Y,Z)     if( sqlite3_os_trace ) sqlite3DebugPrintf(X,Y,Z)
#define TRACE4(X,Y,Z,A)   if( sqlite3_os_trace ) sqlite3DebugPrintf(X,Y,Z,A)
#define TRACE5(X,Y,Z,A,B) if( sqlite3_os_trace ) sqlite3DebugPrintf(X,Y,Z,A,B)
#define TRACE6(X,Y,Z,A,B,C) if(sqlite3_os_trace) sqlite3DebugPrintf(X,Y,Z,A,B,C)
#define TRACE7(X,Y,Z,A,B,C,D) \
    if(sqlite3_os_trace) sqlite3DebugPrintf(X,Y,Z,A,B,C,D)
#else
#define SEEK(X)
#define TRACE1(X)
#define TRACE2(X,Y)
#define TRACE3(X,Y,Z)
#define TRACE4(X,Y,Z,A)
#define TRACE5(X,Y,Z,A,B)
#define TRACE6(X,Y,Z,A,B,C)
#define TRACE7(X,Y,Z,A,B,C,D)
#endif

/*
** Macros for performance tracing.  Normally turned off.  Only works
** on i486 hardware.
*/
#ifdef SQLITE_PERFORMANCE_TRACE
__inline__ unsigned long long int hwtime(void){
  unsigned long long int x;
  __asm__("rdtsc\n\t"
          "mov %%edx, %%ecx\n\t"
          :"=A" (x));
  return x;
}
static unsigned long long int g_start;
static unsigned int elapse;
#define TIMER_START       g_start=hwtime()
#define TIMER_END         elapse=hwtime()-g_start
#define TIMER_ELAPSED     elapse
#else
#define TIMER_START
#define TIMER_END
#define TIMER_ELAPSED     0
#endif

/*
** If we compile with the SQLITE_TEST macro set, then the following block
** of code will give us the ability to simulate a disk I/O error.  This
** is used for testing the I/O recovery logic.
*/
#ifdef SQLITE_TEST
int sqlite3_io_error_pending = 0;
int sqlite3_diskfull_pending = 0;
#define SimulateIOError(A)  \
   if( sqlite3_io_error_pending ) \
     if( sqlite3_io_error_pending-- == 1 ){ local_ioerr(); return A; }
static void local_ioerr(){
  sqlite3_io_error_pending = 0;  /* Really just a place to set a breakpoint */
}
#define SimulateDiskfullError \
   if( sqlite3_diskfull_pending ) \
     if( sqlite3_diskfull_pending-- == 1 ){ local_ioerr(); return SQLITE_FULL; }
#else
#define SimulateIOError(A)
#define SimulateDiskfullError
#endif

/*
** When testing, keep a count of the number of open files.
*/
#ifdef SQLITE_TEST
int sqlite3_open_file_count = 0;
#define OpenCounter(X)  sqlite3_open_file_count+=(X)
#else
#define OpenCounter(X)
#endif
