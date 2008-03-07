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

/* Ignore this whole file if pragmas are disabled
*/
#if !defined(SQLITE_OMIT_PRAGMA) && !defined(SQLITE_OMIT_PARSER)

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

#ifndef SQLITE_OMIT_AUTOVACUUM
/*
** Interpret the given string as an auto-vacuum mode value.
**
** The following strings, "none", "full" and "incremental" are 
** acceptable, as are their numeric equivalents: 0, 1 and 2 respectively.
*/
static int getAutoVacuum(const char *z){
  int i;
  if( 0==sqlite3StrICmp(z, "none") ) return BTREE_AUTOVACUUM_NONE;
  if( 0==sqlite3StrICmp(z, "full") ) return BTREE_AUTOVACUUM_FULL;
  if( 0==sqlite3StrICmp(z, "incremental") ) return BTREE_AUTOVACUUM_INCR;
  i = atoi(z);
  return ((i>=0&&i<=2)?i:0);
}
#endif /* ifndef SQLITE_OMIT_AUTOVACUUM */

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
  int mem = ++pParse->nMem;
  sqlite3VdbeAddOp2(v, OP_Integer, value, mem);
  if( pParse->explain==0 ){
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, zLabel, P4_STATIC);
  }
  sqlite3VdbeAddOp2(v, OP_ResultRow, mem, 1);
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
    { "full_column_names",        SQLITE_FullColNames  },
    { "short_column_names",       SQLITE_ShortColNames },
    { "count_changes",            SQLITE_CountRows     },
    { "empty_result_callbacks",   SQLITE_NullCallback  },
    { "legacy_file_format",       SQLITE_LegacyFileFmt },
    { "fullfsync",                SQLITE_FullFSync     },
#ifdef SQLITE_DEBUG
    { "sql_trace",                SQLITE_SqlTrace      },
    { "vdbe_listing",             SQLITE_VdbeListing   },
    { "vdbe_trace",               SQLITE_VdbeTrace     },
