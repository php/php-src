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
** This is the implementation of the page cache subsystem or "pager".
** 
** The pager is used to access a database disk file.  It implements
** atomic commit and rollback through the use of a journal file that
** is separate from the database file.  The pager also implements file
** locking to prevent two processes from writing the same database
** file simultaneously, or one process from reading the database while
** another is writing.
**
** @(#) $Id$
*/
#ifndef SQLITE_OMIT_DISKIO
#include "sqliteInt.h"
#include <assert.h>
#include <string.h>

/*
** Macros for troubleshooting.  Normally turned off
*/
#if 0
#define sqlite3DebugPrintf printf
#define PAGERTRACE1(X)       sqlite3DebugPrintf(X)
#define PAGERTRACE2(X,Y)     sqlite3DebugPrintf(X,Y)
#define PAGERTRACE3(X,Y,Z)   sqlite3DebugPrintf(X,Y,Z)
#define PAGERTRACE4(X,Y,Z,W) sqlite3DebugPrintf(X,Y,Z,W)
#define PAGERTRACE5(X,Y,Z,W,V) sqlite3DebugPrintf(X,Y,Z,W,V)
#else
#define PAGERTRACE1(X)
#define PAGERTRACE2(X,Y)
#define PAGERTRACE3(X,Y,Z)
#define PAGERTRACE4(X,Y,Z,W)
#define PAGERTRACE5(X,Y,Z,W,V)
#endif

/*
** The following two macros are used within the PAGERTRACEX() macros above
** to print out file-descriptors. 
**
** PAGERID() takes a pointer to a Pager struct as its argument. The
** associated file-descriptor is returned. FILEHANDLEID() takes an sqlite3_file
** struct as its argument.
*/
#define PAGERID(p) ((int)(p->fd))
#define FILEHANDLEID(fd) ((int)fd)

/*
** The page cache as a whole is always in one of the following
** states:
**
**   PAGER_UNLOCK        The page cache is not currently reading or 
**                       writing the database file.  There is no
**                       data held in memory.  This is the initial
**                       state.
**
**   PAGER_SHARED        The page cache is reading the database.
**                       Writing is not permitted.  There can be
**                       multiple readers accessing the same database
**                       file at the same time.
**
**   PAGER_RESERVED      This process has reserved the database for writing
**                       but has not yet made any changes.  Only one process
**                       at a time can reserve the database.  The original
**                       database file has not been modified so other
**                       processes may still be reading the on-disk
**                       database file.
**
**   PAGER_EXCLUSIVE     The page cache is writing the database.
**                       Access is exclusive.  No other processes or
**                       threads can be reading or writing while one
**                       process is writing.
**
**   PAGER_SYNCED        The pager moves to this state from PAGER_EXCLUSIVE
**                       after all dirty pages have been written to the
**                       database file and the file has been synced to
**                       disk. All that remains to do is to remove or
**                       truncate the journal file and the transaction 
**                       will be committed.
**
** The page cache comes up in PAGER_UNLOCK.  The first time a
** sqlite3PagerGet() occurs, the state transitions to PAGER_SHARED.
** After all pages have been released using sqlite_page_unref(),
** the state transitions back to PAGER_UNLOCK.  The first time
** that sqlite3PagerWrite() is called, the state transitions to
** PAGER_RESERVED.  (Note that sqlite3PagerWrite() can only be
** called on an outstanding page which means that the pager must
** be in PAGER_SHARED before it transitions to PAGER_RESERVED.)
** PAGER_RESERVED means that there is an open rollback journal.
** The transition to PAGER_EXCLUSIVE occurs before any changes
** are made to the database file, though writes to the rollback
** journal occurs with just PAGER_RESERVED.  After an sqlite3PagerRollback()
** or sqlite3PagerCommitPhaseTwo(), the state can go back to PAGER_SHARED,
** or it can stay at PAGER_EXCLUSIVE if we are in exclusive access mode.
*/
#define PAGER_UNLOCK      0
#define PAGER_SHARED      1   /* same as SHARED_LOCK */
#define PAGER_RESERVED    2   /* same as RESERVED_LOCK */
#define PAGER_EXCLUSIVE   4   /* same as EXCLUSIVE_LOCK */
#define PAGER_SYNCED      5

/*
** If the SQLITE_BUSY_RESERVED_LOCK macro is set to true at compile-time,
** then failed attempts to get a reserved lock will invoke the busy callback.
** This is off by default.  To see why, consider the following scenario:
** 
** Suppose thread A already has a shared lock and wants a reserved lock.
** Thread B already has a reserved lock and wants an exclusive lock.  If
** both threads are using their busy callbacks, it might be a long time
** be for one of the threads give up and allows the other to proceed.
** But if the thread trying to get the reserved lock gives up quickly
** (if it never invokes its busy callback) then the contention will be
** resolved quickly.
*/
#ifndef SQLITE_BUSY_RESERVED_LOCK
# define SQLITE_BUSY_RESERVED_LOCK 0
#endif

/*
** This macro rounds values up so that if the value is an address it
** is guaranteed to be an address that is aligned to an 8-byte boundary.
*/
#define FORCE_ALIGNMENT(X)   (((X)+7)&~7)

typedef struct PgHdr PgHdr;

/*
** Each pager stores all currently unreferenced pages in a list sorted
** in least-recently-used (LRU) order (i.e. the first item on the list has 
** not been referenced in a long time, the last item has been recently
** used). An instance of this structure is included as part of each
** pager structure for this purpose (variable Pager.lru).
**
** Additionally, if memory-management is enabled, all unreferenced pages 
** are stored in a global LRU list (global variable sqlite3LruPageList).
**
** In both cases, the PagerLruList.pFirstSynced variable points to
** the first page in the corresponding list that does not require an
** fsync() operation before its memory can be reclaimed. If no such
** page exists, PagerLruList.pFirstSynced is set to NULL.
*/
typedef struct PagerLruList PagerLruList;
struct PagerLruList {
  PgHdr *pFirst;         /* First page in LRU list */
  PgHdr *pLast;          /* Last page in LRU list (the most recently used) */
  PgHdr *pFirstSynced;   /* First page in list with PgHdr.needSync==0 */
};

/*
** The following structure contains the next and previous pointers used
** to link a PgHdr structure into a PagerLruList linked list. 
*/
typedef struct PagerLruLink PagerLruLink;
struct PagerLruLink {
  PgHdr *pNext;
  PgHdr *pPrev;
};

/*
** Each in-memory image of a page begins with the following header.
** This header is only visible to this pager module.  The client
** code that calls pager sees only the data that follows the header.
**
** Client code should call sqlite3PagerWrite() on a page prior to making
** any modifications to that page.  The first time sqlite3PagerWrite()
** is called, the original page contents are written into the rollback
** journal and PgHdr.inJournal and PgHdr.needSync are set.  Later, once
** the journal page has made it onto the disk surface, PgHdr.needSync
** is cleared.  The modified page cannot be written back into the original
** database file until the journal pages has been synced to disk and the
** PgHdr.needSync has been cleared.
**
** The PgHdr.dirty flag is set when sqlite3PagerWrite() is called and
** is cleared again when the page content is written back to the original
** database file.
**
** Details of important structure elements:
**
** needSync
**
**     If this is true, this means that it is not safe to write the page
**     content to the database because the original content needed
**     for rollback has not by synced to the main rollback journal.
**     The original content may have been written to the rollback journal
**     but it has not yet been synced.  So we cannot write to the database
**     file because power failure might cause the page in the journal file
**     to never reach the disk.  It is as if the write to the journal file
**     does not occur until the journal file is synced.
**     
**     This flag is false if the page content exactly matches what
**     currently exists in the database file.  The needSync flag is also
**     false if the original content has been written to the main rollback
**     journal and synced.  If the page represents a new page that has
**     been added onto the end of the database during the current
**     transaction, the needSync flag is true until the original database
**     size in the journal header has been synced to disk.
**
** inJournal
**
**     This is true if the original page has been written into the main
**     rollback journal.  This is always false for new pages added to
**     the end of the database file during the current transaction.
**     And this flag says nothing about whether or not the journal
**     has been synced to disk.  For pages that are in the original
**     database file, the following expression should always be true:
**
**       inJournal = (pPager->aInJournal[(pgno-1)/8] & (1<<((pgno-1)%8))!=0
**
**     The pPager->aInJournal[] array is only valid for the original
**     pages of the database, not new pages that are added to the end
**     of the database, so obviously the above expression cannot be
**     valid for new pages.  For new pages inJournal is always 0.
**
** dirty
**
**     When true, this means that the content of the page has been
**     modified and needs to be written back to the database file.
**     If false, it means that either the content of the page is
**     unchanged or else the content is unimportant and we do not
**     care whether or not it is preserved.
**
** alwaysRollback
**
**     This means that the sqlite3PagerDontRollback() API should be
**     ignored for this page.  The DontRollback() API attempts to say
**     that the content of the page on disk is unimportant (it is an
**     unused page on the freelist) so that it is unnecessary to 
**     rollback changes to this page because the content of the page
**     can change without changing the meaning of the database.  This
**     flag overrides any DontRollback() attempt.  This flag is set
**     when a page that originally contained valid data is added to
**     the freelist.  Later in the same transaction, this page might
**     be pulled from the freelist and reused for something different
**     and at that point the DontRollback() API will be called because
**     pages taken from the freelist do not need to be protected by
**     the rollback journal.  But this flag says that the page was
**     not originally part of the freelist so that it still needs to
**     be rolled back in spite of any subsequent DontRollback() calls.
**
** needRead 
**
**     This flag means (when true) that the content of the page has
**     not yet been loaded from disk.  The in-memory content is just
**     garbage.  (Actually, we zero the content, but you should not
**     make any assumptions about the content nevertheless.)  If the
**     content is needed in the future, it should be read from the
**     original database file.
*/
struct PgHdr {
  Pager *pPager;                 /* The pager to which this page belongs */
  Pgno pgno;                     /* The page number for this page */
  PgHdr *pNextHash, *pPrevHash;  /* Hash collision chain for PgHdr.pgno */
  PagerLruLink free;             /* Next and previous free pages */
  PgHdr *pNextAll;               /* A list of all pages */
  u8 inJournal;                  /* TRUE if has been written to journal */
  u8 dirty;                      /* TRUE if we need to write back changes */
  u8 needSync;                   /* Sync journal before writing this page */
  u8 alwaysRollback;             /* Disable DontRollback() for this page */
  u8 needRead;                   /* Read content if PagerWrite() is called */
  short int nRef;                /* Number of users of this page */
  PgHdr *pDirty, *pPrevDirty;    /* Dirty pages */
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  PagerLruLink gfree;            /* Global list of nRef==0 pages */
#endif
#ifdef SQLITE_CHECK_PAGES
  u32 pageHash;
#endif
  void *pData;                   /* Page data */
  /* Pager.nExtra bytes of local data appended to this header */
};

/*
** For an in-memory only database, some extra information is recorded about
** each page so that changes can be rolled back.  (Journal files are not
** used for in-memory databases.)  The following information is added to
** the end of every EXTRA block for in-memory databases.
**
** This information could have been added directly to the PgHdr structure.
** But then it would take up an extra 8 bytes of storage on every PgHdr
** even for disk-based databases.  Splitting it out saves 8 bytes.  This
** is only a savings of 0.8% but those percentages add up.
*/
typedef struct PgHistory PgHistory;
struct PgHistory {
  u8 *pOrig;     /* Original page text.  Restore to this on a full rollback */
  u8 *pStmt;     /* Text as it was at the beginning of the current statement */
  PgHdr *pNextStmt, *pPrevStmt;  /* List of pages in the statement journal */
  u8 inStmt;                     /* TRUE if in the statement subjournal */
};

/*
** A macro used for invoking the codec if there is one
*/
#ifdef SQLITE_HAS_CODEC
# define CODEC1(P,D,N,X) if( P->xCodec!=0 ){ P->xCodec(P->pCodecArg,D,N,X); }
# define CODEC2(P,D,N,X) ((char*)(P->xCodec!=0?P->xCodec(P->pCodecArg,D,N,X):D))
#else
# define CODEC1(P,D,N,X) /* NO-OP */
# define CODEC2(P,D,N,X) ((char*)D)
#endif

/*
** Convert a pointer to a PgHdr into a pointer to its data
** and back again.
*/
#define PGHDR_TO_DATA(P)    ((P)->pData)
#define PGHDR_TO_EXTRA(G,P) ((void*)&((G)[1]))
#define PGHDR_TO_HIST(P,PGR)  \
            ((PgHistory*)&((char*)(&(P)[1]))[(PGR)->nExtra])

/*
** A open page cache is an instance of the following structure.
**
** Pager.errCode may be set to SQLITE_IOERR, SQLITE_CORRUPT, or
** or SQLITE_FULL. Once one of the first three errors occurs, it persists
** and is returned as the result of every major pager API call.  The
** SQLITE_FULL return code is slightly different. It persists only until the
** next successful rollback is performed on the pager cache. Also,
** SQLITE_FULL does not affect the sqlite3PagerGet() and sqlite3PagerLookup()
** APIs, they may still be used successfully.
*/
struct Pager {
  sqlite3_vfs *pVfs;          /* OS functions to use for IO */
  u8 journalOpen;             /* True if journal file descriptors is valid */
  u8 journalStarted;          /* True if header of journal is synced */
  u8 useJournal;              /* Use a rollback journal on this file */
  u8 noReadlock;              /* Do not bother to obtain readlocks */
  u8 stmtOpen;                /* True if the statement subjournal is open */
  u8 stmtInUse;               /* True we are in a statement subtransaction */
  u8 stmtAutoopen;            /* Open stmt journal when main journal is opened*/
  u8 noSync;                  /* Do not sync the journal if true */
  u8 fullSync;                /* Do extra syncs of the journal for robustness */
  u8 sync_flags;              /* One of SYNC_NORMAL or SYNC_FULL */
  u8 state;                   /* PAGER_UNLOCK, _SHARED, _RESERVED, etc. */
  u8 tempFile;                /* zFilename is a temporary file */
  u8 readOnly;                /* True for a read-only database */
  u8 needSync;                /* True if an fsync() is needed on the journal */
  u8 dirtyCache;              /* True if cached pages have changed */
  u8 alwaysRollback;          /* Disable DontRollback() for all pages */
  u8 memDb;                   /* True to inhibit all file I/O */
  u8 setMaster;               /* True if a m-j name has been written to jrnl */
  u8 doNotSync;               /* Boolean. While true, do not spill the cache */
  u8 exclusiveMode;           /* Boolean. True if locking_mode==EXCLUSIVE */
  u8 changeCountDone;         /* Set after incrementing the change-counter */
  u32 vfsFlags;               /* Flags for sqlite3_vfs.xOpen() */
  int errCode;                /* One of several kinds of errors */
  int dbSize;                 /* Number of pages in the file */
  int origDbSize;             /* dbSize before the current change */
  int stmtSize;               /* Size of database (in pages) at stmt_begin() */
  int nRec;                   /* Number of pages written to the journal */
  u32 cksumInit;              /* Quasi-random value added to every checksum */
  int stmtNRec;               /* Number of records in stmt subjournal */
  int nExtra;                 /* Add this many bytes to each in-memory page */
  int pageSize;               /* Number of bytes in a page */
  int nPage;                  /* Total number of in-memory pages */
  int nRef;                   /* Number of in-memory pages with PgHdr.nRef>0 */
  int mxPage;                 /* Maximum number of pages to hold in cache */
  Pgno mxPgno;                /* Maximum allowed size of the database */
  u8 *aInJournal;             /* One bit for each page in the database file */
  u8 *aInStmt;                /* One bit for each page in the database */
  char *zFilename;            /* Name of the database file */
  char *zJournal;             /* Name of the journal file */
  char *zDirectory;           /* Directory hold database and journal files */
  char *zStmtJrnl;            /* Name of the statement journal file */
  sqlite3_file *fd, *jfd;     /* File descriptors for database and journal */
  sqlite3_file *stfd;         /* File descriptor for the statement subjournal*/
  BusyHandler *pBusyHandler;  /* Pointer to sqlite.busyHandler */
  PagerLruList lru;           /* LRU list of free pages */
  PgHdr *pAll;                /* List of all pages */
  PgHdr *pStmt;               /* List of pages in the statement subjournal */
  PgHdr *pDirty;              /* List of all dirty pages */
  i64 journalOff;             /* Current byte offset in the journal file */
  i64 journalHdr;             /* Byte offset to previous journal header */
  i64 stmtHdrOff;             /* First journal header written this statement */
  i64 stmtCksum;              /* cksumInit when statement was started */
  i64 stmtJSize;              /* Size of journal at stmt_begin() */
  int sectorSize;             /* Assumed sector size during rollback */
#ifdef SQLITE_TEST
  int nHit, nMiss;            /* Cache hits and missing */
  int nRead, nWrite;          /* Database pages read/written */
#endif
  void (*xDestructor)(DbPage*,int); /* Call this routine when freeing pages */
  void (*xReiniter)(DbPage*,int);   /* Call this routine when reloading pages */
#ifdef SQLITE_HAS_CODEC
  void *(*xCodec)(void*,void*,Pgno,int); /* Routine for en/decoding data */
  void *pCodecArg;            /* First argument to xCodec() */
#endif
  int nHash;                  /* Size of the pager hash table */
  PgHdr **aHash;              /* Hash table to map page number to PgHdr */
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  Pager *pNext;               /* Doubly linked list of pagers on which */
  Pager *pPrev;               /* sqlite3_release_memory() will work */
  int iInUseMM;               /* Non-zero if unavailable to MM */
  int iInUseDB;               /* Non-zero if in sqlite3_release_memory() */
#endif
  char *pTmpSpace;            /* Pager.pageSize bytes of space for tmp use */
  char dbFileVers[16];        /* Changes whenever database file changes */
};

/*
** The following global variables hold counters used for
** testing purposes only.  These variables do not exist in
** a non-testing build.  These variables are not thread-safe.
*/
#ifdef SQLITE_TEST
int sqlite3_pager_readdb_count = 0;    /* Number of full pages read from DB */
int sqlite3_pager_writedb_count = 0;   /* Number of full pages written to DB */
int sqlite3_pager_writej_count = 0;    /* Number of pages written to journal */
int sqlite3_pager_pgfree_count = 0;    /* Number of cache pages freed */
# define PAGER_INCR(v)  v++
#else
# define PAGER_INCR(v)
#endif

/*
** The following variable points to the head of a double-linked list
** of all pagers that are eligible for page stealing by the
** sqlite3_release_memory() interface.  Access to this list is
** protected by the SQLITE_MUTEX_STATIC_MEM2 mutex.
*/
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
static Pager *sqlite3PagerList = 0;
static PagerLruList sqlite3LruPageList = {0, 0, 0};
#endif


/*
** Journal files begin with the following magic string.  The data
** was obtained from /dev/random.  It is used only as a sanity check.
**
** Since version 2.8.0, the journal format contains additional sanity
** checking information.  If the power fails while the journal is begin
** written, semi-random garbage data might appear in the journal
** file after power is restored.  If an attempt is then made
** to roll the journal back, the database could be corrupted.  The additional
** sanity checking data is an attempt to discover the garbage in the
** journal and ignore it.
**
** The sanity checking information for the new journal format consists
** of a 32-bit checksum on each page of data.  The checksum covers both
** the page number and the pPager->pageSize bytes of data for the page.
** This cksum is initialized to a 32-bit random value that appears in the
** journal file right after the header.  The random initializer is important,
** because garbage data that appears at the end of a journal is likely
** data that was once in other files that have now been deleted.  If the
** garbage data came from an obsolete journal file, the checksums might
** be correct.  But by initializing the checksum to random value which
** is different for every journal, we minimize that risk.
*/
static const unsigned char aJournalMagic[] = {
  0xd9, 0xd5, 0x05, 0xf9, 0x20, 0xa1, 0x63, 0xd7,
};

/*
** The size of the header and of each page in the journal is determined
** by the following macros.
*/
#define JOURNAL_PG_SZ(pPager)  ((pPager->pageSize) + 8)

/*
** The journal header size for this pager. In the future, this could be
** set to some value read from the disk controller. The important
** characteristic is that it is the same size as a disk sector.
*/
#define JOURNAL_HDR_SZ(pPager) (pPager->sectorSize)

/*
** The macro MEMDB is true if we are dealing with an in-memory database.
** We do this as a macro so that if the SQLITE_OMIT_MEMORYDB macro is set,
** the value of MEMDB will be a constant and the compiler will optimize
** out code that would never execute.
*/
#ifdef SQLITE_OMIT_MEMORYDB
# define MEMDB 0
#else
# define MEMDB pPager->memDb
#endif

/*
** Page number PAGER_MJ_PGNO is never used in an SQLite database (it is
** reserved for working around a windows/posix incompatibility). It is
** used in the journal to signify that the remainder of the journal file 
** is devoted to storing a master journal name - there are no more pages to
** roll back. See comments for function writeMasterJournal() for details.
*/
/* #define PAGER_MJ_PGNO(x) (PENDING_BYTE/((x)->pageSize)) */
#define PAGER_MJ_PGNO(x) ((PENDING_BYTE/((x)->pageSize))+1)

/*
** The maximum legal page number is (2^31 - 1).
*/
#define PAGER_MAX_PGNO 2147483647

/*
** The pagerEnter() and pagerLeave() routines acquire and release
** a mutex on each pager.  The mutex is recursive.
**
** This is a special-purpose mutex.  It only provides mutual exclusion
** between the Btree and the Memory Management sqlite3_release_memory()
** function.  It does not prevent, for example, two Btrees from accessing
** the same pager at the same time.  Other general-purpose mutexes in
** the btree layer handle that chore.
*/
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  static void pagerEnter(Pager *p){
    p->iInUseDB++;
    if( p->iInUseMM && p->iInUseDB==1 ){
      sqlite3_mutex *mutex;
      mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MEM2);
      p->iInUseDB = 0;
      sqlite3_mutex_enter(mutex);
      p->iInUseDB = 1;
      sqlite3_mutex_leave(mutex);
    }
    assert( p->iInUseMM==0 );
  }
  static void pagerLeave(Pager *p){
    p->iInUseDB--;
    assert( p->iInUseDB>=0 );
  }
#else
# define pagerEnter(X)
# define pagerLeave(X)
#endif

/*
** Add page pPg to the end of the linked list managed by structure
** pList (pPg becomes the last entry in the list - the most recently 
** used). Argument pLink should point to either pPg->free or pPg->gfree,
** depending on whether pPg is being added to the pager-specific or
** global LRU list.
*/
static void listAdd(PagerLruList *pList, PagerLruLink *pLink, PgHdr *pPg){
  pLink->pNext = 0;
  pLink->pPrev = pList->pLast;

#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  assert(pLink==&pPg->free || pLink==&pPg->gfree);
  assert(pLink==&pPg->gfree || pList!=&sqlite3LruPageList);
#endif

  if( pList->pLast ){
    int iOff = (char *)pLink - (char *)pPg;
    PagerLruLink *pLastLink = (PagerLruLink *)(&((u8 *)pList->pLast)[iOff]);
    pLastLink->pNext = pPg;
  }else{
    assert(!pList->pFirst);
    pList->pFirst = pPg;
  }

  pList->pLast = pPg;
  if( !pList->pFirstSynced && pPg->needSync==0 ){
    pList->pFirstSynced = pPg;
  }
}

