/*
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
*
*/
#include "sqliteInt.h"

#ifndef SQLITE_OMIT_TRIGGER
/*
** Delete a linked list of TriggerStep structures.
*/
void sqlite3DeleteTriggerStep(TriggerStep *pTriggerStep){
  while( pTriggerStep ){
    TriggerStep * pTmp = pTriggerStep;
    pTriggerStep = pTriggerStep->pNext;

    if( pTmp->target.dyn ) sqliteFree((char*)pTmp->target.z);
    sqlite3ExprDelete(pTmp->pWhere);
    sqlite3ExprListDelete(pTmp->pExprList);
    sqlite3SelectDelete(pTmp->pSelect);
    sqlite3IdListDelete(pTmp->pIdList);

    sqliteFree(pTmp);
  }
}

/*
** This is called by the parser when it sees a CREATE TRIGGER statement
** up to the point of the BEGIN before the trigger actions.  A Trigger
** structure is generated based on the information available and stored
** in pParse->pNewTrigger.  After the trigger actions have been parsed, the
** sqlite3FinishTrigger() function is called to complete the trigger
** construction process.
*/
void sqlite3BeginTrigger(
  Parse *pParse,      /* The parse context of the CREATE TRIGGER statement */
  Token *pName1,      /* The name of the trigger */
  Token *pName2,      /* The name of the trigger */
  int tr_tm,          /* One of TK_BEFORE, TK_AFTER, TK_INSTEAD */
  int op,             /* One of TK_INSERT, TK_UPDATE, TK_DELETE */
  IdList *pColumns,   /* column list if this is an UPDATE OF trigger */
  SrcList *pTableName,/* The name of the table/view the trigger applies to */
  Expr *pWhen,        /* WHEN clause */
  int isTemp,         /* True if the TEMPORARY keyword is present */
  int noErr           /* Suppress errors if the trigger already exists */
){
  Trigger *pTrigger = 0;
  Table *pTab;
  char *zName = 0;        /* Name of the trigger */
  sqlite3 *db = pParse->db;
  int iDb;                /* The database to store the trigger in */
  Token *pName;           /* The unqualified db name */
  DbFixer sFix;
  int iTabDb;

  assert( pName1!=0 );   /* pName1->z might be NULL, but not pName1 itself */
  assert( pName2!=0 );
  if( isTemp ){
    /* If TEMP was specified, then the trigger name may not be qualified. */
    if( pName2->n>0 ){
      sqlite3ErrorMsg(pParse, "temporary trigger may not have qualified name");
      goto trigger_cleanup;
    }
    iDb = 1;
    pName = pName1;
  }else{
    /* Figure out the db that the the trigger will be created in */
    iDb = sqlite3TwoPartName(pParse, pName1, pName2, &pName);
    if( iDb<0 ){
      goto trigger_cleanup;
    }
  }

  /* If the trigger name was unqualified, and the table is a temp table,
  ** then set iDb to 1 to create the trigger in the temporary database.
  ** If sqlite3SrcListLookup() returns 0, indicating the table does not
  ** exist, the error is caught by the block below.
  */
  if( !pTableName || sqlite3MallocFailed() ){
    goto trigger_cleanup;
  }
  pTab = sqlite3SrcListLookup(pParse, pTableName);
  if( pName2->n==0 && pTab && pTab->pSchema==db->aDb[1].pSchema ){
    iDb = 1;
  }

  /* Ensure the table name matches database name and that the table exists */
  if( sqlite3MallocFailed() ) goto trigger_cleanup;
  assert( pTableName->nSrc==1 );
  if( sqlite3FixInit(&sFix, pParse, iDb, "trigger", pName) && 
      sqlite3FixSrcList(&sFix, pTableName) ){
    goto trigger_cleanup;
  }
  pTab = sqlite3SrcListLookup(pParse, pTableName);
  if( !pTab ){
    /* The table does not exist. */
    goto trigger_cleanup;
  }
  if( IsVirtual(pTab) ){
    sqlite3ErrorMsg(pParse, "cannot create triggers on virtual tables");
    goto trigger_cleanup;
  }

  /* Check that the trigger name is not reserved and that no trigger of the
  ** specified name exists */
  zName = sqlite3NameFromToken(pName);
  if( !zName || SQLITE_OK!=sqlite3CheckObjectName(pParse, zName) ){
    goto trigger_cleanup;
  }
  if( sqlite3HashFind(&(db->aDb[iDb].pSchema->trigHash), zName,strlen(zName)) ){
    if( !noErr ){
      sqlite3ErrorMsg(pParse, "trigger %T already exists", pName);
    }
    goto trigger_cleanup;
  }

  /* Do not create a trigger on a system table */
  if( sqlite3StrNICmp(pTab->zName, "sqlite_", 7)==0 ){
    sqlite3ErrorMsg(pParse, "cannot create trigger on system table");
    pParse->nErr++;
    goto trigger_cleanup;
  }

  /* INSTEAD of triggers are only for views and views only support INSTEAD
  ** of triggers.
  */
  if( pTab->pSelect && tr_tm!=TK_INSTEAD ){
    sqlite3ErrorMsg(pParse, "cannot create %s trigger on view: %S", 
        (tr_tm == TK_BEFORE)?"BEFORE":"AFTER", pTableName, 0);
    goto trigger_cleanup;
  }
  if( !pTab->pSelect && tr_tm==TK_INSTEAD ){
    sqlite3ErrorMsg(pParse, "cannot create INSTEAD OF"
        " trigger on table: %S", pTableName, 0);
    goto trigger_cleanup;
  }
  iTabDb = sqlite3SchemaToIndex(db, pTab->pSchema);

#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_CREATE_TRIGGER;
    const char *zDb = db->aDb[iTabDb].zName;
    const char *zDbTrig = isTemp ? db->aDb[1].zName : zDb;
    if( iTabDb==1 || isTemp ) code = SQLITE_CREATE_TEMP_TRIGGER;
    if( sqlite3AuthCheck(pParse, code, zName, pTab->zName, zDbTrig) ){
      goto trigger_cleanup;
    }
    if( sqlite3AuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(iTabDb),0,zDb)){
      goto trigger_cleanup;
    }
  }