#endif
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

          /* Many of the flag-pragmas modify the code generated by the SQL 
          ** compiler (eg. count_changes). So add an opcode to expire all
          ** compiled SQL statements after modifying a pragma value.
          */
          sqlite3VdbeAddOp2(v, OP_Expire, 0, 0);
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
  Vdbe *v = pParse->pVdbe = sqlite3VdbeCreate(db);
  if( v==0 ) return;
  pParse->nMem = 2;

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

  zLeft = sqlite3NameFromToken(db, pId);
  if( !zLeft ) return;
  if( minusFlag ){
    zRight = sqlite3MPrintf(db, "-%T", pValue);
  }else{
    zRight = sqlite3NameFromToken(db, pValue);
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
      { OP_ReadCookie,  0, 1,        2},  /* 0 */
      { OP_IfPos,       1, 6,        0},
      { OP_Integer,     0, 2,        0},
      { OP_Subtract,    1, 2,        1},
      { OP_IfPos,       1, 6,        0},
      { OP_Integer,     0, 1,        0},  /* 5 */
      { OP_ResultRow,   1, 1,        0},
    };
    int addr;
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    sqlite3VdbeUsesBtree(v, iDb);
    if( !zRight ){
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "cache_size", P4_STATIC);
      pParse->nMem += 2;
      addr = sqlite3VdbeAddOpList(v, ArraySize(getCacheSize), getCacheSize);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP1(v, addr+5, SQLITE_DEFAULT_CACHE_SIZE);
    }else{
      int size = atoi(zRight);
      if( size<0 ) size = -size;
      sqlite3BeginWriteOperation(pParse, 0, iDb);
      sqlite3VdbeAddOp2(v, OP_Integer, size, 1);
      sqlite3VdbeAddOp3(v, OP_ReadCookie, iDb, 2, 2);
      addr = sqlite3VdbeAddOp2(v, OP_IfPos, 2, 0);
      sqlite3VdbeAddOp2(v, OP_Integer, -size, 1);
      sqlite3VdbeJumpHere(v, addr);
      sqlite3VdbeAddOp3(v, OP_SetCookie, iDb, 2, 1);
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
      /* Malloc may fail when setting the page-size, as there is an internal
      ** buffer that the pager module resizes using sqlite3_realloc().
      */
      if( SQLITE_NOMEM==sqlite3BtreeSetPageSize(pBt, atoi(zRight), -1) ){
        db->mallocFailed = 1;
      }
    }
  }else

  /*
  **  PRAGMA [database.]max_page_count
  **  PRAGMA [database.]max_page_count=N
  **
  ** The first form reports the current setting for the
  ** maximum number of pages in the database file.  The 
  ** second form attempts to change this setting.  Both
  ** forms return the current setting.
  */
  if( sqlite3StrICmp(zLeft,"max_page_count")==0 ){
    Btree *pBt = pDb->pBt;
    int newMax = 0;
    if( zRight ){
      newMax = atoi(zRight);
    }
    if( pBt ){
      newMax = sqlite3BtreeMaxPageCount(pBt, newMax);
    }
    returnSingleInt(pParse, "max_page_count", newMax);
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
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "locking_mode", P4_STATIC);
    sqlite3VdbeAddOp4(v, OP_String8, 0, 1, 0, zRet, 0);
    sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
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
    if( sqlite3ReadSchema(pParse) ){
      goto pragma_out;
    }
    if( !zRight ){
      int auto_vacuum = 
          pBt ? sqlite3BtreeGetAutoVacuum(pBt) : SQLITE_DEFAULT_AUTOVACUUM;
      returnSingleInt(pParse, "auto_vacuum", auto_vacuum);
    }else{
      int eAuto = getAutoVacuum(zRight);
      db->nextAutovac = eAuto;
      if( eAuto>=0 ){
        /* Call SetAutoVacuum() to set initialize the internal auto and
        ** incr-vacuum flags. This is required in case this connection
        ** creates the database file. It is important that it is created
        ** as an auto-vacuum capable db.
        */
        int rc = sqlite3BtreeSetAutoVacuum(pBt, eAuto);
        if( rc==SQLITE_OK && (eAuto==1 || eAuto==2) ){
          /* When setting the auto_vacuum mode to either "full" or 
          ** "incremental", write the value of meta[6] in the database
          ** file. Before writing to meta[6], check that meta[3] indicates
          ** that this really is an auto-vacuum capable database.
          */
          static const VdbeOpList setMeta6[] = {
            { OP_Transaction,    0,               1,        0},    /* 0 */
            { OP_ReadCookie,     0,               1,        3},    /* 1 */
            { OP_If,             1,               0,        0},    /* 2 */
            { OP_Halt,           SQLITE_OK,       OE_Abort, 0},    /* 3 */
            { OP_Integer,        0,               1,        0},    /* 4 */
            { OP_SetCookie,      0,               6,        1},    /* 5 */
          };
          int iAddr;
          iAddr = sqlite3VdbeAddOpList(v, ArraySize(setMeta6), setMeta6);
          sqlite3VdbeChangeP1(v, iAddr, iDb);
          sqlite3VdbeChangeP1(v, iAddr+1, iDb);
          sqlite3VdbeChangeP2(v, iAddr+2, iAddr+4);
          sqlite3VdbeChangeP1(v, iAddr+4, eAuto-1);
          sqlite3VdbeChangeP1(v, iAddr+5, iDb);
          sqlite3VdbeUsesBtree(v, iDb);
        }
      }
    }
  }else
#endif

  /*
  **  PRAGMA [database.]incremental_vacuum(N)
  **
  ** Do N steps of incremental vacuuming on a database.
  */
