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
** A TCL Interface to SQLite
**
** $Id$
*/
#ifndef NO_TCL     /* Omit this whole file if TCL is unavailable */

#include "sqliteInt.h"
#include "hash.h"
#include "tcl.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
** If TCL uses UTF-8 and SQLite is configured to use iso8859, then we
** have to do a translation when going between the two.  Set the 
** UTF_TRANSLATION_NEEDED macro to indicate that we need to do
** this translation.  
*/
#if defined(TCL_UTF_MAX) && !defined(SQLITE_UTF8)
# define UTF_TRANSLATION_NEEDED 1
#endif

/*
** New SQL functions can be created as TCL scripts.  Each such function
** is described by an instance of the following structure.
*/
typedef struct SqlFunc SqlFunc;
struct SqlFunc {
  Tcl_Interp *interp;   /* The TCL interpret to execute the function */
  char *zScript;        /* The script to be run */
  SqlFunc *pNext;       /* Next function on the list of them all */
};

/*
** New collation sequences function can be created as TCL scripts.  Each such
** function is described by an instance of the following structure.
*/
typedef struct SqlCollate SqlCollate;
struct SqlCollate {
  Tcl_Interp *interp;   /* The TCL interpret to execute the function */
  char *zScript;        /* The script to be run */
  SqlCollate *pNext;       /* Next function on the list of them all */
};

/*
** There is one instance of this structure for each SQLite database
** that has been opened by the SQLite TCL interface.
*/
typedef struct SqliteDb SqliteDb;
struct SqliteDb {
  sqlite3 *db;          /* The "real" database structure */
  Tcl_Interp *interp;   /* The interpreter used for this database */
  char *zBusy;          /* The busy callback routine */
  char *zCommit;        /* The commit hook callback routine */
  char *zTrace;         /* The trace callback routine */
  char *zProgress;      /* The progress callback routine */
  char *zAuth;          /* The authorization callback routine */
  SqlFunc *pFunc;       /* List of SQL functions */
  SqlCollate *pCollate; /* List of SQL collation functions */
  int rc;               /* Return code of most recent sqlite3_exec() */
  Tcl_Obj *pCollateNeeded;  /* Collation needed script */
};

/*
** TCL calls this procedure when an sqlite3 database command is
** deleted.
*/
static void DbDeleteCmd(void *db){
  SqliteDb *pDb = (SqliteDb*)db;
  sqlite3_close(pDb->db);
  while( pDb->pFunc ){
    SqlFunc *pFunc = pDb->pFunc;
    pDb->pFunc = pFunc->pNext;
    Tcl_Free((char*)pFunc);
  }
  while( pDb->pCollate ){
    SqlCollate *pCollate = pDb->pCollate;
    pDb->pCollate = pCollate->pNext;
    Tcl_Free((char*)pCollate);
  }
  if( pDb->zBusy ){
    Tcl_Free(pDb->zBusy);
  }
  if( pDb->zTrace ){
    Tcl_Free(pDb->zTrace);
  }
  if( pDb->zAuth ){
    Tcl_Free(pDb->zAuth);
  }
  Tcl_Free((char*)pDb);
}

/*
** This routine is called when a database file is locked while trying
** to execute SQL.
*/
static int DbBusyHandler(void *cd, int nTries){
  SqliteDb *pDb = (SqliteDb*)cd;
  int rc;
  char zVal[30];
  char *zCmd;
  Tcl_DString cmd;

  Tcl_DStringInit(&cmd);
  Tcl_DStringAppend(&cmd, pDb->zBusy, -1);
  sprintf(zVal, "%d", nTries);
  Tcl_DStringAppendElement(&cmd, zVal);
  zCmd = Tcl_DStringValue(&cmd);
  rc = Tcl_Eval(pDb->interp, zCmd);
  Tcl_DStringFree(&cmd);
  if( rc!=TCL_OK || atoi(Tcl_GetStringResult(pDb->interp)) ){
    return 0;
  }
  return 1;
}

/*
** This routine is invoked as the 'progress callback' for the database.
*/
static int DbProgressHandler(void *cd){
  SqliteDb *pDb = (SqliteDb*)cd;
  int rc;

  assert( pDb->zProgress );
  rc = Tcl_Eval(pDb->interp, pDb->zProgress);
  if( rc!=TCL_OK || atoi(Tcl_GetStringResult(pDb->interp)) ){
    return 1;
  }
  return 0;
}

/*
** This routine is called by the SQLite trace handler whenever a new
** block of SQL is executed.  The TCL script in pDb->zTrace is executed.
*/
static void DbTraceHandler(void *cd, const char *zSql){
  SqliteDb *pDb = (SqliteDb*)cd;
  Tcl_DString str;

  Tcl_DStringInit(&str);
  Tcl_DStringAppend(&str, pDb->zTrace, -1);
  Tcl_DStringAppendElement(&str, zSql);
  Tcl_Eval(pDb->interp, Tcl_DStringValue(&str));
  Tcl_DStringFree(&str);
  Tcl_ResetResult(pDb->interp);
}

/*
** This routine is called when a transaction is committed.  The
** TCL script in pDb->zCommit is executed.  If it returns non-zero or
** if it throws an exception, the transaction is rolled back instead
** of being committed.
*/
static int DbCommitHandler(void *cd){
  SqliteDb *pDb = (SqliteDb*)cd;
  int rc;

  rc = Tcl_Eval(pDb->interp, pDb->zCommit);
  if( rc!=TCL_OK || atoi(Tcl_GetStringResult(pDb->interp)) ){
    return 1;
  }
  return 0;
}

static void tclCollateNeeded(
  void *pCtx,
  sqlite3 *db,
  int enc,
  const char *zName
){
  SqliteDb *pDb = (SqliteDb *)pCtx;
  Tcl_Obj *pScript = Tcl_DuplicateObj(pDb->pCollateNeeded);
  Tcl_IncrRefCount(pScript);
  Tcl_ListObjAppendElement(0, pScript, Tcl_NewStringObj(zName, -1));
  Tcl_EvalObjEx(pDb->interp, pScript, 0);
  Tcl_DecrRefCount(pScript);
}

