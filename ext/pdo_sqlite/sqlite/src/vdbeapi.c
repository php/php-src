/*
** 2004 May 26
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
**
** This file contains code use to implement APIs that are part of the
** VDBE.
*/
#include "sqliteInt.h"
#include "vdbeInt.h"
#include "os.h"

/*
** Return TRUE (non-zero) of the statement supplied as an argument needs
** to be recompiled.  A statement needs to be recompiled whenever the
** execution environment changes in a way that would alter the program
** that sqlite3_prepare() generates.  For example, if new functions or
** collating sequences are registered or if an authorizer function is
** added or changed.
*/
int sqlite3_expired(sqlite3_stmt *pStmt){
  Vdbe *p = (Vdbe*)pStmt;
  return p==0 || p->expired;
}

/**************************** sqlite3_value_  *******************************
** The following routines extract information from a Mem or sqlite3_value
** structure.
*/
const void *sqlite3_value_blob(sqlite3_value *pVal){
  Mem *p = (Mem*)pVal;
  if( p->flags & (MEM_Blob|MEM_Str) ){
    return p->z;
  }else{
    return sqlite3_value_text(pVal);
  }
}
int sqlite3_value_bytes(sqlite3_value *pVal){
  return sqlite3ValueBytes(pVal, SQLITE_UTF8);
}
int sqlite3_value_bytes16(sqlite3_value *pVal){
  return sqlite3ValueBytes(pVal, SQLITE_UTF16NATIVE);
}
double sqlite3_value_double(sqlite3_value *pVal){
  return sqlite3VdbeRealValue((Mem*)pVal);
}
int sqlite3_value_int(sqlite3_value *pVal){
  return sqlite3VdbeIntValue((Mem*)pVal);
}
sqlite_int64 sqlite3_value_int64(sqlite3_value *pVal){
  return sqlite3VdbeIntValue((Mem*)pVal);
}
const unsigned char *sqlite3_value_text(sqlite3_value *pVal){
  return (const unsigned char *)sqlite3ValueText(pVal, SQLITE_UTF8);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_value_text16(sqlite3_value* pVal){
  return sqlite3ValueText(pVal, SQLITE_UTF16NATIVE);
}
const void *sqlite3_value_text16be(sqlite3_value *pVal){
  return sqlite3ValueText(pVal, SQLITE_UTF16BE);
}
const void *sqlite3_value_text16le(sqlite3_value *pVal){
  return sqlite3ValueText(pVal, SQLITE_UTF16LE);
}
#endif /* SQLITE_OMIT_UTF16 */
int sqlite3_value_type(sqlite3_value* pVal){
  return pVal->type;
}
/* sqlite3_value_numeric_type() defined in vdbe.c */

/**************************** sqlite3_result_  *******************************
** The following routines are used by user-defined functions to specify
** the function result.
*/
void sqlite3_result_blob(
  sqlite3_context *pCtx, 
  const void *z, 
  int n, 
  void (*xDel)(void *)
){
  assert( n>=0 );
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, 0, xDel);
}
void sqlite3_result_double(sqlite3_context *pCtx, double rVal){
  sqlite3VdbeMemSetDouble(&pCtx->s, rVal);
}
void sqlite3_result_error(sqlite3_context *pCtx, const char *z, int n){
  pCtx->isError = 1;
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF8, SQLITE_TRANSIENT);
}
#ifndef SQLITE_OMIT_UTF16
void sqlite3_result_error16(sqlite3_context *pCtx, const void *z, int n){
  pCtx->isError = 1;
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF16NATIVE, SQLITE_TRANSIENT);
}
#endif
void sqlite3_result_int(sqlite3_context *pCtx, int iVal){
  sqlite3VdbeMemSetInt64(&pCtx->s, (i64)iVal);
}
void sqlite3_result_int64(sqlite3_context *pCtx, i64 iVal){
  sqlite3VdbeMemSetInt64(&pCtx->s, iVal);
}
void sqlite3_result_null(sqlite3_context *pCtx){
  sqlite3VdbeMemSetNull(&pCtx->s);
}
void sqlite3_result_text(
  sqlite3_context *pCtx, 
  const char *z, 
  int n,
  void (*xDel)(void *)
){
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF8, xDel);
}
#ifndef SQLITE_OMIT_UTF16
void sqlite3_result_text16(
  sqlite3_context *pCtx, 
  const void *z, 
  int n, 
  void (*xDel)(void *)
){
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF16NATIVE, xDel);
}
void sqlite3_result_text16be(
  sqlite3_context *pCtx, 
  const void *z, 
  int n, 
  void (*xDel)(void *)
){
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF16BE, xDel);
}
void sqlite3_result_text16le(
  sqlite3_context *pCtx, 
  const void *z, 
  int n, 
  void (*xDel)(void *)
){
  sqlite3VdbeMemSetStr(&pCtx->s, z, n, SQLITE_UTF16LE, xDel);
}
#endif /* SQLITE_OMIT_UTF16 */
void sqlite3_result_value(sqlite3_context *pCtx, sqlite3_value *pValue){
  sqlite3VdbeMemCopy(&pCtx->s, pValue);
}


