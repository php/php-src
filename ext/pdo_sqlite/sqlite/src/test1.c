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
** Code for testing the printf() interface to SQLite.  This code
** is not included in the SQLite library.  It is used for automated
** testing of the SQLite library.
**
** $Id$
*/
#include "sqliteInt.h"
#include "tcl.h"
#include "os.h"
#include <stdlib.h>
#include <string.h>

static const char * errorName(int rc){
  const char *zName = 0;
  switch( rc ){
    case SQLITE_OK:         zName = "SQLITE_OK";          break;
    case SQLITE_ERROR:      zName = "SQLITE_ERROR";       break;
    case SQLITE_INTERNAL:   zName = "SQLITE_INTERNAL";    break;
    case SQLITE_PERM:       zName = "SQLITE_PERM";        break;
    case SQLITE_ABORT:      zName = "SQLITE_ABORT";       break;
    case SQLITE_BUSY:       zName = "SQLITE_BUSY";        break;
    case SQLITE_LOCKED:     zName = "SQLITE_LOCKED";      break;
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
    case SQLITE_AUTH:       zName = "SQLITE_AUTH";        break;
    case SQLITE_FORMAT:     zName = "SQLITE_FORMAT";      break;
    case SQLITE_RANGE:      zName = "SQLITE_RANGE";       break;
    case SQLITE_ROW:        zName = "SQLITE_ROW";         break;
    case SQLITE_DONE:       zName = "SQLITE_DONE";        break;
    case SQLITE_NOTADB:     zName = "SQLITE_NOTADB";      break;
    default:                zName = "SQLITE_Unknown";     break;
  }
  return zName;
}

/*
** Convert an sqlite3_stmt* into an sqlite3*.  This depends on the
** fact that the sqlite3* is the first field in the Vdbe structure.
*/
#define StmtToDb(X)   (*(sqlite3**)(X))

/*
** Check a return value to make sure it agrees with the results
** from sqlite3_errcode.
*/
int sqlite3TestErrCode(Tcl_Interp *interp, sqlite3 *db, int rc){
  if( rc!=SQLITE_MISUSE && rc!=SQLITE_OK && sqlite3_errcode(db)!=rc ){
    char zBuf[200];
    int r2 = sqlite3_errcode(db);
    sprintf(zBuf, "error code %s (%d) does not match sqlite3_errcode %s (%d)",
       errorName(rc), rc, errorName(r2), r2);
    Tcl_ResetResult(interp);
    Tcl_AppendResult(interp, zBuf, 0);
    return 1;
  }
  return 0;
}

/*
** Decode a pointer to an sqlite3 object.
*/
static int getDbPointer(Tcl_Interp *interp, const char *zA, sqlite3 **ppDb){
  *ppDb = (sqlite3*)sqlite3TextToPtr(zA);
  return TCL_OK;
}

/*
** Decode a pointer to an sqlite3_stmt object.
*/
static int getStmtPointer(
  Tcl_Interp *interp, 
  const char *zArg,  
  sqlite3_stmt **ppStmt
){
  *ppStmt = (sqlite3_stmt*)sqlite3TextToPtr(zArg);
  return TCL_OK;
}

/*
** Decode a pointer to an sqlite3_stmt object.
*/
static int getFilePointer(
  Tcl_Interp *interp, 
  const char *zArg,  
  OsFile **ppFile
){
  *ppFile = (OsFile*)sqlite3TextToPtr(zArg);
  return TCL_OK;
}

/*
** Generate a text representation of a pointer that can be understood
** by the getDbPointer and getVmPointer routines above.
**
** The problem is, on some machines (Solaris) if you do a printf with
** "%p" you cannot turn around and do a scanf with the same "%p" and
** get your pointer back.  You have to prepend a "0x" before it will
** work.  Or at least that is what is reported to me (drh).  But this
** behavior varies from machine to machine.  The solution used her is
** to test the string right after it is generated to see if it can be
** understood by scanf, and if not, try prepending an "0x" to see if
** that helps.  If nothing works, a fatal error is generated.
*/
static int makePointerStr(Tcl_Interp *interp, char *zPtr, void *p){
  sqlite3_snprintf(100, zPtr, "%p", p);
  return TCL_OK;
}

/*
** The callback routine for sqlite3_exec_printf().
*/
static int exec_printf_cb(void *pArg, int argc, char **argv, char **name){
  Tcl_DString *str = (Tcl_DString*)pArg;
  int i;

  if( Tcl_DStringLength(str)==0 ){
    for(i=0; i<argc; i++){
      Tcl_DStringAppendElement(str, name[i] ? name[i] : "NULL");
    }
  }
  for(i=0; i<argc; i++){
    Tcl_DStringAppendElement(str, argv[i] ? argv[i] : "NULL");
  }
  return 0;
}

/*
** Usage:  sqlite3_exec_printf  DB  FORMAT  STRING
**
** Invoke the sqlite3_exec_printf() interface using the open database
** DB.  The SQL is the string FORMAT.  The format string should contain
** one %s or %q.  STRING is the value inserted into %s or %q.
*/
static int test_exec_printf(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  Tcl_DString str;
  int rc;
  char *zErr = 0;
  char *zSql;
  char zBuf[30];
  if( argc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], 
       " DB FORMAT STRING", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  Tcl_DStringInit(&str);
  zSql = sqlite3_mprintf(argv[2], argv[3]);
  rc = sqlite3_exec(db, zSql, exec_printf_cb, &str, &zErr);
  sqlite3_free(zSql);
  sprintf(zBuf, "%d", rc);
  Tcl_AppendElement(interp, zBuf);
  Tcl_AppendElement(interp, rc==SQLITE_OK ? Tcl_DStringValue(&str) : zErr);
  Tcl_DStringFree(&str);
  if( zErr ) free(zErr);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}

/*
** Usage:  sqlite3_mprintf_z_test  SEPARATOR  ARG0  ARG1 ...
**
** Test the %z format of mprintf().  Use multiple mprintf() calls to 
** concatenate arg0 through argn using separator as the separator.
** Return the result.
*/
static int test_mprintf_z(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  char *zResult = 0;
  int i;

  for(i=2; i<argc; i++){
    zResult = sqlite3MPrintf("%z%s%s", zResult, argv[1], argv[i]);
  }
  Tcl_AppendResult(interp, zResult, 0);
  sqliteFree(zResult);
  return TCL_OK;
}

/*
** Usage:  sqlite3_get_table_printf  DB  FORMAT  STRING
**
** Invoke the sqlite3_get_table_printf() interface using the open database
** DB.  The SQL is the string FORMAT.  The format string should contain
** one %s or %q.  STRING is the value inserted into %s or %q.
*/
static int test_get_table_printf(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  Tcl_DString str;
  int rc;
  char *zErr = 0;
  int nRow, nCol;
  char **aResult;
  int i;
  char zBuf[30];
  char *zSql;
  if( argc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], 
       " DB FORMAT STRING", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  Tcl_DStringInit(&str);
  zSql = sqlite3_mprintf(argv[2],argv[3]);
  rc = sqlite3_get_table(db, zSql, &aResult, &nRow, &nCol, &zErr);
  sqlite3_free(zSql);
  sprintf(zBuf, "%d", rc);
  Tcl_AppendElement(interp, zBuf);
  if( rc==SQLITE_OK ){
    sprintf(zBuf, "%d", nRow);
    Tcl_AppendElement(interp, zBuf);
    sprintf(zBuf, "%d", nCol);
    Tcl_AppendElement(interp, zBuf);
    for(i=0; i<(nRow+1)*nCol; i++){
      Tcl_AppendElement(interp, aResult[i] ? aResult[i] : "NULL");
    }
  }else{
    Tcl_AppendElement(interp, zErr);
  }
  sqlite3_free_table(aResult);
  if( zErr ) free(zErr);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}


/*
** Usage:  sqlite3_last_insert_rowid DB
**
** Returns the integer ROWID of the most recent insert.
*/
static int test_last_rowid(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  char zBuf[30];

  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], " DB\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  sprintf(zBuf, "%lld", sqlite3_last_insert_rowid(db));
  Tcl_AppendResult(interp, zBuf, 0);
  return SQLITE_OK;
}

