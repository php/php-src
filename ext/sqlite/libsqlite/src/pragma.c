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

/*
** Interpret the given string as a boolean value.
*/
static int getBoolean(const char *z){
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
** 1 for ON or NORMAL and 2 for FULL.  Return 1 for an empty or 
** unrecognized string argument.
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
** Interpret the given string as a temp db location. Return 1 for file
** backed temporary databases, 2 for the Red-Black tree in memory database
** and 0 to use the compile-time default.
*/
static int getTempStore(const char *z){
  if( z[0]>='0' && z[0]<='2' ){
    return z[0] - '0';
  }else if( sqliteStrICmp(z, "file")==0 ){
    return 1;
  }else if( sqliteStrICmp(z, "memory")==0 ){
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
  sqlite *db = pParse->db;
  if( db->temp_store==ts ) return SQLITE_OK;
  if( db->aDb[1].pBt!=0 ){
    if( db->flags & SQLITE_InTrans ){
      sqliteErrorMsg(pParse, "temporary storage cannot be changed "
        "from within a transaction");
      return SQLITE_ERROR;
    }
    sqliteBtreeClose(db->aDb[1].pBt);
    db->aDb[1].pBt = 0;
    sqliteResetInternalSchema(db, 0);
  }
  db->temp_store = ts;
  return SQLITE_OK;
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
    { "full_column_names",        SQLITE_FullColNames  },
    { "short_column_names",       SQLITE_ShortColNames },
    { "show_datatypes",           SQLITE_ReportTypes   },
    { "count_changes",            SQLITE_CountRows     },
    { "empty_result_callbacks",   SQLITE_NullCallback  },
  };
  int i;
  for(i=0; i<sizeof(aPragma)/sizeof(aPragma[0]); i++){
    if( sqliteStrICmp(zLeft, aPragma[i].zName)==0 ){
      sqlite *db = pParse->db;
      Vdbe *v;
      if( strcmp(zLeft,zRight)==0 && (v = sqliteGetVdbe(pParse))!=0 ){
        sqliteVdbeOp3(v, OP_ColumnName, 0, 1, aPragma[i].zName, P3_STATIC);
        sqliteVdbeOp3(v, OP_ColumnName, 1, 0, "boolean", P3_STATIC);
        sqliteVdbeCode(v, OP_Integer, (db->flags & aPragma[i].mask)!=0, 0,
                          OP_Callback, 1, 0,
                          0);
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
  Vdbe *v = sqliteGetVdbe(pParse);
  if( v==0 ) return;

  zLeft = sqliteStrNDup(pLeft->z, pLeft->n);
  sqliteDequote(zLeft);
  if( minusFlag ){
    zRight = 0;
    sqliteSetNString(&zRight, "-", 1, pRight->z, pRight->n, 0);
  }else{
    zRight = sqliteStrNDup(pRight->z, pRight->n);
    sqliteDequote(zRight);
  }
  if( sqliteAuthCheck(pParse, SQLITE_PRAGMA, zLeft, zRight, 0) ){
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
    static VdbeOpList getCacheSize[] = {
      { OP_ReadCookie,  0, 2,        0},
      { OP_AbsValue,    0, 0,        0},
      { OP_Dup,         0, 0,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Ne,          0, 6,        0},
      { OP_Integer,     0, 0,        0},  /* 5 */
      { OP_ColumnName,  0, 1,        "cache_size"},
      { OP_Callback,    1, 0,        0},
    };
    int addr;
    if( pRight->z==pLeft->z ){
      addr = sqliteVdbeAddOpList(v, ArraySize(getCacheSize), getCacheSize);
      sqliteVdbeChangeP1(v, addr+5, MAX_PAGES);
    }else{
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
      sqliteBtreeSetCacheSize(db->aDb[0].pBt, db->cache_size);
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
    static VdbeOpList getCacheSize[] = {
      { OP_ColumnName,  0, 1,        "cache_size"},
      { OP_Callback,    1, 0,        0},
    };
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
      sqliteBtreeSetCacheSize(db->aDb[0].pBt, db->cache_size);
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
    static VdbeOpList getSync[] = {
      { OP_ColumnName,  0, 1,        "synchronous"},
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
      sqliteBtreeSetCacheSize(db->aDb[0].pBt, db->cache_size);
      sqliteBtreeSetSafetyLevel(db->aDb[0].pBt, db->safety_level);
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
    static VdbeOpList getSync[] = {
      { OP_ColumnName,  0, 1,        "synchronous"},
      { OP_Callback,    1, 0,        0},
    };
    if( pRight->z==pLeft->z ){
      sqliteVdbeAddOp(v, OP_Integer, db->safety_level-1, 0);
      sqliteVdbeAddOpList(v, ArraySize(getSync), getSync);
    }else{
      int size = db->cache_size;
      if( size<0 ) size = -size;
      db->safety_level = getSafetyLevel(zRight)+1;
      if( db->safety_level==1 ) size = -size;
      db->cache_size = size;
      sqliteBtreeSetCacheSize(db->aDb[0].pBt, db->cache_size);
      sqliteBtreeSetSafetyLevel(db->aDb[0].pBt, db->safety_level);
    }
  }else

#ifndef NDEBUG
  if( sqliteStrICmp(zLeft, "trigger_overhead_test")==0 ){
    if( getBoolean(zRight) ){
      always_code_trigger_setup = 1;
    }else{
      always_code_trigger_setup = 0;
    }
  }else
#endif

  if( flagPragma(pParse, zLeft, zRight) ){
    /* The flagPragma() call also generates any necessary code */
  }else

  if( sqliteStrICmp(zLeft, "table_info")==0 ){
    Table *pTab;
    pTab = sqliteFindTable(db, zRight, 0);
    if( pTab ){
      static VdbeOpList tableInfoPreface[] = {
        { OP_ColumnName,  0, 0,       "cid"},
        { OP_ColumnName,  1, 0,       "name"},
        { OP_ColumnName,  2, 0,       "type"},
        { OP_ColumnName,  3, 0,       "notnull"},
        { OP_ColumnName,  4, 0,       "dflt_value"},
        { OP_ColumnName,  5, 1,       "pk"},
      };
      int i;
      sqliteVdbeAddOpList(v, ArraySize(tableInfoPreface), tableInfoPreface);
      sqliteViewGetColumnNames(pParse, pTab);
      for(i=0; i<pTab->nCol; i++){
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeOp3(v, OP_String, 0, 0, pTab->aCol[i].zName, 0);
        sqliteVdbeOp3(v, OP_String, 0, 0,
           pTab->aCol[i].zType ? pTab->aCol[i].zType : "numeric", 0);
        sqliteVdbeAddOp(v, OP_Integer, pTab->aCol[i].notNull, 0);
        sqliteVdbeOp3(v, OP_String, 0, 0,
           pTab->aCol[i].zDflt, P3_STATIC);
        sqliteVdbeAddOp(v, OP_Integer, pTab->aCol[i].isPrimKey, 0);
        sqliteVdbeAddOp(v, OP_Callback, 6, 0);
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "index_info")==0 ){
    Index *pIdx;
    Table *pTab;
    pIdx = sqliteFindIndex(db, zRight, 0);
    if( pIdx ){
      static VdbeOpList tableInfoPreface[] = {
        { OP_ColumnName,  0, 0,       "seqno"},
        { OP_ColumnName,  1, 0,       "cid"},
        { OP_ColumnName,  2, 1,       "name"},
      };
      int i;
      pTab = pIdx->pTable;
      sqliteVdbeAddOpList(v, ArraySize(tableInfoPreface), tableInfoPreface);
      for(i=0; i<pIdx->nColumn; i++){
        int cnum = pIdx->aiColumn[i];
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeAddOp(v, OP_Integer, cnum, 0);
        assert( pTab->nCol>cnum );
        sqliteVdbeOp3(v, OP_String, 0, 0, pTab->aCol[cnum].zName, 0);
        sqliteVdbeAddOp(v, OP_Callback, 3, 0);
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "index_list")==0 ){
    Index *pIdx;
    Table *pTab;
    pTab = sqliteFindTable(db, zRight, 0);
    if( pTab ){
      v = sqliteGetVdbe(pParse);
      pIdx = pTab->pIndex;
    }
    if( pTab && pIdx ){
      int i = 0; 
      static VdbeOpList indexListPreface[] = {
        { OP_ColumnName,  0, 0,       "seq"},
        { OP_ColumnName,  1, 0,       "name"},
        { OP_ColumnName,  2, 1,       "unique"},
      };

      sqliteVdbeAddOpList(v, ArraySize(indexListPreface), indexListPreface);
      while(pIdx){
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeOp3(v, OP_String, 0, 0, pIdx->zName, 0);
        sqliteVdbeAddOp(v, OP_Integer, pIdx->onError!=OE_None, 0);
        sqliteVdbeAddOp(v, OP_Callback, 3, 0);
        ++i;
        pIdx = pIdx->pNext;
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "foreign_key_list")==0 ){
    FKey *pFK;
    Table *pTab;
    pTab = sqliteFindTable(db, zRight, 0);
    if( pTab ){
      v = sqliteGetVdbe(pParse);
      pFK = pTab->pFKey;
    }
    if( pTab && pFK ){
      int i = 0; 
      static VdbeOpList indexListPreface[] = {
        { OP_ColumnName,  0, 0,       "id"},
        { OP_ColumnName,  1, 0,       "seq"},
        { OP_ColumnName,  2, 0,       "table"},
        { OP_ColumnName,  3, 0,       "from"},
        { OP_ColumnName,  4, 1,       "to"},
      };

      sqliteVdbeAddOpList(v, ArraySize(indexListPreface), indexListPreface);
      while(pFK){
        int j;
        for(j=0; j<pFK->nCol; j++){
          sqliteVdbeAddOp(v, OP_Integer, i, 0);
          sqliteVdbeAddOp(v, OP_Integer, j, 0);
          sqliteVdbeOp3(v, OP_String, 0, 0, pFK->zTo, 0);
          sqliteVdbeOp3(v, OP_String, 0, 0,
                           pTab->aCol[pFK->aCol[j].iFrom].zName, 0);
          sqliteVdbeOp3(v, OP_String, 0, 0, pFK->aCol[j].zCol, 0);
          sqliteVdbeAddOp(v, OP_Callback, 5, 0);
        }
        ++i;
        pFK = pFK->pNextFrom;
      }
    }
  }else

  if( sqliteStrICmp(zLeft, "database_list")==0 ){
    int i;
    static VdbeOpList indexListPreface[] = {
      { OP_ColumnName,  0, 0,       "seq"},
      { OP_ColumnName,  1, 0,       "name"},
      { OP_ColumnName,  2, 1,       "file"},
    };

    sqliteVdbeAddOpList(v, ArraySize(indexListPreface), indexListPreface);
    for(i=0; i<db->nDb; i++){
      if( db->aDb[i].pBt==0 ) continue;
      assert( db->aDb[i].zName!=0 );
      sqliteVdbeAddOp(v, OP_Integer, i, 0);
      sqliteVdbeOp3(v, OP_String, 0, 0, db->aDb[i].zName, 0);
      sqliteVdbeOp3(v, OP_String, 0, 0,
           sqliteBtreeGetFilename(db->aDb[i].pBt), 0);
      sqliteVdbeAddOp(v, OP_Callback, 3, 0);
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
  if( sqliteStrICmp(zLeft, "temp_store")==0 ){
    static VdbeOpList getTmpDbLoc[] = {
      { OP_ColumnName,  0, 1,        "temp_store"},
      { OP_Callback,    1, 0,        0},
    };
    if( pRight->z==pLeft->z ){
      sqliteVdbeAddOp(v, OP_Integer, db->temp_store, 0);
      sqliteVdbeAddOpList(v, ArraySize(getTmpDbLoc), getTmpDbLoc);
    }else{
      changeTempStorage(pParse, zRight);
    }
  }else

  /*
  **   PRAGMA default_temp_store
  **   PRAGMA default_temp_store = "default"|"memory"|"file"
  **
  ** Return or set the value of the persistent temp_store flag.  Any
  ** change does not take effect until the next time the database is
  ** opened.
  **
  ** Note that it is possible for the library compile-time options to
  ** override this setting
  */
  if( sqliteStrICmp(zLeft, "default_temp_store")==0 ){
    static VdbeOpList getTmpDbLoc[] = {
      { OP_ColumnName,  0, 1,        "temp_store"},
      { OP_ReadCookie,  0, 5,        0},
      { OP_Callback,    1, 0,        0}};
    if( pRight->z==pLeft->z ){
      sqliteVdbeAddOpList(v, ArraySize(getTmpDbLoc), getTmpDbLoc);
    }else{
      sqliteBeginWriteOperation(pParse, 0, 0);
      sqliteVdbeAddOp(v, OP_Integer, getTempStore(zRight), 0);
      sqliteVdbeAddOp(v, OP_SetCookie, 0, 5);
      sqliteEndWriteOperation(pParse);
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
    int i, j, addr;

    /* Code that initializes the integrity check program.  Set the
    ** error count 0
    */
    static VdbeOpList initCode[] = {
      { OP_Integer,     0, 0,        0},
      { OP_MemStore,    0, 1,        0},
      { OP_ColumnName,  0, 1,        "integrity_check"},
    };

    /* Code to do an BTree integrity check on a single database file.
    */
    static VdbeOpList checkDb[] = {
      { OP_SetInsert,   0, 0,        "2"},
      { OP_Integer,     0, 0,        0},    /* 1 */
      { OP_OpenRead,    0, 2,        0},
      { OP_Rewind,      0, 7,        0},    /* 3 */
      { OP_Column,      0, 3,        0},    /* 4 */
      { OP_SetInsert,   0, 0,        0},
      { OP_Next,        0, 4,        0},    /* 6 */
      { OP_IntegrityCk, 0, 0,        0},    /* 7 */
      { OP_Dup,         0, 1,        0},
      { OP_String,      0, 0,        "ok"},
      { OP_StrEq,       0, 12,       0},    /* 10 */
      { OP_MemIncr,     0, 0,        0},
      { OP_String,      0, 0,        "*** in database "},
      { OP_String,      0, 0,        0},    /* 13 */
      { OP_String,      0, 0,        " ***\n"},
      { OP_Pull,        3, 0,        0},
      { OP_Concat,      4, 1,        0},
      { OP_Callback,    1, 0,        0},
    };

    /* Code that appears at the end of the integrity check.  If no error
    ** messages have been generated, output OK.  Otherwise output the
    ** error message
    */
    static VdbeOpList endCode[] = {
      { OP_MemLoad,     0, 0,        0},
      { OP_Integer,     0, 0,        0},
      { OP_Ne,          0, 0,        0},    /* 2 */
      { OP_String,      0, 0,        "ok"},
      { OP_Callback,    1, 0,        0},
    };

    /* Initialize the VDBE program */
    sqliteVdbeAddOpList(v, ArraySize(initCode), initCode);

    /* Do an integrity check on each database file */
    for(i=0; i<db->nDb; i++){
      HashElem *x;

      /* Do an integrity check of the B-Tree
      */
      addr = sqliteVdbeAddOpList(v, ArraySize(checkDb), checkDb);
      sqliteVdbeChangeP1(v, addr+1, i);
      sqliteVdbeChangeP2(v, addr+3, addr+7);
      sqliteVdbeChangeP2(v, addr+6, addr+4);
      sqliteVdbeChangeP2(v, addr+7, i);
      sqliteVdbeChangeP2(v, addr+10, addr+ArraySize(checkDb));
      sqliteVdbeChangeP3(v, addr+13, db->aDb[i].zName, P3_STATIC);

      /* Make sure all the indices are constructed correctly.
      */
      sqliteCodeVerifySchema(pParse, i);
      for(x=sqliteHashFirst(&db->aDb[i].tblHash); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        int loopTop;

        if( pTab->pIndex==0 ) continue;
        sqliteVdbeAddOp(v, OP_Integer, i, 0);
        sqliteVdbeOp3(v, OP_OpenRead, 1, pTab->tnum, pTab->zName, 0);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          if( pIdx->tnum==0 ) continue;
          sqliteVdbeAddOp(v, OP_Integer, pIdx->iDb, 0);
          sqliteVdbeOp3(v, OP_OpenRead, j+2, pIdx->tnum, pIdx->zName, 0);
        }
        sqliteVdbeAddOp(v, OP_Integer, 0, 0);
        sqliteVdbeAddOp(v, OP_MemStore, 1, 1);
        loopTop = sqliteVdbeAddOp(v, OP_Rewind, 1, 0);
        sqliteVdbeAddOp(v, OP_MemIncr, 1, 0);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          int k, jmp2;
          static VdbeOpList idxErr[] = {
            { OP_MemIncr,     0,  0,  0},
            { OP_String,      0,  0,  "rowid "},
            { OP_Recno,       1,  0,  0},
            { OP_String,      0,  0,  " missing from index "},
            { OP_String,      0,  0,  0},    /* 4 */
            { OP_Concat,      4,  0,  0},
            { OP_Callback,    1,  0,  0},
          };
          sqliteVdbeAddOp(v, OP_Recno, 1, 0);
          for(k=0; k<pIdx->nColumn; k++){
            int idx = pIdx->aiColumn[k];
            if( idx==pTab->iPKey ){
              sqliteVdbeAddOp(v, OP_Recno, 1, 0);
            }else{
              sqliteVdbeAddOp(v, OP_Column, 1, idx);
            }
          }
          sqliteVdbeAddOp(v, OP_MakeIdxKey, pIdx->nColumn, 0);
          if( db->file_format>=4 ) sqliteAddIdxKeyType(v, pIdx);
          jmp2 = sqliteVdbeAddOp(v, OP_Found, j+2, 0);
          addr = sqliteVdbeAddOpList(v, ArraySize(idxErr), idxErr);
          sqliteVdbeChangeP3(v, addr+4, pIdx->zName, P3_STATIC);
          sqliteVdbeChangeP2(v, jmp2, sqliteVdbeCurrentAddr(v));
        }
        sqliteVdbeAddOp(v, OP_Next, 1, loopTop+1);
        sqliteVdbeChangeP2(v, loopTop, sqliteVdbeCurrentAddr(v));
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          static VdbeOpList cntIdx[] = {
             { OP_Integer,      0,  0,  0},
             { OP_MemStore,     2,  1,  0},
             { OP_Rewind,       0,  0,  0},  /* 2 */
             { OP_MemIncr,      2,  0,  0},
             { OP_Next,         0,  0,  0},  /* 4 */
             { OP_MemLoad,      1,  0,  0},
             { OP_MemLoad,      2,  0,  0},
             { OP_Eq,           0,  0,  0},  /* 7 */
             { OP_MemIncr,      0,  0,  0},
             { OP_String,       0,  0,  "wrong # of entries in index "},
             { OP_String,       0,  0,  0},  /* 10 */
             { OP_Concat,       2,  0,  0},
             { OP_Callback,     1,  0,  0},
          };
          if( pIdx->tnum==0 ) continue;
          addr = sqliteVdbeAddOpList(v, ArraySize(cntIdx), cntIdx);
          sqliteVdbeChangeP1(v, addr+2, j+2);
          sqliteVdbeChangeP2(v, addr+2, addr+5);
          sqliteVdbeChangeP1(v, addr+4, j+2);
          sqliteVdbeChangeP2(v, addr+4, addr+3);
          sqliteVdbeChangeP2(v, addr+7, addr+ArraySize(cntIdx));
          sqliteVdbeChangeP3(v, addr+10, pIdx->zName, P3_STATIC);
        }
      } 
    }
    addr = sqliteVdbeAddOpList(v, ArraySize(endCode), endCode);
    sqliteVdbeChangeP2(v, addr+2, addr+ArraySize(endCode));
  }else

  {}
  sqliteFree(zLeft);
  sqliteFree(zRight);
}