/*
** Remove pPg from the list managed by the structure pointed to by pList.
**
** Argument pLink should point to either pPg->free or pPg->gfree, depending 
** on whether pPg is being added to the pager-specific or global LRU list.
*/
static void listRemove(PagerLruList *pList, PagerLruLink *pLink, PgHdr *pPg){
  int iOff = (char *)pLink - (char *)pPg;

#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  assert(pLink==&pPg->free || pLink==&pPg->gfree);
  assert(pLink==&pPg->gfree || pList!=&sqlite3LruPageList);
#endif

  if( pPg==pList->pFirst ){
    pList->pFirst = pLink->pNext;
  }
  if( pPg==pList->pLast ){
    pList->pLast = pLink->pPrev;
  }
  if( pLink->pPrev ){
    PagerLruLink *pPrevLink = (PagerLruLink *)(&((u8 *)pLink->pPrev)[iOff]);
    pPrevLink->pNext = pLink->pNext;
  }
  if( pLink->pNext ){
    PagerLruLink *pNextLink = (PagerLruLink *)(&((u8 *)pLink->pNext)[iOff]);
    pNextLink->pPrev = pLink->pPrev;
  }
  if( pPg==pList->pFirstSynced ){
    PgHdr *p = pLink->pNext;
    while( p && p->needSync ){
      PagerLruLink *pL = (PagerLruLink *)(&((u8 *)p)[iOff]);
      p = pL->pNext;
    }
    pList->pFirstSynced = p;
  }

  pLink->pNext = pLink->pPrev = 0;
}