#endif

  /* INSTEAD OF triggers can only appear on views and BEFORE triggers
  ** cannot appear on views.  So we might as well translate every
  ** INSTEAD OF trigger into a BEFORE trigger.  It simplifies code
  ** elsewhere.
  */
  if (tr_tm == TK_INSTEAD){
    tr_tm = TK_BEFORE;
  }

  /* Build the Trigger object */
  pTrigger = (Trigger*)sqliteMalloc(sizeof(Trigger));
  if( pTrigger==0 ) goto trigger_cleanup;
  pTrigger->name = zName;
  zName = 0;
  pTrigger->table = sqliteStrDup(pTableName->a[0].zName);
  pTrigger->pSchema = db->aDb[iDb].pSchema;
  pTrigger->pTabSchema = pTab->pSchema;
  pTrigger->op = op;
  pTrigger->tr_tm = tr_tm==TK_BEFORE ? TRIGGER_BEFORE : TRIGGER_AFTER;
  pTrigger->pWhen = sqlite3ExprDup(pWhen);
  pTrigger->pColumns = sqlite3IdListDup(pColumns);
  sqlite3TokenCopy(&pTrigger->nameToken,pName);
  assert( pParse->pNewTrigger==0 );
  pParse->pNewTrigger = pTrigger;

trigger_cleanup:
  sqliteFree(zName);
  sqlite3SrcListDelete(pTableName);
  sqlite3IdListDelete(pColumns);
  sqlite3ExprDelete(pWhen);
  if( !pParse->pNewTrigger ){
    sqlite3DeleteTrigger(pTrigger);
  }else{
    assert( pParse->pNewTrigger==pTrigger );
  }
}

