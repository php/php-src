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
**     COPY
**     VACUUM
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
  pParse->explain = explainFlag;
  if((db->flags & SQLITE_Initialized)==0 && pParse->initFlag==0 ){
    int rc = sqliteInit(db, &pParse->zErrMsg);
    if( rc!=SQLITE_OK ){
      pParse->rc = rc;
      pParse->nErr++;
    }
  }
}

/*
** This is a fake callback procedure used when sqlite_exec() is
** invoked with a NULL callback pointer.  If we pass a NULL callback
** pointer into sqliteVdbeExec() it will return at every OP_Callback,
** which we do not want it to do.  So we substitute a pointer to this
** procedure in place of the NULL.
*/
static int fakeCallback(void *NotUsed, int n, char **az1, char **az2){
  return 0;
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
  int rc = SQLITE_OK;
  sqlite *db = pParse->db;
  Vdbe *v = pParse->pVdbe;
  int (*xCallback)(void*,int,char**,char**);

  if( sqlite_malloc_failed ) return;
  xCallback = pParse->xCallback;
  if( xCallback==0 && pParse->useCallback ) xCallback = fakeCallback;
  if( v && pParse->nErr==0 ){
    FILE *trace = (db->flags & SQLITE_VdbeTrace)!=0 ? stdout : 0;
    sqliteVdbeTrace(v, trace);
    sqliteVdbeMakeReady(v, xCallback, pParse->pArg, pParse->explain);
    if( pParse->useCallback ){
      if( pParse->explain ){
        rc = sqliteVdbeList(v);
        db->next_cookie = db->schema_cookie;
      }else{
        sqliteVdbeExec(v);
      }
      rc = sqliteVdbeFinalize(v, &pParse->zErrMsg);
      if( rc ) pParse->nErr++;
      pParse->pVdbe = 0;
      pParse->rc = rc;
      if( rc ) pParse->nErr++;
    }else{
      pParse->rc = pParse->nErr ? SQLITE_ERROR : SQLITE_DONE;
    }
    pParse->colNamesSet = 0;
    pParse->schemaVerified = 0;
  }else if( pParse->useCallback==0 ){
    pParse->rc = SQLITE_ERROR;
  }
  pParse->nTab = 0;
  pParse->nMem = 0;
  pParse->nSet = 0;
  pParse->nAgg = 0;
}

/*
** Locate the in-memory structure that describes 
** a particular database table given the name
** of that table.  Return NULL if not found.
*/
Table *sqliteFindTable(sqlite *db, const char *zName){
  Table *p;
  p = sqliteHashFind(&db->tblHash, zName, strlen(zName)+1);
  return p;
}

