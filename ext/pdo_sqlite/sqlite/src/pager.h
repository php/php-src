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
# define SQLITE_MAX_PAGE_SIZE 8192
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
** Allowed values for the flags parameter to sqlite3pager_open().
**
** NOTE: This values must match the corresponding BTREE_ values in btree.h.
*/
#define PAGER_OMIT_JOURNAL  0x0001    /* Do not use a rollback journal */
#define PAGER_NO_READLOCK   0x0002    /* Omit readlocks on readonly files */


/*
** See source code comments for a detailed description of the following
** routines:
*/
int sqlite3pager_open(Pager **ppPager, const char *zFilename,
                     int nExtra, int flags);
void sqlite3pager_set_busyhandler(Pager*, BusyHandler *pBusyHandler);
void sqlite3pager_set_destructor(Pager*, void(*)(void*,int));
void sqlite3pager_set_reiniter(Pager*, void(*)(void*,int));
int sqlite3pager_set_pagesize(Pager*, int);
void sqlite3pager_read_fileheader(Pager*, int, unsigned char*);
void sqlite3pager_set_cachesize(Pager*, int);
int sqlite3pager_close(Pager *pPager);
int sqlite3pager_get(Pager *pPager, Pgno pgno, void **ppPage);
void *sqlite3pager_lookup(Pager *pPager, Pgno pgno);
int sqlite3pager_ref(void*);
int sqlite3pager_unref(void*);
Pgno sqlite3pager_pagenumber(void*);
int sqlite3pager_write(void*);
int sqlite3pager_iswriteable(void*);
int sqlite3pager_overwrite(Pager *pPager, Pgno pgno, void*);
int sqlite3pager_pagecount(Pager*);
int sqlite3pager_truncate(Pager*,Pgno);
int sqlite3pager_begin(void*, int exFlag);
int sqlite3pager_commit(Pager*);
int sqlite3pager_sync(Pager*,const char *zMaster, Pgno);
int sqlite3pager_rollback(Pager*);
int sqlite3pager_isreadonly(Pager*);
int sqlite3pager_stmt_begin(Pager*);
int sqlite3pager_stmt_commit(Pager*);
int sqlite3pager_stmt_rollback(Pager*);
void sqlite3pager_dont_rollback(void*);
void sqlite3pager_dont_write(Pager*, Pgno);
int *sqlite3pager_stats(Pager*);
void sqlite3pager_set_safety_level(Pager*,int);
const char *sqlite3pager_filename(Pager*);
const char *sqlite3pager_dirname(Pager*);
const char *sqlite3pager_journalname(Pager*);
int sqlite3pager_nosync(Pager*);
int sqlite3pager_rename(Pager*, const char *zNewName);
void sqlite3pager_set_codec(Pager*,void(*)(void*,void*,Pgno,int),void*);
int sqlite3pager_movepage(Pager*,void*,Pgno);
int sqlite3pager_reset(Pager*);

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
int sqlite3pager_lockstate(Pager*);
#endif

#ifdef SQLITE_TEST
void sqlite3pager_refdump(Pager*);
int pager3_refinfo_enable;
#endif
