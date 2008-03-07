/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code to implement a pseudo-random number
** generator (PRNG) for SQLite.
**
** Random numbers are used by some of the database backends in order
** to generate random integer keys for tables or random filenames.
**
** $Id$
*/
#include "sqliteInt.h"


/* All threads share a single random number generator.
** This structure is the current state of the generator.
*/
static struct sqlite3PrngType {
  unsigned char isInit;          /* True if initialized */
  unsigned char i, j;            /* State variables */
  unsigned char s[256];          /* State variables */
} sqlite3Prng;

/*
** Get a single 8-bit random value from the RC4 PRNG.  The Mutex
** must be held while executing this routine.
**
** Why not just use a library random generator like lrand48() for this?
** Because the OP_NewRowid opcode in the VDBE depends on having a very
** good source of random numbers.  The lrand48() library function may
** well be good enough.  But maybe not.  Or maybe lrand48() has some
** subtle problems on some systems that could cause problems.  It is hard
** to know.  To minimize the risk of problems due to bad lrand48()
** implementations, SQLite uses this random number generator based
** on RC4, which we know works very well.
**
** (Later):  Actually, OP_NewRowid does not depend on a good source of
** randomness any more.  But we will leave this code in all the same.
*/
static int randomByte(void){
  unsigned char t;


  /* Initialize the state of the random number generator once,
  ** the first time this routine is called.  The seed value does
  ** not need to contain a lot of randomness since we are not
  ** trying to do secure encryption or anything like that...
  **
  ** Nothing in this file or anywhere else in SQLite does any kind of
  ** encryption.  The RC4 algorithm is being used as a PRNG (pseudo-random
  ** number generator) not as an encryption device.
  */
  if( !sqlite3Prng.isInit ){
    int i;
    char k[256];
    sqlite3Prng.j = 0;
    sqlite3Prng.i = 0;
    sqlite3OsRandomness(sqlite3_vfs_find(0), 256, k);
    for(i=0; i<256; i++){
      sqlite3Prng.s[i] = i;
    }
    for(i=0; i<256; i++){
      sqlite3Prng.j += sqlite3Prng.s[i] + k[i];
      t = sqlite3Prng.s[sqlite3Prng.j];
      sqlite3Prng.s[sqlite3Prng.j] = sqlite3Prng.s[i];
      sqlite3Prng.s[i] = t;
    }
    sqlite3Prng.isInit = 1;
  }

  /* Generate and return single random byte
  */
  sqlite3Prng.i++;
  t = sqlite3Prng.s[sqlite3Prng.i];
  sqlite3Prng.j += t;
  sqlite3Prng.s[sqlite3Prng.i] = sqlite3Prng.s[sqlite3Prng.j];
  sqlite3Prng.s[sqlite3Prng.j] = t;
  t += sqlite3Prng.s[sqlite3Prng.i];
  return sqlite3Prng.s[t];
}

/*
** Return N random bytes.
*/
void sqlite3Randomness(int N, void *pBuf){
  unsigned char *zBuf = pBuf;
  static sqlite3_mutex *mutex = 0;
  if( mutex==0 ){
    mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_PRNG);
  }
  sqlite3_mutex_enter(mutex);
  while( N-- ){
    *(zBuf++) = randomByte();
  }
  sqlite3_mutex_leave(mutex);
}

#ifdef SQLITE_TEST
/*
** For testing purposes, we sometimes want to preserve the state of
** PRNG and restore the PRNG to its saved state at a later time.
*/
static struct sqlite3PrngType sqlite3SavedPrng;
void sqlite3SavePrngState(void){
  memcpy(&sqlite3SavedPrng, &sqlite3Prng, sizeof(sqlite3Prng));
}
void sqlite3RestorePrngState(void){
  memcpy(&sqlite3Prng, &sqlite3SavedPrng, sizeof(sqlite3Prng));
}
void sqlite3ResetPrngState(void){
  sqlite3Prng.isInit = 0;
}
#endif /* SQLITE_TEST */
