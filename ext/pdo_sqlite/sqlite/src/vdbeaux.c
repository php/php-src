/*
** 2003 September 6
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This file contains code used for creating, destroying, and populating
** a VDBE (or an "sqlite3_stmt" as it is known to the outside world.)  Prior
** to version 2.8.7, all this code was combined into the vdbe.c source file.
** But that file was getting too big so this subroutines were split out.
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include "vdbeInt.h"


/*
** When debugging the code generator in a symbolic debugger, one can
** set the sqlite3_vdbe_addop_trace to 1 and all opcodes will be printed
** as they are added to the instruction stream.
*/
#ifndef NDEBUG
int sqlite3_vdbe_addop_trace = 0;
#endif


/*
** Create a new virtual database engine.
*/
Vdbe *sqlite3VdbeCreate(sqlite3 *db){
  Vdbe *p;
  p = sqliteMalloc( sizeof(Vdbe) );
  if( p==0 ) return 0;
  p->db = db;
  if( db->pVdbe ){
    db->pVdbe->pPrev = p;
  }
  p->pNext = db->pVdbe;
  p->pPrev = 0;
  db->pVdbe = p;
  p->magic = VDBE_MAGIC_INIT;
  return p;
}

/*
** Turn tracing on or off
*/
void sqlite3VdbeTrace(Vdbe *p, FILE *trace){
  p->trace = trace;
}

/*
** Resize the Vdbe.aOp array so that it contains at least N
** elements.
*/
static void resizeOpArray(Vdbe *p, int N){
  if( p->nOpAlloc<N ){
    int oldSize = p->nOpAlloc;
    p->nOpAlloc = N+100;
    p->aOp = sqliteRealloc(p->aOp, p->nOpAlloc*sizeof(Op));
    if( p->aOp ){
      memset(&p->aOp[oldSize], 0, (p->nOpAlloc-oldSize)*sizeof(Op));
    }
  }
}

/*
** Add a new instruction to the list of instructions current in the
** VDBE.  Return the address of the new instruction.
**
** Parameters:
**
**    p               Pointer to the VDBE
**
**    op              The opcode for this instruction
**
**    p1, p2          First two of the three possible operands.
**
** Use the sqlite3VdbeResolveLabel() function to fix an address and
** the sqlite3VdbeChangeP3() function to change the value of the P3
** operand.
*/
int sqlite3VdbeAddOp(Vdbe *p, int op, int p1, int p2){
  int i;
  VdbeOp *pOp;

  i = p->nOp;
  p->nOp++;
  assert( p->magic==VDBE_MAGIC_INIT );
  resizeOpArray(p, i+1);
  if( p->aOp==0 ){
    return 0;
  }
  pOp = &p->aOp[i];
  pOp->opcode = op;
  pOp->p1 = p1;
  pOp->p2 = p2;
  pOp->p3 = 0;
  pOp->p3type = P3_NOTUSED;
#ifndef NDEBUG
  if( sqlite3_vdbe_addop_trace ) sqlite3VdbePrintOp(0, i, &p->aOp[i]);
#endif
  return i;
}

/*
** Add an opcode that includes the p3 value.
*/
int sqlite3VdbeOp3(Vdbe *p, int op, int p1, int p2, const char *zP3,int p3type){
  int addr = sqlite3VdbeAddOp(p, op, p1, p2);
  sqlite3VdbeChangeP3(p, addr, zP3, p3type);
  return addr;
}

/*
** Create a new symbolic label for an instruction that has yet to be
** coded.  The symbolic label is really just a negative number.  The
** label can be used as the P2 value of an operation.  Later, when
** the label is resolved to a specific address, the VDBE will scan
** through its operation list and change all values of P2 which match
** the label into the resolved address.
**
** The VDBE knows that a P2 value is a label because labels are
** always negative and P2 values are suppose to be non-negative.
** Hence, a negative P2 value is a label that has yet to be resolved.
**
** Zero is returned if a malloc() fails.
*/
int sqlite3VdbeMakeLabel(Vdbe *p){
  int i;
  i = p->nLabel++;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( i>=p->nLabelAlloc ){
    p->nLabelAlloc = p->nLabelAlloc*2 + 10;
    p->aLabel = sqliteRealloc( p->aLabel, p->nLabelAlloc*sizeof(p->aLabel[0]));
  }
  if( p->aLabel ){
    p->aLabel[i] = -1;
  }
  return -1-i;
}

/*
** Resolve label "x" to be the address of the next instruction to
** be inserted.  The parameter "x" must have been obtained from
** a prior call to sqlite3VdbeMakeLabel().
*/
void sqlite3VdbeResolveLabel(Vdbe *p, int x){
  int j = -1-x;
  assert( p->magic==VDBE_MAGIC_INIT );
  assert( j>=0 && j<p->nLabel );
  if( p->aLabel ){
    p->aLabel[j] = p->nOp;
  }
}

/*
** Loop through the program looking for P2 values that are negative.
** Each such value is a label.  Resolve the label by setting the P2
** value to its correct non-zero value.
**
** This routine is called once after all opcodes have been inserted.
*/
static void resolveP2Values(Vdbe *p){
  int i;
  Op *pOp;
  int *aLabel = p->aLabel;
  if( aLabel==0 ) return;
  for(pOp=p->aOp, i=p->nOp-1; i>=0; i--, pOp++){
    if( pOp->p2>=0 ) continue;
    assert( -1-pOp->p2<p->nLabel );
    pOp->p2 = aLabel[-1-pOp->p2];
  }
  sqliteFree(p->aLabel);
  p->aLabel = 0;
}

/*
** Return the address of the next instruction to be inserted.
*/
int sqlite3VdbeCurrentAddr(Vdbe *p){
  assert( p->magic==VDBE_MAGIC_INIT );
  return p->nOp;
}

/*
** Add a whole list of operations to the operation stack.  Return the
** address of the first operation added.
*/
int sqlite3VdbeAddOpList(Vdbe *p, int nOp, VdbeOpList const *aOp){
  int addr;
  assert( p->magic==VDBE_MAGIC_INIT );
  resizeOpArray(p, p->nOp + nOp);
  if( p->aOp==0 ){
    return 0;
  }
  addr = p->nOp;
  if( nOp>0 ){
    int i;
    VdbeOpList const *pIn = aOp;
    for(i=0; i<nOp; i++, pIn++){
      int p2 = pIn->p2;
      VdbeOp *pOut = &p->aOp[i+addr];
      pOut->opcode = pIn->opcode;
      pOut->p1 = pIn->p1;
      pOut->p2 = p2<0 ? addr + ADDR(p2) : p2;
      pOut->p3 = pIn->p3;
      pOut->p3type = pIn->p3 ? P3_STATIC : P3_NOTUSED;
#ifndef NDEBUG
      if( sqlite3_vdbe_addop_trace ){
        sqlite3VdbePrintOp(0, i+addr, &p->aOp[i+addr]);
      }
#endif
    }
    p->nOp += nOp;
  }
  return addr;
}

/*
** Change the value of the P1 operand for a specific instruction.
** This routine is useful when a large program is loaded from a
** static array using sqlite3VdbeAddOpList but we want to make a
** few minor changes to the program.
*/
void sqlite3VdbeChangeP1(Vdbe *p, int addr, int val){
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p1 = val;
  }
}

/*
** Change the value of the P2 operand for a specific instruction.
** This routine is useful for setting a jump destination.
*/
void sqlite3VdbeChangeP2(Vdbe *p, int addr, int val){
  assert( val>=0 );
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p2 = val;
  }
}

