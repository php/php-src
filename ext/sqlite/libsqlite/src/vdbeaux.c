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
** a VDBE (or an "sqlite_vm" as it is known to the outside world.)  Prior
** to version 2.8.7, all this code was combined into the vdbe.c source file.
** But that file was getting too big so this subroutines were split out.
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include "vdbeInt.h"


/*
** When debugging the code generator in a symbolic debugger, one can
** set the sqlite_vdbe_addop_trace to 1 and all opcodes will be printed
** as they are added to the instruction stream.
*/
#ifndef NDEBUG
int sqlite_vdbe_addop_trace = 0;
#endif


/*
** Create a new virtual database engine.
*/
Vdbe *sqliteVdbeCreate(sqlite *db){
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
void sqliteVdbeTrace(Vdbe *p, FILE *trace){
  p->trace = trace;
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
** Use the sqliteVdbeResolveLabel() function to fix an address and
** the sqliteVdbeChangeP3() function to change the value of the P3
** operand.
*/
int sqliteVdbeAddOp(Vdbe *p, int op, int p1, int p2){
  int i;
  VdbeOp *pOp;

  i = p->nOp;
  p->nOp++;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( i>=p->nOpAlloc ){
    int oldSize = p->nOpAlloc;
    Op *aNew;
    p->nOpAlloc = p->nOpAlloc*2 + 100;
    aNew = sqliteRealloc(p->aOp, p->nOpAlloc*sizeof(Op));
    if( aNew==0 ){
      p->nOpAlloc = oldSize;
      return 0;
    }
    p->aOp = aNew;
    memset(&p->aOp[oldSize], 0, (p->nOpAlloc-oldSize)*sizeof(Op));
  }
  pOp = &p->aOp[i];
  pOp->opcode = op;
  pOp->p1 = p1;
  if( p2<0 && (-1-p2)<p->nLabel && p->aLabel[-1-p2]>=0 ){
    p2 = p->aLabel[-1-p2];
  }
  pOp->p2 = p2;
  pOp->p3 = 0;
  pOp->p3type = P3_NOTUSED;
#ifndef NDEBUG
  if( sqlite_vdbe_addop_trace ) sqliteVdbePrintOp(0, i, &p->aOp[i]);
#endif
  return i;
}

/*
** Add an opcode that includes the p3 value.
*/
int sqliteVdbeOp3(Vdbe *p, int op, int p1, int p2, const char *zP3, int p3type){
  int addr = sqliteVdbeAddOp(p, op, p1, p2);
  sqliteVdbeChangeP3(p, addr, zP3, p3type);
  return addr;
}

/*
** Add multiple opcodes.  The list is terminated by an opcode of 0.
*/
int sqliteVdbeCode(Vdbe *p, ...){
  int addr;
  va_list ap;
  int opcode, p1, p2;
  va_start(ap, p);
  addr = p->nOp;
  while( (opcode = va_arg(ap,int))!=0 ){
    p1 = va_arg(ap,int);
    p2 = va_arg(ap,int);
    sqliteVdbeAddOp(p, opcode, p1, p2);
  }
  va_end(ap);
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
*/
int sqliteVdbeMakeLabel(Vdbe *p){
  int i;
  i = p->nLabel++;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( i>=p->nLabelAlloc ){
    int *aNew;
    p->nLabelAlloc = p->nLabelAlloc*2 + 10;
    aNew = sqliteRealloc( p->aLabel, p->nLabelAlloc*sizeof(p->aLabel[0]));
    if( aNew==0 ){
      sqliteFree(p->aLabel);
    }
    p->aLabel = aNew;
  }
  if( p->aLabel==0 ){
    p->nLabel = 0;
    p->nLabelAlloc = 0;
    return 0;
  }
  p->aLabel[i] = -1;
  return -1-i;
}

/*
** Resolve label "x" to be the address of the next instruction to
** be inserted.  The parameter "x" must have been obtained from
** a prior call to sqliteVdbeMakeLabel().
*/
void sqliteVdbeResolveLabel(Vdbe *p, int x){
  int j;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( x<0 && (-x)<=p->nLabel && p->aOp ){
    if( p->aLabel[-1-x]==p->nOp ) return;
    assert( p->aLabel[-1-x]<0 );
    p->aLabel[-1-x] = p->nOp;
    for(j=0; j<p->nOp; j++){
      if( p->aOp[j].p2==x ) p->aOp[j].p2 = p->nOp;
    }
  }
}

/*
** Return the address of the next instruction to be inserted.
*/
int sqliteVdbeCurrentAddr(Vdbe *p){
  assert( p->magic==VDBE_MAGIC_INIT );
  return p->nOp;
}

/*
** Add a whole list of operations to the operation stack.  Return the
** address of the first operation added.
*/
int sqliteVdbeAddOpList(Vdbe *p, int nOp, VdbeOpList const *aOp){
  int addr;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->nOp + nOp >= p->nOpAlloc ){
    int oldSize = p->nOpAlloc;
    Op *aNew;
    p->nOpAlloc = p->nOpAlloc*2 + nOp + 10;
    aNew = sqliteRealloc(p->aOp, p->nOpAlloc*sizeof(Op));
    if( aNew==0 ){
      p->nOpAlloc = oldSize;
      return 0;
    }
    p->aOp = aNew;
    memset(&p->aOp[oldSize], 0, (p->nOpAlloc-oldSize)*sizeof(Op));
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
      if( sqlite_vdbe_addop_trace ){
        sqliteVdbePrintOp(0, i+addr, &p->aOp[i+addr]);
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
** static array using sqliteVdbeAddOpList but we want to make a
** few minor changes to the program.
*/
void sqliteVdbeChangeP1(Vdbe *p, int addr, int val){
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p1 = val;
  }
}

/*
** Change the value of the P2 operand for a specific instruction.
** This routine is useful for setting a jump destination.
*/
void sqliteVdbeChangeP2(Vdbe *p, int addr, int val){
  assert( val>=0 );
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p2 = val;
  }
}

/*
** Change the value of the P3 operand for a specific instruction.
** This routine is useful when a large program is loaded from a
** static array using sqliteVdbeAddOpList but we want to make a
** few minor changes to the program.
**
** If n>=0 then the P3 operand is dynamic, meaning that a copy of
** the string is made into memory obtained from sqliteMalloc().
** A value of n==0 means copy bytes of zP3 up to and including the
** first null byte.  If n>0 then copy n+1 bytes of zP3.
**
** If n==P3_STATIC  it means that zP3 is a pointer to a constant static
** string and we can just copy the pointer.  n==P3_POINTER means zP3 is
** a pointer to some object other than a string.
**
** If addr<0 then change P3 on the most recently inserted instruction.
*/
void sqliteVdbeChangeP3(Vdbe *p, int addr, const char *zP3, int n){
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
  }else if( n<0 ){
    pOp->p3 = (char*)zP3;
    pOp->p3type = n;
  }else{
    sqliteSetNString(&pOp->p3, zP3, n, 0);
    pOp->p3type = P3_DYNAMIC;
  }
}

/*
** If the P3 operand to the specified instruction appears
** to be a quoted string token, then this procedure removes 
** the quotes.
**
** The quoting operator can be either a grave ascent (ASCII 0x27)
** or a double quote character (ASCII 0x22).  Two quotes in a row
** resolve to be a single actual quote character within the string.
*/
void sqliteVdbeDequoteP3(Vdbe *p, int addr){
  Op *pOp;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->aOp==0 ) return;
  if( addr<0 || addr>=p->nOp ){
    addr = p->nOp - 1;
    if( addr<0 ) return;
  }
  pOp = &p->aOp[addr];
  if( pOp->p3==0 || pOp->p3[0]==0 ) return;
  if( pOp->p3type==P3_POINTER ) return;
  if( pOp->p3type!=P3_DYNAMIC ){
    pOp->p3 = sqliteStrDup(pOp->p3);
    pOp->p3type = P3_DYNAMIC;
  }
  sqliteDequote(pOp->p3);
}

/*
** On the P3 argument of the given instruction, change all
** strings of whitespace characters into a single space and
** delete leading and trailing whitespace.
*/
void sqliteVdbeCompressSpace(Vdbe *p, int addr){
  unsigned char *z;
  int i, j;
  Op *pOp;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->aOp==0 || addr<0 || addr>=p->nOp ) return;
  pOp = &p->aOp[addr];
  if( pOp->p3type==P3_POINTER ){
    return;
  }
  if( pOp->p3type!=P3_DYNAMIC ){
    pOp->p3 = sqliteStrDup(pOp->p3);
    pOp->p3type = P3_DYNAMIC;
  }
  z = (unsigned char*)pOp->p3;
  if( z==0 ) return;
  i = j = 0;
  while( isspace(z[i]) ){ i++; }
  while( z[i] ){
    if( isspace(z[i]) ){
      z[j++] = ' ';
      while( isspace(z[++i]) ){}
    }else{
      z[j++] = z[i++];
    }
  }
  while( j>0 && isspace(z[j-1]) ){ j--; }
  z[j] = 0;
}

