/*
** 2007 May 1
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
** This file contains code used to implement incremental BLOB I/O.
**
** $Id$
*/

#include "sqliteInt.h"
#include "vdbeInt.h"

#ifndef SQLITE_OMIT_INCRBLOB

/*
** Valid sqlite3_blob* handles point to Incrblob structures.
*/
typedef struct Incrblob Incrblob;
struct Incrblob {
  int flags;              /* Copy of "flags" passed to sqlite3_blob_open() */
  int nByte;              /* Size of open blob, in bytes */
  int iOffset;            /* Byte offset of blob in cursor data */
  BtCursor *pCsr;         /* Cursor pointing at blob row */
  sqlite3_stmt *pStmt;    /* Statement holding cursor open */
  sqlite3 *db;            /* The associated database */
};

/*
** Open a blob handle.
*/
int sqlite3_blob_open(
  sqlite3* db,            /* The database connection */
  const char *zDb,        /* The attached database containing the blob */
  const char *zTable,     /* The table containing the blob */
  const char *zColumn,    /* The column containing the blob */
  sqlite_int64 iRow,      /* The row containing the glob */
  int flags,              /* True -> read/write access, false -> read-only */
  sqlite3_blob **ppBlob   /* Handle for accessing the blob returned here */
){
  int nAttempt = 0;
  int iCol;               /* Index of zColumn in row-record */

  /* This VDBE program seeks a btree cursor to the identified 
  ** db/table/row entry. The reason for using a vdbe program instead
  ** of writing code to use the b-tree layer directly is that the
  ** vdbe program will take advantage of the various transaction,
  ** locking and error handling infrastructure built into the vdbe.
  **
  ** After seeking the cursor, the vdbe executes an OP_ResultRow.
  ** Code external to the Vdbe then "borrows" the b-tree cursor and
  ** uses it to implement the blob_read(), blob_write() and 
  ** blob_bytes() functions.
  **
  ** The sqlite3_blob_close() function finalizes the vdbe program,
  ** which closes the b-tree cursor and (possibly) commits the 
  ** transaction.
  */
  static const VdbeOpList openBlob[] = {
    {OP_Transaction, 0, 0, 0},     /* 0: Start a transaction */
    {OP_VerifyCookie, 0, 0, 0},    /* 1: Check the schema cookie */

    /* One of the following two instructions is replaced by an
    ** OP_Noop before exection.
    */
    {OP_OpenRead, 0, 0, 0},        /* 2: Open cursor 0 for reading */
    {OP_OpenWrite, 0, 0, 0},       /* 3: Open cursor 0 for read/write */
    {OP_SetNumColumns, 0, 0, 0},   /* 4: Num cols for cursor */

    {OP_Variable, 1, 1, 0},        /* 5: Push the rowid to the stack */
    {OP_NotExists, 0, 10, 1},      /* 6: Seek the cursor */
    {OP_Column, 0, 0, 1},          /* 7  */
    {OP_ResultRow, 1, 0, 0},       /* 8  */
    {OP_Close, 0, 0, 0},           /* 9  */
    {OP_Halt, 0, 0, 0},            /* 10 */
  };

  Vdbe *v = 0;
  int rc = SQLITE_OK;
  char zErr[128];

  zErr[0] = 0;
  sqlite3_mutex_enter(db->mutex);
  do {
    Parse sParse;
    Table *pTab;

    memset(&sParse, 0, sizeof(Parse));
    sParse.db = db;

    rc = sqlite3SafetyOn(db);
    if( rc!=SQLITE_OK ){
      sqlite3_mutex_leave(db->mutex);
      return rc;
    }

    sqlite3BtreeEnterAll(db);
    pTab = sqlite3LocateTable(&sParse, 0, zTable, zDb);
    if( !pTab ){
      if( sParse.zErrMsg ){
        sqlite3_snprintf(sizeof(zErr), zErr, "%s", sParse.zErrMsg);
      }
      sqlite3_free(sParse.zErrMsg);
      rc = SQLITE_ERROR;
      (void)sqlite3SafetyOff(db);
      sqlite3BtreeLeaveAll(db);
      goto blob_open_out;
    }

    /* Now search pTab for the exact column. */
    for(iCol=0; iCol < pTab->nCol; iCol++) {
      if( sqlite3StrICmp(pTab->aCol[iCol].zName, zColumn)==0 ){
        break;
      }
    }
    if( iCol==pTab->nCol ){
      sqlite3_snprintf(sizeof(zErr), zErr, "no such column: \"%s\"", zColumn);
      rc = SQLITE_ERROR;
      (void)sqlite3SafetyOff(db);
      sqlite3BtreeLeaveAll(db);
      goto blob_open_out;
    }

    /* If the value is being opened for writing, check that the
    ** column is not indexed. It is against the rules to open an
    ** indexed column for writing.
    */
    if( flags ){
      Index *pIdx;
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        int j;
        for(j=0; j<pIdx->nColumn; j++){
          if( pIdx->aiColumn[j]==iCol ){
            sqlite3_snprintf(sizeof(zErr), zErr,
                             "cannot open indexed column for writing");
            rc = SQLITE_ERROR;
            (void)sqlite3SafetyOff(db);
            sqlite3BtreeLeaveAll(db);
            goto blob_open_out;
          }
        }
      }
    }

    v = sqlite3VdbeCreate(db);
    if( v ){
      int iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
      sqlite3VdbeAddOpList(v, sizeof(openBlob)/sizeof(VdbeOpList), openBlob);

      /* Configure the OP_Transaction */
      sqlite3VdbeChangeP1(v, 0, iDb);
      sqlite3VdbeChangeP2(v, 0, (flags ? 1 : 0));

      /* Configure the OP_VerifyCookie */
      sqlite3VdbeChangeP1(v, 1, iDb);
      sqlite3VdbeChangeP2(v, 1, pTab->pSchema->schema_cookie);

      /* Make sure a mutex is held on the table to be accessed */
      sqlite3VdbeUsesBtree(v, iDb); 

      /* Remove either the OP_OpenWrite or OpenRead. Set the P2 
      ** parameter of the other to pTab->tnum. 
      */
      sqlite3VdbeChangeToNoop(v, (flags ? 2 : 3), 1);
      sqlite3VdbeChangeP2(v, (flags ? 3 : 2), pTab->tnum);
      sqlite3VdbeChangeP3(v, (flags ? 3 : 2), iDb);

      /* Configure the OP_SetNumColumns. Configure the cursor to
      ** think that the table has one more column than it really
      ** does. An OP_Column to retrieve this imaginary column will
      ** always return an SQL NULL. This is useful because it means
      ** we can invoke OP_Column to fill in the vdbe cursors type 
      ** and offset cache without causing any IO.
      */
      sqlite3VdbeChangeP2(v, 4, pTab->nCol+1);
      if( !db->mallocFailed ){
        sqlite3VdbeMakeReady(v, 1, 1, 1, 0);
      }
    }
   
    sqlite3BtreeLeaveAll(db);
    rc = sqlite3SafetyOff(db);
    if( rc!=SQLITE_OK || db->mallocFailed ){
      goto blob_open_out;
    }

    sqlite3_bind_int64((sqlite3_stmt *)v, 1, iRow);
    rc = sqlite3_step((sqlite3_stmt *)v);
    if( rc!=SQLITE_ROW ){
      nAttempt++;
      rc = sqlite3_finalize((sqlite3_stmt *)v);
      sqlite3_snprintf(sizeof(zErr), zErr, sqlite3_errmsg(db));
      v = 0;
    }
  } while( nAttempt<5 && rc==SQLITE_SCHEMA );

  if( rc==SQLITE_ROW ){
    /* The row-record has been opened successfully. Check that the
    ** column in question contains text or a blob. If it contains
    ** text, it is up to the caller to get the encoding right.
    */
    Incrblob *pBlob;
    u32 type = v->apCsr[0]->aType[iCol];

    if( type<12 ){
      sqlite3_snprintf(sizeof(zErr), zErr, "cannot open value of type %s",
          type==0?"null": type==7?"real": "integer"
      );
      rc = SQLITE_ERROR;
      goto blob_open_out;
    }
    pBlob = (Incrblob *)sqlite3DbMallocZero(db, sizeof(Incrblob));
    if( db->mallocFailed ){
      sqlite3_free(pBlob);
      goto blob_open_out;
    }
    pBlob->flags = flags;
    pBlob->pCsr =  v->apCsr[0]->pCursor;
    sqlite3BtreeEnterCursor(pBlob->pCsr);
    sqlite3BtreeCacheOverflow(pBlob->pCsr);
    sqlite3BtreeLeaveCursor(pBlob->pCsr);
    pBlob->pStmt = (sqlite3_stmt *)v;
    pBlob->iOffset = v->apCsr[0]->aOffset[iCol];
    pBlob->nByte = sqlite3VdbeSerialTypeLen(type);
    pBlob->db = db;
    *ppBlob = (sqlite3_blob *)pBlob;
    rc = SQLITE_OK;
  }else if( rc==SQLITE_OK ){
    sqlite3_snprintf(sizeof(zErr), zErr, "no such rowid: %lld", iRow);
    rc = SQLITE_ERROR;
  }

