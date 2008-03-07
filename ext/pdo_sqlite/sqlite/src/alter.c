/*
** 2005 February 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains C code routines that used to generate VDBE code
** that implements the ALTER TABLE command.
**
** $Id$
*/
#include "sqliteInt.h"
#include <ctype.h>

/*
** The code in this file only exists if we are not omitting the
** ALTER TABLE logic from the build.
*/
#ifndef SQLITE_OMIT_ALTERTABLE


/*
** This function is used by SQL generated to implement the 
** ALTER TABLE command. The first argument is the text of a CREATE TABLE or
** CREATE INDEX command. The second is a table name. The table name in 
** the CREATE TABLE or CREATE INDEX statement is replaced with the third
** argument and the result returned. Examples:
**
** sqlite_rename_table('CREATE TABLE abc(a, b, c)', 'def')
**     -> 'CREATE TABLE def(a, b, c)'
**
** sqlite_rename_table('CREATE INDEX i ON abc(a)', 'def')
**     -> 'CREATE INDEX i ON def(a, b, c)'
*/
static void renameTableFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  unsigned char const *zSql = sqlite3_value_text(argv[0]);
  unsigned char const *zTableName = sqlite3_value_text(argv[1]);

  int token;
  Token tname;
  unsigned char const *zCsr = zSql;
  int len = 0;
  char *zRet;

  sqlite3 *db = sqlite3_user_data(context);

  /* The principle used to locate the table name in the CREATE TABLE 
  ** statement is that the table name is the first token that is immediatedly
  ** followed by a left parenthesis - TK_LP - or "USING" TK_USING.
  */
  if( zSql ){
    do {
      if( !*zCsr ){
        /* Ran out of input before finding an opening bracket. Return NULL. */
        return;
      }

      /* Store the token that zCsr points to in tname. */
      tname.z = zCsr;
      tname.n = len;

      /* Advance zCsr to the next token. Store that token type in 'token',
      ** and its length in 'len' (to be used next iteration of this loop).
      */
      do {
        zCsr += len;
        len = sqlite3GetToken(zCsr, &token);
      } while( token==TK_SPACE );
      assert( len>0 );
    } while( token!=TK_LP && token!=TK_USING );

    zRet = sqlite3MPrintf(db, "%.*s%Q%s", tname.z - zSql, zSql, 
       zTableName, tname.z+tname.n);
    sqlite3_result_text(context, zRet, -1, sqlite3_free);
  }
}

#ifndef SQLITE_OMIT_TRIGGER
/* This function is used by SQL generated to implement the
** ALTER TABLE command. The first argument is the text of a CREATE TRIGGER 
** statement. The second is a table name. The table name in the CREATE 
** TRIGGER statement is replaced with the third argument and the result 
** returned. This is analagous to renameTableFunc() above, except for CREATE
** TRIGGER, not CREATE INDEX and CREATE TABLE.
*/
static void renameTriggerFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
){
  unsigned char const *zSql = sqlite3_value_text(argv[0]);
  unsigned char const *zTableName = sqlite3_value_text(argv[1]);

  int token;
  Token tname;
  int dist = 3;
  unsigned char const *zCsr = zSql;
  int len = 0;
  char *zRet;

  sqlite3 *db = sqlite3_user_data(context);

  /* The principle used to locate the table name in the CREATE TRIGGER 
  ** statement is that the table name is the first token that is immediatedly
  ** preceded by either TK_ON or TK_DOT and immediatedly followed by one
  ** of TK_WHEN, TK_BEGIN or TK_FOR.
  */
  if( zSql ){
    do {

      if( !*zCsr ){
        /* Ran out of input before finding the table name. Return NULL. */
        return;
      }

      /* Store the token that zCsr points to in tname. */
      tname.z = zCsr;
      tname.n = len;

      /* Advance zCsr to the next token. Store that token type in 'token',
      ** and its length in 'len' (to be used next iteration of this loop).
      */
      do {
        zCsr += len;
        len = sqlite3GetToken(zCsr, &token);
      }while( token==TK_SPACE );
      assert( len>0 );

      /* Variable 'dist' stores the number of tokens read since the most
      ** recent TK_DOT or TK_ON. This means that when a WHEN, FOR or BEGIN 
      ** token is read and 'dist' equals 2, the condition stated above
      ** to be met.
      **
      ** Note that ON cannot be a database, table or column name, so
      ** there is no need to worry about syntax like 
      ** "CREATE TRIGGER ... ON ON.ON BEGIN ..." etc.
      */
      dist++;
      if( token==TK_DOT || token==TK_ON ){
        dist = 0;
      }
    } while( dist!=2 || (token!=TK_WHEN && token!=TK_FOR && token!=TK_BEGIN) );

    /* Variable tname now contains the token that is the old table-name
    ** in the CREATE TRIGGER statement.
    */
    zRet = sqlite3MPrintf(db, "%.*s%Q%s", tname.z - zSql, zSql, 
       zTableName, tname.z+tname.n);
    sqlite3_result_text(context, zRet, -1, sqlite3_free);
  }
}
#endif   /* !SQLITE_OMIT_TRIGGER */