/*
** Search for the current program for the given opcode and P2
** value.  Return the address plus 1 if found and 0 if not found.
*/
int sqliteVdbeFindOp(Vdbe *p, int op, int p2){
  int i;
  assert( p->magic==VDBE_MAGIC_INIT );
  for(i=0; i<p->nOp; i++){
    if( p->aOp[i].opcode==op && p->aOp[i].p2==p2 ) return i+1;
  }
  return 0;
}

/*
** Return the opcode for a given address.
*/
VdbeOp *sqliteVdbeGetOp(Vdbe *p, int addr){
  assert( p->magic==VDBE_MAGIC_INIT );
  assert( addr>=0 && addr<p->nOp );
  return &p->aOp[addr];
}

/*
** The following group or routines are employed by installable functions
** to return their results.
**
** The sqlite_set_result_string() routine can be used to return a string
** value or to return a NULL.  To return a NULL, pass in NULL for zResult.
** A copy is made of the string before this routine returns so it is safe
** to pass in an ephemeral string.
**
** sqlite_set_result_error() works like sqlite_set_result_string() except
** that it signals a fatal error.  The string argument, if any, is the
** error message.  If the argument is NULL a generic substitute error message
** is used.
**
** The sqlite_set_result_int() and sqlite_set_result_double() set the return
** value of the user function to an integer or a double.
**
** These routines are defined here in vdbe.c because they depend on knowing
** the internals of the sqlite_func structure which is only defined in 
** this source file.
*/
char *sqlite_set_result_string(sqlite_func *p, const char *zResult, int n){
  assert( !p->isStep );
  if( p->s.flags & MEM_Dyn ){
    sqliteFree(p->s.z);
  }
  if( zResult==0 ){
    p->s.flags = MEM_Null;
    n = 0;
    p->s.z = 0;
    p->s.n = 0;
  }else{
    if( n<0 ) n = strlen(zResult);
    if( n<NBFS-1 ){
      memcpy(p->s.zShort, zResult, n);
      p->s.zShort[n] = 0;
      p->s.flags = MEM_Str | MEM_Short;
      p->s.z = p->s.zShort;
    }else{
      p->s.z = sqliteMallocRaw( n+1 );
      if( p->s.z ){
        memcpy(p->s.z, zResult, n);
        p->s.z[n] = 0;
      }
      p->s.flags = MEM_Str | MEM_Dyn;
    }
    p->s.n = n+1;
  }
  return p->s.z;
}
void sqlite_set_result_int(sqlite_func *p, int iResult){
  assert( !p->isStep );
  if( p->s.flags & MEM_Dyn ){
    sqliteFree(p->s.z);
  }
  p->s.i = iResult;
  p->s.flags = MEM_Int;
}
void sqlite_set_result_double(sqlite_func *p, double rResult){
  assert( !p->isStep );
  if( p->s.flags & MEM_Dyn ){
    sqliteFree(p->s.z);
  }
  p->s.r = rResult;
  p->s.flags = MEM_Real;
}
void sqlite_set_result_error(sqlite_func *p, const char *zMsg, int n){
  assert( !p->isStep );
  sqlite_set_result_string(p, zMsg, n);
  p->isError = 1;
}