/*
** Execute the statement pStmt, either until a row of data is ready, the
** statement is completely executed or an error occurs.
**
** This routine implements the bulk of the logic behind the sqlite_step()
** API.  The only thing omitted is the automatic recompile if a 
** schema change has occurred.  That detail is handled by the
** outer sqlite3_step() wrapper procedure.
*/
static int sqlite3Step(Vdbe *p){
  sqlite3 *db;
  int rc;

  /* Assert that malloc() has not failed */
  assert( !sqlite3MallocFailed() );

  if( p==0 || p->magic!=VDBE_MAGIC_RUN ){
    return SQLITE_MISUSE;
  }
  if( p->aborted ){
    return SQLITE_ABORT;
  }
  if( p->pc<=0 && p->expired ){
    if( p->rc==SQLITE_OK ){
      p->rc = SQLITE_SCHEMA;
    }
    rc = SQLITE_ERROR;
    goto end_of_step;
  }
  db = p->db;
  if( sqlite3SafetyOn(db) ){
    p->rc = SQLITE_MISUSE;
    return SQLITE_MISUSE;
  }
  if( p->pc<0 ){
    /* If there are no other statements currently running, then
    ** reset the interrupt flag.  This prevents a call to sqlite3_interrupt
    ** from interrupting a statement that has not yet started.
    */
    if( db->activeVdbeCnt==0 ){
      db->u1.isInterrupted = 0;
    }

#ifndef SQLITE_OMIT_TRACE
    /* Invoke the trace callback if there is one
    */
    if( db->xTrace && !db->init.busy ){
      assert( p->nOp>0 );
      assert( p->aOp[p->nOp-1].opcode==OP_Noop );
      assert( p->aOp[p->nOp-1].p3!=0 );
      assert( p->aOp[p->nOp-1].p3type==P3_DYNAMIC );
      sqlite3SafetyOff(db);
      db->xTrace(db->pTraceArg, p->aOp[p->nOp-1].p3);
      if( sqlite3SafetyOn(db) ){
        p->rc = SQLITE_MISUSE;
        return SQLITE_MISUSE;
      }
    }
    if( db->xProfile && !db->init.busy ){
      double rNow;
      sqlite3OsCurrentTime(&rNow);
      p->startTime = (rNow - (int)rNow)*3600.0*24.0*1000000000.0;
    }
#endif

    /* Print a copy of SQL as it is executed if the SQL_TRACE pragma is turned
    ** on in debugging mode.
    */
#ifdef SQLITE_DEBUG
    if( (db->flags & SQLITE_SqlTrace)!=0 ){
      sqlite3DebugPrintf("SQL-trace: %s\n", p->aOp[p->nOp-1].p3);
    }
#endif /* SQLITE_DEBUG */

    db->activeVdbeCnt++;
    p->pc = 0;
  }
#ifndef SQLITE_OMIT_EXPLAIN
  if( p->explain ){
    rc = sqlite3VdbeList(p);
  }else
#endif /* SQLITE_OMIT_EXPLAIN */
  {
    rc = sqlite3VdbeExec(p);
  }

  if( sqlite3SafetyOff(db) ){
    rc = SQLITE_MISUSE;
  }

#ifndef SQLITE_OMIT_TRACE
  /* Invoke the profile callback if there is one
  */
  if( rc!=SQLITE_ROW && db->xProfile && !db->init.busy ){
    double rNow;
    u64 elapseTime;

    sqlite3OsCurrentTime(&rNow);
    elapseTime = (rNow - (int)rNow)*3600.0*24.0*1000000000.0 - p->startTime;
    assert( p->nOp>0 );
    assert( p->aOp[p->nOp-1].opcode==OP_Noop );
    assert( p->aOp[p->nOp-1].p3!=0 );
    assert( p->aOp[p->nOp-1].p3type==P3_DYNAMIC );
    db->xProfile(db->pProfileArg, p->aOp[p->nOp-1].p3, elapseTime);
  }
#endif

  sqlite3Error(p->db, rc, 0);
  p->rc = sqlite3ApiExit(p->db, p->rc);
end_of_step:
  assert( (rc&0xff)==rc );
  if( p->zSql && (rc&0xff)<SQLITE_ROW ){
    /* This behavior occurs if sqlite3_prepare_v2() was used to build
    ** the prepared statement.  Return error codes directly */
    return p->rc;
  }else{
    /* This is for legacy sqlite3_prepare() builds and when the code
    ** is SQLITE_ROW or SQLITE_DONE */
    return rc;
  }
}