/*
** Register built-in functions used to help implement ALTER TABLE
*/
void sqlite3AlterFunctions(sqlite3 *db){
  static const struct {
     char *zName;
     signed char nArg;
     void (*xFunc)(sqlite3_context*,int,sqlite3_value **);
  } aFuncs[] = {
    { "sqlite_rename_table",    2, renameTableFunc},
#ifndef SQLITE_OMIT_TRIGGER
    { "sqlite_rename_trigger",  2, renameTriggerFunc},
#endif
  };
  int i;

  for(i=0; i<sizeof(aFuncs)/sizeof(aFuncs[0]); i++){
    sqlite3CreateFunc(db, aFuncs[i].zName, aFuncs[i].nArg,
        SQLITE_UTF8, (void *)db, aFuncs[i].xFunc, 0, 0);
  }
}

/*
** Generate the text of a WHERE expression which can be used to select all
** temporary triggers on table pTab from the sqlite_temp_master table. If
** table pTab has no temporary triggers, or is itself stored in the 
** temporary database, NULL is returned.
*/
static char *whereTempTriggers(Parse *pParse, Table *pTab){
  Trigger *pTrig;
  char *zWhere = 0;
  char *tmp = 0;
  const Schema *pTempSchema = pParse->db->aDb[1].pSchema; /* Temp db schema */

  /* If the table is not located in the temp-db (in which case NULL is 
  ** returned, loop through the tables list of triggers. For each trigger
  ** that is not part of the temp-db schema, add a clause to the WHERE 
  ** expression being built up in zWhere.
  */
  if( pTab->pSchema!=pTempSchema ){
    sqlite3 *db = pParse->db;
    for( pTrig=pTab->pTrigger; pTrig; pTrig=pTrig->pNext ){
      if( pTrig->pSchema==pTempSchema ){
        if( !zWhere ){
          zWhere = sqlite3MPrintf(db, "name=%Q", pTrig->name);
        }else{
          tmp = zWhere;
          zWhere = sqlite3MPrintf(db, "%s OR name=%Q", zWhere, pTrig->name);
          sqlite3_free(tmp);
        }
      }
    }
  }
  return zWhere;
}