/*
** Extract the user data from a sqlite_func structure and return a
** pointer to it.
*/
void *sqlite_user_data(sqlite_func *p){
  assert( p && p->pFunc );
  return p->pFunc->pUserData;
}

/*
** Allocate or return the aggregate context for a user function.  A new
** context is allocated on the first call.  Subsequent calls return the
** same context that was returned on prior calls.
**
** This routine is defined here in vdbe.c because it depends on knowing
** the internals of the sqlite_func structure which is only defined in
** this source file.
*/
void *sqlite_aggregate_context(sqlite_func *p, int nByte){
  assert( p && p->pFunc && p->pFunc->xStep );
  if( p->pAgg==0 ){
    if( nByte<=NBFS ){
      p->pAgg = (void*)p->s.z;
      memset(p->pAgg, 0, nByte);
    }else{
      p->pAgg = sqliteMalloc( nByte );
    }
  }
  return p->pAgg;
}

/*
** Return the number of times the Step function of a aggregate has been 
** called.
**
** This routine is defined here in vdbe.c because it depends on knowing
** the internals of the sqlite_func structure which is only defined in
** this source file.
*/
int sqlite_aggregate_count(sqlite_func *p){
  assert( p && p->pFunc && p->pFunc->xStep );
  return p->cnt;
}

