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
#include "sqliteInt.h"
#include "os.h"
#include "pager.h"
#include <assert.h>
#include <string.h>

/*
** Macros for troubleshooting.  Normally turned off
*/
#if 0
#define TRACE1(X)       sqlite3DebugPrintf(X)
#define TRACE2(X,Y)     sqlite3DebugPrintf(X,Y)
#define TRACE3(X,Y,Z)   sqlite3DebugPrintf(X,Y,Z)
#define TRACE4(X,Y,Z,W) sqlite3DebugPrintf(X,Y,Z,W)
#else
#define TRACE1(X)
#define TRACE2(X,Y)
#define TRACE3(X,Y,Z)
#define TRACE4(X,Y,Z,W)
#endif


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
**                       disk. All that remains to do is to remove the
**                       journal file and the transaction will be
**                       committed.
**
** The page cache comes up in PAGER_UNLOCK.  The first time a
** sqlite3pager_get() occurs, the state transitions to PAGER_SHARED.
** After all pages have been released using sqlite_page_unref(),
** the state transitions back to PAGER_UNLOCK.  The first time
** that sqlite3pager_write() is called, the state transitions to
** PAGER_RESERVED.  (Note that sqlite_page_write() can only be
** called on an outstanding page which means that the pager must
** be in PAGER_SHARED before it transitions to PAGER_RESERVED.)
** The transition to PAGER_EXCLUSIVE occurs when before any changes
** are made to the database file.  After an sqlite3pager_rollback()
** or sqlite_pager_commit(), the state goes back to PAGER_SHARED.
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
** Each in-memory image of a page begins with the following header.
** This header is only visible to this pager module.  The client
** code that calls pager sees only the data that follows the header.
**
** Client code should call sqlite3pager_write() on a page prior to making
** any modifications to that page.  The first time sqlite3pager_write()
** is called, the original page contents are written into the rollback
** journal and PgHdr.inJournal and PgHdr.needSync are set.  Later, once
** the journal page has made it onto the disk surface, PgHdr.needSync
** is cleared.  The modified page cannot be written back into the original
** database file until the journal pages has been synced to disk and the
** PgHdr.needSync has been cleared.
**
** The PgHdr.dirty flag is set when sqlite3pager_write() is called and
** is cleared again when the page content is written back to the original
** database file.
*/
typedef struct PgHdr PgHdr;
struct PgHdr {
  Pager *pPager;                 /* The pager to which this page belongs */
  Pgno pgno;                     /* The page number for this page */
  PgHdr *pNextHash, *pPrevHash;  /* Hash collision chain for PgHdr.pgno */
  PgHdr *pNextFree, *pPrevFree;  /* Freelist of pages where nRef==0 */
  PgHdr *pNextAll;               /* A list of all pages */
  PgHdr *pNextStmt, *pPrevStmt;  /* List of pages in the statement journal */
  u8 inJournal;                  /* TRUE if has been written to journal */
  u8 inStmt;                     /* TRUE if in the statement subjournal */
  u8 dirty;                      /* TRUE if we need to write back changes */
  u8 needSync;                   /* Sync journal before writing this page */
  u8 alwaysRollback;             /* Disable dont_rollback() for this page */
  short int nRef;                /* Number of users of this page */
  PgHdr *pDirty;                 /* Dirty pages sorted by PgHdr.pgno */
  /* pPager->pageSize bytes of page data follow this header */
  /* Pager.nExtra bytes of local data follow the page data */
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
};

/*
** A macro used for invoking the codec if there is one
*/
#ifdef SQLITE_HAS_CODEC
# define CODEC(P,D,N,X) if( P->xCodec ){ P->xCodec(P->pCodecArg,D,N,X); }
#else
# define CODEC(P,D,N,X)
#endif

/*
** Convert a pointer to a PgHdr into a pointer to its data
** and back again.
*/
#define PGHDR_TO_DATA(P)  ((void*)(&(P)[1]))
#define DATA_TO_PGHDR(D)  (&((PgHdr*)(D))[-1])
#define PGHDR_TO_EXTRA(G,P) ((void*)&((char*)(&(G)[1]))[(P)->pageSize])
#define PGHDR_TO_HIST(P,PGR)  \
            ((PgHistory*)&((char*)(&(P)[1]))[(PGR)->pageSize+(PGR)->nExtra])

/*
** How big to make the hash table used for locating in-memory pages
** by page number.
*/
#define N_PG_HASH 2048

/*
** Hash a page number
*/
#define pager_hash(PN)  ((PN)&(N_PG_HASH-1))

/*
** A open page cache is an instance of the following structure.
*/
struct Pager {
  char *zFilename;            /* Name of the database file */
  char *zJournal;             /* Name of the journal file */
  char *zDirectory;           /* Directory hold database and journal files */
  OsFile fd, jfd;             /* File descriptors for database and journal */
  OsFile stfd;                /* File descriptor for the statement subjournal*/
  int dbSize;                 /* Number of pages in the file */
  int origDbSize;             /* dbSize before the current change */
  int stmtSize;               /* Size of database (in pages) at stmt_begin() */
  i64 stmtJSize;              /* Size of journal at stmt_begin() */
  int nRec;                   /* Number of pages written to the journal */
  u32 cksumInit;              /* Quasi-random value added to every checksum */
  int stmtNRec;               /* Number of records in stmt subjournal */
  int nExtra;                 /* Add this many bytes to each in-memory page */
  void (*xDestructor)(void*,int); /* Call this routine when freeing pages */
  void (*xReiniter)(void*,int);   /* Call this routine when reloading pages */
  int pageSize;               /* Number of bytes in a page */
  int nPage;                  /* Total number of in-memory pages */
  int nRef;                   /* Number of in-memory pages with PgHdr.nRef>0 */
  int mxPage;                 /* Maximum number of pages to hold in cache */
  int nHit, nMiss, nOvfl;     /* Cache hits, missing, and LRU overflows */
  void (*xCodec)(void*,void*,Pgno,int); /* Routine for en/decoding data */
  void *pCodecArg;            /* First argument to xCodec() */
  u8 journalOpen;             /* True if journal file descriptors is valid */
  u8 journalStarted;          /* True if header of journal is synced */
  u8 useJournal;              /* Use a rollback journal on this file */
  u8 stmtOpen;                /* True if the statement subjournal is open */
  u8 stmtInUse;               /* True we are in a statement subtransaction */
  u8 stmtAutoopen;            /* Open stmt journal when main journal is opened*/
  u8 noSync;                  /* Do not sync the journal if true */
  u8 fullSync;                /* Do extra syncs of the journal for robustness */
  u8 state;                   /* PAGER_UNLOCK, _SHARED, _RESERVED, etc. */
  u8 errMask;                 /* One of several kinds of errors */
  u8 tempFile;                /* zFilename is a temporary file */
  u8 readOnly;                /* True for a read-only database */
  u8 needSync;                /* True if an fsync() is needed on the journal */
  u8 dirtyCache;              /* True if cached pages have changed */
  u8 alwaysRollback;          /* Disable dont_rollback() for all pages */
  u8 memDb;                   /* True to inhibit all file I/O */
  u8 *aInJournal;             /* One bit for each page in the database file */
  u8 *aInStmt;                /* One bit for each page in the database */
  u8 setMaster;               /* True if a m-j name has been written to jrnl */
  BusyHandler *pBusyHandler;  /* Pointer to sqlite.busyHandler */
  PgHdr *pFirst, *pLast;      /* List of free pages */
  PgHdr *pFirstSynced;        /* First free page with PgHdr.needSync==0 */
  PgHdr *pAll;                /* List of all pages */
  PgHdr *pStmt;               /* List of pages in the statement subjournal */
  i64 journalOff;             /* Current byte offset in the journal file */
  i64 journalHdr;             /* Byte offset to previous journal header */
  i64 stmtHdrOff;             /* First journal header written this statement */
  i64 stmtCksum;              /* cksumInit when statement was started */
  int sectorSize;             /* Assumed sector size during rollback */
  PgHdr *aHash[N_PG_HASH];    /* Hash table to map page number to PgHdr */
};

/*
** These are bits that can be set in Pager.errMask.
*/
#define PAGER_ERR_FULL     0x01  /* a write() failed */
#define PAGER_ERR_MEM      0x02  /* malloc() failed */
#define PAGER_ERR_LOCK     0x04  /* error in the locking protocol */
#define PAGER_ERR_CORRUPT  0x08  /* database or journal corruption */
#define PAGER_ERR_DISK     0x10  /* general disk I/O error - bad hard drive? */

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

#define PAGER_SECTOR_SIZE 512

/*
** Page number PAGER_MJ_PGNO is never used in an SQLite database (it is
** reserved for working around a windows/posix incompatibility). It is
** used in the journal to signify that the remainder of the journal file 
** is devoted to storing a master journal name - there are no more pages to
** roll back. See comments for function writeMasterJournal() for details.
*/
#define PAGER_MJ_PGNO(x) (PENDING_BYTE/((x)->pageSize))

/*
** Enable reference count tracking (for debugging) here:
*/
#ifdef SQLITE_TEST
  int pager3_refinfo_enable = 0;
  static void pager_refinfo(PgHdr *p){
    static int cnt = 0;
    if( !pager3_refinfo_enable ) return;
    sqlite3DebugPrintf(
       "REFCNT: %4d addr=%p nRef=%d\n",
       p->pgno, PGHDR_TO_DATA(p), p->nRef
    );
    cnt++;   /* Something to set a breakpoint on */
  }
# define REFINFO(X)  pager_refinfo(X)
#else
# define REFINFO(X)
#endif

/*
** Read a 32-bit integer from the given file descriptor.  Store the integer
** that is read in *pRes.  Return SQLITE_OK if everything worked, or an
** error code is something goes wrong.
**
** All values are stored on disk as big-endian.
*/
static int read32bits(OsFile *fd, u32 *pRes){
  u32 res;
  int rc;
  rc = sqlite3OsRead(fd, &res, sizeof(res));
  if( rc==SQLITE_OK ){
    unsigned char ac[4];
    memcpy(ac, &res, 4);
    res = (ac[0]<<24) | (ac[1]<<16) | (ac[2]<<8) | ac[3];
  }
  *pRes = res;
  return rc;
}

/*
** Write a 32-bit integer into the given file descriptor.  Return SQLITE_OK
** on success or an error code is something goes wrong.
*/
static int write32bits(OsFile *fd, u32 val){
  unsigned char ac[4];
  ac[0] = (val>>24) & 0xff;
  ac[1] = (val>>16) & 0xff;
  ac[2] = (val>>8) & 0xff;
  ac[3] = val & 0xff;
  return sqlite3OsWrite(fd, ac, 4);
}

/*
** Write the 32-bit integer 'val' into the page identified by page header
** 'p' at offset 'offset'.
*/
static void store32bits(u32 val, PgHdr *p, int offset){
  unsigned char *ac;
  ac = &((unsigned char*)PGHDR_TO_DATA(p))[offset];
  ac[0] = (val>>24) & 0xff;
  ac[1] = (val>>16) & 0xff;
  ac[2] = (val>>8) & 0xff;
  ac[3] = val & 0xff;
}

/*
** Read a 32-bit integer at offset 'offset' from the page identified by
** page header 'p'.
*/
static u32 retrieve32bits(PgHdr *p, int offset){
  unsigned char *ac;
  ac = &((unsigned char*)PGHDR_TO_DATA(p))[offset];
  return (ac[0]<<24) | (ac[1]<<16) | (ac[2]<<8) | ac[3];
}


