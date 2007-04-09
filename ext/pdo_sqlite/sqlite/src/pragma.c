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
#include "os.h"
#include <ctype.h>

/* Ignore this whole file if pragmas are disabled
*/
#if !defined(SQLITE_OMIT_PRAGMA) && !defined(SQLITE_OMIT_PARSER)

#if defined(SQLITE_DEBUG) || defined(SQLITE_TEST)
# include "pager.h"
# include "btree.h"
#endif

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
static int getSafetyLevel(const char *z){
                             /* 123456789 123456789 */
  static const char zText[] = "onoffalseyestruefull";
  static const u8 iOffset[] = {0, 1, 2, 4, 9, 12, 16};
  static const u8 iLength[] = {2, 2, 3, 5, 3, 4, 4};
  static const u8 iValue[] =  {1, 0, 0, 0, 1, 1, 2};
  int i, n;
  if( isdigit(*z) ){
    return atoi(z);
  }
  n = strlen(z);
  for(i=0; i<sizeof(iLength); i++){
    if( iLength[i]==n && sqlite3StrNICmp(&zText[iOffset[i]],z,n)==0 ){
      return iValue[i];
    }
  }
  return 1;
}

/*
** Interpret the given string as a boolean value.
*/
static int getBoolean(const char *z){
  return getSafetyLevel(z)&1;
}

/*
** Interpret the given string as a locking mode value.
*/
static int getLockingMode(const char *z){
  if( z ){
    if( 0==sqlite3StrICmp(z, "exclusive") ) return PAGER_LOCKINGMODE_EXCLUSIVE;
    if( 0==sqlite3StrICmp(z, "normal") ) return PAGER_LOCKINGMODE_NORMAL;
  }
  return PAGER_LOCKINGMODE_QUERY;
}

#ifndef SQLITE_OMIT_PAGER_PRAGMAS
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
#endif /* SQLITE_PAGER_PRAGMAS */

#ifndef SQLITE_OMIT_PAGER_PRAGMAS
/*
** Invalidate temp storage, either when the temp storage is changed
** from default, or when 'file' and the temp_store_directory has changed
*/
static int invalidateTempStorage(Parse *pParse){
  sqlite3 *db = pParse->db;
  if( db->aDb[1].pBt!=0 ){
    if( !db->autoCommit ){
      sqlite3ErrorMsg(pParse, "temporary storage cannot be changed "
        "from within a transaction");
      return SQLITE_ERROR;
    }
    sqlite3BtreeClose(db->aDb[1].pBt);
    db->aDb[1].pBt = 0;
    sqlite3ResetInternalSchema(db, 0);
  }
  return SQLITE_OK;
}
#endif /* SQLITE_PAGER_PRAGMAS */

#ifndef SQLITE_OMIT_PAGER_PRAGMAS
/*
** If the TEMP database is open, close it and mark the database schema
** as needing reloading.  This must be done when using the TEMP_STORE
** or DEFAULT_TEMP_STORE pragmas.
*/
static int changeTempStorage(Parse *pParse, const char *zStorageType){
  int ts = getTempStore(zStorageType);
  sqlite3 *db = pParse->db;
  if( db->temp_store==ts ) return SQLITE_OK;
  if( invalidateTempStorage( pParse ) != SQLITE_OK ){
    return SQLITE_ERROR;
  }
  db->temp_store = ts;
  return SQLITE_OK;
}
#endif /* SQLITE_PAGER_PRAGMAS */

/*
** Generate code to return a single integer value.
*/
static void returnSingleInt(Parse *pParse, const char *zLabel, int value){
  Vdbe *v = sqlite3GetVdbe(pParse);
  sqlite3VdbeAddOp(v, OP_Integer, value, 0);
  if( pParse->explain==0 ){
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, zLabel, P3_STATIC);
  }
  sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
}