/*
** Generate code to drop and reload the internal representation of table
** pTab from the database, including triggers and temporary triggers.
** Argument zName is the name of the table in the database schema at
** the time the generated code is executed. This can be different from
** pTab->zName if this function is being called to code part of an 
** "ALTER TABLE RENAME TO" statement.
*/
static void reloadTableSchema(Parse *pParse, Table *pTab, const char *zName){
  Vdbe *v;
  char *zWhere;
  int iDb;                   /* Index of database containing pTab */
#ifndef SQLITE_OMIT_TRIGGER
  Trigger *pTrig;
#endif

  v = sqlite3GetVdbe(pParse);
  if( !v ) return;
  assert( sqlite3BtreeHoldsAllMutexes(pParse->db) );
  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
  assert( iDb>=0 );

#ifndef SQLITE_OMIT_TRIGGER
  /* Drop any table triggers from the internal schema. */
  for(pTrig=pTab->pTrigger; pTrig; pTrig=pTrig->pNext){
    int iTrigDb = sqlite3SchemaToIndex(pParse->db, pTrig->pSchema);
    assert( iTrigDb==iDb || iTrigDb==1 );
    sqlite3VdbeAddOp4(v, OP_DropTrigger, iTrigDb, 0, 0, pTrig->name, 0);
  }
#endif

  /* Drop the table and index from the internal schema */
  sqlite3VdbeAddOp4(v, OP_DropTable, iDb, 0, 0, pTab->zName, 0);

  /* Reload the table, index and permanent trigger schemas. */
  zWhere = sqlite3MPrintf(pParse->db, "tbl_name=%Q", zName);
  if( !zWhere ) return;
  sqlite3VdbeAddOp4(v, OP_ParseSchema, iDb, 0, 0, zWhere, P4_DYNAMIC);

#ifndef SQLITE_OMIT_TRIGGER
  /* Now, if the table is not stored in the temp database, reload any temp 
  ** triggers. Don't use IN(...) in case SQLITE_OMIT_SUBQUERY is defined. 
  */
  if( (zWhere=whereTempTriggers(pParse, pTab))!=0 ){
    sqlite3VdbeAddOp4(v, OP_ParseSchema, 1, 0, 0, zWhere, P4_DYNAMIC);
  }
#endif
}

/*
** Generate code to implement the "ALTER TABLE xxx RENAME TO yyy" 
** command. 
*/
void sqlite3AlterRenameTable(
  Parse *pParse,            /* Parser context. */
  SrcList *pSrc,            /* The table to rename. */
  Token *pName              /* The new table name. */
){
  int iDb;                  /* Database that contains the table */
  char *zDb;                /* Name of database iDb */
  Table *pTab;              /* Table being renamed */
  char *zName = 0;          /* NULL-terminated version of pName */ 
  sqlite3 *db = pParse->db; /* Database connection */
  int nTabName;             /* Number of UTF-8 characters in zTabName */
  const char *zTabName;     /* Original name of the table */
  Vdbe *v;
#ifndef SQLITE_OMIT_TRIGGER
  char *zWhere = 0;         /* Where clause to locate temp triggers */
#endif
  int isVirtualRename = 0;  /* True if this is a v-table with an xRename() */
  
  if( db->mallocFailed ) goto exit_rename_table;
  assert( pSrc->nSrc==1 );
  assert( sqlite3BtreeHoldsAllMutexes(pParse->db) );

  pTab = sqlite3LocateTable(pParse, 0, pSrc->a[0].zName, pSrc->a[0].zDatabase);
  if( !pTab ) goto exit_rename_table;
  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
  zDb = db->aDb[iDb].zName;

  /* Get a NULL terminated version of the new table name. */
  zName = sqlite3NameFromToken(db, pName);
  if( !zName ) goto exit_rename_table;

  /* Check that a table or index named 'zName' does not already exist
  ** in database iDb. If so, this is an error.
  */
  if( sqlite3FindTable(db, zName, zDb) || sqlite3FindIndex(db, zName, zDb) ){
    sqlite3ErrorMsg(pParse, 
        "there is already another table or index with this name: %s", zName);
    goto exit_rename_table;
  }

  /* Make sure it is not a system table being altered, or a reserved name
  ** that the table is being renamed to.
  */
  if( strlen(pTab->zName)>6 && 0==sqlite3StrNICmp(pTab->zName, "sqlite_", 7) ){
    sqlite3ErrorMsg(pParse, "table %s may not be altered", pTab->zName);
    goto exit_rename_table;
  }
  if( SQLITE_OK!=sqlite3CheckObjectName(pParse, zName) ){
    goto exit_rename_table;
  }

#ifndef SQLITE_OMIT_VIEW
  if( pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "view %s may not be altered", pTab->zName);
    goto exit_rename_table;
  }
#endif