/*
** Change the value of the P3 operand for a specific instruction.
** This routine is useful when a large program is loaded from a
** static array using sqlite3VdbeAddOpList but we want to make a
** few minor changes to the program.
**
** If n>=0 then the P3 operand is dynamic, meaning that a copy of
** the string is made into memory obtained from sqliteMalloc().
** A value of n==0 means copy bytes of zP3 up to and including the
** first null byte.  If n>0 then copy n+1 bytes of zP3.
**
** If n==P3_STATIC  it means that zP3 is a pointer to a constant static
** string and we can just copy the pointer.  n==P3_POINTER means zP3 is
** a pointer to some object other than a string.  n==P3_COLLSEQ and
** n==P3_KEYINFO mean that zP3 is a pointer to a CollSeq or KeyInfo
** structure.  A copy is made of KeyInfo structures into memory obtained
** from sqliteMalloc.
**
** If addr<0 then change P3 on the most recently inserted instruction.
*/
void sqlite3VdbeChangeP3(Vdbe *p, int addr, const char *zP3, int n){
  Op *pOp;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p==0 || p->aOp==0 ) return;
  if( addr<0 || addr>=p->nOp ){
    addr = p->nOp - 1;
    if( addr<0 ) return;
  }
  pOp = &p->aOp[addr];
  if( pOp->p3 && pOp->p3type==P3_DYNAMIC ){
    sqliteFree(pOp->p3);
    pOp->p3 = 0;
  }
  if( zP3==0 ){
    pOp->p3 = 0;
    pOp->p3type = P3_NOTUSED;
  }else if( n==P3_KEYINFO ){
    KeyInfo *pKeyInfo;
    int nField, nByte;
    nField = ((KeyInfo*)zP3)->nField;
    nByte = sizeof(*pKeyInfo) + (nField-1)*sizeof(pKeyInfo->aColl[0]);
    pKeyInfo = sqliteMallocRaw( nByte );
    pOp->p3 = (char*)pKeyInfo;
    if( pKeyInfo ){
      memcpy(pKeyInfo, zP3, nByte);
      pOp->p3type = P3_KEYINFO;
    }else{
      pOp->p3type = P3_NOTUSED;
    }
  }else if( n==P3_KEYINFO_HANDOFF ){
    pOp->p3 = (char*)zP3;
    pOp->p3type = P3_KEYINFO;
  }else if( n<0 ){
    pOp->p3 = (char*)zP3;
    pOp->p3type = n;
  }else{
    if( n==0 ) n = strlen(zP3);
    pOp->p3 = sqliteStrNDup(zP3, n);
    pOp->p3type = P3_DYNAMIC;
  }
}

#ifndef NDEBUG
/*
** Replace the P3 field of the most recently coded instruction with
** comment text.
*/
void sqlite3VdbeComment(Vdbe *p, const char *zFormat, ...){
  va_list ap;
  assert( p->nOp>0 );
  assert( p->aOp==0 || p->aOp[p->nOp-1].p3==0 );
  va_start(ap, zFormat);
  sqlite3VdbeChangeP3(p, -1, sqlite3VMPrintf(zFormat, ap), P3_DYNAMIC);
  va_end(ap);
}
#endif

/*
** If the P3 operand to the specified instruction appears
** to be a quoted string token, then this procedure removes 
** the quotes.
**
** The quoting operator can be either a grave ascent (ASCII 0x27)
** or a double quote character (ASCII 0x22).  Two quotes in a row
** resolve to be a single actual quote character within the string.
*/
void sqlite3VdbeDequoteP3(Vdbe *p, int addr){
  Op *pOp;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->aOp==0 ) return;
  if( addr<0 || addr>=p->nOp ){
    addr = p->nOp - 1;
    if( addr<0 ) return;
  }
  pOp = &p->aOp[addr];
  if( pOp->p3==0 || pOp->p3[0]==0 ) return;
  if( pOp->p3type==P3_STATIC ){
    pOp->p3 = sqliteStrDup(pOp->p3);
    pOp->p3type = P3_DYNAMIC;
  }
  assert( pOp->p3type==P3_DYNAMIC );
  sqlite3Dequote(pOp->p3);
}

/*
** Search the current program starting at instruction addr for the given
** opcode and P2 value.  Return the address plus 1 if found and 0 if not
** found.
*/
int sqlite3VdbeFindOp(Vdbe *p, int addr, int op, int p2){
  int i;
  assert( p->magic==VDBE_MAGIC_INIT );
  for(i=addr; i<p->nOp; i++){
    if( p->aOp[i].opcode==op && p->aOp[i].p2==p2 ) return i+1;
  }
  return 0;
}

/*
** Return the opcode for a given address.
*/
VdbeOp *sqlite3VdbeGetOp(Vdbe *p, int addr){
  assert( p->magic==VDBE_MAGIC_INIT );
  assert( addr>=0 && addr<p->nOp );
  return &p->aOp[addr];
}

/*
** Compute a string that describes the P3 parameter for an opcode.
** Use zTemp for any required temporary buffer space.
*/
static char *displayP3(Op *pOp, char *zTemp, int nTemp){
  char *zP3;
  assert( nTemp>=20 );
  switch( pOp->p3type ){
    case P3_POINTER: {
      sprintf(zTemp, "ptr(%#x)", (int)pOp->p3);
      zP3 = zTemp;
      break;
    }
    case P3_KEYINFO: {
      int i, j;
      KeyInfo *pKeyInfo = (KeyInfo*)pOp->p3;
      sprintf(zTemp, "keyinfo(%d", pKeyInfo->nField);
      i = strlen(zTemp);
      for(j=0; j<pKeyInfo->nField; j++){
        CollSeq *pColl = pKeyInfo->aColl[j];
        if( pColl ){
          int n = strlen(pColl->zName);
          if( i+n>nTemp-6 ){
            strcpy(&zTemp[i],",...");
            break;
          }
          zTemp[i++] = ',';
          if( pKeyInfo->aSortOrder && pKeyInfo->aSortOrder[j] ){
            zTemp[i++] = '-';
          }
          strcpy(&zTemp[i], pColl->zName);
          i += n;
        }else if( i+4<nTemp-6 ){
          strcpy(&zTemp[i],",nil");
          i += 4;
        }
      }
      zTemp[i++] = ')';
      zTemp[i] = 0;
      assert( i<nTemp );
      zP3 = zTemp;
      break;
    }
    case P3_COLLSEQ: {
      CollSeq *pColl = (CollSeq*)pOp->p3;
      sprintf(zTemp, "collseq(%.20s)", pColl->zName);
      zP3 = zTemp;
      break;
    }
    case P3_FUNCDEF: {
      FuncDef *pDef = (FuncDef*)pOp->p3;
      char zNum[30];
      sprintf(zTemp, "%.*s", nTemp, pDef->zName);
      sprintf(zNum,"(%d)", pDef->nArg);
      if( strlen(zTemp)+strlen(zNum)+1<=nTemp ){
        strcat(zTemp, zNum);
      }
      zP3 = zTemp;
      break;
    }
    default: {
      zP3 = pOp->p3;
      if( zP3==0 || pOp->opcode==OP_Noop ){
        zP3 = "";
      }
    }
  }
  return zP3;
}


#if !defined(NDEBUG) || defined(VDBE_PROFILE) || defined(SQLITE_DEBUG)
/*
** Print a single opcode.  This routine is used for debugging only.
*/
void sqlite3VdbePrintOp(FILE *pOut, int pc, Op *pOp){
  char *zP3;
  char zPtr[50];
  static const char *zFormat1 = "%4d %-13s %4d %4d %s\n";
  if( pOut==0 ) pOut = stdout;
  zP3 = displayP3(pOp, zPtr, sizeof(zPtr));
  fprintf(pOut, zFormat1,
      pc, sqlite3OpcodeNames[pOp->opcode], pOp->p1, pOp->p2, zP3);
  fflush(pOut);
}
#endif