blob_open_out:
  zErr[sizeof(zErr)-1] = '\0';
  if( rc!=SQLITE_OK || db->mallocFailed ){
    sqlite3_finalize((sqlite3_stmt *)v);
  }
  sqlite3Error(db, rc, (rc==SQLITE_OK?0:zErr));
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** Close a blob handle that was previously created using
** sqlite3_blob_open().
*/
int sqlite3_blob_close(sqlite3_blob *pBlob){
  Incrblob *p = (Incrblob *)pBlob;
  int rc;

  rc = sqlite3_finalize(p->pStmt);
  sqlite3_free(p);
  return rc;
}

/*
** Perform a read or write operation on a blob
*/
static int blobReadWrite(
  sqlite3_blob *pBlob, 
  void *z, 
  int n, 
  int iOffset, 
  int (*xCall)(BtCursor*, u32, u32, void*)
){
  int rc;
  Incrblob *p = (Incrblob *)pBlob;
  Vdbe *v;
  sqlite3 *db = p->db;  

  /* Request is out of range. Return a transient error. */
  if( (iOffset+n)>p->nByte ){
    return SQLITE_ERROR;
  }
  sqlite3_mutex_enter(db->mutex);

  /* If there is no statement handle, then the blob-handle has
  ** already been invalidated. Return SQLITE_ABORT in this case.
  */
  v = (Vdbe*)p->pStmt;
  if( v==0 ){
    rc = SQLITE_ABORT;
  }else{
    /* Call either BtreeData() or BtreePutData(). If SQLITE_ABORT is
    ** returned, clean-up the statement handle.
    */
    assert( db == v->db );
    sqlite3BtreeEnterCursor(p->pCsr);
    rc = xCall(p->pCsr, iOffset+p->iOffset, n, z);
    sqlite3BtreeLeaveCursor(p->pCsr);
    if( rc==SQLITE_ABORT ){
      sqlite3VdbeFinalize(v);
      p->pStmt = 0;
    }else{
      db->errCode = rc;
      v->rc = rc;
    }
  }
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** Read data from a blob handle.
*/
int sqlite3_blob_read(sqlite3_blob *pBlob, void *z, int n, int iOffset){
  return blobReadWrite(pBlob, z, n, iOffset, sqlite3BtreeData);
}

/*
** Write data to a blob handle.
*/
int sqlite3_blob_write(sqlite3_blob *pBlob, const void *z, int n, int iOffset){
  return blobReadWrite(pBlob, (void *)z, n, iOffset, sqlite3BtreePutData);
}

/*
** Query a blob handle for the size of the data.
**
** The Incrblob.nByte field is fixed for the lifetime of the Incrblob
** so no mutex is required for access.
*/
int sqlite3_blob_bytes(sqlite3_blob *pBlob){
  Incrblob *p = (Incrblob *)pBlob;
  return p->nByte;
}

#endif /* #ifndef SQLITE_OMIT_INCRBLOB */
