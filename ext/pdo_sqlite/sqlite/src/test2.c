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
** Code for testing the pager.c module in SQLite.  This code
** is not included in the SQLite library.  It is used for automated
** testing of the SQLite library.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"
#include "pager.h"
#include "tcl.h"
#include <stdlib.h>
#include <string.h>

/*
** Interpret an SQLite error number
*/
static char *errorName(int rc){
  char *zName;
  switch( rc ){
    case SQLITE_OK:         zName = "SQLITE_OK";          break;
    case SQLITE_ERROR:      zName = "SQLITE_ERROR";       break;
    case SQLITE_INTERNAL:   zName = "SQLITE_INTERNAL";    break;
    case SQLITE_PERM:       zName = "SQLITE_PERM";        break;
    case SQLITE_ABORT:      zName = "SQLITE_ABORT";       break;
    case SQLITE_BUSY:       zName = "SQLITE_BUSY";        break;
    case SQLITE_NOMEM:      zName = "SQLITE_NOMEM";       break;
    case SQLITE_READONLY:   zName = "SQLITE_READONLY";    break;
    case SQLITE_INTERRUPT:  zName = "SQLITE_INTERRUPT";   break;
    case SQLITE_IOERR:      zName = "SQLITE_IOERR";       break;
    case SQLITE_CORRUPT:    zName = "SQLITE_CORRUPT";     break;
    case SQLITE_NOTFOUND:   zName = "SQLITE_NOTFOUND";    break;
    case SQLITE_FULL:       zName = "SQLITE_FULL";        break;
    case SQLITE_CANTOPEN:   zName = "SQLITE_CANTOPEN";    break;
    case SQLITE_PROTOCOL:   zName = "SQLITE_PROTOCOL";    break;
    case SQLITE_EMPTY:      zName = "SQLITE_EMPTY";       break;
    case SQLITE_SCHEMA:     zName = "SQLITE_SCHEMA";      break;
    case SQLITE_TOOBIG:     zName = "SQLITE_TOOBIG";      break;
    case SQLITE_CONSTRAINT: zName = "SQLITE_CONSTRAINT";  break;
    case SQLITE_MISMATCH:   zName = "SQLITE_MISMATCH";    break;
    case SQLITE_MISUSE:     zName = "SQLITE_MISUSE";      break;
    case SQLITE_NOLFS:      zName = "SQLITE_NOLFS";       break;
    default:                zName = "SQLITE_Unknown";     break;
  }
  return zName;
}

/*
** Page size and reserved size used for testing.
*/
static int test_pagesize = 1024;

