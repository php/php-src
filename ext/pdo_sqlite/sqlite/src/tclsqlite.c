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
** A TCL Interface to SQLite.  Append this file to sqlite3.c and
** compile the whole thing to build a TCL-enabled version of SQLite.
**
** $Id$
*/
#include "tcl.h"

/*
** Some additional include files are needed if this file is not
** appended to the amalgamation.
*/
#ifndef SQLITE_AMALGAMATION
# include "sqliteInt.h"
# include "hash.h"
# include <stdlib.h>
# include <string.h>
# include <assert.h>
# include <ctype.h>
#endif

/*
 * Windows needs to know which symbols to export.  Unix does not.
 * BUILD_sqlite should be undefined for Unix.
 */
#ifdef BUILD_sqlite
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif /* BUILD_sqlite */

#define NUM_PREPARED_STMTS 10
#define MAX_PREPARED_STMTS 100

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
  Tcl_Obj *pScript;     /* The Tcl_Obj representation of the script */
  int useEvalObjv;      /* True if it is safe to use Tcl_EvalObjv */
  char *zName;          /* Name of this function */
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
  SqlCollate *pNext;    /* Next function on the list of them all */
};

/*
** Prepared statements are cached for faster execution.  Each prepared
** statement is described by an instance of the following structure.
*/
typedef struct SqlPreparedStmt SqlPreparedStmt;
struct SqlPreparedStmt {
  SqlPreparedStmt *pNext;  /* Next in linked list */
  SqlPreparedStmt *pPrev;  /* Previous on the list */
  sqlite3_stmt *pStmt;     /* The prepared statement */
  int nSql;                /* chars in zSql[] */
  char zSql[1];            /* Text of the SQL statement */
};

/*
** There is one instance of this structure for each SQLite database
** that has been opened by the SQLite TCL interface.
*/
typedef struct SqliteDb SqliteDb;
struct SqliteDb {
  sqlite3 *db;               /* The "real" database structure. MUST BE FIRST */
  Tcl_Interp *interp;        /* The interpreter used for this database */
  char *zBusy;               /* The busy callback routine */
  char *zCommit;             /* The commit hook callback routine */
  char *zTrace;              /* The trace callback routine */
  char *zProfile;            /* The profile callback routine */
  char *zProgress;           /* The progress callback routine */
  char *zAuth;               /* The authorization callback routine */
  char *zNull;               /* Text to substitute for an SQL NULL value */
  SqlFunc *pFunc;            /* List of SQL functions */
  Tcl_Obj *pUpdateHook;      /* Update hook script (if any) */
  Tcl_Obj *pRollbackHook;    /* Rollback hook script (if any) */
  SqlCollate *pCollate;      /* List of SQL collation functions */
  int rc;                    /* Return code of most recent sqlite3_exec() */
  Tcl_Obj *pCollateNeeded;   /* Collation needed script */
  SqlPreparedStmt *stmtList; /* List of prepared statements*/
  SqlPreparedStmt *stmtLast; /* Last statement in the list */
  int maxStmt;               /* The next maximum number of stmtList */
  int nStmt;                 /* Number of statements in stmtList */
};

/*
** Look at the script prefix in pCmd.  We will be executing this script
** after first appending one or more arguments.  This routine analyzes
** the script to see if it is safe to use Tcl_EvalObjv() on the script
** rather than the more general Tcl_EvalEx().  Tcl_EvalObjv() is much
** faster.
**
** Scripts that are safe to use with Tcl_EvalObjv() consists of a
** command name followed by zero or more arguments with no [...] or $
** or {...} or ; to be seen anywhere.  Most callback scripts consist
** of just a single procedure name and they meet this requirement.
*/
static int safeToUseEvalObjv(Tcl_Interp *interp, Tcl_Obj *pCmd){
  /* We could try to do something with Tcl_Parse().  But we will instead
  ** just do a search for forbidden characters.  If any of the forbidden
  ** characters appear in pCmd, we will report the string as unsafe.
  */
  const char *z;
  int n;
  z = Tcl_GetStringFromObj(pCmd, &n);
  while( n-- > 0 ){
    int c = *(z++);
    if( c=='$' || c=='[' || c==';' ) return 0;
  }
  return 1;
}

/*
** Find an SqlFunc structure with the given name.  Or create a new
** one if an existing one cannot be found.  Return a pointer to the
** structure.
*/
static SqlFunc *findSqlFunc(SqliteDb *pDb, const char *zName){
  SqlFunc *p, *pNew;
  int i;
  pNew = (SqlFunc*)Tcl_Alloc( sizeof(*pNew) + strlen(zName) + 1 );
  pNew->zName = (char*)&pNew[1];
  for(i=0; zName[i]; i++){ pNew->zName[i] = tolower(zName[i]); }
  pNew->zName[i] = 0;
  for(p=pDb->pFunc; p; p=p->pNext){ 
    if( strcmp(p->zName, pNew->zName)==0 ){
      Tcl_Free((char*)pNew);
      return p;
    }
  }
  pNew->interp = pDb->interp;
  pNew->pScript = 0;
  pNew->pNext = pDb->pFunc;
  pDb->pFunc = pNew;
  return pNew;
}

/*
** Finalize and free a list of prepared statements
*/
static void flushStmtCache( SqliteDb *pDb ){
  SqlPreparedStmt *pPreStmt;

  while(  pDb->stmtList ){
    sqlite3_finalize( pDb->stmtList->pStmt );
    pPreStmt = pDb->stmtList;
    pDb->stmtList = pDb->stmtList->pNext;
    Tcl_Free( (char*)pPreStmt );
  }
  pDb->nStmt = 0;
  pDb->stmtLast = 0;
}