#ifndef SQLITE_OMIT_AUTHORIZATION
  /* Invoke the authorization callback. */
  if( sqlite3AuthCheck(pParse, SQLITE_ALTER_TABLE, zDb, pTab->zName, 0) ){
    goto exit_rename_table;
  }
#endif

#ifndef SQLITE_OMIT_VIRTUALTABLE
  if( sqlite3ViewGetColumnNames(pParse, pTab) ){
    goto exit_rename_table;
  }
  if( IsVirtual(pTab) && pTab->pMod->pModule->xRename ){
    isVirtualRename = 1;
  }
#endif

  /* Begin a transaction and code the VerifyCookie for database iDb. 
  ** Then modify the schema cookie (since the ALTER TABLE modifies the
  ** schema). Open a statement transaction if the table is a virtual
  ** table.
  */
  v = sqlite3GetVdbe(pParse);
  if( v==0 ){
    goto exit_rename_table;
  }
  sqlite3BeginWriteOperation(pParse, isVirtualRename, iDb);
  sqlite3ChangeCookie(pParse, iDb);

  /* If this is a virtual table, invoke the xRename() function if
  ** one is defined. The xRename() callback will modify the names
  ** of any resources used by the v-table implementation (including other
  ** SQLite tables) that are identified by the name of the virtual table.
  */
#ifndef SQLITE_OMIT_VIRTUALTABLE
  if( isVirtualRename ){
    int i = ++pParse->nMem;
    sqlite3VdbeAddOp4(v, OP_String8, 0, i, 0, zName, 0);
    sqlite3VdbeAddOp4(v, OP_VRename, i, 0, 0,(const char*)pTab->pVtab, P4_VTAB);
  }
#endif

  /* figure out how many UTF-8 characters are in zName */
  zTabName = pTab->zName;
  nTabName = sqlite3Utf8CharLen(zTabName, -1);

  /* Modify the sqlite_master table to use the new table name. */
  sqlite3NestedParse(pParse,
      "UPDATE %Q.%s SET "
#ifdef SQLITE_OMIT_TRIGGER
          "sql = sqlite_rename_table(sql, %Q), "
#else
          "sql = CASE "
            "WHEN type = 'trigger' THEN sqlite_rename_trigger(sql, %Q)"
            "ELSE sqlite_rename_table(sql, %Q) END, "
#endif
          "tbl_name = %Q, "
          "name = CASE "
            "WHEN type='table' THEN %Q "
            "WHEN name LIKE 'sqlite_autoindex%%' AND type='index' THEN "
             "'sqlite_autoindex_' || %Q || substr(name,%d+18) "
            "ELSE name END "
      "WHERE tbl_name=%Q AND "
          "(type='table' OR type='index' OR type='trigger');", 
      zDb, SCHEMA_TABLE(iDb), zName, zName, zName, 
#ifndef SQLITE_OMIT_TRIGGER
      zName,
#endif
      zName, nTabName, zTabName
  );

#ifndef SQLITE_OMIT_AUTOINCREMENT
  /* If the sqlite_sequence table exists in this database, then update 
  ** it with the new table name.
  */
  if( sqlite3FindTable(db, "sqlite_sequence", zDb) ){
    sqlite3NestedParse(pParse,
        "UPDATE %Q.sqlite_sequence set name = %Q WHERE name = %Q",
        zDb, zName, pTab->zName);
  }
#endif

#ifndef SQLITE_OMIT_TRIGGER
  /* If there are TEMP triggers on this table, modify the sqlite_temp_master
  ** table. Don't do this if the table being ALTERed is itself located in
  ** the temp database.
  */
  if( (zWhere=whereTempTriggers(pParse, pTab))!=0 ){
    sqlite3NestedParse(pParse, 
        "UPDATE sqlite_temp_master SET "
            "sql = sqlite_rename_trigger(sql, %Q), "
            "tbl_name = %Q "
            "WHERE %s;", zName, zName, zWhere);
    sqlite3_free(zWhere);
  }
#endif

  /* Drop and reload the internal table schema. */
  reloadTableSchema(pParse, pTab, zName);

