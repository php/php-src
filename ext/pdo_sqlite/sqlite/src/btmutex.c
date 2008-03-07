/*
** 2007 August 27
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
** $Id$
**
** This file contains code used to implement mutexes on Btree objects.
** This code really belongs in btree.c.  But btree.c is getting too
** big and we want to break it down some.  This packaged seemed like
** a good breakout.
*/
#include "btreeInt.h"
#if SQLITE_THREADSAFE && !defined(SQLITE_OMIT_SHARED_CACHE)


/*
** Enter a mutex on the given BTree object.
**
** If the object is not sharable, then no mutex is ever required
** and this routine is a no-op.  The underlying mutex is non-recursive.
** But we keep a reference count in Btree.wantToLock so the behavior
** of this interface is recursive.
**
** To avoid deadlocks, multiple Btrees are locked in the same order
** by all database connections.  The p->pNext is a list of other
** Btrees belonging to the same database connection as the p Btree
** which need to be locked after p.  If we cannot get a lock on
** p, then first unlock all of the others on p->pNext, then wait
** for the lock to become available on p, then relock all of the
** subsequent Btrees that desire a lock.
*/
void sqlite3BtreeEnter(Btree *p){
  Btree *pLater;

  /* Some basic sanity checking on the Btree.  The list of Btrees
  ** connected by pNext and pPrev should be in sorted order by
  ** Btree.pBt value. All elements of the list should belong to
  ** the same connection. Only shared Btrees are on the list. */
  assert( p->pNext==0 || p->pNext->pBt>p->pBt );
  assert( p->pPrev==0 || p->pPrev->pBt<p->pBt );
  assert( p->pNext==0 || p->pNext->db==p->db );
  assert( p->pPrev==0 || p->pPrev->db==p->db );
  assert( p->sharable || (p->pNext==0 && p->pPrev==0) );

  /* Check for locking consistency */
  assert( !p->locked || p->wantToLock>0 );
  assert( p->sharable || p->wantToLock==0 );

  /* We should already hold a lock on the database connection */
  assert( sqlite3_mutex_held(p->db->mutex) );

  if( !p->sharable ) return;
  p->wantToLock++;
  if( p->locked ) return;

#ifndef SQLITE_MUTEX_NOOP
  /* In most cases, we should be able to acquire the lock we
  ** want without having to go throught the ascending lock
  ** procedure that follows.  Just be sure not to block.
  */
  if( sqlite3_mutex_try(p->pBt->mutex)==SQLITE_OK ){
    p->locked = 1;
    return;
  }

  /* To avoid deadlock, first release all locks with a larger
  ** BtShared address.  Then acquire our lock.  Then reacquire
  ** the other BtShared locks that we used to hold in ascending
  ** order.
  */
  for(pLater=p->pNext; pLater; pLater=pLater->pNext){
    assert( pLater->sharable );
    assert( pLater->pNext==0 || pLater->pNext->pBt>pLater->pBt );
    assert( !pLater->locked || pLater->wantToLock>0 );
    if( pLater->locked ){
      sqlite3_mutex_leave(pLater->pBt->mutex);
      pLater->locked = 0;
    }
  }
  sqlite3_mutex_enter(p->pBt->mutex);
  p->locked = 1;
  for(pLater=p->pNext; pLater; pLater=pLater->pNext){
    if( pLater->wantToLock ){
      sqlite3_mutex_enter(pLater->pBt->mutex);
      pLater->locked = 1;
    }
  }
#endif /* SQLITE_MUTEX_NOOP */
}

/*
** Exit the recursive mutex on a Btree.
*/
void sqlite3BtreeLeave(Btree *p){
  if( p->sharable ){
    assert( p->wantToLock>0 );
    p->wantToLock--;
    if( p->wantToLock==0 ){
      assert( p->locked );
      sqlite3_mutex_leave(p->pBt->mutex);
      p->locked = 0;
    }
  }
}

#ifndef NDEBUG
/*
** Return true if the BtShared mutex is held on the btree.  
**
** This routine makes no determination one why or another if the
** database connection mutex is held.
**
** This routine is used only from within assert() statements.
*/
int sqlite3BtreeHoldsMutex(Btree *p){
  return (p->sharable==0 ||
             (p->locked && p->wantToLock && sqlite3_mutex_held(p->pBt->mutex)));
}
#endif


#ifndef SQLITE_OMIT_INCRBLOB
/*
** Enter and leave a mutex on a Btree given a cursor owned by that
** Btree.  These entry points are used by incremental I/O and can be
** omitted if that module is not used.
*/
void sqlite3BtreeEnterCursor(BtCursor *pCur){
  sqlite3BtreeEnter(pCur->pBtree);
}
void sqlite3BtreeLeaveCursor(BtCursor *pCur){
  sqlite3BtreeLeave(pCur->pBtree);
}
#endif /* SQLITE_OMIT_INCRBLOB */


