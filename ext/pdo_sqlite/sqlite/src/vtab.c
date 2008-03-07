/*
** 2006 June 10
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used to help implement virtual tables.
**
** $Id$
*/
#ifndef SQLITE_OMIT_VIRTUALTABLE
#include "sqliteInt.h"

static int createModule(
  sqlite3 *db,                    /* Database in which module is registered */
  const char *zName,              /* Name assigned to this module */
  const sqlite3_module *pModule,  /* The definition of the module */
  void *pAux,                     /* Context pointer for xCreate/xConnect */
  void (*xDestroy)(void *)        /* Module destructor function */
) {
  int rc, nName;
  Module *pMod;

  sqlite3_mutex_enter(db->mutex);
  nName = strlen(zName);
  pMod = (Module *)sqlite3DbMallocRaw(db, sizeof(Module) + nName + 1);
  if( pMod ){
    char *zCopy = (char *)(&pMod[1]);
    memcpy(zCopy, zName, nName+1);
    pMod->zName = zCopy;
    pMod->pModule = pModule;
    pMod->pAux = pAux;
    pMod->xDestroy = xDestroy;
    pMod = (Module *)sqlite3HashInsert(&db->aModule, zCopy, nName, (void*)pMod);
    if( pMod && pMod->xDestroy ){
      pMod->xDestroy(pMod->pAux);
    }
    sqlite3_free(pMod);
    sqlite3ResetInternalSchema(db, 0);
  }
  rc = sqlite3ApiExit(db, SQLITE_OK);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}


/*
** External API function used to create a new virtual-table module.
*/
int sqlite3_create_module(
  sqlite3 *db,                    /* Database in which module is registered */
  const char *zName,              /* Name assigned to this module */
  const sqlite3_module *pModule,  /* The definition of the module */
  void *pAux                      /* Context pointer for xCreate/xConnect */
){
  return createModule(db, zName, pModule, pAux, 0);
}

/*
** External API function used to create a new virtual-table module.
*/
int sqlite3_create_module_v2(
  sqlite3 *db,                    /* Database in which module is registered */
  const char *zName,              /* Name assigned to this module */
  const sqlite3_module *pModule,  /* The definition of the module */
  void *pAux,                     /* Context pointer for xCreate/xConnect */
  void (*xDestroy)(void *)        /* Module destructor function */
){
  return createModule(db, zName, pModule, pAux, xDestroy);
}

/*
** Lock the virtual table so that it cannot be disconnected.
** Locks nest.  Every lock should have a corresponding unlock.
** If an unlock is omitted, resources leaks will occur.  
**
** If a disconnect is attempted while a virtual table is locked,
** the disconnect is deferred until all locks have been removed.
*/
void sqlite3VtabLock(sqlite3_vtab *pVtab){
  pVtab->nRef++;
}

/*
** Unlock a virtual table.  When the last lock is removed,
** disconnect the virtual table.
*/
void sqlite3VtabUnlock(sqlite3 *db, sqlite3_vtab *pVtab){
  pVtab->nRef--;
  assert(db);
  assert( sqlite3SafetyCheckOk(db) );
  if( pVtab->nRef==0 ){
    if( db->magic==SQLITE_MAGIC_BUSY ){
      (void)sqlite3SafetyOff(db);
      pVtab->pModule->xDisconnect(pVtab);
      (void)sqlite3SafetyOn(db);
    } else {
      pVtab->pModule->xDisconnect(pVtab);
    }
  }
}

/*
** Clear any and all virtual-table information from the Table record.
** This routine is called, for example, just before deleting the Table
** record.
*/
void sqlite3VtabClear(Table *p){
  sqlite3_vtab *pVtab = p->pVtab;
  if( pVtab ){
    assert( p->pMod && p->pMod->pModule );
    sqlite3VtabUnlock(p->pSchema->db, pVtab);
    p->pVtab = 0;
  }
  if( p->azModuleArg ){
    int i;
    for(i=0; i<p->nModuleArg; i++){
      sqlite3_free(p->azModuleArg[i]);
    }
    sqlite3_free(p->azModuleArg);
  }
}