/*
** This routine is called after all of the trigger actions have been parsed
** in order to complete the process of building the trigger.
*/
void sqlite3FinishTrigger(
  Parse *pParse,          /* Parser context */
  TriggerStep *pStepList, /* The triggered program */
  Token *pAll             /* Token that describes the complete CREATE TRIGGER */
){
  Trigger *pTrig = 0;     /* The trigger whose construction is finishing up */
  sqlite3 *db = pParse->db;  /* The database */
  DbFixer sFix;
  int iDb;                   /* Database containing the trigger */

  pTrig = pParse->pNewTrigger;
  pParse->pNewTrigger = 0;
  if( pParse->nErr || !pTrig ) goto triggerfinish_cleanup;
  iDb = sqlite3SchemaToIndex(pParse->db, pTrig->pSchema);
  pTrig->step_list = pStepList;
  while( pStepList ){
    pStepList->pTrig = pTrig;
    pStepList = pStepList->pNext;
  }
  if( sqlite3FixInit(&sFix, pParse, iDb, "trigger", &pTrig->nameToken) 
          && sqlite3FixTriggerStep(&sFix, pTrig->step_list) ){
    goto triggerfinish_cleanup;
  }

  /* if we are not initializing, and this trigger is not on a TEMP table, 
  ** build the sqlite_master entry
  */
  if( !db->init.busy ){
    static const VdbeOpList insertTrig[] = {
      { OP_NewRowid,   0, 0,  0          },
      { OP_String8,    0, 0,  "trigger"  },
      { OP_String8,    0, 0,  0          },  /* 2: trigger name */
      { OP_String8,    0, 0,  0          },  /* 3: table name */
      { OP_Integer,    0, 0,  0          },
      { OP_String8,    0, 0,  "CREATE TRIGGER "},
      { OP_String8,    0, 0,  0          },  /* 6: SQL */
      { OP_Concat,     0, 0,  0          }, 
      { OP_MakeRecord, 5, 0,  "aaada"    },
      { OP_Insert,     0, 0,  0          },
    };
    int addr;
    Vdbe *v;

    /* Make an entry in the sqlite_master table */
    v = sqlite3GetVdbe(pParse);
    if( v==0 ) goto triggerfinish_cleanup;
    sqlite3BeginWriteOperation(pParse, 0, iDb);
    sqlite3OpenMasterTable(pParse, iDb);
    addr = sqlite3VdbeAddOpList(v, ArraySize(insertTrig), insertTrig);
    sqlite3VdbeChangeP3(v, addr+2, pTrig->name, 0); 
    sqlite3VdbeChangeP3(v, addr+3, pTrig->table, 0); 
    sqlite3VdbeChangeP3(v, addr+6, (char*)pAll->z, pAll->n);
    sqlite3ChangeCookie(db, v, iDb);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
    sqlite3VdbeOp3(v, OP_ParseSchema, iDb, 0, 
       sqlite3MPrintf("type='trigger' AND name='%q'", pTrig->name), P3_DYNAMIC);
  }

  if( db->init.busy ){
    int n;
    Table *pTab;
    Trigger *pDel;
    pDel = sqlite3HashInsert(&db->aDb[iDb].pSchema->trigHash, 
                     pTrig->name, strlen(pTrig->name), pTrig);
    if( pDel ){
      assert( sqlite3MallocFailed() && pDel==pTrig );
      goto triggerfinish_cleanup;
    }
    n = strlen(pTrig->table) + 1;
    pTab = sqlite3HashFind(&pTrig->pTabSchema->tblHash, pTrig->table, n);
    assert( pTab!=0 );
    pTrig->pNext = pTab->pTrigger;
    pTab->pTrigger = pTrig;
    pTrig = 0;
  }

triggerfinish_cleanup:
  sqlite3DeleteTrigger(pTrig);
  assert( !pParse->pNewTrigger );
  sqlite3DeleteTriggerStep(pStepList);
}

