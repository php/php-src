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
**   |  Ptr(0) | Key(0) | Ptr(1) | Key(1) | ... | Key(N) | Ptr(N+1) |
**   ----------------------------------------------------------------
**
** All of the keys on the page that Ptr(0) points to have values less
** than Key(0).  All of the keys on page Ptr(1) and its subpages have
** values greater than Key(0) and less than Key(1).  All of the keys
** on Ptr(N+1) and its subpages have values greater than Key(N).  And
** so forth.
**
** Finding a particular key requires reading O(log(M)) pages from the 
** disk where M is the number of entries in the tree.
**
** In this implementation, a single file can hold one or more separate 
** BTrees.  Each BTree is identified by the index of its root page.  The
** key and data for any entry are combined to form the "payload".  Up to
** MX_LOCAL_PAYLOAD bytes of payload can be carried directly on the
** database page.  If the payload is larger than MX_LOCAL_PAYLOAD bytes
** then surplus bytes are stored on overflow pages.  The payload for an
** entry and the preceding pointer are combined to form a "Cell".  Each 
** page has a small header which contains the Ptr(N+1) pointer.
**
** The first page of the file contains a magic string used to verify that
** the file really is a valid BTree database, a pointer to a list of unused
** pages in the file, and some meta information.  The root of the first
** BTree begins on page 2 of the file.  (Pages are numbered beginning with
** 1, not 0.)  Thus a minimum database contains 2 pages.
*/
#include "sqliteInt.h"
#include "pager.h"
#include "btree.h"
#include <assert.h>

/* Forward declarations */
static BtOps sqliteBtreeOps;
static BtCursorOps sqliteBtreeCursorOps;

/*
** Macros used for byteswapping.  B is a pointer to the Btree
** structure.  This is needed to access the Btree.needSwab boolean
** in order to tell if byte swapping is needed or not.
** X is an unsigned integer.  SWAB16 byte swaps a 16-bit integer.
** SWAB32 byteswaps a 32-bit integer.
*/
#define SWAB16(B,X)   ((B)->needSwab? swab16((u16)X) : ((u16)X))
#define SWAB32(B,X)   ((B)->needSwab? swab32(X) : (X))
#define SWAB_ADD(B,X,A) \
   if((B)->needSwab){ X=swab32(swab32(X)+A); }else{ X += (A); }

/*
** The following global variable - available only if SQLITE_TEST is
** defined - is used to determine whether new databases are created in
** native byte order or in non-native byte order.  Non-native byte order
** databases are created for testing purposes only.  Under normal operation,
** only native byte-order databases should be created, but we should be
** able to read or write existing databases regardless of the byteorder.
*/
#ifdef SQLITE_TEST
int btree_native_byte_order = 1;
#else
# define btree_native_byte_order 1
#endif

/*
** Forward declarations of structures used only in this file.
*/
typedef struct PageOne PageOne;
typedef struct MemPage MemPage;
typedef struct PageHdr PageHdr;
typedef struct Cell Cell;
typedef struct CellHdr CellHdr;
typedef struct FreeBlk FreeBlk;
typedef struct OverflowPage OverflowPage;
typedef struct FreelistInfo FreelistInfo;

/*
** All structures on a database page are aligned to 4-byte boundries.
** This routine rounds up a number of bytes to the next multiple of 4.
**
** This might need to change for computer architectures that require
** and 8-byte alignment boundry for structures.
*/
#define ROUNDUP(X)  ((X+3) & ~3)

/*
** This is a magic string that appears at the beginning of every
** SQLite database in order to identify the file as a real database.
*/
static const char zMagicHeader[] = 
   "** This file contains an SQLite 2.1 database **";
#define MAGIC_SIZE (sizeof(zMagicHeader))

/*
** This is a magic integer also used to test the integrity of the database
** file.  This integer is used in addition to the string above so that
** if the file is written on a little-endian architecture and read
** on a big-endian architectures (or vice versa) we can detect the
** problem.
**
** The number used was obtained at random and has no special
** significance other than the fact that it represents a different
** integer on little-endian and big-endian machines.
*/
#define MAGIC 0xdae37528

/*
** The first page of the database file contains a magic header string
** to identify the file as an SQLite database file.  It also contains
** a pointer to the first free page of the file.  Page 2 contains the
** root of the principle BTree.  The file might contain other BTrees
** rooted on pages above 2.
**
** The first page also contains SQLITE_N_BTREE_META integers that
** can be used by higher-level routines.
**
** Remember that pages are numbered beginning with 1.  (See pager.c
** for additional information.)  Page 0 does not exist and a page
** number of 0 is used to mean "no such page".
*/
struct PageOne {
  char zMagic[MAGIC_SIZE]; /* String that identifies the file as a database */
  int iMagic;              /* Integer to verify correct byte order */
  Pgno freeList;           /* First free page in a list of all free pages */
  int nFree;               /* Number of pages on the free list */
  int aMeta[SQLITE_N_BTREE_META-1];  /* User defined integers */
};

/*
** Each database page has a header that is an instance of this
** structure.
**
** PageHdr.firstFree is 0 if there is no free space on this page.
** Otherwise, PageHdr.firstFree is the index in MemPage.u.aDisk[] of a 
** FreeBlk structure that describes the first block of free space.  
** All free space is defined by a linked list of FreeBlk structures.
**
** Data is stored in a linked list of Cell structures.  PageHdr.firstCell
** is the index into MemPage.u.aDisk[] of the first cell on the page.  The
** Cells are kept in sorted order.
**
** A Cell contains all information about a database entry and a pointer
** to a child page that contains other entries less than itself.  In
** other words, the i-th Cell contains both Ptr(i) and Key(i).  The
** right-most pointer of the page is contained in PageHdr.rightChild.
*/
struct PageHdr {
  Pgno rightChild;  /* Child page that comes after all cells on this page */
  u16 firstCell;    /* Index in MemPage.u.aDisk[] of the first cell */
  u16 firstFree;    /* Index in MemPage.u.aDisk[] of the first free block */
};

/*
** Entries on a page of the database are called "Cells".  Each Cell
** has a header and data.  This structure defines the header.  The
** key and data (collectively the "payload") follow this header on
** the database page.
**
** A definition of the complete Cell structure is given below.  The
** header for the cell must be defined first in order to do some
** of the sizing #defines that follow.
*/
struct CellHdr {
  Pgno leftChild; /* Child page that comes before this cell */
  u16 nKey;       /* Number of bytes in the key */
  u16 iNext;      /* Index in MemPage.u.aDisk[] of next cell in sorted order */
  u8 nKeyHi;      /* Upper 8 bits of key size for keys larger than 64K bytes */
  u8 nDataHi;     /* Upper 8 bits of data size when the size is more than 64K */
  u16 nData;      /* Number of bytes of data */
};

/*
** The key and data size are split into a lower 16-bit segment and an
** upper 8-bit segment in order to pack them together into a smaller
** space.  The following macros reassembly a key or data size back
** into an integer.
*/
#define NKEY(b,h)  (SWAB16(b,h.nKey) + h.nKeyHi*65536)
#define NDATA(b,h) (SWAB16(b,h.nData) + h.nDataHi*65536)

/*
** The minimum size of a complete Cell.  The Cell must contain a header
** and at least 4 bytes of payload.
*/
#define MIN_CELL_SIZE  (sizeof(CellHdr)+4)

/*
** The maximum number of database entries that can be held in a single
** page of the database. 
*/
#define MX_CELL ((SQLITE_USABLE_SIZE-sizeof(PageHdr))/MIN_CELL_SIZE)

/*
** The amount of usable space on a single page of the BTree.  This is the
** page size minus the overhead of the page header.
*/
#define USABLE_SPACE  (SQLITE_USABLE_SIZE - sizeof(PageHdr))

/*
** The maximum amount of payload (in bytes) that can be stored locally for
** a database entry.  If the entry contains more data than this, the
** extra goes onto overflow pages.
**
** This number is chosen so that at least 4 cells will fit on every page.
*/
#define MX_LOCAL_PAYLOAD ((USABLE_SPACE/4-(sizeof(CellHdr)+sizeof(Pgno)))&~3)

/*
** Data on a database page is stored as a linked list of Cell structures.
** Both the key and the data are stored in aPayload[].  The key always comes
** first.  The aPayload[] field grows as necessary to hold the key and data,
** up to a maximum of MX_LOCAL_PAYLOAD bytes.  If the size of the key and
** data combined exceeds MX_LOCAL_PAYLOAD bytes, then Cell.ovfl is the
** page number of the first overflow page.
**
** Though this structure is fixed in size, the Cell on the database
** page varies in size.  Every cell has a CellHdr and at least 4 bytes
** of payload space.  Additional payload bytes (up to the maximum of
** MX_LOCAL_PAYLOAD) and the Cell.ovfl value are allocated only as
** needed.
*/
struct Cell {
  CellHdr h;                        /* The cell header */
  char aPayload[MX_LOCAL_PAYLOAD];  /* Key and data */
  Pgno ovfl;                        /* The first overflow page */
};

/*
** Free space on a page is remembered using a linked list of the FreeBlk
** structures.  Space on a database page is allocated in increments of
** at least 4 bytes and is always aligned to a 4-byte boundry.  The
** linked list of FreeBlks is always kept in order by address.
*/
struct FreeBlk {
  u16 iSize;      /* Number of bytes in this block of free space */
  u16 iNext;      /* Index in MemPage.u.aDisk[] of the next free block */
};

/*
** The number of bytes of payload that will fit on a single overflow page.
*/
#define OVERFLOW_SIZE (SQLITE_USABLE_SIZE-sizeof(Pgno))

/*
** When the key and data for a single entry in the BTree will not fit in
** the MX_LOCAL_PAYLOAD bytes of space available on the database page,
** then all extra bytes are written to a linked list of overflow pages.
** Each overflow page is an instance of the following structure.
**
** Unused pages in the database are also represented by instances of
** the OverflowPage structure.  The PageOne.freeList field is the
** page number of the first page in a linked list of unused database
** pages.
*/
struct OverflowPage {
  Pgno iNext;
  char aPayload[OVERFLOW_SIZE];
};

/*
** The PageOne.freeList field points to a linked list of overflow pages
** hold information about free pages.  The aPayload section of each
** overflow page contains an instance of the following structure.  The
** aFree[] array holds the page number of nFree unused pages in the disk
** file.
*/
struct FreelistInfo {
  int nFree;
  Pgno aFree[(OVERFLOW_SIZE-sizeof(int))/sizeof(Pgno)];
};

/*
** For every page in the database file, an instance of the following structure
** is stored in memory.  The u.aDisk[] array contains the raw bits read from
** the disk.  The rest is auxiliary information held in memory only. The
** auxiliary info is only valid for regular database pages - it is not
** used for overflow pages and pages on the freelist.
**
** Of particular interest in the auxiliary info is the apCell[] entry.  Each
** apCell[] entry is a pointer to a Cell structure in u.aDisk[].  The cells are
** put in this array so that they can be accessed in constant time, rather
** than in linear time which would be needed if we had to walk the linked 
** list on every access.
**
** Note that apCell[] contains enough space to hold up to two more Cells
** than can possibly fit on one page.  In the steady state, every apCell[]
** points to memory inside u.aDisk[].  But in the middle of an insert
** operation, some apCell[] entries may temporarily point to data space
** outside of u.aDisk[].  This is a transient situation that is quickly
** resolved.  But while it is happening, it is possible for a database
** page to hold as many as two more cells than it might otherwise hold.
** The extra two entries in apCell[] are an allowance for this situation.
**
** The pParent field points back to the parent page.  This allows us to
** walk up the BTree from any leaf to the root.  Care must be taken to
** unref() the parent page pointer when this page is no longer referenced.
** The pageDestructor() routine handles that chore.
*/
struct MemPage {
  union u_page_data {
    char aDisk[SQLITE_PAGE_SIZE];  /* Page data stored on disk */
    PageHdr hdr;                   /* Overlay page header */
  } u;
  u8 isInit;                     /* True if auxiliary data is initialized */
  u8 idxShift;                   /* True if apCell[] indices have changed */
  u8 isOverfull;                 /* Some apCell[] points outside u.aDisk[] */
  MemPage *pParent;              /* The parent of this page.  NULL for root */
  int idxParent;                 /* Index in pParent->apCell[] of this node */
  int nFree;                     /* Number of free bytes in u.aDisk[] */
  int nCell;                     /* Number of entries on this page */
  Cell *apCell[MX_CELL+2];       /* All data entires in sorted order */
};

/*
** The in-memory image of a disk page has the auxiliary information appended
** to the end.  EXTRA_SIZE is the number of bytes of space needed to hold
** that extra information.
*/
#define EXTRA_SIZE (sizeof(MemPage)-sizeof(union u_page_data))

/*
** Everything we need to know about an open database
*/
struct Btree {
  BtOps *pOps;          /* Function table */
  Pager *pPager;        /* The page cache */
  BtCursor *pCursor;    /* A list of all open cursors */
  PageOne *page1;       /* First page of the database */
  u8 inTrans;           /* True if a transaction is in progress */
  u8 inCkpt;            /* True if there is a checkpoint on the transaction */
  u8 readOnly;          /* True if the underlying file is readonly */
  u8 needSwab;          /* Need to byte-swapping */
};
typedef Btree Bt;

/*
** A cursor is a pointer to a particular entry in the BTree.
** The entry is identified by its MemPage and the index in
** MemPage.apCell[] of the entry.
*/
struct BtCursor {
  BtCursorOps *pOps;        /* Function table */
  Btree *pBt;               /* The Btree to which this cursor belongs */
  BtCursor *pNext, *pPrev;  /* Forms a linked list of all cursors */
  BtCursor *pShared;        /* Loop of cursors with the same root page */
  Pgno pgnoRoot;            /* The root page of this tree */
  MemPage *pPage;           /* Page that contains the entry */
  int idx;                  /* Index of the entry in pPage->apCell[] */
  u8 wrFlag;                /* True if writable */
  u8 eSkip;                 /* Determines if next step operation is a no-op */
  u8 iMatch;                /* compare result from last sqliteBtreeMoveto() */
};

/*
** Legal values for BtCursor.eSkip.
*/
#define SKIP_NONE     0   /* Always step the cursor */
#define SKIP_NEXT     1   /* The next sqliteBtreeNext() is a no-op */
#define SKIP_PREV     2   /* The next sqliteBtreePrevious() is a no-op */
#define SKIP_INVALID  3   /* Calls to Next() and Previous() are invalid */

/* Forward declarations */
static int fileBtreeCloseCursor(BtCursor *pCur);

/*
** Routines for byte swapping.
*/
u16 swab16(u16 x){
  return ((x & 0xff)<<8) | ((x>>8)&0xff);
}
u32 swab32(u32 x){
  return ((x & 0xff)<<24) | ((x & 0xff00)<<8) |
         ((x>>8) & 0xff00) | ((x>>24)&0xff);
}

/*
** Compute the total number of bytes that a Cell needs on the main
** database page.  The number returned includes the Cell header,
** local payload storage, and the pointer to overflow pages (if
** applicable).  Additional space allocated on overflow pages
** is NOT included in the value returned from this routine.
*/
static int cellSize(Btree *pBt, Cell *pCell){
  int n = NKEY(pBt, pCell->h) + NDATA(pBt, pCell->h);
  if( n>MX_LOCAL_PAYLOAD ){
    n = MX_LOCAL_PAYLOAD + sizeof(Pgno);
  }else{
    n = ROUNDUP(n);
  }
  n += sizeof(CellHdr);
  return n;
}