/*
** This routine is called to evaluate an SQL collation function implemented
** using TCL script.
*/
static int tclSqlCollate(
  void *pCtx,
  int nA,
  const void *zA,
  int nB,
  const void *zB
){
  SqlCollate *p = (SqlCollate *)pCtx;
  Tcl_Obj *pCmd;

  pCmd = Tcl_NewStringObj(p->zScript, -1);
  Tcl_IncrRefCount(pCmd);
  Tcl_ListObjAppendElement(p->interp, pCmd, Tcl_NewStringObj(zA, nA));
  Tcl_ListObjAppendElement(p->interp, pCmd, Tcl_NewStringObj(zB, nB));
  Tcl_EvalObjEx(p->interp, pCmd, 0);
  Tcl_DecrRefCount(pCmd);
  return (atoi(Tcl_GetStringResult(p->interp)));
}

/*
** This routine is called to evaluate an SQL function implemented
** using TCL script.
*/
static void tclSqlFunc(sqlite3_context *context, int argc, sqlite3_value **argv){
  SqlFunc *p = sqlite3_user_data(context);
  Tcl_DString cmd;
  int i;
  int rc;

  Tcl_DStringInit(&cmd);
  Tcl_DStringAppend(&cmd, p->zScript, -1);
  for(i=0; i<argc; i++){
    if( SQLITE_NULL==sqlite3_value_type(argv[i]) ){
      Tcl_DStringAppendElement(&cmd, "");
    }else{
      Tcl_DStringAppendElement(&cmd, sqlite3_value_text(argv[i]));
    }
  }
  rc = Tcl_Eval(p->interp, Tcl_DStringValue(&cmd));
  if( rc ){
    sqlite3_result_error(context, Tcl_GetStringResult(p->interp), -1); 
  }else{
    sqlite3_result_text(context, Tcl_GetStringResult(p->interp), -1, 
        SQLITE_TRANSIENT);
  }
}

#ifndef SQLITE_OMIT_AUTHORIZATION
/*
** This is the authentication function.  It appends the authentication
** type code and the two arguments to zCmd[] then invokes the result
** on the interpreter.  The reply is examined to determine if the
** authentication fails or succeeds.
*/
static int auth_callback(
  void *pArg,
  int code,
  const char *zArg1,
  const char *zArg2,
  const char *zArg3,
  const char *zArg4
){
  char *zCode;
  Tcl_DString str;
  int rc;
  const char *zReply;
  SqliteDb *pDb = (SqliteDb*)pArg;

  switch( code ){
    case SQLITE_COPY              : zCode="SQLITE_COPY"; break;
    case SQLITE_CREATE_INDEX      : zCode="SQLITE_CREATE_INDEX"; break;
    case SQLITE_CREATE_TABLE      : zCode="SQLITE_CREATE_TABLE"; break;
    case SQLITE_CREATE_TEMP_INDEX : zCode="SQLITE_CREATE_TEMP_INDEX"; break;
    case SQLITE_CREATE_TEMP_TABLE : zCode="SQLITE_CREATE_TEMP_TABLE"; break;
    case SQLITE_CREATE_TEMP_TRIGGER: zCode="SQLITE_CREATE_TEMP_TRIGGER"; break;
    case SQLITE_CREATE_TEMP_VIEW  : zCode="SQLITE_CREATE_TEMP_VIEW"; break;
    case SQLITE_CREATE_TRIGGER    : zCode="SQLITE_CREATE_TRIGGER"; break;
    case SQLITE_CREATE_VIEW       : zCode="SQLITE_CREATE_VIEW"; break;
    case SQLITE_DELETE            : zCode="SQLITE_DELETE"; break;
    case SQLITE_DROP_INDEX        : zCode="SQLITE_DROP_INDEX"; break;
    case SQLITE_DROP_TABLE        : zCode="SQLITE_DROP_TABLE"; break;
    case SQLITE_DROP_TEMP_INDEX   : zCode="SQLITE_DROP_TEMP_INDEX"; break;
    case SQLITE_DROP_TEMP_TABLE   : zCode="SQLITE_DROP_TEMP_TABLE"; break;
    case SQLITE_DROP_TEMP_TRIGGER : zCode="SQLITE_DROP_TEMP_TRIGGER"; break;
    case SQLITE_DROP_TEMP_VIEW    : zCode="SQLITE_DROP_TEMP_VIEW"; break;
    case SQLITE_DROP_TRIGGER      : zCode="SQLITE_DROP_TRIGGER"; break;
    case SQLITE_DROP_VIEW         : zCode="SQLITE_DROP_VIEW"; break;
    case SQLITE_INSERT            : zCode="SQLITE_INSERT"; break;
    case SQLITE_PRAGMA            : zCode="SQLITE_PRAGMA"; break;
    case SQLITE_READ              : zCode="SQLITE_READ"; break;
    case SQLITE_SELECT            : zCode="SQLITE_SELECT"; break;
    case SQLITE_TRANSACTION       : zCode="SQLITE_TRANSACTION"; break;
    case SQLITE_UPDATE            : zCode="SQLITE_UPDATE"; break;
    case SQLITE_ATTACH            : zCode="SQLITE_ATTACH"; break;
    case SQLITE_DETACH            : zCode="SQLITE_DETACH"; break;
    default                       : zCode="????"; break;
  }
  Tcl_DStringInit(&str);
  Tcl_DStringAppend(&str, pDb->zAuth, -1);
  Tcl_DStringAppendElement(&str, zCode);
  Tcl_DStringAppendElement(&str, zArg1 ? zArg1 : "");
  Tcl_DStringAppendElement(&str, zArg2 ? zArg2 : "");
  Tcl_DStringAppendElement(&str, zArg3 ? zArg3 : "");
  Tcl_DStringAppendElement(&str, zArg4 ? zArg4 : "");
  rc = Tcl_GlobalEval(pDb->interp, Tcl_DStringValue(&str));
  Tcl_DStringFree(&str);
  zReply = Tcl_GetStringResult(pDb->interp);
  if( strcmp(zReply,"SQLITE_OK")==0 ){
    rc = SQLITE_OK;
  }else if( strcmp(zReply,"SQLITE_DENY")==0 ){
    rc = SQLITE_DENY;
  }else if( strcmp(zReply,"SQLITE_IGNORE")==0 ){
    rc = SQLITE_IGNORE;
  }else{
    rc = 999;
  }
  return rc;
}
#endif /* SQLITE_OMIT_AUTHORIZATION */

