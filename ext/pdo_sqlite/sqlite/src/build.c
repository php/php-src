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
** This file contains C code routines that are called by the SQLite parser
** when syntax rules are reduced.  The routines in this file handle the
** following kinds of SQL syntax:
**
**     CREATE TABLE
**     DROP TABLE
**     CREATE INDEX
**     DROP INDEX
**     creating ID lists
**     BEGIN TRANSACTION
**     COMMIT
**     ROLLBACK
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

/*
** This routine is called when a new SQL statement is beginning to
** be parsed.  Initialize the pParse structure as needed.
*/
void sqlite3BeginParse(Parse *pParse, int explainFlag){
  pParse->explain = explainFlag;
  pParse->nVar = 0;
}

/*
** This routine is called after a single SQL statement has been
** parsed and a VDBE program to execute that statement has been
** prepared.  This routine puts the finishing touches on the
** VDBE program and resets the pParse structure for the next
** parse.
**
** Note that if an error occurred, it might be the case that
** no VDBE code was generated.
*/
void sqlite3FinishCoding(Parse *pParse){
  sqlite3 *db;
  Vdbe *v;

  if( sqlite3_malloc_failed ) return;
  if( pParse->nested ) return;
  if( !pParse->pVdbe ){
    if( pParse->rc==SQLITE_OK && pParse->nErr ){
      pParse->rc = SQLITE_ERROR;
    }
    return;
  }

  /* Begin by generating some termination code at the end of the
  ** vdbe program
  */
  db = pParse->db;
  v = sqlite3GetVdbe(pParse);
  if( v ){
    sqlite3VdbeAddOp(v, OP_Halt, 0, 0);

    /* The cookie mask contains one bit for each database file open.
    ** (Bit 0 is for main, bit 1 is for temp, and so forth.)  Bits are
    ** set for each database that is used.  Generate code to start a
    ** transaction on each used database and to verify the schema cookie
    ** on each used database.
    */
    if( pParse->cookieGoto>0 ){
      u32 mask;
      int iDb;
      sqlite3VdbeChangeP2(v, pParse->cookieGoto-1, sqlite3VdbeCurrentAddr(v));
      for(iDb=0, mask=1; iDb<db->nDb; mask<<=1, iDb++){
        if( (mask & pParse->cookieMask)==0 ) continue;
        sqlite3VdbeAddOp(v, OP_Transaction, iDb, (mask & pParse->writeMask)!=0);
        sqlite3VdbeAddOp(v, OP_VerifyCookie, iDb, pParse->cookieValue[iDb]);
      }
      sqlite3VdbeAddOp(v, OP_Goto, 0, pParse->cookieGoto);
    }

    /* Add a No-op that contains the complete text of the compiled SQL
    ** statement as its P3 argument.  This does not change the functionality
    ** of the program. 
    **
    ** This is used to implement sqlite3_trace().
    */
    sqlite3VdbeOp3(v, OP_Noop, 0, 0, pParse->zSql, pParse->zTail-pParse->zSql);
  }


  /* Get the VDBE program ready for execution
  */
  if( v && pParse->nErr==0 ){
    FILE *trace = (db->flags & SQLITE_VdbeTrace)!=0 ? stdout : 0;
    sqlite3VdbeTrace(v, trace);
    sqlite3VdbeMakeReady(v, pParse->nVar, pParse->nMem+3,
                         pParse->nTab+3, pParse->nMaxDepth+1, pParse->explain);
    pParse->rc = SQLITE_DONE;
    pParse->colNamesSet = 0;
  }else if( pParse->rc==SQLITE_OK ){
    pParse->rc = SQLITE_ERROR;
  }
  pParse->nTab = 0;
  pParse->nMem = 0;
  pParse->nSet = 0;
  pParse->nVar = 0;
  pParse->cookieMask = 0;
  pParse->cookieGoto = 0;
}

/*
** Run the parser and code generator recursively in order to generate
** code for the SQL statement given onto the end of the pParse context
** currently under construction.  When the parser is run recursively
** this way, the final OP_Halt is not appended and other initialization
** and finalization steps are omitted because those are handling by the
** outermost parser.
**
** Not everything is nestable.  This facility is designed to permit
** INSERT, UPDATE, and DELETE operations against SQLITE_MASTER.  Use
** care if you decide to try to use this routine for some other purposes.
*/
void sqlite3NestedParse(Parse *pParse, const char *zFormat, ...){
  va_list ap;
  char *zSql;
  int rc;
# define SAVE_SZ  (sizeof(Parse) - offsetof(Parse,nVar))
  char saveBuf[SAVE_SZ];

  if( pParse->nErr ) return;
  assert( pParse->nested<10 );  /* Nesting should only be of limited depth */
  va_start(ap, zFormat);
  zSql = sqlite3VMPrintf(zFormat, ap);
  va_end(ap);
  if( zSql==0 ){
    return;   /* A malloc must have failed */
  }
  pParse->nested++;
  memcpy(saveBuf, &pParse->nVar, SAVE_SZ);
  memset(&pParse->nVar, 0, SAVE_SZ);
  rc = sqlite3RunParser(pParse, zSql, 0);
  sqliteFree(zSql);
  memcpy(&pParse->nVar, saveBuf, SAVE_SZ);
  pParse->nested--;
}

/*
** Locate the in-memory structure that describes a particular database
** table given the name of that table and (optionally) the name of the
** database containing the table.  Return NULL if not found.
**
** If zDatabase is 0, all databases are searched for the table and the
** first matching table is returned.  (No checking for duplicate table
** names is done.)  The search order is TEMP first, then MAIN, then any
** auxiliary databases added using the ATTACH command.
**
** See also sqlite3LocateTable().
*/
Table *sqlite3FindTable(sqlite3 *db, const char *zName, const char *zDatabase){
  Table *p = 0;
  int i;
  assert( zName!=0 );
  assert( (db->flags & SQLITE_Initialized) || db->init.busy );
  for(i=OMIT_TEMPDB; i<db->nDb; i++){
    int j = (i<2) ? i^1 : i;   /* Search TEMP before MAIN */
    if( zDatabase!=0 && sqlite3StrICmp(zDatabase, db->aDb[j].zName) ) continue;
    p = sqlite3HashFind(&db->aDb[j].tblHash, zName, strlen(zName)+1);
    if( p ) break;
  }
  return p;
}

/*
** Locate the in-memory structure that describes a particular database
** table given the name of that table and (optionally) the name of the
** database containing the table.  Return NULL if not found.  Also leave an
** error message in pParse->zErrMsg.
**
** The difference between this routine and sqlite3FindTable() is that this
** routine leaves an error message in pParse->zErrMsg where
** sqlite3FindTable() does not.
*/
Table *sqlite3LocateTable(Parse *pParse, const char *zName, const char *zDbase){
  Table *p;

  /* Read the database schema. If an error occurs, leave an error message
  ** and code in pParse and return NULL. */
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    return 0;
  }

  p = sqlite3FindTable(pParse->db, zName, zDbase);
  if( p==0 ){
    if( zDbase ){
      sqlite3ErrorMsg(pParse, "no such table: %s.%s", zDbase, zName);
    }else{
      sqlite3ErrorMsg(pParse, "no such table: %s", zName);
    }
    pParse->checkSchema = 1;
  }
  return p;
}

/*
** Locate the in-memory structure that describes 
** a particular index given the name of that index
** and the name of the database that contains the index.
** Return NULL if not found.
**
** If zDatabase is 0, all databases are searched for the
** table and the first matching index is returned.  (No checking
** for duplicate index names is done.)  The search order is
** TEMP first, then MAIN, then any auxiliary databases added
** using the ATTACH command.
*/
Index *sqlite3FindIndex(sqlite3 *db, const char *zName, const char *zDb){
  Index *p = 0;
  int i;
  assert( (db->flags & SQLITE_Initialized) || db->init.busy );
  for(i=OMIT_TEMPDB; i<db->nDb; i++){
    int j = (i<2) ? i^1 : i;  /* Search TEMP before MAIN */
    if( zDb && sqlite3StrICmp(zDb, db->aDb[j].zName) ) continue;
    p = sqlite3HashFind(&db->aDb[j].idxHash, zName, strlen(zName)+1);
    if( p ) break;
  }
  return p;
}

/*
** Reclaim the memory used by an index
*/
static void freeIndex(Index *p){
  sqliteFree(p->zColAff);
  sqliteFree(p);
}

/*
** Remove the given index from the index hash table, and free
** its memory structures.
**
** The index is removed from the database hash tables but
** it is not unlinked from the Table that it indexes.
** Unlinking from the Table must be done by the calling function.
*/
static void sqliteDeleteIndex(sqlite3 *db, Index *p){
  Index *pOld;

  assert( db!=0 && p->zName!=0 );
  pOld = sqlite3HashInsert(&db->aDb[p->iDb].idxHash, p->zName,
                          strlen(p->zName)+1, 0);
  assert( pOld==0 || pOld==p );
  freeIndex(p);
}

/*
** For the index called zIdxName which is found in the database iDb,
** unlike that index from its Table then remove the index from
** the index hash table and free all memory structures associated
** with the index.
*/
void sqlite3UnlinkAndDeleteIndex(sqlite3 *db, int iDb, const char *zIdxName){
  Index *pIndex;
  int len;

  len = strlen(zIdxName);
  pIndex = sqlite3HashInsert(&db->aDb[iDb].idxHash, zIdxName, len+1, 0);
  if( pIndex ){
    if( pIndex->pTable->pIndex==pIndex ){
      pIndex->pTable->pIndex = pIndex->pNext;
    }else{
      Index *p;
      for(p=pIndex->pTable->pIndex; p && p->pNext!=pIndex; p=p->pNext){}
      if( p && p->pNext==pIndex ){
        p->pNext = pIndex->pNext;
      }
    }
    freeIndex(pIndex);
  }
  db->flags |= SQLITE_InternChanges;
}

/*
** Erase all schema information from the in-memory hash tables of
** a single database.  This routine is called to reclaim memory
** before the database closes.  It is also called during a rollback
** if there were schema changes during the transaction or if a
** schema-cookie mismatch occurs.
**
** If iDb<=0 then reset the internal schema tables for all database
** files.  If iDb>=2 then reset the internal schema for only the
** single file indicated.
*/
void sqlite3ResetInternalSchema(sqlite3 *db, int iDb){
  HashElem *pElem;
  Hash temp1;
  Hash temp2;
  int i, j;

  assert( iDb>=0 && iDb<db->nDb );
  db->flags &= ~SQLITE_Initialized;
  for(i=iDb; i<db->nDb; i++){
    Db *pDb = &db->aDb[i];
    temp1 = pDb->tblHash;
    temp2 = pDb->trigHash;
    sqlite3HashInit(&pDb->trigHash, SQLITE_HASH_STRING, 0);
    sqlite3HashClear(&pDb->aFKey);
    sqlite3HashClear(&pDb->idxHash);
    for(pElem=sqliteHashFirst(&temp2); pElem; pElem=sqliteHashNext(pElem)){
      sqlite3DeleteTrigger((Trigger*)sqliteHashData(pElem));
    }
    sqlite3HashClear(&temp2);
    sqlite3HashInit(&pDb->tblHash, SQLITE_HASH_STRING, 0);
    for(pElem=sqliteHashFirst(&temp1); pElem; pElem=sqliteHashNext(pElem)){
      Table *pTab = sqliteHashData(pElem);
      sqlite3DeleteTable(db, pTab);
    }
    sqlite3HashClear(&temp1);
    pDb->pSeqTab = 0;
    DbClearProperty(db, i, DB_SchemaLoaded);
    if( iDb>0 ) return;
  }
  assert( iDb==0 );
  db->flags &= ~SQLITE_InternChanges;

  /* If one or more of the auxiliary database files has been closed,
  ** then remove then from the auxiliary database list.  We take the
  ** opportunity to do this here since we have just deleted all of the
  ** schema hash tables and therefore do not have to make any changes
  ** to any of those tables.
  */
  for(i=0; i<db->nDb; i++){
    struct Db *pDb = &db->aDb[i];
    if( pDb->pBt==0 ){
      if( pDb->pAux && pDb->xFreeAux ) pDb->xFreeAux(pDb->pAux);
      pDb->pAux = 0;
    }
  }
  for(i=j=2; i<db->nDb; i++){
    struct Db *pDb = &db->aDb[i];
    if( pDb->pBt==0 ){
      sqliteFree(pDb->zName);
      pDb->zName = 0;
      continue;
    }
    if( j<i ){
      db->aDb[j] = db->aDb[i];
    }
    j++;
  }
  memset(&db->aDb[j], 0, (db->nDb-j)*sizeof(db->aDb[j]));
  db->nDb = j;
  if( db->nDb<=2 && db->aDb!=db->aDbStatic ){
    memcpy(db->aDbStatic, db->aDb, 2*sizeof(db->aDb[0]));
    sqliteFree(db->aDb);
    db->aDb = db->aDbStatic;
  }
}

/*
** This routine is called whenever a rollback occurs.  If there were
** schema changes during the transaction, then we have to reset the
** internal hash tables and reload them from disk.
*/
void sqlite3RollbackInternalChanges(sqlite3 *db){
  if( db->flags & SQLITE_InternChanges ){
    sqlite3ResetInternalSchema(db, 0);
  }
}

