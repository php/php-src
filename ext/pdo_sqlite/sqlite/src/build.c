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
**     PRAGMA
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

/*
** This routine is called when a new SQL statement is beginning to
** be parsed.  Check to see if the schema for the database needs
** to be read from the SQLITE_MASTER and SQLITE_TEMP_MASTER tables.
** If it does, then read it.
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
    ** This is used to implement sqlite3_trace() functionality.
    */
    sqlite3VdbeOp3(v, OP_Noop, 0, 0, pParse->zSql, pParse->zTail-pParse->zSql);
  }


  /* Get the VDBE program ready for execution
  */
  if( v && pParse->nErr==0 ){
    FILE *trace = (db->flags & SQLITE_VdbeTrace)!=0 ? stdout : 0;
    sqlite3VdbeTrace(v, trace);
    sqlite3VdbeMakeReady(v, pParse->nVar, pParse->nMem+3,
                         pParse->nTab+3, pParse->explain);
    pParse->rc = pParse->nErr ? SQLITE_ERROR : SQLITE_DONE;
    pParse->colNamesSet = 0;
  }else if( pParse->rc==SQLITE_OK ){
    pParse->rc = SQLITE_ERROR;
  }
  pParse->nTab = 0;
  pParse->nMem = 0;
  pParse->nSet = 0;
  pParse->nAgg = 0;
  pParse->nVar = 0;
  pParse->cookieMask = 0;
  pParse->cookieGoto = 0;
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
  for(i=0; i<db->nDb; i++){
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
    }else if( sqlite3FindTable(pParse->db, zName, 0)!=0 ){
      sqlite3ErrorMsg(pParse, "table \"%s\" is not in database \"%s\"",
         zName, zDbase);
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
  for(i=0; i<db->nDb; i++){
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
  if( pOld!=0 && pOld!=p ){
    sqlite3HashInsert(&db->aDb[p->iDb].idxHash, pOld->zName,
                     strlen(pOld->zName)+1, pOld);
  }
  freeIndex(p);
}

/*
** Unlink the given index from its table, then remove
** the index from the index hash table and free its memory
** structures.
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
      Trigger *pTrigger = sqliteHashData(pElem);
      sqlite3DeleteTrigger(pTrigger);
    }
    sqlite3HashClear(&temp2);
    sqlite3HashInit(&pDb->tblHash, SQLITE_HASH_STRING, 0);
    for(pElem=sqliteHashFirst(&temp1); pElem; pElem=sqliteHashNext(pElem)){
      Table *pTab = sqliteHashData(pElem);
      sqlite3DeleteTable(db, pTab);
    }
    sqlite3HashClear(&temp1);
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
  for(i=0, pCol=pTable->aCol; i<pTable->nCol; i++, pCol++){
    sqliteFree(pCol->zName);
    sqliteFree(pCol->zDflt);
    sqliteFree(pCol->zType);
  }
  sqliteFree(pTable->aCol);
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

  /* Delete all indices associated with this table
  */
  for(pIndex = pTable->pIndex; pIndex; pIndex=pNext){
    pNext = pIndex->pNext;
    assert( pIndex->iDb==pTable->iDb || (pTable->iDb==0 && pIndex->iDb==1) );
    sqliteDeleteIndex(db, pIndex);
  }

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
** Tokens are really just pointers into the original SQL text and so
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
int findDb(sqlite3 *db, Token *pName){
  int i;
  Db *pDb;
  for(pDb=db->aDb, i=0; i<db->nDb; i++, pDb++){
    if( pName->n==strlen(pDb->zName) && 
        0==sqlite3StrNICmp(pDb->zName, pName->z, pName->n) ){
      return i;
    }
  }
  return -1;
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
    iDb = findDb(db, pName1);
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
  if( !pParse->db->init.busy && 0==sqlite3StrNICmp(zName, "sqlite_", 7) ){
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
  char *zName;
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
  if( isTemp && iDb>1 ){
    /* If creating a temp table, the name may not be qualified */
    sqlite3ErrorMsg(pParse, "temporary table name must be unqualified");
    pParse->nErr++;
    return;
  }
  if( isTemp ) iDb = 1;

  pParse->sNameToken = *pName;
  zName = sqlite3NameFromToken(pName);
  if( zName==0 ) return;
  if( SQLITE_OK!=sqlite3CheckObjectName(pParse, zName) ){
    sqliteFree(zName);
    return;
  }
  if( db->init.iDb==1 ) isTemp = 1;
#ifndef SQLITE_OMIT_AUTHORIZATION
  assert( (isTemp & 1)==isTemp );
  {
    int code;
    char *zDb = db->aDb[iDb].zName;
    if( sqlite3AuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0, zDb) ){
      sqliteFree(zName);
      return;
    }
    if( isView ){
      if( isTemp ){
        code = SQLITE_CREATE_TEMP_VIEW;
      }else{
        code = SQLITE_CREATE_VIEW;
      }
    }else{
      if( isTemp ){
        code = SQLITE_CREATE_TEMP_TABLE;
      }else{
        code = SQLITE_CREATE_TABLE;
      }
    }
    if( sqlite3AuthCheck(pParse, code, zName, 0, zDb) ){
      sqliteFree(zName);
      return;
    }
  }
#endif

  /* Make sure the new table name does not collide with an existing
  ** index or table name in the same database.  Issue an error message if
  ** it does.
  */
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ) return;
  pTable = sqlite3FindTable(db, zName, db->aDb[iDb].zName);
  if( pTable ){
    sqlite3ErrorMsg(pParse, "table %T already exists", pName);
    sqliteFree(zName);
    return;
  }
  if( (pIdx = sqlite3FindIndex(db, zName, 0))!=0 && 
      ( iDb==0 || !db->init.busy) ){
    sqlite3ErrorMsg(pParse, "there is already an index named %s", zName);
    sqliteFree(zName);
    return;
  }
  pTable = sqliteMalloc( sizeof(Table) );
  if( pTable==0 ){
    pParse->rc = SQLITE_NOMEM;
    pParse->nErr++;
    sqliteFree(zName);
    return;
  }
  pTable->zName = zName;
  pTable->nCol = 0;
  pTable->aCol = 0;
  pTable->iPKey = -1;
  pTable->pIndex = 0;
  pTable->iDb = iDb;
  if( pParse->pNewTable ) sqlite3DeleteTable(db, pParse->pNewTable);
  pParse->pNewTable = pTable;

  /* Begin generating the code that will insert the table record into
  ** the SQLITE_MASTER table.  Note in particular that we must go ahead
  ** and allocate the record number for the table entry now.  Before any
  ** PRIMARY KEY or UNIQUE keywords are parsed.  Those keywords will cause
  ** indices to be created and the table record must come before the 
  ** indices.  Hence, the record number for the table must be allocated
  ** now.
  */
  if( !db->init.busy && (v = sqlite3GetVdbe(pParse))!=0 ){
    sqlite3BeginWriteOperation(pParse, 0, iDb);
    /* Every time a new table is created the file-format
    ** and encoding meta-values are set in the database, in
    ** case this is the first table created.
    */
    sqlite3VdbeAddOp(v, OP_Integer, db->file_format, 0);
    sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 1);
    sqlite3VdbeAddOp(v, OP_Integer, db->enc, 0);
    sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 4);

    sqlite3OpenMasterTable(v, iDb);
    sqlite3VdbeAddOp(v, OP_NewRecno, 0, 0);
    sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
    sqlite3VdbeAddOp(v, OP_String8, 0, 0);
    sqlite3VdbeAddOp(v, OP_PutIntKey, 0, 0);
  }
}

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
    if( sqlite3StrICmp(z, p->aCol[i].zName)==0 ){
      sqlite3ErrorMsg(pParse, "duplicate column name: %s", z);
      sqliteFree(z);
      return;
    }
  }
  if( (p->nCol & 0x7)==0 ){
    Column *aNew;
    aNew = sqliteRealloc( p->aCol, (p->nCol+8)*sizeof(p->aCol[0]));
    if( aNew==0 ) return;
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
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.  The pFirst token is the first
** token in the sequence of tokens that describe the type of the
** column currently under construction.   pLast is the last token
** in the sequence.  Use this information to construct a string
** that contains the typename of the column and store that string
** in zType.
*/ 
void sqlite3AddColumnType(Parse *pParse, Token *pFirst, Token *pLast){
  Table *p;
  int i, j;
  int n;
  char *z, **pz;
  Column *pCol;
  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i<0 ) return;
  pCol = &p->aCol[i];
  pz = &pCol->zType;
  n = pLast->n + (pLast->z - pFirst->z);
  assert( pCol->zType==0 );
  z = pCol->zType = sqlite3MPrintf("%.*s", n, pFirst->z);
  if( z==0 ) return;
  for(i=j=0; z[i]; i++){
    int c = z[i];
    if( isspace(c) ) continue;
    z[j++] = c;
  }
  z[j] = 0;
  pCol->affinity = sqlite3AffinityType(z, n);
}