/*
** Add a new module argument to pTable->azModuleArg[].
** The string is not copied - the pointer is stored.  The
** string will be freed automatically when the table is
** deleted.
*/
static void addModuleArgument(sqlite3 *db, Table *pTable, char *zArg){
  int i = pTable->nModuleArg++;
  int nBytes = sizeof(char *)*(1+pTable->nModuleArg);
  char **azModuleArg;
  azModuleArg = sqlite3DbRealloc(db, pTable->azModuleArg, nBytes);
  if( azModuleArg==0 ){
    int j;
    for(j=0; j<i; j++){
      sqlite3_free(pTable->azModuleArg[j]);
    }
    sqlite3_free(zArg);
    sqlite3_free(pTable->azModuleArg);
    pTable->nModuleArg = 0;
  }else{
    azModuleArg[i] = zArg;
    azModuleArg[i+1] = 0;
  }
  pTable->azModuleArg = azModuleArg;
}

/*
** The parser calls this routine when it first sees a CREATE VIRTUAL TABLE
** statement.  The module name has been parsed, but the optional list
** of parameters that follow the module name are still pending.
*/
void sqlite3VtabBeginParse(
  Parse *pParse,        /* Parsing context */
  Token *pName1,        /* Name of new table, or database name */
  Token *pName2,        /* Name of new table or NULL */
  Token *pModuleName    /* Name of the module for the virtual table */
){
  int iDb;              /* The database the table is being created in */
  Table *pTable;        /* The new virtual table */
  sqlite3 *db;          /* Database connection */

  if( pParse->db->flags & SQLITE_SharedCache ){
    sqlite3ErrorMsg(pParse, "Cannot use virtual tables in shared-cache mode");
    return;
  }

  sqlite3StartTable(pParse, pName1, pName2, 0, 0, 1, 0);
  pTable = pParse->pNewTable;
  if( pTable==0 || pParse->nErr ) return;
  assert( 0==pTable->pIndex );

  db = pParse->db;
  iDb = sqlite3SchemaToIndex(db, pTable->pSchema);
  assert( iDb>=0 );

  pTable->isVirtual = 1;
  pTable->nModuleArg = 0;
  addModuleArgument(db, pTable, sqlite3NameFromToken(db, pModuleName));
  addModuleArgument(db, pTable, sqlite3DbStrDup(db, db->aDb[iDb].zName));
  addModuleArgument(db, pTable, sqlite3DbStrDup(db, pTable->zName));
  pParse->sNameToken.n = pModuleName->z + pModuleName->n - pName1->z;

#ifndef SQLITE_OMIT_AUTHORIZATION
  /* Creating a virtual table invokes the authorization callback twice.
  ** The first invocation, to obtain permission to INSERT a row into the
  ** sqlite_master table, has already been made by sqlite3StartTable().
  ** The second call, to obtain permission to create the table, is made now.
  */
  if( pTable->azModuleArg ){
    sqlite3AuthCheck(pParse, SQLITE_CREATE_VTABLE, pTable->zName, 
            pTable->azModuleArg[0], pParse->db->aDb[iDb].zName);
  }
#endif
}

/*
** This routine takes the module argument that has been accumulating
** in pParse->zArg[] and appends it to the list of arguments on the
** virtual table currently under construction in pParse->pTable.
*/
static void addArgumentToVtab(Parse *pParse){
  if( pParse->sArg.z && pParse->pNewTable ){
    const char *z = (const char*)pParse->sArg.z;
    int n = pParse->sArg.n;
    sqlite3 *db = pParse->db;
    addModuleArgument(db, pParse->pNewTable, sqlite3DbStrNDup(db, z, n));
  }
}

