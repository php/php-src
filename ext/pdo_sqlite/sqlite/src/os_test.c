/*
** 2004 May 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
******************************************************************************
**
** This file contains code that is specific to Unix systems. It is used
** for testing SQLite only.
*/
#if OS_TEST              /* This file is used for the test backend only */
#include "sqliteInt.h"
#include "os.h"          /* Must be first to enable large file support */

#define sqlite3OsOpenReadWrite     sqlite3RealOpenReadWrite
#define sqlite3OsOpenExclusive     sqlite3RealOpenExclusive
#define sqlite3OsOpenReadOnly      sqlite3RealOpenReadOnly
#define sqlite3OsOpenDirectory     sqlite3RealOpenDirectory
#define sqlite3OsClose             sqlite3RealClose
#define sqlite3OsRead              sqlite3RealRead
#define sqlite3OsWrite             sqlite3RealWrite
#define sqlite3OsSeek              sqlite3RealSeek
#define sqlite3OsSync              sqlite3RealSync
#define sqlite3OsTruncate          sqlite3RealTruncate
#define sqlite3OsFileSize          sqlite3RealFileSize
#define sqlite3OsLock              sqlite3RealLock
#define sqlite3OsUnlock            sqlite3RealUnlock
#define sqlite3OsCheckReservedLock sqlite3RealCheckReservedLock

#define OsFile OsRealFile
#define OS_UNIX 1
#include "os_unix.c"
#undef OS_UNIX
#undef OsFile

#undef sqlite3OsOpenReadWrite     
#undef sqlite3OsOpenExclusive     
#undef sqlite3OsOpenReadOnly      
#undef sqlite3OsOpenDirectory     
#undef sqlite3OsClose             
#undef sqlite3OsRead              
#undef sqlite3OsWrite             
#undef sqlite3OsSeek              
#undef sqlite3OsSync              
#undef sqlite3OsTruncate          
#undef sqlite3OsFileSize          
#undef sqlite3OsLock              
#undef sqlite3OsUnlock            
#undef sqlite3OsCheckReservedLock 

#define BLOCKSIZE 512
#define BLOCK_OFFSET(x) ((x) * BLOCKSIZE)


/*
** The following variables control when a simulated crash occurs.
**
** If iCrashDelay is non-zero, then zCrashFile contains (full path) name of
** a file that SQLite will call sqlite3OsSync() on. Each time this happens
** iCrashDelay is decremented. If iCrashDelay is zero after being
** decremented, a "crash" occurs during the sync() operation.
**
** In other words, a crash occurs the iCrashDelay'th time zCrashFile is
** synced.
*/
static int iCrashDelay = 0;
char zCrashFile[256];

/*
** Set the value of the two crash parameters.
*/
void sqlite3SetCrashParams(int iDelay, char const *zFile){
  sqlite3OsEnterMutex();
  assert( strlen(zFile)<256 );
  strcpy(zCrashFile, zFile);
  iCrashDelay = iDelay;
  sqlite3OsLeaveMutex();
}

/*
** File zPath is being sync()ed. Return non-zero if this should
** cause a crash.
*/
static int crashRequired(char const *zPath){
  int r;
  int n;
  sqlite3OsEnterMutex();
  n = strlen(zCrashFile);
  if( zCrashFile[n-1]=='*' ){
    n--;
  }else if( strlen(zPath)>n ){
    n = strlen(zPath);
  }
  r = 0;
  if( iCrashDelay>0 && strncmp(zPath, zCrashFile, n)==0 ){
    iCrashDelay--;
    if( iCrashDelay<=0 ){
      r = 1;
    }
  }
  sqlite3OsLeaveMutex();
  return r;
}


static OsTestFile *pAllFiles = 0;

