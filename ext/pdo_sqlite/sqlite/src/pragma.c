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
** This file contains code used to implement the PRAGMA command.
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
# include "pager.h"
# include "btree.h"
#endif

/*
** Interpret the given string as a boolean value.
*/
static int getBoolean(const u8 *z){
  static const u8 *azTrue[] = { "yes", "on", "true" };
  int i;
  if( z[0]==0 ) return 0;
  if( sqlite3IsNumber(z, 0, SQLITE_UTF8) ){
    return atoi(z);
  }
  for(i=0; i<sizeof(azTrue)/sizeof(azTrue[0]); i++){
    if( sqlite3StrICmp(z,azTrue[i])==0 ) return 1;
  }
  return 0;
}

/*
** Interpret the given string as a safety level.  Return 0 for OFF,
** 1 for ON or NORMAL and 2 for FULL.  Return 1 for an empty or 
** unrecognized string argument.
**
** Note that the values returned are one less that the values that
** should be passed into sqlite3BtreeSetSafetyLevel().  The is done
** to support legacy SQL code.  The safety level used to be boolean
** and older scripts may have used numbers 0 for OFF and 1 for ON.
*/
static int getSafetyLevel(u8 *z){
  static const struct {
    const u8 *zWord;
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
  if( sqlite3IsNumber(z, 0, SQLITE_UTF8) ){
    return atoi(z);
  }
  for(i=0; i<sizeof(aKey)/sizeof(aKey[0]); i++){
    if( sqlite3StrICmp(z,aKey[i].zWord)==0 ) return aKey[i].val;
  }
  return 1;
}

/*
** Interpret the given string as a temp db location. Return 1 for file
** backed temporary databases, 2 for the Red-Black tree in memory database
** and 0 to use the compile-time default.
*/
static int getTempStore(const char *z){
  if( z[0]>='0' && z[0]<='2' ){
    return z[0] - '0';
  }else if( sqlite3StrICmp(z, "file")==0 ){
    return 1;
  }else if( sqlite3StrICmp(z, "memory")==0 ){
    return 2;
  }else{
    return 0;
  }
}

/*
** If the TEMP database is open, close it and mark the database schema
** as needing reloading.  This must be done when using the TEMP_STORE
** or DEFAULT_TEMP_STORE pragmas.
*/
static int changeTempStorage(Parse *pParse, const char *zStorageType){
  int ts = getTempStore(zStorageType);
  sqlite3 *db = pParse->db;
  if( db->temp_store==ts ) return SQLITE_OK;
  if( db->aDb[1].pBt!=0 ){
    if( db->flags & SQLITE_InTrans ){
      sqlite3ErrorMsg(pParse, "temporary storage cannot be changed "
        "from within a transaction");
      return SQLITE_ERROR;
    }
    sqlite3BtreeClose(db->aDb[1].pBt);
    db->aDb[1].pBt = 0;
    sqlite3ResetInternalSchema(db, 0);
  }
  db->temp_store = ts;
  return SQLITE_OK;
}

/*
** Generate code to return a single integer value.
*/
static void returnSingleInt(Parse *pParse, const char *zLabel, int value){
  Vdbe *v = sqlite3GetVdbe(pParse);
  sqlite3VdbeAddOp(v, OP_Integer, value, 0);
  if( pParse->explain==0 ){
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, zLabel, P3_STATIC);
  }
  sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
}

