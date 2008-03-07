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
** The file change counter is incremented when the database is changed
** This counter allows other processes to know when the file has changed
** and thus when they need to flush their cache.
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
** cell pointer array, and the cell content area.  Page 1 also has a 100-byte
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
** file header points to the first in a linked list of trunk page.  Each trunk
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

/*
** Page type flags.  An ORed combination of these flags appear as the
** first byte of on-disk image of every BTree page.
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
**
** Access to all fields of this structure is controlled by the mutex
** stored in MemPage.pBt->mutex.
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
  u16 maxLocal;        /* Copy of BtShared.maxLocal or BtShared.maxLeaf */
  u16 minLocal;        /* Copy of BtShared.minLocal or BtShared.minLeaf */
  u16 cellOffset;      /* Index in aData of first cell pointer */
  u16 idxParent;       /* Index in parent of this node */
  u16 nFree;           /* Number of free bytes on the page */
  u16 nCell;           /* Number of cells on this page, local and ovfl */
  struct _OvflCell {   /* Cells that will not fit on aData[] */
    u8 *pCell;          /* Pointers to the body of the overflow cell */
    u16 idx;            /* Insert this cell before idx-th non-overflow cell */
  } aOvfl[5];
  BtShared *pBt;       /* Pointer to BtShared that this page is part of */
  u8 *aData;           /* Pointer to disk image of the page data */
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

/* A Btree handle
**
** A database connection contains a pointer to an instance of
** this object for every database file that it has open.  This structure
** is opaque to the database connection.  The database connection cannot
** see the internals of this structure and only deals with pointers to
** this structure.
**
** For some database files, the same underlying database cache might be 
** shared between multiple connections.  In that case, each contection
** has it own pointer to this object.  But each instance of this object
** points to the same BtShared object.  The database cache and the
** schema associated with the database file are all contained within
** the BtShared object.
**
** All fields in this structure are accessed under sqlite3.mutex.
** The pBt pointer itself may not be changed while there exists cursors 
** in the referenced BtShared that point back to this Btree since those
** cursors have to do go through this Btree to find their BtShared and
** they often do so without holding sqlite3.mutex.
*/
struct Btree {
  sqlite3 *db;       /* The database connection holding this btree */
  BtShared *pBt;     /* Sharable content of this btree */
  u8 inTrans;        /* TRANS_NONE, TRANS_READ or TRANS_WRITE */
  u8 sharable;       /* True if we can share pBt with another db */
  u8 locked;         /* True if db currently has pBt locked */
  int wantToLock;    /* Number of nested calls to sqlite3BtreeEnter() */
  Btree *pNext;      /* List of other sharable Btrees from the same db */
  Btree *pPrev;      /* Back pointer of the same list */
};

/*
** Btree.inTrans may take one of the following values.
**
** If the shared-data extension is enabled, there may be multiple users
** of the Btree structure. At most one of these may open a write transaction,
** but any number may have active read transactions.
*/
#define TRANS_NONE  0
#define TRANS_READ  1
#define TRANS_WRITE 2