/*
** This is the top-level implementation of sqlite3_step().  Call
** sqlite3Step() to do most of the work.  If a schema error occurs,
** call sqlite3Reprepare() and try again.
*/
#ifdef SQLITE_OMIT_PARSER
int sqlite3_step(sqlite3_stmt *pStmt){
  return sqlite3Step((Vdbe*)pStmt);
}
#else
int sqlite3_step(sqlite3_stmt *pStmt){
  int cnt = 0;
  int rc;
  Vdbe *v = (Vdbe*)pStmt;
  while( (rc = sqlite3Step(v))==SQLITE_SCHEMA
         && cnt++ < 5
         && sqlite3Reprepare(v) ){
    sqlite3_reset(pStmt);
    v->expired = 0;
  }
  return rc;
}
#endif

/*
** Extract the user data from a sqlite3_context structure and return a
** pointer to it.
*/
void *sqlite3_user_data(sqlite3_context *p){
  assert( p && p->pFunc );
  return p->pFunc->pUserData;
}

/*
** The following is the implementation of an SQL function that always
** fails with an error message stating that the function is used in the
** wrong context.  The sqlite3_overload_function() API might construct
** SQL function that use this routine so that the functions will exist
** for name resolution but are actually overloaded by the xFindFunction
** method of virtual tables.
*/
void sqlite3InvalidFunction(
  sqlite3_context *context,  /* The function calling context */
  int argc,                  /* Number of arguments to the function */
  sqlite3_value **argv       /* Value of each argument */
){
  const char *zName = context->pFunc->zName;
  char *zErr;
  zErr = sqlite3MPrintf(
      "unable to use function %s in the requested context", zName);
  sqlite3_result_error(context, zErr, -1);
  sqliteFree(zErr);
}

/*
** Allocate or return the aggregate context for a user function.  A new
** context is allocated on the first call.  Subsequent calls return the
** same context that was returned on prior calls.
*/
void *sqlite3_aggregate_context(sqlite3_context *p, int nByte){
  Mem *pMem = p->pMem;
  assert( p && p->pFunc && p->pFunc->xStep );
  if( (pMem->flags & MEM_Agg)==0 ){
    if( nByte==0 ){
      assert( pMem->flags==MEM_Null );
      pMem->z = 0;
    }else{
      pMem->flags = MEM_Agg;
      pMem->xDel = sqlite3FreeX;
      pMem->u.pDef = p->pFunc;
      if( nByte<=NBFS ){
        pMem->z = pMem->zShort;
        memset(pMem->z, 0, nByte);
      }else{
        pMem->z = sqliteMalloc( nByte );
      }
    }
  }
  return (void*)pMem->z;
}

/*
** Return the auxilary data pointer, if any, for the iArg'th argument to
** the user-function defined by pCtx.
*/
void *sqlite3_get_auxdata(sqlite3_context *pCtx, int iArg){
  VdbeFunc *pVdbeFunc = pCtx->pVdbeFunc;
  if( !pVdbeFunc || iArg>=pVdbeFunc->nAux || iArg<0 ){
    return 0;
  }
  return pVdbeFunc->apAux[iArg].pAux;
}