/*
** This routine is called when a commit occurs.
*/
void sqlite3CommitInternalChanges(sqlite3 *db){
  db->flags &= ~SQLITE_InternChanges;
}

/*
** Clear the column names from a table or view.
*/
static void sqliteResetColumnNames(Table *pTable){
  int i;
  Column *pCol;
  assert( pTable!=0 );
  if( (pCol = pTable->aCol)!=0 ){
    for(i=0; i<pTable->nCol; i++, pCol++){
      sqliteFree(pCol->zName);
      sqlite3ExprDelete(pCol->pDflt);
      sqliteFree(pCol->zType);
    }
    sqliteFree(pTable->aCol);
  }
  pTable->aCol = 0;
  pTable->nCol = 0;
}

/*
** Remove the memory data structures associated with the given
** Table.  No changes are made to disk by this routine.
**
** This routine just deletes the data structure.  It does not unlink
** the table data structure from the hash table.  Nor does it remove
** foreign keys from the sqlite.aFKey hash table.  But it does destroy
** memory structures of the indices and foreign keys associated with 
** the table.
**
** Indices associated with the table are unlinked from the "db"
** data structure if db!=NULL.  If db==NULL, indices attached to
** the table are deleted, but it is assumed they have already been
** unlinked.
*/
void sqlite3DeleteTable(sqlite3 *db, Table *pTable){
  Index *pIndex, *pNext;
  FKey *pFKey, *pNextFKey;

  if( pTable==0 ) return;

  /* Do not delete the table until the reference count reaches zero. */
  pTable->nRef--;
  if( pTable->nRef>0 ){
    return;
  }
  assert( pTable->nRef==0 );

  /* Delete all indices associated with this table
  */
  for(pIndex = pTable->pIndex; pIndex; pIndex=pNext){
    pNext = pIndex->pNext;
    assert( pIndex->iDb==pTable->iDb || (pTable->iDb==0 && pIndex->iDb==1) );
    sqliteDeleteIndex(db, pIndex);
  }

#ifndef SQLITE_OMIT_FOREIGN_KEY
  /* Delete all foreign keys associated with this table.  The keys
  ** should have already been unlinked from the db->aFKey hash table 
  */
  for(pFKey=pTable->pFKey; pFKey; pFKey=pNextFKey){
    pNextFKey = pFKey->pNextFrom;
    assert( pTable->iDb<db->nDb );
    assert( sqlite3HashFind(&db->aDb[pTable->iDb].aFKey,
                           pFKey->zTo, strlen(pFKey->zTo)+1)!=pFKey );
    sqliteFree(pFKey);
  }
#endif

  /* Delete the Table structure itself.
  */
  sqliteResetColumnNames(pTable);
  sqliteFree(pTable->zName);
  sqliteFree(pTable->zColAff);
  sqlite3SelectDelete(pTable->pSelect);
  sqliteFree(pTable);
}

/*
** Unlink the given table from the hash tables and the delete the
** table structure with all its indices and foreign keys.
*/
void sqlite3UnlinkAndDeleteTable(sqlite3 *db, int iDb, const char *zTabName){
  Table *p;
  FKey *pF1, *pF2;
  Db *pDb;

  assert( db!=0 );
  assert( iDb>=0 && iDb<db->nDb );
  assert( zTabName && zTabName[0] );
  pDb = &db->aDb[iDb];
  p = sqlite3HashInsert(&pDb->tblHash, zTabName, strlen(zTabName)+1, 0);
  if( p ){
#ifndef SQLITE_OMIT_FOREIGN_KEY
    for(pF1=p->pFKey; pF1; pF1=pF1->pNextFrom){
      int nTo = strlen(pF1->zTo) + 1;
      pF2 = sqlite3HashFind(&pDb->aFKey, pF1->zTo, nTo);
      if( pF2==pF1 ){
        sqlite3HashInsert(&pDb->aFKey, pF1->zTo, nTo, pF1->pNextTo);
      }else{
        while( pF2 && pF2->pNextTo!=pF1 ){ pF2=pF2->pNextTo; }
        if( pF2 ){
          pF2->pNextTo = pF1->pNextTo;
        }
      }
    }
#endif
    sqlite3DeleteTable(db, p);
  }
  db->flags |= SQLITE_InternChanges;
}

/*
** Given a token, return a string that consists of the text of that
** token with any quotations removed.  Space to hold the returned string
** is obtained from sqliteMalloc() and must be freed by the calling
** function.
**
** Tokens are often just pointers into the original SQL text and so
** are not \000 terminated and are not persistent.  The returned string
** is \000 terminated and is persistent.
*/
char *sqlite3NameFromToken(Token *pName){
  char *zName;
  if( pName ){
    zName = sqliteStrNDup(pName->z, pName->n);
    sqlite3Dequote(zName);
  }else{
    zName = 0;
  }
  return zName;
}

/*
** Open the sqlite_master table stored in database number iDb for
** writing. The table is opened using cursor 0.
*/
void sqlite3OpenMasterTable(Vdbe *v, int iDb){
  sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
  sqlite3VdbeAddOp(v, OP_OpenWrite, 0, MASTER_ROOT);
  sqlite3VdbeAddOp(v, OP_SetNumColumns, 0, 5); /* sqlite_master has 5 columns */
}

/*
** The token *pName contains the name of a database (either "main" or
** "temp" or the name of an attached db). This routine returns the
** index of the named database in db->aDb[], or -1 if the named db 
** does not exist.
*/
int sqlite3FindDb(sqlite3 *db, Token *pName){
  int i = -1;    /* Database number */
  int n;         /* Number of characters in the name */
  Db *pDb;       /* A database whose name space is being searched */
  char *zName;   /* Name we are searching for */

  zName = sqlite3NameFromToken(pName);
  if( zName ){
    n = strlen(zName);
    for(i=(db->nDb-1), pDb=&db->aDb[i]; i>=0; i--, pDb--){
      if( (!OMIT_TEMPDB || i!=1 ) && n==strlen(pDb->zName) && 
          0==sqlite3StrICmp(pDb->zName, zName) ){
        break;
      }
    }
    sqliteFree(zName);
  }
  return i;
}

/* The table or view or trigger name is passed to this routine via tokens
** pName1 and pName2. If the table name was fully qualified, for example:
**
** CREATE TABLE xxx.yyy (...);
** 
** Then pName1 is set to "xxx" and pName2 "yyy". On the other hand if
** the table name is not fully qualified, i.e.:
**
** CREATE TABLE yyy(...);
**
** Then pName1 is set to "yyy" and pName2 is "".
**
** This routine sets the *ppUnqual pointer to point at the token (pName1 or
** pName2) that stores the unqualified table name.  The index of the
** database "xxx" is returned.
*/
int sqlite3TwoPartName(
  Parse *pParse,      /* Parsing and code generating context */
  Token *pName1,      /* The "xxx" in the name "xxx.yyy" or "xxx" */
  Token *pName2,      /* The "yyy" in the name "xxx.yyy" */
  Token **pUnqual     /* Write the unqualified object name here */
){
  int iDb;                    /* Database holding the object */
  sqlite3 *db = pParse->db;

  if( pName2 && pName2->n>0 ){
    assert( !db->init.busy );
    *pUnqual = pName2;
    iDb = sqlite3FindDb(db, pName1);
    if( iDb<0 ){
      sqlite3ErrorMsg(pParse, "unknown database %T", pName1);
      pParse->nErr++;
      return -1;
    }
  }else{
    assert( db->init.iDb==0 || db->init.busy );
    iDb = db->init.iDb;
    *pUnqual = pName1;
  }
  return iDb;
}

/*
** This routine is used to check if the UTF-8 string zName is a legal
** unqualified name for a new schema object (table, index, view or
** trigger). All names are legal except those that begin with the string
** "sqlite_" (in upper, lower or mixed case). This portion of the namespace
** is reserved for internal use.
*/
int sqlite3CheckObjectName(Parse *pParse, const char *zName){
  if( !pParse->db->init.busy && pParse->nested==0 
          && (pParse->db->flags & SQLITE_WriteSchema)==0
          && 0==sqlite3StrNICmp(zName, "sqlite_", 7) ){
    sqlite3ErrorMsg(pParse, "object name reserved for internal use: %s", zName);
    return SQLITE_ERROR;
  }
  return SQLITE_OK;
}

/*
** Begin constructing a new table representation in memory.  This is
** the first of several action routines that get called in response
** to a CREATE TABLE statement.  In particular, this routine is called
** after seeing tokens "CREATE" and "TABLE" and the table name.  The
** pStart token is the CREATE and pName is the table name.  The isTemp
** flag is true if the table should be stored in the auxiliary database
** file instead of in the main database file.  This is normally the case
** when the "TEMP" or "TEMPORARY" keyword occurs in between
** CREATE and TABLE.
**
** The new table record is initialized and put in pParse->pNewTable.
** As more of the CREATE TABLE statement is parsed, additional action
** routines will be called to add more information to this record.
** At the end of the CREATE TABLE statement, the sqlite3EndTable() routine
** is called to complete the construction of the new table record.
*/
void sqlite3StartTable(
  Parse *pParse,   /* Parser context */
  Token *pStart,   /* The "CREATE" token */
  Token *pName1,   /* First part of the name of the table or view */
  Token *pName2,   /* Second part of the name of the table or view */
  int isTemp,      /* True if this is a TEMP table */
  int isView       /* True if this is a VIEW */
){
  Table *pTable;
  Index *pIdx;
  char *zName = 0; /* The name of the new table */
  sqlite3 *db = pParse->db;
  Vdbe *v;
  int iDb;         /* Database number to create the table in */
  Token *pName;    /* Unqualified name of the table to create */

  /* The table or view name to create is passed to this routine via tokens
  ** pName1 and pName2. If the table name was fully qualified, for example:
  **
  ** CREATE TABLE xxx.yyy (...);
  ** 
  ** Then pName1 is set to "xxx" and pName2 "yyy". On the other hand if
  ** the table name is not fully qualified, i.e.:
  **
  ** CREATE TABLE yyy(...);
  **
  ** Then pName1 is set to "yyy" and pName2 is "".
  **
  ** The call below sets the pName pointer to point at the token (pName1 or
  ** pName2) that stores the unqualified table name. The variable iDb is
  ** set to the index of the database that the table or view is to be
  ** created in.
  */
  iDb = sqlite3TwoPartName(pParse, pName1, pName2, &pName);
  if( iDb<0 ) return;
  if( !OMIT_TEMPDB && isTemp && iDb>1 ){
    /* If creating a temp table, the name may not be qualified */
    sqlite3ErrorMsg(pParse, "temporary table name must be unqualified");
    return;
  }
  if( !OMIT_TEMPDB && isTemp ) iDb = 1;

  pParse->sNameToken = *pName;
  zName = sqlite3NameFromToken(pName);
  if( zName==0 ) return;
  if( SQLITE_OK!=sqlite3CheckObjectName(pParse, zName) ){
    goto begin_table_error;
  }
  if( db->init.iDb==1 ) isTemp = 1;
#ifndef SQLITE_OMIT_AUTHORIZATION
  assert( (isTemp & 1)==isTemp );
  {
    int code;
    char *zDb = db->aDb[iDb].zName;
    if( sqlite3AuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0, zDb) ){
      goto begin_table_error;
    }
    if( isView ){
      if( !OMIT_TEMPDB && isTemp ){
        code = SQLITE_CREATE_TEMP_VIEW;
      }else{
        code = SQLITE_CREATE_VIEW;
      }
    }else{
      if( !OMIT_TEMPDB && isTemp ){
        code = SQLITE_CREATE_TEMP_TABLE;
      }else{
        code = SQLITE_CREATE_TABLE;
      }
    }
    if( sqlite3AuthCheck(pParse, code, zName, 0, zDb) ){
      goto begin_table_error;
    }
  }
#endif

  /* Make sure the new table name does not collide with an existing
  ** index or table name in the same database.  Issue an error message if
  ** it does.
  */
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    goto begin_table_error;
  }
  pTable = sqlite3FindTable(db, zName, db->aDb[iDb].zName);
  if( pTable ){
    sqlite3ErrorMsg(pParse, "table %T already exists", pName);
    goto begin_table_error;
  }
  if( (pIdx = sqlite3FindIndex(db, zName, 0))!=0 && 
      ( iDb==0 || !db->init.busy) ){
    sqlite3ErrorMsg(pParse, "there is already an index named %s", zName);
    goto begin_table_error;
  }
  pTable = sqliteMalloc( sizeof(Table) );
  if( pTable==0 ){
    pParse->rc = SQLITE_NOMEM;
    pParse->nErr++;
    goto begin_table_error;
  }
  pTable->zName = zName;
  pTable->nCol = 0;
  pTable->aCol = 0;
  pTable->iPKey = -1;
  pTable->pIndex = 0;
  pTable->iDb = iDb;
  pTable->nRef = 1;
  if( pParse->pNewTable ) sqlite3DeleteTable(db, pParse->pNewTable);
  pParse->pNewTable = pTable;

  /* If this is the magic sqlite_sequence table used by autoincrement,
  ** then record a pointer to this table in the main database structure
  ** so that INSERT can find the table easily.
  */
#ifndef SQLITE_OMIT_AUTOINCREMENT
  if( !pParse->nested && strcmp(zName, "sqlite_sequence")==0 ){
    db->aDb[iDb].pSeqTab = pTable;
  }
