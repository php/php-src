/*
** 2005 July 8
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code associated with the ANALYZE command.
**
** @(#) $Id$
*/
#ifndef SQLITE_OMIT_ANALYZE
#include "sqliteInt.h"

/*
** This routine generates code that opens the sqlite_stat1 table on cursor
** iStatCur.
**
** If the sqlite_stat1 tables does not previously exist, it is created.
** If it does previously exist, all entires associated with table zWhere
** are removed.  If zWhere==0 then all entries are removed.
*/
static void openStatTable(
  Parse *pParse,          /* Parsing context */
  int iDb,                /* The database we are looking in */
  int iStatCur,           /* Open the sqlite_stat1 table on this cursor */
  const char *zWhere      /* Delete entries associated with this table */
){
  sqlite3 *db = pParse->db;
  Db *pDb;
  int iRootPage;
  Table *pStat;
  Vdbe *v = sqlite3GetVdbe(pParse);

  pDb = &db->aDb[iDb];
  if( (pStat = sqlite3FindTable(db, "sqlite_stat1", pDb->zName))==0 ){
    /* The sqlite_stat1 tables does not exist.  Create it.  
    ** Note that a side-effect of the CREATE TABLE statement is to leave
    ** the rootpage of the new table on the top of the stack.  This is
    ** important because the OpenWrite opcode below will be needing it. */
    sqlite3NestedParse(pParse,
      "CREATE TABLE %Q.sqlite_stat1(tbl,idx,stat)",
      pDb->zName
    );
    iRootPage = 0;  /* Cause rootpage to be taken from top of stack */
  }else if( zWhere ){
    /* The sqlite_stat1 table exists.  Delete all entries associated with
    ** the table zWhere. */
    sqlite3NestedParse(pParse,
       "DELETE FROM %Q.sqlite_stat1 WHERE tbl=%Q",
       pDb->zName, zWhere
    );
    iRootPage = pStat->tnum;
  }else{
    /* The sqlite_stat1 table already exists.  Delete all rows. */
    iRootPage = pStat->tnum;
    sqlite3VdbeAddOp(v, OP_Clear, pStat->tnum, iDb);
  }

  /* Open the sqlite_stat1 table for writing. Unless it was created
  ** by this vdbe program, lock it for writing at the shared-cache level. 
  ** If this vdbe did create the sqlite_stat1 table, then it must have 
  ** already obtained a schema-lock, making the write-lock redundant.
  */
  if( iRootPage>0 ){
    sqlite3TableLock(pParse, iDb, iRootPage, 1, "sqlite_stat1");
  }
  sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
  sqlite3VdbeAddOp(v, OP_OpenWrite, iStatCur, iRootPage);
  sqlite3VdbeAddOp(v, OP_SetNumColumns, iStatCur, 3);
}