/*
** Usage:   pager_open FILENAME N-PAGE
**
** Open a new pager
*/
static int pager_open(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int nPage;
  int rc;
  char zBuf[100];
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME N-PAGE\"", 0);
    return TCL_ERROR;
  }
  if( Tcl_GetInt(interp, argv[2], &nPage) ) return TCL_ERROR;
  rc = sqlite3pager_open(&pPager, argv[1], 0, 1);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  sqlite3pager_set_cachesize(pPager, nPage);
  sqlite3pager_set_pagesize(pPager, test_pagesize);
  sqlite3_snprintf(sizeof(zBuf),zBuf,"%p",pPager);
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage:   pager_close ID
**
** Close the given pager.
*/
static int pager_close(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_close(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_rollback ID
**
** Rollback changes
*/
static int pager_rollback(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_rollback(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_commit ID
**
** Commit all changes
*/
static int pager_commit(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_commit(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_stmt_begin ID
**
** Start a new checkpoint.
*/
static int pager_stmt_begin(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_stmt_begin(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_stmt_rollback ID
**
** Rollback changes to a checkpoint
*/
static int pager_stmt_rollback(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_stmt_rollback(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_stmt_commit ID
**
** Commit changes to a checkpoint
*/
static int pager_stmt_commit(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_stmt_commit(pPager);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   pager_stats ID
**
** Return pager statistics.
*/
static int pager_stats(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  int i, *a;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  a = sqlite3pager_stats(pPager);
  for(i=0; i<9; i++){
    static char *zName[] = {
      "ref", "page", "max", "size", "state", "err",
      "hit", "miss", "ovfl",
    };
    char zBuf[100];
    Tcl_AppendElement(interp, zName[i]);
    sqlite3_snprintf(sizeof(zBuf),zBuf,"%d",a[i]);
    Tcl_AppendElement(interp, zBuf);
  }
  return TCL_OK;
}

/*
** Usage:   pager_pagecount ID
**
** Return the size of the database file.
*/
static int pager_pagecount(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  char zBuf[100];
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  sqlite3_snprintf(sizeof(zBuf),zBuf,"%d",sqlite3pager_pagecount(pPager));
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage:   page_get ID PGNO
**
** Return a pointer to a page from the database.
*/
static int page_get(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  char zBuf[100];
  void *pPage;
  int pgno;
  int rc;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID PGNO\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  if( Tcl_GetInt(interp, argv[2], &pgno) ) return TCL_ERROR;
  rc = sqlite3pager_get(pPager, pgno, &pPage);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  sqlite3_snprintf(sizeof(zBuf),zBuf,"%p",pPage);
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage:   page_lookup ID PGNO
**
** Return a pointer to a page if the page is already in cache.
** If not in cache, return an empty string.
*/
static int page_lookup(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  Pager *pPager;
  char zBuf[100];
  void *pPage;
  int pgno;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " ID PGNO\"", 0);
    return TCL_ERROR;
  }
  pPager = sqlite3TextToPtr(argv[1]);
  if( Tcl_GetInt(interp, argv[2], &pgno) ) return TCL_ERROR;
  pPage = sqlite3pager_lookup(pPager, pgno);
  if( pPage ){
    sqlite3_snprintf(sizeof(zBuf),zBuf,"%p",pPage);
    Tcl_AppendResult(interp, zBuf, 0);
  }
  return TCL_OK;
}

/*
** Usage:   page_unref PAGE
**
** Drop a pointer to a page.
*/
static int page_unref(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  void *pPage;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " PAGE\"", 0);
    return TCL_ERROR;
  }
  pPage = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_unref(pPage);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:   page_read PAGE
**
** Return the content of a page
*/
static int page_read(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  char zBuf[100];
  void *pPage;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " PAGE\"", 0);
    return TCL_ERROR;
  }
  pPage = sqlite3TextToPtr(argv[1]);
  memcpy(zBuf, pPage, sizeof(zBuf));
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage:   page_number PAGE
**
** Return the page number for a page.
*/
static int page_number(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  char zBuf[100];
  void *pPage;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " PAGE\"", 0);
    return TCL_ERROR;
  }
  pPage = sqlite3TextToPtr(argv[1]);
  sqlite3_snprintf(sizeof(zBuf), zBuf, "%d", sqlite3pager_pagenumber(pPage));
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage:   page_write PAGE DATA
**
** Write something into a page.
*/
static int page_write(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  void *pPage;
  int rc;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " PAGE DATA\"", 0);
    return TCL_ERROR;
  }
  pPage = sqlite3TextToPtr(argv[1]);
  rc = sqlite3pager_write(pPage);
  if( rc!=SQLITE_OK ){
    Tcl_AppendResult(interp, errorName(rc), 0);
    return TCL_ERROR;
  }
  strncpy((char*)pPage, argv[2], test_pagesize-1);
  ((char*)pPage)[test_pagesize-1] = 0;
  return TCL_OK;
}

/*
** Usage:   fake_big_file  N  FILENAME
**
** Write a few bytes at the N megabyte point of FILENAME.  This will
** create a large file.  If the file was a valid SQLite database, then
** the next time the database is opened, SQLite will begin allocating
** new pages after N.  If N is 2096 or bigger, this will test the
** ability of SQLite to write to large files.
*/
static int fake_big_file(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  const char **argv      /* Text of each argument */
){
  int rc;
  int n;
  i64 offset;
  OsFile fd;
  int readOnly = 0;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " N-MEGABYTES FILE\"", 0);
    return TCL_ERROR;
  }
  if( Tcl_GetInt(interp, argv[1], &n) ) return TCL_ERROR;
  memset(&fd, 0, sizeof(fd));
  rc = sqlite3OsOpenReadWrite(argv[2], &fd, &readOnly);
  if( rc ){
    Tcl_AppendResult(interp, "open failed: ", errorName(rc), 0);
    return TCL_ERROR;
  }
  offset = n;
  offset *= 1024*1024;
  rc = sqlite3OsSeek(&fd, offset);
  if( rc ){
    Tcl_AppendResult(interp, "seek failed: ", errorName(rc), 0);
    return TCL_ERROR;
  }
  rc = sqlite3OsWrite(&fd, "Hello, World!", 14);
  sqlite3OsClose(&fd);
  if( rc ){
    Tcl_AppendResult(interp, "write failed: ", errorName(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Register commands with the TCL interpreter.
*/
int Sqlitetest2_Init(Tcl_Interp *interp){
  extern int sqlite3_io_error_pending;
  extern int sqlite3_diskfull_pending;
  static struct {
    char *zName;
    Tcl_CmdProc *xProc;
  } aCmd[] = {
    { "pager_open",              (Tcl_CmdProc*)pager_open          },
    { "pager_close",             (Tcl_CmdProc*)pager_close         },
    { "pager_commit",            (Tcl_CmdProc*)pager_commit        },
    { "pager_rollback",          (Tcl_CmdProc*)pager_rollback      },
    { "pager_stmt_begin",        (Tcl_CmdProc*)pager_stmt_begin    },
    { "pager_stmt_commit",       (Tcl_CmdProc*)pager_stmt_commit   },
    { "pager_stmt_rollback",     (Tcl_CmdProc*)pager_stmt_rollback },
    { "pager_stats",             (Tcl_CmdProc*)pager_stats         },
    { "pager_pagecount",         (Tcl_CmdProc*)pager_pagecount     },
    { "page_get",                (Tcl_CmdProc*)page_get            },
    { "page_lookup",             (Tcl_CmdProc*)page_lookup         },
    { "page_unref",              (Tcl_CmdProc*)page_unref          },
    { "page_read",               (Tcl_CmdProc*)page_read           },
    { "page_write",              (Tcl_CmdProc*)page_write          },
    { "page_number",             (Tcl_CmdProc*)page_number         },
    { "fake_big_file",           (Tcl_CmdProc*)fake_big_file       },
  };
  int i;
  for(i=0; i<sizeof(aCmd)/sizeof(aCmd[0]); i++){
    Tcl_CreateCommand(interp, aCmd[i].zName, aCmd[i].xProc, 0, 0);
  }
  Tcl_LinkVar(interp, "sqlite_io_error_pending",
     (char*)&sqlite3_io_error_pending, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_diskfull_pending",
     (char*)&sqlite3_diskfull_pending, TCL_LINK_INT);
  Tcl_LinkVar(interp, "pager_pagesize",
     (char*)&test_pagesize, TCL_LINK_INT);
  return TCL_OK;
}