/*
** Make a copy of all components of the given trigger step.  This has
** the effect of copying all Expr.token.z values into memory obtained
** from sqliteMalloc().  As initially created, the Expr.token.z values
** all point to the input string that was fed to the parser.  But that
** string is ephemeral - it will go away as soon as the sqlite3_exec()
** call that started the parser exits.  This routine makes a persistent
** copy of all the Expr.token.z strings so that the TriggerStep structure
** will be valid even after the sqlite3_exec() call returns.
*/
static void sqlitePersistTriggerStep(TriggerStep *p){
  if( p->target.z ){
    p->target.z = (u8*)sqliteStrNDup((char*)p->target.z, p->target.n);
    p->target.dyn = 1;
  }
  if( p->pSelect ){
    Select *pNew = sqlite3SelectDup(p->pSelect);
    sqlite3SelectDelete(p->pSelect);
    p->pSelect = pNew;
  }
  if( p->pWhere ){
    Expr *pNew = sqlite3ExprDup(p->pWhere);
    sqlite3ExprDelete(p->pWhere);
    p->pWhere = pNew;
  }
  if( p->pExprList ){
    ExprList *pNew = sqlite3ExprListDup(p->pExprList);
    sqlite3ExprListDelete(p->pExprList);
    p->pExprList = pNew;
  }
  if( p->pIdList ){
    IdList *pNew = sqlite3IdListDup(p->pIdList);
    sqlite3IdListDelete(p->pIdList);
    p->pIdList = pNew;
  }
}

/*
** Turn a SELECT statement (that the pSelect parameter points to) into
** a trigger step.  Return a pointer to a TriggerStep structure.
**
** The parser calls this routine when it finds a SELECT statement in
** body of a TRIGGER.  
*/
TriggerStep *sqlite3TriggerSelectStep(Select *pSelect){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ) {
    sqlite3SelectDelete(pSelect);
    return 0;
  }

  pTriggerStep->op = TK_SELECT;
  pTriggerStep->pSelect = pSelect;
  pTriggerStep->orconf = OE_Default;
  sqlitePersistTriggerStep(pTriggerStep);

  return pTriggerStep;
}

/*
** Build a trigger step out of an INSERT statement.  Return a pointer
** to the new trigger step.
**
** The parser calls this routine when it sees an INSERT inside the
** body of a trigger.
*/
TriggerStep *sqlite3TriggerInsertStep(
  Token *pTableName,  /* Name of the table into which we insert */
  IdList *pColumn,    /* List of columns in pTableName to insert into */
  ExprList *pEList,   /* The VALUE clause: a list of values to be inserted */
  Select *pSelect,    /* A SELECT statement that supplies values */
  int orconf          /* The conflict algorithm (OE_Abort, OE_Replace, etc.) */
){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));

  assert(pEList == 0 || pSelect == 0);
  assert(pEList != 0 || pSelect != 0);

  if( pTriggerStep ){
    pTriggerStep->op = TK_INSERT;
    pTriggerStep->pSelect = pSelect;
    pTriggerStep->target  = *pTableName;
    pTriggerStep->pIdList = pColumn;
    pTriggerStep->pExprList = pEList;
    pTriggerStep->orconf = orconf;
    sqlitePersistTriggerStep(pTriggerStep);
  }else{
    sqlite3IdListDelete(pColumn);
    sqlite3ExprListDelete(pEList);
    sqlite3SelectDup(pSelect);
  }

  return pTriggerStep;
}

/*
** Construct a trigger step that implements an UPDATE statement and return
** a pointer to that trigger step.  The parser calls this routine when it
** sees an UPDATE statement inside the body of a CREATE TRIGGER.
*/
TriggerStep *sqlite3TriggerUpdateStep(
  Token *pTableName,   /* Name of the table to be updated */
  ExprList *pEList,    /* The SET clause: list of column and new values */
  Expr *pWhere,        /* The WHERE clause */
  int orconf           /* The conflict algorithm. (OE_Abort, OE_Ignore, etc) */
){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ){
     sqlite3ExprListDelete(pEList);
     sqlite3ExprDelete(pWhere);
     return 0;
  }

  pTriggerStep->op = TK_UPDATE;
  pTriggerStep->target  = *pTableName;
  pTriggerStep->pExprList = pEList;
  pTriggerStep->pWhere = pWhere;
  pTriggerStep->orconf = orconf;
  sqlitePersistTriggerStep(pTriggerStep);

  return pTriggerStep;
}

/*
** Construct a trigger step that implements a DELETE statement and return
** a pointer to that trigger step.  The parser calls this routine when it
** sees a DELETE statement inside the body of a CREATE TRIGGER.
*/
TriggerStep *sqlite3TriggerDeleteStep(Token *pTableName, Expr *pWhere){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ){
    sqlite3ExprDelete(pWhere);
    return 0;
  }

  pTriggerStep->op = TK_DELETE;
  pTriggerStep->target  = *pTableName;
  pTriggerStep->pWhere = pWhere;
  pTriggerStep->orconf = OE_Default;
  sqlitePersistTriggerStep(pTriggerStep);

  return pTriggerStep;
}