/*
** Set the auxilary data pointer and delete function, for the iArg'th
** argument to the user-function defined by pCtx. Any previous value is
** deleted by calling the delete function specified when it was set.
*/
void sqlite3_set_auxdata(
  sqlite3_context *pCtx, 
  int iArg, 
  void *pAux, 
  void (*xDelete)(void*)
){
  struct AuxData *pAuxData;
  VdbeFunc *pVdbeFunc;
  if( iArg<0 ) return;

  pVdbeFunc = pCtx->pVdbeFunc;
  if( !pVdbeFunc || pVdbeFunc->nAux<=iArg ){
    int nMalloc = sizeof(VdbeFunc) + sizeof(struct AuxData)*iArg;
    pVdbeFunc = sqliteRealloc(pVdbeFunc, nMalloc);
    if( !pVdbeFunc ) return;
    pCtx->pVdbeFunc = pVdbeFunc;
    memset(&pVdbeFunc->apAux[pVdbeFunc->nAux], 0, 
             sizeof(struct AuxData)*(iArg+1-pVdbeFunc->nAux));
    pVdbeFunc->nAux = iArg+1;
    pVdbeFunc->pFunc = pCtx->pFunc;
  }

  pAuxData = &pVdbeFunc->apAux[iArg];
  if( pAuxData->pAux && pAuxData->xDelete ){
    pAuxData->xDelete(pAuxData->pAux);
  }
  pAuxData->pAux = pAux;
  pAuxData->xDelete = xDelete;
}

/*
** Return the number of times the Step function of a aggregate has been 
** called.
**
** This function is deprecated.  Do not use it for new code.  It is
** provide only to avoid breaking legacy code.  New aggregate function
** implementations should keep their own counts within their aggregate
** context.
*/
int sqlite3_aggregate_count(sqlite3_context *p){
  assert( p && p->pFunc && p->pFunc->xStep );
  return p->pMem->n;
}

/*
** Return the number of columns in the result set for the statement pStmt.
*/
int sqlite3_column_count(sqlite3_stmt *pStmt){
  Vdbe *pVm = (Vdbe *)pStmt;
  return pVm ? pVm->nResColumn : 0;
}

/*
** Return the number of values available from the current row of the
** currently executing statement pStmt.
*/
int sqlite3_data_count(sqlite3_stmt *pStmt){
  Vdbe *pVm = (Vdbe *)pStmt;
  if( pVm==0 || !pVm->resOnStack ) return 0;
  return pVm->nResColumn;
}


/*
** Check to see if column iCol of the given statement is valid.  If
** it is, return a pointer to the Mem for the value of that column.
** If iCol is not valid, return a pointer to a Mem which has a value
** of NULL.
*/
static Mem *columnMem(sqlite3_stmt *pStmt, int i){
  Vdbe *pVm = (Vdbe *)pStmt;
  int vals = sqlite3_data_count(pStmt);
  if( i>=vals || i<0 ){
    static const Mem nullMem = {{0}, 0.0, "", 0, MEM_Null, MEM_Null };
    sqlite3Error(pVm->db, SQLITE_RANGE, 0);
    return (Mem*)&nullMem;
  }
  return &pVm->pTos[(1-vals)+i];
}

/*
** This function is called after invoking an sqlite3_value_XXX function on a 
** column value (i.e. a value returned by evaluating an SQL expression in the
** select list of a SELECT statement) that may cause a malloc() failure. If 
** malloc() has failed, the threads mallocFailed flag is cleared and the result
** code of statement pStmt set to SQLITE_NOMEM.
**
** Specificly, this is called from within:
**
**     sqlite3_column_int()
**     sqlite3_column_int64()
**     sqlite3_column_text()
**     sqlite3_column_text16()
**     sqlite3_column_real()
**     sqlite3_column_bytes()
**     sqlite3_column_bytes16()
**
** But not for sqlite3_column_blob(), which never calls malloc().
*/
static void columnMallocFailure(sqlite3_stmt *pStmt)
{
  /* If malloc() failed during an encoding conversion within an
  ** sqlite3_column_XXX API, then set the return code of the statement to
  ** SQLITE_NOMEM. The next call to _step() (if any) will return SQLITE_ERROR
  ** and _finalize() will return NOMEM.
  */
  Vdbe *p = (Vdbe *)pStmt;
  p->rc = sqlite3ApiExit(0, p->rc);
}