#if !defined(NDEBUG) || defined(VDBE_PROFILE)
/*
** Print a single opcode.  This routine is used for debugging only.
*/
void sqliteVdbePrintOp(FILE *pOut, int pc, Op *pOp){
  char *zP3;
  char zPtr[40];
  if( pOp->p3type==P3_POINTER ){
    sprintf(zPtr, "ptr(%#lx)", (long)pOp->p3);
    zP3 = zPtr;
  }else{
    zP3 = pOp->p3;
  }
  if( pOut==0 ) pOut = stdout;
  fprintf(pOut,"%4d %-12s %4d %4d %s\n",
      pc, sqliteOpcodeNames[pOp->opcode], pOp->p1, pOp->p2, zP3 ? zP3 : "");
  fflush(pOut);
}
#endif

/*
** Give a listing of the program in the virtual machine.
**
** The interface is the same as sqliteVdbeExec().  But instead of
** running the code, it invokes the callback once for each instruction.
** This feature is used to implement "EXPLAIN".
*/
int sqliteVdbeList(
  Vdbe *p                   /* The VDBE */
){
  sqlite *db = p->db;
  int i;
  int rc = SQLITE_OK;
  static char *azColumnNames[] = {
     "addr", "opcode", "p1",  "p2",  "p3", 
     "int",  "text",   "int", "int", "text",
     0
  };

  assert( p->popStack==0 );
  assert( p->explain );
  p->azColName = azColumnNames;
  p->azResColumn = p->zArgv;
  for(i=0; i<5; i++) p->zArgv[i] = p->aStack[i].zShort;
  i = p->pc;
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
    sqliteSetString(&p->zErrMsg, sqlite_error_string(p->rc), (char*)0);
  }else{
    sprintf(p->zArgv[0],"%d",i);
    sprintf(p->zArgv[2],"%d", p->aOp[i].p1);
    sprintf(p->zArgv[3],"%d", p->aOp[i].p2);
    if( p->aOp[i].p3type==P3_POINTER ){
      sprintf(p->aStack[4].zShort, "ptr(%#lx)", (long)p->aOp[i].p3);
      p->zArgv[4] = p->aStack[4].zShort;
    }else{
      p->zArgv[4] = p->aOp[i].p3;
    }
    p->zArgv[1] = sqliteOpcodeNames[p->aOp[i].opcode];
    p->pc = i+1;
    p->azResColumn = p->zArgv;
    p->nResColumn = 5;
    p->rc = SQLITE_OK;
    rc = SQLITE_ROW;
  }
  return rc;
}