/* 
** Recursively delete a Trigger structure
*/
void sqlite3DeleteTrigger(Trigger *pTrigger){
  if( pTrigger==0 ) return;
  sqlite3DeleteTriggerStep(pTrigger->step_list);
  sqliteFree(pTrigger->name);
  sqliteFree(pTrigger->table);
  sqlite3ExprDelete(pTrigger->pWhen);
  sqlite3IdListDelete(pTrigger->pColumns);
  if( pTrigger->nameToken.dyn ) sqliteFree((char*)pTrigger->nameToken.z);
  sqliteFree(pTrigger);
}

/*
** This function is called to drop a trigger from the database schema. 
**
** This may be called directly from the parser and therefore identifies
** the trigger by name.  The sqlite3DropTriggerPtr() routine does the
** same job as this routine except it takes a pointer to the trigger
** instead of the trigger name.
**/
void sqlite3DropTrigger(Parse *pParse, SrcList *pName, int noErr){
  Trigger *pTrigger = 0;
  int i;
  const char *zDb;
  const char *zName;
  int nName;
  sqlite3 *db = pParse->db;

  if( sqlite3MallocFailed() ) goto drop_trigger_cleanup;
  if( SQLITE_OK!=sqlite3ReadSchema(pParse) ){
    goto drop_trigger_cleanup;
  }

  assert( pName->nSrc==1 );
  zDb = pName->a[0].zDatabase;
  zName = pName->a[0].zName;
  nName = strlen(zName);
  for(i=OMIT_TEMPDB; i<db->nDb; i++){
    int j = (i<2) ? i^1 : i;  /* Search TEMP before MAIN */
    if( zDb && sqlite3StrICmp(db->aDb[j].zName, zDb) ) continue;
    pTrigger = sqlite3HashFind(&(db->aDb[j].pSchema->trigHash), zName, nName);
    if( pTrigger ) break;
  }
  if( !pTrigger ){
    if( !noErr ){
      sqlite3ErrorMsg(pParse, "no such trigger: %S", pName, 0);
    }
    goto drop_trigger_cleanup;
  }
  sqlite3DropTriggerPtr(pParse, pTrigger);

drop_trigger_cleanup:
  sqlite3SrcListDelete(pName);
}

/*
** Return a pointer to the Table structure for the table that a trigger
** is set on.
*/
static Table *tableOfTrigger(Trigger *pTrigger){
  int n = strlen(pTrigger->table) + 1;
  return sqlite3HashFind(&pTrigger->pTabSchema->tblHash, pTrigger->table, n);
}


/*
** Drop a trigger given a pointer to that trigger. 
*/
void sqlite3DropTriggerPtr(Parse *pParse, Trigger *pTrigger){
  Table   *pTable;
  Vdbe *v;
  sqlite3 *db = pParse->db;
  int iDb;

  iDb = sqlite3SchemaToIndex(pParse->db, pTrigger->pSchema);
  assert( iDb>=0 && iDb<db->nDb );
  pTable = tableOfTrigger(pTrigger);
  assert( pTable );
  assert( pTable->pSchema==pTrigger->pSchema || iDb==1 );
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_DROP_TRIGGER;
    const char *zDb = db->aDb[iDb].zName;
    const char *zTab = SCHEMA_TABLE(iDb);
    if( iDb==1 ) code = SQLITE_DROP_TEMP_TRIGGER;
    if( sqlite3AuthCheck(pParse, code, pTrigger->name, pTable->zName, zDb) ||
      sqlite3AuthCheck(pParse, SQLITE_DELETE, zTab, 0, zDb) ){
      return;
    }
  }