#ifndef SQLITE_OMIT_FLAG_PRAGMAS
/*
** Check to see if zRight and zLeft refer to a pragma that queries
** or changes one of the flags in db->flags.  Return 1 if so and 0 if not.
** Also, implement the pragma.
*/
static int flagPragma(Parse *pParse, const char *zLeft, const char *zRight){
  static const struct sPragmaType {
    const char *zName;  /* Name of the pragma */
    int mask;           /* Mask for the db->flags value */
  } aPragma[] = {
    { "vdbe_trace",               SQLITE_VdbeTrace     },
    { "sql_trace",                SQLITE_SqlTrace      },
    { "vdbe_listing",             SQLITE_VdbeListing   },
    { "full_column_names",        SQLITE_FullColNames  },
    { "short_column_names",       SQLITE_ShortColNames },
    { "count_changes",            SQLITE_CountRows     },
    { "empty_result_callbacks",   SQLITE_NullCallback  },
    { "legacy_file_format",       SQLITE_LegacyFileFmt },
    { "fullfsync",                SQLITE_FullFSync     },
#ifndef SQLITE_OMIT_CHECK
    { "ignore_check_constraints", SQLITE_IgnoreChecks  },
#endif
    /* The following is VERY experimental */
    { "writable_schema",          SQLITE_WriteSchema|SQLITE_RecoveryMode },
    { "omit_readlock",            SQLITE_NoReadlock    },

    /* TODO: Maybe it shouldn't be possible to change the ReadUncommitted
    ** flag if there are any active statements. */
    { "read_uncommitted",         SQLITE_ReadUncommitted },
  };
  int i;
  const struct sPragmaType *p;
  for(i=0, p=aPragma; i<sizeof(aPragma)/sizeof(aPragma[0]); i++, p++){
    if( sqlite3StrICmp(zLeft, p->zName)==0 ){
      sqlite3 *db = pParse->db;
      Vdbe *v;
      v = sqlite3GetVdbe(pParse);
      if( v ){
        if( zRight==0 ){
          returnSingleInt(pParse, p->zName, (db->flags & p->mask)!=0 );
        }else{
          if( getBoolean(zRight) ){
            db->flags |= p->mask;
          }else{
            db->flags &= ~p->mask;
          }
        }
      }
      return 1;
    }
  }
  return 0;
}
#endif /* SQLITE_OMIT_FLAG_PRAGMAS */

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

  /* If the temp database has been explicitly named as part of the 
  ** pragma, make sure it is open. 
  */
  if( iDb==1 && sqlite3OpenTempDatabase(pParse) ){
    return;
  }

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
 
#ifndef SQLITE_OMIT_PAGER_PRAGMAS
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
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "cache_size", P3_STATIC);
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
      pDb->pSchema->cache_size = size;
      sqlite3BtreeSetCacheSize(pDb->pBt, pDb->pSchema->cache_size);
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
      sqlite3BtreeSetPageSize(pBt, atoi(zRight), -1);
    }
  }else

  /*
  **  PRAGMA [database.]locking_mode
  **  PRAGMA [database.]locking_mode = (normal|exclusive)
  */
  if( sqlite3StrICmp(zLeft,"locking_mode")==0 ){
    const char *zRet = "normal";
    int eMode = getLockingMode(zRight);

    if( pId2->n==0 && eMode==PAGER_LOCKINGMODE_QUERY ){
      /* Simple "PRAGMA locking_mode;" statement. This is a query for
      ** the current default locking mode (which may be different to
      ** the locking-mode of the main database).
      */
      eMode = db->dfltLockMode;
    }else{
      Pager *pPager;
      if( pId2->n==0 ){
        /* This indicates that no database name was specified as part
        ** of the PRAGMA command. In this case the locking-mode must be
        ** set on all attached databases, as well as the main db file.
        **
        ** Also, the sqlite3.dfltLockMode variable is set so that
        ** any subsequently attached databases also use the specified
        ** locking mode.
        */
        int ii;
        assert(pDb==&db->aDb[0]);
        for(ii=2; ii<db->nDb; ii++){
          pPager = sqlite3BtreePager(db->aDb[ii].pBt);
          sqlite3PagerLockingMode(pPager, eMode);
        }
        db->dfltLockMode = eMode;
      }
      pPager = sqlite3BtreePager(pDb->pBt);
      eMode = sqlite3PagerLockingMode(pPager, eMode);
    }

    assert(eMode==PAGER_LOCKINGMODE_NORMAL||eMode==PAGER_LOCKINGMODE_EXCLUSIVE);
    if( eMode==PAGER_LOCKINGMODE_EXCLUSIVE ){
      zRet = "exclusive";
    }
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "locking_mode", P3_STATIC);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, zRet, 0);
    sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
  }else