/*
** zText is a pointer to text obtained via an sqlite3_result_text()
** or similar interface. This routine returns a Tcl string object, 
** reference count set to 0, containing the text. If a translation
** between iso8859 and UTF-8 is required, it is preformed.
*/
static Tcl_Obj *dbTextToObj(char const *zText){
  Tcl_Obj *pVal;
#ifdef UTF_TRANSLATION_NEEDED
  Tcl_DString dCol;
  Tcl_DStringInit(&dCol);
  Tcl_ExternalToUtfDString(NULL, zText, -1, &dCol);
  pVal = Tcl_NewStringObj(Tcl_DStringValue(&dCol), -1);
  Tcl_DStringFree(&dCol);
#else
  pVal = Tcl_NewStringObj(zText, -1);
#endif
  return pVal;
}

/*
** The "sqlite" command below creates a new Tcl command for each
** connection it opens to an SQLite database.  This routine is invoked
** whenever one of those connection-specific commands is executed
** in Tcl.  For example, if you run Tcl code like this:
**
**       sqlite3 db1  "my_database"
**       db1 close
**
** The first command opens a connection to the "my_database" database
** and calls that connection "db1".  The second command causes this
** subroutine to be invoked.
*/
static int DbObjCmd(void *cd, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv){
  SqliteDb *pDb = (SqliteDb*)cd;
  int choice;
  int rc = TCL_OK;
  static const char *DB_strs[] = {
    "authorizer",         "busy",              "changes",
    "close",              "collate",           "collation_needed",
    "commit_hook",        "complete",          "errorcode",
    "eval",               "function",          "last_insert_rowid",
    "onecolumn",          "progress",          "rekey",
    "timeout",            "total_changes",     "trace",
    0                    
  };
  enum DB_enum {
    DB_AUTHORIZER,        DB_BUSY,             DB_CHANGES,
    DB_CLOSE,             DB_COLLATE,          DB_COLLATION_NEEDED,
    DB_COMMIT_HOOK,       DB_COMPLETE,         DB_ERRORCODE,
    DB_EVAL,              DB_FUNCTION,         DB_LAST_INSERT_ROWID,
    DB_ONECOLUMN,         DB_PROGRESS,         DB_REKEY,
    DB_TIMEOUT,           DB_TOTAL_CHANGES,    DB_TRACE,
  };

  if( objc<2 ){
    Tcl_WrongNumArgs(interp, 1, objv, "SUBCOMMAND ...");
    return TCL_ERROR;
  }
  if( Tcl_GetIndexFromObj(interp, objv[1], DB_strs, "option", 0, &choice) ){
    return TCL_ERROR;
  }

  switch( (enum DB_enum)choice ){

  /*    $db authorizer ?CALLBACK?
  **
  ** Invoke the given callback to authorize each SQL operation as it is
  ** compiled.  5 arguments are appended to the callback before it is
  ** invoked:
  **
  **   (1) The authorization type (ex: SQLITE_CREATE_TABLE, SQLITE_INSERT, ...)
  **   (2) First descriptive name (depends on authorization type)
  **   (3) Second descriptive name
  **   (4) Name of the database (ex: "main", "temp")
  **   (5) Name of trigger that is doing the access
  **
  ** The callback should return on of the following strings: SQLITE_OK,
  ** SQLITE_IGNORE, or SQLITE_DENY.  Any other return value is an error.
  **
  ** If this method is invoked with no arguments, the current authorization
  ** callback string is returned.
  */
  case DB_AUTHORIZER: {
#ifdef SQLITE_OMIT_AUTHORIZATION
    Tcl_AppendResult(interp, "authorization not available in this build", 0);
    return TCL_ERROR;
#else
    if( objc>3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "?CALLBACK?");
      return TCL_ERROR;
    }else if( objc==2 ){
      if( pDb->zAuth ){
        Tcl_AppendResult(interp, pDb->zAuth, 0);
      }
    }else{
      char *zAuth;
      int len;
      if( pDb->zAuth ){
        Tcl_Free(pDb->zAuth);
      }
      zAuth = Tcl_GetStringFromObj(objv[2], &len);
      if( zAuth && len>0 ){
        pDb->zAuth = Tcl_Alloc( len + 1 );
        strcpy(pDb->zAuth, zAuth);
      }else{
        pDb->zAuth = 0;
      }
      if( pDb->zAuth ){
        pDb->interp = interp;
        sqlite3_set_authorizer(pDb->db, auth_callback, pDb);
      }else{
        sqlite3_set_authorizer(pDb->db, 0, 0);
      }
    }
#endif
    break;
  }

  /*    $db busy ?CALLBACK?
  **
  ** Invoke the given callback if an SQL statement attempts to open
  ** a locked database file.
  */
  case DB_BUSY: {
    if( objc>3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "CALLBACK");
      return TCL_ERROR;
    }else if( objc==2 ){
      if( pDb->zBusy ){
        Tcl_AppendResult(interp, pDb->zBusy, 0);
      }
    }else{
      char *zBusy;
      int len;
      if( pDb->zBusy ){
        Tcl_Free(pDb->zBusy);
      }
      zBusy = Tcl_GetStringFromObj(objv[2], &len);
      if( zBusy && len>0 ){
        pDb->zBusy = Tcl_Alloc( len + 1 );
        strcpy(pDb->zBusy, zBusy);
      }else{
        pDb->zBusy = 0;
      }
      if( pDb->zBusy ){
        pDb->interp = interp;
        sqlite3_busy_handler(pDb->db, DbBusyHandler, pDb);
      }else{
        sqlite3_busy_handler(pDb->db, 0, 0);
      }
    }
    break;
  }

  /*     $db changes
  **
  ** Return the number of rows that were modified, inserted, or deleted by
  ** the most recent INSERT, UPDATE or DELETE statement, not including 
  ** any changes made by trigger programs.
  */
  case DB_CHANGES: {
    Tcl_Obj *pResult;
    if( objc!=2 ){
      Tcl_WrongNumArgs(interp, 2, objv, "");
      return TCL_ERROR;
    }
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetIntObj(pResult, sqlite3_changes(pDb->db));
    break;
  }

  /*    $db close
  **
  ** Shutdown the database
  */
  case DB_CLOSE: {
    Tcl_DeleteCommand(interp, Tcl_GetStringFromObj(objv[0], 0));
    break;
  }

  /*    $db commit_hook ?CALLBACK?
  **
  ** Invoke the given callback just before committing every SQL transaction.
  ** If the callback throws an exception or returns non-zero, then the
  ** transaction is aborted.  If CALLBACK is an empty string, the callback
  ** is disabled.
  */
  case DB_COMMIT_HOOK: {
    if( objc>3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "?CALLBACK?");
      return TCL_ERROR;
    }else if( objc==2 ){
      if( pDb->zCommit ){
        Tcl_AppendResult(interp, pDb->zCommit, 0);
      }
    }else{
      char *zCommit;
      int len;
      if( pDb->zCommit ){
        Tcl_Free(pDb->zCommit);
      }
      zCommit = Tcl_GetStringFromObj(objv[2], &len);
      if( zCommit && len>0 ){
        pDb->zCommit = Tcl_Alloc( len + 1 );
        strcpy(pDb->zCommit, zCommit);
      }else{
        pDb->zCommit = 0;
      }
      if( pDb->zCommit ){
        pDb->interp = interp;
        sqlite3_commit_hook(pDb->db, DbCommitHandler, pDb);
      }else{
        sqlite3_commit_hook(pDb->db, 0, 0);
      }
    }
    break;
  }

  /*
  **     $db collate NAME SCRIPT
  **
  ** Create a new SQL collation function called NAME.  Whenever
  ** that function is called, invoke SCRIPT to evaluate the function.
  */
  case DB_COLLATE: {
    SqlCollate *pCollate;
    char *zName;
    char *zScript;
    int nScript;
    if( objc!=4 ){
      Tcl_WrongNumArgs(interp, 2, objv, "NAME SCRIPT");
      return TCL_ERROR;
    }
    zName = Tcl_GetStringFromObj(objv[2], 0);
    zScript = Tcl_GetStringFromObj(objv[3], &nScript);
    pCollate = (SqlCollate*)Tcl_Alloc( sizeof(*pCollate) + nScript + 1 );
    if( pCollate==0 ) return TCL_ERROR;
    pCollate->interp = interp;
    pCollate->pNext = pDb->pCollate;
    pCollate->zScript = (char*)&pCollate[1];
    pDb->pCollate = pCollate;
    strcpy(pCollate->zScript, zScript);
    if( sqlite3_create_collation(pDb->db, zName, SQLITE_UTF8, 
        pCollate, tclSqlCollate) ){
      return TCL_ERROR;
    }
    break;
  }

  /*
  **     $db collation_needed SCRIPT
  **
  ** Create a new SQL collation function called NAME.  Whenever
  ** that function is called, invoke SCRIPT to evaluate the function.
  */
  case DB_COLLATION_NEEDED: {
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "SCRIPT");
      return TCL_ERROR;
    }
    if( pDb->pCollateNeeded ){
      Tcl_DecrRefCount(pDb->pCollateNeeded);
    }
    pDb->pCollateNeeded = Tcl_DuplicateObj(objv[2]);
    Tcl_IncrRefCount(pDb->pCollateNeeded);
    sqlite3_collation_needed(pDb->db, pDb, tclCollateNeeded);
    break;
  }

  /*    $db complete SQL
  **
  ** Return TRUE if SQL is a complete SQL statement.  Return FALSE if
  ** additional lines of input are needed.  This is similar to the
  ** built-in "info complete" command of Tcl.
  */
  case DB_COMPLETE: {
    Tcl_Obj *pResult;
    int isComplete;
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "SQL");
      return TCL_ERROR;
    }
    isComplete = sqlite3_complete( Tcl_GetStringFromObj(objv[2], 0) );
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetBooleanObj(pResult, isComplete);
    break;
  }

  /*
  **    $db errorcode
  **
  ** Return the numeric error code that was returned by the most recent
  ** call to sqlite3_exec().
  */
  case DB_ERRORCODE: {
    Tcl_SetObjResult(interp, Tcl_NewIntObj(sqlite3_errcode(pDb->db)));
    break;
  }
   
  /*
  **    $db eval $sql ?array? ?{  ...code... }?
  **    $db onecolumn $sql
  **
  ** The SQL statement in $sql is evaluated.  For each row, the values are
  ** placed in elements of the array named "array" and ...code... is executed.
  ** If "array" and "code" are omitted, then no callback is every invoked.
  ** If "array" is an empty string, then the values are placed in variables
  ** that have the same name as the fields extracted by the query.
  **
  ** The onecolumn method is the equivalent of:
  **     lindex [$db eval $sql] 0
  */
  case DB_ONECOLUMN:
  case DB_EVAL: {
    char const *zSql;      /* Next SQL statement to execute */
    char const *zLeft;     /* What is left after first stmt in zSql */
    sqlite3_stmt *pStmt;   /* Compiled SQL statment */
    Tcl_Obj *pArray;       /* Name of array into which results are written */
    Tcl_Obj *pScript;      /* Script to run for each result set */
    Tcl_Obj **apParm;      /* Parameters that need a Tcl_DecrRefCount() */
    int nParm;             /* Number of entries used in apParm[] */
    Tcl_Obj *aParm[10];    /* Static space for apParm[] in the common case */
    Tcl_Obj *pRet;         /* Value to be returned */

    if( choice==DB_ONECOLUMN ){
      if( objc!=3 ){
        Tcl_WrongNumArgs(interp, 2, objv, "SQL");
        return TCL_ERROR;
      }
      pRet = 0;
    }else{
      if( objc<3 || objc>5 ){
        Tcl_WrongNumArgs(interp, 2, objv, "SQL ?ARRAY-NAME? ?SCRIPT?");
        return TCL_ERROR;
      }
      pRet = Tcl_NewObj();
      Tcl_IncrRefCount(pRet);
    }
    if( objc==3 ){
      pArray = pScript = 0;
    }else if( objc==4 ){
      pArray = 0;
      pScript = objv[3];
    }else{
      pArray = objv[3];
      if( Tcl_GetString(pArray)[0]==0 ) pArray = 0;
      pScript = objv[4];
    }

    Tcl_IncrRefCount(objv[2]);
    zSql = Tcl_GetStringFromObj(objv[2], 0);
    while( rc==TCL_OK && zSql[0] ){
      int i;      /* Loop counter */
      int nVar;   /* Number of wildcards in the SQL */
      int nCol;   /* Number of columns in the result set */
      Tcl_Obj **apColName = 0;   /* Array of column names */
  
      /* Compile a single SQL statement */
      if( SQLITE_OK!=sqlite3_prepare(pDb->db, zSql, -1, &pStmt, &zLeft) ){
        Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
        rc = TCL_ERROR;
        break;
      }
      if( pStmt==0 ){
        if( SQLITE_OK!=sqlite3_errcode(pDb->db) ){
          Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
          rc = TCL_ERROR;
          break;
        }else{
          zSql = zLeft;
          continue;
        }
      }

      /* Bind values to wildcards that begin with $ or : */  
      nVar = sqlite3_bind_parameter_count(pStmt);
      nParm = 0;
      if( nVar>sizeof(aParm)/sizeof(aParm[0]) ){
        apParm = (Tcl_Obj**)Tcl_Alloc(nVar*sizeof(apParm[0]));
      }else{
        apParm = aParm;
      }
      for(i=1; i<=nVar; i++){
        const char *zVar = sqlite3_bind_parameter_name(pStmt, i);
        if( zVar[0]=='$' || zVar[0]==':' ){
          Tcl_Obj *pVar = Tcl_GetVar2Ex(interp, &zVar[1], 0, 0);
          if( pVar ){
            int n;
            u8 *data;
            char *zType = pVar->typePtr ? pVar->typePtr->name : "";
            char c = zType[0];
            if( c=='b' && strcmp(zType,"bytearray")==0 ){
              data = Tcl_GetByteArrayFromObj(pVar, &n);
              sqlite3_bind_blob(pStmt, i, data, n, SQLITE_STATIC);
              Tcl_IncrRefCount(pVar);
              apParm[nParm++] = pVar;
            }else if( (c=='b' && strcmp(zType,"boolean")==0) ||
                  (c=='i' && strcmp(zType,"int")==0) ){
              Tcl_GetIntFromObj(interp, pVar, &n);
              sqlite3_bind_int(pStmt, i, n);
            }else if( c=='d' && strcmp(zType,"double")==0 ){
              double r;
              Tcl_GetDoubleFromObj(interp, pVar, &r);
              sqlite3_bind_double(pStmt, i, r);
            }else{
              data = Tcl_GetStringFromObj(pVar, &n);
              sqlite3_bind_text(pStmt, i, data, n, SQLITE_STATIC);
              Tcl_IncrRefCount(pVar);
              apParm[nParm++] = pVar;
            }
          }
        }
      }

      /* Compute column names */
      nCol = sqlite3_column_count(pStmt);
      if( pScript ){
        apColName = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*)*nCol );
        if( apColName==0 ) break;
        for(i=0; i<nCol; i++){
          apColName[i] = dbTextToObj(sqlite3_column_name(pStmt,i));
          Tcl_IncrRefCount(apColName[i]);
        }
      }

      /* If results are being stored in an array variable, then create
      ** the array(*) entry for that array
      */
      if( pArray ){
        Tcl_Obj *pColList = Tcl_NewObj();
        Tcl_IncrRefCount(pColList);
        for(i=0; i<nCol; i++){
          Tcl_ListObjAppendElement(interp, pColList, apColName[i]);
        }
        Tcl_ObjSetVar2(interp, pArray, Tcl_NewStringObj("*",-1), pColList,0);
      }

      /* Execute the SQL
      */
      while( rc==TCL_OK && pStmt && SQLITE_ROW==sqlite3_step(pStmt) ){
        for(i=0; i<nCol; i++){
          Tcl_Obj *pVal;
          
          /* Set pVal to contain the i'th column of this row. */
          switch( sqlite3_column_type(pStmt, i) ){
            case SQLITE_BLOB: {
              int bytes = sqlite3_column_bytes(pStmt, i);
              pVal = Tcl_NewByteArrayObj(sqlite3_column_blob(pStmt, i), bytes);
              break;
            }
            case SQLITE_INTEGER: {
              sqlite_int64 v = sqlite3_column_int64(pStmt, i);
              if( v>=-2147483647 && v<=2147483647 ){
                pVal = Tcl_NewIntObj(v);
              }else{
                pVal = Tcl_NewWideIntObj(v);
              }
              break;
            }
            case SQLITE_FLOAT: {
              double r = sqlite3_column_double(pStmt, i);
              pVal = Tcl_NewDoubleObj(r);
              break;
            }
            default: {
              pVal = dbTextToObj(sqlite3_column_text(pStmt, i));
              break;
            }
          }
  
          if( pScript ){
            if( pArray==0 ){
              Tcl_ObjSetVar2(interp, apColName[i], 0, pVal, 0);
            }else{
              Tcl_ObjSetVar2(interp, pArray, apColName[i], pVal, 0);
            }
          }else if( choice==DB_ONECOLUMN ){
            if( pRet==0 ){
              pRet = pVal;
              Tcl_IncrRefCount(pRet);
            }
            rc = TCL_BREAK;
          }else{
            Tcl_ListObjAppendElement(interp, pRet, pVal);
          }
        }
  
        if( pScript ){
          rc = Tcl_EvalObjEx(interp, pScript, 0);
          if( rc==TCL_CONTINUE ){
            rc = TCL_OK;
          }
        }
      }
      if( rc==TCL_BREAK ){
        rc = TCL_OK;
      }

      /* Free the column name objects */
      if( pScript ){
        for(i=0; i<nCol; i++){
          Tcl_DecrRefCount(apColName[i]);
        }
        Tcl_Free((char*)apColName);
      }

      /* Free the bound string and blob parameters */
      for(i=0; i<nParm; i++){
        Tcl_DecrRefCount(apParm[i]);
      }
      if( apParm!=aParm ){
        Tcl_Free((char*)apParm);
      }

      /* Finalize the statement.  If the result code is SQLITE_SCHEMA, then
      ** try again to execute the same statement
      */
      if( SQLITE_SCHEMA==sqlite3_finalize(pStmt) ){
        continue;
      }
  
      if( SQLITE_OK!=sqlite3_errcode(pDb->db) ){
        Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
        rc = TCL_ERROR;
        break;
      }

      zSql = zLeft;
    }
    Tcl_DecrRefCount(objv[2]);

    if( pRet ){
      if( rc==TCL_OK ){
        Tcl_SetObjResult(interp, pRet);
      }
      Tcl_DecrRefCount(pRet);
    }
    break;
  }

  /*
  **     $db function NAME SCRIPT
  **
  ** Create a new SQL function called NAME.  Whenever that function is
  ** called, invoke SCRIPT to evaluate the function.
  */
  case DB_FUNCTION: {
    SqlFunc *pFunc;
    char *zName;
    char *zScript;
    int nScript;
    if( objc!=4 ){
      Tcl_WrongNumArgs(interp, 2, objv, "NAME SCRIPT");
      return TCL_ERROR;
    }
    zName = Tcl_GetStringFromObj(objv[2], 0);
    zScript = Tcl_GetStringFromObj(objv[3], &nScript);
    pFunc = (SqlFunc*)Tcl_Alloc( sizeof(*pFunc) + nScript + 1 );
    if( pFunc==0 ) return TCL_ERROR;
    pFunc->interp = interp;
    pFunc->pNext = pDb->pFunc;
    pFunc->zScript = (char*)&pFunc[1];
    pDb->pFunc = pFunc;
    strcpy(pFunc->zScript, zScript);
    rc = sqlite3_create_function(pDb->db, zName, -1, SQLITE_UTF8,
        pFunc, tclSqlFunc, 0, 0);
    if( rc!=SQLITE_OK ) rc = TCL_ERROR;
    break;
  }

  /*
  **     $db last_insert_rowid 
  **
  ** Return an integer which is the ROWID for the most recent insert.
  */
  case DB_LAST_INSERT_ROWID: {
    Tcl_Obj *pResult;
    int rowid;
    if( objc!=2 ){
      Tcl_WrongNumArgs(interp, 2, objv, "");
      return TCL_ERROR;
    }
    rowid = sqlite3_last_insert_rowid(pDb->db);
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetIntObj(pResult, rowid);
    break;
  }

  /*
  ** The DB_ONECOLUMN method is implemented together with DB_EVAL.
  */

  /*    $db progress ?N CALLBACK?
  ** 
  ** Invoke the given callback every N virtual machine opcodes while executing
  ** queries.
  */
  case DB_PROGRESS: {
    if( objc==2 ){
      if( pDb->zProgress ){
        Tcl_AppendResult(interp, pDb->zProgress, 0);
      }
    }else if( objc==4 ){
      char *zProgress;
      int len;
      int N;
      if( TCL_OK!=Tcl_GetIntFromObj(interp, objv[2], &N) ){
	return TCL_ERROR;
      };
      if( pDb->zProgress ){
        Tcl_Free(pDb->zProgress);
      }
      zProgress = Tcl_GetStringFromObj(objv[3], &len);
      if( zProgress && len>0 ){
        pDb->zProgress = Tcl_Alloc( len + 1 );
        strcpy(pDb->zProgress, zProgress);
      }else{
        pDb->zProgress = 0;
      }
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
      if( pDb->zProgress ){
        pDb->interp = interp;
        sqlite3_progress_handler(pDb->db, N, DbProgressHandler, pDb);
      }else{
        sqlite3_progress_handler(pDb->db, 0, 0, 0);
      }
#endif
    }else{
      Tcl_WrongNumArgs(interp, 2, objv, "N CALLBACK");
      return TCL_ERROR;
    }
    break;
  }

  /*
  **     $db rekey KEY
  **
  ** Change the encryption key on the currently open database.
  */
  case DB_REKEY: {
    int nKey;
    void *pKey;
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "KEY");
      return TCL_ERROR;
    }
    pKey = Tcl_GetByteArrayFromObj(objv[2], &nKey);