#endif

  /* Begin generating the code that will insert the table record into
  ** the SQLITE_MASTER table.  Note in particular that we must go ahead
  ** and allocate the record number for the table entry now.  Before any
  ** PRIMARY KEY or UNIQUE keywords are parsed.  Those keywords will cause
  ** indices to be created and the table record must come before the 
  ** indices.  Hence, the record number for the table must be allocated
  ** now.
  */
  if( !db->init.busy && (v = sqlite3GetVdbe(pParse))!=0 ){
    int lbl;
    sqlite3BeginWriteOperation(pParse, 0, iDb);

    /* If the file format and encoding in the database have not been set, 
    ** set them now.
    */
    sqlite3VdbeAddOp(v, OP_ReadCookie, iDb, 1);   /* file_format */
    lbl = sqlite3VdbeMakeLabel(v);
    sqlite3VdbeAddOp(v, OP_If, 0, lbl);
    sqlite3VdbeAddOp(v, OP_Integer, db->file_format, 0);
    sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 1);
    sqlite3VdbeAddOp(v, OP_Integer, db->enc, 0);
    sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 4);
    sqlite3VdbeResolveLabel(v, lbl);

    /* This just creates a place-holder record in the sqlite_master table.
    ** The record created does not contain anything yet.  It will be replaced
    ** by the real entry in code generated at sqlite3EndTable().
    **
    ** The rowid for the new entry is left on the top of the stack.
    ** The rowid value is needed by the code that sqlite3EndTable will
    ** generate.
    */
#ifndef SQLITE_OMIT_VIEW
    if( isView ){
      sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
    }else
#endif
    {
      sqlite3VdbeAddOp(v, OP_CreateTable, iDb, 0);
    }
    sqlite3OpenMasterTable(v, iDb);
    sqlite3VdbeAddOp(v, OP_NewRowid, 0, 0);
    sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
    sqlite3VdbeAddOp(v, OP_Null, 0, 0);
    sqlite3VdbeAddOp(v, OP_Insert, 0, 0);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
    sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
  }

  /* Normal (non-error) return. */
  return;

  /* If an error occurs, we jump here */
begin_table_error:
  sqliteFree(zName);
  return;
}

/*
** This macro is used to compare two strings in a case-insensitive manner.
** It is slightly faster than calling sqlite3StrICmp() directly, but
** produces larger code.
**
** WARNING: This macro is not compatible with the strcmp() family. It
** returns true if the two strings are equal, otherwise false.
*/
#define STRICMP(x, y) (\
sqlite3UpperToLower[*(unsigned char *)(x)]==   \
sqlite3UpperToLower[*(unsigned char *)(y)]     \
&& sqlite3StrICmp((x)+1,(y)+1)==0 )

/*
** Add a new column to the table currently being constructed.
**
** The parser calls this routine once for each column declaration
** in a CREATE TABLE statement.  sqlite3StartTable() gets called
** first to get things going.  Then this routine is called for each
** column.
*/
void sqlite3AddColumn(Parse *pParse, Token *pName){
  Table *p;
  int i;
  char *z;
  Column *pCol;
  if( (p = pParse->pNewTable)==0 ) return;
  z = sqlite3NameFromToken(pName);
  if( z==0 ) return;
  for(i=0; i<p->nCol; i++){
    if( STRICMP(z, p->aCol[i].zName) ){
      sqlite3ErrorMsg(pParse, "duplicate column name: %s", z);
      sqliteFree(z);
      return;
    }
  }
  if( (p->nCol & 0x7)==0 ){
    Column *aNew;
    aNew = sqliteRealloc( p->aCol, (p->nCol+8)*sizeof(p->aCol[0]));
    if( aNew==0 ){
      sqliteFree(z);
      return;
    }
    p->aCol = aNew;
  }
  pCol = &p->aCol[p->nCol];
  memset(pCol, 0, sizeof(p->aCol[0]));
  pCol->zName = z;
 
  /* If there is no type specified, columns have the default affinity
  ** 'NONE'. If there is a type specified, then sqlite3AddColumnType() will
  ** be called next to set pCol->affinity correctly.
  */
  pCol->affinity = SQLITE_AFF_NONE;
  pCol->pColl = pParse->db->pDfltColl;
  p->nCol++;
}

/*
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.  A "NOT NULL" constraint has
** been seen on a column.  This routine sets the notNull flag on
** the column currently under construction.
*/
void sqlite3AddNotNull(Parse *pParse, int onError){
  Table *p;
  int i;
  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i>=0 ) p->aCol[i].notNull = onError;
}

/*
** Scan the column type name zType (length nType) and return the
** associated affinity type.
**
** This routine does a case-independent search of zType for the 
** substrings in the following table. If one of the substrings is
** found, the corresponding affinity is returned. If zType contains
** more than one of the substrings, entries toward the top of 
** the table take priority. For example, if zType is 'BLOBINT', 
** SQLITE_AFF_INTEGER is returned.
**
** Substring     | Affinity
** --------------------------------
** 'INT'         | SQLITE_AFF_INTEGER
** 'CHAR'        | SQLITE_AFF_TEXT
** 'CLOB'        | SQLITE_AFF_TEXT
** 'TEXT'        | SQLITE_AFF_TEXT
** 'BLOB'        | SQLITE_AFF_NONE
**
** If none of the substrings in the above table are found,
** SQLITE_AFF_NUMERIC is returned.
*/
char sqlite3AffinityType(const Token *pType){
  u32 h = 0;
  char aff = SQLITE_AFF_NUMERIC;
  const unsigned char *zIn = pType->z;
  const unsigned char *zEnd = &pType->z[pType->n];

  while( zIn!=zEnd ){
    h = (h<<8) + sqlite3UpperToLower[*zIn];
    zIn++;
    if( h==(('c'<<24)+('h'<<16)+('a'<<8)+'r') ){             /* CHAR */
      aff = SQLITE_AFF_TEXT; 
    }else if( h==(('c'<<24)+('l'<<16)+('o'<<8)+'b') ){       /* CLOB */
      aff = SQLITE_AFF_TEXT;
    }else if( h==(('t'<<24)+('e'<<16)+('x'<<8)+'t') ){       /* TEXT */
      aff = SQLITE_AFF_TEXT;
    }else if( h==(('b'<<24)+('l'<<16)+('o'<<8)+'b')          /* BLOB */
        && aff==SQLITE_AFF_NUMERIC ){
      aff = SQLITE_AFF_NONE;
    }else if( (h&0x00FFFFFF)==(('i'<<16)+('n'<<8)+'t') ){    /* INT */
      aff = SQLITE_AFF_INTEGER; 
      break;
    }
  }

  return aff;
}

/*
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.  The pFirst token is the first
** token in the sequence of tokens that describe the type of the
** column currently under construction.   pLast is the last token
** in the sequence.  Use this information to construct a string
** that contains the typename of the column and store that string
** in zType.
*/ 
void sqlite3AddColumnType(Parse *pParse, Token *pType){
  Table *p;
  int i;
  Column *pCol;

  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i<0 ) return;
  pCol = &p->aCol[i];
  assert( pCol->zType==0 );
  pCol->zType = sqlite3NameFromToken(pType);
  pCol->affinity = sqlite3AffinityType(pType);
}

/*
** The expression is the default value for the most recently added column
** of the table currently under construction.
**
** Default value expressions must be constant.  Raise an exception if this
** is not the case.
**
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.
*/
void sqlite3AddDefaultValue(Parse *pParse, Expr *pExpr){
  Table *p;
  Column *pCol;
  if( (p = pParse->pNewTable)!=0 ){
    pCol = &(p->aCol[p->nCol-1]);
    if( !sqlite3ExprIsConstantOrFunction(pExpr) ){
      sqlite3ErrorMsg(pParse, "default value of column [%s] is not constant",
          pCol->zName);
    }else{
      sqlite3ExprDelete(pCol->pDflt);
      pCol->pDflt = sqlite3ExprDup(pExpr);
    }
  }
  sqlite3ExprDelete(pExpr);
}

/*
** Designate the PRIMARY KEY for the table.  pList is a list of names 
** of columns that form the primary key.  If pList is NULL, then the
** most recently added column of the table is the primary key.
**
** A table can have at most one primary key.  If the table already has
** a primary key (and this is the second primary key) then create an
** error.
**
** If the PRIMARY KEY is on a single column whose datatype is INTEGER,
** then we will try to use that column as the rowid.  Set the Table.iPKey
** field of the table under construction to be the index of the
** INTEGER PRIMARY KEY column.  Table.iPKey is set to -1 if there is
** no INTEGER PRIMARY KEY.
**
** If the key is not an INTEGER PRIMARY KEY, then create a unique
** index for the key.  No index is created for INTEGER PRIMARY KEYs.
*/
void sqlite3AddPrimaryKey(
  Parse *pParse,    /* Parsing context */
  ExprList *pList,  /* List of field names to be indexed */
  int onError,      /* What to do with a uniqueness conflict */
  int autoInc       /* True if the AUTOINCREMENT keyword is present */
){
  Table *pTab = pParse->pNewTable;
  char *zType = 0;
  int iCol = -1, i;
  if( pTab==0 ) goto primary_key_exit;
  if( pTab->hasPrimKey ){
    sqlite3ErrorMsg(pParse, 
      "table \"%s\" has more than one primary key", pTab->zName);
    goto primary_key_exit;
  }
  pTab->hasPrimKey = 1;
  if( pList==0 ){
    iCol = pTab->nCol - 1;
    pTab->aCol[iCol].isPrimKey = 1;
  }else{
    for(i=0; i<pList->nExpr; i++){
      for(iCol=0; iCol<pTab->nCol; iCol++){
        if( sqlite3StrICmp(pList->a[i].zName, pTab->aCol[iCol].zName)==0 ){
          break;
        }
      }
      if( iCol<pTab->nCol ) pTab->aCol[iCol].isPrimKey = 1;
    }
    if( pList->nExpr>1 ) iCol = -1;
  }
  if( iCol>=0 && iCol<pTab->nCol ){
    zType = pTab->aCol[iCol].zType;
  }
  if( zType && sqlite3StrICmp(zType, "INTEGER")==0 ){
    pTab->iPKey = iCol;
    pTab->keyConf = onError;
    pTab->autoInc = autoInc;
  }else if( autoInc ){
#ifndef SQLITE_OMIT_AUTOINCREMENT
    sqlite3ErrorMsg(pParse, "AUTOINCREMENT is only allowed on an "
       "INTEGER PRIMARY KEY");
#endif
  }else{
    sqlite3CreateIndex(pParse, 0, 0, 0, pList, onError, 0, 0);
    pList = 0;
  }

primary_key_exit:
  sqlite3ExprListDelete(pList);
  return;
}

/*
** Set the collation function of the most recently parsed table column
** to the CollSeq given.
*/
void sqlite3AddCollateType(Parse *pParse, const char *zType, int nType){
  Table *p;
  Index *pIdx;
  CollSeq *pColl;
  int i;

  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;

  pColl = sqlite3LocateCollSeq(pParse, zType, nType);
  p->aCol[i].pColl = pColl;

  /* If the column is declared as "<name> PRIMARY KEY COLLATE <type>",
  ** then an index may have been created on this column before the
  ** collation type was added. Correct this if it is the case.
  */
  for(pIdx = p->pIndex; pIdx; pIdx=pIdx->pNext){
    assert( pIdx->nColumn==1 );
    if( pIdx->aiColumn[0]==i ) pIdx->keyInfo.aColl[0] = pColl;
  }
}

/*
** Call sqlite3CheckCollSeq() for all collating sequences in an index,
** in order to verify that all the necessary collating sequences are
** loaded.
*/
int sqlite3CheckIndexCollSeq(Parse *pParse, Index *pIdx){
  if( pIdx ){
    int i;
    for(i=0; i<pIdx->nColumn; i++){
      if( sqlite3CheckCollSeq(pParse, pIdx->keyInfo.aColl[i]) ){
        return SQLITE_ERROR;
      }
    }
  }
  return SQLITE_OK;
}

/*
** This function returns the collation sequence for database native text
** encoding identified by the string zName, length nName.
**
** If the requested collation sequence is not available, or not available
** in the database native encoding, the collation factory is invoked to
** request it. If the collation factory does not supply such a sequence,
** and the sequence is available in another text encoding, then that is
** returned instead.
**
** If no versions of the requested collations sequence are available, or
** another error occurs, NULL is returned and an error message written into
** pParse.
*/
CollSeq *sqlite3LocateCollSeq(Parse *pParse, const char *zName, int nName){
  sqlite3 *db = pParse->db;
  u8 enc = db->enc;
  u8 initbusy = db->init.busy;

  CollSeq *pColl = sqlite3FindCollSeq(db, enc, zName, nName, initbusy);
  if( !initbusy && (!pColl || !pColl->xCmp) ){
    pColl = sqlite3GetCollSeq(db, pColl, zName, nName);
    if( !pColl ){
      if( nName<0 ){
        nName = strlen(zName);
      }
      sqlite3ErrorMsg(pParse, "no such collation sequence: %.*s", nName, zName);
      pColl = 0;
    }
  }

  return pColl;
}