#endif /* SQLITE_OMIT_PAGER_PRAGMAS */

  /*
  **  PRAGMA [database.]auto_vacuum
  **  PRAGMA [database.]auto_vacuum=N
  **
  ** Get or set the (boolean) value of the database 'auto-vacuum' parameter.
  */
#ifndef SQLITE_OMIT_AUTOVACUUM
  if( sqlite3StrICmp(zLeft,"auto_vacuum")==0 ){
    Btree *pBt = pDb->pBt;
    if( !zRight ){
      int auto_vacuum = 
          pBt ? sqlite3BtreeGetAutoVacuum(pBt) : SQLITE_DEFAULT_AUTOVACUUM;
      returnSingleInt(pParse, "auto_vacuum", auto_vacuum);
    }else{
      sqlite3BtreeSetAutoVacuum(pBt, getBoolean(zRight));
    }
  }else
#endif

#ifndef SQLITE_OMIT_PAGER_PRAGMAS
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
      returnSingleInt(pParse, "cache_size", pDb->pSchema->cache_size);
    }else{
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      pDb->pSchema->cache_size = size;
      sqlite3BtreeSetCacheSize(pDb->pBt, pDb->pSchema->cache_size);
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
  **   PRAGMA temp_store_directory
  **   PRAGMA temp_store_directory = ""|"directory_name"
  **
  ** Return or set the local value of the temp_store_directory flag.  Changing
  ** the value sets a specific directory to be used for temporary files.
  ** Setting to a null string reverts to the default temporary directory search.
  ** If temporary directory is changed, then invalidateTempStorage.
  **
  */
  if( sqlite3StrICmp(zLeft, "temp_store_directory")==0 ){
    if( !zRight ){
      if( sqlite3_temp_directory ){
        sqlite3VdbeSetNumCols(v, 1);
        sqlite3VdbeSetColName(v, 0, COLNAME_NAME, 
            "temp_store_directory", P3_STATIC);
        sqlite3VdbeOp3(v, OP_String8, 0, 0, sqlite3_temp_directory, 0);
        sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
      }
    }else{
      if( zRight[0] && !sqlite3OsIsDirWritable(zRight) ){
        sqlite3ErrorMsg(pParse, "not a writable directory");
        goto pragma_out;
      }
      if( TEMP_STORE==0
       || (TEMP_STORE==1 && db->temp_store<=1)
       || (TEMP_STORE==2 && db->temp_store==1)
      ){
        invalidateTempStorage(pParse);
      }
      sqliteFree(sqlite3_temp_directory);
      if( zRight[0] ){
        sqlite3_temp_directory = zRight;
        zRight = 0;
      }else{
        sqlite3_temp_directory = 0;
      }
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
      }
    }
  }else
#endif /* SQLITE_OMIT_PAGER_PRAGMAS */

#ifndef SQLITE_OMIT_FLAG_PRAGMAS
  if( flagPragma(pParse, zLeft, zRight) ){
    /* The flagPragma() subroutine also generates any necessary code
    ** there is nothing more to do here */
  }else
#endif /* SQLITE_OMIT_FLAG_PRAGMAS */