/*
** Defragment the page given.  All Cells are moved to the
** beginning of the page and all free space is collected 
** into one big FreeBlk at the end of the page.
*/
static void defragmentPage(Btree *pBt, MemPage *pPage){
  int pc, i, n;
  FreeBlk *pFBlk;
  char newPage[SQLITE_USABLE_SIZE];

  assert( sqlitepager_iswriteable(pPage) );
  assert( pPage->isInit );
  pc = sizeof(PageHdr);
  pPage->u.hdr.firstCell = SWAB16(pBt, pc);
  memcpy(newPage, pPage->u.aDisk, pc);
  for(i=0; i<pPage->nCell; i++){
    Cell *pCell = pPage->apCell[i];

    /* This routine should never be called on an overfull page.  The
    ** following asserts verify that constraint. */
    assert( Addr(pCell) > Addr(pPage) );
    assert( Addr(pCell) < Addr(pPage) + SQLITE_USABLE_SIZE );

    n = cellSize(pBt, pCell);
    pCell->h.iNext = SWAB16(pBt, pc + n);
    memcpy(&newPage[pc], pCell, n);
    pPage->apCell[i] = (Cell*)&pPage->u.aDisk[pc];
    pc += n;
  }
  assert( pPage->nFree==SQLITE_USABLE_SIZE-pc );
  memcpy(pPage->u.aDisk, newPage, pc);
  if( pPage->nCell>0 ){
    pPage->apCell[pPage->nCell-1]->h.iNext = 0;
  }
  pFBlk = (FreeBlk*)&pPage->u.aDisk[pc];
  pFBlk->iSize = SWAB16(pBt, SQLITE_USABLE_SIZE - pc);
  pFBlk->iNext = 0;
  pPage->u.hdr.firstFree = SWAB16(pBt, pc);
  memset(&pFBlk[1], 0, SQLITE_USABLE_SIZE - pc - sizeof(FreeBlk));
}

/*
** Allocate nByte bytes of space on a page.  nByte must be a 
** multiple of 4.
**
** Return the index into pPage->u.aDisk[] of the first byte of
** the new allocation. Or return 0 if there is not enough free
** space on the page to satisfy the allocation request.
**
** If the page contains nBytes of free space but does not contain
** nBytes of contiguous free space, then this routine automatically
** calls defragementPage() to consolidate all free space before 
** allocating the new chunk.
*/
static int allocateSpace(Btree *pBt, MemPage *pPage, int nByte){
  FreeBlk *p;
  u16 *pIdx;
  int start;
  int iSize;
#ifndef NDEBUG
  int cnt = 0;
#endif

  assert( sqlitepager_iswriteable(pPage) );
  assert( nByte==ROUNDUP(nByte) );
  assert( pPage->isInit );
  if( pPage->nFree<nByte || pPage->isOverfull ) return 0;
  pIdx = &pPage->u.hdr.firstFree;
  p = (FreeBlk*)&pPage->u.aDisk[SWAB16(pBt, *pIdx)];
  while( (iSize = SWAB16(pBt, p->iSize))<nByte ){
    assert( cnt++ < SQLITE_USABLE_SIZE/4 );
    if( p->iNext==0 ){
      defragmentPage(pBt, pPage);
      pIdx = &pPage->u.hdr.firstFree;
    }else{
      pIdx = &p->iNext;
    }
    p = (FreeBlk*)&pPage->u.aDisk[SWAB16(pBt, *pIdx)];
  }
  if( iSize==nByte ){
    start = SWAB16(pBt, *pIdx);
    *pIdx = p->iNext;
  }else{
    FreeBlk *pNew;
    start = SWAB16(pBt, *pIdx);
    pNew = (FreeBlk*)&pPage->u.aDisk[start + nByte];
    pNew->iNext = p->iNext;
    pNew->iSize = SWAB16(pBt, iSize - nByte);
    *pIdx = SWAB16(pBt, start + nByte);
  }
  pPage->nFree -= nByte;
  return start;
}

/*
** Return a section of the MemPage.u.aDisk[] to the freelist.
** The first byte of the new free block is pPage->u.aDisk[start]
** and the size of the block is "size" bytes.  Size must be
** a multiple of 4.
**
** Most of the effort here is involved in coalesing adjacent
** free blocks into a single big free block.
*/
static void freeSpace(Btree *pBt, MemPage *pPage, int start, int size){
  int end = start + size;
  u16 *pIdx, idx;
  FreeBlk *pFBlk;
  FreeBlk *pNew;
  FreeBlk *pNext;
  int iSize;

  assert( sqlitepager_iswriteable(pPage) );
  assert( size == ROUNDUP(size) );
  assert( start == ROUNDUP(start) );
  assert( pPage->isInit );
  pIdx = &pPage->u.hdr.firstFree;
  idx = SWAB16(pBt, *pIdx);
  while( idx!=0 && idx<start ){
    pFBlk = (FreeBlk*)&pPage->u.aDisk[idx];
    iSize = SWAB16(pBt, pFBlk->iSize);
    if( idx + iSize == start ){
      pFBlk->iSize = SWAB16(pBt, iSize + size);
      if( idx + iSize + size == SWAB16(pBt, pFBlk->iNext) ){
        pNext = (FreeBlk*)&pPage->u.aDisk[idx + iSize + size];
        if( pBt->needSwab ){
          pFBlk->iSize = swab16((u16)swab16(pNext->iSize)+iSize+size);
        }else{
          pFBlk->iSize += pNext->iSize;
        }
        pFBlk->iNext = pNext->iNext;
      }
      pPage->nFree += size;
      return;
    }
    pIdx = &pFBlk->iNext;
    idx = SWAB16(pBt, *pIdx);
  }
  pNew = (FreeBlk*)&pPage->u.aDisk[start];
  if( idx != end ){
    pNew->iSize = SWAB16(pBt, size);
    pNew->iNext = SWAB16(pBt, idx);
  }else{
    pNext = (FreeBlk*)&pPage->u.aDisk[idx];
    pNew->iSize = SWAB16(pBt, size + SWAB16(pBt, pNext->iSize));
    pNew->iNext = pNext->iNext;
  }
  *pIdx = SWAB16(pBt, start);
  pPage->nFree += size;
}

/*
** Initialize the auxiliary information for a disk block.
**
** The pParent parameter must be a pointer to the MemPage which
** is the parent of the page being initialized.  The root of the
** BTree (usually page 2) has no parent and so for that page, 
** pParent==NULL.
**
** Return SQLITE_OK on success.  If we see that the page does
** not contain a well-formed database page, then return 
** SQLITE_CORRUPT.  Note that a return of SQLITE_OK does not
** guarantee that the page is well-formed.  It only shows that
** we failed to detect any corruption.
*/
static int initPage(Bt *pBt, MemPage *pPage, Pgno pgnoThis, MemPage *pParent){
  int idx;           /* An index into pPage->u.aDisk[] */
  Cell *pCell;       /* A pointer to a Cell in pPage->u.aDisk[] */
  FreeBlk *pFBlk;    /* A pointer to a free block in pPage->u.aDisk[] */
  int sz;            /* The size of a Cell in bytes */
  int freeSpace;     /* Amount of free space on the page */

  if( pPage->pParent ){
    assert( pPage->pParent==pParent );
    return SQLITE_OK;
  }
  if( pParent ){
    pPage->pParent = pParent;
    sqlitepager_ref(pParent);
  }
  if( pPage->isInit ) return SQLITE_OK;
  pPage->isInit = 1;
  pPage->nCell = 0;
  freeSpace = USABLE_SPACE;
  idx = SWAB16(pBt, pPage->u.hdr.firstCell);
  while( idx!=0 ){
    if( idx>SQLITE_USABLE_SIZE-MIN_CELL_SIZE ) goto page_format_error;
    if( idx<sizeof(PageHdr) ) goto page_format_error;
    if( idx!=ROUNDUP(idx) ) goto page_format_error;
    pCell = (Cell*)&pPage->u.aDisk[idx];
    sz = cellSize(pBt, pCell);
    if( idx+sz > SQLITE_USABLE_SIZE ) goto page_format_error;
    freeSpace -= sz;
    pPage->apCell[pPage->nCell++] = pCell;
    idx = SWAB16(pBt, pCell->h.iNext);
  }
  pPage->nFree = 0;
  idx = SWAB16(pBt, pPage->u.hdr.firstFree);
  while( idx!=0 ){
    int iNext;
    if( idx>SQLITE_USABLE_SIZE-sizeof(FreeBlk) ) goto page_format_error;
    if( idx<sizeof(PageHdr) ) goto page_format_error;
    pFBlk = (FreeBlk*)&pPage->u.aDisk[idx];
    pPage->nFree += SWAB16(pBt, pFBlk->iSize);
    iNext = SWAB16(pBt, pFBlk->iNext);
    if( iNext>0 && iNext <= idx ) goto page_format_error;
    idx = iNext;
  }
  if( pPage->nCell==0 && pPage->nFree==0 ){
    /* As a special case, an uninitialized root page appears to be
    ** an empty database */
    return SQLITE_OK;
  }
  if( pPage->nFree!=freeSpace ) goto page_format_error;
  return SQLITE_OK;

page_format_error:
  return SQLITE_CORRUPT;
}

/*
** Set up a raw page so that it looks like a database page holding
** no entries.
*/
static void zeroPage(Btree *pBt, MemPage *pPage){
  PageHdr *pHdr;
  FreeBlk *pFBlk;
  assert( sqlitepager_iswriteable(pPage) );
  memset(pPage, 0, SQLITE_USABLE_SIZE);
  pHdr = &pPage->u.hdr;
  pHdr->firstCell = 0;
  pHdr->firstFree = SWAB16(pBt, sizeof(*pHdr));
  pFBlk = (FreeBlk*)&pHdr[1];
  pFBlk->iNext = 0;
  pPage->nFree = SQLITE_USABLE_SIZE - sizeof(*pHdr);
  pFBlk->iSize = SWAB16(pBt, pPage->nFree);
  pPage->nCell = 0;
  pPage->isOverfull = 0;
}

/*
** This routine is called when the reference count for a page
** reaches zero.  We need to unref the pParent pointer when that
** happens.
*/
static void pageDestructor(void *pData){
  MemPage *pPage = (MemPage*)pData;
  if( pPage->pParent ){
    MemPage *pParent = pPage->pParent;
    pPage->pParent = 0;
    sqlitepager_unref(pParent);
  }
}

/*
** Open a new database.
**
** Actually, this routine just sets up the internal data structures
** for accessing the database.  We do not open the database file 
** until the first page is loaded.
**
** zFilename is the name of the database file.  If zFilename is NULL
** a new database with a random name is created.  This randomly named
** database file will be deleted when sqliteBtreeClose() is called.
*/
int sqliteBtreeOpen(
  const char *zFilename,    /* Name of the file containing the BTree database */
  int omitJournal,          /* if TRUE then do not journal this file */
  int nCache,               /* How many pages in the page cache */
  Btree **ppBtree           /* Pointer to new Btree object written here */
){
  Btree *pBt;
  int rc;

  /*
  ** The following asserts make sure that structures used by the btree are
  ** the right size.  This is to guard against size changes that result
  ** when compiling on a different architecture.
  */
  assert( sizeof(u32)==4 );
  assert( sizeof(u16)==2 );
  assert( sizeof(Pgno)==4 );
  assert( sizeof(PageHdr)==8 );
  assert( sizeof(CellHdr)==12 );
  assert( sizeof(FreeBlk)==4 );
  assert( sizeof(OverflowPage)==SQLITE_USABLE_SIZE );
  assert( sizeof(FreelistInfo)==OVERFLOW_SIZE );
  assert( sizeof(ptr)==sizeof(char*) );
  assert( sizeof(uptr)==sizeof(ptr) );

  pBt = sqliteMalloc( sizeof(*pBt) );
  if( pBt==0 ){
    *ppBtree = 0;
    return SQLITE_NOMEM;
  }
  if( nCache<10 ) nCache = 10;
  rc = sqlitepager_open(&pBt->pPager, zFilename, nCache, EXTRA_SIZE,
                        !omitJournal);
  if( rc!=SQLITE_OK ){
    if( pBt->pPager ) sqlitepager_close(pBt->pPager);
    sqliteFree(pBt);
    *ppBtree = 0;
    return rc;
  }
  sqlitepager_set_destructor(pBt->pPager, pageDestructor);
  pBt->pCursor = 0;
  pBt->page1 = 0;
  pBt->readOnly = sqlitepager_isreadonly(pBt->pPager);
  pBt->pOps = &sqliteBtreeOps;
  *ppBtree = pBt;
  return SQLITE_OK;
}