/*
** Usage:  sqlite3_key DB KEY
**
** Set the codec key.
*/
static int test_key(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  const char *zKey;
  int nKey;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  zKey = argv[2];
  nKey = strlen(zKey);
#ifdef SQLITE_HAS_CODEC
  sqlite3_key(db, zKey, nKey);
#endif
  return TCL_OK;
}

/*
** Usage:  sqlite3_rekey DB KEY
**
** Change the codec key.
*/
static int test_rekey(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  const char *zKey;
  int nKey;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  zKey = argv[2];
  nKey = strlen(zKey);
#ifdef SQLITE_HAS_CODEC
  sqlite3_rekey(db, zKey, nKey);
#endif
  return TCL_OK;
}

/*
** Usage:  sqlite3_close DB
**
** Closes the database opened by sqlite3_open.
*/
static int sqlite_test_close(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  rc = sqlite3_close(db);
  Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
  return TCL_OK;
}

/*
** Implementation of the x_coalesce() function.
** Return the first argument non-NULL argument.
*/
static void ifnullFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  int i;
  for(i=0; i<argc; i++){
    if( SQLITE_NULL!=sqlite3_value_type(argv[i]) ){
      sqlite3_result_text(context, sqlite3_value_text(argv[i]), -1,
          SQLITE_TRANSIENT);
      break;
    }
  }
}

/*
** A structure into which to accumulate text.
*/
struct dstr {
  int nAlloc;  /* Space allocated */
  int nUsed;   /* Space used */
  char *z;     /* The space */
};

/*
** Append text to a dstr
*/
static void dstrAppend(struct dstr *p, const char *z, int divider){
  int n = strlen(z);
  if( p->nUsed + n + 2 > p->nAlloc ){
    char *zNew;
    p->nAlloc = p->nAlloc*2 + n + 200;
    zNew = sqliteRealloc(p->z, p->nAlloc);
    if( zNew==0 ){
      sqliteFree(p->z);
      memset(p, 0, sizeof(*p));
      return;
    }
    p->z = zNew;
  }
  if( divider && p->nUsed>0 ){
    p->z[p->nUsed++] = divider;
  }
  memcpy(&p->z[p->nUsed], z, n+1);
  p->nUsed += n;
}

/*
** Invoked for each callback from sqlite3ExecFunc
*/
static int execFuncCallback(void *pData, int argc, char **argv, char **NotUsed){
  struct dstr *p = (struct dstr*)pData;
  int i;
  for(i=0; i<argc; i++){
    if( argv[i]==0 ){
      dstrAppend(p, "NULL", ' ');
    }else{
      dstrAppend(p, argv[i], ' ');
    }
  }
  return 0;
}

/*
** Implementation of the x_sqlite_exec() function.  This function takes
** a single argument and attempts to execute that argument as SQL code.
** This is illegal and should set the SQLITE_MISUSE flag on the database.
**
** 2004-Jan-07:  We have changed this to make it legal to call sqlite3_exec()
** from within a function call.  
** 
** This routine simulates the effect of having two threads attempt to
** use the same database at the same time.
*/
static void sqlite3ExecFunc(
  sqlite3_context *context, 
  int argc,  
  sqlite3_value **argv
){
  struct dstr x;
  memset(&x, 0, sizeof(x));
  sqlite3_exec((sqlite3*)sqlite3_user_data(context),
      sqlite3_value_text(argv[0]),
      execFuncCallback, &x, 0);
  sqlite3_result_text(context, x.z, x.nUsed, SQLITE_TRANSIENT);
  sqliteFree(x.z);
}

/*
** Usage:  sqlite_test_create_function DB
**
** Call the sqlite3_create_function API on the given database in order
** to create a function named "x_coalesce".  This function does the same thing
** as the "coalesce" function.  This function also registers an SQL function
** named "x_sqlite_exec" that invokes sqlite3_exec().  Invoking sqlite3_exec()
** in this way is illegal recursion and should raise an SQLITE_MISUSE error.
** The effect is similar to trying to use the same database connection from
** two threads at the same time.
**
** The original motivation for this routine was to be able to call the
** sqlite3_create_function function while a query is in progress in order
** to test the SQLITE_MISUSE detection logic.
*/
static int test_create_function(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int rc;
  sqlite3 *db;
  sqlite3_value *pVal;
  extern void Md5_Register(sqlite3*);

  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  rc = sqlite3_create_function(db, "x_coalesce", -1, SQLITE_ANY, 0, 
        ifnullFunc, 0, 0);

  /* Use the sqlite3_create_function16() API here. Mainly for fun, but also 
  ** because it is not tested anywhere else. */
  if( rc==SQLITE_OK ){
    pVal = sqlite3ValueNew();
    sqlite3ValueSetStr(pVal, -1, "x_sqlite_exec", SQLITE_UTF8, SQLITE_STATIC);
    rc = sqlite3_create_function16(db, 
              sqlite3ValueText(pVal, SQLITE_UTF16NATIVE),
              1, SQLITE_UTF16, db, sqlite3ExecFunc, 0, 0);
    sqlite3ValueFree(pVal);
  }
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}

/*
** Routines to implement the x_count() aggregate function.
*/
typedef struct CountCtx CountCtx;
struct CountCtx {
  int n;
};
static void countStep(sqlite3_context *context, int argc, sqlite3_value **argv){
  CountCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  if( (argc==0 || SQLITE_NULL!=sqlite3_value_type(argv[0]) ) && p ){
    p->n++;
  }
}   
static void countFinalize(sqlite3_context *context){
  CountCtx *p;
  p = sqlite3_aggregate_context(context, sizeof(*p));
  sqlite3_result_int(context, p ? p->n : 0);
}

/*
** Usage:  sqlite_test_create_aggregate DB
**
** Call the sqlite3_create_function API on the given database in order
** to create a function named "x_count".  This function does the same thing
** as the "md5sum" function.
**
** The original motivation for this routine was to be able to call the
** sqlite3_create_aggregate function while a query is in progress in order
** to test the SQLITE_MISUSE detection logic.
*/
static int test_create_aggregate(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  int rc;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FILENAME\"", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  rc = sqlite3_create_function(db, "x_count", 0, SQLITE_UTF8, 0, 0,
      countStep,countFinalize);
  if( rc==SQLITE_OK ){
    sqlite3_create_function(db, "x_count", 1, SQLITE_UTF8, 0, 0,
        countStep,countFinalize);
  }
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}



/*
** Usage:  sqlite3_mprintf_int FORMAT INTEGER INTEGER INTEGER
**
** Call mprintf with three integer arguments
*/
static int sqlite3_mprintf_int(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int a[3], i;
  char *z;
  if( argc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT INT INT INT\"", 0);
    return TCL_ERROR;
  }
  for(i=2; i<5; i++){
    if( Tcl_GetInt(interp, argv[i], &a[i-2]) ) return TCL_ERROR;
  }
  z = sqlite3_mprintf(argv[1], a[0], a[1], a[2]);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** If zNum represents an integer that will fit in 64-bits, then set
** *pValue to that integer and return true.  Otherwise return false.
*/
static int sqlite3GetInt64(const char *zNum, i64 *pValue){
  if( sqlite3FitsIn64Bits(zNum) ){
    sqlite3atoi64(zNum, pValue);
    return 1;
  }
  return 0;
}

/*
** Usage:  sqlite3_mprintf_int64 FORMAT INTEGER INTEGER INTEGER
**
** Call mprintf with three 64-bit integer arguments
*/
static int sqlite3_mprintf_int64(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int i;
  sqlite_int64 a[3];
  char *z;
  if( argc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT INT INT INT\"", 0);
    return TCL_ERROR;
  }
  for(i=2; i<5; i++){
    if( !sqlite3GetInt64(argv[i], &a[i-2]) ){
      Tcl_AppendResult(interp, "argument is not a valid 64-bit integer", 0);
      return TCL_ERROR;
    }
  }
  z = sqlite3_mprintf(argv[1], a[0], a[1], a[2]);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** Usage:  sqlite3_mprintf_str FORMAT INTEGER INTEGER STRING