#ifndef SQLITE_OMIT_SCHEMA_PRAGMAS
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
      Column *pCol;
      sqlite3VdbeSetNumCols(v, 6);
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "cid", P3_STATIC);
      sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P3_STATIC);
      sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "type", P3_STATIC);
      sqlite3VdbeSetColName(v, 3, COLNAME_NAME, "notnull", P3_STATIC);
      sqlite3VdbeSetColName(v, 4, COLNAME_NAME, "dflt_value", P3_STATIC);
      sqlite3VdbeSetColName(v, 5, COLNAME_NAME, "pk", P3_STATIC);
      sqlite3ViewGetColumnNames(pParse, pTab);
      for(i=0, pCol=pTab->aCol; i<pTab->nCol; i++, pCol++){
        const Token *pDflt;
        sqlite3VdbeAddOp(v, OP_Integer, i, 0);
        sqlite3VdbeOp3(v, OP_String8, 0, 0, pCol->zName, 0);
        sqlite3VdbeOp3(v, OP_String8, 0, 0,
           pCol->zType ? pCol->zType : "", 0);
        sqlite3VdbeAddOp(v, OP_Integer, pCol->notNull, 0);
        if( pCol->pDflt && (pDflt = &pCol->pDflt->span)->z ){
          sqlite3VdbeOp3(v, OP_String8, 0, 0, (char*)pDflt->z, pDflt->n);
        }else{
          sqlite3VdbeAddOp(v, OP_Null, 0, 0);
        }
        sqlite3VdbeAddOp(v, OP_Integer, pCol->isPrimKey, 0);
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
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seqno", P3_STATIC);
      sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "cid", P3_STATIC);
      sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "name", P3_STATIC);
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
        sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P3_STATIC);
        sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P3_STATIC);
        sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "unique", P3_STATIC);
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

  if( sqlite3StrICmp(zLeft, "database_list")==0 ){
    int i;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    sqlite3VdbeSetNumCols(v, 3);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P3_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P3_STATIC);
    sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "file", P3_STATIC);
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

  if( sqlite3StrICmp(zLeft, "collation_list")==0 ){
    int i = 0;
    HashElem *p;
    sqlite3VdbeSetNumCols(v, 2);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P3_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P3_STATIC);
    for(p=sqliteHashFirst(&db->aCollSeq); p; p=sqliteHashNext(p)){
      CollSeq *pColl = (CollSeq *)sqliteHashData(p);
      sqlite3VdbeAddOp(v, OP_Integer, i++, 0);
      sqlite3VdbeOp3(v, OP_String8, 0, 0, pColl->zName, 0);
      sqlite3VdbeAddOp(v, OP_Callback, 2, 0);
    }
  }else
#endif /* SQLITE_OMIT_SCHEMA_PRAGMAS */

#ifndef SQLITE_OMIT_FOREIGN_KEY
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
        sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "id", P3_STATIC);
        sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "seq", P3_STATIC);
        sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "table", P3_STATIC);
        sqlite3VdbeSetColName(v, 3, COLNAME_NAME, "from", P3_STATIC);
        sqlite3VdbeSetColName(v, 4, COLNAME_NAME, "to", P3_STATIC);
        while(pFK){
          int j;
          for(j=0; j<pFK->nCol; j++){
            char *zCol = pFK->aCol[j].zCol;
            sqlite3VdbeAddOp(v, OP_Integer, i, 0);
            sqlite3VdbeAddOp(v, OP_Integer, j, 0);
            sqlite3VdbeOp3(v, OP_String8, 0, 0, pFK->zTo, 0);
            sqlite3VdbeOp3(v, OP_String8, 0, 0,
                             pTab->aCol[pFK->aCol[j].iFrom].zName, 0);
            sqlite3VdbeOp3(v, zCol ? OP_String8 : OP_Null, 0, 0, zCol, 0);
            sqlite3VdbeAddOp(v, OP_Callback, 5, 0);
          }
          ++i;
          pFK = pFK->pNextFrom;
        }
      }
    }
  }else
#endif /* !defined(SQLITE_OMIT_FOREIGN_KEY) */

#ifndef NDEBUG
  if( sqlite3StrICmp(zLeft, "parser_trace")==0 ){
    extern void sqlite3ParserTrace(FILE*, char *);
    if( zRight ){
      if( getBoolean(zRight) ){
        sqlite3ParserTrace(stderr, "parser: ");
      }else{
        sqlite3ParserTrace(0, 0);
      }
    }
  }else
#endif

  /* Reinstall the LIKE and GLOB functions.  The variant of LIKE
  ** used will be case sensitive or not depending on the RHS.
  */
  if( sqlite3StrICmp(zLeft, "case_sensitive_like")==0 ){
    if( zRight ){
      sqlite3RegisterLikeFunctions(db, getBoolean(zRight));
    }
  }else

#ifndef SQLITE_INTEGRITY_CHECK_ERROR_MAX
# define SQLITE_INTEGRITY_CHECK_ERROR_MAX 100
#endif