/*
** Initialise the os_test.c specific fields of pFile.
*/
static void initFile(OsFile *id, char const *zName){
  OsTestFile *pFile = (OsTestFile *)
      sqliteMalloc(sizeof(OsTestFile) + strlen(zName)+1);
  pFile->nMaxWrite = 0; 
  pFile->nBlk = 0; 
  pFile->apBlk = 0; 
  pFile->zName = (char *)(&pFile[1]);
  strcpy(pFile->zName, zName);
  *id = pFile;
  pFile->pNext = pAllFiles;
  pAllFiles = pFile;
}

/*
** Undo the work done by initFile. Delete the OsTestFile structure
** and unlink the structure from the pAllFiles list.
*/
static void closeFile(OsFile *id){
  OsTestFile *pFile = *id;
  if( pFile==pAllFiles ){
    pAllFiles = pFile->pNext;
  }else{
    OsTestFile *p;
    for(p=pAllFiles; p->pNext!=pFile; p=p->pNext ){
      assert( p );
    }
    p->pNext = pFile->pNext;
  }
  sqliteFree(pFile);
  *id = 0;
}

/*
** Return the current seek offset from the start of the file. This
** is unix-only code.
*/
static i64 osTell(OsTestFile *pFile){
  return lseek(pFile->fd.h, 0, SEEK_CUR);
}

/*
** Load block 'blk' into the cache of pFile.
*/
static int cacheBlock(OsTestFile *pFile, int blk){
  if( blk>=pFile->nBlk ){
    int n = ((pFile->nBlk * 2) + 100 + blk);
    /* if( pFile->nBlk==0 ){ printf("DIRTY %s\n", pFile->zName); } */
    pFile->apBlk = (u8 **)sqliteRealloc(pFile->apBlk, n * sizeof(u8*));
    if( !pFile->apBlk ) return SQLITE_NOMEM;
    memset(&pFile->apBlk[pFile->nBlk], 0, (n - pFile->nBlk)*sizeof(u8*));
    pFile->nBlk = n;
  }

  if( !pFile->apBlk[blk] ){
    i64 filesize;
    int rc;

    u8 *p = sqliteMalloc(BLOCKSIZE);
    if( !p ) return SQLITE_NOMEM;
    pFile->apBlk[blk] = p;

    rc = sqlite3RealFileSize(&pFile->fd, &filesize);
    if( rc!=SQLITE_OK ) return rc;

    if( BLOCK_OFFSET(blk)<filesize ){
      int len = BLOCKSIZE;
      rc = sqlite3RealSeek(&pFile->fd, blk*BLOCKSIZE);
      if( BLOCK_OFFSET(blk+1)>filesize ){
        len = filesize - BLOCK_OFFSET(blk);
      }
      if( rc!=SQLITE_OK ) return rc;
      rc = sqlite3RealRead(&pFile->fd, p, len);
      if( rc!=SQLITE_OK ) return rc;
    }
  }

  return SQLITE_OK;
}

/* #define TRACE_WRITECACHE */

/*
** Write the cache of pFile to disk. If crash is non-zero, randomly
** skip blocks when writing. The cache is deleted before returning.
*/
static int writeCache2(OsTestFile *pFile, int crash){
  int i;
  int nMax = pFile->nMaxWrite;
  i64 offset;
  int rc = SQLITE_OK;

  offset = osTell(pFile);
  for(i=0; i<pFile->nBlk; i++){
    u8 *p = pFile->apBlk[i];
    if( p ){
      int skip = 0;
      int trash = 0;
      if( crash ){
        char random;
        sqlite3Randomness(1, &random);
        if( random & 0x01 ){
          if( random & 0x02 ){
            trash = 1;
#ifdef TRACE_WRITECACHE
printf("Trashing block %d of %s\n", i, pFile->zName); 
#endif
          }else{
            skip = 1;
#ifdef TRACE_WRITECACHE
printf("Skiping block %d of %s\n", i, pFile->zName); 
#endif
          }
        }else{
#ifdef TRACE_WRITECACHE
printf("Writing block %d of %s\n", i, pFile->zName); 
#endif
        }
      }
      if( rc==SQLITE_OK ){
        rc = sqlite3RealSeek(&pFile->fd, BLOCK_OFFSET(i));
      }
      if( rc==SQLITE_OK && !skip ){
        int len = BLOCKSIZE;
        if( BLOCK_OFFSET(i+1)>nMax ){
          len = nMax-BLOCK_OFFSET(i);
        }
        if( len>0 ){
          if( trash ){
            sqlite3Randomness(len, p);
          }
          rc = sqlite3RealWrite(&pFile->fd, p, len);
        }
      }
      sqliteFree(p);
    }
  }
  sqliteFree(pFile->apBlk);
  pFile->nBlk = 0;
  pFile->apBlk = 0;
  pFile->nMaxWrite = 0;

  if( rc==SQLITE_OK ){
    rc = sqlite3RealSeek(&pFile->fd, offset);
  }
  return rc;
}

