/*
** 2003 April 6
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used to implement the VACUUM command.
**
** Most of the code in this file may be omitted by defining the
** SQLITE_OMIT_VACUUM macro.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"

/*
** A structure for holding a dynamic string - a string that can grow
** without bound. 
*/
typedef struct dynStr dynStr;
struct dynStr {
  char *z;        /* Text of the string in space obtained from sqliteMalloc() */
  int nAlloc;     /* Amount of space allocated to z[] */
  int nUsed;      /* Next unused slot in z[] */
};

/*
** A structure that holds the vacuum context
*/
typedef struct vacuumStruct vacuumStruct;
struct vacuumStruct {
  sqlite *dbOld;       /* Original database */
  sqlite *dbNew;       /* New database */
  char **pzErrMsg;     /* Write errors here */
  int rc;              /* Set to non-zero on an error */
  const char *zTable;  /* Name of a table being copied */
  const char *zPragma; /* Pragma to execute with results */
  dynStr s1, s2;       /* Two dynamic strings */
};

#if !defined(SQLITE_OMIT_VACUUM) || SQLITE_OMIT_VACUUM
/*
** Append text to a dynamic string
*/
static void appendText(dynStr *p, const char *zText, int nText){
  if( nText<0 ) nText = strlen(zText);
  if( p->z==0 || p->nUsed + nText + 1 >= p->nAlloc ){
    char *zNew;
    p->nAlloc = p->nUsed + nText + 1000;
    zNew = sqliteRealloc(p->z, p->nAlloc);
    if( zNew==0 ){
      sqliteFree(p->z);
      memset(p, 0, sizeof(*p));
      return;
    }
    p->z = zNew;
  }
  memcpy(&p->z[p->nUsed], zText, nText+1);
  p->nUsed += nText;
}

/*
** Append text to a dynamic string, having first put the text in quotes.
*/
static void appendQuoted(dynStr *p, const char *zText){
  int i, j;
  appendText(p, "'", 1);
  for(i=j=0; zText[i]; i++){
    if( zText[i]=='\'' ){
      appendText(p, &zText[j], i-j+1);
      j = i + 1;
      appendText(p, "'", 1);
    }
  }
  if( j<i ){
    appendText(p, &zText[j], i-j);
  }
  appendText(p, "'", 1);
}

/*
** Execute statements of SQL.  If an error occurs, write the error
** message into *pzErrMsg and return non-zero.
*/
static int execsql(char **pzErrMsg, sqlite *db, const char *zSql){ 
  char *zErrMsg = 0;
  int rc;

  /* printf("***** executing *****\n%s\n", zSql); */
  rc = sqlite_exec(db, zSql, 0, 0, &zErrMsg);
  if( zErrMsg ){
    sqliteSetString(pzErrMsg, zErrMsg, (char*)0);
    sqlite_freemem(zErrMsg);
  }
  return rc;
}

/*
** This is the second stage callback.  Each invocation contains all the
** data for a single row of a single table in the original database.  This
** routine must write that information into the new database.
*/
static int vacuumCallback2(void *pArg, int argc, char **argv, char **NotUsed){
  vacuumStruct *p = (vacuumStruct*)pArg;
  const char *zSep = "(";
  int i;

  if( argv==0 ) return 0;
  p->s2.nUsed = 0;
  appendText(&p->s2, "INSERT INTO ", -1);
  appendQuoted(&p->s2, p->zTable);
  appendText(&p->s2, " VALUES", -1);
  for(i=0; i<argc; i++){
    appendText(&p->s2, zSep, 1);
    zSep = ",";
    if( argv[i]==0 ){
      appendText(&p->s2, "NULL", 4);
    }else{
      appendQuoted(&p->s2, argv[i]);
    }
  }
  appendText(&p->s2,")", 1);
  p->rc = execsql(p->pzErrMsg, p->dbNew, p->s2.z);
  return p->rc;
}

