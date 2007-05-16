/*
** 2004 April 6
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** $Id$
**
** This file implements a external (disk-based) database using BTrees.
** For a detailed discussion of BTrees, refer to
**
**     Donald E. Knuth, THE ART OF COMPUTER PROGRAMMING, Volume 3:
**     "Sorting And Searching", pages 473-480. Addison-Wesley
**     Publishing Company, Reading, Massachusetts.
**
** The basic idea is that each page of the file contains N database
** entries and N+1 pointers to subpages.
**
**   ----------------------------------------------------------------
**   |  Ptr(0) | Key(0) | Ptr(1) | Key(1) | ... | Key(N-1) | Ptr(N) |
**   ----------------------------------------------------------------
**
** All of the keys on the page that Ptr(0) points to have values less
** than Key(0).  All of the keys on page Ptr(1) and its subpages have
** values greater than Key(0) and less than Key(1).  All of the keys
** on Ptr(N) and its subpages have values greater than Key(N-1).  And
** so forth.
**
** Finding a particular key requires reading O(log(M)) pages from the 
** disk where M is the number of entries in the tree.
**
** In this implementation, a single file can hold one or more separate 
** BTrees.  Each BTree is identified by the index of its root page.  The
** key and data for any entry are combined to form the "payload".  A
** fixed amount of payload can be carried directly on the database
** page.  If the payload is larger than the preset amount then surplus
** bytes are stored on overflow pages.  The payload for an entry
** and the preceding pointer are combined to form a "Cell".  Each 
** page has a small header which contains the Ptr(N) pointer and other
** information such as the size of key and data.
**
** FORMAT DETAILS
**
** The file is divided into pages.  The first page is called page 1,
** the second is page 2, and so forth.  A page number of zero indicates
** "no such page".  The page size can be anything between 512 and 65536.
** Each page can be either a btree page, a freelist page or an overflow
** page.
**
** The first page is always a btree page.  The first 100 bytes of the first
** page contain a special header (the "file header") that describes the file.
** The format of the file header is as follows:
**
**   OFFSET   SIZE    DESCRIPTION
**      0      16     Header string: "SQLite format 3\000"
**     16       2     Page size in bytes.  
**     18       1     File format write version
**     19       1     File format read version
**     20       1     Bytes of unused space at the end of each page
**     21       1     Max embedded payload fraction
**     22       1     Min embedded payload fraction
**     23       1     Min leaf payload fraction
**     24       4     File change counter
**     28       4     Reserved for future use
**     32       4     First freelist page
**     36       4     Number of freelist pages in the file
**     40      60     15 4-byte meta values passed to higher layers
**
** All of the integer values are big-endian (most significant byte first).
**
** The file change counter is incremented when the database is changed more
** than once within the same second.  This counter, together with the
** modification time of the file, allows other processes to know
** when the file has changed and thus when they need to flush their
** cache.
**
** The max embedded payload fraction is the amount of the total usable
** space in a page that can be consumed by a single cell for standard
** B-tree (non-LEAFDATA) tables.  A value of 255 means 100%.  The default
** is to limit the maximum cell size so that at least 4 cells will fit
** on one page.  Thus the default max embedded payload fraction is 64.
**
** If the payload for a cell is larger than the max payload, then extra
** payload is spilled to overflow pages.  Once an overflow page is allocated,
** as many bytes as possible are moved into the overflow pages without letting
** the cell size drop below the min embedded payload fraction.
**
** The min leaf payload fraction is like the min embedded payload fraction
** except that it applies to leaf nodes in a LEAFDATA tree.  The maximum
** payload fraction for a LEAFDATA tree is always 100% (or 255) and it
** not specified in the header.
**
** Each btree pages is divided into three sections:  The header, the
** cell pointer array, and the cell area area.  Page 1 also has a 100-byte
** file header that occurs before the page header.
**
**      |----------------|
**      | file header    |   100 bytes.  Page 1 only.
**      |----------------|
**      | page header    |   8 bytes for leaves.  12 bytes for interior nodes
**      |----------------|
**      | cell pointer   |   |  2 bytes per cell.  Sorted order.
**      | array          |   |  Grows downward
**      |                |   v
**      |----------------|
**      | unallocated    |
**      | space          |
**      |----------------|   ^  Grows upwards
**      | cell content   |   |  Arbitrary order interspersed with freeblocks.
**      | area           |   |  and free space fragments.
**      |----------------|
**
** The page headers looks like this:
**
**   OFFSET   SIZE     DESCRIPTION
**      0       1      Flags. 1: intkey, 2: zerodata, 4: leafdata, 8: leaf
**      1       2      byte offset to the first freeblock
**      3       2      number of cells on this page
**      5       2      first byte of the cell content area
**      7       1      number of fragmented free bytes
**      8       4      Right child (the Ptr(N) value).  Omitted on leaves.
**
** The flags define the format of this btree page.  The leaf flag means that
** this page has no children.  The zerodata flag means that this page carries
** only keys and no data.  The intkey flag means that the key is a integer
** which is stored in the key size entry of the cell header rather than in
** the payload area.
**
** The cell pointer array begins on the first byte after the page header.
** The cell pointer array contains zero or more 2-byte numbers which are
** offsets from the beginning of the page to the cell content in the cell
** content area.  The cell pointers occur in sorted order.  The system strives
** to keep free space after the last cell pointer so that new cells can
** be easily added without having to defragment the page.
**
** Cell content is stored at the very end of the page and grows toward the
** beginning of the page.
**
** Unused space within the cell content area is collected into a linked list of
** freeblocks.  Each freeblock is at least 4 bytes in size.  The byte offset
** to the first freeblock is given in the header.  Freeblocks occur in
** increasing order.  Because a freeblock must be at least 4 bytes in size,
** any group of 3 or fewer unused bytes in the cell content area cannot
** exist on the freeblock chain.  A group of 3 or fewer free bytes is called
** a fragment.  The total number of bytes in all fragments is recorded.
** in the page header at offset 7.
**
**    SIZE    DESCRIPTION
**      2     Byte offset of the next freeblock
**      2     Bytes in this freeblock
**
** Cells are of variable length.  Cells are stored in the cell content area at
** the end of the page.  Pointers to the cells are in the cell pointer array
** that immediately follows the page header.  Cells is not necessarily
** contiguous or in order, but cell pointers are contiguous and in order.
**
** Cell content makes use of variable length integers.  A variable
** length integer is 1 to 9 bytes where the lower 7 bits of each 
** byte are used.  The integer consists of all bytes that have bit 8 set and
** the first byte with bit 8 clear.  The most significant byte of the integer
** appears first.  A variable-length integer may not be more than 9 bytes long.
** As a special case, all 8 bytes of the 9th byte are used as data.  This
** allows a 64-bit integer to be encoded in 9 bytes.
**
**    0x00                      becomes  0x00000000
**    0x7f                      becomes  0x0000007f
**    0x81 0x00                 becomes  0x00000080
**    0x82 0x00                 becomes  0x00000100
**    0x80 0x7f                 becomes  0x0000007f
**    0x8a 0x91 0xd1 0xac 0x78  becomes  0x12345678
**    0x81 0x81 0x81 0x81 0x01  becomes  0x10204081
**
** Variable length integers are used for rowids and to hold the number of
** bytes of key and data in a btree cell.
**
** The content of a cell looks like this:
**
**    SIZE    DESCRIPTION
**      4     Page number of the left child. Omitted if leaf flag is set.
**     var    Number of bytes of data. Omitted if the zerodata flag is set.
**     var    Number of bytes of key. Or the key itself if intkey flag is set.
**      *     Payload
**      4     First page of the overflow chain.  Omitted if no overflow
**
** Overflow pages form a linked list.  Each page except the last is completely
** filled with data (pagesize - 4 bytes).  The last page can have as little
** as 1 byte of data.
**
**    SIZE    DESCRIPTION
**      4     Page number of next overflow page
**      *     Data
**
** Freelist pages come in two subtypes: trunk pages and leaf pages.  The
** file header points to first in a linked list of trunk page.  Each trunk
** page points to multiple leaf pages.  The content of a leaf page is
** unspecified.  A trunk page looks like this:
**
**    SIZE    DESCRIPTION
**      4     Page number of next trunk page
**      4     Number of leaf pointers on this page
**      *     zero or more pages numbers of leaves
*/
#include "sqliteInt.h"
#include "pager.h"
#include "btree.h"
#include "os.h"
#include <assert.h>

/* Round up a number to the next larger multiple of 8.  This is used
** to force 8-byte alignment on 64-bit architectures.
*/
#define ROUND8(x)   ((x+7)&~7)


/* The following value is the maximum cell size assuming a maximum page
** size give above.
*/
#define MX_CELL_SIZE(pBt)  (pBt->pageSize-8)

/* The maximum number of cells on a single page of the database.  This
** assumes a minimum cell size of 3 bytes.  Such small cells will be
** exceedingly rare, but they are possible.
*/
#define MX_CELL(pBt) ((pBt->pageSize-8)/3)

/* Forward declarations */
typedef struct MemPage MemPage;
typedef struct BtLock BtLock;

/*
** This is a magic string that appears at the beginning of every
** SQLite database in order to identify the file as a real database.
**
** You can change this value at compile-time by specifying a
** -DSQLITE_FILE_HEADER="..." on the compiler command-line.  The
** header must be exactly 16 bytes including the zero-terminator so
** the string itself should be 15 characters long.  If you change
** the header, then your custom library will not be able to read 
** databases generated by the standard tools and the standard tools
** will not be able to read databases created by your custom library.
*/
#ifndef SQLITE_FILE_HEADER /* 123456789 123456 */
#  define SQLITE_FILE_HEADER "SQLite format 3"
#endif
static const char zMagicHeader[] = SQLITE_FILE_HEADER;

/*
** Page type flags.  An ORed combination of these flags appear as the
** first byte of every BTree page.
*/
#define PTF_INTKEY    0x01
#define PTF_ZERODATA  0x02
#define PTF_LEAFDATA  0x04
#define PTF_LEAF      0x08

/*
** As each page of the file is loaded into memory, an instance of the following
** structure is appended and initialized to zero.  This structure stores
** information about the page that is decoded from the raw file page.
**
** The pParent field points back to the parent page.  This allows us to
** walk up the BTree from any leaf to the root.  Care must be taken to
** unref() the parent page pointer when this page is no longer referenced.
** The pageDestructor() routine handles that chore.
*/
struct MemPage {
  u8 isInit;           /* True if previously initialized. MUST BE FIRST! */
  u8 idxShift;         /* True if Cell indices have changed */
  u8 nOverflow;        /* Number of overflow cell bodies in aCell[] */
  u8 intKey;           /* True if intkey flag is set */
  u8 leaf;             /* True if leaf flag is set */
  u8 zeroData;         /* True if table stores keys only */
  u8 leafData;         /* True if tables stores data on leaves only */
  u8 hasData;          /* True if this page stores data */
  u8 hdrOffset;        /* 100 for page 1.  0 otherwise */
  u8 childPtrSize;     /* 0 if leaf==1.  4 if leaf==0 */
  u16 maxLocal;        /* Copy of Btree.maxLocal or Btree.maxLeaf */
  u16 minLocal;        /* Copy of Btree.minLocal or Btree.minLeaf */
  u16 cellOffset;      /* Index in aData of first cell pointer */
  u16 idxParent;       /* Index in parent of this node */
  u16 nFree;           /* Number of free bytes on the page */
  u16 nCell;           /* Number of cells on this page, local and ovfl */
  struct _OvflCell {   /* Cells that will not fit on aData[] */
    u8 *pCell;          /* Pointers to the body of the overflow cell */
    u16 idx;            /* Insert this cell before idx-th non-overflow cell */
  } aOvfl[5];
  BtShared *pBt;       /* Pointer back to BTree structure */
  u8 *aData;           /* Pointer back to the start of the page */
  DbPage *pDbPage;     /* Pager page handle */
  Pgno pgno;           /* Page number for this page */
  MemPage *pParent;    /* The parent of this page.  NULL for root */
};

/*
** The in-memory image of a disk page has the auxiliary information appended
** to the end.  EXTRA_SIZE is the number of bytes of space needed to hold
** that extra information.
*/
#define EXTRA_SIZE sizeof(MemPage)

/* Btree handle */
struct Btree {
  sqlite3 *pSqlite;
  BtShared *pBt;
  u8 inTrans;            /* TRANS_NONE, TRANS_READ or TRANS_WRITE */
};

/*
** Btree.inTrans may take one of the following values.
**
** If the shared-data extension is enabled, there may be multiple users
** of the Btree structure. At most one of these may open a write transaction,
** but any number may have active read transactions. Variable Btree.pDb 
** points to the handle that owns any current write-transaction.
*/
#define TRANS_NONE  0
#define TRANS_READ  1
#define TRANS_WRITE 2

/*
** Everything we need to know about an open database
*/
struct BtShared {
  Pager *pPager;        /* The page cache */
  BtCursor *pCursor;    /* A list of all open cursors */
  MemPage *pPage1;      /* First page of the database */
  u8 inStmt;            /* True if we are in a statement subtransaction */
  u8 readOnly;          /* True if the underlying file is readonly */
  u8 maxEmbedFrac;      /* Maximum payload as % of total page size */
  u8 minEmbedFrac;      /* Minimum payload as % of total page size */
  u8 minLeafFrac;       /* Minimum leaf payload as % of total page size */
  u8 pageSizeFixed;     /* True if the page size can no longer be changed */
#ifndef SQLITE_OMIT_AUTOVACUUM
  u8 autoVacuum;        /* True if database supports auto-vacuum */
#endif
  u16 pageSize;         /* Total number of bytes on a page */
  u16 usableSize;       /* Number of usable bytes on each page */
  int maxLocal;         /* Maximum local payload in non-LEAFDATA tables */
  int minLocal;         /* Minimum local payload in non-LEAFDATA tables */
  int maxLeaf;          /* Maximum local payload in a LEAFDATA table */
  int minLeaf;          /* Minimum local payload in a LEAFDATA table */
  BusyHandler *pBusyHandler;   /* Callback for when there is lock contention */
  u8 inTransaction;     /* Transaction state */
  int nRef;             /* Number of references to this structure */
  int nTransaction;     /* Number of open transactions (read + write) */
  void *pSchema;        /* Pointer to space allocated by sqlite3BtreeSchema() */
  void (*xFreeSchema)(void*);  /* Destructor for BtShared.pSchema */
#ifndef SQLITE_OMIT_SHARED_CACHE
  BtLock *pLock;        /* List of locks held on this shared-btree struct */
  BtShared *pNext;      /* Next in ThreadData.pBtree linked list */
#endif
};

/*
** An instance of the following structure is used to hold information
** about a cell.  The parseCellPtr() function fills in this structure
** based on information extract from the raw disk page.
*/
typedef struct CellInfo CellInfo;
struct CellInfo {
  u8 *pCell;     /* Pointer to the start of cell content */
  i64 nKey;      /* The key for INTKEY tables, or number of bytes in key */
  u32 nData;     /* Number of bytes of data */
  u32 nPayload;  /* Total amount of payload */
  u16 nHeader;   /* Size of the cell content header in bytes */
  u16 nLocal;    /* Amount of payload held locally */
  u16 iOverflow; /* Offset to overflow page number.  Zero if no overflow */
  u16 nSize;     /* Size of the cell content on the main b-tree page */
};

/*
** A cursor is a pointer to a particular entry in the BTree.
** The entry is identified by its MemPage and the index in
** MemPage.aCell[] of the entry.
*/
struct BtCursor {
  Btree *pBtree;            /* The Btree to which this cursor belongs */
  BtCursor *pNext, *pPrev;  /* Forms a linked list of all cursors */
  int (*xCompare)(void*,int,const void*,int,const void*); /* Key comp func */
  void *pArg;               /* First arg to xCompare() */
  Pgno pgnoRoot;            /* The root page of this tree */
  MemPage *pPage;           /* Page that contains the entry */
  int idx;                  /* Index of the entry in pPage->aCell[] */
  CellInfo info;            /* A parse of the cell we are pointing at */
  u8 wrFlag;                /* True if writable */
  u8 eState;                /* One of the CURSOR_XXX constants (see below) */
  void *pKey;      /* Saved key that was cursor's last known position */
  i64 nKey;        /* Size of pKey, or last integer key */
  int skip;        /* (skip<0) -> Prev() is a no-op. (skip>0) -> Next() is */
};

/*
** Potential values for BtCursor.eState.
**
** CURSOR_VALID:
**   Cursor points to a valid entry. getPayload() etc. may be called.
**
** CURSOR_INVALID:
**   Cursor does not point to a valid entry. This can happen (for example) 
**   because the table is empty or because BtreeCursorFirst() has not been
**   called.
**
** CURSOR_REQUIRESEEK:
**   The table that this cursor was opened on still exists, but has been 
**   modified since the cursor was last used. The cursor position is saved
**   in variables BtCursor.pKey and BtCursor.nKey. When a cursor is in 
**   this state, restoreOrClearCursorPosition() can be called to attempt to
**   seek the cursor to the saved position.
*/
#define CURSOR_INVALID           0
#define CURSOR_VALID             1
#define CURSOR_REQUIRESEEK       2

/*
** The TRACE macro will print high-level status information about the
** btree operation when the global variable sqlite3_btree_trace is
** enabled.
*/
#if SQLITE_TEST
# define TRACE(X)   if( sqlite3_btree_trace )\
/*                        { sqlite3DebugPrintf X; fflush(stdout); } */ \
{ printf X; fflush(stdout); }
int sqlite3_btree_trace=0;  /* True to enable tracing */
#else
# define TRACE(X)
#endif

/*
** Forward declaration
*/
static int checkReadLocks(Btree*,Pgno,BtCursor*);

/*
** Read or write a two- and four-byte big-endian integer values.
*/
static u32 get2byte(unsigned char *p){
  return (p[0]<<8) | p[1];
}
static u32 get4byte(unsigned char *p){
  return (p[0]<<24) | (p[1]<<16) | (p[2]<<8) | p[3];
}
static void put2byte(unsigned char *p, u32 v){
  p[0] = v>>8;
  p[1] = v;
}
static void put4byte(unsigned char *p, u32 v){
  p[0] = v>>24;
  p[1] = v>>16;
  p[2] = v>>8;
  p[3] = v;
}

/*
** Routines to read and write variable-length integers.  These used to
** be defined locally, but now we use the varint routines in the util.c
** file.
*/
#define getVarint    sqlite3GetVarint
/* #define getVarint32  sqlite3GetVarint32 */
#define getVarint32(A,B)  ((*B=*(A))<=0x7f?1:sqlite3GetVarint32(A,B))
#define putVarint    sqlite3PutVarint

/* The database page the PENDING_BYTE occupies. This page is never used.
** TODO: This macro is very similary to PAGER_MJ_PGNO() in pager.c. They
** should possibly be consolidated (presumably in pager.h).
**
** If disk I/O is omitted (meaning that the database is stored purely
** in memory) then there is no pending byte.
*/
#ifdef SQLITE_OMIT_DISKIO
# define PENDING_BYTE_PAGE(pBt)  0x7fffffff
#else
# define PENDING_BYTE_PAGE(pBt) ((PENDING_BYTE/(pBt)->pageSize)+1)
#endif

/*
** A linked list of the following structures is stored at BtShared.pLock.
** Locks are added (or upgraded from READ_LOCK to WRITE_LOCK) when a cursor 
** is opened on the table with root page BtShared.iTable. Locks are removed
** from this list when a transaction is committed or rolled back, or when
** a btree handle is closed.
*/
struct BtLock {
  Btree *pBtree;        /* Btree handle holding this lock */
  Pgno iTable;          /* Root page of table */
  u8 eLock;             /* READ_LOCK or WRITE_LOCK */
  BtLock *pNext;        /* Next in BtShared.pLock list */
};

/* Candidate values for BtLock.eLock */
#define READ_LOCK     1
#define WRITE_LOCK    2

#ifdef SQLITE_OMIT_SHARED_CACHE
  /*
  ** The functions queryTableLock(), lockTable() and unlockAllTables()
  ** manipulate entries in the BtShared.pLock linked list used to store
  ** shared-cache table level locks. If the library is compiled with the
  ** shared-cache feature disabled, then there is only ever one user
  ** of each BtShared structure and so this locking is not necessary. 
  ** So define the lock related functions as no-ops.
  */
  #define queryTableLock(a,b,c) SQLITE_OK
  #define lockTable(a,b,c) SQLITE_OK
  #define unlockAllTables(a)
#else


/*
** Query to see if btree handle p may obtain a lock of type eLock 
** (READ_LOCK or WRITE_LOCK) on the table with root-page iTab. Return
** SQLITE_OK if the lock may be obtained (by calling lockTable()), or
** SQLITE_LOCKED if not.
*/
static int queryTableLock(Btree *p, Pgno iTab, u8 eLock){
  BtShared *pBt = p->pBt;
  BtLock *pIter;

  /* This is a no-op if the shared-cache is not enabled */
  if( 0==sqlite3ThreadDataReadOnly()->useSharedData ){
    return SQLITE_OK;
  }

  /* This (along with lockTable()) is where the ReadUncommitted flag is
  ** dealt with. If the caller is querying for a read-lock and the flag is
  ** set, it is unconditionally granted - even if there are write-locks
  ** on the table. If a write-lock is requested, the ReadUncommitted flag
  ** is not considered.
  **
  ** In function lockTable(), if a read-lock is demanded and the 
  ** ReadUncommitted flag is set, no entry is added to the locks list 
  ** (BtShared.pLock).
  **
  ** To summarize: If the ReadUncommitted flag is set, then read cursors do
  ** not create or respect table locks. The locking procedure for a 
  ** write-cursor does not change.
  */
  if( 
    !p->pSqlite || 
    0==(p->pSqlite->flags&SQLITE_ReadUncommitted) || 
    eLock==WRITE_LOCK ||
    iTab==MASTER_ROOT
  ){
    for(pIter=pBt->pLock; pIter; pIter=pIter->pNext){
      if( pIter->pBtree!=p && pIter->iTable==iTab && 
          (pIter->eLock!=eLock || eLock!=READ_LOCK) ){
        return SQLITE_LOCKED;
      }
    }
  }
  return SQLITE_OK;
}

/*
** Add a lock on the table with root-page iTable to the shared-btree used
** by Btree handle p. Parameter eLock must be either READ_LOCK or 
** WRITE_LOCK.
**
** SQLITE_OK is returned if the lock is added successfully. SQLITE_BUSY and
** SQLITE_NOMEM may also be returned.
*/
static int lockTable(Btree *p, Pgno iTable, u8 eLock){
  BtShared *pBt = p->pBt;
  BtLock *pLock = 0;
  BtLock *pIter;

  /* This is a no-op if the shared-cache is not enabled */
  if( 0==sqlite3ThreadDataReadOnly()->useSharedData ){
    return SQLITE_OK;
  }

  assert( SQLITE_OK==queryTableLock(p, iTable, eLock) );

  /* If the read-uncommitted flag is set and a read-lock is requested,
  ** return early without adding an entry to the BtShared.pLock list. See
  ** comment in function queryTableLock() for more info on handling 
  ** the ReadUncommitted flag.
  */
  if( 
    (p->pSqlite) && 
    (p->pSqlite->flags&SQLITE_ReadUncommitted) && 
    (eLock==READ_LOCK) &&
    iTable!=MASTER_ROOT
  ){
    return SQLITE_OK;
  }

  /* First search the list for an existing lock on this table. */
  for(pIter=pBt->pLock; pIter; pIter=pIter->pNext){
    if( pIter->iTable==iTable && pIter->pBtree==p ){
      pLock = pIter;
      break;
    }
  }

  /* If the above search did not find a BtLock struct associating Btree p
  ** with table iTable, allocate one and link it into the list.
  */
  if( !pLock ){
    pLock = (BtLock *)sqliteMalloc(sizeof(BtLock));
    if( !pLock ){
      return SQLITE_NOMEM;
    }
    pLock->iTable = iTable;
    pLock->pBtree = p;
    pLock->pNext = pBt->pLock;
    pBt->pLock = pLock;
  }

  /* Set the BtLock.eLock variable to the maximum of the current lock
  ** and the requested lock. This means if a write-lock was already held
  ** and a read-lock requested, we don't incorrectly downgrade the lock.
  */
  assert( WRITE_LOCK>READ_LOCK );
  if( eLock>pLock->eLock ){
    pLock->eLock = eLock;
  }

  return SQLITE_OK;
}

/*
** Release all the table locks (locks obtained via calls to the lockTable()
** procedure) held by Btree handle p.
*/
static void unlockAllTables(Btree *p){
  BtLock **ppIter = &p->pBt->pLock;

  /* If the shared-cache extension is not enabled, there should be no
  ** locks in the BtShared.pLock list, making this procedure a no-op. Assert
  ** that this is the case.
  */
  assert( sqlite3ThreadDataReadOnly()->useSharedData || 0==*ppIter );

  while( *ppIter ){
    BtLock *pLock = *ppIter;
    if( pLock->pBtree==p ){
      *ppIter = pLock->pNext;
      sqliteFree(pLock);
    }else{
      ppIter = &pLock->pNext;
    }
  }
}
#endif /* SQLITE_OMIT_SHARED_CACHE */

static void releasePage(MemPage *pPage);  /* Forward reference */

/*
** Save the current cursor position in the variables BtCursor.nKey 
** and BtCursor.pKey. The cursor's state is set to CURSOR_REQUIRESEEK.
*/
static int saveCursorPosition(BtCursor *pCur){
  int rc;

  assert( CURSOR_VALID==pCur->eState );
  assert( 0==pCur->pKey );

  rc = sqlite3BtreeKeySize(pCur, &pCur->nKey);

  /* If this is an intKey table, then the above call to BtreeKeySize()
  ** stores the integer key in pCur->nKey. In this case this value is
  ** all that is required. Otherwise, if pCur is not open on an intKey
  ** table, then malloc space for and store the pCur->nKey bytes of key 
  ** data.
  */
  if( rc==SQLITE_OK && 0==pCur->pPage->intKey){
    void *pKey = sqliteMalloc(pCur->nKey);
    if( pKey ){
      rc = sqlite3BtreeKey(pCur, 0, pCur->nKey, pKey);
      if( rc==SQLITE_OK ){
        pCur->pKey = pKey;
      }else{
        sqliteFree(pKey);
      }
    }else{
      rc = SQLITE_NOMEM;
    }
  }
  assert( !pCur->pPage->intKey || !pCur->pKey );

  if( rc==SQLITE_OK ){
    releasePage(pCur->pPage);
    pCur->pPage = 0;
    pCur->eState = CURSOR_REQUIRESEEK;
  }

  return rc;
}

/*
** Save the positions of all cursors except pExcept open on the table 
** with root-page iRoot. Usually, this is called just before cursor
** pExcept is used to modify the table (BtreeDelete() or BtreeInsert()).
*/
static int saveAllCursors(BtShared *pBt, Pgno iRoot, BtCursor *pExcept){
  BtCursor *p;
  for(p=pBt->pCursor; p; p=p->pNext){
    if( p!=pExcept && (0==iRoot || p->pgnoRoot==iRoot) && 
        p->eState==CURSOR_VALID ){
      int rc = saveCursorPosition(p);
      if( SQLITE_OK!=rc ){
        return rc;
      }
    }
  }
  return SQLITE_OK;
}

/*
** Clear the current cursor position.
*/
static void clearCursorPosition(BtCursor *pCur){
  sqliteFree(pCur->pKey);
  pCur->pKey = 0;
  pCur->eState = CURSOR_INVALID;
}

/*
** Restore the cursor to the position it was in (or as close to as possible)
** when saveCursorPosition() was called. Note that this call deletes the 
** saved position info stored by saveCursorPosition(), so there can be
** at most one effective restoreOrClearCursorPosition() call after each 
** saveCursorPosition().
**
** If the second argument argument - doSeek - is false, then instead of 
** returning the cursor to it's saved position, any saved position is deleted
** and the cursor state set to CURSOR_INVALID.
*/
static int restoreOrClearCursorPositionX(BtCursor *pCur){
  int rc;
  assert( pCur->eState==CURSOR_REQUIRESEEK );
  pCur->eState = CURSOR_INVALID;
  rc = sqlite3BtreeMoveto(pCur, pCur->pKey, pCur->nKey, 0, &pCur->skip);
  if( rc==SQLITE_OK ){
    sqliteFree(pCur->pKey);
    pCur->pKey = 0;
    assert( pCur->eState==CURSOR_VALID || pCur->eState==CURSOR_INVALID );
  }
  return rc;
}

#define restoreOrClearCursorPosition(p) \
  (p->eState==CURSOR_REQUIRESEEK?restoreOrClearCursorPositionX(p):SQLITE_OK)

#ifndef SQLITE_OMIT_AUTOVACUUM
/*
** These macros define the location of the pointer-map entry for a 
** database page. The first argument to each is the number of usable
** bytes on each page of the database (often 1024). The second is the
** page number to look up in the pointer map.
**
** PTRMAP_PAGENO returns the database page number of the pointer-map
** page that stores the required pointer. PTRMAP_PTROFFSET returns
** the offset of the requested map entry.
**
** If the pgno argument passed to PTRMAP_PAGENO is a pointer-map page,
** then pgno is returned. So (pgno==PTRMAP_PAGENO(pgsz, pgno)) can be
** used to test if pgno is a pointer-map page. PTRMAP_ISPAGE implements
** this test.
*/
#define PTRMAP_PAGENO(pBt, pgno) ptrmapPageno(pBt, pgno)
#define PTRMAP_PTROFFSET(pBt, pgno) (5*(pgno-ptrmapPageno(pBt, pgno)-1))
#define PTRMAP_ISPAGE(pBt, pgno) (PTRMAP_PAGENO((pBt),(pgno))==(pgno))

static Pgno ptrmapPageno(BtShared *pBt, Pgno pgno){
  int nPagesPerMapPage = (pBt->usableSize/5)+1;
  int iPtrMap = (pgno-2)/nPagesPerMapPage;
  int ret = (iPtrMap*nPagesPerMapPage) + 2; 
  if( ret==PENDING_BYTE_PAGE(pBt) ){
    ret++;
  }
  return ret;
}