#ifdef SQLITE_HAS_CODEC
    rc = sqlite3_rekey(pDb->db, pKey, nKey);
    if( rc ){
      Tcl_AppendResult(interp, sqlite3ErrStr(rc), 0);
      rc = TCL_ERROR;
    }
#endif
    break;
  }

  /*
  **     $db timeout MILLESECONDS
  **
  ** Delay for the number of milliseconds specified when a file is locked.
  */
  case DB_TIMEOUT: {
    int ms;
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "MILLISECONDS");
      return TCL_ERROR;
    }
    if( Tcl_GetIntFromObj(interp, objv[2], &ms) ) return TCL_ERROR;
    sqlite3_busy_timeout(pDb->db, ms);
    break;
  }

  /*
  **     $db total_changes
  **
  ** Return the number of rows that were modified, inserted, or deleted 
  ** since the database handle was created.
  */
  case DB_TOTAL_CHANGES: {
    Tcl_Obj *pResult;
    if( objc!=2 ){
      Tcl_WrongNumArgs(interp, 2, objv, "");
      return TCL_ERROR;
    }
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetIntObj(pResult, sqlite3_total_changes(pDb->db));
    break;
  }

  /*    $db trace ?CALLBACK?
  **
  ** Make arrangements to invoke the CALLBACK routine for each SQL statement
  ** that is executed.  The text of the SQL is appended to CALLBACK before
  ** it is executed.
  */
  case DB_TRACE: {
    if( objc>3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "?CALLBACK?");
      return TCL_ERROR;
    }else if( objc==2 ){
      if( pDb->zTrace ){
        Tcl_AppendResult(interp, pDb->zTrace, 0);
      }
    }else{
      char *zTrace;
      int len;
      if( pDb->zTrace ){
        Tcl_Free(pDb->zTrace);
      }
      zTrace = Tcl_GetStringFromObj(objv[2], &len);
      if( zTrace && len>0 ){
        pDb->zTrace = Tcl_Alloc( len + 1 );
        strcpy(pDb->zTrace, zTrace);
      }else{
        pDb->zTrace = 0;
      }
      if( pDb->zTrace ){
        pDb->interp = interp;
        sqlite3_trace(pDb->db, DbTraceHandler, pDb);
      }else{
        sqlite3_trace(pDb->db, 0, 0);
      }
    }
    break;
  }

  } /* End of the SWITCH statement */
  return rc;
}