/*
** The given token is the default value for the last column added to
** the table currently under construction.  If "minusFlag" is true, it
** means the value token was preceded by a minus sign.
**
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.
*/
void sqlite3AddDefaultValue(Parse *pParse, Token *pVal, int minusFlag){
  Table *p;
  int i;
  char *z;
  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i<0 ) return;
  assert( p->aCol[i].zDflt==0 );
  z = p->aCol[i].zDflt = sqlite3MPrintf("%s%T", minusFlag ? "-" : "", pVal);
  sqlite3Dequote(z);
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
** then we will try to use that column as the row id.  (Exception:
** For backwards compatibility with older databases, do not do this
** if the file format version number is less than 1.)  Set the Table.iPKey
** field of the table under construction to be the index of the
** INTEGER PRIMARY KEY column.  Table.iPKey is set to -1 if there is
** no INTEGER PRIMARY KEY.
**
** If the key is not an INTEGER PRIMARY KEY, then create a unique
** index for the key.  No index is created for INTEGER PRIMARY KEYs.
*/
void sqlite3AddPrimaryKey(Parse *pParse, ExprList *pList, int onError){
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
** Locate and return an entry from the db.aCollSeq hash table. If the entry
** specified by zName and nName is not found and parameter 'create' is
** true, then create a new entry. Otherwise return NULL.
**
** Each pointer stored in the sqlite3.aCollSeq hash table contains an
** array of three CollSeq structures. The first is the collation sequence
** prefferred for UTF-8, the second UTF-16le, and the third UTF-16be.
**
** Stored immediately after the three collation sequences is a copy of
** the collation sequence name. A pointer to this string is stored in
** each collation sequence structure.
*/
static CollSeq * findCollSeqEntry(
  sqlite3 *db,
  const char *zName,
  int nName,
  int create
){
  CollSeq *pColl;
  if( nName<0 ) nName = strlen(zName);
  pColl = sqlite3HashFind(&db->aCollSeq, zName, nName);

  if( 0==pColl && create ){
    pColl = sqliteMalloc( 3*sizeof(*pColl) + nName + 1 );
    if( pColl ){
      pColl[0].zName = (char*)&pColl[3];
      pColl[0].enc = SQLITE_UTF8;
      pColl[1].zName = (char*)&pColl[3];
      pColl[1].enc = SQLITE_UTF16LE;
      pColl[2].zName = (char*)&pColl[3];
      pColl[2].enc = SQLITE_UTF16BE;
      memcpy(pColl[0].zName, zName, nName);
      pColl[0].zName[nName] = 0;
      sqlite3HashInsert(&db->aCollSeq, pColl[0].zName, nName, pColl);
    }
  }
  return pColl;
}

/*
** Parameter zName points to a UTF-8 encoded string nName bytes long.
** Return the CollSeq* pointer for the collation sequence named zName
** for the encoding 'enc' from the database 'db'.
**
** If the entry specified is not found and 'create' is true, then create a
** new entry.  Otherwise return NULL.
*/
CollSeq *sqlite3FindCollSeq(
  sqlite3 *db,
  u8 enc,
  const char *zName,
  int nName,
  int create
){
  CollSeq *pColl = findCollSeqEntry(db, zName, nName, create);
  assert( SQLITE_UTF8==1 && SQLITE_UTF16LE==2 && SQLITE_UTF16BE==3 );
  assert( enc>=SQLITE_UTF8 && enc<=SQLITE_UTF16BE );
  if( pColl ) pColl += enc-1;
  return pColl;
}

/*
** Invoke the 'collation needed' callback to request a collation sequence
** in the database text encoding of name zName, length nName.
** If the collation sequence
*/
static void callCollNeeded(sqlite3 *db, const char *zName, int nName){
  assert( !db->xCollNeeded || !db->xCollNeeded16 );
  if( nName<0 ) nName = strlen(zName);
  if( db->xCollNeeded ){
    char *zExternal = sqliteStrNDup(zName, nName);
    if( !zExternal ) return;
    db->xCollNeeded(db->pCollNeededArg, db, (int)db->enc, zExternal);
    sqliteFree(zExternal);
  }
  if( db->xCollNeeded16 ){
    char const *zExternal;
    sqlite3_value *pTmp = sqlite3GetTransientValue(db);
    sqlite3ValueSetStr(pTmp, -1, zName, SQLITE_UTF8, SQLITE_STATIC);
    zExternal = sqlite3ValueText(pTmp, SQLITE_UTF16NATIVE);
    if( !zExternal ) return;
    db->xCollNeeded16(db->pCollNeededArg, db, (int)db->enc, zExternal);
  }
}

/*
** This routine is called if the collation factory fails to deliver a
** collation function in the best encoding but there may be other versions
** of this collation function (for other text encodings) available. Use one
** of these instead if they exist. Avoid a UTF-8 <-> UTF-16 conversion if
** possible.
*/
static int synthCollSeq(Parse *pParse, CollSeq *pColl){
  CollSeq *pColl2;
  char *z = pColl->zName;
  int n = strlen(z);
  sqlite3 *db = pParse->db;
  int i;
  static const u8 aEnc[] = { SQLITE_UTF16BE, SQLITE_UTF16LE, SQLITE_UTF8 };
  for(i=0; i<3; i++){
    pColl2 = sqlite3FindCollSeq(db, aEnc[i], z, n, 0);
    if( pColl2->xCmp!=0 ){
      memcpy(pColl, pColl2, sizeof(CollSeq));
      return SQLITE_OK;
    }
  }
  if( pParse->nErr==0 ){
    sqlite3ErrorMsg(pParse, "no such collation sequence: %.*s", n, z);
  }
  pParse->nErr++;
  return SQLITE_ERROR;
}

/*
** This routine is called on a collation sequence before it is used to
** check that it is defined. An undefined collation sequence exists when
** a database is loaded that contains references to collation sequences
** that have not been defined by sqlite3_create_collation() etc.
**
** If required, this routine calls the 'collation needed' callback to
** request a definition of the collating sequence. If this doesn't work, 
** an equivalent collating sequence that uses a text encoding different
** from the main database is substituted, if one is available.
*/
int sqlite3CheckCollSeq(Parse *pParse, CollSeq *pColl){
  if( pColl && !pColl->xCmp ){
    /* No collation sequence of this type for this encoding is registered.
    ** Call the collation factory to see if it can supply us with one.
    */
    callCollNeeded(pParse->db, pColl->zName, strlen(pColl->zName));
    if( !pColl->xCmp && synthCollSeq(pParse, pColl) ){
      return SQLITE_ERROR;
    }
  }
  return SQLITE_OK;
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
  u8 enc = pParse->db->enc;
  u8 initbusy = pParse->db->init.busy;
  CollSeq *pColl = sqlite3FindCollSeq(pParse->db, enc, zName, nName, initbusy);
  if( nName<0 ) nName = strlen(zName);
  if( !initbusy && (!pColl || !pColl->xCmp) ){
    /* No collation sequence of this type for this encoding is registered.
    ** Call the collation factory to see if it can supply us with one.
    */
    callCollNeeded(pParse->db, zName, nName);
    pColl = sqlite3FindCollSeq(pParse->db, enc, zName, nName, 0);
    if( pColl && !pColl->xCmp ){
      /* There may be a version of the collation sequence that requires
      ** translation between encodings. Search for it with synthCollSeq().
      */
      if( synthCollSeq(pParse, pColl) ){
        return 0;
      }
    }
  }

  /* If nothing has been found, write the error message into pParse */
  if( !initbusy && (!pColl || !pColl->xCmp) ){
    if( pParse->nErr==0 ){
      sqlite3ErrorMsg(pParse, "no such collation sequence: %.*s", nName, zName);
    }
    pColl = 0;
  }
  return pColl;
}



/*
** Scan the column type name zType (length nType) and return the
** associated affinity type.
*/
char sqlite3AffinityType(const char *zType, int nType){
  int n, i;
  static const struct {
    const char *zSub;  /* Keywords substring to search for */
    char nSub;         /* length of zSub */
    char affinity;     /* Affinity to return if it matches */
  } substrings[] = {
    {"INT",  3, SQLITE_AFF_INTEGER},
    {"CHAR", 4, SQLITE_AFF_TEXT},
    {"CLOB", 4, SQLITE_AFF_TEXT},
    {"TEXT", 4, SQLITE_AFF_TEXT},
    {"BLOB", 4, SQLITE_AFF_NONE},
  };

  if( nType==0 ){
    return SQLITE_AFF_NONE;
  }
  for(i=0; i<sizeof(substrings)/sizeof(substrings[0]); i++){
    int c1 = substrings[i].zSub[0];
    int c2 = tolower(c1);
    int limit = nType - substrings[i].nSub;
    const char *z = substrings[i].zSub;
    for(n=0; n<=limit; n++){
      int c = zType[n];
      if( (c==c1 || c==c2)
             && 0==sqlite3StrNICmp(&zType[n], z, substrings[i].nSub) ){
        return substrings[i].affinity;
      }
    }
  }
  return SQLITE_AFF_NUMERIC;
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
  strcpy(zStmt, p->iDb==1 ? "CREATE TEMP TABLE " : "CREATE TABLE ");
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
** recently changes, so the entry for this table already exists in
** the sqlite_master table.  We do not want to create it again.
**
** If the pSelect argument is not NULL, it means that this routine
** was called to create a table generated from a 
** "CREATE TABLE ... AS SELECT ..." statement.  The column names of
** the new table will match the result set of the SELECT.
*/
void sqlite3EndTable(Parse *pParse, Token *pEnd, Select *pSelect){
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

    v = sqlite3GetVdbe(pParse);
    if( v==0 ) return;

    if( p->pSelect==0 ){
      /* A regular table */
      sqlite3VdbeAddOp(v, OP_CreateTable, p->iDb, 0);
    }else{
      /* A view */
      sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
    }

    sqlite3VdbeAddOp(v, OP_Close, 0, 0);

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
  
    sqlite3OpenMasterTable(v, p->iDb);

    sqlite3VdbeOp3(v, OP_String8, 0, 0, p->pSelect==0?"table":"view",P3_STATIC);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, p->zName, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, p->zName, 0);
    sqlite3VdbeAddOp(v, OP_Pull, 3, 0);

    if( pSelect ){
      char *z = createTableStmt(p);
      n = z ? strlen(z) : 0;
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      sqlite3VdbeChangeP3(v, -1, z, n);
      sqliteFree(z);
    }else{
      if( p->pSelect ){
        sqlite3VdbeOp3(v, OP_String8, 0, 0, "CREATE VIEW ", P3_STATIC);
      }else{
        sqlite3VdbeOp3(v, OP_String8, 0, 0, "CREATE TABLE ", P3_STATIC);
      }
      assert( pEnd!=0 );
      n = Addr(pEnd->z) - Addr(pParse->sNameToken.z) + 1;
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      sqlite3VdbeChangeP3(v, -1, pParse->sNameToken.z, n);
      sqlite3VdbeAddOp(v, OP_Concat, 0, 0);
    }
    sqlite3VdbeOp3(v, OP_MakeRecord, 5, 0, "tttit", P3_STATIC);
    sqlite3VdbeAddOp(v, OP_PutIntKey, 0, 0);
    sqlite3ChangeCookie(db, v, p->iDb);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
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
    for(pFKey=p->pFKey; pFKey; pFKey=pFKey->pNextFrom){
      int nTo = strlen(pFKey->zTo) + 1;
      pFKey->pNextTo = sqlite3HashFind(&pDb->aFKey, pFKey->zTo, nTo);
      sqlite3HashInsert(&pDb->aFKey, pFKey->zTo, nTo, pFKey);
    }
    pParse->pNewTable = 0;
    db->nTable++;
    db->flags |= SQLITE_InternChanges;
  }
}

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
  sqlite3EndTable(pParse, &sEnd, 0);
  return;
}