#ifndef SQLITE_OMIT_AUTOVACUUM
  if( sqlite3StrICmp(zLeft,"incremental_vacuum")==0 ){
    int iLimit, addr;
    if( sqlite3ReadSchema(pParse) ){
      goto pragma_out;
    }
    if( zRight==0 || !sqlite3GetInt32(zRight, &iLimit) || iLimit<=0 ){
      iLimit = 0x7fffffff;
    }
    sqlite3BeginWriteOperation(pParse, 0, iDb);
    sqlite3VdbeAddOp2(v, OP_Integer, iLimit, 1);
    addr = sqlite3VdbeAddOp1(v, OP_IncrVacuum, iDb);
    sqlite3VdbeAddOp1(v, OP_ResultRow, 1);
    sqlite3VdbeAddOp2(v, OP_AddImm, 1, -1);
    sqlite3VdbeAddOp2(v, OP_IfPos, 1, addr);
    sqlite3VdbeJumpHere(v, addr);
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
            "temp_store_directory", P4_STATIC);
        sqlite3VdbeAddOp4(v, OP_String8, 0, 1, 0, sqlite3_temp_directory, 0);
        sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
      }
    }else{
      if( zRight[0] 
       && !sqlite3OsAccess(db->pVfs, zRight, SQLITE_ACCESS_READWRITE) 
      ){
        sqlite3ErrorMsg(pParse, "not a writable directory");
        goto pragma_out;
      }
      if( TEMP_STORE==0
       || (TEMP_STORE==1 && db->temp_store<=1)
       || (TEMP_STORE==2 && db->temp_store==1)
      ){
        invalidateTempStorage(pParse);
      }
      sqlite3_free(sqlite3_temp_directory);
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
      int nHidden = 0;
      Column *pCol;
      sqlite3VdbeSetNumCols(v, 6);
      pParse->nMem = 6;
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "cid", P4_STATIC);
      sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P4_STATIC);
      sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "type", P4_STATIC);
      sqlite3VdbeSetColName(v, 3, COLNAME_NAME, "notnull", P4_STATIC);
      sqlite3VdbeSetColName(v, 4, COLNAME_NAME, "dflt_value", P4_STATIC);
      sqlite3VdbeSetColName(v, 5, COLNAME_NAME, "pk", P4_STATIC);
      sqlite3ViewGetColumnNames(pParse, pTab);
      for(i=0, pCol=pTab->aCol; i<pTab->nCol; i++, pCol++){
        const Token *pDflt;
        if( IsHiddenColumn(pCol) ){
          nHidden++;
          continue;
        }
        sqlite3VdbeAddOp2(v, OP_Integer, i-nHidden, 1);
        sqlite3VdbeAddOp4(v, OP_String8, 0, 2, 0, pCol->zName, 0);
        sqlite3VdbeAddOp4(v, OP_String8, 0, 3, 0,
           pCol->zType ? pCol->zType : "", 0);
        sqlite3VdbeAddOp2(v, OP_Integer, pCol->notNull, 4);
        if( pCol->pDflt && (pDflt = &pCol->pDflt->span)->z ){
          sqlite3VdbeAddOp4(v, OP_String8, 0, 5, 0, (char*)pDflt->z, pDflt->n);
        }else{
          sqlite3VdbeAddOp2(v, OP_Null, 0, 5);
        }
        sqlite3VdbeAddOp2(v, OP_Integer, pCol->isPrimKey, 6);
        sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 6);
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
      pParse->nMem = 3;
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seqno", P4_STATIC);
      sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "cid", P4_STATIC);
      sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "name", P4_STATIC);
      for(i=0; i<pIdx->nColumn; i++){
        int cnum = pIdx->aiColumn[i];
        sqlite3VdbeAddOp2(v, OP_Integer, i, 1);
        sqlite3VdbeAddOp2(v, OP_Integer, cnum, 2);
        assert( pTab->nCol>cnum );
        sqlite3VdbeAddOp4(v, OP_String8, 0, 3, 0, pTab->aCol[cnum].zName, 0);
        sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 3);
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
        pParse->nMem = 3;
        sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P4_STATIC);
        sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P4_STATIC);
        sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "unique", P4_STATIC);
        while(pIdx){
          sqlite3VdbeAddOp2(v, OP_Integer, i, 1);
          sqlite3VdbeAddOp4(v, OP_String8, 0, 2, 0, pIdx->zName, 0);
          sqlite3VdbeAddOp2(v, OP_Integer, pIdx->onError!=OE_None, 3);
          sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 3);
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
    pParse->nMem = 3;
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P4_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P4_STATIC);
    sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "file", P4_STATIC);
    for(i=0; i<db->nDb; i++){
      if( db->aDb[i].pBt==0 ) continue;
      assert( db->aDb[i].zName!=0 );
      sqlite3VdbeAddOp2(v, OP_Integer, i, 1);
      sqlite3VdbeAddOp4(v, OP_String8, 0, 2, 0, db->aDb[i].zName, 0);
      sqlite3VdbeAddOp4(v, OP_String8, 0, 3, 0,
           sqlite3BtreeGetFilename(db->aDb[i].pBt), 0);
      sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 3);
    }
  }else

  if( sqlite3StrICmp(zLeft, "collation_list")==0 ){
    int i = 0;
    HashElem *p;
    sqlite3VdbeSetNumCols(v, 2);
    pParse->nMem = 2;
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "seq", P4_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "name", P4_STATIC);
    for(p=sqliteHashFirst(&db->aCollSeq); p; p=sqliteHashNext(p)){
      CollSeq *pColl = (CollSeq *)sqliteHashData(p);
      sqlite3VdbeAddOp2(v, OP_Integer, i++, 1);
      sqlite3VdbeAddOp4(v, OP_String8, 0, 2, 0, pColl->zName, 0);
      sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 2);
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
        pParse->nMem = 5;
        sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "id", P4_STATIC);
        sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "seq", P4_STATIC);
        sqlite3VdbeSetColName(v, 2, COLNAME_NAME, "table", P4_STATIC);
        sqlite3VdbeSetColName(v, 3, COLNAME_NAME, "from", P4_STATIC);
        sqlite3VdbeSetColName(v, 4, COLNAME_NAME, "to", P4_STATIC);
        while(pFK){
          int j;
          for(j=0; j<pFK->nCol; j++){
            char *zCol = pFK->aCol[j].zCol;
            sqlite3VdbeAddOp2(v, OP_Integer, i, 1);
            sqlite3VdbeAddOp2(v, OP_Integer, j, 2);
            sqlite3VdbeAddOp4(v, OP_String8, 0, 3, 0, pFK->zTo, 0);
            sqlite3VdbeAddOp4(v, OP_String8, 0, 4, 0,
                              pTab->aCol[pFK->aCol[j].iFrom].zName, 0);
            sqlite3VdbeAddOp4(v, zCol ? OP_String8 : OP_Null, 0, 5, 0, zCol, 0);
            sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 5);
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
  /* Pragma "quick_check" is an experimental reduced version of 
  ** integrity_check designed to detect most database corruption
  ** without most of the overhead of a full integrity-check.
  */
  if( sqlite3StrICmp(zLeft, "integrity_check")==0
   || sqlite3StrICmp(zLeft, "quick_check")==0 
  ){
    int i, j, addr, mxErr;

    /* Code that appears at the end of the integrity check.  If no error
    ** messages have been generated, output OK.  Otherwise output the
    ** error message
    */
    static const VdbeOpList endCode[] = {
      { OP_AddImm,      1, 0,        0},    /* 0 */
      { OP_IfNeg,       1, 0,        0},    /* 1 */
      { OP_String8,     0, 3,        0},    /* 2 */
      { OP_ResultRow,   3, 1,        0},
    };

    int isQuick = (zLeft[0]=='q');

    /* Initialize the VDBE program */
    if( sqlite3ReadSchema(pParse) ) goto pragma_out;
    pParse->nMem = 6;
    sqlite3VdbeSetNumCols(v, 1);
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "integrity_check", P4_STATIC);

    /* Set the maximum error count */
    mxErr = SQLITE_INTEGRITY_CHECK_ERROR_MAX;
    if( zRight ){
      mxErr = atoi(zRight);
      if( mxErr<=0 ){
        mxErr = SQLITE_INTEGRITY_CHECK_ERROR_MAX;
      }
    }
    sqlite3VdbeAddOp2(v, OP_Integer, mxErr, 1);  /* reg[1] holds errors left */

    /* Do an integrity check on each database file */
    for(i=0; i<db->nDb; i++){
      HashElem *x;
      Hash *pTbls;
      int cnt = 0;

      if( OMIT_TEMPDB && i==1 ) continue;

      sqlite3CodeVerifySchema(pParse, i);
      addr = sqlite3VdbeAddOp1(v, OP_IfPos, 1); /* Halt if out of errors */
      sqlite3VdbeAddOp2(v, OP_Halt, 0, 0);
      sqlite3VdbeJumpHere(v, addr);

      /* Do an integrity check of the B-Tree
      **
      ** Begin by filling registers 2, 3, ... with the root pages numbers
      ** for all tables and indices in the database.
      */
      pTbls = &db->aDb[i].pSchema->tblHash;
      for(x=sqliteHashFirst(pTbls); x; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        sqlite3VdbeAddOp2(v, OP_Integer, pTab->tnum, 2+cnt);
        cnt++;
        for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
          sqlite3VdbeAddOp2(v, OP_Integer, pIdx->tnum, 2+cnt);
          cnt++;
        }
      }
      if( cnt==0 ) continue;

      /* Make sure sufficient number of registers have been allocated */
      if( pParse->nMem < cnt+3 ){
        pParse->nMem = cnt+3;
      }

      /* Do the b-tree integrity checks */
      sqlite3VdbeAddOp3(v, OP_IntegrityCk, 2, cnt, 1);
      sqlite3VdbeChangeP5(v, i);
      addr = sqlite3VdbeAddOp1(v, OP_IsNull, 2);
      sqlite3VdbeAddOp4(v, OP_String8, 0, 3, 0,
         sqlite3MPrintf(db, "*** in database %s ***\n", db->aDb[i].zName),
         P4_DYNAMIC);
      sqlite3VdbeAddOp3(v, OP_Concat, 2, 3, 2);
      sqlite3VdbeAddOp2(v, OP_ResultRow, 2, 1);
      sqlite3VdbeJumpHere(v, addr);

      /* Make sure all the indices are constructed correctly.
      */
      for(x=sqliteHashFirst(pTbls); x && !isQuick; x=sqliteHashNext(x)){
        Table *pTab = sqliteHashData(x);
        Index *pIdx;
        int loopTop;

        if( pTab->pIndex==0 ) continue;
        addr = sqlite3VdbeAddOp1(v, OP_IfPos, 1);  /* Stop if out of errors */
        sqlite3VdbeAddOp2(v, OP_Halt, 0, 0);
        sqlite3VdbeJumpHere(v, addr);
        sqlite3OpenTableAndIndices(pParse, pTab, 1, OP_OpenRead);
        sqlite3VdbeAddOp2(v, OP_Integer, 0, 2);  /* reg(2) will count entries */
        loopTop = sqlite3VdbeAddOp2(v, OP_Rewind, 1, 0);
        sqlite3VdbeAddOp2(v, OP_AddImm, 2, 1);   /* increment entry count */
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          int jmp2;
          static const VdbeOpList idxErr[] = {
            { OP_AddImm,      1, -1,  0},
            { OP_String8,     0,  3,  0},    /* 1 */
            { OP_Rowid,       1,  4,  0},
            { OP_String8,     0,  5,  0},    /* 3 */
            { OP_String8,     0,  6,  0},    /* 4 */
            { OP_Concat,      4,  3,  3},
            { OP_Concat,      5,  3,  3},
            { OP_Concat,      6,  3,  3},
            { OP_ResultRow,   3,  1,  0},
          };
          sqlite3GenerateIndexKey(pParse, pIdx, 1, 3);
          jmp2 = sqlite3VdbeAddOp3(v, OP_Found, j+2, 0, 3);
          addr = sqlite3VdbeAddOpList(v, ArraySize(idxErr), idxErr);
          sqlite3VdbeChangeP4(v, addr+1, "rowid ", P4_STATIC);
          sqlite3VdbeChangeP4(v, addr+3, " missing from index ", P4_STATIC);
          sqlite3VdbeChangeP4(v, addr+4, pIdx->zName, P4_STATIC);
          sqlite3VdbeJumpHere(v, jmp2);
        }
        sqlite3VdbeAddOp2(v, OP_Next, 1, loopTop+1);
        sqlite3VdbeJumpHere(v, loopTop);
        for(j=0, pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext, j++){
          static const VdbeOpList cntIdx[] = {
             { OP_Integer,      0,  3,  0},
             { OP_Rewind,       0,  0,  0},  /* 1 */
             { OP_AddImm,       3,  1,  0},
             { OP_Next,         0,  0,  0},  /* 3 */
             { OP_Eq,           2,  0,  3},  /* 4 */
             { OP_AddImm,       1, -1,  0},
             { OP_String8,      0,  2,  0},  /* 6 */
             { OP_String8,      0,  3,  0},  /* 7 */
             { OP_Concat,       3,  2,  2},
             { OP_ResultRow,    2,  1,  0},
          };
          if( pIdx->tnum==0 ) continue;
          addr = sqlite3VdbeAddOp1(v, OP_IfPos, 1);
          sqlite3VdbeAddOp2(v, OP_Halt, 0, 0);
          sqlite3VdbeJumpHere(v, addr);
          addr = sqlite3VdbeAddOpList(v, ArraySize(cntIdx), cntIdx);
          sqlite3VdbeChangeP1(v, addr+1, j+2);
          sqlite3VdbeChangeP2(v, addr+1, addr+4);
          sqlite3VdbeChangeP1(v, addr+3, j+2);
          sqlite3VdbeChangeP2(v, addr+3, addr+2);
          sqlite3VdbeJumpHere(v, addr+4);
          sqlite3VdbeChangeP4(v, addr+6, 
                     "wrong # of entries in index ", P4_STATIC);
          sqlite3VdbeChangeP4(v, addr+7, pIdx->zName, P4_STATIC);
        }
      } 
    }
    addr = sqlite3VdbeAddOpList(v, ArraySize(endCode), endCode);
    sqlite3VdbeChangeP2(v, addr, -mxErr);
    sqlite3VdbeJumpHere(v, addr+1);
    sqlite3VdbeChangeP4(v, addr+2, "ok", P4_STATIC);
  }else
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