/*
** Prepare a virtual machine for execution.  This involves things such
** as allocating stack space and initializing the program counter.
** After the VDBE has be prepped, it can be executed by one or more
** calls to sqliteVdbeExec().  
*/
void sqliteVdbeMakeReady(
  Vdbe *p,                       /* The VDBE */
  int nVar,                      /* Number of '?' see in the SQL statement */
  int isExplain                  /* True if the EXPLAIN keywords is present */
){
  int n;

  assert( p!=0 );
  assert( p->magic==VDBE_MAGIC_INIT );

  /* Add a HALT instruction to the very end of the program.
  */
  if( p->nOp==0 || (p->aOp && p->aOp[p->nOp-1].opcode!=OP_Halt) ){
    sqliteVdbeAddOp(p, OP_Halt, 0, 0);
  }

  /* No instruction ever pushes more than a single element onto the
  ** stack.  And the stack never grows on successive executions of the
  ** same loop.  So the total number of instructions is an upper bound
  ** on the maximum stack depth required.
  **
  ** Allocation all the stack space we will ever need.
  */
  if( p->aStack==0 ){
    p->nVar = nVar;
    assert( nVar>=0 );
    n = isExplain ? 10 : p->nOp;
    p->aStack = sqliteMalloc(
      n*(sizeof(p->aStack[0]) + 2*sizeof(char*))     /* aStack and zArgv */
        + p->nVar*(sizeof(char*)+sizeof(int)+1)    /* azVar, anVar, abVar */
    );
    p->zArgv = (char**)&p->aStack[n];
    p->azColName = (char**)&p->zArgv[n];
    p->azVar = (char**)&p->azColName[n];
    p->anVar = (int*)&p->azVar[p->nVar];
    p->abVar = (u8*)&p->anVar[p->nVar];
  }

  sqliteHashInit(&p->agg.hash, SQLITE_HASH_BINARY, 0);
  p->agg.pSearch = 0;
#ifdef MEMORY_DEBUG
  if( sqliteOsFileExists("vdbe_trace") ){
    p->trace = stdout;
  }
#endif
  p->pTos = &p->aStack[-1];
  p->pc = 0;
  p->rc = SQLITE_OK;
  p->uniqueCnt = 0;
  p->returnDepth = 0;
  p->errorAction = OE_Abort;
  p->undoTransOnError = 0;
  p->popStack =  0;
  p->explain |= isExplain;
  p->magic = VDBE_MAGIC_RUN;
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
void sqliteVdbeSorterReset(Vdbe *p){
  while( p->pSort ){
    Sorter *pSorter = p->pSort;
    p->pSort = pSorter->pNext;
    sqliteFree(pSorter->zKey);
    sqliteFree(pSorter->pData);
    sqliteFree(pSorter);
  }
}

/*
** Reset an Agg structure.  Delete all its contents. 
**
** For installable aggregate functions, if the step function has been
** called, make sure the finalizer function has also been called.  The
** finalizer might need to free memory that was allocated as part of its
** private context.  If the finalizer has not been called yet, call it
** now.
*/
void sqliteVdbeAggReset(Agg *pAgg){
  int i;
  HashElem *p;
  for(p = sqliteHashFirst(&pAgg->hash); p; p = sqliteHashNext(p)){
    AggElem *pElem = sqliteHashData(p);
    assert( pAgg->apFunc!=0 );
    for(i=0; i<pAgg->nMem; i++){
      Mem *pMem = &pElem->aMem[i];
      if( pAgg->apFunc[i] && (pMem->flags & MEM_AggCtx)!=0 ){
        sqlite_func ctx;
        ctx.pFunc = pAgg->apFunc[i];
        ctx.s.flags = MEM_Null;
        ctx.pAgg = pMem->z;
        ctx.cnt = pMem->i;
        ctx.isStep = 0;
        ctx.isError = 0;
        (*pAgg->apFunc[i]->xFinalize)(&ctx);
        if( pMem->z!=0 && pMem->z!=pMem->zShort ){
          sqliteFree(pMem->z);
        }
        if( ctx.s.flags & MEM_Dyn ){
          sqliteFree(ctx.s.z);
        }
      }else if( pMem->flags & MEM_Dyn ){
        sqliteFree(pMem->z);
      }
    }
    sqliteFree(pElem);
  }
  sqliteHashClear(&pAgg->hash);
  sqliteFree(pAgg->apFunc);
  pAgg->apFunc = 0;
  pAgg->pCurrent = 0;
  pAgg->pSearch = 0;
  pAgg->nMem = 0;
}

/*
** Delete a keylist
*/
void sqliteVdbeKeylistFree(Keylist *p){
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
void sqliteVdbeCleanupCursor(Cursor *pCx){
  if( pCx->pCursor ){
    sqliteBtreeCloseCursor(pCx->pCursor);
  }
  if( pCx->pBt ){
    sqliteBtreeClose(pCx->pBt);
  }
  sqliteFree(pCx->pData);
  memset(pCx, 0, sizeof(Cursor));
}

/*
** Close all cursors
*/
static void closeAllCursors(Vdbe *p){
  int i;
  for(i=0; i<p->nCursor; i++){
    sqliteVdbeCleanupCursor(&p->aCsr[i]);
  }
  sqliteFree(p->aCsr);
  p->aCsr = 0;
  p->nCursor = 0;
}

/*
** Clean up the VM after execution.
**
** This routine will automatically close any cursors, lists, and/or
** sorters that were left open.  It also deletes the values of
** variables in the azVariable[] array.
*/
static void Cleanup(Vdbe *p){
  int i;
  if( p->aStack ){
    Mem *pTos = p->pTos;
    while( pTos>=p->aStack ){
      if( pTos->flags & MEM_Dyn ){
        sqliteFree(pTos->z);
      }
      pTos--;
    }
    p->pTos = pTos;
  }
  closeAllCursors(p);
  if( p->aMem ){
    for(i=0; i<p->nMem; i++){
      if( p->aMem[i].flags & MEM_Dyn ){
        sqliteFree(p->aMem[i].z);
      }
    }
  }
  sqliteFree(p->aMem);
  p->aMem = 0;
  p->nMem = 0;
  if( p->pList ){
    sqliteVdbeKeylistFree(p->pList);
    p->pList = 0;
  }
  sqliteVdbeSorterReset(p);
  if( p->pFile ){
    if( p->pFile!=stdin ) fclose(p->pFile);
    p->pFile = 0;
  }
  if( p->azField ){
    sqliteFree(p->azField);
    p->azField = 0;
  }
  p->nField = 0;
  if( p->zLine ){
    sqliteFree(p->zLine);
    p->zLine = 0;
  }
  p->nLineAlloc = 0;
  sqliteVdbeAggReset(&p->agg);
  if( p->aSet ){
    for(i=0; i<p->nSet; i++){
      sqliteHashClear(&p->aSet[i].hash);
    }
  }
  sqliteFree(p->aSet);
  p->aSet = 0;
  p->nSet = 0;
  if( p->keylistStack ){
    int ii;
    for(ii = 0; ii < p->keylistStackDepth; ii++){
      sqliteVdbeKeylistFree(p->keylistStack[ii]);
    }
    sqliteFree(p->keylistStack);
    p->keylistStackDepth = 0;
    p->keylistStack = 0;
  }
  sqliteFree(p->contextStack);
  p->contextStack = 0;
  sqliteFree(p->zErrMsg);
  p->zErrMsg = 0;
}

/*
** Clean up a VDBE after execution but do not delete the VDBE just yet.
** Write any error messages into *pzErrMsg.  Return the result code.
**
** After this routine is run, the VDBE should be ready to be executed
** again.
*/
int sqliteVdbeReset(Vdbe *p, char **pzErrMsg){
  sqlite *db = p->db;
  int i;

  if( p->magic!=VDBE_MAGIC_RUN && p->magic!=VDBE_MAGIC_HALT ){
    sqliteSetString(pzErrMsg, sqlite_error_string(SQLITE_MISUSE), (char*)0);
    return SQLITE_MISUSE;
  }
  if( p->zErrMsg ){
    if( pzErrMsg && *pzErrMsg==0 ){
      *pzErrMsg = p->zErrMsg;
    }else{
      sqliteFree(p->zErrMsg);
    }
    p->zErrMsg = 0;
  }else if( p->rc ){
    sqliteSetString(pzErrMsg, sqlite_error_string(p->rc), (char*)0);
  }
  Cleanup(p);
  if( p->rc!=SQLITE_OK ){
    switch( p->errorAction ){
      case OE_Abort: {
        if( !p->undoTransOnError ){
          for(i=0; i<db->nDb; i++){
            if( db->aDb[i].pBt ){
              sqliteBtreeRollbackCkpt(db->aDb[i].pBt);
            }
          }
          break;
        }
        /* Fall through to ROLLBACK */
      }
      case OE_Rollback: {
        sqliteRollbackAll(db);
        db->flags &= ~SQLITE_InTrans;
        db->onError = OE_Default;
        break;
      }
      default: {
        if( p->undoTransOnError ){
          sqliteRollbackAll(db);
          db->flags &= ~SQLITE_InTrans;
          db->onError = OE_Default;
        }
        break;
      }
    }
    sqliteRollbackInternalChanges(db);
  }
  for(i=0; i<db->nDb; i++){
    if( db->aDb[i].pBt && db->aDb[i].inTrans==2 ){
      sqliteBtreeCommitCkpt(db->aDb[i].pBt);
      db->aDb[i].inTrans = 1;
    }
  }
  assert( p->pTos<&p->aStack[p->pc] || sqlite_malloc_failed==1 );
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
        sqliteVdbePrintOp(out, i, &p->aOp[i]);
      }
      fclose(out);
    }
  }