/*
** Generate code that will increment the schema cookie.
**
** The schema cookie is used to determine when the schema for the
** database changes.  After each schema change, the cookie value
** changes.  When a process first reads the schema it records the
** cookie.  Thereafter, whenever it goes to access the database,
** it checks the cookie to make sure the schema has not changed
** since it was last read.
**
** This plan is not completely bullet-proof.  It is possible for
** the schema to change multiple times and for the cookie to be
** set back to prior value.  But schema changes are infrequent
** and the probability of hitting the same cookie value is only
** 1 chance in 2^32.  So we're safe enough.
*/
void sqlite3ChangeCookie(sqlite3 *db, Vdbe *v, int iDb){
  sqlite3VdbeAddOp(v, OP_Integer, db->aDb[iDb].schema_cookie+1, 0);
  sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 0);
}

/*
** Measure the number of characters needed to output the given
** identifier.  The number returned includes any quotes used
** but does not include the null terminator.
**
** The estimate is conservative.  It might be larger that what is
** really needed.
*/
static int identLength(const char *z){
  int n;
  for(n=0; *z; n++, z++){
    if( *z=='"' ){ n++; }
  }
  return n + 2;
}

/*
** Write an identifier onto the end of the given string.  Add
** quote characters as needed.
*/
static void identPut(char *z, int *pIdx, char *zSignedIdent){
  unsigned char *zIdent = (unsigned char*)zSignedIdent;
  int i, j, needQuote;
  i = *pIdx;
  for(j=0; zIdent[j]; j++){
    if( !isalnum(zIdent[j]) && zIdent[j]!='_' ) break;
  }
  needQuote =  zIdent[j]!=0 || isdigit(zIdent[0])
                  || sqlite3KeywordCode(zIdent, j)!=TK_ID;
  if( needQuote ) z[i++] = '"';
  for(j=0; zIdent[j]; j++){
    z[i++] = zIdent[j];
    if( zIdent[j]=='"' ) z[i++] = '"';
  }
  if( needQuote ) z[i++] = '"';
  z[i] = 0;
  *pIdx = i;
}

/*
** Generate a CREATE TABLE statement appropriate for the given
** table.  Memory to hold the text of the statement is obtained
** from sqliteMalloc() and must be freed by the calling function.
*/
static char *createTableStmt(Table *p){
  int i, k, n;
  char *zStmt;
  char *zSep, *zSep2, *zEnd, *z;
  Column *pCol;
  n = 0;
  for(pCol = p->aCol, i=0; i<p->nCol; i++, pCol++){
    n += identLength(pCol->zName);
    z = pCol->zType;
    if( z ){
      n += (strlen(z) + 1);
    }
  }
  n += identLength(p->zName);
  if( n<50 ){
    zSep = "";
    zSep2 = ",";
    zEnd = ")";
  }else{
    zSep = "\n  ";
    zSep2 = ",\n  ";
    zEnd = "\n)";
  }
  n += 35 + 6*p->nCol;
  zStmt = sqliteMallocRaw( n );
  if( zStmt==0 ) return 0;
  strcpy(zStmt, !OMIT_TEMPDB&&p->iDb==1 ? "CREATE TEMP TABLE ":"CREATE TABLE ");
  k = strlen(zStmt);
  identPut(zStmt, &k, p->zName);
  zStmt[k++] = '(';
  for(pCol=p->aCol, i=0; i<p->nCol; i++, pCol++){
    strcpy(&zStmt[k], zSep);
    k += strlen(&zStmt[k]);
    zSep = zSep2;
    identPut(zStmt, &k, pCol->zName);
    if( (z = pCol->zType)!=0 ){
      zStmt[k++] = ' ';
      strcpy(&zStmt[k], z);
      k += strlen(z);
    }
  }
  strcpy(&zStmt[k], zEnd);
  return zStmt;
}

/*
** This routine is called to report the final ")" that terminates
** a CREATE TABLE statement.
**
** The table structure that other action routines have been building
** is added to the internal hash tables, assuming no errors have
** occurred.
**
** An entry for the table is made in the master table on disk, unless
** this is a temporary table or db->init.busy==1.  When db->init.busy==1
** it means we are reading the sqlite_master table because we just
** connected to the database or because the sqlite_master table has
** recently changed, so the entry for this table already exists in
** the sqlite_master table.  We do not want to create it again.
**
** If the pSelect argument is not NULL, it means that this routine
** was called to create a table generated from a 
** "CREATE TABLE ... AS SELECT ..." statement.  The column names of
** the new table will match the result set of the SELECT.
*/
void sqlite3EndTable(
  Parse *pParse,          /* Parse context */
  Token *pCons,           /* The ',' token after the last column defn. */
  Token *pEnd,            /* The final ')' token in the CREATE TABLE */
  Select *pSelect         /* Select from a "CREATE ... AS SELECT" */
){
  Table *p;
  sqlite3 *db = pParse->db;

  if( (pEnd==0 && pSelect==0) || pParse->nErr || sqlite3_malloc_failed ) return;
  p = pParse->pNewTable;
  if( p==0 ) return;

  assert( !db->init.busy || !pSelect );

  /* If the db->init.busy is 1 it means we are reading the SQL off the
  ** "sqlite_master" or "sqlite_temp_master" table on the disk.
  ** So do not write to the disk again.  Extract the root page number
  ** for the table from the db->init.newTnum field.  (The page number
  ** should have been put there by the sqliteOpenCb routine.)
  */
  if( db->init.busy ){
    p->tnum = db->init.newTnum;
  }

  /* If not initializing, then create a record for the new table
  ** in the SQLITE_MASTER table of the database.  The record number
  ** for the new table entry should already be on the stack.
  **
  ** If this is a TEMPORARY table, write the entry into the auxiliary
  ** file instead of into the main database file.
  */
  if( !db->init.busy ){
    int n;
    Vdbe *v;
    char *zType;    /* "view" or "table" */
    char *zType2;   /* "VIEW" or "TABLE" */
    char *zStmt;    /* Text of the CREATE TABLE or CREATE VIEW statement */

    v = sqlite3GetVdbe(pParse);
    if( v==0 ) return;

    sqlite3VdbeAddOp(v, OP_Close, 0, 0);

    /* Create the rootpage for the new table and push it onto the stack.
    ** A view has no rootpage, so just push a zero onto the stack for
    ** views.  Initialize zType at the same time.
    */
    if( p->pSelect==0 ){
      /* A regular table */
      zType = "table";
      zType2 = "TABLE";
#ifndef SQLITE_OMIT_VIEW
    }else{
      /* A view */
      zType = "view";
      zType2 = "VIEW";
#endif
    }

    /* If this is a CREATE TABLE xx AS SELECT ..., execute the SELECT
    ** statement to populate the new table. The root-page number for the
    ** new table is on the top of the vdbe stack.
    **
    ** Once the SELECT has been coded by sqlite3Select(), it is in a
    ** suitable state to query for the column names and types to be used
    ** by the new table.
    */
    if( pSelect ){
      Table *pSelTab;
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3VdbeAddOp(v, OP_Integer, p->iDb, 0);
      sqlite3VdbeAddOp(v, OP_OpenWrite, 1, 0);
      pParse->nTab = 2;
      sqlite3Select(pParse, pSelect, SRT_Table, 1, 0, 0, 0, 0);
      sqlite3VdbeAddOp(v, OP_Close, 1, 0);
      if( pParse->nErr==0 ){
        pSelTab = sqlite3ResultSetOfSelect(pParse, 0, pSelect);
        if( pSelTab==0 ) return;
        assert( p->aCol==0 );
        p->nCol = pSelTab->nCol;
        p->aCol = pSelTab->aCol;
        pSelTab->nCol = 0;
        pSelTab->aCol = 0;
        sqlite3DeleteTable(0, pSelTab);
      }
    }

    /* Compute the complete text of the CREATE statement */
    if( pSelect ){
      zStmt = createTableStmt(p);
    }else{
      n = pEnd->z - pParse->sNameToken.z + 1;
      zStmt = sqlite3MPrintf("CREATE %s %.*s", zType2, n, pParse->sNameToken.z);
    }

    /* A slot for the record has already been allocated in the 
    ** SQLITE_MASTER table.  We just need to update that slot with all
    ** the information we've collected.  The rowid for the preallocated
    ** slot is the 2nd item on the stack.  The top of the stack is the
    ** root page for the new table (or a 0 if this is a view).
    */
    sqlite3NestedParse(pParse,
      "UPDATE %Q.%s "
         "SET type='%s', name=%Q, tbl_name=%Q, rootpage=#0, sql=%Q "
       "WHERE rowid=#1",
      db->aDb[p->iDb].zName, SCHEMA_TABLE(p->iDb),
      zType,
      p->zName,
      p->zName,
      zStmt
    );
    sqliteFree(zStmt);
    sqlite3ChangeCookie(db, v, p->iDb);

#ifndef SQLITE_OMIT_AUTOINCREMENT
    /* Check to see if we need to create an sqlite_sequence table for
    ** keeping track of autoincrement keys.
    */
    if( p->autoInc ){
      Db *pDb = &db->aDb[p->iDb];
      if( pDb->pSeqTab==0 ){
        sqlite3NestedParse(pParse,
          "CREATE TABLE %Q.sqlite_sequence(name,seq)",
          pDb->zName
        );
      }
    }
#endif

    /* Reparse everything to update our internal data structures */
    sqlite3VdbeOp3(v, OP_ParseSchema, p->iDb, 0,
        sqlite3MPrintf("tbl_name='%q'",p->zName), P3_DYNAMIC);
  }


  /* Add the table to the in-memory representation of the database.
  */
  if( db->init.busy && pParse->nErr==0 ){
    Table *pOld;
    FKey *pFKey; 
    Db *pDb = &db->aDb[p->iDb];
    pOld = sqlite3HashInsert(&pDb->tblHash, p->zName, strlen(p->zName)+1, p);
    if( pOld ){
      assert( p==pOld );  /* Malloc must have failed inside HashInsert() */
      return;
    }
#ifndef SQLITE_OMIT_FOREIGN_KEY
    for(pFKey=p->pFKey; pFKey; pFKey=pFKey->pNextFrom){
      int nTo = strlen(pFKey->zTo) + 1;
      pFKey->pNextTo = sqlite3HashFind(&pDb->aFKey, pFKey->zTo, nTo);
      sqlite3HashInsert(&pDb->aFKey, pFKey->zTo, nTo, pFKey);
    }
#endif
    pParse->pNewTable = 0;
    db->nTable++;
    db->flags |= SQLITE_InternChanges;

#ifndef SQLITE_OMIT_ALTERTABLE
    if( !p->pSelect ){
      assert( !pSelect && pCons && pEnd );
      if( pCons->z==0 ) pCons = pEnd;
      p->addColOffset = 13 + (pCons->z - pParse->sNameToken.z);
    }
#endif
  }
}

#ifndef SQLITE_OMIT_VIEW
/*
** The parser calls this routine in order to create a new VIEW
*/
void sqlite3CreateView(
  Parse *pParse,     /* The parsing context */
  Token *pBegin,     /* The CREATE token that begins the statement */
  Token *pName1,     /* The token that holds the name of the view */
  Token *pName2,     /* The token that holds the name of the view */
  Select *pSelect,   /* A SELECT statement that will become the new view */
  int isTemp         /* TRUE for a TEMPORARY view */
){
  Table *p;
  int n;
  const unsigned char *z;
  Token sEnd;
  DbFixer sFix;
  Token *pName;

  if( pParse->nVar>0 ){
    sqlite3ErrorMsg(pParse, "parameters are not allowed in views");
    sqlite3SelectDelete(pSelect);
    return;
  }
  sqlite3StartTable(pParse, pBegin, pName1, pName2, isTemp, 1);
  p = pParse->pNewTable;
  if( p==0 || pParse->nErr ){
    sqlite3SelectDelete(pSelect);
    return;
  }
  sqlite3TwoPartName(pParse, pName1, pName2, &pName);
  if( sqlite3FixInit(&sFix, pParse, p->iDb, "view", pName)
    && sqlite3FixSelect(&sFix, pSelect)
  ){
    sqlite3SelectDelete(pSelect);
    return;
  }

  /* Make a copy of the entire SELECT statement that defines the view.
  ** This will force all the Expr.token.z values to be dynamically
  ** allocated rather than point to the input string - which means that
  ** they will persist after the current sqlite3_exec() call returns.
  */
  p->pSelect = sqlite3SelectDup(pSelect);
  sqlite3SelectDelete(pSelect);
  if( !pParse->db->init.busy ){
    sqlite3ViewGetColumnNames(pParse, p);
  }

  /* Locate the end of the CREATE VIEW statement.  Make sEnd point to
  ** the end.
  */
  sEnd = pParse->sLastToken;
  if( sEnd.z[0]!=0 && sEnd.z[0]!=';' ){
    sEnd.z += sEnd.n;
  }
  sEnd.n = 0;
  n = sEnd.z - pBegin->z;
  z = (const unsigned char*)pBegin->z;
  while( n>0 && (z[n-1]==';' || isspace(z[n-1])) ){ n--; }
  sEnd.z = &z[n-1];
  sEnd.n = 1;

  /* Use sqlite3EndTable() to add the view to the SQLITE_MASTER table */
  sqlite3EndTable(pParse, 0, &sEnd, 0);
  return;
}
#endif /* SQLITE_OMIT_VIEW */