/*
** The Table structure pTable is really a VIEW.  Fill in the names of
** the columns of the view in the pTable structure.  Return the number
** of errors.  If an error is seen leave an error message in pParse->zErrMsg.
*/
int sqlite3ViewGetColumnNames(Parse *pParse, Table *pTable){
  ExprList *pEList;
  Select *pSel;
  Table *pSelTab;
  int nErr = 0;

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
  if( pTable->nCol<0 ){
    sqlite3ErrorMsg(pParse, "view %s is circularly defined", pTable->zName);
    return 1;
  }

  /* If we get this far, it means we need to compute the table names.
  */
  assert( pTable->pSelect ); /* If nCol==0, then pTable must be a VIEW */
  pSel = pTable->pSelect;

  /* Note that the call to sqlite3ResultSetOfSelect() will expand any
  ** "*" elements in this list.  But we will need to restore the list
  ** back to its original configuration afterwards, so we save a copy of
  ** the original in pEList.
  */
  pEList = pSel->pEList;
  pSel->pEList = sqlite3ExprListDup(pEList);
  if( pSel->pEList==0 ){
    pSel->pEList = pEList;
    return 1;  /* Malloc failed */
  }
  pTable->nCol = -1;
  pSelTab = sqlite3ResultSetOfSelect(pParse, 0, pSel);
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
  sqlite3SelectUnbind(pSel);
  sqlite3ExprListDelete(pSel->pEList);
  pSel->pEList = pEList;
  return nErr;  
}

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