/* 
** Add page pPg to the list of free pages for the pager. If 
** memory-management is enabled, also add the page to the global 
** list of free pages.
*/
static void lruListAdd(PgHdr *pPg){
  listAdd(&pPg->pPager->lru, &pPg->free, pPg);
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  if( !pPg->pPager->memDb ){
    sqlite3_mutex_enter(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
    listAdd(&sqlite3LruPageList, &pPg->gfree, pPg);
    sqlite3_mutex_leave(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
  }
#endif
}

/* 
** Remove page pPg from the list of free pages for the associated pager.
** If memory-management is enabled, also remove pPg from the global list
** of free pages.
*/
static void lruListRemove(PgHdr *pPg){
  listRemove(&pPg->pPager->lru, &pPg->free, pPg);
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  if( !pPg->pPager->memDb ){
    sqlite3_mutex_enter(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
    listRemove(&sqlite3LruPageList, &pPg->gfree, pPg);
    sqlite3_mutex_leave(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
  }
#endif
}

/* 
** This function is called just after the needSync flag has been cleared
** from all pages managed by pPager (usually because the journal file
** has just been synced). It updates the pPager->lru.pFirstSynced variable
** and, if memory-management is enabled, the sqlite3LruPageList.pFirstSynced
** variable also.
*/
static void lruListSetFirstSynced(Pager *pPager){
  pPager->lru.pFirstSynced = pPager->lru.pFirst;
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  if( !pPager->memDb ){
    PgHdr *p;
    sqlite3_mutex_enter(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
    for(p=sqlite3LruPageList.pFirst; p && p->needSync; p=p->gfree.pNext);
    assert(p==pPager->lru.pFirstSynced || p==sqlite3LruPageList.pFirstSynced);
    sqlite3LruPageList.pFirstSynced = p;
    sqlite3_mutex_leave(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
  }
#endif
}

/*
** Return true if page *pPg has already been written to the statement
** journal (or statement snapshot has been created, if *pPg is part
** of an in-memory database).
*/
static int pageInStatement(PgHdr *pPg){
  Pager *pPager = pPg->pPager;
  if( MEMDB ){
    return PGHDR_TO_HIST(pPg, pPager)->inStmt;
  }else{
    Pgno pgno = pPg->pgno;
    u8 *a = pPager->aInStmt;
    return (a && (int)pgno<=pPager->stmtSize && (a[pgno/8] & (1<<(pgno&7))));
  }
}

/*
** Change the size of the pager hash table to N.  N must be a power
** of two.
*/
static void pager_resize_hash_table(Pager *pPager, int N){
  PgHdr **aHash, *pPg;
  assert( N>0 && (N&(N-1))==0 );
  pagerLeave(pPager);
  sqlite3FaultBenign(SQLITE_FAULTINJECTOR_MALLOC, pPager->aHash!=0);
  aHash = sqlite3MallocZero( sizeof(aHash[0])*N );
  sqlite3FaultBenign(SQLITE_FAULTINJECTOR_MALLOC, 0);
  pagerEnter(pPager);
  if( aHash==0 ){
    /* Failure to rehash is not an error.  It is only a performance hit. */
    return;
  }
  sqlite3_free(pPager->aHash);
  pPager->nHash = N;
  pPager->aHash = aHash;
  for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
    int h;
    if( pPg->pgno==0 ){
      assert( pPg->pNextHash==0 && pPg->pPrevHash==0 );
      continue;
    }
    h = pPg->pgno & (N-1);
    pPg->pNextHash = aHash[h];
    if( aHash[h] ){
      aHash[h]->pPrevHash = pPg;
    }
    aHash[h] = pPg;
    pPg->pPrevHash = 0;
  }
}

/*
** Read a 32-bit integer from the given file descriptor.  Store the integer
** that is read in *pRes.  Return SQLITE_OK if everything worked, or an
** error code is something goes wrong.
**
** All values are stored on disk as big-endian.
*/
static int read32bits(sqlite3_file *fd, i64 offset, u32 *pRes){
  unsigned char ac[4];
  int rc = sqlite3OsRead(fd, ac, sizeof(ac), offset);
  if( rc==SQLITE_OK ){
    *pRes = sqlite3Get4byte(ac);
  }
  return rc;
}

/*
** Write a 32-bit integer into a string buffer in big-endian byte order.
*/
#define put32bits(A,B)  sqlite3Put4byte((u8*)A,B)

/*
** Write a 32-bit integer into the given file descriptor.  Return SQLITE_OK
** on success or an error code is something goes wrong.
*/
static int write32bits(sqlite3_file *fd, i64 offset, u32 val){
  char ac[4];
  put32bits(ac, val);
  return sqlite3OsWrite(fd, ac, 4, offset);
}

/*
** If file pFd is open, call sqlite3OsUnlock() on it.
*/
static int osUnlock(sqlite3_file *pFd, int eLock){
  if( !pFd->pMethods ){
    return SQLITE_OK;
  }
  return sqlite3OsUnlock(pFd, eLock);
}

/*
** This function determines whether or not the atomic-write optimization
** can be used with this pager. The optimization can be used if:
**
**  (a) the value returned by OsDeviceCharacteristics() indicates that
**      a database page may be written atomically, and
**  (b) the value returned by OsSectorSize() is less than or equal
**      to the page size.
**
** If the optimization cannot be used, 0 is returned. If it can be used,
** then the value returned is the size of the journal file when it
** contains rollback data for exactly one page.
*/
#ifdef SQLITE_ENABLE_ATOMIC_WRITE
static int jrnlBufferSize(Pager *pPager){
  int dc;           /* Device characteristics */
  int nSector;      /* Sector size */
  int nPage;        /* Page size */
  sqlite3_file *fd = pPager->fd;

  if( fd->pMethods ){
    dc = sqlite3OsDeviceCharacteristics(fd);
    nSector = sqlite3OsSectorSize(fd);
    nPage = pPager->pageSize;
  }

  assert(SQLITE_IOCAP_ATOMIC512==(512>>8));
  assert(SQLITE_IOCAP_ATOMIC64K==(65536>>8));

  if( !fd->pMethods || (dc&(SQLITE_IOCAP_ATOMIC|(nPage>>8))&&nSector<=nPage) ){
    return JOURNAL_HDR_SZ(pPager) + JOURNAL_PG_SZ(pPager);
  }
  return 0;
}
#endif

/*
** This function should be called when an error occurs within the pager
** code. The first argument is a pointer to the pager structure, the
** second the error-code about to be returned by a pager API function. 
** The value returned is a copy of the second argument to this function. 
**
** If the second argument is SQLITE_IOERR, SQLITE_CORRUPT, or SQLITE_FULL
** the error becomes persistent. Until the persisten error is cleared,
** subsequent API calls on this Pager will immediately return the same 
** error code.
**
** A persistent error indicates that the contents of the pager-cache 
** cannot be trusted. This state can be cleared by completely discarding 
** the contents of the pager-cache. If a transaction was active when
** the persistent error occured, then the rollback journal may need
** to be replayed.
*/
static void pager_unlock(Pager *pPager);
static int pager_error(Pager *pPager, int rc){
  int rc2 = rc & 0xff;
  assert(
       pPager->errCode==SQLITE_FULL ||
       pPager->errCode==SQLITE_OK ||
       (pPager->errCode & 0xff)==SQLITE_IOERR
  );
  if(
    rc2==SQLITE_FULL ||
    rc2==SQLITE_IOERR ||
    rc2==SQLITE_CORRUPT
  ){
    pPager->errCode = rc;
    if( pPager->state==PAGER_UNLOCK && pPager->nRef==0 ){
      /* If the pager is already unlocked, call pager_unlock() now to
      ** clear the error state and ensure that the pager-cache is 
      ** completely empty.
      */
      pager_unlock(pPager);
    }
  }
  return rc;
}

/*
** If SQLITE_CHECK_PAGES is defined then we do some sanity checking
** on the cache using a hash function.  This is used for testing
** and debugging only.
*/
#ifdef SQLITE_CHECK_PAGES
/*
** Return a 32-bit hash of the page data for pPage.
*/
static u32 pager_datahash(int nByte, unsigned char *pData){
  u32 hash = 0;
  int i;
  for(i=0; i<nByte; i++){
    hash = (hash*1039) + pData[i];
  }
  return hash;
}
static u32 pager_pagehash(PgHdr *pPage){
  return pager_datahash(pPage->pPager->pageSize, 
                        (unsigned char *)PGHDR_TO_DATA(pPage));
}

/*
** The CHECK_PAGE macro takes a PgHdr* as an argument. If SQLITE_CHECK_PAGES
** is defined, and NDEBUG is not defined, an assert() statement checks
** that the page is either dirty or still matches the calculated page-hash.
*/
#define CHECK_PAGE(x) checkPage(x)
static void checkPage(PgHdr *pPg){
  Pager *pPager = pPg->pPager;
  assert( !pPg->pageHash || pPager->errCode || MEMDB || pPg->dirty || 
      pPg->pageHash==pager_pagehash(pPg) );
}

#else
#define pager_datahash(X,Y)  0
#define pager_pagehash(X)  0
#define CHECK_PAGE(x)
#endif

/*
** When this is called the journal file for pager pPager must be open.
** The master journal file name is read from the end of the file and 
** written into memory supplied by the caller. 
**
** zMaster must point to a buffer of at least nMaster bytes allocated by
** the caller. This should be sqlite3_vfs.mxPathname+1 (to ensure there is
** enough space to write the master journal name). If the master journal
** name in the journal is longer than nMaster bytes (including a
** nul-terminator), then this is handled as if no master journal name
** were present in the journal.
**
** If no master journal file name is present zMaster[0] is set to 0 and
** SQLITE_OK returned.
*/
static int readMasterJournal(sqlite3_file *pJrnl, char *zMaster, int nMaster){
  int rc;
  u32 len;
  i64 szJ;
  u32 cksum;
  int i;
  unsigned char aMagic[8]; /* A buffer to hold the magic header */

  zMaster[0] = '\0';

  rc = sqlite3OsFileSize(pJrnl, &szJ);
  if( rc!=SQLITE_OK || szJ<16 ) return rc;

  rc = read32bits(pJrnl, szJ-16, &len);
  if( rc!=SQLITE_OK ) return rc;

  if( len>=nMaster ){
    return SQLITE_OK;
  }

  rc = read32bits(pJrnl, szJ-12, &cksum);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3OsRead(pJrnl, aMagic, 8, szJ-8);
  if( rc!=SQLITE_OK || memcmp(aMagic, aJournalMagic, 8) ) return rc;

  rc = sqlite3OsRead(pJrnl, zMaster, len, szJ-16-len);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  zMaster[len] = '\0';

  /* See if the checksum matches the master journal name */
  for(i=0; i<len; i++){
    cksum -= zMaster[i];
   }
  if( cksum ){
    /* If the checksum doesn't add up, then one or more of the disk sectors
    ** containing the master journal filename is corrupted. This means
    ** definitely roll back, so just return SQLITE_OK and report a (nul)
    ** master-journal filename.
    */
    zMaster[0] = '\0';
  }
   
  return SQLITE_OK;
}

/*
** Seek the journal file descriptor to the next sector boundary where a
** journal header may be read or written. Pager.journalOff is updated with
** the new seek offset.
**
** i.e for a sector size of 512:
**
** Input Offset              Output Offset
** ---------------------------------------
** 0                         0
** 512                       512
** 100                       512
** 2000                      2048
** 
*/
static void seekJournalHdr(Pager *pPager){
  i64 offset = 0;
  i64 c = pPager->journalOff;
  if( c ){
    offset = ((c-1)/JOURNAL_HDR_SZ(pPager) + 1) * JOURNAL_HDR_SZ(pPager);
  }
  assert( offset%JOURNAL_HDR_SZ(pPager)==0 );
  assert( offset>=c );
  assert( (offset-c)<JOURNAL_HDR_SZ(pPager) );
  pPager->journalOff = offset;
}

/*
** The journal file must be open when this routine is called. A journal
** header (JOURNAL_HDR_SZ bytes) is written into the journal file at the
** current location.
**
** The format for the journal header is as follows:
** - 8 bytes: Magic identifying journal format.
** - 4 bytes: Number of records in journal, or -1 no-sync mode is on.
** - 4 bytes: Random number used for page hash.
** - 4 bytes: Initial database page count.
** - 4 bytes: Sector size used by the process that wrote this journal.
** 
** Followed by (JOURNAL_HDR_SZ - 24) bytes of unused space.
*/
static int writeJournalHdr(Pager *pPager){
  char zHeader[sizeof(aJournalMagic)+16];
  int rc;

  if( pPager->stmtHdrOff==0 ){
    pPager->stmtHdrOff = pPager->journalOff;
  }

  seekJournalHdr(pPager);
  pPager->journalHdr = pPager->journalOff;

  memcpy(zHeader, aJournalMagic, sizeof(aJournalMagic));

  /* 
  ** Write the nRec Field - the number of page records that follow this
  ** journal header. Normally, zero is written to this value at this time.
  ** After the records are added to the journal (and the journal synced, 
  ** if in full-sync mode), the zero is overwritten with the true number
  ** of records (see syncJournal()).
  **
  ** A faster alternative is to write 0xFFFFFFFF to the nRec field. When
  ** reading the journal this value tells SQLite to assume that the
  ** rest of the journal file contains valid page records. This assumption
  ** is dangerous, as if a failure occured whilst writing to the journal
  ** file it may contain some garbage data. There are two scenarios
  ** where this risk can be ignored:
  **
  **   * When the pager is in no-sync mode. Corruption can follow a
  **     power failure in this case anyway.
  **
  **   * When the SQLITE_IOCAP_SAFE_APPEND flag is set. This guarantees
  **     that garbage data is never appended to the journal file.
  */
  assert(pPager->fd->pMethods||pPager->noSync);
  if( (pPager->noSync) 
   || (sqlite3OsDeviceCharacteristics(pPager->fd)&SQLITE_IOCAP_SAFE_APPEND) 
  ){
    put32bits(&zHeader[sizeof(aJournalMagic)], 0xffffffff);
  }else{
    put32bits(&zHeader[sizeof(aJournalMagic)], 0);
  }

  /* The random check-hash initialiser */ 
  sqlite3Randomness(sizeof(pPager->cksumInit), &pPager->cksumInit);
  put32bits(&zHeader[sizeof(aJournalMagic)+4], pPager->cksumInit);
  /* The initial database size */
  put32bits(&zHeader[sizeof(aJournalMagic)+8], pPager->dbSize);
  /* The assumed sector size for this process */
  put32bits(&zHeader[sizeof(aJournalMagic)+12], pPager->sectorSize);
  IOTRACE(("JHDR %p %lld %d\n", pPager, pPager->journalHdr, sizeof(zHeader)))
  rc = sqlite3OsWrite(pPager->jfd, zHeader, sizeof(zHeader),pPager->journalOff);
  pPager->journalOff += JOURNAL_HDR_SZ(pPager);

  /* The journal header has been written successfully. Seek the journal
  ** file descriptor to the end of the journal header sector.
  */
  if( rc==SQLITE_OK ){
    IOTRACE(("JTAIL %p %lld\n", pPager, pPager->journalOff-1))
    rc = sqlite3OsWrite(pPager->jfd, "\000", 1, pPager->journalOff-1);
  }
  return rc;
}

/*
** The journal file must be open when this is called. A journal header file
** (JOURNAL_HDR_SZ bytes) is read from the current location in the journal
** file. See comments above function writeJournalHdr() for a description of
** the journal header format.
**
** If the header is read successfully, *nRec is set to the number of
** page records following this header and *dbSize is set to the size of the
** database before the transaction began, in pages. Also, pPager->cksumInit
** is set to the value read from the journal header. SQLITE_OK is returned
** in this case.
**
** If the journal header file appears to be corrupted, SQLITE_DONE is
** returned and *nRec and *dbSize are not set.  If JOURNAL_HDR_SZ bytes
** cannot be read from the journal file an error code is returned.
*/
static int readJournalHdr(
  Pager *pPager, 
  i64 journalSize,
  u32 *pNRec, 
  u32 *pDbSize
){
  int rc;
  unsigned char aMagic[8]; /* A buffer to hold the magic header */
  i64 jrnlOff;

  seekJournalHdr(pPager);
  if( pPager->journalOff+JOURNAL_HDR_SZ(pPager) > journalSize ){
    return SQLITE_DONE;
  }
  jrnlOff = pPager->journalOff;

  rc = sqlite3OsRead(pPager->jfd, aMagic, sizeof(aMagic), jrnlOff);
  if( rc ) return rc;
  jrnlOff += sizeof(aMagic);

  if( memcmp(aMagic, aJournalMagic, sizeof(aMagic))!=0 ){
    return SQLITE_DONE;
  }

  rc = read32bits(pPager->jfd, jrnlOff, pNRec);
  if( rc ) return rc;

  rc = read32bits(pPager->jfd, jrnlOff+4, &pPager->cksumInit);
  if( rc ) return rc;

  rc = read32bits(pPager->jfd, jrnlOff+8, pDbSize);
  if( rc ) return rc;

  /* Update the assumed sector-size to match the value used by 
  ** the process that created this journal. If this journal was
  ** created by a process other than this one, then this routine
  ** is being called from within pager_playback(). The local value
  ** of Pager.sectorSize is restored at the end of that routine.
  */
  rc = read32bits(pPager->jfd, jrnlOff+12, (u32 *)&pPager->sectorSize);
  if( rc ) return rc;

  pPager->journalOff += JOURNAL_HDR_SZ(pPager);
  return SQLITE_OK;
}


/*
** Write the supplied master journal name into the journal file for pager
** pPager at the current location. The master journal name must be the last
** thing written to a journal file. If the pager is in full-sync mode, the
** journal file descriptor is advanced to the next sector boundary before
** anything is written. The format is:
**
** + 4 bytes: PAGER_MJ_PGNO.
** + N bytes: length of master journal name.
** + 4 bytes: N
** + 4 bytes: Master journal name checksum.
** + 8 bytes: aJournalMagic[].
**
** The master journal page checksum is the sum of the bytes in the master
** journal name.
**
** If zMaster is a NULL pointer (occurs for a single database transaction), 
** this call is a no-op.
*/
static int writeMasterJournal(Pager *pPager, const char *zMaster){
  int rc;
  int len; 
  int i; 
  i64 jrnlOff;
  u32 cksum = 0;
  char zBuf[sizeof(aJournalMagic)+2*4];

  if( !zMaster || pPager->setMaster) return SQLITE_OK;
  pPager->setMaster = 1;

  len = strlen(zMaster);
  for(i=0; i<len; i++){
    cksum += zMaster[i];
  }

  /* If in full-sync mode, advance to the next disk sector before writing
  ** the master journal name. This is in case the previous page written to
  ** the journal has already been synced.
  */
  if( pPager->fullSync ){
    seekJournalHdr(pPager);
  }
  jrnlOff = pPager->journalOff;
  pPager->journalOff += (len+20);

  rc = write32bits(pPager->jfd, jrnlOff, PAGER_MJ_PGNO(pPager));
  if( rc!=SQLITE_OK ) return rc;
  jrnlOff += 4;

  rc = sqlite3OsWrite(pPager->jfd, zMaster, len, jrnlOff);
  if( rc!=SQLITE_OK ) return rc;
  jrnlOff += len;

  put32bits(zBuf, len);
  put32bits(&zBuf[4], cksum);
  memcpy(&zBuf[8], aJournalMagic, sizeof(aJournalMagic));
  rc = sqlite3OsWrite(pPager->jfd, zBuf, 8+sizeof(aJournalMagic), jrnlOff);
  pPager->needSync = !pPager->noSync;
  return rc;
}

/*
** Add or remove a page from the list of all pages that are in the
** statement journal.
**
** The Pager keeps a separate list of pages that are currently in
** the statement journal.  This helps the sqlite3PagerStmtCommit()
** routine run MUCH faster for the common case where there are many
** pages in memory but only a few are in the statement journal.
*/
static void page_add_to_stmt_list(PgHdr *pPg){
  Pager *pPager = pPg->pPager;
  PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
  assert( MEMDB );
  if( !pHist->inStmt ){
    assert( pHist->pPrevStmt==0 && pHist->pNextStmt==0 );
    if( pPager->pStmt ){
      PGHDR_TO_HIST(pPager->pStmt, pPager)->pPrevStmt = pPg;
    }
    pHist->pNextStmt = pPager->pStmt;
    pPager->pStmt = pPg;
    pHist->inStmt = 1;
  }
}

/*
** Find a page in the hash table given its page number.  Return
** a pointer to the page or NULL if not found.
*/
static PgHdr *pager_lookup(Pager *pPager, Pgno pgno){
  PgHdr *p;
  if( pPager->aHash==0 ) return 0;
  p = pPager->aHash[pgno & (pPager->nHash-1)];
  while( p && p->pgno!=pgno ){
    p = p->pNextHash;
  }
  return p;
}

/*
** Clear the in-memory cache.  This routine
** sets the state of the pager back to what it was when it was first
** opened.  Any outstanding pages are invalidated and subsequent attempts
** to access those pages will likely result in a coredump.
*/
static void pager_reset(Pager *pPager){
  PgHdr *pPg, *pNext;
  if( pPager->errCode ) return;
  for(pPg=pPager->pAll; pPg; pPg=pNext){
    IOTRACE(("PGFREE %p %d\n", pPager, pPg->pgno));
    PAGER_INCR(sqlite3_pager_pgfree_count);
    pNext = pPg->pNextAll;
    lruListRemove(pPg);
    sqlite3_free(pPg);
  }
  assert(pPager->lru.pFirst==0);
  assert(pPager->lru.pFirstSynced==0);
  assert(pPager->lru.pLast==0);
  pPager->pStmt = 0;
  pPager->pAll = 0;
  pPager->pDirty = 0;
  pPager->nHash = 0;
  sqlite3_free(pPager->aHash);
  pPager->nPage = 0;
  pPager->aHash = 0;
  pPager->nRef = 0;
}

/*
** Unlock the database file. 
**
** If the pager is currently in error state, discard the contents of 
** the cache and reset the Pager structure internal state. If there is
** an open journal-file, then the next time a shared-lock is obtained
** on the pager file (by this or any other process), it will be
** treated as a hot-journal and rolled back.
*/
static void pager_unlock(Pager *pPager){
  if( !pPager->exclusiveMode ){
    if( !MEMDB ){
      if( pPager->fd->pMethods ){
        osUnlock(pPager->fd, NO_LOCK);
      }
      pPager->dbSize = -1;
      IOTRACE(("UNLOCK %p\n", pPager))

      /* If Pager.errCode is set, the contents of the pager cache cannot be
      ** trusted. Now that the pager file is unlocked, the contents of the
      ** cache can be discarded and the error code safely cleared.
      */
      if( pPager->errCode ){
        pPager->errCode = SQLITE_OK;
        pager_reset(pPager);
        if( pPager->stmtOpen ){
          sqlite3OsClose(pPager->stfd);
          sqlite3_free(pPager->aInStmt);
          pPager->aInStmt = 0;
        }
        if( pPager->journalOpen ){
          sqlite3OsClose(pPager->jfd);
          pPager->journalOpen = 0;
          sqlite3_free(pPager->aInJournal);
          pPager->aInJournal = 0;
        }
        pPager->stmtOpen = 0;
        pPager->stmtInUse = 0;
        pPager->journalOff = 0;
        pPager->journalStarted = 0;
        pPager->stmtAutoopen = 0;
        pPager->origDbSize = 0;
      }
    }

    if( !MEMDB || pPager->errCode==SQLITE_OK ){
      pPager->state = PAGER_UNLOCK;
      pPager->changeCountDone = 0;
    }
  }
}

/*
** Execute a rollback if a transaction is active and unlock the 
** database file. If the pager has already entered the error state, 
** do not attempt the rollback.
*/
static void pagerUnlockAndRollback(Pager *p){
  assert( p->state>=PAGER_RESERVED || p->journalOpen==0 );
  if( p->errCode==SQLITE_OK && p->state>=PAGER_RESERVED ){
    sqlite3PagerRollback(p);
  }
  pager_unlock(p);
  assert( p->errCode || !p->journalOpen || (p->exclusiveMode&&!p->journalOff) );
  assert( p->errCode || !p->stmtOpen || p->exclusiveMode );
}

/*
** This routine ends a transaction.  A transaction is ended by either
** a COMMIT or a ROLLBACK.
**
** When this routine is called, the pager has the journal file open and
** a RESERVED or EXCLUSIVE lock on the database.  This routine will release
** the database lock and acquires a SHARED lock in its place if that is
** the appropriate thing to do.  Release locks usually is appropriate,
** unless we are in exclusive access mode or unless this is a 
** COMMIT AND BEGIN or ROLLBACK AND BEGIN operation.
**
** The journal file is either deleted or truncated.
**
** TODO: Consider keeping the journal file open for temporary databases.
** This might give a performance improvement on windows where opening
** a file is an expensive operation.
*/
static int pager_end_transaction(Pager *pPager){
  PgHdr *pPg;
  int rc = SQLITE_OK;
  int rc2 = SQLITE_OK;
  assert( !MEMDB );
  if( pPager->state<PAGER_RESERVED ){
    return SQLITE_OK;
  }
  sqlite3PagerStmtCommit(pPager);
  if( pPager->stmtOpen && !pPager->exclusiveMode ){
    sqlite3OsClose(pPager->stfd);
    pPager->stmtOpen = 0;
  }
  if( pPager->journalOpen ){
    if( pPager->exclusiveMode 
          && (rc = sqlite3OsTruncate(pPager->jfd, 0))==SQLITE_OK ){;
      pPager->journalOff = 0;
      pPager->journalStarted = 0;
    }else{
      sqlite3OsClose(pPager->jfd);
      pPager->journalOpen = 0;
      if( rc==SQLITE_OK ){
        rc = sqlite3OsDelete(pPager->pVfs, pPager->zJournal, 0);
      }
    }
    sqlite3_free( pPager->aInJournal );
    pPager->aInJournal = 0;
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      pPg->inJournal = 0;
      pPg->dirty = 0;
      pPg->needSync = 0;
      pPg->alwaysRollback = 0;
#ifdef SQLITE_CHECK_PAGES
      pPg->pageHash = pager_pagehash(pPg);
#endif
    }
    pPager->pDirty = 0;
    pPager->dirtyCache = 0;
    pPager->nRec = 0;
  }else{
    assert( pPager->aInJournal==0 );
    assert( pPager->dirtyCache==0 || pPager->useJournal==0 );
  }

  if( !pPager->exclusiveMode ){
    rc2 = osUnlock(pPager->fd, SHARED_LOCK);
    pPager->state = PAGER_SHARED;
  }else if( pPager->state==PAGER_SYNCED ){
    pPager->state = PAGER_EXCLUSIVE;
  }
  pPager->origDbSize = 0;
  pPager->setMaster = 0;
  pPager->needSync = 0;
  lruListSetFirstSynced(pPager);
  pPager->dbSize = -1;

  return (rc==SQLITE_OK?rc2:rc);
}

/*
** Compute and return a checksum for the page of data.
**
** This is not a real checksum.  It is really just the sum of the 
** random initial value and the page number.  We experimented with
** a checksum of the entire data, but that was found to be too slow.
**
** Note that the page number is stored at the beginning of data and
** the checksum is stored at the end.  This is important.  If journal
** corruption occurs due to a power failure, the most likely scenario
** is that one end or the other of the record will be changed.  It is
** much less likely that the two ends of the journal record will be
** correct and the middle be corrupt.  Thus, this "checksum" scheme,
** though fast and simple, catches the mostly likely kind of corruption.
**
** FIX ME:  Consider adding every 200th (or so) byte of the data to the
** checksum.  That way if a single page spans 3 or more disk sectors and
** only the middle sector is corrupt, we will still have a reasonable
** chance of failing the checksum and thus detecting the problem.
*/
static u32 pager_cksum(Pager *pPager, const u8 *aData){
  u32 cksum = pPager->cksumInit;
  int i = pPager->pageSize-200;
  while( i>0 ){
    cksum += aData[i];
    i -= 200;
  }
  return cksum;
}

/* Forward declaration */
static void makeClean(PgHdr*);

/*
** Read a single page from the journal file opened on file descriptor
** jfd.  Playback this one page.
**
** If useCksum==0 it means this journal does not use checksums.  Checksums
** are not used in statement journals because statement journals do not
** need to survive power failures.
*/
static int pager_playback_one_page(
  Pager *pPager, 
  sqlite3_file *jfd,
  i64 offset,
  int useCksum
){
  int rc;
  PgHdr *pPg;                   /* An existing page in the cache */
  Pgno pgno;                    /* The page number of a page in journal */
  u32 cksum;                    /* Checksum used for sanity checking */
  u8 *aData = (u8 *)pPager->pTmpSpace;   /* Temp storage for a page */

  /* useCksum should be true for the main journal and false for
  ** statement journals.  Verify that this is always the case
  */
  assert( jfd == (useCksum ? pPager->jfd : pPager->stfd) );
  assert( aData );

  rc = read32bits(jfd, offset, &pgno);
  if( rc!=SQLITE_OK ) return rc;
  rc = sqlite3OsRead(jfd, aData, pPager->pageSize, offset+4);
  if( rc!=SQLITE_OK ) return rc;
  pPager->journalOff += pPager->pageSize + 4;

  /* Sanity checking on the page.  This is more important that I originally
  ** thought.  If a power failure occurs while the journal is being written,
  ** it could cause invalid data to be written into the journal.  We need to
  ** detect this invalid data (with high probability) and ignore it.
  */
  if( pgno==0 || pgno==PAGER_MJ_PGNO(pPager) ){
    return SQLITE_DONE;
  }
  if( pgno>(unsigned)pPager->dbSize ){
    return SQLITE_OK;
  }
  if( useCksum ){
    rc = read32bits(jfd, offset+pPager->pageSize+4, &cksum);
    if( rc ) return rc;
    pPager->journalOff += 4;
    if( pager_cksum(pPager, aData)!=cksum ){
      return SQLITE_DONE;
    }
  }

  assert( pPager->state==PAGER_RESERVED || pPager->state>=PAGER_EXCLUSIVE );

  /* If the pager is in RESERVED state, then there must be a copy of this
  ** page in the pager cache. In this case just update the pager cache,
  ** not the database file. The page is left marked dirty in this case.
  **
  ** An exception to the above rule: If the database is in no-sync mode
  ** and a page is moved during an incremental vacuum then the page may
  ** not be in the pager cache. Later: if a malloc() or IO error occurs
  ** during a Movepage() call, then the page may not be in the cache
  ** either. So the condition described in the above paragraph is not
  ** assert()able.
  **
  ** If in EXCLUSIVE state, then we update the pager cache if it exists
  ** and the main file. The page is then marked not dirty.
  **
  ** Ticket #1171:  The statement journal might contain page content that is
  ** different from the page content at the start of the transaction.
  ** This occurs when a page is changed prior to the start of a statement
  ** then changed again within the statement.  When rolling back such a
  ** statement we must not write to the original database unless we know
  ** for certain that original page contents are synced into the main rollback
  ** journal.  Otherwise, a power loss might leave modified data in the
  ** database file without an entry in the rollback journal that can
  ** restore the database to its original form.  Two conditions must be
  ** met before writing to the database files. (1) the database must be
  ** locked.  (2) we know that the original page content is fully synced
  ** in the main journal either because the page is not in cache or else
  ** the page is marked as needSync==0.
  */
  pPg = pager_lookup(pPager, pgno);
  PAGERTRACE4("PLAYBACK %d page %d hash(%08x)\n",
               PAGERID(pPager), pgno, pager_datahash(pPager->pageSize, aData));
  if( pPager->state>=PAGER_EXCLUSIVE && (pPg==0 || pPg->needSync==0) ){
    i64 offset = (pgno-1)*(i64)pPager->pageSize;
    rc = sqlite3OsWrite(pPager->fd, aData, pPager->pageSize, offset);
    if( pPg ){
      makeClean(pPg);
    }
  }
  if( pPg ){
    /* No page should ever be explicitly rolled back that is in use, except
    ** for page 1 which is held in use in order to keep the lock on the
    ** database active. However such a page may be rolled back as a result
    ** of an internal error resulting in an automatic call to
    ** sqlite3PagerRollback().
    */
    void *pData;
    /* assert( pPg->nRef==0 || pPg->pgno==1 ); */
    pData = PGHDR_TO_DATA(pPg);
    memcpy(pData, aData, pPager->pageSize);
    if( pPager->xReiniter ){
      pPager->xReiniter(pPg, pPager->pageSize);
    }
#ifdef SQLITE_CHECK_PAGES
    pPg->pageHash = pager_pagehash(pPg);
#endif
    /* If this was page 1, then restore the value of Pager.dbFileVers.
    ** Do this before any decoding. */
    if( pgno==1 ){
      memcpy(&pPager->dbFileVers, &((u8*)pData)[24],sizeof(pPager->dbFileVers));
    }

    /* Decode the page just read from disk */
    CODEC1(pPager, pData, pPg->pgno, 3);
  }
  return rc;
}

/*
** Parameter zMaster is the name of a master journal file. A single journal
** file that referred to the master journal file has just been rolled back.
** This routine checks if it is possible to delete the master journal file,
** and does so if it is.
**
** Argument zMaster may point to Pager.pTmpSpace. So that buffer is not 
** available for use within this function.
**
**
** The master journal file contains the names of all child journals.
** To tell if a master journal can be deleted, check to each of the
** children.  If all children are either missing or do not refer to
** a different master journal, then this master journal can be deleted.
*/
static int pager_delmaster(Pager *pPager, const char *zMaster){
  sqlite3_vfs *pVfs = pPager->pVfs;
  int rc;
  int master_open = 0;
  sqlite3_file *pMaster;
  sqlite3_file *pJournal;
  char *zMasterJournal = 0; /* Contents of master journal file */
  i64 nMasterJournal;       /* Size of master journal file */

  /* Open the master journal file exclusively in case some other process
  ** is running this routine also. Not that it makes too much difference.
  */
  pMaster = (sqlite3_file *)sqlite3_malloc(pVfs->szOsFile * 2);
  pJournal = (sqlite3_file *)(((u8 *)pMaster) + pVfs->szOsFile);
  if( !pMaster ){
    rc = SQLITE_NOMEM;
  }else{
    int flags = (SQLITE_OPEN_READONLY|SQLITE_OPEN_MASTER_JOURNAL);
    rc = sqlite3OsOpen(pVfs, zMaster, pMaster, flags, 0);
  }
  if( rc!=SQLITE_OK ) goto delmaster_out;
  master_open = 1;

  rc = sqlite3OsFileSize(pMaster, &nMasterJournal);
  if( rc!=SQLITE_OK ) goto delmaster_out;

  if( nMasterJournal>0 ){
    char *zJournal;
    char *zMasterPtr = 0;
    int nMasterPtr = pPager->pVfs->mxPathname+1;

    /* Load the entire master journal file into space obtained from
    ** sqlite3_malloc() and pointed to by zMasterJournal. 
    */
    zMasterJournal = (char *)sqlite3_malloc(nMasterJournal + nMasterPtr);
    if( !zMasterJournal ){
      rc = SQLITE_NOMEM;
      goto delmaster_out;
    }
    zMasterPtr = &zMasterJournal[nMasterJournal];
    rc = sqlite3OsRead(pMaster, zMasterJournal, nMasterJournal, 0);
    if( rc!=SQLITE_OK ) goto delmaster_out;

    zJournal = zMasterJournal;
    while( (zJournal-zMasterJournal)<nMasterJournal ){
      if( sqlite3OsAccess(pVfs, zJournal, SQLITE_ACCESS_EXISTS) ){
        /* One of the journals pointed to by the master journal exists.
        ** Open it and check if it points at the master journal. If
        ** so, return without deleting the master journal file.
        */
        int c;
        int flags = (SQLITE_OPEN_READONLY|SQLITE_OPEN_MAIN_JOURNAL);
        rc = sqlite3OsOpen(pVfs, zJournal, pJournal, flags, 0);
        if( rc!=SQLITE_OK ){
          goto delmaster_out;
        }

        rc = readMasterJournal(pJournal, zMasterPtr, nMasterPtr);
        sqlite3OsClose(pJournal);
        if( rc!=SQLITE_OK ){
          goto delmaster_out;
        }

        c = zMasterPtr[0]!=0 && strcmp(zMasterPtr, zMaster)==0;
        if( c ){
          /* We have a match. Do not delete the master journal file. */
          goto delmaster_out;
        }
      }
      zJournal += (strlen(zJournal)+1);
    }
  }
  
  rc = sqlite3OsDelete(pVfs, zMaster, 0);

delmaster_out:
  if( zMasterJournal ){
    sqlite3_free(zMasterJournal);
  }  
  if( master_open ){
    sqlite3OsClose(pMaster);
  }
  sqlite3_free(pMaster);
  return rc;
}


static void pager_truncate_cache(Pager *pPager);

/*
** Truncate the main file of the given pager to the number of pages
** indicated. Also truncate the cached representation of the file.
**
** Might might be the case that the file on disk is smaller than nPage.
** This can happen, for example, if we are in the middle of a transaction
** which has extended the file size and the new pages are still all held
** in cache, then an INSERT or UPDATE does a statement rollback.  Some
** operating system implementations can get confused if you try to
** truncate a file to some size that is larger than it currently is,
** so detect this case and do not do the truncation.
*/
static int pager_truncate(Pager *pPager, int nPage){
  int rc = SQLITE_OK;
  if( pPager->state>=PAGER_EXCLUSIVE && pPager->fd->pMethods ){
    i64 currentSize, newSize;
    rc = sqlite3OsFileSize(pPager->fd, &currentSize);
    newSize = pPager->pageSize*(i64)nPage;
    if( rc==SQLITE_OK && currentSize>newSize ){
      rc = sqlite3OsTruncate(pPager->fd, newSize);
    }
  }
  if( rc==SQLITE_OK ){
    pPager->dbSize = nPage;
    pager_truncate_cache(pPager);
  }
  return rc;
}

/*
** Set the sectorSize for the given pager.
**
** The sector size is the larger of the sector size reported
** by sqlite3OsSectorSize() and the pageSize.
*/
static void setSectorSize(Pager *pPager){
  assert(pPager->fd->pMethods||pPager->tempFile);
  if( !pPager->tempFile ){
    /* Sector size doesn't matter for temporary files. Also, the file
    ** may not have been opened yet, in whcih case the OsSectorSize()
    ** call will segfault.
    */
    pPager->sectorSize = sqlite3OsSectorSize(pPager->fd);
  }
  if( pPager->sectorSize<pPager->pageSize ){
    pPager->sectorSize = pPager->pageSize;
  }
}

/*
** Playback the journal and thus restore the database file to
** the state it was in before we started making changes.  
**
** The journal file format is as follows: 
**
**  (1)  8 byte prefix.  A copy of aJournalMagic[].
**  (2)  4 byte big-endian integer which is the number of valid page records
**       in the journal.  If this value is 0xffffffff, then compute the
**       number of page records from the journal size.
**  (3)  4 byte big-endian integer which is the initial value for the 
**       sanity checksum.
**  (4)  4 byte integer which is the number of pages to truncate the
**       database to during a rollback.
**  (5)  4 byte integer which is the number of bytes in the master journal
**       name.  The value may be zero (indicate that there is no master
**       journal.)
**  (6)  N bytes of the master journal name.  The name will be nul-terminated
**       and might be shorter than the value read from (5).  If the first byte
**       of the name is \000 then there is no master journal.  The master
**       journal name is stored in UTF-8.
**  (7)  Zero or more pages instances, each as follows:
**        +  4 byte page number.
**        +  pPager->pageSize bytes of data.
**        +  4 byte checksum
**
** When we speak of the journal header, we mean the first 6 items above.
** Each entry in the journal is an instance of the 7th item.
**
** Call the value from the second bullet "nRec".  nRec is the number of
** valid page entries in the journal.  In most cases, you can compute the
** value of nRec from the size of the journal file.  But if a power
** failure occurred while the journal was being written, it could be the
** case that the size of the journal file had already been increased but
** the extra entries had not yet made it safely to disk.  In such a case,
** the value of nRec computed from the file size would be too large.  For
** that reason, we always use the nRec value in the header.
**
** If the nRec value is 0xffffffff it means that nRec should be computed
** from the file size.  This value is used when the user selects the
** no-sync option for the journal.  A power failure could lead to corruption
** in this case.  But for things like temporary table (which will be
** deleted when the power is restored) we don't care.  
**
** If the file opened as the journal file is not a well-formed
** journal file then all pages up to the first corrupted page are rolled
** back (or no pages if the journal header is corrupted). The journal file
** is then deleted and SQLITE_OK returned, just as if no corruption had
** been encountered.
**
** If an I/O or malloc() error occurs, the journal-file is not deleted
** and an error code is returned.
*/
static int pager_playback(Pager *pPager, int isHot){
  sqlite3_vfs *pVfs = pPager->pVfs;
  i64 szJ;                 /* Size of the journal file in bytes */
  u32 nRec;                /* Number of Records in the journal */
  int i;                   /* Loop counter */
  Pgno mxPg = 0;           /* Size of the original file in pages */
  int rc;                  /* Result code of a subroutine */
  char *zMaster = 0;       /* Name of master journal file if any */

  /* Figure out how many records are in the journal.  Abort early if
  ** the journal is empty.
  */
  assert( pPager->journalOpen );
  rc = sqlite3OsFileSize(pPager->jfd, &szJ);
  if( rc!=SQLITE_OK || szJ==0 ){
    goto end_playback;
  }

  /* Read the master journal name from the journal, if it is present.
  ** If a master journal file name is specified, but the file is not
  ** present on disk, then the journal is not hot and does not need to be
  ** played back.
  */
  zMaster = pPager->pTmpSpace;
  rc = readMasterJournal(pPager->jfd, zMaster, pPager->pVfs->mxPathname+1);
  assert( rc!=SQLITE_DONE );
  if( rc!=SQLITE_OK 
   || (zMaster[0] && !sqlite3OsAccess(pVfs, zMaster, SQLITE_ACCESS_EXISTS)) 
  ){
    zMaster = 0;
    if( rc==SQLITE_DONE ) rc = SQLITE_OK;
    goto end_playback;
  }
  pPager->journalOff = 0;
  zMaster = 0;

  /* This loop terminates either when the readJournalHdr() call returns
  ** SQLITE_DONE or an IO error occurs. */
  while( 1 ){

    /* Read the next journal header from the journal file.  If there are
    ** not enough bytes left in the journal file for a complete header, or
    ** it is corrupted, then a process must of failed while writing it.
    ** This indicates nothing more needs to be rolled back.
    */
    rc = readJournalHdr(pPager, szJ, &nRec, &mxPg);
    if( rc!=SQLITE_OK ){ 
      if( rc==SQLITE_DONE ){
        rc = SQLITE_OK;
      }
      goto end_playback;
    }

    /* If nRec is 0xffffffff, then this journal was created by a process
    ** working in no-sync mode. This means that the rest of the journal
    ** file consists of pages, there are no more journal headers. Compute
    ** the value of nRec based on this assumption.
    */
    if( nRec==0xffffffff ){
      assert( pPager->journalOff==JOURNAL_HDR_SZ(pPager) );
      nRec = (szJ - JOURNAL_HDR_SZ(pPager))/JOURNAL_PG_SZ(pPager);
    }

    /* If nRec is 0 and this rollback is of a transaction created by this
    ** process and if this is the final header in the journal, then it means
    ** that this part of the journal was being filled but has not yet been
    ** synced to disk.  Compute the number of pages based on the remaining
    ** size of the file.
    **
    ** The third term of the test was added to fix ticket #2565.
    */
    if( nRec==0 && !isHot &&
        pPager->journalHdr+JOURNAL_HDR_SZ(pPager)==pPager->journalOff ){
      nRec = (szJ - pPager->journalOff) / JOURNAL_PG_SZ(pPager);
    }

    /* If this is the first header read from the journal, truncate the
    ** database file back to its original size.
    */
    if( pPager->journalOff==JOURNAL_HDR_SZ(pPager) ){
      rc = pager_truncate(pPager, mxPg);
      if( rc!=SQLITE_OK ){
        goto end_playback;
      }
    }

    /* Copy original pages out of the journal and back into the database file.
    */
    for(i=0; i<nRec; i++){
      rc = pager_playback_one_page(pPager, pPager->jfd, pPager->journalOff, 1);
      if( rc!=SQLITE_OK ){
        if( rc==SQLITE_DONE ){
          rc = SQLITE_OK;
          pPager->journalOff = szJ;
          break;
        }else{
          goto end_playback;
        }
      }
    }
  }
  /*NOTREACHED*/
  assert( 0 );

end_playback:
  if( rc==SQLITE_OK ){
    zMaster = pPager->pTmpSpace;
    rc = readMasterJournal(pPager->jfd, zMaster, pPager->pVfs->mxPathname+1);
  }
  if( rc==SQLITE_OK ){
    rc = pager_end_transaction(pPager);
  }
  if( rc==SQLITE_OK && zMaster[0] ){
    /* If there was a master journal and this routine will return success,
    ** see if it is possible to delete the master journal.
    */
    rc = pager_delmaster(pPager, zMaster);
  }

  /* The Pager.sectorSize variable may have been updated while rolling
  ** back a journal created by a process with a different sector size
  ** value. Reset it to the correct value for this process.
  */
  setSectorSize(pPager);
  return rc;
}

/*
** Playback the statement journal.
**
** This is similar to playing back the transaction journal but with
** a few extra twists.
**
**    (1)  The number of pages in the database file at the start of
**         the statement is stored in pPager->stmtSize, not in the
**         journal file itself.
**
**    (2)  In addition to playing back the statement journal, also
**         playback all pages of the transaction journal beginning
**         at offset pPager->stmtJSize.
*/
static int pager_stmt_playback(Pager *pPager){
  i64 szJ;                 /* Size of the full journal */
  i64 hdrOff;
  int nRec;                /* Number of Records */
  int i;                   /* Loop counter */
  int rc;

  szJ = pPager->journalOff;
#ifndef NDEBUG 
  {
    i64 os_szJ;
    rc = sqlite3OsFileSize(pPager->jfd, &os_szJ);
    if( rc!=SQLITE_OK ) return rc;
    assert( szJ==os_szJ );
  }
#endif

  /* Set hdrOff to be the offset just after the end of the last journal
  ** page written before the first journal-header for this statement
  ** transaction was written, or the end of the file if no journal
  ** header was written.
  */
  hdrOff = pPager->stmtHdrOff;
  assert( pPager->fullSync || !hdrOff );
  if( !hdrOff ){
    hdrOff = szJ;
  }
  
  /* Truncate the database back to its original size.
  */
  rc = pager_truncate(pPager, pPager->stmtSize);
  assert( pPager->state>=PAGER_SHARED );

  /* Figure out how many records are in the statement journal.
  */
  assert( pPager->stmtInUse && pPager->journalOpen );
  nRec = pPager->stmtNRec;
  
  /* Copy original pages out of the statement journal and back into the
  ** database file.  Note that the statement journal omits checksums from
  ** each record since power-failure recovery is not important to statement
  ** journals.
  */
  for(i=0; i<nRec; i++){
    i64 offset = i*(4+pPager->pageSize);
    rc = pager_playback_one_page(pPager, pPager->stfd, offset, 0);
    assert( rc!=SQLITE_DONE );
    if( rc!=SQLITE_OK ) goto end_stmt_playback;
  }

  /* Now roll some pages back from the transaction journal. Pager.stmtJSize
  ** was the size of the journal file when this statement was started, so
  ** everything after that needs to be rolled back, either into the
  ** database, the memory cache, or both.
  **
  ** If it is not zero, then Pager.stmtHdrOff is the offset to the start
  ** of the first journal header written during this statement transaction.
  */
  pPager->journalOff = pPager->stmtJSize;
  pPager->cksumInit = pPager->stmtCksum;
  while( pPager->journalOff < hdrOff ){
    rc = pager_playback_one_page(pPager, pPager->jfd, pPager->journalOff, 1);
    assert( rc!=SQLITE_DONE );
    if( rc!=SQLITE_OK ) goto end_stmt_playback;
  }

  while( pPager->journalOff < szJ ){
    u32 nJRec;         /* Number of Journal Records */
    u32 dummy;
    rc = readJournalHdr(pPager, szJ, &nJRec, &dummy);
    if( rc!=SQLITE_OK ){
      assert( rc!=SQLITE_DONE );
      goto end_stmt_playback;
    }
    if( nJRec==0 ){
      nJRec = (szJ - pPager->journalOff) / (pPager->pageSize+8);
    }
    for(i=nJRec-1; i>=0 && pPager->journalOff < szJ; i--){
      rc = pager_playback_one_page(pPager, pPager->jfd, pPager->journalOff, 1);
      assert( rc!=SQLITE_DONE );
      if( rc!=SQLITE_OK ) goto end_stmt_playback;
    }
  }

  pPager->journalOff = szJ;
  
end_stmt_playback:
  if( rc==SQLITE_OK) {
    pPager->journalOff = szJ;
    /* pager_reload_cache(pPager); */
  }
  return rc;
}

/*
** Change the maximum number of in-memory pages that are allowed.
*/
void sqlite3PagerSetCachesize(Pager *pPager, int mxPage){
  if( mxPage>10 ){
    pPager->mxPage = mxPage;
  }else{
    pPager->mxPage = 10;
  }
}

/*
** Adjust the robustness of the database to damage due to OS crashes
** or power failures by changing the number of syncs()s when writing
** the rollback journal.  There are three levels:
**
**    OFF       sqlite3OsSync() is never called.  This is the default
**              for temporary and transient files.
**
**    NORMAL    The journal is synced once before writes begin on the
**              database.  This is normally adequate protection, but
**              it is theoretically possible, though very unlikely,
**              that an inopertune power failure could leave the journal
**              in a state which would cause damage to the database
**              when it is rolled back.
**
**    FULL      The journal is synced twice before writes begin on the
**              database (with some additional information - the nRec field
**              of the journal header - being written in between the two
**              syncs).  If we assume that writing a
**              single disk sector is atomic, then this mode provides
**              assurance that the journal will not be corrupted to the
**              point of causing damage to the database during rollback.
**
** Numeric values associated with these states are OFF==1, NORMAL=2,
** and FULL=3.
*/
#ifndef SQLITE_OMIT_PAGER_PRAGMAS
void sqlite3PagerSetSafetyLevel(Pager *pPager, int level, int full_fsync){
  pPager->noSync =  level==1 || pPager->tempFile;
  pPager->fullSync = level==3 && !pPager->tempFile;
  pPager->sync_flags = (full_fsync?SQLITE_SYNC_FULL:SQLITE_SYNC_NORMAL);
  if( pPager->noSync ) pPager->needSync = 0;
}
#endif

/*
** The following global variable is incremented whenever the library
** attempts to open a temporary file.  This information is used for
** testing and analysis only.  
*/
#ifdef SQLITE_TEST
int sqlite3_opentemp_count = 0;
#endif

/*
** Open a temporary file. 
**
** Write the file descriptor into *fd.  Return SQLITE_OK on success or some
** other error code if we fail. The OS will automatically delete the temporary
** file when it is closed.
*/
static int sqlite3PagerOpentemp(
  sqlite3_vfs *pVfs,    /* The virtual file system layer */
  sqlite3_file *pFile,  /* Write the file descriptor here */
  char *zFilename,      /* Name of the file.  Might be NULL */
  int vfsFlags          /* Flags passed through to the VFS */
){
  int rc;
  assert( zFilename!=0 );

#ifdef SQLITE_TEST
  sqlite3_opentemp_count++;  /* Used for testing and analysis only */
#endif

  vfsFlags |=  SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
            SQLITE_OPEN_EXCLUSIVE | SQLITE_OPEN_DELETEONCLOSE;
  rc = sqlite3OsOpen(pVfs, zFilename, pFile, vfsFlags, 0);
  assert( rc!=SQLITE_OK || pFile->pMethods );
  return rc;
}

/*
** Create a new page cache and put a pointer to the page cache in *ppPager.
** The file to be cached need not exist.  The file is not locked until
** the first call to sqlite3PagerGet() and is only held open until the
** last page is released using sqlite3PagerUnref().
**
** If zFilename is NULL then a randomly-named temporary file is created
** and used as the file to be cached.  The file will be deleted
** automatically when it is closed.
**
** If zFilename is ":memory:" then all information is held in cache.
** It is never written to disk.  This can be used to implement an
** in-memory database.
*/
int sqlite3PagerOpen(
  sqlite3_vfs *pVfs,       /* The virtual file system to use */
  Pager **ppPager,         /* Return the Pager structure here */
  const char *zFilename,   /* Name of the database file to open */
  int nExtra,              /* Extra bytes append to each in-memory page */
  int flags,               /* flags controlling this file */
  int vfsFlags             /* flags passed through to sqlite3_vfs.xOpen() */
){
  u8 *pPtr;
  Pager *pPager = 0;
  int rc = SQLITE_OK;
  int i;
  int tempFile = 0;
  int memDb = 0;
  int readOnly = 0;
  int useJournal = (flags & PAGER_OMIT_JOURNAL)==0;
  int noReadlock = (flags & PAGER_NO_READLOCK)!=0;
  int journalFileSize = sqlite3JournalSize(pVfs);
  int nDefaultPage = SQLITE_DEFAULT_PAGE_SIZE;
  char *zPathname;
  int nPathname;

  /* The default return is a NULL pointer */
  *ppPager = 0;

  /* Compute the full pathname */
  nPathname = pVfs->mxPathname+1;
  zPathname = sqlite3_malloc(nPathname);
  if( zPathname==0 ){
    return SQLITE_NOMEM;
  }
  if( zFilename && zFilename[0] ){
#ifndef SQLITE_OMIT_MEMORYDB
    if( strcmp(zFilename,":memory:")==0 ){
      memDb = 1;
      zPathname[0] = 0;
    }else
#endif
    {
      rc = sqlite3OsFullPathname(pVfs, zFilename, nPathname, zPathname);
    }
  }else{
    rc = sqlite3OsGetTempname(pVfs, nPathname, zPathname);
  }
  if( rc!=SQLITE_OK ){
    sqlite3_free(zPathname);
    return rc;
  }
  nPathname = strlen(zPathname);

  /* Allocate memory for the pager structure */
  pPager = sqlite3MallocZero(
    sizeof(*pPager) +           /* Pager structure */
    journalFileSize +           /* The journal file structure */ 
    pVfs->szOsFile * 2 +        /* The db and stmt journal files */ 
    4*nPathname + 40            /* zFilename, zDirectory, zJournal, zStmtJrnl */
  );
  if( !pPager ){
    sqlite3_free(zPathname);
    return SQLITE_NOMEM;
  }
  pPtr = (u8 *)&pPager[1];
  pPager->vfsFlags = vfsFlags;
  pPager->fd = (sqlite3_file*)&pPtr[pVfs->szOsFile*0];
  pPager->stfd = (sqlite3_file*)&pPtr[pVfs->szOsFile*1];
  pPager->jfd = (sqlite3_file*)&pPtr[pVfs->szOsFile*2];
  pPager->zFilename = (char*)&pPtr[pVfs->szOsFile*2+journalFileSize];
  pPager->zDirectory = &pPager->zFilename[nPathname+1];
  pPager->zJournal = &pPager->zDirectory[nPathname+1];
  pPager->zStmtJrnl = &pPager->zJournal[nPathname+10];
  pPager->pVfs = pVfs;
  memcpy(pPager->zFilename, zPathname, nPathname+1);
  sqlite3_free(zPathname);

  /* Open the pager file.
  */
  if( zFilename && zFilename[0] && !memDb ){
    if( nPathname>(pVfs->mxPathname - sizeof("-journal")) ){
      rc = SQLITE_CANTOPEN;
    }else{
      int fout = 0;
      rc = sqlite3OsOpen(pVfs, pPager->zFilename, pPager->fd,
                         pPager->vfsFlags, &fout);
      readOnly = (fout&SQLITE_OPEN_READONLY);

      /* If the file was successfully opened for read/write access,
      ** choose a default page size in case we have to create the
      ** database file. The default page size is the maximum of:
      **
      **    + SQLITE_DEFAULT_PAGE_SIZE,
      **    + The value returned by sqlite3OsSectorSize()
      **    + The largest page size that can be written atomically.
      */
      if( rc==SQLITE_OK && !readOnly ){
        int iSectorSize = sqlite3OsSectorSize(pPager->fd);
        if( nDefaultPage<iSectorSize ){
          nDefaultPage = iSectorSize;
        }
#ifdef SQLITE_ENABLE_ATOMIC_WRITE
        {
          int iDc = sqlite3OsDeviceCharacteristics(pPager->fd);
          int ii;
          assert(SQLITE_IOCAP_ATOMIC512==(512>>8));
          assert(SQLITE_IOCAP_ATOMIC64K==(65536>>8));
          assert(SQLITE_MAX_DEFAULT_PAGE_SIZE<=65536);
          for(ii=nDefaultPage; ii<=SQLITE_MAX_DEFAULT_PAGE_SIZE; ii=ii*2){
            if( iDc&(SQLITE_IOCAP_ATOMIC|(ii>>8)) ) nDefaultPage = ii;
          }
        }
#endif
        if( nDefaultPage>SQLITE_MAX_DEFAULT_PAGE_SIZE ){
          nDefaultPage = SQLITE_MAX_DEFAULT_PAGE_SIZE;
        }
      }
    }
  }else if( !memDb ){
    /* If a temporary file is requested, it is not opened immediately.
    ** In this case we accept the default page size and delay actually
    ** opening the file until the first call to OsWrite().
    */ 
    tempFile = 1;
    pPager->state = PAGER_EXCLUSIVE;
  }

  if( pPager && rc==SQLITE_OK ){
    pPager->pTmpSpace = (char *)sqlite3_malloc(nDefaultPage);
  }

  /* If an error occured in either of the blocks above.
  ** Free the Pager structure and close the file.
  ** Since the pager is not allocated there is no need to set 
  ** any Pager.errMask variables.
  */
  if( !pPager || !pPager->pTmpSpace ){
    sqlite3OsClose(pPager->fd);
    sqlite3_free(pPager);
    return ((rc==SQLITE_OK)?SQLITE_NOMEM:rc);
  }

  PAGERTRACE3("OPEN %d %s\n", FILEHANDLEID(pPager->fd), pPager->zFilename);
  IOTRACE(("OPEN %p %s\n", pPager, pPager->zFilename))

  /* Fill in Pager.zDirectory[] */
  memcpy(pPager->zDirectory, pPager->zFilename, nPathname+1);
  for(i=strlen(pPager->zDirectory); i>0 && pPager->zDirectory[i-1]!='/'; i--){}
  if( i>0 ) pPager->zDirectory[i-1] = 0;

  /* Fill in Pager.zJournal[] and Pager.zStmtJrnl[] */
  memcpy(pPager->zJournal, pPager->zFilename, nPathname);
  memcpy(&pPager->zJournal[nPathname], "-journal", 9);
  memcpy(pPager->zStmtJrnl, pPager->zFilename, nPathname);
  memcpy(&pPager->zStmtJrnl[nPathname], "-stmtjrnl", 10);

  /* pPager->journalOpen = 0; */
  pPager->useJournal = useJournal && !memDb;
  pPager->noReadlock = noReadlock && readOnly;
  /* pPager->stmtOpen = 0; */
  /* pPager->stmtInUse = 0; */
  /* pPager->nRef = 0; */
  pPager->dbSize = memDb-1;
  pPager->pageSize = nDefaultPage;
  /* pPager->stmtSize = 0; */
  /* pPager->stmtJSize = 0; */
  /* pPager->nPage = 0; */
  pPager->mxPage = 100;
  pPager->mxPgno = SQLITE_MAX_PAGE_COUNT;
  /* pPager->state = PAGER_UNLOCK; */
  assert( pPager->state == (tempFile ? PAGER_EXCLUSIVE : PAGER_UNLOCK) );
  /* pPager->errMask = 0; */
  pPager->tempFile = tempFile;
  assert( tempFile==PAGER_LOCKINGMODE_NORMAL 
          || tempFile==PAGER_LOCKINGMODE_EXCLUSIVE );
  assert( PAGER_LOCKINGMODE_EXCLUSIVE==1 );
  pPager->exclusiveMode = tempFile; 
  pPager->memDb = memDb;
  pPager->readOnly = readOnly;
  /* pPager->needSync = 0; */
  pPager->noSync = pPager->tempFile || !useJournal;
  pPager->fullSync = (pPager->noSync?0:1);
  pPager->sync_flags = SQLITE_SYNC_NORMAL;
  /* pPager->pFirst = 0; */
  /* pPager->pFirstSynced = 0; */
  /* pPager->pLast = 0; */
  pPager->nExtra = FORCE_ALIGNMENT(nExtra);
  assert(pPager->fd->pMethods||memDb||tempFile);
  if( !memDb ){
    setSectorSize(pPager);
  }
  /* pPager->pBusyHandler = 0; */
  /* memset(pPager->aHash, 0, sizeof(pPager->aHash)); */
  *ppPager = pPager;
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  pPager->iInUseMM = 0;
  pPager->iInUseDB = 0;
  if( !memDb ){
    sqlite3_mutex *mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MEM2);
    sqlite3_mutex_enter(mutex);
    pPager->pNext = sqlite3PagerList;
    if( sqlite3PagerList ){
      assert( sqlite3PagerList->pPrev==0 );
      sqlite3PagerList->pPrev = pPager;
    }
    pPager->pPrev = 0;
    sqlite3PagerList = pPager;
    sqlite3_mutex_leave(mutex);
  }
#endif
  return SQLITE_OK;
}

/*
** Set the busy handler function.
*/
void sqlite3PagerSetBusyhandler(Pager *pPager, BusyHandler *pBusyHandler){
  pPager->pBusyHandler = pBusyHandler;
}

/*
** Set the destructor for this pager.  If not NULL, the destructor is called
** when the reference count on each page reaches zero.  The destructor can
** be used to clean up information in the extra segment appended to each page.
**
** The destructor is not called as a result sqlite3PagerClose().  
** Destructors are only called by sqlite3PagerUnref().
*/
void sqlite3PagerSetDestructor(Pager *pPager, void (*xDesc)(DbPage*,int)){
  pPager->xDestructor = xDesc;
}

/*
** Set the reinitializer for this pager.  If not NULL, the reinitializer
** is called when the content of a page in cache is restored to its original
** value as a result of a rollback.  The callback gives higher-level code
** an opportunity to restore the EXTRA section to agree with the restored
** page data.
*/
void sqlite3PagerSetReiniter(Pager *pPager, void (*xReinit)(DbPage*,int)){
  pPager->xReiniter = xReinit;
}

/*
** Set the page size to *pPageSize. If the suggest new page size is
** inappropriate, then an alternative page size is set to that
** value before returning.
*/
int sqlite3PagerSetPagesize(Pager *pPager, u16 *pPageSize){
  int rc = SQLITE_OK;
  u16 pageSize = *pPageSize;
  assert( pageSize==0 || (pageSize>=512 && pageSize<=SQLITE_MAX_PAGE_SIZE) );
  if( pageSize && pageSize!=pPager->pageSize 
   && !pPager->memDb && pPager->nRef==0 
  ){
    char *pNew = (char *)sqlite3_malloc(pageSize);
    if( !pNew ){
      rc = SQLITE_NOMEM;
    }else{
      pagerEnter(pPager);
      pager_reset(pPager);
      pPager->pageSize = pageSize;
      setSectorSize(pPager);
      sqlite3_free(pPager->pTmpSpace);
      pPager->pTmpSpace = pNew;
      pagerLeave(pPager);
    }
  }
  *pPageSize = pPager->pageSize;
  return rc;
}

/*
** Return a pointer to the "temporary page" buffer held internally
** by the pager.  This is a buffer that is big enough to hold the
** entire content of a database page.  This buffer is used internally
** during rollback and will be overwritten whenever a rollback
** occurs.  But other modules are free to use it too, as long as
** no rollbacks are happening.
*/
void *sqlite3PagerTempSpace(Pager *pPager){
  return pPager->pTmpSpace;
}

/*
** Attempt to set the maximum database page count if mxPage is positive. 
** Make no changes if mxPage is zero or negative.  And never reduce the
** maximum page count below the current size of the database.
**
** Regardless of mxPage, return the current maximum page count.
*/
int sqlite3PagerMaxPageCount(Pager *pPager, int mxPage){
  if( mxPage>0 ){
    pPager->mxPgno = mxPage;
  }
  sqlite3PagerPagecount(pPager);
  return pPager->mxPgno;
}

/*
** The following set of routines are used to disable the simulated
** I/O error mechanism.  These routines are used to avoid simulated
** errors in places where we do not care about errors.
**
** Unless -DSQLITE_TEST=1 is used, these routines are all no-ops
** and generate no code.
*/
#ifdef SQLITE_TEST
extern int sqlite3_io_error_pending;
extern int sqlite3_io_error_hit;
static int saved_cnt;
void disable_simulated_io_errors(void){
  saved_cnt = sqlite3_io_error_pending;
  sqlite3_io_error_pending = -1;
}
void enable_simulated_io_errors(void){
  sqlite3_io_error_pending = saved_cnt;
}
#else
# define disable_simulated_io_errors()
# define enable_simulated_io_errors()
#endif

/*
** Read the first N bytes from the beginning of the file into memory
** that pDest points to. 
**
** No error checking is done. The rational for this is that this function 
** may be called even if the file does not exist or contain a header. In 
** these cases sqlite3OsRead() will return an error, to which the correct 
** response is to zero the memory at pDest and continue.  A real IO error 
** will presumably recur and be picked up later (Todo: Think about this).
*/
int sqlite3PagerReadFileheader(Pager *pPager, int N, unsigned char *pDest){
  int rc = SQLITE_OK;
  memset(pDest, 0, N);
  assert(MEMDB||pPager->fd->pMethods||pPager->tempFile);
  if( pPager->fd->pMethods ){
    IOTRACE(("DBHDR %p 0 %d\n", pPager, N))
    rc = sqlite3OsRead(pPager->fd, pDest, N, 0);
    if( rc==SQLITE_IOERR_SHORT_READ ){
      rc = SQLITE_OK;
    }
  }
  return rc;
}

/*
** Return the total number of pages in the disk file associated with
** pPager. 
**
** If the PENDING_BYTE lies on the page directly after the end of the
** file, then consider this page part of the file too. For example, if
** PENDING_BYTE is byte 4096 (the first byte of page 5) and the size of the
** file is 4096 bytes, 5 is returned instead of 4.
*/
int sqlite3PagerPagecount(Pager *pPager){
  i64 n = 0;
  int rc;
  assert( pPager!=0 );
  if( pPager->errCode ){
    return 0;
  }
  if( pPager->dbSize>=0 ){
    n = pPager->dbSize;
  } else {
    assert(pPager->fd->pMethods||pPager->tempFile);
    if( (pPager->fd->pMethods)
     && (rc = sqlite3OsFileSize(pPager->fd, &n))!=SQLITE_OK ){
      pPager->nRef++;
      pager_error(pPager, rc);
      pPager->nRef--;
      return 0;
    }
    if( n>0 && n<pPager->pageSize ){
      n = 1;
    }else{
      n /= pPager->pageSize;
    }
    if( pPager->state!=PAGER_UNLOCK ){
      pPager->dbSize = n;
    }
  }
  if( n==(PENDING_BYTE/pPager->pageSize) ){
    n++;
  }
  if( n>pPager->mxPgno ){
    pPager->mxPgno = n;
  }
  return n;
}


#ifndef SQLITE_OMIT_MEMORYDB
/*
** Clear a PgHistory block
*/
static void clearHistory(PgHistory *pHist){
  sqlite3_free(pHist->pOrig);
  sqlite3_free(pHist->pStmt);
  pHist->pOrig = 0;
  pHist->pStmt = 0;
}
#else
#define clearHistory(x)
#endif

/*
** Forward declaration
*/
static int syncJournal(Pager*);

/*
** Unlink pPg from its hash chain. Also set the page number to 0 to indicate
** that the page is not part of any hash chain. This is required because the
** sqlite3PagerMovepage() routine can leave a page in the 
** pNextFree/pPrevFree list that is not a part of any hash-chain.
*/
static void unlinkHashChain(Pager *pPager, PgHdr *pPg){
  if( pPg->pgno==0 ){
    assert( pPg->pNextHash==0 && pPg->pPrevHash==0 );
    return;
  }
  if( pPg->pNextHash ){
    pPg->pNextHash->pPrevHash = pPg->pPrevHash;
  }
  if( pPg->pPrevHash ){
    assert( pPager->aHash[pPg->pgno & (pPager->nHash-1)]!=pPg );
    pPg->pPrevHash->pNextHash = pPg->pNextHash;
  }else{
    int h = pPg->pgno & (pPager->nHash-1);
    pPager->aHash[h] = pPg->pNextHash;
  }
  if( MEMDB ){
    clearHistory(PGHDR_TO_HIST(pPg, pPager));
  }
  pPg->pgno = 0;
  pPg->pNextHash = pPg->pPrevHash = 0;
}

/*
** Unlink a page from the free list (the list of all pages where nRef==0)
** and from its hash collision chain.
*/
static void unlinkPage(PgHdr *pPg){
  Pager *pPager = pPg->pPager;

  /* Unlink from free page list */
  lruListRemove(pPg);

  /* Unlink from the pgno hash table */
  unlinkHashChain(pPager, pPg);
}

/*
** This routine is used to truncate the cache when a database
** is truncated.  Drop from the cache all pages whose pgno is
** larger than pPager->dbSize and is unreferenced.
**
** Referenced pages larger than pPager->dbSize are zeroed.
**
** Actually, at the point this routine is called, it would be
** an error to have a referenced page.  But rather than delete
** that page and guarantee a subsequent segfault, it seems better
** to zero it and hope that we error out sanely.
*/
static void pager_truncate_cache(Pager *pPager){
  PgHdr *pPg;
  PgHdr **ppPg;
  int dbSize = pPager->dbSize;

  ppPg = &pPager->pAll;
  while( (pPg = *ppPg)!=0 ){
    if( pPg->pgno<=dbSize ){
      ppPg = &pPg->pNextAll;
    }else if( pPg->nRef>0 ){
      memset(PGHDR_TO_DATA(pPg), 0, pPager->pageSize);
      ppPg = &pPg->pNextAll;
    }else{
      *ppPg = pPg->pNextAll;
      IOTRACE(("PGFREE %p %d\n", pPager, pPg->pgno));
      PAGER_INCR(sqlite3_pager_pgfree_count);
      unlinkPage(pPg);
      makeClean(pPg);
      sqlite3_free(pPg);
      pPager->nPage--;
    }
  }
}

/*
** Try to obtain a lock on a file.  Invoke the busy callback if the lock
** is currently not available.  Repeat until the busy callback returns
** false or until the lock succeeds.
**
** Return SQLITE_OK on success and an error code if we cannot obtain
** the lock.
*/
static int pager_wait_on_lock(Pager *pPager, int locktype){
  int rc;

  /* The OS lock values must be the same as the Pager lock values */
  assert( PAGER_SHARED==SHARED_LOCK );
  assert( PAGER_RESERVED==RESERVED_LOCK );
  assert( PAGER_EXCLUSIVE==EXCLUSIVE_LOCK );

  /* If the file is currently unlocked then the size must be unknown */
  assert( pPager->state>=PAGER_SHARED || pPager->dbSize<0 || MEMDB );

  if( pPager->state>=locktype ){
    rc = SQLITE_OK;
  }else{
    do {
      rc = sqlite3OsLock(pPager->fd, locktype);
    }while( rc==SQLITE_BUSY && sqlite3InvokeBusyHandler(pPager->pBusyHandler) );
    if( rc==SQLITE_OK ){
      pPager->state = locktype;
      IOTRACE(("LOCK %p %d\n", pPager, locktype))
    }
  }
  return rc;
}

/*
** Truncate the file to the number of pages specified.
*/
int sqlite3PagerTruncate(Pager *pPager, Pgno nPage){
  int rc;
  assert( pPager->state>=PAGER_SHARED || MEMDB );
  sqlite3PagerPagecount(pPager);
  if( pPager->errCode ){
    rc = pPager->errCode;
    return rc;
  }
  if( nPage>=(unsigned)pPager->dbSize ){
    return SQLITE_OK;
  }
  if( MEMDB ){
    pPager->dbSize = nPage;
    pager_truncate_cache(pPager);
    return SQLITE_OK;
  }
  pagerEnter(pPager);
  rc = syncJournal(pPager);
  pagerLeave(pPager);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  /* Get an exclusive lock on the database before truncating. */
  pagerEnter(pPager);
  rc = pager_wait_on_lock(pPager, EXCLUSIVE_LOCK);
  pagerLeave(pPager);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  rc = pager_truncate(pPager, nPage);
  return rc;
}

/*
** Shutdown the page cache.  Free all memory and close all files.
**
** If a transaction was in progress when this routine is called, that
** transaction is rolled back.  All outstanding pages are invalidated
** and their memory is freed.  Any attempt to use a page associated
** with this page cache after this function returns will likely
** result in a coredump.
**
** This function always succeeds. If a transaction is active an attempt
** is made to roll it back. If an error occurs during the rollback 
** a hot journal may be left in the filesystem but no error is returned
** to the caller.
*/
int sqlite3PagerClose(Pager *pPager){
#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
  if( !MEMDB ){
    sqlite3_mutex *mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MEM2);
    sqlite3_mutex_enter(mutex);
    if( pPager->pPrev ){
      pPager->pPrev->pNext = pPager->pNext;
    }else{
      sqlite3PagerList = pPager->pNext;
    }
    if( pPager->pNext ){
      pPager->pNext->pPrev = pPager->pPrev;
    }
    sqlite3_mutex_leave(mutex);
  }
#endif

  disable_simulated_io_errors();
  pPager->errCode = 0;
  pPager->exclusiveMode = 0;
  pager_reset(pPager);
  pagerUnlockAndRollback(pPager);
  enable_simulated_io_errors();
  PAGERTRACE2("CLOSE %d\n", PAGERID(pPager));
  IOTRACE(("CLOSE %p\n", pPager))
  assert( pPager->errCode || (pPager->journalOpen==0 && pPager->stmtOpen==0) );
  if( pPager->journalOpen ){
    sqlite3OsClose(pPager->jfd);
  }
  sqlite3_free(pPager->aInJournal);
  if( pPager->stmtOpen ){
    sqlite3OsClose(pPager->stfd);
  }
  sqlite3OsClose(pPager->fd);
  /* Temp files are automatically deleted by the OS
  ** if( pPager->tempFile ){
  **   sqlite3OsDelete(pPager->zFilename);
  ** }
  */

  sqlite3_free(pPager->aHash);
  sqlite3_free(pPager->pTmpSpace);
  sqlite3_free(pPager);
  return SQLITE_OK;
}

#if !defined(NDEBUG) || defined(SQLITE_TEST)
/*
** Return the page number for the given page data.
*/
Pgno sqlite3PagerPagenumber(DbPage *p){
  return p->pgno;
}
#endif

/*
** The page_ref() function increments the reference count for a page.
** If the page is currently on the freelist (the reference count is zero) then
** remove it from the freelist.
**
** For non-test systems, page_ref() is a macro that calls _page_ref()
** online of the reference count is zero.  For test systems, page_ref()
** is a real function so that we can set breakpoints and trace it.
*/
static void _page_ref(PgHdr *pPg){
  if( pPg->nRef==0 ){
    /* The page is currently on the freelist.  Remove it. */
    lruListRemove(pPg);
    pPg->pPager->nRef++;
  }
  pPg->nRef++;
}
#ifdef SQLITE_DEBUG
  static void page_ref(PgHdr *pPg){
    if( pPg->nRef==0 ){
      _page_ref(pPg);
    }else{
      pPg->nRef++;
    }
  }
#else
# define page_ref(P)   ((P)->nRef==0?_page_ref(P):(void)(P)->nRef++)
#endif

/*
** Increment the reference count for a page.  The input pointer is
** a reference to the page data.
*/
int sqlite3PagerRef(DbPage *pPg){
  pagerEnter(pPg->pPager);
  page_ref(pPg);
  pagerLeave(pPg->pPager);
  return SQLITE_OK;
}

/*
** Sync the journal.  In other words, make sure all the pages that have
** been written to the journal have actually reached the surface of the
** disk.  It is not safe to modify the original database file until after
** the journal has been synced.  If the original database is modified before
** the journal is synced and a power failure occurs, the unsynced journal
** data would be lost and we would be unable to completely rollback the
** database changes.  Database corruption would occur.
** 
** This routine also updates the nRec field in the header of the journal.
** (See comments on the pager_playback() routine for additional information.)
** If the sync mode is FULL, two syncs will occur.  First the whole journal
** is synced, then the nRec field is updated, then a second sync occurs.
**
** For temporary databases, we do not care if we are able to rollback
** after a power failure, so no sync occurs.
**
** If the IOCAP_SEQUENTIAL flag is set for the persistent media on which
** the database is stored, then OsSync() is never called on the journal
** file. In this case all that is required is to update the nRec field in
** the journal header.
**
** This routine clears the needSync field of every page current held in
** memory.
*/
static int syncJournal(Pager *pPager){
  PgHdr *pPg;
  int rc = SQLITE_OK;


  /* Sync the journal before modifying the main database
  ** (assuming there is a journal and it needs to be synced.)
  */
  if( pPager->needSync ){
    if( !pPager->tempFile ){
      int iDc = sqlite3OsDeviceCharacteristics(pPager->fd);
      assert( pPager->journalOpen );

      /* assert( !pPager->noSync ); // noSync might be set if synchronous
      ** was turned off after the transaction was started.  Ticket #615 */
#ifndef NDEBUG
      {
        /* Make sure the pPager->nRec counter we are keeping agrees
        ** with the nRec computed from the size of the journal file.
        */
        i64 jSz;
        rc = sqlite3OsFileSize(pPager->jfd, &jSz);
        if( rc!=0 ) return rc;
        assert( pPager->journalOff==jSz );
      }
#endif
      if( 0==(iDc&SQLITE_IOCAP_SAFE_APPEND) ){
        /* Write the nRec value into the journal file header. If in
        ** full-synchronous mode, sync the journal first. This ensures that
        ** all data has really hit the disk before nRec is updated to mark
        ** it as a candidate for rollback.
        **
        ** This is not required if the persistent media supports the
        ** SAFE_APPEND property. Because in this case it is not possible 
        ** for garbage data to be appended to the file, the nRec field
        ** is populated with 0xFFFFFFFF when the journal header is written
        ** and never needs to be updated.
        */
        i64 jrnlOff;
        if( pPager->fullSync && 0==(iDc&SQLITE_IOCAP_SEQUENTIAL) ){
          PAGERTRACE2("SYNC journal of %d\n", PAGERID(pPager));
          IOTRACE(("JSYNC %p\n", pPager))
          rc = sqlite3OsSync(pPager->jfd, pPager->sync_flags);
          if( rc!=0 ) return rc;
        }

        jrnlOff = pPager->journalHdr + sizeof(aJournalMagic);
        IOTRACE(("JHDR %p %lld %d\n", pPager, jrnlOff, 4));
        rc = write32bits(pPager->jfd, jrnlOff, pPager->nRec);
        if( rc ) return rc;
      }
      if( 0==(iDc&SQLITE_IOCAP_SEQUENTIAL) ){
        PAGERTRACE2("SYNC journal of %d\n", PAGERID(pPager));
        IOTRACE(("JSYNC %p\n", pPager))
        rc = sqlite3OsSync(pPager->jfd, pPager->sync_flags| 
          (pPager->sync_flags==SQLITE_SYNC_FULL?SQLITE_SYNC_DATAONLY:0)
        );
        if( rc!=0 ) return rc;
      }
      pPager->journalStarted = 1;
    }
    pPager->needSync = 0;

    /* Erase the needSync flag from every page.
    */
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      pPg->needSync = 0;
    }
    lruListSetFirstSynced(pPager);
  }

#ifndef NDEBUG
  /* If the Pager.needSync flag is clear then the PgHdr.needSync
  ** flag must also be clear for all pages.  Verify that this
  ** invariant is true.
  */
  else{
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      assert( pPg->needSync==0 );
    }
    assert( pPager->lru.pFirstSynced==pPager->lru.pFirst );
  }
#endif

  return rc;
}

