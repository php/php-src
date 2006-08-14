/*
** 2005 November 29
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
** This file contains OS interface code that is common to all
** architectures.
*/
#define _SQLITE_OS_C_ 1
#include "sqliteInt.h"
#include "os.h"

/*
** The following routines are convenience wrappers around methods
** of the OsFile object.  This is mostly just syntactic sugar.  All
** of this would be completely automatic if SQLite were coded using
** C++ instead of plain old C.
*/
int sqlite3OsClose(OsFile **pId){
  OsFile *id;
  if( pId!=0 && (id = *pId)!=0 ){
    return id->pMethod->xClose(pId);
  }else{
    return SQLITE_OK;
  }
}
int sqlite3OsOpenDirectory(OsFile *id, const char *zName){
  return id->pMethod->xOpenDirectory(id, zName);
}
int sqlite3OsRead(OsFile *id, void *pBuf, int amt){
  return id->pMethod->xRead(id, pBuf, amt);
}
int sqlite3OsWrite(OsFile *id, const void *pBuf, int amt){
  return id->pMethod->xWrite(id, pBuf, amt);
}
int sqlite3OsSeek(OsFile *id, i64 offset){
  return id->pMethod->xSeek(id, offset);
}
int sqlite3OsTruncate(OsFile *id, i64 size){
  return id->pMethod->xTruncate(id, size);
}
int sqlite3OsSync(OsFile *id, int fullsync){
  return id->pMethod->xSync(id, fullsync);
}
void sqlite3OsSetFullSync(OsFile *id, int value){
  id->pMethod->xSetFullSync(id, value);
}
#if defined(SQLITE_TEST) || defined(SQLITE_DEBUG)
/* This method is currently only used while interactively debugging the 
** pager. More specificly, it can only be used when sqlite3DebugPrintf() is
** included in the build. */
int sqlite3OsFileHandle(OsFile *id){
  return id->pMethod->xFileHandle(id);
}
#endif
int sqlite3OsFileSize(OsFile *id, i64 *pSize){
  return id->pMethod->xFileSize(id, pSize);
}
int sqlite3OsLock(OsFile *id, int lockType){
  return id->pMethod->xLock(id, lockType);
}
int sqlite3OsUnlock(OsFile *id, int lockType){
  return id->pMethod->xUnlock(id, lockType);
}
int sqlite3OsLockState(OsFile *id){
  return id->pMethod->xLockState(id);
}
int sqlite3OsCheckReservedLock(OsFile *id){
  return id->pMethod->xCheckReservedLock(id);
}

#ifdef SQLITE_ENABLE_REDEF_IO
/*
** A function to return a pointer to the virtual function table.
** This routine really does not accomplish very much since the
** virtual function table is a global variable and anybody who
** can call this function can just as easily access the variable
** for themselves.  Nevertheless, we include this routine for
** backwards compatibility with an earlier redefinable I/O
** interface design.
*/
struct sqlite3OsVtbl *sqlite3_os_switch(void){
  return &sqlite3Os;
}
#endif