/*
** The pointer map is a lookup table that identifies the parent page for
** each child page in the database file.  The parent page is the page that
** contains a pointer to the child.  Every page in the database contains
** 0 or 1 parent pages.  (In this context 'database page' refers
** to any page that is not part of the pointer map itself.)  Each pointer map
** entry consists of a single byte 'type' and a 4 byte parent page number.
** The PTRMAP_XXX identifiers below are the valid types.
**
** The purpose of the pointer map is to facility moving pages from one
** position in the file to another as part of autovacuum.  When a page
** is moved, the pointer in its parent must be updated to point to the
** new location.  The pointer map is used to locate the parent page quickly.
**
** PTRMAP_ROOTPAGE: The database page is a root-page. The page-number is not
**                  used in this case.
**
** PTRMAP_FREEPAGE: The database page is an unused (free) page. The page-number 
**                  is not used in this case.
**
** PTRMAP_OVERFLOW1: The database page is the first page in a list of 
**                   overflow pages. The page number identifies the page that
**                   contains the cell with a pointer to this overflow page.
**
** PTRMAP_OVERFLOW2: The database page is the second or later page in a list of
**                   overflow pages. The page-number identifies the previous
**                   page in the overflow page list.
**
** PTRMAP_BTREE: The database page is a non-root btree page. The page number
**               identifies the parent page in the btree.
*/
#define PTRMAP_ROOTPAGE 1
#define PTRMAP_FREEPAGE 2
#define PTRMAP_OVERFLOW1 3
#define PTRMAP_OVERFLOW2 4
#define PTRMAP_BTREE 5

/*
** Write an entry into the pointer map.
**
** This routine updates the pointer map entry for page number 'key'
** so that it maps to type 'eType' and parent page number 'pgno'.
** An error code is returned if something goes wrong, otherwise SQLITE_OK.
*/
static int ptrmapPut(BtShared *pBt, Pgno key, u8 eType, Pgno parent){
  DbPage *pDbPage;  /* The pointer map page */
  u8 *pPtrmap;      /* The pointer map data */
  Pgno iPtrmap;     /* The pointer map page number */
  int offset;       /* Offset in pointer map page */
  int rc;

  /* The master-journal page number must never be used as a pointer map page */
  assert( 0==PTRMAP_ISPAGE(pBt, PENDING_BYTE_PAGE(pBt)) );

  assert( pBt->autoVacuum );
  if( key==0 ){
    return SQLITE_CORRUPT_BKPT;
  }
  iPtrmap = PTRMAP_PAGENO(pBt, key);
  rc = sqlite3PagerGet(pBt->pPager, iPtrmap, &pDbPage);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  offset = PTRMAP_PTROFFSET(pBt, key);
  pPtrmap = (u8 *)sqlite3PagerGetData(pDbPage);

  if( eType!=pPtrmap[offset] || get4byte(&pPtrmap[offset+1])!=parent ){
    TRACE(("PTRMAP_UPDATE: %d->(%d,%d)\n", key, eType, parent));
    rc = sqlite3PagerWrite(pDbPage);
    if( rc==SQLITE_OK ){
      pPtrmap[offset] = eType;
      put4byte(&pPtrmap[offset+1], parent);
    }
  }

  sqlite3PagerUnref(pDbPage);
  return rc;
}

/*
** Read an entry from the pointer map.
**
** This routine retrieves the pointer map entry for page 'key', writing
** the type and parent page number to *pEType and *pPgno respectively.
** An error code is returned if something goes wrong, otherwise SQLITE_OK.
*/
static int ptrmapGet(BtShared *pBt, Pgno key, u8 *pEType, Pgno *pPgno){
  DbPage *pDbPage;   /* The pointer map page */
  int iPtrmap;       /* Pointer map page index */
  u8 *pPtrmap;       /* Pointer map page data */
  int offset;        /* Offset of entry in pointer map */
  int rc;

  iPtrmap = PTRMAP_PAGENO(pBt, key);
  rc = sqlite3PagerGet(pBt->pPager, iPtrmap, &pDbPage);
  if( rc!=0 ){
    return rc;
  }
  pPtrmap = (u8 *)sqlite3PagerGetData(pDbPage);

  offset = PTRMAP_PTROFFSET(pBt, key);
  assert( pEType!=0 );
  *pEType = pPtrmap[offset];
  if( pPgno ) *pPgno = get4byte(&pPtrmap[offset+1]);

  sqlite3PagerUnref(pDbPage);
  if( *pEType<1 || *pEType>5 ) return SQLITE_CORRUPT_BKPT;
  return SQLITE_OK;
}

#endif /* SQLITE_OMIT_AUTOVACUUM */

/*
** Given a btree page and a cell index (0 means the first cell on
** the page, 1 means the second cell, and so forth) return a pointer
** to the cell content.
**
** This routine works only for pages that do not contain overflow cells.
*/
static u8 *findCell(MemPage *pPage, int iCell){
  u8 *data = pPage->aData;
  assert( iCell>=0 );
  assert( iCell<get2byte(&data[pPage->hdrOffset+3]) );
  return data + get2byte(&data[pPage->cellOffset+2*iCell]);
}

/*
** This a more complex version of findCell() that works for
** pages that do contain overflow cells.  See insert
*/
static u8 *findOverflowCell(MemPage *pPage, int iCell){
  int i;
  for(i=pPage->nOverflow-1; i>=0; i--){
    int k;
    struct _OvflCell *pOvfl;
    pOvfl = &pPage->aOvfl[i];
    k = pOvfl->idx;
    if( k<=iCell ){
      if( k==iCell ){
        return pOvfl->pCell;
      }
      iCell--;
    }
  }
  return findCell(pPage, iCell);
}

/*
** Parse a cell content block and fill in the CellInfo structure.  There
** are two versions of this function.  parseCell() takes a cell index
** as the second argument and parseCellPtr() takes a pointer to the
** body of the cell as its second argument.
*/
static void parseCellPtr(
  MemPage *pPage,         /* Page containing the cell */
  u8 *pCell,              /* Pointer to the cell text. */
  CellInfo *pInfo         /* Fill in this structure */
){
  int n;                  /* Number bytes in cell content header */
  u32 nPayload;           /* Number of bytes of cell payload */

  pInfo->pCell = pCell;
  assert( pPage->leaf==0 || pPage->leaf==1 );
  n = pPage->childPtrSize;
  assert( n==4-4*pPage->leaf );
  if( pPage->hasData ){
    n += getVarint32(&pCell[n], &nPayload);
  }else{
    nPayload = 0;
  }
  pInfo->nData = nPayload;
  if( pPage->intKey ){
    n += getVarint(&pCell[n], (u64 *)&pInfo->nKey);
  }else{
    u32 x;
    n += getVarint32(&pCell[n], &x);
    pInfo->nKey = x;
    nPayload += x;
  }
  pInfo->nPayload = nPayload;
  pInfo->nHeader = n;
  if( nPayload<=pPage->maxLocal ){
    /* This is the (easy) common case where the entire payload fits
    ** on the local page.  No overflow is required.
    */
    int nSize;          /* Total size of cell content in bytes */
    pInfo->nLocal = nPayload;
    pInfo->iOverflow = 0;
    nSize = nPayload + n;
    if( nSize<4 ){
      nSize = 4;        /* Minimum cell size is 4 */
    }
    pInfo->nSize = nSize;
  }else{
    /* If the payload will not fit completely on the local page, we have
    ** to decide how much to store locally and how much to spill onto
    ** overflow pages.  The strategy is to minimize the amount of unused
    ** space on overflow pages while keeping the amount of local storage
    ** in between minLocal and maxLocal.
    **
    ** Warning:  changing the way overflow payload is distributed in any
    ** way will result in an incompatible file format.
    */
    int minLocal;  /* Minimum amount of payload held locally */
    int maxLocal;  /* Maximum amount of payload held locally */
    int surplus;   /* Overflow payload available for local storage */

    minLocal = pPage->minLocal;
    maxLocal = pPage->maxLocal;
    surplus = minLocal + (nPayload - minLocal)%(pPage->pBt->usableSize - 4);
    if( surplus <= maxLocal ){
      pInfo->nLocal = surplus;
    }else{
      pInfo->nLocal = minLocal;
    }
    pInfo->iOverflow = pInfo->nLocal + n;
    pInfo->nSize = pInfo->iOverflow + 4;
  }
}
static void parseCell(
  MemPage *pPage,         /* Page containing the cell */
  int iCell,              /* The cell index.  First cell is 0 */
  CellInfo *pInfo         /* Fill in this structure */
){
  parseCellPtr(pPage, findCell(pPage, iCell), pInfo);
}

/*
** Compute the total number of bytes that a Cell needs in the cell
** data area of the btree-page.  The return number includes the cell
** data header and the local payload, but not any overflow page or
** the space used by the cell pointer.
*/
#ifndef NDEBUG
static int cellSize(MemPage *pPage, int iCell){
  CellInfo info;
  parseCell(pPage, iCell, &info);
  return info.nSize;
}
#endif
static int cellSizePtr(MemPage *pPage, u8 *pCell){
  CellInfo info;
  parseCellPtr(pPage, pCell, &info);
  return info.nSize;
}

#ifndef SQLITE_OMIT_AUTOVACUUM
/*
** If the cell pCell, part of page pPage contains a pointer
** to an overflow page, insert an entry into the pointer-map
** for the overflow page.
*/
static int ptrmapPutOvflPtr(MemPage *pPage, u8 *pCell){
  if( pCell ){
    CellInfo info;
    parseCellPtr(pPage, pCell, &info);
    assert( (info.nData+(pPage->intKey?0:info.nKey))==info.nPayload );
    if( (info.nData+(pPage->intKey?0:info.nKey))>info.nLocal ){
      Pgno ovfl = get4byte(&pCell[info.iOverflow]);
      return ptrmapPut(pPage->pBt, ovfl, PTRMAP_OVERFLOW1, pPage->pgno);
    }
  }
  return SQLITE_OK;
}
/*
** If the cell with index iCell on page pPage contains a pointer
** to an overflow page, insert an entry into the pointer-map
** for the overflow page.
*/
static int ptrmapPutOvfl(MemPage *pPage, int iCell){
  u8 *pCell;
  pCell = findOverflowCell(pPage, iCell);
  return ptrmapPutOvflPtr(pPage, pCell);
}
#endif


/* A bunch of assert() statements to check the transaction state variables
** of handle p (type Btree*) are internally consistent.
*/
#define btreeIntegrity(p) \
  assert( p->inTrans!=TRANS_NONE || p->pBt->nTransaction<p->pBt->nRef ); \
  assert( p->pBt->nTransaction<=p->pBt->nRef ); \
  assert( p->pBt->inTransaction!=TRANS_NONE || p->pBt->nTransaction==0 ); \
  assert( p->pBt->inTransaction>=p->inTrans ); 

/*
** Defragment the page given.  All Cells are moved to the
** end of the page and all free space is collected into one
** big FreeBlk that occurs in between the header and cell
** pointer array and the cell content area.
*/
static int defragmentPage(MemPage *pPage){
  int i;                     /* Loop counter */
  int pc;                    /* Address of a i-th cell */
  int addr;                  /* Offset of first byte after cell pointer array */
  int hdr;                   /* Offset to the page header */
  int size;                  /* Size of a cell */
  int usableSize;            /* Number of usable bytes on a page */
  int cellOffset;            /* Offset to the cell pointer array */
  int brk;                   /* Offset to the cell content area */
  int nCell;                 /* Number of cells on the page */
  unsigned char *data;       /* The page data */
  unsigned char *temp;       /* Temp area for cell content */

  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  assert( pPage->pBt!=0 );
  assert( pPage->pBt->usableSize <= SQLITE_MAX_PAGE_SIZE );
  assert( pPage->nOverflow==0 );
  temp = sqliteMalloc( pPage->pBt->pageSize );
  if( temp==0 ) return SQLITE_NOMEM;
  data = pPage->aData;
  hdr = pPage->hdrOffset;
  cellOffset = pPage->cellOffset;
  nCell = pPage->nCell;
  assert( nCell==get2byte(&data[hdr+3]) );
  usableSize = pPage->pBt->usableSize;
  brk = get2byte(&data[hdr+5]);
  memcpy(&temp[brk], &data[brk], usableSize - brk);
  brk = usableSize;
  for(i=0; i<nCell; i++){
    u8 *pAddr;     /* The i-th cell pointer */
    pAddr = &data[cellOffset + i*2];
    pc = get2byte(pAddr);
    assert( pc<pPage->pBt->usableSize );
    size = cellSizePtr(pPage, &temp[pc]);
    brk -= size;
    memcpy(&data[brk], &temp[pc], size);
    put2byte(pAddr, brk);
  }
  assert( brk>=cellOffset+2*nCell );
  put2byte(&data[hdr+5], brk);
  data[hdr+1] = 0;
  data[hdr+2] = 0;
  data[hdr+7] = 0;
  addr = cellOffset+2*nCell;
  memset(&data[addr], 0, brk-addr);
  sqliteFree(temp);
  return SQLITE_OK;
}

/*
** Allocate nByte bytes of space on a page.
**
** Return the index into pPage->aData[] of the first byte of
** the new allocation. Or return 0 if there is not enough free
** space on the page to satisfy the allocation request.
**
** If the page contains nBytes of free space but does not contain
** nBytes of contiguous free space, then this routine automatically
** calls defragementPage() to consolidate all free space before 
** allocating the new chunk.
*/
static int allocateSpace(MemPage *pPage, int nByte){
  int addr, pc, hdr;
  int size;
  int nFrag;
  int top;
  int nCell;
  int cellOffset;
  unsigned char *data;
  
  data = pPage->aData;
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  assert( pPage->pBt );
  if( nByte<4 ) nByte = 4;
  if( pPage->nFree<nByte || pPage->nOverflow>0 ) return 0;
  pPage->nFree -= nByte;
  hdr = pPage->hdrOffset;

  nFrag = data[hdr+7];
  if( nFrag<60 ){
    /* Search the freelist looking for a slot big enough to satisfy the
    ** space request. */
    addr = hdr+1;
    while( (pc = get2byte(&data[addr]))>0 ){
      size = get2byte(&data[pc+2]);
      if( size>=nByte ){
        if( size<nByte+4 ){
          memcpy(&data[addr], &data[pc], 2);
          data[hdr+7] = nFrag + size - nByte;
          return pc;
        }else{
          put2byte(&data[pc+2], size-nByte);
          return pc + size - nByte;
        }
      }
      addr = pc;
    }
  }

  /* Allocate memory from the gap in between the cell pointer array
  ** and the cell content area.
  */
  top = get2byte(&data[hdr+5]);
  nCell = get2byte(&data[hdr+3]);
  cellOffset = pPage->cellOffset;
  if( nFrag>=60 || cellOffset + 2*nCell > top - nByte ){
    if( defragmentPage(pPage) ) return 0;
    top = get2byte(&data[hdr+5]);
  }
  top -= nByte;
  assert( cellOffset + 2*nCell <= top );
  put2byte(&data[hdr+5], top);
  return top;
}

/*
** Return a section of the pPage->aData to the freelist.
** The first byte of the new free block is pPage->aDisk[start]
** and the size of the block is "size" bytes.
**
** Most of the effort here is involved in coalesing adjacent
** free blocks into a single big free block.
*/
static void freeSpace(MemPage *pPage, int start, int size){
  int addr, pbegin, hdr;
  unsigned char *data = pPage->aData;

  assert( pPage->pBt!=0 );
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  assert( start>=pPage->hdrOffset+6+(pPage->leaf?0:4) );
  assert( (start + size)<=pPage->pBt->usableSize );
  if( size<4 ) size = 4;

#ifdef SQLITE_SECURE_DELETE
  /* Overwrite deleted information with zeros when the SECURE_DELETE 
  ** option is enabled at compile-time */
  memset(&data[start], 0, size);
#endif

  /* Add the space back into the linked list of freeblocks */
  hdr = pPage->hdrOffset;
  addr = hdr + 1;
  while( (pbegin = get2byte(&data[addr]))<start && pbegin>0 ){
    assert( pbegin<=pPage->pBt->usableSize-4 );
    assert( pbegin>addr );
    addr = pbegin;
  }
  assert( pbegin<=pPage->pBt->usableSize-4 );
  assert( pbegin>addr || pbegin==0 );
  put2byte(&data[addr], start);
  put2byte(&data[start], pbegin);
  put2byte(&data[start+2], size);
  pPage->nFree += size;

  /* Coalesce adjacent free blocks */
  addr = pPage->hdrOffset + 1;
  while( (pbegin = get2byte(&data[addr]))>0 ){
    int pnext, psize;
    assert( pbegin>addr );
    assert( pbegin<=pPage->pBt->usableSize-4 );
    pnext = get2byte(&data[pbegin]);
    psize = get2byte(&data[pbegin+2]);
    if( pbegin + psize + 3 >= pnext && pnext>0 ){
      int frag = pnext - (pbegin+psize);
      assert( frag<=data[pPage->hdrOffset+7] );
      data[pPage->hdrOffset+7] -= frag;
      put2byte(&data[pbegin], get2byte(&data[pnext]));
      put2byte(&data[pbegin+2], pnext+get2byte(&data[pnext+2])-pbegin);
    }else{
      addr = pbegin;
    }
  }

  /* If the cell content area begins with a freeblock, remove it. */
  if( data[hdr+1]==data[hdr+5] && data[hdr+2]==data[hdr+6] ){
    int top;
    pbegin = get2byte(&data[hdr+1]);
    memcpy(&data[hdr+1], &data[pbegin], 2);
    top = get2byte(&data[hdr+5]);
    put2byte(&data[hdr+5], top + get2byte(&data[pbegin+2]));
  }
}

/*
** Decode the flags byte (the first byte of the header) for a page
** and initialize fields of the MemPage structure accordingly.
*/
static void decodeFlags(MemPage *pPage, int flagByte){
  BtShared *pBt;     /* A copy of pPage->pBt */

  assert( pPage->hdrOffset==(pPage->pgno==1 ? 100 : 0) );
  pPage->intKey = (flagByte & (PTF_INTKEY|PTF_LEAFDATA))!=0;
  pPage->zeroData = (flagByte & PTF_ZERODATA)!=0;
  pPage->leaf = (flagByte & PTF_LEAF)!=0;
  pPage->childPtrSize = 4*(pPage->leaf==0);
  pBt = pPage->pBt;
  if( flagByte & PTF_LEAFDATA ){
    pPage->leafData = 1;
    pPage->maxLocal = pBt->maxLeaf;
    pPage->minLocal = pBt->minLeaf;
  }else{
    pPage->leafData = 0;
    pPage->maxLocal = pBt->maxLocal;
    pPage->minLocal = pBt->minLocal;
  }
  pPage->hasData = !(pPage->zeroData || (!pPage->leaf && pPage->leafData));
}

/*
** Initialize the auxiliary information for a disk block.
**
** The pParent parameter must be a pointer to the MemPage which
** is the parent of the page being initialized.  The root of a
** BTree has no parent and so for that page, pParent==NULL.
**
** Return SQLITE_OK on success.  If we see that the page does
** not contain a well-formed database page, then return 
** SQLITE_CORRUPT.  Note that a return of SQLITE_OK does not
** guarantee that the page is well-formed.  It only shows that
** we failed to detect any corruption.
*/
static int initPage(
  MemPage *pPage,        /* The page to be initialized */
  MemPage *pParent       /* The parent.  Might be NULL */
){
  int pc;            /* Address of a freeblock within pPage->aData[] */
  int hdr;           /* Offset to beginning of page header */
  u8 *data;          /* Equal to pPage->aData */
  BtShared *pBt;        /* The main btree structure */
  int usableSize;    /* Amount of usable space on each page */
  int cellOffset;    /* Offset from start of page to first cell pointer */
  int nFree;         /* Number of unused bytes on the page */
  int top;           /* First byte of the cell content area */

  pBt = pPage->pBt;
  assert( pBt!=0 );
  assert( pParent==0 || pParent->pBt==pBt );
  assert( pPage->pgno==sqlite3PagerPagenumber(pPage->pDbPage) );
  assert( pPage->aData == &((unsigned char*)pPage)[-pBt->pageSize] );
  if( pPage->pParent!=pParent && (pPage->pParent!=0 || pPage->isInit) ){
    /* The parent page should never change unless the file is corrupt */
    return SQLITE_CORRUPT_BKPT;
  }
  if( pPage->isInit ) return SQLITE_OK;
  if( pPage->pParent==0 && pParent!=0 ){
    pPage->pParent = pParent;
    sqlite3PagerRef(pParent->pDbPage);
  }
  hdr = pPage->hdrOffset;
  data = pPage->aData;
  decodeFlags(pPage, data[hdr]);
  pPage->nOverflow = 0;
  pPage->idxShift = 0;
  usableSize = pBt->usableSize;
  pPage->cellOffset = cellOffset = hdr + 12 - 4*pPage->leaf;
  top = get2byte(&data[hdr+5]);
  pPage->nCell = get2byte(&data[hdr+3]);
  if( pPage->nCell>MX_CELL(pBt) ){
    /* To many cells for a single page.  The page must be corrupt */
    return SQLITE_CORRUPT_BKPT;
  }
  if( pPage->nCell==0 && pParent!=0 && pParent->pgno!=1 ){
    /* All pages must have at least one cell, except for root pages */
    return SQLITE_CORRUPT_BKPT;
  }

  /* Compute the total free space on the page */
  pc = get2byte(&data[hdr+1]);
  nFree = data[hdr+7] + top - (cellOffset + 2*pPage->nCell);
  while( pc>0 ){
    int next, size;
    if( pc>usableSize-4 ){
      /* Free block is off the page */
      return SQLITE_CORRUPT_BKPT; 
    }
    next = get2byte(&data[pc]);
    size = get2byte(&data[pc+2]);
    if( next>0 && next<=pc+size+3 ){
      /* Free blocks must be in accending order */
      return SQLITE_CORRUPT_BKPT; 
    }
    nFree += size;
    pc = next;
  }
  pPage->nFree = nFree;
  if( nFree>=usableSize ){
    /* Free space cannot exceed total page size */
    return SQLITE_CORRUPT_BKPT; 
  }

  pPage->isInit = 1;
  return SQLITE_OK;
}

/*
** Set up a raw page so that it looks like a database page holding
** no entries.
*/
static void zeroPage(MemPage *pPage, int flags){
  unsigned char *data = pPage->aData;
  BtShared *pBt = pPage->pBt;
  int hdr = pPage->hdrOffset;
  int first;

  assert( sqlite3PagerPagenumber(pPage->pDbPage)==pPage->pgno );
  assert( &data[pBt->pageSize] == (unsigned char*)pPage );
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  memset(&data[hdr], 0, pBt->usableSize - hdr);
  data[hdr] = flags;
  first = hdr + 8 + 4*((flags&PTF_LEAF)==0);
  memset(&data[hdr+1], 0, 4);
  data[hdr+7] = 0;
  put2byte(&data[hdr+5], pBt->usableSize);
  pPage->nFree = pBt->usableSize - first;
  decodeFlags(pPage, flags);
  pPage->hdrOffset = hdr;
  pPage->cellOffset = first;
  pPage->nOverflow = 0;
  pPage->idxShift = 0;
  pPage->nCell = 0;
  pPage->isInit = 1;
}

/*
** Get a page from the pager.  Initialize the MemPage.pBt and
** MemPage.aData elements if needed.
**
** If the noContent flag is set, it means that we do not care about
** the content of the page at this time.  So do not go to the disk
** to fetch the content.  Just fill in the content with zeros for now.
** If in the future we call sqlite3PagerWrite() on this page, that
** means we have started to be concerned about content and the disk
** read should occur at that point.
*/
static int getPage(BtShared *pBt, Pgno pgno, MemPage **ppPage, int noContent){
  int rc;
  MemPage *pPage;
  DbPage *pDbPage;

  rc = sqlite3PagerAcquire(pBt->pPager, pgno, (DbPage**)&pDbPage, noContent);
  if( rc ) return rc;
  pPage = (MemPage *)sqlite3PagerGetExtra(pDbPage);
  pPage->aData = sqlite3PagerGetData(pDbPage);
  pPage->pDbPage = pDbPage;
  pPage->pBt = pBt;
  pPage->pgno = pgno;
  pPage->hdrOffset = pPage->pgno==1 ? 100 : 0;
  *ppPage = pPage;
  return SQLITE_OK;
}

/*
** Get a page from the pager and initialize it.  This routine
** is just a convenience wrapper around separate calls to
** getPage() and initPage().
*/
static int getAndInitPage(
  BtShared *pBt,          /* The database file */
  Pgno pgno,           /* Number of the page to get */
  MemPage **ppPage,    /* Write the page pointer here */
  MemPage *pParent     /* Parent of the page */
){
  int rc;
  if( pgno==0 ){
    return SQLITE_CORRUPT_BKPT; 
  }
  rc = getPage(pBt, pgno, ppPage, 0);
  if( rc==SQLITE_OK && (*ppPage)->isInit==0 ){
    rc = initPage(*ppPage, pParent);
  }
  return rc;
}

/*
** Release a MemPage.  This should be called once for each prior
** call to getPage.
*/
static void releasePage(MemPage *pPage){
  if( pPage ){
    assert( pPage->aData );
    assert( pPage->pBt );
    assert( &pPage->aData[pPage->pBt->pageSize]==(unsigned char*)pPage );
    sqlite3PagerUnref(pPage->pDbPage);
  }
}

/*
** This routine is called when the reference count for a page
** reaches zero.  We need to unref the pParent pointer when that
** happens.
*/
static void pageDestructor(DbPage *pData, int pageSize){
  MemPage *pPage;
  assert( (pageSize & 7)==0 );
  pPage = (MemPage *)sqlite3PagerGetExtra(pData);
  if( pPage->pParent ){
    MemPage *pParent = pPage->pParent;
    pPage->pParent = 0;
    releasePage(pParent);
  }
  pPage->isInit = 0;
}

/*
** During a rollback, when the pager reloads information into the cache
** so that the cache is restored to its original state at the start of
** the transaction, for each page restored this routine is called.
**
** This routine needs to reset the extra data section at the end of the
** page to agree with the restored data.
*/
static void pageReinit(DbPage *pData, int pageSize){
  MemPage *pPage;
  assert( (pageSize & 7)==0 );
  pPage = (MemPage *)sqlite3PagerGetExtra(pData);
  if( pPage->isInit ){
    pPage->isInit = 0;
    initPage(pPage, pPage->pParent);
  }
}

/*
** Open a database file.
** 
** zFilename is the name of the database file.  If zFilename is NULL
** a new database with a random name is created.  This randomly named
** database file will be deleted when sqlite3BtreeClose() is called.
*/
int sqlite3BtreeOpen(
  const char *zFilename,  /* Name of the file containing the BTree database */
  sqlite3 *pSqlite,       /* Associated database handle */
  Btree **ppBtree,        /* Pointer to new Btree object written here */
  int flags               /* Options */
){
  BtShared *pBt;          /* Shared part of btree structure */
  Btree *p;               /* Handle to return */
  int rc;
  int nReserve;
  unsigned char zDbHeader[100];
#if !defined(SQLITE_OMIT_SHARED_CACHE) && !defined(SQLITE_OMIT_DISKIO)
  const ThreadData *pTsdro;
#endif

  /* Set the variable isMemdb to true for an in-memory database, or 
  ** false for a file-based database. This symbol is only required if
  ** either of the shared-data or autovacuum features are compiled 
  ** into the library.
  */
#if !defined(SQLITE_OMIT_SHARED_CACHE) || !defined(SQLITE_OMIT_AUTOVACUUM)
  #ifdef SQLITE_OMIT_MEMORYDB
    const int isMemdb = 0;
  #else
    const int isMemdb = zFilename && !strcmp(zFilename, ":memory:");
  #endif
#endif

  p = sqliteMalloc(sizeof(Btree));
  if( !p ){
    return SQLITE_NOMEM;
  }
  p->inTrans = TRANS_NONE;
  p->pSqlite = pSqlite;

  /* Try to find an existing Btree structure opened on zFilename. */
#if !defined(SQLITE_OMIT_SHARED_CACHE) && !defined(SQLITE_OMIT_DISKIO)
  pTsdro = sqlite3ThreadDataReadOnly();
  if( pTsdro->useSharedData && zFilename && !isMemdb ){
    char *zFullPathname = sqlite3OsFullPathname(zFilename);
    if( !zFullPathname ){
      sqliteFree(p);
      return SQLITE_NOMEM;
    }
    for(pBt=pTsdro->pBtree; pBt; pBt=pBt->pNext){
      assert( pBt->nRef>0 );
      if( 0==strcmp(zFullPathname, sqlite3PagerFilename(pBt->pPager)) ){
        p->pBt = pBt;
        *ppBtree = p;
        pBt->nRef++;
        sqliteFree(zFullPathname);
        return SQLITE_OK;
      }
    }
    sqliteFree(zFullPathname);
  }
#endif

  /*
  ** The following asserts make sure that structures used by the btree are
  ** the right size.  This is to guard against size changes that result
  ** when compiling on a different architecture.
  */
  assert( sizeof(i64)==8 || sizeof(i64)==4 );
  assert( sizeof(u64)==8 || sizeof(u64)==4 );
  assert( sizeof(u32)==4 );
  assert( sizeof(u16)==2 );
  assert( sizeof(Pgno)==4 );

  pBt = sqliteMalloc( sizeof(*pBt) );
  if( pBt==0 ){
    *ppBtree = 0;
    sqliteFree(p);
    return SQLITE_NOMEM;
  }
  rc = sqlite3PagerOpen(&pBt->pPager, zFilename, EXTRA_SIZE, flags);
  if( rc==SQLITE_OK ){
    rc = sqlite3PagerReadFileheader(pBt->pPager,sizeof(zDbHeader),zDbHeader);
  }
  if( rc!=SQLITE_OK ){
    if( pBt->pPager ){
      sqlite3PagerClose(pBt->pPager);
    }
    sqliteFree(pBt);
    sqliteFree(p);
    *ppBtree = 0;
    return rc;
  }
  p->pBt = pBt;

  sqlite3PagerSetDestructor(pBt->pPager, pageDestructor);
  sqlite3PagerSetReiniter(pBt->pPager, pageReinit);
  pBt->pCursor = 0;
  pBt->pPage1 = 0;
  pBt->readOnly = sqlite3PagerIsreadonly(pBt->pPager);
  pBt->pageSize = get2byte(&zDbHeader[16]);
  if( pBt->pageSize<512 || pBt->pageSize>SQLITE_MAX_PAGE_SIZE
       || ((pBt->pageSize-1)&pBt->pageSize)!=0 ){
    pBt->pageSize = SQLITE_DEFAULT_PAGE_SIZE;
    pBt->maxEmbedFrac = 64;   /* 25% */
    pBt->minEmbedFrac = 32;   /* 12.5% */
    pBt->minLeafFrac = 32;    /* 12.5% */
#ifndef SQLITE_OMIT_AUTOVACUUM
    /* If the magic name ":memory:" will create an in-memory database, then
    ** do not set the auto-vacuum flag, even if SQLITE_DEFAULT_AUTOVACUUM
    ** is true. On the other hand, if SQLITE_OMIT_MEMORYDB has been defined,
    ** then ":memory:" is just a regular file-name. Respect the auto-vacuum
    ** default in this case.
    */
    if( zFilename && !isMemdb ){
      pBt->autoVacuum = SQLITE_DEFAULT_AUTOVACUUM;
    }
#endif
    nReserve = 0;
  }else{
    nReserve = zDbHeader[20];
    pBt->maxEmbedFrac = zDbHeader[21];
    pBt->minEmbedFrac = zDbHeader[22];
    pBt->minLeafFrac = zDbHeader[23];
    pBt->pageSizeFixed = 1;
#ifndef SQLITE_OMIT_AUTOVACUUM
    pBt->autoVacuum = (get4byte(&zDbHeader[36 + 4*4])?1:0);
#endif
  }
  pBt->usableSize = pBt->pageSize - nReserve;
  assert( (pBt->pageSize & 7)==0 );  /* 8-byte alignment of pageSize */
  sqlite3PagerSetPagesize(pBt->pPager, pBt->pageSize);

#if !defined(SQLITE_OMIT_SHARED_CACHE) && !defined(SQLITE_OMIT_DISKIO)
  /* Add the new btree to the linked list starting at ThreadData.pBtree.
  ** There is no chance that a malloc() may fail inside of the 
  ** sqlite3ThreadData() call, as the ThreadData structure must have already
  ** been allocated for pTsdro->useSharedData to be non-zero.
  */
  if( pTsdro->useSharedData && zFilename && !isMemdb ){
    pBt->pNext = pTsdro->pBtree;
    sqlite3ThreadData()->pBtree = pBt;
  }
#endif
  pBt->nRef = 1;
  *ppBtree = p;
  return SQLITE_OK;
}