/*
** The parser calls this routine after the CREATE VIRTUAL TABLE statement
** has been completely parsed.
*/
void sqlite3VtabFinishParse(Parse *pParse, Token *pEnd){
  Table *pTab;        /* The table being constructed */
  sqlite3 *db;        /* The database connection */
  char *zModule;      /* The module name of the table: USING modulename */
  Module *pMod = 0;

  addArgumentToVtab(pParse);
  pParse->sArg.z = 0;

  /* Lookup the module name. */
  pTab = pParse->pNewTable;
  if( pTab==0 ) return;
  db = pParse->db;
  if( pTab->nModuleArg<1 ) return;
  zModule = pTab->azModuleArg[0];
  pMod = (Module *)sqlite3HashFind(&db->aModule, zModule, strlen(zModule));
  pTab->pMod = pMod;
  
  /* If the CREATE VIRTUAL TABLE statement is being entered for the
  ** first time (in other words if the virtual table is actually being
  ** created now instead of just being read out of sqlite_master) then
  ** do additional initialization work and store the statement text
  ** in the sqlite_master table.
  */
  if( !db->init.busy ){
    char *zStmt;
    char *zWhere;
    int iDb;
    Vdbe *v;

    /* Compute the complete text of the CREATE VIRTUAL TABLE statement */
    if( pEnd ){
      pParse->sNameToken.n = pEnd->z - pParse->sNameToken.z + pEnd->n;
    }
    zStmt = sqlite3MPrintf(db, "CREATE VIRTUAL TABLE %T", &pParse->sNameToken);

    /* A slot for the record has already been allocated in the 
    ** SQLITE_MASTER table.  We just need to update that slot with all
    ** the information we've collected.  
    **
    ** The VM register number pParse->regRowid holds the rowid of an
    ** entry in the sqlite_master table tht was created for this vtab
    ** by sqlite3StartTable().
    */
    iDb = sqlite3SchemaToIndex(db, pTab->pSchema);
    sqlite3NestedParse(pParse,
      "UPDATE %Q.%s "
         "SET type='table', name=%Q, tbl_name=%Q, rootpage=0, sql=%Q "
       "WHERE rowid=#%d",
      db->aDb[iDb].zName, SCHEMA_TABLE(iDb),
      pTab->zName,
      pTab->zName,
      zStmt,
      pParse->regRowid
    );
    sqlite3_free(zStmt);
    v = sqlite3GetVdbe(pParse);
    sqlite3ChangeCookie(pParse, iDb);

    sqlite3VdbeAddOp2(v, OP_Expire, 0, 0);
    zWhere = sqlite3MPrintf(db, "name='%q'", pTab->zName);
    sqlite3VdbeAddOp4(v, OP_ParseSchema, iDb, 1, 0, zWhere, P4_DYNAMIC);
    sqlite3VdbeAddOp4(v, OP_VCreate, iDb, 0, 0, 
                         pTab->zName, strlen(pTab->zName) + 1);
  }

  /* If we are rereading the sqlite_master table create the in-memory
  ** record of the table. If the module has already been registered,
  ** also call the xConnect method here.
  */
  else {
    Table *pOld;
    Schema *pSchema = pTab->pSchema;
    const char *zName = pTab->zName;
    int nName = strlen(zName) + 1;
    pOld = sqlite3HashInsert(&pSchema->tblHash, zName, nName, pTab);
    if( pOld ){
      db->mallocFailed = 1;
      assert( pTab==pOld );  /* Malloc must have failed inside HashInsert() */
      return;
    }
    pSchema->db = pParse->db;
    pParse->pNewTable = 0;
  }
}

/*
** The parser calls this routine when it sees the first token
** of an argument to the module name in a CREATE VIRTUAL TABLE statement.
*/
void sqlite3VtabArgInit(Parse *pParse){
  addArgumentToVtab(pParse);
  pParse->sArg.z = 0;
  pParse->sArg.n = 0;
}

/*
** The parser calls this routine for each token after the first token
** in an argument to the module name in a CREATE VIRTUAL TABLE statement.
*/
void sqlite3VtabArgExtend(Parse *pParse, Token *p){
  Token *pArg = &pParse->sArg;
  if( pArg->z==0 ){
    pArg->z = p->z;
    pArg->n = p->n;
  }else{
    assert(pArg->z < p->z);
    pArg->n = (p->z + p->n - pArg->z);
  }
}

