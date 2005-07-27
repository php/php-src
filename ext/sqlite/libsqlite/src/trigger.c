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

/*
** Delete a linked list of TriggerStep structures.
*/
static void sqliteDeleteTriggerStep(TriggerStep *pTriggerStep){
  while( pTriggerStep ){
    TriggerStep * pTmp = pTriggerStep;
    pTriggerStep = pTriggerStep->pNext;

    if( pTmp->target.dyn ) sqliteFree((char*)pTmp->target.z);
    sqliteExprDelete(pTmp->pWhere);
    sqliteExprListDelete(pTmp->pExprList);
    sqliteSelectDelete(pTmp->pSelect);
    sqliteIdListDelete(pTmp->pIdList);

    sqliteFree(pTmp);
  }
}

/*
** This is called by the parser when it sees a CREATE TRIGGER statement. See
** comments surrounding struct Trigger in sqliteInt.h for a description of 
** how triggers are stored.
*/
void sqliteCreateTrigger(
  Parse *pParse,      /* The parse context of the CREATE TRIGGER statement */
  Token *pName,       /* The name of the trigger */
  int tr_tm,          /* One of TK_BEFORE, TK_AFTER , TK_INSTEAD */
  int op,             /* One of TK_INSERT, TK_UPDATE, TK_DELETE */
  IdList *pColumns,   /* column list if this is an UPDATE OF trigger */
  Token *pTableName,  /* The name of the table/view the trigger applies to */
  int foreach,        /* One of TK_ROW or TK_STATEMENT */
  Expr *pWhen,        /* WHEN clause */
  TriggerStep *pStepList, /* The triggered program */
  Token *pAll             /* Token that describes the complete CREATE TRIGGER */
){
  Trigger *nt;
  Table   *tab;
  char *zName = 0;    /* Name of the trigger */

  /* Check that: 
  ** 1. the trigger name does not already exist.
  ** 2. the table (or view) does exist.
  ** 3. that we are not trying to create a trigger on the sqlite_master table
  ** 4. That we are not trying to create an INSTEAD OF trigger on a table.
  ** 5. That we are not trying to create a BEFORE or AFTER trigger on a view.
  */
  zName = sqliteStrNDup(pName->z, pName->n);
  if( sqliteHashFind(&(pParse->db->trigHash), zName, pName->n + 1) ){
    sqliteSetNString(&pParse->zErrMsg, "trigger ", -1,
        pName->z, pName->n, " already exists", -1, 0);
    pParse->nErr++;
    goto trigger_cleanup;
  }
  {
    char *tmp_str = sqliteStrNDup(pTableName->z, pTableName->n);
    if( tmp_str==0 ) goto trigger_cleanup;
    tab = sqliteFindTable(pParse->db, tmp_str);
    sqliteFree(tmp_str);
    if( !tab ){
      sqliteSetNString(&pParse->zErrMsg, "no such table: ", -1,
          pTableName->z, pTableName->n, 0);
      pParse->nErr++;
      goto trigger_cleanup;
    }
    if( sqliteStrICmp(tab->zName, MASTER_NAME)==0 ){
      sqliteSetString(&pParse->zErrMsg, "cannot create trigger on system "
         "table: " MASTER_NAME, 0);
      pParse->nErr++;
      goto trigger_cleanup;
    }
    if( sqliteStrICmp(tab->zName, TEMP_MASTER_NAME)==0 ){
      sqliteSetString(&pParse->zErrMsg, "cannot create trigger on system "
         "table: " TEMP_MASTER_NAME, 0);
      pParse->nErr++;
      goto trigger_cleanup;
    }
    if( tab->pSelect && tr_tm != TK_INSTEAD ){
      sqliteSetNString(&pParse->zErrMsg, "cannot create ", -1,
	  (tr_tm == TK_BEFORE)?"BEFORE":"AFTER", -1, " trigger on view: ", -1
          , pTableName->z, pTableName->n, 0);
      goto trigger_cleanup;
    }
    if( !tab->pSelect && tr_tm == TK_INSTEAD ){
      sqliteSetNString(&pParse->zErrMsg, "cannot create INSTEAD OF", -1, 
	  " trigger on table: ", -1, pTableName->z, pTableName->n, 0);
      goto trigger_cleanup;
    }
#ifndef SQLITE_OMIT_AUTHORIZATION
    {
      int code = SQLITE_CREATE_TRIGGER;
      if( tab->isTemp ) code = SQLITE_CREATE_TEMP_TRIGGER;
      if( sqliteAuthCheck(pParse, code, zName, tab->zName) ){
        goto trigger_cleanup;
      }
      if( sqliteAuthCheck(pParse, SQLITE_INSERT, SCHEMA_TABLE(tab->isTemp), 0)){
        goto trigger_cleanup;
      }
    }
#endif
  }

  if (tr_tm == TK_INSTEAD){
    tr_tm = TK_BEFORE;
  }

  /* Build the Trigger object */
  nt = (Trigger*)sqliteMalloc(sizeof(Trigger));
  if( nt==0 ) goto trigger_cleanup;
  nt->name = zName;
  zName = 0;
  nt->table = sqliteStrNDup(pTableName->z, pTableName->n);
  if( sqlite_malloc_failed ) goto trigger_cleanup;
  nt->op = op;
  nt->tr_tm = tr_tm;
  nt->pWhen = sqliteExprDup(pWhen);
  sqliteExprDelete(pWhen);
  nt->pColumns = sqliteIdListDup(pColumns);
  sqliteIdListDelete(pColumns);
  nt->foreach = foreach;
  nt->step_list = pStepList;

  /* if we are not initializing, and this trigger is not on a TEMP table, 
  ** build the sqlite_master entry
  */
  if( !pParse->initFlag ){
    static VdbeOp insertTrig[] = {
      { OP_NewRecno,   0, 0,  0          },
      { OP_String,     0, 0,  "trigger"  },
      { OP_String,     0, 0,  0          },  /* 2: trigger name */
      { OP_String,     0, 0,  0          },  /* 3: table name */
      { OP_Integer,    0, 0,  0          },
      { OP_String,     0, 0,  0          },  /* 5: SQL */
      { OP_MakeRecord, 5, 0,  0          },
      { OP_PutIntKey,  0, 0,  0          },
    };
    int addr;
    Vdbe *v;

    /* Make an entry in the sqlite_master table */
    v = sqliteGetVdbe(pParse);
    if( v==0 ) goto trigger_cleanup;
    sqliteBeginWriteOperation(pParse, 0, 0);
    sqliteOpenMasterTable(v, tab->isTemp);
    addr = sqliteVdbeAddOpList(v, ArraySize(insertTrig), insertTrig);
    sqliteVdbeChangeP3(v, addr, tab->isTemp ? TEMP_MASTER_NAME : MASTER_NAME,
                       P3_STATIC);
    sqliteVdbeChangeP3(v, addr+2, nt->name, 0); 
    sqliteVdbeChangeP3(v, addr+3, nt->table, 0); 
    sqliteVdbeChangeP3(v, addr+5, pAll->z, pAll->n);
    if( !tab->isTemp ){
      sqliteChangeCookie(pParse->db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    sqliteEndWriteOperation(pParse);
  }

  if( !pParse->explain ){
    /* Stick it in the hash-table */
    sqliteHashInsert(&(pParse->db->trigHash), nt->name, pName->n + 1, nt);

    /* Attach it to the table object */
    nt->pNext = tab->pTrigger;
    tab->pTrigger = nt;
    return;
  }else{
    sqliteFree(nt->name);
    sqliteFree(nt->table);
    sqliteFree(nt);
  }

trigger_cleanup:

  sqliteFree(zName);
  sqliteIdListDelete(pColumns);
  sqliteExprDelete(pWhen);
  sqliteDeleteTriggerStep(pStepList);
}

/*
** Make a copy of all components of the given trigger step.  This has
** the effect of copying all Expr.token.z values into memory obtained
** from sqliteMalloc().  As initially created, the Expr.token.z values
** all point to the input string that was fed to the parser.  But that
** string is ephemeral - it will go away as soon as the sqlite_exec()
** call that started the parser exits.  This routine makes a persistent
** copy of all the Expr.token.z strings so that the TriggerStep structure
** will be valid even after the sqlite_exec() call returns.
*/
static void sqlitePersistTriggerStep(TriggerStep *p){
  if( p->target.z ){
    p->target.z = sqliteStrNDup(p->target.z, p->target.n);
    p->target.dyn = 1;
  }
  if( p->pSelect ){
    Select *pNew = sqliteSelectDup(p->pSelect);
    sqliteSelectDelete(p->pSelect);
    p->pSelect = pNew;
  }
  if( p->pWhere ){
    Expr *pNew = sqliteExprDup(p->pWhere);
    sqliteExprDelete(p->pWhere);
    p->pWhere = pNew;
  }
  if( p->pExprList ){
    ExprList *pNew = sqliteExprListDup(p->pExprList);
    sqliteExprListDelete(p->pExprList);
    p->pExprList = pNew;
  }
  if( p->pIdList ){
    IdList *pNew = sqliteIdListDup(p->pIdList);
    sqliteIdListDelete(p->pIdList);
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
TriggerStep *sqliteTriggerSelectStep(Select *pSelect){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ) return 0;

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
TriggerStep *sqliteTriggerInsertStep(
  Token *pTableName,  /* Name of the table into which we insert */
  IdList *pColumn,    /* List of columns in pTableName to insert into */
  ExprList *pEList,   /* The VALUE clause: a list of values to be inserted */
  Select *pSelect,    /* A SELECT statement that supplies values */
  int orconf          /* The conflict algorithm (OE_Abort, OE_Replace, etc.) */
){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ) return 0;

  assert(pEList == 0 || pSelect == 0);
  assert(pEList != 0 || pSelect != 0);

  pTriggerStep->op = TK_INSERT;
  pTriggerStep->pSelect = pSelect;
  pTriggerStep->target  = *pTableName;
  pTriggerStep->pIdList = pColumn;
  pTriggerStep->pExprList = pEList;
  pTriggerStep->orconf = orconf;
  sqlitePersistTriggerStep(pTriggerStep);

  return pTriggerStep;
}

/*
** Construct a trigger step that implements an UPDATE statement and return
** a pointer to that trigger step.  The parser calls this routine when it
** sees an UPDATE statement inside the body of a CREATE TRIGGER.
*/
TriggerStep *sqliteTriggerUpdateStep(
  Token *pTableName,   /* Name of the table to be updated */
  ExprList *pEList,    /* The SET clause: list of column and new values */
  Expr *pWhere,        /* The WHERE clause */
  int orconf           /* The conflict algorithm. (OE_Abort, OE_Ignore, etc) */
){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ) return 0;

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
TriggerStep *sqliteTriggerDeleteStep(Token *pTableName, Expr *pWhere){
  TriggerStep *pTriggerStep = sqliteMalloc(sizeof(TriggerStep));
  if( pTriggerStep==0 ) return 0;

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
void sqliteDeleteTrigger(Trigger *pTrigger){
  sqliteDeleteTriggerStep(pTrigger->step_list);
  sqliteFree(pTrigger->name);
  sqliteFree(pTrigger->table);
  sqliteExprDelete(pTrigger->pWhen);
  sqliteIdListDelete(pTrigger->pColumns);
  sqliteFree(pTrigger);
}

/*
 * This function is called to drop a trigger from the database schema. 
 *
 * This may be called directly from the parser, or from within 
 * sqliteDropTable(). In the latter case the "nested" argument is true.
 *
 * Note that this function does not delete the trigger entirely. Instead it
 * removes it from the internal schema and places it in the trigDrop hash 
 * table. This is so that the trigger can be restored into the database schema
 * if the transaction is rolled back.
 */
void sqliteDropTrigger(Parse *pParse, Token *pName, int nested){
  char *zName;
  Trigger *pTrigger;
  Table   *pTable;
  Vdbe *v;

  zName = sqliteStrNDup(pName->z, pName->n);

  /* ensure that the trigger being dropped exists */
  pTrigger = sqliteHashFind(&(pParse->db->trigHash), zName, pName->n + 1); 
  if( !pTrigger ){
    sqliteSetNString(&pParse->zErrMsg, "no such trigger: ", -1,
        zName, -1, 0);
    sqliteFree(zName);
    return;
  }
  pTable = sqliteFindTable(pParse->db, pTrigger->table);
  assert(pTable);
#ifndef SQLITE_OMIT_AUTHORIZATION
  {
    int code = SQLITE_DROP_TRIGGER;
    if( pTable->isTemp ) code = SQLITE_DROP_TEMP_TRIGGER;
    if( sqliteAuthCheck(pParse, code, pTrigger->name, pTable->zName) ||
      sqliteAuthCheck(pParse, SQLITE_DELETE, SCHEMA_TABLE(pTable->isTemp),0) ){
      sqliteFree(zName);
      return;
    }
  }
#endif

  /*
   * If this is not an "explain", then delete the trigger structure.
   */
  if( !pParse->explain ){
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
    sqliteHashInsert(&(pParse->db->trigHash), zName, pName->n + 1, NULL);
    sqliteDeleteTrigger(pTrigger);
  }

  /* Generate code to destroy the database record of the trigger.
  */
  if( pTable!=0 && !nested && (v = sqliteGetVdbe(pParse))!=0 ){
    int base;
    static VdbeOp dropTrigger[] = {
      { OP_Rewind,     0, ADDR(8),  0},
      { OP_String,     0, 0,        0}, /* 1 */
      { OP_MemStore,   1, 1,        0},
      { OP_MemLoad,    1, 0,        0}, /* 3 */
      { OP_Column,     0, 1,        0},
      { OP_Ne,         0, ADDR(7),  0},
      { OP_Delete,     0, 0,        0},
      { OP_Next,       0, ADDR(3),  0}, /* 7 */
    };

    sqliteBeginWriteOperation(pParse, 0, 0);
    sqliteOpenMasterTable(v, pTable->isTemp);
    base = sqliteVdbeAddOpList(v,  ArraySize(dropTrigger), dropTrigger);
    sqliteVdbeChangeP3(v, base+1, zName, 0);
    if( !pTable->isTemp ){
      sqliteChangeCookie(pParse->db, v);
    }
    sqliteVdbeAddOp(v, OP_Close, 0, 0);
    sqliteEndWriteOperation(pParse);
  }

  sqliteFree(zName);
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
    if( sqliteIdListIndex(pIdList, pEList->a[e].zName)>=0 ) return 1;
  }
  return 0; 
}

/* A global variable that is TRUE if we should always set up temp tables for
 * for triggers, even if there are no triggers to code. This is used to test 
 * how much overhead the triggers algorithm is causing.
 *
 * This flag can be set or cleared using the "trigger_overhead_test" pragma.
 * The pragma is not documented since it is not really part of the interface
 * to SQLite, just the test procedure.
*/
int always_code_trigger_setup = 0;

/*
 * Returns true if a trigger matching op, tr_tm and foreach that is NOT already
 * on the Parse objects trigger-stack (to prevent recursive trigger firing) is
 * found in the list specified as pTrigger.
 */
int sqliteTriggersExist(
  Parse *pParse,          /* Used to check for recursive triggers */
  Trigger *pTrigger,      /* A list of triggers associated with a table */
  int op,                 /* one of TK_DELETE, TK_INSERT, TK_UPDATE */
  int tr_tm,              /* one of TK_BEFORE, TK_AFTER */
  int foreach,            /* one of TK_ROW or TK_STATEMENT */
  ExprList *pChanges      /* Columns that change in an UPDATE statement */
){
  Trigger * pTriggerCursor;

  if( always_code_trigger_setup ){
    return 1;
  }

  pTriggerCursor = pTrigger;
  while( pTriggerCursor ){
    if( pTriggerCursor->op == op && 
	pTriggerCursor->tr_tm == tr_tm && 
	pTriggerCursor->foreach == foreach &&
	checkColumnOverLap(pTriggerCursor->pColumns, pChanges) ){
      TriggerStack * ss;
      ss = pParse->trigStack;
      while( ss && ss->pTrigger != pTrigger ){
	ss = ss->pNext;
      }
      if( !ss )return 1;
    }
    pTriggerCursor = pTriggerCursor->pNext;
  }

  return 0;
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

  while( pTriggerStep ){
    int saveNTab = pParse->nTab;
    orconf = (orconfin == OE_Default)?pTriggerStep->orconf:orconfin;
    pParse->trigStack->orconf = orconf;
    switch( pTriggerStep->op ){
      case TK_SELECT: {
	Select * ss = sqliteSelectDup(pTriggerStep->pSelect);		  
	assert(ss);
	assert(ss->pSrc);
	sqliteSelect(pParse, ss, SRT_Discard, 0, 0, 0, 0);
	sqliteSelectDelete(ss);
	break;
      }
      case TK_UPDATE: {
        sqliteVdbeAddOp(pParse->pVdbe, OP_ListPush, 0, 0);
        sqliteUpdate(pParse, &pTriggerStep->target, 
		sqliteExprListDup(pTriggerStep->pExprList), 
		sqliteExprDup(pTriggerStep->pWhere), orconf);
        sqliteVdbeAddOp(pParse->pVdbe, OP_ListPop, 0, 0);
        break;
      }
      case TK_INSERT: {
        sqliteInsert(pParse, &pTriggerStep->target, 
        sqliteExprListDup(pTriggerStep->pExprList), 
        sqliteSelectDup(pTriggerStep->pSelect), 
        sqliteIdListDup(pTriggerStep->pIdList), orconf);
        break;
      }
      case TK_DELETE: {
        sqliteVdbeAddOp(pParse->pVdbe, OP_ListPush, 0, 0);
        sqliteDeleteFrom(pParse, &pTriggerStep->target, 
	    sqliteExprDup(pTriggerStep->pWhere));
        sqliteVdbeAddOp(pParse->pVdbe, OP_ListPop, 0, 0);
        break;
      }
      default:
        assert(0);
    } 
    pParse->nTab = saveNTab;
    pTriggerStep = pTriggerStep->pNext;
  }

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
int sqliteCodeRowTrigger(
  Parse *pParse,       /* Parse context */
  int op,              /* One of TK_UPDATE, TK_INSERT, TK_DELETE */
  ExprList *pChanges,  /* Changes list for any UPDATE OF triggers */
  int tr_tm,           /* One of TK_BEFORE, TK_AFTER */
  Table *pTab,         /* The table to code triggers from */
  int newIdx,          /* The indice of the "new" row to access */
  int oldIdx,          /* The indice of the "old" row to access */
  int orconf,          /* ON CONFLICT policy */
  int ignoreJump       /* Instruction to jump to for RAISE(IGNORE) */
){
  Trigger * pTrigger;
  TriggerStack * pTriggerStack;

  assert(op == TK_UPDATE || op == TK_INSERT || op == TK_DELETE);
  assert(tr_tm == TK_BEFORE || tr_tm == TK_AFTER);

  assert(newIdx != -1 || oldIdx != -1);

  pTrigger = pTab->pTrigger;
  while( pTrigger ){
    int fire_this = 0;

    /* determine whether we should code this trigger */
    if( pTrigger->op == op && pTrigger->tr_tm == tr_tm && 
        pTrigger->foreach == TK_ROW ){
      fire_this = 1;
      pTriggerStack = pParse->trigStack;
      while( pTriggerStack ){
        if( pTriggerStack->pTrigger == pTrigger ){
	  fire_this = 0;
	}
        pTriggerStack = pTriggerStack->pNext;
      }
      if( op == TK_UPDATE && pTrigger->pColumns &&
          !checkColumnOverLap(pTrigger->pColumns, pChanges) ){
        fire_this = 0;
      }
    }

    if( fire_this && (pTriggerStack = sqliteMalloc(sizeof(TriggerStack)))!=0 ){
      int endTrigger;
      SrcList dummyTablist;
      Expr * whenExpr;

      dummyTablist.nSrc = 0;
      dummyTablist.a = 0;

      /* Push an entry on to the trigger stack */
      pTriggerStack->pTrigger = pTrigger;
      pTriggerStack->newIdx = newIdx;
      pTriggerStack->oldIdx = oldIdx;
      pTriggerStack->pTab = pTab;
      pTriggerStack->pNext = pParse->trigStack;
      pTriggerStack->ignoreJump = ignoreJump;
      pParse->trigStack = pTriggerStack;

      /* code the WHEN clause */
      endTrigger = sqliteVdbeMakeLabel(pParse->pVdbe);
      whenExpr = sqliteExprDup(pTrigger->pWhen);
      if( sqliteExprResolveIds(pParse, 0, &dummyTablist, 0, whenExpr) ){
        pParse->trigStack = pParse->trigStack->pNext;
        sqliteFree(pTriggerStack);
        sqliteExprDelete(whenExpr);
        return 1;
      }
      sqliteExprIfFalse(pParse, whenExpr, endTrigger, 1);
      sqliteExprDelete(whenExpr);

      codeTriggerProgram(pParse, pTrigger->step_list, orconf); 

      /* Pop the entry off the trigger stack */
      pParse->trigStack = pParse->trigStack->pNext;
      sqliteFree(pTriggerStack);

      sqliteVdbeResolveLabel(pParse->pVdbe, endTrigger);
    }
    pTrigger = pTrigger->pNext;
  }

  return 0;
}

/*
 * This function is called to code ON UPDATE and ON DELETE triggers on 
 * views. 
 *
 * This function deletes the data pointed at by the pWhere and pChanges
 * arguments before it completes.
 */
void sqliteViewTriggers(
  Parse *pParse, 
  Table *pTab,         /* The view to code triggers on */
  Expr *pWhere,        /* The WHERE clause of the statement causing triggers*/
  int orconf,          /* The ON CONFLICT policy specified as part of the
			  statement causing these triggers */
  ExprList *pChanges   /* If this is an statement causing triggers to fire
			  is an UPDATE, then this list holds the columns
			  to update and the expressions to update them to.
			  See comments for sqliteUpdate(). */
){
  int oldIdx = -1;
  int newIdx = -1;
  int *aXRef = 0;   
  Vdbe *v;
  int endOfLoop;
  int startOfLoop;
  Select theSelect;
  Token tblNameToken;

  assert(pTab->pSelect);

  tblNameToken.z = pTab->zName;
  tblNameToken.n = strlen(pTab->zName);

  theSelect.isDistinct = 0;
  theSelect.pEList = sqliteExprListAppend(0, sqliteExpr(TK_ALL, 0, 0, 0), 0);
  theSelect.pSrc   = sqliteSrcListAppend(0, &tblNameToken);
  theSelect.pWhere = pWhere;    pWhere = 0;
  theSelect.pGroupBy = 0;
  theSelect.pHaving = 0;
  theSelect.pOrderBy = 0;
  theSelect.op = TK_SELECT; /* ?? */
  theSelect.pPrior = 0;
  theSelect.nLimit = -1;
  theSelect.nOffset = -1;
  theSelect.zSelect = 0;
  theSelect.base = 0;

  v = sqliteGetVdbe(pParse);
  assert(v);
  sqliteBeginWriteOperation(pParse, 1, 0);

  /* Allocate temp tables */
  oldIdx = pParse->nTab++;
  sqliteVdbeAddOp(v, OP_OpenTemp, oldIdx, 0);
  if( pChanges ){
    newIdx = pParse->nTab++;
    sqliteVdbeAddOp(v, OP_OpenTemp, newIdx, 0);
  }

  /* Snapshot the view */
  if( sqliteSelect(pParse, &theSelect, SRT_Table, oldIdx, 0, 0, 0) ){
    goto trigger_cleanup;
  }

  /* loop thru the view snapshot, executing triggers for each row */
  endOfLoop = sqliteVdbeMakeLabel(v);
  sqliteVdbeAddOp(v, OP_Rewind, oldIdx, endOfLoop);

  /* Loop thru the view snapshot, executing triggers for each row */
  startOfLoop = sqliteVdbeCurrentAddr(v);

  /* Build the updated row if required */
  if( pChanges ){
    int ii;

    aXRef = sqliteMalloc( sizeof(int) * pTab->nCol );
    if( aXRef==0 ) goto trigger_cleanup;
    for(ii = 0; ii < pTab->nCol; ii++){
      aXRef[ii] = -1;
    }

    for(ii=0; ii<pChanges->nExpr; ii++){
      int jj;
      if( sqliteExprResolveIds(pParse, oldIdx, theSelect.pSrc , 0, 
            pChanges->a[ii].pExpr) ){
        goto trigger_cleanup;
      }

      if( sqliteExprCheck(pParse, pChanges->a[ii].pExpr, 0, 0) )
        goto trigger_cleanup;

      for(jj=0; jj<pTab->nCol; jj++){
        if( sqliteStrICmp(pTab->aCol[jj].zName, pChanges->a[ii].zName)==0 ){
          aXRef[jj] = ii;
          break;
        }
      }
      if( jj>=pTab->nCol ){
        sqliteSetString(&pParse->zErrMsg, "no such column: ", 
            pChanges->a[ii].zName, 0);
        pParse->nErr++;
        goto trigger_cleanup;
      }
    }

    sqliteVdbeAddOp(v, OP_Integer, 13, 0);

    for(ii = 0; ii<pTab->nCol; ii++){
      if( aXRef[ii] < 0 ){ 
        sqliteVdbeAddOp(v, OP_Column, oldIdx, ii);
      }else{
        sqliteExprCode(pParse, pChanges->a[aXRef[ii]].pExpr);
      }
    }

    sqliteVdbeAddOp(v, OP_MakeRecord, pTab->nCol, 0);
    sqliteVdbeAddOp(v, OP_PutIntKey, newIdx, 0);
    sqliteVdbeAddOp(v, OP_Rewind, newIdx, 0);

    sqliteCodeRowTrigger(pParse, TK_UPDATE, pChanges, TK_BEFORE, 
        pTab, newIdx, oldIdx, orconf, endOfLoop);
    sqliteCodeRowTrigger(pParse, TK_UPDATE, pChanges, TK_AFTER, 
        pTab, newIdx, oldIdx, orconf, endOfLoop);
  }else{
    sqliteCodeRowTrigger(pParse, TK_DELETE, 0, TK_BEFORE, pTab, -1, oldIdx, 
        orconf, endOfLoop);
    sqliteCodeRowTrigger(pParse, TK_DELETE, 0, TK_AFTER, pTab, -1, oldIdx, 
        orconf, endOfLoop);
  }

  sqliteVdbeAddOp(v, OP_Next, oldIdx, startOfLoop);

  sqliteVdbeResolveLabel(v, endOfLoop);
  sqliteEndWriteOperation(pParse);

trigger_cleanup:
  sqliteFree(aXRef);
  sqliteExprListDelete(pChanges);
  sqliteExprDelete(pWhere);
  sqliteExprListDelete(theSelect.pEList);
  sqliteSrcListDelete(theSelect.pSrc);
  sqliteExprDelete(theSelect.pWhere);
  return;
}