/*
** Close an open database and invalidate all cursors.
*/
int sqlite3BtreeClose(Btree *p){
  BtShared *pBt = p->pBt;
  BtCursor *pCur;

#ifndef SQLITE_OMIT_SHARED_CACHE
  ThreadData *pTsd;
#endif

  /* Close all cursors opened via this handle.  */
  pCur = pBt->pCursor;
  while( pCur ){
    BtCursor *pTmp = pCur;
    pCur = pCur->pNext;
    if( pTmp->pBtree==p ){
      sqlite3BtreeCloseCursor(pTmp);
    }
  }

  /* Rollback any active transaction and free the handle structure.
  ** The call to sqlite3BtreeRollback() drops any table-locks held by
  ** this handle.
  */
  sqlite3BtreeRollback(p);
  sqliteFree(p);

#ifndef SQLITE_OMIT_SHARED_CACHE
  /* If there are still other outstanding references to the shared-btree
  ** structure, return now. The remainder of this procedure cleans 
  ** up the shared-btree.
  */
  assert( pBt->nRef>0 );
  pBt->nRef--;
  if( pBt->nRef ){
    return SQLITE_OK;
  }

  /* Remove the shared-btree from the thread wide list. Call 
  ** ThreadDataReadOnly() and then cast away the const property of the 
  ** pointer to avoid allocating thread data if it is not really required.
  */
  pTsd = (ThreadData *)sqlite3ThreadDataReadOnly();
  if( pTsd->pBtree==pBt ){
    assert( pTsd==sqlite3ThreadData() );
    pTsd->pBtree = pBt->pNext;
  }else{
    BtShared *pPrev;
    for(pPrev=pTsd->pBtree; pPrev && pPrev->pNext!=pBt; pPrev=pPrev->pNext){}
    if( pPrev ){
      assert( pTsd==sqlite3ThreadData() );
      pPrev->pNext = pBt->pNext;
    }
  }
#endif

  /* Close the pager and free the shared-btree structure */
  assert( !pBt->pCursor );
  sqlite3PagerClose(pBt->pPager);
  if( pBt->xFreeSchema && pBt->pSchema ){
    pBt->xFreeSchema(pBt->pSchema);
  }
  sqliteFree(pBt->pSchema);
  sqliteFree(pBt);
  return SQLITE_OK;
}

/*
** Change the busy handler callback function.
*/
int sqlite3BtreeSetBusyHandler(Btree *p, BusyHandler *pHandler){
  BtShared *pBt = p->pBt;
  pBt->pBusyHandler = pHandler;
  sqlite3PagerSetBusyhandler(pBt->pPager, pHandler);
  return SQLITE_OK;
}

/*
** Change the limit on the number of pages allowed in the cache.
**
** The maximum number of cache pages is set to the absolute
** value of mxPage.  If mxPage is negative, the pager will
** operate asynchronously - it will not stop to do fsync()s
** to insure data is written to the disk surface before
** continuing.  Transactions still work if synchronous is off,
** and the database cannot be corrupted if this program
** crashes.  But if the operating system crashes or there is
** an abrupt power failure when synchronous is off, the database
** could be left in an inconsistent and unrecoverable state.
** Synchronous is on by default so database corruption is not
** normally a worry.
*/
int sqlite3BtreeSetCacheSize(Btree *p, int mxPage){
  BtShared *pBt = p->pBt;
  sqlite3PagerSetCachesize(pBt->pPager, mxPage);
  return SQLITE_OK;
}

/*
** Change the way data is synced to disk in order to increase or decrease
** how well the database resists damage due to OS crashes and power
** failures.  Level 1 is the same as asynchronous (no syncs() occur and
** there is a high probability of damage)  Level 2 is the default.  There
** is a very low but non-zero probability of damage.  Level 3 reduces the
** probability of damage to near zero but with a write performance reduction.
*/
#ifndef SQLITE_OMIT_PAGER_PRAGMAS
int sqlite3BtreeSetSafetyLevel(Btree *p, int level, int fullSync){
  BtShared *pBt = p->pBt;
  sqlite3PagerSetSafetyLevel(pBt->pPager, level, fullSync);
  return SQLITE_OK;
}
#endif

/*
** Return TRUE if the given btree is set to safety level 1.  In other
** words, return TRUE if no sync() occurs on the disk files.
*/
int sqlite3BtreeSyncDisabled(Btree *p){
  BtShared *pBt = p->pBt;
  assert( pBt && pBt->pPager );
  return sqlite3PagerNosync(pBt->pPager);
}

#if !defined(SQLITE_OMIT_PAGER_PRAGMAS) || !defined(SQLITE_OMIT_VACUUM)
/*
** Change the default pages size and the number of reserved bytes per page.
**
** The page size must be a power of 2 between 512 and 65536.  If the page
** size supplied does not meet this constraint then the page size is not
** changed.
**
** Page sizes are constrained to be a power of two so that the region
** of the database file used for locking (beginning at PENDING_BYTE,
** the first byte past the 1GB boundary, 0x40000000) needs to occur
** at the beginning of a page.
**
** If parameter nReserve is less than zero, then the number of reserved
** bytes per page is left unchanged.
*/
int sqlite3BtreeSetPageSize(Btree *p, int pageSize, int nReserve){
  BtShared *pBt = p->pBt;
  if( pBt->pageSizeFixed ){
    return SQLITE_READONLY;
  }
  if( nReserve<0 ){
    nReserve = pBt->pageSize - pBt->usableSize;
  }
  if( pageSize>=512 && pageSize<=SQLITE_MAX_PAGE_SIZE &&
        ((pageSize-1)&pageSize)==0 ){
    assert( (pageSize & 7)==0 );
    assert( !pBt->pPage1 && !pBt->pCursor );
    pBt->pageSize = sqlite3PagerSetPagesize(pBt->pPager, pageSize);
  }
  pBt->usableSize = pBt->pageSize - nReserve;
  return SQLITE_OK;
}

/*
** Return the currently defined page size
*/
int sqlite3BtreeGetPageSize(Btree *p){
  return p->pBt->pageSize;
}
int sqlite3BtreeGetReserve(Btree *p){
  return p->pBt->pageSize - p->pBt->usableSize;
}
#endif /* !defined(SQLITE_OMIT_PAGER_PRAGMAS) || !defined(SQLITE_OMIT_VACUUM) */

/*
** Change the 'auto-vacuum' property of the database. If the 'autoVacuum'
** parameter is non-zero, then auto-vacuum mode is enabled. If zero, it
** is disabled. The default value for the auto-vacuum property is 
** determined by the SQLITE_DEFAULT_AUTOVACUUM macro.
*/
int sqlite3BtreeSetAutoVacuum(Btree *p, int autoVacuum){
  BtShared *pBt = p->pBt;;
#ifdef SQLITE_OMIT_AUTOVACUUM
  return SQLITE_READONLY;
#else
  if( pBt->pageSizeFixed ){
    return SQLITE_READONLY;
  }
  pBt->autoVacuum = (autoVacuum?1:0);
  return SQLITE_OK;
#endif
}

/*
** Return the value of the 'auto-vacuum' property. If auto-vacuum is 
** enabled 1 is returned. Otherwise 0.
*/
int sqlite3BtreeGetAutoVacuum(Btree *p){
#ifdef SQLITE_OMIT_AUTOVACUUM
  return 0;
#else
  return p->pBt->autoVacuum;
#endif
}


/*
** Get a reference to pPage1 of the database file.  This will
** also acquire a readlock on that file.
**
** SQLITE_OK is returned on success.  If the file is not a
** well-formed database file, then SQLITE_CORRUPT is returned.
** SQLITE_BUSY is returned if the database is locked.  SQLITE_NOMEM
** is returned if we run out of memory. 
*/
static int lockBtree(BtShared *pBt){
  int rc, pageSize;
  MemPage *pPage1;
  if( pBt->pPage1 ) return SQLITE_OK;
  rc = getPage(pBt, 1, &pPage1, 0);
  if( rc!=SQLITE_OK ) return rc;
  

  /* Do some checking to help insure the file we opened really is
  ** a valid database file. 
  */
  rc = SQLITE_NOTADB;
  if( sqlite3PagerPagecount(pBt->pPager)>0 ){
    u8 *page1 = pPage1->aData;
    if( memcmp(page1, zMagicHeader, 16)!=0 ){
      goto page1_init_failed;
    }
    if( page1[18]>1 ){
      pBt->readOnly = 1;
    }
    if( page1[19]>1 ){
      goto page1_init_failed;
    }
    pageSize = get2byte(&page1[16]);
    if( ((pageSize-1)&pageSize)!=0 || pageSize<512 ){
      goto page1_init_failed;
    }
    assert( (pageSize & 7)==0 );
    pBt->pageSize = pageSize;
    pBt->usableSize = pageSize - page1[20];
    if( pBt->usableSize<500 ){
      goto page1_init_failed;
    }
    pBt->maxEmbedFrac = page1[21];
    pBt->minEmbedFrac = page1[22];
    pBt->minLeafFrac = page1[23];
#ifndef SQLITE_OMIT_AUTOVACUUM
    pBt->autoVacuum = (get4byte(&page1[36 + 4*4])?1:0);
#endif
  }

  /* maxLocal is the maximum amount of payload to store locally for
  ** a cell.  Make sure it is small enough so that at least minFanout
  ** cells can will fit on one page.  We assume a 10-byte page header.
  ** Besides the payload, the cell must store:
  **     2-byte pointer to the cell
  **     4-byte child pointer
  **     9-byte nKey value
  **     4-byte nData value
  **     4-byte overflow page pointer
  ** So a cell consists of a 2-byte poiner, a header which is as much as
  ** 17 bytes long, 0 to N bytes of payload, and an optional 4 byte overflow
  ** page pointer.
  */
  pBt->maxLocal = (pBt->usableSize-12)*pBt->maxEmbedFrac/255 - 23;
  pBt->minLocal = (pBt->usableSize-12)*pBt->minEmbedFrac/255 - 23;
  pBt->maxLeaf = pBt->usableSize - 35;
  pBt->minLeaf = (pBt->usableSize-12)*pBt->minLeafFrac/255 - 23;
  if( pBt->minLocal>pBt->maxLocal || pBt->maxLocal<0 ){
    goto page1_init_failed;
  }
  assert( pBt->maxLeaf + 23 <= MX_CELL_SIZE(pBt) );
  pBt->pPage1 = pPage1;
  return SQLITE_OK;

page1_init_failed:
  releasePage(pPage1);
  pBt->pPage1 = 0;
  return rc;
}

/*
** This routine works like lockBtree() except that it also invokes the
** busy callback if there is lock contention.
*/
static int lockBtreeWithRetry(Btree *pRef){
  int rc = SQLITE_OK;
  if( pRef->inTrans==TRANS_NONE ){
    u8 inTransaction = pRef->pBt->inTransaction;
    btreeIntegrity(pRef);
    rc = sqlite3BtreeBeginTrans(pRef, 0);
    pRef->pBt->inTransaction = inTransaction;
    pRef->inTrans = TRANS_NONE;
    if( rc==SQLITE_OK ){
      pRef->pBt->nTransaction--;
    }
    btreeIntegrity(pRef);
  }
  return rc;
}
       

/*
** If there are no outstanding cursors and we are not in the middle
** of a transaction but there is a read lock on the database, then
** this routine unrefs the first page of the database file which 
** has the effect of releasing the read lock.
**
** If there are any outstanding cursors, this routine is a no-op.
**
** If there is a transaction in progress, this routine is a no-op.
*/
static void unlockBtreeIfUnused(BtShared *pBt){
  if( pBt->inTransaction==TRANS_NONE && pBt->pCursor==0 && pBt->pPage1!=0 ){
    if( sqlite3PagerRefcount(pBt->pPager)>=1 ){
      if( pBt->pPage1->aData==0 ){
        MemPage *pPage = pBt->pPage1;
        pPage->aData = &((u8*)pPage)[-pBt->pageSize];
        pPage->pBt = pBt;
        pPage->pgno = 1;
      }
      releasePage(pBt->pPage1);
    }
    pBt->pPage1 = 0;
    pBt->inStmt = 0;
  }
}

/*
** Create a new database by initializing the first page of the
** file.
*/
static int newDatabase(BtShared *pBt){
  MemPage *pP1;
  unsigned char *data;
  int rc;
  if( sqlite3PagerPagecount(pBt->pPager)>0 ) return SQLITE_OK;
  pP1 = pBt->pPage1;
  assert( pP1!=0 );
  data = pP1->aData;
  rc = sqlite3PagerWrite(pP1->pDbPage);
  if( rc ) return rc;
  memcpy(data, zMagicHeader, sizeof(zMagicHeader));
  assert( sizeof(zMagicHeader)==16 );
  put2byte(&data[16], pBt->pageSize);
  data[18] = 1;
  data[19] = 1;
  data[20] = pBt->pageSize - pBt->usableSize;
  data[21] = pBt->maxEmbedFrac;
  data[22] = pBt->minEmbedFrac;
  data[23] = pBt->minLeafFrac;
  memset(&data[24], 0, 100-24);
  zeroPage(pP1, PTF_INTKEY|PTF_LEAF|PTF_LEAFDATA );
  pBt->pageSizeFixed = 1;
#ifndef SQLITE_OMIT_AUTOVACUUM
  if( pBt->autoVacuum ){
    put4byte(&data[36 + 4*4], 1);
  }
#endif
  return SQLITE_OK;
}

/*
** Attempt to start a new transaction. A write-transaction
** is started if the second argument is nonzero, otherwise a read-
** transaction.  If the second argument is 2 or more and exclusive
** transaction is started, meaning that no other process is allowed
** to access the database.  A preexisting transaction may not be
** upgraded to exclusive by calling this routine a second time - the
** exclusivity flag only works for a new transaction.
**
** A write-transaction must be started before attempting any 
** changes to the database.  None of the following routines 
** will work unless a transaction is started first:
**
**      sqlite3BtreeCreateTable()
**      sqlite3BtreeCreateIndex()
**      sqlite3BtreeClearTable()
**      sqlite3BtreeDropTable()
**      sqlite3BtreeInsert()
**      sqlite3BtreeDelete()
**      sqlite3BtreeUpdateMeta()
**
** If an initial attempt to acquire the lock fails because of lock contention
** and the database was previously unlocked, then invoke the busy handler
** if there is one.  But if there was previously a read-lock, do not
** invoke the busy handler - just return SQLITE_BUSY.  SQLITE_BUSY is 
** returned when there is already a read-lock in order to avoid a deadlock.
**
** Suppose there are two processes A and B.  A has a read lock and B has
** a reserved lock.  B tries to promote to exclusive but is blocked because
** of A's read lock.  A tries to promote to reserved but is blocked by B.
** One or the other of the two processes must give way or there can be
** no progress.  By returning SQLITE_BUSY and not invoking the busy callback
** when A already has a read lock, we encourage A to give up and let B
** proceed.
*/
int sqlite3BtreeBeginTrans(Btree *p, int wrflag){
  BtShared *pBt = p->pBt;
  int rc = SQLITE_OK;

  btreeIntegrity(p);

  /* If the btree is already in a write-transaction, or it
  ** is already in a read-transaction and a read-transaction
  ** is requested, this is a no-op.
  */
  if( p->inTrans==TRANS_WRITE || (p->inTrans==TRANS_READ && !wrflag) ){
    return SQLITE_OK;
  }

  /* Write transactions are not possible on a read-only database */
  if( pBt->readOnly && wrflag ){
    return SQLITE_READONLY;
  }

  /* If another database handle has already opened a write transaction 
  ** on this shared-btree structure and a second write transaction is
  ** requested, return SQLITE_BUSY.
  */
  if( pBt->inTransaction==TRANS_WRITE && wrflag ){
    return SQLITE_BUSY;
  }

  do {
    if( pBt->pPage1==0 ){
      rc = lockBtree(pBt);
    }

    if( rc==SQLITE_OK && wrflag ){
      if( pBt->readOnly ){
        rc = SQLITE_READONLY;
      }else{
        rc = sqlite3PagerBegin(pBt->pPage1->pDbPage, wrflag>1);
        if( rc==SQLITE_OK ){
          rc = newDatabase(pBt);
        }
      }
    }
  
    if( rc==SQLITE_OK ){
      if( wrflag ) pBt->inStmt = 0;
    }else{
      unlockBtreeIfUnused(pBt);
    }
  }while( rc==SQLITE_BUSY && pBt->inTransaction==TRANS_NONE &&
          sqlite3InvokeBusyHandler(pBt->pBusyHandler) );

  if( rc==SQLITE_OK ){
    if( p->inTrans==TRANS_NONE ){
      pBt->nTransaction++;
    }
    p->inTrans = (wrflag?TRANS_WRITE:TRANS_READ);
    if( p->inTrans>pBt->inTransaction ){
      pBt->inTransaction = p->inTrans;
    }
  }

  btreeIntegrity(p);
  return rc;
}

#ifndef SQLITE_OMIT_AUTOVACUUM

/*
** Set the pointer-map entries for all children of page pPage. Also, if
** pPage contains cells that point to overflow pages, set the pointer
** map entries for the overflow pages as well.
*/
static int setChildPtrmaps(MemPage *pPage){
  int i;                             /* Counter variable */
  int nCell;                         /* Number of cells in page pPage */
  int rc = SQLITE_OK;                /* Return code */
  BtShared *pBt = pPage->pBt;
  int isInitOrig = pPage->isInit;
  Pgno pgno = pPage->pgno;

  initPage(pPage, 0);
  nCell = pPage->nCell;

  for(i=0; i<nCell; i++){
    u8 *pCell = findCell(pPage, i);

    rc = ptrmapPutOvflPtr(pPage, pCell);
    if( rc!=SQLITE_OK ){
      goto set_child_ptrmaps_out;
    }

    if( !pPage->leaf ){
      Pgno childPgno = get4byte(pCell);
      rc = ptrmapPut(pBt, childPgno, PTRMAP_BTREE, pgno);
      if( rc!=SQLITE_OK ) goto set_child_ptrmaps_out;
    }
  }

  if( !pPage->leaf ){
    Pgno childPgno = get4byte(&pPage->aData[pPage->hdrOffset+8]);
    rc = ptrmapPut(pBt, childPgno, PTRMAP_BTREE, pgno);
  }

set_child_ptrmaps_out:
  pPage->isInit = isInitOrig;
  return rc;
}

/*
** Somewhere on pPage, which is guarenteed to be a btree page, not an overflow
** page, is a pointer to page iFrom. Modify this pointer so that it points to
** iTo. Parameter eType describes the type of pointer to be modified, as 
** follows:
**
** PTRMAP_BTREE:     pPage is a btree-page. The pointer points at a child 
**                   page of pPage.
**
** PTRMAP_OVERFLOW1: pPage is a btree-page. The pointer points at an overflow
**                   page pointed to by one of the cells on pPage.
**
** PTRMAP_OVERFLOW2: pPage is an overflow-page. The pointer points at the next
**                   overflow page in the list.
*/
static int modifyPagePointer(MemPage *pPage, Pgno iFrom, Pgno iTo, u8 eType){
  if( eType==PTRMAP_OVERFLOW2 ){
    /* The pointer is always the first 4 bytes of the page in this case.  */
    if( get4byte(pPage->aData)!=iFrom ){
      return SQLITE_CORRUPT_BKPT;
    }
    put4byte(pPage->aData, iTo);
  }else{
    int isInitOrig = pPage->isInit;
    int i;
    int nCell;

    initPage(pPage, 0);
    nCell = pPage->nCell;

    for(i=0; i<nCell; i++){
      u8 *pCell = findCell(pPage, i);
      if( eType==PTRMAP_OVERFLOW1 ){
        CellInfo info;
        parseCellPtr(pPage, pCell, &info);
        if( info.iOverflow ){
          if( iFrom==get4byte(&pCell[info.iOverflow]) ){
            put4byte(&pCell[info.iOverflow], iTo);
            break;
          }
        }
      }else{
        if( get4byte(pCell)==iFrom ){
          put4byte(pCell, iTo);
          break;
        }
      }
    }
  
    if( i==nCell ){
      if( eType!=PTRMAP_BTREE || 
          get4byte(&pPage->aData[pPage->hdrOffset+8])!=iFrom ){
        return SQLITE_CORRUPT_BKPT;
      }
      put4byte(&pPage->aData[pPage->hdrOffset+8], iTo);
    }

    pPage->isInit = isInitOrig;
  }
  return SQLITE_OK;
}


/*
** Move the open database page pDbPage to location iFreePage in the 
** database. The pDbPage reference remains valid.
*/
static int relocatePage(
  BtShared *pBt,           /* Btree */
  MemPage *pDbPage,        /* Open page to move */
  u8 eType,                /* Pointer map 'type' entry for pDbPage */
  Pgno iPtrPage,           /* Pointer map 'page-no' entry for pDbPage */
  Pgno iFreePage           /* The location to move pDbPage to */
){
  MemPage *pPtrPage;   /* The page that contains a pointer to pDbPage */
  Pgno iDbPage = pDbPage->pgno;
  Pager *pPager = pBt->pPager;
  int rc;

  assert( eType==PTRMAP_OVERFLOW2 || eType==PTRMAP_OVERFLOW1 || 
      eType==PTRMAP_BTREE || eType==PTRMAP_ROOTPAGE );

  /* Move page iDbPage from it's current location to page number iFreePage */
  TRACE(("AUTOVACUUM: Moving %d to free page %d (ptr page %d type %d)\n", 
      iDbPage, iFreePage, iPtrPage, eType));
  rc = sqlite3PagerMovepage(pPager, pDbPage->pDbPage, iFreePage);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  pDbPage->pgno = iFreePage;

  /* If pDbPage was a btree-page, then it may have child pages and/or cells
  ** that point to overflow pages. The pointer map entries for all these
  ** pages need to be changed.
  **
  ** If pDbPage is an overflow page, then the first 4 bytes may store a
  ** pointer to a subsequent overflow page. If this is the case, then
  ** the pointer map needs to be updated for the subsequent overflow page.
  */
  if( eType==PTRMAP_BTREE || eType==PTRMAP_ROOTPAGE ){
    rc = setChildPtrmaps(pDbPage);
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }else{
    Pgno nextOvfl = get4byte(pDbPage->aData);
    if( nextOvfl!=0 ){
      rc = ptrmapPut(pBt, nextOvfl, PTRMAP_OVERFLOW2, iFreePage);
      if( rc!=SQLITE_OK ){
        return rc;
      }
    }
  }

  /* Fix the database pointer on page iPtrPage that pointed at iDbPage so
  ** that it points at iFreePage. Also fix the pointer map entry for
  ** iPtrPage.
  */
  if( eType!=PTRMAP_ROOTPAGE ){
    rc = getPage(pBt, iPtrPage, &pPtrPage, 0);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    rc = sqlite3PagerWrite(pPtrPage->pDbPage);
    if( rc!=SQLITE_OK ){
      releasePage(pPtrPage);
      return rc;
    }
    rc = modifyPagePointer(pPtrPage, iDbPage, iFreePage, eType);
    releasePage(pPtrPage);
    if( rc==SQLITE_OK ){
      rc = ptrmapPut(pBt, iFreePage, eType, iPtrPage);
    }
  }
  return rc;
}

/* Forward declaration required by autoVacuumCommit(). */
static int allocateBtreePage(BtShared *, MemPage **, Pgno *, Pgno, u8);

/*
** This routine is called prior to sqlite3PagerCommit when a transaction
** is commited for an auto-vacuum database.
**
** If SQLITE_OK is returned, then *pnTrunc is set to the number of pages
** the database file should be truncated to during the commit process. 
** i.e. the database has been reorganized so that only the first *pnTrunc
** pages are in use.
*/
static int autoVacuumCommit(BtShared *pBt, Pgno *pnTrunc){
  Pager *pPager = pBt->pPager;
  Pgno nFreeList;            /* Number of pages remaining on the free-list. */
  int nPtrMap;               /* Number of pointer-map pages deallocated */
  Pgno origSize;             /* Pages in the database file */
  Pgno finSize;              /* Pages in the database file after truncation */
  int rc;                    /* Return code */
  u8 eType;
  int pgsz = pBt->pageSize;  /* Page size for this database */
  Pgno iDbPage;              /* The database page to move */
  MemPage *pDbMemPage = 0;   /* "" */
  Pgno iPtrPage;             /* The page that contains a pointer to iDbPage */
  Pgno iFreePage;            /* The free-list page to move iDbPage to */
  MemPage *pFreeMemPage = 0; /* "" */

#ifndef NDEBUG
  int nRef = sqlite3PagerRefcount(pPager);
#endif

  assert( pBt->autoVacuum );
  if( PTRMAP_ISPAGE(pBt, sqlite3PagerPagecount(pPager)) ){
    return SQLITE_CORRUPT_BKPT;
  }

  /* Figure out how many free-pages are in the database. If there are no
  ** free pages, then auto-vacuum is a no-op.
  */
  nFreeList = get4byte(&pBt->pPage1->aData[36]);
  if( nFreeList==0 ){
    *pnTrunc = 0;
    return SQLITE_OK;
  }

  /* This block figures out how many pages there are in the database
  ** now (variable origSize), and how many there will be after the
  ** truncation (variable finSize).
  **
  ** The final size is the original size, less the number of free pages
  ** in the database, less any pointer-map pages that will no longer
  ** be required, less 1 if the pending-byte page was part of the database
  ** but is not after the truncation.
  **/
  origSize = sqlite3PagerPagecount(pPager);
  if( origSize==PENDING_BYTE_PAGE(pBt) ){
    origSize--;
  }
  nPtrMap = (nFreeList-origSize+PTRMAP_PAGENO(pBt, origSize)+pgsz/5)/(pgsz/5);
  finSize = origSize - nFreeList - nPtrMap;
  if( origSize>PENDING_BYTE_PAGE(pBt) && finSize<=PENDING_BYTE_PAGE(pBt) ){
    finSize--;
  }
  while( PTRMAP_ISPAGE(pBt, finSize) || finSize==PENDING_BYTE_PAGE(pBt) ){
    finSize--;
  }
  TRACE(("AUTOVACUUM: Begin (db size %d->%d)\n", origSize, finSize));

  /* Variable 'finSize' will be the size of the file in pages after
  ** the auto-vacuum has completed (the current file size minus the number
  ** of pages on the free list). Loop through the pages that lie beyond
  ** this mark, and if they are not already on the free list, move them
  ** to a free page earlier in the file (somewhere before finSize).
  */
  for( iDbPage=finSize+1; iDbPage<=origSize; iDbPage++ ){
    /* If iDbPage is a pointer map page, or the pending-byte page, skip it. */
    if( PTRMAP_ISPAGE(pBt, iDbPage) || iDbPage==PENDING_BYTE_PAGE(pBt) ){
      continue;
    }

    rc = ptrmapGet(pBt, iDbPage, &eType, &iPtrPage);
    if( rc!=SQLITE_OK ) goto autovacuum_out;
    if( eType==PTRMAP_ROOTPAGE ){
      rc = SQLITE_CORRUPT_BKPT;
      goto autovacuum_out;
    }

    /* If iDbPage is free, do not swap it.  */
    if( eType==PTRMAP_FREEPAGE ){
      continue;
    }
    rc = getPage(pBt, iDbPage, &pDbMemPage, 0);
    if( rc!=SQLITE_OK ) goto autovacuum_out;

    /* Find the next page in the free-list that is not already at the end 
    ** of the file. A page can be pulled off the free list using the 
    ** allocateBtreePage() routine.
    */
    do{
      if( pFreeMemPage ){
        releasePage(pFreeMemPage);
        pFreeMemPage = 0;
      }
      rc = allocateBtreePage(pBt, &pFreeMemPage, &iFreePage, 0, 0);
      if( rc!=SQLITE_OK ){
        releasePage(pDbMemPage);
        goto autovacuum_out;
      }
      assert( iFreePage<=origSize );
    }while( iFreePage>finSize );
    releasePage(pFreeMemPage);
    pFreeMemPage = 0;

    /* Relocate the page into the body of the file. Note that although the 
    ** page has moved within the database file, the pDbMemPage pointer 
    ** remains valid. This means that this function can run without
    ** invalidating cursors open on the btree. This is important in 
    ** shared-cache mode.
    */
    rc = relocatePage(pBt, pDbMemPage, eType, iPtrPage, iFreePage);
    releasePage(pDbMemPage);
    if( rc!=SQLITE_OK ) goto autovacuum_out;
  }

  /* The entire free-list has been swapped to the end of the file. So
  ** truncate the database file to finSize pages and consider the
  ** free-list empty.
  */
  rc = sqlite3PagerWrite(pBt->pPage1->pDbPage);
  if( rc!=SQLITE_OK ) goto autovacuum_out;
  put4byte(&pBt->pPage1->aData[32], 0);
  put4byte(&pBt->pPage1->aData[36], 0);
  *pnTrunc = finSize;
  assert( finSize!=PENDING_BYTE_PAGE(pBt) );

autovacuum_out:
  assert( nRef==sqlite3PagerRefcount(pPager) );
  if( rc!=SQLITE_OK ){
    sqlite3PagerRollback(pPager);
  }
  return rc;
}
#endif