/*
** This routine is called to do the work of a DROP TABLE statement.
** pName is the name of the table to be dropped.
*/
void sqlite3DropTable(Parse *pParse, SrcList *pName, int isView){
  Table *pTab;
  Vdbe *v;
  int base;
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
      if( iDb==1 ){
        code = SQLITE_DROP_TEMP_VIEW;
      }else{
        code = SQLITE_DROP_VIEW;
      }
    }else{
      if( iDb==1 ){
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
  if( pTab->readOnly ){
    sqlite3ErrorMsg(pParse, "table %s may not be dropped", pTab->zName);
    pParse->nErr++;
    goto exit_drop_table;
  }
  if( isView && pTab->pSelect==0 ){
    sqlite3ErrorMsg(pParse, "use DROP TABLE to delete table %s", pTab->zName);
    goto exit_drop_table;
  }
  if( !isView && pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "use DROP VIEW to delete view %s", pTab->zName);
    goto exit_drop_table;
  }

  /* Generate code to remove the table from the master table
  ** on disk.
  */
  v = sqlite3GetVdbe(pParse);
  if( v ){
    static const VdbeOpList dropTable[] = {
      { OP_Rewind,     0, ADDR(13), 0},
      { OP_String8,    0, 0,        0}, /* 1 */
      { OP_MemStore,   1, 1,        0},
      { OP_MemLoad,    1, 0,        0}, /* 3 */
      { OP_Column,     0, 2,        0}, /* sqlite_master.tbl_name */
      { OP_Ne,         0, ADDR(12), 0},
      { OP_String8,    0, 0,        "trigger"},
      { OP_Column,     0, 2,        0}, /* sqlite_master.type */
      { OP_Eq,         0, ADDR(12), 0},
      { OP_Delete,     0, 0,        0},
      { OP_Rewind,     0, ADDR(13), 0},
      { OP_Goto,       0, ADDR(3),  0},
      { OP_Next,       0, ADDR(3),  0}, /* 12 */
    };
    Index *pIdx;
    Trigger *pTrigger;
    sqlite3BeginWriteOperation(pParse, 0, pTab->iDb);

    /* Drop all triggers associated with the table being dropped. Code
    ** is generated to remove entries from sqlite_master and/or
    ** sqlite_temp_master if required.
    */
    pTrigger = pTab->pTrigger;
    while( pTrigger ){
      assert( pTrigger->iDb==pTab->iDb || pTrigger->iDb==1 );
      sqlite3DropTriggerPtr(pParse, pTrigger, 1);
      pTrigger = pTrigger->pNext;
    }

    /* Drop all SQLITE_MASTER table and index entries that refer to the
    ** table. The program name loops through the master table and deletes
    ** every row that refers to a table of the same name as the one being
    ** dropped. Triggers are handled seperately because a trigger can be
    ** created in the temp database that refers to a table in another
    ** database.
    */
    sqlite3OpenMasterTable(v, pTab->iDb);
    base = sqlite3VdbeAddOpList(v, ArraySize(dropTable), dropTable);
    sqlite3VdbeChangeP3(v, base+1, pTab->zName, 0);
    sqlite3ChangeCookie(db, v, pTab->iDb);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
    if( !isView ){
      sqlite3VdbeAddOp(v, OP_Destroy, pTab->tnum, pTab->iDb);
      for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
        sqlite3VdbeAddOp(v, OP_Destroy, pIdx->tnum, pIdx->iDb);
      }
    }
    sqlite3VdbeOp3(v, OP_DropTable, pTab->iDb, 0, pTab->zName, 0);
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
  Table *p = pParse->pNewTable;
  int nByte;
  int i;
  int nCol;
  char *z;
  FKey *pFKey = 0;

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
  Table *pTab;
  FKey *pFKey;
  if( (pTab = pParse->pNewTable)==0 || (pFKey = pTab->pFKey)==0 ) return;
  pFKey->isDeferred = isDeferred;
}