#ifndef SQLITE_OMIT_VIEW
/*
** The Table structure pTable is really a VIEW.  Fill in the names of
** the columns of the view in the pTable structure.  Return the number
** of errors.  If an error is seen leave an error message in pParse->zErrMsg.
*/
int sqlite3ViewGetColumnNames(Parse *pParse, Table *pTable){
  Table *pSelTab;   /* A fake table from which we get the result set */
  Select *pSel;     /* Copy of the SELECT that implements the view */
  int nErr = 0;     /* Number of errors encountered */
  int n;            /* Temporarily holds the number of cursors assigned */

  assert( pTable );

  /* A positive nCol means the columns names for this view are
  ** already known.
  */
  if( pTable->nCol>0 ) return 0;

  /* A negative nCol is a special marker meaning that we are currently
  ** trying to compute the column names.  If we enter this routine with
  ** a negative nCol, it means two or more views form a loop, like this:
  **
  **     CREATE VIEW one AS SELECT * FROM two;
  **     CREATE VIEW two AS SELECT * FROM one;
  **
  ** Actually, this error is caught previously and so the following test
  ** should always fail.  But we will leave it in place just to be safe.
  */
#if 0
  if( pTable->nCol<0 ){
    sqlite3ErrorMsg(pParse, "view %s is circularly defined", pTable->zName);
    return 1;
  }
#endif
  assert( pTable->nCol>=0 );

  /* If we get this far, it means we need to compute the table names.
  ** Note that the call to sqlite3ResultSetOfSelect() will expand any
  ** "*" elements in the results set of the view and will assign cursors
  ** to the elements of the FROM clause.  But we do not want these changes
  ** to be permanent.  So the computation is done on a copy of the SELECT
  ** statement that defines the view.
  */
  assert( pTable->pSelect );
  pSel = sqlite3SelectDup(pTable->pSelect);
  n = pParse->nTab;
  sqlite3SrcListAssignCursors(pParse, pSel->pSrc);
  pTable->nCol = -1;
  pSelTab = sqlite3ResultSetOfSelect(pParse, 0, pSel);
  pParse->nTab = n;
  if( pSelTab ){
    assert( pTable->aCol==0 );
    pTable->nCol = pSelTab->nCol;
    pTable->aCol = pSelTab->aCol;
    pSelTab->nCol = 0;
    pSelTab->aCol = 0;
    sqlite3DeleteTable(0, pSelTab);
    DbSetProperty(pParse->db, pTable->iDb, DB_UnresetViews);
  }else{
    pTable->nCol = 0;
    nErr++;
  }
  sqlite3SelectDelete(pSel);
  return nErr;  
}
#endif /* SQLITE_OMIT_VIEW */

#ifndef SQLITE_OMIT_VIEW
/*
** Clear the column names from every VIEW in database idx.
*/
static void sqliteViewResetAll(sqlite3 *db, int idx){
  HashElem *i;
  if( !DbHasProperty(db, idx, DB_UnresetViews) ) return;
  for(i=sqliteHashFirst(&db->aDb[idx].tblHash); i; i=sqliteHashNext(i)){
    Table *pTab = sqliteHashData(i);
    if( pTab->pSelect ){
      sqliteResetColumnNames(pTab);
    }
  }
  DbClearProperty(db, idx, DB_UnresetViews);
}
#else
# define sqliteViewResetAll(A,B)
#endif /* SQLITE_OMIT_VIEW */

/*
** This function is called by the VDBE to adjust the internal schema
** used by SQLite when the btree layer moves a table root page. The
** root-page of a table or index in database iDb has changed from iFrom
** to iTo.
*/
#ifndef SQLITE_OMIT_AUTOVACUUM
void sqlite3RootPageMoved(Db *pDb, int iFrom, int iTo){
  HashElem *pElem;
  
  for(pElem=sqliteHashFirst(&pDb->tblHash); pElem; pElem=sqliteHashNext(pElem)){
    Table *pTab = sqliteHashData(pElem);
    if( pTab->tnum==iFrom ){
      pTab->tnum = iTo;
      return;
    }
  }
  for(pElem=sqliteHashFirst(&pDb->idxHash); pElem; pElem=sqliteHashNext(pElem)){
    Index *pIdx = sqliteHashData(pElem);
    if( pIdx->tnum==iFrom ){
      pIdx->tnum = iTo;
      return;
    }
  }
  assert(0);
}
#endif

/*
** Write code to erase the table with root-page iTable from database iDb.
** Also write code to modify the sqlite_master table and internal schema
** if a root-page of another table is moved by the btree-layer whilst
** erasing iTable (this can happen with an auto-vacuum database).
*/ 
static void destroyRootPage(Parse *pParse, int iTable, int iDb){
  Vdbe *v = sqlite3GetVdbe(pParse);
  sqlite3VdbeAddOp(v, OP_Destroy, iTable, iDb);
#ifndef SQLITE_OMIT_AUTOVACUUM
  /* OP_Destroy pushes an integer onto the stack. If this integer
  ** is non-zero, then it is the root page number of a table moved to
  ** location iTable. The following code modifies the sqlite_master table to
  ** reflect this.
  **
  ** The "#0" in the SQL is a special constant that means whatever value
  ** is on the top of the stack.  See sqlite3RegisterExpr().
  */
  sqlite3NestedParse(pParse, 
     "UPDATE %Q.%s SET rootpage=%d WHERE #0 AND rootpage=#0",
     pParse->db->aDb[iDb].zName, SCHEMA_TABLE(iDb), iTable);
#endif
}

/*
** Write VDBE code to erase table pTab and all associated indices on disk.
** Code to update the sqlite_master tables and internal schema definitions
** in case a root-page belonging to another table is moved by the btree layer
** is also added (this can happen with an auto-vacuum database).
*/
static void destroyTable(Parse *pParse, Table *pTab){
#ifdef SQLITE_OMIT_AUTOVACUUM
  Index *pIdx;
  destroyRootPage(pParse, pTab->tnum, pTab->iDb);
  for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
    destroyRootPage(pParse, pIdx->tnum, pIdx->iDb);
  }
#else
  /* If the database may be auto-vacuum capable (if SQLITE_OMIT_AUTOVACUUM
  ** is not defined), then it is important to call OP_Destroy on the
  ** table and index root-pages in order, starting with the numerically 
  ** largest root-page number. This guarantees that none of the root-pages
  ** to be destroyed is relocated by an earlier OP_Destroy. i.e. if the
  ** following were coded:
  **
  ** OP_Destroy 4 0
  ** ...
  ** OP_Destroy 5 0
  **
  ** and root page 5 happened to be the largest root-page number in the
  ** database, then root page 5 would be moved to page 4 by the 
  ** "OP_Destroy 4 0" opcode. The subsequent "OP_Destroy 5 0" would hit
  ** a free-list page.
  */
  int iTab = pTab->tnum;
  int iDestroyed = 0;

  while( 1 ){
    Index *pIdx;
    int iLargest = 0;

    if( iDestroyed==0 || iTab<iDestroyed ){
      iLargest = iTab;
    }
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      int iIdx = pIdx->tnum;
      assert( pIdx->iDb==pTab->iDb );
      if( (iDestroyed==0 || (iIdx<iDestroyed)) && iIdx>iLargest ){
        iLargest = iIdx;
      }
    }
    if( iLargest==0 ) return;
    destroyRootPage(pParse, iLargest, pTab->iDb);
    iDestroyed = iLargest;
  }
#endif
}

/*
** This routine is called to do the work of a DROP TABLE statement.
** pName is the name of the table to be dropped.
*/
void sqlite3DropTable(Parse *pParse, SrcList *pName, int isView){
  Table *pTab;
  Vdbe *v;
  sqlite3 *db = pParse->db;
  int iDb;

  if( pParse->nErr || sqlite3_malloc_failed ) goto exit_drop_table;
  assert( pName->nSrc==1 );
  pTab = sqlite3LocateTable(pParse, pName->a[0].zName, pName->a[0].zDatabase);

  if( pTab==0 ) goto exit_drop_table;
  iDb = pTab->iDb;
  assert( iDb>=0 && iDb<db->nDb );
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code;
    const char *zTab = SCHEMA_TABLE(pTab->iDb);
    const char *zDb = db->aDb[pTab->iDb].zName;
    if( sqlite3AuthCheck(pParse, SQLITE_DELETE, zTab, 0, zDb)){
      goto exit_drop_table;
    }
    if( isView ){
      if( !OMIT_TEMPDB && iDb==1 ){
        code = SQLITE_DROP_TEMP_VIEW;
      }else{
        code = SQLITE_DROP_VIEW;
      }
    }else{
      if( !OMIT_TEMPDB && iDb==1 ){
        code = SQLITE_DROP_TEMP_TABLE;
      }else{
        code = SQLITE_DROP_TABLE;
      }
    }
    if( sqlite3AuthCheck(pParse, code, pTab->zName, 0, zDb) ){
      goto exit_drop_table;
    }
    if( sqlite3AuthCheck(pParse, SQLITE_DELETE, pTab->zName, 0, zDb) ){
      goto exit_drop_table;
    }
  }
#endif
  if( pTab->readOnly || pTab==db->aDb[iDb].pSeqTab ){
    sqlite3ErrorMsg(pParse, "table %s may not be dropped", pTab->zName);
    goto exit_drop_table;
  }

#ifndef SQLITE_OMIT_VIEW
  /* Ensure DROP TABLE is not used on a view, and DROP VIEW is not used
  ** on a table.
  */
  if( isView && pTab->pSelect==0 ){
    sqlite3ErrorMsg(pParse, "use DROP TABLE to delete table %s", pTab->zName);
    goto exit_drop_table;
  }
  if( !isView && pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "use DROP VIEW to delete view %s", pTab->zName);
    goto exit_drop_table;
  }
#endif

  /* Generate code to remove the table from the master table
  ** on disk.
  */
  v = sqlite3GetVdbe(pParse);
  if( v ){
    Trigger *pTrigger;
    int iDb = pTab->iDb;
    Db *pDb = &db->aDb[iDb];
    sqlite3BeginWriteOperation(pParse, 0, iDb);

    /* Drop all triggers associated with the table being dropped. Code
    ** is generated to remove entries from sqlite_master and/or
    ** sqlite_temp_master if required.
    */
    pTrigger = pTab->pTrigger;
    while( pTrigger ){
      assert( pTrigger->iDb==iDb || pTrigger->iDb==1 );
      sqlite3DropTriggerPtr(pParse, pTrigger, 1);
      pTrigger = pTrigger->pNext;
    }

#ifndef SQLITE_OMIT_AUTOINCREMENT
    /* Remove any entries of the sqlite_sequence table associated with
    ** the table being dropped. This is done before the table is dropped
    ** at the btree level, in case the sqlite_sequence table needs to
    ** move as a result of the drop (can happen in auto-vacuum mode).
    */
    if( pTab->autoInc ){
      sqlite3NestedParse(pParse,
        "DELETE FROM %s.sqlite_sequence WHERE name=%Q",
        pDb->zName, pTab->zName
      );
    }
#endif

    /* Drop all SQLITE_MASTER table and index entries that refer to the
    ** table. The program name loops through the master table and deletes
    ** every row that refers to a table of the same name as the one being
    ** dropped. Triggers are handled seperately because a trigger can be
    ** created in the temp database that refers to a table in another
    ** database.
    */
    sqlite3NestedParse(pParse, 
        "DELETE FROM %Q.%s WHERE tbl_name=%Q and type!='trigger'",
        pDb->zName, SCHEMA_TABLE(iDb), pTab->zName);
    if( !isView ){
      destroyTable(pParse, pTab);
    }

    /* Remove the table entry from SQLite's internal schema and modify
    ** the schema cookie.
    */
    sqlite3VdbeOp3(v, OP_DropTable, iDb, 0, pTab->zName, 0);
    sqlite3ChangeCookie(db, v, iDb);
  }
  sqliteViewResetAll(db, iDb);

exit_drop_table:
  sqlite3SrcListDelete(pName);
}