**
** Call mprintf with two integer arguments and one string argument
*/
static int sqlite3_mprintf_str(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int a[3], i;
  char *z;
  if( argc<4 || argc>5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT INT INT ?STRING?\"", 0);
    return TCL_ERROR;
  }
  for(i=2; i<4; i++){
    if( Tcl_GetInt(interp, argv[i], &a[i-2]) ) return TCL_ERROR;
  }
  z = sqlite3_mprintf(argv[1], a[0], a[1], argc>4 ? argv[4] : NULL);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** Usage:  sqlite3_mprintf_str FORMAT INTEGER INTEGER DOUBLE
**
** Call mprintf with two integer arguments and one double argument
*/
static int sqlite3_mprintf_double(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int a[3], i;
  double r;
  char *z;
  if( argc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT INT INT STRING\"", 0);
    return TCL_ERROR;
  }
  for(i=2; i<4; i++){
    if( Tcl_GetInt(interp, argv[i], &a[i-2]) ) return TCL_ERROR;
  }
  if( Tcl_GetDouble(interp, argv[4], &r) ) return TCL_ERROR;
  z = sqlite3_mprintf(argv[1], a[0], a[1], r);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** Usage:  sqlite3_mprintf_str FORMAT DOUBLE DOUBLE
**
** Call mprintf with a single double argument which is the product of the
** two arguments given above.  This is used to generate overflow and underflow
** doubles to test that they are converted properly.
*/
static int sqlite3_mprintf_scaled(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int i;
  double r[2];
  char *z;
  if( argc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT DOUBLE DOUBLE\"", 0);
    return TCL_ERROR;
  }
  for(i=2; i<4; i++){
    if( Tcl_GetDouble(interp, argv[i], &r[i-2]) ) return TCL_ERROR;
  }
  z = sqlite3_mprintf(argv[1], r[0]*r[1]);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** Usage:  sqlite3_mprintf_stronly FORMAT STRING
**
** Call mprintf with a single double argument which is the product of the
** two arguments given above.  This is used to generate overflow and underflow
** doubles to test that they are converted properly.
*/
static int sqlite3_mprintf_stronly(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  char *z;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0],
       " FORMAT STRING\"", 0);
    return TCL_ERROR;
  }
  z = sqlite3_mprintf(argv[1], argv[2]);
  Tcl_AppendResult(interp, z, 0);
  sqlite3_free(z);
  return TCL_OK;
}

/*
** Usage: sqlite_malloc_fail N
**
** Rig sqliteMalloc() to fail on the N-th call.  Turn off this mechanism
** and reset the sqlite3_malloc_failed variable is N==0.
*/
#ifdef SQLITE_DEBUG
static int sqlite_malloc_fail(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  int n;
  if( argc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], " N\"", 0);
    return TCL_ERROR;
  }
  if( Tcl_GetInt(interp, argv[1], &n) ) return TCL_ERROR;
  sqlite3_iMallocFail = n;
  sqlite3_malloc_failed = 0;
  return TCL_OK;
}
#endif

/*
** Usage: sqlite_malloc_stat
**
** Return the number of prior calls to sqliteMalloc() and sqliteFree().
*/
#ifdef SQLITE_DEBUG
static int sqlite_malloc_stat(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  char zBuf[200];
  sprintf(zBuf, "%d %d %d", sqlite3_nMalloc, sqlite3_nFree, sqlite3_iMallocFail);
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}
#endif

/*
** Usage:  sqlite_abort
**
** Shutdown the process immediately.  This is not a clean shutdown.
** This command is used to test the recoverability of a database in
** the event of a program crash.
*/
static int sqlite_abort(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  assert( interp==0 );   /* This will always fail */
  return TCL_OK;
}

/*
** The following routine is a user-defined SQL function whose purpose
** is to test the sqlite_set_result() API.
*/
static void testFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  while( argc>=2 ){
    const char *zArg0 = sqlite3_value_text(argv[0]);
    if( zArg0 ){
      if( 0==sqlite3StrICmp(zArg0, "int") ){
        sqlite3_result_int(context, sqlite3_value_int(argv[1]));
      }else if( sqlite3StrICmp(zArg0,"int64")==0 ){
        sqlite3_result_int64(context, sqlite3_value_int64(argv[1]));
      }else if( sqlite3StrICmp(zArg0,"string")==0 ){
        sqlite3_result_text(context, sqlite3_value_text(argv[1]), -1,
            SQLITE_TRANSIENT);
      }else if( sqlite3StrICmp(zArg0,"double")==0 ){
        sqlite3_result_double(context, sqlite3_value_double(argv[1]));
      }else if( sqlite3StrICmp(zArg0,"null")==0 ){
        sqlite3_result_null(context);
      }else if( sqlite3StrICmp(zArg0,"value")==0 ){
        sqlite3_result_value(context, argv[sqlite3_value_int(argv[1])]);
      }else{
        goto error_out;
      }
    }else{
      goto error_out;
    }
    argc -= 2;
    argv += 2;
  }
  return;

error_out:
  sqlite3_result_error(context,"first argument should be one of: "
      "int int64 string double null value", -1);
}

/*
** Usage:   sqlite_register_test_function  DB  NAME
**
** Register the test SQL function on the database DB under the name NAME.
*/
static int test_register_func(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3 *db;
  int rc;
  if( argc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], 
       " DB FUNCTION-NAME", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, argv[1], &db) ) return TCL_ERROR;
  rc = sqlite3_create_function(db, argv[2], -1, SQLITE_UTF8, 0, 
      testFunc, 0, 0);
  if( rc!=0 ){
    Tcl_AppendResult(interp, sqlite3ErrStr(rc), 0);
    return TCL_ERROR;
  }
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}

/*
** Usage:  sqlite3_finalize  STMT 
**
** Finalize a statement handle.
*/
static int test_finalize(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int rc;
  sqlite3 *db;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " <STMT>", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;

  db = StmtToDb(pStmt);
  rc = sqlite3_finalize(pStmt);
  Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;
}

/*
** Usage:  sqlite3_reset  STMT 
**
** Finalize a statement handle.
*/
static int test_reset(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int rc;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " <STMT>", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;

  rc = sqlite3_reset(pStmt);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc ){
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:  sqlite3_changes DB
**
** Return the number of changes made to the database by the last SQL
** execution.
*/
static int test_changes(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
       Tcl_GetString(objv[0]), " DB", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;
  Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_changes(db)));
  return TCL_OK;
}

/*
** This is the "static_bind_value" that variables are bound to when
** the FLAG option of sqlite3_bind is "static"
*/
static char *sqlite_static_bind_value = 0;

/*
** Usage:  sqlite3_bind  VM  IDX  VALUE  FLAGS
**
** Sets the value of the IDX-th occurance of "?" in the original SQL
** string.  VALUE is the new value.  If FLAGS=="null" then VALUE is
** ignored and the value is set to NULL.  If FLAGS=="static" then
** the value is set to the value of a static variable named
** "sqlite_static_bind_value".  If FLAGS=="normal" then a copy
** of the VALUE is made.
*/
static int test_bind(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  sqlite3_stmt *pStmt;
  int rc;
  int idx;
  if( argc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", argv[0], 
       " VM IDX VALUE (null|static|normal)\"", 0);
    return TCL_ERROR;
  }
  if( getStmtPointer(interp, argv[1], &pStmt) ) return TCL_ERROR;
  if( Tcl_GetInt(interp, argv[2], &idx) ) return TCL_ERROR;
  if( strcmp(argv[4],"null")==0 ){
    rc = sqlite3_bind_null(pStmt, idx);
  }else if( strcmp(argv[4],"static")==0 ){
    rc = sqlite3_bind_text(pStmt, idx, sqlite_static_bind_value, -1, 0);
  }else if( strcmp(argv[4],"normal")==0 ){
    rc = sqlite3_bind_text(pStmt, idx, argv[3], -1, SQLITE_TRANSIENT);
  }else{
    Tcl_AppendResult(interp, "4th argument should be "
        "\"null\" or \"static\" or \"normal\"", 0);
    return TCL_ERROR;
  }
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc ){
    char zBuf[50];
    sprintf(zBuf, "(%d) ", rc);
    Tcl_AppendResult(interp, zBuf, sqlite3ErrStr(rc), 0);
    return TCL_ERROR;
  }
  return TCL_OK;
}