/*
** This routine does the first phase of a two-phase commit.  This routine
** causes a rollback journal to be created (if it does not already exist)
** and populated with enough information so that if a power loss occurs
** the database can be restored to its original state by playing back
** the journal.  Then the contents of the journal are flushed out to
** the disk.  After the journal is safely on oxide, the changes to the
** database are written into the database file and flushed to oxide.
** At the end of this call, the rollback journal still exists on the
** disk and we are still holding all locks, so the transaction has not
** committed.  See sqlite3BtreeCommit() for the second phase of the
** commit process.
**
** This call is a no-op if no write-transaction is currently active on pBt.
**
** Otherwise, sync the database file for the btree pBt. zMaster points to
** the name of a master journal file that should be written into the
** individual journal file, or is NULL, indicating no master journal file 
** (single database transaction).
**
** When this is called, the master journal should already have been
** created, populated with this journal pointer and synced to disk.
**
** Once this is routine has returned, the only thing required to commit
** the write-transaction for this database file is to delete the journal.
*/
int sqlite3BtreeCommitPhaseOne(Btree *p, const char *zMaster){
  int rc = SQLITE_OK;
  if( p->inTrans==TRANS_WRITE ){
    BtShared *pBt = p->pBt;
    Pgno nTrunc = 0;
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pBt->autoVacuum ){
      rc = autoVacuumCommit(pBt, &nTrunc); 
      if( rc!=SQLITE_OK ){
        return rc;
      }
    }
#endif
    rc = sqlite3PagerCommitPhaseOne(pBt->pPager, zMaster, nTrunc);
  }
  return rc;
}

/*
** Commit the transaction currently in progress.
**
** This routine implements the second phase of a 2-phase commit.  The
** sqlite3BtreeSync() routine does the first phase and should be invoked
** prior to calling this routine.  The sqlite3BtreeSync() routine did
** all the work of writing information out to disk and flushing the
** contents so that they are written onto the disk platter.  All this
** routine has to do is delete or truncate the rollback journal
** (which causes the transaction to commit) and drop locks.
**
** This will release the write lock on the database file.  If there
** are no active cursors, it also releases the read lock.
*/
int sqlite3BtreeCommitPhaseTwo(Btree *p){
  BtShared *pBt = p->pBt;

  btreeIntegrity(p);

  /* If the handle has a write-transaction open, commit the shared-btrees 
  ** transaction and set the shared state to TRANS_READ.
  */
  if( p->inTrans==TRANS_WRITE ){
    int rc;
    assert( pBt->inTransaction==TRANS_WRITE );
    assert( pBt->nTransaction>0 );
    rc = sqlite3PagerCommitPhaseTwo(pBt->pPager);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    pBt->inTransaction = TRANS_READ;
    pBt->inStmt = 0;
  }
  unlockAllTables(p);

  /* If the handle has any kind of transaction open, decrement the transaction
  ** count of the shared btree. If the transaction count reaches 0, set
  ** the shared state to TRANS_NONE. The unlockBtreeIfUnused() call below
  ** will unlock the pager.
  */
  if( p->inTrans!=TRANS_NONE ){
    pBt->nTransaction--;
    if( 0==pBt->nTransaction ){
      pBt->inTransaction = TRANS_NONE;
    }
  }

  /* Set the handles current transaction state to TRANS_NONE and unlock
  ** the pager if this call closed the only read or write transaction.
  */
  p->inTrans = TRANS_NONE;
  unlockBtreeIfUnused(pBt);

  btreeIntegrity(p);
  return SQLITE_OK;
}

/*
** Do both phases of a commit.
*/
int sqlite3BtreeCommit(Btree *p){
  int rc;
  rc = sqlite3BtreeCommitPhaseOne(p, 0);
  if( rc==SQLITE_OK ){
    rc = sqlite3BtreeCommitPhaseTwo(p);
  }
  return rc;
}

#ifndef NDEBUG
/*
** Return the number of write-cursors open on this handle. This is for use
** in assert() expressions, so it is only compiled if NDEBUG is not
** defined.
*/
static int countWriteCursors(BtShared *pBt){
  BtCursor *pCur;
  int r = 0;
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    if( pCur->wrFlag ) r++; 
  }
  return r;
}
#endif

#if defined(SQLITE_TEST) || defined(SQLITE_DEBUG)
/*
** Print debugging information about all cursors to standard output.
*/
void sqlite3BtreeCursorList(Btree *p){
  BtCursor *pCur;
  BtShared *pBt = p->pBt;
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    MemPage *pPage = pCur->pPage;
    char *zMode = pCur->wrFlag ? "rw" : "ro";
    sqlite3DebugPrintf("CURSOR %p rooted at %4d(%s) currently at %d.%d%s\n",
       pCur, pCur->pgnoRoot, zMode,
       pPage ? pPage->pgno : 0, pCur->idx,
       (pCur->eState==CURSOR_VALID) ? "" : " eof"
    );
  }
}
#endif

/*
** Rollback the transaction in progress.  All cursors will be
** invalided by this operation.  Any attempt to use a cursor
** that was open at the beginning of this operation will result
** in an error.
**
** This will release the write lock on the database file.  If there
** are no active cursors, it also releases the read lock.
*/
int sqlite3BtreeRollback(Btree *p){
  int rc;
  BtShared *pBt = p->pBt;
  MemPage *pPage1;

  rc = saveAllCursors(pBt, 0, 0);
#ifndef SQLITE_OMIT_SHARED_CACHE
  if( rc!=SQLITE_OK ){
    /* This is a horrible situation. An IO or malloc() error occured whilst
    ** trying to save cursor positions. If this is an automatic rollback (as
    ** the result of a constraint, malloc() failure or IO error) then 
    ** the cache may be internally inconsistent (not contain valid trees) so
    ** we cannot simply return the error to the caller. Instead, abort 
    ** all queries that may be using any of the cursors that failed to save.
    */
    while( pBt->pCursor ){
      sqlite3 *db = pBt->pCursor->pBtree->pSqlite;
      if( db ){
        sqlite3AbortOtherActiveVdbes(db, 0);
      }
    }
  }
#endif
  btreeIntegrity(p);
  unlockAllTables(p);

  if( p->inTrans==TRANS_WRITE ){
    int rc2;

    assert( TRANS_WRITE==pBt->inTransaction );
    rc2 = sqlite3PagerRollback(pBt->pPager);
    if( rc2!=SQLITE_OK ){
      rc = rc2;
    }

    /* The rollback may have destroyed the pPage1->aData value.  So
    ** call getPage() on page 1 again to make sure pPage1->aData is
    ** set correctly. */
    if( getPage(pBt, 1, &pPage1, 0)==SQLITE_OK ){
      releasePage(pPage1);
    }
    assert( countWriteCursors(pBt)==0 );
    pBt->inTransaction = TRANS_READ;
  }

  if( p->inTrans!=TRANS_NONE ){
    assert( pBt->nTransaction>0 );
    pBt->nTransaction--;
    if( 0==pBt->nTransaction ){
      pBt->inTransaction = TRANS_NONE;
    }
  }

  p->inTrans = TRANS_NONE;
  pBt->inStmt = 0;
  unlockBtreeIfUnused(pBt);

  btreeIntegrity(p);
  return rc;
}

/*
** Start a statement subtransaction.  The subtransaction can
** can be rolled back independently of the main transaction.
** You must start a transaction before starting a subtransaction.
** The subtransaction is ended automatically if the main transaction
** commits or rolls back.
**
** Only one subtransaction may be active at a time.  It is an error to try
** to start a new subtransaction if another subtransaction is already active.
**
** Statement subtransactions are used around individual SQL statements
** that are contained within a BEGIN...COMMIT block.  If a constraint
** error occurs within the statement, the effect of that one statement
** can be rolled back without having to rollback the entire transaction.
*/
int sqlite3BtreeBeginStmt(Btree *p){
  int rc;
  BtShared *pBt = p->pBt;
  if( (p->inTrans!=TRANS_WRITE) || pBt->inStmt ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( pBt->inTransaction==TRANS_WRITE );
  rc = pBt->readOnly ? SQLITE_OK : sqlite3PagerStmtBegin(pBt->pPager);
  pBt->inStmt = 1;
  return rc;
}


/*
** Commit the statment subtransaction currently in progress.  If no
** subtransaction is active, this is a no-op.
*/
int sqlite3BtreeCommitStmt(Btree *p){
  int rc;
  BtShared *pBt = p->pBt;
  if( pBt->inStmt && !pBt->readOnly ){
    rc = sqlite3PagerStmtCommit(pBt->pPager);
  }else{
    rc = SQLITE_OK;
  }
  pBt->inStmt = 0;
  return rc;
}

/*
** Rollback the active statement subtransaction.  If no subtransaction
** is active this routine is a no-op.
**
** All cursors will be invalidated by this operation.  Any attempt
** to use a cursor that was open at the beginning of this operation
** will result in an error.
*/
int sqlite3BtreeRollbackStmt(Btree *p){
  int rc = SQLITE_OK;
  BtShared *pBt = p->pBt;
  sqlite3MallocDisallow();
  if( pBt->inStmt && !pBt->readOnly ){
    rc = sqlite3PagerStmtRollback(pBt->pPager);
    assert( countWriteCursors(pBt)==0 );
    pBt->inStmt = 0;
  }
  sqlite3MallocAllow();
  return rc;
}

/*
** Default key comparison function to be used if no comparison function
** is specified on the sqlite3BtreeCursor() call.
*/
static int dfltCompare(
  void *NotUsed,             /* User data is not used */
  int n1, const void *p1,    /* First key to compare */
  int n2, const void *p2     /* Second key to compare */
){
  int c;
  c = memcmp(p1, p2, n1<n2 ? n1 : n2);
  if( c==0 ){
    c = n1 - n2;
  }
  return c;
}

/*
** Create a new cursor for the BTree whose root is on the page
** iTable.  The act of acquiring a cursor gets a read lock on 
** the database file.
**
** If wrFlag==0, then the cursor can only be used for reading.
** If wrFlag==1, then the cursor can be used for reading or for
** writing if other conditions for writing are also met.  These
** are the conditions that must be met in order for writing to
** be allowed:
**
** 1:  The cursor must have been opened with wrFlag==1
**
** 2:  Other database connections that share the same pager cache
**     but which are not in the READ_UNCOMMITTED state may not have
**     cursors open with wrFlag==0 on the same table.  Otherwise
**     the changes made by this write cursor would be visible to
**     the read cursors in the other database connection.
**
** 3:  The database must be writable (not on read-only media)
**
** 4:  There must be an active transaction.
**
** No checking is done to make sure that page iTable really is the
** root page of a b-tree.  If it is not, then the cursor acquired
** will not work correctly.
**
** The comparison function must be logically the same for every cursor
** on a particular table.  Changing the comparison function will result
** in incorrect operations.  If the comparison function is NULL, a
** default comparison function is used.  The comparison function is
** always ignored for INTKEY tables.
*/
int sqlite3BtreeCursor(
  Btree *p,                                   /* The btree */
  int iTable,                                 /* Root page of table to open */
  int wrFlag,                                 /* 1 to write. 0 read-only */
  int (*xCmp)(void*,int,const void*,int,const void*), /* Key Comparison func */
  void *pArg,                                 /* First arg to xCompare() */
  BtCursor **ppCur                            /* Write new cursor here */
){
  int rc;
  BtCursor *pCur;
  BtShared *pBt = p->pBt;

  *ppCur = 0;
  if( wrFlag ){
    if( pBt->readOnly ){
      return SQLITE_READONLY;
    }
    if( checkReadLocks(p, iTable, 0) ){
      return SQLITE_LOCKED;
    }
  }

  if( pBt->pPage1==0 ){
    rc = lockBtreeWithRetry(p);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    if( pBt->readOnly && wrFlag ){
      return SQLITE_READONLY;
    }
  }
  pCur = sqliteMalloc( sizeof(*pCur) );
  if( pCur==0 ){
    rc = SQLITE_NOMEM;
    goto create_cursor_exception;
  }
  pCur->pgnoRoot = (Pgno)iTable;
  if( iTable==1 && sqlite3PagerPagecount(pBt->pPager)==0 ){
    rc = SQLITE_EMPTY;
    goto create_cursor_exception;
  }
  rc = getAndInitPage(pBt, pCur->pgnoRoot, &pCur->pPage, 0);
  if( rc!=SQLITE_OK ){
    goto create_cursor_exception;
  }

  /* Now that no other errors can occur, finish filling in the BtCursor
  ** variables, link the cursor into the BtShared list and set *ppCur (the
  ** output argument to this function).
  */
  pCur->xCompare = xCmp ? xCmp : dfltCompare;
  pCur->pArg = pArg;
  pCur->pBtree = p;
  pCur->wrFlag = wrFlag;
  pCur->pNext = pBt->pCursor;
  if( pCur->pNext ){
    pCur->pNext->pPrev = pCur;
  }
  pBt->pCursor = pCur;
  pCur->eState = CURSOR_INVALID;
  *ppCur = pCur;

  return SQLITE_OK;
create_cursor_exception:
  if( pCur ){
    releasePage(pCur->pPage);
    sqliteFree(pCur);
  }
  unlockBtreeIfUnused(pBt);
  return rc;
}

#if 0  /* Not Used */
/*
** Change the value of the comparison function used by a cursor.
*/
void sqlite3BtreeSetCompare(
  BtCursor *pCur,     /* The cursor to whose comparison function is changed */
  int(*xCmp)(void*,int,const void*,int,const void*), /* New comparison func */
  void *pArg          /* First argument to xCmp() */
){
  pCur->xCompare = xCmp ? xCmp : dfltCompare;
  pCur->pArg = pArg;
}
#endif

/*
** Close a cursor.  The read lock on the database file is released
** when the last cursor is closed.
*/
int sqlite3BtreeCloseCursor(BtCursor *pCur){
  BtShared *pBt = pCur->pBtree->pBt;
  clearCursorPosition(pCur);
  if( pCur->pPrev ){
    pCur->pPrev->pNext = pCur->pNext;
  }else{
    pBt->pCursor = pCur->pNext;
  }
  if( pCur->pNext ){
    pCur->pNext->pPrev = pCur->pPrev;
  }
  releasePage(pCur->pPage);
  unlockBtreeIfUnused(pBt);
  sqliteFree(pCur);
  return SQLITE_OK;
}

/*
** Make a temporary cursor by filling in the fields of pTempCur.
** The temporary cursor is not on the cursor list for the Btree.
*/
static void getTempCursor(BtCursor *pCur, BtCursor *pTempCur){
  memcpy(pTempCur, pCur, sizeof(*pCur));
  pTempCur->pNext = 0;
  pTempCur->pPrev = 0;
  if( pTempCur->pPage ){
    sqlite3PagerRef(pTempCur->pPage->pDbPage);
  }
}

/*
** Delete a temporary cursor such as was made by the CreateTemporaryCursor()
** function above.
*/
static void releaseTempCursor(BtCursor *pCur){
  if( pCur->pPage ){
    sqlite3PagerUnref(pCur->pPage->pDbPage);
  }
}

/*
** Make sure the BtCursor.info field of the given cursor is valid.
** If it is not already valid, call parseCell() to fill it in.
**
** BtCursor.info is a cache of the information in the current cell.
** Using this cache reduces the number of calls to parseCell().
*/
static void getCellInfo(BtCursor *pCur){
  if( pCur->info.nSize==0 ){
    parseCell(pCur->pPage, pCur->idx, &pCur->info);
  }else{
#ifndef NDEBUG
    CellInfo info;
    memset(&info, 0, sizeof(info));
    parseCell(pCur->pPage, pCur->idx, &info);
    assert( memcmp(&info, &pCur->info, sizeof(info))==0 );
#endif
  }
}

/*
** Set *pSize to the size of the buffer needed to hold the value of
** the key for the current entry.  If the cursor is not pointing
** to a valid entry, *pSize is set to 0. 
**
** For a table with the INTKEY flag set, this routine returns the key
** itself, not the number of bytes in the key.
*/
int sqlite3BtreeKeySize(BtCursor *pCur, i64 *pSize){
  int rc = restoreOrClearCursorPosition(pCur);
  if( rc==SQLITE_OK ){
    assert( pCur->eState==CURSOR_INVALID || pCur->eState==CURSOR_VALID );
    if( pCur->eState==CURSOR_INVALID ){
      *pSize = 0;
    }else{
      getCellInfo(pCur);
      *pSize = pCur->info.nKey;
    }
  }
  return rc;
}

/*
** Set *pSize to the number of bytes of data in the entry the
** cursor currently points to.  Always return SQLITE_OK.
** Failure is not possible.  If the cursor is not currently
** pointing to an entry (which can happen, for example, if
** the database is empty) then *pSize is set to 0.
*/
int sqlite3BtreeDataSize(BtCursor *pCur, u32 *pSize){
  int rc = restoreOrClearCursorPosition(pCur);
  if( rc==SQLITE_OK ){
    assert( pCur->eState==CURSOR_INVALID || pCur->eState==CURSOR_VALID );
    if( pCur->eState==CURSOR_INVALID ){
      /* Not pointing at a valid entry - set *pSize to 0. */
      *pSize = 0;
    }else{
      getCellInfo(pCur);
      *pSize = pCur->info.nData;
    }
  }
  return rc;
}

/*
** Read payload information from the entry that the pCur cursor is
** pointing to.  Begin reading the payload at "offset" and read
** a total of "amt" bytes.  Put the result in zBuf.
**
** This routine does not make a distinction between key and data.
** It just reads bytes from the payload area.  Data might appear
** on the main page or be scattered out on multiple overflow pages.
*/
static int getPayload(
  BtCursor *pCur,      /* Cursor pointing to entry to read from */
  int offset,          /* Begin reading this far into payload */
  int amt,             /* Read this many bytes */
  unsigned char *pBuf, /* Write the bytes into this buffer */ 
  int skipKey          /* offset begins at data if this is true */
){
  unsigned char *aPayload;
  Pgno nextPage;
  int rc;
  MemPage *pPage;
  BtShared *pBt;
  int ovflSize;
  u32 nKey;

  assert( pCur!=0 && pCur->pPage!=0 );
  assert( pCur->eState==CURSOR_VALID );
  pBt = pCur->pBtree->pBt;
  pPage = pCur->pPage;
  assert( pCur->idx>=0 && pCur->idx<pPage->nCell );
  getCellInfo(pCur);
  aPayload = pCur->info.pCell + pCur->info.nHeader;
  if( pPage->intKey ){
    nKey = 0;
  }else{
    nKey = pCur->info.nKey;
  }
  assert( offset>=0 );
  if( skipKey ){
    offset += nKey;
  }
  if( offset+amt > nKey+pCur->info.nData ){
    return SQLITE_ERROR;
  }
  if( offset<pCur->info.nLocal ){
    int a = amt;
    if( a+offset>pCur->info.nLocal ){
      a = pCur->info.nLocal - offset;
    }
    memcpy(pBuf, &aPayload[offset], a);
    if( a==amt ){
      return SQLITE_OK;
    }
    offset = 0;
    pBuf += a;
    amt -= a;
  }else{
    offset -= pCur->info.nLocal;
  }
  ovflSize = pBt->usableSize - 4;
  if( amt>0 ){
    nextPage = get4byte(&aPayload[pCur->info.nLocal]);
    while( amt>0 && nextPage ){
      DbPage *pDbPage;
      rc = sqlite3PagerGet(pBt->pPager, nextPage, &pDbPage);
      if( rc!=0 ){
        return rc;
      }
      aPayload = sqlite3PagerGetData(pDbPage);
      nextPage = get4byte(aPayload);
      if( offset<ovflSize ){
        int a = amt;
        if( a + offset > ovflSize ){
          a = ovflSize - offset;
        }
        memcpy(pBuf, &aPayload[offset+4], a);
        offset = 0;
        amt -= a;
        pBuf += a;
      }else{
        offset -= ovflSize;
      }
      sqlite3PagerUnref(pDbPage);
    }
  }

  if( amt>0 ){
    return SQLITE_CORRUPT_BKPT;
  }
  return SQLITE_OK;
}

/*
** Read part of the key associated with cursor pCur.  Exactly
** "amt" bytes will be transfered into pBuf[].  The transfer
** begins at "offset".
**
** Return SQLITE_OK on success or an error code if anything goes
** wrong.  An error is returned if "offset+amt" is larger than
** the available payload.
*/
int sqlite3BtreeKey(BtCursor *pCur, u32 offset, u32 amt, void *pBuf){
  int rc = restoreOrClearCursorPosition(pCur);
  if( rc==SQLITE_OK ){
    assert( pCur->eState==CURSOR_VALID );
    assert( pCur->pPage!=0 );
    if( pCur->pPage->intKey ){
      return SQLITE_CORRUPT_BKPT;
    }
    assert( pCur->pPage->intKey==0 );
    assert( pCur->idx>=0 && pCur->idx<pCur->pPage->nCell );
    rc = getPayload(pCur, offset, amt, (unsigned char*)pBuf, 0);
  }
  return rc;
}

/*
** Read part of the data associated with cursor pCur.  Exactly
** "amt" bytes will be transfered into pBuf[].  The transfer
** begins at "offset".
**
** Return SQLITE_OK on success or an error code if anything goes
** wrong.  An error is returned if "offset+amt" is larger than
** the available payload.
*/
int sqlite3BtreeData(BtCursor *pCur, u32 offset, u32 amt, void *pBuf){
  int rc = restoreOrClearCursorPosition(pCur);
  if( rc==SQLITE_OK ){
    assert( pCur->eState==CURSOR_VALID );
    assert( pCur->pPage!=0 );
    assert( pCur->idx>=0 && pCur->idx<pCur->pPage->nCell );
    rc = getPayload(pCur, offset, amt, pBuf, 1);
  }
  return rc;
}

/*
** Return a pointer to payload information from the entry that the 
** pCur cursor is pointing to.  The pointer is to the beginning of
** the key if skipKey==0 and it points to the beginning of data if
** skipKey==1.  The number of bytes of available key/data is written
** into *pAmt.  If *pAmt==0, then the value returned will not be
** a valid pointer.
**
** This routine is an optimization.  It is common for the entire key
** and data to fit on the local page and for there to be no overflow
** pages.  When that is so, this routine can be used to access the
** key and data without making a copy.  If the key and/or data spills
** onto overflow pages, then getPayload() must be used to reassembly
** the key/data and copy it into a preallocated buffer.
**
** The pointer returned by this routine looks directly into the cached
** page of the database.  The data might change or move the next time
** any btree routine is called.
*/
static const unsigned char *fetchPayload(
  BtCursor *pCur,      /* Cursor pointing to entry to read from */
  int *pAmt,           /* Write the number of available bytes here */
  int skipKey          /* read beginning at data if this is true */
){
  unsigned char *aPayload;
  MemPage *pPage;
  u32 nKey;
  int nLocal;

  assert( pCur!=0 && pCur->pPage!=0 );
  assert( pCur->eState==CURSOR_VALID );
  pPage = pCur->pPage;
  assert( pCur->idx>=0 && pCur->idx<pPage->nCell );
  getCellInfo(pCur);
  aPayload = pCur->info.pCell;
  aPayload += pCur->info.nHeader;
  if( pPage->intKey ){
    nKey = 0;
  }else{
    nKey = pCur->info.nKey;
  }
  if( skipKey ){
    aPayload += nKey;
    nLocal = pCur->info.nLocal - nKey;
  }else{
    nLocal = pCur->info.nLocal;
    if( nLocal>nKey ){
      nLocal = nKey;
    }
  }
  *pAmt = nLocal;
  return aPayload;
}


/*
** For the entry that cursor pCur is point to, return as
** many bytes of the key or data as are available on the local
** b-tree page.  Write the number of available bytes into *pAmt.
**
** The pointer returned is ephemeral.  The key/data may move
** or be destroyed on the next call to any Btree routine.
**
** These routines is used to get quick access to key and data
** in the common case where no overflow pages are used.
*/
const void *sqlite3BtreeKeyFetch(BtCursor *pCur, int *pAmt){
  if( pCur->eState==CURSOR_VALID ){
    return (const void*)fetchPayload(pCur, pAmt, 0);
  }
  return 0;
}
const void *sqlite3BtreeDataFetch(BtCursor *pCur, int *pAmt){
  if( pCur->eState==CURSOR_VALID ){
    return (const void*)fetchPayload(pCur, pAmt, 1);
  }
  return 0;
}


/*
** Move the cursor down to a new child page.  The newPgno argument is the
** page number of the child page to move to.
*/
static int moveToChild(BtCursor *pCur, u32 newPgno){
  int rc;
  MemPage *pNewPage;
  MemPage *pOldPage;
  BtShared *pBt = pCur->pBtree->pBt;

  assert( pCur->eState==CURSOR_VALID );
  rc = getAndInitPage(pBt, newPgno, &pNewPage, pCur->pPage);
  if( rc ) return rc;
  pNewPage->idxParent = pCur->idx;
  pOldPage = pCur->pPage;
  pOldPage->idxShift = 0;
  releasePage(pOldPage);
  pCur->pPage = pNewPage;
  pCur->idx = 0;
  pCur->info.nSize = 0;
  if( pNewPage->nCell<1 ){
    return SQLITE_CORRUPT_BKPT;
  }
  return SQLITE_OK;
}

/*
** Return true if the page is the virtual root of its table.
**
** The virtual root page is the root page for most tables.  But
** for the table rooted on page 1, sometime the real root page
** is empty except for the right-pointer.  In such cases the
** virtual root page is the page that the right-pointer of page
** 1 is pointing to.
*/
static int isRootPage(MemPage *pPage){
  MemPage *pParent = pPage->pParent;
  if( pParent==0 ) return 1;
  if( pParent->pgno>1 ) return 0;
  if( get2byte(&pParent->aData[pParent->hdrOffset+3])==0 ) return 1;
  return 0;
}

/*
** Move the cursor up to the parent page.
**
** pCur->idx is set to the cell index that contains the pointer
** to the page we are coming from.  If we are coming from the
** right-most child page then pCur->idx is set to one more than
** the largest cell index.
*/
static void moveToParent(BtCursor *pCur){
  MemPage *pParent;
  MemPage *pPage;
  int idxParent;

  assert( pCur->eState==CURSOR_VALID );
  pPage = pCur->pPage;
  assert( pPage!=0 );
  assert( !isRootPage(pPage) );
  pParent = pPage->pParent;
  assert( pParent!=0 );
  idxParent = pPage->idxParent;
  sqlite3PagerRef(pParent->pDbPage);
  releasePage(pPage);
  pCur->pPage = pParent;
  pCur->info.nSize = 0;
  assert( pParent->idxShift==0 );
  pCur->idx = idxParent;
}

/*
** Move the cursor to the root page
*/
static int moveToRoot(BtCursor *pCur){
  MemPage *pRoot;
  int rc = SQLITE_OK;
  BtShared *pBt = pCur->pBtree->pBt;

  if( pCur->eState==CURSOR_REQUIRESEEK ){
    clearCursorPosition(pCur);
  }
  pRoot = pCur->pPage;
  if( pRoot && pRoot->pgno==pCur->pgnoRoot ){
    assert( pRoot->isInit );
  }else{
    if( 
      SQLITE_OK!=(rc = getAndInitPage(pBt, pCur->pgnoRoot, &pRoot, 0))
    ){
      pCur->eState = CURSOR_INVALID;
      return rc;
    }
    releasePage(pCur->pPage);
    pCur->pPage = pRoot;
  }
  pCur->idx = 0;
  pCur->info.nSize = 0;
  if( pRoot->nCell==0 && !pRoot->leaf ){
    Pgno subpage;
    assert( pRoot->pgno==1 );
    subpage = get4byte(&pRoot->aData[pRoot->hdrOffset+8]);
    assert( subpage>0 );
    pCur->eState = CURSOR_VALID;
    rc = moveToChild(pCur, subpage);
  }
  pCur->eState = ((pCur->pPage->nCell>0)?CURSOR_VALID:CURSOR_INVALID);
  return rc;
}

/*
** Move the cursor down to the left-most leaf entry beneath the
** entry to which it is currently pointing.
**
** The left-most leaf is the one with the smallest key - the first
** in ascending order.
*/
static int moveToLeftmost(BtCursor *pCur){
  Pgno pgno;
  int rc;
  MemPage *pPage;

  assert( pCur->eState==CURSOR_VALID );
  while( !(pPage = pCur->pPage)->leaf ){
    assert( pCur->idx>=0 && pCur->idx<pPage->nCell );
    pgno = get4byte(findCell(pPage, pCur->idx));
    rc = moveToChild(pCur, pgno);
    if( rc ) return rc;
  }
  return SQLITE_OK;
}

/*
** Move the cursor down to the right-most leaf entry beneath the
** page to which it is currently pointing.  Notice the difference
** between moveToLeftmost() and moveToRightmost().  moveToLeftmost()
** finds the left-most entry beneath the *entry* whereas moveToRightmost()
** finds the right-most entry beneath the *page*.
**
** The right-most entry is the one with the largest key - the last
** key in ascending order.
*/
static int moveToRightmost(BtCursor *pCur){
  Pgno pgno;
  int rc;
  MemPage *pPage;

  assert( pCur->eState==CURSOR_VALID );
  while( !(pPage = pCur->pPage)->leaf ){
    pgno = get4byte(&pPage->aData[pPage->hdrOffset+8]);
    pCur->idx = pPage->nCell;
    rc = moveToChild(pCur, pgno);
    if( rc ) return rc;
  }
  pCur->idx = pPage->nCell - 1;
  pCur->info.nSize = 0;
  return SQLITE_OK;
}

/* Move the cursor to the first entry in the table.  Return SQLITE_OK
** on success.  Set *pRes to 0 if the cursor actually points to something
** or set *pRes to 1 if the table is empty.
*/
int sqlite3BtreeFirst(BtCursor *pCur, int *pRes){
  int rc;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  if( pCur->eState==CURSOR_INVALID ){
    assert( pCur->pPage->nCell==0 );
    *pRes = 1;
    return SQLITE_OK;
  }
  assert( pCur->pPage->nCell>0 );
  *pRes = 0;
  rc = moveToLeftmost(pCur);
  return rc;
}

/* Move the cursor to the last entry in the table.  Return SQLITE_OK
** on success.  Set *pRes to 0 if the cursor actually points to something
** or set *pRes to 1 if the table is empty.
*/
int sqlite3BtreeLast(BtCursor *pCur, int *pRes){
  int rc;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  if( CURSOR_INVALID==pCur->eState ){
    assert( pCur->pPage->nCell==0 );
    *pRes = 1;
    return SQLITE_OK;
  }
  assert( pCur->eState==CURSOR_VALID );
  *pRes = 0;
  rc = moveToRightmost(pCur);
  return rc;
}

/* Move the cursor so that it points to an entry near pKey/nKey.
** Return a success code.
**
** For INTKEY tables, only the nKey parameter is used.  pKey is
** ignored.  For other tables, nKey is the number of bytes of data
** in pKey.  The comparison function specified when the cursor was
** created is used to compare keys.
**
** If an exact match is not found, then the cursor is always
** left pointing at a leaf page which would hold the entry if it
** were present.  The cursor might point to an entry that comes
** before or after the key.
**
** The result of comparing the key with the entry to which the
** cursor is written to *pRes if pRes!=NULL.  The meaning of
** this value is as follows:
**
**     *pRes<0      The cursor is left pointing at an entry that
**                  is smaller than pKey or if the table is empty
**                  and the cursor is therefore left point to nothing.
**
**     *pRes==0     The cursor is left pointing at an entry that
**                  exactly matches pKey.
**
**     *pRes>0      The cursor is left pointing at an entry that
**                  is larger than pKey.
*/
int sqlite3BtreeMoveto(
  BtCursor *pCur,        /* The cursor to be moved */
  const void *pKey,      /* The key content for indices.  Not used by tables */
  i64 nKey,              /* Size of pKey.  Or the key for tables */
  int biasRight,         /* If true, bias the search to the high end */
  int *pRes              /* Search result flag */
){
  int rc;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  assert( pCur->pPage );
  assert( pCur->pPage->isInit );
  if( pCur->eState==CURSOR_INVALID ){
    *pRes = -1;
    assert( pCur->pPage->nCell==0 );
    return SQLITE_OK;
  }
  for(;;){
    int lwr, upr;
    Pgno chldPg;
    MemPage *pPage = pCur->pPage;
    int c = -1;  /* pRes return if table is empty must be -1 */
    lwr = 0;
    upr = pPage->nCell-1;
    if( !pPage->intKey && pKey==0 ){
      return SQLITE_CORRUPT_BKPT;
    }
    if( biasRight ){
      pCur->idx = upr;
    }else{
      pCur->idx = (upr+lwr)/2;
    }
    if( lwr<=upr ) for(;;){
      void *pCellKey;
      i64 nCellKey;
      pCur->info.nSize = 0;
      if( pPage->intKey ){
        u8 *pCell;
        pCell = findCell(pPage, pCur->idx) + pPage->childPtrSize;
        if( pPage->hasData ){
          u32 dummy;
          pCell += getVarint32(pCell, &dummy);
        }
        getVarint(pCell, (u64 *)&nCellKey);
        if( nCellKey<nKey ){
          c = -1;
        }else if( nCellKey>nKey ){
          c = +1;
        }else{
          c = 0;
        }
      }else{
        int available;
        pCellKey = (void *)fetchPayload(pCur, &available, 0);
        nCellKey = pCur->info.nKey;
        if( available>=nCellKey ){
          c = pCur->xCompare(pCur->pArg, nCellKey, pCellKey, nKey, pKey);
        }else{
          pCellKey = sqliteMallocRaw( nCellKey );
          if( pCellKey==0 ) return SQLITE_NOMEM;
          rc = sqlite3BtreeKey(pCur, 0, nCellKey, (void *)pCellKey);
          c = pCur->xCompare(pCur->pArg, nCellKey, pCellKey, nKey, pKey);
          sqliteFree(pCellKey);
          if( rc ) return rc;
        }
      }
      if( c==0 ){
        if( pPage->leafData && !pPage->leaf ){
          lwr = pCur->idx;
          upr = lwr - 1;
          break;
        }else{
          if( pRes ) *pRes = 0;
          return SQLITE_OK;
        }
      }
      if( c<0 ){
        lwr = pCur->idx+1;
      }else{
        upr = pCur->idx-1;
      }
      if( lwr>upr ){
        break;
      }
      pCur->idx = (lwr+upr)/2;
    }
    assert( lwr==upr+1 );
    assert( pPage->isInit );
    if( pPage->leaf ){
      chldPg = 0;
    }else if( lwr>=pPage->nCell ){
      chldPg = get4byte(&pPage->aData[pPage->hdrOffset+8]);
    }else{
      chldPg = get4byte(findCell(pPage, lwr));
    }
    if( chldPg==0 ){
      assert( pCur->idx>=0 && pCur->idx<pCur->pPage->nCell );
      if( pRes ) *pRes = c;
      return SQLITE_OK;
    }
    pCur->idx = lwr;
    pCur->info.nSize = 0;
    rc = moveToChild(pCur, chldPg);
    if( rc ){
      return rc;
    }
  }
  /* NOT REACHED */
}

/*
** Return TRUE if the cursor is not pointing at an entry of the table.
**
** TRUE will be returned after a call to sqlite3BtreeNext() moves
** past the last entry in the table or sqlite3BtreePrev() moves past
** the first entry.  TRUE is also returned if the table is empty.
*/
int sqlite3BtreeEof(BtCursor *pCur){
  /* TODO: What if the cursor is in CURSOR_REQUIRESEEK but all table entries
  ** have been deleted? This API will need to change to return an error code
  ** as well as the boolean result value.
  */
  return (CURSOR_VALID!=pCur->eState);
}

/*
** Advance the cursor to the next entry in the database.  If
** successful then set *pRes=0.  If the cursor
** was already pointing to the last entry in the database before
** this routine was called, then set *pRes=1.
*/
int sqlite3BtreeNext(BtCursor *pCur, int *pRes){
  int rc;
  MemPage *pPage;

  rc = restoreOrClearCursorPosition(pCur);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  assert( pRes!=0 );
  pPage = pCur->pPage;
  if( CURSOR_INVALID==pCur->eState ){
    *pRes = 1;
    return SQLITE_OK;
  }
  if( pCur->skip>0 ){
    pCur->skip = 0;
    *pRes = 0;
    return SQLITE_OK;
  }
  pCur->skip = 0;

  assert( pPage->isInit );
  assert( pCur->idx<pPage->nCell );

  pCur->idx++;
  pCur->info.nSize = 0;
  if( pCur->idx>=pPage->nCell ){
    if( !pPage->leaf ){
      rc = moveToChild(pCur, get4byte(&pPage->aData[pPage->hdrOffset+8]));
      if( rc ) return rc;
      rc = moveToLeftmost(pCur);
      *pRes = 0;
      return rc;
    }
    do{
      if( isRootPage(pPage) ){
        *pRes = 1;
        pCur->eState = CURSOR_INVALID;
        return SQLITE_OK;
      }
      moveToParent(pCur);
      pPage = pCur->pPage;
    }while( pCur->idx>=pPage->nCell );
    *pRes = 0;
    if( pPage->leafData ){
      rc = sqlite3BtreeNext(pCur, pRes);
    }else{
      rc = SQLITE_OK;
    }
    return rc;
  }
  *pRes = 0;
  if( pPage->leaf ){
    return SQLITE_OK;
  }
  rc = moveToLeftmost(pCur);
  return rc;
}

/*
** Step the cursor to the back to the previous entry in the database.  If
** successful then set *pRes=0.  If the cursor
** was already pointing to the first entry in the database before
** this routine was called, then set *pRes=1.
*/
int sqlite3BtreePrevious(BtCursor *pCur, int *pRes){
  int rc;
  Pgno pgno;
  MemPage *pPage;

  rc = restoreOrClearCursorPosition(pCur);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  if( CURSOR_INVALID==pCur->eState ){
    *pRes = 1;
    return SQLITE_OK;
  }
  if( pCur->skip<0 ){
    pCur->skip = 0;
    *pRes = 0;
    return SQLITE_OK;
  }
  pCur->skip = 0;

  pPage = pCur->pPage;
  assert( pPage->isInit );
  assert( pCur->idx>=0 );
  if( !pPage->leaf ){
    pgno = get4byte( findCell(pPage, pCur->idx) );
    rc = moveToChild(pCur, pgno);
    if( rc ) return rc;
    rc = moveToRightmost(pCur);
  }else{
    while( pCur->idx==0 ){
      if( isRootPage(pPage) ){
        pCur->eState = CURSOR_INVALID;
        *pRes = 1;
        return SQLITE_OK;
      }
      moveToParent(pCur);
      pPage = pCur->pPage;
    }
    pCur->idx--;
    pCur->info.nSize = 0;
    if( pPage->leafData && !pPage->leaf ){
      rc = sqlite3BtreePrevious(pCur, pRes);
    }else{
      rc = SQLITE_OK;
    }
  }
  *pRes = 0;
  return rc;
}

/*
** Allocate a new page from the database file.
**
** The new page is marked as dirty.  (In other words, sqlite3PagerWrite()
** has already been called on the new page.)  The new page has also
** been referenced and the calling routine is responsible for calling
** sqlite3PagerUnref() on the new page when it is done.
**
** SQLITE_OK is returned on success.  Any other return value indicates
** an error.  *ppPage and *pPgno are undefined in the event of an error.
** Do not invoke sqlite3PagerUnref() on *ppPage if an error is returned.
**
** If the "nearby" parameter is not 0, then a (feeble) effort is made to 
** locate a page close to the page number "nearby".  This can be used in an
** attempt to keep related pages close to each other in the database file,
** which in turn can make database access faster.
**
** If the "exact" parameter is not 0, and the page-number nearby exists 
** anywhere on the free-list, then it is guarenteed to be returned. This
** is only used by auto-vacuum databases when allocating a new table.
*/
static int allocateBtreePage(
  BtShared *pBt, 
  MemPage **ppPage, 
  Pgno *pPgno, 
  Pgno nearby,
  u8 exact
){
  MemPage *pPage1;
  int rc;
  int n;     /* Number of pages on the freelist */
  int k;     /* Number of leaves on the trunk of the freelist */
  MemPage *pTrunk = 0;
  MemPage *pPrevTrunk = 0;

  pPage1 = pBt->pPage1;
  n = get4byte(&pPage1->aData[36]);
  if( n>0 ){
    /* There are pages on the freelist.  Reuse one of those pages. */
    Pgno iTrunk;
    u8 searchList = 0; /* If the free-list must be searched for 'nearby' */
    
    /* If the 'exact' parameter was true and a query of the pointer-map
    ** shows that the page 'nearby' is somewhere on the free-list, then
    ** the entire-list will be searched for that page.
    */
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( exact ){
      u8 eType;
      assert( nearby>0 );
      assert( pBt->autoVacuum );
      rc = ptrmapGet(pBt, nearby, &eType, 0);
      if( rc ) return rc;
      if( eType==PTRMAP_FREEPAGE ){
        searchList = 1;
      }
      *pPgno = nearby;
    }
#endif

    /* Decrement the free-list count by 1. Set iTrunk to the index of the
    ** first free-list trunk page. iPrevTrunk is initially 1.
    */
    rc = sqlite3PagerWrite(pPage1->pDbPage);
    if( rc ) return rc;
    put4byte(&pPage1->aData[36], n-1);

    /* The code within this loop is run only once if the 'searchList' variable
    ** is not true. Otherwise, it runs once for each trunk-page on the
    ** free-list until the page 'nearby' is located.
    */
    do {
      pPrevTrunk = pTrunk;
      if( pPrevTrunk ){
        iTrunk = get4byte(&pPrevTrunk->aData[0]);
      }else{
        iTrunk = get4byte(&pPage1->aData[32]);
      }
      rc = getPage(pBt, iTrunk, &pTrunk, 0);
      if( rc ){
        pTrunk = 0;
        goto end_allocate_page;
      }

      k = get4byte(&pTrunk->aData[4]);
      if( k==0 && !searchList ){
        /* The trunk has no leaves and the list is not being searched. 
        ** So extract the trunk page itself and use it as the newly 
        ** allocated page */
        assert( pPrevTrunk==0 );
        rc = sqlite3PagerWrite(pTrunk->pDbPage);
        if( rc ){
          goto end_allocate_page;
        }
        *pPgno = iTrunk;
        memcpy(&pPage1->aData[32], &pTrunk->aData[0], 4);
        *ppPage = pTrunk;
        pTrunk = 0;
        TRACE(("ALLOCATE: %d trunk - %d free pages left\n", *pPgno, n-1));
      }else if( k>pBt->usableSize/4 - 8 ){
        /* Value of k is out of range.  Database corruption */
        rc = SQLITE_CORRUPT_BKPT;
        goto end_allocate_page;
#ifndef SQLITE_OMIT_AUTOVACUUM
      }else if( searchList && nearby==iTrunk ){
        /* The list is being searched and this trunk page is the page
        ** to allocate, regardless of whether it has leaves.
        */
        assert( *pPgno==iTrunk );
        *ppPage = pTrunk;
        searchList = 0;
        rc = sqlite3PagerWrite(pTrunk->pDbPage);
        if( rc ){
          goto end_allocate_page;
        }
        if( k==0 ){
          if( !pPrevTrunk ){
            memcpy(&pPage1->aData[32], &pTrunk->aData[0], 4);
          }else{
            memcpy(&pPrevTrunk->aData[0], &pTrunk->aData[0], 4);
          }
        }else{
          /* The trunk page is required by the caller but it contains 
          ** pointers to free-list leaves. The first leaf becomes a trunk
          ** page in this case.
          */
          MemPage *pNewTrunk;
          Pgno iNewTrunk = get4byte(&pTrunk->aData[8]);
          rc = getPage(pBt, iNewTrunk, &pNewTrunk, 0);
          if( rc!=SQLITE_OK ){
            goto end_allocate_page;
          }
          rc = sqlite3PagerWrite(pNewTrunk->pDbPage);
          if( rc!=SQLITE_OK ){
            releasePage(pNewTrunk);
            goto end_allocate_page;
          }
          memcpy(&pNewTrunk->aData[0], &pTrunk->aData[0], 4);
          put4byte(&pNewTrunk->aData[4], k-1);
          memcpy(&pNewTrunk->aData[8], &pTrunk->aData[12], (k-1)*4);
          releasePage(pNewTrunk);
          if( !pPrevTrunk ){
            put4byte(&pPage1->aData[32], iNewTrunk);
          }else{
            rc = sqlite3PagerWrite(pPrevTrunk->pDbPage);
            if( rc ){
              goto end_allocate_page;
            }
            put4byte(&pPrevTrunk->aData[0], iNewTrunk);
          }
        }
        pTrunk = 0;
        TRACE(("ALLOCATE: %d trunk - %d free pages left\n", *pPgno, n-1));
#endif
      }else{
        /* Extract a leaf from the trunk */
        int closest;
        Pgno iPage;
        unsigned char *aData = pTrunk->aData;
        rc = sqlite3PagerWrite(pTrunk->pDbPage);
        if( rc ){
          goto end_allocate_page;
        }
        if( nearby>0 ){
          int i, dist;
          closest = 0;
          dist = get4byte(&aData[8]) - nearby;
          if( dist<0 ) dist = -dist;
          for(i=1; i<k; i++){
            int d2 = get4byte(&aData[8+i*4]) - nearby;
            if( d2<0 ) d2 = -d2;
            if( d2<dist ){
              closest = i;
              dist = d2;
            }
          }
        }else{
          closest = 0;
        }

        iPage = get4byte(&aData[8+closest*4]);
        if( !searchList || iPage==nearby ){
          *pPgno = iPage;
          if( *pPgno>sqlite3PagerPagecount(pBt->pPager) ){
            /* Free page off the end of the file */
            return SQLITE_CORRUPT_BKPT;
          }
          TRACE(("ALLOCATE: %d was leaf %d of %d on trunk %d"
                 ": %d more free pages\n",
                 *pPgno, closest+1, k, pTrunk->pgno, n-1));
          if( closest<k-1 ){
            memcpy(&aData[8+closest*4], &aData[4+k*4], 4);
          }
          put4byte(&aData[4], k-1);
          rc = getPage(pBt, *pPgno, ppPage, 1);
          if( rc==SQLITE_OK ){
            sqlite3PagerDontRollback((*ppPage)->pDbPage);
            rc = sqlite3PagerWrite((*ppPage)->pDbPage);
            if( rc!=SQLITE_OK ){
              releasePage(*ppPage);
            }
          }
          searchList = 0;
        }
      }
      releasePage(pPrevTrunk);
      pPrevTrunk = 0;
    }while( searchList );
  }else{
    /* There are no pages on the freelist, so create a new page at the
    ** end of the file */
    *pPgno = sqlite3PagerPagecount(pBt->pPager) + 1;

#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pBt->autoVacuum && PTRMAP_ISPAGE(pBt, *pPgno) ){
      /* If *pPgno refers to a pointer-map page, allocate two new pages
      ** at the end of the file instead of one. The first allocated page
      ** becomes a new pointer-map page, the second is used by the caller.
      */
      TRACE(("ALLOCATE: %d from end of file (pointer-map page)\n", *pPgno));
      assert( *pPgno!=PENDING_BYTE_PAGE(pBt) );
      (*pPgno)++;
    }
#endif

    assert( *pPgno!=PENDING_BYTE_PAGE(pBt) );
    rc = getPage(pBt, *pPgno, ppPage, 0);
    if( rc ) return rc;
    rc = sqlite3PagerWrite((*ppPage)->pDbPage);
    if( rc!=SQLITE_OK ){
      releasePage(*ppPage);
    }
    TRACE(("ALLOCATE: %d from end of file\n", *pPgno));
  }

  assert( *pPgno!=PENDING_BYTE_PAGE(pBt) );

end_allocate_page:
  releasePage(pTrunk);
  releasePage(pPrevTrunk);
  return rc;
}

