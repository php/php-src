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
void sqliteBeginParse(Parse *pParse, int explainFlag){
  sqlite *db = pParse->db;
  int i;
  pParse->explain = explainFlag;
  if((db->flags & SQLITE_Initialized)==0 && db->init.busy==0 ){
    int rc = sqliteInit(db, &pParse->zErrMsg);
    if( rc!=SQLITE_OK ){
      pParse->rc = rc;
      pParse->nErr++;
    }
  }
  for(i=0; i<db->nDb; i++){
    DbClearProperty(db, i, DB_Locked);
    if( !db->aDb[i].inTrans ){
      DbClearProperty(db, i, DB_Cookie);
    }
  }
  pParse->nVar = 0;
}

/*
** This routine is called after a single SQL statement has been
** parsed and we want to execute the VDBE code to implement 
** that statement.  Prior action routines should have already
** constructed VDBE code to do the work of the SQL statement.
** This routine just has to execute the VDBE code.
**
** Note that if an error occurred, it might be the case that
** no VDBE code was generated.
*/
void sqliteExec(Parse *pParse){
  sqlite *db = pParse->db;
  Vdbe *v = pParse->pVdbe;

  if( v==0 && (v = sqliteGetVdbe(pParse))!=0 ){
    sqliteVdbeAddOp(v, OP_Halt, 0, 0);
  }
  if( sqlite_malloc_failed ) return;
  if( v && pParse->nErr==0 ){
    FILE *trace = (db->flags & SQLITE_VdbeTrace)!=0 ? stdout : 0;
    sqliteVdbeTrace(v, trace);
    sqliteVdbeMakeReady(v, pParse->nVar, pParse->explain);
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
}

/*
** Locate the in-memory structure that describes 
** a particular database table given the name
** of that table and (optionally) the name of the database
** containing the table.  Return NULL if not found.
**
** If zDatabase is 0, all databases are searched for the
** table and the first matching table is returned.  (No checking
** for duplicate table names is done.)  The search order is
** TEMP first, then MAIN, then any auxiliary databases added
** using the ATTACH command.
**
** See also sqliteLocateTable().
*/
Table *sqliteFindTable(sqlite *db, const char *zName, const char *zDatabase){
  Table *p = 0;
  int i;
  for(i=0; i<db->nDb; i++){
    int j = (i<2) ? i^1 : i;   /* Search TEMP before MAIN */
    if( zDatabase!=0 && sqliteStrICmp(zDatabase, db->aDb[j].zName) ) continue;
    p = sqliteHashFind(&db->aDb[j].tblHash, zName, strlen(zName)+1);
    if( p ) break;
  }
  return p;
}

/*
** Locate the in-memory structure that describes 
** a particular database table given the name
** of that table and (optionally) the name of the database
** containing the table.  Return NULL if not found.
** Also leave an error message in pParse->zErrMsg.
**
** The difference between this routine and sqliteFindTable()
** is that this routine leaves an error message in pParse->zErrMsg
** where sqliteFindTable() does not.
*/
Table *sqliteLocateTable(Parse *pParse, const char *zName, const char *zDbase){
  Table *p;

  p = sqliteFindTable(pParse->db, zName, zDbase);
  if( p==0 ){
    if( zDbase ){
      sqliteErrorMsg(pParse, "no such table: %s.%s", zDbase, zName);
    }else if( sqliteFindTable(pParse->db, zName, 0)!=0 ){
      sqliteErrorMsg(pParse, "table \"%s\" is not in database \"%s\"",
         zName, zDbase);
    }else{
      sqliteErrorMsg(pParse, "no such table: %s", zName);
    }
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
Index *sqliteFindIndex(sqlite *db, const char *zName, const char *zDb){
  Index *p = 0;
  int i;
  for(i=0; i<db->nDb; i++){
    int j = (i<2) ? i^1 : i;  /* Search TEMP before MAIN */
    if( zDb && sqliteStrICmp(zDb, db->aDb[j].zName) ) continue;
    p = sqliteHashFind(&db->aDb[j].idxHash, zName, strlen(zName)+1);
    if( p ) break;
  }
  return p;
}

/*
** Remove the given index from the index hash table, and free
** its memory structures.
**
** The index is removed from the database hash tables but
** it is not unlinked from the Table that it indexes.
** Unlinking from the Table must be done by the calling function.
*/
static void sqliteDeleteIndex(sqlite *db, Index *p){
  Index *pOld;

  assert( db!=0 && p->zName!=0 );
  pOld = sqliteHashInsert(&db->aDb[p->iDb].idxHash, p->zName,
                          strlen(p->zName)+1, 0);
  if( pOld!=0 && pOld!=p ){
    sqliteHashInsert(&db->aDb[p->iDb].idxHash, pOld->zName,
                     strlen(pOld->zName)+1, pOld);
  }
  sqliteFree(p);
}

/*
** Unlink the given index from its table, then remove
** the index from the index hash table and free its memory
** structures.
*/
void sqliteUnlinkAndDeleteIndex(sqlite *db, Index *pIndex){
  if( pIndex->pTable->pIndex==pIndex ){
    pIndex->pTable->pIndex = pIndex->pNext;
  }else{
    Index *p;
    for(p=pIndex->pTable->pIndex; p && p->pNext!=pIndex; p=p->pNext){}
    if( p && p->pNext==pIndex ){
      p->pNext = pIndex->pNext;
    }
  }
  sqliteDeleteIndex(db, pIndex);
}

/*
** Erase all schema information from the in-memory hash tables of
** database connection.  This routine is called to reclaim memory
** before the connection closes.  It is also called during a rollback
** if there were schema changes during the transaction.
**
** If iDb<=0 then reset the internal schema tables for all database
** files.  If iDb>=2 then reset the internal schema for only the
** single file indicated.
*/
void sqliteResetInternalSchema(sqlite *db, int iDb){
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
    sqliteHashInit(&pDb->trigHash, SQLITE_HASH_STRING, 0);
    sqliteHashClear(&pDb->aFKey);
    sqliteHashClear(&pDb->idxHash);
    for(pElem=sqliteHashFirst(&temp2); pElem; pElem=sqliteHashNext(pElem)){
      Trigger *pTrigger = sqliteHashData(pElem);
      sqliteDeleteTrigger(pTrigger);
    }
    sqliteHashClear(&temp2);
    sqliteHashInit(&pDb->tblHash, SQLITE_HASH_STRING, 0);
    for(pElem=sqliteHashFirst(&temp1); pElem; pElem=sqliteHashNext(pElem)){
      Table *pTab = sqliteHashData(pElem);
      sqliteDeleteTable(db, pTab);
    }
    sqliteHashClear(&temp1);
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
void sqliteRollbackInternalChanges(sqlite *db){
  if( db->flags & SQLITE_InternChanges ){
    sqliteResetInternalSchema(db, 0);
  }
}

/*
** This routine is called when a commit occurs.
*/
void sqliteCommitInternalChanges(sqlite *db){
  db->aDb[0].schema_cookie = db->next_cookie;
  db->flags &= ~SQLITE_InternChanges;
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
void sqliteDeleteTable(sqlite *db, Table *pTable){
  int i;
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
    assert( sqliteHashFind(&db->aDb[pTable->iDb].aFKey,
                           pFKey->zTo, strlen(pFKey->zTo)+1)!=pFKey );
    sqliteFree(pFKey);
  }

  /* Delete the Table structure itself.
  */
  for(i=0; i<pTable->nCol; i++){
    sqliteFree(pTable->aCol[i].zName);
    sqliteFree(pTable->aCol[i].zDflt);
    sqliteFree(pTable->aCol[i].zType);
  }
  sqliteFree(pTable->zName);
  sqliteFree(pTable->aCol);
  sqliteSelectDelete(pTable->pSelect);
  sqliteFree(pTable);
}

/*
** Unlink the given table from the hash tables and the delete the
** table structure with all its indices and foreign keys.
*/
static void sqliteUnlinkAndDeleteTable(sqlite *db, Table *p){
  Table *pOld;
  FKey *pF1, *pF2;
  int i = p->iDb;
  assert( db!=0 );
  pOld = sqliteHashInsert(&db->aDb[i].tblHash, p->zName, strlen(p->zName)+1, 0);
  assert( pOld==0 || pOld==p );
  for(pF1=p->pFKey; pF1; pF1=pF1->pNextFrom){
    int nTo = strlen(pF1->zTo) + 1;
    pF2 = sqliteHashFind(&db->aDb[i].aFKey, pF1->zTo, nTo);
    if( pF2==pF1 ){
      sqliteHashInsert(&db->aDb[i].aFKey, pF1->zTo, nTo, pF1->pNextTo);
    }else{
      while( pF2 && pF2->pNextTo!=pF1 ){ pF2=pF2->pNextTo; }
      if( pF2 ){
        pF2->pNextTo = pF1->pNextTo;
      }
    }
  }
  sqliteDeleteTable(db, p);
}

/*
** Construct the name of a user table or index from a token.
**
** Space to hold the name is obtained from sqliteMalloc() and must
** be freed by the calling function.
*/
char *sqliteTableNameFromToken(Token *pName){
  char *zName = sqliteStrNDup(pName->z, pName->n);
  sqliteDequote(zName);
  return zName;
}

/*
** Generate code to open the appropriate master table.  The table
** opened will be SQLITE_MASTER for persistent tables and 
** SQLITE_TEMP_MASTER for temporary tables.  The table is opened
** on cursor 0.
*/
void sqliteOpenMasterTable(Vdbe *v, int isTemp){
  sqliteVdbeAddOp(v, OP_Integer, isTemp, 0);
  sqliteVdbeAddOp(v, OP_OpenWrite, 0, 2);
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
** At the end of the CREATE TABLE statement, the sqliteEndTable() routine
** is called to complete the construction of the new table record.
*/
void sqliteStartTable(
  Parse *pParse,   /* Parser context */
  Token *pStart,   /* The "CREATE" token */
  Token *pName,    /* Name of table or view to create */
  int isTemp,      /* True if this is a TEMP table */
  int isView       /* True if this is a VIEW */
){
  Table *pTable;
  Index *pIdx;
  char *zName;
  sqlite *db = pParse->db;
  Vdbe *v;
  int iDb;

  pParse->sFirstToken = *pStart;
  zName = sqliteTableNameFromToken(pName);
  if( zName==0 ) return;
  if( db->init.iDb==1 ) isTemp = 1;
#ifndef SQLITE_OMIT_AUTHORIZATION
  assert( (isTemp & 1)==isTemp );
  {
    int code;
    char *zDb = isTemp ? "temp" : "main";
    if( sqliteAuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0, zDb) ){
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
    if( sqliteAuthCheck(pParse, code, zName, 0, zDb) ){
      sqliteFree(zName);
      return;
    }
  }
#endif
 

  /* Before trying to create a temporary table, make sure the Btree for
  ** holding temporary tables is open.
  */
  if( isTemp && db->aDb[1].pBt==0 && !pParse->explain ){
    int rc = sqliteBtreeFactory(db, 0, 0, MAX_PAGES, &db->aDb[1].pBt);
    if( rc!=SQLITE_OK ){
      sqliteErrorMsg(pParse, "unable to open a temporary database "
        "file for storing temporary tables");
      pParse->nErr++;
      return;
    }
    if( db->flags & SQLITE_InTrans ){
      rc = sqliteBtreeBeginTrans(db->aDb[1].pBt);
      if( rc!=SQLITE_OK ){
        sqliteErrorMsg(pParse, "unable to get a write lock on "
          "the temporary database file");
        return;
      }
    }
  }

  /* Make sure the new table name does not collide with an existing
  ** index or table name.  Issue an error message if it does.
  **
  ** If we are re-reading the sqlite_master table because of a schema
  ** change and a new permanent table is found whose name collides with
  ** an existing temporary table, that is not an error.
  */
  pTable = sqliteFindTable(db, zName, 0);
  iDb = isTemp ? 1 : db->init.iDb;
  if( pTable!=0 && (pTable->iDb==iDb || !db->init.busy) ){
    sqliteErrorMsg(pParse, "table %T already exists", pName);
    sqliteFree(zName);
    return;
  }
  if( (pIdx = sqliteFindIndex(db, zName, 0))!=0 &&
          (pIdx->iDb==0 || !db->init.busy) ){
    sqliteErrorMsg(pParse, "there is already an index named %s", zName);
    sqliteFree(zName);
    return;
  }
  pTable = sqliteMalloc( sizeof(Table) );
  if( pTable==0 ){
    sqliteFree(zName);
    return;
  }
  pTable->zName = zName;
  pTable->nCol = 0;
  pTable->aCol = 0;
  pTable->iPKey = -1;
  pTable->pIndex = 0;
  pTable->iDb = iDb;
  if( pParse->pNewTable ) sqliteDeleteTable(db, pParse->pNewTable);
  pParse->pNewTable = pTable;

  /* Begin generating the code that will insert the table record into
  ** the SQLITE_MASTER table.  Note in particular that we must go ahead
  ** and allocate the record number for the table entry now.  Before any
  ** PRIMARY KEY or UNIQUE keywords are parsed.  Those keywords will cause
  ** indices to be created and the table record must come before the 
  ** indices.  Hence, the record number for the table must be allocated
  ** now.
  */
  if( !db->init.busy && (v = sqliteGetVdbe(pParse))!=0 ){
    sqliteBeginWriteOperation(pParse, 0, isTemp);
    if( !isTemp ){
      sqliteVdbeAddOp(v, OP_Integer, db->file_format, 0);
      sqliteVdbeAddOp(v, OP_SetCookie, 0, 1);
    }
    sqliteOpenMasterTable(v, isTemp);
    sqliteVdbeAddOp(v, OP_NewRecno, 0, 0);
    sqliteVdbeAddOp(v, OP_Dup, 0, 0);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeAddOp(v, OP_PutIntKey, 0, 0);
  }
}

/*
** Add a new column to the table currently being constructed.
**
** The parser calls this routine once for each column declaration
** in a CREATE TABLE statement.  sqliteStartTable() gets called
** first to get things going.  Then this routine is called for each
** column.
*/
void sqliteAddColumn(Parse *pParse, Token *pName){
  Table *p;
  int i;
  char *z = 0;
  Column *pCol;
  if( (p = pParse->pNewTable)==0 ) return;
  sqliteSetNString(&z, pName->z, pName->n, 0);
  if( z==0 ) return;
  sqliteDequote(z);
  for(i=0; i<p->nCol; i++){
    if( sqliteStrICmp(z, p->aCol[i].zName)==0 ){
      sqliteErrorMsg(pParse, "duplicate column name: %s", z);
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
  pCol->sortOrder = SQLITE_SO_NUM;
  p->nCol++;
}

/*
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.  A "NOT NULL" constraint has
** been seen on a column.  This routine sets the notNull flag on
** the column currently under construction.
*/
void sqliteAddNotNull(Parse *pParse, int onError){
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
void sqliteAddColumnType(Parse *pParse, Token *pFirst, Token *pLast){
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
  n = pLast->n + Addr(pLast->z) - Addr(pFirst->z);
  sqliteSetNString(pz, pFirst->z, n, 0);
  z = *pz;
  if( z==0 ) return;
  for(i=j=0; z[i]; i++){
    int c = z[i];
    if( isspace(c) ) continue;
    z[j++] = c;
  }
  z[j] = 0;
  if( pParse->db->file_format>=4 ){
    pCol->sortOrder = sqliteCollateType(z, n);
  }else{
    pCol->sortOrder = SQLITE_SO_NUM;
  }
}

/*
** The given token is the default value for the last column added to
** the table currently under construction.  If "minusFlag" is true, it
** means the value token was preceded by a minus sign.
**
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.
*/
void sqliteAddDefaultValue(Parse *pParse, Token *pVal, int minusFlag){
  Table *p;
  int i;
  char **pz;
  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i<0 ) return;
  pz = &p->aCol[i].zDflt;
  if( minusFlag ){
    sqliteSetNString(pz, "-", 1, pVal->z, pVal->n, 0);
  }else{
    sqliteSetNString(pz, pVal->z, pVal->n, 0);
  }
  sqliteDequote(*pz);
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
void sqliteAddPrimaryKey(Parse *pParse, IdList *pList, int onError){
  Table *pTab = pParse->pNewTable;
  char *zType = 0;
  int iCol = -1, i;
  if( pTab==0 ) goto primary_key_exit;
  if( pTab->hasPrimKey ){
    sqliteErrorMsg(pParse, 
      "table \"%s\" has more than one primary key", pTab->zName);
    goto primary_key_exit;
  }
  pTab->hasPrimKey = 1;
  if( pList==0 ){
    iCol = pTab->nCol - 1;
    pTab->aCol[iCol].isPrimKey = 1;
  }else{
    for(i=0; i<pList->nId; i++){
      for(iCol=0; iCol<pTab->nCol; iCol++){
        if( sqliteStrICmp(pList->a[i].zName, pTab->aCol[iCol].zName)==0 ) break;
      }
      if( iCol<pTab->nCol ) pTab->aCol[iCol].isPrimKey = 1;
    }
    if( pList->nId>1 ) iCol = -1;
  }
  if( iCol>=0 && iCol<pTab->nCol ){
    zType = pTab->aCol[iCol].zType;
  }
  if( pParse->db->file_format>=1 && 
           zType && sqliteStrICmp(zType, "INTEGER")==0 ){
    pTab->iPKey = iCol;
    pTab->keyConf = onError;
  }else{
    sqliteCreateIndex(pParse, 0, 0, pList, onError, 0, 0);
    pList = 0;
  }

primary_key_exit:
  sqliteIdListDelete(pList);
  return;
}

/*
** Return the appropriate collating type given a type name.
**
** The collation type is text (SQLITE_SO_TEXT) if the type
** name contains the character stream "text" or "blob" or
** "clob".  Any other type name is collated as numeric
** (SQLITE_SO_NUM).
*/
int sqliteCollateType(const char *zType, int nType){
  int i;
  for(i=0; i<nType-3; i++){
    int c = *(zType++) | 0x60;
    if( (c=='b' || c=='c') && sqliteStrNICmp(zType, "lob", 3)==0 ){
      return SQLITE_SO_TEXT;
    }
    if( c=='c' && sqliteStrNICmp(zType, "har", 3)==0 ){
      return SQLITE_SO_TEXT;
    }
    if( c=='t' && sqliteStrNICmp(zType, "ext", 3)==0 ){
      return SQLITE_SO_TEXT;
    }
  }
  return SQLITE_SO_NUM;
}

/*
** This routine is called by the parser while in the middle of
** parsing a CREATE TABLE statement.  A "COLLATE" clause has
** been seen on a column.  This routine sets the Column.sortOrder on
** the column currently under construction.
*/
void sqliteAddCollateType(Parse *pParse, int collType){
  Table *p;
  int i;
  if( (p = pParse->pNewTable)==0 ) return;
  i = p->nCol-1;
  if( i>=0 ) p->aCol[i].sortOrder = collType;
}

/*
** Come up with a new random value for the schema cookie.  Make sure
** the new value is different from the old.
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
void sqliteChangeCookie(sqlite *db, Vdbe *v){
  if( db->next_cookie==db->aDb[0].schema_cookie ){
    unsigned char r;
    sqliteRandomness(1, &r);
    db->next_cookie = db->aDb[0].schema_cookie + r + 1;
    db->flags |= SQLITE_InternChanges;
    sqliteVdbeAddOp(v, OP_Integer, db->next_cookie, 0);
    sqliteVdbeAddOp(v, OP_SetCookie, 0, 0);
  }
}

/*
** Measure the number of characters needed to output the given
** identifier.  The number returned includes any quotes used
** but does not include the null terminator.
*/
static int identLength(const char *z){
  int n;
  int needQuote = 0;
  for(n=0; *z; n++, z++){
    if( *z=='\'' ){ n++; needQuote=1; }
  }
  return n + needQuote*2;
}

/*
** Write an identifier onto the end of the given string.  Add
** quote characters as needed.
*/
static void identPut(char *z, int *pIdx, char *zIdent){
  int i, j, needQuote;
  i = *pIdx;
  for(j=0; zIdent[j]; j++){
    if( !isalnum(zIdent[j]) && zIdent[j]!='_' ) break;
  }
  needQuote =  zIdent[j]!=0 || isdigit(zIdent[0])
                  || sqliteKeywordCode(zIdent, j)!=TK_ID;
  if( needQuote ) z[i++] = '\'';
  for(j=0; zIdent[j]; j++){
    z[i++] = zIdent[j];
    if( zIdent[j]=='\'' ) z[i++] = '\'';
  }
  if( needQuote ) z[i++] = '\'';
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
  char *zSep, *zSep2, *zEnd;
  n = 0;
  for(i=0; i<p->nCol; i++){
    n += identLength(p->aCol[i].zName);
  }
  n += identLength(p->zName);
  if( n<40 ){
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
  for(i=0; i<p->nCol; i++){
    strcpy(&zStmt[k], zSep);
    k += strlen(&zStmt[k]);
    zSep = zSep2;
    identPut(zStmt, &k, p->aCol[i].zName);
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
void sqliteEndTable(Parse *pParse, Token *pEnd, Select *pSelect){
  Table *p;
  sqlite *db = pParse->db;

  if( (pEnd==0 && pSelect==0) || pParse->nErr || sqlite_malloc_failed ) return;
  p = pParse->pNewTable;
  if( p==0 ) return;

  /* If the table is generated from a SELECT, then construct the
  ** list of columns and the text of the table.
  */
  if( pSelect ){
    Table *pSelTab = sqliteResultSetOfSelect(pParse, 0, pSelect);
    if( pSelTab==0 ) return;
    assert( p->aCol==0 );
    p->nCol = pSelTab->nCol;
    p->aCol = pSelTab->aCol;
    pSelTab->nCol = 0;
    pSelTab->aCol = 0;
    sqliteDeleteTable(0, pSelTab);
  }

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

    v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( p->pSelect==0 ){
      /* A regular table */
      sqliteVdbeOp3(v, OP_CreateTable, 0, p->iDb, (char*)&p->tnum, P3_POINTER);
    }else{
      /* A view */
      sqliteVdbeAddOp(v, OP_Integer, 0, 0);
    }
    p->tnum = 0;
    sqliteVdbeAddOp(v, OP_Pull, 1, 0);
    sqliteVdbeOp3(v, OP_String, 0, 0, p->pSelect==0?"table":"view", P3_STATIC);
    sqliteVdbeOp3(v, OP_String, 0, 0, p->zName, 0);
    sqliteVdbeOp3(v, OP_String, 0, 0, p->zName, 0);
    sqliteVdbeAddOp(v, OP_Dup, 4, 0);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    if( pSelect ){
      char *z = createTableStmt(p);
      n = z ? strlen(z) : 0;
      sqliteVdbeChangeP3(v, -1, z, n);
      sqliteFree(z);
    }else{
      assert( pEnd!=0 );
      n = Addr(pEnd->z) - Addr(pParse->sFirstToken.z) + 1;
      sqliteVdbeChangeP3(v, -1, pParse->sFirstToken.z, n);
    }
    sqliteVdbeAddOp(v, OP_MakeRecord, 5, 0);
    sqliteVdbeAddOp(v, OP_PutIntKey, 0, 0);
    if( !p->iDb ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    if( pSelect ){
      sqliteVdbeAddOp(v, OP_Integer, p->iDb, 0);
      sqliteVdbeAddOp(v, OP_OpenWrite, 1, 0);
      pParse->nTab = 2;
      sqliteSelect(pParse, pSelect, SRT_Table, 1, 0, 0, 0);
    }
    sqliteEndWriteOperation(pParse);
  }

  /* Add the table to the in-memory representation of the database.
  */
  if( pParse->explain==0 && pParse->nErr==0 ){
    Table *pOld;
    FKey *pFKey;
    pOld = sqliteHashInsert(&db->aDb[p->iDb].tblHash, 
                            p->zName, strlen(p->zName)+1, p);
    if( pOld ){
      assert( p==pOld );  /* Malloc must have failed inside HashInsert() */
      return;
    }
    for(pFKey=p->pFKey; pFKey; pFKey=pFKey->pNextFrom){
      int nTo = strlen(pFKey->zTo) + 1;
      pFKey->pNextTo = sqliteHashFind(&db->aDb[p->iDb].aFKey, pFKey->zTo, nTo);
      sqliteHashInsert(&db->aDb[p->iDb].aFKey, pFKey->zTo, nTo, pFKey);
    }
    pParse->pNewTable = 0;
    db->nTable++;
    db->flags |= SQLITE_InternChanges;
  }
}

/*
** The parser calls this routine in order to create a new VIEW
*/
void sqliteCreateView(
  Parse *pParse,     /* The parsing context */
  Token *pBegin,     /* The CREATE token that begins the statement */
  Token *pName,      /* The token that holds the name of the view */
  Select *pSelect,   /* A SELECT statement that will become the new view */
  int isTemp         /* TRUE for a TEMPORARY view */
){
  Table *p;
  int n;
  const char *z;
  Token sEnd;
  DbFixer sFix;

  sqliteStartTable(pParse, pBegin, pName, isTemp, 1);
  p = pParse->pNewTable;
  if( p==0 || pParse->nErr ){
    sqliteSelectDelete(pSelect);
    return;
  }
  if( sqliteFixInit(&sFix, pParse, p->iDb, "view", pName)
    && sqliteFixSelect(&sFix, pSelect)
  ){
    sqliteSelectDelete(pSelect);
    return;
  }

  /* Make a copy of the entire SELECT statement that defines the view.
  ** This will force all the Expr.token.z values to be dynamically
  ** allocated rather than point to the input string - which means that
  ** they will persist after the current sqlite_exec() call returns.
  */
  p->pSelect = sqliteSelectDup(pSelect);
  sqliteSelectDelete(pSelect);
  if( !pParse->db->init.busy ){
    sqliteViewGetColumnNames(pParse, p);
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
  z = pBegin->z;
  while( n>0 && (z[n-1]==';' || isspace(z[n-1])) ){ n--; }
  sEnd.z = &z[n-1];
  sEnd.n = 1;

  /* Use sqliteEndTable() to add the view to the SQLITE_MASTER table */
  sqliteEndTable(pParse, &sEnd, 0);
  return;
}

/*
** The Table structure pTable is really a VIEW.  Fill in the names of
** the columns of the view in the pTable structure.  Return the number
** of errors.  If an error is seen leave an error message in pParse->zErrMsg.
*/
int sqliteViewGetColumnNames(Parse *pParse, Table *pTable){
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
    sqliteErrorMsg(pParse, "view %s is circularly defined", pTable->zName);
    return 1;
  }

  /* If we get this far, it means we need to compute the table names.
  */
  assert( pTable->pSelect ); /* If nCol==0, then pTable must be a VIEW */
  pSel = pTable->pSelect;

  /* Note that the call to sqliteResultSetOfSelect() will expand any
  ** "*" elements in this list.  But we will need to restore the list
  ** back to its original configuration afterwards, so we save a copy of
  ** the original in pEList.
  */
  pEList = pSel->pEList;
  pSel->pEList = sqliteExprListDup(pEList);
  if( pSel->pEList==0 ){
    pSel->pEList = pEList;
    return 1;  /* Malloc failed */
  }
  pTable->nCol = -1;
  pSelTab = sqliteResultSetOfSelect(pParse, 0, pSel);
  if( pSelTab ){
    assert( pTable->aCol==0 );
    pTable->nCol = pSelTab->nCol;
    pTable->aCol = pSelTab->aCol;
    pSelTab->nCol = 0;
    pSelTab->aCol = 0;
    sqliteDeleteTable(0, pSelTab);
    DbSetProperty(pParse->db, pTable->iDb, DB_UnresetViews);
  }else{
    pTable->nCol = 0;
    nErr++;
  }
  sqliteSelectUnbind(pSel);
  sqliteExprListDelete(pSel->pEList);
  pSel->pEList = pEList;
  return nErr;  
}

/*
** Clear the column names from the VIEW pTable.
**
** This routine is called whenever any other table or view is modified.
** The view passed into this routine might depend directly or indirectly
** on the modified or deleted table so we need to clear the old column
** names so that they will be recomputed.
*/
static void sqliteViewResetColumnNames(Table *pTable){
  int i;
  Column *pCol;
  assert( pTable!=0 && pTable->pSelect!=0 );
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
** Clear the column names from every VIEW in database idx.
*/
static void sqliteViewResetAll(sqlite *db, int idx){
  HashElem *i;
  if( !DbHasProperty(db, idx, DB_UnresetViews) ) return;
  for(i=sqliteHashFirst(&db->aDb[idx].tblHash); i; i=sqliteHashNext(i)){
    Table *pTab = sqliteHashData(i);
    if( pTab->pSelect ){
      sqliteViewResetColumnNames(pTab);
    }
  }
  DbClearProperty(db, idx, DB_UnresetViews);
}

/*
** Given a token, look up a table with that name.  If not found, leave
** an error for the parser to find and return NULL.
*/
Table *sqliteTableFromToken(Parse *pParse, Token *pTok){
  char *zName;
  Table *pTab;
  zName = sqliteTableNameFromToken(pTok);
  if( zName==0 ) return 0;
  pTab = sqliteFindTable(pParse->db, zName, 0);
  sqliteFree(zName);
  if( pTab==0 ){
    sqliteErrorMsg(pParse, "no such table: %T", pTok);
  }
  return pTab;
}

/*
** This routine is called to do the work of a DROP TABLE statement.
** pName is the name of the table to be dropped.
*/
void sqliteDropTable(Parse *pParse, Token *pName, int isView){
  Table *pTable;
  Vdbe *v;
  int base;
  sqlite *db = pParse->db;
  int iDb;

  if( pParse->nErr || sqlite_malloc_failed ) return;
  pTable = sqliteTableFromToken(pParse, pName);
  if( pTable==0 ) return;
  iDb = pTable->iDb;
  assert( iDb>=0 && iDb<db->nDb );
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code;
    const char *zTab = SCHEMA_TABLE(pTable->iDb);
    const char *zDb = db->aDb[pTable->iDb].zName;
    if( sqliteAuthCheck(pParse, SQLITE_DELETE, zTab, 0, zDb)){
      return;
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
    if( sqliteAuthCheck(pParse, code, pTable->zName, 0, zDb) ){
      return;
    }
    if( sqliteAuthCheck(pParse, SQLITE_DELETE, pTable->zName, 0, zDb) ){
      return;
    }
  }
#endif
  if( pTable->readOnly ){
    sqliteErrorMsg(pParse, "table %s may not be dropped", pTable->zName);
    pParse->nErr++;
    return;
  }
  if( isView && pTable->pSelect==0 ){
    sqliteErrorMsg(pParse, "use DROP TABLE to delete table %s", pTable->zName);
    return;
  }
  if( !isView && pTable->pSelect ){
    sqliteErrorMsg(pParse, "use DROP VIEW to delete view %s", pTable->zName);
    return;
  }

  /* Generate code to remove the table from the master table
  ** on disk.
  */
  v = sqliteGetVdbe(pParse);
  if( v ){
    static VdbeOpList dropTable[] = {
      { OP_Rewind,     0, ADDR(8),  0},
      { OP_String,     0, 0,        0}, /* 1 */
      { OP_MemStore,   1, 1,        0},
      { OP_MemLoad,    1, 0,        0}, /* 3 */
      { OP_Column,     0, 2,        0},
      { OP_Ne,         0, ADDR(7),  0},
      { OP_Delete,     0, 0,        0},
      { OP_Next,       0, ADDR(3),  0}, /* 7 */
    };
    Index *pIdx;
    Trigger *pTrigger;
    sqliteBeginWriteOperation(pParse, 0, pTable->iDb);

    /* Drop all triggers associated with the table being dropped */
    pTrigger = pTable->pTrigger;
    while( pTrigger ){
      assert( pTrigger->iDb==pTable->iDb || pTrigger->iDb==1 );
      sqliteDropTriggerPtr(pParse, pTrigger, 1);
      if( pParse->explain ){
        pTrigger = pTrigger->pNext;
      }else{
        pTrigger = pTable->pTrigger;
      }
    }

    /* Drop all SQLITE_MASTER entries that refer to the table */
    sqliteOpenMasterTable(v, pTable->iDb);
    base = sqliteVdbeAddOpList(v, ArraySize(dropTable), dropTable);
    sqliteVdbeChangeP3(v, base+1, pTable->zName, 0);

    /* Drop all SQLITE_TEMP_MASTER entries that refer to the table */
    if( pTable->iDb!=1 ){
      sqliteOpenMasterTable(v, 1);
      base = sqliteVdbeAddOpList(v, ArraySize(dropTable), dropTable);
      sqliteVdbeChangeP3(v, base+1, pTable->zName, 0);
    }

    if( pTable->iDb==0 ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    if( !isView ){
      sqliteVdbeAddOp(v, OP_Destroy, pTable->tnum, pTable->iDb);
      for(pIdx=pTable->pIndex; pIdx; pIdx=pIdx->pNext){
        sqliteVdbeAddOp(v, OP_Destroy, pIdx->tnum, pIdx->iDb);
      }
    }
    sqliteEndWriteOperation(pParse);
  }

  /* Delete the in-memory description of the table.
  **
  ** Exception: if the SQL statement began with the EXPLAIN keyword,
  ** then no changes should be made.
  */
  if( !pParse->explain ){
    sqliteUnlinkAndDeleteTable(db, pTable);
    db->flags |= SQLITE_InternChanges;
  }
  sqliteViewResetAll(db, iDb);
}

/*
** This routine constructs a P3 string suitable for an OP_MakeIdxKey
** opcode and adds that P3 string to the most recently inserted instruction
** in the virtual machine.  The P3 string consists of a single character
** for each column in the index pIdx of table pTab.  If the column uses
** a numeric sort order, then the P3 string character corresponding to
** that column is 'n'.  If the column uses a text sort order, then the
** P3 string is 't'.  See the OP_MakeIdxKey opcode documentation for
** additional information.  See also the sqliteAddKeyType() routine.
*/
void sqliteAddIdxKeyType(Vdbe *v, Index *pIdx){
  char *zType;
  Table *pTab;
  int i, n;
  assert( pIdx!=0 && pIdx->pTable!=0 );
  pTab = pIdx->pTable;
  n = pIdx->nColumn;
  zType = sqliteMallocRaw( n+1 );
  if( zType==0 ) return;
  for(i=0; i<n; i++){
    int iCol = pIdx->aiColumn[i];
    assert( iCol>=0 && iCol<pTab->nCol );
    if( (pTab->aCol[iCol].sortOrder & SQLITE_SO_TYPEMASK)==SQLITE_SO_TEXT ){
      zType[i] = 't';
    }else{
      zType[i] = 'n';
    }
  }
  zType[n] = 0;
  sqliteVdbeChangeP3(v, -1, zType, n);
  sqliteFree(zType);
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
** until sqliteEndTable().
**
** The foreign key is set for IMMEDIATE processing.  A subsequent call
** to sqliteDeferForeignKey() might change this to DEFERRED.
*/
void sqliteCreateForeignKey(
  Parse *pParse,       /* Parsing context */
  IdList *pFromCol,    /* Columns in this table that point to other table */
  Token *pTo,          /* Name of the other table */
  IdList *pToCol,      /* Columns in the other table */
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
    if( pToCol && pToCol->nId!=1 ){
      sqliteErrorMsg(pParse, "foreign key on %s"
         " should reference only one column of table %T",
         p->aCol[iCol].zName, pTo);
      goto fk_end;
    }
    nCol = 1;
  }else if( pToCol && pToCol->nId!=pFromCol->nId ){
    sqliteErrorMsg(pParse,
        "number of columns in foreign key does not match the number of "
        "columns in the referenced table");
    goto fk_end;
  }else{
    nCol = pFromCol->nId;
  }
  nByte = sizeof(*pFKey) + nCol*sizeof(pFKey->aCol[0]) + pTo->n + 1;
  if( pToCol ){
    for(i=0; i<pToCol->nId; i++){
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
        if( sqliteStrICmp(p->aCol[j].zName, pFromCol->a[i].zName)==0 ){
          pFKey->aCol[i].iFrom = j;
          break;
        }
      }
      if( j>=p->nCol ){
        sqliteErrorMsg(pParse, 
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
  sqliteIdListDelete(pFromCol);
  sqliteIdListDelete(pToCol);
}

/*
** This routine is called when an INITIALLY IMMEDIATE or INITIALLY DEFERRED
** clause is seen as part of a foreign key definition.  The isDeferred
** parameter is 1 for INITIALLY DEFERRED and 0 for INITIALLY IMMEDIATE.
** The behavior of the most recently created foreign key is adjusted
** accordingly.
*/
void sqliteDeferForeignKey(Parse *pParse, int isDeferred){
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
void sqliteCreateIndex(
  Parse *pParse,   /* All information about this parse */
  Token *pName,    /* Name of the index.  May be NULL */
  SrcList *pTable, /* Name of the table to index.  Use pParse->pNewTable if 0 */
  IdList *pList,   /* A list of columns to be indexed */
  int onError,     /* OE_Abort, OE_Ignore, OE_Replace, or OE_None */
  Token *pStart,   /* The CREATE token that begins a CREATE TABLE statement */
  Token *pEnd      /* The ")" that closes the CREATE INDEX statement */
){
  Table *pTab;     /* Table to be indexed */
  Index *pIndex;   /* The index to be created */
  char *zName = 0;
  int i, j;
  Token nullId;    /* Fake token for an empty ID list */
  DbFixer sFix;    /* For assigning database names to pTable */
  int isTemp;      /* True for a temporary index */
  sqlite *db = pParse->db;

  if( pParse->nErr || sqlite_malloc_failed ) goto exit_create_index;
  if( db->init.busy 
     && sqliteFixInit(&sFix, pParse, db->init.iDb, "index", pName)
     && sqliteFixSrcList(&sFix, pTable)
  ){
    goto exit_create_index;
  }

  /*
  ** Find the table that is to be indexed.  Return early if not found.
  */
  if( pTable!=0 ){
    assert( pName!=0 );
    assert( pTable->nSrc==1 );
    pTab =  sqliteSrcListLookup(pParse, pTable);
  }else{
    assert( pName==0 );
    pTab =  pParse->pNewTable;
  }
  if( pTab==0 || pParse->nErr ) goto exit_create_index;
  if( pTab->readOnly ){
    sqliteErrorMsg(pParse, "table %s may not be indexed", pTab->zName);
    goto exit_create_index;
  }
  if( pTab->iDb>=2 && db->init.busy==0 ){
    sqliteErrorMsg(pParse, "table %s may not have indices added", pTab->zName);
    goto exit_create_index;
  }
  if( pTab->pSelect ){
    sqliteErrorMsg(pParse, "views may not be indexed");
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
  if( pName && !db->init.busy ){
    Index *pISameName;    /* Another index with the same name */
    Table *pTSameName;    /* A table with same name as the index */
    zName = sqliteStrNDup(pName->z, pName->n);
    if( zName==0 ) goto exit_create_index;
    if( (pISameName = sqliteFindIndex(db, zName, 0))!=0 ){
      sqliteErrorMsg(pParse, "index %s already exists", zName);
      goto exit_create_index;
    }
    if( (pTSameName = sqliteFindTable(db, zName, 0))!=0 ){
      sqliteErrorMsg(pParse, "there is already a table named %s", zName);
      goto exit_create_index;
    }
  }else if( pName==0 ){
    char zBuf[30];
    int n;
    Index *pLoop;
    for(pLoop=pTab->pIndex, n=1; pLoop; pLoop=pLoop->pNext, n++){}
    sprintf(zBuf,"%d)",n);
    zName = 0;
    sqliteSetString(&zName, "(", pTab->zName, " autoindex ", zBuf, (char*)0);
    if( zName==0 ) goto exit_create_index;
  }else{
    zName = sqliteStrNDup(pName->z, pName->n);
  }

  /* Check for authorization to create an index.
  */
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    const char *zDb = db->aDb[pTab->iDb].zName;

    assert( pTab->iDb==db->init.iDb || isTemp );
    if( sqliteAuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0, zDb) ){
      goto exit_create_index;
    }
    i = SQLITE_CREATE_INDEX;
    if( isTemp ) i = SQLITE_CREATE_TEMP_INDEX;
    if( sqliteAuthCheck(pParse, i, zName, pTab->zName, zDb) ){
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
    pList = sqliteIdListAppend(0, &nullId);
    if( pList==0 ) goto exit_create_index;
  }

  /* 
  ** Allocate the index structure. 
  */
  pIndex = sqliteMalloc( sizeof(Index) + strlen(zName) + 1 +
                        sizeof(int)*pList->nId );
  if( pIndex==0 ) goto exit_create_index;
  pIndex->aiColumn = (int*)&pIndex[1];
  pIndex->zName = (char*)&pIndex->aiColumn[pList->nId];
  strcpy(pIndex->zName, zName);
  pIndex->pTable = pTab;
  pIndex->nColumn = pList->nId;
  pIndex->onError = onError;
  pIndex->autoIndex = pName==0;
  pIndex->iDb = isTemp ? 1 : db->init.iDb;

  /* Scan the names of the columns of the table to be indexed and
  ** load the column indices into the Index structure.  Report an error
  ** if any column is not found.
  */
  for(i=0; i<pList->nId; i++){
    for(j=0; j<pTab->nCol; j++){
      if( sqliteStrICmp(pList->a[i].zName, pTab->aCol[j].zName)==0 ) break;
    }
    if( j>=pTab->nCol ){
      sqliteErrorMsg(pParse, "table %s has no column named %s",
        pTab->zName, pList->a[i].zName);
      sqliteFree(pIndex);
      goto exit_create_index;
    }
    pIndex->aiColumn[i] = j;
  }

  /* Link the new Index structure to its table and to the other
  ** in-memory database structures. 
  */
  if( !pParse->explain ){
    Index *p;
    p = sqliteHashInsert(&db->aDb[pIndex->iDb].idxHash, 
                         pIndex->zName, strlen(pIndex->zName)+1, pIndex);
    if( p ){
      assert( p==pIndex );  /* Malloc must have failed */
      sqliteFree(pIndex);
      goto exit_create_index;
    }
    db->flags |= SQLITE_InternChanges;
  }

  /* When adding an index to the list of indices for a table, make
  ** sure all indices labeled OE_Replace come after all those labeled
  ** OE_Ignore.  This is necessary for the correct operation of UPDATE
  ** and INSERT.
  */
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

  /* If the db->init.busy is 1 it means we are reading the SQL off the
  ** "sqlite_master" table on the disk.  So do not write to the disk
  ** again.  Extract the table number from the db->init.newTnum field.
  */
  if( db->init.busy && pTable!=0 ){
    pIndex->tnum = db->init.newTnum;
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
  ** If pTable==0 it means this index is generated as a primary key
  ** or UNIQUE constraint of a CREATE TABLE statement.  Since the table
  ** has just been created, it contains no data and the index initialization
  ** step can be skipped.
  */
  else if( db->init.busy==0 ){
    int n;
    Vdbe *v;
    int lbl1, lbl2;
    int i;
    int addr;

    v = sqliteGetVdbe(pParse);
    if( v==0 ) goto exit_create_index;
    if( pTable!=0 ){
      sqliteBeginWriteOperation(pParse, 0, isTemp);
      sqliteOpenMasterTable(v, isTemp);
    }
    sqliteVdbeAddOp(v, OP_NewRecno, 0, 0);
    sqliteVdbeOp3(v, OP_String, 0, 0, "index", P3_STATIC);
    sqliteVdbeOp3(v, OP_String, 0, 0, pIndex->zName, 0);
    sqliteVdbeOp3(v, OP_String, 0, 0, pTab->zName, 0);
    sqliteVdbeOp3(v, OP_CreateIndex, 0, isTemp,(char*)&pIndex->tnum,P3_POINTER);
    pIndex->tnum = 0;
    if( pTable ){
      sqliteVdbeCode(v,
          OP_Dup,       0,      0,
          OP_Integer,   isTemp, 0,
          OP_OpenWrite, 1,      0,
      0);
    }
    addr = sqliteVdbeAddOp(v, OP_String, 0, 0);
    if( pStart && pEnd ){
      n = Addr(pEnd->z) - Addr(pStart->z) + 1;
      sqliteVdbeChangeP3(v, addr, pStart->z, n);
    }
    sqliteVdbeAddOp(v, OP_MakeRecord, 5, 0);
    sqliteVdbeAddOp(v, OP_PutIntKey, 0, 0);
    if( pTable ){
      sqliteVdbeAddOp(v, OP_Integer, pTab->iDb, 0);
      sqliteVdbeOp3(v, OP_OpenRead, 2, pTab->tnum, pTab->zName, 0);
      lbl2 = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_Rewind, 2, lbl2);
      lbl1 = sqliteVdbeAddOp(v, OP_Recno, 2, 0);
      for(i=0; i<pIndex->nColumn; i++){
        int iCol = pIndex->aiColumn[i];
        if( pTab->iPKey==iCol ){
          sqliteVdbeAddOp(v, OP_Dup, i, 0);
        }else{
          sqliteVdbeAddOp(v, OP_Column, 2, iCol);
        }
      }
      sqliteVdbeAddOp(v, OP_MakeIdxKey, pIndex->nColumn, 0);
      if( db->file_format>=4 ) sqliteAddIdxKeyType(v, pIndex);
      sqliteVdbeOp3(v, OP_IdxPut, 1, pIndex->onError!=OE_None,
                      "indexed columns are not unique", P3_STATIC);
      sqliteVdbeAddOp(v, OP_Next, 2, lbl1);
      sqliteVdbeResolveLabel(v, lbl2);
      sqliteVdbeAddOp(v, OP_Close, 2, 0);
      sqliteVdbeAddOp(v, OP_Close, 1, 0);
    }
    if( pTable!=0 ){
      if( !isTemp ){
        sqliteChangeCookie(db, v);
      }
      sqliteVdbeAddOp(v, OP_Close, 0, 0);
      sqliteEndWriteOperation(pParse);
    }
  }

  /* Clean up before exiting */
exit_create_index:
  sqliteIdListDelete(pList);
  sqliteSrcListDelete(pTable);
  sqliteFree(zName);
  return;
}

/*
** This routine will drop an existing named index.  This routine
** implements the DROP INDEX statement.
*/
void sqliteDropIndex(Parse *pParse, SrcList *pName){
  Index *pIndex;
  Vdbe *v;
  sqlite *db = pParse->db;

  if( pParse->nErr || sqlite_malloc_failed ) return;
  assert( pName->nSrc==1 );
  pIndex = sqliteFindIndex(db, pName->a[0].zName, pName->a[0].zDatabase);
  if( pIndex==0 ){
    sqliteErrorMsg(pParse, "no such index: %S", pName, 0);
    goto exit_drop_index;
  }
  if( pIndex->autoIndex ){
    sqliteErrorMsg(pParse, "index associated with UNIQUE "
      "or PRIMARY KEY constraint cannot be dropped", 0);
    goto exit_drop_index;
  }
  if( pIndex->iDb>1 ){
    sqliteErrorMsg(pParse, "cannot alter schema of attached "
       "databases", 0);
    goto exit_drop_index;
  }
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_DROP_INDEX;
    Table *pTab = pIndex->pTable;
    const char *zDb = db->aDb[pIndex->iDb].zName;
    const char *zTab = SCHEMA_TABLE(pIndex->iDb);
    if( sqliteAuthCheck(pParse, SQLITE_DELETE, zTab, 0, zDb) ){
      goto exit_drop_index;
    }
    if( pIndex->iDb ) code = SQLITE_DROP_TEMP_INDEX;
    if( sqliteAuthCheck(pParse, code, pIndex->zName, pTab->zName, zDb) ){
      goto exit_drop_index;
    }
  }
#endif

  /* Generate code to remove the index and from the master table */
  v = sqliteGetVdbe(pParse);
  if( v ){
    static VdbeOpList dropIndex[] = {
      { OP_Rewind,     0, ADDR(9), 0}, 
      { OP_String,     0, 0,       0}, /* 1 */
      { OP_MemStore,   1, 1,       0},
      { OP_MemLoad,    1, 0,       0}, /* 3 */
      { OP_Column,     0, 1,       0},
      { OP_Eq,         0, ADDR(8), 0},
      { OP_Next,       0, ADDR(3), 0},
      { OP_Goto,       0, ADDR(9), 0},
      { OP_Delete,     0, 0,       0}, /* 8 */
    };
    int base;

    sqliteBeginWriteOperation(pParse, 0, pIndex->iDb);
    sqliteOpenMasterTable(v, pIndex->iDb);
    base = sqliteVdbeAddOpList(v, ArraySize(dropIndex), dropIndex);
    sqliteVdbeChangeP3(v, base+1, pIndex->zName, 0);
    if( pIndex->iDb==0 ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    sqliteVdbeAddOp(v, OP_Destroy, pIndex->tnum, pIndex->iDb);
    sqliteEndWriteOperation(pParse);
  }

  /* Delete the in-memory description of this index.
  */
  if( !pParse->explain ){
    sqliteUnlinkAndDeleteIndex(db, pIndex);
    db->flags |= SQLITE_InternChanges;
  }

exit_drop_index:
  sqliteSrcListDelete(pName);
}

/*
** Append a new element to the given IdList.  Create a new IdList if
** need be.
**
** A new IdList is returned, or NULL if malloc() fails.
*/
IdList *sqliteIdListAppend(IdList *pList, Token *pToken){
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
      sqliteIdListDelete(pList);
      return 0;
    }
    pList->a = a;
  }
  memset(&pList->a[pList->nId], 0, sizeof(pList->a[0]));
  if( pToken ){
    char **pz = &pList->a[pList->nId].zName;
    sqliteSetNString(pz, pToken->z, pToken->n, 0);
    if( *pz==0 ){
      sqliteIdListDelete(pList);
      return 0;
    }else{
      sqliteDequote(*pz);
    }
  }
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
**         sqliteSrcListAppend(A,B,0);
**
** Then B is a table name and the database name is unspecified.  If called
** like this:
**
**         sqliteSrcListAppend(A,B,C);
**
** Then C is the table name and B is the database name.
*/
SrcList *sqliteSrcListAppend(SrcList *pList, Token *pTable, Token *pDatabase){
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
      sqliteSrcListDelete(pList);
      return 0;
    }
    pList = pNew;
  }
  memset(&pList->a[pList->nSrc], 0, sizeof(pList->a[0]));
  if( pDatabase && pDatabase->z==0 ){
    pDatabase = 0;
  }
  if( pDatabase && pTable ){
    Token *pTemp = pDatabase;
    pDatabase = pTable;
    pTable = pTemp;
  }
  if( pTable ){
    char **pz = &pList->a[pList->nSrc].zName;
    sqliteSetNString(pz, pTable->z, pTable->n, 0);
    if( *pz==0 ){
      sqliteSrcListDelete(pList);
      return 0;
    }else{
      sqliteDequote(*pz);
    }
  }
  if( pDatabase ){
    char **pz = &pList->a[pList->nSrc].zDatabase;
    sqliteSetNString(pz, pDatabase->z, pDatabase->n, 0);
    if( *pz==0 ){
      sqliteSrcListDelete(pList);
      return 0;
    }else{
      sqliteDequote(*pz);
    }
  }
  pList->a[pList->nSrc].iCursor = -1;
  pList->nSrc++;
  return pList;
}

/*
** Assign cursors to all tables in a SrcList
*/
void sqliteSrcListAssignCursors(Parse *pParse, SrcList *pList){
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
void sqliteSrcListAddAlias(SrcList *pList, Token *pToken){
  if( pList && pList->nSrc>0 ){
    int i = pList->nSrc - 1;
    sqliteSetNString(&pList->a[i].zAlias, pToken->z, pToken->n, 0);
    sqliteDequote(pList->a[i].zAlias);
  }
}

/*
** Delete an IdList.
*/
void sqliteIdListDelete(IdList *pList){
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
int sqliteIdListIndex(IdList *pList, const char *zName){
  int i;
  if( pList==0 ) return -1;
  for(i=0; i<pList->nId; i++){
    if( sqliteStrICmp(pList->a[i].zName, zName)==0 ) return i;
  }
  return -1;
}

/*
** Delete an entire SrcList including all its substructure.
*/
void sqliteSrcListDelete(SrcList *pList){
  int i;
  if( pList==0 ) return;
  for(i=0; i<pList->nSrc; i++){
    sqliteFree(pList->a[i].zDatabase);
    sqliteFree(pList->a[i].zName);
    sqliteFree(pList->a[i].zAlias);
    if( pList->a[i].pTab && pList->a[i].pTab->isTransient ){
      sqliteDeleteTable(0, pList->a[i].pTab);
    }
    sqliteSelectDelete(pList->a[i].pSelect);
    sqliteExprDelete(pList->a[i].pOn);
    sqliteIdListDelete(pList->a[i].pUsing);
  }
  sqliteFree(pList);
}

/*
** Begin a transaction
*/
void sqliteBeginTransaction(Parse *pParse, int onError){
  sqlite *db;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "BEGIN", 0, 0) ) return;
  if( db->flags & SQLITE_InTrans ){
    sqliteErrorMsg(pParse, "cannot start a transaction within a transaction");
    return;
  }
  sqliteBeginWriteOperation(pParse, 0, 0);
  if( !pParse->explain ){
    db->flags |= SQLITE_InTrans;
    db->onError = onError;
  }
}

/*
** Commit a transaction
*/
void sqliteCommitTransaction(Parse *pParse){
  sqlite *db;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "COMMIT", 0, 0) ) return;
  if( (db->flags & SQLITE_InTrans)==0 ){
    sqliteErrorMsg(pParse, "cannot commit - no transaction is active");
    return;
  }
  if( !pParse->explain ){
    db->flags &= ~SQLITE_InTrans;
  }
  sqliteEndWriteOperation(pParse);
  if( !pParse->explain ){
    db->onError = OE_Default;
  }
}

/*
** Rollback a transaction
*/
void sqliteRollbackTransaction(Parse *pParse){
  sqlite *db;
  Vdbe *v;

  if( pParse==0 || (db=pParse->db)==0 || db->aDb[0].pBt==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "ROLLBACK", 0, 0) ) return;
  if( (db->flags & SQLITE_InTrans)==0 ){
    sqliteErrorMsg(pParse, "cannot rollback - no transaction is active");
    return; 
  }
  v = sqliteGetVdbe(pParse);
  if( v ){
    sqliteVdbeAddOp(v, OP_Rollback, 0, 0);
  }
  if( !pParse->explain ){
    db->flags &= ~SQLITE_InTrans;
    db->onError = OE_Default;
  }
}

/*
** Generate VDBE code that will verify the schema cookie for all
** named database files.
*/
void sqliteCodeVerifySchema(Parse *pParse, int iDb){
  sqlite *db = pParse->db;
  Vdbe *v = sqliteGetVdbe(pParse);
  assert( iDb>=0 && iDb<db->nDb );
  assert( db->aDb[iDb].pBt!=0 );
  if( iDb!=1 && !DbHasProperty(db, iDb, DB_Cookie) ){
    sqliteVdbeAddOp(v, OP_VerifyCookie, iDb, db->aDb[iDb].schema_cookie);
    DbSetProperty(db, iDb, DB_Cookie);
  }
}

/*
** Generate VDBE code that prepares for doing an operation that
** might change the database.
**
** This routine starts a new transaction if we are not already within
** a transaction.  If we are already within a transaction, then a checkpoint
** is set if the setCheckpoint parameter is true.  A checkpoint should
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
void sqliteBeginWriteOperation(Parse *pParse, int setCheckpoint, int iDb){
  Vdbe *v;
  sqlite *db = pParse->db;
  if( DbHasProperty(db, iDb, DB_Locked) ) return;
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return;
  if( !db->aDb[iDb].inTrans ){
    sqliteVdbeAddOp(v, OP_Transaction, iDb, 0);
    DbSetProperty(db, iDb, DB_Locked);
    sqliteCodeVerifySchema(pParse, iDb);
    if( iDb!=1 ){
      sqliteBeginWriteOperation(pParse, setCheckpoint, 1);
    }
  }else if( setCheckpoint ){
    sqliteVdbeAddOp(v, OP_Checkpoint, iDb, 0);
    DbSetProperty(db, iDb, DB_Locked);
  }
}

/*
** Generate code that concludes an operation that may have changed
** the database.  If a statement transaction was started, then emit
** an OP_Commit that will cause the changes to be committed to disk.
**
** Note that checkpoints are automatically committed at the end of
** a statement.  Note also that there can be multiple calls to 
** sqliteBeginWriteOperation() but there should only be a single
** call to sqliteEndWriteOperation() at the conclusion of the statement.
*/
void sqliteEndWriteOperation(Parse *pParse){
  Vdbe *v;
  sqlite *db = pParse->db;
  if( pParse->trigStack ) return; /* if this is in a trigger */
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return;
  if( db->flags & SQLITE_InTrans ){
    /* A BEGIN has executed.  Do not commit until we see an explicit
    ** COMMIT statement. */
  }else{
    sqliteVdbeAddOp(v, OP_Commit, 0, 0);
  }
}