/*
** Convert the bits in the pPager->errMask into an approprate
** return code.
*/
static int pager_errcode(Pager *pPager){
  int rc = SQLITE_OK;
  if( pPager->errMask & PAGER_ERR_LOCK )    rc = SQLITE_PROTOCOL;
  if( pPager->errMask & PAGER_ERR_DISK )    rc = SQLITE_IOERR;
  if( pPager->errMask & PAGER_ERR_FULL )    rc = SQLITE_FULL;
  if( pPager->errMask & PAGER_ERR_MEM )     rc = SQLITE_NOMEM;
  if( pPager->errMask & PAGER_ERR_CORRUPT ) rc = SQLITE_CORRUPT;
  return rc;
}

/*
** When this is called the journal file for pager pPager must be open.
** The master journal file name is read from the end of the file and 
** written into memory obtained from sqliteMalloc(). *pzMaster is
** set to point at the memory and SQLITE_OK returned. The caller must
** sqliteFree() *pzMaster.
**
** If no master journal file name is present *pzMaster is set to 0 and
** SQLITE_OK returned.
*/
static int readMasterJournal(OsFile *pJrnl, char **pzMaster){
  int rc;
  u32 len;
  i64 szJ;
  u32 cksum;
  int i;
  unsigned char aMagic[8]; /* A buffer to hold the magic header */

  *pzMaster = 0;

  rc = sqlite3OsFileSize(pJrnl, &szJ);
  if( rc!=SQLITE_OK || szJ<16 ) return rc;

  rc = sqlite3OsSeek(pJrnl, szJ-16);
  if( rc!=SQLITE_OK ) return rc;
 
  rc = read32bits(pJrnl, &len);
  if( rc!=SQLITE_OK ) return rc;

  rc = read32bits(pJrnl, &cksum);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3OsRead(pJrnl, aMagic, 8);
  if( rc!=SQLITE_OK || memcmp(aMagic, aJournalMagic, 8) ) return rc;

  rc = sqlite3OsSeek(pJrnl, szJ-16-len);
  if( rc!=SQLITE_OK ) return rc;

  *pzMaster = (char *)sqliteMalloc(len+1);
  if( !*pzMaster ){
    return SQLITE_NOMEM;
  }
  rc = sqlite3OsRead(pJrnl, *pzMaster, len);
  if( rc!=SQLITE_OK ){
    sqliteFree(*pzMaster);
    *pzMaster = 0;
    return rc;
  }

  /* See if the checksum matches the master journal name */
  for(i=0; i<len; i++){
    cksum -= (*pzMaster)[i];
  }
  if( cksum ){
    /* If the checksum doesn't add up, then one or more of the disk sectors
    ** containing the master journal filename is corrupted. This means
    ** definitely roll back, so just return SQLITE_OK and report a (nul)
    ** master-journal filename.
    */
    sqliteFree(*pzMaster);
    *pzMaster = 0;
  }
  (*pzMaster)[len] = '\0';
   
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
static int seekJournalHdr(Pager *pPager){
  i64 offset = 0;
  i64 c = pPager->journalOff;
  if( c ){
    offset = ((c-1)/JOURNAL_HDR_SZ(pPager) + 1) * JOURNAL_HDR_SZ(pPager);
  }
  assert( offset%JOURNAL_HDR_SZ(pPager)==0 );
  assert( offset>=c );
  assert( (offset-c)<JOURNAL_HDR_SZ(pPager) );
  pPager->journalOff = offset;
  return sqlite3OsSeek(&pPager->jfd, pPager->journalOff);
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

  int rc = seekJournalHdr(pPager);
  if( rc ) return rc;

  pPager->journalHdr = pPager->journalOff;
  if( pPager->stmtHdrOff==0 ){
    pPager->stmtHdrOff = pPager->journalHdr;
  }
  pPager->journalOff += JOURNAL_HDR_SZ(pPager);

  /* FIX ME: 
  **
  ** Possibly for a pager not in no-sync mode, the journal magic should not
  ** be written until nRec is filled in as part of next syncJournal(). 
  **
  ** Actually maybe the whole journal header should be delayed until that
  ** point. Think about this.
  */
  rc = sqlite3OsWrite(&pPager->jfd, aJournalMagic, sizeof(aJournalMagic));

  if( rc==SQLITE_OK ){
    /* The nRec Field. 0xFFFFFFFF for no-sync journals. */
    rc = write32bits(&pPager->jfd, pPager->noSync ? 0xffffffff : 0);
  }
  if( rc==SQLITE_OK ){
    /* The random check-hash initialiser */ 
    sqlite3Randomness(sizeof(pPager->cksumInit), &pPager->cksumInit);
    rc = write32bits(&pPager->jfd, pPager->cksumInit);
  }
  if( rc==SQLITE_OK ){
    /* The initial database size */
    rc = write32bits(&pPager->jfd, pPager->dbSize);
  }
  if( rc==SQLITE_OK ){
    /* The assumed sector size for this process */
    rc = write32bits(&pPager->jfd, pPager->sectorSize);
  }

  /* The journal header has been written successfully. Seek the journal
  ** file descriptor to the end of the journal header sector.
  */
  if( rc==SQLITE_OK ){
    sqlite3OsSeek(&pPager->jfd, pPager->journalOff-1);
    rc = sqlite3OsWrite(&pPager->jfd, "\000", 1);
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

  rc = seekJournalHdr(pPager);
  if( rc ) return rc;

  if( pPager->journalOff+JOURNAL_HDR_SZ(pPager) > journalSize ){
    return SQLITE_DONE;
  }

  rc = sqlite3OsRead(&pPager->jfd, aMagic, sizeof(aMagic));
  if( rc ) return rc;

  if( memcmp(aMagic, aJournalMagic, sizeof(aMagic))!=0 ){
    return SQLITE_DONE;
  }

  rc = read32bits(&pPager->jfd, pNRec);
  if( rc ) return rc;

  rc = read32bits(&pPager->jfd, &pPager->cksumInit);
  if( rc ) return rc;

  rc = read32bits(&pPager->jfd, pDbSize);
  if( rc ) return rc;

  /* Update the assumed sector-size to match the value used by 
  ** the process that created this journal. If this journal was
  ** created by a process other than this one, then this routine
  ** is being called from within pager_playback(). The local value
  ** of Pager.sectorSize is restored at the end of that routine.
  */
  rc = read32bits(&pPager->jfd, (u32 *)&pPager->sectorSize);
  if( rc ) return rc;

  pPager->journalOff += JOURNAL_HDR_SZ(pPager);
  rc = sqlite3OsSeek(&pPager->jfd, pPager->journalOff);
  return rc;
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
*/
static int writeMasterJournal(Pager *pPager, const char *zMaster){
  int rc;
  int len; 
  int i; 
  u32 cksum = 0; 

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
    rc = seekJournalHdr(pPager);
    if( rc!=SQLITE_OK ) return rc;
  }
  pPager->journalOff += (len+20);

  rc = write32bits(&pPager->jfd, PAGER_MJ_PGNO(pPager));
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3OsWrite(&pPager->jfd, zMaster, len);
  if( rc!=SQLITE_OK ) return rc;

  rc = write32bits(&pPager->jfd, len);
  if( rc!=SQLITE_OK ) return rc;

  rc = write32bits(&pPager->jfd, cksum);
  if( rc!=SQLITE_OK ) return rc;

  rc = sqlite3OsWrite(&pPager->jfd, aJournalMagic, sizeof(aJournalMagic));
  pPager->needSync = 1;
  return rc;
}

/*
** Add or remove a page from the list of all pages that are in the
** statement journal.
**
** The Pager keeps a separate list of pages that are currently in
** the statement journal.  This helps the sqlite3pager_stmt_commit()
** routine run MUCH faster for the common case where there are many
** pages in memory but only a few are in the statement journal.
*/
static void page_add_to_stmt_list(PgHdr *pPg){
  Pager *pPager = pPg->pPager;
  if( pPg->inStmt ) return;
  assert( pPg->pPrevStmt==0 && pPg->pNextStmt==0 );
  pPg->pPrevStmt = 0;
  if( pPager->pStmt ){
    pPager->pStmt->pPrevStmt = pPg;
  }
  pPg->pNextStmt = pPager->pStmt;
  pPager->pStmt = pPg;
  pPg->inStmt = 1;
}
static void page_remove_from_stmt_list(PgHdr *pPg){
  if( !pPg->inStmt ) return;
  if( pPg->pPrevStmt ){
    assert( pPg->pPrevStmt->pNextStmt==pPg );
    pPg->pPrevStmt->pNextStmt = pPg->pNextStmt;
  }else{
    assert( pPg->pPager->pStmt==pPg );
    pPg->pPager->pStmt = pPg->pNextStmt;
  }
  if( pPg->pNextStmt ){
    assert( pPg->pNextStmt->pPrevStmt==pPg );
    pPg->pNextStmt->pPrevStmt = pPg->pPrevStmt;
  }
  pPg->pNextStmt = 0;
  pPg->pPrevStmt = 0;
  pPg->inStmt = 0;
}

/*
** Find a page in the hash table given its page number.  Return
** a pointer to the page or NULL if not found.
*/
static PgHdr *pager_lookup(Pager *pPager, Pgno pgno){
  PgHdr *p = pPager->aHash[pager_hash(pgno)];
  while( p && p->pgno!=pgno ){
    p = p->pNextHash;
  }
  return p;
}

/*
** Unlock the database and clear the in-memory cache.  This routine
** sets the state of the pager back to what it was when it was first
** opened.  Any outstanding pages are invalidated and subsequent attempts
** to access those pages will likely result in a coredump.
*/
static void pager_reset(Pager *pPager){
  PgHdr *pPg, *pNext;
  for(pPg=pPager->pAll; pPg; pPg=pNext){
    pNext = pPg->pNextAll;
    sqliteFree(pPg);
  }
  pPager->pFirst = 0;
  pPager->pFirstSynced = 0;
  pPager->pLast = 0;
  pPager->pAll = 0;
  memset(pPager->aHash, 0, sizeof(pPager->aHash));
  pPager->nPage = 0;
  if( pPager->state>=PAGER_RESERVED ){
    sqlite3pager_rollback(pPager);
  }
  sqlite3OsUnlock(&pPager->fd, NO_LOCK);
  pPager->state = PAGER_UNLOCK;
  pPager->dbSize = -1;
  pPager->nRef = 0;
  assert( pPager->journalOpen==0 );
}

/*
** When this routine is called, the pager has the journal file open and
** a RESERVED or EXCLUSIVE lock on the database.  This routine releases
** the database lock and acquires a SHARED lock in its place.  The journal
** file is deleted and closed.
**
** TODO: Consider keeping the journal file open for temporary databases.
** This might give a performance improvement on windows where opening
** a file is an expensive operation.
*/
static int pager_unwritelock(Pager *pPager){
  PgHdr *pPg;
  int rc;
  assert( !pPager->memDb );
  if( pPager->state<PAGER_RESERVED ){
    return SQLITE_OK;
  }
  sqlite3pager_stmt_commit(pPager);
  if( pPager->stmtOpen ){
    sqlite3OsClose(&pPager->stfd);
    pPager->stmtOpen = 0;
  }
  if( pPager->journalOpen ){
    sqlite3OsClose(&pPager->jfd);
    pPager->journalOpen = 0;
    sqlite3OsDelete(pPager->zJournal);
    sqliteFree( pPager->aInJournal );
    pPager->aInJournal = 0;
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      pPg->inJournal = 0;
      pPg->dirty = 0;
      pPg->needSync = 0;
    }
    pPager->dirtyCache = 0;
    pPager->nRec = 0;
  }else{
    assert( pPager->dirtyCache==0 || pPager->useJournal==0 );
  }
  rc = sqlite3OsUnlock(&pPager->fd, SHARED_LOCK);
  pPager->state = PAGER_SHARED;
  pPager->origDbSize = 0;
  pPager->setMaster = 0;
  return rc;
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
static u32 pager_cksum(Pager *pPager, Pgno pgno, const char *aData){
  u32 cksum = pPager->cksumInit;
  int i = pPager->pageSize-200;
  while( i>0 ){
    cksum += aData[i];
    i -= 200;
  }
  return cksum;
}

/*
** Read a single page from the journal file opened on file descriptor
** jfd.  Playback this one page.
**
** If useCksum==0 it means this journal does not use checksums.  Checksums
** are not used in statement journals because statement journals do not
** need to survive power failures.
*/
static int pager_playback_one_page(Pager *pPager, OsFile *jfd, int useCksum){
  int rc;
  PgHdr *pPg;                   /* An existing page in the cache */
  Pgno pgno;                    /* The page number of a page in journal */
  u32 cksum;                    /* Checksum used for sanity checking */
  u8 aData[SQLITE_MAX_PAGE_SIZE];  /* Temp storage for a page */

  rc = read32bits(jfd, &pgno);
  if( rc!=SQLITE_OK ) return rc;
  rc = sqlite3OsRead(jfd, &aData, pPager->pageSize);
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
    rc = read32bits(jfd, &cksum);
    if( rc ) return rc;
    pPager->journalOff += 4;
    if( pager_cksum(pPager, pgno, aData)!=cksum ){
      return SQLITE_DONE;
    }
  }

  assert( pPager->state==PAGER_RESERVED || pPager->state>=PAGER_EXCLUSIVE );

  /* If the pager is in RESERVED state, then there must be a copy of this
  ** page in the pager cache. In this case just update the pager cache,
  ** not the database file. The page is left marked dirty in this case.
  **
  ** If in EXCLUSIVE state, then we update the pager cache if it exists
  ** and the main file. The page is then marked not dirty.
  */
  pPg = pager_lookup(pPager, pgno);
  assert( pPager->state>=PAGER_EXCLUSIVE || pPg );
  TRACE3("PLAYBACK %d page %d\n", pPager->fd.h, pgno);
  if( pPager->state>=PAGER_EXCLUSIVE ){
    sqlite3OsSeek(&pPager->fd, (pgno-1)*(i64)pPager->pageSize);
    rc = sqlite3OsWrite(&pPager->fd, aData, pPager->pageSize);
  }
  if( pPg ){
    /* No page should ever be rolled back that is in use, except for page
    ** 1 which is held in use in order to keep the lock on the database
    ** active.
    */
    void *pData;
    assert( pPg->nRef==0 || pPg->pgno==1 );
    pData = PGHDR_TO_DATA(pPg);
    memcpy(pData, aData, pPager->pageSize);
    if( pPager->xDestructor ){  /*** FIX ME:  Should this be xReinit? ***/
      pPager->xDestructor(pData, pPager->pageSize);
    }
    if( pPager->state>=PAGER_EXCLUSIVE ){
      pPg->dirty = 0;
      pPg->needSync = 0;
    }
    CODEC(pPager, pData, pPg->pgno, 3);
  }
  return rc;
}

/*
** Parameter zMaster is the name of a master journal file. A single journal
** file that referred to the master journal file has just been rolled back.
** This routine checks if it is possible to delete the master journal file,
** and does so if it is.
**
** The master journal file contains the names of all child journals.
** To tell if a master journal can be deleted, check to each of the
** children.  If all children are either missing or do not refer to
** a different master journal, then this master journal can be deleted.
*/
static int pager_delmaster(const char *zMaster){
  int rc;
  int master_open = 0;
  OsFile master;
  char *zMasterJournal = 0; /* Contents of master journal file */
  i64 nMasterJournal;       /* Size of master journal file */

  /* Open the master journal file exclusively in case some other process
  ** is running this routine also. Not that it makes too much difference.
  */
  memset(&master, 0, sizeof(master));
  rc = sqlite3OsOpenReadOnly(zMaster, &master);
  if( rc!=SQLITE_OK ) goto delmaster_out;
  master_open = 1;
  rc = sqlite3OsFileSize(&master, &nMasterJournal);
  if( rc!=SQLITE_OK ) goto delmaster_out;

  if( nMasterJournal>0 ){
    char *zJournal;
    char *zMasterPtr = 0;

    /* Load the entire master journal file into space obtained from
    ** sqliteMalloc() and pointed to by zMasterJournal. 
    */
    zMasterJournal = (char *)sqliteMalloc(nMasterJournal);
    if( !zMasterJournal ){
      rc = SQLITE_NOMEM;
      goto delmaster_out;
    }
    rc = sqlite3OsRead(&master, zMasterJournal, nMasterJournal);
    if( rc!=SQLITE_OK ) goto delmaster_out;

    zJournal = zMasterJournal;
    while( (zJournal-zMasterJournal)<nMasterJournal ){
      if( sqlite3OsFileExists(zJournal) ){
        /* One of the journals pointed to by the master journal exists.
        ** Open it and check if it points at the master journal. If
        ** so, return without deleting the master journal file.
        */
        OsFile journal;

        memset(&journal, 0, sizeof(journal));
        rc = sqlite3OsOpenReadOnly(zJournal, &journal);
        if( rc!=SQLITE_OK ){
          goto delmaster_out;
        }

        rc = readMasterJournal(&journal, &zMasterPtr);
        sqlite3OsClose(&journal);
        if( rc!=SQLITE_OK ){
          goto delmaster_out;
        }

        if( zMasterPtr && !strcmp(zMasterPtr, zMaster) ){
          /* We have a match. Do not delete the master journal file. */
          goto delmaster_out;
        }
      }
      zJournal += (strlen(zJournal)+1);
    }
  }
  
  sqlite3OsDelete(zMaster);

delmaster_out:
  if( zMasterJournal ){
    sqliteFree(zMasterJournal);
  }  
  if( master_open ){
    sqlite3OsClose(&master);
  }
  return rc;
}

/*
** Make every page in the cache agree with what is on disk.  In other words,
** reread the disk to reset the state of the cache.
**
** This routine is called after a rollback in which some of the dirty cache
** pages had never been written out to disk.  We need to roll back the
** cache content and the easiest way to do that is to reread the old content
** back from the disk.
*/
static int pager_reload_cache(Pager *pPager){
  PgHdr *pPg;
  int rc = SQLITE_OK;
  for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
    char zBuf[SQLITE_MAX_PAGE_SIZE];
    if( !pPg->dirty ) continue;
    if( (int)pPg->pgno <= pPager->origDbSize ){
      sqlite3OsSeek(&pPager->fd, pPager->pageSize*(i64)(pPg->pgno-1));
      rc = sqlite3OsRead(&pPager->fd, zBuf, pPager->pageSize);
      TRACE3("REFETCH %d page %d\n", pPager->fd.h, pPg->pgno);
      if( rc ) break;
      CODEC(pPager, zBuf, pPg->pgno, 2);
    }else{
      memset(zBuf, 0, pPager->pageSize);
    }
    if( pPg->nRef==0 || memcmp(zBuf, PGHDR_TO_DATA(pPg), pPager->pageSize) ){
      memcpy(PGHDR_TO_DATA(pPg), zBuf, pPager->pageSize);
      if( pPager->xReiniter ){
        pPager->xReiniter(PGHDR_TO_DATA(pPg), pPager->pageSize);
      }else{
        memset(PGHDR_TO_EXTRA(pPg, pPager), 0, pPager->nExtra);
      }
    }
    pPg->needSync = 0;
    pPg->dirty = 0;
  }
  return rc;
}