/*
** Add a page of the database file to the freelist.
**
** sqlite3PagerUnref() is NOT called for pPage.
*/
static int freePage(MemPage *pPage){
  BtShared *pBt = pPage->pBt;
  MemPage *pPage1 = pBt->pPage1;
  int rc, n, k;

  /* Prepare the page for freeing */
  assert( pPage->pgno>1 );
  pPage->isInit = 0;
  releasePage(pPage->pParent);
  pPage->pParent = 0;

  /* Increment the free page count on pPage1 */
  rc = sqlite3PagerWrite(pPage1->pDbPage);
  if( rc ) return rc;
  n = get4byte(&pPage1->aData[36]);
  put4byte(&pPage1->aData[36], n+1);

#ifdef SQLITE_SECURE_DELETE
  /* If the SQLITE_SECURE_DELETE compile-time option is enabled, then
  ** always fully overwrite deleted information with zeros.
  */
  rc = sqlite3PagerWrite(pPage->pDbPage);
  if( rc ) return rc;
  memset(pPage->aData, 0, pPage->pBt->pageSize);
#endif

#ifndef SQLITE_OMIT_AUTOVACUUM
  /* If the database supports auto-vacuum, write an entry in the pointer-map
  ** to indicate that the page is free.
  */
  if( pBt->autoVacuum ){
    rc = ptrmapPut(pBt, pPage->pgno, PTRMAP_FREEPAGE, 0);
    if( rc ) return rc;
  }
#endif

  if( n==0 ){
    /* This is the first free page */
    rc = sqlite3PagerWrite(pPage->pDbPage);
    if( rc ) return rc;
    memset(pPage->aData, 0, 8);
    put4byte(&pPage1->aData[32], pPage->pgno);
    TRACE(("FREE-PAGE: %d first\n", pPage->pgno));
  }else{
    /* Other free pages already exist.  Retrive the first trunk page
    ** of the freelist and find out how many leaves it has. */
    MemPage *pTrunk;
    rc = getPage(pBt, get4byte(&pPage1->aData[32]), &pTrunk, 0);
    if( rc ) return rc;
    k = get4byte(&pTrunk->aData[4]);
    if( k>=pBt->usableSize/4 - 8 ){
      /* The trunk is full.  Turn the page being freed into a new
      ** trunk page with no leaves. */
      rc = sqlite3PagerWrite(pPage->pDbPage);
      if( rc ) return rc;
      put4byte(pPage->aData, pTrunk->pgno);
      put4byte(&pPage->aData[4], 0);
      put4byte(&pPage1->aData[32], pPage->pgno);
      TRACE(("FREE-PAGE: %d new trunk page replacing %d\n",
              pPage->pgno, pTrunk->pgno));
    }else{
      /* Add the newly freed page as a leaf on the current trunk */
      rc = sqlite3PagerWrite(pTrunk->pDbPage);
      if( rc==SQLITE_OK ){
        put4byte(&pTrunk->aData[4], k+1);
        put4byte(&pTrunk->aData[8+k*4], pPage->pgno);
#ifndef SQLITE_SECURE_DELETE
        sqlite3PagerDontWrite(pPage->pDbPage);
#endif
      }
      TRACE(("FREE-PAGE: %d leaf on trunk page %d\n",pPage->pgno,pTrunk->pgno));
    }
    releasePage(pTrunk);
  }
  return rc;
}

/*
** Free any overflow pages associated with the given Cell.
*/
static int clearCell(MemPage *pPage, unsigned char *pCell){
  BtShared *pBt = pPage->pBt;
  CellInfo info;
  Pgno ovflPgno;
  int rc;
  int nOvfl;
  int ovflPageSize;

  parseCellPtr(pPage, pCell, &info);
  if( info.iOverflow==0 ){
    return SQLITE_OK;  /* No overflow pages. Return without doing anything */
  }
  ovflPgno = get4byte(&pCell[info.iOverflow]);
  ovflPageSize = pBt->usableSize - 4;
  nOvfl = (info.nPayload - info.nLocal + ovflPageSize - 1)/ovflPageSize;
  assert( ovflPgno==0 || nOvfl>0 );
  while( nOvfl-- ){
    MemPage *pOvfl;
    if( ovflPgno==0 || ovflPgno>sqlite3PagerPagecount(pBt->pPager) ){
      return SQLITE_CORRUPT_BKPT;
    }
    rc = getPage(pBt, ovflPgno, &pOvfl, nOvfl==0);
    if( rc ) return rc;
    if( nOvfl ){
      ovflPgno = get4byte(pOvfl->aData);
    }
    rc = freePage(pOvfl);
    sqlite3PagerUnref(pOvfl->pDbPage);
    if( rc ) return rc;
  }
  return SQLITE_OK;
}

/*
** Create the byte sequence used to represent a cell on page pPage
** and write that byte sequence into pCell[].  Overflow pages are
** allocated and filled in as necessary.  The calling procedure
** is responsible for making sure sufficient space has been allocated
** for pCell[].
**
** Note that pCell does not necessary need to point to the pPage->aData
** area.  pCell might point to some temporary storage.  The cell will
** be constructed in this temporary area then copied into pPage->aData
** later.
*/
static int fillInCell(
  MemPage *pPage,                /* The page that contains the cell */
  unsigned char *pCell,          /* Complete text of the cell */
  const void *pKey, i64 nKey,    /* The key */
  const void *pData,int nData,   /* The data */
  int *pnSize                    /* Write cell size here */
){
  int nPayload;
  const u8 *pSrc;
  int nSrc, n, rc;
  int spaceLeft;
  MemPage *pOvfl = 0;
  MemPage *pToRelease = 0;
  unsigned char *pPrior;
  unsigned char *pPayload;
  BtShared *pBt = pPage->pBt;
  Pgno pgnoOvfl = 0;
  int nHeader;
  CellInfo info;

  /* Fill in the header. */
  nHeader = 0;
  if( !pPage->leaf ){
    nHeader += 4;
  }
  if( pPage->hasData ){
    nHeader += putVarint(&pCell[nHeader], nData);
  }else{
    nData = 0;
  }
  nHeader += putVarint(&pCell[nHeader], *(u64*)&nKey);
  parseCellPtr(pPage, pCell, &info);
  assert( info.nHeader==nHeader );
  assert( info.nKey==nKey );
  assert( info.nData==nData );
  
  /* Fill in the payload */
  nPayload = nData;
  if( pPage->intKey ){
    pSrc = pData;
    nSrc = nData;
    nData = 0;
  }else{
    nPayload += nKey;
    pSrc = pKey;
    nSrc = nKey;
  }
  *pnSize = info.nSize;
  spaceLeft = info.nLocal;
  pPayload = &pCell[nHeader];
  pPrior = &pCell[info.iOverflow];

  while( nPayload>0 ){
    if( spaceLeft==0 ){
#ifndef SQLITE_OMIT_AUTOVACUUM
      Pgno pgnoPtrmap = pgnoOvfl; /* Overflow page pointer-map entry page */
#endif
      rc = allocateBtreePage(pBt, &pOvfl, &pgnoOvfl, pgnoOvfl, 0);
#ifndef SQLITE_OMIT_AUTOVACUUM
      /* If the database supports auto-vacuum, and the second or subsequent
      ** overflow page is being allocated, add an entry to the pointer-map
      ** for that page now. The entry for the first overflow page will be
      ** added later, by the insertCell() routine.
      */
      if( pBt->autoVacuum && pgnoPtrmap!=0 && rc==SQLITE_OK ){
        rc = ptrmapPut(pBt, pgnoOvfl, PTRMAP_OVERFLOW2, pgnoPtrmap);
      }
#endif
      if( rc ){
        releasePage(pToRelease);
        return rc;
      }
      put4byte(pPrior, pgnoOvfl);
      releasePage(pToRelease);
      pToRelease = pOvfl;
      pPrior = pOvfl->aData;
      put4byte(pPrior, 0);
      pPayload = &pOvfl->aData[4];
      spaceLeft = pBt->usableSize - 4;
    }
    n = nPayload;
    if( n>spaceLeft ) n = spaceLeft;
    if( n>nSrc ) n = nSrc;
    assert( pSrc );
    memcpy(pPayload, pSrc, n);
    nPayload -= n;
    pPayload += n;
    pSrc += n;
    nSrc -= n;
    spaceLeft -= n;
    if( nSrc==0 ){
      nSrc = nData;
      pSrc = pData;
    }
  }
  releasePage(pToRelease);
  return SQLITE_OK;
}

/*
** Change the MemPage.pParent pointer on the page whose number is
** given in the second argument so that MemPage.pParent holds the
** pointer in the third argument.
*/
static int reparentPage(BtShared *pBt, Pgno pgno, MemPage *pNewParent, int idx){
  MemPage *pThis;
  DbPage *pDbPage;

  assert( pNewParent!=0 );
  if( pgno==0 ) return SQLITE_OK;
  assert( pBt->pPager!=0 );
  pDbPage = sqlite3PagerLookup(pBt->pPager, pgno);
  if( pDbPage ){
    pThis = (MemPage *)sqlite3PagerGetExtra(pDbPage);
    if( pThis->isInit ){
      assert( pThis->aData==(sqlite3PagerGetData(pDbPage)) );
      if( pThis->pParent!=pNewParent ){
        if( pThis->pParent ) sqlite3PagerUnref(pThis->pParent->pDbPage);
        pThis->pParent = pNewParent;
        sqlite3PagerRef(pNewParent->pDbPage);
      }
      pThis->idxParent = idx;
    }
    sqlite3PagerUnref(pDbPage);
  }

#ifndef SQLITE_OMIT_AUTOVACUUM
  if( pBt->autoVacuum ){
    return ptrmapPut(pBt, pgno, PTRMAP_BTREE, pNewParent->pgno);
  }
#endif
  return SQLITE_OK;
}



/*
** Change the pParent pointer of all children of pPage to point back
** to pPage.
**
** In other words, for every child of pPage, invoke reparentPage()
** to make sure that each child knows that pPage is its parent.
**
** This routine gets called after you memcpy() one page into
** another.
*/
static int reparentChildPages(MemPage *pPage){
  int i;
  BtShared *pBt = pPage->pBt;
  int rc = SQLITE_OK;

  if( pPage->leaf ) return SQLITE_OK;

  for(i=0; i<pPage->nCell; i++){
    u8 *pCell = findCell(pPage, i);
    if( !pPage->leaf ){
      rc = reparentPage(pBt, get4byte(pCell), pPage, i);
      if( rc!=SQLITE_OK ) return rc;
    }
  }
  if( !pPage->leaf ){
    rc = reparentPage(pBt, get4byte(&pPage->aData[pPage->hdrOffset+8]), 
       pPage, i);
    pPage->idxShift = 0;
  }
  return rc;
}

/*
** Remove the i-th cell from pPage.  This routine effects pPage only.
** The cell content is not freed or deallocated.  It is assumed that
** the cell content has been copied someplace else.  This routine just
** removes the reference to the cell from pPage.
**
** "sz" must be the number of bytes in the cell.
*/
static void dropCell(MemPage *pPage, int idx, int sz){
  int i;          /* Loop counter */
  int pc;         /* Offset to cell content of cell being deleted */
  u8 *data;       /* pPage->aData */
  u8 *ptr;        /* Used to move bytes around within data[] */

  assert( idx>=0 && idx<pPage->nCell );
  assert( sz==cellSize(pPage, idx) );
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  data = pPage->aData;
  ptr = &data[pPage->cellOffset + 2*idx];
  pc = get2byte(ptr);
  assert( pc>10 && pc+sz<=pPage->pBt->usableSize );
  freeSpace(pPage, pc, sz);
  for(i=idx+1; i<pPage->nCell; i++, ptr+=2){
    ptr[0] = ptr[2];
    ptr[1] = ptr[3];
  }
  pPage->nCell--;
  put2byte(&data[pPage->hdrOffset+3], pPage->nCell);
  pPage->nFree += 2;
  pPage->idxShift = 1;
}