#endif
  p->magic = VDBE_MAGIC_INIT;
  return p->rc;
}

/*
** Clean up and delete a VDBE after execution.  Return an integer which is
** the result code.  Write any error message text into *pzErrMsg.
*/
int sqliteVdbeFinalize(Vdbe *p, char **pzErrMsg){
  int rc;
  sqlite *db;

  if( p->magic!=VDBE_MAGIC_RUN && p->magic!=VDBE_MAGIC_HALT ){
    sqliteSetString(pzErrMsg, sqlite_error_string(SQLITE_MISUSE), (char*)0);
    return SQLITE_MISUSE;
  }
  db = p->db;
  rc = sqliteVdbeReset(p, pzErrMsg);
  sqliteVdbeDelete(p);
  if( db->want_to_close && db->pVdbe==0 ){
    sqlite_close(db);
  }
  if( rc==SQLITE_SCHEMA ){
    sqliteResetInternalSchema(db, 0);
  }
  return rc;
}

/*
** Set the values of all variables.  Variable $1 in the original SQL will
** be the string azValue[0].  $2 will have the value azValue[1].  And
** so forth.  If a value is out of range (for example $3 when nValue==2)
** then its value will be NULL.
**
** This routine overrides any prior call.
*/
int sqlite_bind(sqlite_vm *pVm, int i, const char *zVal, int len, int copy){
  Vdbe *p = (Vdbe*)pVm;
  if( p->magic!=VDBE_MAGIC_RUN || p->pc!=0 ){
    return SQLITE_MISUSE;
  }
  if( i<1 || i>p->nVar ){
    return SQLITE_RANGE;
  }
  i--;
  if( p->abVar[i] ){
    sqliteFree(p->azVar[i]);
  }
  if( zVal==0 ){
    copy = 0;
    len = 0;
  }
  if( len<0 ){
    len = strlen(zVal)+1;
  }
  if( copy ){
    p->azVar[i] = sqliteMalloc( len );
    if( p->azVar[i] ) memcpy(p->azVar[i], zVal, len);
  }else{
    p->azVar[i] = (char*)zVal;
  }
  p->abVar[i] = copy;
  p->anVar[i] = len;
  return SQLITE_OK;
}