/*
** TCL calls this procedure when an sqlite3 database command is
** deleted.
*/
static void DbDeleteCmd(void *db){
  SqliteDb *pDb = (SqliteDb*)db;
  flushStmtCache(pDb);
  sqlite3_close(pDb->db);
  while( pDb->pFunc ){
    SqlFunc *pFunc = pDb->pFunc;
    pDb->pFunc = pFunc->pNext;
    Tcl_DecrRefCount(pFunc->pScript);
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
  if( pDb->zProfile ){
    Tcl_Free(pDb->zProfile);
  }
  if( pDb->zAuth ){
    Tcl_Free(pDb->zAuth);
  }
  if( pDb->zNull ){
    Tcl_Free(pDb->zNull);
  }
  if( pDb->pUpdateHook ){
    Tcl_DecrRefCount(pDb->pUpdateHook);
  }
  if( pDb->pRollbackHook ){
    Tcl_DecrRefCount(pDb->pRollbackHook);
  }
  if( pDb->pCollateNeeded ){
    Tcl_DecrRefCount(pDb->pCollateNeeded);
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

  sprintf(zVal, "%d", nTries);
  rc = Tcl_VarEval(pDb->interp, pDb->zBusy, " ", zVal, (char*)0);
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

#ifndef SQLITE_OMIT_TRACE
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
#endif

#ifndef SQLITE_OMIT_TRACE
/*
** This routine is called by the SQLite profile handler after a statement
** SQL has executed.  The TCL script in pDb->zProfile is evaluated.
*/
static void DbProfileHandler(void *cd, const char *zSql, sqlite_uint64 tm){
  SqliteDb *pDb = (SqliteDb*)cd;
  Tcl_DString str;
  char zTm[100];

  sqlite3_snprintf(sizeof(zTm)-1, zTm, "%lld", tm);
  Tcl_DStringInit(&str);
  Tcl_DStringAppend(&str, pDb->zProfile, -1);
  Tcl_DStringAppendElement(&str, zSql);
  Tcl_DStringAppendElement(&str, zTm);
  Tcl_Eval(pDb->interp, Tcl_DStringValue(&str));
  Tcl_DStringFree(&str);
  Tcl_ResetResult(pDb->interp);
}
#endif

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

static void DbRollbackHandler(void *clientData){
  SqliteDb *pDb = (SqliteDb*)clientData;
  assert(pDb->pRollbackHook);
  if( TCL_OK!=Tcl_EvalObjEx(pDb->interp, pDb->pRollbackHook, 0) ){
    Tcl_BackgroundError(pDb->interp);
  }
}

static void DbUpdateHandler(
  void *p, 
  int op,
  const char *zDb, 
  const char *zTbl, 
  sqlite_int64 rowid
){
  SqliteDb *pDb = (SqliteDb *)p;
  Tcl_Obj *pCmd;

  assert( pDb->pUpdateHook );
  assert( op==SQLITE_INSERT || op==SQLITE_UPDATE || op==SQLITE_DELETE );

  pCmd = Tcl_DuplicateObj(pDb->pUpdateHook);
  Tcl_IncrRefCount(pCmd);
  Tcl_ListObjAppendElement(0, pCmd, Tcl_NewStringObj(
    ( (op==SQLITE_INSERT)?"INSERT":(op==SQLITE_UPDATE)?"UPDATE":"DELETE"), -1));
  Tcl_ListObjAppendElement(0, pCmd, Tcl_NewStringObj(zDb, -1));
  Tcl_ListObjAppendElement(0, pCmd, Tcl_NewStringObj(zTbl, -1));
  Tcl_ListObjAppendElement(0, pCmd, Tcl_NewWideIntObj(rowid));
  Tcl_EvalObjEx(pDb->interp, pCmd, TCL_EVAL_DIRECT);
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
  Tcl_EvalObjEx(p->interp, pCmd, TCL_EVAL_DIRECT);
  Tcl_DecrRefCount(pCmd);
  return (atoi(Tcl_GetStringResult(p->interp)));
}

/*
** This routine is called to evaluate an SQL function implemented
** using TCL script.
*/
static void tclSqlFunc(sqlite3_context *context, int argc, sqlite3_value**argv){
  SqlFunc *p = sqlite3_user_data(context);
  Tcl_Obj *pCmd;
  int i;
  int rc;

  if( argc==0 ){
    /* If there are no arguments to the function, call Tcl_EvalObjEx on the
    ** script object directly.  This allows the TCL compiler to generate
    ** bytecode for the command on the first invocation and thus make
    ** subsequent invocations much faster. */
    pCmd = p->pScript;
    Tcl_IncrRefCount(pCmd);
    rc = Tcl_EvalObjEx(p->interp, pCmd, 0);
    Tcl_DecrRefCount(pCmd);
  }else{
    /* If there are arguments to the function, make a shallow copy of the
    ** script object, lappend the arguments, then evaluate the copy.
    **
    ** By "shallow" copy, we mean a only the outer list Tcl_Obj is duplicated.
    ** The new Tcl_Obj contains pointers to the original list elements. 
    ** That way, when Tcl_EvalObjv() is run and shimmers the first element
    ** of the list to tclCmdNameType, that alternate representation will
    ** be preserved and reused on the next invocation.
    */
    Tcl_Obj **aArg;
    int nArg;
    if( Tcl_ListObjGetElements(p->interp, p->pScript, &nArg, &aArg) ){
      sqlite3_result_error(context, Tcl_GetStringResult(p->interp), -1); 
      return;
    }     
    pCmd = Tcl_NewListObj(nArg, aArg);
    Tcl_IncrRefCount(pCmd);
    for(i=0; i<argc; i++){
      sqlite3_value *pIn = argv[i];
      Tcl_Obj *pVal;
            
      /* Set pVal to contain the i'th column of this row. */
      switch( sqlite3_value_type(pIn) ){
        case SQLITE_BLOB: {
          int bytes = sqlite3_value_bytes(pIn);
          pVal = Tcl_NewByteArrayObj(sqlite3_value_blob(pIn), bytes);
          break;
        }
        case SQLITE_INTEGER: {
          sqlite_int64 v = sqlite3_value_int64(pIn);
          if( v>=-2147483647 && v<=2147483647 ){
            pVal = Tcl_NewIntObj(v);
          }else{
            pVal = Tcl_NewWideIntObj(v);
          }
          break;
        }
        case SQLITE_FLOAT: {
          double r = sqlite3_value_double(pIn);
          pVal = Tcl_NewDoubleObj(r);
          break;
        }
        case SQLITE_NULL: {
          pVal = Tcl_NewStringObj("", 0);
          break;
        }
        default: {
          int bytes = sqlite3_value_bytes(pIn);
          pVal = Tcl_NewStringObj((char *)sqlite3_value_text(pIn), bytes);
          break;
        }
      }
      rc = Tcl_ListObjAppendElement(p->interp, pCmd, pVal);
      if( rc ){
        Tcl_DecrRefCount(pCmd);
        sqlite3_result_error(context, Tcl_GetStringResult(p->interp), -1); 
        return;
      }
    }
    if( !p->useEvalObjv ){
      /* Tcl_EvalObjEx() will automatically call Tcl_EvalObjv() if pCmd
      ** is a list without a string representation.  To prevent this from
      ** happening, make sure pCmd has a valid string representation */
      Tcl_GetString(pCmd);
    }
    rc = Tcl_EvalObjEx(p->interp, pCmd, TCL_EVAL_DIRECT);
    Tcl_DecrRefCount(pCmd);
  }

  if( rc && rc!=TCL_RETURN ){
    sqlite3_result_error(context, Tcl_GetStringResult(p->interp), -1); 
  }else{
    Tcl_Obj *pVar = Tcl_GetObjResult(p->interp);
    int n;
    u8 *data;
    char *zType = pVar->typePtr ? pVar->typePtr->name : "";
    char c = zType[0];
    if( c=='b' && strcmp(zType,"bytearray")==0 && pVar->bytes==0 ){
      /* Only return a BLOB type if the Tcl variable is a bytearray and
      ** has no string representation. */
      data = Tcl_GetByteArrayFromObj(pVar, &n);
      sqlite3_result_blob(context, data, n, SQLITE_TRANSIENT);
    }else if( (c=='b' && strcmp(zType,"boolean")==0) ||
          (c=='i' && strcmp(zType,"int")==0) ){
      Tcl_GetIntFromObj(0, pVar, &n);
      sqlite3_result_int(context, n);
    }else if( c=='d' && strcmp(zType,"double")==0 ){
      double r;
      Tcl_GetDoubleFromObj(0, pVar, &r);
      sqlite3_result_double(context, r);
    }else if( c=='w' && strcmp(zType,"wideInt")==0 ){
      Tcl_WideInt v;
      Tcl_GetWideIntFromObj(0, pVar, &v);
      sqlite3_result_int64(context, v);
    }else{
      data = (unsigned char *)Tcl_GetStringFromObj(pVar, &n);
      sqlite3_result_text(context, (char *)data, n, SQLITE_TRANSIENT);
    }
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
    case SQLITE_ALTER_TABLE       : zCode="SQLITE_ALTER_TABLE"; break;
    case SQLITE_REINDEX           : zCode="SQLITE_REINDEX"; break;
    case SQLITE_ANALYZE           : zCode="SQLITE_ANALYZE"; break;
    case SQLITE_CREATE_VTABLE     : zCode="SQLITE_CREATE_VTABLE"; break;
    case SQLITE_DROP_VTABLE       : zCode="SQLITE_DROP_VTABLE"; break;
    case SQLITE_FUNCTION          : zCode="SQLITE_FUNCTION"; break;
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
** This routine reads a line of text from FILE in, stores
** the text in memory obtained from malloc() and returns a pointer
** to the text.  NULL is returned at end of file, or if malloc()
** fails.
**
** The interface is like "readline" but no command-line editing
** is done.
**
** copied from shell.c from '.import' command
*/
static char *local_getline(char *zPrompt, FILE *in){
  char *zLine;
  int nLine;
  int n;
  int eol;

  nLine = 100;
  zLine = malloc( nLine );
  if( zLine==0 ) return 0;
  n = 0;
  eol = 0;
  while( !eol ){
    if( n+100>nLine ){
      nLine = nLine*2 + 100;
      zLine = realloc(zLine, nLine);
      if( zLine==0 ) return 0;
    }
    if( fgets(&zLine[n], nLine - n, in)==0 ){
      if( n==0 ){
        free(zLine);
        return 0;
      }
      zLine[n] = 0;
      eol = 1;
      break;
    }
    while( zLine[n] ){ n++; }
    if( n>0 && zLine[n-1]=='\n' ){
      n--;
      zLine[n] = 0;
      eol = 1;
    }
  }
  zLine = realloc( zLine, n+1 );
  return zLine;
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
    "authorizer",         "busy",              "cache",
    "changes",            "close",             "collate",
    "collation_needed",   "commit_hook",       "complete",
    "copy",               "enable_load_extension","errorcode",
    "eval",               "exists",            "function",
    "interrupt",          "last_insert_rowid", "nullvalue",
    "onecolumn",          "profile",           "progress",
    "rekey",              "rollback_hook",     "timeout",
    "total_changes",      "trace",             "transaction",
    "update_hook",        "version",           0
  };
  enum DB_enum {
    DB_AUTHORIZER,        DB_BUSY,             DB_CACHE,
    DB_CHANGES,           DB_CLOSE,            DB_COLLATE,
    DB_COLLATION_NEEDED,  DB_COMMIT_HOOK,      DB_COMPLETE,
    DB_COPY,              DB_ENABLE_LOAD_EXTENSION,DB_ERRORCODE,
    DB_EVAL,              DB_EXISTS,           DB_FUNCTION,
    DB_INTERRUPT,         DB_LAST_INSERT_ROWID,DB_NULLVALUE,
    DB_ONECOLUMN,         DB_PROFILE,          DB_PROGRESS,
    DB_REKEY,             DB_ROLLBACK_HOOK,    DB_TIMEOUT,
    DB_TOTAL_CHANGES,     DB_TRACE,            DB_TRANSACTION,
    DB_UPDATE_HOOK,       DB_VERSION,          
  };
  /* don't leave trailing commas on DB_enum, it confuses the AIX xlc compiler */

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

  /*     $db cache flush
  **     $db cache size n
  **
  ** Flush the prepared statement cache, or set the maximum number of
  ** cached statements.
  */
  case DB_CACHE: {
    char *subCmd;
    int n;

    if( objc<=2 ){
      Tcl_WrongNumArgs(interp, 1, objv, "cache option ?arg?");
      return TCL_ERROR;
    }
    subCmd = Tcl_GetStringFromObj( objv[2], 0 );
    if( *subCmd=='f' && strcmp(subCmd,"flush")==0 ){
      if( objc!=3 ){
        Tcl_WrongNumArgs(interp, 2, objv, "flush");
        return TCL_ERROR;
      }else{
        flushStmtCache( pDb );
      }
    }else if( *subCmd=='s' && strcmp(subCmd,"size")==0 ){
      if( objc!=4 ){
        Tcl_WrongNumArgs(interp, 2, objv, "size n");
        return TCL_ERROR;
      }else{
        if( TCL_ERROR==Tcl_GetIntFromObj(interp, objv[3], &n) ){
          Tcl_AppendResult( interp, "cannot convert \"", 
               Tcl_GetStringFromObj(objv[3],0), "\" to integer", 0);
          return TCL_ERROR;
        }else{
          if( n<0 ){
            flushStmtCache( pDb );
            n = 0;
          }else if( n>MAX_PREPARED_STMTS ){
            n = MAX_PREPARED_STMTS;
          }
          pDb->maxStmt = n;
        }
      }
    }else{
      Tcl_AppendResult( interp, "bad option \"", 
          Tcl_GetStringFromObj(objv[0],0), "\": must be flush or size", 0);
      return TCL_ERROR;
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
      Tcl_SetResult(interp, (char *)sqlite3_errmsg(pDb->db), TCL_VOLATILE);
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

  /*    $db complete SQL
  **
  ** Return TRUE if SQL is a complete SQL statement.  Return FALSE if
  ** additional lines of input are needed.  This is similar to the
  ** built-in "info complete" command of Tcl.
  */
  case DB_COMPLETE: {
#ifndef SQLITE_OMIT_COMPLETE
    Tcl_Obj *pResult;
    int isComplete;
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "SQL");
      return TCL_ERROR;
    }
    isComplete = sqlite3_complete( Tcl_GetStringFromObj(objv[2], 0) );
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetBooleanObj(pResult, isComplete);
#endif
    break;
  }

  /*    $db copy conflict-algorithm table filename ?SEPARATOR? ?NULLINDICATOR?
  **
  ** Copy data into table from filename, optionally using SEPARATOR
  ** as column separators.  If a column contains a null string, or the
  ** value of NULLINDICATOR, a NULL is inserted for the column.
  ** conflict-algorithm is one of the sqlite conflict algorithms:
  **    rollback, abort, fail, ignore, replace
  ** On success, return the number of lines processed, not necessarily same
  ** as 'db changes' due to conflict-algorithm selected.
  **
  ** This code is basically an implementation/enhancement of
  ** the sqlite3 shell.c ".import" command.
  **
  ** This command usage is equivalent to the sqlite2.x COPY statement,
  ** which imports file data into a table using the PostgreSQL COPY file format:
  **   $db copy $conflit_algo $table_name $filename \t \\N
  */
  case DB_COPY: {
    char *zTable;               /* Insert data into this table */
    char *zFile;                /* The file from which to extract data */
    char *zConflict;            /* The conflict algorithm to use */
    sqlite3_stmt *pStmt;        /* A statement */
    int rc;                     /* Result code */
    int nCol;                   /* Number of columns in the table */
    int nByte;                  /* Number of bytes in an SQL string */
    int i, j;                   /* Loop counters */
    int nSep;                   /* Number of bytes in zSep[] */
    int nNull;                  /* Number of bytes in zNull[] */
    char *zSql;                 /* An SQL statement */
    char *zLine;                /* A single line of input from the file */
    char **azCol;               /* zLine[] broken up into columns */
    char *zCommit;              /* How to commit changes */
    FILE *in;                   /* The input file */
    int lineno = 0;             /* Line number of input file */
    char zLineNum[80];          /* Line number print buffer */
    Tcl_Obj *pResult;           /* interp result */

    char *zSep;
    char *zNull;
    if( objc<5 || objc>7 ){
      Tcl_WrongNumArgs(interp, 2, objv, 
         "CONFLICT-ALGORITHM TABLE FILENAME ?SEPARATOR? ?NULLINDICATOR?");
      return TCL_ERROR;
    }
    if( objc>=6 ){
      zSep = Tcl_GetStringFromObj(objv[5], 0);
    }else{
      zSep = "\t";
    }
    if( objc>=7 ){
      zNull = Tcl_GetStringFromObj(objv[6], 0);
    }else{
      zNull = "";
    }
    zConflict = Tcl_GetStringFromObj(objv[2], 0);
    zTable = Tcl_GetStringFromObj(objv[3], 0);
    zFile = Tcl_GetStringFromObj(objv[4], 0);
    nSep = strlen(zSep);
    nNull = strlen(zNull);
    if( nSep==0 ){
      Tcl_AppendResult(interp,"Error: non-null separator required for copy",0);
      return TCL_ERROR;
    }
    if(sqlite3StrICmp(zConflict, "rollback") != 0 &&
       sqlite3StrICmp(zConflict, "abort"   ) != 0 &&
       sqlite3StrICmp(zConflict, "fail"    ) != 0 &&
       sqlite3StrICmp(zConflict, "ignore"  ) != 0 &&
       sqlite3StrICmp(zConflict, "replace" ) != 0 ) {
      Tcl_AppendResult(interp, "Error: \"", zConflict, 
            "\", conflict-algorithm must be one of: rollback, "
            "abort, fail, ignore, or replace", 0);
      return TCL_ERROR;
    }
    zSql = sqlite3_mprintf("SELECT * FROM '%q'", zTable);
    if( zSql==0 ){
      Tcl_AppendResult(interp, "Error: no such table: ", zTable, 0);
      return TCL_ERROR;
    }
    nByte = strlen(zSql);
    rc = sqlite3_prepare(pDb->db, zSql, -1, &pStmt, 0);
    sqlite3_free(zSql);
    if( rc ){
      Tcl_AppendResult(interp, "Error: ", sqlite3_errmsg(pDb->db), 0);
      nCol = 0;
    }else{
      nCol = sqlite3_column_count(pStmt);
    }
    sqlite3_finalize(pStmt);
    if( nCol==0 ) {
      return TCL_ERROR;
    }
    zSql = malloc( nByte + 50 + nCol*2 );
    if( zSql==0 ) {
      Tcl_AppendResult(interp, "Error: can't malloc()", 0);
      return TCL_ERROR;
    }
    sqlite3_snprintf(nByte+50, zSql, "INSERT OR %q INTO '%q' VALUES(?",
         zConflict, zTable);
    j = strlen(zSql);
    for(i=1; i<nCol; i++){
      zSql[j++] = ',';
      zSql[j++] = '?';
    }
    zSql[j++] = ')';
    zSql[j] = 0;
    rc = sqlite3_prepare(pDb->db, zSql, -1, &pStmt, 0);
    free(zSql);
    if( rc ){
      Tcl_AppendResult(interp, "Error: ", sqlite3_errmsg(pDb->db), 0);
      sqlite3_finalize(pStmt);
      return TCL_ERROR;
    }
    in = fopen(zFile, "rb");
    if( in==0 ){
      Tcl_AppendResult(interp, "Error: cannot open file: ", zFile, NULL);
      sqlite3_finalize(pStmt);
      return TCL_ERROR;
    }
    azCol = malloc( sizeof(azCol[0])*(nCol+1) );
    if( azCol==0 ) {
      Tcl_AppendResult(interp, "Error: can't malloc()", 0);
      fclose(in);
      return TCL_ERROR;
    }
    (void)sqlite3_exec(pDb->db, "BEGIN", 0, 0, 0);
    zCommit = "COMMIT";
    while( (zLine = local_getline(0, in))!=0 ){
      char *z;
      i = 0;
      lineno++;
      azCol[0] = zLine;
      for(i=0, z=zLine; *z; z++){
        if( *z==zSep[0] && strncmp(z, zSep, nSep)==0 ){
          *z = 0;
          i++;
          if( i<nCol ){
            azCol[i] = &z[nSep];
            z += nSep-1;
          }
        }
      }
      if( i+1!=nCol ){
        char *zErr;
        zErr = malloc(200 + strlen(zFile));
        if( zErr ){
          sprintf(zErr,
             "Error: %s line %d: expected %d columns of data but found %d",
             zFile, lineno, nCol, i+1);
          Tcl_AppendResult(interp, zErr, 0);
          free(zErr);
        }
        zCommit = "ROLLBACK";
        break;
      }
      for(i=0; i<nCol; i++){
        /* check for null data, if so, bind as null */
        if ((nNull>0 && strcmp(azCol[i], zNull)==0) || strlen(azCol[i])==0) {
          sqlite3_bind_null(pStmt, i+1);
        }else{
          sqlite3_bind_text(pStmt, i+1, azCol[i], -1, SQLITE_STATIC);
        }
      }
      sqlite3_step(pStmt);
      rc = sqlite3_reset(pStmt);
      free(zLine);
      if( rc!=SQLITE_OK ){
        Tcl_AppendResult(interp,"Error: ", sqlite3_errmsg(pDb->db), 0);
        zCommit = "ROLLBACK";
        break;
      }
    }
    free(azCol);
    fclose(in);
    sqlite3_finalize(pStmt);
    (void)sqlite3_exec(pDb->db, zCommit, 0, 0, 0);

    if( zCommit[0] == 'C' ){
      /* success, set result as number of lines processed */
      pResult = Tcl_GetObjResult(interp);
      Tcl_SetIntObj(pResult, lineno);
      rc = TCL_OK;
    }else{
      /* failure, append lineno where failed */
      sprintf(zLineNum,"%d",lineno);
      Tcl_AppendResult(interp,", failed while processing line: ",zLineNum,0);
      rc = TCL_ERROR;
    }
    break;
  }

  /*
  **    $db enable_load_extension BOOLEAN
  **
  ** Turn the extension loading feature on or off.  It if off by
  ** default.
  */
  case DB_ENABLE_LOAD_EXTENSION: {
#ifndef SQLITE_OMIT_LOAD_EXTENSION
    int onoff;
    if( objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "BOOLEAN");
      return TCL_ERROR;
    }
    if( Tcl_GetBooleanFromObj(interp, objv[2], &onoff) ){
      return TCL_ERROR;
    }
    sqlite3_enable_load_extension(pDb->db, onoff);
    break;
#else
    Tcl_AppendResult(interp, "extension loading is turned off at compile-time",
                     0);
    return TCL_ERROR;
#endif
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
  case DB_EVAL:
  case DB_EXISTS: {
    char const *zSql;      /* Next SQL statement to execute */
    char const *zLeft;     /* What is left after first stmt in zSql */
    sqlite3_stmt *pStmt;   /* Compiled SQL statment */
    Tcl_Obj *pArray;       /* Name of array into which results are written */
    Tcl_Obj *pScript;      /* Script to run for each result set */
    Tcl_Obj **apParm;      /* Parameters that need a Tcl_DecrRefCount() */
    int nParm;             /* Number of entries used in apParm[] */
    Tcl_Obj *aParm[10];    /* Static space for apParm[] in the common case */
    Tcl_Obj *pRet;         /* Value to be returned */
    SqlPreparedStmt *pPreStmt;  /* Pointer to a prepared statement */
    int rc2;

    if( choice==DB_EVAL ){
      if( objc<3 || objc>5 ){
        Tcl_WrongNumArgs(interp, 2, objv, "SQL ?ARRAY-NAME? ?SCRIPT?");
        return TCL_ERROR;
      }
      pRet = Tcl_NewObj();
      Tcl_IncrRefCount(pRet);
    }else{
      if( objc!=3 ){
        Tcl_WrongNumArgs(interp, 2, objv, "SQL");
        return TCL_ERROR;
      }
      if( choice==DB_EXISTS ){
        pRet = Tcl_NewBooleanObj(0);
        Tcl_IncrRefCount(pRet);
      }else{
        pRet = 0;
      }
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
      int i;                     /* Loop counter */
      int nVar;                  /* Number of bind parameters in the pStmt */
      int nCol;                  /* Number of columns in the result set */
      Tcl_Obj **apColName = 0;   /* Array of column names */
      int len;                   /* String length of zSql */
  
      /* Try to find a SQL statement that has already been compiled and
      ** which matches the next sequence of SQL.
      */
      pStmt = 0;
      pPreStmt = pDb->stmtList;
      len = strlen(zSql);
      if( pPreStmt && sqlite3_expired(pPreStmt->pStmt) ){
        flushStmtCache(pDb);
        pPreStmt = 0;
      }
      for(; pPreStmt; pPreStmt=pPreStmt->pNext){
        int n = pPreStmt->nSql;
        if( len>=n 
            && memcmp(pPreStmt->zSql, zSql, n)==0
            && (zSql[n]==0 || zSql[n-1]==';')
        ){
          pStmt = pPreStmt->pStmt;
          zLeft = &zSql[pPreStmt->nSql];

          /* When a prepared statement is found, unlink it from the
          ** cache list.  It will later be added back to the beginning
          ** of the cache list in order to implement LRU replacement.
          */
          if( pPreStmt->pPrev ){
            pPreStmt->pPrev->pNext = pPreStmt->pNext;
          }else{
            pDb->stmtList = pPreStmt->pNext;
          }
          if( pPreStmt->pNext ){
            pPreStmt->pNext->pPrev = pPreStmt->pPrev;
          }else{
            pDb->stmtLast = pPreStmt->pPrev;
          }
          pDb->nStmt--;
          break;
        }
      }
  
      /* If no prepared statement was found.  Compile the SQL text
      */
      if( pStmt==0 ){
        if( SQLITE_OK!=sqlite3_prepare(pDb->db, zSql, -1, &pStmt, &zLeft) ){
          Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
          rc = TCL_ERROR;
          break;
        }
        if( pStmt==0 ){
          if( SQLITE_OK!=sqlite3_errcode(pDb->db) ){
            /* A compile-time error in the statement
            */
            Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
            rc = TCL_ERROR;
            break;
          }else{
            /* The statement was a no-op.  Continue to the next statement
            ** in the SQL string.
            */
            zSql = zLeft;
            continue;
          }
        }
        assert( pPreStmt==0 );
      }

      /* Bind values to parameters that begin with $ or :
      */  
      nVar = sqlite3_bind_parameter_count(pStmt);
      nParm = 0;
      if( nVar>sizeof(aParm)/sizeof(aParm[0]) ){
        apParm = (Tcl_Obj**)Tcl_Alloc(nVar*sizeof(apParm[0]));
      }else{
        apParm = aParm;
      }
      for(i=1; i<=nVar; i++){
        const char *zVar = sqlite3_bind_parameter_name(pStmt, i);
        if( zVar!=0 && (zVar[0]=='$' || zVar[0]==':') ){
          Tcl_Obj *pVar = Tcl_GetVar2Ex(interp, &zVar[1], 0, 0);
          if( pVar ){
            int n;
            u8 *data;
            char *zType = pVar->typePtr ? pVar->typePtr->name : "";
            char c = zType[0];
            if( c=='b' && strcmp(zType,"bytearray")==0 && pVar->bytes==0 ){
              /* Only load a BLOB type if the Tcl variable is a bytearray and
              ** has no string representation. */
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
            }else if( c=='w' && strcmp(zType,"wideInt")==0 ){
              Tcl_WideInt v;
              Tcl_GetWideIntFromObj(interp, pVar, &v);
              sqlite3_bind_int64(pStmt, i, v);
            }else{
              data = (unsigned char *)Tcl_GetStringFromObj(pVar, &n);
              sqlite3_bind_text(pStmt, i, (char *)data, n, SQLITE_STATIC);
              Tcl_IncrRefCount(pVar);
              apParm[nParm++] = pVar;
            }
          }else{
            sqlite3_bind_null( pStmt, i );
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
        Tcl_Obj *pStar = Tcl_NewStringObj("*", -1);
        Tcl_IncrRefCount(pColList);
        for(i=0; i<nCol; i++){
          Tcl_ListObjAppendElement(interp, pColList, apColName[i]);
        }
        Tcl_ObjSetVar2(interp, pArray, pStar, pColList,0);
        Tcl_DecrRefCount(pColList);
        Tcl_DecrRefCount(pStar);
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
            case SQLITE_NULL: {
              pVal = dbTextToObj(pDb->zNull);
              break;
            }
            default: {
              pVal = dbTextToObj((char *)sqlite3_column_text(pStmt, i));
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
            assert( pRet==0 );
            if( pRet==0 ){
              pRet = pVal;
              Tcl_IncrRefCount(pRet);
            }
            rc = TCL_BREAK;
            i = nCol;
          }else if( choice==DB_EXISTS ){
            Tcl_DecrRefCount(pRet);
            pRet = Tcl_NewBooleanObj(1);
            Tcl_IncrRefCount(pRet);
            rc = TCL_BREAK;
            i = nCol;
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

      /* Reset the statement.  If the result code is SQLITE_SCHEMA, then
      ** flush the statement cache and try the statement again.
      */
      rc2 = sqlite3_reset(pStmt);
      if( SQLITE_SCHEMA==rc2 ){
        /* After a schema change, flush the cache and try to run the
        ** statement again
        */
        flushStmtCache( pDb );
        sqlite3_finalize(pStmt);
        if( pPreStmt ) Tcl_Free((char*)pPreStmt);
        continue;
      }else if( SQLITE_OK!=rc2 ){
        /* If a run-time error occurs, report the error and stop reading
        ** the SQL
        */
        Tcl_SetObjResult(interp, dbTextToObj(sqlite3_errmsg(pDb->db)));
        sqlite3_finalize(pStmt);
        rc = TCL_ERROR;
        if( pPreStmt ) Tcl_Free((char*)pPreStmt);
        break;
      }else if( pDb->maxStmt<=0 ){
        /* If the cache is turned off, deallocated the statement */
        if( pPreStmt ) Tcl_Free((char*)pPreStmt);
        sqlite3_finalize(pStmt);
      }else{
        /* Everything worked and the cache is operational.
        ** Create a new SqlPreparedStmt structure if we need one.
        ** (If we already have one we can just reuse it.)
        */
        if( pPreStmt==0 ){
          len = zLeft - zSql;
          pPreStmt = (SqlPreparedStmt*)Tcl_Alloc( sizeof(*pPreStmt) + len );
          if( pPreStmt==0 ) return TCL_ERROR;
          pPreStmt->pStmt = pStmt;
          pPreStmt->nSql = len;
          memcpy(pPreStmt->zSql, zSql, len);
          pPreStmt->zSql[len] = 0;
        }

        /* Add the prepared statement to the beginning of the cache list
        */
        pPreStmt->pNext = pDb->stmtList;
        pPreStmt->pPrev = 0;
        if( pDb->stmtList ){
         pDb->stmtList->pPrev = pPreStmt;
        }
        pDb->stmtList = pPreStmt;
        if( pDb->stmtLast==0 ){
          assert( pDb->nStmt==0 );
          pDb->stmtLast = pPreStmt;
        }else{
          assert( pDb->nStmt>0 );
        }
        pDb->nStmt++;
   
        /* If we have too many statement in cache, remove the surplus from the
        ** end of the cache list.
        */
        while( pDb->nStmt>pDb->maxStmt ){
          sqlite3_finalize(pDb->stmtLast->pStmt);
          pDb->stmtLast = pDb->stmtLast->pPrev;
          Tcl_Free((char*)pDb->stmtLast->pNext);
          pDb->stmtLast->pNext = 0;
          pDb->nStmt--;
        }
      }

      /* Proceed to the next statement */
      zSql = zLeft;
    }
    Tcl_DecrRefCount(objv[2]);

    if( pRet ){
      if( rc==TCL_OK ){
        Tcl_SetObjResult(interp, pRet);
      }
      Tcl_DecrRefCount(pRet);
    }else if( rc==TCL_OK ){
      Tcl_ResetResult(interp);
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
    Tcl_Obj *pScript;
    char *zName;
    if( objc!=4 ){
      Tcl_WrongNumArgs(interp, 2, objv, "NAME SCRIPT");
      return TCL_ERROR;
    }
    zName = Tcl_GetStringFromObj(objv[2], 0);
    pScript = objv[3];
    pFunc = findSqlFunc(pDb, zName);
    if( pFunc==0 ) return TCL_ERROR;
    if( pFunc->pScript ){
      Tcl_DecrRefCount(pFunc->pScript);
    }
    pFunc->pScript = pScript;
    Tcl_IncrRefCount(pScript);
    pFunc->useEvalObjv = safeToUseEvalObjv(interp, pScript);
    rc = sqlite3_create_function(pDb->db, zName, -1, SQLITE_UTF8,
        pFunc, tclSqlFunc, 0, 0);
    if( rc!=SQLITE_OK ){
      rc = TCL_ERROR;
      Tcl_SetResult(interp, (char *)sqlite3_errmsg(pDb->db), TCL_VOLATILE);
    }else{
      /* Must flush any cached statements */
      flushStmtCache( pDb );
    }
    break;
  }

  /*
  **     $db interrupt
  **
  ** Interrupt the execution of the inner-most SQL interpreter.  This
  ** causes the SQL statement to return an error of SQLITE_INTERRUPT.
  */
  case DB_INTERRUPT: {
    sqlite3_interrupt(pDb->db);
    break;
  }

  /*
  **     $db nullvalue ?STRING?
  **
  ** Change text used when a NULL comes back from the database. If ?STRING?
  ** is not present, then the current string used for NULL is returned.
  ** If STRING is present, then STRING is returned.
  **
  */
  case DB_NULLVALUE: {
    if( objc!=2 && objc!=3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "NULLVALUE");
      return TCL_ERROR;
    }
    if( objc==3 ){
      int len;
      char *zNull = Tcl_GetStringFromObj(objv[2], &len);
      if( pDb->zNull ){
        Tcl_Free(pDb->zNull);
      }
      if( zNull && len>0 ){
        pDb->zNull = Tcl_Alloc( len + 1 );
        strncpy(pDb->zNull, zNull, len);
        pDb->zNull[len] = '\0';
      }else{
        pDb->zNull = 0;
      }
    }
    Tcl_SetObjResult(interp, dbTextToObj(pDb->zNull));
    break;
  }

  /*
  **     $db last_insert_rowid 
  **
  ** Return an integer which is the ROWID for the most recent insert.
  */
  case DB_LAST_INSERT_ROWID: {
    Tcl_Obj *pResult;
    Tcl_WideInt rowid;
    if( objc!=2 ){
      Tcl_WrongNumArgs(interp, 2, objv, "");
      return TCL_ERROR;
    }
    rowid = sqlite3_last_insert_rowid(pDb->db);
    pResult = Tcl_GetObjResult(interp);
    Tcl_SetWideIntObj(pResult, rowid);
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

  /*    $db profile ?CALLBACK?
  **
  ** Make arrangements to invoke the CALLBACK routine after each SQL statement
  ** that has run.  The text of the SQL and the amount of elapse time are
  ** appended to CALLBACK before the script is run.
  */
  case DB_PROFILE: {
    if( objc>3 ){
      Tcl_WrongNumArgs(interp, 2, objv, "?CALLBACK?");
      return TCL_ERROR;
    }else if( objc==2 ){
      if( pDb->zProfile ){
        Tcl_AppendResult(interp, pDb->zProfile, 0);
      }
    }else{
      char *zProfile;
      int len;
      if( pDb->zProfile ){
        Tcl_Free(pDb->zProfile);
      }
      zProfile = Tcl_GetStringFromObj(objv[2], &len);
      if( zProfile && len>0 ){
        pDb->zProfile = Tcl_Alloc( len + 1 );
        strcpy(pDb->zProfile, zProfile);
      }else{
        pDb->zProfile = 0;
      }
#ifndef SQLITE_OMIT_TRACE
      if( pDb->zProfile ){
        pDb->interp = interp;
        sqlite3_profile(pDb->db, DbProfileHandler, pDb);
      }else{
        sqlite3_profile(pDb->db, 0, 0);
      }
#endif
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
#ifndef SQLITE_OMIT_TRACE
      if( pDb->zTrace ){
        pDb->interp = interp;
        sqlite3_trace(pDb->db, DbTraceHandler, pDb);
      }else{
        sqlite3_trace(pDb->db, 0, 0);
      }
#endif
    }
    break;
  }

  /*    $db transaction [-deferred|-immediate|-exclusive] SCRIPT
  **
  ** Start a new transaction (if we are not already in the midst of a
  ** transaction) and execute the TCL script SCRIPT.  After SCRIPT
  ** completes, either commit the transaction or roll it back if SCRIPT
  ** throws an exception.  Or if no new transation was started, do nothing.
  ** pass the exception on up the stack.
  **
  ** This command was inspired by Dave Thomas's talk on Ruby at the
  ** 2005 O'Reilly Open Source Convention (OSCON).
  */
  case DB_TRANSACTION: {
    int inTrans;
    Tcl_Obj *pScript;
    const char *zBegin = "BEGIN";
    if( objc!=3 && objc!=4 ){
      Tcl_WrongNumArgs(interp, 2, objv, "[TYPE] SCRIPT");
      return TCL_ERROR;
    }
    if( objc==3 ){
      pScript = objv[2];
    } else {
      static const char *TTYPE_strs[] = {
        "deferred",   "exclusive",  "immediate", 0
      };
      enum TTYPE_enum {
        TTYPE_DEFERRED, TTYPE_EXCLUSIVE, TTYPE_IMMEDIATE
      };
      int ttype;
      if( Tcl_GetIndexFromObj(interp, objv[2], TTYPE_strs, "transaction type",
                              0, &ttype) ){
        return TCL_ERROR;
      }
      switch( (enum TTYPE_enum)ttype ){
        case TTYPE_DEFERRED:    /* no-op */;                 break;
        case TTYPE_EXCLUSIVE:   zBegin = "BEGIN EXCLUSIVE";  break;
        case TTYPE_IMMEDIATE:   zBegin = "BEGIN IMMEDIATE";  break;
      }
      pScript = objv[3];
    }
    inTrans = !sqlite3_get_autocommit(pDb->db);
    if( !inTrans ){
      (void)sqlite3_exec(pDb->db, zBegin, 0, 0, 0);
    }
    rc = Tcl_EvalObjEx(interp, pScript, 0);
    if( !inTrans ){
      const char *zEnd;
      if( rc==TCL_ERROR ){
        zEnd = "ROLLBACK";
      } else {
        zEnd = "COMMIT";
      }
      (void)sqlite3_exec(pDb->db, zEnd, 0, 0, 0);
    }
    break;
  }

  /*
  **    $db update_hook ?script?
  **    $db rollback_hook ?script?
  */
  case DB_UPDATE_HOOK: 
  case DB_ROLLBACK_HOOK: {

    /* set ppHook to point at pUpdateHook or pRollbackHook, depending on 
    ** whether [$db update_hook] or [$db rollback_hook] was invoked.
    */
    Tcl_Obj **ppHook; 
    if( choice==DB_UPDATE_HOOK ){
      ppHook = &pDb->pUpdateHook;
    }else{
      ppHook = &pDb->pRollbackHook;
    }

    if( objc!=2 && objc!=3 ){
       Tcl_WrongNumArgs(interp, 2, objv, "?SCRIPT?");
       return TCL_ERROR;
    }
    if( *ppHook ){
      Tcl_SetObjResult(interp, *ppHook);
      if( objc==3 ){
        Tcl_DecrRefCount(*ppHook);
        *ppHook = 0;
      }
    }
    if( objc==3 ){
      assert( !(*ppHook) );
      if( Tcl_GetCharLength(objv[2])>0 ){
        *ppHook = objv[2];
        Tcl_IncrRefCount(*ppHook);
      }
    }

    sqlite3_update_hook(pDb->db, (pDb->pUpdateHook?DbUpdateHandler:0), pDb);
    sqlite3_rollback_hook(pDb->db,(pDb->pRollbackHook?DbRollbackHandler:0),pDb);

    break;
  }

  /*    $db version
  **
  ** Return the version string for this database.
  */
  case DB_VERSION: {
    Tcl_SetResult(interp, (char *)sqlite3_libversion(), TCL_STATIC);
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
  Tcl_DString translatedFilename;
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
  zFile = Tcl_TranslateFileName(interp, zFile, &translatedFilename);
  sqlite3_open(zFile, &p->db);
  Tcl_DStringFree(&translatedFilename);
  if( SQLITE_OK!=sqlite3_errcode(p->db) ){
    zErrMsg = sqlite3_mprintf("%s", sqlite3_errmsg(p->db));
    sqlite3_close(p->db);
    p->db = 0;
  }
#ifdef SQLITE_HAS_CODEC
  sqlite3_key(p->db, pKey, nKey);
#endif
  if( p->db==0 ){
    Tcl_SetResult(interp, zErrMsg, TCL_VOLATILE);
    Tcl_Free((char*)p);
    sqlite3_free(zErrMsg);
    return TCL_ERROR;
  }
  p->maxStmt = NUM_PREPARED_STMTS;
  p->interp = interp;
  zArg = Tcl_GetStringFromObj(objv[1], 0);
  Tcl_CreateObjCommand(interp, zArg, DbObjCmd, (char*)p, DbDeleteCmd);

  /* If compiled with SQLITE_TEST turned on, then register the "md5sum"
  ** SQL function.
  */
#ifdef SQLITE_TEST
  {
    extern void Md5_Register(sqlite3*);
#ifdef SQLITE_MEMDEBUG
    int mallocfail = sqlite3_iMallocFail;
    sqlite3_iMallocFail = 0;
#endif
    Md5_Register(p->db);
#ifdef SQLITE_MEMDEBUG
    sqlite3_iMallocFail = mallocfail;
#endif
  }
#endif  
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
** Make sure we have a PACKAGE_VERSION macro defined.  This will be
** defined automatically by the TEA makefile.  But other makefiles
** do not define it.
*/
#ifndef PACKAGE_VERSION
# define PACKAGE_VERSION SQLITE_VERSION
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
EXTERN int Sqlite3_Init(Tcl_Interp *interp){
  Tcl_InitStubs(interp, "8.4", 0);
  Tcl_CreateObjCommand(interp, "sqlite3", (Tcl_ObjCmdProc*)DbMain, 0, 0);
  Tcl_PkgProvide(interp, "sqlite3", PACKAGE_VERSION);
  Tcl_CreateObjCommand(interp, "sqlite", (Tcl_ObjCmdProc*)DbMain, 0, 0);
  Tcl_PkgProvide(interp, "sqlite", PACKAGE_VERSION);
  return TCL_OK;
}
EXTERN int Tclsqlite3_Init(Tcl_Interp *interp){ return Sqlite3_Init(interp); }
EXTERN int Sqlite3_SafeInit(Tcl_Interp *interp){ return TCL_OK; }
EXTERN int Tclsqlite3_SafeInit(Tcl_Interp *interp){ return TCL_OK; }

#ifndef SQLITE_3_SUFFIX_ONLY
EXTERN int Sqlite_Init(Tcl_Interp *interp){ return Sqlite3_Init(interp); }
EXTERN int Tclsqlite_Init(Tcl_Interp *interp){ return Sqlite3_Init(interp); }
EXTERN int Sqlite_SafeInit(Tcl_Interp *interp){ return TCL_OK; }
EXTERN int Tclsqlite_SafeInit(Tcl_Interp *interp){ return TCL_OK; }
#endif

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
    extern int Sqlitetest6_Init(Tcl_Interp*);
    extern int Sqlitetest7_Init(Tcl_Interp*);
    extern int Sqlitetest8_Init(Tcl_Interp*);
    extern int Sqlitetest9_Init(Tcl_Interp*);
    extern int Md5_Init(Tcl_Interp*);
    extern int Sqlitetestsse_Init(Tcl_Interp*);
    extern int Sqlitetestasync_Init(Tcl_Interp*);
    extern int Sqlitetesttclvar_Init(Tcl_Interp*);
    extern int Sqlitetestschema_Init(Tcl_Interp*);
    extern int Sqlitetest_autoext_Init(Tcl_Interp*);
    extern int Sqlitetest_hexio_Init(Tcl_Interp*);

    Sqlitetest1_Init(interp);
    Sqlitetest2_Init(interp);
    Sqlitetest3_Init(interp);
    Sqlitetest4_Init(interp);
    Sqlitetest5_Init(interp);
    Sqlitetest6_Init(interp);
    Sqlitetest7_Init(interp);
    Sqlitetest8_Init(interp);
    Sqlitetest9_Init(interp);
    Sqlitetestasync_Init(interp);
    Sqlitetesttclvar_Init(interp);
    Sqlitetestschema_Init(interp);
    Sqlitetest_autoext_Init(interp);
    Sqlitetest_hexio_Init(interp);
    Md5_Init(interp);
#ifdef SQLITE_SSE
    Sqlitetestsse_Init(interp);
#endif
  }
#endif
  if( argc>=2 || TCLSH==2 ){
    int i;
    char zArgc[32];
    sqlite3_snprintf(sizeof(zArgc), zArgc, "%d", argc-(3-TCLSH));
    Tcl_SetVar(interp,"argc", zArgc, TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp,"argv0",argv[1],TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp,"argv", "", TCL_GLOBAL_ONLY);
    for(i=3-TCLSH; i<argc; i++){
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