#endif

  /* Generate code to destroy the database record of the trigger.
  */
  assert( pTable!=0 );
  if( (v = sqlite3GetVdbe(pParse))!=0 ){
    int base;
    static const VdbeOpList dropTrigger[] = {
      { OP_Rewind,     0, ADDR(9),  0},
      { OP_String8,    0, 0,        0}, /* 1 */
      { OP_Column,     0, 1,        0},
      { OP_Ne,         0, ADDR(8),  0},
      { OP_String8,    0, 0,        "trigger"},
      { OP_Column,     0, 0,        0},
      { OP_Ne,         0, ADDR(8),  0},
      { OP_Delete,     0, 0,        0},
      { OP_Next,       0, ADDR(1),  0}, /* 8 */
    };

    sqlite3BeginWriteOperation(pParse, 0, iDb);
    sqlite3OpenMasterTable(pParse, iDb);
    base = sqlite3VdbeAddOpList(v,  ArraySize(dropTrigger), dropTrigger);
    sqlite3VdbeChangeP3(v, base+1, pTrigger->name, 0);
    sqlite3ChangeCookie(db, v, iDb);
    sqlite3VdbeAddOp(v, OP_Close, 0, 0);
    sqlite3VdbeOp3(v, OP_DropTrigger, iDb, 0, pTrigger->name, 0);
  }
}

/*
** Remove a trigger from the hash tables of the sqlite* pointer.
*/
void sqlite3UnlinkAndDeleteTrigger(sqlite3 *db, int iDb, const char *zName){
  Trigger *pTrigger;
  int nName = strlen(zName);
  pTrigger = sqlite3HashInsert(&(db->aDb[iDb].pSchema->trigHash),
                               zName, nName, 0);
  if( pTrigger ){
    Table *pTable = tableOfTrigger(pTrigger);
    assert( pTable!=0 );
    if( pTable->pTrigger == pTrigger ){
      pTable->pTrigger = pTrigger->pNext;
    }else{
      Trigger *cc = pTable->pTrigger;
      while( cc ){ 
        if( cc->pNext == pTrigger ){
          cc->pNext = cc->pNext->pNext;
          break;
        }
        cc = cc->pNext;
      }
      assert(cc);
    }
    sqlite3DeleteTrigger(pTrigger);
    db->flags |= SQLITE_InternChanges;
  }
}

/*
** pEList is the SET clause of an UPDATE statement.  Each entry
** in pEList is of the format <id>=<expr>.  If any of the entries
** in pEList have an <id> which matches an identifier in pIdList,
** then return TRUE.  If pIdList==NULL, then it is considered a
** wildcard that matches anything.  Likewise if pEList==NULL then
** it matches anything so always return true.  Return false only
** if there is no match.
*/
static int checkColumnOverLap(IdList *pIdList, ExprList *pEList){
  int e;
  if( !pIdList || !pEList ) return 1;
  for(e=0; e<pEList->nExpr; e++){
    if( sqlite3IdListIndex(pIdList, pEList->a[e].zName)>=0 ) return 1;
  }
  return 0; 
}

/*
** Return a bit vector to indicate what kind of triggers exist for operation
** "op" on table pTab.  If pChanges is not NULL then it is a list of columns
** that are being updated.  Triggers only match if the ON clause of the
** trigger definition overlaps the set of columns being updated.
**
** The returned bit vector is some combination of TRIGGER_BEFORE and
** TRIGGER_AFTER.
*/
int sqlite3TriggersExist(
  Parse *pParse,          /* Used to check for recursive triggers */
  Table *pTab,            /* The table the contains the triggers */
  int op,                 /* one of TK_DELETE, TK_INSERT, TK_UPDATE */
  ExprList *pChanges      /* Columns that change in an UPDATE statement */
){
  Trigger *pTrigger;
  int mask = 0;

  pTrigger = IsVirtual(pTab) ? 0 : pTab->pTrigger;
  while( pTrigger ){
    if( pTrigger->op==op && checkColumnOverLap(pTrigger->pColumns, pChanges) ){
      mask |= pTrigger->tr_tm;
    }
    pTrigger = pTrigger->pNext;
  }
  return mask;
}