/*
** Truncate the main file of the given pager to the number of pages
** indicated.
*/
static int pager_truncate(Pager *pPager, int nPage){
  return sqlite3OsTruncate(&pPager->fd, pPager->pageSize*(i64)nPage);
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
static int pager_playback(Pager *pPager){
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
  rc = sqlite3OsFileSize(&pPager->jfd, &szJ);
  if( rc!=SQLITE_OK ){
    goto end_playback;
  }

  /* Read the master journal name from the journal, if it is present.
  ** If a master journal file name is specified, but the file is not
  ** present on disk, then the journal is not hot and does not need to be
  ** played back.
  */
  rc = readMasterJournal(&pPager->jfd, &zMaster);
  assert( rc!=SQLITE_DONE );
  if( rc!=SQLITE_OK || (zMaster && !sqlite3OsFileExists(zMaster)) ){
    sqliteFree(zMaster);
    zMaster = 0;
    if( rc==SQLITE_DONE ) rc = SQLITE_OK;
    goto end_playback;
  }
  sqlite3OsSeek(&pPager->jfd, 0);
  pPager->journalOff = 0;

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

    /* If this is the first header read from the journal, truncate the
    ** database file back to it's original size.
    */
    if( pPager->journalOff==JOURNAL_HDR_SZ(pPager) ){
      assert( pPager->origDbSize==0 || pPager->origDbSize==mxPg );
      rc = pager_truncate(pPager, mxPg);
      if( rc!=SQLITE_OK ){
        goto end_playback;
      }
      pPager->dbSize = mxPg;
    }

    /* rc = sqlite3OsSeek(&pPager->jfd, JOURNAL_HDR_SZ(pPager)); */
    if( rc!=SQLITE_OK ) goto end_playback;
  
    /* Copy original pages out of the journal and back into the database file.
    */
    for(i=0; i<nRec; i++){
      rc = pager_playback_one_page(pPager, &pPager->jfd, 1);
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

  /* Pages that have been written to the journal but never synced
  ** where not restored by the loop above.  We have to restore those
  ** pages by reading them back from the original database.
  */
  assert( rc==SQLITE_OK );
  pager_reload_cache(pPager);

end_playback:
  if( rc==SQLITE_OK ){
    rc = pager_unwritelock(pPager);
  }
  if( zMaster ){
    /* If there was a master journal and this routine will return true,
    ** see if it is possible to delete the master journal. If errors 
    ** occur during this process, ignore them.
    */
    if( rc==SQLITE_OK ){
      pager_delmaster(zMaster);
    }
    sqliteFree(zMaster);
  }

  /* The Pager.sectorSize variable may have been updated while rolling
  ** back a journal created by a process with a different PAGER_SECTOR_SIZE
  ** value. Reset it to the correct value for this process.
  */
  pPager->sectorSize = PAGER_SECTOR_SIZE;
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
    rc = sqlite3OsFileSize(&pPager->jfd, &os_szJ);
    if( rc!=SQLITE_OK ) return rc;
    assert( szJ==os_szJ );
  }
#endif

  /* Set hdrOff to be the offset to the first journal header written
  ** this statement transaction, or the end of the file if no journal
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
  pPager->dbSize = pPager->stmtSize;

  /* Figure out how many records are in the statement journal.
  */
  assert( pPager->stmtInUse && pPager->journalOpen );
  sqlite3OsSeek(&pPager->stfd, 0);
  nRec = pPager->stmtNRec;
  
  /* Copy original pages out of the statement journal and back into the
  ** database file.  Note that the statement journal omits checksums from
  ** each record since power-failure recovery is not important to statement
  ** journals.
  */
  for(i=nRec-1; i>=0; i--){
    rc = pager_playback_one_page(pPager, &pPager->stfd, 0);
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
  rc = sqlite3OsSeek(&pPager->jfd, pPager->stmtJSize);
  if( rc!=SQLITE_OK ){
    goto end_stmt_playback;
  }
  pPager->journalOff = pPager->stmtJSize;
  pPager->cksumInit = pPager->stmtCksum;
  assert( JOURNAL_HDR_SZ(pPager)<(pPager->pageSize+8) );
  while( pPager->journalOff <= (hdrOff-(pPager->pageSize+8)) ){
    rc = pager_playback_one_page(pPager, &pPager->jfd, 1);
    assert( rc!=SQLITE_DONE );
    if( rc!=SQLITE_OK ) goto end_stmt_playback;
  }

  while( pPager->journalOff < szJ ){
    u32 nRec;
    u32 dummy;
    rc = readJournalHdr(pPager, szJ, &nRec, &dummy);
    if( rc!=SQLITE_OK ){
      assert( rc!=SQLITE_DONE );
      goto end_stmt_playback;
    }
    if( nRec==0 ){
      nRec = (szJ - pPager->journalOff) / (pPager->pageSize+8);
    }
    for(i=nRec-1; i>=0 && pPager->journalOff < szJ; i--){
      rc = pager_playback_one_page(pPager, &pPager->jfd, 1);
      assert( rc!=SQLITE_DONE );
      if( rc!=SQLITE_OK ) goto end_stmt_playback;
    }
  }

  pPager->journalOff = szJ;
  
end_stmt_playback:
  if( rc!=SQLITE_OK ){
    pPager->errMask |= PAGER_ERR_CORRUPT;
    rc = SQLITE_CORRUPT;  /* bkpt-CORRUPT */
  }else{
    pPager->journalOff = szJ;
    /* pager_reload_cache(pPager); */
  }
  return rc;
}

/*
** Change the maximum number of in-memory pages that are allowed.
**
** The maximum number is the absolute value of the mxPage parameter.
** If mxPage is negative, the noSync flag is also set.  noSync bypasses
** calls to sqlite3OsSync().  The pager runs much faster with noSync on,
** but if the operating system crashes or there is an abrupt power 
** failure, the database file might be left in an inconsistent and
** unrepairable state.  
*/
void sqlite3pager_set_cachesize(Pager *pPager, int mxPage){
  if( mxPage>=0 ){
    pPager->noSync = pPager->tempFile;
    if( pPager->noSync ) pPager->needSync = 0; 
  }else{
    pPager->noSync = 1;
    mxPage = -mxPage;
  }
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
void sqlite3pager_set_safety_level(Pager *pPager, int level){
  pPager->noSync =  level==1 || pPager->tempFile;
  pPager->fullSync = level==3 && !pPager->tempFile;
  if( pPager->noSync ) pPager->needSync = 0;
}

/*
** Open a temporary file.  Write the name of the file into zName
** (zName must be at least SQLITE_TEMPNAME_SIZE bytes long.)  Write
** the file descriptor into *fd.  Return SQLITE_OK on success or some
** other error code if we fail.
**
** The OS will automatically delete the temporary file when it is
** closed.
*/
static int sqlite3pager_opentemp(char *zFile, OsFile *fd){
  int cnt = 8;
  int rc;
  do{
    cnt--;
    sqlite3OsTempFileName(zFile);
    rc = sqlite3OsOpenExclusive(zFile, fd, 1);
  }while( cnt>0 && rc!=SQLITE_OK && rc!=SQLITE_NOMEM );
  return rc;
}

/*
** Create a new page cache and put a pointer to the page cache in *ppPager.
** The file to be cached need not exist.  The file is not locked until
** the first call to sqlite3pager_get() and is only held open until the
** last page is released using sqlite3pager_unref().
**
** If zFilename is NULL then a randomly-named temporary file is created
** and used as the file to be cached.  The file will be deleted
** automatically when it is closed.
**
** If zFilename is ":memory:" then all information is held in cache.
** It is never written to disk.  This can be used to implement an
** in-memory database.
*/
int sqlite3pager_open(
  Pager **ppPager,         /* Return the Pager structure here */
  const char *zFilename,   /* Name of the database file to open */
  int nExtra,              /* Extra bytes append to each in-memory page */
  int useJournal           /* TRUE to use a rollback journal on this file */
){
  Pager *pPager;
  char *zFullPathname = 0;
  int nameLen;
  OsFile fd;
  int rc = SQLITE_OK;
  int i;
  int tempFile = 0;
  int memDb = 0;
  int readOnly = 0;
  char zTemp[SQLITE_TEMPNAME_SIZE];

  *ppPager = 0;
  memset(&fd, 0, sizeof(fd));
  if( sqlite3_malloc_failed ){
    return SQLITE_NOMEM;
  }
  if( zFilename && zFilename[0] ){
    if( strcmp(zFilename,":memory:")==0 ){
      memDb = 1;
      zFullPathname = sqliteStrDup("");
      rc = SQLITE_OK;
    }else{
      zFullPathname = sqlite3OsFullPathname(zFilename);
      if( zFullPathname ){
        rc = sqlite3OsOpenReadWrite(zFullPathname, &fd, &readOnly);
      }
    }
  }else{
    rc = sqlite3pager_opentemp(zTemp, &fd);
    zFilename = zTemp;
    zFullPathname = sqlite3OsFullPathname(zFilename);
    if( rc==SQLITE_OK ){
      tempFile = 1;
    }
  }
  if( !zFullPathname ){
    sqlite3OsClose(&fd);
    return SQLITE_NOMEM;
  }
  if( rc!=SQLITE_OK ){
    sqlite3OsClose(&fd);
    sqliteFree(zFullPathname);
    return rc;
  }
  nameLen = strlen(zFullPathname);
  pPager = sqliteMalloc( sizeof(*pPager) + nameLen*3 + 30 );
  if( pPager==0 ){
    sqlite3OsClose(&fd);
    sqliteFree(zFullPathname);
    return SQLITE_NOMEM;
  }
  TRACE3("OPEN %d %s\n", fd.h, zFullPathname);
  pPager->zFilename = (char*)&pPager[1];
  pPager->zDirectory = &pPager->zFilename[nameLen+1];
  pPager->zJournal = &pPager->zDirectory[nameLen+1];
  strcpy(pPager->zFilename, zFullPathname);
  strcpy(pPager->zDirectory, zFullPathname);
  for(i=nameLen; i>0 && pPager->zDirectory[i-1]!='/'; i--){}
  if( i>0 ) pPager->zDirectory[i-1] = 0;
  strcpy(pPager->zJournal, zFullPathname);
  sqliteFree(zFullPathname);
  strcpy(&pPager->zJournal[nameLen], "-journal");
  pPager->fd = fd;
#if OS_UNIX
  pPager->fd.pPager = pPager;
#endif
  pPager->journalOpen = 0;
  pPager->useJournal = useJournal && !memDb;
  pPager->stmtOpen = 0;
  pPager->stmtInUse = 0;
  pPager->nRef = 0;
  pPager->dbSize = memDb-1;
  pPager->pageSize = SQLITE_DEFAULT_PAGE_SIZE;
  pPager->stmtSize = 0;
  pPager->stmtJSize = 0;
  pPager->nPage = 0;
  pPager->mxPage = 100;
  pPager->state = PAGER_UNLOCK;
  pPager->errMask = 0;
  pPager->tempFile = tempFile;
  pPager->memDb = memDb;
  pPager->readOnly = readOnly;
  pPager->needSync = 0;
  pPager->noSync = pPager->tempFile || !useJournal;
  pPager->fullSync = (pPager->noSync?0:1);
  pPager->pFirst = 0;
  pPager->pFirstSynced = 0;
  pPager->pLast = 0;
  pPager->nExtra = nExtra;
  pPager->sectorSize = PAGER_SECTOR_SIZE;
  pPager->pBusyHandler = 0;
  memset(pPager->aHash, 0, sizeof(pPager->aHash));
  *ppPager = pPager;
  return SQLITE_OK;
}

/*
** Set the busy handler function.
*/
void sqlite3pager_set_busyhandler(Pager *pPager, BusyHandler *pBusyHandler){
  pPager->pBusyHandler = pBusyHandler;
}

/*
** Set the destructor for this pager.  If not NULL, the destructor is called
** when the reference count on each page reaches zero.  The destructor can
** be used to clean up information in the extra segment appended to each page.
**
** The destructor is not called as a result sqlite3pager_close().  
** Destructors are only called by sqlite3pager_unref().
*/
void sqlite3pager_set_destructor(Pager *pPager, void (*xDesc)(void*,int)){
  pPager->xDestructor = xDesc;
}

/*
** Set the reinitializer for this pager.  If not NULL, the reinitializer
** is called when the content of a page in cache is restored to its original
** value as a result of a rollback.  The callback gives higher-level code
** an opportunity to restore the EXTRA section to agree with the restored
** page data.
*/
void sqlite3pager_set_reiniter(Pager *pPager, void (*xReinit)(void*,int)){
  pPager->xReiniter = xReinit;
}

/*
** Set the page size.
**
** The page size must only be changed when the cache is empty.
*/
void sqlite3pager_set_pagesize(Pager *pPager, int pageSize){
  assert( pageSize>=512 && pageSize<=SQLITE_MAX_PAGE_SIZE );
  pPager->pageSize = pageSize;
}

/*
** Read the first N bytes from the beginning of the file into memory
** that pDest points to.  No error checking is done.
*/
void sqlite3pager_read_fileheader(Pager *pPager, int N, unsigned char *pDest){
  memset(pDest, 0, N);
  if( pPager->memDb==0 ){
    sqlite3OsSeek(&pPager->fd, 0);
    sqlite3OsRead(&pPager->fd, pDest, N);
  }
}

/*
** Return the total number of pages in the disk file associated with
** pPager.
*/
int sqlite3pager_pagecount(Pager *pPager){
  i64 n;
  assert( pPager!=0 );
  if( pPager->dbSize>=0 ){
    return pPager->dbSize;
  }
  if( sqlite3OsFileSize(&pPager->fd, &n)!=SQLITE_OK ){
    pPager->errMask |= PAGER_ERR_DISK;
    return 0;
  }
  n /= pPager->pageSize;
  if( !pPager->memDb && n==PENDING_BYTE/pPager->pageSize ){
    n++;
  }
  if( pPager->state!=PAGER_UNLOCK ){
    pPager->dbSize = n;
  }
  return n;
}

/*
** Forward declaration
*/
static int syncJournal(Pager*);


/*
** Unlink a page from the free list (the list of all pages where nRef==0)
** and from its hash collision chain.
*/
static void unlinkPage(PgHdr *pPg){
  Pager *pPager = pPg->pPager;

  /* Keep the pFirstSynced pointer pointing at the first synchronized page */
  if( pPg==pPager->pFirstSynced ){
    PgHdr *p = pPg->pNextFree;
    while( p && p->needSync ){ p = p->pNextFree; }
    pPager->pFirstSynced = p;
  }

  /* Unlink from the freelist */
  if( pPg->pPrevFree ){
    pPg->pPrevFree->pNextFree = pPg->pNextFree;
  }else{
    assert( pPager->pFirst==pPg );
    pPager->pFirst = pPg->pNextFree;
  }
  if( pPg->pNextFree ){
    pPg->pNextFree->pPrevFree = pPg->pPrevFree;
  }else{
    assert( pPager->pLast==pPg );
    pPager->pLast = pPg->pPrevFree;
  }
  pPg->pNextFree = pPg->pPrevFree = 0;

  /* Unlink from the pgno hash table */
  if( pPg->pNextHash ){
    pPg->pNextHash->pPrevHash = pPg->pPrevHash;
  }
  if( pPg->pPrevHash ){
    pPg->pPrevHash->pNextHash = pPg->pNextHash;
  }else{
    int h = pager_hash(pPg->pgno);
    assert( pPager->aHash[h]==pPg );
    pPager->aHash[h] = pPg->pNextHash;
  }
  pPg->pNextHash = pPg->pPrevHash = 0;
}

/*
** This routine is used to truncate an in-memory database.  Delete
** all pages whose pgno is larger than pPager->dbSize and is unreferenced.
** Referenced pages larger than pPager->dbSize are zeroed.
*/
static void memoryTruncate(Pager *pPager){
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
      unlinkPage(pPg);
      sqliteFree(pPg);
      pPager->nPage--;
    }
  }
}

/*
** Truncate the file to the number of pages specified.
*/
int sqlite3pager_truncate(Pager *pPager, Pgno nPage){
  int rc;
  sqlite3pager_pagecount(pPager);
  if( pPager->errMask!=0 ){
    rc = pager_errcode(pPager);
    return rc;
  }
  if( nPage>=(unsigned)pPager->dbSize ){
    return SQLITE_OK;
  }
  if( pPager->memDb ){
    pPager->dbSize = nPage;
    memoryTruncate(pPager);
    return SQLITE_OK;
  }
  rc = syncJournal(pPager);
  if( rc!=SQLITE_OK ){
    return rc;
  }
  rc = pager_truncate(pPager, nPage);
  if( rc==SQLITE_OK ){
    pPager->dbSize = nPage;
  }
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
*/
int sqlite3pager_close(Pager *pPager){
  PgHdr *pPg, *pNext;
  switch( pPager->state ){
    case PAGER_RESERVED:
    case PAGER_SYNCED: 
    case PAGER_EXCLUSIVE: {
      sqlite3pager_rollback(pPager);
      if( !pPager->memDb ){
        sqlite3OsUnlock(&pPager->fd, NO_LOCK);
      }
      assert( pPager->journalOpen==0 );
      break;
    }
    case PAGER_SHARED: {
      if( !pPager->memDb ){
        sqlite3OsUnlock(&pPager->fd, NO_LOCK);
      }
      break;
    }
    default: {
      /* Do nothing */
      break;
    }
  }
  for(pPg=pPager->pAll; pPg; pPg=pNext){
#ifndef NDEBUG
    if( pPager->memDb ){
      PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
      assert( !pPg->alwaysRollback );
      assert( !pHist->pOrig );
      assert( !pHist->pStmt );
    }
#endif
    pNext = pPg->pNextAll;
    sqliteFree(pPg);
  }
  TRACE2("CLOSE %d\n", pPager->fd.h);
  sqlite3OsClose(&pPager->fd);
  assert( pPager->journalOpen==0 );
  /* Temp files are automatically deleted by the OS
  ** if( pPager->tempFile ){
  **   sqlite3OsDelete(pPager->zFilename);
  ** }
  */
  if( pPager->zFilename!=(char*)&pPager[1] ){
    assert( 0 );  /* Cannot happen */
    sqliteFree(pPager->zFilename);
    sqliteFree(pPager->zJournal);
    sqliteFree(pPager->zDirectory);
  }
  sqliteFree(pPager);
  return SQLITE_OK;
}

/*
** Return the page number for the given page data.
*/
Pgno sqlite3pager_pagenumber(void *pData){
  PgHdr *p = DATA_TO_PGHDR(pData);
  return p->pgno;
}

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
    if( pPg==pPg->pPager->pFirstSynced ){
      PgHdr *p = pPg->pNextFree;
      while( p && p->needSync ){ p = p->pNextFree; }
      pPg->pPager->pFirstSynced = p;
    }
    if( pPg->pPrevFree ){
      pPg->pPrevFree->pNextFree = pPg->pNextFree;
    }else{
      pPg->pPager->pFirst = pPg->pNextFree;
    }
    if( pPg->pNextFree ){
      pPg->pNextFree->pPrevFree = pPg->pPrevFree;
    }else{
      pPg->pPager->pLast = pPg->pPrevFree;
    }
    pPg->pPager->nRef++;
  }
  pPg->nRef++;
  REFINFO(pPg);
}
#ifdef SQLITE_TEST
  static void page_ref(PgHdr *pPg){
    if( pPg->nRef==0 ){
      _page_ref(pPg);
    }else{
      pPg->nRef++;
      REFINFO(pPg);
    }
  }