/*
** This routine is called to create a new foreign key on the table
** currently under construction.  pFromCol determines which columns
** in the current table point to the foreign key.  If pFromCol==0 then
** connect the key to the last column inserted.  pTo is the name of
** the table referred to.  pToCol is a list of tables in the other
** pTo table that the foreign key points to.  flags contains all
** information about the conflict resolution algorithms specified
** in the ON DELETE, ON UPDATE and ON INSERT clauses.
**
** An FKey structure is created and added to the table currently
** under construction in the pParse->pNewTable field.  The new FKey
** is not linked into db->aFKey at this point - that does not happen
** until sqlite3EndTable().
**
** The foreign key is set for IMMEDIATE processing.  A subsequent call
** to sqlite3DeferForeignKey() might change this to DEFERRED.
*/
void sqlite3CreateForeignKey(
  Parse *pParse,       /* Parsing context */
  ExprList *pFromCol,  /* Columns in this table that point to other table */
  Token *pTo,          /* Name of the other table */
  ExprList *pToCol,    /* Columns in the other table */
  int flags            /* Conflict resolution algorithms. */
){
#ifndef SQLITE_OMIT_FOREIGN_KEY
  FKey *pFKey = 0;
  Table *p = pParse->pNewTable;
  int nByte;
  int i;
  int nCol;
  char *z;

  assert( pTo!=0 );
  if( p==0 || pParse->nErr ) goto fk_end;
  if( pFromCol==0 ){
    int iCol = p->nCol-1;
    if( iCol<0 ) goto fk_end;
    if( pToCol && pToCol->nExpr!=1 ){
      sqlite3ErrorMsg(pParse, "foreign key on %s"
         " should reference only one column of table %T",
         p->aCol[iCol].zName, pTo);
      goto fk_end;
    }
    nCol = 1;
  }else if( pToCol && pToCol->nExpr!=pFromCol->nExpr ){
    sqlite3ErrorMsg(pParse,
        "number of columns in foreign key does not match the number of "
        "columns in the referenced table");
    goto fk_end;
  }else{
    nCol = pFromCol->nExpr;
  }
  nByte = sizeof(*pFKey) + nCol*sizeof(pFKey->aCol[0]) + pTo->n + 1;
  if( pToCol ){
    for(i=0; i<pToCol->nExpr; i++){
      nByte += strlen(pToCol->a[i].zName) + 1;
    }
  }
  pFKey = sqliteMalloc( nByte );
  if( pFKey==0 ) goto fk_end;
  pFKey->pFrom = p;
  pFKey->pNextFrom = p->pFKey;
  z = (char*)&pFKey[1];
  pFKey->aCol = (struct sColMap*)z;
  z += sizeof(struct sColMap)*nCol;
  pFKey->zTo = z;
  memcpy(z, pTo->z, pTo->n);
  z[pTo->n] = 0;
  z += pTo->n+1;
  pFKey->pNextTo = 0;
  pFKey->nCol = nCol;
  if( pFromCol==0 ){
    pFKey->aCol[0].iFrom = p->nCol-1;
  }else{
    for(i=0; i<nCol; i++){
      int j;
      for(j=0; j<p->nCol; j++){
        if( sqlite3StrICmp(p->aCol[j].zName, pFromCol->a[i].zName)==0 ){
          pFKey->aCol[i].iFrom = j;
          break;
        }
      }
      if( j>=p->nCol ){
        sqlite3ErrorMsg(pParse, 
          "unknown column \"%s\" in foreign key definition", 
          pFromCol->a[i].zName);
        goto fk_end;
      }
    }
  }
  if( pToCol ){
    for(i=0; i<nCol; i++){
      int n = strlen(pToCol->a[i].zName);
      pFKey->aCol[i].zCol = z;
      memcpy(z, pToCol->a[i].zName, n);
      z[n] = 0;
      z += n+1;
    }
  }
  pFKey->isDeferred = 0;
  pFKey->deleteConf = flags & 0xff;
  pFKey->updateConf = (flags >> 8 ) & 0xff;
  pFKey->insertConf = (flags >> 16 ) & 0xff;

  /* Link the foreign key to the table as the last step.
  */
  p->pFKey = pFKey;
  pFKey = 0;

fk_end:
  sqliteFree(pFKey);
#endif /* !defined(SQLITE_OMIT_FOREIGN_KEY) */
  sqlite3ExprListDelete(pFromCol);
  sqlite3ExprListDelete(pToCol);
}

/*
** This routine is called when an INITIALLY IMMEDIATE or INITIALLY DEFERRED
** clause is seen as part of a foreign key definition.  The isDeferred
** parameter is 1 for INITIALLY DEFERRED and 0 for INITIALLY IMMEDIATE.
** The behavior of the most recently created foreign key is adjusted
** accordingly.
*/
void sqlite3DeferForeignKey(Parse *pParse, int isDeferred){
#ifndef SQLITE_OMIT_FOREIGN_KEY
  Table *pTab;
  FKey *pFKey;
  if( (pTab = pParse->pNewTable)==0 || (pFKey = pTab->pFKey)==0 ) return;
  pFKey->isDeferred = isDeferred;
#endif
}

/*
** Generate code that will erase and refill index *pIdx.  This is
** used to initialize a newly created index or to recompute the
** content of an index in response to a REINDEX command.
**
** if memRootPage is not negative, it means that the index is newly
** created.  The memory cell specified by memRootPage contains the
** root page number of the index.  If memRootPage is negative, then
** the index already exists and must be cleared before being refilled and
** the root page number of the index is taken from pIndex->tnum.
*/
static void sqlite3RefillIndex(Parse *pParse, Index *pIndex, int memRootPage){
  Table *pTab = pIndex->pTable;  /* The table that is indexed */
  int iTab = pParse->nTab;       /* Btree cursor used for pTab */
  int iIdx = pParse->nTab+1;     /* Btree cursor used for pIndex */
  int addr1;                     /* Address of top of loop */
  int tnum;                      /* Root page of index */
  Vdbe *v;                       /* Generate code into this virtual machine */

#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqlite3AuthCheck(pParse, SQLITE_REINDEX, pIndex->zName, 0,
      pParse->db->aDb[pIndex->iDb].zName ) ){
    return;
  }
#endif

  /* Ensure all the required collation sequences are available. This
  ** routine will invoke the collation-needed callback if necessary (and
  ** if one has been registered).
  */
  if( sqlite3CheckIndexCollSeq(pParse, pIndex) ){
    return;
  }

  v = sqlite3GetVdbe(pParse);
  if( v==0 ) return;
  if( memRootPage>=0 ){
    sqlite3VdbeAddOp(v, OP_MemLoad, memRootPage, 0);
    tnum = 0;
  }else{
    tnum = pIndex->tnum;
    sqlite3VdbeAddOp(v, OP_Clear, tnum, pIndex->iDb);
  }
  sqlite3VdbeAddOp(v, OP_Integer, pIndex->iDb, 0);
  sqlite3VdbeOp3(v, OP_OpenWrite, iIdx, tnum,
                    (char*)&pIndex->keyInfo, P3_KEYINFO);
  sqlite3OpenTableForReading(v, iTab, pTab);
  addr1 = sqlite3VdbeAddOp(v, OP_Rewind, iTab, 0);
  sqlite3GenerateIndexKey(v, pIndex, iTab);
  if( pIndex->onError!=OE_None ){
    int curaddr = sqlite3VdbeCurrentAddr(v);
    int addr2 = curaddr+4;
    sqlite3VdbeChangeP2(v, curaddr-1, addr2);
    sqlite3VdbeAddOp(v, OP_Rowid, iTab, 0);
    sqlite3VdbeAddOp(v, OP_AddImm, 1, 0);
    sqlite3VdbeAddOp(v, OP_IsUnique, iIdx, addr2);
    sqlite3VdbeOp3(v, OP_Halt, SQLITE_CONSTRAINT, OE_Abort,
                    "indexed columns are not unique", P3_STATIC);
    assert( addr2==sqlite3VdbeCurrentAddr(v) );
  }
  sqlite3VdbeAddOp(v, OP_IdxInsert, iIdx, 0);
  sqlite3VdbeAddOp(v, OP_Next, iTab, addr1+1);
  sqlite3VdbeChangeP2(v, addr1, sqlite3VdbeCurrentAddr(v));
  sqlite3VdbeAddOp(v, OP_Close, iTab, 0);
  sqlite3VdbeAddOp(v, OP_Close, iIdx, 0);
}

/*
** Create a new index for an SQL table.  pName1.pName2 is the name of the index 
** and pTblList is the name of the table that is to be indexed.  Both will 
** be NULL for a primary key or an index that is created to satisfy a
** UNIQUE constraint.  If pTable and pIndex are NULL, use pParse->pNewTable
** as the table to be indexed.  pParse->pNewTable is a table that is
** currently being constructed by a CREATE TABLE statement.
**
** pList is a list of columns to be indexed.  pList will be NULL if this
** is a primary key or unique-constraint on the most recent column added
** to the table currently under construction.  
*/
void sqlite3CreateIndex(
  Parse *pParse,     /* All information about this parse */
  Token *pName1,     /* First part of index name. May be NULL */
  Token *pName2,     /* Second part of index name. May be NULL */
  SrcList *pTblName, /* Table to index. Use pParse->pNewTable if 0 */
  ExprList *pList,   /* A list of columns to be indexed */
  int onError,       /* OE_Abort, OE_Ignore, OE_Replace, or OE_None */
  Token *pStart,     /* The CREATE token that begins a CREATE TABLE statement */
  Token *pEnd        /* The ")" that closes the CREATE INDEX statement */
){
  Table *pTab = 0;   /* Table to be indexed */
  Index *pIndex = 0; /* The index to be created */
  char *zName = 0;
  int i, j;
  Token nullId;    /* Fake token for an empty ID list */
  DbFixer sFix;    /* For assigning database names to pTable */
  sqlite3 *db = pParse->db;

  int iDb;          /* Index of the database that is being written */
  Token *pName = 0; /* Unqualified name of the index to create */

  if( pParse->nErr || sqlite3_malloc_failed ) goto exit_create_index;

  /*
  ** Find the table that is to be indexed.  Return early if not found.
  */
  if( pTblName!=0 ){

    /* Use the two-part index name to determine the database 
    ** to search for the table. 'Fix' the table name to this db
    ** before looking up the table.
    */
    assert( pName1 && pName2 );
    iDb = sqlite3TwoPartName(pParse, pName1, pName2, &pName);
    if( iDb<0 ) goto exit_create_index;

#ifndef SQLITE_OMIT_TEMPDB
    /* If the index name was unqualified, check if the the table
    ** is a temp table. If so, set the database to 1.
    */
    pTab = sqlite3SrcListLookup(pParse, pTblName);
    if( pName2 && pName2->n==0 && pTab && pTab->iDb==1 ){
      iDb = 1;
    }
#endif

    if( sqlite3FixInit(&sFix, pParse, iDb, "index", pName) &&
        sqlite3FixSrcList(&sFix, pTblName)
    ){
      /* Because the parser constructs pTblName from a single identifier,
      ** sqlite3FixSrcList can never fail. */
      assert(0);
    }
    pTab = sqlite3LocateTable(pParse, pTblName->a[0].zName, 
        pTblName->a[0].zDatabase);
    if( !pTab ) goto exit_create_index;
    assert( iDb==pTab->iDb );
  }else{
    assert( pName==0 );
    pTab =  pParse->pNewTable;
    iDb = pTab->iDb;
  }

  if( pTab==0 || pParse->nErr ) goto exit_create_index;
  if( pTab->readOnly ){
    sqlite3ErrorMsg(pParse, "table %s may not be indexed", pTab->zName);
    goto exit_create_index;
  }
#ifndef SQLITE_OMIT_VIEW
  if( pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "views may not be indexed");
    goto exit_create_index;
  }
#endif

  /*
  ** Find the name of the index.  Make sure there is not already another
  ** index or table with the same name.  
  **
  ** Exception:  If we are reading the names of permanent indices from the
  ** sqlite_master table (because some other process changed the schema) and
  ** one of the index names collides with the name of a temporary table or
  ** index, then we will continue to process this index.
  **
  ** If pName==0 it means that we are
  ** dealing with a primary key or UNIQUE constraint.  We have to invent our
  ** own name.
  */
  if( pName ){
    zName = sqlite3NameFromToken(pName);
    if( SQLITE_OK!=sqlite3ReadSchema(pParse) ) goto exit_create_index;
    if( zName==0 ) goto exit_create_index;
    if( SQLITE_OK!=sqlite3CheckObjectName(pParse, zName) ){
      goto exit_create_index;
    }
    if( !db->init.busy ){
      Index *pISameName;    /* Another index with the same name */
      Table *pTSameName;    /* A table with same name as the index */
      if( SQLITE_OK!=sqlite3ReadSchema(pParse) ) goto exit_create_index;
      if( (pISameName = sqlite3FindIndex(db, zName, db->aDb[iDb].zName))!=0 ){
        sqlite3ErrorMsg(pParse, "index %s already exists", zName);
        goto exit_create_index;
      }
      if( (pTSameName = sqlite3FindTable(db, zName, 0))!=0 ){
        sqlite3ErrorMsg(pParse, "there is already a table named %s", zName);
        goto exit_create_index;
      }
    }
  }else{
    char zBuf[30];
    int n;
    Index *pLoop;
    for(pLoop=pTab->pIndex, n=1; pLoop; pLoop=pLoop->pNext, n++){}
    sprintf(zBuf,"_%d",n);
    zName = 0;
    sqlite3SetString(&zName, "sqlite_autoindex_", pTab->zName, zBuf, (char*)0);
    if( zName==0 ) goto exit_create_index;
  }

  /* Check for authorization to create an index.
  */
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    const char *zDb = db->aDb[iDb].zName;
    if( sqlite3AuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(iDb), 0, zDb) ){
      goto exit_create_index;
    }
    i = SQLITE_CREATE_INDEX;
    if( !OMIT_TEMPDB && iDb==1 ) i = SQLITE_CREATE_TEMP_INDEX;
    if( sqlite3AuthCheck(pParse, i, zName, pTab->zName, zDb) ){
      goto exit_create_index;
    }
  }