exit_rename_table:
  sqlite3SrcListDelete(pSrc);
  sqlite3_free(zName);
}


/*
** This function is called after an "ALTER TABLE ... ADD" statement
** has been parsed. Argument pColDef contains the text of the new
** column definition.
**
** The Table structure pParse->pNewTable was extended to include
** the new column during parsing.
*/
void sqlite3AlterFinishAddColumn(Parse *pParse, Token *pColDef){
  Table *pNew;              /* Copy of pParse->pNewTable */
  Table *pTab;              /* Table being altered */
  int iDb;                  /* Database number */
  const char *zDb;          /* Database name */
  const char *zTab;         /* Table name */
  char *zCol;               /* Null-terminated column definition */
  Column *pCol;             /* The new column */
  Expr *pDflt;              /* Default value for the new column */
  sqlite3 *db;              /* The database connection; */

  if( pParse->nErr ) return;
  pNew = pParse->pNewTable;
  assert( pNew );

  db = pParse->db;
  assert( sqlite3BtreeHoldsAllMutexes(db) );
  iDb = sqlite3SchemaToIndex(db, pNew->pSchema);
  zDb = db->aDb[iDb].zName;
  zTab = pNew->zName;
  pCol = &pNew->aCol[pNew->nCol-1];
  pDflt = pCol->pDflt;
  pTab = sqlite3FindTable(db, zTab, zDb);
  assert( pTab );

#ifndef SQLITE_OMIT_AUTHORIZATION
  /* Invoke the authorization callback. */
  if( sqlite3AuthCheck(pParse, SQLITE_ALTER_TABLE, zDb, pTab->zName, 0) ){
    return;
  }
#endif

  /* If the default value for the new column was specified with a 
  ** literal NULL, then set pDflt to 0. This simplifies checking
  ** for an SQL NULL default below.
  */
  if( pDflt && pDflt->op==TK_NULL ){
    pDflt = 0;
  }

  /* Check that the new column is not specified as PRIMARY KEY or UNIQUE.
  ** If there is a NOT NULL constraint, then the default value for the
  ** column must not be NULL.
  */
  if( pCol->isPrimKey ){
    sqlite3ErrorMsg(pParse, "Cannot add a PRIMARY KEY column");
    return;
  }
  if( pNew->pIndex ){
    sqlite3ErrorMsg(pParse, "Cannot add a UNIQUE column");
    return;
  }
  if( pCol->notNull && !pDflt ){
    sqlite3ErrorMsg(pParse, 
        "Cannot add a NOT NULL column with default value NULL");
    return;
  }

  /* Ensure the default expression is something that sqlite3ValueFromExpr()
  ** can handle (i.e. not CURRENT_TIME etc.)
  */
  if( pDflt ){
    sqlite3_value *pVal;
    if( sqlite3ValueFromExpr(db, pDflt, SQLITE_UTF8, SQLITE_AFF_NONE, &pVal) ){
      db->mallocFailed = 1;
      return;
    }
    if( !pVal ){
      sqlite3ErrorMsg(pParse, "Cannot add a column with non-constant default");
      return;
    }
    sqlite3ValueFree(pVal);
  }

  /* Modify the CREATE TABLE statement. */
  zCol = sqlite3DbStrNDup(db, (char*)pColDef->z, pColDef->n);
  if( zCol ){
    char *zEnd = &zCol[pColDef->n-1];
    while( (zEnd>zCol && *zEnd==';') || isspace(*(unsigned char *)zEnd) ){
      *zEnd-- = '\0';
    }
    sqlite3NestedParse(pParse, 
        "UPDATE %Q.%s SET "
          "sql = substr(sql,1,%d) || ', ' || %Q || substr(sql,%d) "
        "WHERE type = 'table' AND name = %Q", 
      zDb, SCHEMA_TABLE(iDb), pNew->addColOffset, zCol, pNew->addColOffset+1,
      zTab
    );
    sqlite3_free(zCol);
  }

  /* If the default value of the new column is NULL, then set the file
  ** format to 2. If the default value of the new column is not NULL,
  ** the file format becomes 3.
  */
  sqlite3MinimumFileFormat(pParse, iDb, pDflt ? 3 : 2);

  /* Reload the schema of the modified table. */
  reloadTableSchema(pParse, pTab, pTab->zName);
}