/*
** This is the first stage callback.  Each invocation contains three
** arguments where are taken from the SQLITE_MASTER table of the original
** database:  (1) the entry type, (2) the entry name, and (3) the SQL for
** the entry.  In all cases, execute the SQL of the third argument.
** For tables, run a query to select all entries in that table and 
** transfer them to the second-stage callback.
*/
static int vacuumCallback1(void *pArg, int argc, char **argv, char **NotUsed){
  vacuumStruct *p = (vacuumStruct*)pArg;
  int rc = 0;
  assert( argc==3 );
  if( argv==0 ) return 0;
  assert( argv[0]!=0 );
  assert( argv[1]!=0 );
  assert( argv[2]!=0 );
  rc = execsql(p->pzErrMsg, p->dbNew, argv[2]);
  if( rc==SQLITE_OK && strcmp(argv[0],"table")==0 ){
    char *zErrMsg = 0;
    p->s1.nUsed = 0;
    appendText(&p->s1, "SELECT * FROM ", -1);
    appendQuoted(&p->s1, argv[1]);
    p->zTable = argv[1];
    rc = sqlite_exec(p->dbOld, p->s1.z, vacuumCallback2, p, &zErrMsg);
    if( zErrMsg ){
      sqliteSetString(p->pzErrMsg, zErrMsg, (char*)0);
      sqlite_freemem(zErrMsg);
    }
  }
  if( rc!=SQLITE_ABORT ) p->rc = rc;
  return rc;
}

/*
** This callback is used to transfer PRAGMA settings from one database
** to the other.  The value in argv[0] should be passed to a pragma
** identified by ((vacuumStruct*)pArg)->zPragma.
*/
static int vacuumCallback3(void *pArg, int argc, char **argv, char **NotUsed){
  vacuumStruct *p = (vacuumStruct*)pArg;
  char zBuf[200];
  assert( argc==1 );
  if( argv==0 ) return 0;
  assert( argv[0]!=0 );
  assert( strlen(p->zPragma)<100 );
  assert( strlen(argv[0])<30 );
  sprintf(zBuf,"PRAGMA %s=%s;", p->zPragma, argv[0]);
  p->rc = execsql(p->pzErrMsg, p->dbNew, zBuf);
  return p->rc;
}

/*
** Generate a random name of 20 character in length.
*/
static void randomName(unsigned char *zBuf){
  static const unsigned char zChars[] =
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789";
  int i;
  sqliteRandomness(20, zBuf);
  for(i=0; i<20; i++){
    zBuf[i] = zChars[ zBuf[i]%(sizeof(zChars)-1) ];
  }
}
#endif

/*
** The non-standard VACUUM command is used to clean up the database,
** collapse free space, etc.  It is modelled after the VACUUM command
** in PostgreSQL.
**
** In version 1.0.x of SQLite, the VACUUM command would call
** gdbm_reorganize() on all the database tables.  But beginning
** with 2.0.0, SQLite no longer uses GDBM so this command has
** become a no-op.
*/
void sqliteVacuum(Parse *pParse, Token *pTableName){
  Vdbe *v = sqliteGetVdbe(pParse);
  sqliteVdbeAddOp(v, OP_Vacuum, 0, 0);
  return;
}