#endif

  /* If pList==0, it means this routine was called to make a primary
  ** key out of the last column added to the table under construction.
  ** So create a fake list to simulate this.
  */
  if( pList==0 ){
    nullId.z = pTab->aCol[pTab->nCol-1].zName;
    nullId.n = strlen(nullId.z);
    pList = sqlite3ExprListAppend(0, 0, &nullId);
    if( pList==0 ) goto exit_create_index;
  }

  /* 
  ** Allocate the index structure. 
  */
  pIndex = sqliteMalloc( sizeof(Index) + strlen(zName) + 1 + sizeof(int) +
                        (sizeof(int)*2 + sizeof(CollSeq*))*pList->nExpr );
  if( sqlite3_malloc_failed ) goto exit_create_index;
  pIndex->aiColumn = (int*)&pIndex->keyInfo.aColl[pList->nExpr];
  pIndex->aiRowEst = &pIndex->aiColumn[pList->nExpr];
  pIndex->zName = (char*)&pIndex->aiRowEst[pList->nExpr+1];
  strcpy(pIndex->zName, zName);
  pIndex->pTable = pTab;
  pIndex->nColumn = pList->nExpr;
  pIndex->onError = onError;
  pIndex->autoIndex = pName==0;
  pIndex->iDb = iDb;

  /* Scan the names of the columns of the table to be indexed and
  ** load the column indices into the Index structure.  Report an error
  ** if any column is not found.
  */
  for(i=0; i<pList->nExpr; i++){
    for(j=0; j<pTab->nCol; j++){
      if( sqlite3StrICmp(pList->a[i].zName, pTab->aCol[j].zName)==0 ) break;
    }
    if( j>=pTab->nCol ){
      sqlite3ErrorMsg(pParse, "table %s has no column named %s",
        pTab->zName, pList->a[i].zName);
      goto exit_create_index;
    }
    pIndex->aiColumn[i] = j;
    if( pList->a[i].pExpr ){
      assert( pList->a[i].pExpr->pColl );
      pIndex->keyInfo.aColl[i] = pList->a[i].pExpr->pColl;
    }else{
      pIndex->keyInfo.aColl[i] = pTab->aCol[j].pColl;
    }
    assert( pIndex->keyInfo.aColl[i] );
    if( !db->init.busy && 
        sqlite3CheckCollSeq(pParse, pIndex->keyInfo.aColl[i]) 
    ){
      goto exit_create_index;
    }
  }
  pIndex->keyInfo.nField = pList->nExpr;
  sqlite3DefaultRowEst(pIndex);

  if( pTab==pParse->pNewTable ){
    /* This routine has been called to create an automatic index as a
    ** result of a PRIMARY KEY or UNIQUE clause on a column definition, or
    ** a PRIMARY KEY or UNIQUE clause following the column definitions.
    ** i.e. one of:
    **
    ** CREATE TABLE t(x PRIMARY KEY, y);
    ** CREATE TABLE t(x, y, UNIQUE(x, y));
    **
    ** Either way, check to see if the table already has such an index. If
    ** so, don't bother creating this one. This only applies to
    ** automatically created indices. Users can do as they wish with
    ** explicit indices.
    */
    Index *pIdx;
    for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
      int k;
      assert( pIdx->onError!=OE_None );
      assert( pIdx->autoIndex );
      assert( pIndex->onError!=OE_None );

      if( pIdx->nColumn!=pIndex->nColumn ) continue;
      for(k=0; k<pIdx->nColumn; k++){
        if( pIdx->aiColumn[k]!=pIndex->aiColumn[k] ) break;
        if( pIdx->keyInfo.aColl[k]!=pIndex->keyInfo.aColl[k] ) break;
      }
      if( k==pIdx->nColumn ){
        if( pIdx->onError!=pIndex->onError ){
          /* This constraint creates the same index as a previous
          ** constraint specified somewhere in the CREATE TABLE statement.
          ** However the ON CONFLICT clauses are different. If both this 
          ** constraint and the previous equivalent constraint have explicit
          ** ON CONFLICT clauses this is an error. Otherwise, use the
          ** explicitly specified behaviour for the index.
          */
          if( !(pIdx->onError==OE_Default || pIndex->onError==OE_Default) ){
            sqlite3ErrorMsg(pParse, 
                "conflicting ON CONFLICT clauses specified", 0);
          }
          if( pIdx->onError==OE_Default ){
            pIdx->onError = pIndex->onError;
          }
        }
        goto exit_create_index;
      }
    }
  }

  /* Link the new Index structure to its table and to the other
  ** in-memory database structures. 
  */
  if( db->init.busy ){
    Index *p;
    p = sqlite3HashInsert(&db->aDb[pIndex->iDb].idxHash, 
                         pIndex->zName, strlen(pIndex->zName)+1, pIndex);
    if( p ){
      assert( p==pIndex );  /* Malloc must have failed */
      goto exit_create_index;
    }
    db->flags |= SQLITE_InternChanges;
    if( pTblName!=0 ){
      pIndex->tnum = db->init.newTnum;
    }
  }

  /* If the db->init.busy is 0 then create the index on disk.  This
  ** involves writing the index into the master table and filling in the
  ** index with the current table contents.
  **
  ** The db->init.busy is 0 when the user first enters a CREATE INDEX 
  ** command.  db->init.busy is 1 when a database is opened and 
  ** CREATE INDEX statements are read out of the master table.  In
  ** the latter case the index already exists on disk, which is why
  ** we don't want to recreate it.
  **
  ** If pTblName==0 it means this index is generated as a primary key
  ** or UNIQUE constraint of a CREATE TABLE statement.  Since the table
  ** has just been created, it contains no data and the index initialization
  ** step can be skipped.
  */
  else if( db->init.busy==0 ){
    Vdbe *v;
    char *zStmt;
    int iMem = pParse->nMem++;

    v = sqlite3GetVdbe(pParse);
    if( v==0 ) goto exit_create_index;

    /* Create the rootpage for the index
    */
    sqlite3BeginWriteOperation(pParse, 1, iDb);
    sqlite3VdbeAddOp(v, OP_CreateIndex, iDb, 0);
    sqlite3VdbeAddOp(v, OP_MemStore, iMem, 0);

    /* Gather the complete text of the CREATE INDEX statement into
    ** the zStmt variable
    */
    if( pStart && pEnd ){
      /* A named index with an explicit CREATE INDEX statement */
      zStmt = sqlite3MPrintf("CREATE%s INDEX %.*s",
        onError==OE_None ? "" : " UNIQUE",
        pEnd->z - pName->z + 1,
        pName->z);
    }else{
      /* An automatic index created by a PRIMARY KEY or UNIQUE constraint */
      /* zStmt = sqlite3MPrintf(""); */
      zStmt = 0;
    }

    /* Add an entry in sqlite_master for this index
    */
    sqlite3NestedParse(pParse, 
        "INSERT INTO %Q.%s VALUES('index',%Q,%Q,#0,%Q);",
        db->aDb[iDb].zName, SCHEMA_TABLE(iDb),
        pIndex->zName,
        pTab->zName,
        zStmt
    );
    sqlite3VdbeAddOp(v, OP_Pop, 1, 0);
    sqliteFree(zStmt);

    /* Fill the index with data and reparse the schema. Code an OP_Expire
    ** to invalidate all pre-compiled statements.
    */
    if( pTblName ){
      sqlite3RefillIndex(pParse, pIndex, iMem);
      sqlite3ChangeCookie(db, v, iDb);
      sqlite3VdbeOp3(v, OP_ParseSchema, iDb, 0,
         sqlite3MPrintf("name='%q'", pIndex->zName), P3_DYNAMIC);
      sqlite3VdbeAddOp(v, OP_Expire, 0, 0);
    }
  }

  /* When adding an index to the list of indices for a table, make
  ** sure all indices labeled OE_Replace come after all those labeled
  ** OE_Ignore.  This is necessary for the correct operation of UPDATE
  ** and INSERT.
  */
  if( db->init.busy || pTblName==0 ){
    if( onError!=OE_Replace || pTab->pIndex==0
         || pTab->pIndex->onError==OE_Replace){
      pIndex->pNext = pTab->pIndex;
      pTab->pIndex = pIndex;
    }else{
      Index *pOther = pTab->pIndex;
      while( pOther->pNext && pOther->pNext->onError!=OE_Replace ){
        pOther = pOther->pNext;
      }
      pIndex->pNext = pOther->pNext;
      pOther->pNext = pIndex;
    }
    pIndex = 0;
  }

  /* Clean up before exiting */
exit_create_index:
  if( pIndex ){
    freeIndex(pIndex);
  }
  sqlite3ExprListDelete(pList);
  sqlite3SrcListDelete(pTblName);
  sqliteFree(zName);
  return;
}

/*
** Fill the Index.aiRowEst[] array with default information - information
** to be used when we have not run the ANALYZE command.
**
** aiRowEst[0] is suppose to contain the number of elements in the index.
** Since we do not know, guess 1 million.  aiRowEst[1] is an estimate of the
** number of rows in the table that match any particular value of the
** first column of the index.  aiRowEst[2] is an estimate of the number
** of rows that match any particular combiniation of the first 2 columns
** of the index.  And so forth.  It must always be the case that
*
**           aiRowEst[N]<=aiRowEst[N-1]
**           aiRowEst[N]>=1
**
** Apart from that, we have little to go on besides intuition as to
** how aiRowEst[] should be initialized.  The numbers generated here
** are based on typical values found in actual indices.
*/
void sqlite3DefaultRowEst(Index *pIdx){
  int *a = pIdx->aiRowEst;
  int i;
  assert( a!=0 );
  a[0] = 1000000;
  for(i=pIdx->nColumn; i>=1; i--){
    a[i] = 10;
  }
  if( pIdx->onError!=OE_None ){
    a[pIdx->nColumn] = 1;
  }
}

/*
** This routine will drop an existing named index.  This routine
** implements the DROP INDEX statement.
*/
void sqlite3DropIndex(Parse *pParse, SrcList *pName){
  Index *pIndex;
  Vdbe *v;
  sqlite3 *db = pParse->db;

  if( pParse->nErr || sqlite3_malloc_failed ){
    goto exit_drop_index;
  }
  assert( pName->nSrc==1 );
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    goto exit_drop_index;
  }
  pIndex = sqlite3FindIndex(db, pName->a[0].zName, pName->a[0].zDatabase);
  if( pIndex==0 ){
    sqlite3ErrorMsg(pParse, "no such index: %S", pName, 0);
    pParse->checkSchema = 1;
    goto exit_drop_index;
  }
  if( pIndex->autoIndex ){
    sqlite3ErrorMsg(pParse, "index associated with UNIQUE "
      "or PRIMARY KEY constraint cannot be dropped", 0);
    goto exit_drop_index;
  }
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_DROP_INDEX;
    Table *pTab = pIndex->pTable;
    const char *zDb = db->aDb[pIndex->iDb].zName;
    const char *zTab = SCHEMA_TABLE(pIndex->iDb);
    if( sqlite3AuthCheck(pParse, SQLITE_DELETE, zTab, 0, zDb) ){
      goto exit_drop_index;
    }
    if( !OMIT_TEMPDB && pIndex->iDb ) code = SQLITE_DROP_TEMP_INDEX;
    if( sqlite3AuthCheck(pParse, code, pIndex->zName, pTab->zName, zDb) ){
      goto exit_drop_index;
    }
  }
#endif

  /* Generate code to remove the index and from the master table */
  v = sqlite3GetVdbe(pParse);
  if( v ){
    int iDb = pIndex->iDb;
    sqlite3NestedParse(pParse,
       "DELETE FROM %Q.%s WHERE name=%Q",
       db->aDb[iDb].zName, SCHEMA_TABLE(iDb),
       pIndex->zName
    );
    sqlite3ChangeCookie(db, v, iDb);
    destroyRootPage(pParse, pIndex->tnum, iDb);
    sqlite3VdbeOp3(v, OP_DropIndex, iDb, 0, pIndex->zName, 0);
  }

exit_drop_index:
  sqlite3SrcListDelete(pName);
}

/*
** Append a new element to the given IdList.  Create a new IdList if
** need be.
**
** A new IdList is returned, or NULL if malloc() fails.
*/
IdList *sqlite3IdListAppend(IdList *pList, Token *pToken){
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(IdList) );
    if( pList==0 ) return 0;
    pList->nAlloc = 0;
  }
  if( pList->nId>=pList->nAlloc ){
    struct IdList_item *a;
    pList->nAlloc = pList->nAlloc*2 + 5;
    a = sqliteRealloc(pList->a, pList->nAlloc*sizeof(pList->a[0]) );
    if( a==0 ){
      sqlite3IdListDelete(pList);
      return 0;
    }
    pList->a = a;
  }
  memset(&pList->a[pList->nId], 0, sizeof(pList->a[0]));
  pList->a[pList->nId].zName = sqlite3NameFromToken(pToken);
  pList->nId++;
  return pList;
}

/*
** Delete an IdList.
*/
void sqlite3IdListDelete(IdList *pList){
  int i;
  if( pList==0 ) return;
  for(i=0; i<pList->nId; i++){
    sqliteFree(pList->a[i].zName);
  }
  sqliteFree(pList->a);
  sqliteFree(pList);
}

/*
** Return the index in pList of the identifier named zId.  Return -1
** if not found.
*/
int sqlite3IdListIndex(IdList *pList, const char *zName){
  int i;
  if( pList==0 ) return -1;
  for(i=0; i<pList->nId; i++){
    if( sqlite3StrICmp(pList->a[i].zName, zName)==0 ) return i;
  }
  return -1;
}