#else
# define page_ref(P)   ((P)->nRef==0?_page_ref(P):(void)(P)->nRef++)
#endif

/*
** Increment the reference count for a page.  The input pointer is
** a reference to the page data.
*/
int sqlite3pager_ref(void *pData){
  PgHdr *pPg = DATA_TO_PGHDR(pData);
  page_ref(pPg);
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
** after a power failure, so sync occurs.
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
      assert( pPager->journalOpen );
      /* assert( !pPager->noSync ); // noSync might be set if synchronous
      ** was turned off after the transaction was started.  Ticket #615 */
#ifndef NDEBUG
      {
        /* Make sure the pPager->nRec counter we are keeping agrees
        ** with the nRec computed from the size of the journal file.
        */
        i64 jSz;
        rc = sqlite3OsFileSize(&pPager->jfd, &jSz);
        if( rc!=0 ) return rc;
        assert( pPager->journalOff==jSz );
      }
#endif
      {
        /* Write the nRec value into the journal file header. If in
        ** full-synchronous mode, sync the journal first. This ensures that
        ** all data has really hit the disk before nRec is updated to mark
        ** it as a candidate for rollback. 
        */
        if( pPager->fullSync ){
          TRACE2("SYNC journal of %d\n", pPager->fd.h);
          rc = sqlite3OsSync(&pPager->jfd);
          if( rc!=0 ) return rc;
        }
        sqlite3OsSeek(&pPager->jfd, pPager->journalHdr + sizeof(aJournalMagic));
        rc = write32bits(&pPager->jfd, pPager->nRec);
        if( rc ) return rc;

        sqlite3OsSeek(&pPager->jfd, pPager->journalOff);
      }
      TRACE2("SYNC journal of %d\n", pPager->fd.h);
      rc = sqlite3OsSync(&pPager->jfd);
      if( rc!=0 ) return rc;
      pPager->journalStarted = 1;
    }
    pPager->needSync = 0;

    /* Erase the needSync flag from every page.
    */
    for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
      pPg->needSync = 0;
    }
    pPager->pFirstSynced = pPager->pFirst;
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
    assert( pPager->pFirstSynced==pPager->pFirst );
  }
