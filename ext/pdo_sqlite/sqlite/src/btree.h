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
** This header file defines the interface that the sqlite B-Tree file
** subsystem.  See comments in the source code for a detailed description
** of what each interface routine does.
**
** @(#) $Id$
*/
#ifndef _BTREE_H_
#define _BTREE_H_

/* TODO: This definition is just included so other modules compile. It
** needs to be revisited.
*/
#define SQLITE_N_BTREE_META 10

/*
** Forward declarations of structure
*/
typedef struct Btree Btree;
typedef struct BtCursor BtCursor;


int sqlite3BtreeOpen(
  const char *zFilename,   /* Name of database file to open */
  Btree **,                /* Return open Btree* here */
  int flags                /* Flags */
);

/* The flags parameter to sqlite3BtreeOpen can be the bitwise or of the
** following values.
*/
#define BTREE_OMIT_JOURNAL  1  /* Do not use journal.  No argument */
#define BTREE_MEMORY        2  /* In-memory DB.  No argument */

int sqlite3BtreeClose(Btree*);
int sqlite3BtreeSetBusyHandler(Btree*,BusyHandler*);
int sqlite3BtreeSetCacheSize(Btree*,int);
int sqlite3BtreeSetSafetyLevel(Btree*,int);
int sqlite3BtreeSetPageSize(Btree*,int,int);
int sqlite3BtreeGetPageSize(Btree*);
int sqlite3BtreeGetReserve(Btree*);
int sqlite3BtreeBeginTrans(Btree*,int);
int sqlite3BtreeCommit(Btree*);
int sqlite3BtreeRollback(Btree*);
int sqlite3BtreeBeginStmt(Btree*);
int sqlite3BtreeCommitStmt(Btree*);
int sqlite3BtreeRollbackStmt(Btree*);
int sqlite3BtreeCreateTable(Btree*, int*, int flags);
int sqlite3BtreeIsInTrans(Btree*);
int sqlite3BtreeIsInStmt(Btree*);
int sqlite3BtreeSync(Btree*, const char *zMaster);

const char *sqlite3BtreeGetFilename(Btree *);
const char *sqlite3BtreeGetDirname(Btree *);
const char *sqlite3BtreeGetJournalname(Btree *);
int sqlite3BtreeCopyFile(Btree *, Btree *);

/* The flags parameter to sqlite3BtreeCreateTable can be the bitwise OR
** of the following flags:
*/
#define BTREE_INTKEY     1    /* Table has only 64-bit signed integer keys */
#define BTREE_ZERODATA   2    /* Table has keys only - no data */
#define BTREE_LEAFDATA   4    /* Data stored in leaves only.  Implies INTKEY */

int sqlite3BtreeDropTable(Btree*, int);
int sqlite3BtreeClearTable(Btree*, int);
int sqlite3BtreeGetMeta(Btree*, int idx, u32 *pValue);
int sqlite3BtreeUpdateMeta(Btree*, int idx, u32 value);

int sqlite3BtreeCursor(
  Btree*,                              /* BTree containing table to open */
  int iTable,                          /* Index of root page */
  int wrFlag,                          /* 1 for writing.  0 for read-only */
  int(*)(void*,int,const void*,int,const void*),  /* Key comparison function */
  void*,                               /* First argument to compare function */
  BtCursor **ppCursor                  /* Returned cursor */
);

void sqlite3BtreeSetCompare(
  BtCursor *,
  int(*)(void*,int,const void*,int,const void*),
  void*
);

int sqlite3BtreeCloseCursor(BtCursor*);
int sqlite3BtreeMoveto(BtCursor*, const void *pKey, i64 nKey, int *pRes);
int sqlite3BtreeDelete(BtCursor*);
int sqlite3BtreeInsert(BtCursor*, const void *pKey, i64 nKey,
                                  const void *pData, int nData);
int sqlite3BtreeFirst(BtCursor*, int *pRes);
int sqlite3BtreeLast(BtCursor*, int *pRes);
int sqlite3BtreeNext(BtCursor*, int *pRes);
int sqlite3BtreeEof(BtCursor*);
int sqlite3BtreeFlags(BtCursor*);
int sqlite3BtreePrevious(BtCursor*, int *pRes);
int sqlite3BtreeKeySize(BtCursor*, i64 *pSize);
int sqlite3BtreeKey(BtCursor*, u32 offset, u32 amt, void*);
const void *sqlite3BtreeKeyFetch(BtCursor*, int *pAmt);
const void *sqlite3BtreeDataFetch(BtCursor*, int *pAmt);
int sqlite3BtreeDataSize(BtCursor*, u32 *pSize);
int sqlite3BtreeData(BtCursor*, u32 offset, u32 amt, void*);

char *sqlite3BtreeIntegrityCheck(Btree*, int *aRoot, int nRoot);
struct Pager *sqlite3BtreePager(Btree*);


#ifdef SQLITE_TEST
int sqlite3BtreeCursorInfo(BtCursor*, int*, int);
void sqlite3BtreeCursorList(Btree*);
int sqlite3BtreePageDump(Btree*, int, int recursive);
#endif


#endif /* _BTREE_H_ */