/*
** Generate code to do an analysis of all indices associated with
** a single table.
*/
static void analyzeOneTable(
  Parse *pParse,   /* Parser context */
  Table *pTab,     /* Table whose indices are to be analyzed */
  int iStatCur,    /* Cursor that writes to the sqlite_stat1 table */
  int iMem         /* Available memory locations begin here */
){
  Index *pIdx;     /* An index to being analyzed */
  int iIdxCur;     /* Cursor number for index being analyzed */
  int nCol;        /* Number of columns in the index */
  Vdbe *v;         /* The virtual machine being built up */
  int i;           /* Loop counter */
  int topOfLoop;   /* The top of the loop */
  int endOfLoop;   /* The end of the loop */
  int addr;        /* The address of an instruction */
  int iDb;         /* Index of database containing pTab */

  v = sqlite3GetVdbe(pParse);
  if( pTab==0 || pTab->pIndex==0 ){
    /* Do no analysis for tables that have no indices */
    return;
  }

  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
  assert( iDb>=0 );
#ifndef SQLITE_OMIT_AUTHORIZATION
  if( sqlite3AuthCheck(pParse, SQLITE_ANALYZE, pTab->zName, 0,
      pParse->db->aDb[iDb].zName ) ){
    return;
  }
#endif

  /* Establish a read-lock on the table at the shared-cache level. */
  sqlite3TableLock(pParse, iDb, pTab->tnum, 0, pTab->zName);

  iIdxCur = pParse->nTab;
  for(pIdx=pTab->pIndex; pIdx; pIdx=pIdx->pNext){
    KeyInfo *pKey = sqlite3IndexKeyinfo(pParse, pIdx);

    /* Open a cursor to the index to be analyzed
    */
    assert( iDb==sqlite3SchemaToIndex(pParse->db, pIdx->pSchema) );
    sqlite3VdbeAddOp(v, OP_Integer, iDb, 0);
    VdbeComment((v, "# %s", pIdx->zName));
    sqlite3VdbeOp3(v, OP_OpenRead, iIdxCur, pIdx->tnum,
        (char *)pKey, P3_KEYINFO_HANDOFF);
    nCol = pIdx->nColumn;
    if( iMem+nCol*2>=pParse->nMem ){
      pParse->nMem = iMem+nCol*2+1;
    }
    sqlite3VdbeAddOp(v, OP_SetNumColumns, iIdxCur, nCol+1);

    /* Memory cells are used as follows:
    **
    **    mem[iMem]:             The total number of rows in the table.
    **    mem[iMem+1]:           Number of distinct values in column 1
    **    ...
    **    mem[iMem+nCol]:        Number of distinct values in column N
    **    mem[iMem+nCol+1]       Last observed value of column 1
    **    ...
    **    mem[iMem+nCol+nCol]:   Last observed value of column N
    **
    ** Cells iMem through iMem+nCol are initialized to 0.  The others
    ** are initialized to NULL.
    */
    for(i=0; i<=nCol; i++){
      sqlite3VdbeAddOp(v, OP_MemInt, 0, iMem+i);
    }
    for(i=0; i<nCol; i++){
      sqlite3VdbeAddOp(v, OP_MemNull, iMem+nCol+i+1, 0);
    }

    /* Do the analysis.
    */
    endOfLoop = sqlite3VdbeMakeLabel(v);
    sqlite3VdbeAddOp(v, OP_Rewind, iIdxCur, endOfLoop);
    topOfLoop = sqlite3VdbeCurrentAddr(v);
    sqlite3VdbeAddOp(v, OP_MemIncr, 1, iMem);
    for(i=0; i<nCol; i++){
      sqlite3VdbeAddOp(v, OP_Column, iIdxCur, i);
      sqlite3VdbeAddOp(v, OP_MemLoad, iMem+nCol+i+1, 0);
      sqlite3VdbeAddOp(v, OP_Ne, 0x100, 0);
    }
    sqlite3VdbeAddOp(v, OP_Goto, 0, endOfLoop);
    for(i=0; i<nCol; i++){
      addr = sqlite3VdbeAddOp(v, OP_MemIncr, 1, iMem+i+1);
      sqlite3VdbeChangeP2(v, topOfLoop + 3*i + 3, addr);
      sqlite3VdbeAddOp(v, OP_Column, iIdxCur, i);
      sqlite3VdbeAddOp(v, OP_MemStore, iMem+nCol+i+1, 1);
    }
    sqlite3VdbeResolveLabel(v, endOfLoop);
    sqlite3VdbeAddOp(v, OP_Next, iIdxCur, topOfLoop);
    sqlite3VdbeAddOp(v, OP_Close, iIdxCur, 0);

    /* Store the results.  
    **
    ** The result is a single row of the sqlite_stmt1 table.  The first
    ** two columns are the names of the table and index.  The third column
    ** is a string composed of a list of integer statistics about the
    ** index.  The first integer in the list is the total number of entires
    ** in the index.  There is one additional integer in the list for each
    ** column of the table.  This additional integer is a guess of how many
    ** rows of the table the index will select.  If D is the count of distinct
    ** values and K is the total number of rows, then the integer is computed
    ** as:
    **
    **        I = (K+D-1)/D
    **
    ** If K==0 then no entry is made into the sqlite_stat1 table.  
    ** If K>0 then it is always the case the D>0 so division by zero
    ** is never possible.
    */
    sqlite3VdbeAddOp(v, OP_MemLoad, iMem, 0);
    addr = sqlite3VdbeAddOp(v, OP_IfNot, 0, 0);
    sqlite3VdbeAddOp(v, OP_NewRowid, iStatCur, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, pTab->zName, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, pIdx->zName, 0);
    sqlite3VdbeAddOp(v, OP_MemLoad, iMem, 0);
    sqlite3VdbeOp3(v, OP_String8, 0, 0, " ", 0);
    for(i=0; i<nCol; i++){
      sqlite3VdbeAddOp(v, OP_MemLoad, iMem, 0);
      sqlite3VdbeAddOp(v, OP_MemLoad, iMem+i+1, 0);
      sqlite3VdbeAddOp(v, OP_Add, 0, 0);
      sqlite3VdbeAddOp(v, OP_AddImm, -1, 0);
      sqlite3VdbeAddOp(v, OP_MemLoad, iMem+i+1, 0);
      sqlite3VdbeAddOp(v, OP_Divide, 0, 0);
      sqlite3VdbeAddOp(v, OP_ToInt, 0, 0);
      if( i==nCol-1 ){
        sqlite3VdbeAddOp(v, OP_Concat, nCol*2-1, 0);
      }else{
        sqlite3VdbeAddOp(v, OP_Dup, 1, 0);
      }
    }
    sqlite3VdbeOp3(v, OP_MakeRecord, 3, 0, "aaa", 0);
    sqlite3VdbeAddOp(v, OP_Insert, iStatCur, 0);
    sqlite3VdbeJumpHere(v, addr);
  }
}