/*
** Close an open database and invalidate all cursors.
*/
static int fileBtreeClose(Btree *pBt){
  while( pBt->pCursor ){
    fileBtreeCloseCursor(pBt->pCursor);
  }
  sqlitepager_close(pBt->pPager);
  sqliteFree(pBt);
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
static int fileBtreeSetCacheSize(Btree *pBt, int mxPage){
  sqlitepager_set_cachesize(pBt->pPager, mxPage);
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
static int fileBtreeSetSafetyLevel(Btree *pBt, int level){
  sqlitepager_set_safety_level(pBt->pPager, level);
  return SQLITE_OK;
}

/*
** Get a reference to page1 of the database file.  This will
** also acquire a readlock on that file.
**
** SQLITE_OK is returned on success.  If the file is not a
** well-formed database file, then SQLITE_CORRUPT is returned.
** SQLITE_BUSY is returned if the database is locked.  SQLITE_NOMEM
** is returned if we run out of memory.  SQLITE_PROTOCOL is returned
** if there is a locking protocol violation.
*/
static int lockBtree(Btree *pBt){
  int rc;
  if( pBt->page1 ) return SQLITE_OK;
  rc = sqlitepager_get(pBt->pPager, 1, (void**)&pBt->page1);
  if( rc!=SQLITE_OK ) return rc;

  /* Do some checking to help insure the file we opened really is
  ** a valid database file. 
  */
  if( sqlitepager_pagecount(pBt->pPager)>0 ){
    PageOne *pP1 = pBt->page1;
    if( strcmp(pP1->zMagic,zMagicHeader)!=0 ||
          (pP1->iMagic!=MAGIC && swab32(pP1->iMagic)!=MAGIC) ){
      rc = SQLITE_NOTADB;
      goto page1_init_failed;
    }
    pBt->needSwab = pP1->iMagic!=MAGIC;
  }
  return rc;

page1_init_failed:
  sqlitepager_unref(pBt->page1);
  pBt->page1 = 0;
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
static void unlockBtreeIfUnused(Btree *pBt){
  if( pBt->inTrans==0 && pBt->pCursor==0 && pBt->page1!=0 ){
    sqlitepager_unref(pBt->page1);
    pBt->page1 = 0;
    pBt->inTrans = 0;
    pBt->inCkpt = 0;
  }
}

/*
** Create a new database by initializing the first two pages of the
** file.
*/
static int newDatabase(Btree *pBt){
  MemPage *pRoot;
  PageOne *pP1;
  int rc;
  if( sqlitepager_pagecount(pBt->pPager)>1 ) return SQLITE_OK;
  pP1 = pBt->page1;
  rc = sqlitepager_write(pBt->page1);
  if( rc ) return rc;
  rc = sqlitepager_get(pBt->pPager, 2, (void**)&pRoot);
  if( rc ) return rc;
  rc = sqlitepager_write(pRoot);
  if( rc ){
    sqlitepager_unref(pRoot);
    return rc;
  }
  strcpy(pP1->zMagic, zMagicHeader);
  if( btree_native_byte_order ){
    pP1->iMagic = MAGIC;
    pBt->needSwab = 0;
  }else{
    pP1->iMagic = swab32(MAGIC);
    pBt->needSwab = 1;
  }
  zeroPage(pBt, pRoot);
  sqlitepager_unref(pRoot);
  return SQLITE_OK;
}

/*
** Attempt to start a new transaction.
**
** A transaction must be started before attempting any changes
** to the database.  None of the following routines will work
** unless a transaction is started first:
**
**      sqliteBtreeCreateTable()
**      sqliteBtreeCreateIndex()
**      sqliteBtreeClearTable()
**      sqliteBtreeDropTable()
**      sqliteBtreeInsert()
**      sqliteBtreeDelete()
**      sqliteBtreeUpdateMeta()
*/
static int fileBtreeBeginTrans(Btree *pBt){
  int rc;
  if( pBt->inTrans ) return SQLITE_ERROR;
  if( pBt->readOnly ) return SQLITE_READONLY;
  if( pBt->page1==0 ){
    rc = lockBtree(pBt);
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }
  rc = sqlitepager_begin(pBt->page1);
  if( rc==SQLITE_OK ){
    rc = newDatabase(pBt);
  }
  if( rc==SQLITE_OK ){
    pBt->inTrans = 1;
    pBt->inCkpt = 0;
  }else{
    unlockBtreeIfUnused(pBt);
  }
  return rc;
}

/*
** Commit the transaction currently in progress.
**
** This will release the write lock on the database file.  If there
** are no active cursors, it also releases the read lock.
*/
static int fileBtreeCommit(Btree *pBt){
  int rc;
  rc = pBt->readOnly ? SQLITE_OK : sqlitepager_commit(pBt->pPager);
  pBt->inTrans = 0;
  pBt->inCkpt = 0;
  unlockBtreeIfUnused(pBt);
  return rc;
}

/*
** Rollback the transaction in progress.  All cursors will be
** invalided by this operation.  Any attempt to use a cursor
** that was open at the beginning of this operation will result
** in an error.
**
** This will release the write lock on the database file.  If there
** are no active cursors, it also releases the read lock.
*/
static int fileBtreeRollback(Btree *pBt){
  int rc;
  BtCursor *pCur;
  if( pBt->inTrans==0 ) return SQLITE_OK;
  pBt->inTrans = 0;
  pBt->inCkpt = 0;
  rc = pBt->readOnly ? SQLITE_OK : sqlitepager_rollback(pBt->pPager);
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    if( pCur->pPage && pCur->pPage->isInit==0 ){
      sqlitepager_unref(pCur->pPage);
      pCur->pPage = 0;
    }
  }
  unlockBtreeIfUnused(pBt);
  return rc;
}

/*
** Set the checkpoint for the current transaction.  The checkpoint serves
** as a sub-transaction that can be rolled back independently of the
** main transaction.  You must start a transaction before starting a
** checkpoint.  The checkpoint is ended automatically if the transaction
** commits or rolls back.
**
** Only one checkpoint may be active at a time.  It is an error to try
** to start a new checkpoint if another checkpoint is already active.
*/
static int fileBtreeBeginCkpt(Btree *pBt){
  int rc;
  if( !pBt->inTrans || pBt->inCkpt ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  rc = pBt->readOnly ? SQLITE_OK : sqlitepager_ckpt_begin(pBt->pPager);
  pBt->inCkpt = 1;
  return rc;
}


/*
** Commit a checkpoint to transaction currently in progress.  If no
** checkpoint is active, this is a no-op.
*/
static int fileBtreeCommitCkpt(Btree *pBt){
  int rc;
  if( pBt->inCkpt && !pBt->readOnly ){
    rc = sqlitepager_ckpt_commit(pBt->pPager);
  }else{
    rc = SQLITE_OK;
  }
  pBt->inCkpt = 0;
  return rc;
}

/*
** Rollback the checkpoint to the current transaction.  If there
** is no active checkpoint or transaction, this routine is a no-op.
**
** All cursors will be invalided by this operation.  Any attempt
** to use a cursor that was open at the beginning of this operation
** will result in an error.
*/
static int fileBtreeRollbackCkpt(Btree *pBt){
  int rc;
  BtCursor *pCur;
  if( pBt->inCkpt==0 || pBt->readOnly ) return SQLITE_OK;
  rc = sqlitepager_ckpt_rollback(pBt->pPager);
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    if( pCur->pPage && pCur->pPage->isInit==0 ){
      sqlitepager_unref(pCur->pPage);
      pCur->pPage = 0;
    }
  }
  pBt->inCkpt = 0;
  return rc;
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
** 2:  No other cursors may be open with wrFlag==0 on the same table
**
** 3:  The database must be writable (not on read-only media)
**
** 4:  There must be an active transaction.
**
** Condition 2 warrants further discussion.  If any cursor is opened
** on a table with wrFlag==0, that prevents all other cursors from
** writing to that table.  This is a kind of "read-lock".  When a cursor
** is opened with wrFlag==0 it is guaranteed that the table will not
** change as long as the cursor is open.  This allows the cursor to
** do a sequential scan of the table without having to worry about
** entries being inserted or deleted during the scan.  Cursors should
** be opened with wrFlag==0 only if this read-lock property is needed.
** That is to say, cursors should be opened with wrFlag==0 only if they
** intend to use the sqliteBtreeNext() system call.  All other cursors
** should be opened with wrFlag==1 even if they never really intend
** to write.
** 
** No checking is done to make sure that page iTable really is the
** root page of a b-tree.  If it is not, then the cursor acquired
** will not work correctly.
*/
static 
int fileBtreeCursor(Btree *pBt, int iTable, int wrFlag, BtCursor **ppCur){
  int rc;
  BtCursor *pCur, *pRing;

  if( pBt->readOnly && wrFlag ){
    *ppCur = 0;
    return SQLITE_READONLY;
  }
  if( pBt->page1==0 ){
    rc = lockBtree(pBt);
    if( rc!=SQLITE_OK ){
      *ppCur = 0;
      return rc;
    }
  }
  pCur = sqliteMalloc( sizeof(*pCur) );
  if( pCur==0 ){
    rc = SQLITE_NOMEM;
    goto create_cursor_exception;
  }
  pCur->pgnoRoot = (Pgno)iTable;
  rc = sqlitepager_get(pBt->pPager, pCur->pgnoRoot, (void**)&pCur->pPage);
  if( rc!=SQLITE_OK ){
    goto create_cursor_exception;
  }
  rc = initPage(pBt, pCur->pPage, pCur->pgnoRoot, 0);
  if( rc!=SQLITE_OK ){
    goto create_cursor_exception;
  }
  pCur->pOps = &sqliteBtreeCursorOps;
  pCur->pBt = pBt;
  pCur->wrFlag = wrFlag;
  pCur->idx = 0;
  pCur->eSkip = SKIP_INVALID;
  pCur->pNext = pBt->pCursor;
  if( pCur->pNext ){
    pCur->pNext->pPrev = pCur;
  }
  pCur->pPrev = 0;
  pRing = pBt->pCursor;
  while( pRing && pRing->pgnoRoot!=pCur->pgnoRoot ){ pRing = pRing->pNext; }
  if( pRing ){
    pCur->pShared = pRing->pShared;
    pRing->pShared = pCur;
  }else{
    pCur->pShared = pCur;
  }
  pBt->pCursor = pCur;
  *ppCur = pCur;
  return SQLITE_OK;

create_cursor_exception:
  *ppCur = 0;
  if( pCur ){
    if( pCur->pPage ) sqlitepager_unref(pCur->pPage);
    sqliteFree(pCur);
  }
  unlockBtreeIfUnused(pBt);
  return rc;
}

/*
** Close a cursor.  The read lock on the database file is released
** when the last cursor is closed.
*/
static int fileBtreeCloseCursor(BtCursor *pCur){
  Btree *pBt = pCur->pBt;
  if( pCur->pPrev ){
    pCur->pPrev->pNext = pCur->pNext;
  }else{
    pBt->pCursor = pCur->pNext;
  }
  if( pCur->pNext ){
    pCur->pNext->pPrev = pCur->pPrev;
  }
  if( pCur->pPage ){
    sqlitepager_unref(pCur->pPage);
  }
  if( pCur->pShared!=pCur ){
    BtCursor *pRing = pCur->pShared;
    while( pRing->pShared!=pCur ){ pRing = pRing->pShared; }
    pRing->pShared = pCur->pShared;
  }
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
    sqlitepager_ref(pTempCur->pPage);
  }
}

/*
** Delete a temporary cursor such as was made by the CreateTemporaryCursor()
** function above.
*/
static void releaseTempCursor(BtCursor *pCur){
  if( pCur->pPage ){
    sqlitepager_unref(pCur->pPage);
  }
}

/*
** Set *pSize to the number of bytes of key in the entry the
** cursor currently points to.  Always return SQLITE_OK.
** Failure is not possible.  If the cursor is not currently
** pointing to an entry (which can happen, for example, if
** the database is empty) then *pSize is set to 0.
*/
static int fileBtreeKeySize(BtCursor *pCur, int *pSize){
  Cell *pCell;
  MemPage *pPage;

  pPage = pCur->pPage;
  assert( pPage!=0 );
  if( pCur->idx >= pPage->nCell ){
    *pSize = 0;
  }else{
    pCell = pPage->apCell[pCur->idx];
    *pSize = NKEY(pCur->pBt, pCell->h);
  }
  return SQLITE_OK;
}

/*
** Read payload information from the entry that the pCur cursor is
** pointing to.  Begin reading the payload at "offset" and read
** a total of "amt" bytes.  Put the result in zBuf.
**
** This routine does not make a distinction between key and data.
** It just reads bytes from the payload area.
*/
static int getPayload(BtCursor *pCur, int offset, int amt, char *zBuf){
  char *aPayload;
  Pgno nextPage;
  int rc;
  Btree *pBt = pCur->pBt;
  assert( pCur!=0 && pCur->pPage!=0 );
  assert( pCur->idx>=0 && pCur->idx<pCur->pPage->nCell );
  aPayload = pCur->pPage->apCell[pCur->idx]->aPayload;
  if( offset<MX_LOCAL_PAYLOAD ){
    int a = amt;
    if( a+offset>MX_LOCAL_PAYLOAD ){
      a = MX_LOCAL_PAYLOAD - offset;
    }
    memcpy(zBuf, &aPayload[offset], a);
    if( a==amt ){
      return SQLITE_OK;
    }
    offset = 0;
    zBuf += a;
    amt -= a;
  }else{
    offset -= MX_LOCAL_PAYLOAD;
  }
  if( amt>0 ){
    nextPage = SWAB32(pBt, pCur->pPage->apCell[pCur->idx]->ovfl);
  }
  while( amt>0 && nextPage ){
    OverflowPage *pOvfl;
    rc = sqlitepager_get(pBt->pPager, nextPage, (void**)&pOvfl);
    if( rc!=0 ){
      return rc;
    }
    nextPage = SWAB32(pBt, pOvfl->iNext);
    if( offset<OVERFLOW_SIZE ){
      int a = amt;
      if( a + offset > OVERFLOW_SIZE ){
        a = OVERFLOW_SIZE - offset;
      }
      memcpy(zBuf, &pOvfl->aPayload[offset], a);
      offset = 0;
      amt -= a;
      zBuf += a;
    }else{
      offset -= OVERFLOW_SIZE;
    }
    sqlitepager_unref(pOvfl);
  }
  if( amt>0 ){
    return SQLITE_CORRUPT;
  }
  return SQLITE_OK;
}

/*
** Read part of the key associated with cursor pCur.  A maximum
** of "amt" bytes will be transfered into zBuf[].  The transfer
** begins at "offset".  The number of bytes actually read is
** returned. 
**
** Change:  It used to be that the amount returned will be smaller
** than the amount requested if there are not enough bytes in the key
** to satisfy the request.  But now, it must be the case that there
** is enough data available to satisfy the request.  If not, an exception
** is raised.  The change was made in an effort to boost performance
** by eliminating unneeded tests.
*/
static int fileBtreeKey(BtCursor *pCur, int offset, int amt, char *zBuf){
  MemPage *pPage;

  assert( amt>=0 );
  assert( offset>=0 );
  assert( pCur->pPage!=0 );
  pPage = pCur->pPage;
  if( pCur->idx >= pPage->nCell ){
    return 0;
  }
  assert( amt+offset <= NKEY(pCur->pBt, pPage->apCell[pCur->idx]->h) );
  getPayload(pCur, offset, amt, zBuf);
  return amt;
}

/*
** Set *pSize to the number of bytes of data in the entry the
** cursor currently points to.  Always return SQLITE_OK.
** Failure is not possible.  If the cursor is not currently
** pointing to an entry (which can happen, for example, if
** the database is empty) then *pSize is set to 0.
*/
static int fileBtreeDataSize(BtCursor *pCur, int *pSize){
  Cell *pCell;
  MemPage *pPage;

  pPage = pCur->pPage;
  assert( pPage!=0 );
  if( pCur->idx >= pPage->nCell ){
    *pSize = 0;
  }else{
    pCell = pPage->apCell[pCur->idx];
    *pSize = NDATA(pCur->pBt, pCell->h);
  }
  return SQLITE_OK;
}

/*
** Read part of the data associated with cursor pCur.  A maximum
** of "amt" bytes will be transfered into zBuf[].  The transfer
** begins at "offset".  The number of bytes actually read is
** returned.  The amount returned will be smaller than the
** amount requested if there are not enough bytes in the data
** to satisfy the request.
*/
static int fileBtreeData(BtCursor *pCur, int offset, int amt, char *zBuf){
  Cell *pCell;
  MemPage *pPage;

  assert( amt>=0 );
  assert( offset>=0 );
  assert( pCur->pPage!=0 );
  pPage = pCur->pPage;
  if( pCur->idx >= pPage->nCell ){
    return 0;
  }
  pCell = pPage->apCell[pCur->idx];
  assert( amt+offset <= NDATA(pCur->pBt, pCell->h) );
  getPayload(pCur, offset + NKEY(pCur->pBt, pCell->h), amt, zBuf);
  return amt;
}

/*
** Compare an external key against the key on the entry that pCur points to.
**
** The external key is pKey and is nKey bytes long.  The last nIgnore bytes
** of the key associated with pCur are ignored, as if they do not exist.
** (The normal case is for nIgnore to be zero in which case the entire
** internal key is used in the comparison.)
**
** The comparison result is written to *pRes as follows:
**
**    *pRes<0    This means pCur<pKey
**
**    *pRes==0   This means pCur==pKey for all nKey bytes
**
**    *pRes>0    This means pCur>pKey
**
** When one key is an exact prefix of the other, the shorter key is
** considered less than the longer one.  In order to be equal the
** keys must be exactly the same length. (The length of the pCur key
** is the actual key length minus nIgnore bytes.)
*/
static int fileBtreeKeyCompare(
  BtCursor *pCur,       /* Pointer to entry to compare against */
  const void *pKey,     /* Key to compare against entry that pCur points to */
  int nKey,             /* Number of bytes in pKey */
  int nIgnore,          /* Ignore this many bytes at the end of pCur */
  int *pResult          /* Write the result here */
){
  Pgno nextPage;
  int n, c, rc, nLocal;
  Cell *pCell;
  Btree *pBt = pCur->pBt;
  const char *zKey  = (const char*)pKey;

  assert( pCur->pPage );
  assert( pCur->idx>=0 && pCur->idx<pCur->pPage->nCell );
  pCell = pCur->pPage->apCell[pCur->idx];
  nLocal = NKEY(pBt, pCell->h) - nIgnore;
  if( nLocal<0 ) nLocal = 0;
  n = nKey<nLocal ? nKey : nLocal;
  if( n>MX_LOCAL_PAYLOAD ){
    n = MX_LOCAL_PAYLOAD;
  }
  c = memcmp(pCell->aPayload, zKey, n);
  if( c!=0 ){
    *pResult = c;
    return SQLITE_OK;
  }
  zKey += n;
  nKey -= n;
  nLocal -= n;
  nextPage = SWAB32(pBt, pCell->ovfl);
  while( nKey>0 && nLocal>0 ){
    OverflowPage *pOvfl;
    if( nextPage==0 ){
      return SQLITE_CORRUPT;
    }
    rc = sqlitepager_get(pBt->pPager, nextPage, (void**)&pOvfl);
    if( rc ){
      return rc;
    }
    nextPage = SWAB32(pBt, pOvfl->iNext);
    n = nKey<nLocal ? nKey : nLocal;
    if( n>OVERFLOW_SIZE ){
      n = OVERFLOW_SIZE;
    }
    c = memcmp(pOvfl->aPayload, zKey, n);
    sqlitepager_unref(pOvfl);
    if( c!=0 ){
      *pResult = c;
      return SQLITE_OK;
    }
    nKey -= n;
    nLocal -= n;
    zKey += n;
  }
  if( c==0 ){
    c = nLocal - nKey;
  }
  *pResult = c;
  return SQLITE_OK;
}

/*
** Move the cursor down to a new child page.  The newPgno argument is the
** page number of the child page in the byte order of the disk image.
*/
static int moveToChild(BtCursor *pCur, int newPgno){
  int rc;
  MemPage *pNewPage;
  Btree *pBt = pCur->pBt;

  newPgno = SWAB32(pBt, newPgno);
  rc = sqlitepager_get(pBt->pPager, newPgno, (void**)&pNewPage);
  if( rc ) return rc;
  rc = initPage(pBt, pNewPage, newPgno, pCur->pPage);
  if( rc ) return rc;
  assert( pCur->idx>=pCur->pPage->nCell
          || pCur->pPage->apCell[pCur->idx]->h.leftChild==SWAB32(pBt,newPgno) );
  assert( pCur->idx<pCur->pPage->nCell
          || pCur->pPage->u.hdr.rightChild==SWAB32(pBt,newPgno) );
  pNewPage->idxParent = pCur->idx;
  pCur->pPage->idxShift = 0;
  sqlitepager_unref(pCur->pPage);
  pCur->pPage = pNewPage;
  pCur->idx = 0;
  if( pNewPage->nCell<1 ){
    return SQLITE_CORRUPT;
  }
  return SQLITE_OK;
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
  Pgno oldPgno;
  MemPage *pParent;
  MemPage *pPage;
  int idxParent;
  pPage = pCur->pPage;
  assert( pPage!=0 );
  pParent = pPage->pParent;
  assert( pParent!=0 );
  idxParent = pPage->idxParent;
  sqlitepager_ref(pParent);
  sqlitepager_unref(pPage);
  pCur->pPage = pParent;
  assert( pParent->idxShift==0 );
  if( pParent->idxShift==0 ){
    pCur->idx = idxParent;
#ifndef NDEBUG  
    /* Verify that pCur->idx is the correct index to point back to the child
    ** page we just came from 
    */
    oldPgno = SWAB32(pCur->pBt, sqlitepager_pagenumber(pPage));
    if( pCur->idx<pParent->nCell ){
      assert( pParent->apCell[idxParent]->h.leftChild==oldPgno );
    }else{
      assert( pParent->u.hdr.rightChild==oldPgno );
    }
#endif
  }else{
    /* The MemPage.idxShift flag indicates that cell indices might have 
    ** changed since idxParent was set and hence idxParent might be out
    ** of date.  So recompute the parent cell index by scanning all cells
    ** and locating the one that points to the child we just came from.
    */
    int i;
    pCur->idx = pParent->nCell;
    oldPgno = SWAB32(pCur->pBt, sqlitepager_pagenumber(pPage));
    for(i=0; i<pParent->nCell; i++){
      if( pParent->apCell[i]->h.leftChild==oldPgno ){
        pCur->idx = i;
        break;
      }
    }
  }
}

/*
** Move the cursor to the root page
*/
static int moveToRoot(BtCursor *pCur){
  MemPage *pNew;
  int rc;
  Btree *pBt = pCur->pBt;

  rc = sqlitepager_get(pBt->pPager, pCur->pgnoRoot, (void**)&pNew);
  if( rc ) return rc;
  rc = initPage(pBt, pNew, pCur->pgnoRoot, 0);
  if( rc ) return rc;
  sqlitepager_unref(pCur->pPage);
  pCur->pPage = pNew;
  pCur->idx = 0;
  return SQLITE_OK;
}

/*
** Move the cursor down to the left-most leaf entry beneath the
** entry to which it is currently pointing.
*/
static int moveToLeftmost(BtCursor *pCur){
  Pgno pgno;
  int rc;

  while( (pgno = pCur->pPage->apCell[pCur->idx]->h.leftChild)!=0 ){
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
*/
static int moveToRightmost(BtCursor *pCur){
  Pgno pgno;
  int rc;

  while( (pgno = pCur->pPage->u.hdr.rightChild)!=0 ){
    pCur->idx = pCur->pPage->nCell;
    rc = moveToChild(pCur, pgno);
    if( rc ) return rc;
  }
  pCur->idx = pCur->pPage->nCell - 1;
  return SQLITE_OK;
}

/* Move the cursor to the first entry in the table.  Return SQLITE_OK
** on success.  Set *pRes to 0 if the cursor actually points to something
** or set *pRes to 1 if the table is empty.
*/
static int fileBtreeFirst(BtCursor *pCur, int *pRes){
  int rc;
  if( pCur->pPage==0 ) return SQLITE_ABORT;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  if( pCur->pPage->nCell==0 ){
    *pRes = 1;
    return SQLITE_OK;
  }
  *pRes = 0;
  rc = moveToLeftmost(pCur);
  pCur->eSkip = SKIP_NONE;
  return rc;
}

/* Move the cursor to the last entry in the table.  Return SQLITE_OK
** on success.  Set *pRes to 0 if the cursor actually points to something
** or set *pRes to 1 if the table is empty.
*/
static int fileBtreeLast(BtCursor *pCur, int *pRes){
  int rc;
  if( pCur->pPage==0 ) return SQLITE_ABORT;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  assert( pCur->pPage->isInit );
  if( pCur->pPage->nCell==0 ){
    *pRes = 1;
    return SQLITE_OK;
  }
  *pRes = 0;
  rc = moveToRightmost(pCur);
  pCur->eSkip = SKIP_NONE;
  return rc;
}

/* Move the cursor so that it points to an entry near pKey.
** Return a success code.
**
** If an exact match is not found, then the cursor is always
** left pointing at a leaf page which would hold the entry if it
** were present.  The cursor might point to an entry that comes
** before or after the key.
**
** The result of comparing the key with the entry to which the
** cursor is left pointing is stored in pCur->iMatch.  The same
** value is also written to *pRes if pRes!=NULL.  The meaning of
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
static
int fileBtreeMoveto(BtCursor *pCur, const void *pKey, int nKey, int *pRes){
  int rc;
  if( pCur->pPage==0 ) return SQLITE_ABORT;
  pCur->eSkip = SKIP_NONE;
  rc = moveToRoot(pCur);
  if( rc ) return rc;
  for(;;){
    int lwr, upr;
    Pgno chldPg;
    MemPage *pPage = pCur->pPage;
    int c = -1;  /* pRes return if table is empty must be -1 */
    lwr = 0;
    upr = pPage->nCell-1;
    while( lwr<=upr ){
      pCur->idx = (lwr+upr)/2;
      rc = fileBtreeKeyCompare(pCur, pKey, nKey, 0, &c);
      if( rc ) return rc;
      if( c==0 ){
        pCur->iMatch = c;
        if( pRes ) *pRes = 0;
        return SQLITE_OK;
      }
      if( c<0 ){
        lwr = pCur->idx+1;
      }else{
        upr = pCur->idx-1;
      }
    }
    assert( lwr==upr+1 );
    assert( pPage->isInit );
    if( lwr>=pPage->nCell ){
      chldPg = pPage->u.hdr.rightChild;
    }else{
      chldPg = pPage->apCell[lwr]->h.leftChild;
    }
    if( chldPg==0 ){
      pCur->iMatch = c;
      if( pRes ) *pRes = c;
      return SQLITE_OK;
    }
    pCur->idx = lwr;
    rc = moveToChild(pCur, chldPg);
    if( rc ) return rc;
  }
  /* NOT REACHED */
}

/*
** Advance the cursor to the next entry in the database.  If
** successful then set *pRes=0.  If the cursor
** was already pointing to the last entry in the database before
** this routine was called, then set *pRes=1.
*/
static int fileBtreeNext(BtCursor *pCur, int *pRes){
  int rc;
  MemPage *pPage = pCur->pPage;
  assert( pRes!=0 );
  if( pPage==0 ){
    *pRes = 1;
    return SQLITE_ABORT;
  }
  assert( pPage->isInit );
  assert( pCur->eSkip!=SKIP_INVALID );
  if( pPage->nCell==0 ){
    *pRes = 1;
    return SQLITE_OK;
  }
  assert( pCur->idx<pPage->nCell );
  if( pCur->eSkip==SKIP_NEXT ){
    pCur->eSkip = SKIP_NONE;
    *pRes = 0;
    return SQLITE_OK;
  }
  pCur->eSkip = SKIP_NONE;
  pCur->idx++;
  if( pCur->idx>=pPage->nCell ){
    if( pPage->u.hdr.rightChild ){
      rc = moveToChild(pCur, pPage->u.hdr.rightChild);
      if( rc ) return rc;
      rc = moveToLeftmost(pCur);
      *pRes = 0;
      return rc;
    }
    do{
      if( pPage->pParent==0 ){
        *pRes = 1;
        return SQLITE_OK;
      }
      moveToParent(pCur);
      pPage = pCur->pPage;
    }while( pCur->idx>=pPage->nCell );
    *pRes = 0;
    return SQLITE_OK;
  }
  *pRes = 0;
  if( pPage->u.hdr.rightChild==0 ){
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
static int fileBtreePrevious(BtCursor *pCur, int *pRes){
  int rc;
  Pgno pgno;
  MemPage *pPage;
  pPage = pCur->pPage;
  if( pPage==0 ){
    *pRes = 1;
    return SQLITE_ABORT;
  }
  assert( pPage->isInit );
  assert( pCur->eSkip!=SKIP_INVALID );
  if( pPage->nCell==0 ){
    *pRes = 1;
    return SQLITE_OK;
  }
  if( pCur->eSkip==SKIP_PREV ){
    pCur->eSkip = SKIP_NONE;
    *pRes = 0;
    return SQLITE_OK;
  }
  pCur->eSkip = SKIP_NONE;
  assert( pCur->idx>=0 );
  if( (pgno = pPage->apCell[pCur->idx]->h.leftChild)!=0 ){
    rc = moveToChild(pCur, pgno);
    if( rc ) return rc;
    rc = moveToRightmost(pCur);
  }else{
    while( pCur->idx==0 ){
      if( pPage->pParent==0 ){
        if( pRes ) *pRes = 1;
        return SQLITE_OK;
      }
      moveToParent(pCur);
      pPage = pCur->pPage;
    }
    pCur->idx--;
    rc = SQLITE_OK;
  }
  *pRes = 0;
  return rc;
}

/*
** Allocate a new page from the database file.
**
** The new page is marked as dirty.  (In other words, sqlitepager_write()
** has already been called on the new page.)  The new page has also
** been referenced and the calling routine is responsible for calling
** sqlitepager_unref() on the new page when it is done.
**
** SQLITE_OK is returned on success.  Any other return value indicates
** an error.  *ppPage and *pPgno are undefined in the event of an error.
** Do not invoke sqlitepager_unref() on *ppPage if an error is returned.
**
** If the "nearby" parameter is not 0, then a (feeble) effort is made to 
** locate a page close to the page number "nearby".  This can be used in an
** attempt to keep related pages close to each other in the database file,
** which in turn can make database access faster.
*/
static int allocatePage(Btree *pBt, MemPage **ppPage, Pgno *pPgno, Pgno nearby){
  PageOne *pPage1 = pBt->page1;
  int rc;
  if( pPage1->freeList ){
    OverflowPage *pOvfl;
    FreelistInfo *pInfo;

    rc = sqlitepager_write(pPage1);
    if( rc ) return rc;
    SWAB_ADD(pBt, pPage1->nFree, -1);
    rc = sqlitepager_get(pBt->pPager, SWAB32(pBt, pPage1->freeList),
                        (void**)&pOvfl);
    if( rc ) return rc;
    rc = sqlitepager_write(pOvfl);
    if( rc ){
      sqlitepager_unref(pOvfl);
      return rc;
    }
    pInfo = (FreelistInfo*)pOvfl->aPayload;
    if( pInfo->nFree==0 ){
      *pPgno = SWAB32(pBt, pPage1->freeList);
      pPage1->freeList = pOvfl->iNext;
      *ppPage = (MemPage*)pOvfl;
    }else{
      int closest, n;
      n = SWAB32(pBt, pInfo->nFree);
      if( n>1 && nearby>0 ){
        int i, dist;
        closest = 0;
        dist = SWAB32(pBt, pInfo->aFree[0]) - nearby;
        if( dist<0 ) dist = -dist;
        for(i=1; i<n; i++){
          int d2 = SWAB32(pBt, pInfo->aFree[i]) - nearby;
          if( d2<0 ) d2 = -d2;
          if( d2<dist ) closest = i;
        }
      }else{
        closest = 0;
      }
      SWAB_ADD(pBt, pInfo->nFree, -1);
      *pPgno = SWAB32(pBt, pInfo->aFree[closest]);
      pInfo->aFree[closest] = pInfo->aFree[n-1];
      rc = sqlitepager_get(pBt->pPager, *pPgno, (void**)ppPage);
      sqlitepager_unref(pOvfl);
      if( rc==SQLITE_OK ){
        sqlitepager_dont_rollback(*ppPage);
        rc = sqlitepager_write(*ppPage);
      }
    }
  }else{
    *pPgno = sqlitepager_pagecount(pBt->pPager) + 1;
    rc = sqlitepager_get(pBt->pPager, *pPgno, (void**)ppPage);
    if( rc ) return rc;
    rc = sqlitepager_write(*ppPage);
  }
  return rc;
}

/*
** Add a page of the database file to the freelist.  Either pgno or
** pPage but not both may be 0. 
**
** sqlitepager_unref() is NOT called for pPage.
*/
static int freePage(Btree *pBt, void *pPage, Pgno pgno){
  PageOne *pPage1 = pBt->page1;
  OverflowPage *pOvfl = (OverflowPage*)pPage;
  int rc;
  int needUnref = 0;
  MemPage *pMemPage;

  if( pgno==0 ){
    assert( pOvfl!=0 );
    pgno = sqlitepager_pagenumber(pOvfl);
  }
  assert( pgno>2 );
  assert( sqlitepager_pagenumber(pOvfl)==pgno );
  pMemPage = (MemPage*)pPage;
  pMemPage->isInit = 0;
  if( pMemPage->pParent ){
    sqlitepager_unref(pMemPage->pParent);
    pMemPage->pParent = 0;
  }
  rc = sqlitepager_write(pPage1);
  if( rc ){
    return rc;
  }
  SWAB_ADD(pBt, pPage1->nFree, 1);
  if( pPage1->nFree!=0 && pPage1->freeList!=0 ){
    OverflowPage *pFreeIdx;
    rc = sqlitepager_get(pBt->pPager, SWAB32(pBt, pPage1->freeList),
                        (void**)&pFreeIdx);
    if( rc==SQLITE_OK ){
      FreelistInfo *pInfo = (FreelistInfo*)pFreeIdx->aPayload;
      int n = SWAB32(pBt, pInfo->nFree);
      if( n<(sizeof(pInfo->aFree)/sizeof(pInfo->aFree[0])) ){
        rc = sqlitepager_write(pFreeIdx);
        if( rc==SQLITE_OK ){
          pInfo->aFree[n] = SWAB32(pBt, pgno);
          SWAB_ADD(pBt, pInfo->nFree, 1);
          sqlitepager_unref(pFreeIdx);
          sqlitepager_dont_write(pBt->pPager, pgno);
          return rc;
        }
      }
      sqlitepager_unref(pFreeIdx);
    }
  }
  if( pOvfl==0 ){
    assert( pgno>0 );
    rc = sqlitepager_get(pBt->pPager, pgno, (void**)&pOvfl);
    if( rc ) return rc;
    needUnref = 1;
  }
  rc = sqlitepager_write(pOvfl);
  if( rc ){
    if( needUnref ) sqlitepager_unref(pOvfl);
    return rc;
  }
  pOvfl->iNext = pPage1->freeList;
  pPage1->freeList = SWAB32(pBt, pgno);
  memset(pOvfl->aPayload, 0, OVERFLOW_SIZE);
  if( needUnref ) rc = sqlitepager_unref(pOvfl);
  return rc;
}

/*
** Erase all the data out of a cell.  This involves returning overflow
** pages back the freelist.
*/
static int clearCell(Btree *pBt, Cell *pCell){
  Pager *pPager = pBt->pPager;
  OverflowPage *pOvfl;
  Pgno ovfl, nextOvfl;
  int rc;

  if( NKEY(pBt, pCell->h) + NDATA(pBt, pCell->h) <= MX_LOCAL_PAYLOAD ){
    return SQLITE_OK;
  }
  ovfl = SWAB32(pBt, pCell->ovfl);
  pCell->ovfl = 0;
  while( ovfl ){
    rc = sqlitepager_get(pPager, ovfl, (void**)&pOvfl);
    if( rc ) return rc;
    nextOvfl = SWAB32(pBt, pOvfl->iNext);
    rc = freePage(pBt, pOvfl, ovfl);
    if( rc ) return rc;
    sqlitepager_unref(pOvfl);
    ovfl = nextOvfl;
  }
  return SQLITE_OK;
}

/*
** Create a new cell from key and data.  Overflow pages are allocated as
** necessary and linked to this cell.  
*/
static int fillInCell(
  Btree *pBt,              /* The whole Btree.  Needed to allocate pages */
  Cell *pCell,             /* Populate this Cell structure */
  const void *pKey, int nKey,    /* The key */
  const void *pData,int nData    /* The data */
){
  OverflowPage *pOvfl, *pPrior;
  Pgno *pNext;
  int spaceLeft;
  int n, rc;
  int nPayload;
  const char *pPayload;
  char *pSpace;
  Pgno nearby = 0;

  pCell->h.leftChild = 0;
  pCell->h.nKey = SWAB16(pBt, nKey & 0xffff);
  pCell->h.nKeyHi = nKey >> 16;
  pCell->h.nData = SWAB16(pBt, nData & 0xffff);
  pCell->h.nDataHi = nData >> 16;
  pCell->h.iNext = 0;

  pNext = &pCell->ovfl;
  pSpace = pCell->aPayload;
  spaceLeft = MX_LOCAL_PAYLOAD;
  pPayload = pKey;
  pKey = 0;
  nPayload = nKey;
  pPrior = 0;
  while( nPayload>0 ){
    if( spaceLeft==0 ){
      rc = allocatePage(pBt, (MemPage**)&pOvfl, pNext, nearby);
      if( rc ){
        *pNext = 0;
      }else{
        nearby = *pNext;
      }
      if( pPrior ) sqlitepager_unref(pPrior);
      if( rc ){
        clearCell(pBt, pCell);
        return rc;
      }
      if( pBt->needSwab ) *pNext = swab32(*pNext);
      pPrior = pOvfl;
      spaceLeft = OVERFLOW_SIZE;
      pSpace = pOvfl->aPayload;
      pNext = &pOvfl->iNext;
    }
    n = nPayload;
    if( n>spaceLeft ) n = spaceLeft;
    memcpy(pSpace, pPayload, n);
    nPayload -= n;
    if( nPayload==0 && pData ){
      pPayload = pData;
      nPayload = nData;
      pData = 0;
    }else{
      pPayload += n;
    }
    spaceLeft -= n;
    pSpace += n;
  }
  *pNext = 0;
  if( pPrior ){
    sqlitepager_unref(pPrior);
  }
  return SQLITE_OK;
}

/*
** Change the MemPage.pParent pointer on the page whose number is
** given in the second argument so that MemPage.pParent holds the
** pointer in the third argument.
*/
static void reparentPage(Pager *pPager, Pgno pgno, MemPage *pNewParent,int idx){
  MemPage *pThis;

  if( pgno==0 ) return;
  assert( pPager!=0 );
  pThis = sqlitepager_lookup(pPager, pgno);
  if( pThis && pThis->isInit ){
    if( pThis->pParent!=pNewParent ){
      if( pThis->pParent ) sqlitepager_unref(pThis->pParent);
      pThis->pParent = pNewParent;
      if( pNewParent ) sqlitepager_ref(pNewParent);
    }
    pThis->idxParent = idx;
    sqlitepager_unref(pThis);
  }
}

/*
** Reparent all children of the given page to be the given page.
** In other words, for every child of pPage, invoke reparentPage()
** to make sure that each child knows that pPage is its parent.
**
** This routine gets called after you memcpy() one page into
** another.
*/
static void reparentChildPages(Btree *pBt, MemPage *pPage){
  int i;
  Pager *pPager = pBt->pPager;
  for(i=0; i<pPage->nCell; i++){
    reparentPage(pPager, SWAB32(pBt, pPage->apCell[i]->h.leftChild), pPage, i);
  }
  reparentPage(pPager, SWAB32(pBt, pPage->u.hdr.rightChild), pPage, i);
  pPage->idxShift = 0;
}

/*
** Remove the i-th cell from pPage.  This routine effects pPage only.
** The cell content is not freed or deallocated.  It is assumed that
** the cell content has been copied someplace else.  This routine just
** removes the reference to the cell from pPage.
**
** "sz" must be the number of bytes in the cell.
**
** Do not bother maintaining the integrity of the linked list of Cells.
** Only the pPage->apCell[] array is important.  The relinkCellList() 
** routine will be called soon after this routine in order to rebuild 
** the linked list.
*/
static void dropCell(Btree *pBt, MemPage *pPage, int idx, int sz){
  int j;
  assert( idx>=0 && idx<pPage->nCell );
  assert( sz==cellSize(pBt, pPage->apCell[idx]) );
  assert( sqlitepager_iswriteable(pPage) );
  freeSpace(pBt, pPage, Addr(pPage->apCell[idx]) - Addr(pPage), sz);
  for(j=idx; j<pPage->nCell-1; j++){
    pPage->apCell[j] = pPage->apCell[j+1];
  }
  pPage->nCell--;
  pPage->idxShift = 1;
}

/*
** Insert a new cell on pPage at cell index "i".  pCell points to the
** content of the cell.
**
** If the cell content will fit on the page, then put it there.  If it
** will not fit, then just make pPage->apCell[i] point to the content
** and set pPage->isOverfull.  
**
** Do not bother maintaining the integrity of the linked list of Cells.
** Only the pPage->apCell[] array is important.  The relinkCellList() 
** routine will be called soon after this routine in order to rebuild 
** the linked list.
*/
static void insertCell(Btree *pBt, MemPage *pPage, int i, Cell *pCell, int sz){
  int idx, j;
  assert( i>=0 && i<=pPage->nCell );
  assert( sz==cellSize(pBt, pCell) );
  assert( sqlitepager_iswriteable(pPage) );
  idx = allocateSpace(pBt, pPage, sz);
  for(j=pPage->nCell; j>i; j--){
    pPage->apCell[j] = pPage->apCell[j-1];
  }
  pPage->nCell++;
  if( idx<=0 ){
    pPage->isOverfull = 1;
    pPage->apCell[i] = pCell;
  }else{
    memcpy(&pPage->u.aDisk[idx], pCell, sz);
    pPage->apCell[i] = (Cell*)&pPage->u.aDisk[idx];
  }
  pPage->idxShift = 1;
}

/*
** Rebuild the linked list of cells on a page so that the cells
** occur in the order specified by the pPage->apCell[] array.  
** Invoke this routine once to repair damage after one or more
** invocations of either insertCell() or dropCell().
*/
static void relinkCellList(Btree *pBt, MemPage *pPage){
  int i;
  u16 *pIdx;
  assert( sqlitepager_iswriteable(pPage) );
  pIdx = &pPage->u.hdr.firstCell;
  for(i=0; i<pPage->nCell; i++){
    int idx = Addr(pPage->apCell[i]) - Addr(pPage);
    assert( idx>0 && idx<SQLITE_USABLE_SIZE );
    *pIdx = SWAB16(pBt, idx);
    pIdx = &pPage->apCell[i]->h.iNext;
  }
  *pIdx = 0;
}

/*
** Make a copy of the contents of pFrom into pTo.  The pFrom->apCell[]
** pointers that point into pFrom->u.aDisk[] must be adjusted to point
** into pTo->u.aDisk[] instead.  But some pFrom->apCell[] entries might
** not point to pFrom->u.aDisk[].  Those are unchanged.
*/
static void copyPage(MemPage *pTo, MemPage *pFrom){
  uptr from, to;
  int i;
  memcpy(pTo->u.aDisk, pFrom->u.aDisk, SQLITE_USABLE_SIZE);
  pTo->pParent = 0;
  pTo->isInit = 1;
  pTo->nCell = pFrom->nCell;
  pTo->nFree = pFrom->nFree;
  pTo->isOverfull = pFrom->isOverfull;
  to = Addr(pTo);
  from = Addr(pFrom);
  for(i=0; i<pTo->nCell; i++){
    uptr x = Addr(pFrom->apCell[i]);
    if( x>from && x<from+SQLITE_USABLE_SIZE ){
      *((uptr*)&pTo->apCell[i]) = x + to - from;
    }else{
      pTo->apCell[i] = pFrom->apCell[i];
    }
  }
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

/*
** This routine redistributes Cells on pPage and up to two siblings
** of pPage so that all pages have about the same amount of free space.
** Usually one sibling on either side of pPage is used in the balancing,
** though both siblings might come from one side if pPage is the first
** or last child of its parent.  If pPage has fewer than two siblings
** (something which can only happen if pPage is the root page or a 
** child of root) then all available siblings participate in the balancing.
**
** The number of siblings of pPage might be increased or decreased by
** one in an effort to keep pages between 66% and 100% full. The root page
** is special and is allowed to be less than 66% full. If pPage is 
** the root page, then the depth of the tree might be increased
** or decreased by one, as necessary, to keep the root page from being
** overfull or empty.
**
** This routine calls relinkCellList() on its input page regardless of
** whether or not it does any real balancing.  Client routines will typically
** invoke insertCell() or dropCell() before calling this routine, so we
** need to call relinkCellList() to clean up the mess that those other
** routines left behind.
**
** pCur is left pointing to the same cell as when this routine was called
** even if that cell gets moved to a different page.  pCur may be NULL.
** Set the pCur parameter to NULL if you do not care about keeping track
** of a cell as that will save this routine the work of keeping track of it.
**
** Note that when this routine is called, some of the Cells on pPage
** might not actually be stored in pPage->u.aDisk[].  This can happen
** if the page is overfull.  Part of the job of this routine is to
** make sure all Cells for pPage once again fit in pPage->u.aDisk[].
**
** In the course of balancing the siblings of pPage, the parent of pPage
** might become overfull or underfull.  If that happens, then this routine
** is called recursively on the parent.
**
** If this routine fails for any reason, it might leave the database
** in a corrupted state.  So if this routine fails, the database should
** be rolled back.
*/
static int balance(Btree *pBt, MemPage *pPage, BtCursor *pCur){
  MemPage *pParent;            /* The parent of pPage */
  int nCell;                   /* Number of cells in apCell[] */
  int nOld;                    /* Number of pages in apOld[] */
  int nNew;                    /* Number of pages in apNew[] */
  int nDiv;                    /* Number of cells in apDiv[] */
  int i, j, k;                 /* Loop counters */
  int idx;                     /* Index of pPage in pParent->apCell[] */
  int nxDiv;                   /* Next divider slot in pParent->apCell[] */
  int rc;                      /* The return code */
  int iCur;                    /* apCell[iCur] is the cell of the cursor */
  MemPage *pOldCurPage;        /* The cursor originally points to this page */
  int subtotal;                /* Subtotal of bytes in cells on one page */
  MemPage *extraUnref = 0;     /* A page that needs to be unref-ed */
  MemPage *apOld[NB];          /* pPage and up to two siblings */
  Pgno pgnoOld[NB];            /* Page numbers for each page in apOld[] */
  MemPage *apNew[NB+1];        /* pPage and up to NB siblings after balancing */
  Pgno pgnoNew[NB+1];          /* Page numbers for each page in apNew[] */
  int idxDiv[NB];              /* Indices of divider cells in pParent */
  Cell *apDiv[NB];             /* Divider cells in pParent */
  Cell aTemp[NB];              /* Temporary holding area for apDiv[] */
  int cntNew[NB+1];            /* Index in apCell[] of cell after i-th page */
  int szNew[NB+1];             /* Combined size of cells place on i-th page */
  MemPage aOld[NB];            /* Temporary copies of pPage and its siblings */
  Cell *apCell[(MX_CELL+2)*NB]; /* All cells from pages being balanced */
  int szCell[(MX_CELL+2)*NB];  /* Local size of all cells */

  /* 
  ** Return without doing any work if pPage is neither overfull nor
  ** underfull.
  */
  assert( sqlitepager_iswriteable(pPage) );
  if( !pPage->isOverfull && pPage->nFree<SQLITE_USABLE_SIZE/2 
        && pPage->nCell>=2){
    relinkCellList(pBt, pPage);
    return SQLITE_OK;
  }

  /*
  ** Find the parent of the page to be balanceed.
  ** If there is no parent, it means this page is the root page and
  ** special rules apply.
  */
  pParent = pPage->pParent;
  if( pParent==0 ){
    Pgno pgnoChild;
    MemPage *pChild;
    assert( pPage->isInit );
    if( pPage->nCell==0 ){
      if( pPage->u.hdr.rightChild ){
        /*
        ** The root page is empty.  Copy the one child page
        ** into the root page and return.  This reduces the depth
        ** of the BTree by one.
        */
        pgnoChild = SWAB32(pBt, pPage->u.hdr.rightChild);
        rc = sqlitepager_get(pBt->pPager, pgnoChild, (void**)&pChild);
        if( rc ) return rc;
        memcpy(pPage, pChild, SQLITE_USABLE_SIZE);
        pPage->isInit = 0;
        rc = initPage(pBt, pPage, sqlitepager_pagenumber(pPage), 0);
        assert( rc==SQLITE_OK );
        reparentChildPages(pBt, pPage);
        if( pCur && pCur->pPage==pChild ){
          sqlitepager_unref(pChild);
          pCur->pPage = pPage;
          sqlitepager_ref(pPage);
        }
        freePage(pBt, pChild, pgnoChild);
        sqlitepager_unref(pChild);
      }else{
        relinkCellList(pBt, pPage);
      }
      return SQLITE_OK;
    }
    if( !pPage->isOverfull ){
      /* It is OK for the root page to be less than half full.
      */
      relinkCellList(pBt, pPage);
      return SQLITE_OK;
    }
    /*
    ** If we get to here, it means the root page is overfull.
    ** When this happens, Create a new child page and copy the
    ** contents of the root into the child.  Then make the root
    ** page an empty page with rightChild pointing to the new
    ** child.  Then fall thru to the code below which will cause
    ** the overfull child page to be split.
    */
    rc = sqlitepager_write(pPage);
    if( rc ) return rc;
    rc = allocatePage(pBt, &pChild, &pgnoChild, sqlitepager_pagenumber(pPage));
    if( rc ) return rc;
    assert( sqlitepager_iswriteable(pChild) );
    copyPage(pChild, pPage);
    pChild->pParent = pPage;
    pChild->idxParent = 0;
    sqlitepager_ref(pPage);
    pChild->isOverfull = 1;
    if( pCur && pCur->pPage==pPage ){
      sqlitepager_unref(pPage);
      pCur->pPage = pChild;
    }else{
      extraUnref = pChild;
    }
    zeroPage(pBt, pPage);
    pPage->u.hdr.rightChild = SWAB32(pBt, pgnoChild);
    pParent = pPage;
    pPage = pChild;
  }
  rc = sqlitepager_write(pParent);
  if( rc ) return rc;
  assert( pParent->isInit );
  
  /*
  ** Find the Cell in the parent page whose h.leftChild points back
  ** to pPage.  The "idx" variable is the index of that cell.  If pPage
  ** is the rightmost child of pParent then set idx to pParent->nCell 
  */
  if( pParent->idxShift ){
    Pgno pgno, swabPgno;
    pgno = sqlitepager_pagenumber(pPage);
    swabPgno = SWAB32(pBt, pgno);
    for(idx=0; idx<pParent->nCell; idx++){
      if( pParent->apCell[idx]->h.leftChild==swabPgno ){
        break;
      }
    }
    assert( idx<pParent->nCell || pParent->u.hdr.rightChild==swabPgno );
  }else{
    idx = pPage->idxParent;
  }

  /*
  ** Initialize variables so that it will be safe to jump
  ** directly to balance_cleanup at any moment.
  */
  nOld = nNew = 0;
  sqlitepager_ref(pParent);

  /*
  ** Find sibling pages to pPage and the Cells in pParent that divide
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
      idxDiv[i] = k;
      apDiv[i] = pParent->apCell[k];
      nDiv++;
      pgnoOld[i] = SWAB32(pBt, apDiv[i]->h.leftChild);
    }else if( k==pParent->nCell ){
      pgnoOld[i] = SWAB32(pBt, pParent->u.hdr.rightChild);
    }else{
      break;
    }
    rc = sqlitepager_get(pBt->pPager, pgnoOld[i], (void**)&apOld[i]);
    if( rc ) goto balance_cleanup;
    rc = initPage(pBt, apOld[i], pgnoOld[i], pParent);
    if( rc ) goto balance_cleanup;
    apOld[i]->idxParent = k;
    nOld++;
  }

  /*
  ** Set iCur to be the index in apCell[] of the cell that the cursor
  ** is pointing to.  We will need this later on in order to keep the
  ** cursor pointing at the same cell.  If pCur points to a page that
  ** has no involvement with this rebalancing, then set iCur to a large
  ** number so that the iCur==j tests always fail in the main cell
  ** distribution loop below.
  */
  if( pCur ){
    iCur = 0;
    for(i=0; i<nOld; i++){
      if( pCur->pPage==apOld[i] ){
        iCur += pCur->idx;
        break;
      }
      iCur += apOld[i]->nCell;
      if( i<nOld-1 && pCur->pPage==pParent && pCur->idx==idxDiv[i] ){
        break;
      }
      iCur++;
    }
    pOldCurPage = pCur->pPage;
  }

  /*
  ** Make copies of the content of pPage and its siblings into aOld[].
  ** The rest of this function will use data from the copies rather
  ** that the original pages since the original pages will be in the
  ** process of being overwritten.
  */
  for(i=0; i<nOld; i++){
    copyPage(&aOld[i], apOld[i]);
  }

  /*
  ** Load pointers to all cells on sibling pages and the divider cells
  ** into the local apCell[] array.  Make copies of the divider cells
  ** into aTemp[] and remove the the divider Cells from pParent.
  */
  nCell = 0;
  for(i=0; i<nOld; i++){
    MemPage *pOld = &aOld[i];
    for(j=0; j<pOld->nCell; j++){
      apCell[nCell] = pOld->apCell[j];
      szCell[nCell] = cellSize(pBt, apCell[nCell]);
      nCell++;
    }
    if( i<nOld-1 ){
      szCell[nCell] = cellSize(pBt, apDiv[i]);
      memcpy(&aTemp[i], apDiv[i], szCell[nCell]);
      apCell[nCell] = &aTemp[i];
      dropCell(pBt, pParent, nxDiv, szCell[nCell]);
      assert( SWAB32(pBt, apCell[nCell]->h.leftChild)==pgnoOld[i] );
      apCell[nCell]->h.leftChild = pOld->u.hdr.rightChild;
      nCell++;
    }
  }

  /*
  ** Figure out the number of pages needed to hold all nCell cells.
  ** Store this number in "k".  Also compute szNew[] which is the total
  ** size of all cells on the i-th page and cntNew[] which is the index
  ** in apCell[] of the cell that divides path i from path i+1.  
  ** cntNew[k] should equal nCell.
  **
  ** This little patch of code is critical for keeping the tree
  ** balanced. 
  */
  for(subtotal=k=i=0; i<nCell; i++){
    subtotal += szCell[i];
    if( subtotal > USABLE_SPACE ){
      szNew[k] = subtotal - szCell[i];
      cntNew[k] = i;
      subtotal = 0;
      k++;
    }
  }
  szNew[k] = subtotal;
  cntNew[k] = nCell;
  k++;
  for(i=k-1; i>0; i--){
    while( szNew[i]<USABLE_SPACE/2 ){
      cntNew[i-1]--;
      assert( cntNew[i-1]>0 );
      szNew[i] += szCell[cntNew[i-1]];
      szNew[i-1] -= szCell[cntNew[i-1]-1];
    }
  }
  assert( cntNew[0]>0 );

  /*
  ** Allocate k new pages.  Reuse old pages where possible.
  */
  for(i=0; i<k; i++){
    if( i<nOld ){
      apNew[i] = apOld[i];
      pgnoNew[i] = pgnoOld[i];
      apOld[i] = 0;
      sqlitepager_write(apNew[i]);
    }else{
      rc = allocatePage(pBt, &apNew[i], &pgnoNew[i], pgnoNew[i-1]);
      if( rc ) goto balance_cleanup;
    }
    nNew++;
    zeroPage(pBt, apNew[i]);
    apNew[i]->isInit = 1;
  }

  /* Free any old pages that were not reused as new pages.
  */
  while( i<nOld ){
    rc = freePage(pBt, apOld[i], pgnoOld[i]);
    if( rc ) goto balance_cleanup;
    sqlitepager_unref(apOld[i]);
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

  /*
  ** Evenly distribute the data in apCell[] across the new pages.
  ** Insert divider cells into pParent as necessary.
  */
  j = 0;
  for(i=0; i<nNew; i++){
    MemPage *pNew = apNew[i];
    while( j<cntNew[i] ){
      assert( pNew->nFree>=szCell[j] );
      if( pCur && iCur==j ){ pCur->pPage = pNew; pCur->idx = pNew->nCell; }
      insertCell(pBt, pNew, pNew->nCell, apCell[j], szCell[j]);
      j++;
    }
    assert( pNew->nCell>0 );
    assert( !pNew->isOverfull );
    relinkCellList(pBt, pNew);
    if( i<nNew-1 && j<nCell ){
      pNew->u.hdr.rightChild = apCell[j]->h.leftChild;
      apCell[j]->h.leftChild = SWAB32(pBt, pgnoNew[i]);
      if( pCur && iCur==j ){ pCur->pPage = pParent; pCur->idx = nxDiv; }
      insertCell(pBt, pParent, nxDiv, apCell[j], szCell[j]);
      j++;
      nxDiv++;
    }
  }
  assert( j==nCell );
  apNew[nNew-1]->u.hdr.rightChild = aOld[nOld-1].u.hdr.rightChild;
  if( nxDiv==pParent->nCell ){
    pParent->u.hdr.rightChild = SWAB32(pBt, pgnoNew[nNew-1]);
  }else{
    pParent->apCell[nxDiv]->h.leftChild = SWAB32(pBt, pgnoNew[nNew-1]);
  }
  if( pCur ){
    if( j<=iCur && pCur->pPage==pParent && pCur->idx>idxDiv[nOld-1] ){
      assert( pCur->pPage==pOldCurPage );
      pCur->idx += nNew - nOld;
    }else{
      assert( pOldCurPage!=0 );
      sqlitepager_ref(pCur->pPage);
      sqlitepager_unref(pOldCurPage);
    }
  }

  /*
  ** Reparent children of all cells.
  */
  for(i=0; i<nNew; i++){
    reparentChildPages(pBt, apNew[i]);
  }
  reparentChildPages(pBt, pParent);

  /*
  ** balance the parent page.
  */
  rc = balance(pBt, pParent, pCur);

  /*
  ** Cleanup before returning.
  */
balance_cleanup:
  if( extraUnref ){
    sqlitepager_unref(extraUnref);
  }
  for(i=0; i<nOld; i++){
    if( apOld[i]!=0 && apOld[i]!=&aOld[i] ) sqlitepager_unref(apOld[i]);
  }
  for(i=0; i<nNew; i++){
    sqlitepager_unref(apNew[i]);
  }
  if( pCur && pCur->pPage==0 ){
    pCur->pPage = pParent;
    pCur->idx = 0;
  }else{
    sqlitepager_unref(pParent);
  }
  return rc;
}

/*
** This routine checks all cursors that point to the same table
** as pCur points to.  If any of those cursors were opened with
** wrFlag==0 then this routine returns SQLITE_LOCKED.  If all
** cursors point to the same table were opened with wrFlag==1
** then this routine returns SQLITE_OK.
**
** In addition to checking for read-locks (where a read-lock 
** means a cursor opened with wrFlag==0) this routine also moves
** all cursors other than pCur so that they are pointing to the 
** first Cell on root page.  This is necessary because an insert 
** or delete might change the number of cells on a page or delete
** a page entirely and we do not want to leave any cursors 
** pointing to non-existant pages or cells.
*/
static int checkReadLocks(BtCursor *pCur){
  BtCursor *p;
  assert( pCur->wrFlag );
  for(p=pCur->pShared; p!=pCur; p=p->pShared){
    assert( p );
    assert( p->pgnoRoot==pCur->pgnoRoot );
    if( p->wrFlag==0 ) return SQLITE_LOCKED;
    if( sqlitepager_pagenumber(p->pPage)!=p->pgnoRoot ){
      moveToRoot(p);
    }
  }
  return SQLITE_OK;
}

/*
** Insert a new record into the BTree.  The key is given by (pKey,nKey)
** and the data is given by (pData,nData).  The cursor is used only to
** define what database the record should be inserted into.  The cursor
** is left pointing at the new record.
*/
static int fileBtreeInsert(
  BtCursor *pCur,                /* Insert data into the table of this cursor */
  const void *pKey, int nKey,    /* The key of the new record */
  const void *pData, int nData   /* The data of the new record */
){
  Cell newCell;
  int rc;
  int loc;
  int szNew;
  MemPage *pPage;
  Btree *pBt = pCur->pBt;

  if( pCur->pPage==0 ){
    return SQLITE_ABORT;  /* A rollback destroyed this cursor */
  }
  if( !pBt->inTrans || nKey+nData==0 ){
    /* Must start a transaction before doing an insert */
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  assert( !pBt->readOnly );
  if( !pCur->wrFlag ){
    return SQLITE_PERM;   /* Cursor not open for writing */
  }
  if( checkReadLocks(pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }
  rc = fileBtreeMoveto(pCur, pKey, nKey, &loc);
  if( rc ) return rc;
  pPage = pCur->pPage;
  assert( pPage->isInit );
  rc = sqlitepager_write(pPage);
  if( rc ) return rc;
  rc = fillInCell(pBt, &newCell, pKey, nKey, pData, nData);
  if( rc ) return rc;
  szNew = cellSize(pBt, &newCell);
  if( loc==0 ){
    newCell.h.leftChild = pPage->apCell[pCur->idx]->h.leftChild;
    rc = clearCell(pBt, pPage->apCell[pCur->idx]);
    if( rc ) return rc;
    dropCell(pBt, pPage, pCur->idx, cellSize(pBt, pPage->apCell[pCur->idx]));
  }else if( loc<0 && pPage->nCell>0 ){
    assert( pPage->u.hdr.rightChild==0 );  /* Must be a leaf page */
    pCur->idx++;
  }else{
    assert( pPage->u.hdr.rightChild==0 );  /* Must be a leaf page */
  }
  insertCell(pBt, pPage, pCur->idx, &newCell, szNew);
  rc = balance(pCur->pBt, pPage, pCur);
  /* sqliteBtreePageDump(pCur->pBt, pCur->pgnoRoot, 1); */
  /* fflush(stdout); */
  pCur->eSkip = SKIP_INVALID;
  return rc;
}

/*
** Delete the entry that the cursor is pointing to.
**
** The cursor is left pointing at either the next or the previous
** entry.  If the cursor is left pointing to the next entry, then 
** the pCur->eSkip flag is set to SKIP_NEXT which forces the next call to 
** sqliteBtreeNext() to be a no-op.  That way, you can always call
** sqliteBtreeNext() after a delete and the cursor will be left
** pointing to the first entry after the deleted entry.  Similarly,
** pCur->eSkip is set to SKIP_PREV is the cursor is left pointing to
** the entry prior to the deleted entry so that a subsequent call to
** sqliteBtreePrevious() will always leave the cursor pointing at the
** entry immediately before the one that was deleted.
*/
static int fileBtreeDelete(BtCursor *pCur){
  MemPage *pPage = pCur->pPage;
  Cell *pCell;
  int rc;
  Pgno pgnoChild;
  Btree *pBt = pCur->pBt;

  assert( pPage->isInit );
  if( pCur->pPage==0 ){
    return SQLITE_ABORT;  /* A rollback destroyed this cursor */
  }
  if( !pBt->inTrans ){
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
  if( checkReadLocks(pCur) ){
    return SQLITE_LOCKED; /* The table pCur points to has a read lock */
  }
  rc = sqlitepager_write(pPage);
  if( rc ) return rc;
  pCell = pPage->apCell[pCur->idx];
  pgnoChild = SWAB32(pBt, pCell->h.leftChild);
  clearCell(pBt, pCell);
  if( pgnoChild ){
    /*
    ** The entry we are about to delete is not a leaf so if we do not
    ** do something we will leave a hole on an internal page.
    ** We have to fill the hole by moving in a cell from a leaf.  The
    ** next Cell after the one to be deleted is guaranteed to exist and
    ** to be a leaf so we can use it.
    */
    BtCursor leafCur;
    Cell *pNext;
    int szNext;
    int notUsed;
    getTempCursor(pCur, &leafCur);
    rc = fileBtreeNext(&leafCur, &notUsed);
    if( rc!=SQLITE_OK ){
      if( rc!=SQLITE_NOMEM ) rc = SQLITE_CORRUPT;
      return rc;
    }
    rc = sqlitepager_write(leafCur.pPage);
    if( rc ) return rc;
    dropCell(pBt, pPage, pCur->idx, cellSize(pBt, pCell));
    pNext = leafCur.pPage->apCell[leafCur.idx];
    szNext = cellSize(pBt, pNext);
    pNext->h.leftChild = SWAB32(pBt, pgnoChild);
    insertCell(pBt, pPage, pCur->idx, pNext, szNext);
    rc = balance(pBt, pPage, pCur);
    if( rc ) return rc;
    pCur->eSkip = SKIP_NEXT;
    dropCell(pBt, leafCur.pPage, leafCur.idx, szNext);
    rc = balance(pBt, leafCur.pPage, pCur);
    releaseTempCursor(&leafCur);
  }else{
    dropCell(pBt, pPage, pCur->idx, cellSize(pBt, pCell));
    if( pCur->idx>=pPage->nCell ){
      pCur->idx = pPage->nCell-1;
      if( pCur->idx<0 ){ 
        pCur->idx = 0;
        pCur->eSkip = SKIP_NEXT;
      }else{
        pCur->eSkip = SKIP_PREV;
      }
    }else{
      pCur->eSkip = SKIP_NEXT;
    }
    rc = balance(pBt, pPage, pCur);
  }
  return rc;
}

/*
** Create a new BTree table.  Write into *piTable the page
** number for the root page of the new table.
**
** In the current implementation, BTree tables and BTree indices are the 
** the same.  In the future, we may change this so that BTree tables
** are restricted to having a 4-byte integer key and arbitrary data and
** BTree indices are restricted to having an arbitrary key and no data.
** But for now, this routine also serves to create indices.
*/
static int fileBtreeCreateTable(Btree *pBt, int *piTable){
  MemPage *pRoot;
  Pgno pgnoRoot;
  int rc;
  if( !pBt->inTrans ){
    /* Must start a transaction first */
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  if( pBt->readOnly ){
    return SQLITE_READONLY;
  }
  rc = allocatePage(pBt, &pRoot, &pgnoRoot, 0);
  if( rc ) return rc;
  assert( sqlitepager_iswriteable(pRoot) );
  zeroPage(pBt, pRoot);
  sqlitepager_unref(pRoot);
  *piTable = (int)pgnoRoot;
  return SQLITE_OK;
}

/*
** Erase the given database page and all its children.  Return
** the page to the freelist.
*/
static int clearDatabasePage(Btree *pBt, Pgno pgno, int freePageFlag){
  MemPage *pPage;
  int rc;
  Cell *pCell;
  int idx;

  rc = sqlitepager_get(pBt->pPager, pgno, (void**)&pPage);
  if( rc ) return rc;
  rc = sqlitepager_write(pPage);
  if( rc ) return rc;
  rc = initPage(pBt, pPage, pgno, 0);
  if( rc ) return rc;
  idx = SWAB16(pBt, pPage->u.hdr.firstCell);
  while( idx>0 ){
    pCell = (Cell*)&pPage->u.aDisk[idx];
    idx = SWAB16(pBt, pCell->h.iNext);
    if( pCell->h.leftChild ){
      rc = clearDatabasePage(pBt, SWAB32(pBt, pCell->h.leftChild), 1);
      if( rc ) return rc;
    }
    rc = clearCell(pBt, pCell);
    if( rc ) return rc;
  }
  if( pPage->u.hdr.rightChild ){
    rc = clearDatabasePage(pBt, SWAB32(pBt, pPage->u.hdr.rightChild), 1);
    if( rc ) return rc;
  }
  if( freePageFlag ){
    rc = freePage(pBt, pPage, pgno);
  }else{
    zeroPage(pBt, pPage);
  }
  sqlitepager_unref(pPage);
  return rc;
}

/*
** Delete all information from a single table in the database.
*/
static int fileBtreeClearTable(Btree *pBt, int iTable){
  int rc;
  BtCursor *pCur;
  if( !pBt->inTrans ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    if( pCur->pgnoRoot==(Pgno)iTable ){
      if( pCur->wrFlag==0 ) return SQLITE_LOCKED;
      moveToRoot(pCur);
    }
  }
  rc = clearDatabasePage(pBt, (Pgno)iTable, 0);
  if( rc ){
    fileBtreeRollback(pBt);
  }
  return rc;
}

/*
** Erase all information in a table and add the root of the table to
** the freelist.  Except, the root of the principle table (the one on
** page 2) is never added to the freelist.
*/
static int fileBtreeDropTable(Btree *pBt, int iTable){
  int rc;
  MemPage *pPage;
  BtCursor *pCur;
  if( !pBt->inTrans ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  for(pCur=pBt->pCursor; pCur; pCur=pCur->pNext){
    if( pCur->pgnoRoot==(Pgno)iTable ){
      return SQLITE_LOCKED;  /* Cannot drop a table that has a cursor */
    }
  }
  rc = sqlitepager_get(pBt->pPager, (Pgno)iTable, (void**)&pPage);
  if( rc ) return rc;
  rc = fileBtreeClearTable(pBt, iTable);
  if( rc ) return rc;
  if( iTable>2 ){
    rc = freePage(pBt, pPage, iTable);
  }else{
    zeroPage(pBt, pPage);
  }
  sqlitepager_unref(pPage);
  return rc;  
}

#if 0 /* UNTESTED */
/*
** Copy all cell data from one database file into another.
** pages back the freelist.
*/
static int copyCell(Btree *pBtFrom, BTree *pBtTo, Cell *pCell){
  Pager *pFromPager = pBtFrom->pPager;
  OverflowPage *pOvfl;
  Pgno ovfl, nextOvfl;
  Pgno *pPrev;
  int rc = SQLITE_OK;
  MemPage *pNew, *pPrevPg;
  Pgno new;

  if( NKEY(pBtTo, pCell->h) + NDATA(pBtTo, pCell->h) <= MX_LOCAL_PAYLOAD ){
    return SQLITE_OK;
  }
  pPrev = &pCell->ovfl;
  pPrevPg = 0;
  ovfl = SWAB32(pBtTo, pCell->ovfl);
  while( ovfl && rc==SQLITE_OK ){
    rc = sqlitepager_get(pFromPager, ovfl, (void**)&pOvfl);
    if( rc ) return rc;
    nextOvfl = SWAB32(pBtFrom, pOvfl->iNext);
    rc = allocatePage(pBtTo, &pNew, &new, 0);
    if( rc==SQLITE_OK ){
      rc = sqlitepager_write(pNew);
      if( rc==SQLITE_OK ){
        memcpy(pNew, pOvfl, SQLITE_USABLE_SIZE);
        *pPrev = SWAB32(pBtTo, new);
        if( pPrevPg ){
          sqlitepager_unref(pPrevPg);
        }
        pPrev = &pOvfl->iNext;
        pPrevPg = pNew;
      }
    }
    sqlitepager_unref(pOvfl);
    ovfl = nextOvfl;
  }
  if( pPrevPg ){
    sqlitepager_unref(pPrevPg);
  }
  return rc;
}
#endif


#if 0 /* UNTESTED */
/*
** Copy a page of data from one database over to another.
*/
static int copyDatabasePage(
  Btree *pBtFrom,
  Pgno pgnoFrom,
  Btree *pBtTo,
  Pgno *pTo
){
  MemPage *pPageFrom, *pPage;
  Pgno to;
  int rc;
  Cell *pCell;
  int idx;

  rc = sqlitepager_get(pBtFrom->pPager, pgno, (void**)&pPageFrom);
  if( rc ) return rc;
  rc = allocatePage(pBt, &pPage, pTo, 0);
  if( rc==SQLITE_OK ){
    rc = sqlitepager_write(pPage);
  }
  if( rc==SQLITE_OK ){
    memcpy(pPage, pPageFrom, SQLITE_USABLE_SIZE);
    idx = SWAB16(pBt, pPage->u.hdr.firstCell);
    while( idx>0 ){
      pCell = (Cell*)&pPage->u.aDisk[idx];
      idx = SWAB16(pBt, pCell->h.iNext);
      if( pCell->h.leftChild ){
        Pgno newChld;
        rc = copyDatabasePage(pBtFrom, SWAB32(pBtFrom, pCell->h.leftChild),
                              pBtTo, &newChld);
        if( rc ) return rc;
        pCell->h.leftChild = SWAB32(pBtFrom, newChld);
      }
      rc = copyCell(pBtFrom, pBtTo, pCell);
      if( rc ) return rc;
    }
    if( pPage->u.hdr.rightChild ){
      Pgno newChld;
      rc = copyDatabasePage(pBtFrom, SWAB32(pBtFrom, pPage->u.hdr.rightChild), 
                            pBtTo, &newChld);
      if( rc ) return rc;
      pPage->u.hdr.rightChild = SWAB32(pBtTo, newChild);
    }
  }
  sqlitepager_unref(pPage);
  return rc;
}
#endif

/*
** Read the meta-information out of a database file.
*/
static int fileBtreeGetMeta(Btree *pBt, int *aMeta){
  PageOne *pP1;
  int rc;
  int i;

  rc = sqlitepager_get(pBt->pPager, 1, (void**)&pP1);
  if( rc ) return rc;
  aMeta[0] = SWAB32(pBt, pP1->nFree);
  for(i=0; i<sizeof(pP1->aMeta)/sizeof(pP1->aMeta[0]); i++){
    aMeta[i+1] = SWAB32(pBt, pP1->aMeta[i]);
  }
  sqlitepager_unref(pP1);
  return SQLITE_OK;
}

/*
** Write meta-information back into the database.
*/
static int fileBtreeUpdateMeta(Btree *pBt, int *aMeta){
  PageOne *pP1;
  int rc, i;
  if( !pBt->inTrans ){
    return pBt->readOnly ? SQLITE_READONLY : SQLITE_ERROR;
  }
  pP1 = pBt->page1;
  rc = sqlitepager_write(pP1);
  if( rc ) return rc;   
  for(i=0; i<sizeof(pP1->aMeta)/sizeof(pP1->aMeta[0]); i++){
    pP1->aMeta[i] = SWAB32(pBt, aMeta[i+1]);
  }
  return SQLITE_OK;
}

/******************************************************************************
** The complete implementation of the BTree subsystem is above this line.
** All the code the follows is for testing and troubleshooting the BTree
** subsystem.  None of the code that follows is used during normal operation.
******************************************************************************/

/*
** Print a disassembly of the given page on standard output.  This routine
** is used for debugging and testing only.
*/
#ifdef SQLITE_TEST
static int fileBtreePageDump(Btree *pBt, int pgno, int recursive){
  int rc;
  MemPage *pPage;
  int i, j;
  int nFree;
  u16 idx;
  char range[20];
  unsigned char payload[20];
  rc = sqlitepager_get(pBt->pPager, (Pgno)pgno, (void**)&pPage);
  if( rc ){
    return rc;
  }
  if( recursive ) printf("PAGE %d:\n", pgno);
  i = 0;
  idx = SWAB16(pBt, pPage->u.hdr.firstCell);
  while( idx>0 && idx<=SQLITE_USABLE_SIZE-MIN_CELL_SIZE ){
    Cell *pCell = (Cell*)&pPage->u.aDisk[idx];
    int sz = cellSize(pBt, pCell);
    sprintf(range,"%d..%d", idx, idx+sz-1);
    sz = NKEY(pBt, pCell->h) + NDATA(pBt, pCell->h);
    if( sz>sizeof(payload)-1 ) sz = sizeof(payload)-1;
    memcpy(payload, pCell->aPayload, sz);
    for(j=0; j<sz; j++){
      if( payload[j]<0x20 || payload[j]>0x7f ) payload[j] = '.';
    }
    payload[sz] = 0;
    printf(
      "cell %2d: i=%-10s chld=%-4d nk=%-4d nd=%-4d payload=%s\n",
      i, range, (int)pCell->h.leftChild, 
      NKEY(pBt, pCell->h), NDATA(pBt, pCell->h),
      payload
    );
    if( pPage->isInit && pPage->apCell[i]!=pCell ){
      printf("**** apCell[%d] does not match on prior entry ****\n", i);
    }
    i++;
    idx = SWAB16(pBt, pCell->h.iNext);
  }
  if( idx!=0 ){
    printf("ERROR: next cell index out of range: %d\n", idx);
  }
  printf("right_child: %d\n", SWAB32(pBt, pPage->u.hdr.rightChild));
  nFree = 0;
  i = 0;
  idx = SWAB16(pBt, pPage->u.hdr.firstFree);
  while( idx>0 && idx<SQLITE_USABLE_SIZE ){
    FreeBlk *p = (FreeBlk*)&pPage->u.aDisk[idx];
    sprintf(range,"%d..%d", idx, idx+p->iSize-1);
    nFree += SWAB16(pBt, p->iSize);
    printf("freeblock %2d: i=%-10s size=%-4d total=%d\n",
       i, range, SWAB16(pBt, p->iSize), nFree);
    idx = SWAB16(pBt, p->iNext);
    i++;
  }
  if( idx!=0 ){
    printf("ERROR: next freeblock index out of range: %d\n", idx);
  }
  if( recursive && pPage->u.hdr.rightChild!=0 ){
    idx = SWAB16(pBt, pPage->u.hdr.firstCell);
    while( idx>0 && idx<SQLITE_USABLE_SIZE-MIN_CELL_SIZE ){
      Cell *pCell = (Cell*)&pPage->u.aDisk[idx];
      fileBtreePageDump(pBt, SWAB32(pBt, pCell->h.leftChild), 1);
      idx = SWAB16(pBt, pCell->h.iNext);
    }
    fileBtreePageDump(pBt, SWAB32(pBt, pPage->u.hdr.rightChild), 1);
  }
  sqlitepager_unref(pPage);
  return SQLITE_OK;
}
#endif

#ifdef SQLITE_TEST
/*
** Fill aResult[] with information about the entry and page that the
** cursor is pointing to.
** 
**   aResult[0] =  The page number
**   aResult[1] =  The entry number
**   aResult[2] =  Total number of entries on this page
**   aResult[3] =  Size of this entry
**   aResult[4] =  Number of free bytes on this page
**   aResult[5] =  Number of free blocks on the page
**   aResult[6] =  Page number of the left child of this entry
**   aResult[7] =  Page number of the right child for the whole page
**
** This routine is used for testing and debugging only.
*/
static int fileBtreeCursorDump(BtCursor *pCur, int *aResult){
  int cnt, idx;
  MemPage *pPage = pCur->pPage;
  Btree *pBt = pCur->pBt;
  aResult[0] = sqlitepager_pagenumber(pPage);
  aResult[1] = pCur->idx;
  aResult[2] = pPage->nCell;
  if( pCur->idx>=0 && pCur->idx<pPage->nCell ){
    aResult[3] = cellSize(pBt, pPage->apCell[pCur->idx]);
    aResult[6] = SWAB32(pBt, pPage->apCell[pCur->idx]->h.leftChild);
  }else{
    aResult[3] = 0;
    aResult[6] = 0;
  }
  aResult[4] = pPage->nFree;
  cnt = 0;
  idx = SWAB16(pBt, pPage->u.hdr.firstFree);
  while( idx>0 && idx<SQLITE_USABLE_SIZE ){
    cnt++;
    idx = SWAB16(pBt, ((FreeBlk*)&pPage->u.aDisk[idx])->iNext);
  }
  aResult[5] = cnt;
  aResult[7] = SWAB32(pBt, pPage->u.hdr.rightChild);
  return SQLITE_OK;
}
#endif

/*
** Return the pager associated with a BTree.  This routine is used for
** testing and debugging only.
*/
static Pager *fileBtreePager(Btree *pBt){
  return pBt->pPager;
}

/*
** This structure is passed around through all the sanity checking routines
** in order to keep track of some global state information.
*/
typedef struct IntegrityCk IntegrityCk;
struct IntegrityCk {
  Btree *pBt;    /* The tree being checked out */
  Pager *pPager; /* The associated pager.  Also accessible by pBt->pPager */
  int nPage;     /* Number of pages in the database */
  int *anRef;    /* Number of times each page is referenced */
  char *zErrMsg; /* An error message.  NULL of no errors seen. */
};

/*
** Append a message to the error message string.
*/
static void checkAppendMsg(IntegrityCk *pCheck, char *zMsg1, char *zMsg2){
  if( pCheck->zErrMsg ){
    char *zOld = pCheck->zErrMsg;
    pCheck->zErrMsg = 0;
    sqliteSetString(&pCheck->zErrMsg, zOld, "\n", zMsg1, zMsg2, (char*)0);
    sqliteFree(zOld);
  }else{
    sqliteSetString(&pCheck->zErrMsg, zMsg1, zMsg2, (char*)0);
  }
}

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
    char zBuf[100];
    sprintf(zBuf, "invalid page number %d", iPage);
    checkAppendMsg(pCheck, zContext, zBuf);
    return 1;
  }
  if( pCheck->anRef[iPage]==1 ){
    char zBuf[100];
    sprintf(zBuf, "2nd reference to page %d", iPage);
    checkAppendMsg(pCheck, zContext, zBuf);
    return 1;
  }
  return  (pCheck->anRef[iPage]++)>1;
}

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
  char zMsg[100];
  while( N-- > 0 ){
    OverflowPage *pOvfl;
    if( iPage<1 ){
      sprintf(zMsg, "%d pages missing from overflow list", N+1);
      checkAppendMsg(pCheck, zContext, zMsg);
      break;
    }
    if( checkRef(pCheck, iPage, zContext) ) break;
    if( sqlitepager_get(pCheck->pPager, (Pgno)iPage, (void**)&pOvfl) ){
      sprintf(zMsg, "failed to get page %d", iPage);
      checkAppendMsg(pCheck, zContext, zMsg);
      break;
    }
    if( isFreeList ){
      FreelistInfo *pInfo = (FreelistInfo*)pOvfl->aPayload;
      int n = SWAB32(pCheck->pBt, pInfo->nFree);
      for(i=0; i<n; i++){
        checkRef(pCheck, SWAB32(pCheck->pBt, pInfo->aFree[i]), zContext);
      }
      N -= n;
    }
    iPage = SWAB32(pCheck->pBt, pOvfl->iNext);
    sqlitepager_unref(pOvfl);
  }
}

/*
** Return negative if zKey1<zKey2.
** Return zero if zKey1==zKey2.
** Return positive if zKey1>zKey2.
*/
static int keyCompare(
  const char *zKey1, int nKey1,
  const char *zKey2, int nKey2
){
  int min = nKey1>nKey2 ? nKey2 : nKey1;
  int c = memcmp(zKey1, zKey2, min);
  if( c==0 ){
    c = nKey1 - nKey2;
  }
  return c;
}

/*
** Do various sanity checks on a single page of a tree.  Return
** the tree depth.  Root pages return 0.  Parents of root pages
** return 1, and so forth.
** 
** These checks are done:
**
**      1.  Make sure that cells and freeblocks do not overlap
**          but combine to completely cover the page.
**      2.  Make sure cell keys are in order.
**      3.  Make sure no key is less than or equal to zLowerBound.
**      4.  Make sure no key is greater than or equal to zUpperBound.
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
  char *zParentContext, /* Parent context */
  char *zLowerBound,    /* All keys should be greater than this, if not NULL */
  int nLower,           /* Number of characters in zLowerBound */
  char *zUpperBound,    /* All keys should be less than this, if not NULL */
  int nUpper            /* Number of characters in zUpperBound */
){
  MemPage *pPage;
  int i, rc, depth, d2, pgno;
  char *zKey1, *zKey2;
  int nKey1, nKey2;
  BtCursor cur;
  Btree *pBt;
  char zMsg[100];
  char zContext[100];
  char hit[SQLITE_USABLE_SIZE];

  /* Check that the page exists
  */
  cur.pBt = pBt = pCheck->pBt;
  if( iPage==0 ) return 0;
  if( checkRef(pCheck, iPage, zParentContext) ) return 0;
  sprintf(zContext, "On tree page %d: ", iPage);
  if( (rc = sqlitepager_get(pCheck->pPager, (Pgno)iPage, (void**)&pPage))!=0 ){
    sprintf(zMsg, "unable to get the page. error code=%d", rc);
    checkAppendMsg(pCheck, zContext, zMsg);
    return 0;
  }
  if( (rc = initPage(pBt, pPage, (Pgno)iPage, pParent))!=0 ){
    sprintf(zMsg, "initPage() returns error code %d", rc);
    checkAppendMsg(pCheck, zContext, zMsg);
    sqlitepager_unref(pPage);
    return 0;
  }

  /* Check out all the cells.
  */
  depth = 0;
  if( zLowerBound ){
    zKey1 = sqliteMalloc( nLower+1 );
    memcpy(zKey1, zLowerBound, nLower);
    zKey1[nLower] = 0;
  }else{
    zKey1 = 0;
  }
  nKey1 = nLower;
  cur.pPage = pPage;
  for(i=0; i<pPage->nCell; i++){
    Cell *pCell = pPage->apCell[i];
    int sz;

    /* Check payload overflow pages
    */
    nKey2 = NKEY(pBt, pCell->h);
    sz = nKey2 + NDATA(pBt, pCell->h);
    sprintf(zContext, "On page %d cell %d: ", iPage, i);
    if( sz>MX_LOCAL_PAYLOAD ){
      int nPage = (sz - MX_LOCAL_PAYLOAD + OVERFLOW_SIZE - 1)/OVERFLOW_SIZE;
      checkList(pCheck, 0, SWAB32(pBt, pCell->ovfl), nPage, zContext);
    }

    /* Check that keys are in the right order
    */
    cur.idx = i;
    zKey2 = sqliteMallocRaw( nKey2+1 );
    getPayload(&cur, 0, nKey2, zKey2);
    if( zKey1 && keyCompare(zKey1, nKey1, zKey2, nKey2)>=0 ){
      checkAppendMsg(pCheck, zContext, "Key is out of order");
    }

    /* Check sanity of left child page.
    */
    pgno = SWAB32(pBt, pCell->h.leftChild);
    d2 = checkTreePage(pCheck, pgno, pPage, zContext, zKey1,nKey1,zKey2,nKey2);
    if( i>0 && d2!=depth ){
      checkAppendMsg(pCheck, zContext, "Child page depth differs");
    }
    depth = d2;
    sqliteFree(zKey1);
    zKey1 = zKey2;
    nKey1 = nKey2;
  }
  pgno = SWAB32(pBt, pPage->u.hdr.rightChild);
  sprintf(zContext, "On page %d at right child: ", iPage);
  checkTreePage(pCheck, pgno, pPage, zContext, zKey1,nKey1,zUpperBound,nUpper);
  sqliteFree(zKey1);
 
  /* Check for complete coverage of the page
  */
  memset(hit, 0, sizeof(hit));
  memset(hit, 1, sizeof(PageHdr));
  for(i=SWAB16(pBt, pPage->u.hdr.firstCell); i>0 && i<SQLITE_USABLE_SIZE; ){
    Cell *pCell = (Cell*)&pPage->u.aDisk[i];
    int j;
    for(j=i+cellSize(pBt, pCell)-1; j>=i; j--) hit[j]++;
    i = SWAB16(pBt, pCell->h.iNext);
  }
  for(i=SWAB16(pBt,pPage->u.hdr.firstFree); i>0 && i<SQLITE_USABLE_SIZE; ){
    FreeBlk *pFBlk = (FreeBlk*)&pPage->u.aDisk[i];
    int j;
    for(j=i+SWAB16(pBt,pFBlk->iSize)-1; j>=i; j--) hit[j]++;
    i = SWAB16(pBt,pFBlk->iNext);
  }
  for(i=0; i<SQLITE_USABLE_SIZE; i++){
    if( hit[i]==0 ){
      sprintf(zMsg, "Unused space at byte %d of page %d", i, iPage);
      checkAppendMsg(pCheck, zMsg, 0);
      break;
    }else if( hit[i]>1 ){
      sprintf(zMsg, "Multiple uses for byte %d of page %d", i, iPage);
      checkAppendMsg(pCheck, zMsg, 0);
      break;
    }
  }

  /* Check that free space is kept to a minimum
  */
#if 0
  if( pParent && pParent->nCell>2 && pPage->nFree>3*SQLITE_USABLE_SIZE/4 ){
    sprintf(zMsg, "free space (%d) greater than max (%d)", pPage->nFree,
       SQLITE_USABLE_SIZE/3);
    checkAppendMsg(pCheck, zContext, zMsg);
  }
#endif

  sqlitepager_unref(pPage);
  return depth;
}

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
char *fileBtreeIntegrityCheck(Btree *pBt, int *aRoot, int nRoot){
  int i;
  int nRef;
  IntegrityCk sCheck;

  nRef = *sqlitepager_stats(pBt->pPager);
  if( lockBtree(pBt)!=SQLITE_OK ){
    return sqliteStrDup("Unable to acquire a read lock on the database");
  }
  sCheck.pBt = pBt;
  sCheck.pPager = pBt->pPager;
  sCheck.nPage = sqlitepager_pagecount(sCheck.pPager);
  if( sCheck.nPage==0 ){
    unlockBtreeIfUnused(pBt);
    return 0;
  }
  sCheck.anRef = sqliteMallocRaw( (sCheck.nPage+1)*sizeof(sCheck.anRef[0]) );
  sCheck.anRef[1] = 1;
  for(i=2; i<=sCheck.nPage; i++){ sCheck.anRef[i] = 0; }
  sCheck.zErrMsg = 0;

  /* Check the integrity of the freelist
  */
  checkList(&sCheck, 1, SWAB32(pBt, pBt->page1->freeList),
            SWAB32(pBt, pBt->page1->nFree), "Main freelist: ");

  /* Check all the tables.
  */
  for(i=0; i<nRoot; i++){
    if( aRoot[i]==0 ) continue;
    checkTreePage(&sCheck, aRoot[i], 0, "List of tree roots: ", 0,0,0,0);
  }

  /* Make sure every page in the file is referenced
  */
  for(i=1; i<=sCheck.nPage; i++){
    if( sCheck.anRef[i]==0 ){
      char zBuf[100];
      sprintf(zBuf, "Page %d is never used", i);
      checkAppendMsg(&sCheck, zBuf, 0);
    }
  }

  /* Make sure this analysis did not leave any unref() pages
  */
  unlockBtreeIfUnused(pBt);
  if( nRef != *sqlitepager_stats(pBt->pPager) ){
    char zBuf[100];
    sprintf(zBuf, 
      "Outstanding page count goes from %d to %d during this analysis",
      nRef, *sqlitepager_stats(pBt->pPager)
    );
    checkAppendMsg(&sCheck, zBuf, 0);
  }

  /* Clean  up and report errors.
  */
  sqliteFree(sCheck.anRef);
  return sCheck.zErrMsg;
}

/*
** Return the full pathname of the underlying database file.
*/
static const char *fileBtreeGetFilename(Btree *pBt){
  assert( pBt->pPager!=0 );
  return sqlitepager_filename(pBt->pPager);
}

/*
** Copy the complete content of pBtFrom into pBtTo.  A transaction
** must be active for both files.
**
** The size of file pBtFrom may be reduced by this operation.
** If anything goes wrong, the transaction on pBtFrom is rolled back.
*/
static int fileBtreeCopyFile(Btree *pBtTo, Btree *pBtFrom){
  int rc = SQLITE_OK;
  Pgno i, nPage, nToPage;

  if( !pBtTo->inTrans || !pBtFrom->inTrans ) return SQLITE_ERROR;
  if( pBtTo->needSwab!=pBtFrom->needSwab ) return SQLITE_ERROR;
  if( pBtTo->pCursor ) return SQLITE_BUSY;
  memcpy(pBtTo->page1, pBtFrom->page1, SQLITE_USABLE_SIZE);
  rc = sqlitepager_overwrite(pBtTo->pPager, 1, pBtFrom->page1);
  nToPage = sqlitepager_pagecount(pBtTo->pPager);
  nPage = sqlitepager_pagecount(pBtFrom->pPager);
  for(i=2; rc==SQLITE_OK && i<=nPage; i++){
    void *pPage;
    rc = sqlitepager_get(pBtFrom->pPager, i, &pPage);
    if( rc ) break;
    rc = sqlitepager_overwrite(pBtTo->pPager, i, pPage);
    if( rc ) break;
    sqlitepager_unref(pPage);
  }
  for(i=nPage+1; rc==SQLITE_OK && i<=nToPage; i++){
    void *pPage;
    rc = sqlitepager_get(pBtTo->pPager, i, &pPage);
    if( rc ) break;
    rc = sqlitepager_write(pPage);
    sqlitepager_unref(pPage);
    sqlitepager_dont_write(pBtTo->pPager, i);
  }
  if( !rc && nPage<nToPage ){
    rc = sqlitepager_truncate(pBtTo->pPager, nPage);
  }
  if( rc ){
    fileBtreeRollback(pBtTo);
  }
  return rc;  
}

/*
** The following tables contain pointers to all of the interface
** routines for this implementation of the B*Tree backend.  To
** substitute a different implemention of the backend, one has merely
** to provide pointers to alternative functions in similar tables.
*/
static BtOps sqliteBtreeOps = {
    fileBtreeClose,
    fileBtreeSetCacheSize,
    fileBtreeSetSafetyLevel,
    fileBtreeBeginTrans,
    fileBtreeCommit,
    fileBtreeRollback,
    fileBtreeBeginCkpt,
    fileBtreeCommitCkpt,
    fileBtreeRollbackCkpt,
    fileBtreeCreateTable,
    fileBtreeCreateTable,  /* Really sqliteBtreeCreateIndex() */
    fileBtreeDropTable,
    fileBtreeClearTable,
    fileBtreeCursor,
    fileBtreeGetMeta,
    fileBtreeUpdateMeta,
    fileBtreeIntegrityCheck,
    fileBtreeGetFilename,
    fileBtreeCopyFile,
    fileBtreePager,
#ifdef SQLITE_TEST
    fileBtreePageDump,
#endif
};
static BtCursorOps sqliteBtreeCursorOps = {
    fileBtreeMoveto,
    fileBtreeDelete,
    fileBtreeInsert,
    fileBtreeFirst,
    fileBtreeLast,
    fileBtreeNext,
    fileBtreePrevious,
    fileBtreeKeySize,
    fileBtreeKey,
    fileBtreeKeyCompare,
    fileBtreeDataSize,
    fileBtreeData,
    fileBtreeCloseCursor,
#ifdef SQLITE_TEST
    fileBtreeCursorDump,
#endif
};