/*
** Usage: add_test_collate <db ptr> <utf8> <utf16le> <utf16be>
**
** This function is used to test that SQLite selects the correct collation
** sequence callback when multiple versions (for different text encodings)
** are available.
**
** Calling this routine registers the collation sequence "test_collate"
** with database handle <db>. The second argument must be a list of three
** boolean values. If the first is true, then a version of test_collate is
** registered for UTF-8, if the second is true, a version is registered for
** UTF-16le, if the third is true, a UTF-16be version is available.
** Previous versions of test_collate are deleted.
**
** The collation sequence test_collate is implemented by calling the
** following TCL script:
**
**   "test_collate <enc> <lhs> <rhs>"
**
** The <lhs> and <rhs> are the two values being compared, encoded in UTF-8.
** The <enc> parameter is the encoding of the collation function that
** SQLite selected to call. The TCL test script implements the
** "test_collate" proc.
**
** Note that this will only work with one intepreter at a time, as the
** interp pointer to use when evaluating the TCL script is stored in
** pTestCollateInterp.
*/
static Tcl_Interp* pTestCollateInterp;
static int test_collate_func(
  void *pCtx, 
  int nA, const void *zA,
  int nB, const void *zB
){
  Tcl_Interp *i = pTestCollateInterp;
  int encin = (int)pCtx;
  int res;

  sqlite3_value *pVal;
  Tcl_Obj *pX;

  pX = Tcl_NewStringObj("test_collate", -1);
  Tcl_IncrRefCount(pX);

  switch( encin ){
    case SQLITE_UTF8:
      Tcl_ListObjAppendElement(i,pX,Tcl_NewStringObj("UTF-8",-1));
      break;
    case SQLITE_UTF16LE:
      Tcl_ListObjAppendElement(i,pX,Tcl_NewStringObj("UTF-16LE",-1));
      break;
    case SQLITE_UTF16BE:
      Tcl_ListObjAppendElement(i,pX,Tcl_NewStringObj("UTF-16BE",-1));
      break;
    default:
      assert(0);
  }

  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, nA, zA, encin, SQLITE_STATIC);
  Tcl_ListObjAppendElement(i,pX,Tcl_NewStringObj(sqlite3_value_text(pVal),-1));
  sqlite3ValueSetStr(pVal, nB, zB, encin, SQLITE_STATIC);
  Tcl_ListObjAppendElement(i,pX,Tcl_NewStringObj(sqlite3_value_text(pVal),-1));
  sqlite3ValueFree(pVal);

  Tcl_EvalObjEx(i, pX, 0);
  Tcl_DecrRefCount(pX);
  Tcl_GetIntFromObj(i, Tcl_GetObjResult(i), &res);
  return res;
}
static int test_collate(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  int val;
  sqlite3_value *pVal;
  int rc;

  if( objc!=5 ) goto bad_args;
  pTestCollateInterp = interp;
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;

  if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[2], &val) ) return TCL_ERROR;
  rc = sqlite3_create_collation(db, "test_collate", SQLITE_UTF8, 
          (void *)SQLITE_UTF8, val?test_collate_func:0);
  if( rc==SQLITE_OK ){
    if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[3], &val) ) return TCL_ERROR;
    rc = sqlite3_create_collation(db, "test_collate", SQLITE_UTF16LE, 
            (void *)SQLITE_UTF16LE, val?test_collate_func:0);
    if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[4], &val) ) return TCL_ERROR;

    pVal = sqlite3ValueNew();
    sqlite3ValueSetStr(pVal, -1, "test_collate", SQLITE_UTF8, SQLITE_STATIC);
    sqlite3_create_collation16(db, sqlite3ValueText(pVal, SQLITE_UTF16NATIVE), 
          SQLITE_UTF16BE, (void *)SQLITE_UTF16BE, val?test_collate_func:0);
    sqlite3ValueFree(pVal);
  }
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;

bad_args:
  Tcl_AppendResult(interp, "wrong # args: should be \"",
      Tcl_GetStringFromObj(objv[0], 0), " <DB> <utf8> <utf16le> <utf16be>", 0);
  return TCL_ERROR;
}

static void test_collate_needed_cb(
  void *pCtx, 
  sqlite3 *db,
  int eTextRep,
  const void *notUsed
){
  int enc = db->enc;
  sqlite3_create_collation(
      db, "test_collate", db->enc, (void *)enc, test_collate_func);
}

/*
** Usage: add_test_collate_needed DB
*/
static int test_collate_needed(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  int rc;

  if( objc!=2 ) goto bad_args;
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;
  rc = sqlite3_collation_needed16(db, 0, test_collate_needed_cb);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  return TCL_OK;

bad_args:
  Tcl_WrongNumArgs(interp, 1, objv, "DB");
  return TCL_ERROR;
}

/*
** Usage: add_test_function <db ptr> <utf8> <utf16le> <utf16be>
**
** This function is used to test that SQLite selects the correct user
** function callback when multiple versions (for different text encodings)
** are available.
**
** Calling this routine registers up to three versions of the user function
** "test_function" with database handle <db>.  If the second argument is
** true, then a version of test_function is registered for UTF-8, if the
** third is true, a version is registered for UTF-16le, if the fourth is
** true, a UTF-16be version is available.  Previous versions of
** test_function are deleted.
**
** The user function is implemented by calling the following TCL script:
**
**   "test_function <enc> <arg>"
**
** Where <enc> is one of UTF-8, UTF-16LE or UTF16BE, and <arg> is the
** single argument passed to the SQL function. The value returned by
** the TCL script is used as the return value of the SQL function. It
** is passed to SQLite using UTF-16BE for a UTF-8 test_function(), UTF-8
** for a UTF-16LE test_function(), and UTF-16LE for an implementation that
** prefers UTF-16BE.
*/
static void test_function_utf8(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  Tcl_Interp *interp;
  Tcl_Obj *pX;
  sqlite3_value *pVal;
  interp = (Tcl_Interp *)sqlite3_user_data(pCtx);
  pX = Tcl_NewStringObj("test_function", -1);
  Tcl_IncrRefCount(pX);
  Tcl_ListObjAppendElement(interp, pX, Tcl_NewStringObj("UTF-8", -1));
  Tcl_ListObjAppendElement(interp, pX, 
      Tcl_NewStringObj(sqlite3_value_text(argv[0]), -1));
  Tcl_EvalObjEx(interp, pX, 0);
  Tcl_DecrRefCount(pX);
  sqlite3_result_text(pCtx, Tcl_GetStringResult(interp), -1, SQLITE_TRANSIENT);
  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, -1, Tcl_GetStringResult(interp), 
      SQLITE_UTF8, SQLITE_STATIC);
  sqlite3_result_text16be(pCtx, sqlite3_value_text16be(pVal),
      -1, SQLITE_TRANSIENT);
  sqlite3ValueFree(pVal);
}
static void test_function_utf16le(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  Tcl_Interp *interp;
  Tcl_Obj *pX;
  sqlite3_value *pVal;
  interp = (Tcl_Interp *)sqlite3_user_data(pCtx);
  pX = Tcl_NewStringObj("test_function", -1);
  Tcl_IncrRefCount(pX);
  Tcl_ListObjAppendElement(interp, pX, Tcl_NewStringObj("UTF-16LE", -1));
  Tcl_ListObjAppendElement(interp, pX, 
      Tcl_NewStringObj(sqlite3_value_text(argv[0]), -1));
  Tcl_EvalObjEx(interp, pX, 0);
  Tcl_DecrRefCount(pX);
  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, -1, Tcl_GetStringResult(interp), 
      SQLITE_UTF8, SQLITE_STATIC);
  sqlite3_result_text(pCtx,sqlite3_value_text(pVal),-1,SQLITE_TRANSIENT);
  sqlite3ValueFree(pVal);
}
static void test_function_utf16be(
  sqlite3_context *pCtx, 
  int nArg,
  sqlite3_value **argv
){
  Tcl_Interp *interp;
  Tcl_Obj *pX;
  sqlite3_value *pVal;
  interp = (Tcl_Interp *)sqlite3_user_data(pCtx);
  pX = Tcl_NewStringObj("test_function", -1);
  Tcl_IncrRefCount(pX);
  Tcl_ListObjAppendElement(interp, pX, Tcl_NewStringObj("UTF-16BE", -1));
  Tcl_ListObjAppendElement(interp, pX, 
      Tcl_NewStringObj(sqlite3_value_text(argv[0]), -1));
  Tcl_EvalObjEx(interp, pX, 0);
  Tcl_DecrRefCount(pX);
  pVal = sqlite3ValueNew();
  sqlite3ValueSetStr(pVal, -1, Tcl_GetStringResult(interp), 
      SQLITE_UTF8, SQLITE_STATIC);
  sqlite3_result_text16le(pCtx, sqlite3_value_text16le(pVal),
      -1, SQLITE_TRANSIENT);
  sqlite3ValueFree(pVal);
}
static int test_function(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  int val;

  if( objc!=5 ) goto bad_args;
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;

  if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[2], &val) ) return TCL_ERROR;
  if( val ){
    sqlite3_create_function(db, "test_function", 1, SQLITE_UTF8, 
        interp, test_function_utf8, 0, 0);
  }
  if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[3], &val) ) return TCL_ERROR;
  if( val ){
    sqlite3_create_function(db, "test_function", 1, SQLITE_UTF16LE, 
        interp, test_function_utf16le, 0, 0);
  }
  if( TCL_OK!=Tcl_GetBooleanFromObj(interp, objv[4], &val) ) return TCL_ERROR;
  if( val ){
    sqlite3_create_function(db, "test_function", 1, SQLITE_UTF16BE, 
        interp, test_function_utf16be, 0, 0);
  }

  return TCL_OK;