/*
** Invoke a virtual table constructor (either xCreate or xConnect). The
** pointer to the function to invoke is passed as the fourth parameter
** to this procedure.
*/
static int vtabCallConstructor(
  sqlite3 *db, 
  Table *pTab,
  Module *pMod,
  int (*xConstruct)(sqlite3*,void*,int,const char*const*,sqlite3_vtab**,char**),
  char **pzErr
){
  int rc;
  int rc2;
  sqlite3_vtab *pVtab = 0;
  const char *const*azArg = (const char *const*)pTab->azModuleArg;
  int nArg = pTab->nModuleArg;
  char *zErr = 0;
  char *zModuleName = sqlite3MPrintf(db, "%s", pTab->zName);

  if( !zModuleName ){
    return SQLITE_NOMEM;
  }

  assert( !db->pVTab );
  assert( xConstruct );

  db->pVTab = pTab;
  rc = sqlite3SafetyOff(db);
  assert( rc==SQLITE_OK );
  rc = xConstruct(db, pMod->pAux, nArg, azArg, &pVtab, &zErr);
  rc2 = sqlite3SafetyOn(db);
  if( rc==SQLITE_OK && pVtab ){
    pVtab->pModule = pMod->pModule;
    pVtab->nRef = 1;
    pTab->pVtab = pVtab;
  }

  if( SQLITE_OK!=rc ){
    if( zErr==0 ){
      *pzErr = sqlite3MPrintf(db, "vtable constructor failed: %s", zModuleName);
    }else {
      *pzErr = sqlite3MPrintf(db, "%s", zErr);
      sqlite3_free(zErr);
    }
  }else if( db->pVTab ){
    const char *zFormat = "vtable constructor did not declare schema: %s";
    *pzErr = sqlite3MPrintf(db, zFormat, pTab->zName);
    rc = SQLITE_ERROR;
  } 
  if( rc==SQLITE_OK ){
    rc = rc2;
  }
  db->pVTab = 0;
  sqlite3_free(zModuleName);

  /* If everything went according to plan, loop through the columns
  ** of the table to see if any of them contain the token "hidden".
  ** If so, set the Column.isHidden flag and remove the token from
  ** the type string.
  */
  if( rc==SQLITE_OK ){
    int iCol;
    for(iCol=0; iCol<pTab->nCol; iCol++){
      char *zType = pTab->aCol[iCol].zType;
      int nType;
      int i = 0;
      if( !zType ) continue;
      nType = strlen(zType);
      if( sqlite3StrNICmp("hidden", zType, 6) || (zType[6] && zType[6]!=' ') ){
        for(i=0; i<nType; i++){
          if( (0==sqlite3StrNICmp(" hidden", &zType[i], 7))
           && (zType[i+7]=='\0' || zType[i+7]==' ')
          ){
            i++;
            break;
          }
        }
      }
      if( i<nType ){
        int j;
        int nDel = 6 + (zType[i+6] ? 1 : 0);
        for(j=i; (j+nDel)<=nType; j++){
          zType[j] = zType[j+nDel];
        }
        if( zType[i]=='\0' && i>0 ){
          assert(zType[i-1]==' ');
          zType[i-1] = '\0';
        }
        pTab->aCol[iCol].isHidden = 1;
      }
    }
  }
  return rc;
}

/*
** This function is invoked by the parser to call the xConnect() method
** of the virtual table pTab. If an error occurs, an error code is returned 
** and an error left in pParse.
**
** This call is a no-op if table pTab is not a virtual table.
*/
int sqlite3VtabCallConnect(Parse *pParse, Table *pTab){
  Module *pMod;
  int rc = SQLITE_OK;

  if( !pTab || !pTab->isVirtual || pTab->pVtab ){
    return SQLITE_OK;
  }

  pMod = pTab->pMod;
  if( !pMod ){
    const char *zModule = pTab->azModuleArg[0];
    sqlite3ErrorMsg(pParse, "no such module: %s", zModule);
    rc = SQLITE_ERROR;
  } else {
    char *zErr = 0;
    sqlite3 *db = pParse->db;
    rc = vtabCallConstructor(db, pTab, pMod, pMod->pModule->xConnect, &zErr);
    if( rc!=SQLITE_OK ){
      sqlite3ErrorMsg(pParse, "%s", zErr);
    }
    sqlite3_free(zErr);
  }

  return rc;
}