#endif

  return rc;
}

/*
** Try to obtain a lock on a file.  Invoke the busy callback if the lock
** is currently not available.  Repeate until the busy callback returns
** false or until the lock succeeds.
**
** Return SQLITE_OK on success and an error code if we cannot obtain
** the lock.
*/
static int pager_wait_on_lock(Pager *pPager, int locktype){
  int rc;
  assert( PAGER_SHARED==SHARED_LOCK );
  assert( PAGER_RESERVED==RESERVED_LOCK );
  assert( PAGER_EXCLUSIVE==EXCLUSIVE_LOCK );
  if( pPager->state>=locktype ){
    rc = SQLITE_OK;
  }else{
    int busy = 1;
    do {
      rc = sqlite3OsLock(&pPager->fd, locktype);
    }while( rc==SQLITE_BUSY && 
        pPager->pBusyHandler && 
        pPager->pBusyHandler->xFunc && 
        pPager->pBusyHandler->xFunc(pPager->pBusyHandler->pArg, busy++)
    );
    if( rc==SQLITE_OK ){
      pPager->state = locktype;
    }
  }
  return rc;
}

/*
** Given a list of pages (connected by the PgHdr.pDirty pointer) write
** every one of those pages out to the database file and mark them all
** as clean.
*/
static int pager_write_pagelist(PgHdr *pList){
  Pager *pPager;
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

  while( pList ){
    assert( pList->dirty );
    sqlite3OsSeek(&pPager->fd, (pList->pgno-1)*(i64)pPager->pageSize);
    CODEC(pPager, PGHDR_TO_DATA(pList), pList->pgno, 6);
    TRACE3("STORE %d page %d\n", pPager->fd.h, pList->pgno);
    rc = sqlite3OsWrite(&pPager->fd, PGHDR_TO_DATA(pList), pPager->pageSize);
    CODEC(pPager, PGHDR_TO_DATA(pList), pList->pgno, 0);
    if( rc ) return rc;
    pList->dirty = 0;
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
  PgHdr *p, *pList;
  pList = 0;
  for(p=pPager->pAll; p; p=p->pNextAll){
    if( p->dirty ){
      p->pDirty = pList;
      pList = p;
    }
  }
  return pList;
}

/*
** Acquire a page.
**
** A read lock on the disk file is obtained when the first page is acquired. 
** This read lock is dropped when the last page is released.
**
** A _get works for any page number greater than 0.  If the database
** file is smaller than the requested page, then no actual disk
** read occurs and the memory image of the page is initialized to
** all zeros.  The extra data appended to a page is always initialized
** to zeros the first time a page is loaded into memory.
**
** The acquisition might fail for several reasons.  In all cases,
** an appropriate error code is returned and *ppPage is set to NULL.
**
** See also sqlite3pager_lookup().  Both this routine and _lookup() attempt
** to find a page in the in-memory cache first.  If the page is not already
** in memory, this routine goes to disk to read it in whereas _lookup()
** just returns 0.  This routine acquires a read-lock the first time it
** has to go to disk, and could also playback an old journal if necessary.
** Since _lookup() never goes to disk, it never has to deal with locks
** or journal files.
*/
int sqlite3pager_get(Pager *pPager, Pgno pgno, void **ppPage){
  PgHdr *pPg;
  int rc;

  /* Make sure we have not hit any critical errors.
  */ 
  assert( pPager!=0 );
  assert( pgno!=0 );
  *ppPage = 0;
  if( pPager->errMask & ~(PAGER_ERR_FULL) ){
    return pager_errcode(pPager);
  }

  /* If this is the first page accessed, then get a SHARED lock
  ** on the database file.
  */
  if( pPager->nRef==0 && !pPager->memDb ){
    rc = pager_wait_on_lock(pPager, SHARED_LOCK);
    if( rc!=SQLITE_OK ){
      return rc;
    }

    /* If a journal file exists, and there is no RESERVED lock on the
    ** database file, then it either needs to be played back or deleted.
    */
    if( pPager->useJournal && 
        sqlite3OsFileExists(pPager->zJournal) &&
        !sqlite3OsCheckReservedLock(&pPager->fd) 
    ){
       int rc;

       /* Get an EXCLUSIVE lock on the database file. At this point it is
       ** important that a RESERVED lock is not obtained on the way to the
       ** EXCLUSIVE lock. If it were, another process might open the
       ** database file, detect the RESERVED lock, and conclude that the
       ** database is safe to read while this process is still rolling it 
       ** back.
       ** 
       ** Because the intermediate RESERVED lock is not requested, the
       ** second process will get to this point in the code and fail to
       ** obtain it's own EXCLUSIVE lock on the database file.
       */
       rc = sqlite3OsLock(&pPager->fd, EXCLUSIVE_LOCK);
       if( rc!=SQLITE_OK ){
         sqlite3OsUnlock(&pPager->fd, NO_LOCK);
         pPager->state = PAGER_UNLOCK;
         return rc;
       }
       pPager->state = PAGER_EXCLUSIVE;

       /* Open the journal for reading only.  Return SQLITE_BUSY if
       ** we are unable to open the journal file. 
       **
       ** The journal file does not need to be locked itself.  The
       ** journal file is never open unless the main database file holds
       ** a write lock, so there is never any chance of two or more
       ** processes opening the journal at the same time.
       */
       rc = sqlite3OsOpenReadOnly(pPager->zJournal, &pPager->jfd);
       if( rc!=SQLITE_OK ){
         sqlite3OsUnlock(&pPager->fd, NO_LOCK);
         pPager->state = PAGER_UNLOCK;
         return SQLITE_BUSY;
       }
       pPager->journalOpen = 1;
       pPager->journalStarted = 0;
       pPager->journalOff = 0;
       pPager->setMaster = 0;
       pPager->journalHdr = 0;

       /* Playback and delete the journal.  Drop the database write
       ** lock and reacquire the read lock.
       */
       rc = pager_playback(pPager);
       if( rc!=SQLITE_OK ){
         return rc;
       }
    }
    pPg = 0;
  }else{
    /* Search for page in cache */
    pPg = pager_lookup(pPager, pgno);
    if( pPager->memDb && pPager->state==PAGER_UNLOCK ){
      pPager->state = PAGER_SHARED;
    }
  }
  if( pPg==0 ){
    /* The requested page is not in the page cache. */
    int h;
    pPager->nMiss++;
    if( pPager->nPage<pPager->mxPage || pPager->pFirst==0 || pPager->memDb ){
      /* Create a new page */
      pPg = sqliteMallocRaw( sizeof(*pPg) + pPager->pageSize 
                              + sizeof(u32) + pPager->nExtra
                              + pPager->memDb*sizeof(PgHistory) );
      if( pPg==0 ){
        if( !pPager->memDb ){
          pager_unwritelock(pPager);
        }
        pPager->errMask |= PAGER_ERR_MEM;
        return SQLITE_NOMEM;
      }
      memset(pPg, 0, sizeof(*pPg));
      if( pPager->memDb ){
        memset(PGHDR_TO_HIST(pPg, pPager), 0, sizeof(PgHistory));
      }
      pPg->pPager = pPager;
      pPg->pNextAll = pPager->pAll;
      pPager->pAll = pPg;
      pPager->nPage++;
    }else{
      /* Find a page to recycle.  Try to locate a page that does not
      ** require us to do an fsync() on the journal.
      */
      pPg = pPager->pFirstSynced;

      /* If we could not find a page that does not require an fsync()
      ** on the journal file then fsync the journal file.  This is a
      ** very slow operation, so we work hard to avoid it.  But sometimes
      ** it can't be helped.
      */
      if( pPg==0 ){
        int rc = syncJournal(pPager);
        if( rc!=0 ){
          sqlite3pager_rollback(pPager);
          return SQLITE_IOERR;
        }
        if( pPager->fullSync ){
          /* If in full-sync mode, write a new journal header into the
	  ** journal file. This is done to avoid ever modifying a journal
	  ** header that is involved in the rollback of pages that have
	  ** already been written to the database (in case the header is
	  ** trashed when the nRec field is updated).
          */
          pPager->nRec = 0;
          assert( pPager->journalOff > 0 );
          rc = writeJournalHdr(pPager);
          if( rc!=0 ){
            sqlite3pager_rollback(pPager);
            return SQLITE_IOERR;
          }
        }
        pPg = pPager->pFirst;
      }
      assert( pPg->nRef==0 );

      /* Write the page to the database file if it is dirty.
      */
      if( pPg->dirty ){
        assert( pPg->needSync==0 );
        pPg->pDirty = 0;
        rc = pager_write_pagelist( pPg );
        if( rc!=SQLITE_OK ){
          sqlite3pager_rollback(pPager);
          return SQLITE_IOERR;
        }
      }
      assert( pPg->dirty==0 );

      /* If the page we are recycling is marked as alwaysRollback, then
      ** set the global alwaysRollback flag, thus disabling the
      ** sqlite_dont_rollback() optimization for the rest of this transaction.
      ** It is necessary to do this because the page marked alwaysRollback
      ** might be reloaded at a later time but at that point we won't remember
      ** that is was marked alwaysRollback.  This means that all pages must
      ** be marked as alwaysRollback from here on out.
      */
      if( pPg->alwaysRollback ){
        pPager->alwaysRollback = 1;
      }

      /* Unlink the old page from the free list and the hash table
      */
      unlinkPage(pPg);
      pPager->nOvfl++;
    }
    pPg->pgno = pgno;
    if( pPager->aInJournal && (int)pgno<=pPager->origDbSize ){
      sqlite3CheckMemory(pPager->aInJournal, pgno/8);
      assert( pPager->journalOpen );
      pPg->inJournal = (pPager->aInJournal[pgno/8] & (1<<(pgno&7)))!=0;
      pPg->needSync = 0;
    }else{
      pPg->inJournal = 0;
      pPg->needSync = 0;
    }
    if( pPager->aInStmt && (int)pgno<=pPager->stmtSize
             && (pPager->aInStmt[pgno/8] & (1<<(pgno&7)))!=0 ){
      page_add_to_stmt_list(pPg);
    }else{
      page_remove_from_stmt_list(pPg);
    }
    pPg->dirty = 0;
    pPg->nRef = 1;
    REFINFO(pPg);
    pPager->nRef++;
    h = pager_hash(pgno);
    pPg->pNextHash = pPager->aHash[h];
    pPager->aHash[h] = pPg;
    if( pPg->pNextHash ){
      assert( pPg->pNextHash->pPrevHash==0 );
      pPg->pNextHash->pPrevHash = pPg;
    }
    if( pPager->nExtra>0 ){
      memset(PGHDR_TO_EXTRA(pPg, pPager), 0, pPager->nExtra);
    }
    sqlite3pager_pagecount(pPager);
    if( pPager->errMask!=0 ){
      sqlite3pager_unref(PGHDR_TO_DATA(pPg));
      rc = pager_errcode(pPager);
      return rc;
    }
    if( pPager->dbSize<(int)pgno ){
      memset(PGHDR_TO_DATA(pPg), 0, pPager->pageSize);
    }else{
      int rc;
      assert( pPager->memDb==0 );
      sqlite3OsSeek(&pPager->fd, (pgno-1)*(i64)pPager->pageSize);
      rc = sqlite3OsRead(&pPager->fd, PGHDR_TO_DATA(pPg), pPager->pageSize);
      TRACE3("FETCH %d page %d\n", pPager->fd.h, pPg->pgno);
      CODEC(pPager, PGHDR_TO_DATA(pPg), pPg->pgno, 3);
      if( rc!=SQLITE_OK ){
        i64 fileSize;
        if( sqlite3OsFileSize(&pPager->fd,&fileSize)!=SQLITE_OK
               || fileSize>=pgno*pPager->pageSize ){
          sqlite3pager_unref(PGHDR_TO_DATA(pPg));
          return rc;
        }else{
          memset(PGHDR_TO_DATA(pPg), 0, pPager->pageSize);
        }
      }
    }
  }else{
    /* The requested page is in the page cache. */
    pPager->nHit++;
    page_ref(pPg);
  }
  *ppPage = PGHDR_TO_DATA(pPg);
  return SQLITE_OK;
}

/*
** Acquire a page if it is already in the in-memory cache.  Do
** not read the page from disk.  Return a pointer to the page,
** or 0 if the page is not in cache.
**
** See also sqlite3pager_get().  The difference between this routine
** and sqlite3pager_get() is that _get() will go to the disk and read
** in the page if the page is not already in cache.  This routine
** returns NULL if the page is not in cache or if a disk I/O error 
** has ever happened.
*/
void *sqlite3pager_lookup(Pager *pPager, Pgno pgno){
  PgHdr *pPg;

  assert( pPager!=0 );
  assert( pgno!=0 );
  if( pPager->errMask & ~(PAGER_ERR_FULL) ){
    return 0;
  }
  pPg = pager_lookup(pPager, pgno);
  if( pPg==0 ) return 0;
  page_ref(pPg);
  return PGHDR_TO_DATA(pPg);
}

/*
** Release a page.
**
** If the number of references to the page drop to zero, then the
** page is added to the LRU list.  When all references to all pages
** are released, a rollback occurs and the lock on the database is
** removed.
*/
int sqlite3pager_unref(void *pData){
  PgHdr *pPg;

  /* Decrement the reference count for this page
  */
  pPg = DATA_TO_PGHDR(pData);
  assert( pPg->nRef>0 );
  pPg->nRef--;
  REFINFO(pPg);

  /* When the number of references to a page reach 0, call the
  ** destructor and add the page to the freelist.
  */
  if( pPg->nRef==0 ){
    Pager *pPager;
    pPager = pPg->pPager;
    pPg->pNextFree = 0;
    pPg->pPrevFree = pPager->pLast;
    pPager->pLast = pPg;
    if( pPg->pPrevFree ){
      pPg->pPrevFree->pNextFree = pPg;
    }else{
      pPager->pFirst = pPg;
    }
    if( pPg->needSync==0 && pPager->pFirstSynced==0 ){
      pPager->pFirstSynced = pPg;
    }
    if( pPager->xDestructor ){
      pPager->xDestructor(pData, pPager->pageSize);
    }
  
    /* When all pages reach the freelist, drop the read lock from
    ** the database file.
    */
    pPager->nRef--;
    assert( pPager->nRef>=0 );
    if( pPager->nRef==0 && !pPager->memDb ){
      pager_reset(pPager);
    }
  }
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
  int rc;
  assert( !pPager->memDb );
  assert( pPager->state>=PAGER_RESERVED );
  assert( pPager->journalOpen==0 );
  assert( pPager->useJournal );
  sqlite3pager_pagecount(pPager);
  pPager->aInJournal = sqliteMalloc( pPager->dbSize/8 + 1 );
  if( pPager->aInJournal==0 ){
    rc = SQLITE_NOMEM;
    goto failed_to_open_journal;
  }
  rc = sqlite3OsOpenExclusive(pPager->zJournal, &pPager->jfd,pPager->tempFile);
  pPager->journalOff = 0;
  pPager->setMaster = 0;
  pPager->journalHdr = 0;
  if( rc!=SQLITE_OK ){
    goto failed_to_open_journal;
  }
  sqlite3OsOpenDirectory(pPager->zDirectory, &pPager->jfd);
  pPager->journalOpen = 1;
  pPager->journalStarted = 0;
  pPager->needSync = 0;
  pPager->alwaysRollback = 0;
  pPager->nRec = 0;
  if( pPager->errMask!=0 ){
    rc = pager_errcode(pPager);
    return rc;
  }
  pPager->origDbSize = pPager->dbSize;

  rc = writeJournalHdr(pPager);

  if( pPager->stmtAutoopen && rc==SQLITE_OK ){
    rc = sqlite3pager_stmt_begin(pPager);
  }
  if( rc!=SQLITE_OK ){
    rc = pager_unwritelock(pPager);
    if( rc==SQLITE_OK ){
      rc = SQLITE_FULL;
    }
  }
  return rc;

failed_to_open_journal:
  sqliteFree(pPager->aInJournal);
  pPager->aInJournal = 0;
  sqlite3OsUnlock(&pPager->fd, NO_LOCK);
  pPager->state = PAGER_UNLOCK;
  return rc;
}

/*
** Acquire a write-lock on the database.  The lock is removed when
** the any of the following happen:
**
**   *  sqlite3pager_commit() is called.
**   *  sqlite3pager_rollback() is called.
**   *  sqlite3pager_close() is called.
**   *  sqlite3pager_unref() is called to on every outstanding page.
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
int sqlite3pager_begin(void *pData, int exFlag){
  PgHdr *pPg = DATA_TO_PGHDR(pData);
  Pager *pPager = pPg->pPager;
  int rc = SQLITE_OK;
  assert( pPg->nRef>0 );
  assert( pPager->state!=PAGER_UNLOCK );
  if( pPager->state==PAGER_SHARED ){
    assert( pPager->aInJournal==0 );
    if( pPager->memDb ){
      pPager->state = PAGER_EXCLUSIVE;
      pPager->origDbSize = pPager->dbSize;
    }else{
      if( SQLITE_BUSY_RESERVED_LOCK || exFlag ){
        rc = pager_wait_on_lock(pPager, RESERVED_LOCK);
      }else{
        rc = sqlite3OsLock(&pPager->fd, RESERVED_LOCK);
      }
      if( rc==SQLITE_OK ){
        pPager->state = PAGER_RESERVED;
        if( exFlag ){
          rc = pager_wait_on_lock(pPager, EXCLUSIVE_LOCK);
        }
      }
      if( rc!=SQLITE_OK ){
        return rc;
      }
      pPager->dirtyCache = 0;
      TRACE2("TRANSACTION %d\n", pPager->fd.h);
      if( pPager->useJournal && !pPager->tempFile ){
        rc = pager_open_journal(pPager);
      }
    }
  }
  return rc;
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
** is a call to sqlite3pager_commit() or sqlite3pager_rollback() to
** reset.
*/
int sqlite3pager_write(void *pData){
  PgHdr *pPg = DATA_TO_PGHDR(pData);
  Pager *pPager = pPg->pPager;
  int rc = SQLITE_OK;

  /* Check for errors
  */
  if( pPager->errMask ){ 
    return pager_errcode(pPager);
  }
  if( pPager->readOnly ){
    return SQLITE_PERM;
  }

  assert( !pPager->setMaster );

  /* Mark the page as dirty.  If the page has already been written
  ** to the journal then we can return right away.
  */
  pPg->dirty = 1;
  if( pPg->inJournal && (pPg->inStmt || pPager->stmtInUse==0) ){
    pPager->dirtyCache = 1;
    return SQLITE_OK;
  }

  /* If we get this far, it means that the page needs to be
  ** written to the transaction journal or the ckeckpoint journal
  ** or both.
  **
  ** First check to see that the transaction journal exists and
  ** create it if it does not.
  */
  assert( pPager->state!=PAGER_UNLOCK );
  rc = sqlite3pager_begin(pData, 0);
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
  if( !pPg->inJournal && (pPager->useJournal || pPager->memDb) ){
    if( (int)pPg->pgno <= pPager->origDbSize ){
      int szPg;
      u32 saved;
      if( pPager->memDb ){
        PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
        TRACE3("JOURNAL %d page %d\n", pPager->fd.h, pPg->pgno);
        assert( pHist->pOrig==0 );
        pHist->pOrig = sqliteMallocRaw( pPager->pageSize );
        if( pHist->pOrig ){
          memcpy(pHist->pOrig, PGHDR_TO_DATA(pPg), pPager->pageSize);
        }
      }else{
        u32 cksum;
        CODEC(pPager, pData, pPg->pgno, 7);
        cksum = pager_cksum(pPager, pPg->pgno, pData);
        saved = *(u32*)PGHDR_TO_EXTRA(pPg, pPager);
        store32bits(cksum, pPg, pPager->pageSize);
        szPg = pPager->pageSize+8;
        store32bits(pPg->pgno, pPg, -4);
        rc = sqlite3OsWrite(&pPager->jfd, &((char*)pData)[-4], szPg);
        pPager->journalOff += szPg;
        TRACE4("JOURNAL %d page %d needSync=%d\n",
                pPager->fd.h, pPg->pgno, pPg->needSync);
        CODEC(pPager, pData, pPg->pgno, 0);
        *(u32*)PGHDR_TO_EXTRA(pPg, pPager) = saved;
        if( rc!=SQLITE_OK ){
          sqlite3pager_rollback(pPager);
          pPager->errMask |= PAGER_ERR_FULL;
          return rc;
        }
        pPager->nRec++;
        assert( pPager->aInJournal!=0 );
        pPager->aInJournal[pPg->pgno/8] |= 1<<(pPg->pgno&7);
        pPg->needSync = !pPager->noSync;
        if( pPager->stmtInUse ){
          pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
          page_add_to_stmt_list(pPg);
        }
      }
    }else{
      pPg->needSync = !pPager->journalStarted && !pPager->noSync;
      TRACE4("APPEND %d page %d needSync=%d\n",
              pPager->fd.h, pPg->pgno, pPg->needSync);
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
  if( pPager->stmtInUse && !pPg->inStmt && (int)pPg->pgno<=pPager->stmtSize ){
    assert( pPg->inJournal || (int)pPg->pgno>pPager->origDbSize );
    if( pPager->memDb ){
      PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
      assert( pHist->pStmt==0 );
      pHist->pStmt = sqliteMallocRaw( pPager->pageSize );
      if( pHist->pStmt ){
        memcpy(pHist->pStmt, PGHDR_TO_DATA(pPg), pPager->pageSize);
      }
      TRACE3("STMT-JOURNAL %d page %d\n", pPager->fd.h, pPg->pgno);
    }else{
      store32bits(pPg->pgno, pPg, -4);
      CODEC(pPager, pData, pPg->pgno, 7);
      rc = sqlite3OsWrite(&pPager->stfd, ((char*)pData)-4, pPager->pageSize+4);
      TRACE3("STMT-JOURNAL %d page %d\n", pPager->fd.h, pPg->pgno);
      CODEC(pPager, pData, pPg->pgno, 0);
      if( rc!=SQLITE_OK ){
        sqlite3pager_rollback(pPager);
        pPager->errMask |= PAGER_ERR_FULL;
        return rc;
      }
      pPager->stmtNRec++;
      assert( pPager->aInStmt!=0 );
      pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
    }
    page_add_to_stmt_list(pPg);
  }

  /* Update the database size and return.
  */
  if( pPager->dbSize<(int)pPg->pgno ){
    pPager->dbSize = pPg->pgno;
    if( !pPager->memDb && pPager->dbSize==PENDING_BYTE/pPager->pageSize ){
      pPager->dbSize++;
    }
  }
  return rc;
}

/*
** Return TRUE if the page given in the argument was previously passed
** to sqlite3pager_write().  In other words, return TRUE if it is ok
** to change the content of the page.
*/
int sqlite3pager_iswriteable(void *pData){
  PgHdr *pPg = DATA_TO_PGHDR(pData);
  return pPg->dirty;
}

/*
** Replace the content of a single page with the information in the third
** argument.
*/
int sqlite3pager_overwrite(Pager *pPager, Pgno pgno, void *pData){
  void *pPage;
  int rc;

  rc = sqlite3pager_get(pPager, pgno, &pPage);
  if( rc==SQLITE_OK ){
    rc = sqlite3pager_write(pPage);
    if( rc==SQLITE_OK ){
      memcpy(pPage, pData, pPager->pageSize);
    }
    sqlite3pager_unref(pPage);
  }
  return rc;
}

/*
** A call to this routine tells the pager that it is not necessary to
** write the information on page "pgno" back to the disk, even though
** that page might be marked as dirty.
**
** The overlying software layer calls this routine when all of the data
** on the given page is unused.  The pager marks the page as clean so
** that it does not get written to disk.
**
** Tests show that this optimization, together with the
** sqlite3pager_dont_rollback() below, more than double the speed
** of large INSERT operations and quadruple the speed of large DELETEs.
**
** When this routine is called, set the alwaysRollback flag to true.
** Subsequent calls to sqlite3pager_dont_rollback() for the same page
** will thereafter be ignored.  This is necessary to avoid a problem
** where a page with data is added to the freelist during one part of
** a transaction then removed from the freelist during a later part
** of the same transaction and reused for some other purpose.  When it
** is first added to the freelist, this routine is called.  When reused,
** the dont_rollback() routine is called.  But because the page contains
** critical data, we still need to be sure it gets rolled back in spite
** of the dont_rollback() call.
*/
void sqlite3pager_dont_write(Pager *pPager, Pgno pgno){
  PgHdr *pPg;

  if( pPager->memDb ) return;

  pPg = pager_lookup(pPager, pgno);
  pPg->alwaysRollback = 1;
  if( pPg && pPg->dirty ){
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
      TRACE3("DONT_WRITE page %d of %d\n", pgno, pPager->fd.h);
      pPg->dirty = 0;
    }
  }
}

/*
** A call to this routine tells the pager that if a rollback occurs,
** it is not necessary to restore the data on the given page.  This
** means that the pager does not have to record the given page in the
** rollback journal.
*/
void sqlite3pager_dont_rollback(void *pData){
  PgHdr *pPg = DATA_TO_PGHDR(pData);
  Pager *pPager = pPg->pPager;

  if( pPager->state!=PAGER_EXCLUSIVE || pPager->journalOpen==0 ) return;
  if( pPg->alwaysRollback || pPager->alwaysRollback || pPager->memDb ) return;
  if( !pPg->inJournal && (int)pPg->pgno <= pPager->origDbSize ){
    assert( pPager->aInJournal!=0 );
    pPager->aInJournal[pPg->pgno/8] |= 1<<(pPg->pgno&7);
    pPg->inJournal = 1;
    if( pPager->stmtInUse ){
      pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
      page_add_to_stmt_list(pPg);
    }
    TRACE3("DONT_ROLLBACK page %d of %d\n", pPg->pgno, pPager->fd.h);
  }
  if( pPager->stmtInUse && !pPg->inStmt && (int)pPg->pgno<=pPager->stmtSize ){
    assert( pPg->inJournal || (int)pPg->pgno>pPager->origDbSize );
    assert( pPager->aInStmt!=0 );
    pPager->aInStmt[pPg->pgno/8] |= 1<<(pPg->pgno&7);
    page_add_to_stmt_list(pPg);
  }
}


/*
** Clear a PgHistory block
*/
static void clearHistory(PgHistory *pHist){
  sqliteFree(pHist->pOrig);
  sqliteFree(pHist->pStmt);
  pHist->pOrig = 0;
  pHist->pStmt = 0;
}

/*
** Commit all changes to the database and release the write lock.
**
** If the commit fails for any reason, a rollback attempt is made
** and an error code is returned.  If the commit worked, SQLITE_OK
** is returned.
*/
int sqlite3pager_commit(Pager *pPager){
  int rc;
  PgHdr *pPg;

  if( pPager->errMask==PAGER_ERR_FULL ){
    rc = sqlite3pager_rollback(pPager);
    if( rc==SQLITE_OK ){
      rc = SQLITE_FULL;
    }
    return rc;
  }
  if( pPager->errMask!=0 ){
    rc = pager_errcode(pPager);
    return rc;
  }
  if( pPager->state<PAGER_RESERVED ){
    return SQLITE_ERROR;
  }
  TRACE2("COMMIT %d\n", pPager->fd.h);
  if( pPager->memDb ){
    pPg = pager_get_all_dirty_pages(pPager);
    while( pPg ){
      clearHistory(PGHDR_TO_HIST(pPg, pPager));
      pPg->dirty = 0;
      pPg->inJournal = 0;
      pPg->inStmt = 0;
      pPg->pPrevStmt = pPg->pNextStmt = 0;
      pPg = pPg->pDirty;
    }
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
    return SQLITE_OK;
  }
  if( pPager->dirtyCache==0 ){
    /* Exit early (without doing the time-consuming sqlite3OsSync() calls)
    ** if there have been no changes to the database file. */
    assert( pPager->needSync==0 );
    rc = pager_unwritelock(pPager);
    pPager->dbSize = -1;
    return rc;
  }
  assert( pPager->journalOpen );
  rc = sqlite3pager_sync(pPager, 0);
  if( rc!=SQLITE_OK ){
    goto commit_abort;
  }
  rc = pager_unwritelock(pPager);
  pPager->dbSize = -1;
  return rc;

  /* Jump here if anything goes wrong during the commit process.
  */
commit_abort:
  sqlite3pager_rollback(pPager);
  return rc;
}

/*
** Rollback all changes.  The database falls back to PAGER_SHARED mode.
** All in-memory cache pages revert to their original data contents.
** The journal is deleted.
**
** This routine cannot fail unless some other process is not following
** the correct locking protocol (SQLITE_PROTOCOL) or unless some other
** process is writing trash into the journal file (SQLITE_CORRUPT) or
** unless a prior malloc() failed (SQLITE_NOMEM).  Appropriate error
** codes are returned for all these occasions.  Otherwise,
** SQLITE_OK is returned.
*/
int sqlite3pager_rollback(Pager *pPager){
  int rc;
  TRACE2("ROLLBACK %d\n", pPager->fd.h);
  if( pPager->memDb ){
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
        TRACE3("ROLLBACK-PAGE %d of %d\n", p->pgno, pPager->fd.h);
      }else{
        TRACE3("PAGE %d is clean on %d\n", p->pgno, pPager->fd.h);
      }
      clearHistory(pHist);
      p->dirty = 0;
      p->inJournal = 0;
      p->inStmt = 0;
      p->pPrevStmt = p->pNextStmt = 0;

      if( pPager->xReiniter ){
        pPager->xReiniter(PGHDR_TO_DATA(p), pPager->pageSize);
      }
      
    }
    pPager->pStmt = 0;
    pPager->dbSize = pPager->origDbSize;
    memoryTruncate(pPager);
    pPager->stmtInUse = 0;
    pPager->state = PAGER_SHARED;
    return SQLITE_OK;
  }

  if( !pPager->dirtyCache || !pPager->journalOpen ){
    rc = pager_unwritelock(pPager);
    pPager->dbSize = -1;
    return rc;
  }

  if( pPager->errMask!=0 && pPager->errMask!=PAGER_ERR_FULL ){
    if( pPager->state>=PAGER_EXCLUSIVE ){
      pager_playback(pPager);
    }
    return pager_errcode(pPager);
  }
  if( pPager->state==PAGER_RESERVED ){
    int rc2, rc3;
    rc = pager_reload_cache(pPager);
    rc2 = pager_truncate(pPager, pPager->origDbSize);
    rc3 = pager_unwritelock(pPager);
    if( rc==SQLITE_OK ){
      rc = rc2;
      if( rc3 ) rc = rc3;
    }
  }else{
    rc = pager_playback(pPager);
  }
  if( rc!=SQLITE_OK ){
    rc = SQLITE_CORRUPT;  /* bkpt-CORRUPT */
    pPager->errMask |= PAGER_ERR_CORRUPT;
  }
  pPager->dbSize = -1;
  return rc;
}