/*
** An instance of this object represents a single database file.
** 
** A single database file can be in use as the same time by two
** or more database connections.  When two or more connections are
** sharing the same database file, each connection has it own
** private Btree object for the file and each of those Btrees points
** to this one BtShared object.  BtShared.nRef is the number of
** connections currently sharing this database file.
**
** Fields in this structure are accessed under the BtShared.mutex
** mutex, except for nRef and pNext which are accessed under the
** global SQLITE_MUTEX_STATIC_MASTER mutex.  The pPager field
** may not be modified once it is initially set as long as nRef>0.
** The pSchema field may be set once under BtShared.mutex and
** thereafter is unchanged as long as nRef>0.
*/
struct BtShared {
  Pager *pPager;        /* The page cache */
  sqlite3 *db;          /* Database connection currently using this Btree */
  BtCursor *pCursor;    /* A list of all open cursors */
  MemPage *pPage1;      /* First page of the database */
  u8 inStmt;            /* True if we are in a statement subtransaction */
  u8 readOnly;          /* True if the underlying file is readonly */
  u8 maxEmbedFrac;      /* Maximum payload as % of total page size */
  u8 minEmbedFrac;      /* Minimum payload as % of total page size */
  u8 minLeafFrac;       /* Minimum leaf payload as % of total page size */
  u8 pageSizeFixed;     /* True if the page size can no longer be changed */
#ifndef SQLITE_OMIT_AUTOVACUUM
  u8 autoVacuum;        /* True if auto-vacuum is enabled */
  u8 incrVacuum;        /* True if incr-vacuum is enabled */
  Pgno nTrunc;          /* Non-zero if the db will be truncated (incr vacuum) */
#endif
  u16 pageSize;         /* Total number of bytes on a page */
  u16 usableSize;       /* Number of usable bytes on each page */
  int maxLocal;         /* Maximum local payload in non-LEAFDATA tables */
  int minLocal;         /* Minimum local payload in non-LEAFDATA tables */
  int maxLeaf;          /* Maximum local payload in a LEAFDATA table */
  int minLeaf;          /* Minimum local payload in a LEAFDATA table */
  u8 inTransaction;     /* Transaction state */
  int nTransaction;     /* Number of open transactions (read + write) */
  void *pSchema;        /* Pointer to space allocated by sqlite3BtreeSchema() */
  void (*xFreeSchema)(void*);  /* Destructor for BtShared.pSchema */
  sqlite3_mutex *mutex; /* Non-recursive mutex required to access this struct */
  BusyHandler busyHdr;  /* The busy handler for this btree */
#ifndef SQLITE_OMIT_SHARED_CACHE
  int nRef;             /* Number of references to this structure */
  BtShared *pNext;      /* Next on a list of sharable BtShared structs */
  BtLock *pLock;        /* List of locks held on this shared-btree struct */
  Btree *pExclusive;    /* Btree with an EXCLUSIVE lock on the whole db */
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
** A cursor is a pointer to a particular entry within a particular
** b-tree within a database file.
**
** The entry is identified by its MemPage and the index in
** MemPage.aCell[] of the entry.
**
** When a single database file can shared by two more database connections,
** but cursors cannot be shared.  Each cursor is associated with a
** particular database connection identified BtCursor.pBtree.db.
**
** Fields in this structure are accessed under the BtShared.mutex
** found at self->pBt->mutex. 
*/
struct BtCursor {
  Btree *pBtree;            /* The Btree to which this cursor belongs */
  BtShared *pBt;            /* The BtShared this cursor points to */
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
#ifndef SQLITE_OMIT_INCRBLOB
  u8 isIncrblobHandle;      /* True if this cursor is an incr. io handle */
  Pgno *aOverflow;          /* Cache of overflow page locations */
#endif
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
**
** CURSOR_FAULT:
**   A unrecoverable error (an I/O error or a malloc failure) has occurred
**   on a different connection that shares the BtShared cache with this
**   cursor.  The error has left the cache in an inconsistent state.
**   Do nothing else with this cursor.  Any attempt to use the cursor
**   should return the error code stored in BtCursor.skip
*/
#define CURSOR_INVALID           0
#define CURSOR_VALID             1
#define CURSOR_REQUIRESEEK       2
#define CURSOR_FAULT             3

/*
** The TRACE macro will print high-level status information about the
** btree operation when the global variable sqlite3_btree_trace is
** enabled.
*/
#if SQLITE_TEST
# define TRACE(X)   if( sqlite3_btree_trace ){ printf X; fflush(stdout); }
#else
# define TRACE(X)
#endif

/*
** Routines to read and write variable-length integers.  These used to
** be defined locally, but now we use the varint routines in the util.c
** file.
*/
#define getVarint    sqlite3GetVarint
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

/* A bunch of assert() statements to check the transaction state variables
** of handle p (type Btree*) are internally consistent.
*/
#define btreeIntegrity(p) \
  assert( p->pBt->inTransaction!=TRANS_NONE || p->pBt->nTransaction==0 ); \
  assert( p->pBt->inTransaction>=p->inTrans ); 


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

/*
** Read or write a two- and four-byte big-endian integer values.
*/
#define get2byte(x)   ((x)[0]<<8 | (x)[1])
#define put2byte(p,v) ((p)[0] = (v)>>8, (p)[1] = (v))
#define get4byte sqlite3Get4byte
#define put4byte sqlite3Put4byte

/*
** Internal routines that should be accessed by the btree layer only.
*/
int sqlite3BtreeGetPage(BtShared*, Pgno, MemPage**, int);
int sqlite3BtreeInitPage(MemPage *pPage, MemPage *pParent);
void sqlite3BtreeParseCellPtr(MemPage*, u8*, CellInfo*);
void sqlite3BtreeParseCell(MemPage*, int, CellInfo*);
#ifdef SQLITE_TEST
u8 *sqlite3BtreeFindCell(MemPage *pPage, int iCell);
#endif
int sqlite3BtreeRestoreOrClearCursorPosition(BtCursor *pCur);
void sqlite3BtreeGetTempCursor(BtCursor *pCur, BtCursor *pTempCur);
void sqlite3BtreeReleaseTempCursor(BtCursor *pCur);
int sqlite3BtreeIsRootPage(MemPage *pPage);
void sqlite3BtreeMoveToParent(BtCursor *pCur);