/*
** Enter the mutex on every Btree associated with a database
** connection.  This is needed (for example) prior to parsing
** a statement since we will be comparing table and column names
** against all schemas and we do not want those schemas being
** reset out from under us.
**
** There is a corresponding leave-all procedures.
**
** Enter the mutexes in accending order by BtShared pointer address
** to avoid the possibility of deadlock when two threads with
** two or more btrees in common both try to lock all their btrees
** at the same instant.
*/
void sqlite3BtreeEnterAll(sqlite3 *db){
  int i;
  Btree *p, *pLater;
  assert( sqlite3_mutex_held(db->mutex) );
  for(i=0; i<db->nDb; i++){
    p = db->aDb[i].pBt;
    if( p && p->sharable ){
      p->wantToLock++;
      if( !p->locked ){
        assert( p->wantToLock==1 );
        while( p->pPrev ) p = p->pPrev;
        while( p->locked && p->pNext ) p = p->pNext;
        for(pLater = p->pNext; pLater; pLater=pLater->pNext){
          if( pLater->locked ){
            sqlite3_mutex_leave(pLater->pBt->mutex);
            pLater->locked = 0;
          }
        }
        while( p ){
          sqlite3_mutex_enter(p->pBt->mutex);
          p->locked++;
          p = p->pNext;
        }
      }
    }
  }
}
void sqlite3BtreeLeaveAll(sqlite3 *db){
  int i;
  Btree *p;
  assert( sqlite3_mutex_held(db->mutex) );
  for(i=0; i<db->nDb; i++){
    p = db->aDb[i].pBt;
    if( p && p->sharable ){
      assert( p->wantToLock>0 );
      p->wantToLock--;
      if( p->wantToLock==0 ){
        assert( p->locked );
        sqlite3_mutex_leave(p->pBt->mutex);
        p->locked = 0;
      }
    }
  }
}

#ifndef NDEBUG
/*
** Return true if the current thread holds the database connection
** mutex and all required BtShared mutexes.
**
** This routine is used inside assert() statements only.
*/
int sqlite3BtreeHoldsAllMutexes(sqlite3 *db){
  int i;
  if( !sqlite3_mutex_held(db->mutex) ){
    return 0;
  }
  for(i=0; i<db->nDb; i++){
    Btree *p;
    p = db->aDb[i].pBt;
    if( p && p->sharable &&
         (p->wantToLock==0 || !sqlite3_mutex_held(p->pBt->mutex)) ){
      return 0;
    }
  }
  return 1;
}
#endif /* NDEBUG */

/*
** Potentially dd a new Btree pointer to a BtreeMutexArray.
** Really only add the Btree if it can possibly be shared with
** another database connection.
**
** The Btrees are kept in sorted order by pBtree->pBt.  That
** way when we go to enter all the mutexes, we can enter them
** in order without every having to backup and retry and without
** worrying about deadlock.
**
** The number of shared btrees will always be small (usually 0 or 1)
** so an insertion sort is an adequate algorithm here.
*/
void sqlite3BtreeMutexArrayInsert(BtreeMutexArray *pArray, Btree *pBtree){
  int i, j;
  BtShared *pBt;
  if( pBtree==0 || pBtree->sharable==0 ) return;
#ifndef NDEBUG
  {
    for(i=0; i<pArray->nMutex; i++){
      assert( pArray->aBtree[i]!=pBtree );
    }
  }
#endif
  assert( pArray->nMutex>=0 );
  assert( pArray->nMutex<sizeof(pArray->aBtree)/sizeof(pArray->aBtree[0])-1 );
  pBt = pBtree->pBt;
  for(i=0; i<pArray->nMutex; i++){
    assert( pArray->aBtree[i]!=pBtree );
    if( pArray->aBtree[i]->pBt>pBt ){
      for(j=pArray->nMutex; j>i; j--){
        pArray->aBtree[j] = pArray->aBtree[j-1];
      }
      pArray->aBtree[i] = pBtree;
      pArray->nMutex++;
      return;
    }
  }
  pArray->aBtree[pArray->nMutex++] = pBtree;
}

/*
** Enter the mutex of every btree in the array.  This routine is
** called at the beginning of sqlite3VdbeExec().  The mutexes are
** exited at the end of the same function.
*/
void sqlite3BtreeMutexArrayEnter(BtreeMutexArray *pArray){
  int i;
  for(i=0; i<pArray->nMutex; i++){
    Btree *p = pArray->aBtree[i];
    /* Some basic sanity checking */
    assert( i==0 || pArray->aBtree[i-1]->pBt<p->pBt );
    assert( !p->locked || p->wantToLock>0 );

    /* We should already hold a lock on the database connection */
    assert( sqlite3_mutex_held(p->db->mutex) );

    p->wantToLock++;
    if( !p->locked && p->sharable ){
      sqlite3_mutex_enter(p->pBt->mutex);
      p->locked = 1;
    }
  }
}

/*
** Leave the mutex of every btree in the group.
*/
void sqlite3BtreeMutexArrayLeave(BtreeMutexArray *pArray){
  int i;
  for(i=0; i<pArray->nMutex; i++){
    Btree *p = pArray->aBtree[i];
    /* Some basic sanity checking */
    assert( i==0 || pArray->aBtree[i-1]->pBt<p->pBt );
    assert( p->locked || !p->sharable );
    assert( p->wantToLock>0 );

    /* We should already hold a lock on the database connection */
    assert( sqlite3_mutex_held(p->db->mutex) );

    p->wantToLock--;
    if( p->wantToLock==0 && p->locked ){
      sqlite3_mutex_leave(p->pBt->mutex);
      p->locked = 0;
    }
  }
}


#endif  /* SQLITE_THREADSAFE && !SQLITE_OMIT_SHARED_CACHE */
