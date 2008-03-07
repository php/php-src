/*
** 2008 Jan 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code to implement a fault-injector used for
** testing and verification of SQLite.
**
** Subsystems within SQLite can call sqlite3FaultStep() to see if
** they should simulate a fault.  sqlite3FaultStep() normally returns
** zero but will return non-zero if a fault should be simulated.
** Fault injectors can be used, for example, to simulate memory
** allocation failures or I/O errors.
**
** The fault injector is omitted from the code if SQLite is
** compiled with -DSQLITE_OMIT_FAULTINJECTOR=1.  There is a very
** small performance hit for leaving the fault injector in the code.
** Commerical products will probably want to omit the fault injector
** from production builds.  But safety-critical systems who work
** under the motto "fly what you test and test what you fly" may
** choose to leave the fault injector enabled even in production.
*/
#include "sqliteInt.h"

#ifndef SQLITE_OMIT_FAULTINJECTOR

/*
** There can be various kinds of faults.  For example, there can be
** a memory allocation failure.  Or an I/O failure.  For each different
** fault type, there is a separate FaultInjector structure to keep track
** of the status of that fault.
*/
static struct FaultInjector {
  int iCountdown;   /* Number of pending successes before we hit a failure */
  int nRepeat;      /* Number of times to repeat the failure */
  int nBenign;      /* Number of benign failures seen since last config */
  int nFail;        /* Number of failures seen since last config */
  u8 enable;        /* True if enabled */
  u8 benign;        /* Ture if next failure will be benign */
} aFault[SQLITE_FAULTINJECTOR_COUNT];

/*
** This routine configures and enables a fault injector.  After
** calling this routine, aFaultStep() will return false (zero)
** nDelay times, then it will return true nRepeat times,
** then it will again begin returning false.
*/
void sqlite3FaultConfig(int id, int nDelay, int nRepeat){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  aFault[id].iCountdown = nDelay;
  aFault[id].nRepeat = nRepeat;
  aFault[id].nBenign = 0;
  aFault[id].nFail = 0;
  aFault[id].enable = nDelay>=0;
  aFault[id].benign = 0;
}

/*
** Return the number of faults (both hard and benign faults) that have
** occurred since the injector was last configured.
*/
int sqlite3FaultFailures(int id){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  return aFault[id].nFail;
}

/*
** Return the number of benign faults that have occurred since the
** injector was last configured.
*/
int sqlite3FaultBenignFailures(int id){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  return aFault[id].nBenign;
}

/*
** Return the number of successes that will occur before the next failure.
** If no failures are scheduled, return -1.
*/
int sqlite3FaultPending(int id){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  if( aFault[id].enable ){
    return aFault[id].iCountdown;
  }else{
    return -1;
  }
}

/* 
** After this routine causes subsequent faults to be either benign
** or hard (not benign), according to the "enable" parameter.
**
** Most faults are hard.  In other words, most faults cause
** an error to be propagated back up to the application interface.
** However, sometimes a fault is easily recoverable.  For example,
** if a malloc fails while resizing a hash table, this is completely
** recoverable simply by not carrying out the resize.  The hash table
** will continue to function normally.  So a malloc failure during
** a hash table resize is a benign fault.  
*/
void sqlite3FaultBenign(int id, int enable){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  aFault[id].benign = enable;
}

/*
** This routine exists as a place to set a breakpoint that will
** fire on any simulated fault.
*/
static void sqlite3Fault(void){
  static int cnt = 0;
  cnt++;
}


/*
** Check to see if a fault should be simulated.  Return true to simulate
** the fault.  Return false if the fault should not be simulated.
*/
int sqlite3FaultStep(int id){
  assert( id>=0 && id<SQLITE_FAULTINJECTOR_COUNT );
  if( likely(!aFault[id].enable) ){
    return 0;
  }
  if( aFault[id].iCountdown>0 ){
    aFault[id].iCountdown--;
    return 0;
  }
  sqlite3Fault();
  aFault[id].nFail++;
  if( aFault[id].benign ){
    aFault[id].nBenign++;
  }
  aFault[id].nRepeat--;
  if( aFault[id].nRepeat<=0 ){
    aFault[id].enable = 0;
  }
  return 1;  
}

#endif /* SQLITE_OMIT_FAULTINJECTOR */