/*
** Append a new table name to the given SrcList.  Create a new SrcList if
** need be.  A new entry is created in the SrcList even if pToken is NULL.
**
** A new SrcList is returned, or NULL if malloc() fails.
**
** If pDatabase is not null, it means that the table has an optional
** database name prefix.  Like this:  "database.table".  The pDatabase
** points to the table name and the pTable points to the database name.
** The SrcList.a[].zName field is filled with the table name which might
** come from pTable (if pDatabase is NULL) or from pDatabase.  
** SrcList.a[].zDatabase is filled with the database name from pTable,
** or with NULL if no database is specified.
**
** In other words, if call like this:
**
**         sqlite3SrcListAppend(A,B,0);
**
** Then B is a table name and the database name is unspecified.  If called
** like this:
**
**         sqlite3SrcListAppend(A,B,C);
**
** Then C is the table name and B is the database name.
*/
SrcList *sqlite3SrcListAppend(SrcList *pList, Token *pTable, Token *pDatabase){
  struct SrcList_item *pItem;
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(SrcList) );
    if( pList==0 ) return 0;
    pList->nAlloc = 1;
  }
  if( pList->nSrc>=pList->nAlloc ){
    SrcList *pNew;
    pList->nAlloc *= 2;
    pNew = sqliteRealloc(pList,
               sizeof(*pList) + (pList->nAlloc-1)*sizeof(pList->a[0]) );
    if( pNew==0 ){
      sqlite3SrcListDelete(pList);
      return 0;
    }
    pList = pNew;
  }
  pItem = &pList->a[pList->nSrc];
  memset(pItem, 0, sizeof(pList->a[0]));
  if( pDatabase && pDatabase->z==0 ){
    pDatabase = 0;
  }
  if( pDatabase && pTable ){
    Token *pTemp = pDatabase;
    pDatabase = pTable;
    pTable = pTemp;
  }
  pItem->zName = sqlite3NameFromToken(pTable);
  pItem->zDatabase = sqlite3NameFromToken(pDatabase);
  pItem->iCursor = -1;
  pList->nSrc++;
  return pList;
}

/*
** Assign cursors to all tables in a SrcList
*/
void sqlite3SrcListAssignCursors(Parse *pParse, SrcList *pList){
  int i;
  struct SrcList_item *pItem;
  for(i=0, pItem=pList->a; i<pList->nSrc; i++, pItem++){
    if( pItem->iCursor>=0 ) break;
    pItem->iCursor = pParse->nTab++;
    if( pItem->pSelect ){
      sqlite3SrcListAssignCursors(pParse, pItem->pSelect->pSrc);
    }
  }
}

/*
** Add an alias to the last identifier on the given identifier list.
*/
void sqlite3SrcListAddAlias(SrcList *pList, Token *pToken){
  if( pList && pList->nSrc>0 ){
    pList->a[pList->nSrc-1].zAlias = sqlite3NameFromToken(pToken);
  }
}

/*
** Delete an entire SrcList including all its substructure.
*/
void sqlite3SrcListDelete(SrcList *pList){
  int i;
  struct SrcList_item *pItem;
  if( pList==0 ) return;
  for(pItem=pList->a, i=0; i<pList->nSrc; i++, pItem++){
    sqliteFree(pItem->zDatabase);
    sqliteFree(pItem->zName);
    sqliteFree(pItem->zAlias);
    sqlite3DeleteTable(0, pItem->pTab);
    sqlite3SelectDelete(pItem->pSelect);
    sqlite3ExprDelete(pItem->pOn);
    sqlite3IdListDelete(pItem->pUsing);
  }
  sqliteFree(pList);
}

/*
** Begin a transaction
*/
void sqlite3BeginTransaction(Parse *pParse, int type){
  sqlite3 *db;
  Vdbe *v;
  int i;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite3_malloc_failed ) return;
  if( sqlite3AuthCheck(pParse, SQLITE_TRANSACTION, "BEGIN", 0, 0) ) return;

  v = sqlite3GetVdbe(pParse);
  if( !v ) return;
  if( type!=TK_DEFERRED ){
    for(i=0; i<db->nDb; i++){
      sqlite3VdbeAddOp(v, OP_Transaction, i, (type==TK_EXCLUSIVE)+1);
    }
  }
  sqlite3VdbeAddOp(v, OP_AutoCommit, 0, 0);
}

/*
** Commit a transaction
*/
void sqlite3CommitTransaction(Parse *pParse){
  sqlite3 *db;
  Vdbe *v;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite3_malloc_failed ) return;
  if( sqlite3AuthCheck(pParse, SQLITE_TRANSACTION, "COMMIT", 0, 0) ) return;

  v = sqlite3GetVdbe(pParse);
  if( v ){
    sqlite3VdbeAddOp(v, OP_AutoCommit, 1, 0);
  }
}

/*
** Rollback a transaction
*/
void sqlite3RollbackTransaction(Parse *pParse){
  sqlite3 *db;
  Vdbe *v;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite3_malloc_failed ) return;
  if( sqlite3AuthCheck(pParse, SQLITE_TRANSACTION, "ROLLBACK", 0, 0) ) return;

  v = sqlite3GetVdbe(pParse);
  if( v ){
    sqlite3VdbeAddOp(v, OP_AutoCommit, 1, 1);
  }
}

/*
** Make sure the TEMP database is open and available for use.  Return
** the number of errors.  Leave any error messages in the pParse structure.
*/
static int sqlite3OpenTempDatabase(Parse *pParse){
  sqlite3 *db = pParse->db;
  if( db->aDb[1].pBt==0 && !pParse->explain ){
    int rc = sqlite3BtreeFactory(db, 0, 0, MAX_PAGES, &db->aDb[1].pBt);
    if( rc!=SQLITE_OK ){
      sqlite3ErrorMsg(pParse, "unable to open a temporary database "
        "file for storing temporary tables");
      pParse->rc = rc;
      return 1;
    }
    if( db->flags & !db->autoCommit ){
      rc = sqlite3BtreeBeginTrans(db->aDb[1].pBt, 1);
      if( rc!=SQLITE_OK ){
        sqlite3ErrorMsg(pParse, "unable to get a write lock on "
          "the temporary database file");
        pParse->rc = rc;
        return 1;
      }
    }
  }
  return 0;
}

/*
** Generate VDBE code that will verify the schema cookie and start
** a read-transaction for all named database files.
**
** It is important that all schema cookies be verified and all
** read transactions be started before anything else happens in
** the VDBE program.  But this routine can be called after much other
** code has been generated.  So here is what we do:
**
** The first time this routine is called, we code an OP_Goto that
** will jump to a subroutine at the end of the program.  Then we
** record every database that needs its schema verified in the
** pParse->cookieMask field.  Later, after all other code has been
** generated, the subroutine that does the cookie verifications and
** starts the transactions will be coded and the OP_Goto P2 value
** will be made to point to that subroutine.  The generation of the
** cookie verification subroutine code happens in sqlite3FinishCoding().
**
** If iDb<0 then code the OP_Goto only - don't set flag to verify the
** schema on any databases.  This can be used to position the OP_Goto
** early in the code, before we know if any database tables will be used.
*/
void sqlite3CodeVerifySchema(Parse *pParse, int iDb){
  sqlite3 *db;
  Vdbe *v;
  int mask;

  v = sqlite3GetVdbe(pParse);
  if( v==0 ) return;  /* This only happens if there was a prior error */
  db = pParse->db;
  if( pParse->cookieGoto==0 ){
    pParse->cookieGoto = sqlite3VdbeAddOp(v, OP_Goto, 0, 0)+1;
  }
  if( iDb>=0 ){
    assert( iDb<db->nDb );
    assert( db->aDb[iDb].pBt!=0 || iDb==1 );
    assert( iDb<32 );
    mask = 1<<iDb;
    if( (pParse->cookieMask & mask)==0 ){
      pParse->cookieMask |= mask;
      pParse->cookieValue[iDb] = db->aDb[iDb].schema_cookie;
      if( !OMIT_TEMPDB && iDb==1 ){
        sqlite3OpenTempDatabase(pParse);
      }
    }
  }
}

/*
** Generate VDBE code that prepares for doing an operation that
** might change the database.
**
** This routine starts a new transaction if we are not already within
** a transaction.  If we are already within a transaction, then a checkpoint
** is set if the setStatement parameter is true.  A checkpoint should
** be set for operations that might fail (due to a constraint) part of
** the way through and which will need to undo some writes without having to
** rollback the whole transaction.  For operations where all constraints
** can be checked before any changes are made to the database, it is never
** necessary to undo a write and the checkpoint should not be set.
**
** Only database iDb and the temp database are made writable by this call.
** If iDb==0, then the main and temp databases are made writable.   If
** iDb==1 then only the temp database is made writable.  If iDb>1 then the
** specified auxiliary database and the temp database are made writable.
*/
void sqlite3BeginWriteOperation(Parse *pParse, int setStatement, int iDb){
  Vdbe *v = sqlite3GetVdbe(pParse);
  if( v==0 ) return;
  sqlite3CodeVerifySchema(pParse, iDb);
  pParse->writeMask |= 1<<iDb;
  if( setStatement && pParse->nested==0 ){
    sqlite3VdbeAddOp(v, OP_Statement, iDb, 0);
  }
  if( (OMIT_TEMPDB || iDb!=1) && pParse->db->aDb[1].pBt!=0 ){
    sqlite3BeginWriteOperation(pParse, setStatement, 1);
  }
}

/*
** Check to see if pIndex uses the collating sequence pColl.  Return
** true if it does and false if it does not.
*/
#ifndef SQLITE_OMIT_REINDEX
static int collationMatch(CollSeq *pColl, Index *pIndex){
  int n = pIndex->keyInfo.nField;
  CollSeq **pp = pIndex->keyInfo.aColl;
  while( n-- ){
    if( *pp==pColl ) return 1;
    pp++;
  }
  return 0;
}
#endif

/*
** Recompute all indices of pTab that use the collating sequence pColl.
** If pColl==0 then recompute all indices of pTab.
*/
#ifndef SQLITE_OMIT_REINDEX
static void reindexTable(Parse *pParse, Table *pTab, CollSeq *pColl){
  Index *pIndex;              /* An index associated with pTab */

  for(pIndex=pTab->pIndex; pIndex; pIndex=pIndex->pNext){
    if( pColl==0 || collationMatch(pColl,pIndex) ){
      sqlite3BeginWriteOperation(pParse, 0, pTab->iDb);
      sqlite3RefillIndex(pParse, pIndex, -1);
    }
  }
}
#endif

/*
** Recompute all indices of all tables in all databases where the
** indices use the collating sequence pColl.  If pColl==0 then recompute
** all indices everywhere.
*/
#ifndef SQLITE_OMIT_REINDEX
static void reindexDatabases(Parse *pParse, CollSeq *pColl){
  Db *pDb;                    /* A single database */
  int iDb;                    /* The database index number */
  sqlite3 *db = pParse->db;   /* The database connection */
  HashElem *k;                /* For looping over tables in pDb */
  Table *pTab;                /* A table in the database */

  for(iDb=0, pDb=db->aDb; iDb<db->nDb; iDb++, pDb++){
    if( pDb==0 ) continue;
    for(k=sqliteHashFirst(&pDb->tblHash);  k; k=sqliteHashNext(k)){
      pTab = (Table*)sqliteHashData(k);
      reindexTable(pParse, pTab, pColl);
    }
  }
}
#endif

/*
** Generate code for the REINDEX command.
**
**        REINDEX                            -- 1
**        REINDEX  <collation>               -- 2
**        REINDEX  ?<database>.?<tablename>  -- 3
**        REINDEX  ?<database>.?<indexname>  -- 4
**
** Form 1 causes all indices in all attached databases to be rebuilt.
** Form 2 rebuilds all indices in all databases that use the named
** collating function.  Forms 3 and 4 rebuild the named index or all
** indices associated with the named table.
*/
#ifndef SQLITE_OMIT_REINDEX
void sqlite3Reindex(Parse *pParse, Token *pName1, Token *pName2){
  CollSeq *pColl;             /* Collating sequence to be reindexed, or NULL */
  char *z;                    /* Name of a table or index */
  const char *zDb;            /* Name of the database */
  Table *pTab;                /* A table in the database */
  Index *pIndex;              /* An index associated with pTab */
  int iDb;                    /* The database index number */
  sqlite3 *db = pParse->db;   /* The database connection */
  Token *pObjName;            /* Name of the table or index to be reindexed */

  /* Read the database schema. If an error occurs, leave an error message
  ** and code in pParse and return NULL. */
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    return;
  }

  if( pName1==0 || pName1->z==0 ){
    reindexDatabases(pParse, 0);
    return;
  }else if( pName2==0 || pName2->z==0 ){
    pColl = sqlite3FindCollSeq(db, db->enc, pName1->z, pName1->n, 0);
    if( pColl ){
      reindexDatabases(pParse, pColl);
      return;
    }
  }
  iDb = sqlite3TwoPartName(pParse, pName1, pName2, &pObjName);
  if( iDb<0 ) return;
  z = sqlite3NameFromToken(pObjName);
  zDb = db->aDb[iDb].zName;
  pTab = sqlite3FindTable(db, z, zDb);
  if( pTab ){
    reindexTable(pParse, pTab, 0);
    sqliteFree(z);
    return;
  }
  pIndex = sqlite3FindIndex(db, z, zDb);
  sqliteFree(z);
  if( pIndex ){
    sqlite3BeginWriteOperation(pParse, 0, iDb);
    sqlite3RefillIndex(pParse, pIndex, -1);
    return;
  }
  sqlite3ErrorMsg(pParse, "unable to identify the object to be reindexed");
}
#endif