/*
** Release an array of N Mem elements
*/
static void releaseMemArray(Mem *p, int N){
  if( p ){
    while( N-->0 ){
      sqlite3VdbeMemRelease(p++);
    }
  }
}

/*
** Give a listing of the program in the virtual machine.
**
** The interface is the same as sqlite3VdbeExec().  But instead of
** running the code, it invokes the callback once for each instruction.
** This feature is used to implement "EXPLAIN".
*/
int sqlite3VdbeList(
  Vdbe *p                   /* The VDBE */
){
  sqlite3 *db = p->db;
  int i;
  int rc = SQLITE_OK;

  assert( p->explain );

  /* Even though this opcode does not put dynamic strings onto the
  ** the stack, they may become dynamic if the user calls
  ** sqlite3_column_text16(), causing a translation to UTF-16 encoding.
  */
  if( p->pTos==&p->aStack[4] ){
    releaseMemArray(p->aStack, 5);
  }
  p->resOnStack = 0;

  i = p->pc++;
  if( i>=p->nOp ){
    p->rc = SQLITE_OK;
    rc = SQLITE_DONE;
  }else if( db->flags & SQLITE_Interrupt ){
    db->flags &= ~SQLITE_Interrupt;
    if( db->magic!=SQLITE_MAGIC_BUSY ){
      p->rc = SQLITE_MISUSE;
    }else{
      p->rc = SQLITE_INTERRUPT;
    }
    rc = SQLITE_ERROR;
    sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(p->rc), (char*)0);
  }else{
    Op *pOp = &p->aOp[i];
    Mem *pMem = p->aStack;
    pMem->flags = MEM_Int;
    pMem->type = SQLITE_INTEGER;
    pMem->i = i;                                /* Program counter */
    pMem++;

    pMem->flags = MEM_Static|MEM_Str|MEM_Term;
    pMem->z = sqlite3OpcodeNames[pOp->opcode];  /* Opcode */
    pMem->n = strlen(pMem->z);
    pMem->type = SQLITE_TEXT;
    pMem->enc = SQLITE_UTF8;
    pMem++;

    pMem->flags = MEM_Int;
    pMem->i = pOp->p1;                          /* P1 */
    pMem->type = SQLITE_INTEGER;
    pMem++;

    pMem->flags = MEM_Int;
    pMem->i = pOp->p2;                          /* P2 */
    pMem->type = SQLITE_INTEGER;
    pMem++;

    pMem->flags = MEM_Short|MEM_Str|MEM_Term;   /* P3 */
    pMem->z = displayP3(pOp, pMem->zShort, sizeof(pMem->zShort));
    pMem->type = SQLITE_TEXT;
    pMem->enc = SQLITE_UTF8;

    p->nResColumn = 5;
    p->pTos = pMem;
    p->rc = SQLITE_OK;
    p->resOnStack = 1;
    rc = SQLITE_ROW;
  }
  return rc;
}

/*
** Print the SQL that was used to generate a VDBE program.
*/
void sqlite3VdbePrintSql(Vdbe *p){
#ifdef SQLITE_DEBUG
  int nOp = p->nOp;
  VdbeOp *pOp;
  if( nOp<1 ) return;
  pOp = &p->aOp[nOp-1];
  if( pOp->opcode==OP_Noop && pOp->p3!=0 ){
    const char *z = pOp->p3;
    while( isspace(*(u8*)z) ) z++;
    printf("SQL: [%s]\n", z);
  }
#endif
}

/*
** Prepare a virtual machine for execution.  This involves things such
** as allocating stack space and initializing the program counter.
** After the VDBE has be prepped, it can be executed by one or more
** calls to sqlite3VdbeExec().  
**
** This is the only way to move a VDBE from VDBE_MAGIC_INIT to
** VDBE_MAGIC_RUN.
*/
void sqlite3VdbeMakeReady(
  Vdbe *p,                       /* The VDBE */
  int nVar,                      /* Number of '?' see in the SQL statement */
  int nMem,                      /* Number of memory cells to allocate */
  int nCursor,                   /* Number of cursors to allocate */
  int isExplain                  /* True if the EXPLAIN keywords is present */
){
  int n;

  assert( p!=0 );
  assert( p->magic==VDBE_MAGIC_INIT );

  /* There should be at least one opcode.
  */
  assert( p->nOp>0 );

  /* No instruction ever pushes more than a single element onto the
  ** stack.  And the stack never grows on successive executions of the
  ** same loop.  So the total number of instructions is an upper bound
  ** on the maximum stack depth required.
  **
  ** Allocation all the stack space we will ever need.
  */
  if( p->aStack==0 ){
    resolveP2Values(p);
    assert( nVar>=0 );
    n = isExplain ? 10 : p->nOp;
    p->aStack = sqliteMalloc(
        n*sizeof(p->aStack[0])         /* aStack */
      + n*sizeof(Mem*)                 /* apArg */
      + nVar*sizeof(Mem)               /* aVar */
      + nVar*sizeof(char*)             /* azVar */
      + nMem*sizeof(Mem)               /* aMem */
      + nCursor*sizeof(Cursor*)        /* apCsr */
    );
    if( !sqlite3_malloc_failed ){
      p->aMem = &p->aStack[n];
      p->nMem = nMem;
      p->aVar = &p->aMem[nMem];
      p->nVar = nVar;
      p->okVar = 0;
      p->apArg = (Mem**)&p->aVar[nVar];
      p->azVar = (char**)&p->apArg[n];
      p->apCsr = (Cursor**)&p->azVar[nVar];
      p->nCursor = nCursor;
      for(n=0; n<nVar; n++){
        p->aVar[n].flags = MEM_Null;
      }
      for(n=0; n<nMem; n++){
        p->aMem[n].flags = MEM_Null;
      }
    }
  }

#ifdef SQLITE_DEBUG
  if( (p->db->flags & SQLITE_VdbeListing)!=0
    || sqlite3OsFileExists("vdbe_explain")
  ){
    int i;
    printf("VDBE Program Listing:\n");
    sqlite3VdbePrintSql(p);
    for(i=0; i<p->nOp; i++){
      sqlite3VdbePrintOp(stdout, i, &p->aOp[i]);
    }
  }
  if( sqlite3OsFileExists("vdbe_trace") ){
    p->trace = stdout;
  }
#endif
  p->pTos = &p->aStack[-1];
  p->pc = -1;
  p->rc = SQLITE_OK;
  p->uniqueCnt = 0;
  p->returnDepth = 0;
  p->errorAction = OE_Abort;
  p->popStack =  0;
  p->explain |= isExplain;
  p->magic = VDBE_MAGIC_RUN;
  p->nChange = 0;
#ifdef VDBE_PROFILE
  {
    int i;
    for(i=0; i<p->nOp; i++){
      p->aOp[i].cnt = 0;
      p->aOp[i].cycles = 0;
    }
  }
#endif
}


/*
** Remove any elements that remain on the sorter for the VDBE given.
*/
void sqlite3VdbeSorterReset(Vdbe *p){
  while( p->pSort ){
    Sorter *pSorter = p->pSort;
    p->pSort = pSorter->pNext;
    sqliteFree(pSorter->zKey);
    sqlite3VdbeMemRelease(&pSorter->data);
    sqliteFree(pSorter);
  }
}