/*
** Check to see if zRight and zLeft refer to a pragma that queries
** or changes one of the flags in db->flags.  Return 1 if so and 0 if not.
** Also, implement the pragma.
*/
static int flagPragma(Parse *pParse, const char *zLeft, const char *zRight){
  static const struct {
    const char *zName;  /* Name of the pragma */
    int mask;           /* Mask for the db->flags value */
  } aPragma[] = {
    { "vdbe_trace",               SQLITE_VdbeTrace     },
    { "sql_trace",                SQLITE_SqlTrace      },
    { "vdbe_listing",             SQLITE_VdbeListing   },
#if 1  /* FIX ME:  Remove the following pragmas */
    { "full_column_names",        SQLITE_FullColNames  },
    { "short_column_names",       SQLITE_ShortColNames },
    { "count_changes",            SQLITE_CountRows     },
    { "empty_result_callbacks",   SQLITE_NullCallback  },
#endif
  };
  int i;
  for(i=0; i<sizeof(aPragma)/sizeof(aPragma[0]); i++){
    if( sqlite3StrICmp(zLeft, aPragma[i].zName)==0 ){
      sqlite3 *db = pParse->db;
      Vdbe *v;
      if( zRight==0 ){
        v = sqlite3GetVdbe(pParse);
        if( v ){
          returnSingleInt(pParse,
               aPragma[i].zName, (db->flags&aPragma[i].mask)!=0);
        }
      }else if( getBoolean(zRight) ){
        db->flags |= aPragma[i].mask;
      }else{
        db->flags &= ~aPragma[i].mask;
      }
      return 1;
    }
  }
  return 0;
}

/*
** Process a pragma statement.  
**
** Pragmas are of this form:
**
**      PRAGMA [database.]id [= value]
**
** The identifier might also be a string.  The value is a string, and
** identifier, or a number.  If minusFlag is true, then the value is
** a number that was preceded by a minus sign.
**
** If the left side is "database.id" then pId1 is the database name
** and pId2 is the id.  If the left side is just "id" then pId1 is the
** id and pId2 is any empty string.
*/
void sqlite3Pragma(
  Parse *pParse, 
  Token *pId1,        /* First part of [database.]id field */
  Token *pId2,        /* Second part of [database.]id field, or NULL */
  Token *pValue,      /* Token for <value>, or NULL */
  int minusFlag       /* True if a '-' sign preceded <value> */
){
  char *zLeft = 0;       /* Nul-terminated UTF-8 string <id> */
  char *zRight = 0;      /* Nul-terminated UTF-8 string <value>, or NULL */
  const char *zDb = 0;   /* The database name */
  Token *pId;            /* Pointer to <id> token */
  int iDb;               /* Database index for <database> */
  sqlite3 *db = pParse->db;
  Db *pDb;
  Vdbe *v = sqlite3GetVdbe(pParse);
  if( v==0 ) return;

  /* Interpret the [database.] part of the pragma statement. iDb is the
  ** index of the database this pragma is being applied to in db.aDb[]. */
  iDb = sqlite3TwoPartName(pParse, pId1, pId2, &pId);
  if( iDb<0 ) return;
  pDb = &db->aDb[iDb];

  zLeft = sqlite3NameFromToken(pId);
  if( !zLeft ) return;
  if( minusFlag ){
    zRight = sqlite3MPrintf("-%T", pValue);
  }else{
    zRight = sqlite3NameFromToken(pValue);
  }

  zDb = ((iDb>0)?pDb->zName:0);
  if( sqlite3AuthCheck(pParse, SQLITE_PRAGMA, zLeft, zRight, zDb) ){
    goto pragma_out;
  }
 
  /*
  **  PRAGMA [database.]default_cache_size
  **  PRAGMA [database.]default_cache_size=N
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
  if( sqlite3StrICmp(zLeft,"default_cache_size")==0 ){
    static const VdbeOpList getCacheSize[] = {
      { OP_ReadCookie,  0, 2,        0},  /* 0 */
      { OP_AbsValue,    0, 0,        0},
      { OP_Dup,         0, 0,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Ne,          0, 6,        0},
      { OP_Integer,     0, 0,        0},  /* 5 */
      { OP_Callback,    1, 0,        0},
    };
    int addr;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    if( !zRight ){
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, "cache_size", P3_STATIC);
      addr = sqlite3VdbeAddOpList(v, ArraySize(getCacheSize), getCacheSize);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP1(v, addr+5, MAX_PAGES);
    }else{
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      sqlite3BeginWriteOperation(pParse, 0, iDb);
      sqlite3VdbeAddOp(v, OP_Integer, size, 0);
      sqlite3VdbeAddOp(v, OP_ReadCookie, iDb, 2);
      addr = sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
      sqlite3VdbeAddOp(v, OP_Ge, 0, addr+3);
      sqlite3VdbeAddOp(v, OP_Negative, 0, 0);
      sqlite3VdbeAddOp(v, OP_SetCookie, iDb, 2);
      pDb->cache_size = size;
      sqlite3BtreeSetCacheSize(pDb->pBt, pDb->cache_size);
    }
  }else

  /*
  **  PRAGMA [database.]page_size
  **  PRAGMA [database.]page_size=N
  **
  ** The first form reports the current setting for the
  ** database page size in bytes.  The second form sets the
  ** database page size value.  The value can only be set if
  ** the database has not yet been created.
  */
  if( sqlite3StrICmp(zLeft,"page_size")==0 ){
    Btree *pBt = pDb->pBt;
    if( !zRight ){
      int size = pBt ? sqlite3BtreeGetPageSize(pBt) : 0;
      returnSingleInt(pParse, "page_size", size);
    }else{
      sqlite3BtreeSetPageSize(pBt, atoi(zRight), sqlite3BtreeGetReserve(pBt));
    }
  }else

  /*
  **  PRAGMA [database.]cache_size
  **  PRAGMA [database.]cache_size=N
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
  if( sqlite3StrICmp(zLeft,"cache_size")==0 ){
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    if( !zRight ){
      returnSingleInt(pParse, "cache_size", pDb->cache_size);
    }else{
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      pDb->cache_size = size;
      sqlite3BtreeSetCacheSize(pDb->pBt, pDb->cache_size);
    }
  }else

  /*
  **   PRAGMA temp_store
  **   PRAGMA temp_store = "default"|"memory"|"file"
  **
  ** Return or set the local value of the temp_store flag.  Changing
  ** the local value does not make changes to the disk file and the default
  ** value will be restored the next time the database is opened.
  **
  ** Note that it is possible for the library compile-time options to
  ** override this setting
  */
  if( sqlite3StrICmp(zLeft, "temp_store")==0 ){
    if( !zRight ){
      returnSingleInt(pParse, "temp_store", db->temp_store);
    }else{
      changeTempStorage(pParse, zRight);
    }
  }else

  /*
  **   PRAGMA [database.]synchronous
  **   PRAGMA [database.]synchronous=OFF|ON|NORMAL|FULL
  **
  ** Return or set the local value of the synchronous flag.  Changing
  ** the local value does not make changes to the disk file and the
  ** default value will be restored the next time the database is
  ** opened.
  */
  if( sqlite3StrICmp(zLeft,"synchronous")==0 ){
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    if( !zRight ){
      returnSingleInt(pParse, "synchronous", pDb->safety_level-1);
    }else{
      if( !db->autoCommit ){
        sqlite3ErrorMsg(pParse, 
            "Safety level may not be changed inside a transaction");
      }else{
        pDb->safety_level = getSafetyLevel(zRight)+1;
        sqlite3BtreeSetSafetyLevel(pDb->pBt, pDb->safety_level);
      }
    }
  }else