bad_args:
  Tcl_AppendResult(interp, "wrong # args: should be \"",
      Tcl_GetStringFromObj(objv[0], 0), " <DB> <utf8> <utf16le> <utf16be>", 0);
  return TCL_ERROR;
}

/*
** Usage:         test_errstr <err code>
**
** Test that the english language string equivalents for sqlite error codes
** are sane. The parameter is an integer representing an sqlite error code.
** The result is a list of two elements, the string representation of the
** error code and the english language explanation.
*/
static int test_errstr(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  char *zCode;
  int i;
  if( objc!=1 ){
    Tcl_WrongNumArgs(interp, 1, objv, "<error code>");
  }

  zCode = Tcl_GetString(objv[1]);
  for(i=0; i<200; i++){
    if( 0==strcmp(errorName(i), zCode) ) break;
  }
  Tcl_SetResult(interp, (char *)sqlite3ErrStr(i), 0);
  return TCL_OK;
}

static int sqlite3_crashparams(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
#ifdef OS_TEST
  int delay;
  if( objc!=3 ) goto bad_args;
  if( Tcl_GetIntFromObj(interp, objv[1], &delay) ) return TCL_ERROR;
  sqlite3SetCrashParams(delay, Tcl_GetString(objv[2]));
#endif
  return TCL_OK;

#ifdef OS_TEST
bad_args:
  Tcl_AppendResult(interp, "wrong # args: should be \"",
      Tcl_GetStringFromObj(objv[0], 0), "<delay> <filename>", 0);
  return TCL_ERROR;
#endif
}


/*
** Usage:    breakpoint
**
** This routine exists for one purpose - to provide a place to put a
** breakpoint with GDB that can be triggered using TCL code.  The use
** for this is when a particular test fails on (say) the 1485th iteration.
** In the TCL test script, we can add code like this:
**
**     if {$i==1485} breakpoint
**
** Then run testfixture in the debugger and wait for the breakpoint to
** fire.  Then additional breakpoints can be set to trace down the bug.
*/
static int test_breakpoint(
  void *NotUsed,
  Tcl_Interp *interp,    /* The TCL interpreter that invoked this command */
  int argc,              /* Number of arguments */
  char **argv            /* Text of each argument */
){
  return TCL_OK;         /* Do nothing */
}