#ifndef SQLITE_OMIT_UTF16
  /*
  **   PRAGMA encoding
  **   PRAGMA encoding = "utf-8"|"utf-16"|"utf-16le"|"utf-16be"
  **
  ** In its first form, this pragma returns the encoding of the main
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
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "encoding", P4_STATIC);
      sqlite3VdbeAddOp2(v, OP_String8, 0, 1);
      for(pEnc=&encnames[0]; pEnc->zName; pEnc++){
        if( pEnc->enc==ENC(pParse->db) ){
          sqlite3VdbeChangeP4(v, -1, pEnc->zName, P4_STATIC);
          break;
        }
      }
      sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 1);
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
  if( sqlite3StrICmp(zLeft, "schema_version")==0 
   || sqlite3StrICmp(zLeft, "user_version")==0 
   || sqlite3StrICmp(zLeft, "freelist_count")==0 
  ){

    int iCookie;   /* Cookie index. 0 for schema-cookie, 6 for user-cookie. */
    sqlite3VdbeUsesBtree(v, iDb);
    switch( zLeft[0] ){
      case 's': case 'S':
        iCookie = 0;
        break;
      case 'f': case 'F':
        iCookie = 1;
        iDb = (-1*(iDb+1));
        assert(iDb<=0);
        break;
      default:
        iCookie = 5;
        break;
    }

    if( zRight && iDb>=0 ){
      /* Write the specified cookie value */
      static const VdbeOpList setCookie[] = {
        { OP_Transaction,    0,  1,  0},    /* 0 */
        { OP_Integer,        0,  1,  0},    /* 1 */
        { OP_SetCookie,      0,  0,  1},    /* 2 */
      };
      int addr = sqlite3VdbeAddOpList(v, ArraySize(setCookie), setCookie);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP1(v, addr+1, atoi(zRight));
      sqlite3VdbeChangeP1(v, addr+2, iDb);
      sqlite3VdbeChangeP2(v, addr+2, iCookie);
    }else{
      /* Read the specified cookie value */
      static const VdbeOpList readCookie[] = {
        { OP_ReadCookie,      0,  1,  0},    /* 0 */
        { OP_ResultRow,       1,  1,  0}
      };
      int addr = sqlite3VdbeAddOpList(v, ArraySize(readCookie), readCookie);
      sqlite3VdbeChangeP1(v, addr, iDb);
      sqlite3VdbeChangeP3(v, addr, iCookie);
      sqlite3VdbeSetNumCols(v, 1);
      sqlite3VdbeSetColName(v, 0, COLNAME_NAME, zLeft, P4_TRANSIENT);
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
    pParse->nMem = 2;
    sqlite3VdbeSetColName(v, 0, COLNAME_NAME, "database", P4_STATIC);
    sqlite3VdbeSetColName(v, 1, COLNAME_NAME, "status", P4_STATIC);
    for(i=0; i<db->nDb; i++){
      Btree *pBt;
      Pager *pPager;
      const char *zState = "unknown";
      int j;
      if( db->aDb[i].zName==0 ) continue;
      sqlite3VdbeAddOp4(v, OP_String8, 0, 1, 0, db->aDb[i].zName, P4_STATIC);
      pBt = db->aDb[i].pBt;
      if( pBt==0 || (pPager = sqlite3BtreePager(pBt))==0 ){
        zState = "closed";
      }else if( sqlite3_file_control(db, i ? db->aDb[i].zName : 0, 
                                     SQLITE_FCNTL_LOCKSTATE, &j)==SQLITE_OK ){
         zState = azLockName[j];
      }
      sqlite3VdbeAddOp4(v, OP_String8, 0, 2, 0, zState, P4_STATIC);
      sqlite3VdbeAddOp2(v, OP_ResultRow, 1, 2);
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
    sqlite3VdbeAddOp2(v, OP_Expire, 1, 0);

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
  sqlite3_free(zLeft);
  sqlite3_free(zRight);
}

#endif /* SQLITE_OMIT_PRAGMA || SQLITE_OMIT_PARSER */