/*
** Create a new index for an SQL table.  pIndex is the name of the index 
** and pTable is the name of the table that is to be indexed.  Both will 
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
  Parse *pParse,   /* All information about this parse */
  Token *pName1,   /* First part of index name. May be NULL */
  Token *pName2,   /* Second part of index name. May be NULL */
  SrcList *pTblName,  /* Table to index. Use pParse->pNewTable if 0 */
  ExprList *pList,   /* A list of columns to be indexed */
  int onError,     /* OE_Abort, OE_Ignore, OE_Replace, or OE_None */
  Token *pStart,   /* The CREATE token that begins a CREATE TABLE statement */
  Token *pEnd      /* The ")" that closes the CREATE INDEX statement */
){
  Table *pTab = 0; /* Table to be indexed */
  Index *pIndex = 0; /* The index to be created */
  char *zName = 0;
  int i, j;
  Token nullId;    /* Fake token for an empty ID list */
  DbFixer sFix;    /* For assigning database names to pTable */
  int isTemp;      /* True for a temporary index */
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

    /* If the index name was unqualified, check if the the table
    ** is a temp table. If so, set the database to 1.
    */
    pTab = sqlite3SrcListLookup(pParse, pTblName);
    if( pName2 && pName2->n==0 && pTab && pTab->iDb==1 ){
      iDb = 1;
    }

    if( sqlite3FixInit(&sFix, pParse, iDb, "index", pName) &&
        sqlite3FixSrcList(&sFix, pTblName)
    ){
      goto exit_create_index;
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
  if( pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "views may not be indexed");
    goto exit_create_index;
  }
  isTemp = pTab->iDb==1;

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
  }else if( pName==0 ){
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
    const char *zDb = db->aDb[pTab->iDb].zName;
    if( sqlite3AuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0, zDb) ){
      goto exit_create_index;
    }
    i = SQLITE_CREATE_INDEX;
    if( isTemp ) i = SQLITE_CREATE_TEMP_INDEX;
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
  pIndex = sqliteMalloc( sizeof(Index) + strlen(zName) + 1 +
                        (sizeof(int) + sizeof(CollSeq*))*pList->nExpr );
  if( pIndex==0 ) goto exit_create_index;
  pIndex->aiColumn = (int*)&pIndex->keyInfo.aColl[pList->nExpr];
  pIndex->zName = (char*)&pIndex->aiColumn[pList->nExpr];
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
    int n;
    Vdbe *v;
    int lbl1, lbl2;

    v = sqlite3GetVdbe(pParse);
    if( v==0 ) goto exit_create_index;
    if( pTblName!=0 ){
      sqlite3BeginWriteOperation(pParse, 0, iDb);
      sqlite3OpenMasterTable(v, iDb);
    }
    sqlite3VdbeAddOp(v, OP_NewRecno, 0, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, "index", P3_STATIC);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, pIndex->zName, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, pTab->zName, 0);
    sqlite3VdbeAddOp(v, OP_CreateIndex, iDb, 0);
    if( pTblName ){
      sqlite3VdbeAddOp(v, OP_Dup, 0, 0);
      sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
      sqlite3VdbeOp3(v, OP_OpenWrite, 1, 0,
                     (char*)&pIndex->keyInfo, P3_KEYINFO);
    }
    sqlite3VdbeAddOp(v, OP_String8, 0, 0);
    if( pStart && pEnd ){
      if( onError==OE_None ){
        sqlite3VdbeChangeP3(v, -1, "CREATE INDEX ", P3_STATIC);
      }else{
        sqlite3VdbeChangeP3(v, -1, "CREATE UNIQUE INDEX ", P3_STATIC);
      }
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      n = Addr(pEnd->z) - Addr(pName->z) + 1;
      sqlite3VdbeChangeP3(v, -1, pName->z, n);
      sqlite3VdbeAddOp(v, OP_Concat, 0, 0);
    }
    sqlite3VdbeOp3(v, OP_MakeRecord, 5, 0, "tttit", P3_STATIC);
    sqlite3VdbeAddOp(v, OP_PutIntKey, 0, 0);
    if( pTblName ){
      sqlite3VdbeAddOp(v, OP_Integer, pTab->iDb, 0);
      sqlite3VdbeAddOp(v, OP_OpenRead, 2, pTab->tnum);
      /* VdbeComment((v, "%s", pTab->zName)); */
      sqlite3VdbeAddOp(v, OP_SetNumColumns, 2, pTab->nCol);
      lbl2 = sqlite3VdbeMakeLabel(v);
      sqlite3VdbeAddOp(v, OP_Rewind, 2, lbl2);
      lbl1 = sqlite3VdbeCurrentAddr(v);
      sqlite3GenerateIndexKey(v, pIndex, 2);
      sqlite3VdbeOp3(v, OP_IdxPut, 1, pIndex->onError!=OE_None,
                      "indexed columns are not unique", P3_STATIC);
      sqlite3VdbeAddOp(v, OP_Next, 2, lbl1);
      sqlite3VdbeResolveLabel(v, lbl2);
      sqlite3VdbeAddOp(v, OP_Close, 2, 0);
      sqlite3VdbeAddOp(v, OP_Close, 1, 0);
      sqlite3ChangeCookie(db, v, iDb);
      sqlite3VdbeAddOp(v, OP_Close, 0, 0);
      sqlite3VdbeOp3(v, OP_ParseSchema, iDb, 0,
         sqlite3MPrintf("name='%q'", pIndex->zName), P3_DYNAMIC);
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
** This routine will drop an existing named index.  This routine
** implements the DROP INDEX statement.
*/
void sqlite3DropIndex(Parse *pParse, SrcList *pName){
  Index *pIndex;
  Vdbe *v;
  sqlite3 *db = pParse->db;

  if( pParse->nErr || sqlite3_malloc_failed ) return;
  assert( pName->nSrc==1 );
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ) return;
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
    if( pIndex->iDb ) code = SQLITE_DROP_TEMP_INDEX;
    if( sqlite3AuthCheck(pParse, code, pIndex->zName, pTab->zName, zDb) ){
      goto exit_drop_index;
    }
  }
