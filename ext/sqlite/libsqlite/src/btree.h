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

typedef struct Btree Btree;
typedef struct BtCursor BtCursor;

int sqliteBtreeOpen(const char *zFilename, int mode, int nPg, Btree **ppBtree);
int sqliteBtreeClose(Btree*);
int sqliteBtreeSetCacheSize(Btree*, int);
int sqliteBtreeSetSafetyLevel(Btree*, int);

int sqliteBtreeBeginTrans(Btree*);
int sqliteBtreeCommit(Btree*);
int sqliteBtreeRollback(Btree*);
int sqliteBtreeBeginCkpt(Btree*);
int sqliteBtreeCommitCkpt(Btree*);
int sqliteBtreeRollbackCkpt(Btree*);

int sqliteBtreeCreateTable(Btree*, int*);
int sqliteBtreeCreateIndex(Btree*, int*);
int sqliteBtreeDropTable(Btree*, int);
int sqliteBtreeClearTable(Btree*, int);

int sqliteBtreeCursor(Btree*, int iTable, int wrFlag, BtCursor **ppCur);
int sqliteBtreeMoveto(BtCursor*, const void *pKey, int nKey, int *pRes);
int sqliteBtreeDelete(BtCursor*);
int sqliteBtreeInsert(BtCursor*, const void *pKey, int nKey,
                                 const void *pData, int nData);
int sqliteBtreeFirst(BtCursor*, int *pRes);
int sqliteBtreeLast(BtCursor*, int *pRes);
int sqliteBtreeNext(BtCursor*, int *pRes);
int sqliteBtreePrevious(BtCursor*, int *pRes);
int sqliteBtreeKeySize(BtCursor*, int *pSize);
int sqliteBtreeKey(BtCursor*, int offset, int amt, char *zBuf);
int sqliteBtreeKeyCompare(BtCursor*, const void *pKey, int nKey,
                          int nIgnore, int *pRes);
int sqliteBtreeDataSize(BtCursor*, int *pSize);
int sqliteBtreeData(BtCursor*, int offset, int amt, char *zBuf);
int sqliteBtreeCloseCursor(BtCursor*);

#define SQLITE_N_BTREE_META 10
int sqliteBtreeGetMeta(Btree*, int*);
int sqliteBtreeUpdateMeta(Btree*, int*);

char *sqliteBtreeIntegrityCheck(Btree*, int*, int);

#ifdef SQLITE_TEST
int sqliteBtreePageDump(Btree*, int, int);
int sqliteBtreeCursorDump(BtCursor*, int*);
struct Pager *sqliteBtreePager(Btree*);
int btree_native_byte_order;
#endif

#endif /* _BTREE_H_ */