/*
** Usage:   sqlite3_bind_int  STMT N VALUE
**
** Test the sqlite3_bind_int interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a 32-bit integer VALUE to that wildcard.
*/
static int test_bind_int(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  int value;
  int rc;

  if( objc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N VALUE", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[3], &value) ) return TCL_ERROR;

  rc = sqlite3_bind_int(pStmt, idx, value);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}


/*
** Usage:   sqlite3_bind_int64  STMT N VALUE
**
** Test the sqlite3_bind_int64 interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a 64-bit integer VALUE to that wildcard.
*/
static int test_bind_int64(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  i64 value;
  int rc;

  if( objc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N VALUE", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  if( Tcl_GetWideIntFromObj(interp, objv[3], &value) ) return TCL_ERROR;

  rc = sqlite3_bind_int64(pStmt, idx, value);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}


/*
** Usage:   sqlite3_bind_double  STMT N VALUE
**
** Test the sqlite3_bind_double interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a 64-bit integer VALUE to that wildcard.
*/
static int test_bind_double(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  double value;
  int rc;

  if( objc!=4 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N VALUE", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  if( Tcl_GetDoubleFromObj(interp, objv[3], &value) ) return TCL_ERROR;

  rc = sqlite3_bind_double(pStmt, idx, value);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_null  STMT N
**
** Test the sqlite3_bind_null interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a NULL to the wildcard.
*/
static int test_bind_null(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  int rc;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;

  rc = sqlite3_bind_null(pStmt, idx);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_text  STMT N STRING BYTES
**
** Test the sqlite3_bind_text interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a UTF-8 string STRING to the wildcard.  The string is BYTES bytes
** long.
*/
static int test_bind_text(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  int bytes;
  char *value;
  int rc;

  if( objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N VALUE BYTES", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  value = Tcl_GetString(objv[3]);
  if( Tcl_GetIntFromObj(interp, objv[4], &bytes) ) return TCL_ERROR;

  rc = sqlite3_bind_text(pStmt, idx, value, bytes, SQLITE_TRANSIENT);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_text16  STMT N STRING BYTES
**
** Test the sqlite3_bind_text16 interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a UTF-16 string STRING to the wildcard.  The string is BYTES bytes
** long.
*/
static int test_bind_text16(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  int bytes;
  char *value;
  int rc;

  if( objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N VALUE BYTES", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  value = Tcl_GetByteArrayFromObj(objv[3], 0);
  if( Tcl_GetIntFromObj(interp, objv[4], &bytes) ) return TCL_ERROR;

  rc = sqlite3_bind_text16(pStmt, idx, (void *)value, bytes, SQLITE_TRANSIENT);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_blob  STMT N DATA BYTES
**
** Test the sqlite3_bind_blob interface.  STMT is a prepared statement.
** N is the index of a wildcard in the prepared statement.  This command
** binds a BLOB to the wildcard.  The BLOB is BYTES bytes in size.
*/
static int test_bind_blob(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int idx;
  int bytes;
  char *value;
  int rc;

  if( objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"",
        Tcl_GetStringFromObj(objv[0], 0), " STMT N DATA BYTES", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &idx) ) return TCL_ERROR;
  value = Tcl_GetString(objv[3]);
  if( Tcl_GetIntFromObj(interp, objv[4], &bytes) ) return TCL_ERROR;

  rc = sqlite3_bind_blob(pStmt, idx, value, bytes, SQLITE_TRANSIENT);
  if( sqlite3TestErrCode(interp, StmtToDb(pStmt), rc) ) return TCL_ERROR;
  if( rc!=SQLITE_OK ){
    return TCL_ERROR;
  }

  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_parameter_count  STMT
**
** Return the number of wildcards in the given statement.
*/
static int test_bind_parameter_count(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;

  if( objc!=2 ){
    Tcl_WrongNumArgs(interp, 1, objv, "STMT");
    return TCL_ERROR;
  }
  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_bind_parameter_count(pStmt)));
  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_parameter_name  STMT  N
**
** Return the name of the Nth wildcard.  The first wildcard is 1.
** An empty string is returned if N is out of range or if the wildcard
** is nameless.
*/
static int test_bind_parameter_name(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int i;

  if( objc!=3 ){
    Tcl_WrongNumArgs(interp, 1, objv, "STMT N");
    return TCL_ERROR;
  }
  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &i) ) return TCL_ERROR;
  Tcl_SetObjResult(interp, 
     Tcl_NewStringObj(sqlite3_bind_parameter_name(pStmt,i),-1)
  );
  return TCL_OK;
}

/*
** Usage:   sqlite3_bind_parameter_index  STMT  NAME
**
** Return the index of the wildcard called NAME.  Return 0 if there is
** no such wildcard.
*/
static int test_bind_parameter_index(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;

  if( objc!=3 ){
    Tcl_WrongNumArgs(interp, 1, objv, "STMT NAME");
    return TCL_ERROR;
  }
  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  Tcl_SetObjResult(interp, 
     Tcl_NewIntObj(
       sqlite3_bind_parameter_index(pStmt,Tcl_GetString(objv[2]))
     )
  );
  return TCL_OK;
}

/*
** Usage: sqlite3_errcode DB
**
** Return the string representation of the most recent sqlite3_* API
** error code. e.g. "SQLITE_ERROR".
*/
static int test_errcode(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " DB", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;
  Tcl_SetResult(interp, (char *)errorName(sqlite3_errcode(db)), 0);
  return TCL_OK;
}

/*
** Usage:   test_errmsg DB
**
** Returns the UTF-8 representation of the error message string for the
** most recent sqlite3_* API call.
*/
static int test_errmsg(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  const char *zErr;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " DB", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;

  zErr = sqlite3_errmsg(db);
  Tcl_SetObjResult(interp, Tcl_NewStringObj(zErr, -1));
  return TCL_OK;
}

/*
** Usage:   test_errmsg16 DB
**
** Returns the UTF-16 representation of the error message string for the
** most recent sqlite3_* API call. This is a byte array object at the TCL 
** level, and it includes the 0x00 0x00 terminator bytes at the end of the
** UTF-16 string.
*/
static int test_errmsg16(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  const void *zErr;
  int bytes;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " DB", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;

  zErr = sqlite3_errmsg16(db);
  bytes = sqlite3utf16ByteLen(zErr, -1);
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(zErr, bytes));
  return TCL_OK;
}

/*
** Usage: sqlite3_prepare DB sql bytes tailvar
**
** Compile up to <bytes> bytes of the supplied SQL string <sql> using
** database handle <DB>. The parameter <tailval> is the name of a global
** variable that is set to the unused portion of <sql> (if any). A
** STMT handle is returned.
*/
static int test_prepare(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  const char *zSql;
  int bytes;
  const char *zTail = 0;
  sqlite3_stmt *pStmt = 0;
  char zBuf[50];
  int rc;

  if( objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " DB sql bytes tailvar", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;
  zSql = Tcl_GetString(objv[2]);
  if( Tcl_GetIntFromObj(interp, objv[3], &bytes) ) return TCL_ERROR;

  rc = sqlite3_prepare(db, zSql, bytes, &pStmt, &zTail);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  if( zTail ){
    if( bytes>=0 ){
      bytes = bytes - (zTail-zSql);
    }
    Tcl_ObjSetVar2(interp, objv[4], 0, Tcl_NewStringObj(zTail, bytes), 0);
  }
  if( rc!=SQLITE_OK ){
    assert( pStmt==0 );
    sprintf(zBuf, "(%d) ", rc);
    Tcl_AppendResult(interp, zBuf, sqlite3_errmsg(db), 0);
    return TCL_ERROR;
  }

  if( pStmt ){
    if( makePointerStr(interp, zBuf, pStmt) ) return TCL_ERROR;
    Tcl_AppendResult(interp, zBuf, 0);
  }
  return TCL_OK;
}

/*
** Usage: sqlite3_prepare DB sql bytes tailvar
**
** Compile up to <bytes> bytes of the supplied SQL string <sql> using
** database handle <DB>. The parameter <tailval> is the name of a global
** variable that is set to the unused portion of <sql> (if any). A
** STMT handle is returned.
*/
static int test_prepare16(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3 *db;
  const void *zSql;
  const void *zTail = 0;
  Tcl_Obj *pTail = 0;
  sqlite3_stmt *pStmt = 0;
  char zBuf[50]; 
  int rc;
  int bytes;                /* The integer specified as arg 3 */
  int objlen;               /* The byte-array length of arg 2 */

  if( objc!=5 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " DB sql bytes tailvar", 0);
    return TCL_ERROR;
  }
  if( getDbPointer(interp, Tcl_GetString(objv[1]), &db) ) return TCL_ERROR;
  zSql = Tcl_GetByteArrayFromObj(objv[2], &objlen);
  if( Tcl_GetIntFromObj(interp, objv[3], &bytes) ) return TCL_ERROR;

  rc = sqlite3_prepare16(db, zSql, bytes, &pStmt, &zTail);
  if( sqlite3TestErrCode(interp, db, rc) ) return TCL_ERROR;
  if( rc ){
    return TCL_ERROR;
  }

  if( zTail ){
    objlen = objlen - ((u8 *)zTail-(u8 *)zSql);
  }else{
    objlen = 0;
  }
  pTail = Tcl_NewByteArrayObj((u8 *)zTail, objlen);
  Tcl_IncrRefCount(pTail);
  Tcl_ObjSetVar2(interp, objv[4], 0, pTail, 0);
  Tcl_DecrRefCount(pTail);

  if( pStmt ){
    if( makePointerStr(interp, zBuf, pStmt) ) return TCL_ERROR;
  }
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage: sqlite3_open filename ?options-list?
*/
static int test_open(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  const char *zFilename;
  sqlite3 *db;
  int rc;
  char zBuf[100];

  if( objc!=3 && objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " filename options-list", 0);
    return TCL_ERROR;
  }

  zFilename = Tcl_GetString(objv[1]);
  rc = sqlite3_open(zFilename, &db);
  
  if( makePointerStr(interp, zBuf, db) ) return TCL_ERROR;
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage: sqlite3_open16 filename options
*/
static int test_open16(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  const void *zFilename;
  sqlite3 *db;
  int rc;
  char zBuf[100];

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " filename options-list", 0);
    return TCL_ERROR;
  }

  zFilename = Tcl_GetByteArrayFromObj(objv[1], 0);
  rc = sqlite3_open16(zFilename, &db);
  
  if( makePointerStr(interp, zBuf, db) ) return TCL_ERROR;
  Tcl_AppendResult(interp, zBuf, 0);
  return TCL_OK;
}

/*
** Usage: sqlite3_complete16 <UTF-16 string>
**
** Return 1 if the supplied argument is a complete SQL statement, or zero
** otherwise.
*/
static int test_complete16(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  char *zBuf;

  if( objc!=2 ){
    Tcl_WrongNumArgs(interp, 1, objv, "<utf-16 sql>");
    return TCL_ERROR;
  }

  zBuf = Tcl_GetByteArrayFromObj(objv[1], 0);
  Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_complete16(zBuf)));
  return TCL_OK;
}

/*
** Usage: sqlite3_step STMT
**
** Advance the statement to the next row.
*/
static int test_step(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int rc;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  rc = sqlite3_step(pStmt);

  /* if( rc!=SQLITE_DONE && rc!=SQLITE_ROW ) return TCL_ERROR; */
  Tcl_SetResult(interp, (char *)errorName(rc), 0);
  return TCL_OK;
}

/*
** Usage: sqlite3_column_count STMT 
**
** Return the number of columns returned by the sql statement STMT.
*/
static int test_column_count(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_column_count(pStmt)));
  return TCL_OK;
}