/*
** Free all resources allociated with AggElem pElem, an element of
** aggregate pAgg.
*/
void freeAggElem(AggElem *pElem, Agg *pAgg){
  int i;
  for(i=0; i<pAgg->nMem; i++){
    Mem *pMem = &pElem->aMem[i];
    if( pAgg->apFunc && pAgg->apFunc[i] && (pMem->flags & MEM_AggCtx)!=0 ){
      sqlite3_context ctx;
      ctx.pFunc = pAgg->apFunc[i];
      ctx.s.flags = MEM_Null;
      ctx.pAgg = pMem->z;
      ctx.cnt = pMem->i;
      ctx.isStep = 0;
      ctx.isError = 0;
      (*pAgg->apFunc[i]->xFinalize)(&ctx);
      pMem->z = ctx.pAgg;
      if( pMem->z!=0 && pMem->z!=pMem->zShort ){
        sqliteFree(pMem->z);
      }
      sqlite3VdbeMemRelease(&ctx.s);
    }else{
      sqlite3VdbeMemRelease(pMem);
    }
  }
  sqliteFree(pElem);
}

/*
** Reset an Agg structure.  Delete all its contents.
**
** For installable aggregate functions, if the step function has been
** called, make sure the finalizer function has also been called.  The
** finalizer might need to free memory that was allocated as part of its
** private context.  If the finalizer has not been called yet, call it
** now.
**
** If db is NULL, then this is being called from sqliteVdbeReset(). In
** this case clean up all references to the temp-table used for
** aggregates (if it was ever opened).
**
** If db is not NULL, then this is being called from with an OP_AggReset
** opcode. Open the temp-table, if it has not already been opened and
** delete the contents of the table used for aggregate information, ready
** for the next round of aggregate processing.
*/
int sqlite3VdbeAggReset(sqlite3 *db, Agg *pAgg, KeyInfo *pKeyInfo){
  int rc = 0;
  BtCursor *pCsr = pAgg->pCsr;

  assert( (pCsr && pAgg->nTab>0) || (!pCsr && pAgg->nTab==0)
         || sqlite3_malloc_failed );

  /* If pCsr is not NULL, then the table used for aggregate information
  ** is open. Loop through it and free the AggElem* structure pointed at
  ** by each entry. If the finalizer has not been called for an AggElem,
  ** do that too. Finally, clear the btree table itself.
  */
  if( pCsr ){
    int res;
    assert( pAgg->pBtree );
    assert( pAgg->nTab>0 );

    rc=sqlite3BtreeFirst(pCsr, &res);
    while( res==0 && rc==SQLITE_OK ){
      AggElem *pElem;
      rc = sqlite3BtreeData(pCsr, 0, sizeof(AggElem*), (char *)&pElem);
      if( res!=SQLITE_OK ){
        return rc;
      }
      assert( pAgg->apFunc!=0 );
      freeAggElem(pElem, pAgg);
      rc=sqlite3BtreeNext(pCsr, &res);
    }
    if( rc!=SQLITE_OK ){
      return rc;
    }

    sqlite3BtreeCloseCursor(pCsr);
    sqlite3BtreeClearTable(pAgg->pBtree, pAgg->nTab);
  }else{ 
    /* The cursor may not be open because the aggregator was never used,
    ** or it could be that it was used but there was no GROUP BY clause.
    */
    if( pAgg->pCurrent ){
      freeAggElem(pAgg->pCurrent, pAgg);
    }
  }

  /* If db is not NULL and we have not yet and we have not yet opened
  ** the temporary btree then do so and create the table to store aggregate
  ** information.
  **
  ** If db is NULL, then close the temporary btree if it is open.
  */
  if( db ){
    if( !pAgg->pBtree ){
      assert( pAgg->nTab==0 );
      rc = sqlite3BtreeFactory(db, ":memory:", 0, TEMP_PAGES, &pAgg->pBtree);
      if( rc!=SQLITE_OK ) return rc;
      sqlite3BtreeBeginTrans(pAgg->pBtree, 1);
      rc = sqlite3BtreeCreateTable(pAgg->pBtree, &pAgg->nTab, 0);
      if( rc!=SQLITE_OK ) return rc;
    }
    assert( pAgg->nTab!=0 );

    rc = sqlite3BtreeCursor(pAgg->pBtree, pAgg->nTab, 1,
        sqlite3VdbeRecordCompare, pKeyInfo, &pAgg->pCsr);
    if( rc!=SQLITE_OK ) return rc;
  }else{
    if( pAgg->pBtree ){
      sqlite3BtreeClose(pAgg->pBtree);
      pAgg->pBtree = 0;
      pAgg->nTab = 0;
    }
    pAgg->pCsr = 0;
  }

  if( pAgg->apFunc ){ 
    sqliteFree(pAgg->apFunc);
    pAgg->apFunc = 0;
  }
  pAgg->pCurrent = 0;
  pAgg->nMem = 0;
  pAgg->searching = 0;
  return SQLITE_OK;
}


/*
** Delete a keylist
*/
void sqlite3VdbeKeylistFree(Keylist *p){
  while( p ){
    Keylist *pNext = p->pNext;
    sqliteFree(p);
    p = pNext;
  }
}

/*
** Close a cursor and release all the resources that cursor happens
** to hold.
*/
void sqlite3VdbeFreeCursor(Cursor *pCx){
  if( pCx==0 ){
    return;
  }
  if( pCx->pCursor ){
    sqlite3BtreeCloseCursor(pCx->pCursor);
  }
  if( pCx->pBt ){
    sqlite3BtreeClose(pCx->pBt);
  }
  sqliteFree(pCx->pData);
  sqliteFree(pCx->aType);
  sqliteFree(pCx);
}

/*
** Close all cursors
*/
static void closeAllCursors(Vdbe *p){
  int i;
  if( p->apCsr==0 ) return;
  for(i=0; i<p->nCursor; i++){
    sqlite3VdbeFreeCursor(p->apCsr[i]);
    p->apCsr[i] = 0;
  }
}

/*
** Clean up the VM after execution.
**
** This routine will automatically close any cursors, lists, and/or
** sorters that were left open.  It also deletes the values of
** variables in the aVar[] array.
*/
static void Cleanup(Vdbe *p){
  int i;
  if( p->aStack ){
    releaseMemArray(p->aStack, 1 + (p->pTos - p->aStack));
    p->pTos = &p->aStack[-1];
  }
  closeAllCursors(p);
  releaseMemArray(p->aMem, p->nMem);
  if( p->pList ){
    sqlite3VdbeKeylistFree(p->pList);
    p->pList = 0;
  }
  if( p->contextStack ){
    for(i=0; i<p->contextStackTop; i++){
      sqlite3VdbeKeylistFree(p->contextStack[i].pList);
    }
    sqliteFree(p->contextStack);
  }
  sqlite3VdbeSorterReset(p);
  sqlite3VdbeAggReset(0, &p->agg, 0);
  p->contextStack = 0;
  p->contextStackDepth = 0;
  p->contextStackTop = 0;
  sqliteFree(p->zErrMsg);
  p->zErrMsg = 0;
}

/*
** Set the number of result columns that will be returned by this SQL
** statement. This is now set at compile time, rather than during
** execution of the vdbe program so that sqlite3_column_count() can
** be called on an SQL statement before sqlite3_step().
*/
void sqlite3VdbeSetNumCols(Vdbe *p, int nResColumn){
  Mem *pColName;
  int n;
  assert( 0==p->nResColumn );
  p->nResColumn = nResColumn;
  n = nResColumn*2;
  p->aColName = pColName = (Mem*)sqliteMalloc( sizeof(Mem)*n );
  if( p->aColName==0 ) return;
  while( n-- > 0 ){
    (pColName++)->flags = MEM_Null;
  }
}