/*
** Merge two lists of pages connected by pDirty and in pgno order.
** Do not both fixing the pPrevDirty pointers.
*/
static PgHdr *merge_pagelist(PgHdr *pA, PgHdr *pB){
  PgHdr result, *pTail;
  pTail = &result;
  while( pA && pB ){
    if( pA->pgno<pB->pgno ){
      pTail->pDirty = pA;
      pTail = pA;
      pA = pA->pDirty;
    }else{
      pTail->pDirty = pB;
      pTail = pB;
      pB = pB->pDirty;
    }
  }
  if( pA ){
    pTail->pDirty = pA;
  }else if( pB ){
    pTail->pDirty = pB;
  }else{
    pTail->pDirty = 0;
  }
  return result.pDirty;
}

/*
** Sort the list of pages in accending order by pgno.  Pages are
** connected by pDirty pointers.  The pPrevDirty pointers are
** corrupted by this sort.
*/
#define N_SORT_BUCKET_ALLOC 25
#define N_SORT_BUCKET       25
#ifdef SQLITE_TEST
  int sqlite3_pager_n_sort_bucket = 0;
  #undef N_SORT_BUCKET
  #define N_SORT_BUCKET \
   (sqlite3_pager_n_sort_bucket?sqlite3_pager_n_sort_bucket:N_SORT_BUCKET_ALLOC)
