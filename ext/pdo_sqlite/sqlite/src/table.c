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
** This file contains the sqlite3_get_table() and sqlite3_free_table()
** interface routines.  These are just wrappers around the main
** interface routine of sqlite3_exec().
**
** These routines are in a separate files so that they will not be linked
** if they are not used.
*/
#include <stdlib.h>
#include <string.h>
#include "sqliteInt.h"

/*
** This structure is used to pass data from sqlite3_get_table() through
** to the callback function is uses to build the result.
*/
typedef struct TabResult {
  char **azResult;
  char *zErrMsg;
  int nResult;
  int nAlloc;
  int nRow;
  int nColumn;
  int nData;
  int rc;
} TabResult;

/*
** This routine is called once for each row in the result table.  Its job
** is to fill in the TabResult structure appropriately, allocating new
** memory as necessary.
*/
static int sqlite3_get_table_cb(void *pArg, int nCol, char **argv, char **colv){
  TabResult *p = (TabResult*)pArg;
  int need;
  int i;
  char *z;

  /* Make sure there is enough space in p->azResult to hold everything
  ** we need to remember from this invocation of the callback.
  */
  if( p->nRow==0 && argv!=0 ){
    need = nCol*2;
  }else{
    need = nCol;
  }
  if( p->nData + need >= p->nAlloc ){
    char **azNew;
    p->nAlloc = p->nAlloc*2 + need + 1;
    azNew = realloc( p->azResult, sizeof(char*)*p->nAlloc );
    if( azNew==0 ) goto malloc_failed;
    p->azResult = azNew;
  }

  /* If this is the first row, then generate an extra row containing
  ** the names of all columns.
  */
  if( p->nRow==0 ){
    p->nColumn = nCol;
    for(i=0; i<nCol; i++){
      if( colv[i]==0 ){
        z = 0;
      }else{
        z = malloc( strlen(colv[i])+1 );
        if( z==0 ) goto malloc_failed;
        strcpy(z, colv[i]);
      }
      p->azResult[p->nData++] = z;
    }
  }else if( p->nColumn!=nCol ){
    sqlite3SetString(&p->zErrMsg,
       "sqlite3_get_table() called with two or more incompatible queries",
       (char*)0);
    p->rc = SQLITE_ERROR;
    return 1;
  }

  /* Copy over the row data
  */
  if( argv!=0 ){
    for(i=0; i<nCol; i++){
      if( argv[i]==0 ){
        z = 0;
      }else{
        z = malloc( strlen(argv[i])+1 );
        if( z==0 ) goto malloc_failed;
        strcpy(z, argv[i]);
      }
      p->azResult[p->nData++] = z;
    }
    p->nRow++;
  }
  return 0;

malloc_failed:
  p->rc = SQLITE_NOMEM;
  return 1;
}

/*
** Query the database.  But instead of invoking a callback for each row,
** malloc() for space to hold the result and return the entire results
** at the conclusion of the call.
**
** The result that is written to ***pazResult is held in memory obtained
** from malloc().  But the caller cannot free this memory directly.  
** Instead, the entire table should be passed to sqlite3_free_table() when
** the calling procedure is finished using it.
*/
int sqlite3_get_table(
  sqlite3 *db,                /* The database on which the SQL executes */
  const char *zSql,           /* The SQL to be executed */
  char ***pazResult,          /* Write the result table here */
  int *pnRow,                 /* Write the number of rows in the result here */
  int *pnColumn,              /* Write the number of columns of result here */
  char **pzErrMsg             /* Write error messages here */
){
  int rc;
  TabResult res;
  if( pazResult==0 ){ return SQLITE_ERROR; }
  *pazResult = 0;
  if( pnColumn ) *pnColumn = 0;
  if( pnRow ) *pnRow = 0;
  res.zErrMsg = 0;
  res.nResult = 0;
  res.nRow = 0;
  res.nColumn = 0;
  res.nData = 1;
  res.nAlloc = 20;
  res.rc = SQLITE_OK;
  res.azResult = malloc( sizeof(char*)*res.nAlloc );
  if( res.azResult==0 ) return SQLITE_NOMEM;
  res.azResult[0] = 0;
  rc = sqlite3_exec(db, zSql, sqlite3_get_table_cb, &res, pzErrMsg);
  if( res.azResult ){
    res.azResult[0] = (char*)res.nData;
  }
  if( rc==SQLITE_ABORT ){
    sqlite3_free_table(&res.azResult[1]);
    if( res.zErrMsg ){
      if( pzErrMsg ){
        free(*pzErrMsg);
        *pzErrMsg = sqlite3_mprintf("%s",res.zErrMsg);
      }
      sqliteFree(res.zErrMsg);
    }
    db->errCode = res.rc;
    return res.rc;
  }
  sqliteFree(res.zErrMsg);
  if( rc!=SQLITE_OK ){
    sqlite3_free_table(&res.azResult[1]);
    return rc;
  }
  if( res.nAlloc>res.nData ){
    char **azNew;
    azNew = realloc( res.azResult, sizeof(char*)*(res.nData+1) );
    if( azNew==0 ){
      sqlite3_free_table(&res.azResult[1]);
      return SQLITE_NOMEM;
    }
    res.nAlloc = res.nData+1;
    res.azResult = azNew;
  }
  *pazResult = &res.azResult[1];
  if( pnColumn ) *pnColumn = res.nColumn;
  if( pnRow ) *pnRow = res.nRow;
  return rc;
}

/*
** This routine frees the space the sqlite3_get_table() malloced.
*/
void sqlite3_free_table(
  char **azResult            /* Result returned from from sqlite3_get_table() */
){
  if( azResult ){
    int i, n;
    azResult--;
    if( azResult==0 ) return;
    n = (int)azResult[0];
    for(i=1; i<n; i++){ if( azResult[i] ) free(azResult[i]); }
    free(azResult);
  }
}