/*
** Write the cache to disk.
*/
static int writeCache(OsTestFile *pFile){
  if( pFile->apBlk ){
    int c = crashRequired(pFile->zName);
    if( c ){
      OsTestFile *p;
#ifdef TRACE_WRITECACHE
      printf("\nCrash during sync of %s\n", pFile->zName);
#endif
      for(p=pAllFiles; p; p=p->pNext){
        writeCache2(p, 1);
      }
      exit(-1);
    }else{
      return writeCache2(pFile, 0);
    }
  }
  return SQLITE_OK;
}

/*
** Close the file.
*/
int sqlite3OsClose(OsFile *id){
  if( !(*id) ) return SQLITE_OK;
  if( (*id)->fd.isOpen ){
    /* printf("CLOSE %s (%d blocks)\n", (*id)->zName, (*id)->nBlk); */
    writeCache(*id);
    sqlite3RealClose(&(*id)->fd);
  }
  closeFile(id);
  return SQLITE_OK;
}

int sqlite3OsRead(OsFile *id, void *pBuf, int amt){
  i64 offset;       /* The current offset from the start of the file */
  i64 end;          /* The byte just past the last byte read */
  int blk;            /* Block number the read starts on */
  int i;
  u8 *zCsr;
  int rc = SQLITE_OK;
  OsTestFile *pFile = *id;

  offset = osTell(pFile);
  end = offset+amt;
  blk = (offset/BLOCKSIZE);

  zCsr = (u8 *)pBuf;
  for(i=blk; i*BLOCKSIZE<end; i++){
    int off = 0;
    int len = 0;


    if( BLOCK_OFFSET(i) < offset ){
      off = offset-BLOCK_OFFSET(i);
    }
    len = BLOCKSIZE - off;
    if( BLOCK_OFFSET(i+1) > end ){
      len = len - (BLOCK_OFFSET(i+1)-end);
    }

    if( i<pFile->nBlk && pFile->apBlk[i]){
      u8 *pBlk = pFile->apBlk[i];
      memcpy(zCsr, &pBlk[off], len);
    }else{
      rc = sqlite3RealSeek(&pFile->fd, BLOCK_OFFSET(i) + off);
      if( rc!=SQLITE_OK ) return rc;
      rc = sqlite3RealRead(&pFile->fd, zCsr, len);
      if( rc!=SQLITE_OK ) return rc;
    }

    zCsr += len;
  }
  assert( zCsr==&((u8 *)pBuf)[amt] );

  rc = sqlite3RealSeek(&pFile->fd, end);
  return rc;
}