#endif
static PgHdr *sort_pagelist(PgHdr *pIn){
  PgHdr *a[N_SORT_BUCKET_ALLOC], *p;
  int i;
  memset(a, 0, sizeof(a));
  while( pIn ){
    p = pIn;
    pIn = p->pDirty;
    p->pDirty = 0;
    for(i=0; i<N_SORT_BUCKET-1; i++){
      if( a[i]==0 ){
        a[i] = p;
        break;
      }else{
        p = merge_pagelist(a[i], p);
        a[i] = 0;
      }
    }
    if( i==N_SORT_BUCKET-1 ){
      /* Coverage: To get here, there need to be 2^(N_SORT_BUCKET) 
      ** elements in the input list. This is possible, but impractical.
      ** Testing this line is the point of global variable
      ** sqlite3_pager_n_sort_bucket.
      */
      a[i] = merge_pagelist(a[i], p);
    }
  }
  p = a[0];
  for(i=1; i<N_SORT_BUCKET; i++){
    p = merge_pagelist(p, a[i]);
  }
  return p;
}

/*
** Given a list of pages (connected by the PgHdr.pDirty pointer) write
** every one of those pages out to the database file and mark them all
** as clean.
*/
static int pager_write_pagelist(PgHdr *pList){
  Pager *pPager;
  PgHdr *p;
  int rc;

  if( pList==0 ) return SQLITE_OK;
  pPager = pList->pPager;

  /* At this point there may be either a RESERVED or EXCLUSIVE lock on the
  ** database file. If there is already an EXCLUSIVE lock, the following
  ** calls to sqlite3OsLock() are no-ops.
  **
  ** Moving the lock from RESERVED to EXCLUSIVE actually involves going
  ** through an intermediate state PENDING.   A PENDING lock prevents new
  ** readers from attaching to the database but is unsufficient for us to
  ** write.  The idea of a PENDING lock is to prevent new readers from
  ** coming in while we wait for existing readers to clear.
  **
  ** While the pager is in the RESERVED state, the original database file
  ** is unchanged and we can rollback without having to playback the
  ** journal into the original database file.  Once we transition to
  ** EXCLUSIVE, it means the database file has been changed and any rollback
  ** will require a journal playback.
  */
  rc = pager_wait_on_lock(pPager, EXCLUSIVE_LOCK);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  pList = sort_pagelist(pList);
  for(p=pList; p; p=p->pDirty){
    assert( p->dirty );
    p->dirty = 0;
  }
  while( pList ){

    /* If the file has not yet been opened, open it now. */
    if( !pPager->fd->pMethods ){
      assert(pPager->tempFile);
      rc = sqlite3PagerOpentemp(pPager->pVfs, pPager->fd, pPager->zFilename,
                                pPager->vfsFlags);
      if( rc ) return rc;
    }

    /* If there are dirty pages in the page cache with page numbers greater
    ** than Pager.dbSize, this means sqlite3PagerTruncate() was called to
    ** make the file smaller (presumably by auto-vacuum code). Do not write
    ** any such pages to the file.
    */
    if( pList->pgno<=pPager->dbSize ){
      i64 offset = (pList->pgno-1)*(i64)pPager->pageSize;
      char *pData = CODEC2(pPager, PGHDR_TO_DATA(pList), pList->pgno, 6);
      PAGERTRACE4("STORE %d page %d hash(%08x)\n",
                   PAGERID(pPager), pList->pgno, pager_pagehash(pList));
      IOTRACE(("PGOUT %p %d\n", pPager, pList->pgno));
      rc = sqlite3OsWrite(pPager->fd, pData, pPager->pageSize, offset);
      PAGER_INCR(sqlite3_pager_writedb_count);
      PAGER_INCR(pPager->nWrite);
      if( pList->pgno==1 ){
        memcpy(&pPager->dbFileVers, &pData[24], sizeof(pPager->dbFileVers));
      }
    }
#ifndef NDEBUG
    else{
      PAGERTRACE3("NOSTORE %d page %d\n", PAGERID(pPager), pList->pgno);
    }
#endif
    if( rc ) return rc;
#ifdef SQLITE_CHECK_PAGES
    pList->pageHash = pager_pagehash(pList);
#endif
    pList = pList->pDirty;
  }
  return SQLITE_OK;
}

/*
** Collect every dirty page into a dirty list and
** return a pointer to the head of that list.  All pages are
** collected even if they are still in use.
*/
static PgHdr *pager_get_all_dirty_pages(Pager *pPager){
  return pPager->pDirty;
}

/*
** Return TRUE if there is a hot journal on the given pager.
** A hot journal is one that needs to be played back.
**
** If the current size of the database file is 0 but a journal file
** exists, that is probably an old journal left over from a prior
** database with the same name.  Just delete the journal.
*/
static int hasHotJournal(Pager *pPager){
  sqlite3_vfs *pVfs = pPager->pVfs;
  if( !pPager->useJournal ) return 0;
  if( !pPager->fd->pMethods ) return 0;
  if( !sqlite3OsAccess(pVfs, pPager->zJournal, SQLITE_ACCESS_EXISTS) ){
    return 0;
  }
  if( sqlite3OsCheckReservedLock(pPager->fd) ){
    return 0;
  }
  if( sqlite3PagerPagecount(pPager)==0 ){
    sqlite3OsDelete(pVfs, pPager->zJournal, 0);
    return 0;
  }else{
    return 1;
  }
}

/*
** Try to find a page in the cache that can be recycled. 
**
** This routine may return SQLITE_IOERR, SQLITE_FULL or SQLITE_OK. It 
** does not set the pPager->errCode variable.
*/
static int pager_recycle(Pager *pPager, PgHdr **ppPg){
  PgHdr *pPg;
  *ppPg = 0;

  /* It is illegal to call this function unless the pager object
  ** pointed to by pPager has at least one free page (page with nRef==0).
  */ 
  assert(!MEMDB);
  assert(pPager->lru.pFirst);

  /* Find a page to recycle.  Try to locate a page that does not
  ** require us to do an fsync() on the journal.
  */
  pPg = pPager->lru.pFirstSynced;

  /* If we could not find a page that does not require an fsync()
  ** on the journal file then fsync the journal file.  This is a
  ** very slow operation, so we work hard to avoid it.  But sometimes
  ** it can't be helped.
  */
  if( pPg==0 && pPager->lru.pFirst){
    int iDc = sqlite3OsDeviceCharacteristics(pPager->fd);
    int rc = syncJournal(pPager);
    if( rc!=0 ){
      return rc;
    }
    if( pPager->fullSync && 0==(iDc&SQLITE_IOCAP_SAFE_APPEND) ){
      /* If in full-sync mode, write a new journal header into the
      ** journal file. This is done to avoid ever modifying a journal
      ** header that is involved in the rollback of pages that have
      ** already been written to the database (in case the header is
      ** trashed when the nRec field is updated).
      */
      pPager->nRec = 0;
      assert( pPager->journalOff > 0 );
      assert( pPager->doNotSync==0 );
      rc = writeJournalHdr(pPager);
      if( rc!=0 ){
        return rc;
      }
    }
    pPg = pPager->lru.pFirst;
  }

  assert( pPg->nRef==0 );

  /* Write the page to the database file if it is dirty.
  */
  if( pPg->dirty ){
    int rc;
    assert( pPg->needSync==0 );
    makeClean(pPg);
    pPg->dirty = 1;
    pPg->pDirty = 0;
    rc = pager_write_pagelist( pPg );
    pPg->dirty = 0;
    if( rc!=SQLITE_OK ){
      return rc;
    }
  }
  assert( pPg->dirty==0 );

  /* If the page we are recycling is marked as alwaysRollback, then
  ** set the global alwaysRollback flag, thus disabling the
  ** sqlite3PagerDontRollback() optimization for the rest of this transaction.
  ** It is necessary to do this because the page marked alwaysRollback
  ** might be reloaded at a later time but at that point we won't remember
  ** that is was marked alwaysRollback.  This means that all pages must
  ** be marked as alwaysRollback from here on out.
  */
  if( pPg->alwaysRollback ){
    IOTRACE(("ALWAYS_ROLLBACK %p\n", pPager))
    pPager->alwaysRollback = 1;
  }

  /* Unlink the old page from the free list and the hash table
  */
  unlinkPage(pPg);
  assert( pPg->pgno==0 );

  *ppPg = pPg;
  return SQLITE_OK;
}