/*
** This routine implements the OP_Vacuum opcode of the VDBE.
*/
int sqliteRunVacuum(char **pzErrMsg, sqlite *db){
#if !defined(SQLITE_OMIT_VACUUM) || SQLITE_OMIT_VACUUM
  const char *zFilename;  /* full pathname of the database file */
  int nFilename;          /* number of characters  in zFilename[] */
  char *zTemp = 0;        /* a temporary file in same directory as zFilename */
  sqlite *dbNew = 0;      /* The new vacuumed database */
  int rc = SQLITE_OK;     /* Return code from service routines */
  int i;                  /* Loop counter */
  char *zErrMsg;          /* Error message */
  vacuumStruct sVac;      /* Information passed to callbacks */

  /* These are all of the pragmas that need to be transferred over
  ** to the new database */
  static const char *zPragma[] = {
     "default_synchronous",
     "default_cache_size",
     /* "default_temp_store", */
  };

  if( db->flags & SQLITE_InTrans ){
    sqliteSetString(pzErrMsg, "cannot VACUUM from within a transaction", 
       (char*)0);
    return SQLITE_ERROR;
  }
  if( db->flags & SQLITE_Interrupt ){
    return SQLITE_INTERRUPT;
  }
  memset(&sVac, 0, sizeof(sVac));

  /* Get the full pathname of the database file and create two
  ** temporary filenames in the same directory as the original file.
  */
  zFilename = sqliteBtreeGetFilename(db->aDb[0].pBt);
  if( zFilename==0 ){
    /* This only happens with the in-memory database.  VACUUM is a no-op
    ** there, so just return */
    return SQLITE_OK;
  }
  nFilename = strlen(zFilename);
  zTemp = sqliteMalloc( nFilename+100 );
  if( zTemp==0 ) return SQLITE_NOMEM;
  strcpy(zTemp, zFilename);
  for(i=0; i<10; i++){
    zTemp[nFilename] = '-';
    randomName((unsigned char*)&zTemp[nFilename+1]);
    if( !sqliteOsFileExists(zTemp) ) break;
  }
  if( i>=10 ){
    sqliteSetString(pzErrMsg, "unable to create a temporary database file "
       "in the same directory as the original database", (char*)0);
    goto end_of_vacuum;
  }

  
  dbNew = sqlite_open(zTemp, 0, &zErrMsg);
  if( dbNew==0 ){
    sqliteSetString(pzErrMsg, "unable to open a temporary database at ",
       zTemp, " - ", zErrMsg, (char*)0);
    goto end_of_vacuum;
  }
  if( (rc = execsql(pzErrMsg, db, "BEGIN"))!=0 ) goto end_of_vacuum;
  if( (rc = execsql(pzErrMsg, dbNew, "PRAGMA synchronous=off; BEGIN"))!=0 ){
    goto end_of_vacuum;
  }
  
  sVac.dbOld = db;
  sVac.dbNew = dbNew;
  sVac.pzErrMsg = pzErrMsg;
  for(i=0; rc==SQLITE_OK && i<sizeof(zPragma)/sizeof(zPragma[0]); i++){
    char zBuf[200];
    assert( strlen(zPragma[i])<100 );
    sprintf(zBuf, "PRAGMA %s;", zPragma[i]);
    sVac.zPragma = zPragma[i];
    rc = sqlite_exec(db, zBuf, vacuumCallback3, &sVac, &zErrMsg);
  }
  if( rc==SQLITE_OK ){
    rc = sqlite_exec(db, 
      "SELECT type, name, sql FROM sqlite_master "
      "WHERE sql NOT NULL AND type!='view' "
      "UNION ALL "
      "SELECT type, name, sql FROM sqlite_master "
      "WHERE sql NOT NULL AND type=='view'",
      vacuumCallback1, &sVac, &zErrMsg);
  }
  if( rc==SQLITE_OK ){
    rc = sqliteBtreeCopyFile(db->aDb[0].pBt, dbNew->aDb[0].pBt);
    sqlite_exec(db, "COMMIT", 0, 0, 0);
    sqliteResetInternalSchema(db, 0);
  }

end_of_vacuum:
  if( rc && zErrMsg!=0 ){
    sqliteSetString(pzErrMsg, "unable to vacuum database - ", 
       zErrMsg, (char*)0);
  }
  sqlite_exec(db, "ROLLBACK", 0, 0, 0);
  if( (dbNew && (dbNew->flags & SQLITE_Interrupt)) 
         || (db->flags & SQLITE_Interrupt) ){
    rc = SQLITE_INTERRUPT;
  }
  if( dbNew ) sqlite_close(dbNew);
  sqliteOsDelete(zTemp);
  sqliteFree(zTemp);
  sqliteFree(sVac.s1.z);
  sqliteFree(sVac.s2.z);
  if( zErrMsg ) sqlite_freemem(zErrMsg);
  if( rc==SQLITE_ABORT && sVac.rc!=SQLITE_INTERRUPT ) sVac.rc = SQLITE_ERROR;
  return sVac.rc;
#endif
}