/*
** Add the virtual table pVtab to the array sqlite3.aVTrans[].
*/
static int addToVTrans(sqlite3 *db, sqlite3_vtab *pVtab){
  const int ARRAY_INCR = 5;

  /* Grow the sqlite3.aVTrans array if required */
  if( (db->nVTrans%ARRAY_INCR)==0 ){
    sqlite3_vtab **aVTrans;
    int nBytes = sizeof(sqlite3_vtab *) * (db->nVTrans + ARRAY_INCR);
    aVTrans = sqlite3DbRealloc(db, (void *)db->aVTrans, nBytes);
    if( !aVTrans ){
      return SQLITE_NOMEM;
    }
    memset(&aVTrans[db->nVTrans], 0, sizeof(sqlite3_vtab *)*ARRAY_INCR);
    db->aVTrans = aVTrans;
  }

  /* Add pVtab to the end of sqlite3.aVTrans */
  db->aVTrans[db->nVTrans++] = pVtab;
  sqlite3VtabLock(pVtab);
  return SQLITE_OK;
}

/*
** This function is invoked by the vdbe to call the xCreate method
** of the virtual table named zTab in database iDb. 
**
** If an error occurs, *pzErr is set to point an an English language
** description of the error and an SQLITE_XXX error code is returned.
** In this case the caller must call sqlite3_free() on *pzErr.
*/
int sqlite3VtabCallCreate(sqlite3 *db, int iDb, const char *zTab, char **pzErr){
  int rc = SQLITE_OK;
  Table *pTab;
  Module *pMod;
  const char *zModule;

  pTab = sqlite3FindTable(db, zTab, db->aDb[iDb].zName);
  assert(pTab && pTab->isVirtual && !pTab->pVtab);
  pMod = pTab->pMod;
  zModule = pTab->azModuleArg[0];

  /* If the module has been registered and includes a Create method, 
  ** invoke it now. If the module has not been registered, return an 
  ** error. Otherwise, do nothing.
  */
  if( !pMod ){
    *pzErr = sqlite3MPrintf(db, "no such module: %s", zModule);
    rc = SQLITE_ERROR;
  }else{
    rc = vtabCallConstructor(db, pTab, pMod, pMod->pModule->xCreate, pzErr);
  }

  if( rc==SQLITE_OK && pTab->pVtab ){
      rc = addToVTrans(db, pTab->pVtab);
  }

  return rc;
}

/*
** This function is used to set the schema of a virtual table.  It is only
** valid to call this function from within the xCreate() or xConnect() of a
** virtual table module.
*/
int sqlite3_declare_vtab(sqlite3 *db, const char *zCreateTable){
  Parse sParse;

  int rc = SQLITE_OK;
  Table *pTab;
  char *zErr = 0;

  sqlite3_mutex_enter(db->mutex);
  pTab = db->pVTab;
  if( !pTab ){
    sqlite3Error(db, SQLITE_MISUSE, 0);
    sqlite3_mutex_leave(db->mutex);
    return SQLITE_MISUSE;
  }
  assert(pTab->isVirtual && pTab->nCol==0 && pTab->aCol==0);

  memset(&sParse, 0, sizeof(Parse));
  sParse.declareVtab = 1;
  sParse.db = db;

  if( 
      SQLITE_OK == sqlite3RunParser(&sParse, zCreateTable, &zErr) && 
      sParse.pNewTable && 
      !sParse.pNewTable->pSelect && 
      !sParse.pNewTable->isVirtual 
  ){
    pTab->aCol = sParse.pNewTable->aCol;
    pTab->nCol = sParse.pNewTable->nCol;
    sParse.pNewTable->nCol = 0;
    sParse.pNewTable->aCol = 0;
    db->pVTab = 0;
  } else {
    sqlite3Error(db, SQLITE_ERROR, zErr);
    sqlite3_free(zErr);
    rc = SQLITE_ERROR;
  }
  sParse.declareVtab = 0;

  sqlite3_finalize((sqlite3_stmt*)sParse.pVdbe);
  sqlite3DeleteTable(sParse.pNewTable);
  sParse.pNewTable = 0;

  assert( (rc&0xff)==rc );
  rc = sqlite3ApiExit(db, rc);
  sqlite3_mutex_leave(db->mutex);
  return rc;
}