/*
**   sqlite3 DBNAME FILENAME ?MODE? ?-key KEY?
**
** This is the main Tcl command.  When the "sqlite" Tcl command is
** invoked, this routine runs to process that command.
**
** The first argument, DBNAME, is an arbitrary name for a new
** database connection.  This command creates a new command named
** DBNAME that is used to control that connection.  The database
** connection is deleted when the DBNAME command is deleted.
**
** The second argument is the name of the directory that contains
** the sqlite database that is to be accessed.
**
** For testing purposes, we also support the following:
**
**  sqlite3 -encoding
**
**       Return the encoding used by LIKE and GLOB operators.  Choices
**       are UTF-8 and iso8859.
**
**  sqlite3 -version
**
**       Return the version number of the SQLite library.
**
**  sqlite3 -tcl-uses-utf
**
**       Return "1" if compiled with a Tcl uses UTF-8.  Return "0" if
**       not.  Used by tests to make sure the library was compiled 
**       correctly.
*/
static int DbMain(void *cd, Tcl_Interp *interp, int objc,Tcl_Obj *const*objv){
  SqliteDb *p;
  void *pKey = 0;
  int nKey = 0;
  const char *zArg;
  char *zErrMsg;
  const char *zFile;
  char zBuf[80];
  if( objc==2 ){
    zArg = Tcl_GetStringFromObj(objv[1], 0);
    if( strcmp(zArg,"-version")==0 ){
      Tcl_AppendResult(interp,sqlite3_version,0);
      return TCL_OK;
    }
    if( strcmp(zArg,"-has-codec")==0 ){
#ifdef SQLITE_HAS_CODEC
      Tcl_AppendResult(interp,"1",0);
#else
      Tcl_AppendResult(interp,"0",0);
#endif
      return TCL_OK;
    }
    if( strcmp(zArg,"-tcl-uses-utf")==0 ){
#ifdef TCL_UTF_MAX
      Tcl_AppendResult(interp,"1",0);
#else
      Tcl_AppendResult(interp,"0",0);
#endif
      return TCL_OK;
    }
  }
  if( objc==5 || objc==6 ){
    zArg = Tcl_GetStringFromObj(objv[objc-2], 0);
    if( strcmp(zArg,"-key")==0 ){
      pKey = Tcl_GetByteArrayFromObj(objv[objc-1], &nKey);
      objc -= 2;
    }
  }
  if( objc!=3 && objc!=4 ){
    Tcl_WrongNumArgs(interp, 1, objv, 
#ifdef SQLITE_HAS_CODEC
      "HANDLE FILENAME ?-key CODEC-KEY?"
#else
      "HANDLE FILENAME ?MODE?"
#endif
    );
    return TCL_ERROR;
  }
  zErrMsg = 0;
  p = (SqliteDb*)Tcl_Alloc( sizeof(*p) );
  if( p==0 ){
    Tcl_SetResult(interp, "malloc failed", TCL_STATIC);
    return TCL_ERROR;
  }
  memset(p, 0, sizeof(*p));
  zFile = Tcl_GetStringFromObj(objv[2], 0);
  sqlite3_open(zFile, &p->db);
  if( SQLITE_OK!=sqlite3_errcode(p->db) ){
    zErrMsg = strdup(sqlite3_errmsg(p->db));
    sqlite3_close(p->db);
    p->db = 0;
  }
#ifdef SQLITE_HAS_CODEC
  sqlite3_key(p->db, pKey, nKey);
#endif
  if( p->db==0 ){
    Tcl_SetResult(interp, zErrMsg, TCL_VOLATILE);
    Tcl_Free((char*)p);
    free(zErrMsg);
    return TCL_ERROR;
  }
  zArg = Tcl_GetStringFromObj(objv[1], 0);
  Tcl_CreateObjCommand(interp, zArg, DbObjCmd, (char*)p, DbDeleteCmd);

  /* The return value is the value of the sqlite* pointer
  */
  sprintf(zBuf, "%p", p->db);
  if( strncmp(zBuf,"0x",2) ){
    sprintf(zBuf, "0x%p", p->db);
  }
  Tcl_AppendResult(interp, zBuf, 0);

  /* If compiled with SQLITE_TEST turned on, then register the "md5sum"
  ** SQL function.
  */
#ifdef SQLITE_TEST
  {
    extern void Md5_Register(sqlite3*);
#ifdef SQLITE_DEBUG
    int mallocfail = sqlite3_iMallocFail;
    sqlite3_iMallocFail = 0;
#endif
    Md5_Register(p->db);
#ifdef SQLITE_DEBUG
    sqlite3_iMallocFail = mallocfail;
#endif
   }
#endif  
  p->interp = interp;
  return TCL_OK;
}