#if 0  /* Used once during development.  No longer needed */
  if( sqlite3StrICmp(zLeft, "trigger_overhead_test")==0 ){
    if( getBoolean(zRight) ){
      sqlite3_always_code_trigger_setup = 1;
    }else{
      sqlite3_always_code_trigger_setup = 0;
    }
  }else
#endif

  if( flagPragma(pParse, zLeft, zRight) ){
    /* The flagPragma() subroutine also generates any necessary code
    ** there is nothing more to do here */
  }else

  /*
  **   PRAGMA table_info(<table>)
  **
  ** Return a single row for each column of the named table. The columns of
  ** the returned data set are:
  **
  ** cid:        Column id (numbered from left to right, starting at 0)
  ** name:       Column name
  ** type:       Column declaration type.
  ** notnull:    True if 'NOT NULL' is part of column declaration
  ** dflt_value: The default value for the column, if any.
  */
  if( sqlite3StrICmp(zLeft, "table_info")==0 && zRight ){
    Table *pTab;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    pTab = sqlite3FindTable(db, zRight, zDb);
    if( pTab ){
      int i;
      sqlite3VdbeSetNumCols(v, 6);
      sqlite3VdbeSetColName(v, 0, "cid", P3_STATIC);
      sqlite3VdbeSetColName(v, 1, "name", P3_STATIC);
      sqlite3VdbeSetColName(v, 2, "type", P3_STATIC);
      sqlite3VdbeSetColName(v, 3, "notnull", P3_STATIC);
      sqlite3VdbeSetColName(v, 4, "dflt_value", P3_STATIC);
      sqlite3VdbeSetColName(v, 5, "pk", P3_STATIC);
      sqlite3ViewGetColumnNames(pParse, pTab);
      for(i=0; i<pTab->nCol; i++){
        sqlite3VdbeAddOp(v, OP_Integer, i, 0);
        sqlite3VdbeOp3(v, OP_String8, 0, 0, pTab->aCol[i].zName, 0);
        sqlite3VdbeOp3(v, OP_String8, 0, 0,
           pTab->aCol[i].zType ? pTab->aCol[i].zType : "numeric", 0);
        sqlite3VdbeAddOp(v, OP_Integer, pTab->aCol[i].notNull, 0);
        sqlite3VdbeOp3(v, OP_String8, 0, 0,
           pTab->aCol[i].zDflt, P3_STATIC);
        sqlite3VdbeAddOp(v, OP_Integer, pTab->aCol[i].isPrimKey, 0);
        sqlite3VdbeAddOp(v, OP_Callback, 6, 0);
      }
    }
  }else

  if( sqlite3StrICmp(zLeft, "index_info")==0 && zRight ){
    Index *pIdx;
    Table *pTab;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    pIdx = sqlite3FindIndex(db, zRight, zDb);
    if( pIdx ){
      int i;
      pTab = pIdx->pTable;
      sqlite3VdbeSetNumCols(v, 3);
      sqlite3VdbeSetColName(v, 0, "seqno", P3_STATIC);
      sqlite3VdbeSetColName(v, 1, "cid", P3_STATIC);
      sqlite3VdbeSetColName(v, 2, "name", P3_STATIC);
      for(i=0; i<pIdx->nColumn; i++){
        int cnum = pIdx->aiColumn[i];
        sqlite3VdbeAddOp(v, OP_Integer, i, 0);
        sqlite3VdbeAddOp(v, OP_Integer, cnum, 0);
        assert( pTab->nCol>cnum );
        sqlite3VdbeOp3(v, OP_String8, 0, 0, pTab->aCol[cnum].zName, 0);
        sqlite3VdbeAddOp(v, OP_Callback, 3, 0);
      }
    }
  }else

  if( sqlite3StrICmp(zLeft, "index_list")==0 && zRight ){
    Index *pIdx;
    Table *pTab;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    pTab = sqlite3FindTable(db, zRight, zDb);
    if( pTab ){
      v = sqlite3GetVdbe(pParse);
      pIdx = pTab->pIndex;
      if( pIdx ){
        int i = 0; 
        sqlite3VdbeSetNumCols(v, 3);
        sqlite3VdbeSetColName(v, 0, "seq", P3_STATIC);
        sqlite3VdbeSetColName(v, 1, "name", P3_STATIC);
        sqlite3VdbeSetColName(v, 2, "unique", P3_STATIC);
        while(pIdx){
          sqlite3VdbeAddOp(v, OP_Integer, i, 0);
          sqlite3VdbeOp3(v, OP_String8, 0, 0, pIdx->zName, 0);
          sqlite3VdbeAddOp(v, OP_Integer, pIdx->onError!=OE_None, 0);
          sqlite3VdbeAddOp(v, OP_Callback, 3, 0);
          ++i;
          pIdx = pIdx->pNext;
        }
      }
    }
  }else

  if( sqlite3StrICmp(zLeft, "foreign_key_list")==0 && zRight ){
    FKey *pFK;
    Table *pTab;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    pTab = sqlite3FindTable(db, zRight, zDb);
    if( pTab ){
      v = sqlite3GetVdbe(pParse);
      pFK = pTab->pFKey;
      if( pFK ){
        int i = 0; 
        sqlite3VdbeSetNumCols(v, 5);
        sqlite3VdbeSetColName(v, 0, "id", P3_STATIC);
        sqlite3VdbeSetColName(v, 1, "seq", P3_STATIC);
        sqlite3VdbeSetColName(v, 2, "table", P3_STATIC);
        sqlite3VdbeSetColName(v, 3, "from", P3_STATIC);
        sqlite3VdbeSetColName(v, 4, "to", P3_STATIC);
        while(pFK){
          int j;
          for(j=0; j<pFK->nCol; j++){
            sqlite3VdbeAddOp(v, OP_Integer, i, 0);
            sqlite3VdbeAddOp(v, OP_Integer, j, 0);
            sqlite3VdbeOp3(v, OP_String8, 0, 0, pFK->zTo, 0);
            sqlite3VdbeOp3(v, OP_String8, 0, 0,
                             pTab->aCol[pFK->aCol[j].iFrom].zName, 0);
            sqlite3VdbeOp3(v, OP_String8, 0, 0, pFK->aCol[j].zCol, 0);
            sqlite3VdbeAddOp(v, OP_Callback, 5, 0);
          }
          ++i;
          pFK = pFK->pNextFrom;
        }
      }
    }
  }else

  if( sqlite3StrICmp(zLeft, "database_list")==0 ){
    int i;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    sqlite3VdbeSetNumCols(v, 3);
    sqlite3VdbeSetColName(v, 0, "seq", P3_STATIC);
    sqlite3VdbeSetColName(v, 1, "name", P3_STATIC);
    sqlite3VdbeSetColName(v, 2, "file", P3_STATIC);
    for(i=0; i<db->nDb; i++){
      if( db->aDb[i].pBt==0 ) continue;
      assert( db->aDb[i].zName!=0 );
      sqlite3VdbeAddOp(v, OP_Integer, i, 0);
      sqlite3VdbeOp3(v, OP_String8, 0, 0, db->aDb[i].zName, 0);
      sqlite3VdbeOp3(v, OP_String8, 0, 0,
           sqlite3BtreeGetFilename(db->aDb[i].pBt), 0);
      sqlite3VdbeAddOp(v, OP_Callback, 3, 0);
    }
  }else