/*
** Generate code that will cause the most recent index analysis to
** be laoded into internal hash tables where is can be used.
*/
static void loadAnalysis(Parse *pParse, int iDb){
  Vdbe *v = sqlite3GetVdbe(pParse);
  sqlite3VdbeAddOp(v, OP_LoadAnalysis, iDb, 0);
}

/*
** Generate code that will do an analysis of an entire database
*/
static void analyzeDatabase(Parse *pParse, int iDb){
  sqlite3 *db = pParse->db;
  Schema *pSchema = db->aDb[iDb].pSchema;    /* Schema of database iDb */
  HashElem *k;
  int iStatCur;
  int iMem;

  sqlite3BeginWriteOperation(pParse, 0, iDb);
  iStatCur = pParse->nTab++;
  openStatTable(pParse, iDb, iStatCur, 0);
  iMem = pParse->nMem;
  for(k=sqliteHashFirst(&pSchema->tblHash); k; k=sqliteHashNext(k)){
    Table *pTab = (Table*)sqliteHashData(k);
    analyzeOneTable(pParse, pTab, iStatCur, iMem);
  }
  loadAnalysis(pParse, iDb);
}

/*
** Generate code that will do an analysis of a single table in
** a database.
*/
static void analyzeTable(Parse *pParse, Table *pTab){
  int iDb;
  int iStatCur;

  assert( pTab!=0 );
  iDb = sqlite3SchemaToIndex(pParse->db, pTab->pSchema);
  sqlite3BeginWriteOperation(pParse, 0, iDb);
  iStatCur = pParse->nTab++;
  openStatTable(pParse, iDb, iStatCur, pTab->zName);
  analyzeOneTable(pParse, pTab, iStatCur, pParse->nMem);
  loadAnalysis(pParse, iDb);
}