/*
** Insert a new cell on pPage at cell index "i".  pCell points to the
** content of the cell.
**
** If the cell content will fit on the page, then put it there.  If it
** will not fit, then make a copy of the cell content into pTemp if
** pTemp is not null.  Regardless of pTemp, allocate a new entry
** in pPage->aOvfl[] and make it point to the cell content (either
** in pTemp or the original pCell) and also record its index. 
** Allocating a new entry in pPage->aCell[] implies that 
** pPage->nOverflow is incremented.
**
** If nSkip is non-zero, then do not copy the first nSkip bytes of the
** cell. The caller will overwrite them after this function returns. If
** nSkip is non-zero, then pCell may not point to an invalid memory location 
** (but pCell+nSkip is always valid).
*/
static int insertCell(
  MemPage *pPage,   /* Page into which we are copying */
  int i,            /* New cell becomes the i-th cell of the page */
  u8 *pCell,        /* Content of the new cell */
  int sz,           /* Bytes of content in pCell */
  u8 *pTemp,        /* Temp storage space for pCell, if needed */
  u8 nSkip          /* Do not write the first nSkip bytes of the cell */
){
  int idx;          /* Where to write new cell content in data[] */
  int j;            /* Loop counter */
  int top;          /* First byte of content for any cell in data[] */
  int end;          /* First byte past the last cell pointer in data[] */
  int ins;          /* Index in data[] where new cell pointer is inserted */
  int hdr;          /* Offset into data[] of the page header */
  int cellOffset;   /* Address of first cell pointer in data[] */
  u8 *data;         /* The content of the whole page */
  u8 *ptr;          /* Used for moving information around in data[] */

  assert( i>=0 && i<=pPage->nCell+pPage->nOverflow );
  assert( sz==cellSizePtr(pPage, pCell) );
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  if( pPage->nOverflow || sz+2>pPage->nFree ){
    if( pTemp ){
      memcpy(pTemp+nSkip, pCell+nSkip, sz-nSkip);
      pCell = pTemp;
    }
    j = pPage->nOverflow++;
    assert( j<sizeof(pPage->aOvfl)/sizeof(pPage->aOvfl[0]) );
    pPage->aOvfl[j].pCell = pCell;
    pPage->aOvfl[j].idx = i;
    pPage->nFree = 0;
  }else{
    data = pPage->aData;
    hdr = pPage->hdrOffset;
    top = get2byte(&data[hdr+5]);
    cellOffset = pPage->cellOffset;
    end = cellOffset + 2*pPage->nCell + 2;
    ins = cellOffset + 2*i;
    if( end > top - sz ){
      int rc = defragmentPage(pPage);
      if( rc!=SQLITE_OK ) return rc;
      top = get2byte(&data[hdr+5]);
      assert( end + sz <= top );
    }
    idx = allocateSpace(pPage, sz);
    assert( idx>0 );
    assert( end <= get2byte(&data[hdr+5]) );
    pPage->nCell++;
    pPage->nFree -= 2;
    memcpy(&data[idx+nSkip], pCell+nSkip, sz-nSkip);
    for(j=end-2, ptr=&data[j]; j>ins; j-=2, ptr-=2){
      ptr[0] = ptr[-2];
      ptr[1] = ptr[-1];
    }
    put2byte(&data[ins], idx);
    put2byte(&data[hdr+3], pPage->nCell);
    pPage->idxShift = 1;
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pPage->pBt->autoVacuum ){
      /* The cell may contain a pointer to an overflow page. If so, write
      ** the entry for the overflow page into the pointer map.
      */
      CellInfo info;
      parseCellPtr(pPage, pCell, &info);
      assert( (info.nData+(pPage->intKey?0:info.nKey))==info.nPayload );
      if( (info.nData+(pPage->intKey?0:info.nKey))>info.nLocal ){
        Pgno pgnoOvfl = get4byte(&pCell[info.iOverflow]);
        int rc = ptrmapPut(pPage->pBt, pgnoOvfl, PTRMAP_OVERFLOW1, pPage->pgno);
        if( rc!=SQLITE_OK ) return rc;
      }
    }
#endif
  }

  return SQLITE_OK;
}

/*
** Add a list of cells to a page.  The page should be initially empty.
** The cells are guaranteed to fit on the page.
*/
static void assemblePage(
  MemPage *pPage,   /* The page to be assemblied */
  int nCell,        /* The number of cells to add to this page */
  u8 **apCell,      /* Pointers to cell bodies */
  int *aSize        /* Sizes of the cells */
){
  int i;            /* Loop counter */
  int totalSize;    /* Total size of all cells */
  int hdr;          /* Index of page header */
  int cellptr;      /* Address of next cell pointer */
  int cellbody;     /* Address of next cell body */
  u8 *data;         /* Data for the page */

  assert( pPage->nOverflow==0 );
  totalSize = 0;
  for(i=0; i<nCell; i++){
    totalSize += aSize[i];
  }
  assert( totalSize+2*nCell<=pPage->nFree );
  assert( pPage->nCell==0 );
  cellptr = pPage->cellOffset;
  data = pPage->aData;
  hdr = pPage->hdrOffset;
  put2byte(&data[hdr+3], nCell);
  if( nCell ){
    cellbody = allocateSpace(pPage, totalSize);
    assert( cellbody>0 );
    assert( pPage->nFree >= 2*nCell );
    pPage->nFree -= 2*nCell;
    for(i=0; i<nCell; i++){
      put2byte(&data[cellptr], cellbody);
      memcpy(&data[cellbody], apCell[i], aSize[i]);
      cellptr += 2;
      cellbody += aSize[i];
    }
    assert( cellbody==pPage->pBt->usableSize );
  }
  pPage->nCell = nCell;
}

/*
** The following parameters determine how many adjacent pages get involved
** in a balancing operation.  NN is the number of neighbors on either side
** of the page that participate in the balancing operation.  NB is the
** total number of pages that participate, including the target page and
** NN neighbors on either side.
**
** The minimum value of NN is 1 (of course).  Increasing NN above 1
** (to 2 or 3) gives a modest improvement in SELECT and DELETE performance
** in exchange for a larger degradation in INSERT and UPDATE performance.
** The value of NN appears to give the best results overall.
*/
#define NN 1             /* Number of neighbors on either side of pPage */
#define NB (NN*2+1)      /* Total pages involved in the balance */

/* Forward reference */
static int balance(MemPage*, int);

#ifndef SQLITE_OMIT_QUICKBALANCE
/*
** This version of balance() handles the common special case where
** a new entry is being inserted on the extreme right-end of the
** tree, in other words, when the new entry will become the largest
** entry in the tree.
**
** Instead of trying balance the 3 right-most leaf pages, just add
** a new page to the right-hand side and put the one new entry in
** that page.  This leaves the right side of the tree somewhat
** unbalanced.  But odds are that we will be inserting new entries
** at the end soon afterwards so the nearly empty page will quickly
** fill up.  On average.
**
** pPage is the leaf page which is the right-most page in the tree.
** pParent is its parent.  pPage must have a single overflow entry
** which is also the right-most entry on the page.
*/
static int balance_quick(MemPage *pPage, MemPage *pParent){
  int rc;
  MemPage *pNew;
  Pgno pgnoNew;
  u8 *pCell;
  int szCell;
  CellInfo info;
  BtShared *pBt = pPage->pBt;
  int parentIdx = pParent->nCell;   /* pParent new divider cell index */
  int parentSize;                   /* Size of new divider cell */
  u8 parentCell[64];                /* Space for the new divider cell */

  /* Allocate a new page. Insert the overflow cell from pPage
  ** into it. Then remove the overflow cell from pPage.
  */
  rc = allocateBtreePage(pBt, &pNew, &pgnoNew, 0, 0);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  pCell = pPage->aOvfl[0].pCell;
  szCell = cellSizePtr(pPage, pCell);
  zeroPage(pNew, pPage->aData[0]);
  assemblePage(pNew, 1, &pCell, &szCell);
  pPage->nOverflow = 0;

  /* Set the parent of the newly allocated page to pParent. */
  pNew->pParent = pParent;
  sqlite3PagerRef(pParent->pDbPage);

  /* pPage is currently the right-child of pParent. Change this
  ** so that the right-child is the new page allocated above and
  ** pPage is the next-to-right child. 
  */
  assert( pPage->nCell>0 );
  parseCellPtr(pPage, findCell(pPage, pPage->nCell-1), &info);
  rc = fillInCell(pParent, parentCell, 0, info.nKey, 0, 0, &parentSize);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  assert( parentSize<64 );
  rc = insertCell(pParent, parentIdx, parentCell, parentSize, 0, 4);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  put4byte(findOverflowCell(pParent,parentIdx), pPage->pgno);
  put4byte(&pParent->aData[pParent->hdrOffset+8], pgnoNew);

#ifndef SQLITE_OMIT_AUTOVACUUM
  /* If this is an auto-vacuum database, update the pointer map
  ** with entries for the new page, and any pointer from the 
  ** cell on the page to an overflow page.
  */
  if( pBt->autoVacuum ){
    rc = ptrmapPut(pBt, pgnoNew, PTRMAP_BTREE, pParent->pgno);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    rc = ptrmapPutOvfl(pNew, 0);
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }
#endif

  /* Release the reference to the new page and balance the parent page,
  ** in case the divider cell inserted caused it to become overfull.
  */
  releasePage(pNew);
  return balance(pParent, 0);
}
#endif /* SQLITE_OMIT_QUICKBALANCE */

/*
** The ISAUTOVACUUM macro is used within balance_nonroot() to determine
** if the database supports auto-vacuum or not. Because it is used
** within an expression that is an argument to another macro 
** (sqliteMallocRaw), it is not possible to use conditional compilation.
** So, this macro is defined instead.
*/
#ifndef SQLITE_OMIT_AUTOVACUUM
#define ISAUTOVACUUM (pBt->autoVacuum)
#else
#define ISAUTOVACUUM 0
#endif

/*
** This routine redistributes Cells on pPage and up to NN*2 siblings
** of pPage so that all pages have about the same amount of free space.
** Usually NN siblings on either side of pPage is used in the balancing,
** though more siblings might come from one side if pPage is the first
** or last child of its parent.  If pPage has fewer than 2*NN siblings
** (something which can only happen if pPage is the root page or a 
** child of root) then all available siblings participate in the balancing.
**
** The number of siblings of pPage might be increased or decreased by one or
** two in an effort to keep pages nearly full but not over full. The root page
** is special and is allowed to be nearly empty. If pPage is 
** the root page, then the depth of the tree might be increased
** or decreased by one, as necessary, to keep the root page from being
** overfull or completely empty.
**
** Note that when this routine is called, some of the Cells on pPage
** might not actually be stored in pPage->aData[].  This can happen
** if the page is overfull.  Part of the job of this routine is to
** make sure all Cells for pPage once again fit in pPage->aData[].
**
** In the course of balancing the siblings of pPage, the parent of pPage
** might become overfull or underfull.  If that happens, then this routine
** is called recursively on the parent.
**
** If this routine fails for any reason, it might leave the database
** in a corrupted state.  So if this routine fails, the database should
** be rolled back.
*/
static int balance_nonroot(MemPage *pPage){
  MemPage *pParent;            /* The parent of pPage */
  BtShared *pBt;                  /* The whole database */
  int nCell = 0;               /* Number of cells in apCell[] */
  int nMaxCells = 0;           /* Allocated size of apCell, szCell, aFrom. */
  int nOld;                    /* Number of pages in apOld[] */
  int nNew;                    /* Number of pages in apNew[] */
  int nDiv;                    /* Number of cells in apDiv[] */
  int i, j, k;                 /* Loop counters */
  int idx;                     /* Index of pPage in pParent->aCell[] */
  int nxDiv;                   /* Next divider slot in pParent->aCell[] */
  int rc;                      /* The return code */
  int leafCorrection;          /* 4 if pPage is a leaf.  0 if not */
  int leafData;                /* True if pPage is a leaf of a LEAFDATA tree */
  int usableSpace;             /* Bytes in pPage beyond the header */
  int pageFlags;               /* Value of pPage->aData[0] */
  int subtotal;                /* Subtotal of bytes in cells on one page */
  int iSpace = 0;              /* First unused byte of aSpace[] */
  MemPage *apOld[NB];          /* pPage and up to two siblings */
  Pgno pgnoOld[NB];            /* Page numbers for each page in apOld[] */
  MemPage *apCopy[NB];         /* Private copies of apOld[] pages */
  MemPage *apNew[NB+2];        /* pPage and up to NB siblings after balancing */
  Pgno pgnoNew[NB+2];          /* Page numbers for each page in apNew[] */
  u8 *apDiv[NB];               /* Divider cells in pParent */
  int cntNew[NB+2];            /* Index in aCell[] of cell after i-th page */
  int szNew[NB+2];             /* Combined size of cells place on i-th page */
  u8 **apCell = 0;             /* All cells begin balanced */
  int *szCell;                 /* Local size of all cells in apCell[] */
  u8 *aCopy[NB];               /* Space for holding data of apCopy[] */
  u8 *aSpace;                  /* Space to hold copies of dividers cells */
#ifndef SQLITE_OMIT_AUTOVACUUM
  u8 *aFrom = 0;
#endif

  /* 
  ** Find the parent page.
  */
  assert( pPage->isInit );
  assert( sqlite3PagerIswriteable(pPage->pDbPage) );
  pBt = pPage->pBt;
  pParent = pPage->pParent;
  assert( pParent );
  if( SQLITE_OK!=(rc = sqlite3PagerWrite(pParent->pDbPage)) ){
    return rc;
  }
  TRACE(("BALANCE: begin page %d child of %d\n", pPage->pgno, pParent->pgno));

#ifndef SQLITE_OMIT_QUICKBALANCE
  /*
  ** A special case:  If a new entry has just been inserted into a
  ** table (that is, a btree with integer keys and all data at the leaves)
  ** and the new entry is the right-most entry in the tree (it has the
  ** largest key) then use the special balance_quick() routine for
  ** balancing.  balance_quick() is much faster and results in a tighter
  ** packing of data in the common case.
  */
  if( pPage->leaf &&
      pPage->intKey &&
      pPage->leafData &&
      pPage->nOverflow==1 &&
      pPage->aOvfl[0].idx==pPage->nCell &&
      pPage->pParent->pgno!=1 &&
      get4byte(&pParent->aData[pParent->hdrOffset+8])==pPage->pgno
  ){
    /*
    ** TODO: Check the siblings to the left of pPage. It may be that
    ** they are not full and no new page is required.
    */
    return balance_quick(pPage, pParent);
  }
#endif

  /*
  ** Find the cell in the parent page whose left child points back
  ** to pPage.  The "idx" variable is the index of that cell.  If pPage
  ** is the rightmost child of pParent then set idx to pParent->nCell 
  */
  if( pParent->idxShift ){
    Pgno pgno;
    pgno = pPage->pgno;
    assert( pgno==sqlite3PagerPagenumber(pPage->pDbPage) );
    for(idx=0; idx<pParent->nCell; idx++){
      if( get4byte(findCell(pParent, idx))==pgno ){
        break;
      }
    }
    assert( idx<pParent->nCell
             || get4byte(&pParent->aData[pParent->hdrOffset+8])==pgno );
  }else{
    idx = pPage->idxParent;
  }

  /*
  ** Initialize variables so that it will be safe to jump
  ** directly to balance_cleanup at any moment.
  */
  nOld = nNew = 0;
  sqlite3PagerRef(pParent->pDbPage);

  /*
  ** Find sibling pages to pPage and the cells in pParent that divide
  ** the siblings.  An attempt is made to find NN siblings on either
  ** side of pPage.  More siblings are taken from one side, however, if
  ** pPage there are fewer than NN siblings on the other side.  If pParent
  ** has NB or fewer children then all children of pParent are taken.
  */
  nxDiv = idx - NN;
  if( nxDiv + NB > pParent->nCell ){
    nxDiv = pParent->nCell - NB + 1;
  }
  if( nxDiv<0 ){
    nxDiv = 0;
  }
  nDiv = 0;
  for(i=0, k=nxDiv; i<NB; i++, k++){
    if( k<pParent->nCell ){
      apDiv[i] = findCell(pParent, k);
      nDiv++;
      assert( !pParent->leaf );
      pgnoOld[i] = get4byte(apDiv[i]);
    }else if( k==pParent->nCell ){
      pgnoOld[i] = get4byte(&pParent->aData[pParent->hdrOffset+8]);
    }else{
      break;
    }
    rc = getAndInitPage(pBt, pgnoOld[i], &apOld[i], pParent);
    if( rc ) goto balance_cleanup;
    apOld[i]->idxParent = k;
    apCopy[i] = 0;
    assert( i==nOld );
    nOld++;
    nMaxCells += 1+apOld[i]->nCell+apOld[i]->nOverflow;
  }

  /* Make nMaxCells a multiple of 2 in order to preserve 8-byte
  ** alignment */
  nMaxCells = (nMaxCells + 1)&~1;

  /*
  ** Allocate space for memory structures
  */
  apCell = sqliteMallocRaw( 
       nMaxCells*sizeof(u8*)                           /* apCell */
     + nMaxCells*sizeof(int)                           /* szCell */
     + ROUND8(sizeof(MemPage))*NB                      /* aCopy */
     + pBt->pageSize*(5+NB)                            /* aSpace */
     + (ISAUTOVACUUM ? nMaxCells : 0)                  /* aFrom */
  );
  if( apCell==0 ){
    rc = SQLITE_NOMEM;
    goto balance_cleanup;
  }
  szCell = (int*)&apCell[nMaxCells];
  aCopy[0] = (u8*)&szCell[nMaxCells];
  assert( ((aCopy[0] - (u8*)apCell) & 7)==0 ); /* 8-byte alignment required */
  for(i=1; i<NB; i++){
    aCopy[i] = &aCopy[i-1][pBt->pageSize+ROUND8(sizeof(MemPage))];
    assert( ((aCopy[i] - (u8*)apCell) & 7)==0 ); /* 8-byte alignment required */
  }
  aSpace = &aCopy[NB-1][pBt->pageSize+ROUND8(sizeof(MemPage))];
  assert( ((aSpace - (u8*)apCell) & 7)==0 ); /* 8-byte alignment required */
#ifndef SQLITE_OMIT_AUTOVACUUM
  if( pBt->autoVacuum ){
    aFrom = &aSpace[5*pBt->pageSize];
  }
#endif
  
  /*
  ** Make copies of the content of pPage and its siblings into aOld[].
  ** The rest of this function will use data from the copies rather
  ** that the original pages since the original pages will be in the
  ** process of being overwritten.
  */
  for(i=0; i<nOld; i++){
    MemPage *p = apCopy[i] = (MemPage*)&aCopy[i][pBt->pageSize];
    p->aData = &((u8*)p)[-pBt->pageSize];
    memcpy(p->aData, apOld[i]->aData, pBt->pageSize + sizeof(MemPage));
    /* The memcpy() above changes the value of p->aData so we have to
    ** set it again. */
    p->aData = &((u8*)p)[-pBt->pageSize];
  }

  /*
  ** Load pointers to all cells on sibling pages and the divider cells
  ** into the local apCell[] array.  Make copies of the divider cells
  ** into space obtained form aSpace[] and remove the the divider Cells
  ** from pParent.
  **
  ** If the siblings are on leaf pages, then the child pointers of the
  ** divider cells are stripped from the cells before they are copied
  ** into aSpace[].  In this way, all cells in apCell[] are without
  ** child pointers.  If siblings are not leaves, then all cell in
  ** apCell[] include child pointers.  Either way, all cells in apCell[]
  ** are alike.
  **
  ** leafCorrection:  4 if pPage is a leaf.  0 if pPage is not a leaf.
  **       leafData:  1 if pPage holds key+data and pParent holds only keys.
  */
  nCell = 0;
  leafCorrection = pPage->leaf*4;
  leafData = pPage->leafData && pPage->leaf;
  for(i=0; i<nOld; i++){
    MemPage *pOld = apCopy[i];
    int limit = pOld->nCell+pOld->nOverflow;
    for(j=0; j<limit; j++){
      assert( nCell<nMaxCells );
      apCell[nCell] = findOverflowCell(pOld, j);
      szCell[nCell] = cellSizePtr(pOld, apCell[nCell]);
#ifndef SQLITE_OMIT_AUTOVACUUM
      if( pBt->autoVacuum ){
        int a;
        aFrom[nCell] = i;
        for(a=0; a<pOld->nOverflow; a++){
          if( pOld->aOvfl[a].pCell==apCell[nCell] ){
            aFrom[nCell] = 0xFF;
            break;
          }
        }
      }
#endif
      nCell++;
    }
    if( i<nOld-1 ){
      int sz = cellSizePtr(pParent, apDiv[i]);
      if( leafData ){
        /* With the LEAFDATA flag, pParent cells hold only INTKEYs that
        ** are duplicates of keys on the child pages.  We need to remove
        ** the divider cells from pParent, but the dividers cells are not
        ** added to apCell[] because they are duplicates of child cells.
        */
        dropCell(pParent, nxDiv, sz);
      }else{
        u8 *pTemp;
        assert( nCell<nMaxCells );
        szCell[nCell] = sz;
        pTemp = &aSpace[iSpace];
        iSpace += sz;
        assert( iSpace<=pBt->pageSize*5 );
        memcpy(pTemp, apDiv[i], sz);
        apCell[nCell] = pTemp+leafCorrection;
#ifndef SQLITE_OMIT_AUTOVACUUM
        if( pBt->autoVacuum ){
          aFrom[nCell] = 0xFF;
        }
#endif
        dropCell(pParent, nxDiv, sz);
        szCell[nCell] -= leafCorrection;
        assert( get4byte(pTemp)==pgnoOld[i] );
        if( !pOld->leaf ){
          assert( leafCorrection==0 );
          /* The right pointer of the child page pOld becomes the left
          ** pointer of the divider cell */
          memcpy(apCell[nCell], &pOld->aData[pOld->hdrOffset+8], 4);
        }else{
          assert( leafCorrection==4 );
        }
        nCell++;
      }
    }
  }

  /*
  ** Figure out the number of pages needed to hold all nCell cells.
  ** Store this number in "k".  Also compute szNew[] which is the total
  ** size of all cells on the i-th page and cntNew[] which is the index
  ** in apCell[] of the cell that divides page i from page i+1.  
  ** cntNew[k] should equal nCell.
  **
  ** Values computed by this block:
  **
  **           k: The total number of sibling pages
  **    szNew[i]: Spaced used on the i-th sibling page.
  **   cntNew[i]: Index in apCell[] and szCell[] for the first cell to
  **              the right of the i-th sibling page.
  ** usableSpace: Number of bytes of space available on each sibling.
  ** 
  */
  usableSpace = pBt->usableSize - 12 + leafCorrection;
  for(subtotal=k=i=0; i<nCell; i++){
    assert( i<nMaxCells );
    subtotal += szCell[i] + 2;
    if( subtotal > usableSpace ){
      szNew[k] = subtotal - szCell[i];
      cntNew[k] = i;
      if( leafData ){ i--; }
      subtotal = 0;
      k++;
    }
  }
  szNew[k] = subtotal;
  cntNew[k] = nCell;
  k++;

  /*
  ** The packing computed by the previous block is biased toward the siblings
  ** on the left side.  The left siblings are always nearly full, while the
  ** right-most sibling might be nearly empty.  This block of code attempts
  ** to adjust the packing of siblings to get a better balance.
  **
  ** This adjustment is more than an optimization.  The packing above might
  ** be so out of balance as to be illegal.  For example, the right-most
  ** sibling might be completely empty.  This adjustment is not optional.
  */
  for(i=k-1; i>0; i--){
    int szRight = szNew[i];  /* Size of sibling on the right */
    int szLeft = szNew[i-1]; /* Size of sibling on the left */
    int r;              /* Index of right-most cell in left sibling */
    int d;              /* Index of first cell to the left of right sibling */

    r = cntNew[i-1] - 1;
    d = r + 1 - leafData;
    assert( d<nMaxCells );
    assert( r<nMaxCells );
    while( szRight==0 || szRight+szCell[d]+2<=szLeft-(szCell[r]+2) ){
      szRight += szCell[d] + 2;
      szLeft -= szCell[r] + 2;
      cntNew[i-1]--;
      r = cntNew[i-1] - 1;
      d = r + 1 - leafData;
    }
    szNew[i] = szRight;
    szNew[i-1] = szLeft;
  }

  /* Either we found one or more cells (cntnew[0])>0) or we are the
  ** a virtual root page.  A virtual root page is when the real root
  ** page is page 1 and we are the only child of that page.
  */
  assert( cntNew[0]>0 || (pParent->pgno==1 && pParent->nCell==0) );

  /*
  ** Allocate k new pages.  Reuse old pages where possible.
  */
  assert( pPage->pgno>1 );
  pageFlags = pPage->aData[0];
  for(i=0; i<k; i++){
    MemPage *pNew;
    if( i<nOld ){
      pNew = apNew[i] = apOld[i];
      pgnoNew[i] = pgnoOld[i];
      apOld[i] = 0;
      rc = sqlite3PagerWrite(pNew->pDbPage);
      nNew++;
      if( rc ) goto balance_cleanup;
    }else{
      assert( i>0 );
      rc = allocateBtreePage(pBt, &pNew, &pgnoNew[i], pgnoNew[i-1], 0);
      if( rc ) goto balance_cleanup;
      apNew[i] = pNew;
      nNew++;
    }
    zeroPage(pNew, pageFlags);
  }

  /* Free any old pages that were not reused as new pages.
  */
  while( i<nOld ){
    rc = freePage(apOld[i]);
    if( rc ) goto balance_cleanup;
    releasePage(apOld[i]);
    apOld[i] = 0;
    i++;
  }

  /*
  ** Put the new pages in accending order.  This helps to
  ** keep entries in the disk file in order so that a scan
  ** of the table is a linear scan through the file.  That
  ** in turn helps the operating system to deliver pages
  ** from the disk more rapidly.
  **
  ** An O(n^2) insertion sort algorithm is used, but since
  ** n is never more than NB (a small constant), that should
  ** not be a problem.
  **
  ** When NB==3, this one optimization makes the database
  ** about 25% faster for large insertions and deletions.
  */
  for(i=0; i<k-1; i++){
    int minV = pgnoNew[i];
    int minI = i;
    for(j=i+1; j<k; j++){
      if( pgnoNew[j]<(unsigned)minV ){
        minI = j;
        minV = pgnoNew[j];
      }
    }
    if( minI>i ){
      int t;
      MemPage *pT;
      t = pgnoNew[i];
      pT = apNew[i];
      pgnoNew[i] = pgnoNew[minI];
      apNew[i] = apNew[minI];
      pgnoNew[minI] = t;
      apNew[minI] = pT;
    }
  }
  TRACE(("BALANCE: old: %d %d %d  new: %d(%d) %d(%d) %d(%d) %d(%d) %d(%d)\n",
    pgnoOld[0], 
    nOld>=2 ? pgnoOld[1] : 0,
    nOld>=3 ? pgnoOld[2] : 0,
    pgnoNew[0], szNew[0],
    nNew>=2 ? pgnoNew[1] : 0, nNew>=2 ? szNew[1] : 0,
    nNew>=3 ? pgnoNew[2] : 0, nNew>=3 ? szNew[2] : 0,
    nNew>=4 ? pgnoNew[3] : 0, nNew>=4 ? szNew[3] : 0,
    nNew>=5 ? pgnoNew[4] : 0, nNew>=5 ? szNew[4] : 0));

  /*
  ** Evenly distribute the data in apCell[] across the new pages.
  ** Insert divider cells into pParent as necessary.
  */
  j = 0;
  for(i=0; i<nNew; i++){
    /* Assemble the new sibling page. */
    MemPage *pNew = apNew[i];
    assert( j<nMaxCells );
    assert( pNew->pgno==pgnoNew[i] );
    assemblePage(pNew, cntNew[i]-j, &apCell[j], &szCell[j]);
    assert( pNew->nCell>0 || (nNew==1 && cntNew[0]==0) );
    assert( pNew->nOverflow==0 );

#ifndef SQLITE_OMIT_AUTOVACUUM
    /* If this is an auto-vacuum database, update the pointer map entries
    ** that point to the siblings that were rearranged. These can be: left
    ** children of cells, the right-child of the page, or overflow pages
    ** pointed to by cells.
    */
    if( pBt->autoVacuum ){
      for(k=j; k<cntNew[i]; k++){
        assert( k<nMaxCells );
        if( aFrom[k]==0xFF || apCopy[aFrom[k]]->pgno!=pNew->pgno ){
          rc = ptrmapPutOvfl(pNew, k-j);
          if( rc!=SQLITE_OK ){
            goto balance_cleanup;
          }
        }
      }
    }
#endif

    j = cntNew[i];

    /* If the sibling page assembled above was not the right-most sibling,
    ** insert a divider cell into the parent page.
    */
    if( i<nNew-1 && j<nCell ){
      u8 *pCell;
      u8 *pTemp;
      int sz;

      assert( j<nMaxCells );
      pCell = apCell[j];
      sz = szCell[j] + leafCorrection;
      if( !pNew->leaf ){
        memcpy(&pNew->aData[8], pCell, 4);
        pTemp = 0;
      }else if( leafData ){
	/* If the tree is a leaf-data tree, and the siblings are leaves, 
        ** then there is no divider cell in apCell[]. Instead, the divider 
        ** cell consists of the integer key for the right-most cell of 
        ** the sibling-page assembled above only.
        */
        CellInfo info;
        j--;
        parseCellPtr(pNew, apCell[j], &info);
        pCell = &aSpace[iSpace];
        fillInCell(pParent, pCell, 0, info.nKey, 0, 0, &sz);
        iSpace += sz;
        assert( iSpace<=pBt->pageSize*5 );
        pTemp = 0;
      }else{
        pCell -= 4;
        pTemp = &aSpace[iSpace];
        iSpace += sz;
        assert( iSpace<=pBt->pageSize*5 );
      }
      rc = insertCell(pParent, nxDiv, pCell, sz, pTemp, 4);
      if( rc!=SQLITE_OK ) goto balance_cleanup;
      put4byte(findOverflowCell(pParent,nxDiv), pNew->pgno);
#ifndef SQLITE_OMIT_AUTOVACUUM
      /* If this is an auto-vacuum database, and not a leaf-data tree,
      ** then update the pointer map with an entry for the overflow page
      ** that the cell just inserted points to (if any).
      */
      if( pBt->autoVacuum && !leafData ){
        rc = ptrmapPutOvfl(pParent, nxDiv);
        if( rc!=SQLITE_OK ){
          goto balance_cleanup;
        }
      }
#endif
      j++;
      nxDiv++;
    }
  }
  assert( j==nCell );
  assert( nOld>0 );
  assert( nNew>0 );
  if( (pageFlags & PTF_LEAF)==0 ){
    memcpy(&apNew[nNew-1]->aData[8], &apCopy[nOld-1]->aData[8], 4);
  }
  if( nxDiv==pParent->nCell+pParent->nOverflow ){
    /* Right-most sibling is the right-most child of pParent */
    put4byte(&pParent->aData[pParent->hdrOffset+8], pgnoNew[nNew-1]);
  }else{
    /* Right-most sibling is the left child of the first entry in pParent
    ** past the right-most divider entry */
    put4byte(findOverflowCell(pParent, nxDiv), pgnoNew[nNew-1]);
  }

  /*
  ** Reparent children of all cells.
  */
  for(i=0; i<nNew; i++){
    rc = reparentChildPages(apNew[i]);
    if( rc!=SQLITE_OK ) goto balance_cleanup;
  }
  rc = reparentChildPages(pParent);
  if( rc!=SQLITE_OK ) goto balance_cleanup;

  /*
  ** Balance the parent page.  Note that the current page (pPage) might
  ** have been added to the freelist so it might no longer be initialized.
  ** But the parent page will always be initialized.
  */
  assert( pParent->isInit );
  rc = balance(pParent, 0);
  
  /*
  ** Cleanup before returning.
  */
balance_cleanup:
  sqliteFree(apCell);
  for(i=0; i<nOld; i++){
    releasePage(apOld[i]);
  }
  for(i=0; i<nNew; i++){
    releasePage(apNew[i]);
  }
  releasePage(pParent);
  TRACE(("BALANCE: finished with %d: old=%d new=%d cells=%d\n",
          pPage->pgno, nOld, nNew, nCell));
  return rc;
}