/*
** Usage: sqlite3_column_type STMT column
**
** Return the type of the data in column 'column' of the current row.
*/
static int test_column_type(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  int tp;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  tp = sqlite3_column_type(pStmt, col);
  switch( tp ){
    case SQLITE_INTEGER: 
      Tcl_SetResult(interp, "INTEGER", TCL_STATIC); 
      break;
    case SQLITE_NULL:
      Tcl_SetResult(interp, "NULL", TCL_STATIC); 
      break;
    case SQLITE_FLOAT:
      Tcl_SetResult(interp, "FLOAT", TCL_STATIC); 
      break;
    case SQLITE_TEXT:
      Tcl_SetResult(interp, "TEXT", TCL_STATIC); 
      break;
    case SQLITE_BLOB:
      Tcl_SetResult(interp, "BLOB", TCL_STATIC); 
      break;
    default:
      assert(0);
  }

  return TCL_OK;
}

/*
** Usage: sqlite3_column_int64 STMT column
**
** Return the data in column 'column' of the current row cast as an
** wide (64-bit) integer.
*/
static int test_column_int64(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  i64 iVal;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  iVal = sqlite3_column_int64(pStmt, col);
  Tcl_SetObjResult(interp, Tcl_NewWideIntObj(iVal));
  return TCL_OK;
}

/*
** Usage: sqlite3_column_blob STMT column
*/
static int test_column_blob(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;

  int len;
  const void *pBlob;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  pBlob = sqlite3_column_blob(pStmt, col);
  len = sqlite3_column_bytes(pStmt, col);
  Tcl_SetObjResult(interp, Tcl_NewByteArrayObj(pBlob, len));
  return TCL_OK;
}

/*
** Usage: sqlite3_column_double STMT column
**
** Return the data in column 'column' of the current row cast as a double.
*/
static int test_column_double(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  double rVal;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  rVal = sqlite3_column_double(pStmt, col);
  Tcl_SetObjResult(interp, Tcl_NewDoubleObj(rVal));
  return TCL_OK;
}

/*
** Usage: sqlite3_data_count STMT 
**
** Return the number of columns returned by the sql statement STMT.
*/
static int test_data_count(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;

  Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_data_count(pStmt)));
  return TCL_OK;
}

/*
** Usage: sqlite3_column_text STMT column
**
** Usage: sqlite3_column_decltype STMT column
**
** Usage: sqlite3_column_name STMT column
*/
static int test_stmt_utf8(
  void * clientData,        /* Pointer to SQLite API function to be invoke */
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  const char *(*xFunc)(sqlite3_stmt*, int) = clientData;
  const char *zRet;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;
  zRet = xFunc(pStmt, col);
  if( zRet ){
    Tcl_SetResult(interp, (char *)zRet, 0);
  }
  return TCL_OK;
}

/*
** Usage: sqlite3_column_text STMT column
**
** Usage: sqlite3_column_decltype STMT column
**
** Usage: sqlite3_column_name STMT column
*/
static int test_stmt_utf16(
  void * clientData,     /* Pointer to SQLite API function to be invoked */
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  Tcl_Obj *pRet;
  const void *zName16;
  const void *(*xFunc)(sqlite3_stmt*, int) = clientData;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  zName16 = xFunc(pStmt, col);
  if( zName16 ){
    pRet = Tcl_NewByteArrayObj(zName16, sqlite3utf16ByteLen(zName16, -1)+2);
    Tcl_SetObjResult(interp, pRet);
  }

  return TCL_OK;
}

/*
** Usage: sqlite3_column_int STMT column
**
** Usage: sqlite3_column_bytes STMT column
**
** Usage: sqlite3_column_bytes16 STMT column
**
*/
static int test_stmt_int(
  void * clientData,    /* Pointer to SQLite API function to be invoked */
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  sqlite3_stmt *pStmt;
  int col;
  int (*xFunc)(sqlite3_stmt*, int) = clientData;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " STMT column", 0);
    return TCL_ERROR;
  }

  if( getStmtPointer(interp, Tcl_GetString(objv[1]), &pStmt) ) return TCL_ERROR;
  if( Tcl_GetIntFromObj(interp, objv[2], &col) ) return TCL_ERROR;

  Tcl_SetObjResult(interp, Tcl_NewIntObj(xFunc(pStmt, col)));
  return TCL_OK;
}

/*
** Usage:  sqlite3OsOpenReadWrite <filename>
*/
static int test_sqlite3OsOpenReadWrite(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  OsFile * pFile;
  int rc;
  int dummy;
  char zBuf[100];

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " filename", 0);
    return TCL_ERROR;
  }

  pFile = sqliteMalloc(sizeof(OsFile));
  rc = sqlite3OsOpenReadWrite(Tcl_GetString(objv[1]), pFile, &dummy);
  if( rc!=SQLITE_OK ){
    sqliteFree(pFile);
    Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
    return TCL_ERROR;
  }
  makePointerStr(interp, zBuf, pFile);
  Tcl_SetResult(interp, zBuf, 0);
  return TCL_ERROR;
}

/*
** Usage:  sqlite3OsClose <file handle>
*/
static int test_sqlite3OsClose(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  OsFile * pFile;
  int rc;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " filehandle", 0);
    return TCL_ERROR;
  }

  if( getFilePointer(interp, Tcl_GetString(objv[1]), &pFile) ){
    return TCL_ERROR;
  }
  rc = sqlite3OsClose(pFile);
  if( rc!=SQLITE_OK ){
    Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
    return TCL_ERROR;
  }
  sqliteFree(pFile);
  return TCL_OK;
}