/*
** Convert the pStep->target token into a SrcList and return a pointer
** to that SrcList.
**
** This routine adds a specific database name, if needed, to the target when
** forming the SrcList.  This prevents a trigger in one database from
** referring to a target in another database.  An exception is when the
** trigger is in TEMP in which case it can refer to any other database it
** wants.
*/
static SrcList *targetSrcList(
  Parse *pParse,       /* The parsing context */
  TriggerStep *pStep   /* The trigger containing the target token */
){
  Token sDb;           /* Dummy database name token */
  int iDb;             /* Index of the database to use */
  SrcList *pSrc;       /* SrcList to be returned */

  iDb = sqlite3SchemaToIndex(pParse->db, pStep->pTrig->pSchema);
  if( iDb==0 || iDb>=2 ){
    assert( iDb<pParse->db->nDb );
    sDb.z = (u8*)pParse->db->aDb[iDb].zName;
    sDb.n = strlen((char*)sDb.z);
    pSrc = sqlite3SrcListAppend(0, &sDb, &pStep->target);
  } else {
    pSrc = sqlite3SrcListAppend(0, &pStep->target, 0);
  }
  return pSrc;
}

/*
** Generate VDBE code for zero or more statements inside the body of a
** trigger.  
*/
static int codeTriggerProgram(
  Parse *pParse,            /* The parser context */
  TriggerStep *pStepList,   /* List of statements inside the trigger body */
  int orconfin              /* Conflict algorithm. (OE_Abort, etc) */  
){
  TriggerStep * pTriggerStep = pStepList;
  int orconf;
  Vdbe *v = pParse->pVdbe;

  assert( pTriggerStep!=0 );
  assert( v!=0 );
  sqlite3VdbeAddOp(v, OP_ContextPush, 0, 0);
  VdbeComment((v, "# begin trigger %s", pStepList->pTrig->name));
  while( pTriggerStep ){
    orconf = (orconfin == OE_Default)?pTriggerStep->orconf:orconfin;
    pParse->trigStack->orconf = orconf;
    switch( pTriggerStep->op ){
      case TK_SELECT: {
	Select *ss = sqlite3SelectDup(pTriggerStep->pSelect);
        if( ss ){
          sqlite3SelectResolve(pParse, ss, 0);
          sqlite3Select(pParse, ss, SRT_Discard, 0, 0, 0, 0, 0);
          sqlite3SelectDelete(ss);
        }
	break;
      }
      case TK_UPDATE: {
        SrcList *pSrc;
        pSrc = targetSrcList(pParse, pTriggerStep);
        sqlite3VdbeAddOp(v, OP_ResetCount, 0, 0);
        sqlite3Update(pParse, pSrc,
		sqlite3ExprListDup(pTriggerStep->pExprList), 
		sqlite3ExprDup(pTriggerStep->pWhere), orconf);
        sqlite3VdbeAddOp(v, OP_ResetCount, 1, 0);
        break;
      }
      case TK_INSERT: {
        SrcList *pSrc;
        pSrc = targetSrcList(pParse, pTriggerStep);
        sqlite3VdbeAddOp(v, OP_ResetCount, 0, 0);
        sqlite3Insert(pParse, pSrc,
          sqlite3ExprListDup(pTriggerStep->pExprList), 
          sqlite3SelectDup(pTriggerStep->pSelect), 
          sqlite3IdListDup(pTriggerStep->pIdList), orconf);
        sqlite3VdbeAddOp(v, OP_ResetCount, 1, 0);
        break;
      }
      case TK_DELETE: {
        SrcList *pSrc;
        sqlite3VdbeAddOp(v, OP_ResetCount, 0, 0);
        pSrc = targetSrcList(pParse, pTriggerStep);
        sqlite3DeleteFrom(pParse, pSrc, sqlite3ExprDup(pTriggerStep->pWhere));
        sqlite3VdbeAddOp(v, OP_ResetCount, 1, 0);
        break;
      }
      default:
        assert(0);
    } 
    pTriggerStep = pTriggerStep->pNext;
  }
  sqlite3VdbeAddOp(v, OP_ContextPop, 0, 0);
  VdbeComment((v, "# end trigger %s", pStepList->pTrig->name));

  return 0;
}