#ifndef SQLITE_OMIT_INTEGRITY_CHECK
  if( sqlite3StrICmp(zLeft, "integrity_check")==0 ){
    int i, j, addr, mxErr;

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
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "integrity_check", P3_STATIC);

    /* Set the maximum error count */
    mxErr = SQLITE_INTEGRITY_CHECK_ERROR_MAX;
    if( zRight ){
      mxErr = atoi(zRight);
      if( mxErr<=0 ){
        mxErr = SQLITE_INTEGRITY_CHECK_ERROR_MAX;
      }
    }
    sqlite3VdbeAddOp(v, OP_MemInt, mxErr, 0);

    /* Do an integrity check on each database file */
    for(i=0; i<db->nDb; i++){
      HashElem *x;
      Hash *pTbls;
      int cnt = 0;

      if( OMIT_TEMPDB && i==1 ) continue;

      sqlite3CodeVerifySchema(pParse, i);
      addr = sqlite3VdbeAddOp(v, OP_IfMemPos, 0, 0);
      sqlite3VdbeAddOp(v, OP_Halt, 0, 0);
      sqlite3VdbeJumpHere(v, addr);

      /* Do an integrity check of the B-Tree
      */
      pTbls = &db->aDb[i].pSchema->tblHash;
      for(x=sqliteHashFirst(pTbls); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        sqlite3VdbeAddOp(v, OP_Integer, pTab->tnum, 0);
        cnt++;
        for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
          sqlite3VdbeAddOp(v, OP_Integer, pIdx->tnum, 0);
          cnt++;
        }
      }
      if( cnt==0 ) continue;
      sqlite3VdbeAddOp(v, OP_IntegrityCk, 0, i);
      addr = sqlite3VdbeAddOp(v, OP_IsNull, -1, 0);
      sqlite3VdbeOp3(v, OP_String8, 0, 0,
         sqlite3MPrintf("*** in database %s ***\n", db->aDb[i].zName),
         P3_DYNAMIC);
      sqlite3VdbeAddOp(v, OP_Pull, 1, 0);
      sqlite3VdbeAddOp(v, OP_Concat, 0, 0);
      sqlite3VdbeAddOp(v, OP_Callback, 1, 0);
      sqlite3VdbeJumpHere(v, addr);

      /* Make sure all the indices are constructed correctly.
      */
      for(x=sqliteHashFirst(pTbls); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        int loopTop;

        if( pTab->pIndex==0 ) continue;
        addr = sqlite3VdbeAddOp(v, OP_IfMemPos, 0, 0);
        sqlite3VdbeAddOp(v, OP_Halt, 0, 0);
        sqlite3VdbeJumpHere(v, addr);
        sqlite3OpenTableAndIndices(pParse, pTab, 1, OP_OpenRead);
        sqlite3VdbeAddOp(v, OP_MemInt, 0, 1);
        loopTop = sqlite3VdbeAddOp(v, OP_Rewind, 1, 0);
        sqlite3VdbeAddOp(v, OP_MemIncr, 1, 1);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          int jmp2;
          static const VdbeOpList idxErr[] = {
            { OP_MemIncr,    -1,  0,  0},
            { OP_String8,     0,  0,  "rowid "},
            { OP_Rowid,       1,  0,  0},
            { OP_String8,     0,  0,  " missing from index "},
            { OP_String8,     0,  0,  0},    /* 4 */
            { OP_Concat,      2,  0,  0},
            { OP_Callback,    1,  0,  0},
          };
          sqlite3GenerateIndexKey(v, pIdx, 1);
          jmp2 = sqlite3VdbeAddOp(v, OP_Found, j+2, 0);
          addr = sqlite3VdbeAddOpList(v, ArraySize(idxErr), idxErr);
          sqlite3VdbeChangeP3(v, addr+4, pIdx->zName, P3_STATIC);
          sqlite3VdbeJumpHere(v, jmp2);
        }
        sqlite3VdbeAddOp(v, OP_Next, 1, loopTop+1);
        sqlite3VdbeJumpHere(v, loopTop);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          static const VdbeOpList cntIdx[] = {
             { OP_MemInt,       0,  2,  0},
             { OP_Rewind,       0,  0,  0},  /* 1 */
             { OP_MemIncr,      1,  2,  0},
             { OP_Next,         0,  0,  0},  /* 3 */
             { OP_MemLoad,      1,  0,  0},
             { OP_MemLoad,      2,  0,  0},
             { OP_Eq,           0,  0,  0},  /* 6 */
             { OP_MemIncr,     -1,  0,  0},
             { OP_String8,      0,  0,  "wrong # of entries in index "},
             { OP_String8,      0,  0,  0},  /* 9 */
             { OP_Concat,       0,  0,  0},
             { OP_Callback,     1,  0,  0},
          };
          if( pIdx->tnum==0 ) continue;
          addr = sqlite3VdbeAddOp(v, OP_IfMemPos, 0, 0);
          sqlite3VdbeAddOp(v, OP_Halt, 0, 0);
          sqlite3VdbeJumpHere(v, addr);
          addr = sqlite3VdbeAddOpList(v, ArraySize(cntIdx), cntIdx);
          sqlite3VdbeChangeP1(v, addr+1, j+2);
          sqlite3VdbeChangeP2(v, addr+1, addr+4);
          sqlite3VdbeChangeP1(v, addr+3, j+2);
          sqlite3VdbeChangeP2(v, addr+3, addr+2);
          sqlite3VdbeJumpHere(v, addr+6);
          sqlite3VdbeChangeP3(v, addr+9, pIdx->zName, P3_STATIC);
        }
      } 
    }
    addr = sqlite3VdbeAddOpList(v, ArraySize(endCode), endCode);
    sqlite3VdbeChangeP1(v, addr+1, mxErr);
    sqlite3VdbeJumpHere(v, addr+2);
  }else
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