#ifdef SQLITE_ENABLE_MEMORY_MANAGEMENT
/*
** This function is called to free superfluous dynamically allocated memory
** held by the pager system. Memory in use by any SQLite pager allocated
** by the current thread may be sqlite3_free()ed.
**
** nReq is the number of bytes of memory required. Once this much has
** been released, the function returns. The return value is the total number 
** of bytes of memory released.
*/
int sqlite3PagerReleaseMemory(int nReq){
  int nReleased = 0;          /* Bytes of memory released so far */
  sqlite3_mutex *mutex;       /* The MEM2 mutex */
  Pager *pPager;              /* For looping over pagers */
  BusyHandler *savedBusy;     /* Saved copy of the busy handler */
  int rc = SQLITE_OK;

  /* Acquire the memory-management mutex
  */
  mutex = sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_MEM2);
  sqlite3_mutex_enter(mutex);

  /* Signal all database connections that memory management wants
  ** to have access to the pagers.
  */
  for(pPager=sqlite3PagerList; pPager; pPager=pPager->pNext){
     pPager->iInUseMM = 1;
  }

  while( rc==SQLITE_OK && (nReq<0 || nReleased<nReq) ){
    PgHdr *pPg;
    PgHdr *pRecycled;
 
    /* Try to find a page to recycle that does not require a sync(). If
    ** this is not possible, find one that does require a sync().
    */
    sqlite3_mutex_enter(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));
    pPg = sqlite3LruPageList.pFirstSynced;
    while( pPg && (pPg->needSync || pPg->pPager->iInUseDB) ){
      pPg = pPg->gfree.pNext;
    }
    if( !pPg ){
      pPg = sqlite3LruPageList.pFirst;
      while( pPg && pPg->pPager->iInUseDB ){
        pPg = pPg->gfree.pNext;
      }
    }
    sqlite3_mutex_leave(sqlite3_mutex_alloc(SQLITE_MUTEX_STATIC_LRU));

    /* If pPg==0, then the block above has failed to find a page to
    ** recycle. In this case return early - no further memory will
    ** be released.
    */
    if( !pPg ) break;

    pPager = pPg->pPager;
    assert(!pPg->needSync || pPg==pPager->lru.pFirst);
    assert(pPg->needSync || pPg==pPager->lru.pFirstSynced);
  
    savedBusy = pPager->pBusyHandler;
    pPager->pBusyHandler = 0;
    rc = pager_recycle(pPager, &pRecycled);
    pPager->pBusyHandler = savedBusy;
    assert(pRecycled==pPg || rc!=SQLITE_OK);
    if( rc==SQLITE_OK ){
      /* We've found a page to free. At this point the page has been 
      ** removed from the page hash-table, free-list and synced-list 
      ** (pFirstSynced). It is still in the all pages (pAll) list. 
      ** Remove it from this list before freeing.
      **
      ** Todo: Check the Pager.pStmt list to make sure this is Ok. It 
      ** probably is though.
      */
      PgHdr *pTmp;
      assert( pPg );
      if( pPg==pPager->pAll ){
         pPager->pAll = pPg->pNextAll;
      }else{
        for( pTmp=pPager->pAll; pTmp->pNextAll!=pPg; pTmp=pTmp->pNextAll ){}
        pTmp->pNextAll = pPg->pNextAll;
      }
      nReleased += (
          sizeof(*pPg) + pPager->pageSize
          + sizeof(u32) + pPager->nExtra
          + MEMDB*sizeof(PgHistory) 
      );
      IOTRACE(("PGFREE %p %d *\n", pPager, pPg->pgno));
      PAGER_INCR(sqlite3_pager_pgfree_count);
      sqlite3_free(pPg);
      pPager->nPage--;
    }else{
      /* An error occured whilst writing to the database file or 
      ** journal in pager_recycle(). The error is not returned to the 
      ** caller of this function. Instead, set the Pager.errCode variable.
      ** The error will be returned to the user (or users, in the case 
      ** of a shared pager cache) of the pager for which the error occured.
      */
      assert(
          (rc&0xff)==SQLITE_IOERR ||
          rc==SQLITE_FULL ||
          rc==SQLITE_BUSY
      );
      assert( pPager->state>=PAGER_RESERVED );
      pager_error(pPager, rc);
    }
  }

  /* Clear the memory management flags and release the mutex
  */
  for(pPager=sqlite3PagerList; pPager; pPager=pPager->pNext){
     pPager->iInUseMM = 0;
  }
  sqlite3_mutex_leave(mutex);

  /* Return the number of bytes released
  */
  return nReleased;
}
#endif /* SQLITE_ENABLE_MEMORY_MANAGEMENT */

/*
** Read the content of page pPg out of the database file.
*/
static int readDbPage(Pager *pPager, PgHdr *pPg, Pgno pgno){
  int rc;
  i64 offset;
  assert( MEMDB==0 );
  assert(pPager->fd->pMethods||pPager->tempFile);
  if( !pPager->fd->pMethods ){
    return SQLITE_IOERR_SHORT_READ;
  }
  offset = (pgno-1)*(i64)pPager->pageSize;
  rc = sqlite3OsRead(pPager->fd, PGHDR_TO_DATA(pPg), pPager->pageSize, offset);
  PAGER_INCR(sqlite3_pager_readdb_count);
  PAGER_INCR(pPager->nRead);
  IOTRACE(("PGIN %p %d\n", pPager, pgno));
  if( pgno==1 ){
    memcpy(&pPager->dbFileVers, &((u8*)PGHDR_TO_DATA(pPg))[24],
                                              sizeof(pPager->dbFileVers));
  }
  CODEC1(pPager, PGHDR_TO_DATA(pPg), pPg->pgno, 3);
  PAGERTRACE4("FETCH %d page %d hash(%08x)\n",
               PAGERID(pPager), pPg->pgno, pager_pagehash(pPg));
  return rc;
}


/*
** This function is called to obtain the shared lock required before
** data may be read from the pager cache. If the shared lock has already
** been obtained, this function is a no-op.
**
** Immediately after obtaining the shared lock (if required), this function
** checks for a hot-journal file. If one is found, an emergency rollback
** is performed immediately.
*/
static int pagerSharedLock(Pager *pPager){
  int rc = SQLITE_OK;
  int isHot = 0;

  /* If this database is opened for exclusive access, has no outstanding 
  ** page references and is in an error-state, now is the chance to clear
  ** the error. Discard the contents of the pager-cache and treat any
  ** open journal file as a hot-journal.
  */
  if( !MEMDB && pPager->exclusiveMode && pPager->nRef==0 && pPager->errCode ){
    if( pPager->journalOpen ){
      isHot = 1;
    }
    pager_reset(pPager);
    pPager->errCode = SQLITE_OK;
  }

  /* If the pager is still in an error state, do not proceed. The error 
  ** state will be cleared at some point in the future when all page 
  ** references are dropped and the cache can be discarded.
  */
  if( pPager->errCode && pPager->errCode!=SQLITE_FULL ){
    return pPager->errCode;
  }

  if( pPager->state==PAGER_UNLOCK || isHot ){
    sqlite3_vfs *pVfs = pPager->pVfs;
    if( !MEMDB ){
      assert( pPager->nRef==0 );
      if( !pPager->noReadlock ){
        rc = pager_wait_on_lock(pPager, SHARED_LOCK);
        if( rc!=SQLITE_OK ){
          return pager_error(pPager, rc);
        }
        assert( pPager->state>=SHARED_LOCK );
      }
  
      /* If a journal file exists, and there is no RESERVED lock on the
      ** database file, then it either needs to be played back or deleted.
      */
      if( hasHotJournal(pPager) || isHot ){
        /* Get an EXCLUSIVE lock on the database file. At this point it is
        ** important that a RESERVED lock is not obtained on the way to the
        ** EXCLUSIVE lock. If it were, another process might open the
        ** database file, detect the RESERVED lock, and conclude that the
        ** database is safe to read while this process is still rolling it 
        ** back.
        ** 
        ** Because the intermediate RESERVED lock is not requested, the
        ** second process will get to this point in the code and fail to
        ** obtain its own EXCLUSIVE lock on the database file.
        */
        if( pPager->state<EXCLUSIVE_LOCK ){
          rc = sqlite3OsLock(pPager->fd, EXCLUSIVE_LOCK);
          if( rc!=SQLITE_OK ){
            pager_unlock(pPager);
            return pager_error(pPager, rc);
          }
          pPager->state = PAGER_EXCLUSIVE;
        }
 
        /* Open the journal for reading only.  Return SQLITE_BUSY if
        ** we are unable to open the journal file. 
        **
        ** The journal file does not need to be locked itself.  The
        ** journal file is never open unless the main database file holds
        ** a write lock, so there is never any chance of two or more
        ** processes opening the journal at the same time.
        **
        ** Open the journal for read/write access. This is because in 
        ** exclusive-access mode the file descriptor will be kept open and
        ** possibly used for a transaction later on. On some systems, the
        ** OsTruncate() call used in exclusive-access mode also requires
        ** a read/write file handle.
        */
        if( !isHot ){
          rc = SQLITE_BUSY;
          if( sqlite3OsAccess(pVfs, pPager->zJournal, SQLITE_ACCESS_EXISTS) ){
            int fout = 0;
            int f = SQLITE_OPEN_READWRITE|SQLITE_OPEN_MAIN_JOURNAL;
            assert( !pPager->tempFile );
            rc = sqlite3OsOpen(pVfs, pPager->zJournal, pPager->jfd, f, &fout);
            assert( rc!=SQLITE_OK || pPager->jfd->pMethods );
            if( fout&SQLITE_OPEN_READONLY ){
              rc = SQLITE_BUSY;
              sqlite3OsClose(pPager->jfd);
            }
          }
        }
        if( rc!=SQLITE_OK ){
          pager_unlock(pPager);
          return ((rc==SQLITE_NOMEM||rc==SQLITE_IOERR_NOMEM)?rc:SQLITE_BUSY);
        }
        pPager->journalOpen = 1;
        pPager->journalStarted = 0;
        pPager->journalOff = 0;
        pPager->setMaster = 0;
        pPager->journalHdr = 0;
 
        /* Playback and delete the journal.  Drop the database write
        ** lock and reacquire the read lock.
        */
        rc = pager_playback(pPager, 1);
        if( rc!=SQLITE_OK ){
          return pager_error(pPager, rc);
        }
        assert(pPager->state==PAGER_SHARED || 
            (pPager->exclusiveMode && pPager->state>PAGER_SHARED)
        );
      }

      if( pPager->pAll ){
        /* The shared-lock has just been acquired on the database file
        ** and there are already pages in the cache (from a previous
        ** read or write transaction).  Check to see if the database
        ** has been modified.  If the database has changed, flush the
        ** cache.
        **
        ** Database changes is detected by looking at 15 bytes beginning
        ** at offset 24 into the file.  The first 4 of these 16 bytes are
        ** a 32-bit counter that is incremented with each change.  The
        ** other bytes change randomly with each file change when
        ** a codec is in use.
        ** 
        ** There is a vanishingly small chance that a change will not be 
        ** detected.  The chance of an undetected change is so small that
        ** it can be neglected.
        */
        char dbFileVers[sizeof(pPager->dbFileVers)];
        sqlite3PagerPagecount(pPager);

        if( pPager->errCode ){
          return pPager->errCode;
        }

        if( pPager->dbSize>0 ){
          IOTRACE(("CKVERS %p %d\n", pPager, sizeof(dbFileVers)));
          rc = sqlite3OsRead(pPager->fd, &dbFileVers, sizeof(dbFileVers), 24);
          if( rc!=SQLITE_OK ){
            return rc;
          }
        }else{
          memset(dbFileVers, 0, sizeof(dbFileVers));
        }

        if( memcmp(pPager->dbFileVers, dbFileVers, sizeof(dbFileVers))!=0 ){
          pager_reset(pPager);
        }
      }
    }
    assert( pPager->exclusiveMode || pPager->state<=PAGER_SHARED );
    if( pPager->state==PAGER_UNLOCK ){
      pPager->state = PAGER_SHARED;
    }
  }

  return rc;
}

/*
** Allocate a PgHdr object.   Either create a new one or reuse
** an existing one that is not otherwise in use.
**
** A new PgHdr structure is created if any of the following are
** true:
**
**     (1)  We have not exceeded our maximum allocated cache size
**          as set by the "PRAGMA cache_size" command.
**
**     (2)  There are no unused PgHdr objects available at this time.
**
**     (3)  This is an in-memory database.
**
**     (4)  There are no PgHdr objects that do not require a journal
**          file sync and a sync of the journal file is currently
**          prohibited.
**
** Otherwise, reuse an existing PgHdr.  In other words, reuse an
** existing PgHdr if all of the following are true:
**
**     (1)  We have reached or exceeded the maximum cache size
**          allowed by "PRAGMA cache_size".
**
**     (2)  There is a PgHdr available with PgHdr->nRef==0
**
**     (3)  We are not in an in-memory database
**
**     (4)  Either there is an available PgHdr that does not need
**          to be synced to disk or else disk syncing is currently
**          allowed.
*/
static int pagerAllocatePage(Pager *pPager, PgHdr **ppPg){
  int rc = SQLITE_OK;
  PgHdr *pPg;
  int nByteHdr;

  /* Create a new PgHdr if any of the four conditions defined 
  ** above are met: */
  if( pPager->nPage<pPager->mxPage
   || pPager->lru.pFirst==0 
   || MEMDB
   || (pPager->lru.pFirstSynced==0 && pPager->doNotSync)
  ){
    if( pPager->nPage>=pPager->nHash ){
      pager_resize_hash_table(pPager,
         pPager->nHash<256 ? 256 : pPager->nHash*2);
      if( pPager->nHash==0 ){
        rc = SQLITE_NOMEM;
        goto pager_allocate_out;
      }
    }
    pagerLeave(pPager);
    nByteHdr = sizeof(*pPg) + sizeof(u32) + pPager->nExtra
              + MEMDB*sizeof(PgHistory);
    pPg = sqlite3_malloc( nByteHdr + pPager->pageSize );
    pagerEnter(pPager);
    if( pPg==0 ){
      rc = SQLITE_NOMEM;
      goto pager_allocate_out;
    }
    memset(pPg, 0, nByteHdr);
    pPg->pData = (void*)(nByteHdr + (char*)pPg);
    pPg->pPager = pPager;
    pPg->pNextAll = pPager->pAll;
    pPager->pAll = pPg;
    pPager->nPage++;
  }else{
    /* Recycle an existing page with a zero ref-count. */
    rc = pager_recycle(pPager, &pPg);
    if( rc==SQLITE_BUSY ){
      rc = SQLITE_IOERR_BLOCKED;
    }
    if( rc!=SQLITE_OK ){
      goto pager_allocate_out;
    }
    assert( pPager->state>=SHARED_LOCK );
    assert(pPg);
  }
  *ppPg = pPg;

pager_allocate_out:
  return rc;
}

/*
** Make sure we have the content for a page.  If the page was
** previously acquired with noContent==1, then the content was
** just initialized to zeros instead of being read from disk.
** But now we need the real data off of disk.  So make sure we
** have it.  Read it in if we do not have it already.
*/
static int pager_get_content(PgHdr *pPg){
  if( pPg->needRead ){
    int rc = readDbPage(pPg->pPager, pPg, pPg->pgno);
    if( rc==SQLITE_OK ){
      pPg->needRead = 0;
    }else{
      return rc;
    }
  }
  return SQLITE_OK;
}

/*
** Acquire a page.
**
** A read lock on the disk file is obtained when the first page is acquired. 
** This read lock is dropped when the last page is released.
**
** This routine works for any page number greater than 0.  If the database
** file is smaller than the requested page, then no actual disk
** read occurs and the memory image of the page is initialized to
** all zeros.  The extra data appended to a page is always initialized
** to zeros the first time a page is loaded into memory.
**
** The acquisition might fail for several reasons.  In all cases,
** an appropriate error code is returned and *ppPage is set to NULL.
**
** See also sqlite3PagerLookup().  Both this routine and Lookup() attempt
** to find a page in the in-memory cache first.  If the page is not already
** in memory, this routine goes to disk to read it in whereas Lookup()
** just returns 0.  This routine acquires a read-lock the first time it
** has to go to disk, and could also playback an old journal if necessary.
** Since Lookup() never goes to disk, it never has to deal with locks
** or journal files.
**
** If noContent is false, the page contents are actually read from disk.
** If noContent is true, it means that we do not care about the contents
** of the page at this time, so do not do a disk read.  Just fill in the
** page content with zeros.  But mark the fact that we have not read the
** content by setting the PgHdr.needRead flag.  Later on, if 
** sqlite3PagerWrite() is called on this page or if this routine is
** called again with noContent==0, that means that the content is needed
** and the disk read should occur at that point.
*/
static int pagerAcquire(
  Pager *pPager,      /* The pager open on the database file */
  Pgno pgno,          /* Page number to fetch */
  DbPage **ppPage,    /* Write a pointer to the page here */
  int noContent       /* Do not bother reading content from disk if true */
){
  PgHdr *pPg;
  int rc;

  assert( pPager->state==PAGER_UNLOCK || pPager->nRef>0 || pgno==1 );

  /* The maximum page number is 2^31. Return SQLITE_CORRUPT if a page
  ** number greater than this, or zero, is requested.
  */
  if( pgno>PAGER_MAX_PGNO || pgno==0 || pgno==PAGER_MJ_PGNO(pPager) ){
    return SQLITE_CORRUPT_BKPT;
  }

  /* Make sure we have not hit any critical errors.
  */ 
  assert( pPager!=0 );
  *ppPage = 0;

  /* If this is the first page accessed, then get a SHARED lock
  ** on the database file. pagerSharedLock() is a no-op if 
  ** a database lock is already held.
  */
  rc = pagerSharedLock(pPager);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  assert( pPager->state!=PAGER_UNLOCK );

  pPg = pager_lookup(pPager, pgno);
  if( pPg==0 ){
    /* The requested page is not in the page cache. */
    int nMax;
    int h;
    PAGER_INCR(pPager->nMiss);
    rc = pagerAllocatePage(pPager, &pPg);
    if( rc!=SQLITE_OK ){
      return rc;
    }

    pPg->pgno = pgno;
    assert( !MEMDB || pgno>pPager->stmtSize );
    if( pPager->aInJournal && (int)pgno<=pPager->origDbSize ){
#if 0
      sqlite3CheckMemory(pPager->aInJournal, pgno/8);
#endif
      assert( pPager->journalOpen );
      pPg->inJournal = (pPager->aInJournal[pgno/8] & (1<<(pgno&7)))!=0;
      pPg->needSync = 0;
    }else{
      pPg->inJournal = 0;
      pPg->needSync = 0;
    }

    makeClean(pPg);
    pPg->nRef = 1;

    pPager->nRef++;
    if( pPager->nExtra>0 ){
      memset(PGHDR_TO_EXTRA(pPg, pPager), 0, pPager->nExtra);
    }
    nMax = sqlite3PagerPagecount(pPager);
    if( pPager->errCode ){
      rc = pPager->errCode;
      sqlite3PagerUnref(pPg);
      return rc;
    }

    /* Populate the page with data, either by reading from the database
    ** file, or by setting the entire page to zero.
    */
    if( nMax<(int)pgno || MEMDB || (noContent && !pPager->alwaysRollback) ){
      if( pgno>pPager->mxPgno ){
        sqlite3PagerUnref(pPg);
        return SQLITE_FULL;
      }
      memset(PGHDR_TO_DATA(pPg), 0, pPager->pageSize);
      pPg->needRead = noContent && !pPager->alwaysRollback;
      IOTRACE(("ZERO %p %d\n", pPager, pgno));
    }else{
      rc = readDbPage(pPager, pPg, pgno);
      if( rc!=SQLITE_OK && rc!=SQLITE_IOERR_SHORT_READ ){
        pPg->pgno = 0;
        sqlite3PagerUnref(pPg);
        return rc;
      }
      pPg->needRead = 0;
    }

    /* Link the page into the page hash table */
    h = pgno & (pPager->nHash-1);
    assert( pgno!=0 );
    pPg->pNextHash = pPager->aHash[h];
    pPager->aHash[h] = pPg;
    if( pPg->pNextHash ){
      assert( pPg->pNextHash->pPrevHash==0 );
      pPg->pNextHash->pPrevHash = pPg;
    }

#ifdef SQLITE_CHECK_PAGES
    pPg->pageHash = pager_pagehash(pPg);
#endif
  }else{
    /* The requested page is in the page cache. */
    assert(pPager->nRef>0 || pgno==1);
    PAGER_INCR(pPager->nHit);
    if( !noContent ){
      rc = pager_get_content(pPg);
      if( rc ){
        return rc;
      }
    }
    page_ref(pPg);
  }
  *ppPage = pPg;
  return SQLITE_OK;
}
int sqlite3PagerAcquire(
  Pager *pPager,      /* The pager open on the database file */
  Pgno pgno,          /* Page number to fetch */
  DbPage **ppPage,    /* Write a pointer to the page here */
  int noContent       /* Do not bother reading content from disk if true */
){
  int rc;
  pagerEnter(pPager);
  rc = pagerAcquire(pPager, pgno, ppPage, noContent);
  pagerLeave(pPager);
  return rc;
}


/*
** Acquire a page if it is already in the in-memory cache.  Do
** not read the page from disk.  Return a pointer to the page,
** or 0 if the page is not in cache.
**
** See also sqlite3PagerGet().  The difference between this routine
** and sqlite3PagerGet() is that _get() will go to the disk and read
** in the page if the page is not already in cache.  This routine
** returns NULL if the page is not in cache or if a disk I/O error 
** has ever happened.
*/
DbPage *sqlite3PagerLookup(Pager *pPager, Pgno pgno){
  PgHdr *pPg = 0;

  assert( pPager!=0 );
  assert( pgno!=0 );

  pagerEnter(pPager);
  if( pPager->state==PAGER_UNLOCK ){
    assert( !pPager->pAll || pPager->exclusiveMode );
  }else if( pPager->errCode && pPager->errCode!=SQLITE_FULL ){
    /* Do nothing */
  }else if( (pPg = pager_lookup(pPager, pgno))!=0 ){
    page_ref(pPg);
  }
  pagerLeave(pPager);
  return pPg;
}

/*
** Release a page.
**
** If the number of references to the page drop to zero, then the
** page is added to the LRU list.  When all references to all pages
** are released, a rollback occurs and the lock on the database is
** removed.
*/
int sqlite3PagerUnref(DbPage *pPg){
  Pager *pPager = pPg->pPager;

  /* Decrement the reference count for this page
  */
  assert( pPg->nRef>0 );
  pagerEnter(pPg->pPager);
  pPg->nRef--;

  CHECK_PAGE(pPg);

  /* When the number of references to a page reach 0, call the
  ** destructor and add the page to the freelist.
  */
  if( pPg->nRef==0 ){

    lruListAdd(pPg);
    if( pPager->xDestructor ){
      pPager->xDestructor(pPg, pPager->pageSize);
    }
  
    /* When all pages reach the freelist, drop the read lock from
    ** the database file.
    */
    pPager->nRef--;
    assert( pPager->nRef>=0 );
    if( pPager->nRef==0 && (!pPager->exclusiveMode || pPager->journalOff>0) ){
      pagerUnlockAndRollback(pPager);
    }
  }
  pagerLeave(pPager);
  return SQLITE_OK;
}

/*
** Create a journal file for pPager.  There should already be a RESERVED
** or EXCLUSIVE lock on the database file when this routine is called.
**
** Return SQLITE_OK if everything.  Return an error code and release the
** write lock if anything goes wrong.
*/
static int pager_open_journal(Pager *pPager){
  sqlite3_vfs *pVfs = pPager->pVfs;
  int flags = (SQLITE_OPEN_READWRITE|SQLITE_OPEN_EXCLUSIVE|SQLITE_OPEN_CREATE);

  int rc;
  assert( !MEMDB );
  assert( pPager->state>=PAGER_RESERVED );
  assert( pPager->journalOpen==0 );
  assert( pPager->useJournal );
  assert( pPager->aInJournal==0 );
  sqlite3PagerPagecount(pPager);
  pagerLeave(pPager);
  pPager->aInJournal = sqlite3MallocZero( pPager->dbSize/8 + 1 );
  pagerEnter(pPager);
  if( pPager->aInJournal==0 ){
    rc = SQLITE_NOMEM;
    goto failed_to_open_journal;
  }

  if( pPager->tempFile ){
    flags |= (SQLITE_OPEN_DELETEONCLOSE|SQLITE_OPEN_TEMP_JOURNAL);
  }else{
    flags |= (SQLITE_OPEN_MAIN_JOURNAL);
  }
#ifdef SQLITE_ENABLE_ATOMIC_WRITE
  rc = sqlite3JournalOpen(
      pVfs, pPager->zJournal, pPager->jfd, flags, jrnlBufferSize(pPager)
  );
#else
  rc = sqlite3OsOpen(pVfs, pPager->zJournal, pPager->jfd, flags, 0);
#endif
  assert( rc!=SQLITE_OK || pPager->jfd->pMethods );
  pPager->journalOff = 0;
  pPager->setMaster = 0;
  pPager->journalHdr = 0;
  if( rc!=SQLITE_OK ){
    if( rc==SQLITE_NOMEM ){
      sqlite3OsDelete(pVfs, pPager->zJournal, 0);
    }
    goto failed_to_open_journal;
  }
  pPager->journalOpen = 1;
  pPager->journalStarted = 0;
  pPager->needSync = 0;
  pPager->alwaysRollback = 0;
  pPager->nRec = 0;
  if( pPager->errCode ){
    rc = pPager->errCode;
    goto failed_to_open_journal;
  }
  pPager->origDbSize = pPager->dbSize;

  rc = writeJournalHdr(pPager);

  if( pPager->stmtAutoopen && rc==SQLITE_OK ){
    rc = sqlite3PagerStmtBegin(pPager);
  }
  if( rc!=SQLITE_OK && rc!=SQLITE_NOMEM && rc!=SQLITE_IOERR_NOMEM ){
    rc = pager_end_transaction(pPager);
    if( rc==SQLITE_OK ){
      rc = SQLITE_FULL;
    }
  }
  return rc;

failed_to_open_journal:
  sqlite3_free(pPager->aInJournal);
  pPager->aInJournal = 0;
  return rc;
}

