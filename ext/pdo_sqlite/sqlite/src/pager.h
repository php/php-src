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
** The default size of a database page.
*/
#ifndef SQLITE_DEFAULT_PAGE_SIZE
# define SQLITE_DEFAULT_PAGE_SIZE 1024
#endif

/* Maximum page size.  The upper bound on this value is 32768.  This a limit
** imposed by necessity of storing the value in a 2-byte unsigned integer
** and the fact that the page size must be a power of 2.
**
** This value is used to initialize certain arrays on the stack at
** various places in the code.  On embedded machines where stack space
** is limited and the flexibility of having large pages is not needed,
** it makes good sense to reduce the maximum page size to something more
** reasonable, like 1024.
*/
#ifndef SQLITE_MAX_PAGE_SIZE
# define SQLITE_MAX_PAGE_SIZE 32768
#endif

/*
** Maximum number of pages in one database.
*/
#define SQLITE_MAX_PAGE 1073741823

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
int sqlite3PagerOpen(Pager **ppPager, const char *zFilename,
                     int nExtra, int flags);
void sqlite3PagerSetBusyhandler(Pager*, BusyHandler *pBusyHandler);
void sqlite3PagerSetDestructor(Pager*, void(*)(DbPage*,int));
void sqlite3PagerSetReiniter(Pager*, void(*)(DbPage*,int));
int sqlite3PagerSetPagesize(Pager*, int);
int sqlite3PagerReadFileheader(Pager*, int, unsigned char*);
void sqlite3PagerSetCachesize(Pager*, int);
int sqlite3PagerClose(Pager *pPager);
int sqlite3PagerAcquire(Pager *pPager, Pgno pgno, DbPage **ppPage, int clrFlag);
#define sqlite3PagerGet(A,B,C) sqlite3PagerAcquire(A,B,C,0)
DbPage *sqlite3PagerLookup(Pager *pPager, Pgno pgno);
int sqlite3PagerRef(DbPage*);
int sqlite3PagerUnref(DbPage*);
Pgno sqlite3PagerPagenumber(DbPage*);
int sqlite3PagerWrite(DbPage*);
int sqlite3PagerIswriteable(DbPage*);
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
int *sqlite3PagerStats(Pager*);
void sqlite3PagerSetSafetyLevel(Pager*,int,int);
const char *sqlite3PagerFilename(Pager*);
const char *sqlite3PagerDirname(Pager*);
const char *sqlite3PagerJournalname(Pager*);
int sqlite3PagerNosync(Pager*);
int sqlite3PagerRename(Pager*, const char *zNewName);
void sqlite3PagerSetCodec(Pager*,void*(*)(void*,void*,Pgno,int),void*);
int sqlite3PagerMovepage(Pager*,DbPage*,Pgno);
int sqlite3PagerReset(Pager*);
int sqlite3PagerReleaseMemory(int);

void *sqlite3PagerGetData(DbPage *); 
void *sqlite3PagerGetExtra(DbPage *); 
int sqlite3PagerLockingMode(Pager *, int);

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
int sqlite3PagerLockstate(Pager*);
#endif

#ifdef SQLITE_TEST
void sqlite3PagerRefdump(Pager*);
int pager3_refinfo_enable;
#endif

#ifdef SQLITE_TEST
void disable_simulated_io_errors(void);
void enable_simulated_io_errors(void);
#else
# define disable_simulated_io_errors()
# define enable_simulated_io_errors()
#endif

#endif /* _PAGER_H_ */