/*
** This function is invoked by the vdbe to call the xDestroy method
** of the virtual table named zTab in database iDb. This occurs
** when a DROP TABLE is mentioned.
**
** This call is a no-op if zTab is not a virtual table.
*/
int sqlite3VtabCallDestroy(sqlite3 *db, int iDb, const char *zTab)
{
  int rc = SQLITE_OK;
  Table *pTab;

  pTab = sqlite3FindTable(db, zTab, db->aDb[iDb].zName);
  assert(pTab);
  if( pTab->pVtab ){
    int (*xDestroy)(sqlite3_vtab *pVTab) = pTab->pMod->pModule->xDestroy;
    rc = sqlite3SafetyOff(db);
    assert( rc==SQLITE_OK );
    if( xDestroy ){
      rc = xDestroy(pTab->pVtab);
    }
    (void)sqlite3SafetyOn(db);
    if( rc==SQLITE_OK ){
      pTab->pVtab = 0;
    }
  }

  return rc;
}

/*
** This function invokes either the xRollback or xCommit method
** of each of the virtual tables in the sqlite3.aVTrans array. The method
** called is identified by the second argument, "offset", which is
** the offset of the method to call in the sqlite3_module structure.
**
** The array is cleared after invoking the callbacks. 
*/
static void callFinaliser(sqlite3 *db, int offset){
  int i;
  if( db->aVTrans ){
    for(i=0; i<db->nVTrans && db->aVTrans[i]; i++){
      sqlite3_vtab *pVtab = db->aVTrans[i];
      int (*x)(sqlite3_vtab *);
      x = *(int (**)(sqlite3_vtab *))((char *)pVtab->pModule + offset);
      if( x ) x(pVtab);
      sqlite3VtabUnlock(db, pVtab);
    }
    sqlite3_free(db->aVTrans);
    db->nVTrans = 0;
    db->aVTrans = 0;
  }
}

/*
** If argument rc2 is not SQLITE_OK, then return it and do nothing. 
** Otherwise, invoke the xSync method of all virtual tables in the 
** sqlite3.aVTrans array. Return the error code for the first error 
** that occurs, or SQLITE_OK if all xSync operations are successful.
*/
int sqlite3VtabSync(sqlite3 *db, int rc2){
  int i;
  int rc = SQLITE_OK;
  int rcsafety;
  sqlite3_vtab **aVTrans = db->aVTrans;
  if( rc2!=SQLITE_OK ) return rc2;

  rc = sqlite3SafetyOff(db);
  db->aVTrans = 0;
  for(i=0; rc==SQLITE_OK && i<db->nVTrans && aVTrans[i]; i++){
    sqlite3_vtab *pVtab = aVTrans[i];
    int (*x)(sqlite3_vtab *);
    x = pVtab->pModule->xSync;
    if( x ){
      rc = x(pVtab);
    }
  }
  db->aVTrans = aVTrans;
  rcsafety = sqlite3SafetyOn(db);

  if( rc==SQLITE_OK ){
    rc = rcsafety;
  }
  return rc;
}

/*
** Invoke the xRollback method of all virtual tables in the 
** sqlite3.aVTrans array. Then clear the array itself.
*/
int sqlite3VtabRollback(sqlite3 *db){
  callFinaliser(db, (int)(&((sqlite3_module *)0)->xRollback));
  return SQLITE_OK;
}

/*
** Invoke the xCommit method of all virtual tables in the 
** sqlite3.aVTrans array. Then clear the array itself.
*/
int sqlite3VtabCommit(sqlite3 *db){
  callFinaliser(db, (int)(&((sqlite3_module *)0)->xCommit));
  return SQLITE_OK;
}

