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
** This file contains code used to implement the COPY command.
**
** $Id$
*/
#include "sqliteInt.h"

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
  SrcList *pTableName, /* The name of the table into which we will insert */
  Token *pFilename,    /* The file from which to obtain information */
  Token *pDelimiter,   /* Use this as the field delimiter */
  int onError          /* What to do if a constraint fails */
){
  Table *pTab;
  int i;
  Vdbe *v;
  int addr, end;
  char *zFile = 0;
  const char *zDb;
  sqlite *db = pParse->db;


  if( sqlite_malloc_failed  ) goto copy_cleanup;
  assert( pTableName->nSrc==1 );
  pTab = sqliteSrcListLookup(pParse, pTableName);
  if( pTab==0 || sqliteIsReadOnly(pParse, pTab, 0) ) goto copy_cleanup;
  zFile = sqliteStrNDup(pFilename->z, pFilename->n);
  sqliteDequote(zFile);
  assert( pTab->iDb<db->nDb );
  zDb = db->aDb[pTab->iDb].zName;
  if( sqliteAuthCheck(pParse, SQLITE_INSERT, pTab->zName, 0, zDb)
      || sqliteAuthCheck(pParse, SQLITE_COPY, pTab->zName, zFile, zDb) ){
    goto copy_cleanup;
  }
  v = sqliteGetVdbe(pParse);
  if( v ){
    sqliteBeginWriteOperation(pParse, 1, pTab->iDb);
    addr = sqliteVdbeOp3(v, OP_FileOpen, 0, 0, pFilename->z, pFilename->n);
    sqliteVdbeDequoteP3(v, addr);
    sqliteOpenTableAndIndices(pParse, pTab, 0);
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
    sqliteGenerateConstraintChecks(pParse, pTab, 0, 0, pTab->iPKey>=0, 
                                   0, onError, addr);
    sqliteCompleteInsertion(pParse, pTab, 0, 0, 0, 0, -1);
    if( (db->flags & SQLITE_CountRows)!=0 ){
      sqliteVdbeAddOp(v, OP_AddImm, 1, 0);  /* Increment row count */
    }
    sqliteVdbeAddOp(v, OP_Goto, 0, addr);
    sqliteVdbeResolveLabel(v, end);
    sqliteVdbeAddOp(v, OP_Noop, 0, 0);
    sqliteEndWriteOperation(pParse);
    if( db->flags & SQLITE_CountRows ){
      sqliteVdbeAddOp(v, OP_ColumnName, 0, 1);
      sqliteVdbeChangeP3(v, -1, "rows inserted", P3_STATIC);
      sqliteVdbeAddOp(v, OP_Callback, 1, 0);
    }
  }
  
copy_cleanup:
  sqliteSrcListDelete(pTableName);
  sqliteFree(zFile);
  return;
}