/*
** Locate the in-memory structure that describes 
** a particular index given the name of that index.
** Return NULL if not found.
*/
Index *sqliteFindIndex(sqlite *db, const char *zName){
  Index *p;
  p = sqliteHashFind(&db->idxHash, zName, strlen(zName)+1);
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
  pOld = sqliteHashInsert(&db->idxHash, p->zName, strlen(p->zName)+1, 0);
  if( pOld!=0 && pOld!=p ){
    sqliteHashInsert(&db->idxHash, pOld->zName, strlen(pOld->zName)+1, pOld);
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
*/
void sqliteResetInternalSchema(sqlite *db){
  HashElem *pElem;
  Hash temp1;
  Hash temp2;

  sqliteHashClear(&db->aFKey);
  temp1 = db->tblHash;
  temp2 = db->trigHash;
  sqliteHashInit(&db->trigHash, SQLITE_HASH_STRING, 0);
  sqliteHashClear(&db->idxHash);
  for(pElem=sqliteHashFirst(&temp2); pElem; pElem=sqliteHashNext(pElem)){
    Trigger *pTrigger = sqliteHashData(pElem);
    sqliteDeleteTrigger(pTrigger);
  }
  sqliteHashClear(&temp2);
  sqliteHashInit(&db->tblHash, SQLITE_HASH_STRING, 0);
  for(pElem=sqliteHashFirst(&temp1); pElem; pElem=sqliteHashNext(pElem)){
    Table *pTab = sqliteHashData(pElem);
    sqliteDeleteTable(db, pTab);
  }
  sqliteHashClear(&temp1);
  db->flags &= ~(SQLITE_Initialized|SQLITE_InternChanges);
}

/*
** This routine is called whenever a rollback occurs.  If there were
** schema changes during the transaction, then we have to reset the
** internal hash tables and reload them from disk.
*/
void sqliteRollbackInternalChanges(sqlite *db){
  if( db->flags & SQLITE_InternChanges ){
    sqliteResetInternalSchema(db);
  }
}

/*
** This routine is called when a commit occurs.
*/
void sqliteCommitInternalChanges(sqlite *db){
  db->schema_cookie = db->next_cookie;
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
    sqliteDeleteIndex(db, pIndex);
  }

  /* Delete all foreign keys associated with this table.  The keys
  ** should have already been unlinked from the db->aFKey hash table 
  */
  for(pFKey=pTable->pFKey; pFKey; pFKey=pNextFKey){
    pNextFKey = pFKey->pNextFrom;
    assert( sqliteHashFind(&db->aFKey,pFKey->zTo,strlen(pFKey->zTo)+1)!=pFKey );
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
  assert( db!=0 );
  pOld = sqliteHashInsert(&db->tblHash, p->zName, strlen(p->zName)+1, 0);
  assert( pOld==0 || pOld==p );
  for(pF1=p->pFKey; pF1; pF1=pF1->pNextFrom){
    int nTo = strlen(pF1->zTo) + 1;
    pF2 = sqliteHashFind(&db->aFKey, pF1->zTo, nTo);
    if( pF2==pF1 ){
      sqliteHashInsert(&db->aFKey, pF1->zTo, nTo, pF1->pNextTo);
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
  if( isTemp ){
    sqliteVdbeAddOp(v, OP_OpenWrAux, 0, 2);
    sqliteVdbeChangeP3(v, -1, TEMP_MASTER_NAME, P3_STATIC);
  }else{
    sqliteVdbeAddOp(v, OP_OpenWrite, 0, 2);
    sqliteVdbeChangeP3(v, -1, MASTER_NAME, P3_STATIC);
  }
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

  pParse->sFirstToken = *pStart;
  zName = sqliteTableNameFromToken(pName);
  if( zName==0 ) return;
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqliteAuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(isTemp), 0) ){
    sqliteFree(zName);
    return;
  }
  {
    int code;
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
    if( sqliteAuthCheck(pParse, code, zName, 0) ){
      sqliteFree(zName);
      return;
    }
  }
#endif
 

  /* Before trying to create a temporary table, make sure the Btree for
  ** holding temporary tables is open.
  */
  if( isTemp && db->pBeTemp==0 ){
    int rc = sqliteBtreeOpen(0, 0, MAX_PAGES, &db->pBeTemp);
    if( rc!=SQLITE_OK ){
      sqliteSetString(&pParse->zErrMsg, "unable to open a temporary database "
        "file for storing temporary tables", 0);
      pParse->nErr++;
      return;
    }
    if( db->flags & SQLITE_InTrans ){
      rc = sqliteBtreeBeginTrans(db->pBeTemp);
      if( rc!=SQLITE_OK ){
        sqliteSetNString(&pParse->zErrMsg, "unable to get a write lock on "
          "the temporary database file", 0);
        pParse->nErr++;
        return;
      }
    }
  }

  /* Make sure the new table name does not collide with an existing
  ** index or table name.  Issue an error message if it does.
  **
  ** If we are re-reading the sqlite_master table because of a schema
  ** change and a new permanent table is found whose name collides with
  ** an existing temporary table, then ignore the new permanent table.
  ** We will continue parsing, but the pParse->nameClash flag will be set
  ** so we will know to discard the table record once parsing has finished.
  */
  pTable = sqliteFindTable(db, zName);
  if( pTable!=0 ){
    if( pTable->isTemp && pParse->initFlag ){
      pParse->nameClash = 1;
    }else{
      sqliteSetNString(&pParse->zErrMsg, "table ", 0, pName->z, pName->n,
          " already exists", 0, 0);
      sqliteFree(zName);
      pParse->nErr++;
      return;
    }
  }else{
    pParse->nameClash = 0;
  }
  if( (pIdx = sqliteFindIndex(db, zName))!=0 &&
          (!pIdx->pTable->isTemp || !pParse->initFlag) ){
    sqliteSetString(&pParse->zErrMsg, "there is already an index named ", 
       zName, 0);
    sqliteFree(zName);
    pParse->nErr++;
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
  pTable->isTemp = isTemp;
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
  if( !pParse->initFlag && (v = sqliteGetVdbe(pParse))!=0 ){
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
      sqliteSetString(&pParse->zErrMsg, "duplicate column name: ", z, 0);
      pParse->nErr++;
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
  int iCol = -1;
  if( pTab==0 ) return;
  if( pTab->hasPrimKey ){
    sqliteSetString(&pParse->zErrMsg, "table \"", pTab->zName, 
        "\" has more than one primary key", 0);
    pParse->nErr++;
    return;
  }
  pTab->hasPrimKey = 1;
  if( pList==0 ){
    iCol = pTab->nCol - 1;
  }else if( pList->nId==1 ){
    for(iCol=0; iCol<pTab->nCol; iCol++){
      if( sqliteStrICmp(pList->a[0].zName, pTab->aCol[iCol].zName)==0 ) break;
    }
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
  }
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
  for(i=0; i<nType-1; i++){
    switch( zType[i] ){
      case 'b':
      case 'B': {
        if( i<nType-3 && sqliteStrNICmp(&zType[i],"blob",4)==0 ){
          return SQLITE_SO_TEXT;
        }
        break;
      }
      case 'c':
      case 'C': {
        if( i<nType-3 && (sqliteStrNICmp(&zType[i],"char",4)==0 ||
                           sqliteStrNICmp(&zType[i],"clob",4)==0)
        ){
          return SQLITE_SO_TEXT;
        }
        break;
      }
      case 'x':
      case 'X': {
        if( i>=2 && sqliteStrNICmp(&zType[i-2],"text",4)==0 ){
          return SQLITE_SO_TEXT;
        }
        break;
      }
      default: {
        break;
      }
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
  if( db->next_cookie==db->schema_cookie ){
    db->next_cookie = db->schema_cookie + sqliteRandomByte() + 1;
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
  strcpy(zStmt, p->isTemp ? "CREATE TEMP TABLE " : "CREATE TABLE ");
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
** An entry for the table is made in the master table on disk,
** unless this is a temporary table or initFlag==1.  When initFlag==1,
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

  /* If the initFlag is 1 it means we are reading the SQL off the
  ** "sqlite_master" or "sqlite_temp_master" table on the disk.
  ** So do not write to the disk again.  Extract the root page number
  ** for the table from the pParse->newTnum field.  (The page number
  ** should have been put there by the sqliteOpenCb routine.)
  */
  if( pParse->initFlag ){
    p->tnum = pParse->newTnum;
  }

  /* If not initializing, then create a record for the new table
  ** in the SQLITE_MASTER table of the database.  The record number
  ** for the new table entry should already be on the stack.
  **
  ** If this is a TEMPORARY table, write the entry into the auxiliary
  ** file instead of into the main database file.
  */
  if( !pParse->initFlag ){
    int n;
    Vdbe *v;

    v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( p->pSelect==0 ){
      /* A regular table */
      sqliteVdbeAddOp(v, OP_CreateTable, 0, p->isTemp);
      sqliteVdbeChangeP3(v, -1, (char *)&p->tnum, P3_POINTER);
    }else{
      /* A view */
      sqliteVdbeAddOp(v, OP_Integer, 0, 0);
    }
    p->tnum = 0;
    sqliteVdbeAddOp(v, OP_Pull, 1, 0);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    if( p->pSelect==0 ){
      sqliteVdbeChangeP3(v, -1, "table", P3_STATIC);
    }else{
      sqliteVdbeChangeP3(v, -1, "view", P3_STATIC);
    }
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeChangeP3(v, -1, p->zName, P3_STATIC);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeChangeP3(v, -1, p->zName, P3_STATIC);
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
    if( !p->isTemp ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    if( pSelect ){
      int op = p->isTemp ? OP_OpenWrAux : OP_OpenWrite;
      sqliteVdbeAddOp(v, op, 1, 0);
      pParse->nTab = 2;
      sqliteSelect(pParse, pSelect, SRT_Table, 1, 0, 0, 0);
    }
    sqliteEndWriteOperation(pParse);
  }

  /* Add the table to the in-memory representation of the database.
  */
  assert( pParse->nameClash==0 || pParse->initFlag==1 );
  if( pParse->explain==0 && pParse->nameClash==0 && pParse->nErr==0 ){
    Table *pOld;
    FKey *pFKey;
    pOld = sqliteHashInsert(&db->tblHash, p->zName, strlen(p->zName)+1, p);
    if( pOld ){
      assert( p==pOld );  /* Malloc must have failed inside HashInsert() */
      return;
    }
    for(pFKey=p->pFKey; pFKey; pFKey=pFKey->pNextFrom){
      int nTo = strlen(pFKey->zTo) + 1;
      pFKey->pNextTo = sqliteHashFind(&db->aFKey, pFKey->zTo, nTo);
      sqliteHashInsert(&db->aFKey, pFKey->zTo, nTo, pFKey);
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

  sqliteStartTable(pParse, pBegin, pName, isTemp, 1);
  p = pParse->pNewTable;
  if( p==0 || pParse->nErr ){
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
  if( !pParse->initFlag ){
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
  n = ((int)sEnd.z) - (int)pBegin->z;
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
** of errors.  If an error is seen leave an error message in pPare->zErrMsg.
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
    sqliteSetString(&pParse->zErrMsg, "view ", pTable->zName,
         " is circularly defined", 0);
    pParse->nErr++;
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
    pParse->db->flags |= SQLITE_UnresetViews;
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
  if( pTable==0 || pTable->pSelect==0 ) return;
  if( pTable->nCol==0 ) return;
  for(i=0; i<pTable->nCol; i++){
    sqliteFree(pTable->aCol[i].zName);
    sqliteFree(pTable->aCol[i].zDflt);
    sqliteFree(pTable->aCol[i].zType);
  }
  sqliteFree(pTable->aCol);
  pTable->aCol = 0;
  pTable->nCol = 0;
}

/*
** Clear the column names from every VIEW.
*/
void sqliteViewResetAll(sqlite *db){
  HashElem *i;
  if( (db->flags & SQLITE_UnresetViews)==0 ) return;
  for(i=sqliteHashFirst(&db->tblHash); i; i=sqliteHashNext(i)){
    Table *pTab = sqliteHashData(i);
    if( pTab->pSelect ){
      sqliteViewResetColumnNames(pTab);
    }
  }
  db->flags &= ~SQLITE_UnresetViews;
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
  pTab = sqliteFindTable(pParse->db, zName);
  sqliteFree(zName);
  if( pTab==0 ){
    sqliteSetNString(&pParse->zErrMsg, "no such table: ", 0, 
        pTok->z, pTok->n, 0);
    pParse->nErr++;
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

  if( pParse->nErr || sqlite_malloc_failed ) return;
  pTable = sqliteTableFromToken(pParse, pName);
  if( pTable==0 ) return;
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqliteAuthCheck(pParse, SQLITE_DELETE, SCHEMA_TABLE(pTable->isTemp),0)){
    return;
  }
  {
    int code;
    if( isView ){
      if( pTable->isTemp ){
        code = SQLITE_DROP_TEMP_VIEW;
      }else{
        code = SQLITE_DROP_VIEW;
      }
    }else{
      if( pTable->isTemp ){
        code = SQLITE_DROP_TEMP_TABLE;
      }else{
        code = SQLITE_DROP_TABLE;
      }
    }
    if( sqliteAuthCheck(pParse, code, pTable->zName, 0) ){
      return;
    }
    if( sqliteAuthCheck(pParse, SQLITE_DELETE, pTable->zName, 0) ){
      return;
    }
  }
#endif
  if( pTable->readOnly ){
    sqliteSetString(&pParse->zErrMsg, "table ", pTable->zName, 
       " may not be dropped", 0);
    pParse->nErr++;
    return;
  }
  if( isView && pTable->pSelect==0 ){
    sqliteSetString(&pParse->zErrMsg, "use DROP TABLE to delete table ",
      pTable->zName, 0);
    pParse->nErr++;
    return;
  }
  if( !isView && pTable->pSelect ){
    sqliteSetString(&pParse->zErrMsg, "use DROP VIEW to delete view ",
      pTable->zName, 0);
    pParse->nErr++;
    return;
  }

  /* Generate code to remove the table from the master table
  ** on disk.
  */
  v = sqliteGetVdbe(pParse);
  if( v ){
    static VdbeOp dropTable[] = {
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
    sqliteBeginWriteOperation(pParse, 0, pTable->isTemp);
    sqliteOpenMasterTable(v, pTable->isTemp);
    /* Drop all triggers associated with the table being dropped */
    pTrigger = pTable->pTrigger;
    while( pTrigger ){
      Token tt;
      tt.z = pTable->pTrigger->name;
      tt.n = strlen(pTable->pTrigger->name);
      sqliteDropTrigger(pParse, &tt, 1);
      if( pParse->explain ){
        pTrigger = pTrigger->pNext;
      }else{
        pTrigger = pTable->pTrigger;
      }
    }
    base = sqliteVdbeAddOpList(v, ArraySize(dropTable), dropTable);
    sqliteVdbeChangeP3(v, base+1, pTable->zName, 0);
    if( !pTable->isTemp ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    if( !isView ){
      sqliteVdbeAddOp(v, OP_Destroy, pTable->tnum, pTable->isTemp);
      for(pIdx=pTable->pIndex; pIdx; pIdx=pIdx->pNext){
        sqliteVdbeAddOp(v, OP_Destroy, pIdx->tnum, pTable->isTemp);
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
  sqliteViewResetAll(db);
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
      sqliteSetNString(&pParse->zErrMsg, "foreign key on ", -1,
         p->aCol[iCol].zName, -1, 
         " should reference only one column of table ", -1,
         pTo->z, pTo->n, 0);
      pParse->nErr++;
      goto fk_end;
    }
    nCol = 1;
  }else if( pToCol && pToCol->nId!=pFromCol->nId ){
    sqliteSetString(&pParse->zErrMsg, 
        "number of columns in foreign key does not match the number of "
        "columns in the referenced table", 0);
    pParse->nErr++;
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
        sqliteSetString(&pParse->zErrMsg, "unknown column \"", 
          pFromCol->a[i].zName, "\" in foreign key definition", 0);
        pParse->nErr++;
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
  Token *pTable,   /* Name of the table to index.  Use pParse->pNewTable if 0 */
  IdList *pList,   /* A list of columns to be indexed */
  int onError,     /* OE_Abort, OE_Ignore, OE_Replace, or OE_None */
  Token *pStart,   /* The CREATE token that begins a CREATE TABLE statement */
  Token *pEnd      /* The ")" that closes the CREATE INDEX statement */
){
  Table *pTab;     /* Table to be indexed */
  Index *pIndex;   /* The index to be created */
  char *zName = 0;
  int i, j;
  Token nullId;             /* Fake token for an empty ID list */
  sqlite *db = pParse->db;
  int hideName = 0;         /* Do not put table name in the hash table */

  if( pParse->nErr || sqlite_malloc_failed ) goto exit_create_index;

  /*
  ** Find the table that is to be indexed.  Return early if not found.
  */
  if( pTable!=0 ){
    assert( pName!=0 );
    pTab =  sqliteTableFromToken(pParse, pTable);
  }else{
    assert( pName==0 );
    pTab =  pParse->pNewTable;
  }
  if( pTab==0 || pParse->nErr ) goto exit_create_index;
  if( pTab->readOnly ){
    sqliteSetString(&pParse->zErrMsg, "table ", pTab->zName, 
      " may not have new indices added", 0);
    pParse->nErr++;
    goto exit_create_index;
  }
  if( pTab->pSelect ){
    sqliteSetString(&pParse->zErrMsg, "views may not be indexed", 0);
    pParse->nErr++;
    goto exit_create_index;
  }

  /* If this index is created while re-reading the schema from sqlite_master
  ** but the table associated with this index is a temporary table, it can
  ** only mean that the table that this index is really associated with is
  ** one whose name is hidden behind a temporary table with the same name.
  ** Since its table has been suppressed, we need to also suppress the
  ** index.
  */
  if( pParse->initFlag && !pParse->isTemp && pTab->isTemp ){
    goto exit_create_index;
  }

  /*
  ** Find the name of the index.  Make sure there is not already another
  ** index or table with the same name.  
  **
  ** Exception:  If we are reading the names of permanent indices from the
  ** sqlite_master table (because some other process changed the schema) and
  ** one of the index names collides with the name of a temporary table or
  ** index, then we will continue to process this index, but we will not
  ** store its name in the hash table.  Set the hideName flag to accomplish
  ** this.
  **
  ** If pName==0 it means that we are
  ** dealing with a primary key or UNIQUE constraint.  We have to invent our
  ** own name.
  */
  if( pName ){
    Index *pISameName;    /* Another index with the same name */
    Table *pTSameName;    /* A table with same name as the index */
    zName = sqliteTableNameFromToken(pName);
    if( zName==0 ) goto exit_create_index;
    if( (pISameName = sqliteFindIndex(db, zName))!=0 ){
      if( pISameName->pTable->isTemp && pParse->initFlag ){
        hideName = 1;
      }else{
        sqliteSetString(&pParse->zErrMsg, "index ", zName, 
           " already exists", 0);
        pParse->nErr++;
        goto exit_create_index;
      }
    }
    if( (pTSameName = sqliteFindTable(db, zName))!=0 ){
      if( pTSameName->isTemp && pParse->initFlag ){
        hideName = 1;
      }else{
        sqliteSetString(&pParse->zErrMsg, "there is already a table named ",
           zName, 0);
        pParse->nErr++;
        goto exit_create_index;
      }
    }
  }else{
    char zBuf[30];
    int n;
    Index *pLoop;
    for(pLoop=pTab->pIndex, n=1; pLoop; pLoop=pLoop->pNext, n++){}
    sprintf(zBuf,"%d)",n);
    zName = 0;
    sqliteSetString(&zName, "(", pTab->zName, " autoindex ", zBuf, 0);
    if( zName==0 ) goto exit_create_index;
    hideName = sqliteFindIndex(db, zName)!=0;
  }

  /* Check for authorization to create an index.
  */
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqliteAuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(pTab->isTemp), 0) ){
    goto exit_create_index;
  }
  i = SQLITE_CREATE_INDEX;
  if( pTab->isTemp ) i = SQLITE_CREATE_TEMP_INDEX;
  if( sqliteAuthCheck(pParse, i, zName, pTab->zName) ){
    goto exit_create_index;
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
  pIndex->onError = pIndex->isUnique = onError;
  pIndex->autoIndex = pName==0;

  /* Scan the names of the columns of the table to be indexed and
  ** load the column indices into the Index structure.  Report an error
  ** if any column is not found.
  */
  for(i=0; i<pList->nId; i++){
    for(j=0; j<pTab->nCol; j++){
      if( sqliteStrICmp(pList->a[i].zName, pTab->aCol[j].zName)==0 ) break;
    }
    if( j>=pTab->nCol ){
      sqliteSetString(&pParse->zErrMsg, "table ", pTab->zName, 
        " has no column named ", pList->a[i].zName, 0);
      pParse->nErr++;
      sqliteFree(pIndex);
      goto exit_create_index;
    }
    pIndex->aiColumn[i] = j;
  }

  /* Link the new Index structure to its table and to the other
  ** in-memory database structures. 
  */
  if( !pParse->explain && !hideName ){
    Index *p;
    p = sqliteHashInsert(&db->idxHash, pIndex->zName, strlen(zName)+1, pIndex);
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

  /* If the initFlag is 1 it means we are reading the SQL off the
  ** "sqlite_master" table on the disk.  So do not write to the disk
  ** again.  Extract the table number from the pParse->newTnum field.
  */
  if( pParse->initFlag && pTable!=0 ){
    pIndex->tnum = pParse->newTnum;
  }

  /* If the initFlag is 0 then create the index on disk.  This
  ** involves writing the index into the master table and filling in the
  ** index with the current table contents.
  **
  ** The initFlag is 0 when the user first enters a CREATE INDEX 
  ** command.  The initFlag is 1 when a database is opened and 
  ** CREATE INDEX statements are read out of the master table.  In
  ** the latter case the index already exists on disk, which is why
  ** we don't want to recreate it.
  **
  ** If pTable==0 it means this index is generated as a primary key
  ** or UNIQUE constraint of a CREATE TABLE statement.  Since the table
  ** has just been created, it contains no data and the index initialization
  ** step can be skipped.
  */
  else if( pParse->initFlag==0 ){
    int n;
    Vdbe *v;
    int lbl1, lbl2;
    int i;
    int addr;
    int isTemp = pTab->isTemp;

    v = sqliteGetVdbe(pParse);
    if( v==0 ) goto exit_create_index;
    if( pTable!=0 ){
      sqliteBeginWriteOperation(pParse, 0, isTemp);
      sqliteOpenMasterTable(v, isTemp);
    }
    sqliteVdbeAddOp(v, OP_NewRecno, 0, 0);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeChangeP3(v, -1, "index", P3_STATIC);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeChangeP3(v, -1, pIndex->zName, P3_STATIC);
    sqliteVdbeAddOp(v, OP_String, 0, 0);
    sqliteVdbeChangeP3(v, -1, pTab->zName, P3_STATIC);
    addr = sqliteVdbeAddOp(v, OP_CreateIndex, 0, isTemp);
    sqliteVdbeChangeP3(v, addr, (char*)&pIndex->tnum, P3_POINTER);
    pIndex->tnum = 0;
    if( pTable ){
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);
      if( isTemp ){
        sqliteVdbeAddOp(v, OP_OpenWrAux, 1, 0);
      }else{
        sqliteVdbeAddOp(v, OP_OpenWrite, 1, 0);
      }
    }
    addr = sqliteVdbeAddOp(v, OP_String, 0, 0);
    if( pStart && pEnd ){
      n = Addr(pEnd->z) - Addr(pStart->z) + 1;
      sqliteVdbeChangeP3(v, addr, pStart->z, n);
    }
    sqliteVdbeAddOp(v, OP_MakeRecord, 5, 0);
    sqliteVdbeAddOp(v, OP_PutIntKey, 0, 0);
    if( pTable ){
      sqliteVdbeAddOp(v, isTemp ? OP_OpenAux : OP_Open, 2, pTab->tnum);
      sqliteVdbeChangeP3(v, -1, pTab->zName, P3_STATIC);
      lbl2 = sqliteVdbeMakeLabel(v);
      sqliteVdbeAddOp(v, OP_Rewind, 2, lbl2);
      lbl1 = sqliteVdbeAddOp(v, OP_Recno, 2, 0);
      for(i=0; i<pIndex->nColumn; i++){
        sqliteVdbeAddOp(v, OP_Column, 2, pIndex->aiColumn[i]);
      }
      sqliteVdbeAddOp(v, OP_MakeIdxKey, pIndex->nColumn, 0);
      if( db->file_format>=4 ) sqliteAddIdxKeyType(v, pIndex);
      sqliteVdbeAddOp(v, OP_IdxPut, 1, pIndex->onError!=OE_None);
      sqliteVdbeChangeP3(v, -1, "indexed columns are not unique", P3_STATIC);
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
  sqliteFree(zName);
  return;
}

/*
** This routine will drop an existing named index.  This routine
** implements the DROP INDEX statement.
*/
void sqliteDropIndex(Parse *pParse, Token *pName){
  Index *pIndex;
  char *zName;
  Vdbe *v;
  sqlite *db = pParse->db;

  if( pParse->nErr || sqlite_malloc_failed ) return;
  zName = sqliteTableNameFromToken(pName);
  if( zName==0 ) return;
  pIndex = sqliteFindIndex(db, zName);
  sqliteFree(zName);
  if( pIndex==0 ){
    sqliteSetNString(&pParse->zErrMsg, "no such index: ", 0, 
        pName->z, pName->n, 0);
    pParse->nErr++;
    return;
  }
  if( pIndex->autoIndex ){
    sqliteSetString(&pParse->zErrMsg, "index associated with UNIQUE "
      "or PRIMARY KEY constraint cannot be dropped", 0);
    pParse->nErr++;
    return;
  }
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_DROP_INDEX;
    Table *pTab = pIndex->pTable;
    if( sqliteAuthCheck(pParse, SQLITE_DELETE, SCHEMA_TABLE(pTab->isTemp), 0) ){
      return;
    }
    if( pTab->isTemp ) code = SQLITE_DROP_TEMP_INDEX;
    if( sqliteAuthCheck(pParse, code, pIndex->zName, pTab->zName) ){
      return;
    }
  }
#endif

  /* Generate code to remove the index and from the master table */
  v = sqliteGetVdbe(pParse);
  if( v ){
    static VdbeOp dropIndex[] = {
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
    Table *pTab = pIndex->pTable;

    sqliteBeginWriteOperation(pParse, 0, pTab->isTemp);
    sqliteOpenMasterTable(v, pTab->isTemp);
    base = sqliteVdbeAddOpList(v, ArraySize(dropIndex), dropIndex);
    sqliteVdbeChangeP3(v, base+1, pIndex->zName, 0);
    if( !pTab->isTemp ){
      sqliteChangeCookie(db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    sqliteVdbeAddOp(v, OP_Destroy, pIndex->tnum, pTab->isTemp);
    sqliteEndWriteOperation(pParse);
  }

  /* Delete the in-memory description of this index.
  */
  if( !pParse->explain ){
    sqliteUnlinkAndDeleteIndex(db, pIndex);
    db->flags |= SQLITE_InternChanges;
  }
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
  }
  if( (pList->nId & 7)==0 ){
    struct IdList_item *a;
    a = sqliteRealloc(pList->a, (pList->nId+8)*sizeof(pList->a[0]) );
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
*/
SrcList *sqliteSrcListAppend(SrcList *pList, Token *pToken){
  if( pList==0 ){
    pList = sqliteMalloc( sizeof(IdList) );
    if( pList==0 ) return 0;
  }
  if( (pList->nSrc & 7)==0 ){
    struct SrcList_item *a;
    a = sqliteRealloc(pList->a, (pList->nSrc+8)*sizeof(pList->a[0]) );
    if( a==0 ){
      sqliteSrcListDelete(pList);
      return 0;
    }
    pList->a = a;
  }
  memset(&pList->a[pList->nSrc], 0, sizeof(pList->a[0]));
  if( pToken ){
    char **pz = &pList->a[pList->nSrc].zName;
    sqliteSetNString(pz, pToken->z, pToken->n, 0);
    if( *pz==0 ){
      sqliteSrcListDelete(pList);
      return 0;
    }else{
      sqliteDequote(*pz);
    }
  }
  pList->nSrc++;
  return pList;
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
    sqliteFree(pList->a[i].zName);
    sqliteFree(pList->a[i].zAlias);
    if( pList->a[i].pTab && pList->a[i].pTab->isTransient ){
      sqliteDeleteTable(0, pList->a[i].pTab);
    }
    sqliteSelectDelete(pList->a[i].pSelect);
    sqliteExprDelete(pList->a[i].pOn);
    sqliteIdListDelete(pList->a[i].pUsing);
  }
  sqliteFree(pList->a);
  sqliteFree(pList);
}

/*
** The COPY command is for compatibility with PostgreSQL and specificially
** for the ability to read the output of pg_dump.  The format is as
** follows:
**
**    COPY table FROM file [USING DELIMITERS string]
**
** "table" is an existing table name.  We will read lines of code from
** file to fill this table with data.  File might be "stdin".  The optional
** delimiter string identifies the field separators.  The default is a tab.
*/
void sqliteCopy(
  Parse *pParse,       /* The parser context */
  Token *pTableName,   /* The name of the table into which we will insert */
  Token *pFilename,    /* The file from which to obtain information */
  Token *pDelimiter,   /* Use this as the field delimiter */
  int onError          /* What to do if a constraint fails */
){
  Table *pTab;
  char *zTab;
  int i;
  Vdbe *v;
  int addr, end;
  Index *pIdx;
  char *zFile = 0;
  sqlite *db = pParse->db;


  zTab = sqliteTableNameFromToken(pTableName);
  if( sqlite_malloc_failed || zTab==0 ) goto copy_cleanup;
  pTab = sqliteTableNameToTable(pParse, zTab);
  sqliteFree(zTab);
  if( pTab==0 ) goto copy_cleanup;
  zFile = sqliteStrNDup(pFilename->z, pFilename->n);
  sqliteDequote(zFile);
  if( sqliteAuthCheck(pParse, SQLITE_INSERT, pTab->zName, zFile)
      || sqliteAuthCheck(pParse, SQLITE_COPY, pTab->zName, zFile) ){
    goto copy_cleanup;
  }
  v = sqliteGetVdbe(pParse);
  if( v ){
    int openOp;
    sqliteBeginWriteOperation(pParse, 1, pTab->isTemp);
    addr = sqliteVdbeAddOp(v, OP_FileOpen, 0, 0);
    sqliteVdbeChangeP3(v, addr, pFilename->z, pFilename->n);
    sqliteVdbeDequoteP3(v, addr);
    openOp = pTab->isTemp ? OP_OpenWrAux : OP_OpenWrite;
    sqliteVdbeAddOp(v, openOp, 0, pTab->tnum);
    sqliteVdbeChangeP3(v, -1, pTab->zName, P3_STATIC);
    for(i=1, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, i++){
      sqliteVdbeAddOp(v, openOp, i, pIdx->tnum);
      sqliteVdbeChangeP3(v, -1, pIdx->zName, P3_STATIC);
    }
    if( db->flags & SQLITE_CountRows ){
      sqliteVdbeAddOp(v, OP_Integer, 0, 0);  /* Initialize the row count */
    }
    end = sqliteVdbeMakeLabel(v);
    addr = sqliteVdbeAddOp(v, OP_FileRead, pTab->nCol, end);
    if( pDelimiter ){
      sqliteVdbeChangeP3(v, addr, pDelimiter->z, pDelimiter->n);
      sqliteVdbeDequoteP3(v, addr);
    }else{
      sqliteVdbeChangeP3(v, addr, "\t", 1);
    }
    if( pTab->iPKey>=0 ){
      sqliteVdbeAddOp(v, OP_FileColumn, pTab->iPKey, 0);
      sqliteVdbeAddOp(v, OP_MustBeInt, 0, 0);
    }else{
      sqliteVdbeAddOp(v, OP_NewRecno, 0, 0);
    }
    for(i=0; i<pTab->nCol; i++){
      if( i==pTab->iPKey ){
        /* The integer primary key column is filled with NULL since its
        ** value is always pulled from the record number */
        sqliteVdbeAddOp(v, OP_String, 0, 0);
      }else{
        sqliteVdbeAddOp(v, OP_FileColumn, i, 0);
      }
    }
    sqliteGenerateConstraintChecks(pParse, pTab, 0, 0, 0, 0, onError, addr);
    sqliteCompleteInsertion(pParse, pTab, 0, 0, 0, 0);
    if( (db->flags & SQLITE_CountRows)!=0 ){
      sqliteVdbeAddOp(v, OP_AddImm, 1, 0);  /* Increment row count */
    }
    sqliteVdbeAddOp(v, OP_Goto, 0, addr);
    sqliteVdbeResolveLabel(v, end);
    sqliteVdbeAddOp(v, OP_Noop, 0, 0);
    sqliteEndWriteOperation(pParse);
    if( db->flags & SQLITE_CountRows ){
      sqliteVdbeAddOp(v, OP_ColumnName, 0, 0);
      sqliteVdbeChangeP3(v, -1, "rows inserted", P3_STATIC);
      sqliteVdbeAddOp(v, OP_Callback, 1, 0);
    }
  }
  
copy_cleanup:
  sqliteFree(zFile);
  return;
}

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
  /* Do nothing */
}

/*
** Begin a transaction
*/
void sqliteBeginTransaction(Parse *pParse, int onError){
  sqlite *db;

  if( pParse==0 || (db=pParse->db)==0 || db->pBe==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "BEGIN", 0) ) return;
  if( db->flags & SQLITE_InTrans ){
    pParse->nErr++;
    sqliteSetString(&pParse->zErrMsg, "cannot start a transaction "
       "within a transaction", 0);
    return;
  }
  sqliteBeginWriteOperation(pParse, 0, 0);
  db->flags |= SQLITE_InTrans;
  db->onError = onError;
}

/*
** Commit a transaction
*/
void sqliteCommitTransaction(Parse *pParse){
  sqlite *db;

  if( pParse==0 || (db=pParse->db)==0 || db->pBe==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "COMMIT", 0) ) return;
  if( (db->flags & SQLITE_InTrans)==0 ){
    pParse->nErr++;
    sqliteSetString(&pParse->zErrMsg, 
       "cannot commit - no transaction is active", 0);
    return;
  }
  db->flags &= ~SQLITE_InTrans;
  sqliteEndWriteOperation(pParse);
  db->onError = OE_Default;
}

/*
** Rollback a transaction
*/
void sqliteRollbackTransaction(Parse *pParse){
  sqlite *db;
  Vdbe *v;

  if( pParse==0 || (db=pParse->db)==0 || db->pBe==0 ) return;
  if( pParse->nErr || sqlite_malloc_failed ) return;
  if( sqliteAuthCheck(pParse, SQLITE_TRANSACTION, "ROLLBACK", 0) ) return;
  if( (db->flags & SQLITE_InTrans)==0 ){
    pParse->nErr++;
    sqliteSetString(&pParse->zErrMsg,
       "cannot rollback - no transaction is active", 0);
    return; 
  }
  v = sqliteGetVdbe(pParse);
  if( v ){
    sqliteVdbeAddOp(v, OP_Rollback, 0, 0);
  }
  db->flags &= ~SQLITE_InTrans;
  db->onError = OE_Default;
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
** The tempOnly flag indicates that only temporary tables will be changed
** during this write operation.  The primary database table is not
** write-locked.  Only the temporary database file gets a write lock.
** Other processes can continue to read or write the primary database file.
*/
void sqliteBeginWriteOperation(Parse *pParse, int setCheckpoint, int tempOnly){
  Vdbe *v;
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return;
  if( pParse->trigStack ) return; /* if this is in a trigger */
  if( (pParse->db->flags & SQLITE_InTrans)==0 ){
    sqliteVdbeAddOp(v, OP_Transaction, tempOnly, 0);
    if( !tempOnly ){
      sqliteVdbeAddOp(v, OP_VerifyCookie, pParse->db->schema_cookie, 0);
      pParse->schemaVerified = 1;
    }
  }else if( setCheckpoint ){
    sqliteVdbeAddOp(v, OP_Checkpoint, 0, 0);
  }
}

/*
** Generate code that concludes an operation that may have changed
** the database.  This is a companion function to BeginWriteOperation().
** If a transaction was started, then commit it.  If a checkpoint was
** started then commit that.
*/
void sqliteEndWriteOperation(Parse *pParse){
  Vdbe *v;
  if( pParse->trigStack ) return; /* if this is in a trigger */
  v = sqliteGetVdbe(pParse);
  if( v==0 ) return;
  if( pParse->db->flags & SQLITE_InTrans ){
    /* Do Nothing */
  }else{
    sqliteVdbeAddOp(v, OP_Commit, 0, 0);
  }
}


/*
** Interpret the given string as a boolean value.
*/
static int getBoolean(char *z){
  static char *azTrue[] = { "yes", "on", "true" };
  int i;
  if( z[0]==0 ) return 0;
  if( isdigit(z[0]) || (z[0]=='-' && isdigit(z[1])) ){
    return atoi(z);
  }
  for(i=0; i<sizeof(azTrue)/sizeof(azTrue[0]); i++){
    if( sqliteStrICmp(z,azTrue[i])==0 ) return 1;
  }
  return 0;
}

/*
** Interpret the given string as a safety level.  Return 0 for OFF,
** 1 for ON or NORMAL and 2 for FULL.
**
** Note that the values returned are one less that the values that
** should be passed into sqliteBtreeSetSafetyLevel().  The is done
** to support legacy SQL code.  The safety level used to be boolean
** and older scripts may have used numbers 0 for OFF and 1 for ON.
*/
static int getSafetyLevel(char *z){
  static const struct {
    const char *zWord;
    int val;
  } aKey[] = {
    { "no",    0 },
    { "off",   0 },
    { "false", 0 },
    { "yes",   1 },
    { "on",    1 },
    { "true",  1 },
    { "full",  2 },
  };
  int i;
  if( z[0]==0 ) return 1;
  if( isdigit(z[0]) || (z[0]=='-' && isdigit(z[1])) ){
    return atoi(z);
  }
  for(i=0; i<sizeof(aKey)/sizeof(aKey[0]); i++){
    if( sqliteStrICmp(z,aKey[i].zWord)==0 ) return aKey[i].val;
  }
  return 1;
}

/*
** Process a pragma statement.  
**
** Pragmas are of this form:
**
**      PRAGMA id = value
**
** The identifier might also be a string.  The value is a string, and
** identifier, or a number.  If minusFlag is true, then the value is
** a number that was preceded by a minus sign.
*/
void sqlitePragma(Parse *pParse, Token *pLeft, Token *pRight, int minusFlag){
  char *zLeft = 0;
  char *zRight = 0;
  sqlite *db = pParse->db;

  zLeft = sqliteStrNDup(pLeft->z, pLeft->n);
  sqliteDequote(zLeft);
  if( minusFlag ){
    zRight = 0;
    sqliteSetNString(&zRight, "-", 1, pRight->z, pRight->n, 0);
  }else{
    zRight = sqliteStrNDup(pRight->z, pRight->n);
    sqliteDequote(zRight);
  }
  if( sqliteAuthCheck(pParse, SQLITE_PRAGMA, zLeft, zRight) ){
    sqliteFree(zLeft);
    sqliteFree(zRight);
    return;
  }
 
  /*
  **  PRAGMA default_cache_size
  **  PRAGMA default_cache_size=N
  **
  ** The first form reports the current persistent setting for the
  ** page cache size.  The value returned is the maximum number of
  ** pages in the page cache.  The second form sets both the current
  ** page cache size value and the persistent page cache size value
  ** stored in the database file.
  **
  ** The default cache size is stored in meta-value 2 of page 1 of the
  ** database file.  The cache size is actually the absolute value of
  ** this memory location.  The sign of meta-value 2 determines the
  ** synchronous setting.  A negative value means synchronous is off
  ** and a positive value means synchronous is on.
  */
  if( sqliteStrICmp(zLeft,"default_cache_size")==0 ){
    static VdbeOp getCacheSize[] = {
      { OP_ReadCookie,  0, 2,        0},
      { OP_AbsValue,    0, 0,        0},
      { OP_Dup,         0, 0,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Ne,          0, 6,        0},
      { OP_Integer,     MAX_PAGES,0, 0},
      { OP_ColumnName,  0, 0,        "cache_size"},
      { OP_Callback,    1, 0,        0},
    };
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( pRight->z==pLeft->z ){
      sqliteVdbeAddOpList(v, ArraySize(getCacheSize), getCacheSize);
    }else{
      int addr;
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      sqliteBeginWriteOperation(pParse, 0, 0);
      sqliteVdbeAddOp(v, OP_Integer, size, 0);
      sqliteVdbeAddOp(v, OP_ReadCookie, 0, 2);
      addr = sqliteVdbeAddOp(v, OP_Integer, 0, 0);
      sqliteVdbeAddOp(v, OP_Ge, 0, addr+3);
      sqliteVdbeAddOp(v, OP_Negative, 0, 0);
      sqliteVdbeAddOp(v, OP_SetCookie, 0, 2);
      sqliteEndWriteOperation(pParse);
      db->cache_size = db->cache_size<0 ? -size : size;
      sqliteBtreeSetCacheSize(db->pBe, db->cache_size);
    }
  }else

  /*
  **  PRAGMA cache_size
  **  PRAGMA cache_size=N
  **
  ** The first form reports the current local setting for the
  ** page cache size.  The local setting can be different from
  ** the persistent cache size value that is stored in the database
  ** file itself.  The value returned is the maximum number of
  ** pages in the page cache.  The second form sets the local
  ** page cache size value.  It does not change the persistent
  ** cache size stored on the disk so the cache size will revert
  ** to its default value when the database is closed and reopened.
  ** N should be a positive integer.
  */
  if( sqliteStrICmp(zLeft,"cache_size")==0 ){
    static VdbeOp getCacheSize[] = {
      { OP_ColumnName,  0, 0,        "cache_size"},
      { OP_Callback,    1, 0,        0},
    };
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( pRight->z==pLeft->z ){
      int size = db->cache_size;;
      if( size<0 ) size = -size;
      sqliteVdbeAddOp(v, OP_Integer, size, 0);
      sqliteVdbeAddOpList(v, ArraySize(getCacheSize), getCacheSize);
    }else{
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      if( db->cache_size<0 ) size = -size;
      db->cache_size = size;
      sqliteBtreeSetCacheSize(db->pBe, db->cache_size);
    }
  }else

  /*
  **  PRAGMA default_synchronous
  **  PRAGMA default_synchronous=ON|OFF|NORMAL|FULL
  **
  ** The first form returns the persistent value of the "synchronous" setting
  ** that is stored in the database.  This is the synchronous setting that
  ** is used whenever the database is opened unless overridden by a separate
  ** "synchronous" pragma.  The second form changes the persistent and the
  ** local synchronous setting to the value given.
  **
  ** If synchronous is OFF, SQLite does not attempt any fsync() systems calls
  ** to make sure data is committed to disk.  Write operations are very fast,
  ** but a power failure can leave the database in an inconsistent state.
  ** If synchronous is ON or NORMAL, SQLite will do an fsync() system call to
  ** make sure data is being written to disk.  The risk of corruption due to
  ** a power loss in this mode is negligible but non-zero.  If synchronous
  ** is FULL, extra fsync()s occur to reduce the risk of corruption to near
  ** zero, but with a write performance penalty.  The default mode is NORMAL.
  */
  if( sqliteStrICmp(zLeft,"default_synchronous")==0 ){
    static VdbeOp getSync[] = {
      { OP_ColumnName,  0, 0,        "synchronous"},
      { OP_ReadCookie,  0, 3,        0},
      { OP_Dup,         0, 0,        0},
      { OP_If,          0, 0,        0},  /* 3 */
      { OP_ReadCookie,  0, 2,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Lt,          0, 5,        0},
      { OP_AddImm,      1, 0,        0},
      { OP_Callback,    1, 0,        0},
      { OP_Halt,        0, 0,        0},
      { OP_AddImm,     -1, 0,        0},  /* 10 */
      { OP_Callback,    1, 0,        0}
    };
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( pRight->z==pLeft->z ){
      int addr = sqliteVdbeAddOpList(v, ArraySize(getSync), getSync);
      sqliteVdbeChangeP2(v, addr+3, addr+10);
    }else{
      int addr;
      int size = db->cache_size;
      if( size<0 ) size = -size;
      sqliteBeginWriteOperation(pParse, 0, 0);
      sqliteVdbeAddOp(v, OP_ReadCookie, 0, 2);
      sqliteVdbeAddOp(v, OP_Dup, 0, 0);
      addr = sqliteVdbeAddOp(v, OP_Integer, 0, 0);
      sqliteVdbeAddOp(v, OP_Ne, 0, addr+3);
      sqliteVdbeAddOp(v, OP_AddImm, MAX_PAGES, 0);
      sqliteVdbeAddOp(v, OP_AbsValue, 0, 0);
      db->safety_level = getSafetyLevel(zRight)+1;
      if( db->safety_level==1 ){
        sqliteVdbeAddOp(v, OP_Negative, 0, 0);
        size = -size;
      }
      sqliteVdbeAddOp(v, OP_SetCookie, 0, 2);
      sqliteVdbeAddOp(v, OP_Integer, db->safety_level, 0);
      sqliteVdbeAddOp(v, OP_SetCookie, 0, 3);
      sqliteEndWriteOperation(pParse);
      db->cache_size = size;
      sqliteBtreeSetCacheSize(db->pBe, db->cache_size);
      sqliteBtreeSetSafetyLevel(db->pBe, db->safety_level);
    }
  }else

  /*
  **   PRAGMA synchronous
  **   PRAGMA synchronous=OFF|ON|NORMAL|FULL
  **
  ** Return or set the local value of the synchronous flag.  Changing
  ** the local value does not make changes to the disk file and the
  ** default value will be restored the next time the database is
  ** opened.
  */
  if( sqliteStrICmp(zLeft,"synchronous")==0 ){
    static VdbeOp getSync[] = {
      { OP_ColumnName,  0, 0,        "synchronous"},
      { OP_Callback,    1, 0,        0},
    };
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    if( pRight->z==pLeft->z ){
      sqliteVdbeAddOp(v, OP_Integer, db->safety_level-1, 0);
      sqliteVdbeAddOpList(v, ArraySize(getSync), getSync);
    }else{
      int size = db->cache_size;
      if( size<0 ) size = -size;
      db->safety_level = getSafetyLevel(zRight)+1;
      if( db->safety_level==1 ) size = -size;
      db->cache_size = size;
      sqliteBtreeSetCacheSize(db->pBe, db->cache_size);
      sqliteBtreeSetSafetyLevel(db->pBe, db->safety_level);
    }
  }else

  if( sqliteStrICmp(zLeft, "trigger_overhead_test")==0 ){
    if( getBoolean(zRight) ){
      always_code_trigger_setup = 1;
    }else{
      always_code_trigger_setup = 0;
    }
  }else

  if( sqliteStrICmp(zLeft, "vdbe_trace")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_VdbeTrace;
    }else{
      db->flags &= ~SQLITE_VdbeTrace;
    }
  }else

  if( sqliteStrICmp(zLeft, "full_column_names")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_FullColNames;
    }else{
      db->flags &= ~SQLITE_FullColNames;
    }
  }else

  if( sqliteStrICmp(zLeft, "show_datatypes")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_ReportTypes;
    }else{
      db->flags &= ~SQLITE_ReportTypes;
    }
  }else

  if( sqliteStrICmp(zLeft, "result_set_details")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_ResultDetails;
    }else{
      db->flags &= ~SQLITE_ResultDetails;
    }
  }else

  if( sqliteStrICmp(zLeft, "count_changes")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_CountRows;
    }else{
      db->flags &= ~SQLITE_CountRows;
    }
  }else

  if( sqliteStrICmp(zLeft, "empty_result_callbacks")==0 ){
    if( getBoolean(zRight) ){
      db->flags |= SQLITE_NullCallback;
    }else{
      db->flags &= ~SQLITE_NullCallback;
    }
  }else

  if( sqliteStrICmp(zLeft, "table_info")==0 ){
    Table *pTab;
    Vdbe *v;
    pTab = sqliteFindTable(db, zRight);
    if( pTab ) v = sqliteGetVdbe(pParse);
    if( pTab && v ){
      static VdbeOp tableInfoPreface[] = {
        { OP_ColumnName,  0, 0,       "cid"},
        { OP_ColumnName,  1, 0,       "name"},
        { OP_ColumnName,  2, 0,       "type"},
        { OP_ColumnName,  3, 0,       "notnull"},
        { OP_ColumnName,  4, 0,       "dflt_value"},
      };
      int i;
      sqliteVdbeAddOpList(v, ArraySize(tableInfoPreface), tableInfoPreface);
      sqliteViewGetColumnNames(pParse, pTab);
      for(i=0; i<pTab->nCol; i++){
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        sqliteVdbeChangeP3(v, -1, pTab->aCol[i].zName, P3_STATIC);
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        sqliteVdbeChangeP3(v, -1, 
           pTab->aCol[i].zType ? pTab->aCol[i].zType : "numeric", P3_STATIC);
        sqliteVdbeAddOp(v, OP_Integer, pTab->aCol[i].notNull, 0);
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        sqliteVdbeChangeP3(v, -1, pTab->aCol[i].zDflt, P3_STATIC);
        sqliteVdbeAddOp(v, OP_Callback, 5, 0);
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "index_info")==0 ){
    Index *pIdx;
    Table *pTab;
    Vdbe *v;
    pIdx = sqliteFindIndex(db, zRight);
    if( pIdx ) v = sqliteGetVdbe(pParse);
    if( pIdx && v ){
      static VdbeOp tableInfoPreface[] = {
        { OP_ColumnName,  0, 0,       "seqno"},
        { OP_ColumnName,  1, 0,       "cid"},
        { OP_ColumnName,  2, 0,       "name"},
      };
      int i;
      pTab = pIdx->pTable;
      sqliteVdbeAddOpList(v, ArraySize(tableInfoPreface), tableInfoPreface);
      for(i=0; i<pIdx->nColumn; i++){
        int cnum = pIdx->aiColumn[i];
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeAddOp(v, OP_Integer, cnum, 0);
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        assert( pTab->nCol>cnum );
        sqliteVdbeChangeP3(v, -1, pTab->aCol[cnum].zName, P3_STATIC);
        sqliteVdbeAddOp(v, OP_Callback, 3, 0);
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "index_list")==0 ){
    Index *pIdx;
    Table *pTab;
    Vdbe *v;
    pTab = sqliteFindTable(db, zRight);
    if( pTab ){
      v = sqliteGetVdbe(pParse);
      pIdx = pTab->pIndex;
    }
    if( pTab && pIdx && v ){
      int i = 0; 
      static VdbeOp indexListPreface[] = {
        { OP_ColumnName,  0, 0,       "seq"},
        { OP_ColumnName,  1, 0,       "name"},
        { OP_ColumnName,  2, 0,       "unique"},
      };

      sqliteVdbeAddOpList(v, ArraySize(indexListPreface), indexListPreface);
      while(pIdx){
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeAddOp(v, OP_String, 0, 0);
        sqliteVdbeChangeP3(v, -1, pIdx->zName, P3_STATIC);
        sqliteVdbeAddOp(v, OP_Integer, pIdx->onError!=OE_None, 0);
        sqliteVdbeAddOp(v, OP_Callback, 3, 0);
        ++i;
        pIdx = pIdx->pNext;
      }
    }
  }else

#ifndef NDEBUG
  if( sqliteStrICmp(zLeft, "parser_trace")==0 ){
    extern void sqliteParserTrace(FILE*, char *);
    if( getBoolean(zRight) ){
      sqliteParserTrace(stdout, "parser: ");
    }else{
      sqliteParserTrace(0, 0);
    }
  }else
#endif

  if( sqliteStrICmp(zLeft, "integrity_check")==0 ){
    static VdbeOp checkDb[] = {
      { OP_SetInsert,   0, 0,        "2"},
      { OP_Open,        0, 2,        0},
      { OP_Rewind,      0, 6,        0},
      { OP_Column,      0, 3,        0},    /* 3 */
      { OP_SetInsert,   0, 0,        0},
      { OP_Next,        0, 3,        0},
      { OP_IntegrityCk, 0, 0,        0},    /* 6 */
      { OP_ColumnName,  0, 0,        "integrity_check"},
      { OP_Callback,    1, 0,        0},
      { OP_SetInsert,   1, 0,        "2"},
      { OP_OpenAux,     1, 2,        0},
      { OP_Rewind,      1, 15,       0},
      { OP_Column,      1, 3,        0},    /* 12 */
      { OP_SetInsert,   1, 0,        0},
      { OP_Next,        1, 12,       0},
      { OP_IntegrityCk, 1, 1,        0},    /* 15 */
      { OP_Callback,    1, 0,        0},
    };
    Vdbe *v = sqliteGetVdbe(pParse);
    if( v==0 ) return;
    sqliteVdbeAddOpList(v, ArraySize(checkDb), checkDb);
  }else

  {}
  sqliteFree(zLeft);
  sqliteFree(zRight);
}
