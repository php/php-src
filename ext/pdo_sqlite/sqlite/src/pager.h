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
** This header file defines the interface that the sqlite page cache
** subsystem.  The page cache subsystem reads and writes a file a page
** at a time and provides a journal for rollback.
**
** @(#) $Id$
*/

#ifndef _PAGER_H_
#define _PAGER_H_

/*
** The type used to represent a page number.  The first page in a file
** is called page 1.  0 is used to represent "not a page".
*/
typedef unsigned int Pgno;

/*
** Each open file is managed by a separate instance of the "Pager" structure.
*/
typedef struct Pager Pager;

/*
** Handle type for pages.
*/
typedef struct PgHdr DbPage;

/*
** Allowed values for the flags parameter to sqlite3PagerOpen().
**
** NOTE: This values must match the corresponding BTREE_ values in btree.h.
*/
#define PAGER_OMIT_JOURNAL  0x0001    /* Do not use a rollback journal */
#define PAGER_NO_READLOCK   0x0002    /* Omit readlocks on readonly files */

/*
** Valid values for the second argument to sqlite3PagerLockingMode().
*/
#define PAGER_LOCKINGMODE_QUERY      -1
#define PAGER_LOCKINGMODE_NORMAL      0
#define PAGER_LOCKINGMODE_EXCLUSIVE   1

/*
** See source code comments for a detailed description of the following
** routines:
*/
int sqlite3PagerOpen(sqlite3_vfs *, Pager **ppPager, const char*, int,int,int);
void sqlite3PagerSetBusyhandler(Pager*, BusyHandler *pBusyHandler);
void sqlite3PagerSetDestructor(Pager*, void(*)(DbPage*,int));
void sqlite3PagerSetReiniter(Pager*, void(*)(DbPage*,int));
int sqlite3PagerSetPagesize(Pager*, u16*);
int sqlite3PagerMaxPageCount(Pager*, int);
int sqlite3PagerReadFileheader(Pager*, int, unsigned char*);
void sqlite3PagerSetCachesize(Pager*, int);
int sqlite3PagerClose(Pager *pPager);
int sqlite3PagerAcquire(Pager *pPager, Pgno pgno, DbPage **ppPage, int clrFlag);
#define sqlite3PagerGet(A,B,C) sqlite3PagerAcquire(A,B,C,0)
DbPage *sqlite3PagerLookup(Pager *pPager, Pgno pgno);
int sqlite3PagerRef(DbPage*);
int sqlite3PagerUnref(DbPage*);
int sqlite3PagerWrite(DbPage*);
int sqlite3PagerOverwrite(Pager *pPager, Pgno pgno, void*);
int sqlite3PagerPagecount(Pager*);
int sqlite3PagerTruncate(Pager*,Pgno);
int sqlite3PagerBegin(DbPage*, int exFlag);
int sqlite3PagerCommitPhaseOne(Pager*,const char *zMaster, Pgno);
int sqlite3PagerCommitPhaseTwo(Pager*);
int sqlite3PagerRollback(Pager*);
int sqlite3PagerIsreadonly(Pager*);
int sqlite3PagerStmtBegin(Pager*);
int sqlite3PagerStmtCommit(Pager*);
int sqlite3PagerStmtRollback(Pager*);
void sqlite3PagerDontRollback(DbPage*);
void sqlite3PagerDontWrite(DbPage*);
int sqlite3PagerRefcount(Pager*);
void sqlite3PagerSetSafetyLevel(Pager*,int,int);
const char *sqlite3PagerFilename(Pager*);
const sqlite3_vfs *sqlite3PagerVfs(Pager*);
sqlite3_file *sqlite3PagerFile(Pager*);
const char *sqlite3PagerDirname(Pager*);
const char *sqlite3PagerJournalname(Pager*);
int sqlite3PagerNosync(Pager*);
int sqlite3PagerMovepage(Pager*,DbPage*,Pgno);
void *sqlite3PagerGetData(DbPage *); 
void *sqlite3PagerGetExtra(DbPage *); 
int sqlite3PagerLockingMode(Pager *, int);
void *sqlite3PagerTempSpace(Pager*);

#if defined(SQLITE_ENABLE_MEMORY_MANAGEMENT) && !defined(SQLITE_OMIT_DISKIO)
  int sqlite3PagerReleaseMemory(int);
#endif

#ifdef SQLITE_HAS_CODEC
  void sqlite3PagerSetCodec(Pager*,void*(*)(void*,void*,Pgno,int),void*);
#endif

#if !defined(NDEBUG) || defined(SQLITE_TEST)
  Pgno sqlite3PagerPagenumber(DbPage*);
  int sqlite3PagerIswriteable(DbPage*);
#endif

#ifdef SQLITE_TEST
  int *sqlite3PagerStats(Pager*);
  void sqlite3PagerRefdump(Pager*);
#endif

#ifdef SQLITE_TEST
void disable_simulated_io_errors(void);
void enable_simulated_io_errors(void);
#else
# define disable_simulated_io_errors()
# define enable_simulated_io_errors()
#endif

#endif /* _PAGER_H_ */