/*
** Set the name of the idx'th column to be returned by the SQL statement.
** zName must be a pointer to a nul terminated string.
**
** This call must be made after a call to sqlite3VdbeSetNumCols().
**
** If N==P3_STATIC  it means that zName is a pointer to a constant static
** string and we can just copy the pointer. If it is P3_DYNAMIC, then 
** the string is freed using sqliteFree() when the vdbe is finished with
** it. Otherwise, N bytes of zName are copied.
*/
int sqlite3VdbeSetColName(Vdbe *p, int idx, const char *zName, int N){
  int rc;
  Mem *pColName;
  assert( idx<(2*p->nResColumn) );
  if( sqlite3_malloc_failed ) return SQLITE_NOMEM;
  assert( p->aColName!=0 );
  pColName = &(p->aColName[idx]);
  if( N==P3_DYNAMIC || N==P3_STATIC ){
    rc = sqlite3VdbeMemSetStr(pColName, zName, -1, SQLITE_UTF8, SQLITE_STATIC);
  }else{
    rc = sqlite3VdbeMemSetStr(pColName, zName, N, SQLITE_UTF8,SQLITE_TRANSIENT);
  }
  if( rc==SQLITE_OK && N==P3_DYNAMIC ){
    pColName->flags = (pColName->flags&(~MEM_Static))|MEM_Dyn;
    pColName->xDel = 0;
  }
  return rc;
}

/*
** A read or write transaction may or may not be active on database handle
** db. If a transaction is active, commit it. If there is a
** write-transaction spanning more than one database file, this routine
** takes care of the master journal trickery.
*/
static int vdbeCommit(sqlite3 *db){
  int i;
  int nTrans = 0;  /* Number of databases with an active write-transaction */
  int rc = SQLITE_OK;
  int needXcommit = 0;

  for(i=0; i<db->nDb; i++){ 
    Btree *pBt = db->aDb[i].pBt;
    if( pBt && sqlite3BtreeIsInTrans(pBt) ){
      needXcommit = 1;
      if( i!=1 ) nTrans++;
    }
  }

  /* If there are any write-transactions at all, invoke the commit hook */
  if( needXcommit && db->xCommitCallback ){
    int rc;
    sqlite3SafetyOff(db);
    rc = db->xCommitCallback(db->pCommitArg);
    sqlite3SafetyOn(db);
    if( rc ){
      return SQLITE_CONSTRAINT;
    }
  }

  /* The simple case - no more than one database file (not counting the
  ** TEMP database) has a transaction active.   There is no need for the
  ** master-journal.
  **
  ** If the return value of sqlite3BtreeGetFilename() is a zero length
  ** string, it means the main database is :memory:.  In that case we do
  ** not support atomic multi-file commits, so use the simple case then
  ** too.
  */
  if( 0==strlen(sqlite3BtreeGetFilename(db->aDb[0].pBt)) || nTrans<=1 ){
    for(i=0; rc==SQLITE_OK && i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        rc = sqlite3BtreeSync(pBt, 0);
      }
    }

    /* Do the commit only if all databases successfully synced */
    if( rc==SQLITE_OK ){
      for(i=0; i<db->nDb; i++){
        Btree *pBt = db->aDb[i].pBt;
        if( pBt ){
          sqlite3BtreeCommit(pBt);
        }
      }
    }
  }

  /* The complex case - There is a multi-file write-transaction active.
  ** This requires a master journal file to ensure the transaction is
  ** committed atomicly.
  */
  else{
    char *zMaster = 0;   /* File-name for the master journal */
    char const *zMainFile = sqlite3BtreeGetFilename(db->aDb[0].pBt);
    OsFile master;

    /* Select a master journal file name */
    do {
      u32 random;
      sqliteFree(zMaster);
      sqlite3Randomness(sizeof(random), &random);
      zMaster = sqlite3MPrintf("%s-mj%08X", zMainFile, random&0x7fffffff);
      if( !zMaster ){
        return SQLITE_NOMEM;
      }
    }while( sqlite3OsFileExists(zMaster) );

    /* Open the master journal. */
    memset(&master, 0, sizeof(master));
    rc = sqlite3OsOpenExclusive(zMaster, &master, 0);
    if( rc!=SQLITE_OK ){
      sqliteFree(zMaster);
      return rc;
    }
 
    /* Write the name of each database file in the transaction into the new
    ** master journal file. If an error occurs at this point close
    ** and delete the master journal file. All the individual journal files
    ** still have 'null' as the master journal pointer, so they will roll
    ** back independantly if a failure occurs.
    */
    for(i=0; i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( i==1 ) continue;   /* Ignore the TEMP database */
      if( pBt && sqlite3BtreeIsInTrans(pBt) ){
        char const *zFile = sqlite3BtreeGetJournalname(pBt);
        if( zFile[0]==0 ) continue;  /* Ignore :memory: databases */
        rc = sqlite3OsWrite(&master, zFile, strlen(zFile)+1);
        if( rc!=SQLITE_OK ){
          sqlite3OsClose(&master);
          sqlite3OsDelete(zMaster);
          sqliteFree(zMaster);
          return rc;
        }
      }
    }


    /* Sync the master journal file. Before doing this, open the directory
    ** the master journal file is store in so that it gets synced too.
    */
    zMainFile = sqlite3BtreeGetDirname(db->aDb[0].pBt);
    rc = sqlite3OsOpenDirectory(zMainFile, &master);
    if( rc!=SQLITE_OK ){
      sqlite3OsClose(&master);
      sqlite3OsDelete(zMaster);
      sqliteFree(zMaster);
      return rc;
    }
    rc = sqlite3OsSync(&master);
    if( rc!=SQLITE_OK ){
      sqlite3OsClose(&master);
      sqliteFree(zMaster);
      return rc;
    }

    /* Sync all the db files involved in the transaction. The same call
    ** sets the master journal pointer in each individual journal. If
    ** an error occurs here, do not delete the master journal file.
    **
    ** If the error occurs during the first call to sqlite3BtreeSync(),
    ** then there is a chance that the master journal file will be
    ** orphaned. But we cannot delete it, in case the master journal
    ** file name was written into the journal file before the failure
    ** occured.
    */
    for(i=0; i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( pBt && sqlite3BtreeIsInTrans(pBt) ){
        rc = sqlite3BtreeSync(pBt, zMaster);
        if( rc!=SQLITE_OK ){
          sqlite3OsClose(&master);
          sqliteFree(zMaster);
          return rc;
        }
      }
    }
    sqlite3OsClose(&master);

    /* Delete the master journal file. This commits the transaction. After
    ** doing this the directory is synced again before any individual
    ** transaction files are deleted.
    */
    rc = sqlite3OsDelete(zMaster);
    assert( rc==SQLITE_OK );
    sqliteFree(zMaster);
    zMaster = 0;
    rc = sqlite3OsSyncDirectory(zMainFile);
    if( rc!=SQLITE_OK ){
      /* This is not good. The master journal file has been deleted, but
      ** the directory sync failed. There is no completely safe course of
      ** action from here. The individual journals contain the name of the
      ** master journal file, but there is no way of knowing if that
      ** master journal exists now or if it will exist after the operating
      ** system crash that may follow the fsync() failure.
      */
      assert(0);
      sqliteFree(zMaster);
      return rc;
    }

    /* All files and directories have already been synced, so the following
    ** calls to sqlite3BtreeCommit() are only closing files and deleting
    ** journals. If something goes wrong while this is happening we don't
    ** really care. The integrity of the transaction is already guaranteed,
    ** but some stray 'cold' journals may be lying around. Returning an
    ** error code won't help matters.
    */
    for(i=0; i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        sqlite3BtreeCommit(pBt);
      }
    }
  }

  return rc;
}

/*
** Find every active VM other than pVdbe and change its status to
** aborted.  This happens when one VM causes a rollback due to an
** ON CONFLICT ROLLBACK clause (for example).  The other VMs must be
** aborted so that they do not have data rolled out from underneath
** them leading to a segfault.
*/
static void abortOtherActiveVdbes(Vdbe *pVdbe){
  Vdbe *pOther;
  for(pOther=pVdbe->db->pVdbe; pOther; pOther=pOther->pNext){
    if( pOther==pVdbe ) continue;
    if( pOther->magic!=VDBE_MAGIC_RUN || pOther->pc<0 ) continue;
    closeAllCursors(pOther);
    pOther->aborted = 1;
  }
}