#ifndef SQLITE_OMIT_UTF16
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
    static const struct EncName {
      char *zName;
      u8 enc;
    } encnames[] = {
      { "UTF-8",    SQLITE_UTF8        },
      { "UTF8",     SQLITE_UTF8        },
      { "UTF-16le", SQLITE_UTF16LE     },
      { "UTF16le",  SQLITE_UTF16LE     },
      { "UTF-16be", SQLITE_UTF16BE     },
      { "UTF16be",  SQLITE_UTF16BE     },
      { "UTF-16",   0                  }, /* SQLITE_UTF16NATIVE */
      { "UTF16",    0                  }, /* SQLITE_UTF16NATIVE */
      { 0, 0 }
    };
    const struct EncName *pEnc;
    if( !zRight ){    /* "PRAGMA encoding" */
      if( sqlite3ReadSchema(pParse) ) goto pragma_out;
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "encoding", P3_STATIC);
      sqlite3VdbeAddOp(v, OP_String8, 0, 0);
      for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
        if( pEnc->enc==ENC(pParse->db) ){
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
      if( 
        !(DbHasProperty(db, 0, DB_SchemaLoaded)) || 
        DbHasProperty(db, 0, DB_Empty) 
      ){
        for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
          if( 0==sqlite3StrICmp(zRight, pEnc->zName) ){
            ENC(pParse->db) = pEnc->enc ? pEnc->enc : SQLITE_UTF16NATIVE;
            break;
          }
        }
        if( !pEnc->zName ){
          sqlite3ErrorMsg(pParse, "unsupported encoding: %s", zRight);
        }
      }
    }
  }else
#endif /* SQLITE_OMIT_UTF16 */

#ifndef SQLITE_OMIT_SCHEMA_VERSION_PRAGMAS
  /*
  **   PRAGMA [database.]schema_version
  **   PRAGMA [database.]schema_version = <integer>
  **
  **   PRAGMA [database.]user_version
  **   PRAGMA [database.]user_version = <integer>
  **
  ** The pragma's schema_version and user_version are used to set or get
  ** the value of the schema-version and user-version, respectively. Both
  ** the schema-version and the user-version are 32-bit signed integers
  ** stored in the database header.
  **
  ** The schema-cookie is usually only manipulated internally by SQLite. It
  ** is incremented by SQLite whenever the database schema is modified (by
  ** creating or dropping a table or index). The schema version is used by
  ** SQLite each time a query is executed to ensure that the internal cache
  ** of the schema used when compiling the SQL query matches the schema of
  ** the database against which the compiled query is actually executed.
  ** Subverting this mechanism by using "PRAGMA schema_version" to modify
  ** the schema-version is potentially dangerous and may lead to program
  ** crashes or database corruption. Use with caution!
  **
  ** The user-version is not used internally by SQLite. It may be used by
  ** applications for any purpose.
  */
  if( sqlite3StrICmp(zLeft, "schema_version")==0 ||
      sqlite3StrICmp(zLeft, "user_version")==0 ){

    int iCookie;   /* Cookie index. 0 for schema-cookie, 6 for user-cookie. */
    if( zLeft[0]=='s' || zLeft[0]=='S' ){
      iCookie = 0;
    }else{
      iCookie = 5;
    }

    if( zRight ){
      /* Write the specified cookie value */
      static const VdbeOpList setCookie[] = {
        { OP_Transaction,    0,  1,  0},    /* 0 */
        { OP_Integer,        0,  0,  0},    /* 1 */
        { OP_SetCookie,      0,  0,  0},    /* 2 */
      };
      int addr = sqlite3VdbeAddOpList(v, ArraySize(setCookie), setCookie);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP1(v, addr+1, atoi(zRight));
      sqlite3VdbeChangeP1(v, addr+2, iDb);
      sqlite3VdbeChangeP2(v, addr+2, iCookie);
    }else{
      /* Read the specified cookie value */
      static const VdbeOpList readCookie[] = {
        { OP_ReadCookie,      0,  0,  0},    /* 0 */
        { OP_Callback,        1,  0,  0}
      };
      int addr = sqlite3VdbeAddOpList(v, ArraySize(readCookie), readCookie);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP2(v, addr, iCookie);
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, zLeft, P3_TRANSIENT);
    }
  }else