/*
** Provide a dummy Tcl_InitStubs if we are using this as a static
** library.
*/
#ifndef USE_TCL_STUBS
# undef  Tcl_InitStubs
# define Tcl_InitStubs(a,b,c)
#endif

/*
** Initialize this module.
**
** This Tcl module contains only a single new Tcl command named "sqlite".
** (Hence there is no namespace.  There is no point in using a namespace
** if the extension only supplies one new name!)  The "sqlite" command is
** used to open a new SQLite database.  See the DbMain() routine above
** for additional information.
*/
int Sqlite3_Init(Tcl_Interp *interp){
  Tcl_InitStubs(interp, "8.4", 0);
  Tcl_CreateObjCommand(interp, "sqlite3", (Tcl_ObjCmdProc*)DbMain, 0, 0);
  Tcl_PkgProvide(interp, "sqlite3", "3.0");
  return TCL_OK;
}
int Tclsqlite3_Init(Tcl_Interp *interp){
  Tcl_InitStubs(interp, "8.4", 0);
  Tcl_CreateObjCommand(interp, "sqlite3", (Tcl_ObjCmdProc*)DbMain, 0, 0);
  Tcl_PkgProvide(interp, "sqlite3", "3.0");
  return TCL_OK;
}
int Sqlite3_SafeInit(Tcl_Interp *interp){
  return TCL_OK;
}
int Tclsqlite3_SafeInit(Tcl_Interp *interp){
  return TCL_OK;
}