/*
** Acquire a write-lock on the database.  The lock is removed when
** the any of the following happen:
**
**   *  sqlite3PagerCommitPhaseTwo() is called.
**   *  sqlite3PagerRollback() is called.
**   *  sqlite3PagerClose() is called.
**   *  sqlite3PagerUnref() is called to on every outstanding page.
**
** The first parameter to this routine is a pointer to any open page of the
** database file.  Nothing changes about the page - it is used merely to
** acquire a pointer to the Pager structure and as proof that there is
** already a read-lock on the database.
**
** The second parameter indicates how much space in bytes to reserve for a
** master journal file-name at the start of the journal when it is created.
**
** A journal file is opened if this is not a temporary file.  For temporary
** files, the opening of the journal file is deferred until there is an
** actual need to write to the journal.
**
** If the database is already reserved for writing, this routine is a no-op.
**
** If exFlag is true, go ahead and get an EXCLUSIVE lock on the file
** immediately instead of waiting until we try to flush the cache.  The
** exFlag is ignored if a transaction is already active.
*/
int sqlite3PagerBegin(DbPage *pPg, int exFlag){
  Pager *pPager = pPg->pPager;
  int rc = SQLITE_OK;
  pagerEnter(pPager);
  assert( pPg->nRef>0 );
  assert( pPager->state!=PAGER_UNLOCK );
  if( pPager->state==PAGER_SHARED ){
    assert( pPager->aInJournal==0 );
    if( MEMDB ){
      pPager->state = PAGER_EXCLUSIVE;
      pPager->origDbSize = pPager->dbSize;
    }else{
      rc = sqlite3OsLock(pPager->fd, RESERVED_LOCK);
      if( rc==SQLITE_OK ){
        pPager->state = PAGER_RESERVED;
        if( exFlag ){
          rc = pager_wait_on_lock(pPager, EXCLUSIVE_LOCK);
        }
      }
      if( rc!=SQLITE_OK ){
        pagerLeave(pPager);
        return rc;
      }
      pPager->dirtyCache = 0;
      PAGERTRACE2("TRANSACTION %d\n", PAGERID(pPager));
      if( pPager->useJournal && !pPager->tempFile ){
        rc = pager_open_journal(pPager);
      }
    }
  }else if( pPager->journalOpen && pPager->journalOff==0 ){
    /* This happens when the pager was in exclusive-access mode last
    ** time a (read or write) transaction was successfully concluded
    ** by this connection. Instead of deleting the journal file it was 
    ** kept open and truncated to 0 bytes.
    */
    assert( pPager->nRec==0 );
    assert( pPager->origDbSize==0 );
    assert( pPager->aInJournal==0 );
    sqlite3PagerPagecount(pPager);
    pagerLeave(pPager);
    pPager->aInJournal = sqlite3MallocZero( pPager->dbSize/8 + 1 );
    pagerEnter(pPager);
    if( !pPager->aInJournal ){
      rc = SQLITE_NOMEM;
    }else{
      pPager->origDbSize = pPager->dbSize;
      rc = writeJournalHdr(pPager);
    }
  }
  assert( !pPager->journalOpen || pPager->journalOff>0 || rc!=SQLITE_OK );
  pagerLeave(pPager);
  return rc;
}

/*
** Make a page dirty.  Set its dirty flag and add it to the dirty
** page list.
*/
static void makeDirty(PgHdr *pPg){
  if( pPg->dirty==0 ){
    Pager *pPager = pPg->pPager;
    pPg->dirty = 1;
    pPg->pDirty = pPager->pDirty;
    if( pPager->pDirty ){
      pPager->pDirty->pPrevDirty = pPg;
    }
    pPg->pPrevDirty = 0;
    pPager->pDirty = pPg;
  }
}

/*
** Make a page clean.  Clear its dirty bit and remove it from the
** dirty page list.
*/
static void makeClean(PgHdr *pPg){
  if( pPg->dirty ){
    pPg->dirty = 0;
    if( pPg->pDirty ){
      assert( pPg->pDirty->pPrevDirty==pPg );
      pPg->pDirty->pPrevDirty = pPg->pPrevDirty;
    }
    if( pPg->pPrevDirty ){
      assert( pPg->pPrevDirty->pDirty==pPg );
      pPg->pPrevDirty->pDirty = pPg->pDirty;
    }else{
      assert( pPg->pPager->pDirty==pPg );
      pPg->pPager->pDirty = pPg->pDirty;
    }
  }
}


/*
** Mark a data page as writeable.  The page is written into the journal 
** if it is not there already.  This routine must be called before making
** changes to a page.
**
** The first time this routine is called, the pager creates a new
** journal and acquires a RESERVED lock on the database.  If the RESERVED
** lock could not be acquired, this routine returns SQLITE_BUSY.  The
** calling routine must check for that return value and be careful not to
** change any page data until this routine returns SQLITE_OK.
**
** If the journal file could not be written because the disk is full,
** then this routine returns SQLITE_FULL and does an immediate rollback.
** All subsequent write attempts also return SQLITE_FULL until there
** is a call to sqlite3PagerCommit() or sqlite3PagerRollback() to
** reset.
*/
static int pager_write(PgHdr *pPg){
  void *pData = PGHDR_TO_DATA(pPg);
  Pager *pPager = pPg->pPager;
  int rc = SQLITE_OK;

  /* Check for errors
  */
  if( pPager->errCode ){ 
    return pPager->errCode;
  }
  if( pPager->readOnly ){
    return SQLITE_PERM;
  }

  assert( !pPager->setMaster );

  CHECK_PAGE(pPg);

  /* If this page was previously acquired with noContent==1, that means
  ** we didn't really read in the content of the page.  This can happen
  ** (for example) when the page is being moved to the freelist.  But
  ** now we are (perhaps) moving the page off of the freelist for
  ** reuse and we need to know its original content so that content
  ** can be stored in the rollback journal.  So do the read at this
  ** time.
  */
  rc = pager_get_content(pPg);
  if( rc ){
    return rc;
  }

  /* Mark the page as dirty.  If the page has already been written
  ** to the journal then we can return right away.
  */
  makeDirty(pPg);
  if( pPg->inJournal && (pageInStatement(pPg) || pPager->stmtInUse==0) ){
    pPager->dirtyCache = 1;
  }else{

    /* If we get this far, it means that the page needs to be
    ** written to the transaction journal or the ckeckpoint journal
    ** or both.
    **
    ** First check to see that the transaction journal exists and
    ** create it if it does not.
    */
    assert( pPager->state!=PAGER_UNLOCK );
    rc = sqlite3PagerBegin(pPg, 0);
    if( rc!=SQLITE_OK ){
      return rc;
    }
    assert( pPager->state>=PAGER_RESERVED );
    if( !pPager->journalOpen && pPager->useJournal ){
      rc = pager_open_journal(pPager);
      if( rc!=SQLITE_OK ) return rc;
    }
    assert( pPager->journalOpen || !pPager->useJournal );
    pPager->dirtyCache = 1;
  
    /* The transaction journal now exists and we have a RESERVED or an
    ** EXCLUSIVE lock on the main database file.  Write the current page to
    ** the transaction journal if it is not there already.
    */
    if( !pPg->inJournal && (pPager->useJournal || MEMDB) ){
      if( (int)pPg->pgno <= pPager->origDbSize ){
        if( MEMDB ){
          PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
          PAGERTRACE3("JOURNAL %d page %d\n", PAGERID(pPager), pPg->pgno);
          assert( pHist->pOrig==0 );
          pHist->pOrig = sqlite3_malloc( pPager->pageSize );
          if( !pHist->pOrig ){
            return SQLITE_NOMEM;
          }
          memcpy(pHist->pOrig, PGHDR_TO_DATA(pPg), pPager->pageSize);
        }else{
          u32 cksum;
          char *pData2;

          /* We should never write to the journal file the page that
          ** contains the database locks.  The following assert verifies
          ** that we do not. */
          assert( pPg->pgno!=PAGER_MJ_PGNO(pPager) );
          pData2 = CODEC2(pPager, pData, pPg->pgno, 7);
          cksum = pager_cksum(pPager, (u8*)pData2);
          rc = write32bits(pPager->jfd, pPager->journalOff, pPg->pgno);
          if( rc==SQLITE_OK ){
            rc = sqlite3OsWrite(pPager->jfd, pData2, pPager->pageSize,
                                pPager->journalOff + 4);
            pPager->journalOff += pPager->pageSize+4;
          }
          if( rc==SQLITE_OK ){
            rc = write32bits(pPager->jfd, pPager->journalOff, cksum);
            pPager->journalOff += 4;
          }
          IOTRACE(("JOUT %p %d %lld %d\n", pPager, pPg->pgno, 
                   pPager->journalOff, pPager->pageSize));
          PAGER_INCR(sqlite3_pager_writej_count);
          PAGERTRACE5("JOURNAL %d page %d needSync=%d hash(%08x)\n",
               PAGERID(pPager), pPg->pgno, pPg->needSync, pager_pagehash(pPg));

          /* An error has occured writing to the journal file. The 
          ** transaction will be rolled back by the layer above.
          */
          if( rc!=SQLITE_OK ){
            return rc;
          }

          pPager->nRec++;
          assert( pPager->aInJournal!=0 );
          pPager->aInJournal[pPg->pgno/8] |= 1<<(pPg->pgno&7);
          pPg->needSync = !pPager->noSync;
          if( pPager->stmtInUse ){
            pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
          }
        }
      }else{
        pPg->needSync = !pPager->journalStarted && !pPager->noSync;
        PAGERTRACE4("APPEND %d page %d needSync=%d\n",
                PAGERID(pPager), pPg->pgno, pPg->needSync);
      }
      if( pPg->needSync ){
        pPager->needSync = 1;
      }
      pPg->inJournal = 1;
    }
  
    /* If the statement journal is open and the page is not in it,
    ** then write the current page to the statement journal.  Note that
    ** the statement journal format differs from the standard journal format
    ** in that it omits the checksums and the header.
    */
    if( pPager->stmtInUse 
     && !pageInStatement(pPg) 
     && (int)pPg->pgno<=pPager->stmtSize 
    ){
      assert( pPg->inJournal || (int)pPg->pgno>pPager->origDbSize );
      if( MEMDB ){
        PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
        assert( pHist->pStmt==0 );
        pHist->pStmt = sqlite3_malloc( pPager->pageSize );
        if( pHist->pStmt ){
          memcpy(pHist->pStmt, PGHDR_TO_DATA(pPg), pPager->pageSize);
        }
        PAGERTRACE3("STMT-JOURNAL %d page %d\n", PAGERID(pPager), pPg->pgno);
        page_add_to_stmt_list(pPg);
      }else{
        i64 offset = pPager->stmtNRec*(4+pPager->pageSize);
        char *pData2 = CODEC2(pPager, pData, pPg->pgno, 7);
        rc = write32bits(pPager->stfd, offset, pPg->pgno);
        if( rc==SQLITE_OK ){
          rc = sqlite3OsWrite(pPager->stfd, pData2, pPager->pageSize, offset+4);
        }
        PAGERTRACE3("STMT-JOURNAL %d page %d\n", PAGERID(pPager), pPg->pgno);
        if( rc!=SQLITE_OK ){
          return rc;
        }
        pPager->stmtNRec++;
        assert( pPager->aInStmt!=0 );
        pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
      }
    }
  }

  /* Update the database size and return.
  */
  assert( pPager->state>=PAGER_SHARED );
  if( pPager->dbSize<(int)pPg->pgno ){
    pPager->dbSize = pPg->pgno;
    if( !MEMDB && pPager->dbSize==PENDING_BYTE/pPager->pageSize ){
      pPager->dbSize++;
    }
  }
  return rc;
}

/*
** This function is used to mark a data-page as writable. It uses 
** pager_write() to open a journal file (if it is not already open)
** and write the page *pData to the journal.
**
** The difference between this function and pager_write() is that this
** function also deals with the special case where 2 or more pages
** fit on a single disk sector. In this case all co-resident pages
** must have been written to the journal file before returning.
*/
int sqlite3PagerWrite(DbPage *pDbPage){
  int rc = SQLITE_OK;

  PgHdr *pPg = pDbPage;
  Pager *pPager = pPg->pPager;
  Pgno nPagePerSector = (pPager->sectorSize/pPager->pageSize);

  pagerEnter(pPager);
  if( !MEMDB && nPagePerSector>1 ){
    Pgno nPageCount;          /* Total number of pages in database file */
    Pgno pg1;                 /* First page of the sector pPg is located on. */
    int nPage;                /* Number of pages starting at pg1 to journal */
    int ii;
    int needSync = 0;

    /* Set the doNotSync flag to 1. This is because we cannot allow a journal
    ** header to be written between the pages journaled by this function.
    */
    assert( pPager->doNotSync==0 );
    pPager->doNotSync = 1;

    /* This trick assumes that both the page-size and sector-size are
    ** an integer power of 2. It sets variable pg1 to the identifier
    ** of the first page of the sector pPg is located on.
    */
    pg1 = ((pPg->pgno-1) & ~(nPagePerSector-1)) + 1;

    nPageCount = sqlite3PagerPagecount(pPager);
    if( pPg->pgno>nPageCount ){
      nPage = (pPg->pgno - pg1)+1;
    }else if( (pg1+nPagePerSector-1)>nPageCount ){
      nPage = nPageCount+1-pg1;
    }else{
      nPage = nPagePerSector;
    }
    assert(nPage>0);
    assert(pg1<=pPg->pgno);
    assert((pg1+nPage)>pPg->pgno);

    for(ii=0; ii<nPage && rc==SQLITE_OK; ii++){
      Pgno pg = pg1+ii;
      PgHdr *pPage;
      if( !pPager->aInJournal || pg==pPg->pgno || 
          pg>pPager->origDbSize || !(pPager->aInJournal[pg/8]&(1<<(pg&7)))
      ) {
        if( pg!=PAGER_MJ_PGNO(pPager) ){
          rc = sqlite3PagerGet(pPager, pg, &pPage);
          if( rc==SQLITE_OK ){
            rc = pager_write(pPage);
            if( pPage->needSync ){
              needSync = 1;
            }
            sqlite3PagerUnref(pPage);
          }
        }
      }else if( (pPage = pager_lookup(pPager, pg)) ){
        if( pPage->needSync ){
          needSync = 1;
        }
      }
    }

    /* If the PgHdr.needSync flag is set for any of the nPage pages 
    ** starting at pg1, then it needs to be set for all of them. Because
    ** writing to any of these nPage pages may damage the others, the
    ** journal file must contain sync()ed copies of all of them
    ** before any of them can be written out to the database file.
    */
    if( needSync ){
      for(ii=0; ii<nPage && needSync; ii++){
        PgHdr *pPage = pager_lookup(pPager, pg1+ii);
        if( pPage ) pPage->needSync = 1;
      }
      assert(pPager->needSync);
    }

    assert( pPager->doNotSync==1 );
    pPager->doNotSync = 0;
  }else{
    rc = pager_write(pDbPage);
  }
  pagerLeave(pPager);
  return rc;
}

/*
** Return TRUE if the page given in the argument was previously passed
** to sqlite3PagerWrite().  In other words, return TRUE if it is ok
** to change the content of the page.
*/
#ifndef NDEBUG
int sqlite3PagerIswriteable(DbPage *pPg){
  return pPg->dirty;
}
#endif

#ifndef SQLITE_OMIT_VACUUM
/*
** Replace the content of a single page with the information in the third
** argument.
*/
int sqlite3PagerOverwrite(Pager *pPager, Pgno pgno, void *pData){
  PgHdr *pPg;
  int rc;

  pagerEnter(pPager);
  rc = sqlite3PagerGet(pPager, pgno, &pPg);
  if( rc==SQLITE_OK ){
    rc = sqlite3PagerWrite(pPg);
    if( rc==SQLITE_OK ){
      memcpy(sqlite3PagerGetData(pPg), pData, pPager->pageSize);
    }
    sqlite3PagerUnref(pPg);
  }
  pagerLeave(pPager);
  return rc;
}
#endif

/*
** A call to this routine tells the pager that it is not necessary to
** write the information on page pPg back to the disk, even though
** that page might be marked as dirty.
**
** The overlying software layer calls this routine when all of the data
** on the given page is unused.  The pager marks the page as clean so
** that it does not get written to disk.
**
** Tests show that this optimization, together with the
** sqlite3PagerDontRollback() below, more than double the speed
** of large INSERT operations and quadruple the speed of large DELETEs.
**
** When this routine is called, set the alwaysRollback flag to true.
** Subsequent calls to sqlite3PagerDontRollback() for the same page
** will thereafter be ignored.  This is necessary to avoid a problem
** where a page with data is added to the freelist during one part of
** a transaction then removed from the freelist during a later part
** of the same transaction and reused for some other purpose.  When it
** is first added to the freelist, this routine is called.  When reused,
** the sqlite3PagerDontRollback() routine is called.  But because the
** page contains critical data, we still need to be sure it gets
** rolled back in spite of the sqlite3PagerDontRollback() call.
*/
void sqlite3PagerDontWrite(DbPage *pDbPage){
  PgHdr *pPg = pDbPage;
  Pager *pPager = pPg->pPager;

  if( MEMDB ) return;
  pagerEnter(pPager);
  pPg->alwaysRollback = 1;
  if( pPg->dirty && !pPager->stmtInUse ){
    assert( pPager->state>=PAGER_SHARED );
    if( pPager->dbSize==(int)pPg->pgno && pPager->origDbSize<pPager->dbSize ){
      /* If this pages is the last page in the file and the file has grown
      ** during the current transaction, then do NOT mark the page as clean.
      ** When the database file grows, we must make sure that the last page
      ** gets written at least once so that the disk file will be the correct
      ** size. If you do not write this page and the size of the file
      ** on the disk ends up being too small, that can lead to database
      ** corruption during the next transaction.
      */
    }else{
      PAGERTRACE3("DONT_WRITE page %d of %d\n", pPg->pgno, PAGERID(pPager));
      IOTRACE(("CLEAN %p %d\n", pPager, pPg->pgno))
      makeClean(pPg);
#ifdef SQLITE_CHECK_PAGES
      pPg->pageHash = pager_pagehash(pPg);
#endif
    }
  }
  pagerLeave(pPager);
}

/*
** A call to this routine tells the pager that if a rollback occurs,
** it is not necessary to restore the data on the given page.  This
** means that the pager does not have to record the given page in the
** rollback journal.
**
** If we have not yet actually read the content of this page (if
** the PgHdr.needRead flag is set) then this routine acts as a promise
** that we will never need to read the page content in the future.
** so the needRead flag can be cleared at this point.
**
** This routine is only called from a single place in the sqlite btree
** code (when a leaf is removed from the free-list). This allows the
** following assumptions to be made about pPg:
**
**   1. PagerDontWrite() has been called on the page, OR 
**      PagerWrite() has not yet been called on the page.
**
**   2. The page existed when the transaction was started.
**
** Details: DontRollback() (this routine) is only called when a leaf is
** removed from the free list. DontWrite() is called whenever a page 
** becomes a free-list leaf.
*/
void sqlite3PagerDontRollback(DbPage *pPg){
  Pager *pPager = pPg->pPager;

  pagerEnter(pPager);
  assert( pPager->state>=PAGER_RESERVED );

  /* If the journal file is not open, or DontWrite() has been called on
  ** this page (DontWrite() sets the alwaysRollback flag), then this
  ** function is a no-op.
  */
  if( pPager->journalOpen==0 || pPg->alwaysRollback || pPager->alwaysRollback ){
    pagerLeave(pPager);
    return;
  }
  assert( !MEMDB );    /* For a memdb, pPager->journalOpen is always 0 */

  /* Check that PagerWrite() has not yet been called on this page, and
  ** that the page existed when the transaction started.
  */
  assert( !pPg->inJournal && (int)pPg->pgno <= pPager->origDbSize );

  assert( pPager->aInJournal!=0 );
  pPager->aInJournal[pPg->pgno/8] |= 1<<(pPg->pgno&7);
  pPg->inJournal = 1;
  pPg->needRead = 0;
  if( pPager->stmtInUse ){
    assert( pPager->stmtSize <= pPager->origDbSize );
    pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
  }
  PAGERTRACE3("DONT_ROLLBACK page %d of %d\n", pPg->pgno, PAGERID(pPager));
  IOTRACE(("GARBAGE %p %d\n", pPager, pPg->pgno))
  pagerLeave(pPager);
}


/*
** This routine is called to increment the database file change-counter,
** stored at byte 24 of the pager file.
*/
static int pager_incr_changecounter(Pager *pPager, int isDirect){
  PgHdr *pPgHdr;
  u32 change_counter;
  int rc = SQLITE_OK;

  if( !pPager->changeCountDone ){
    /* Open page 1 of the file for writing. */
    rc = sqlite3PagerGet(pPager, 1, &pPgHdr);
    if( rc!=SQLITE_OK ) return rc;

    if( !isDirect ){
      rc = sqlite3PagerWrite(pPgHdr);
      if( rc!=SQLITE_OK ){
        sqlite3PagerUnref(pPgHdr);
        return rc;
      }
    }

    /* Increment the value just read and write it back to byte 24. */
    change_counter = sqlite3Get4byte((u8*)pPager->dbFileVers);
    change_counter++;
    put32bits(((char*)PGHDR_TO_DATA(pPgHdr))+24, change_counter);

    if( isDirect && pPager->fd->pMethods ){
      const void *zBuf = PGHDR_TO_DATA(pPgHdr);
      rc = sqlite3OsWrite(pPager->fd, zBuf, pPager->pageSize, 0);
    }

    /* Release the page reference. */
    sqlite3PagerUnref(pPgHdr);
    pPager->changeCountDone = 1;
  }
  return rc;
}