/**************************** sqlite3_column_  *******************************
** The following routines are used to access elements of the current row
** in the result set.
*/
const void *sqlite3_column_blob(sqlite3_stmt *pStmt, int i){
  const void *val;
  sqlite3MallocDisallow();
  val = sqlite3_value_blob( columnMem(pStmt,i) );
  sqlite3MallocAllow();
  return val;
}
int sqlite3_column_bytes(sqlite3_stmt *pStmt, int i){
  int val = sqlite3_value_bytes( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
int sqlite3_column_bytes16(sqlite3_stmt *pStmt, int i){
  int val = sqlite3_value_bytes16( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
double sqlite3_column_double(sqlite3_stmt *pStmt, int i){
  double val = sqlite3_value_double( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
int sqlite3_column_int(sqlite3_stmt *pStmt, int i){
  int val = sqlite3_value_int( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
sqlite_int64 sqlite3_column_int64(sqlite3_stmt *pStmt, int i){
  sqlite_int64 val = sqlite3_value_int64( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
const unsigned char *sqlite3_column_text(sqlite3_stmt *pStmt, int i){
  const unsigned char *val = sqlite3_value_text( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
sqlite3_value *sqlite3_column_value(sqlite3_stmt *pStmt, int i){
  return columnMem(pStmt, i);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_text16(sqlite3_stmt *pStmt, int i){
  const void *val = sqlite3_value_text16( columnMem(pStmt,i) );
  columnMallocFailure(pStmt);
  return val;
}
#endif /* SQLITE_OMIT_UTF16 */
int sqlite3_column_type(sqlite3_stmt *pStmt, int i){
  return sqlite3_value_type( columnMem(pStmt,i) );
}

/* The following function is experimental and subject to change or
** removal */
/*int sqlite3_column_numeric_type(sqlite3_stmt *pStmt, int i){
**  return sqlite3_value_numeric_type( columnMem(pStmt,i) );
**}
*/

/*
** Convert the N-th element of pStmt->pColName[] into a string using
** xFunc() then return that string.  If N is out of range, return 0.
**
** There are up to 5 names for each column.  useType determines which
** name is returned.  Here are the names:
**
**    0      The column name as it should be displayed for output
**    1      The datatype name for the column
**    2      The name of the database that the column derives from
**    3      The name of the table that the column derives from
**    4      The name of the table column that the result column derives from
**
** If the result is not a simple column reference (if it is an expression
** or a constant) then useTypes 2, 3, and 4 return NULL.
*/
static const void *columnName(
  sqlite3_stmt *pStmt,
  int N,
  const void *(*xFunc)(Mem*),
  int useType
){
  const void *ret;
  Vdbe *p = (Vdbe *)pStmt;
  int n = sqlite3_column_count(pStmt);

  if( p==0 || N>=n || N<0 ){
    return 0;
  }
  N += useType*n;
  ret = xFunc(&p->aColName[N]);

  /* A malloc may have failed inside of the xFunc() call. If this is the case,
  ** clear the mallocFailed flag and return NULL.
  */
  sqlite3ApiExit(0, 0);
  return ret;
}

/*
** Return the name of the Nth column of the result set returned by SQL
** statement pStmt.
*/
const char *sqlite3_column_name(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text, COLNAME_NAME);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_name16(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text16, COLNAME_NAME);
}
#endif

/*
** Return the column declaration type (if applicable) of the 'i'th column
** of the result set of SQL statement pStmt.
*/
const char *sqlite3_column_decltype(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text, COLNAME_DECLTYPE);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_decltype16(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text16, COLNAME_DECLTYPE);
}
#endif /* SQLITE_OMIT_UTF16 */

#ifdef SQLITE_ENABLE_COLUMN_METADATA
/*
** Return the name of the database from which a result column derives.
** NULL is returned if the result column is an expression or constant or
** anything else which is not an unabiguous reference to a database column.
*/
const char *sqlite3_column_database_name(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text, COLNAME_DATABASE);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_database_name16(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text16, COLNAME_DATABASE);
}
#endif /* SQLITE_OMIT_UTF16 */

/*
** Return the name of the table from which a result column derives.
** NULL is returned if the result column is an expression or constant or
** anything else which is not an unabiguous reference to a database column.
*/
const char *sqlite3_column_table_name(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text, COLNAME_TABLE);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_table_name16(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text16, COLNAME_TABLE);
}
#endif /* SQLITE_OMIT_UTF16 */

/*
** Return the name of the table column from which a result column derives.
** NULL is returned if the result column is an expression or constant or
** anything else which is not an unabiguous reference to a database column.
*/
const char *sqlite3_column_origin_name(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text, COLNAME_COLUMN);
}
#ifndef SQLITE_OMIT_UTF16
const void *sqlite3_column_origin_name16(sqlite3_stmt *pStmt, int N){
  return columnName(
      pStmt, N, (const void*(*)(Mem*))sqlite3_value_text16, COLNAME_COLUMN);
}
#endif /* SQLITE_OMIT_UTF16 */
#endif /* SQLITE_ENABLE_COLUMN_METADATA */


/******************************* sqlite3_bind_  ***************************
** 
** Routines used to attach values to wildcards in a compiled SQL statement.
*/
/*
** Unbind the value bound to variable i in virtual machine p. This is the 
** the same as binding a NULL value to the column. If the "i" parameter is
** out of range, then SQLITE_RANGE is returned. Othewise SQLITE_OK.
**
** The error code stored in database p->db is overwritten with the return
** value in any case.
*/
static int vdbeUnbind(Vdbe *p, int i){
  Mem *pVar;
  if( p==0 || p->magic!=VDBE_MAGIC_RUN || p->pc>=0 ){
    if( p ) sqlite3Error(p->db, SQLITE_MISUSE, 0);
    return SQLITE_MISUSE;
  }
  if( i<1 || i>p->nVar ){
    sqlite3Error(p->db, SQLITE_RANGE, 0);
    return SQLITE_RANGE;
  }
  i--;
  pVar = &p->aVar[i];
  sqlite3VdbeMemRelease(pVar);
  pVar->flags = MEM_Null;
  sqlite3Error(p->db, SQLITE_OK, 0);
  return SQLITE_OK;
}

/*
** Bind a text or BLOB value.
*/
static int bindText(
  sqlite3_stmt *pStmt, 
  int i, 
  const void *zData, 
  int nData, 
  void (*xDel)(void*),
  int encoding
){
  Vdbe *p = (Vdbe *)pStmt;
  Mem *pVar;
  int rc;

  rc = vdbeUnbind(p, i);
  if( rc || zData==0 ){
    return rc;
  }
  pVar = &p->aVar[i-1];
  rc = sqlite3VdbeMemSetStr(pVar, zData, nData, encoding, xDel);
  if( rc==SQLITE_OK && encoding!=0 ){
    rc = sqlite3VdbeChangeEncoding(pVar, ENC(p->db));
  }

  sqlite3Error(((Vdbe *)pStmt)->db, rc, 0);
  return sqlite3ApiExit(((Vdbe *)pStmt)->db, rc);
}


/*
** Bind a blob value to an SQL statement variable.
*/
int sqlite3_bind_blob(
  sqlite3_stmt *pStmt, 
  int i, 
  const void *zData, 
  int nData, 
  void (*xDel)(void*)
){
  return bindText(pStmt, i, zData, nData, xDel, 0);
}
int sqlite3_bind_double(sqlite3_stmt *pStmt, int i, double rValue){
  int rc;
  Vdbe *p = (Vdbe *)pStmt;
  rc = vdbeUnbind(p, i);
  if( rc==SQLITE_OK ){
    sqlite3VdbeMemSetDouble(&p->aVar[i-1], rValue);
  }
  return rc;
}
int sqlite3_bind_int(sqlite3_stmt *p, int i, int iValue){
  return sqlite3_bind_int64(p, i, (i64)iValue);
}
int sqlite3_bind_int64(sqlite3_stmt *pStmt, int i, sqlite_int64 iValue){
  int rc;
  Vdbe *p = (Vdbe *)pStmt;
  rc = vdbeUnbind(p, i);
  if( rc==SQLITE_OK ){
    sqlite3VdbeMemSetInt64(&p->aVar[i-1], iValue);
  }
  return rc;
}
int sqlite3_bind_null(sqlite3_stmt* p, int i){
  return vdbeUnbind((Vdbe *)p, i);
}
int sqlite3_bind_text( 
  sqlite3_stmt *pStmt, 
  int i, 
  const char *zData, 
  int nData, 
  void (*xDel)(void*)
){
  return bindText(pStmt, i, zData, nData, xDel, SQLITE_UTF8);
}
#ifndef SQLITE_OMIT_UTF16
int sqlite3_bind_text16(
  sqlite3_stmt *pStmt, 
  int i, 
  const void *zData, 
  int nData, 
  void (*xDel)(void*)
){
  return bindText(pStmt, i, zData, nData, xDel, SQLITE_UTF16NATIVE);
}
#endif /* SQLITE_OMIT_UTF16 */
int sqlite3_bind_value(sqlite3_stmt *pStmt, int i, const sqlite3_value *pValue){
  int rc;
  Vdbe *p = (Vdbe *)pStmt;
  rc = vdbeUnbind(p, i);
  if( rc==SQLITE_OK ){
    sqlite3VdbeMemCopy(&p->aVar[i-1], pValue);
  }
  return rc;
}

/*
** Return the number of wildcards that can be potentially bound to.
** This routine is added to support DBD::SQLite.  
*/
int sqlite3_bind_parameter_count(sqlite3_stmt *pStmt){
  Vdbe *p = (Vdbe*)pStmt;
  return p ? p->nVar : 0;
}

/*
** Create a mapping from variable numbers to variable names
** in the Vdbe.azVar[] array, if such a mapping does not already
** exist.
*/
static void createVarMap(Vdbe *p){
  if( !p->okVar ){
    int j;
    Op *pOp;
    for(j=0, pOp=p->aOp; j<p->nOp; j++, pOp++){
      if( pOp->opcode==OP_Variable ){
        assert( pOp->p1>0 && pOp->p1<=p->nVar );
        p->azVar[pOp->p1-1] = pOp->p3;
      }
    }
    p->okVar = 1;
  }
}

/*
** Return the name of a wildcard parameter.  Return NULL if the index
** is out of range or if the wildcard is unnamed.
**
** The result is always UTF-8.
*/
const char *sqlite3_bind_parameter_name(sqlite3_stmt *pStmt, int i){
  Vdbe *p = (Vdbe*)pStmt;
  if( p==0 || i<1 || i>p->nVar ){
    return 0;
  }
  createVarMap(p);
  return p->azVar[i-1];
}

/*
** Given a wildcard parameter name, return the index of the variable
** with that name.  If there is no variable with the given name,
** return 0.
*/
int sqlite3_bind_parameter_index(sqlite3_stmt *pStmt, const char *zName){
  Vdbe *p = (Vdbe*)pStmt;
  int i;
  if( p==0 ){
    return 0;
  }
  createVarMap(p); 
  if( zName ){
    for(i=0; i<p->nVar; i++){
      const char *z = p->azVar[i];
      if( z && strcmp(z,zName)==0 ){
        return i+1;
      }
    }
  }
  return 0;
}

/*
** Transfer all bindings from the first statement over to the second.
** If the two statements contain a different number of bindings, then
** an SQLITE_ERROR is returned.
*/
int sqlite3_transfer_bindings(sqlite3_stmt *pFromStmt, sqlite3_stmt *pToStmt){
  Vdbe *pFrom = (Vdbe*)pFromStmt;
  Vdbe *pTo = (Vdbe*)pToStmt;
  int i, rc = SQLITE_OK;
  if( (pFrom->magic!=VDBE_MAGIC_RUN && pFrom->magic!=VDBE_MAGIC_HALT)
    || (pTo->magic!=VDBE_MAGIC_RUN && pTo->magic!=VDBE_MAGIC_HALT) ){
    return SQLITE_MISUSE;
  }
  if( pFrom->nVar!=pTo->nVar ){
    return SQLITE_ERROR;
  }
  for(i=0; rc==SQLITE_OK && i<pFrom->nVar; i++){
    sqlite3MallocDisallow();
    rc = sqlite3VdbeMemMove(&pTo->aVar[i], &pFrom->aVar[i]);
    sqlite3MallocAllow();
  }
  assert( rc==SQLITE_OK || rc==SQLITE_NOMEM );
  return rc;
}

/*
** Return the sqlite3* database handle to which the prepared statement given
** in the argument belongs.  This is the same database handle that was
** the first argument to the sqlite3_prepare() that was used to create
** the statement in the first place.
*/
sqlite3 *sqlite3_db_handle(sqlite3_stmt *pStmt){
  return pStmt ? ((Vdbe*)pStmt)->db : 0;
}