#ifndef NDEBUG
  if( sqlite3StrICmp(zLeft, "parser_trace")==0 ){
    extern void sqlite3ParserTrace(FILE*, char *);
    if( getBoolean(zRight) ){
      sqlite3ParserTrace(stdout, "parser: ");
    }else{
      sqlite3ParserTrace(0, 0);
    }
  }else
#endif

  if( sqlite3StrICmp(zLeft, "integrity_check")==0 ){
    int i, j, addr;

    /* Code that initializes the integrity check program.  Set the
    ** error count 0
    */
    static const VdbeOpList initCode[] = {
      { OP_Integer,     0, 0,        0},
      { OP_MemStore,    0, 1,        0},
    };

    /* Code that appears at the end of the integrity check.  If no error
    ** messages have been generated, output OK.  Otherwise output the
    ** error message
    */
    static const VdbeOpList endCode[] = {
      { OP_MemLoad,     0, 0,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Ne,          0, 0,        0},    /* 2 */
      { OP_String8,     0, 0,        "ok"},
      { OP_Callback,    1, 0,        0},
    };

    /* Initialize the VDBE program */
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, "integrity_check", P3_STATIC);
    sqlite3VdbeAddOpList(v, ArraySize(initCode), initCode);

    /* Do an integrity check on each database file */
    for(i=0; i<db->nDb; i++){
      HashElem *x;
      int cnt = 0;

      sqlite3CodeVerifySchema(pParse, i);

      /* Do an integrity check of the B-Tree
      */
      for(x=sqliteHashFirst(&db->aDb[i].tblHash); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        sqlite3VdbeAddOp(v, OP_Integer, pTab->tnum, 0);
        cnt++;
        for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
          if( sqlite3CheckIndexCollSeq(pParse, pIdx) ) goto pragma_out;
          sqlite3VdbeAddOp(v, OP_Integer, pIdx->tnum, 0);
          cnt++;
        }
      }
      assert( cnt>0 );
      sqlite3VdbeAddOp(v, OP_IntegrityCk, cnt, i);
      sqlite3VdbeAddOp(v, OP_Dup, 0, 1);
      addr = sqlite3VdbeOp3(v, OP_String8, 0, 0, "ok", P3_STATIC);
      sqlite3VdbeAddOp(v, OP_Eq, 0, addr+6);
      sqlite3VdbeOp3(v, OP_String8, 0, 0,
         sqlite3MPrintf("*** in database %s ***\n", db->aDb[i].zName),
         P3_DYNAMIC);
      sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
      sqlite3VdbeAddOp(v, OP_Concat, 0, 1);
      sqlite3VdbeAddOp(v, OP_Callback, 1, 0);

      /* Make sure all the indices are constructed correctly.
      */
      sqlite3CodeVerifySchema(pParse, i);
      for(x=sqliteHashFirst(&db->aDb[i].tblHash); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        int loopTop;

        if( pTab->pIndex==0 ) continue;
        sqlite3OpenTableAndIndices(pParse, pTab, 1, OP_OpenRead);
        sqlite3VdbeAddOp(v, OP_Integer, 0, 0);
        sqlite3VdbeAddOp(v, OP_MemStore, 1, 1);
        loopTop = sqlite3VdbeAddOp(v, OP_Rewind, 1, 0);
        sqlite3VdbeAddOp(v, OP_MemIncr, 1, 0);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          int jmp2;
          static const VdbeOpList idxErr[] = {
            { OP_MemIncr,     0,  0,  0},
            { OP_String8,     0,  0,  "rowid "},
            { OP_Recno,       1,  0,  0},
            { OP_String8,     0,  0,  " missing from index "},
            { OP_String8,     0,  0,  0},    /* 4 */
            { OP_Concat,      2,  0,  0},
            { OP_Callback,    1,  0,  0},
          };
          sqlite3GenerateIndexKey(v, pIdx, 1);
          jmp2 = sqlite3VdbeAddOp(v, OP_Found, j+2, 0);
          addr = sqlite3VdbeAddOpList(v, ArraySize(idxErr), idxErr);
          sqlite3VdbeChangeP3(v, addr+4, pIdx->zName, P3_STATIC);
          sqlite3VdbeChangeP2(v, jmp2, sqlite3VdbeCurrentAddr(v));
        }
        sqlite3VdbeAddOp(v, OP_Next, 1, loopTop+1);
        sqlite3VdbeChangeP2(v, loopTop, sqlite3VdbeCurrentAddr(v));
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          static const VdbeOpList cntIdx[] = {
             { OP_Integer,      0,  0,  0},
             { OP_MemStore,     2,  1,  0},
             { OP_Rewind,       0,  0,  0},  /* 2 */
             { OP_MemIncr,      2,  0,  0},
             { OP_Next,         0,  0,  0},  /* 4 */
             { OP_MemLoad,      1,  0,  0},
             { OP_MemLoad,      2,  0,  0},
             { OP_Eq,           0,  0,  0},  /* 7 */
             { OP_MemIncr,      0,  0,  0},
             { OP_String8,      0,  0,  "wrong # of entries in index "},
             { OP_String8,      0,  0,  0},  /* 10 */
             { OP_Concat,       0,  0,  0},
             { OP_Callback,     1,  0,  0},
          };
          if( pIdx->tnum==0 ) continue;
          addr = sqlite3VdbeAddOpList(v, ArraySize(cntIdx), cntIdx);
          sqlite3VdbeChangeP1(v, addr+2, j+2);
          sqlite3VdbeChangeP2(v, addr+2, addr+5);
          sqlite3VdbeChangeP1(v, addr+4, j+2);
          sqlite3VdbeChangeP2(v, addr+4, addr+3);
          sqlite3VdbeChangeP2(v, addr+7, addr+ArraySize(cntIdx));
          sqlite3VdbeChangeP3(v, addr+10, pIdx->zName, P3_STATIC);
        }
      } 
    }
    addr = sqlite3VdbeAddOpList(v, ArraySize(endCode), endCode);
    sqlite3VdbeChangeP2(v, addr+2, addr+ArraySize(endCode));
  }else
  /*
  **   PRAGMA encoding
  **   PRAGMA encoding = "utf-8"|"utf-16"|"utf-16le"|"utf-16be"
  **
  ** In it's first form, this pragma returns the encoding of the main
  ** database. If the database is not initialized, it is initialized now.
  **
  ** The second form of this pragma is a no-op if the main database file
  ** has not already been initialized. In this case it sets the default
  ** encoding that will be used for the main database file if a new file
  ** is created. If an existing main database file is opened, then the
  ** default text encoding for the existing database is used.
  ** 
  ** In all cases new databases created using the ATTACH command are
  ** created to use the same default text encoding as the main database. If
  ** the main database has not been initialized and/or created when ATTACH
  ** is executed, this is done before the ATTACH operation.
  **
  ** In the second form this pragma sets the text encoding to be used in
  ** new database files created using this database handle. It is only
  ** useful if invoked immediately after the main database i
  */
  if( sqlite3StrICmp(zLeft, "encoding")==0 ){
    static struct EncName {
      char *zName;
      u8 enc;
    } encnames[] = {
      { "UTF-8",    SQLITE_UTF8        },
      { "UTF8",     SQLITE_UTF8        },
      { "UTF-16le", SQLITE_UTF16LE     },
      { "UTF16le",  SQLITE_UTF16LE     },
      { "UTF-16be", SQLITE_UTF16BE     },
      { "UTF16be",  SQLITE_UTF16BE     },
      { "UTF-16",   0 /* Filled in at run-time */ },
      { "UTF16",    0 /* Filled in at run-time */ },
      { 0, 0 }
    };
    struct EncName *pEnc;
    encnames[6].enc = encnames[7].enc = SQLITE_UTF16NATIVE;
    if( !zRight ){    /* "PRAGMA encoding" */
      if( sqlite3ReadSchema(pParse) ) goto pragma_out;
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, "encoding", P3_STATIC);
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
        if( pEnc->enc==pParse->db->enc ){
          sqlite3VdbeChangeP3(v, -1, pEnc->zName, P3_STATIC);
          break;
        }
      }
      sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
    }else{                        /* "PRAGMA encoding = XXX" */
      /* Only change the value of sqlite.enc if the database handle is not
      ** initialized. If the main database exists, the new sqlite.enc value
      ** will be overwritten when the schema is next loaded. If it does not
      ** already exists, it will be created to use the new encoding value.
      */
      if( !(pParse->db->flags&SQLITE_Initialized) ){
        for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
          if( 0==sqlite3StrICmp(zRight, pEnc->zName) ){
            pParse->db->enc = pEnc->enc;
            break;
          }
        }
        if( !pEnc->zName ){
          sqlite3ErrorMsg(pParse, "unsupported encoding: %s", zRight);
        }
      }
    }
  }else

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
  /*
  ** Report the current state of file logs for all databases
  */
  if( sqlite3StrICmp(zLeft, "lock_status")==0 ){
    static const char *const azLockName[] = {
      "unlocked", "shared", "reserved", "pending", "exclusive"
    };
    int i;
    Vdbe *v = sqlite3GetVdbe(pParse);
    sqlite3VdbeSetNumCols(v, 2);
    sqlite3VdbeSetColName(v, 0, "database", P3_STATIC);
    sqlite3VdbeSetColName(v, 1, "status", P3_STATIC);
    for(i=0; i<db->nDb; i++){
      Btree *pBt;
      Pager *pPager;
      if( db->aDb[i].zName==0 ) continue;
      sqlite3VdbeOp3(v, OP_String, 0, 0, db->aDb[i].zName, P3_STATIC);
      pBt = db->aDb[i].pBt;
      if( pBt==0 || (pPager = sqlite3BtreePager(pBt))==0 ){
        sqlite3VdbeOp3(v, OP_String, 0, 0, "closed", P3_STATIC);
      }else{
        int j = sqlite3pager_lockstate(pPager);
        sqlite3VdbeOp3(v, OP_String, 0, 0, 
            (j>=0 && j<=4) ? azLockName[j] : "unknown", P3_STATIC);
      }
      sqlite3VdbeAddOp(v, OP_Callback, 2, 0);
    }
  }else
#endif

  {}
pragma_out:
  sqliteFree(zLeft);
  sqliteFree(zRight);
}