/*
** This routine is called for the root page of a btree when the root
** page contains no cells.  This is an opportunity to make the tree
** shallower by one level.
*/
static int balance_shallower(MemPage *pPage){
  MemPage *pChild;             /* The only child page of pPage */
  Pgno pgnoChild;              /* Page number for pChild */
  int rc = SQLITE_OK;          /* Return code from subprocedures */
  BtShared *pBt;                  /* The main BTree structure */
  int mxCellPerPage;           /* Maximum number of cells per page */
  u8 **apCell;                 /* All cells from pages being balanced */
  int *szCell;                 /* Local size of all cells */

  assert( pPage->pParent==0 );
  assert( pPage->nCell==0 );
  pBt = pPage->pBt;
  mxCellPerPage = MX_CELL(pBt);
  apCell = sqliteMallocRaw( mxCellPerPage*(sizeof(u8*)+sizeof(int)) );
  if( apCell==0 ) return SQLITE_NOMEM;
  szCell = (int*)&apCell[mxCellPerPage];
  if( pPage->leaf ){
    /* The table is completely empty */
    TRACE(("BALANCE: empty table %d\n", pPage->pgno));
  }else{
    /* The root page is empty but has one child.  Transfer the
    ** information from that one child into the root page if it 
    ** will fit.  This reduces the depth of the tree by one.
    **
    ** If the root page is page 1, it has less space available than
    ** its child (due to the 100 byte header that occurs at the beginning
    ** of the database fle), so it might not be able to hold all of the 
    ** information currently contained in the child.  If this is the 
    ** case, then do not do the transfer.  Leave page 1 empty except
    ** for the right-pointer to the child page.  The child page becomes
    ** the virtual root of the tree.
    */
    pgnoChild = get4byte(&pPage->aData[pPage->hdrOffset+8]);
    assert( pgnoChild>0 );
    assert( pgnoChild<=sqlite3PagerPagecount(pPage->pBt->pPager) );
    rc = getPage(pPage->pBt, pgnoChild, &pChild, 0);
    if( rc ) goto end_shallow_balance;
    if( pPage->pgno==1 ){
      rc = initPage(pChild, pPage);
      if( rc ) goto end_shallow_balance;
      assert( pChild->nOverflow==0 );
      if( pChild->nFree>=100 ){
        /* The child information will fit on the root page, so do the
        ** copy */
        int i;
        zeroPage(pPage, pChild->aData[0]);
        for(i=0; i<pChild->nCell; i++){
          apCell[i] = findCell(pChild,i);
          szCell[i] = cellSizePtr(pChild, apCell[i]);
        }
        assemblePage(pPage, pChild->nCell, apCell, szCell);
        /* Copy the right-pointer of the child to the parent. */
        put4byte(&pPage->aData[pPage->hdrOffset+8], 
            get4byte(&pChild->aData[pChild->hdrOffset+8]));
        freePage(pChild);
        TRACE(("BALANCE: child %d transfer to page 1\n", pChild->pgno));
      }else{
        /* The child has more information that will fit on the root.
        ** The tree is already balanced.  Do nothing. */
        TRACE(("BALANCE: child %d will not fit on page 1\n", pChild->pgno));
      }
    }else{
      memcpy(pPage->aData, pChild->aData, pPage->pBt->usableSize);
      pPage->isInit = 0;
      pPage->pParent = 0;
      rc = initPage(pPage, 0);
      assert( rc==SQLITE_OK );
      freePage(pChild);
      TRACE(("BALANCE: transfer child %d into root %d\n",
              pChild->pgno, pPage->pgno));
    }
    rc = reparentChildPages(pPage);
    assert( pPage->nOverflow==0 );
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pBt->autoVacuum ){
      int i;
      for(i=0; i<pPage->nCell; i++){ 
        rc = ptrmapPutOvfl(pPage, i);
        if( rc!=SQLITE_OK ){
          goto end_shallow_balance;
        }
      }
    }
#endif
    if( rc!=SQLITE_OK ) goto end_shallow_balance;
    releasePage(pChild);
  }
end_shallow_balance:
  sqliteFree(apCell);
  return rc;
}


/*
** The root page is overfull
**
** When this happens, Create a new child page and copy the
** contents of the root into the child.  Then make the root
** page an empty page with rightChild pointing to the new
** child.   Finally, call balance_internal() on the new child
** to cause it to split.
*/
static int balance_deeper(MemPage *pPage){
  int rc;             /* Return value from subprocedures */
  MemPage *pChild;    /* Pointer to a new child page */
  Pgno pgnoChild;     /* Page number of the new child page */
  BtShared *pBt;         /* The BTree */
  int usableSize;     /* Total usable size of a page */
  u8 *data;           /* Content of the parent page */
  u8 *cdata;          /* Content of the child page */
  int hdr;            /* Offset to page header in parent */
  int brk;            /* Offset to content of first cell in parent */

  assert( pPage->pParent==0 );
  assert( pPage->nOverflow>0 );
  pBt = pPage->pBt;
  rc = allocateBtreePage(pBt, &pChild, &pgnoChild, pPage->pgno, 0);
  if( rc ) return rc;
  assert( sqlite3PagerIswriteable(pChild->pDbPage) );
  usableSize = pBt->usableSize;
  data = pPage->aData;
  hdr = pPage->hdrOffset;
  brk = get2byte(&data[hdr+5]);
  cdata = pChild->aData;
  memcpy(cdata, &data[hdr], pPage->cellOffset+2*pPage->nCell-hdr);
  memcpy(&cdata[brk], &data[brk], usableSize-brk);
  assert( pChild->isInit==0 );
  rc = initPage(pChild, pPage);
  if( rc ) goto balancedeeper_out;
  memcpy(pChild->aOvfl, pPage->aOvfl, pPage->nOverflow*sizeof(pPage->aOvfl[0]));
  pChild->nOverflow = pPage->nOverflow;
  if( pChild->nOverflow ){
    pChild->nFree = 0;
  }
  assert( pChild->nCell==pPage->nCell );
  zeroPage(pPage, pChild->aData[0] & ~PTF_LEAF);
  put4byte(&pPage->aData[pPage->hdrOffset+8], pgnoChild);
  TRACE(("BALANCE: copy root %d into %d\n", pPage->pgno, pChild->pgno));
#ifndef SQLITE_OMIT_AUTOVACUUM
  if( pBt->autoVacuum ){
    int i;
    rc = ptrmapPut(pBt, pChild->pgno, PTRMAP_BTREE, pPage->pgno);
    if( rc ) goto balancedeeper_out;
    for(i=0; i<pChild->nCell; i++){
      rc = ptrmapPutOvfl(pChild, i);
      if( rc!=SQLITE_OK ){
        return rc;
      }
    }
  }
#endif
  rc = balance_nonroot(pChild);

balancedeeper_out:
  releasePage(pChild);
  return rc;
}

/*
** Decide if the page pPage needs to be balanced.  If balancing is
** required, call the appropriate balancing routine.
*/
static int balance(MemPage *pPage, int insert){
  int rc = SQLITE_OK;
  if( pPage->pParent==0 ){
    if( pPage->nOverflow>0 ){
      rc = balance_deeper(pPage);
    }
    if( rc==SQLITE_OK && pPage->nCell==0 ){
      rc = balance_shallower(pPage);
    }
  }else{
    if( pPage->nOverflow>0 || 
        (!insert && pPage->nFree>pPage->pBt->usableSize*2/3) ){
      rc = balance_nonroot(pPage);
    }
  }
  return rc;
}

/*
** This routine checks all cursors that point to table pgnoRoot.
** If any of those cursors were opened with wrFlag==0 in a different
** database connection (a database connection that shares the pager
** cache with the current connection) and that other connection 
** is not in the ReadUncommmitted state, then this routine returns 
** SQLITE_LOCKED.
**
** In addition to checking for read-locks (where a read-lock 
** means a cursor opened with wrFlag==0) this routine also moves
** all cursors write cursors so that they are pointing to the 
** first Cell on the root page.  This is necessary because an insert 
** or delete might change the number of cells on a page or delete
** a page entirely and we do not want to leave any cursors 
** pointing to non-existant pages or cells.
*/
static int checkReadLocks(Btree *pBtree, Pgno pgnoRoot, BtCursor *pExclude){
  BtCursor *p;
  BtShared *pBt = pBtree->pBt;
  sqlite3 *db = pBtree->pSqlite;
  for(p=pBt->pCursor; p; p=p->pNext){
    if( p==pExclude ) continue;
    if( p->eState!=CURSOR_VALID ) continue;
    if( p->pgnoRoot!=pgnoRoot ) continue;
    if( p->wrFlag==0 ){
      sqlite3 *dbOther = p->pBtree->pSqlite;
      if( dbOther==0 ||
         (dbOther!=db && (dbOther->flags & SQLITE_ReadUncommitted)==0) ){
        return SQLITE_LOCKED;
      }
    }else if( p->pPage->pgno!=p->pgnoRoot ){
      moveToRoot(p);
    }
  }
  return SQLITE_OK;
}