/*
** Usage:  sqlite3OsLock <file handle> <locktype>
*/
static int test_sqlite3OsLock(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  OsFile * pFile;
  int rc;

  if( objc!=3 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
        Tcl_GetString(objv[0]), 
        " filehandle (SHARED|RESERVED|PENDING|EXCLUSIVE)", 0);
    return TCL_ERROR;
  }

  if( getFilePointer(interp, Tcl_GetString(objv[1]), &pFile) ){
    return TCL_ERROR;
  }

  if( 0==strcmp("SHARED", Tcl_GetString(objv[2])) ){
    rc = sqlite3OsLock(pFile, SHARED_LOCK);
  }
  else if( 0==strcmp("RESERVED", Tcl_GetString(objv[2])) ){
    rc = sqlite3OsLock(pFile, RESERVED_LOCK);
  }
  else if( 0==strcmp("PENDING", Tcl_GetString(objv[2])) ){
    rc = sqlite3OsLock(pFile, PENDING_LOCK);
  }
  else if( 0==strcmp("EXCLUSIVE", Tcl_GetString(objv[2])) ){
    rc = sqlite3OsLock(pFile, EXCLUSIVE_LOCK);
  }else{
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
        Tcl_GetString(objv[0]), 
        " filehandle (SHARED|RESERVED|PENDING|EXCLUSIVE)", 0);
    return TCL_ERROR;
  }

  if( rc!=SQLITE_OK ){
    Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:  sqlite3OsUnlock <file handle>
*/
static int test_sqlite3OsUnlock(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  OsFile * pFile;
  int rc;

  if( objc!=2 ){
    Tcl_AppendResult(interp, "wrong # args: should be \"", 
       Tcl_GetString(objv[0]), " filehandle", 0);
    return TCL_ERROR;
  }

  if( getFilePointer(interp, Tcl_GetString(objv[1]), &pFile) ){
    return TCL_ERROR;
  }
  rc = sqlite3OsUnlock(pFile, NO_LOCK);
  if( rc!=SQLITE_OK ){
    Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
** Usage:  sqlite3OsTempFileName
*/
static int test_sqlite3OsTempFileName(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  char zFile[SQLITE_TEMPNAME_SIZE];
  int rc;

  rc = sqlite3OsTempFileName(zFile);
  if( rc!=SQLITE_OK ){
    Tcl_SetResult(interp, (char *)errorName(rc), TCL_STATIC);
    return TCL_ERROR;
  }
  Tcl_AppendResult(interp, zFile, 0);
  return TCL_OK;
}

/*
** Usage:  tcl_variable_type VARIABLENAME
**
** Return the name of the internal representation for the
** value of the given variable.
*/
static int tcl_variable_type(
  void * clientData,
  Tcl_Interp *interp,
  int objc,
  Tcl_Obj *CONST objv[]
){
  Tcl_Obj *pVar;
  if( objc!=2 ){
    Tcl_WrongNumArgs(interp, 1, objv, "VARIABLE");
    return TCL_ERROR;
  }
  pVar = Tcl_GetVar2Ex(interp, Tcl_GetString(objv[1]), 0, TCL_LEAVE_ERR_MSG);
  if( pVar==0 ) return TCL_ERROR;
  if( pVar->typePtr ){
    Tcl_SetObjResult(interp, Tcl_NewStringObj(pVar->typePtr->name, -1));
  }
  return TCL_OK;
}

/*
** Register commands with the TCL interpreter.
*/
int Sqlitetest1_Init(Tcl_Interp *interp){
  extern int sqlite3_search_count;
  extern int sqlite3_interrupt_count;
  extern int sqlite3_open_file_count;
  extern int sqlite3_current_time;
  static struct {
     char *zName;
     Tcl_CmdProc *xProc;
  } aCmd[] = {
     { "sqlite3_mprintf_int",           (Tcl_CmdProc*)sqlite3_mprintf_int    },
     { "sqlite3_mprintf_int64",         (Tcl_CmdProc*)sqlite3_mprintf_int64  },
     { "sqlite3_mprintf_str",           (Tcl_CmdProc*)sqlite3_mprintf_str    },
     { "sqlite3_mprintf_stronly",       (Tcl_CmdProc*)sqlite3_mprintf_stronly},
     { "sqlite3_mprintf_double",        (Tcl_CmdProc*)sqlite3_mprintf_double },
     { "sqlite3_mprintf_scaled",        (Tcl_CmdProc*)sqlite3_mprintf_scaled },
     { "sqlite3_mprintf_z_test",        (Tcl_CmdProc*)test_mprintf_z        },
     { "sqlite3_last_insert_rowid",     (Tcl_CmdProc*)test_last_rowid       },
     { "sqlite3_exec_printf",           (Tcl_CmdProc*)test_exec_printf      },
     { "sqlite3_get_table_printf",      (Tcl_CmdProc*)test_get_table_printf },
     { "sqlite3_close",                 (Tcl_CmdProc*)sqlite_test_close     },
     { "sqlite3_create_function",       (Tcl_CmdProc*)test_create_function  },
     { "sqlite3_create_aggregate",      (Tcl_CmdProc*)test_create_aggregate },
     { "sqlite_register_test_function", (Tcl_CmdProc*)test_register_func    },
     { "sqlite_abort",                  (Tcl_CmdProc*)sqlite_abort          },
#ifdef SQLITE_DEBUG
     { "sqlite_malloc_fail",            (Tcl_CmdProc*)sqlite_malloc_fail    },
     { "sqlite_malloc_stat",            (Tcl_CmdProc*)sqlite_malloc_stat    },
#endif
     { "sqlite_bind",                   (Tcl_CmdProc*)test_bind             },
     { "breakpoint",                    (Tcl_CmdProc*)test_breakpoint       },
     { "sqlite3_key",                   (Tcl_CmdProc*)test_key              },
     { "sqlite3_rekey",                 (Tcl_CmdProc*)test_rekey            },
  };
  static struct {
     char *zName;
     Tcl_ObjCmdProc *xProc;
     void *clientData;
  } aObjCmd[] = {
     { "sqlite3_bind_int",              test_bind_int,      0 },
     { "sqlite3_bind_int64",            test_bind_int64,    0 },
     { "sqlite3_bind_double",           test_bind_double,   0 },
     { "sqlite3_bind_null",             test_bind_null     ,0 },
     { "sqlite3_bind_text",             test_bind_text     ,0 },
     { "sqlite3_bind_text16",           test_bind_text16   ,0 },
     { "sqlite3_bind_blob",             test_bind_blob     ,0 },
     { "sqlite3_bind_parameter_count",  test_bind_parameter_count, 0},
     { "sqlite3_bind_parameter_name",   test_bind_parameter_name,  0},
     { "sqlite3_bind_parameter_index",  test_bind_parameter_index, 0},
     { "sqlite3_errcode",               test_errcode       ,0 },
     { "sqlite3_errmsg",                test_errmsg        ,0 },
     { "sqlite3_errmsg16",              test_errmsg16      ,0 },
     { "sqlite3_open",                  test_open          ,0 },
     { "sqlite3_open16",                test_open16        ,0 },
     { "sqlite3_complete16",            test_complete16    ,0 },

     { "sqlite3_prepare",               test_prepare       ,0 },
     { "sqlite3_prepare16",             test_prepare16     ,0 },
     { "sqlite3_finalize",              test_finalize      ,0 },
     { "sqlite3_reset",                 test_reset         ,0 },
     { "sqlite3_changes",               test_changes       ,0 },
     { "sqlite3_step",                  test_step          ,0 },

     /* sqlite3_column_*() API */
     { "sqlite3_column_count",          test_column_count  ,0 },
     { "sqlite3_data_count",            test_data_count    ,0 },
     { "sqlite3_column_type",           test_column_type   ,0 },
     { "sqlite3_column_blob",           test_column_blob   ,0 },
     { "sqlite3_column_double",         test_column_double ,0 },
     { "sqlite3_column_int64",          test_column_int64  ,0 },
     { "sqlite3_column_int",        test_stmt_int,   sqlite3_column_int       },
     { "sqlite3_column_bytes",      test_stmt_int,   sqlite3_column_bytes     },
     { "sqlite3_column_bytes16",    test_stmt_int,   sqlite3_column_bytes16   },
     { "sqlite3_column_text",       test_stmt_utf8,  sqlite3_column_text      },
     { "sqlite3_column_decltype",   test_stmt_utf8,  sqlite3_column_decltype  },
     { "sqlite3_column_name",       test_stmt_utf8,  sqlite3_column_name      },
     { "sqlite3_column_text16",     test_stmt_utf16, sqlite3_column_text16    },
     { "sqlite3_column_decltype16", test_stmt_utf16, sqlite3_column_decltype16},
     { "sqlite3_column_name16",     test_stmt_utf16, sqlite3_column_name16    },

     /* Functions from os.h */
     { "sqlite3OsOpenReadWrite",test_sqlite3OsOpenReadWrite, 0 },
     { "sqlite3OsClose",        test_sqlite3OsClose, 0 },
     { "sqlite3OsLock",         test_sqlite3OsLock, 0 },
     { "sqlite3OsTempFileName", test_sqlite3OsTempFileName, 0 },
   
     /* Custom test interfaces */
     { "sqlite3OsUnlock",         test_sqlite3OsUnlock, 0    },
     { "add_test_collate",        test_collate, 0            },
     { "add_test_collate_needed", test_collate_needed, 0     },
     { "add_test_function",       test_function, 0           },
     { "sqlite3_crashparams",     sqlite3_crashparams, 0     },
     { "sqlite3_test_errstr",     test_errstr, 0             },
     { "tcl_variable_type",       tcl_variable_type, 0       },

  };
  int i;
  extern int sqlite3_os_trace;

  for(i=0; i<sizeof(aCmd)/sizeof(aCmd[0]); i++){
    Tcl_CreateCommand(interp, aCmd[i].zName, aCmd[i].xProc, 0, 0);
  }
  for(i=0; i<sizeof(aObjCmd)/sizeof(aObjCmd[0]); i++){
    Tcl_CreateObjCommand(interp, aObjCmd[i].zName, 
        aObjCmd[i].xProc, aObjCmd[i].clientData, 0);
  }
  Tcl_LinkVar(interp, "sqlite_search_count", 
      (char*)&sqlite3_search_count, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_interrupt_count", 
      (char*)&sqlite3_interrupt_count, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_open_file_count", 
      (char*)&sqlite3_open_file_count, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_current_time", 
      (char*)&sqlite3_current_time, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_os_trace",
      (char*)&sqlite3_os_trace, TCL_LINK_INT);
  Tcl_LinkVar(interp, "sqlite_static_bind_value",
      (char*)&sqlite_static_bind_value, TCL_LINK_STRING);
  Tcl_LinkVar(interp, "sqlite_temp_directory",
      (char*)&sqlite3_temp_directory, TCL_LINK_STRING);
  return TCL_OK;
}
