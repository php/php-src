/*
** 2007 August 14
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains the C functions that implement mutexes.
**
** The implementation in this file does not provide any mutual
** exclusion and is thus suitable for use only in applications
** that use SQLite in a single thread.  But this implementation
** does do a lot of error checking on mutexes to make sure they
** are called correctly and at appropriate times.  Hence, this
** implementation is suitable for testing.
** debugging purposes
**
** $Id$
*/
#include "sqliteInt.h"

#ifdef SQLITE_MUTEX_NOOP_DEBUG
/*
** In this implementation, mutexes do not provide any mutual exclusion.
** But the error checking is provided.  This implementation is useful
** for test purposes.
*/

/*
** The mutex object
*/
struct sqlite3_mutex {
  int id;     /* The mutex type */
  int cnt;    /* Number of entries without a matching leave */
};

/*
** The sqlite3_mutex_alloc() routine allocates a new
** mutex and returns a pointer to it.  If it returns NULL
** that means that a mutex could not be allocated. 
*/
sqlite3_mutex *sqlite3_mutex_alloc(int id){
  static sqlite3_mutex aStatic[5];
  sqlite3_mutex *pNew = 0;
  switch( id ){
    case SQLITE_MUTEX_FAST:
    case SQLITE_MUTEX_RECURSIVE: {
      pNew = sqlite3_malloc(sizeof(*pNew));
      if( pNew ){
        pNew->id = id;
        pNew->cnt = 0;
      }
      break;
    }
    default: {
      assert( id-2 >= 0 );
      assert( id-2 < sizeof(aStatic)/sizeof(aStatic[0]) );
      pNew = &aStatic[id-2];
      pNew->id = id;
      break;
    }
  }
  return pNew;
}

/*
** This routine deallocates a previously allocated mutex.
*/
void sqlite3_mutex_free(sqlite3_mutex *p){
  assert( p );
  assert( p->cnt==0 );
  assert( p->id==SQLITE_MUTEX_FAST || p->id==SQLITE_MUTEX_RECURSIVE );
  sqlite3_free(p);
}

/*
** The sqlite3_mutex_enter() and sqlite3_mutex_try() routines attempt
** to enter a mutex.  If another thread is already within the mutex,
** sqlite3_mutex_enter() will block and sqlite3_mutex_try() will return
** SQLITE_BUSY.  The sqlite3_mutex_try() interface returns SQLITE_OK
** upon successful entry.  Mutexes created using SQLITE_MUTEX_RECURSIVE can
** be entered multiple times by the same thread.  In such cases the,
** mutex must be exited an equal number of times before another thread
** can enter.  If the same thread tries to enter any other kind of mutex
** more than once, the behavior is undefined.
*/
void sqlite3_mutex_enter(sqlite3_mutex *p){
  assert( p );
  assert( p->id==SQLITE_MUTEX_RECURSIVE || sqlite3_mutex_notheld(p) );
  p->cnt++;
}
int sqlite3_mutex_try(sqlite3_mutex *p){
  assert( p );
  assert( p->id==SQLITE_MUTEX_RECURSIVE || sqlite3_mutex_notheld(p) );
  p->cnt++;
  return SQLITE_OK;
}

/*
** The sqlite3_mutex_leave() routine exits a mutex that was
** previously entered by the same thread.  The behavior
** is undefined if the mutex is not currently entered or
** is not currently allocated.  SQLite will never do either.
*/
void sqlite3_mutex_leave(sqlite3_mutex *p){
  assert( p );
  assert( sqlite3_mutex_held(p) );
  p->cnt--;
  assert( p->id==SQLITE_MUTEX_RECURSIVE || sqlite3_mutex_notheld(p) );
}

/*
** The sqlite3_mutex_held() and sqlite3_mutex_notheld() routine are
** intended for use inside assert() statements.
*/
int sqlite3_mutex_held(sqlite3_mutex *p){
  return p==0 || p->cnt>0;
}
int sqlite3_mutex_notheld(sqlite3_mutex *p){
  return p==0 || p->cnt==0;
}
#endif /* SQLITE_MUTEX_NOOP_DEBUG */