/*
** Return TRUE if the database file is opened read-only.  Return FALSE
** if the database is (in theory) writable.
*/
int sqlite3pager_isreadonly(Pager *pPager){
  return pPager->readOnly;
}

/*
** This routine is used for testing and analysis only.
*/
int *sqlite3pager_stats(Pager *pPager){
  static int a[9];
  a[0] = pPager->nRef;
  a[1] = pPager->nPage;
  a[2] = pPager->mxPage;
  a[3] = pPager->dbSize;
  a[4] = pPager->state;
  a[5] = pPager->errMask;
  a[6] = pPager->nHit;
  a[7] = pPager->nMiss;
  a[8] = pPager->nOvfl;
  return a;
}

/*
** Set the statement rollback point.
**
** This routine should be called with the transaction journal already
** open.  A new statement journal is created that can be used to rollback
** changes of a single SQL command within a larger transaction.
*/
int sqlite3pager_stmt_begin(Pager *pPager){
  int rc;
  char zTemp[SQLITE_TEMPNAME_SIZE];
  assert( !pPager->stmtInUse );
  assert( pPager->dbSize>=0 );
  TRACE2("STMT-BEGIN %d\n", pPager->fd.h);
  if( pPager->memDb ){
    pPager->stmtInUse = 1;
    pPager->stmtSize = pPager->dbSize;
    return SQLITE_OK;
  }
  if( !pPager->journalOpen ){
    pPager->stmtAutoopen = 1;
    return SQLITE_OK;
  }
  assert( pPager->journalOpen );
  pPager->aInStmt = sqliteMalloc( pPager->dbSize/8 + 1 );
  if( pPager->aInStmt==0 ){
    sqlite3OsLock(&pPager->fd, SHARED_LOCK);
    return SQLITE_NOMEM;
  }
#ifndef NDEBUG
  rc = sqlite3OsFileSize(&pPager->jfd, &pPager->stmtJSize);
  if( rc ) goto stmt_begin_failed;
  assert( pPager->stmtJSize == pPager->journalOff );
#endif
  pPager->stmtJSize = pPager->journalOff;
  pPager->stmtSize = pPager->dbSize;
  pPager->stmtHdrOff = 0;
  pPager->stmtCksum = pPager->cksumInit;
  if( !pPager->stmtOpen ){
    rc = sqlite3pager_opentemp(zTemp, &pPager->stfd);
    if( rc ) goto stmt_begin_failed;
    pPager->stmtOpen = 1;
    pPager->stmtNRec = 0;
  }
  pPager->stmtInUse = 1;
  return SQLITE_OK;
 
stmt_begin_failed:
  if( pPager->aInStmt ){
    sqliteFree(pPager->aInStmt);
    pPager->aInStmt = 0;
  }
  return rc;
}