/* 
** This routine checks that the sqlite3.activeVdbeCnt count variable
** matches the number of vdbe's in the list sqlite3.pVdbe that are
** currently active. An assertion fails if the two counts do not match.
** This is an internal self-check only - it is not an essential processing
** step.
**
** This is a no-op if NDEBUG is defined.
*/
#ifndef NDEBUG
static void checkActiveVdbeCnt(sqlite3 *db){
  Vdbe *p;
  int cnt = 0;
  p = db->pVdbe;
  while( p ){
    if( p->magic==VDBE_MAGIC_RUN && p->pc>=0 ){
      cnt++;
    }
    p = p->pNext;
  }
  assert( cnt==db->activeVdbeCnt );
}
#else
#define checkActiveVdbeCnt(x)
#endif

/*
** This routine is called the when a VDBE tries to halt.  If the VDBE
** has made changes and is in autocommit mode, then commit those
** changes.  If a rollback is needed, then do the rollback.
**
** This routine is the only way to move the state of a VM from
** SQLITE_MAGIC_RUN to SQLITE_MAGIC_HALT.
**
** Return an error code.  If the commit could not complete because of
** lock contention, return SQLITE_BUSY.  If SQLITE_BUSY is returned, it
** means the close did not happen and needs to be repeated.
*/
int sqlite3VdbeHalt(Vdbe *p){
  sqlite3 *db = p->db;
  int i;
  int (*xFunc)(Btree *pBt) = 0;  /* Function to call on each btree backend */

  if( p->magic!=VDBE_MAGIC_RUN ){
    /* Already halted.  Nothing to do. */
    assert( p->magic==VDBE_MAGIC_HALT );
    return SQLITE_OK;
  }
  closeAllCursors(p);
  checkActiveVdbeCnt(db);
  if( db->autoCommit && db->activeVdbeCnt==1 ){
    if( p->rc==SQLITE_OK || p->errorAction==OE_Fail ){
      /* The auto-commit flag is true, there are no other active queries
      ** using this handle and the vdbe program was successful or hit an
      ** 'OR FAIL' constraint. This means a commit is required.
      */
      int rc = vdbeCommit(db);
      if( rc==SQLITE_BUSY ){
        return SQLITE_BUSY;
      }else if( rc!=SQLITE_OK ){
        p->rc = rc;
        xFunc = sqlite3BtreeRollback;
      }
    }else{
      xFunc = sqlite3BtreeRollback;
    }
  }else{
    if( p->rc==SQLITE_OK || p->errorAction==OE_Fail ){
      xFunc = sqlite3BtreeCommitStmt;
    }else if( p->errorAction==OE_Abort ){
      xFunc = sqlite3BtreeRollbackStmt;
    }else{
      xFunc = sqlite3BtreeRollback;
      db->autoCommit = 1;
      abortOtherActiveVdbes(p);
    }
  }

  /* If xFunc is not NULL, then it is one of sqlite3BtreeRollback,
  ** sqlite3BtreeRollbackStmt or sqlite3BtreeCommitStmt. Call it once on
  ** each backend. If an error occurs and the return code is still
  ** SQLITE_OK, set the return code to the new error value.
  */
  for(i=0; xFunc && i<db->nDb; i++){ 
    int rc;
    Btree *pBt = db->aDb[i].pBt;
    if( pBt ){
      rc = xFunc(pBt);
      if( p->rc==SQLITE_OK ) p->rc = rc;
    }
  }

  /* If this was an INSERT, UPDATE or DELETE, set the change counter. */
  if( p->changeCntOn ){
    if( !xFunc || xFunc==sqlite3BtreeCommitStmt ){
      sqlite3VdbeSetChanges(db, p->nChange);
    }else{
      sqlite3VdbeSetChanges(db, 0);
    }
    p->nChange = 0;
  }

  /* Rollback or commit any schema changes that occurred. */
  if( p->rc!=SQLITE_OK ){
    sqlite3RollbackInternalChanges(db);
  }else if( db->flags & SQLITE_InternChanges ){
    sqlite3CommitInternalChanges(db);
  }

  /* We have successfully halted and closed the VM.  Record this fact. */
  if( p->pc>=0 ){
    db->activeVdbeCnt--;
  }
  p->magic = VDBE_MAGIC_HALT;
  checkActiveVdbeCnt(db);

  return SQLITE_OK;
}

/*
** Clean up a VDBE after execution but do not delete the VDBE just yet.
** Write any error messages into *pzErrMsg.  Return the result code.
**
** After this routine is run, the VDBE should be ready to be executed
** again.
**
** To look at it another way, this routine resets the state of the
** virtual machine from VDBE_MAGIC_RUN or VDBE_MAGIC_HALT back to
** VDBE_MAGIC_INIT.
*/
int sqlite3VdbeReset(Vdbe *p){
  if( p->magic!=VDBE_MAGIC_RUN && p->magic!=VDBE_MAGIC_HALT ){
    sqlite3Error(p->db, SQLITE_MISUSE, 0);
    return SQLITE_MISUSE;
  }

  /* If the VM did not run to completion or if it encountered an
  ** error, then it might not have been halted properly.  So halt
  ** it now.
  */
  sqlite3VdbeHalt(p);

  /* Transfer the error code and error message from the VDBE into the
  ** main database structure.
  */
  if( p->zErrMsg ){
    sqlite3Error(p->db, p->rc, "%s", p->zErrMsg);
    sqliteFree(p->zErrMsg);
    p->zErrMsg = 0;
  }else if( p->rc ){
    sqlite3Error(p->db, p->rc, 0);
  }else{
    sqlite3Error(p->db, SQLITE_OK, 0);
  }

  /* Reclaim all memory used by the VDBE
  */
  Cleanup(p);

  /* Save profiling information from this VDBE run.
  */
  assert( p->pTos<&p->aStack[p->pc<0?0:p->pc] || sqlite3_malloc_failed==1 );
#ifdef VDBE_PROFILE
  {
    FILE *out = fopen("vdbe_profile.out", "a");
    if( out ){
      int i;
      fprintf(out, "---- ");
      for(i=0; i<p->nOp; i++){
        fprintf(out, "%02x", p->aOp[i].opcode);
      }
      fprintf(out, "\n");
      for(i=0; i<p->nOp; i++){
        fprintf(out, "%6d %10lld %8lld ",
           p->aOp[i].cnt,
           p->aOp[i].cycles,
           p->aOp[i].cnt>0 ? p->aOp[i].cycles/p->aOp[i].cnt : 0
        );
        sqlite3VdbePrintOp(out, i, &p->aOp[i]);
      }
      fclose(out);
    }
  }
#endif
  p->magic = VDBE_MAGIC_INIT;
  p->aborted = 0;
  return p->rc;
}
 
/*
** Clean up and delete a VDBE after execution.  Return an integer which is
** the result code.  Write any error message text into *pzErrMsg.
*/
int sqlite3VdbeFinalize(Vdbe *p){
  int rc = SQLITE_OK;
  sqlite3 *db = p->db;

  if( p->magic==VDBE_MAGIC_RUN || p->magic==VDBE_MAGIC_HALT ){
    rc = sqlite3VdbeReset(p);
  }else if( p->magic!=VDBE_MAGIC_INIT ){
    /* sqlite3Error(p->db, SQLITE_MISUSE, 0); */
    return SQLITE_MISUSE;
  }
  sqlite3VdbeDelete(p);
  if( rc==SQLITE_SCHEMA ){
    sqlite3ResetInternalSchema(db, 0);
  }
  return rc;
}