#endif

  /* Generate code to remove the index and from the master table */
  v = sqlite3GetVdbe(pParse);
  if( v ){
    static const VdbeOpList dropIndex[] = {
      { OP_Rewind,     0, ADDR(9), 0}, 
      { OP_String8,    0, 0,       0}, /* 1 */
      { OP_MemStore,   1, 1,       0},
      { OP_MemLoad,    1, 0,       0}, /* 3 */
      { OP_Column,     0, 1,       0},
      { OP_Eq,         0, ADDR(8), 0},
      { OP_Next,       0, ADDR(3), 0},
      { OP_Goto,       0, ADDR(9), 0},
      { OP_Delete,     0, 0,       0}, /* 8 */
    };
    int base;

    sqlite3BeginWriteOperation(pParse, 0, pIndex->iDb);
    sqlite3OpenMasterTable(v, pIndex->iDb);
    base = sqlite3VdbeAddOpList(v, ArraySize(dropIndex), dropIndex);
    sqlite3VdbeChangeP3(v, base+1, pIndex->zName, 0);
    sqlite3ChangeCookie(db, v, pIndex->iDb);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
    sqlite3VdbeAddOp(v, OP_Destroy, pIndex->tnum, pIndex->iDb);
    sqlite3VdbeOp3(v, OP_DropIndex, pIndex->iDb, 0, pIndex->zName, 0);
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
  for(i=0; i<pList->nSrc; i++){
    if( pList->a[i].iCursor<0 ){
      pList->a[i].iCursor = pParse->nTab++;
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
    if( pItem->pTab && pItem->pTab->isTransient ){
      sqlite3DeleteTable(0, pItem->pTab);
    }
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
      if( iDb==1 ){
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
  if( setStatement ){
    sqlite3VdbeAddOp(v, OP_Statement, iDb, 0);
  }
  if( iDb!=1 && pParse->db->aDb[1].pBt!=0 ){
    sqlite3BeginWriteOperation(pParse, setStatement, 1);
  }
}

/* 
** Return the transient sqlite3_value object used for encoding conversions
** during SQL compilation.
*/
sqlite3_value *sqlite3GetTransientValue(sqlite3 *db){
  if( !db->pValue ){
    db->pValue = sqlite3ValueNew();
  }
  return db->pValue;
}