/*
** If the virtual table pVtab supports the transaction interface
** (xBegin/xRollback/xCommit and optionally xSync) and a transaction is
** not currently open, invoke the xBegin method now.
**
** If the xBegin call is successful, place the sqlite3_vtab pointer
** in the sqlite3.aVTrans array.
*/
int sqlite3VtabBegin(sqlite3 *db, sqlite3_vtab *pVtab){
  int rc = SQLITE_OK;
  const sqlite3_module *pModule;

  /* Special case: If db->aVTrans is NULL and db->nVTrans is greater
  ** than zero, then this function is being called from within a
  ** virtual module xSync() callback. It is illegal to write to 
  ** virtual module tables in this case, so return SQLITE_LOCKED.
  */
  if( 0==db->aVTrans && db->nVTrans>0 ){
    return SQLITE_LOCKED;
  }
  if( !pVtab ){
    return SQLITE_OK;
  } 
  pModule = pVtab->pModule;

  if( pModule->xBegin ){
    int i;


    /* If pVtab is already in the aVTrans array, return early */
    for(i=0; (i<db->nVTrans) && 0!=db->aVTrans[i]; i++){
      if( db->aVTrans[i]==pVtab ){
        return SQLITE_OK;
      }
    }

    /* Invoke the xBegin method */
    rc = pModule->xBegin(pVtab);
    if( rc!=SQLITE_OK ){
      return rc;
    }

    rc = addToVTrans(db, pVtab);
  }
  return rc;
}

/*
** The first parameter (pDef) is a function implementation.  The
** second parameter (pExpr) is the first argument to this function.
** If pExpr is a column in a virtual table, then let the virtual
** table implementation have an opportunity to overload the function.
**
** This routine is used to allow virtual table implementations to
** overload MATCH, LIKE, GLOB, and REGEXP operators.
**
** Return either the pDef argument (indicating no change) or a 
** new FuncDef structure that is marked as ephemeral using the
** SQLITE_FUNC_EPHEM flag.
*/
FuncDef *sqlite3VtabOverloadFunction(
  sqlite3 *db,    /* Database connection for reporting malloc problems */
  FuncDef *pDef,  /* Function to possibly overload */
  int nArg,       /* Number of arguments to the function */
  Expr *pExpr     /* First argument to the function */
){
  Table *pTab;
  sqlite3_vtab *pVtab;
  sqlite3_module *pMod;
  void (*xFunc)(sqlite3_context*,int,sqlite3_value**);
  void *pArg;
  FuncDef *pNew;
  int rc = 0;
  char *zLowerName;
  unsigned char *z;


  /* Check to see the left operand is a column in a virtual table */
  if( pExpr==0 ) return pDef;
  if( pExpr->op!=TK_COLUMN ) return pDef;
  pTab = pExpr->pTab;
  if( pTab==0 ) return pDef;
  if( !pTab->isVirtual ) return pDef;
  pVtab = pTab->pVtab;
  assert( pVtab!=0 );
  assert( pVtab->pModule!=0 );
  pMod = (sqlite3_module *)pVtab->pModule;
  if( pMod->xFindFunction==0 ) return pDef;
 
  /* Call the xFindFunction method on the virtual table implementation
  ** to see if the implementation wants to overload this function 
  */
  zLowerName = sqlite3DbStrDup(db, pDef->zName);
  if( zLowerName ){
    for(z=(unsigned char*)zLowerName; *z; z++){
      *z = sqlite3UpperToLower[*z];
    }
    rc = pMod->xFindFunction(pVtab, nArg, zLowerName, &xFunc, &pArg);
    sqlite3_free(zLowerName);
  }
  if( rc==0 ){
    return pDef;
  }

  /* Create a new ephemeral function definition for the overloaded
  ** function */
  pNew = sqlite3DbMallocZero(db, sizeof(*pNew) + strlen(pDef->zName) );
  if( pNew==0 ){
    return pDef;
  }
  *pNew = *pDef;
  memcpy(pNew->zName, pDef->zName, strlen(pDef->zName)+1);
  pNew->xFunc = xFunc;
  pNew->pUserData = pArg;
  pNew->flags |= SQLITE_FUNC_EPHEM;
  return pNew;
}

#endif /* SQLITE_OMIT_VIRTUALTABLE */