/*
** Insert a new record into the BTree.  The key is given by (pKey,nKey)
** and the data is given by (pData,nData).  The cursor is used only to
** define what table the record should be inserted into.  The cursor
** is left pointing at a random location.
**
** For an INTKEY table, only the nKey value of the key is used.  pKey is
** ignored.  For a ZERODATA table, the pData and nData are both ignored.
*/
int sqlite3BtreeInsert(
  BtCursor *pCur,                /* Insert data into the table of this cursor */
  const void *pKey, i64 nKey,    /* The key of the new record */
  const void *pData, int nData,  /* The data of the new record */
  int appendBias                 /* True if this is likely an append */
){
  int rc;
  int loc;
  int szNew;
  MemPage *pPage;
  BtShared *pBt = pCur->pBtree->pBt;
  unsigned char *oldCell;
  unsigned char *newCell = 0;

  if( pBt->inTransaction!=TRANS_WRITE ){
    /* Must start a transaction before doing an insert */
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( !pBt->readOnly );
  if( !pCur->wrFlag ){
    return SQLITE_PERM;   /* Cursor not open for writing */
  }
  if( checkReadLocks(pCur->pBtree, pCur->pgnoRoot, pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }

  /* Save the positions of any other cursors open on this table */
  clearCursorPosition(pCur);
  if( 
    SQLITE_OK!=(rc = saveAllCursors(pBt, pCur->pgnoRoot, pCur)) ||
    SQLITE_OK!=(rc = sqlite3BtreeMoveto(pCur, pKey, nKey, appendBias, &loc))
  ){
    return rc;
  }

  pPage = pCur->pPage;
  assert( pPage->intKey || nKey>=0 );
  assert( pPage->leaf || !pPage->leafData );
  TRACE(("INSERT: table=%d nkey=%lld ndata=%d page=%d %s\n",
          pCur->pgnoRoot, nKey, nData, pPage->pgno,
          loc==0 ? "overwrite" : "new entry"));
  assert( pPage->isInit );
  rc = sqlite3PagerWrite(pPage->pDbPage);
  if( rc ) return rc;
  newCell = sqliteMallocRaw( MX_CELL_SIZE(pBt) );
  if( newCell==0 ) return SQLITE_NOMEM;
  rc = fillInCell(pPage, newCell, pKey, nKey, pData, nData, &szNew);
  if( rc ) goto end_insert;
  assert( szNew==cellSizePtr(pPage, newCell) );
  assert( szNew<=MX_CELL_SIZE(pBt) );
  if( loc==0 && CURSOR_VALID==pCur->eState ){
    int szOld;
    assert( pCur->idx>=0 && pCur->idx<pPage->nCell );
    oldCell = findCell(pPage, pCur->idx);
    if( !pPage->leaf ){
      memcpy(newCell, oldCell, 4);
    }
    szOld = cellSizePtr(pPage, oldCell);
    rc = clearCell(pPage, oldCell);
    if( rc ) goto end_insert;
    dropCell(pPage, pCur->idx, szOld);
  }else if( loc<0 && pPage->nCell>0 ){
    assert( pPage->leaf );
    pCur->idx++;
    pCur->info.nSize = 0;
  }else{
    assert( pPage->leaf );
  }
  rc = insertCell(pPage, pCur->idx, newCell, szNew, 0, 0);
  if( rc!=SQLITE_OK ) goto end_insert;
  rc = balance(pPage, 1);
  /* sqlite3BtreePageDump(pCur->pBt, pCur->pgnoRoot, 1); */
  /* fflush(stdout); */
  if( rc==SQLITE_OK ){
    moveToRoot(pCur);
  }
end_insert:
  sqliteFree(newCell);
  return rc;
}

/*
** Delete the entry that the cursor is pointing to.  The cursor
** is left pointing at a random location.
*/
int sqlite3BtreeDelete(BtCursor *pCur){
  MemPage *pPage = pCur->pPage;
  unsigned char *pCell;
  int rc;
  Pgno pgnoChild = 0;
  BtShared *pBt = pCur->pBtree->pBt;

  assert( pPage->isInit );
  if( pBt->inTransaction!=TRANS_WRITE ){
    /* Must start a transaction before doing a delete */
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( !pBt->readOnly );
  if( pCur->idx >= pPage->nCell ){
    return SQLITE_ERROR;  /* The cursor is not pointing to anything */
  }
  if( !pCur->wrFlag ){
    return SQLITE_PERM;   /* Did not open this cursor for writing */
  }
  if( checkReadLocks(pCur->pBtree, pCur->pgnoRoot, pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }

  /* Restore the current cursor position (a no-op if the cursor is not in 
  ** CURSOR_REQUIRESEEK state) and save the positions of any other cursors 
  ** open on the same table. Then call sqlite3PagerWrite() on the page
  ** that the entry will be deleted from.
  */
  if( 
    (rc = restoreOrClearCursorPosition(pCur))!=0 ||
    (rc = saveAllCursors(pBt, pCur->pgnoRoot, pCur))!=0 ||
    (rc = sqlite3PagerWrite(pPage->pDbPage))!=0
  ){
    return rc;
  }

  /* Locate the cell within it's page and leave pCell pointing to the
  ** data. The clearCell() call frees any overflow pages associated with the
  ** cell. The cell itself is still intact.
  */
  pCell = findCell(pPage, pCur->idx);
  if( !pPage->leaf ){
    pgnoChild = get4byte(pCell);
  }
  rc = clearCell(pPage, pCell);
  if( rc ) return rc;

  if( !pPage->leaf ){
    /*
    ** The entry we are about to delete is not a leaf so if we do not
    ** do something we will leave a hole on an internal page.
    ** We have to fill the hole by moving in a cell from a leaf.  The
    ** next Cell after the one to be deleted is guaranteed to exist and
    ** to be a leaf so we can use it.
    */
    BtCursor leafCur;
    unsigned char *pNext;
    int szNext;  /* The compiler warning is wrong: szNext is always 
                 ** initialized before use.  Adding an extra initialization
                 ** to silence the compiler slows down the code. */
    int notUsed;
    unsigned char *tempCell = 0;
    assert( !pPage->leafData );
    getTempCursor(pCur, &leafCur);
    rc = sqlite3BtreeNext(&leafCur, &notUsed);
    if( rc==SQLITE_OK ){
      rc = sqlite3PagerWrite(leafCur.pPage->pDbPage);
    }
    if( rc==SQLITE_OK ){
      TRACE(("DELETE: table=%d delete internal from %d replace from leaf %d\n",
         pCur->pgnoRoot, pPage->pgno, leafCur.pPage->pgno));
      dropCell(pPage, pCur->idx, cellSizePtr(pPage, pCell));
      pNext = findCell(leafCur.pPage, leafCur.idx);
      szNext = cellSizePtr(leafCur.pPage, pNext);
      assert( MX_CELL_SIZE(pBt)>=szNext+4 );
      tempCell = sqliteMallocRaw( MX_CELL_SIZE(pBt) );
      if( tempCell==0 ){
        rc = SQLITE_NOMEM;
      }
    }
    if( rc==SQLITE_OK ){
      rc = insertCell(pPage, pCur->idx, pNext-4, szNext+4, tempCell, 0);
    }
    if( rc==SQLITE_OK ){
      put4byte(findOverflowCell(pPage, pCur->idx), pgnoChild);
      rc = balance(pPage, 0);
    }
    if( rc==SQLITE_OK ){
      dropCell(leafCur.pPage, leafCur.idx, szNext);
      rc = balance(leafCur.pPage, 0);
    }
    sqliteFree(tempCell);
    releaseTempCursor(&leafCur);
  }else{
    TRACE(("DELETE: table=%d delete from leaf %d\n",
       pCur->pgnoRoot, pPage->pgno));
    dropCell(pPage, pCur->idx, cellSizePtr(pPage, pCell));
    rc = balance(pPage, 0);
  }
  if( rc==SQLITE_OK ){
    moveToRoot(pCur);
  }
  return rc;
}

/*
** Create a new BTree table.  Write into *piTable the page
** number for the root page of the new table.
**
** The type of type is determined by the flags parameter.  Only the
** following values of flags are currently in use.  Other values for
** flags might not work:
**
**     BTREE_INTKEY|BTREE_LEAFDATA     Used for SQL tables with rowid keys
**     BTREE_ZERODATA                  Used for SQL indices
*/
int sqlite3BtreeCreateTable(Btree *p, int *piTable, int flags){
  BtShared *pBt = p->pBt;
  MemPage *pRoot;
  Pgno pgnoRoot;
  int rc;
  if( pBt->inTransaction!=TRANS_WRITE ){
    /* Must start a transaction first */
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( !pBt->readOnly );

  /* It is illegal to create a table if any cursors are open on the
  ** database. This is because in auto-vacuum mode the backend may
  ** need to move a database page to make room for the new root-page.
  ** If an open cursor was using the page a problem would occur.
  */
  if( pBt->pCursor ){
    return SQLITE_LOCKED;
  }

#ifdef SQLITE_OMIT_AUTOVACUUM
  rc = allocateBtreePage(pBt, &pRoot, &pgnoRoot, 1, 0);
  if( rc ) return rc;
#else
  if( pBt->autoVacuum ){
    Pgno pgnoMove;      /* Move a page here to make room for the root-page */
    MemPage *pPageMove; /* The page to move to. */

    /* Read the value of meta[3] from the database to determine where the
    ** root page of the new table should go. meta[3] is the largest root-page
    ** created so far, so the new root-page is (meta[3]+1).
    */
    rc = sqlite3BtreeGetMeta(p, 4, &pgnoRoot);
    if( rc!=SQLITE_OK ) return rc;
    pgnoRoot++;

    /* The new root-page may not be allocated on a pointer-map page, or the
    ** PENDING_BYTE page.
    */
    if( pgnoRoot==PTRMAP_PAGENO(pBt, pgnoRoot) ||
        pgnoRoot==PENDING_BYTE_PAGE(pBt) ){
      pgnoRoot++;
    }
    assert( pgnoRoot>=3 );

    /* Allocate a page. The page that currently resides at pgnoRoot will
    ** be moved to the allocated page (unless the allocated page happens
    ** to reside at pgnoRoot).
    */
    rc = allocateBtreePage(pBt, &pPageMove, &pgnoMove, pgnoRoot, 1);
    if( rc!=SQLITE_OK ){
      return rc;
    }

    if( pgnoMove!=pgnoRoot ){
      /* pgnoRoot is the page that will be used for the root-page of
      ** the new table (assuming an error did not occur). But we were
      ** allocated pgnoMove. If required (i.e. if it was not allocated
      ** by extending the file), the current page at position pgnoMove
      ** is already journaled.
      */
      u8 eType;
      Pgno iPtrPage;

      releasePage(pPageMove);

      /* Move the page currently at pgnoRoot to pgnoMove. */
      rc = getPage(pBt, pgnoRoot, &pRoot, 0);
      if( rc!=SQLITE_OK ){
        return rc;
      }
      rc = ptrmapGet(pBt, pgnoRoot, &eType, &iPtrPage);
      if( rc!=SQLITE_OK || eType==PTRMAP_ROOTPAGE || eType==PTRMAP_FREEPAGE ){
        releasePage(pRoot);
        return rc;
      }
      assert( eType!=PTRMAP_ROOTPAGE );
      assert( eType!=PTRMAP_FREEPAGE );
      rc = sqlite3PagerWrite(pRoot->pDbPage);
      if( rc!=SQLITE_OK ){
        releasePage(pRoot);
        return rc;
      }
      rc = relocatePage(pBt, pRoot, eType, iPtrPage, pgnoMove);
      releasePage(pRoot);

      /* Obtain the page at pgnoRoot */
      if( rc!=SQLITE_OK ){
        return rc;
      }
      rc = getPage(pBt, pgnoRoot, &pRoot, 0);
      if( rc!=SQLITE_OK ){
        return rc;
      }
      rc = sqlite3PagerWrite(pRoot->pDbPage);
      if( rc!=SQLITE_OK ){
        releasePage(pRoot);
        return rc;
      }
    }else{
      pRoot = pPageMove;
    } 

    /* Update the pointer-map and meta-data with the new root-page number. */
    rc = ptrmapPut(pBt, pgnoRoot, PTRMAP_ROOTPAGE, 0);
    if( rc ){
      releasePage(pRoot);
      return rc;
    }
    rc = sqlite3BtreeUpdateMeta(p, 4, pgnoRoot);
    if( rc ){
      releasePage(pRoot);
      return rc;
    }

  }else{
    rc = allocateBtreePage(pBt, &pRoot, &pgnoRoot, 1, 0);
    if( rc ) return rc;
  }
#endif
  assert( sqlite3PagerIswriteable(pRoot->pDbPage) );
  zeroPage(pRoot, flags | PTF_LEAF);
  sqlite3PagerUnref(pRoot->pDbPage);
  *piTable = (int)pgnoRoot;
  return SQLITE_OK;
}

/*
** Erase the given database page and all its children.  Return
** the page to the freelist.
*/
static int clearDatabasePage(
  BtShared *pBt,           /* The BTree that contains the table */
  Pgno pgno,            /* Page number to clear */
  MemPage *pParent,     /* Parent page.  NULL for the root */
  int freePageFlag      /* Deallocate page if true */
){
  MemPage *pPage = 0;
  int rc;
  unsigned char *pCell;
  int i;

  if( pgno>sqlite3PagerPagecount(pBt->pPager) ){
    return SQLITE_CORRUPT_BKPT;
  }

  rc = getAndInitPage(pBt, pgno, &pPage, pParent);
  if( rc ) goto cleardatabasepage_out;
  for(i=0; i<pPage->nCell; i++){
    pCell = findCell(pPage, i);
    if( !pPage->leaf ){
      rc = clearDatabasePage(pBt, get4byte(pCell), pPage->pParent, 1);
      if( rc ) goto cleardatabasepage_out;
    }
    rc = clearCell(pPage, pCell);
    if( rc ) goto cleardatabasepage_out;
  }
  if( !pPage->leaf ){
    rc = clearDatabasePage(pBt, get4byte(&pPage->aData[8]), pPage->pParent, 1);
    if( rc ) goto cleardatabasepage_out;
  }
  if( freePageFlag ){
    rc = freePage(pPage);
  }else if( (rc = sqlite3PagerWrite(pPage->pDbPage))==0 ){
    zeroPage(pPage, pPage->aData[0] | PTF_LEAF);
  }

cleardatabasepage_out:
  releasePage(pPage);
  return rc;
}

/*
** Delete all information from a single table in the database.  iTable is
** the page number of the root of the table.  After this routine returns,
** the root page is empty, but still exists.
**
** This routine will fail with SQLITE_LOCKED if there are any open
** read cursors on the table.  Open write cursors are moved to the
** root of the table.
*/
int sqlite3BtreeClearTable(Btree *p, int iTable){
  int rc;
  BtShared *pBt = p->pBt;
  if( p->inTrans!=TRANS_WRITE ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  rc = checkReadLocks(p, iTable, 0);
  if( rc ){
    return rc;
  }

  /* Save the position of all cursors open on this table */
  if( SQLITE_OK!=(rc = saveAllCursors(pBt, iTable, 0)) ){
    return rc;
  }

  return clearDatabasePage(pBt, (Pgno)iTable, 0, 0);
}

/*
** Erase all information in a table and add the root of the table to
** the freelist.  Except, the root of the principle table (the one on
** page 1) is never added to the freelist.
**
** This routine will fail with SQLITE_LOCKED if there are any open
** cursors on the table.
**
** If AUTOVACUUM is enabled and the page at iTable is not the last
** root page in the database file, then the last root page 
** in the database file is moved into the slot formerly occupied by
** iTable and that last slot formerly occupied by the last root page
** is added to the freelist instead of iTable.  In this say, all
** root pages are kept at the beginning of the database file, which
** is necessary for AUTOVACUUM to work right.  *piMoved is set to the 
** page number that used to be the last root page in the file before
** the move.  If no page gets moved, *piMoved is set to 0.
** The last root page is recorded in meta[3] and the value of
** meta[3] is updated by this procedure.
*/
int sqlite3BtreeDropTable(Btree *p, int iTable, int *piMoved){
  int rc;
  MemPage *pPage = 0;
  BtShared *pBt = p->pBt;

  if( p->inTrans!=TRANS_WRITE ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }

  /* It is illegal to drop a table if any cursors are open on the
  ** database. This is because in auto-vacuum mode the backend may
  ** need to move another root-page to fill a gap left by the deleted
  ** root page. If an open cursor was using this page a problem would 
  ** occur.
  */
  if( pBt->pCursor ){
    return SQLITE_LOCKED;
  }

  rc = getPage(pBt, (Pgno)iTable, &pPage, 0);
  if( rc ) return rc;
  rc = sqlite3BtreeClearTable(p, iTable);
  if( rc ){
    releasePage(pPage);
    return rc;
  }

  *piMoved = 0;

  if( iTable>1 ){
#ifdef SQLITE_OMIT_AUTOVACUUM
    rc = freePage(pPage);
    releasePage(pPage);
#else
    if( pBt->autoVacuum ){
      Pgno maxRootPgno;
      rc = sqlite3BtreeGetMeta(p, 4, &maxRootPgno);
      if( rc!=SQLITE_OK ){
        releasePage(pPage);
        return rc;
      }

      if( iTable==maxRootPgno ){
        /* If the table being dropped is the table with the largest root-page
        ** number in the database, put the root page on the free list. 
        */
        rc = freePage(pPage);
        releasePage(pPage);
        if( rc!=SQLITE_OK ){
          return rc;
        }
      }else{
        /* The table being dropped does not have the largest root-page
        ** number in the database. So move the page that does into the 
        ** gap left by the deleted root-page.
        */
        MemPage *pMove;
        releasePage(pPage);
        rc = getPage(pBt, maxRootPgno, &pMove, 0);
        if( rc!=SQLITE_OK ){
          return rc;
        }
        rc = relocatePage(pBt, pMove, PTRMAP_ROOTPAGE, 0, iTable);
        releasePage(pMove);
        if( rc!=SQLITE_OK ){
          return rc;
        }
        rc = getPage(pBt, maxRootPgno, &pMove, 0);
        if( rc!=SQLITE_OK ){
          return rc;
        }
        rc = freePage(pMove);
        releasePage(pMove);
        if( rc!=SQLITE_OK ){
          return rc;
        }
        *piMoved = maxRootPgno;
      }

      /* Set the new 'max-root-page' value in the database header. This
      ** is the old value less one, less one more if that happens to
      ** be a root-page number, less one again if that is the
      ** PENDING_BYTE_PAGE.
      */
      maxRootPgno--;
      if( maxRootPgno==PENDING_BYTE_PAGE(pBt) ){
        maxRootPgno--;
      }
      if( maxRootPgno==PTRMAP_PAGENO(pBt, maxRootPgno) ){
        maxRootPgno--;
      }
      assert( maxRootPgno!=PENDING_BYTE_PAGE(pBt) );

      rc = sqlite3BtreeUpdateMeta(p, 4, maxRootPgno);
    }else{
      rc = freePage(pPage);
      releasePage(pPage);
    }
#endif
  }else{
    /* If sqlite3BtreeDropTable was called on page 1. */
    zeroPage(pPage, PTF_INTKEY|PTF_LEAF );
    releasePage(pPage);
  }
  return rc;  
}


/*
** Read the meta-information out of a database file.  Meta[0]
** is the number of free pages currently in the database.  Meta[1]
** through meta[15] are available for use by higher layers.  Meta[0]
** is read-only, the others are read/write.
** 
** The schema layer numbers meta values differently.  At the schema
** layer (and the SetCookie and ReadCookie opcodes) the number of
** free pages is not visible.  So Cookie[0] is the same as Meta[1].
*/
int sqlite3BtreeGetMeta(Btree *p, int idx, u32 *pMeta){
  DbPage *pDbPage;
  int rc;
  unsigned char *pP1;
  BtShared *pBt = p->pBt;

  /* Reading a meta-data value requires a read-lock on page 1 (and hence
  ** the sqlite_master table. We grab this lock regardless of whether or
  ** not the SQLITE_ReadUncommitted flag is set (the table rooted at page
  ** 1 is treated as a special case by queryTableLock() and lockTable()).
  */
  rc = queryTableLock(p, 1, READ_LOCK);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  assert( idx>=0 && idx<=15 );
  rc = sqlite3PagerGet(pBt->pPager, 1, &pDbPage);
  if( rc ) return rc;
  pP1 = (unsigned char *)sqlite3PagerGetData(pDbPage);
  *pMeta = get4byte(&pP1[36 + idx*4]);
  sqlite3PagerUnref(pDbPage);

  /* If autovacuumed is disabled in this build but we are trying to 
  ** access an autovacuumed database, then make the database readonly. 
  */
#ifdef SQLITE_OMIT_AUTOVACUUM
  if( idx==4 && *pMeta>0 ) pBt->readOnly = 1;
#endif

  /* Grab the read-lock on page 1. */
  rc = lockTable(p, 1, READ_LOCK);
  return rc;
}

/*
** Write meta-information back into the database.  Meta[0] is
** read-only and may not be written.
*/
int sqlite3BtreeUpdateMeta(Btree *p, int idx, u32 iMeta){
  BtShared *pBt = p->pBt;
  unsigned char *pP1;
  int rc;
  assert( idx>=1 && idx<=15 );
  if( p->inTrans!=TRANS_WRITE ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( pBt->pPage1!=0 );
  pP1 = pBt->pPage1->aData;
  rc = sqlite3PagerWrite(pBt->pPage1->pDbPage);
  if( rc ) return rc;
  put4byte(&pP1[36 + idx*4], iMeta);
  return SQLITE_OK;
}

/*
** Return the flag byte at the beginning of the page that the cursor
** is currently pointing to.
*/
int sqlite3BtreeFlags(BtCursor *pCur){
  /* TODO: What about CURSOR_REQUIRESEEK state? Probably need to call
  ** restoreOrClearCursorPosition() here.
  */
  MemPage *pPage = pCur->pPage;
  return pPage ? pPage->aData[pPage->hdrOffset] : 0;
}

#ifdef SQLITE_DEBUG
/*
** Print a disassembly of the given page on standard output.  This routine
** is used for debugging and testing only.
*/
static int btreePageDump(BtShared *pBt, int pgno, int recursive, MemPage *pParent){
  int rc;
  MemPage *pPage;
  int i, j, c;
  int nFree;
  u16 idx;
  int hdr;
  int nCell;
  int isInit;
  unsigned char *data;
  char range[20];
  unsigned char payload[20];

  rc = getPage(pBt, (Pgno)pgno, &pPage, 0);
  isInit = pPage->isInit;
  if( pPage->isInit==0 ){
    initPage(pPage, pParent);
  }
  if( rc ){
    return rc;
  }
  hdr = pPage->hdrOffset;
  data = pPage->aData;
  c = data[hdr];
  pPage->intKey = (c & (PTF_INTKEY|PTF_LEAFDATA))!=0;
  pPage->zeroData = (c & PTF_ZERODATA)!=0;
  pPage->leafData = (c & PTF_LEAFDATA)!=0;
  pPage->leaf = (c & PTF_LEAF)!=0;
  pPage->hasData = !(pPage->zeroData || (!pPage->leaf && pPage->leafData));
  nCell = get2byte(&data[hdr+3]);
  sqlite3DebugPrintf("PAGE %d:  flags=0x%02x  frag=%d   parent=%d\n", pgno,
    data[hdr], data[hdr+7], 
    (pPage->isInit && pPage->pParent) ? pPage->pParent->pgno : 0);
  assert( hdr == (pgno==1 ? 100 : 0) );
  idx = hdr + 12 - pPage->leaf*4;
  for(i=0; i<nCell; i++){
    CellInfo info;
    Pgno child;
    unsigned char *pCell;
    int sz;
    int addr;

    addr = get2byte(&data[idx + 2*i]);
    pCell = &data[addr];
    parseCellPtr(pPage, pCell, &info);
    sz = info.nSize;
    sprintf(range,"%d..%d", addr, addr+sz-1);
    if( pPage->leaf ){
      child = 0;
    }else{
      child = get4byte(pCell);
    }
    sz = info.nData;
    if( !pPage->intKey ) sz += info.nKey;
    if( sz>sizeof(payload)-1 ) sz = sizeof(payload)-1;
    memcpy(payload, &pCell[info.nHeader], sz);
    for(j=0; j<sz; j++){
      if( payload[j]<0x20 || payload[j]>0x7f ) payload[j] = '.';
    }
    payload[sz] = 0;
    sqlite3DebugPrintf(
      "cell %2d: i=%-10s chld=%-4d nk=%-4lld nd=%-4d payload=%s\n",
      i, range, child, info.nKey, info.nData, payload
    );
  }
  if( !pPage->leaf ){
    sqlite3DebugPrintf("right_child: %d\n", get4byte(&data[hdr+8]));
  }
  nFree = 0;
  i = 0;
  idx = get2byte(&data[hdr+1]);
  while( idx>0 && idx<pPage->pBt->usableSize ){
    int sz = get2byte(&data[idx+2]);
    sprintf(range,"%d..%d", idx, idx+sz-1);
    nFree += sz;
    sqlite3DebugPrintf("freeblock %2d: i=%-10s size=%-4d total=%d\n",
       i, range, sz, nFree);
    idx = get2byte(&data[idx]);
    i++;
  }
  if( idx!=0 ){
    sqlite3DebugPrintf("ERROR: next freeblock index out of range: %d\n", idx);
  }
  if( recursive && !pPage->leaf ){
    for(i=0; i<nCell; i++){
      unsigned char *pCell = findCell(pPage, i);
      btreePageDump(pBt, get4byte(pCell), 1, pPage);
      idx = get2byte(pCell);
    }
    btreePageDump(pBt, get4byte(&data[hdr+8]), 1, pPage);
  }
  pPage->isInit = isInit;
  sqlite3PagerUnref(pPage->pDbPage);
  fflush(stdout);
  return SQLITE_OK;
}
int sqlite3BtreePageDump(Btree *p, int pgno, int recursive){
  return btreePageDump(p->pBt, pgno, recursive, 0);
}
#endif

#if defined(SQLITE_TEST) || defined(SQLITE_DEBUG)
/*
** Fill aResult[] with information about the entry and page that the
** cursor is pointing to.
** 
**   aResult[0] =  The page number
**   aResult[1] =  The entry number
**   aResult[2] =  Total number of entries on this page
**   aResult[3] =  Cell size (local payload + header)
**   aResult[4] =  Number of free bytes on this page
**   aResult[5] =  Number of free blocks on the page
**   aResult[6] =  Total payload size (local + overflow)
**   aResult[7] =  Header size in bytes
**   aResult[8] =  Local payload size
**   aResult[9] =  Parent page number
**   aResult[10]=  Page number of the first overflow page
**
** This routine is used for testing and debugging only.
*/
int sqlite3BtreeCursorInfo(BtCursor *pCur, int *aResult, int upCnt){
  int cnt, idx;
  MemPage *pPage = pCur->pPage;
  BtCursor tmpCur;

  int rc = restoreOrClearCursorPosition(pCur);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  assert( pPage->isInit );
  getTempCursor(pCur, &tmpCur);
  while( upCnt-- ){
    moveToParent(&tmpCur);
  }
  pPage = tmpCur.pPage;
  aResult[0] = sqlite3PagerPagenumber(pPage->pDbPage);
  assert( aResult[0]==pPage->pgno );
  aResult[1] = tmpCur.idx;
  aResult[2] = pPage->nCell;
  if( tmpCur.idx>=0 && tmpCur.idx<pPage->nCell ){
    getCellInfo(&tmpCur);
    aResult[3] = tmpCur.info.nSize;
    aResult[6] = tmpCur.info.nData;
    aResult[7] = tmpCur.info.nHeader;
    aResult[8] = tmpCur.info.nLocal;
  }else{
    aResult[3] = 0;
    aResult[6] = 0;
    aResult[7] = 0;
    aResult[8] = 0;
  }
  aResult[4] = pPage->nFree;
  cnt = 0;
  idx = get2byte(&pPage->aData[pPage->hdrOffset+1]);
  while( idx>0 && idx<pPage->pBt->usableSize ){
    cnt++;
    idx = get2byte(&pPage->aData[idx]);
  }
  aResult[5] = cnt;
  if( pPage->pParent==0 || isRootPage(pPage) ){
    aResult[9] = 0;
  }else{
    aResult[9] = pPage->pParent->pgno;
  }
  if( tmpCur.info.iOverflow ){
    aResult[10] = get4byte(&tmpCur.info.pCell[tmpCur.info.iOverflow]);
  }else{
    aResult[10] = 0;
  }
  releaseTempCursor(&tmpCur);
  return SQLITE_OK;
}
#endif

/*
** Return the pager associated with a BTree.  This routine is used for
** testing and debugging only.
*/
Pager *sqlite3BtreePager(Btree *p){
  return p->pBt->pPager;
}

/*
** This structure is passed around through all the sanity checking routines
** in order to keep track of some global state information.
*/
typedef struct IntegrityCk IntegrityCk;
struct IntegrityCk {
  BtShared *pBt;    /* The tree being checked out */
  Pager *pPager;    /* The associated pager.  Also accessible by pBt->pPager */
  int nPage;        /* Number of pages in the database */
  int *anRef;       /* Number of times each page is referenced */
  int mxErr;        /* Stop accumulating errors when this reaches zero */
  char *zErrMsg;    /* An error message.  NULL if no errors seen. */
  int nErr;         /* Number of messages written to zErrMsg so far */
};

#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/*
** Append a message to the error message string.
*/
static void checkAppendMsg(
  IntegrityCk *pCheck,
  char *zMsg1,
  const char *zFormat,
  ...
){
  va_list ap;
  char *zMsg2;
  if( !pCheck->mxErr ) return;
  pCheck->mxErr--;
  pCheck->nErr++;
  va_start(ap, zFormat);
  zMsg2 = sqlite3VMPrintf(zFormat, ap);
  va_end(ap);
  if( zMsg1==0 ) zMsg1 = "";
  if( pCheck->zErrMsg ){
    char *zOld = pCheck->zErrMsg;
    pCheck->zErrMsg = 0;
    sqlite3SetString(&pCheck->zErrMsg, zOld, "\n", zMsg1, zMsg2, (char*)0);
    sqliteFree(zOld);
  }else{
    sqlite3SetString(&pCheck->zErrMsg, zMsg1, zMsg2, (char*)0);
  }
  sqliteFree(zMsg2);
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/*
** Add 1 to the reference count for page iPage.  If this is the second
** reference to the page, add an error message to pCheck->zErrMsg.
** Return 1 if there are 2 ore more references to the page and 0 if
** if this is the first reference to the page.
**
** Also check that the page number is in bounds.
*/
static int checkRef(IntegrityCk *pCheck, int iPage, char *zContext){
  if( iPage==0 ) return 1;
  if( iPage>pCheck->nPage || iPage<0 ){
    checkAppendMsg(pCheck, zContext, "invalid page number %d", iPage);
    return 1;
  }
  if( pCheck->anRef[iPage]==1 ){
    checkAppendMsg(pCheck, zContext, "2nd reference to page %d", iPage);
    return 1;
  }
  return  (pCheck->anRef[iPage]++)>1;
}

#ifndef SQLITE_OMIT_AUTOVACUUM
/*
** Check that the entry in the pointer-map for page iChild maps to 
** page iParent, pointer type ptrType. If not, append an error message
** to pCheck.
*/
static void checkPtrmap(
  IntegrityCk *pCheck,   /* Integrity check context */
  Pgno iChild,           /* Child page number */
  u8 eType,              /* Expected pointer map type */
  Pgno iParent,          /* Expected pointer map parent page number */
  char *zContext         /* Context description (used for error msg) */
){
  int rc;
  u8 ePtrmapType;
  Pgno iPtrmapParent;

  rc = ptrmapGet(pCheck->pBt, iChild, &ePtrmapType, &iPtrmapParent);
  if( rc!=SQLITE_OK ){
    checkAppendMsg(pCheck, zContext, "Failed to read ptrmap key=%d", iChild);
    return;
  }

  if( ePtrmapType!=eType || iPtrmapParent!=iParent ){
    checkAppendMsg(pCheck, zContext, 
      "Bad ptr map entry key=%d expected=(%d,%d) got=(%d,%d)", 
      iChild, eType, iParent, ePtrmapType, iPtrmapParent);
  }
}
#endif

/*
** Check the integrity of the freelist or of an overflow page list.
** Verify that the number of pages on the list is N.
*/
static void checkList(
  IntegrityCk *pCheck,  /* Integrity checking context */
  int isFreeList,       /* True for a freelist.  False for overflow page list */
  int iPage,            /* Page number for first page in the list */
  int N,                /* Expected number of pages in the list */
  char *zContext        /* Context for error messages */
){
  int i;
  int expected = N;
  int iFirst = iPage;
  while( N-- > 0 && pCheck->mxErr ){
    DbPage *pOvflPage;
    unsigned char *pOvflData;
    if( iPage<1 ){
      checkAppendMsg(pCheck, zContext,
         "%d of %d pages missing from overflow list starting at %d",
          N+1, expected, iFirst);
      break;
    }
    if( checkRef(pCheck, iPage, zContext) ) break;
    if( sqlite3PagerGet(pCheck->pPager, (Pgno)iPage, &pOvflPage) ){
      checkAppendMsg(pCheck, zContext, "failed to get page %d", iPage);
      break;
    }
    pOvflData = (unsigned char *)sqlite3PagerGetData(pOvflPage);
    if( isFreeList ){
      int n = get4byte(&pOvflData[4]);
#ifndef SQLITE_OMIT_AUTOVACUUM
      if( pCheck->pBt->autoVacuum ){
        checkPtrmap(pCheck, iPage, PTRMAP_FREEPAGE, 0, zContext);
      }
#endif
      if( n>pCheck->pBt->usableSize/4-8 ){
        checkAppendMsg(pCheck, zContext,
           "freelist leaf count too big on page %d", iPage);
        N--;
      }else{
        for(i=0; i<n; i++){
          Pgno iFreePage = get4byte(&pOvflData[8+i*4]);
#ifndef SQLITE_OMIT_AUTOVACUUM
          if( pCheck->pBt->autoVacuum ){
            checkPtrmap(pCheck, iFreePage, PTRMAP_FREEPAGE, 0, zContext);
          }
#endif
          checkRef(pCheck, iFreePage, zContext);
        }
        N -= n;
      }
    }
#ifndef SQLITE_OMIT_AUTOVACUUM
    else{
      /* If this database supports auto-vacuum and iPage is not the last
      ** page in this overflow list, check that the pointer-map entry for
      ** the following page matches iPage.
      */
      if( pCheck->pBt->autoVacuum && N>0 ){
        i = get4byte(pOvflData);
        checkPtrmap(pCheck, i, PTRMAP_OVERFLOW2, iPage, zContext);
      }
    }
#endif
    iPage = get4byte(pOvflData);
    sqlite3PagerUnref(pOvflPage);
  }
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/*
** Do various sanity checks on a single page of a tree.  Return
** the tree depth.  Root pages return 0.  Parents of root pages
** return 1, and so forth.
** 
** These checks are done:
**
**      1.  Make sure that cells and freeblocks do not overlap
**          but combine to completely cover the page.
**  NO  2.  Make sure cell keys are in order.
**  NO  3.  Make sure no key is less than or equal to zLowerBound.
**  NO  4.  Make sure no key is greater than or equal to zUpperBound.
**      5.  Check the integrity of overflow pages.
**      6.  Recursively call checkTreePage on all children.
**      7.  Verify that the depth of all children is the same.
**      8.  Make sure this page is at least 33% full or else it is
**          the root of the tree.
*/
static int checkTreePage(
  IntegrityCk *pCheck,  /* Context for the sanity check */
  int iPage,            /* Page number of the page to check */
  MemPage *pParent,     /* Parent page */
  char *zParentContext  /* Parent context */
){
  MemPage *pPage;
  int i, rc, depth, d2, pgno, cnt;
  int hdr, cellStart;
  int nCell;
  u8 *data;
  BtShared *pBt;
  int usableSize;
  char zContext[100];
  char *hit;

  sprintf(zContext, "Page %d: ", iPage);

  /* Check that the page exists
  */
  pBt = pCheck->pBt;
  usableSize = pBt->usableSize;
  if( iPage==0 ) return 0;
  if( checkRef(pCheck, iPage, zParentContext) ) return 0;
  if( (rc = getPage(pBt, (Pgno)iPage, &pPage, 0))!=0 ){
    checkAppendMsg(pCheck, zContext,
       "unable to get the page. error code=%d", rc);
    return 0;
  }
  if( (rc = initPage(pPage, pParent))!=0 ){
    checkAppendMsg(pCheck, zContext, "initPage() returns error code %d", rc);
    releasePage(pPage);
    return 0;
  }

  /* Check out all the cells.
  */
  depth = 0;
  for(i=0; i<pPage->nCell && pCheck->mxErr; i++){
    u8 *pCell;
    int sz;
    CellInfo info;

    /* Check payload overflow pages
    */
    sprintf(zContext, "On tree page %d cell %d: ", iPage, i);
    pCell = findCell(pPage,i);
    parseCellPtr(pPage, pCell, &info);
    sz = info.nData;
    if( !pPage->intKey ) sz += info.nKey;
    assert( sz==info.nPayload );
    if( sz>info.nLocal ){
      int nPage = (sz - info.nLocal + usableSize - 5)/(usableSize - 4);
      Pgno pgnoOvfl = get4byte(&pCell[info.iOverflow]);
#ifndef SQLITE_OMIT_AUTOVACUUM
      if( pBt->autoVacuum ){
        checkPtrmap(pCheck, pgnoOvfl, PTRMAP_OVERFLOW1, iPage, zContext);
      }
#endif
      checkList(pCheck, 0, pgnoOvfl, nPage, zContext);
    }

    /* Check sanity of left child page.
    */
    if( !pPage->leaf ){
      pgno = get4byte(pCell);
#ifndef SQLITE_OMIT_AUTOVACUUM
      if( pBt->autoVacuum ){
        checkPtrmap(pCheck, pgno, PTRMAP_BTREE, iPage, zContext);
      }
#endif
      d2 = checkTreePage(pCheck,pgno,pPage,zContext);
      if( i>0 && d2!=depth ){
        checkAppendMsg(pCheck, zContext, "Child page depth differs");
      }
      depth = d2;
    }
  }
  if( !pPage->leaf ){
    pgno = get4byte(&pPage->aData[pPage->hdrOffset+8]);
    sprintf(zContext, "On page %d at right child: ", iPage);
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pBt->autoVacuum ){
      checkPtrmap(pCheck, pgno, PTRMAP_BTREE, iPage, 0);
    }
#endif
    checkTreePage(pCheck, pgno, pPage, zContext);
  }
 
  /* Check for complete coverage of the page
  */
  data = pPage->aData;
  hdr = pPage->hdrOffset;
  hit = sqliteMalloc( usableSize );
  if( hit ){
    memset(hit, 1, get2byte(&data[hdr+5]));
    nCell = get2byte(&data[hdr+3]);
    cellStart = hdr + 12 - 4*pPage->leaf;
    for(i=0; i<nCell; i++){
      int pc = get2byte(&data[cellStart+i*2]);
      int size = cellSizePtr(pPage, &data[pc]);
      int j;
      if( (pc+size-1)>=usableSize || pc<0 ){
        checkAppendMsg(pCheck, 0, 
            "Corruption detected in cell %d on page %d",i,iPage,0);
      }else{
        for(j=pc+size-1; j>=pc; j--) hit[j]++;
      }
    }
    for(cnt=0, i=get2byte(&data[hdr+1]); i>0 && i<usableSize && cnt<10000; 
           cnt++){
      int size = get2byte(&data[i+2]);
      int j;
      if( (i+size-1)>=usableSize || i<0 ){
        checkAppendMsg(pCheck, 0,  
            "Corruption detected in cell %d on page %d",i,iPage,0);
      }else{
        for(j=i+size-1; j>=i; j--) hit[j]++;
      }
      i = get2byte(&data[i]);
    }
    for(i=cnt=0; i<usableSize; i++){
      if( hit[i]==0 ){
        cnt++;
      }else if( hit[i]>1 ){
        checkAppendMsg(pCheck, 0,
          "Multiple uses for byte %d of page %d", i, iPage);
        break;
      }
    }
    if( cnt!=data[hdr+7] ){
      checkAppendMsg(pCheck, 0, 
          "Fragmented space is %d byte reported as %d on page %d",
          cnt, data[hdr+7], iPage);
    }
  }
  sqliteFree(hit);

  releasePage(pPage);
  return depth+1;
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/*
** This routine does a complete check of the given BTree file.  aRoot[] is
** an array of pages numbers were each page number is the root page of
** a table.  nRoot is the number of entries in aRoot.
**
** If everything checks out, this routine returns NULL.  If something is
** amiss, an error message is written into memory obtained from malloc()
** and a pointer to that error message is returned.  The calling function
** is responsible for freeing the error message when it is done.
*/
char *sqlite3BtreeIntegrityCheck(
  Btree *p,     /* The btree to be checked */
  int *aRoot,   /* An array of root pages numbers for individual trees */
  int nRoot,    /* Number of entries in aRoot[] */
  int mxErr,    /* Stop reporting errors after this many */
  int *pnErr    /* Write number of errors seen to this variable */
){
  int i;
  int nRef;
  IntegrityCk sCheck;
  BtShared *pBt = p->pBt;

  nRef = sqlite3PagerRefcount(pBt->pPager);
  if( lockBtreeWithRetry(p)!=SQLITE_OK ){
    return sqliteStrDup("Unable to acquire a read lock on the database");
  }
  sCheck.pBt = pBt;
  sCheck.pPager = pBt->pPager;
  sCheck.nPage = sqlite3PagerPagecount(sCheck.pPager);
  sCheck.mxErr = mxErr;
  sCheck.nErr = 0;
  *pnErr = 0;
  if( sCheck.nPage==0 ){
    unlockBtreeIfUnused(pBt);
    return 0;
  }
  sCheck.anRef = sqliteMallocRaw( (sCheck.nPage+1)*sizeof(sCheck.anRef[0]) );
  if( !sCheck.anRef ){
    unlockBtreeIfUnused(pBt);
    *pnErr = 1;
    return sqlite3MPrintf("Unable to malloc %d bytes", 
        (sCheck.nPage+1)*sizeof(sCheck.anRef[0]));
  }
  for(i=0; i<=sCheck.nPage; i++){ sCheck.anRef[i] = 0; }
  i = PENDING_BYTE_PAGE(pBt);
  if( i<=sCheck.nPage ){
    sCheck.anRef[i] = 1;
  }
  sCheck.zErrMsg = 0;

  /* Check the integrity of the freelist
  */
  checkList(&sCheck, 1, get4byte(&pBt->pPage1->aData[32]),
            get4byte(&pBt->pPage1->aData[36]), "Main freelist: ");

  /* Check all the tables.
  */
  for(i=0; i<nRoot && sCheck.mxErr; i++){
    if( aRoot[i]==0 ) continue;
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( pBt->autoVacuum && aRoot[i]>1 ){
      checkPtrmap(&sCheck, aRoot[i], PTRMAP_ROOTPAGE, 0, 0);
    }
#endif
    checkTreePage(&sCheck, aRoot[i], 0, "List of tree roots: ");
  }

  /* Make sure every page in the file is referenced
  */
  for(i=1; i<=sCheck.nPage && sCheck.mxErr; i++){
#ifdef SQLITE_OMIT_AUTOVACUUM
    if( sCheck.anRef[i]==0 ){
      checkAppendMsg(&sCheck, 0, "Page %d is never used", i);
    }
#else
    /* If the database supports auto-vacuum, make sure no tables contain
    ** references to pointer-map pages.
    */
    if( sCheck.anRef[i]==0 && 
       (PTRMAP_PAGENO(pBt, i)!=i || !pBt->autoVacuum) ){
      checkAppendMsg(&sCheck, 0, "Page %d is never used", i);
    }
    if( sCheck.anRef[i]!=0 && 
       (PTRMAP_PAGENO(pBt, i)==i && pBt->autoVacuum) ){
      checkAppendMsg(&sCheck, 0, "Pointer map page %d is referenced", i);
    }
#endif
  }

  /* Make sure this analysis did not leave any unref() pages
  */
  unlockBtreeIfUnused(pBt);
  if( nRef != sqlite3PagerRefcount(pBt->pPager) ){
    checkAppendMsg(&sCheck, 0, 
      "Outstanding page count goes from %d to %d during this analysis",
      nRef, sqlite3PagerRefcount(pBt->pPager)
    );
  }

  /* Clean  up and report errors.
  */
  sqliteFree(sCheck.anRef);
  *pnErr = sCheck.nErr;
  return sCheck.zErrMsg;
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

/*
** Return the full pathname of the underlying database file.
*/
const char *sqlite3BtreeGetFilename(Btree *p){
  assert( p->pBt->pPager!=0 );
  return sqlite3PagerFilename(p->pBt->pPager);
}

/*
** Return the pathname of the directory that contains the database file.
*/
const char *sqlite3BtreeGetDirname(Btree *p){
  assert( p->pBt->pPager!=0 );
  return sqlite3PagerDirname(p->pBt->pPager);
}

/*
** Return the pathname of the journal file for this database. The return
** value of this routine is the same regardless of whether the journal file
** has been created or not.
*/
const char *sqlite3BtreeGetJournalname(Btree *p){
  assert( p->pBt->pPager!=0 );
  return sqlite3PagerJournalname(p->pBt->pPager);
}

#ifndef SQLITE_OMIT_VACUUM
/*
** Copy the complete content of pBtFrom into pBtTo.  A transaction
** must be active for both files.
**
** The size of file pBtFrom may be reduced by this operation.
** If anything goes wrong, the transaction on pBtFrom is rolled back.
*/
int sqlite3BtreeCopyFile(Btree *pTo, Btree *pFrom){
  int rc = SQLITE_OK;
  Pgno i, nPage, nToPage, iSkip;

  BtShared *pBtTo = pTo->pBt;
  BtShared *pBtFrom = pFrom->pBt;

  if( pTo->inTrans!=TRANS_WRITE || pFrom->inTrans!=TRANS_WRITE ){
    return SQLITE_ERROR;
  }
  if( pBtTo->pCursor ) return SQLITE_BUSY;
  nToPage = sqlite3PagerPagecount(pBtTo->pPager);
  nPage = sqlite3PagerPagecount(pBtFrom->pPager);
  iSkip = PENDING_BYTE_PAGE(pBtTo);
  for(i=1; rc==SQLITE_OK && i<=nPage; i++){
    DbPage *pDbPage;
    if( i==iSkip ) continue;
    rc = sqlite3PagerGet(pBtFrom->pPager, i, &pDbPage);
    if( rc ) break;
    rc = sqlite3PagerOverwrite(pBtTo->pPager, i, sqlite3PagerGetData(pDbPage));
    sqlite3PagerUnref(pDbPage);
  }

  /* If the file is shrinking, journal the pages that are being truncated
  ** so that they can be rolled back if the commit fails.
  */
  for(i=nPage+1; rc==SQLITE_OK && i<=nToPage; i++){
    DbPage *pDbPage;
    if( i==iSkip ) continue;
    rc = sqlite3PagerGet(pBtTo->pPager, i, &pDbPage);
    if( rc ) break;
    rc = sqlite3PagerWrite(pDbPage);
    sqlite3PagerDontWrite(pDbPage);
    /* Yeah.  It seems wierd to call DontWrite() right after Write().  But
    ** that is because the names of those procedures do not exactly 
    ** represent what they do.  Write() really means "put this page in the
    ** rollback journal and mark it as dirty so that it will be written
    ** to the database file later."  DontWrite() undoes the second part of
    ** that and prevents the page from being written to the database.  The
    ** page is still on the rollback journal, though.  And that is the whole
    ** point of this loop: to put pages on the rollback journal. */
    sqlite3PagerUnref(pDbPage);
  }
  if( !rc && nPage<nToPage ){
    rc = sqlite3PagerTruncate(pBtTo->pPager, nPage);
  }

  if( rc ){
    sqlite3BtreeRollback(pTo);
  }
  return rc;  
}
#endif /* SQLITE_OMIT_VACUUM */

/*
** Return non-zero if a transaction is active.
*/
int sqlite3BtreeIsInTrans(Btree *p){
  return (p && (p->inTrans==TRANS_WRITE));
}

/*
** Return non-zero if a statement transaction is active.
*/
int sqlite3BtreeIsInStmt(Btree *p){
  return (p->pBt && p->pBt->inStmt);
}

/*
** Return non-zero if a read (or write) transaction is active.
*/
int sqlite3BtreeIsInReadTrans(Btree *p){
  return (p && (p->inTrans!=TRANS_NONE));
}

/*
** This function returns a pointer to a blob of memory associated with
** a single shared-btree. The memory is used by client code for it's own
** purposes (for example, to store a high-level schema associated with 
** the shared-btree). The btree layer manages reference counting issues.
**
** The first time this is called on a shared-btree, nBytes bytes of memory
** are allocated, zeroed, and returned to the caller. For each subsequent 
** call the nBytes parameter is ignored and a pointer to the same blob
** of memory returned. 
**
** Just before the shared-btree is closed, the function passed as the 
** xFree argument when the memory allocation was made is invoked on the 
** blob of allocated memory. This function should not call sqliteFree()
** on the memory, the btree layer does that.
*/
void *sqlite3BtreeSchema(Btree *p, int nBytes, void(*xFree)(void *)){
  BtShared *pBt = p->pBt;
  if( !pBt->pSchema ){
    pBt->pSchema = sqliteMalloc(nBytes);
    pBt->xFreeSchema = xFree;
  }
  return pBt->pSchema;
}

/*
** Return true if another user of the same shared btree as the argument
** handle holds an exclusive lock on the sqlite_master table.
*/
int sqlite3BtreeSchemaLocked(Btree *p){
  return (queryTableLock(p, MASTER_ROOT, READ_LOCK)!=SQLITE_OK);
}


#ifndef SQLITE_OMIT_SHARED_CACHE
/*
** Obtain a lock on the table whose root page is iTab.  The
** lock is a write lock if isWritelock is true or a read lock
** if it is false.
*/
int sqlite3BtreeLockTable(Btree *p, int iTab, u8 isWriteLock){
  int rc = SQLITE_OK;
  u8 lockType = (isWriteLock?WRITE_LOCK:READ_LOCK);
  rc = queryTableLock(p, iTab, lockType);
  if( rc==SQLITE_OK ){
    rc = lockTable(p, iTab, lockType);
  }
  return rc;
}
#endif

/*
** The following debugging interface has to be in this file (rather
** than in, for example, test1.c) so that it can get access to
** the definition of BtShared.
*/
#if defined(SQLITE_DEBUG) && defined(TCLSH)
#include <tcl.h>
int sqlite3_shared_cache_report(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
#ifndef SQLITE_OMIT_SHARED_CACHE
  const ThreadData *pTd = sqlite3ThreadDataReadOnly();
  if( pTd->useSharedData ){
    BtShared *pBt;
    Tcl_Obj *pRet = Tcl_NewObj();
    for(pBt=pTd->pBtree; pBt; pBt=pBt->pNext){
      const char *zFile = sqlite3PagerFilename(pBt->pPager);
      Tcl_ListObjAppendElement(interp, pRet, Tcl_NewStringObj(zFile, -1));
      Tcl_ListObjAppendElement(interp, pRet, Tcl_NewIntObj(pBt->nRef));
    }
    Tcl_SetObjResult(interp, pRet);
  }
#endif
  return TCL_OK;
}
#endif
