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
** Main file for the SQLite library.  The routines in this file
** implement the programmer interface to the library.  Routines in
** other files are for internal use by SQLite and should not be
** accessed by users of the library.
**
** $Id$
*/

#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>

/*
** Execute SQL code.  Return one of the SQLITE_ success/failure
** codes.  Also write an error message into memory obtained from
** malloc() and make *pzErrMsg point to that message.
**
** If the SQL is a query, then for each row in the query result
** the xCallback() function is called.  pArg becomes the first
** argument to xCallback().  If xCallback=NULL then no callback
** is invoked, even for queries.
*/
int sqlite3_exec(
  sqlite3 *db,                /* The database on which the SQL executes */
  const char *zSql,           /* The SQL to be executed */
  sqlite3_callback xCallback, /* Invoke this callback routine */
  void *pArg,                 /* First argument to xCallback() */
  char **pzErrMsg             /* Write error messages here */
){
  int rc = SQLITE_OK;
  const char *zLeftover;
  sqlite3_stmt *pStmt = 0;
  char **azCols = 0;

  int nRetry = 0;
  int nChange = 0;
  int nCallback;

  if( zSql==0 ) return SQLITE_OK;
  while( (rc==SQLITE_OK || (rc==SQLITE_SCHEMA && (++nRetry)<2)) && zSql[0] ){
    int nCol;
    char **azVals = 0;

    pStmt = 0;
    rc = sqlite3_prepare(db, zSql, -1, &pStmt, &zLeftover);
    if( rc!=SQLITE_OK ){
      if( pStmt ) sqlite3_finalize(pStmt);
      continue;
    }
    if( !pStmt ){
      /* this happens for a comment or white-space */
      zSql = zLeftover;
      continue;
    }

    db->nChange += nChange;
    nCallback = 0;

    nCol = sqlite3_column_count(pStmt);
    azCols = sqliteMalloc(2*nCol*sizeof(const char *));
    if( nCol && !azCols ){
      rc = SQLITE_NOMEM;
      goto exec_out;
    }

    while( 1 ){
      int i;
      rc = sqlite3_step(pStmt);

      /* Invoke the callback function if required */
      if( xCallback && (SQLITE_ROW==rc || 
          (SQLITE_DONE==rc && !nCallback && db->flags&SQLITE_NullCallback)) ){
        if( 0==nCallback ){
          for(i=0; i<nCol; i++){
            azCols[i] = (char *)sqlite3_column_name(pStmt, i);
          }
          nCallback++;
        }
        if( rc==SQLITE_ROW ){
          azVals = &azCols[nCol];
          for(i=0; i<nCol; i++){
            azVals[i] = (char *)sqlite3_column_text(pStmt, i);
          }
        }
        if( xCallback(pArg, nCol, azVals, azCols) ){
          rc = SQLITE_ABORT;
          goto exec_out;
        }
      }

      if( rc!=SQLITE_ROW ){
        rc = sqlite3_finalize(pStmt);
        pStmt = 0;
        if( db->pVdbe==0 ){
          nChange = db->nChange;
        }
        if( rc!=SQLITE_SCHEMA ){
          nRetry = 0;
          zSql = zLeftover;
          while( isspace((unsigned char)zSql[0]) ) zSql++;
        }
        break;
      }
    }

    sqliteFree(azCols);
    azCols = 0;
  }

exec_out:
  if( pStmt ) sqlite3_finalize(pStmt);
  if( azCols ) sqliteFree(azCols);

  if( sqlite3_malloc_failed ){
    rc = SQLITE_NOMEM;
  }
  if( rc!=SQLITE_OK && rc==sqlite3_errcode(db) && pzErrMsg ){
    *pzErrMsg = malloc(1+strlen(sqlite3_errmsg(db)));
    if( *pzErrMsg ){
      strcpy(*pzErrMsg, sqlite3_errmsg(db));
    }
  }else if( pzErrMsg ){
    *pzErrMsg = 0;
  }

  return rc;
}