#ifdef TCLSH
/*****************************************************************************
** The code that follows is used to build standalone TCL interpreters
*/

/*
** If the macro TCLSH is one, then put in code this for the
** "main" routine that will initialize Tcl and take input from
** standard input.
*/
#if TCLSH==1
static char zMainloop[] =
  "set line {}\n"
  "while {![eof stdin]} {\n"
    "if {$line!=\"\"} {\n"
      "puts -nonewline \"> \"\n"
    "} else {\n"
      "puts -nonewline \"% \"\n"
    "}\n"
    "flush stdout\n"
    "append line [gets stdin]\n"
    "if {[info complete $line]} {\n"
      "if {[catch {uplevel #0 $line} result]} {\n"
        "puts stderr \"Error: $result\"\n"
      "} elseif {$result!=\"\"} {\n"
        "puts $result\n"
      "}\n"
      "set line {}\n"
    "} else {\n"
      "append line \\n\n"
    "}\n"
  "}\n"
;
#endif

/*
** If the macro TCLSH is two, then get the main loop code out of
** the separate file "spaceanal_tcl.h".
*/
#if TCLSH==2
static char zMainloop[] = 
#include "spaceanal_tcl.h"
;
#endif

#define TCLSH_MAIN main   /* Needed to fake out mktclapp */
int TCLSH_MAIN(int argc, char **argv){
  Tcl_Interp *interp;
  Tcl_FindExecutable(argv[0]);
  interp = Tcl_CreateInterp();
  Sqlite3_Init(interp);
#ifdef SQLITE_TEST
  {
    extern int Sqlitetest1_Init(Tcl_Interp*);
    extern int Sqlitetest2_Init(Tcl_Interp*);
    extern int Sqlitetest3_Init(Tcl_Interp*);
    extern int Sqlitetest4_Init(Tcl_Interp*);
    extern int Sqlitetest5_Init(Tcl_Interp*);
    extern int Md5_Init(Tcl_Interp*);
    Sqlitetest1_Init(interp);
    Sqlitetest2_Init(interp);
    Sqlitetest3_Init(interp);
    Sqlitetest4_Init(interp);
    Sqlitetest5_Init(interp);
    Md5_Init(interp);
  }
#endif
  if( argc>=2 || TCLSH==2 ){
    int i;
    Tcl_SetVar(interp,"argv0",argv[1],TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp,"argv", "", TCL_GLOBAL_ONLY);
    for(i=2; i<argc; i++){
      Tcl_SetVar(interp, "argv", argv[i],
          TCL_GLOBAL_ONLY | TCL_LIST_ELEMENT | TCL_APPEND_VALUE);
    }
    if( TCLSH==1 && Tcl_EvalFile(interp, argv[1])!=TCL_OK ){
      const char *zInfo = Tcl_GetVar(interp, "errorInfo", TCL_GLOBAL_ONLY);
      if( zInfo==0 ) zInfo = interp->result;
      fprintf(stderr,"%s: %s\n", *argv, zInfo);
      return 1;
    }
  }
  if( argc<=1 || TCLSH==2 ){
    Tcl_GlobalEval(interp, zMainloop);
  }
  return 0;
}
#endif /* TCLSH */

#endif /* !defined(NO_TCL) */