/*
** Generate code for the ANALYZE command.  The parser calls this routine
** when it recognizes an ANALYZE command.
**
**        ANALYZE                            -- 1
**        ANALYZE  <database>                -- 2
**        ANALYZE  ?<database>.?<tablename>  -- 3
**
** Form 1 causes all indices in all attached databases to be analyzed.
** Form 2 analyzes all indices the single database named.
** Form 3 analyzes all indices associated with the named table.
*/
void sqlite3Analyze(Parse *pParse, Token *pName1, Token *pName2){
  sqlite3 *db = pParse->db;
  int iDb;
  int i;
  char *z, *zDb;
  Table *pTab;
  Token *pTableName;

  /* Read the database schema. If an error occurs, leave an error message
  ** and code in pParse and return NULL. */
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    return;
  }

  if( pName1==0 ){
    /* Form 1:  Analyze everything */
    for(i=0; i<db->nDb; i++){
      if( i==1 ) continue;  /* Do not analyze the TEMP database */
      analyzeDatabase(pParse, i);
    }
  }else if( pName2==0 || pName2->n==0 ){
    /* Form 2:  Analyze the database or table named */
    iDb = sqlite3FindDb(db, pName1);
    if( iDb>=0 ){
      analyzeDatabase(pParse, iDb);
    }else{
      z = sqlite3NameFromToken(pName1);
      pTab = sqlite3LocateTable(pParse, z, 0);
      sqliteFree(z);
      if( pTab ){
        analyzeTable(pParse, pTab);
      }
    }
  }else{
    /* Form 3: Analyze the fully qualified table name */
    iDb = sqlite3TwoPartName(pParse, pName1, pName2, &pTableName);
    if( iDb>=0 ){
      zDb = db->aDb[iDb].zName;
      z = sqlite3NameFromToken(pTableName);
      pTab = sqlite3LocateTable(pParse, z, zDb);
      sqliteFree(z);
      if( pTab ){
        analyzeTable(pParse, pTab);
      }
    }   
  }
}

/*
** Used to pass information from the analyzer reader through to the
** callback routine.
*/
typedef struct analysisInfo analysisInfo;
struct analysisInfo {
  sqlite3 *db;
  const char *zDatabase;
};

/*
** This callback is invoked once for each index when reading the
** sqlite_stat1 table.  
**
**     argv[0] = name of the index
**     argv[1] = results of analysis - on integer for each column
*/
static int analysisLoader(void *pData, int argc, char **argv, char **azNotUsed){
  analysisInfo *pInfo = (analysisInfo*)pData;
  Index *pIndex;
  int i, c;
  unsigned int v;
  const char *z;

  assert( argc==2 );
  if( argv==0 || argv[0]==0 || argv[1]==0 ){
    return 0;
  }
  pIndex = sqlite3FindIndex(pInfo->db, argv[0], pInfo->zDatabase);
  if( pIndex==0 ){
    return 0;
  }
  z = argv[1];
  for(i=0; *z && i<=pIndex->nColumn; i++){
    v = 0;
    while( (c=z[0])>='0' && c<='9' ){
      v = v*10 + c - '0';
      z++;
    }
    pIndex->aiRowEst[i] = v;
    if( *z==' ' ) z++;
  }
  return 0;
}

/*
** Load the content of the sqlite_stat1 table into the index hash tables.
*/
void sqlite3AnalysisLoad(sqlite3 *db, int iDb){
  analysisInfo sInfo;
  HashElem *i;
  char *zSql;

  /* Clear any prior statistics */
  for(i=sqliteHashFirst(&db->aDb[iDb].pSchema->idxHash);i;i=sqliteHashNext(i)){
    Index *pIdx = sqliteHashData(i);
    sqlite3DefaultRowEst(pIdx);
  }

  /* Check to make sure the sqlite_stat1 table existss */
  sInfo.db = db;
  sInfo.zDatabase = db->aDb[iDb].zName;
  if( sqlite3FindTable(db, "sqlite_stat1", sInfo.zDatabase)==0 ){
     return;
  }


  /* Load new statistics out of the sqlite_stat1 table */
  zSql = sqlite3MPrintf("SELECT idx, stat FROM %Q.sqlite_stat1",
                        sInfo.zDatabase);
  sqlite3SafetyOff(db);
  sqlite3_exec(db, zSql, analysisLoader, &sInfo, 0);
  sqlite3SafetyOn(db);
  sqliteFree(zSql);
}


#endif /* SQLITE_OMIT_ANALYZE */