/*
** Commit a statement.
*/
int sqlite3pager_stmt_commit(Pager *pPager){
  if( pPager->stmtInUse ){
    PgHdr *pPg, *pNext;
    TRACE2("STMT-COMMIT %d\n", pPager->fd.h);
    if( !pPager->memDb ){
      sqlite3OsSeek(&pPager->stfd, 0);
      /* sqlite3OsTruncate(&pPager->stfd, 0); */
      sqliteFree( pPager->aInStmt );
      pPager->aInStmt = 0;
    }
    for(pPg=pPager->pStmt; pPg; pPg=pNext){
      pNext = pPg->pNextStmt;
      assert( pPg->inStmt );
      pPg->inStmt = 0;
      pPg->pPrevStmt = pPg->pNextStmt = 0;
      if( pPager->memDb ){
        PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
        sqliteFree(pHist->pStmt);
        pHist->pStmt = 0;
      }
    }
    pPager->stmtNRec = 0;
    pPager->stmtInUse = 0;
    pPager->pStmt = 0;
  }
  pPager->stmtAutoopen = 0;
  return SQLITE_OK;
}

/*
** Rollback a statement.
*/
int sqlite3pager_stmt_rollback(Pager *pPager){
  int rc;
  if( pPager->stmtInUse ){
    TRACE2("STMT-ROLLBACK %d\n", pPager->fd.h);
    if( pPager->memDb ){
      PgHdr *pPg;
      for(pPg=pPager->pStmt; pPg; pPg=pPg->pNextStmt){
        PgHistory *pHist = PGHDR_TO_HIST(pPg, pPager);
        if( pHist->pStmt ){
          memcpy(PGHDR_TO_DATA(pPg), pHist->pStmt, pPager->pageSize);
          sqliteFree(pHist->pStmt);
          pHist->pStmt = 0;
        }
      }
      pPager->dbSize = pPager->stmtSize;
      memoryTruncate(pPager);
      rc = SQLITE_OK;
    }else{
      rc = pager_stmt_playback(pPager);
    }
    sqlite3pager_stmt_commit(pPager);
  }else{
    rc = SQLITE_OK;
  }
  pPager->stmtAutoopen = 0;
  return rc;
}