/*
** Call the destructor for each auxdata entry in pVdbeFunc for which
** the corresponding bit in mask is clear.  Auxdata entries beyond 31
** are always destroyed.  To destroy all auxdata entries, call this
** routine with mask==0.
*/
void sqlite3VdbeDeleteAuxData(VdbeFunc *pVdbeFunc, int mask){
  int i;
  for(i=0; i<pVdbeFunc->nAux; i++){
    struct AuxData *pAux = &pVdbeFunc->apAux[i];
    if( (i>31 || !(mask&(1<<i))) && pAux->pAux ){
      if( pAux->xDelete ){
        pAux->xDelete(pAux->pAux);
      }
      pAux->pAux = 0;
    }
  }
}

/*
** Delete an entire VDBE.
*/
void sqlite3VdbeDelete(Vdbe *p){
  int i;
  if( p==0 ) return;
  Cleanup(p);
  if( p->pPrev ){
    p->pPrev->pNext = p->pNext;
  }else{
    assert( p->db->pVdbe==p );
    p->db->pVdbe = p->pNext;
  }
  if( p->pNext ){
    p->pNext->pPrev = p->pPrev;
  }
  if( p->aOp ){
    for(i=0; i<p->nOp; i++){
      Op *pOp = &p->aOp[i];
      if( pOp->p3type==P3_DYNAMIC || pOp->p3type==P3_KEYINFO ){
        sqliteFree(pOp->p3);
      }
      if( pOp->p3type==P3_VDBEFUNC ){
        VdbeFunc *pVdbeFunc = (VdbeFunc *)pOp->p3;
        sqlite3VdbeDeleteAuxData(pVdbeFunc, 0);
        sqliteFree(pVdbeFunc);
      }
    }
    sqliteFree(p->aOp);
  }
  releaseMemArray(p->aVar, p->nVar);
  sqliteFree(p->aLabel);
  sqliteFree(p->aStack);
  releaseMemArray(p->aColName, p->nResColumn*2);
  sqliteFree(p->aColName);
  p->magic = VDBE_MAGIC_DEAD;
  sqliteFree(p);
}

/*
** If a MoveTo operation is pending on the given cursor, then do that
** MoveTo now.  Return an error code.  If no MoveTo is pending, this
** routine does nothing and returns SQLITE_OK.
*/
int sqlite3VdbeCursorMoveto(Cursor *p){
  if( p->deferredMoveto ){
    int res;
    extern int sqlite3_search_count;
    assert( p->intKey );
    if( p->intKey ){
      sqlite3BtreeMoveto(p->pCursor, 0, p->movetoTarget, &res);
    }else{
      sqlite3BtreeMoveto(p->pCursor,(char*)&p->movetoTarget,sizeof(i64),&res);
    }
    *p->pIncrKey = 0;
    p->lastRecno = keyToInt(p->movetoTarget);
    p->recnoIsValid = res==0;
    if( res<0 ){
      sqlite3BtreeNext(p->pCursor, &res);
    }
    sqlite3_search_count++;
    p->deferredMoveto = 0;
    p->cacheValid = 0;
  }
  return SQLITE_OK;
}

/*
** The following functions:
**
** sqlite3VdbeSerialType()
** sqlite3VdbeSerialTypeLen()
** sqlite3VdbeSerialRead()
** sqlite3VdbeSerialLen()
** sqlite3VdbeSerialWrite()
**
** encapsulate the code that serializes values for storage in SQLite
** data and index records. Each serialized value consists of a
** 'serial-type' and a blob of data. The serial type is an 8-byte unsigned
** integer, stored as a varint.
**
** In an SQLite index record, the serial type is stored directly before
** the blob of data that it corresponds to. In a table record, all serial
** types are stored at the start of the record, and the blobs of data at
** the end. Hence these functions allow the caller to handle the
** serial-type and data blob seperately.
**
** The following table describes the various storage classes for data:
**
**   serial type        bytes of data      type
**   --------------     ---------------    ---------------
**      0                     0            NULL
**      1                     1            signed integer
**      2                     2            signed integer
**      3                     3            signed integer
**      4                     4            signed integer
**      5                     6            signed integer
**      6                     8            signed integer
**      7                     8            IEEE float
**     8-11                                reserved for expansion
**    N>=12 and even       (N-12)/2        BLOB
**    N>=13 and odd        (N-13)/2        text
**
*/

/*
** Return the serial-type for the value stored in pMem.
*/
u32 sqlite3VdbeSerialType(Mem *pMem){
  int flags = pMem->flags;

  if( flags&MEM_Null ){
    return 0;
  }
  if( flags&MEM_Int ){
    /* Figure out whether to use 1, 2, 4 or 8 bytes. */
    i64 i = pMem->i;
    if( i>=-127 && i<=127 ) return 1;
    if( i>=-32767 && i<=32767 ) return 2;
    if( i>=-8388607 && i<=8388607 ) return 3;
    if( i>=-2147483647 && i<=2147483647 ) return 4;
    if( i>=-140737488355328L && i<=140737488355328L ) return 5;
    return 6;
  }
  if( flags&MEM_Real ){
    return 7;
  }
  if( flags&MEM_Str ){
    int n = pMem->n;
    assert( n>=0 );
    return ((n*2) + 13);
  }
  if( flags&MEM_Blob ){
    return (pMem->n*2 + 12);
  }
  return 0;
}

/*
** Return the length of the data corresponding to the supplied serial-type.
*/
int sqlite3VdbeSerialTypeLen(u32 serial_type){
  if( serial_type>=12 ){
    return (serial_type-12)/2;
  }else{
    static const u8 aSize[] = { 0, 1, 2, 3, 4, 6, 8, 8, 0, 0, 0, 0 };
    return aSize[serial_type];
  }
}

/*
** Write the serialized data blob for the value stored in pMem into 
** buf. It is assumed that the caller has allocated sufficient space.
** Return the number of bytes written.
*/ 
int sqlite3VdbeSerialPut(unsigned char *buf, Mem *pMem){
  u32 serial_type = sqlite3VdbeSerialType(pMem);
  int len;

  /* NULL */
  if( serial_type==0 ){
    return 0;
  }
 
  /* Integer and Real */
  if( serial_type<=7 ){
    u64 v;
    int i;
    if( serial_type==7 ){
      v = *(u64*)&pMem->r;
    }else{
      v = *(u64*)&pMem->i;
    }
    len = i = sqlite3VdbeSerialTypeLen(serial_type);
    while( i-- ){
      buf[i] = (v&0xFF);
      v >>= 8;
    }
    return len;
  }
  
  /* String or blob */
  assert( serial_type>=12 );
  len = sqlite3VdbeSerialTypeLen(serial_type);
  memcpy(buf, pMem->z, len);
  return len;
}

/*
** Deserialize the data blob pointed to by buf as serial type serial_type
** and store the result in pMem.  Return the number of bytes read.
*/ 
int sqlite3VdbeSerialGet(
  const unsigned char *buf,     /* Buffer to deserialize from */
  u32 serial_type,              /* Serial type to deserialize */
  Mem *pMem                     /* Memory cell to write value into */
){
  int len;

  if( serial_type==0 ){
    /* NULL */
    pMem->flags = MEM_Null;
    return 0;
  }
  len = sqlite3VdbeSerialTypeLen(serial_type);
  if( serial_type<=7 ){
    /* Integer and Real */
    if( serial_type<=4 ){
      /* 32-bit integer type.  This is handled by a special case for
      ** performance reasons. */
      int v = buf[0];
      int n;
      if( v&0x80 ){
        v |= -256;
      }
      for(n=1; n<len; n++){
        v = (v<<8) | buf[n];
      }
      pMem->flags = MEM_Int;
      pMem->i = v;
      return n;
    }else{
      u64 v = 0;
      int n;

      if( buf[0]&0x80 ){
        v = -1;
      }
      for(n=0; n<len; n++){
        v = (v<<8) | buf[n];
      }
      if( serial_type==7 ){
        pMem->flags = MEM_Real;
        pMem->r = *(double*)&v;
      }else{
        pMem->flags = MEM_Int;
        pMem->i = *(i64*)&v;
      }
    }
  }else{
    /* String or blob */
    assert( serial_type>=12 );
    pMem->z = (char *)buf;
    pMem->n = len;
    pMem->xDel = 0;
    if( serial_type&0x01 ){
      pMem->flags = MEM_Str | MEM_Ephem;
    }else{
      pMem->flags = MEM_Blob | MEM_Ephem;
    }
  }
  return len;
}