/*
** This function is called by the parser after the table-name in
** an "ALTER TABLE <table-name> ADD" statement is parsed. Argument 
** pSrc is the full-name of the table being altered.
**
** This routine makes a (partial) copy of the Table structure
** for the table being altered and sets Parse.pNewTable to point
** to it. Routines called by the parser as the column definition
** is parsed (i.e. sqlite3AddColumn()) add the new Column data to 
** the copy. The copy of the Table structure is deleted by tokenize.c 
** after parsing is finished.
**
** Routine sqlite3AlterFinishAddColumn() will be called to complete
** coding the "ALTER TABLE ... ADD" statement.
*/
void sqlite3AlterBeginAddColumn(Parse *pParse, SrcList *pSrc){
  Table *pNew;
  Table *pTab;
  Vdbe *v;
  int iDb;
  int i;
  int nAlloc;
  sqlite3 *db = pParse->db;

  /* Look up the table being altered. */
  assert( pParse->pNewTable==0 );
  assert( sqlite3BtreeHoldsAllMutexes(db) );
  if( db->mallocFailed ) goto exit_begin_add_column;
  pTab = sqlite3LocateTable(pParse, 0, pSrc->a[0].zName, pSrc->a[0].zDatabase);
  if( !pTab ) goto exit_begin_add_column;

#ifndef SQLITE_OMIT_VIRTUALTABLE
  if( IsVirtual(pTab) ){
    sqlite3ErrorMsg(pParse, "virtual tables may not be altered");
    goto exit_begin_add_column;
  }
#endif

  /* Make sure this is not an attempt to ALTER a view. */
  if( pTab->pSelect ){
    sqlite3ErrorMsg(pParse, "Cannot add a column to a view");
    goto exit_begin_add_column;
  }

  assert( pTab->addColOffset>0 );
  iDb = sqlite3SchemaToIndex(db, pTab->pSchema);

  /* Put a copy of the Table struct in Parse.pNewTable for the
  ** sqlite3AddColumn() function and friends to modify.
  */
  pNew = (Table*)sqlite3DbMallocZero(db, sizeof(Table));
  if( !pNew ) goto exit_begin_add_column;
  pParse->pNewTable = pNew;
  pNew->nRef = 1;
  pNew->nCol = pTab->nCol;
  assert( pNew->nCol>0 );
  nAlloc = (((pNew->nCol-1)/8)*8)+8;
  assert( nAlloc>=pNew->nCol && nAlloc%8==0 && nAlloc-pNew->nCol<8 );
  pNew->aCol = (Column*)sqlite3DbMallocZero(db, sizeof(Column)*nAlloc);
  pNew->zName = sqlite3DbStrDup(db, pTab->zName);
  if( !pNew->aCol || !pNew->zName ){
    db->mallocFailed = 1;
    goto exit_begin_add_column;
  }
  memcpy(pNew->aCol, pTab->aCol, sizeof(Column)*pNew->nCol);
  for(i=0; i<pNew->nCol; i++){
    Column *pCol = &pNew->aCol[i];
    pCol->zName = sqlite3DbStrDup(db, pCol->zName);
    pCol->zColl = 0;
    pCol->zType = 0;
    pCol->pDflt = 0;
  }
  pNew->pSchema = db->aDb[iDb].pSchema;
  pNew->addColOffset = pTab->addColOffset;
  pNew->nRef = 1;

  /* Begin a transaction and increment the schema cookie.  */
  sqlite3BeginWriteOperation(pParse, 0, iDb);
  v = sqlite3GetVdbe(pParse);
  if( !v ) goto exit_begin_add_column;
  sqlite3ChangeCookie(pParse, iDb);

exit_begin_add_column:
  sqlite3SrcListDelete(pSrc);
  return;
}
#endif  /* SQLITE_ALTER_TABLE */