/*
** Return the full pathname of the database file.
*/
const char *sqlite3pager_filename(Pager *pPager){
  return pPager->zFilename;
}

/*
** Return the directory of the database file.
*/
const char *sqlite3pager_dirname(Pager *pPager){
  return pPager->zDirectory;
}

/*
** Return the full pathname of the journal file.
*/
const char *sqlite3pager_journalname(Pager *pPager){
  return pPager->zJournal;
}

/*
** Set the codec for this pager
*/
void sqlite3pager_set_codec(
  Pager *pPager,
  void (*xCodec)(void*,void*,Pgno,int),
  void *pCodecArg
){
  pPager->xCodec = xCodec;
  pPager->pCodecArg = pCodecArg;
}

/*
** This routine is called to increment the database file change-counter,
** stored at byte 24 of the pager file.
*/
static int pager_incr_changecounter(Pager *pPager){
  void *pPage;
  PgHdr *pPgHdr;
  u32 change_counter;
  int rc;

  /* Open page 1 of the file for writing. */
  rc = sqlite3pager_get(pPager, 1, &pPage);
  if( rc!=SQLITE_OK ) return rc;
  rc = sqlite3pager_write(pPage);
  if( rc!=SQLITE_OK ) return rc;

  /* Read the current value at byte 24. */
  pPgHdr = DATA_TO_PGHDR(pPage);
  change_counter = retrieve32bits(pPgHdr, 24);

  /* Increment the value just read and write it back to byte 24. */
  change_counter++;
  store32bits(change_counter, pPgHdr, 24);

  /* Release the page reference. */
  sqlite3pager_unref(pPage);
  return SQLITE_OK;
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
** passed to an sqlite3pager_sync() call.
*/
int sqlite3pager_sync(Pager *pPager, const char *zMaster){
  int rc = SQLITE_OK;

  /* If this is an in-memory db, or no pages have been written to, or this
  ** function has already been called, it is a no-op.
  */
  if( pPager->state!=PAGER_SYNCED && !pPager->memDb && pPager->dirtyCache ){
    PgHdr *pPg;
    assert( pPager->journalOpen );

    /* If a master journal file name has already been written to the
    ** journal file, then no sync is required. This happens when it is
    ** written, then the process fails to upgrade from a RESERVED to an
    ** EXCLUSIVE lock. The next time the process tries to commit the
    ** transaction the m-j name will have already been written.
    */
    if( !pPager->setMaster ){
      rc = pager_incr_changecounter(pPager);
      if( rc!=SQLITE_OK ) goto sync_exit;
      rc = writeMasterJournal(pPager, zMaster);
      if( rc!=SQLITE_OK ) goto sync_exit;
      rc = syncJournal(pPager);
      if( rc!=SQLITE_OK ) goto sync_exit;
    }

    /* Write all dirty pages to the database file */
    pPg = pager_get_all_dirty_pages(pPager);
    rc = pager_write_pagelist(pPg);
    if( rc!=SQLITE_OK ) goto sync_exit;

    /* Sync the database file. */
    if( !pPager->noSync ){
      rc = sqlite3OsSync(&pPager->fd);
    }

    pPager->state = PAGER_SYNCED;
  }

sync_exit:
  return rc;
}

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
/*
** Return the current state of the file lock for the given pager.
** The return value is one of NO_LOCK, SHARED_LOCK, RESERVED_LOCK,
** PENDING_LOCK, or EXCLUSIVE_LOCK.
*/
int sqlite3pager_lockstate(Pager *pPager){
#ifdef OS_TEST
  return pPager->fd->fd.locktype;
#else
  return pPager->fd.locktype;
#endif
}
#endif

#ifdef SQLITE_TEST
/*
** Print a listing of all referenced pages and their ref count.
*/
void sqlite3pager_refdump(Pager *pPager){
  PgHdr *pPg;
  for(pPg=pPager->pAll; pPg; pPg=pPg->pNextAll){
    if( pPg->nRef<=0 ) continue;
    sqlite3DebugPrintf("PAGE %3d addr=%p nRef=%d\n", 
       pPg->pgno, PGHDR_TO_DATA(pPg), pPg->nRef);
  }
}
#endif
