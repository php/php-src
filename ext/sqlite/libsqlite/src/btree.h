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

/*
** Forward declarations of structure
*/
typedef struct Btree Btree;
typedef struct BtCursor BtCursor;
typedef struct BtOps BtOps;
typedef struct BtCursorOps BtCursorOps;


/*
** An instance of the following structure contains pointers to all
** methods against an open BTree.  Alternative BTree implementations
** (examples: file based versus in-memory) can be created by substituting
** different methods.  Users of the BTree cannot tell the difference.
**
** In C++ we could do this by defining a virtual base class and then
** creating subclasses for each different implementation.  But this is
** C not C++ so we have to be a little more explicit.
*/
struct BtOps {
    int (*Close)(Btree*);
    int (*SetCacheSize)(Btree*, int);
    int (*SetSafetyLevel)(Btree*, int);
    int (*BeginTrans)(Btree*);
    int (*Commit)(Btree*);
    int (*Rollback)(Btree*);
    int (*BeginCkpt)(Btree*);
    int (*CommitCkpt)(Btree*);
    int (*RollbackCkpt)(Btree*);
    int (*CreateTable)(Btree*, int*);
    int (*CreateIndex)(Btree*, int*);
    int (*DropTable)(Btree*, int);
    int (*ClearTable)(Btree*, int);
    int (*Cursor)(Btree*, int iTable, int wrFlag, BtCursor **ppCur);
    int (*GetMeta)(Btree*, int*);
    int (*UpdateMeta)(Btree*, int*);
    char *(*IntegrityCheck)(Btree*, int*, int);
    const char *(*GetFilename)(Btree*);
    int (*Copyfile)(Btree*,Btree*);
    struct Pager *(*Pager)(Btree*);
#ifdef SQLITE_TEST
    int (*PageDump)(Btree*, int, int);
#endif
};

/*
** An instance of this structure defines all of the methods that can
** be executed against a cursor.
*/
struct BtCursorOps {
    int (*Moveto)(BtCursor*, const void *pKey, int nKey, int *pRes);
    int (*Delete)(BtCursor*);
    int (*Insert)(BtCursor*, const void *pKey, int nKey,
                             const void *pData, int nData);
    int (*First)(BtCursor*, int *pRes);
    int (*Last)(BtCursor*, int *pRes);
    int (*Next)(BtCursor*, int *pRes);
    int (*Previous)(BtCursor*, int *pRes);
    int (*KeySize)(BtCursor*, int *pSize);
    int (*Key)(BtCursor*, int offset, int amt, char *zBuf);
    int (*KeyCompare)(BtCursor*, const void *pKey, int nKey,
                                 int nIgnore, int *pRes);
    int (*DataSize)(BtCursor*, int *pSize);
    int (*Data)(BtCursor*, int offset, int amt, char *zBuf);
    int (*CloseCursor)(BtCursor*);
#ifdef SQLITE_TEST
    int (*CursorDump)(BtCursor*, int*);
#endif
};

/*
** The number of 4-byte "meta" values contained on the first page of each
** database file.
*/
#define SQLITE_N_BTREE_META 10

int sqliteBtreeOpen(const char *zFilename, int mode, int nPg, Btree **ppBtree);
int sqliteRbtreeOpen(const char *zFilename, int mode, int nPg, Btree **ppBtree);

#define btOps(pBt) (*((BtOps **)(pBt)))
#define btCOps(pCur) (*((BtCursorOps **)(pCur)))

#define sqliteBtreeClose(pBt)              (btOps(pBt)->Close(pBt))
#define sqliteBtreeSetCacheSize(pBt, sz)   (btOps(pBt)->SetCacheSize(pBt, sz))
#define sqliteBtreeSetSafetyLevel(pBt, sl) (btOps(pBt)->SetSafetyLevel(pBt, sl))
#define sqliteBtreeBeginTrans(pBt)         (btOps(pBt)->BeginTrans(pBt))
#define sqliteBtreeCommit(pBt)             (btOps(pBt)->Commit(pBt))
#define sqliteBtreeRollback(pBt)           (btOps(pBt)->Rollback(pBt))
#define sqliteBtreeBeginCkpt(pBt)          (btOps(pBt)->BeginCkpt(pBt))
#define sqliteBtreeCommitCkpt(pBt)         (btOps(pBt)->CommitCkpt(pBt))
#define sqliteBtreeRollbackCkpt(pBt)       (btOps(pBt)->RollbackCkpt(pBt))
#define sqliteBtreeCreateTable(pBt,piTable)\
                (btOps(pBt)->CreateTable(pBt,piTable))