/*
** This function compares the two table rows or index records specified by 
** {nKey1, pKey1} and {nKey2, pKey2}, returning a negative, zero
** or positive integer if {nKey1, pKey1} is less than, equal to or 
** greater than {nKey2, pKey2}.  Both Key1 and Key2 must be byte strings
** composed by the OP_MakeRecord opcode of the VDBE.
*/
int sqlite3VdbeRecordCompare(
  void *userData,
  int nKey1, const void *pKey1, 
  int nKey2, const void *pKey2
){
  KeyInfo *pKeyInfo = (KeyInfo*)userData;
  u32 d1, d2;          /* Offset into aKey[] of next data element */
  u32 idx1, idx2;      /* Offset into aKey[] of next header element */
  u32 szHdr1, szHdr2;  /* Number of bytes in header */
  int i = 0;
  int nField;
  int rc = 0;
  const unsigned char *aKey1 = (const unsigned char *)pKey1;
  const unsigned char *aKey2 = (const unsigned char *)pKey2;

  Mem mem1;
  Mem mem2;
  mem1.enc = pKeyInfo->enc;
  mem2.enc = pKeyInfo->enc;
  
  idx1 = sqlite3GetVarint32(pKey1, &szHdr1);
  d1 = szHdr1;
  idx2 = sqlite3GetVarint32(pKey2, &szHdr2);
  d2 = szHdr2;
  nField = pKeyInfo->nField;
  while( idx1<szHdr1 && idx2<szHdr2 ){
    u32 serial_type1;
    u32 serial_type2;

    /* Read the serial types for the next element in each key. */
    idx1 += sqlite3GetVarint32(&aKey1[idx1], &serial_type1);
    if( d1>=nKey1 && sqlite3VdbeSerialTypeLen(serial_type1)>0 ) break;
    idx2 += sqlite3GetVarint32(&aKey2[idx2], &serial_type2);
    if( d2>=nKey2 && sqlite3VdbeSerialTypeLen(serial_type2)>0 ) break;

    /* Assert that there is enough space left in each key for the blob of
    ** data to go with the serial type just read. This assert may fail if
    ** the file is corrupted.  Then read the value from each key into mem1
    ** and mem2 respectively.
    */
    d1 += sqlite3VdbeSerialGet(&aKey1[d1], serial_type1, &mem1);
    d2 += sqlite3VdbeSerialGet(&aKey2[d2], serial_type2, &mem2);

    rc = sqlite3MemCompare(&mem1, &mem2, i<nField ? pKeyInfo->aColl[i] : 0);
    sqlite3VdbeMemRelease(&mem1);
    sqlite3VdbeMemRelease(&mem2);
    if( rc!=0 ){
      break;
    }
    i++;
  }

  /* One of the keys ran out of fields, but all the fields up to that point
  ** were equal. If the incrKey flag is true, then the second key is
  ** treated as larger.
  */
  if( rc==0 ){
    if( pKeyInfo->incrKey ){
      rc = -1;
    }else if( d1<nKey1 ){
      rc = 1;
    }else if( d2<nKey2 ){
      rc = -1;
    }
  }

  if( pKeyInfo->aSortOrder && i<pKeyInfo->nField && pKeyInfo->aSortOrder[i] ){
    rc = -rc;
  }

  return rc;
}

/*
** The argument is an index entry composed using the OP_MakeRecord opcode.
** The last entry in this record should be an integer (specifically
** an integer rowid).  This routine returns the number of bytes in
** that integer.
*/
int sqlite3VdbeIdxRowidLen(int nKey, const u8 *aKey){
  u32 szHdr;        /* Size of the header */
  u32 typeRowid;    /* Serial type of the rowid */

  sqlite3GetVarint32(aKey, &szHdr);
  sqlite3GetVarint32(&aKey[szHdr-1], &typeRowid);
  return sqlite3VdbeSerialTypeLen(typeRowid);
}
  

/*
** pCur points at an index entry created using the OP_MakeRecord opcode.
** Read the rowid (the last field in the record) and store it in *rowid.
** Return SQLITE_OK if everything works, or an error code otherwise.
*/
int sqlite3VdbeIdxRowid(BtCursor *pCur, i64 *rowid){
  i64 nCellKey;
  int rc;
  u32 szHdr;        /* Size of the header */
  u32 typeRowid;    /* Serial type of the rowid */
  u32 lenRowid;     /* Size of the rowid */
  Mem m, v;

  sqlite3BtreeKeySize(pCur, &nCellKey);
  if( nCellKey<=0 ){
    return SQLITE_CORRUPT;
  }
  rc = sqlite3VdbeMemFromBtree(pCur, 0, nCellKey, 1, &m);
  if( rc ){
    return rc;
  }
  sqlite3GetVarint32(m.z, &szHdr);
  sqlite3GetVarint32(&m.z[szHdr-1], &typeRowid);
  lenRowid = sqlite3VdbeSerialTypeLen(typeRowid);
  sqlite3VdbeSerialGet(&m.z[m.n-lenRowid], typeRowid, &v);
  *rowid = v.i;
  sqlite3VdbeMemRelease(&m);
  return SQLITE_OK;
}

/*
** Compare the key of the index entry that cursor pC is point to against
** the key string in pKey (of length nKey).  Write into *pRes a number
** that is negative, zero, or positive if pC is less than, equal to,
** or greater than pKey.  Return SQLITE_OK on success.
**
** pKey is either created without a rowid or is truncated so that it
** omits the rowid at the end.  The rowid at the end of the index entry
** is ignored as well.
*/
int sqlite3VdbeIdxKeyCompare(
  Cursor *pC,                 /* The cursor to compare against */
  int nKey, const u8 *pKey,   /* The key to compare */
  int *res                    /* Write the comparison result here */
){
  i64 nCellKey;
  int rc;
  BtCursor *pCur = pC->pCursor;
  int lenRowid;
  Mem m;

  sqlite3BtreeKeySize(pCur, &nCellKey);
  if( nCellKey<=0 ){
    *res = 0;
    return SQLITE_OK;
  }
  rc = sqlite3VdbeMemFromBtree(pC->pCursor, 0, nCellKey, 1, &m);
  if( rc ){
    return rc;
  }
  lenRowid = sqlite3VdbeIdxRowidLen(m.n, m.z);
  *res = sqlite3VdbeRecordCompare(pC->pKeyInfo, m.n-lenRowid, m.z, nKey, pKey);
  sqlite3VdbeMemRelease(&m);
  return SQLITE_OK;
}

/*
** This routine sets the value to be returned by subsequent calls to
** sqlite3_changes() on the database handle 'db'. 
*/
void sqlite3VdbeSetChanges(sqlite3 *db, int nChange){
  db->nChange = nChange;
  db->nTotalChange += nChange;
}

/*
** Set a flag in the vdbe to update the change counter when it is finalised
** or reset.
*/
void sqlite3VdbeCountChanges(Vdbe *p){
  p->changeCntOn = 1;
}