/*
** Delete an entire VDBE.
*/
void sqliteVdbeDelete(Vdbe *p){
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
  p->pPrev = p->pNext = 0;
  if( p->nOpAlloc==0 ){
    p->aOp = 0;
    p->nOp = 0;
  }
  for(i=0; i<p->nOp; i++){
    if( p->aOp[i].p3type==P3_DYNAMIC ){
      sqliteFree(p->aOp[i].p3);
    }
  }
  for(i=0; i<p->nVar; i++){
    if( p->abVar[i] ) sqliteFree(p->azVar[i]);
  }
  sqliteFree(p->aOp);
  sqliteFree(p->aLabel);
  sqliteFree(p->aStack);
  p->magic = VDBE_MAGIC_DEAD;
  sqliteFree(p);
}

/*
** Convert an integer in between the native integer format and
** the bigEndian format used as the record number for tables.
**
** The bigEndian format (most significant byte first) is used for
** record numbers so that records will sort into the correct order
** even though memcmp() is used to compare the keys.  On machines
** whose native integer format is little endian (ex: i486) the
** order of bytes is reversed.  On native big-endian machines
** (ex: Alpha, Sparc, Motorola) the byte order is the same.
**
** This function is its own inverse.  In other words
**
**         X == byteSwap(byteSwap(X))
*/
int sqliteVdbeByteSwap(int x){
  union {
     char zBuf[sizeof(int)];
     int i;
  } ux;
  ux.zBuf[3] = x&0xff;
  ux.zBuf[2] = (x>>8)&0xff;
  ux.zBuf[1] = (x>>16)&0xff;
  ux.zBuf[0] = (x>>24)&0xff;
  return ux.i;
}

/*
** If a MoveTo operation is pending on the given cursor, then do that
** MoveTo now.  Return an error code.  If no MoveTo is pending, this
** routine does nothing and returns SQLITE_OK.
*/
int sqliteVdbeCursorMoveto(Cursor *p){
  if( p->deferredMoveto ){
    int res;
    extern int sqlite_search_count;
    sqliteBtreeMoveto(p->pCursor, (char*)&p->movetoTarget, sizeof(int), &res);
    p->lastRecno = keyToInt(p->movetoTarget);
    p->recnoIsValid = res==0;
    if( res<0 ){
      sqliteBtreeNext(p->pCursor, &res);
    }
    sqlite_search_count++;
    p->deferredMoveto = 0;
  }
  return SQLITE_OK;
}