#define sqliteBtreeCreateIndex(pBt, piIndex)\
                (btOps(pBt)->CreateIndex(pBt, piIndex))
#define sqliteBtreeDropTable(pBt, iTable) (btOps(pBt)->DropTable(pBt, iTable))
#define sqliteBtreeClearTable(pBt, iTable)\
                (btOps(pBt)->ClearTable(pBt, iTable))
#define sqliteBtreeCursor(pBt, iTable, wrFlag, ppCur)\
                (btOps(pBt)->Cursor(pBt, iTable, wrFlag, ppCur))
#define sqliteBtreeMoveto(pCur, pKey, nKey, pRes)\
                (btCOps(pCur)->Moveto(pCur, pKey, nKey, pRes))
#define sqliteBtreeDelete(pCur)           (btCOps(pCur)->Delete(pCur))
#define sqliteBtreeInsert(pCur, pKey, nKey, pData, nData) \
                (btCOps(pCur)->Insert(pCur, pKey, nKey, pData, nData))
#define sqliteBtreeFirst(pCur, pRes)      (btCOps(pCur)->First(pCur, pRes))
#define sqliteBtreeLast(pCur, pRes)       (btCOps(pCur)->Last(pCur, pRes))
#define sqliteBtreeNext(pCur, pRes)       (btCOps(pCur)->Next(pCur, pRes))
#define sqliteBtreePrevious(pCur, pRes)   (btCOps(pCur)->Previous(pCur, pRes))
#define sqliteBtreeKeySize(pCur, pSize)   (btCOps(pCur)->KeySize(pCur, pSize) )
#define sqliteBtreeKey(pCur, offset, amt, zBuf)\
                (btCOps(pCur)->Key(pCur, offset, amt, zBuf))
#define sqliteBtreeKeyCompare(pCur, pKey, nKey, nIgnore, pRes)\
                (btCOps(pCur)->KeyCompare(pCur, pKey, nKey, nIgnore, pRes))
#define sqliteBtreeDataSize(pCur, pSize)  (btCOps(pCur)->DataSize(pCur, pSize))
#define sqliteBtreeData(pCur, offset, amt, zBuf)\
                (btCOps(pCur)->Data(pCur, offset, amt, zBuf))
#define sqliteBtreeCloseCursor(pCur)      (btCOps(pCur)->CloseCursor(pCur))
#define sqliteBtreeGetMeta(pBt, aMeta)    (btOps(pBt)->GetMeta(pBt, aMeta))
#define sqliteBtreeUpdateMeta(pBt, aMeta) (btOps(pBt)->UpdateMeta(pBt, aMeta))
#define sqliteBtreeIntegrityCheck(pBt, aRoot, nRoot)\
                (btOps(pBt)->IntegrityCheck(pBt, aRoot, nRoot))
#define sqliteBtreeGetFilename(pBt)       (btOps(pBt)->GetFilename(pBt))
#define sqliteBtreeCopyFile(pBt1, pBt2)   (btOps(pBt1)->Copyfile(pBt1, pBt2))
#define sqliteBtreePager(pBt)             (btOps(pBt)->Pager(pBt))

#ifdef SQLITE_TEST
#define sqliteBtreePageDump(pBt, pgno, recursive)\
                (btOps(pBt)->PageDump(pBt, pgno, recursive))
#define sqliteBtreeCursorDump(pCur, aResult)\
                (btCOps(pCur)->CursorDump(pCur, aResult))
int btree_native_byte_order;
#endif /* SQLITE_TEST */


#endif /* _BTREE_H_ */