int sqlite3OsWrite(OsFile *id, const void *pBuf, int amt){
  i64 offset;       /* The current offset from the start of the file */
  i64 end;          /* The byte just past the last byte written */
  int blk;            /* Block number the write starts on */
  int i;
  const u8 *zCsr;
  int rc = SQLITE_OK;
  OsTestFile *pFile = *id;

  offset = osTell(pFile);
  end = offset+amt;
  blk = (offset/BLOCKSIZE);

  zCsr = (u8 *)pBuf;
  for(i=blk; i*BLOCKSIZE<end; i++){
    u8 *pBlk;
    int off = 0;
    int len = 0;

    /* Make sure the block is in the cache */
    rc = cacheBlock(pFile, i);
    if( rc!=SQLITE_OK ) return rc;

    /* Write into the cache */
    pBlk = pFile->apBlk[i];
    assert( pBlk );

    if( BLOCK_OFFSET(i) < offset ){
      off = offset-BLOCK_OFFSET(i);
    }
    len = BLOCKSIZE - off;
    if( BLOCK_OFFSET(i+1) > end ){
      len = len - (BLOCK_OFFSET(i+1)-end);
    }
    memcpy(&pBlk[off], zCsr, len);
    zCsr += len;
  }
  if( pFile->nMaxWrite<end ){
    pFile->nMaxWrite = end;
  }
  assert( zCsr==&((u8 *)pBuf)[amt] );

  rc = sqlite3RealSeek(&pFile->fd, end);
  return rc;
}

/*
** Sync the file. First flush the write-cache to disk, then call the
** real sync() function.
*/
int sqlite3OsSync(OsFile *id){
  int rc;
  /* printf("SYNC %s (%d blocks)\n", (*id)->zName, (*id)->nBlk); */
  rc = writeCache(*id);
  if( rc!=SQLITE_OK ) return rc;
  rc = sqlite3RealSync(&(*id)->fd);
  return rc;
}

/*
** Truncate the file. Set the internal OsFile.nMaxWrite variable to the new
** file size to ensure that nothing in the write-cache past this point
** is written to disk.
*/
int sqlite3OsTruncate(OsFile *id, i64 nByte){
  (*id)->nMaxWrite = nByte;
  return sqlite3RealTruncate(&(*id)->fd, nByte);
}

/*
** Return the size of the file. If the cache contains a write that extended
** the file, then return this size instead of the on-disk size.
*/
int sqlite3OsFileSize(OsFile *id, i64 *pSize){
  int rc = sqlite3RealFileSize(&(*id)->fd, pSize);
  if( rc==SQLITE_OK && pSize && *pSize<(*id)->nMaxWrite ){
    *pSize = (*id)->nMaxWrite;
  }
  return rc;
}

/*
** The three functions used to open files. All that is required is to
** initialise the os_test.c specific fields and then call the corresponding
** os_unix.c function to really open the file.
*/
int sqlite3OsOpenReadWrite(const char *zFilename, OsFile *id, int *pReadonly){
  initFile(id, zFilename);
  return sqlite3RealOpenReadWrite(zFilename, &(*id)->fd, pReadonly);
}
int sqlite3OsOpenExclusive(const char *zFilename, OsFile *id, int delFlag){
  initFile(id, zFilename);
  return sqlite3RealOpenExclusive(zFilename, &(*id)->fd, delFlag);
}
int sqlite3OsOpenReadOnly(const char *zFilename, OsFile *id){
  initFile(id, zFilename);
  return sqlite3RealOpenReadOnly(zFilename, &(*id)->fd);
}

/*
** These six function calls are passed straight through to the os_unix.c
** backend.
*/
int sqlite3OsSeek(OsFile *id, i64 offset){
  return sqlite3RealSeek(&(*id)->fd, offset);
}
int sqlite3OsCheckReservedLock(OsFile *id){
  return sqlite3RealCheckReservedLock(&(*id)->fd);
}
int sqlite3OsLock(OsFile *id, int locktype){
  return sqlite3RealLock(&(*id)->fd, locktype);
}
int sqlite3OsUnlock(OsFile *id, int locktype){
  return sqlite3RealUnlock(&(*id)->fd, locktype);
}
int sqlite3OsOpenDirectory(const char *zDirname, OsFile *id){
  return sqlite3RealOpenDirectory(zDirname, &(*id)->fd);
}

#endif /* OS_TEST */