/*
** This is called to code FOR EACH ROW triggers.
**
** When the code that this function generates is executed, the following 
** must be true:
**
** 1. No cursors may be open in the main database.  (But newIdx and oldIdx
**    can be indices of cursors in temporary tables.  See below.)
**
** 2. If the triggers being coded are ON INSERT or ON UPDATE triggers, then
**    a temporary vdbe cursor (index newIdx) must be open and pointing at
**    a row containing values to be substituted for new.* expressions in the
**    trigger program(s).
**
** 3. If the triggers being coded are ON DELETE or ON UPDATE triggers, then
**    a temporary vdbe cursor (index oldIdx) must be open and pointing at
**    a row containing values to be substituted for old.* expressions in the
**    trigger program(s).
**
*/
int sqlite3CodeRowTrigger(
  Parse *pParse,       /* Parse context */
  int op,              /* One of TK_UPDATE, TK_INSERT, TK_DELETE */
  ExprList *pChanges,  /* Changes list for any UPDATE OF triggers */
  int tr_tm,           /* One of TRIGGER_BEFORE, TRIGGER_AFTER */
  Table *pTab,         /* The table to code triggers from */
  int newIdx,          /* The indice of the "new" row to access */
  int oldIdx,          /* The indice of the "old" row to access */
  int orconf,          /* ON CONFLICT policy */
  int ignoreJump       /* Instruction to jump to for RAISE(IGNORE) */
){
  Trigger *p;
  TriggerStack trigStackEntry;

  assert(op == TK_UPDATE || op == TK_INSERT || op == TK_DELETE);
  assert(tr_tm == TRIGGER_BEFORE || tr_tm == TRIGGER_AFTER );

  assert(newIdx != -1 || oldIdx != -1);

  for(p=pTab->pTrigger; p; p=p->pNext){
    int fire_this = 0;

    /* Determine whether we should code this trigger */
    if( 
      p->op==op && 
      p->tr_tm==tr_tm && 
      (p->pSchema==p->pTabSchema || p->pSchema==pParse->db->aDb[1].pSchema) &&
      (op!=TK_UPDATE||!p->pColumns||checkColumnOverLap(p->pColumns,pChanges))
    ){
      TriggerStack *pS;      /* Pointer to trigger-stack entry */
      for(pS=pParse->trigStack; pS && p!=pS->pTrigger; pS=pS->pNext){}
      if( !pS ){
        fire_this = 1;
      }
#if 0    /* Give no warning for recursive triggers.  Just do not do them */
      else{
        sqlite3ErrorMsg(pParse, "recursive triggers not supported (%s)",
            p->name);
        return SQLITE_ERROR;
      }
#endif
    }
 
    if( fire_this ){
      int endTrigger;
      Expr * whenExpr;
      AuthContext sContext;
      NameContext sNC;

      memset(&sNC, 0, sizeof(sNC));
      sNC.pParse = pParse;

      /* Push an entry on to the trigger stack */
      trigStackEntry.pTrigger = p;
      trigStackEntry.newIdx = newIdx;
      trigStackEntry.oldIdx = oldIdx;
      trigStackEntry.pTab = pTab;
      trigStackEntry.pNext = pParse->trigStack;
      trigStackEntry.ignoreJump = ignoreJump;
      pParse->trigStack = &trigStackEntry;
      sqlite3AuthContextPush(pParse, &sContext, p->name);

      /* code the WHEN clause */
      endTrigger = sqlite3VdbeMakeLabel(pParse->pVdbe);
      whenExpr = sqlite3ExprDup(p->pWhen);
      if( sqlite3ExprResolveNames(&sNC, whenExpr) ){
        pParse->trigStack = trigStackEntry.pNext;
        sqlite3ExprDelete(whenExpr);
        return 1;
      }
      sqlite3ExprIfFalse(pParse, whenExpr, endTrigger, 1);
      sqlite3ExprDelete(whenExpr);

      codeTriggerProgram(pParse, p->step_list, orconf); 

      /* Pop the entry off the trigger stack */
      pParse->trigStack = trigStackEntry.pNext;
      sqlite3AuthContextPop(&sContext);

      sqlite3VdbeResolveLabel(pParse->pVdbe, endTrigger);
    }
  }
  return 0;
}
#endif /* !defined(SQLITE_OMIT_TRIGGER) */