/*
** Sync the database file for the pager pPager. zMaster points to the name
** of a master journal file that should be written into the individual
** journal file. zMaster may be NULL, which is interpreted as no master
** journal (a single database transaction).
**
** This routine ensures that the journal is synced, all dirty pages written
** to the database file and the database file synced. The only thing that
** remains to commit the transaction is to delete the journal file (or
** master journal file if specified).
**
** Note that if zMaster==NULL, this does not overwrite a previous value
** passed to an sqlite3PagerCommitPhaseOne() call.
**
** If parameter nTrunc is non-zero, then the pager file is truncated to
** nTrunc pages (this is used by auto-vacuum databases).
*/
int sqlite3PagerCommitPhaseOne(Pager *pPager, const char *zMaster, Pgno nTrunc){
  int rc = SQLITE_OK;

  PAGERTRACE4("DATABASE SYNC: File=%s zMaster=%s nTrunc=%d\n", 
      pPager->zFilename, zMaster, nTrunc);
  pagerEnter(pPager);

  /* If this is an in-memory db, or no pages have been written to, or this
  ** function has already been called, it is a no-op.
  */
  if( pPager->state!=PAGER_SYNCED && !MEMDB && pPager->dirtyCache ){
    PgHdr *pPg;

#ifdef SQLITE_ENABLE_ATOMIC_WRITE
    /* The atomic-write optimization can be used if all of the
    ** following are true:
    **
    **    + The file-system supports the atomic-write property for
    **      blocks of size page-size, and
    **    + This commit is not part of a multi-file transaction, and
    **    + Exactly one page has been modified and store in the journal file.
    **
    ** If the optimization can be used, then the journal file will never
    ** be created for this transaction.
    */
    int useAtomicWrite = (
        !zMaster && 
        pPager->journalOff==jrnlBufferSize(pPager) && 
        nTrunc==0 && 
        (0==pPager->pDirty || 0==pPager->pDirty->pDirty)
    );
    if( useAtomicWrite ){
      /* Update the nRec field in the journal file. */
      int offset = pPager->journalHdr + sizeof(aJournalMagic);
      assert(pPager->nRec==1);
      rc = write32bits(pPager->jfd, offset, pPager->nRec);

      /* Update the db file change counter. The following call will modify
      ** the in-memory representation of page 1 to include the updated
      ** change counter and then write page 1 directly to the database
      ** file. Because of the atomic-write property of the host file-system, 
      ** this is safe.
      */
      if( rc==SQLITE_OK ){
        rc = pager_incr_changecounter(pPager, 1);
      }
    }else{
      rc = sqlite3JournalCreate(pPager->jfd);
    }

    if( !useAtomicWrite && rc==SQLITE_OK )
#endif

    /* If a master journal file name has already been written to the
    ** journal file, then no sync is required. This happens when it is
    ** written, then the process fails to upgrade from a RESERVED to an
    ** EXCLUSIVE lock. The next time the process tries to commit the
    ** transaction the m-j name will have already been written.
    */
    if( !pPager->setMaster ){
      assert( pPager->journalOpen );
      rc = pager_incr_changecounter(pPager, 0);
      if( rc!=SQLITE_OK ) goto sync_exit;
#ifndef SQLITE_OMIT_AUTOVACUUM
      if( nTrunc!=0 ){
        /* If this transaction has made the database smaller, then all pages
        ** being discarded by the truncation must be written to the journal
        ** file.
        */
        Pgno i;
        int iSkip = PAGER_MJ_PGNO(pPager);
        for( i=nTrunc+1; i<=pPager->origDbSize; i++ ){
          if( !(pPager->aInJournal[i/8] & (1<<(i&7))) && i!=iSkip ){
            rc = sqlite3PagerGet(pPager, i, &pPg);
            if( rc!=SQLITE_OK ) goto sync_exit;
            rc = sqlite3PagerWrite(pPg);
            sqlite3PagerUnref(pPg);
            if( rc!=SQLITE_OK ) goto sync_exit;
          }
        } 
      }
#endif
      rc = writeMasterJournal(pPager, zMaster);
      if( rc!=SQLITE_OK ) goto sync_exit;
      rc = syncJournal(pPager);
    }
    if( rc!=SQLITE_OK ) goto sync_exit;

#ifndef SQLITE_OMIT_AUTOVACUUM
    if( nTrunc!=0 ){
      rc = sqlite3PagerTruncate(pPager, nTrunc);
      if( rc!=SQLITE_OK ) goto sync_exit;
    }
#endif

    /* Write all dirty pages to the database file */
    pPg = pager_get_all_dirty_pages(pPager);
    rc = pager_write_pagelist(pPg);
    if( rc!=SQLITE_OK ){
      while( pPg && !pPg->dirty ){ pPg = pPg->pDirty; }
      pPager->pDirty = pPg;
      goto sync_exit;
    }
    pPager->pDirty = 0;

    /* Sync the database file. */
    if( !pPager->noSync ){
      rc = sqlite3OsSync(pPager->fd, pPager->sync_flags);
    }
    IOTRACE(("DBSYNC %p\n", pPager))

    pPager->state = PAGER_SYNCED;
  }else if( MEMDB && nTrunc!=0 ){
    rc = sqlite3PagerTruncate(pPager, nTrunc);
  }

sync_exit:
  if( rc==SQLITE_IOERR_BLOCKED ){
    /* pager_incr_changecounter() may attempt to obtain an exclusive
     * lock to spill the cache and return IOERR_BLOCKED. But since 
     * there is no chance the cache is inconsistent, it is
     * better to return SQLITE_BUSY.
     */
    rc = SQLITE_BUSY;
  }
  pagerLeave(pPager);
  return rc;
}


/*
** Commit all changes to the database and release the write lock.
**
** If the commit fails for any reason, a rollback attempt is made
** and an error code is returned.  If the commit worked, SQLITE_OK
** is returned.
*/
int sqlite3PagerCommitPhaseTwo(Pager *pPager){
  int rc;
  PgHdr *pPg;

  if( pPager->errCode ){
    return pPager->errCode;
  }
  if( pPager->state<PAGER_RESERVED ){
    return SQLITE_ERROR;
  }
  pagerEnter(pPager);
  PAGERTRACE2("COMMIT %d\n", PAGERID(pPager));
  if( MEMDB ){
    pPg = pager_get_all_dirty_pages(pPager);
    while( pPg ){
      PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
      clearHistory(pHist);
      pPg->dirty = 0;
      pPg->inJournal = 0;
      pHist->inStmt = 0;
      pPg->needSync = 0;
      pHist->pPrevStmt = pHist->pNextStmt = 0;
      pPg = pPg->pDirty;
    }
    pPager->pDirty = 0;
#ifndef NDEBUG
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
      assert( !pPg->alwaysRollback );
      assert( !pHist->pOrig );
      assert( !pHist->pStmt );
    }
#endif
    pPager->pStmt = 0;
    pPager->state = PAGER_SHARED;
    pagerLeave(pPager);
    return SQLITE_OK;
  }
  assert( pPager->journalOpen || !pPager->dirtyCache );
  assert( pPager->state==PAGER_SYNCED || !pPager->dirtyCache );
  rc = pager_end_transaction(pPager);
  rc = pager_error(pPager, rc);
  pagerLeave(pPager);
  return rc;
}

/*
** Rollback all changes.  The database falls back to PAGER_SHARED mode.
** All in-memory cache pages revert to their original data contents.
** The journal is deleted.
**
** This routine cannot fail unless some other process is not following
** the correct locking protocol or unless some other
** process is writing trash into the journal file (SQLITE_CORRUPT) or
** unless a prior malloc() failed (SQLITE_NOMEM).  Appropriate error
** codes are returned for all these occasions.  Otherwise,
** SQLITE_OK is returned.
*/
int sqlite3PagerRollback(Pager *pPager){
  int rc;
  PAGERTRACE2("ROLLBACK %d\n", PAGERID(pPager));
  if( MEMDB ){
    PgHdr *p;
    for(p=pPager->pAll; p; p=p->pNextAll){
      PgHistory *pHist;
      assert( !p->alwaysRollback );
      if( !p->dirty ){
        assert( !((PgHistory *)PGHDR_TO_HIST(p, pPager))->pOrig );
        assert( !((PgHistory *)PGHDR_TO_HIST(p, pPager))->pStmt );
        continue;
      }

      pHist = PGHDR_TO_HIST(p, pPager);
      if( pHist->pOrig ){
        memcpy(PGHDR_TO_DATA(p), pHist->pOrig, pPager->pageSize);
        PAGERTRACE3("ROLLBACK-PAGE %d of %d\n", p->pgno, PAGERID(pPager));
      }else{
        PAGERTRACE3("PAGE %d is clean on %d\n", p->pgno, PAGERID(pPager));
      }
      clearHistory(pHist);
      p->dirty = 0;
      p->inJournal = 0;
      pHist->inStmt = 0;
      pHist->pPrevStmt = pHist->pNextStmt = 0;
      if( pPager->xReiniter ){
        pPager->xReiniter(p, pPager->pageSize);
      }
    }
    pPager->pDirty = 0;
    pPager->pStmt = 0;
    pPager->dbSize = pPager->origDbSize;
    pager_truncate_cache(pPager);
    pPager->stmtInUse = 0;
    pPager->state = PAGER_SHARED;
    return SQLITE_OK;
  }

  pagerEnter(pPager);
  if( !pPager->dirtyCache || !pPager->journalOpen ){
    rc = pager_end_transaction(pPager);
    pagerLeave(pPager);
    return rc;
  }

  if( pPager->errCode && pPager->errCode!=SQLITE_FULL ){
    if( pPager->state>=PAGER_EXCLUSIVE ){
      pager_playback(pPager, 0);
    }
    pagerLeave(pPager);
    return pPager->errCode;
  }
  if( pPager->state==PAGER_RESERVED ){
    int rc2;
    rc = pager_playback(pPager, 0);
    rc2 = pager_end_transaction(pPager);
    if( rc==SQLITE_OK ){
      rc = rc2;
    }
  }else{
    rc = pager_playback(pPager, 0);
  }
  /* pager_reset(pPager); */
  pPager->dbSize = -1;

  /* If an error occurs during a ROLLBACK, we can no longer trust the pager
  ** cache. So call pager_error() on the way out to make any error 
  ** persistent.
  */
  rc = pager_error(pPager, rc);
  pagerLeave(pPager);
  return rc;
}

/*
** Return TRUE if the database file is opened read-only.  Return FALSE
** if the database is (in theory) writable.
*/
int sqlite3PagerIsreadonly(Pager *pPager){
  return pPager->readOnly;
}

/*
** Return the number of references to the pager.
*/
int sqlite3PagerRefcount(Pager *pPager){
  return pPager->nRef;
}

#ifdef SQLITE_TEST
/*
** This routine is used for testing and analysis only.
*/
int *sqlite3PagerStats(Pager *pPager){
  static int a[11];
  a[0] = pPager->nRef;
  a[1] = pPager->nPage;
  a[2] = pPager->mxPage;
  a[3] = pPager->dbSize;
  a[4] = pPager->state;
  a[5] = pPager->errCode;
  a[6] = pPager->nHit;
  a[7] = pPager->nMiss;
  a[8] = 0;  /* Used to be pPager->nOvfl */
  a[9] = pPager->nRead;
  a[10] = pPager->nWrite;
  return a;
}
#endif

/*
** Set the statement rollback point.
**
** This routine should be called with the transaction journal already
** open.  A new statement journal is created that can be used to rollback
** changes of a single SQL command within a larger transaction.
*/
static int pagerStmtBegin(Pager *pPager){
  int rc;
  assert( !pPager->stmtInUse );
  assert( pPager->state>=PAGER_SHARED );
  assert( pPager->dbSize>=0 );
  PAGERTRACE2("STMT-BEGIN %d\n", PAGERID(pPager));
  if( MEMDB ){
    pPager->stmtInUse = 1;
    pPager->stmtSize = pPager->dbSize;
    return SQLITE_OK;
  }
  if( !pPager->journalOpen ){
    pPager->stmtAutoopen = 1;
    return SQLITE_OK;
  }
  assert( pPager->journalOpen );
  pagerLeave(pPager);
  assert( pPager->aInStmt==0 );
  pPager->aInStmt = sqlite3MallocZero( pPager->dbSize/8 + 1 );
  pagerEnter(pPager);
  if( pPager->aInStmt==0 ){
    /* sqlite3OsLock(pPager->fd, SHARED_LOCK); */
    return SQLITE_NOMEM;
  }
#ifndef NDEBUG
  rc = sqlite3OsFileSize(pPager->jfd, &pPager->stmtJSize);
  if( rc ) goto stmt_begin_failed;
  assert( pPager->stmtJSize == pPager->journalOff );
#endif
  pPager->stmtJSize = pPager->journalOff;
  pPager->stmtSize = pPager->dbSize;
  pPager->stmtHdrOff = 0;
  pPager->stmtCksum = pPager->cksumInit;
  if( !pPager->stmtOpen ){
    rc = sqlite3PagerOpentemp(pPager->pVfs, pPager->stfd, pPager->zStmtJrnl,
                              SQLITE_OPEN_SUBJOURNAL);
    if( rc ){
      goto stmt_begin_failed;
    }
    pPager->stmtOpen = 1;
    pPager->stmtNRec = 0;
  }
  pPager->stmtInUse = 1;
  return SQLITE_OK;
 
stmt_begin_failed:
  if( pPager->aInStmt ){
    sqlite3_free(pPager->aInStmt);
    pPager->aInStmt = 0;
  }
  return rc;
}
int sqlite3PagerStmtBegin(Pager *pPager){
  int rc;
  pagerEnter(pPager);
  rc = pagerStmtBegin(pPager);
  pagerLeave(pPager);
  return rc;
}

/*
** Commit a statement.
*/
int sqlite3PagerStmtCommit(Pager *pPager){
  pagerEnter(pPager);
  if( pPager->stmtInUse ){
    PgHdr *pPg, *pNext;
    PAGERTRACE2("STMT-COMMIT %d\n", PAGERID(pPager));
    if( !MEMDB ){
      /* sqlite3OsTruncate(pPager->stfd, 0); */
      sqlite3_free( pPager->aInStmt );
      pPager->aInStmt = 0;
    }else{
      for(pPg=pPager->pStmt; pPg; pPg=pNext){
        PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
        pNext = pHist->pNextStmt;
        assert( pHist->inStmt );
        pHist->inStmt = 0;
        pHist->pPrevStmt = pHist->pNextStmt = 0;
        sqlite3_free(pHist->pStmt);
        pHist->pStmt = 0;
      }
    }
    pPager->stmtNRec = 0;
    pPager->stmtInUse = 0;
    pPager->pStmt = 0;
  }
  pPager->stmtAutoopen = 0;
  pagerLeave(pPager);
  return SQLITE_OK;
}

/*
** Rollback a statement.
*/
int sqlite3PagerStmtRollback(Pager *pPager){
  int rc;
  pagerEnter(pPager);
  if( pPager->stmtInUse ){
    PAGERTRACE2("STMT-ROLLBACK %d\n", PAGERID(pPager));
    if( MEMDB ){
      PgHdr *pPg;
      PgHistory *pHist;
      for(pPg=pPager->pStmt; pPg; pPg=pHist->pNextStmt){
        pHist = PGHDR_TO_HIST(pPg, pPager);
        if( pHist->pStmt ){
          memcpy(PGHDR_TO_DATA(pPg), pHist->pStmt, pPager->pageSize);
          sqlite3_free(pHist->pStmt);
          pHist->pStmt = 0;
        }
      }
      pPager->dbSize = pPager->stmtSize;
      pager_truncate_cache(pPager);
      rc = SQLITE_OK;
    }else{
      rc = pager_stmt_playback(pPager);
    }
    sqlite3PagerStmtCommit(pPager);
  }else{
    rc = SQLITE_OK;
  }
  pPager->stmtAutoopen = 0;
  pagerLeave(pPager);
  return rc;
}

/*
** Return the full pathname of the database file.
*/
const char *sqlite3PagerFilename(Pager *pPager){
  return pPager->zFilename;
}

/*
** Return the VFS structure for the pager.
*/
const sqlite3_vfs *sqlite3PagerVfs(Pager *pPager){
  return pPager->pVfs;
}

/*
** Return the file handle for the database file associated
** with the pager.  This might return NULL if the file has
** not yet been opened.
*/
sqlite3_file *sqlite3PagerFile(Pager *pPager){
  return pPager->fd;
}

/*
** Return the directory of the database file.
*/
const char *sqlite3PagerDirname(Pager *pPager){
  return pPager->zDirectory;
}

/*
** Return the full pathname of the journal file.
*/
const char *sqlite3PagerJournalname(Pager *pPager){
  return pPager->zJournal;
}

/*
** Return true if fsync() calls are disabled for this pager.  Return FALSE
** if fsync()s are executed normally.
*/
int sqlite3PagerNosync(Pager *pPager){
  return pPager->noSync;
}

#ifdef SQLITE_HAS_CODEC
/*
** Set the codec for this pager
*/
void sqlite3PagerSetCodec(
  Pager *pPager,
  void *(*xCodec)(void*,void*,Pgno,int),
  void *pCodecArg
){
  pPager->xCodec = xCodec;
  pPager->pCodecArg = pCodecArg;
}
#endif

#ifndef SQLITE_OMIT_AUTOVACUUM
/*
** Move the page pPg to location pgno in the file.
**
** There must be no references to the page previously located at
** pgno (which we call pPgOld) though that page is allowed to be
** in cache.  If the page previous located at pgno is not already
** in the rollback journal, it is not put there by by this routine.
**
** References to the page pPg remain valid. Updating any
** meta-data associated with pPg (i.e. data stored in the nExtra bytes
** allocated along with the page) is the responsibility of the caller.
**
** A transaction must be active when this routine is called. It used to be
** required that a statement transaction was not active, but this restriction
** has been removed (CREATE INDEX needs to move a page when a statement
** transaction is active).
*/
int sqlite3PagerMovepage(Pager *pPager, DbPage *pPg, Pgno pgno){
  PgHdr *pPgOld;  /* The page being overwritten. */
  int h;
  Pgno needSyncPgno = 0;

  pagerEnter(pPager);
  assert( pPg->nRef>0 );

  PAGERTRACE5("MOVE %d page %d (needSync=%d) moves to %d\n", 
      PAGERID(pPager), pPg->pgno, pPg->needSync, pgno);
  IOTRACE(("MOVE %p %d %d\n", pPager, pPg->pgno, pgno))

  pager_get_content(pPg);
  if( pPg->needSync ){
    needSyncPgno = pPg->pgno;
    assert( pPg->inJournal || (int)pgno>pPager->origDbSize );
    assert( pPg->dirty );
    assert( pPager->needSync );
  }

  /* Unlink pPg from its hash-chain */
  unlinkHashChain(pPager, pPg);

  /* If the cache contains a page with page-number pgno, remove it
  ** from its hash chain. Also, if the PgHdr.needSync was set for 
  ** page pgno before the 'move' operation, it needs to be retained 
  ** for the page moved there.
  */
  pPg->needSync = 0;
  pPgOld = pager_lookup(pPager, pgno);
  if( pPgOld ){
    assert( pPgOld->nRef==0 );
    unlinkHashChain(pPager, pPgOld);
    makeClean(pPgOld);
    pPg->needSync = pPgOld->needSync;
  }else{
    pPg->needSync = 0;
  }
  if( pPager->aInJournal && (int)pgno<=pPager->origDbSize ){
    pPg->inJournal =  (pPager->aInJournal[pgno/8] & (1<<(pgno&7)))!=0;
  }else{
    pPg->inJournal = 0;
    assert( pPg->needSync==0 || (int)pgno>pPager->origDbSize );
  }

  /* Change the page number for pPg and insert it into the new hash-chain. */
  assert( pgno!=0 );
  pPg->pgno = pgno;
  h = pgno & (pPager->nHash-1);
  if( pPager->aHash[h] ){
    assert( pPager->aHash[h]->pPrevHash==0 );
    pPager->aHash[h]->pPrevHash = pPg;
  }
  pPg->pNextHash = pPager->aHash[h];
  pPager->aHash[h] = pPg;
  pPg->pPrevHash = 0;

  makeDirty(pPg);
  pPager->dirtyCache = 1;

  if( needSyncPgno ){
    /* If needSyncPgno is non-zero, then the journal file needs to be 
    ** sync()ed before any data is written to database file page needSyncPgno.
    ** Currently, no such page exists in the page-cache and the 
    ** Pager.aInJournal bit has been set. This needs to be remedied by loading
    ** the page into the pager-cache and setting the PgHdr.needSync flag.
    **
    ** If the attempt to load the page into the page-cache fails, (due
    ** to a malloc() or IO failure), clear the bit in the aInJournal[]
    ** array. Otherwise, if the page is loaded and written again in
    ** this transaction, it may be written to the database file before
    ** it is synced into the journal file. This way, it may end up in
    ** the journal file twice, but that is not a problem.
    **
    ** The sqlite3PagerGet() call may cause the journal to sync. So make
    ** sure the Pager.needSync flag is set too.
    */
    int rc;
    PgHdr *pPgHdr;
    assert( pPager->needSync );
    rc = sqlite3PagerGet(pPager, needSyncPgno, &pPgHdr);
    if( rc!=SQLITE_OK ){
      if( pPager->aInJournal && (int)needSyncPgno<=pPager->origDbSize ){
        pPager->aInJournal[needSyncPgno/8] &= ~(1<<(needSyncPgno&7));
      }
      pagerLeave(pPager);
      return rc;
    }
    pPager->needSync = 1;
    pPgHdr->needSync = 1;
    pPgHdr->inJournal = 1;
    makeDirty(pPgHdr);
    sqlite3PagerUnref(pPgHdr);
  }

  pagerLeave(pPager);
  return SQLITE_OK;
}
#endif

/*
** Return a pointer to the data for the specified page.
*/
void *sqlite3PagerGetData(DbPage *pPg){
  return PGHDR_TO_DATA(pPg);
}

/*
** Return a pointer to the Pager.nExtra bytes of "extra" space 
** allocated along with the specified page.
*/
void *sqlite3PagerGetExtra(DbPage *pPg){
  Pager *pPager = pPg->pPager;
  return (pPager?PGHDR_TO_EXTRA(pPg, pPager):0);
}

/*
** Get/set the locking-mode for this pager. Parameter eMode must be one
** of PAGER_LOCKINGMODE_QUERY, PAGER_LOCKINGMODE_NORMAL or 
** PAGER_LOCKINGMODE_EXCLUSIVE. If the parameter is not _QUERY, then
** the locking-mode is set to the value specified.
**
** The returned value is either PAGER_LOCKINGMODE_NORMAL or
** PAGER_LOCKINGMODE_EXCLUSIVE, indicating the current (possibly updated)
** locking-mode.
*/
int sqlite3PagerLockingMode(Pager *pPager, int eMode){
  assert( eMode==PAGER_LOCKINGMODE_QUERY
            || eMode==PAGER_LOCKINGMODE_NORMAL
            || eMode==PAGER_LOCKINGMODE_EXCLUSIVE );
  assert( PAGER_LOCKINGMODE_QUERY<0 );
  assert( PAGER_LOCKINGMODE_NORMAL>=0 && PAGER_LOCKINGMODE_EXCLUSIVE>=0 );
  if( eMode>=0 && !pPager->tempFile ){
    pPager->exclusiveMode = eMode;
  }
  return (int)pPager->exclusiveMode;
}

#ifdef SQLITE_TEST
/*
** Print a listing of all referenced pages and their ref count.
*/
void sqlite3PagerRefdump(Pager *pPager){
  PgHdr *pPg;
  for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
    if( pPg->nRef<=0 ) continue;
    sqlite3DebugPrintf("PAGE %3d addr=%p nRef=%d\n", 
       pPg->pgno, PGHDR_TO_DATA(pPg), pPg->nRef);
  }
}
#endif

#endif /* SQLITE_OMIT_DISKIO */