#endif /* SQLITE_OMIT_SCHEMA_VERSION_PRAGMAS */

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
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "database", P3_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "status", P3_STATIC);
    for(i=0; i<db->nDb; i++){
      Btree *pBt;
      Pager *pPager;
      if( db->aDb[i].zName==0 ) continue;
      sqlite3VdbeOp3(v, OP_String8, 0, 0, db->aDb[i].zName, P3_STATIC);
      pBt = db->aDb[i].pBt;
      if( pBt==0 || (pPager = sqlite3BtreePager(pBt))==0 ){
        sqlite3VdbeOp3(v, OP_String8, 0, 0, "closed", P3_STATIC);
      }else{
        int j = sqlite3PagerLockstate(pPager);
        sqlite3VdbeOp3(v, OP_String8, 0, 0, 
            (j>=0 && j<=4) ? azLockName[j] : "unknown", P3_STATIC);
      }
      sqlite3VdbeAddOp(v, OP_Callback, 2, 0);
    }
  }else
#endif

#ifdef SQLITE_SSE
  /*
  ** Check to see if the sqlite_statements table exists.  Create it
  ** if it does not.
  */
  if( sqlite3StrICmp(zLeft, "create_sqlite_statement_table")==0 ){
    extern int sqlite3CreateStatementsTable(Parse*);
    sqlite3CreateStatementsTable(pParse);
  }else
#endif

#if SQLITE_HAS_CODEC
  if( sqlite3StrICmp(zLeft, "key")==0 ){
    sqlite3_key(db, zRight, strlen(zRight));
  }else
#endif
#if SQLITE_HAS_CODEC || defined(SQLITE_ENABLE_CEROD)
  if( sqlite3StrICmp(zLeft, "activate_extensions")==0 ){
#if SQLITE_HAS_CODEC
    if( sqlite3StrNICmp(zRight, "see-", 4)==0 ){
      extern void sqlite3_activate_see(const char*);
      sqlite3_activate_see(&zRight[4]);
    }
#endif
#ifdef SQLITE_ENABLE_CEROD
    if( sqlite3StrNICmp(zRight, "cerod-", 6)==0 ){
      extern void sqlite3_activate_cerod(const char*);
      sqlite3_activate_cerod(&zRight[6]);
    }
#endif
  }
#endif

  {}

  if( v ){
    /* Code an OP_Expire at the end of each PRAGMA program to cause
    ** the VDBE implementing the pragma to expire. Most (all?) pragmas
    ** are only valid for a single execution.
    */
    sqlite3VdbeAddOp(v, OP_Expire, 1, 0);

    /*
    ** Reset the safety level, in case the fullfsync flag or synchronous
    ** setting changed.
    */
#ifndef SQLITE_OMIT_PAGER_PRAGMAS
    if( db->autoCommit ){
      sqlite3BtreeSetSafetyLevel(pDb->pBt, pDb->safety_level,
                 (db->flags&SQLITE_FullFSync)!=0);
    }
#endif
  }
pragma_out:
  sqliteFree(zLeft);
  sqliteFree(zRight);
}

#endif /* SQLITE_OMIT_PRAGMA || SQLITE_OMIT_PARSER */
