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
#include <ctype.h>
#include "vdbeInt.h"



/*
** When debugging the code generator in a symbolic debugger, one can
** set the sqlite3_vdbe_addop_trace to 1 and all opcodes will be printed
** as they are added to the instruction stream.
*/
#ifdef SQLITE_DEBUG
int sqlite3_vdbe_addop_trace = 0;
#endif


/*
** Create a new virtual database engine.
*/
Vdbe *sqlite3VdbeCreate(sqlite3 *db){
  Vdbe *p;
  p = sqlite3DbMallocZero(db, sizeof(Vdbe) );
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
** Remember the SQL string for a prepared statement.
*/
void sqlite3VdbeSetSql(Vdbe *p, const char *z, int n){
  if( p==0 ) return;
  assert( p->zSql==0 );
  p->zSql = sqlite3DbStrNDup(p->db, z, n);
}

/*
** Return the SQL associated with a prepared statement
*/
const char *sqlite3_sql(sqlite3_stmt *pStmt){
  return ((Vdbe *)pStmt)->zSql;
}

/*
** Swap all content between two VDBE structures.
*/
void sqlite3VdbeSwap(Vdbe *pA, Vdbe *pB){
  Vdbe tmp, *pTmp;
  char *zTmp;
  int nTmp;
  tmp = *pA;
  *pA = *pB;
  *pB = tmp;
  pTmp = pA->pNext;
  pA->pNext = pB->pNext;
  pB->pNext = pTmp;
  pTmp = pA->pPrev;
  pA->pPrev = pB->pPrev;
  pB->pPrev = pTmp;
  zTmp = pA->zSql;
  pA->zSql = pB->zSql;
  pB->zSql = zTmp;
  nTmp = pA->nSql;
  pA->nSql = pB->nSql;
  pB->nSql = nTmp;
}

#ifdef SQLITE_DEBUG
/*
** Turn tracing on or off
*/
void sqlite3VdbeTrace(Vdbe *p, FILE *trace){
  p->trace = trace;
}
#endif

/*
** Resize the Vdbe.aOp array so that it contains at least N
** elements.
**
** If an out-of-memory error occurs while resizing the array,
** Vdbe.aOp and Vdbe.nOpAlloc remain unchanged (this is so that
** any opcodes already allocated can be correctly deallocated
** along with the rest of the Vdbe).
*/
static void resizeOpArray(Vdbe *p, int N){
  VdbeOp *pNew;
  int oldSize = p->nOpAlloc;
  pNew = sqlite3DbRealloc(p->db, p->aOp, N*sizeof(Op));
  if( pNew ){
    p->nOpAlloc = N;
    p->aOp = pNew;
    if( N>oldSize ){
      memset(&p->aOp[oldSize], 0, (N-oldSize)*sizeof(Op));
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
**    p1, p2, p3      Operands
**
** Use the sqlite3VdbeResolveLabel() function to fix an address and
** the sqlite3VdbeChangeP4() function to change the value of the P4
** operand.
*/
int sqlite3VdbeAddOp3(Vdbe *p, int op, int p1, int p2, int p3){
  int i;
  VdbeOp *pOp;

  i = p->nOp;
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->nOpAlloc<=i ){
    resizeOpArray(p, p->nOpAlloc*2 + 100);
    if( p->db->mallocFailed ){
      return 0;
    }
  }
  p->nOp++;
  pOp = &p->aOp[i];
  pOp->opcode = op;
  pOp->p1 = p1;
  pOp->p2 = p2;
  pOp->p3 = p3;
  pOp->p4.p = 0;
  pOp->p4type = P4_NOTUSED;
  p->expired = 0;
#ifdef SQLITE_DEBUG
  if( sqlite3_vdbe_addop_trace ) sqlite3VdbePrintOp(0, i, &p->aOp[i]);
#endif
  return i;
}
int sqlite3VdbeAddOp0(Vdbe *p, int op){
  return sqlite3VdbeAddOp3(p, op, 0, 0, 0);
}
int sqlite3VdbeAddOp1(Vdbe *p, int op, int p1){
  return sqlite3VdbeAddOp3(p, op, p1, 0, 0);
}
int sqlite3VdbeAddOp2(Vdbe *p, int op, int p1, int p2){
  return sqlite3VdbeAddOp3(p, op, p1, p2, 0);
}


/*
** Add an opcode that includes the p4 value as a pointer.
*/
int sqlite3VdbeAddOp4(
  Vdbe *p,            /* Add the opcode to this VM */
  int op,             /* The new opcode */
  int p1,             /* The P1 operand */
  int p2,             /* The P2 operand */
  int p3,             /* The P3 operand */
  const char *zP4,    /* The P4 operand */
  int p4type          /* P4 operand type */
){
  int addr = sqlite3VdbeAddOp3(p, op, p1, p2, p3);
  sqlite3VdbeChangeP4(p, addr, zP4, p4type);
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
    p->aLabel = sqlite3DbReallocOrFree(p->db, p->aLabel,
                                    p->nLabelAlloc*sizeof(p->aLabel[0]));
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
** Loop through the program looking for P2 values that are negative
** on jump instructions.  Each such value is a label.  Resolve the
** label by setting the P2 value to its correct non-zero value.
**
** This routine is called once after all opcodes have been inserted.
**
** Variable *pMaxFuncArgs is set to the maximum value of any P2 argument 
** to an OP_Function, OP_AggStep or OP_VFilter opcode. This is used by 
** sqlite3VdbeMakeReady() to size the Vdbe.apArg[] array.
**
** This routine also does the following optimization:  It scans for
** instructions that might cause a statement rollback.  Such instructions
** are:
**
**   *  OP_Halt with P1=SQLITE_CONSTRAINT and P2=OE_Abort.
**   *  OP_Destroy
**   *  OP_VUpdate
**   *  OP_VRename
**
** If no such instruction is found, then every Statement instruction 
** is changed to a Noop.  In this way, we avoid creating the statement 
** journal file unnecessarily.
*/
static void resolveP2Values(Vdbe *p, int *pMaxFuncArgs){
  int i;
  int nMaxArgs = 0;
  Op *pOp;
  int *aLabel = p->aLabel;
  int doesStatementRollback = 0;
  int hasStatementBegin = 0;
  for(pOp=p->aOp, i=p->nOp-1; i>=0; i--, pOp++){
    u8 opcode = pOp->opcode;

    if( opcode==OP_Function ){
      if( pOp->p5>nMaxArgs ) nMaxArgs = pOp->p5;
    }else if( opcode==OP_AggStep 
#ifndef SQLITE_OMIT_VIRTUALTABLE
        || opcode==OP_VUpdate
#endif
    ){
      if( pOp->p2>nMaxArgs ) nMaxArgs = pOp->p2;
    }
    if( opcode==OP_Halt ){
      if( pOp->p1==SQLITE_CONSTRAINT && pOp->p2==OE_Abort ){
        doesStatementRollback = 1;
      }
    }else if( opcode==OP_Statement ){
      hasStatementBegin = 1;
    }else if( opcode==OP_Destroy ){
      doesStatementRollback = 1;
#ifndef SQLITE_OMIT_VIRTUALTABLE
    }else if( opcode==OP_VUpdate || opcode==OP_VRename ){
      doesStatementRollback = 1;
    }else if( opcode==OP_VFilter ){
      int n;
      assert( p->nOp - i >= 3 );
      assert( pOp[-1].opcode==OP_Integer );
      n = pOp[-1].p1;
      if( n>nMaxArgs ) nMaxArgs = n;
#endif
    }

    if( sqlite3VdbeOpcodeHasProperty(opcode, OPFLG_JUMP) && pOp->p2<0 ){
      assert( -1-pOp->p2<p->nLabel );
      pOp->p2 = aLabel[-1-pOp->p2];
    }
  }
  sqlite3_free(p->aLabel);
  p->aLabel = 0;

  *pMaxFuncArgs = nMaxArgs;

  /* If we never rollback a statement transaction, then statement
  ** transactions are not needed.  So change every OP_Statement
  ** opcode into an OP_Noop.  This avoid a call to sqlite3OsOpenExclusive()
  ** which can be expensive on some platforms.
  */
  if( hasStatementBegin && !doesStatementRollback ){
    for(pOp=p->aOp, i=p->nOp-1; i>=0; i--, pOp++){
      if( pOp->opcode==OP_Statement ){
        pOp->opcode = OP_Noop;
      }
    }
  }
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
  if( p->nOp + nOp > p->nOpAlloc ){
    resizeOpArray(p, p->nOp*2 + nOp);
  }
  if( p->db->mallocFailed ){
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
      if( p2<0 && sqlite3VdbeOpcodeHasProperty(pOut->opcode, OPFLG_JUMP) ){
        pOut->p2 = addr + ADDR(p2);
      }else{
        pOut->p2 = p2;
      }
      pOut->p3 = pIn->p3;
      pOut->p4type = P4_NOTUSED;
      pOut->p4.p = 0;
      pOut->p5 = 0;
#ifdef SQLITE_DEBUG
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
  assert( p==0 || p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p1 = val;
  }
}

/*
** Change the value of the P2 operand for a specific instruction.
** This routine is useful for setting a jump destination.
*/
void sqlite3VdbeChangeP2(Vdbe *p, int addr, int val){
  assert( p==0 || p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p2 = val;
  }
}

/*
** Change the value of the P3 operand for a specific instruction.
*/
void sqlite3VdbeChangeP3(Vdbe *p, int addr, int val){
  assert( p==0 || p->magic==VDBE_MAGIC_INIT );
  if( p && addr>=0 && p->nOp>addr && p->aOp ){
    p->aOp[addr].p3 = val;
  }
}

/*
** Change the value of the P5 operand for the most recently
** added operation.
*/
void sqlite3VdbeChangeP5(Vdbe *p, u8 val){
  assert( p==0 || p->magic==VDBE_MAGIC_INIT );
  if( p && p->aOp ){
    assert( p->nOp>0 );
    p->aOp[p->nOp-1].p5 = val;
  }
}

/*
** Change the P2 operand of instruction addr so that it points to
** the address of the next instruction to be coded.
*/
void sqlite3VdbeJumpHere(Vdbe *p, int addr){
  sqlite3VdbeChangeP2(p, addr, p->nOp);
}


/*
** If the input FuncDef structure is ephemeral, then free it.  If
** the FuncDef is not ephermal, then do nothing.
*/
static void freeEphemeralFunction(FuncDef *pDef){
  if( pDef && (pDef->flags & SQLITE_FUNC_EPHEM)!=0 ){
    sqlite3_free(pDef);
  }
}

/*
** Delete a P4 value if necessary.
*/
static void freeP4(int p4type, void *p3){
  if( p3 ){
    switch( p4type ){
      case P4_REAL:
      case P4_INT64:
      case P4_MPRINTF:
      case P4_DYNAMIC:
      case P4_KEYINFO:
      case P4_KEYINFO_HANDOFF: {
        sqlite3_free(p3);
        break;
      }
      case P4_VDBEFUNC: {
        VdbeFunc *pVdbeFunc = (VdbeFunc *)p3;
        freeEphemeralFunction(pVdbeFunc->pFunc);
        sqlite3VdbeDeleteAuxData(pVdbeFunc, 0);
        sqlite3_free(pVdbeFunc);
        break;
      }
      case P4_FUNCDEF: {
        freeEphemeralFunction((FuncDef*)p3);
        break;
      }
      case P4_MEM: {
        sqlite3ValueFree((sqlite3_value*)p3);
        break;
      }
    }
  }
}


/*
** Change N opcodes starting at addr to No-ops.
*/
void sqlite3VdbeChangeToNoop(Vdbe *p, int addr, int N){
  if( p && p->aOp ){
    VdbeOp *pOp = &p->aOp[addr];
    while( N-- ){
      freeP4(pOp->p4type, pOp->p4.p);
      memset(pOp, 0, sizeof(pOp[0]));
      pOp->opcode = OP_Noop;
      pOp++;
    }
  }
}

/*
** Change the value of the P4 operand for a specific instruction.
** This routine is useful when a large program is loaded from a
** static array using sqlite3VdbeAddOpList but we want to make a
** few minor changes to the program.
**
** If n>=0 then the P4 operand is dynamic, meaning that a copy of
** the string is made into memory obtained from sqlite3_malloc().
** A value of n==0 means copy bytes of zP4 up to and including the
** first null byte.  If n>0 then copy n+1 bytes of zP4.
**
** If n==P4_KEYINFO it means that zP4 is a pointer to a KeyInfo structure.
** A copy is made of the KeyInfo structure into memory obtained from
** sqlite3_malloc, to be freed when the Vdbe is finalized.
** n==P4_KEYINFO_HANDOFF indicates that zP4 points to a KeyInfo structure
** stored in memory that the caller has obtained from sqlite3_malloc. The 
** caller should not free the allocation, it will be freed when the Vdbe is
** finalized.
** 
** Other values of n (P4_STATIC, P4_COLLSEQ etc.) indicate that zP4 points
** to a string or structure that is guaranteed to exist for the lifetime of
** the Vdbe. In these cases we can just copy the pointer.
**
** If addr<0 then change P4 on the most recently inserted instruction.
*/
void sqlite3VdbeChangeP4(Vdbe *p, int addr, const char *zP4, int n){
  Op *pOp;
  assert( p!=0 );
  assert( p->magic==VDBE_MAGIC_INIT );
  if( p->aOp==0 || p->db->mallocFailed ){
    if (n != P4_KEYINFO) {
      freeP4(n, (void*)*(char**)&zP4);
    }
    return;
  }
  assert( addr<p->nOp );
  if( addr<0 ){
    addr = p->nOp - 1;
    if( addr<0 ) return;
  }
  pOp = &p->aOp[addr];
  freeP4(pOp->p4type, pOp->p4.p);
  pOp->p4.p = 0;
  if( n==P4_INT32 ){
    pOp->p4.i = (int)zP4;
    pOp->p4type = n;
  }else if( zP4==0 ){
    pOp->p4.p = 0;
    pOp->p4type = P4_NOTUSED;
  }else if( n==P4_KEYINFO ){
    KeyInfo *pKeyInfo;
    int nField, nByte;

    nField = ((KeyInfo*)zP4)->nField;
    nByte = sizeof(*pKeyInfo) + (nField-1)*sizeof(pKeyInfo->aColl[0]) + nField;
    pKeyInfo = sqlite3_malloc( nByte );
    pOp->p4.pKeyInfo = pKeyInfo;
    if( pKeyInfo ){
      memcpy(pKeyInfo, zP4, nByte);
      /* In the current implementation, P4_KEYINFO is only ever used on
      ** KeyInfo structures that have no aSortOrder component.  Elements
      ** with an aSortOrder always use P4_KEYINFO_HANDOFF.  So we do not
      ** need to bother with duplicating the aSortOrder. */
      assert( pKeyInfo->aSortOrder==0 );
#if 0
      aSortOrder = pKeyInfo->aSortOrder;
      if( aSortOrder ){
        pKeyInfo->aSortOrder = (unsigned char*)&pKeyInfo->aColl[nField];
        memcpy(pKeyInfo->aSortOrder, aSortOrder, nField);
      }
#endif
      pOp->p4type = P4_KEYINFO;
    }else{
      p->db->mallocFailed = 1;
      pOp->p4type = P4_NOTUSED;
    }
  }else if( n==P4_KEYINFO_HANDOFF ){
    pOp->p4.p = (void*)zP4;
    pOp->p4type = P4_KEYINFO;
  }else if( n<0 ){
    pOp->p4.p = (void*)zP4;
    pOp->p4type = n;
  }else{
    if( n==0 ) n = strlen(zP4);
    pOp->p4.z = sqlite3DbStrNDup(p->db, zP4, n);
    pOp->p4type = P4_DYNAMIC;
  }
}

#ifndef NDEBUG
/*
** Change the comment on the the most recently coded instruction.
*/
void sqlite3VdbeComment(Vdbe *p, const char *zFormat, ...){
  va_list ap;
  assert( p->nOp>0 || p->aOp==0 );
  assert( p->aOp==0 || p->aOp[p->nOp-1].zComment==0 || p->db->mallocFailed );
  if( p->nOp ){
    char **pz = &p->aOp[p->nOp-1].zComment;
    va_start(ap, zFormat);
    sqlite3_free(*pz);
    *pz = sqlite3VMPrintf(p->db, zFormat, ap);
    va_end(ap);
  }
}
#endif

/*
** Return the opcode for a given address.
*/
VdbeOp *sqlite3VdbeGetOp(Vdbe *p, int addr){
  assert( p->magic==VDBE_MAGIC_INIT );
  assert( (addr>=0 && addr<p->nOp) || p->db->mallocFailed );
  return ((addr>=0 && addr<p->nOp)?(&p->aOp[addr]):0);
}

#if !defined(SQLITE_OMIT_EXPLAIN) || !defined(NDEBUG) \
     || defined(VDBE_PROFILE) || defined(SQLITE_DEBUG)
/*
** Compute a string that describes the P4 parameter for an opcode.
** Use zTemp for any required temporary buffer space.
*/
static char *displayP4(Op *pOp, char *zTemp, int nTemp){
  char *zP4 = zTemp;
  assert( nTemp>=20 );
  switch( pOp->p4type ){
    case P4_KEYINFO: {
      int i, j;
      KeyInfo *pKeyInfo = pOp->p4.pKeyInfo;
      sqlite3_snprintf(nTemp, zTemp, "keyinfo(%d", pKeyInfo->nField);
      i = strlen(zTemp);
      for(j=0; j<pKeyInfo->nField; j++){
        CollSeq *pColl = pKeyInfo->aColl[j];
        if( pColl ){
          int n = strlen(pColl->zName);
          if( i+n>nTemp-6 ){
            memcpy(&zTemp[i],",...",4);
            break;
          }
          zTemp[i++] = ',';
          if( pKeyInfo->aSortOrder && pKeyInfo->aSortOrder[j] ){
            zTemp[i++] = '-';
          }
          memcpy(&zTemp[i], pColl->zName,n+1);
          i += n;
        }else if( i+4<nTemp-6 ){
          memcpy(&zTemp[i],",nil",4);
          i += 4;
        }
      }
      zTemp[i++] = ')';
      zTemp[i] = 0;
      assert( i<nTemp );
      break;
    }
    case P4_COLLSEQ: {
      CollSeq *pColl = pOp->p4.pColl;
      sqlite3_snprintf(nTemp, zTemp, "collseq(%.20s)", pColl->zName);
      break;
    }
    case P4_FUNCDEF: {
      FuncDef *pDef = pOp->p4.pFunc;
      sqlite3_snprintf(nTemp, zTemp, "%s(%d)", pDef->zName, pDef->nArg);
      break;
    }
    case P4_INT64: {
      sqlite3_snprintf(nTemp, zTemp, "%lld", *pOp->p4.pI64);
      break;
    }
    case P4_INT32: {
      sqlite3_snprintf(nTemp, zTemp, "%d", pOp->p4.i);
      break;
    }
    case P4_REAL: {
      sqlite3_snprintf(nTemp, zTemp, "%.16g", *pOp->p4.pReal);
      break;
    }
    case P4_MEM: {
      Mem *pMem = pOp->p4.pMem;
      assert( (pMem->flags & MEM_Null)==0 );
      if( pMem->flags & MEM_Str ){
        zP4 = pMem->z;
      }else if( pMem->flags & MEM_Int ){
        sqlite3_snprintf(nTemp, zTemp, "%lld", pMem->u.i);
      }else if( pMem->flags & MEM_Real ){
        sqlite3_snprintf(nTemp, zTemp, "%.16g", pMem->r);
      }
      break;
    }
#ifndef SQLITE_OMIT_VIRTUALTABLE
    case P4_VTAB: {
      sqlite3_vtab *pVtab = pOp->p4.pVtab;
      sqlite3_snprintf(nTemp, zTemp, "vtab:%p:%p", pVtab, pVtab->pModule);
      break;
    }
#endif
    default: {
      zP4 = pOp->p4.z;
      if( zP4==0 ){
        zP4 = zTemp;
        zTemp[0] = 0;
      }
    }
  }
  assert( zP4!=0 );
  return zP4;
}
#endif

/*
** Declare to the Vdbe that the BTree object at db->aDb[i] is used.
**
*/
void sqlite3VdbeUsesBtree(Vdbe *p, int i){
  int mask;
  assert( i>=0 && i<p->db->nDb );
  assert( i<sizeof(p->btreeMask)*8 );
  mask = 1<<i;
  if( (p->btreeMask & mask)==0 ){
    p->btreeMask |= mask;
    sqlite3BtreeMutexArrayInsert(&p->aMutex, p->db->aDb[i].pBt);
  }
}


#if defined(VDBE_PROFILE) || defined(SQLITE_DEBUG)
/*
** Print a single opcode.  This routine is used for debugging only.
*/
void sqlite3VdbePrintOp(FILE *pOut, int pc, Op *pOp){
  char *zP4;
  char zPtr[50];
  static const char *zFormat1 = "%4d %-13s %4d %4d %4d %-4s %.2X %s\n";
  if( pOut==0 ) pOut = stdout;
  zP4 = displayP4(pOp, zPtr, sizeof(zPtr));
  fprintf(pOut, zFormat1, pc, 
      sqlite3OpcodeName(pOp->opcode), pOp->p1, pOp->p2, pOp->p3, zP4, pOp->p5,
#ifdef SQLITE_DEBUG
      pOp->zComment ? pOp->zComment : ""
#else
      ""
#endif
  );
  fflush(pOut);
}
#endif

/*
** Release an array of N Mem elements
*/
static void releaseMemArray(Mem *p, int N){
  if( p ){
    while( N-->0 ){
      assert( N<2 || p[0].db==p[1].db );
      sqlite3VdbeMemSetNull(p++);
    }
  }
}

#ifndef SQLITE_OMIT_EXPLAIN
/*
** Give a listing of the program in the virtual machine.
**
** The interface is the same as sqlite3VdbeExec().  But instead of
** running the code, it invokes the callback once for each instruction.
** This feature is used to implement "EXPLAIN".
**
** When p->explain==1, each instruction is listed.  When
** p->explain==2, only OP_Explain instructions are listed and these
** are shown in a different format.  p->explain==2 is used to implement
** EXPLAIN QUERY PLAN.
*/
int sqlite3VdbeList(
  Vdbe *p                   /* The VDBE */
){
  sqlite3 *db = p->db;
  int i;
  int rc = SQLITE_OK;
  Mem *pMem = p->pResultSet = &p->aMem[1];

  assert( p->explain );
  if( p->magic!=VDBE_MAGIC_RUN ) return SQLITE_MISUSE;
  assert( db->magic==SQLITE_MAGIC_BUSY );
  assert( p->rc==SQLITE_OK || p->rc==SQLITE_BUSY );

  /* Even though this opcode does not use dynamic strings for
  ** the result, result columns may become dynamic if the user calls
  ** sqlite3_column_text16(), causing a translation to UTF-16 encoding.
  */
  releaseMemArray(pMem, p->nMem);

  do{
    i = p->pc++;
  }while( i<p->nOp && p->explain==2 && p->aOp[i].opcode!=OP_Explain );
  if( i>=p->nOp ){
    p->rc = SQLITE_OK;
    rc = SQLITE_DONE;
  }else if( db->u1.isInterrupted ){
    p->rc = SQLITE_INTERRUPT;
    rc = SQLITE_ERROR;
    sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(p->rc), (char*)0);
  }else{
    Op *pOp = &p->aOp[i];
    if( p->explain==1 ){
      pMem->flags = MEM_Int;
      pMem->type = SQLITE_INTEGER;
      pMem->u.i = i;                                /* Program counter */
      pMem++;
  
      pMem->flags = MEM_Static|MEM_Str|MEM_Term;
      pMem->z = (char*)sqlite3OpcodeName(pOp->opcode);  /* Opcode */
      assert( pMem->z!=0 );
      pMem->n = strlen(pMem->z);
      pMem->type = SQLITE_TEXT;
      pMem->enc = SQLITE_UTF8;
      pMem++;
    }

    pMem->flags = MEM_Int;
    pMem->u.i = pOp->p1;                          /* P1 */
    pMem->type = SQLITE_INTEGER;
    pMem++;

    pMem->flags = MEM_Int;
    pMem->u.i = pOp->p2;                          /* P2 */
    pMem->type = SQLITE_INTEGER;
    pMem++;

    if( p->explain==1 ){
      pMem->flags = MEM_Int;
      pMem->u.i = pOp->p3;                          /* P3 */
      pMem->type = SQLITE_INTEGER;
      pMem++;
    }

    pMem->flags = MEM_Ephem|MEM_Str|MEM_Term;     /* P4 */
    pMem->z = displayP4(pOp, pMem->zShort, sizeof(pMem->zShort));
    assert( pMem->z!=0 );
    pMem->n = strlen(pMem->z);
    pMem->type = SQLITE_TEXT;
    pMem->enc = SQLITE_UTF8;
    pMem++;

    if( p->explain==1 ){
      pMem->flags = MEM_Str|MEM_Term|MEM_Short;
      pMem->n = sprintf(pMem->zShort, "%.2x", pOp->p5);   /* P5 */
      pMem->z = pMem->zShort;
      pMem->type = SQLITE_TEXT;
      pMem->enc = SQLITE_UTF8;
      pMem++;
  
      pMem->flags = MEM_Null;                       /* Comment */
#ifdef SQLITE_DEBUG
      if( pOp->zComment ){
        pMem->flags = MEM_Str|MEM_Term;
        pMem->z = pOp->zComment;
        pMem->n = strlen(pMem->z);
        pMem->enc = SQLITE_UTF8;
      }
#endif
    }

    p->nResColumn = 8 - 5*(p->explain-1);
    p->rc = SQLITE_OK;
    rc = SQLITE_ROW;
  }
  return rc;
}
#endif /* SQLITE_OMIT_EXPLAIN */

#ifdef SQLITE_DEBUG
/*
** Print the SQL that was used to generate a VDBE program.
*/
void sqlite3VdbePrintSql(Vdbe *p){
  int nOp = p->nOp;
  VdbeOp *pOp;
  if( nOp<1 ) return;
  pOp = &p->aOp[0];
  if( pOp->opcode==OP_Trace && pOp->p4.z!=0 ){
    const char *z = pOp->p4.z;
    while( isspace(*(u8*)z) ) z++;
    printf("SQL: [%s]\n", z);
  }
}
#endif

#if !defined(SQLITE_OMIT_TRACE) && defined(SQLITE_ENABLE_IOTRACE)
/*
** Print an IOTRACE message showing SQL content.
*/
void sqlite3VdbeIOTraceSql(Vdbe *p){
  int nOp = p->nOp;
  VdbeOp *pOp;
  if( sqlite3_io_trace==0 ) return;
  if( nOp<1 ) return;
  pOp = &p->aOp[0];
  if( pOp->opcode==OP_Trace && pOp->p4.z!=0 ){
    int i, j;
    char z[1000];
    sqlite3_snprintf(sizeof(z), z, "%s", pOp->p4.z);
    for(i=0; isspace((unsigned char)z[i]); i++){}
    for(j=0; z[i]; i++){
      if( isspace((unsigned char)z[i]) ){
        if( z[i-1]!=' ' ){
          z[j++] = ' ';
        }
      }else{
        z[j++] = z[i];
      }
    }
    z[j] = 0;
    sqlite3_io_trace("SQL %s\n", z);
  }
}
#endif /* !SQLITE_OMIT_TRACE && SQLITE_ENABLE_IOTRACE */


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
  sqlite3 *db = p->db;

  assert( p!=0 );
  assert( p->magic==VDBE_MAGIC_INIT );

  /* There should be at least one opcode.
  */
  assert( p->nOp>0 );

  /* Set the magic to VDBE_MAGIC_RUN sooner rather than later. This
   * is because the call to resizeOpArray() below may shrink the
   * p->aOp[] array to save memory if called when in VDBE_MAGIC_RUN 
   * state.
   */
  p->magic = VDBE_MAGIC_RUN;

  /*
  ** Allocation space for registers.
  */
  if( p->aMem==0 ){
    int nArg;       /* Maximum number of args passed to a user function. */
    resolveP2Values(p, &nArg);
    resizeOpArray(p, p->nOp);
    assert( nVar>=0 );
    if( isExplain && nMem<10 ){
      p->nMem = nMem = 10;
    }
    p->aMem = sqlite3DbMallocZero(db,
        nMem*sizeof(Mem)               /* aMem */
      + nVar*sizeof(Mem)               /* aVar */
      + nArg*sizeof(Mem*)              /* apArg */
      + nVar*sizeof(char*)             /* azVar */
      + nCursor*sizeof(Cursor*) + 1    /* apCsr */
    );
    if( !db->mallocFailed ){
      p->aMem--;             /* aMem[] goes from 1..nMem */
      p->nMem = nMem;        /*       not from 0..nMem-1 */
      p->aVar = &p->aMem[nMem+1];
      p->nVar = nVar;
      p->okVar = 0;
      p->apArg = (Mem**)&p->aVar[nVar];
      p->azVar = (char**)&p->apArg[nArg];
      p->apCsr = (Cursor**)&p->azVar[nVar];
      p->nCursor = nCursor;
      for(n=0; n<nVar; n++){
        p->aVar[n].flags = MEM_Null;
        p->aVar[n].db = db;
      }
      for(n=1; n<=nMem; n++){
        p->aMem[n].flags = MEM_Null;
        p->aMem[n].db = db;
      }
    }
  }
#ifdef SQLITE_DEBUG
  for(n=1; n<p->nMem; n++){
    assert( p->aMem[n].db==db );
    assert( p->aMem[n].flags==MEM_Null );
  }
#endif

  p->pc = -1;
  p->rc = SQLITE_OK;
  p->uniqueCnt = 0;
  p->returnDepth = 0;
  p->errorAction = OE_Abort;
  p->explain |= isExplain;
  p->magic = VDBE_MAGIC_RUN;
  p->nChange = 0;
  p->cacheCtr = 1;
  p->minWriteFileFormat = 255;
  p->openedStatement = 0;
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
** Close a VDBE cursor and release all the resources that cursor happens
** to hold.
*/
void sqlite3VdbeFreeCursor(Vdbe *p, Cursor *pCx){
  if( pCx==0 ){
    return;
  }
  if( pCx->pCursor ){
    sqlite3BtreeCloseCursor(pCx->pCursor);
  }
  if( pCx->pBt ){
    sqlite3BtreeClose(pCx->pBt);
  }
#ifndef SQLITE_OMIT_VIRTUALTABLE
  if( pCx->pVtabCursor ){
    sqlite3_vtab_cursor *pVtabCursor = pCx->pVtabCursor;
    const sqlite3_module *pModule = pCx->pModule;
    p->inVtabMethod = 1;
    (void)sqlite3SafetyOff(p->db);
    pModule->xClose(pVtabCursor);
    (void)sqlite3SafetyOn(p->db);
    p->inVtabMethod = 0;
  }
#endif
  sqlite3_free(pCx->pData);
  sqlite3_free(pCx->aType);
  sqlite3_free(pCx);
}

/*
** Close all cursors except for VTab cursors that are currently
** in use.
*/
static void closeAllCursorsExceptActiveVtabs(Vdbe *p){
  int i;
  if( p->apCsr==0 ) return;
  for(i=0; i<p->nCursor; i++){
    Cursor *pC = p->apCsr[i];
    if( pC && (!p->inVtabMethod || !pC->pVtabCursor) ){
      sqlite3VdbeFreeCursor(p, pC);
      p->apCsr[i] = 0;
    }
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
  closeAllCursorsExceptActiveVtabs(p);
  releaseMemArray(&p->aMem[1], p->nMem);
  sqlite3VdbeFifoClear(&p->sFifo);
  if( p->contextStack ){
    for(i=0; i<p->contextStackTop; i++){
      sqlite3VdbeFifoClear(&p->contextStack[i].sFifo);
    }
    sqlite3_free(p->contextStack);
  }
  p->contextStack = 0;
  p->contextStackDepth = 0;
  p->contextStackTop = 0;
  sqlite3_free(p->zErrMsg);
  p->zErrMsg = 0;
  p->pResultSet = 0;
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

  releaseMemArray(p->aColName, p->nResColumn*COLNAME_N);
  sqlite3_free(p->aColName);
  n = nResColumn*COLNAME_N;
  p->nResColumn = nResColumn;
  p->aColName = pColName = (Mem*)sqlite3DbMallocZero(p->db, sizeof(Mem)*n );
  if( p->aColName==0 ) return;
  while( n-- > 0 ){
    pColName->flags = MEM_Null;
    pColName->db = p->db;
    pColName++;
  }
}

/*
** Set the name of the idx'th column to be returned by the SQL statement.
** zName must be a pointer to a nul terminated string.
**
** This call must be made after a call to sqlite3VdbeSetNumCols().
**
** If N==P4_STATIC  it means that zName is a pointer to a constant static
** string and we can just copy the pointer. If it is P4_DYNAMIC, then 
** the string is freed using sqlite3_free() when the vdbe is finished with
** it. Otherwise, N bytes of zName are copied.
*/
int sqlite3VdbeSetColName(Vdbe *p, int idx, int var, const char *zName, int N){
  int rc;
  Mem *pColName;
  assert( idx<p->nResColumn );
  assert( var<COLNAME_N );
  if( p->db->mallocFailed ) return SQLITE_NOMEM;
  assert( p->aColName!=0 );
  pColName = &(p->aColName[idx+var*p->nResColumn]);
  if( N==P4_DYNAMIC || N==P4_STATIC ){
    rc = sqlite3VdbeMemSetStr(pColName, zName, -1, SQLITE_UTF8, SQLITE_STATIC);
  }else{
    rc = sqlite3VdbeMemSetStr(pColName, zName, N, SQLITE_UTF8,SQLITE_TRANSIENT);
  }
  if( rc==SQLITE_OK && N==P4_DYNAMIC ){
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

  /* Before doing anything else, call the xSync() callback for any
  ** virtual module tables written in this transaction. This has to
  ** be done before determining whether a master journal file is 
  ** required, as an xSync() callback may add an attached database
  ** to the transaction.
  */
  rc = sqlite3VtabSync(db, rc);
  if( rc!=SQLITE_OK ){
    return rc;
  }

  /* This loop determines (a) if the commit hook should be invoked and
  ** (b) how many database files have open write transactions, not 
  ** including the temp database. (b) is important because if more than 
  ** one database file has an open write transaction, a master journal
  ** file is required for an atomic commit.
  */ 
  for(i=0; i<db->nDb; i++){ 
    Btree *pBt = db->aDb[i].pBt;
    if( sqlite3BtreeIsInTrans(pBt) ){
      needXcommit = 1;
      if( i!=1 ) nTrans++;
    }
  }

  /* If there are any write-transactions at all, invoke the commit hook */
  if( needXcommit && db->xCommitCallback ){
    (void)sqlite3SafetyOff(db);
    rc = db->xCommitCallback(db->pCommitArg);
    (void)sqlite3SafetyOn(db);
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
        rc = sqlite3BtreeCommitPhaseOne(pBt, 0);
      }
    }

    /* Do the commit only if all databases successfully complete phase 1. 
    ** If one of the BtreeCommitPhaseOne() calls fails, this indicates an
    ** IO error while deleting or truncating a journal file. It is unlikely,
    ** but could happen. In this case abandon processing and return the error.
    */
    for(i=0; rc==SQLITE_OK && i<db->nDb; i++){
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        rc = sqlite3BtreeCommitPhaseTwo(pBt);
      }
    }
    if( rc==SQLITE_OK ){
      sqlite3VtabCommit(db);
    }
  }

  /* The complex case - There is a multi-file write-transaction active.
  ** This requires a master journal file to ensure the transaction is
  ** committed atomicly.
  */
#ifndef SQLITE_OMIT_DISKIO
  else{
    sqlite3_vfs *pVfs = db->pVfs;
    int needSync = 0;
    char *zMaster = 0;   /* File-name for the master journal */
    char const *zMainFile = sqlite3BtreeGetFilename(db->aDb[0].pBt);
    sqlite3_file *pMaster = 0;
    i64 offset = 0;

    /* Select a master journal file name */
    do {
      u32 random;
      sqlite3_free(zMaster);
      sqlite3Randomness(sizeof(random), &random);
      zMaster = sqlite3MPrintf(db, "%s-mj%08X", zMainFile, random&0x7fffffff);
      if( !zMaster ){
        return SQLITE_NOMEM;
      }
    }while( sqlite3OsAccess(pVfs, zMaster, SQLITE_ACCESS_EXISTS) );

    /* Open the master journal. */
    rc = sqlite3OsOpenMalloc(pVfs, zMaster, &pMaster, 
        SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE|
        SQLITE_OPEN_EXCLUSIVE|SQLITE_OPEN_MASTER_JOURNAL, 0
    );
    if( rc!=SQLITE_OK ){
      sqlite3_free(zMaster);
      return rc;
    }
 
    /* Write the name of each database file in the transaction into the new
    ** master journal file. If an error occurs at this point close
    ** and delete the master journal file. All the individual journal files
    ** still have 'null' as the master journal pointer, so they will roll
    ** back independently if a failure occurs.
    */
    for(i=0; i<db->nDb; i++){
      Btree *pBt = db->aDb[i].pBt;
      if( i==1 ) continue;   /* Ignore the TEMP database */
      if( sqlite3BtreeIsInTrans(pBt) ){
        char const *zFile = sqlite3BtreeGetJournalname(pBt);
        if( zFile[0]==0 ) continue;  /* Ignore :memory: databases */
        if( !needSync && !sqlite3BtreeSyncDisabled(pBt) ){
          needSync = 1;
        }
        rc = sqlite3OsWrite(pMaster, zFile, strlen(zFile)+1, offset);
        offset += strlen(zFile)+1;
        if( rc!=SQLITE_OK ){
          sqlite3OsCloseFree(pMaster);
          sqlite3OsDelete(pVfs, zMaster, 0);
          sqlite3_free(zMaster);
          return rc;
        }
      }
    }

    /* Sync the master journal file. If the IOCAP_SEQUENTIAL device
    ** flag is set this is not required.
    */
    zMainFile = sqlite3BtreeGetDirname(db->aDb[0].pBt);
    if( (needSync 
     && (0==(sqlite3OsDeviceCharacteristics(pMaster)&SQLITE_IOCAP_SEQUENTIAL))
     && (rc=sqlite3OsSync(pMaster, SQLITE_SYNC_NORMAL))!=SQLITE_OK) ){
      sqlite3OsCloseFree(pMaster);
      sqlite3OsDelete(pVfs, zMaster, 0);
      sqlite3_free(zMaster);
      return rc;
    }

    /* Sync all the db files involved in the transaction. The same call
    ** sets the master journal pointer in each individual journal. If
    ** an error occurs here, do not delete the master journal file.
    **
    ** If the error occurs during the first call to
    ** sqlite3BtreeCommitPhaseOne(), then there is a chance that the
    ** master journal file will be orphaned. But we cannot delete it,
    ** in case the master journal file name was written into the journal
    ** file before the failure occured.
    */
    for(i=0; rc==SQLITE_OK && i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        rc = sqlite3BtreeCommitPhaseOne(pBt, zMaster);
      }
    }
    sqlite3OsCloseFree(pMaster);
    if( rc!=SQLITE_OK ){
      sqlite3_free(zMaster);
      return rc;
    }

    /* Delete the master journal file. This commits the transaction. After
    ** doing this the directory is synced again before any individual
    ** transaction files are deleted.
    */
    rc = sqlite3OsDelete(pVfs, zMaster, 1);
    sqlite3_free(zMaster);
    zMaster = 0;
    if( rc ){
      return rc;
    }

    /* All files and directories have already been synced, so the following
    ** calls to sqlite3BtreeCommitPhaseTwo() are only closing files and
    ** deleting or truncating journals. If something goes wrong while
    ** this is happening we don't really care. The integrity of the
    ** transaction is already guaranteed, but some stray 'cold' journals
    ** may be lying around. Returning an error code won't help matters.
    */
    disable_simulated_io_errors();
    for(i=0; i<db->nDb; i++){ 
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        sqlite3BtreeCommitPhaseTwo(pBt);
      }
    }
    enable_simulated_io_errors();

    sqlite3VtabCommit(db);
  }
#endif

  return rc;
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
** For every Btree that in database connection db which 
** has been modified, "trip" or invalidate each cursor in
** that Btree might have been modified so that the cursor
** can never be used again.  This happens when a rollback
*** occurs.  We have to trip all the other cursors, even
** cursor from other VMs in different database connections,
** so that none of them try to use the data at which they
** were pointing and which now may have been changed due
** to the rollback.
**
** Remember that a rollback can delete tables complete and
** reorder rootpages.  So it is not sufficient just to save
** the state of the cursor.  We have to invalidate the cursor
** so that it is never used again.
*/
static void invalidateCursorsOnModifiedBtrees(sqlite3 *db){
  int i;
  for(i=0; i<db->nDb; i++){
    Btree *p = db->aDb[i].pBt;
    if( p && sqlite3BtreeIsInTrans(p) ){
      sqlite3BtreeTripAllCursors(p, SQLITE_ABORT);
    }
  }
}

/*
** This routine is called the when a VDBE tries to halt.  If the VDBE
** has made changes and is in autocommit mode, then commit those
** changes.  If a rollback is needed, then do the rollback.
**
** This routine is the only way to move the state of a VM from
** SQLITE_MAGIC_RUN to SQLITE_MAGIC_HALT.  It is harmless to
** call this on a VM that is in the SQLITE_MAGIC_HALT state.
**
** Return an error code.  If the commit could not complete because of
** lock contention, return SQLITE_BUSY.  If SQLITE_BUSY is returned, it
** means the close did not happen and needs to be repeated.
*/
int sqlite3VdbeHalt(Vdbe *p){
  sqlite3 *db = p->db;
  int i;
  int (*xFunc)(Btree *pBt) = 0;  /* Function to call on each btree backend */
  int isSpecialError;            /* Set to true if SQLITE_NOMEM or IOERR */

  /* This function contains the logic that determines if a statement or
  ** transaction will be committed or rolled back as a result of the
  ** execution of this virtual machine. 
  **
  ** If any of the following errors occur:
  **
  **     SQLITE_NOMEM
  **     SQLITE_IOERR
  **     SQLITE_FULL
  **     SQLITE_INTERRUPT
  **
  ** Then the internal cache might have been left in an inconsistent
  ** state.  We need to rollback the statement transaction, if there is
  ** one, or the complete transaction if there is no statement transaction.
  */

  if( p->db->mallocFailed ){
    p->rc = SQLITE_NOMEM;
  }
  closeAllCursorsExceptActiveVtabs(p);
  if( p->magic!=VDBE_MAGIC_RUN ){
    return SQLITE_OK;
  }
  checkActiveVdbeCnt(db);

  /* No commit or rollback needed if the program never started */
  if( p->pc>=0 ){
    int mrc;   /* Primary error code from p->rc */

    /* Lock all btrees used by the statement */
    sqlite3BtreeMutexArrayEnter(&p->aMutex);

    /* Check for one of the special errors */
    mrc = p->rc & 0xff;
    isSpecialError = mrc==SQLITE_NOMEM || mrc==SQLITE_IOERR
                     || mrc==SQLITE_INTERRUPT || mrc==SQLITE_FULL;
    if( isSpecialError ){
      /* This loop does static analysis of the query to see which of the
      ** following three categories it falls into:
      **
      **     Read-only
      **     Query with statement journal
      **     Query without statement journal
      **
      ** We could do something more elegant than this static analysis (i.e.
      ** store the type of query as part of the compliation phase), but 
      ** handling malloc() or IO failure is a fairly obscure edge case so 
      ** this is probably easier. Todo: Might be an opportunity to reduce 
      ** code size a very small amount though...
      */
      int notReadOnly = 0;
      int isStatement = 0;
      assert(p->aOp || p->nOp==0);
      for(i=0; i<p->nOp; i++){ 
        switch( p->aOp[i].opcode ){
          case OP_Transaction:
            notReadOnly |= p->aOp[i].p2;
            break;
          case OP_Statement:
            isStatement = 1;
            break;
        }
      }

   
      /* If the query was read-only, we need do no rollback at all. Otherwise,
      ** proceed with the special handling.
      */
      if( notReadOnly || mrc!=SQLITE_INTERRUPT ){
        if( p->rc==SQLITE_IOERR_BLOCKED && isStatement ){
          xFunc = sqlite3BtreeRollbackStmt;
          p->rc = SQLITE_BUSY;
        } else if( (mrc==SQLITE_NOMEM || mrc==SQLITE_FULL) && isStatement ){
          xFunc = sqlite3BtreeRollbackStmt;
        }else{
          /* We are forced to roll back the active transaction. Before doing
          ** so, abort any other statements this handle currently has active.
          */
          invalidateCursorsOnModifiedBtrees(db);
          sqlite3RollbackAll(db);
          db->autoCommit = 1;
        }
      }
    }
  
    /* If the auto-commit flag is set and this is the only active vdbe, then
    ** we do either a commit or rollback of the current transaction. 
    **
    ** Note: This block also runs if one of the special errors handled 
    ** above has occured. 
    */
    if( db->autoCommit && db->activeVdbeCnt==1 ){
      if( p->rc==SQLITE_OK || (p->errorAction==OE_Fail && !isSpecialError) ){
        /* The auto-commit flag is true, and the vdbe program was 
        ** successful or hit an 'OR FAIL' constraint. This means a commit 
        ** is required.
        */
        int rc = vdbeCommit(db);
        if( rc==SQLITE_BUSY ){
          sqlite3BtreeMutexArrayLeave(&p->aMutex);
          return SQLITE_BUSY;
        }else if( rc!=SQLITE_OK ){
          p->rc = rc;
          sqlite3RollbackAll(db);
        }else{
          sqlite3CommitInternalChanges(db);
        }
      }else{
        sqlite3RollbackAll(db);
      }
    }else if( !xFunc ){
      if( p->rc==SQLITE_OK || p->errorAction==OE_Fail ){
        if( p->openedStatement ){
          xFunc = sqlite3BtreeCommitStmt;
        } 
      }else if( p->errorAction==OE_Abort ){
        xFunc = sqlite3BtreeRollbackStmt;
      }else{
        invalidateCursorsOnModifiedBtrees(db);
        sqlite3RollbackAll(db);
        db->autoCommit = 1;
      }
    }
  
    /* If xFunc is not NULL, then it is one of sqlite3BtreeRollbackStmt or
    ** sqlite3BtreeCommitStmt. Call it once on each backend. If an error occurs
    ** and the return code is still SQLITE_OK, set the return code to the new
    ** error value.
    */
    assert(!xFunc ||
      xFunc==sqlite3BtreeCommitStmt ||
      xFunc==sqlite3BtreeRollbackStmt
    );
    for(i=0; xFunc && i<db->nDb; i++){ 
      int rc;
      Btree *pBt = db->aDb[i].pBt;
      if( pBt ){
        rc = xFunc(pBt);
        if( rc && (p->rc==SQLITE_OK || p->rc==SQLITE_CONSTRAINT) ){
          p->rc = rc;
          sqlite3SetString(&p->zErrMsg, 0);
        }
      }
    }
  
    /* If this was an INSERT, UPDATE or DELETE and the statement was committed, 
    ** set the change counter. 
    */
    if( p->changeCntOn && p->pc>=0 ){
      if( !xFunc || xFunc==sqlite3BtreeCommitStmt ){
        sqlite3VdbeSetChanges(db, p->nChange);
      }else{
        sqlite3VdbeSetChanges(db, 0);
      }
      p->nChange = 0;
    }
  
    /* Rollback or commit any schema changes that occurred. */
    if( p->rc!=SQLITE_OK && db->flags&SQLITE_InternChanges ){
      sqlite3ResetInternalSchema(db, 0);
      db->flags = (db->flags | SQLITE_InternChanges);
    }

    /* Release the locks */
    sqlite3BtreeMutexArrayLeave(&p->aMutex);
  }

  /* We have successfully halted and closed the VM.  Record this fact. */
  if( p->pc>=0 ){
    db->activeVdbeCnt--;
  }
  p->magic = VDBE_MAGIC_HALT;
  checkActiveVdbeCnt(db);
  if( p->db->mallocFailed ){
    p->rc = SQLITE_NOMEM;
  }
  checkActiveVdbeCnt(db);

  return SQLITE_OK;
}


/*
** Each VDBE holds the result of the most recent sqlite3_step() call
** in p->rc.  This routine sets that result back to SQLITE_OK.
*/
void sqlite3VdbeResetStepResult(Vdbe *p){
  p->rc = SQLITE_OK;
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
  sqlite3 *db;
  db = p->db;

  /* If the VM did not run to completion or if it encountered an
  ** error, then it might not have been halted properly.  So halt
  ** it now.
  */
  (void)sqlite3SafetyOn(db);
  sqlite3VdbeHalt(p);
  (void)sqlite3SafetyOff(db);

  /* If the VDBE has be run even partially, then transfer the error code
  ** and error message from the VDBE into the main database structure.  But
  ** if the VDBE has just been set to run but has not actually executed any
  ** instructions yet, leave the main database error information unchanged.
  */
  if( p->pc>=0 ){
    if( p->zErrMsg ){
      sqlite3ValueSetStr(db->pErr,-1,p->zErrMsg,SQLITE_UTF8,sqlite3_free);
      db->errCode = p->rc;
      p->zErrMsg = 0;
    }else if( p->rc ){
      sqlite3Error(db, p->rc, 0);
    }else{
      sqlite3Error(db, SQLITE_OK, 0);
    }
  }else if( p->rc && p->expired ){
    /* The expired flag was set on the VDBE before the first call
    ** to sqlite3_step(). For consistency (since sqlite3_step() was
    ** called), set the database error in this case as well.
    */
    sqlite3Error(db, p->rc, 0);
    sqlite3ValueSetStr(db->pErr, -1, p->zErrMsg, SQLITE_UTF8, sqlite3_free);
    p->zErrMsg = 0;
  }

  /* Reclaim all memory used by the VDBE
  */
  Cleanup(p);

  /* Save profiling information from this VDBE run.
  */
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
  return p->rc & db->errMask;
}
 
/*
** Clean up and delete a VDBE after execution.  Return an integer which is
** the result code.  Write any error message text into *pzErrMsg.
*/
int sqlite3VdbeFinalize(Vdbe *p){
  int rc = SQLITE_OK;
  if( p->magic==VDBE_MAGIC_RUN || p->magic==VDBE_MAGIC_HALT ){
    rc = sqlite3VdbeReset(p);
    assert( (rc & p->db->errMask)==rc );
  }else if( p->magic!=VDBE_MAGIC_INIT ){
    return SQLITE_MISUSE;
  }
  sqlite3VdbeDelete(p);
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
    Op *pOp = p->aOp;
    for(i=0; i<p->nOp; i++, pOp++){
      freeP4(pOp->p4type, pOp->p4.p);
#ifdef SQLITE_DEBUG
      sqlite3_free(pOp->zComment);
#endif     
    }
    sqlite3_free(p->aOp);
  }
  releaseMemArray(p->aVar, p->nVar);
  sqlite3_free(p->aLabel);
  if( p->aMem ){
    sqlite3_free(&p->aMem[1]);
  }
  releaseMemArray(p->aColName, p->nResColumn*COLNAME_N);
  sqlite3_free(p->aColName);
  sqlite3_free(p->zSql);
  p->magic = VDBE_MAGIC_DEAD;
  sqlite3_free(p);
}

/*
** If a MoveTo operation is pending on the given cursor, then do that
** MoveTo now.  Return an error code.  If no MoveTo is pending, this
** routine does nothing and returns SQLITE_OK.
*/
int sqlite3VdbeCursorMoveto(Cursor *p){
  if( p->deferredMoveto ){
    int res, rc;
#ifdef SQLITE_TEST
    extern int sqlite3_search_count;
#endif
    assert( p->isTable );
    rc = sqlite3BtreeMoveto(p->pCursor, 0, p->movetoTarget, 0, &res);
    if( rc ) return rc;
    *p->pIncrKey = 0;
    p->lastRowid = keyToInt(p->movetoTarget);
    p->rowidIsValid = res==0;
    if( res<0 ){
      rc = sqlite3BtreeNext(p->pCursor, &res);
      if( rc ) return rc;
    }
#ifdef SQLITE_TEST
    sqlite3_search_count++;
#endif
    p->deferredMoveto = 0;
    p->cacheStatus = CACHE_STALE;
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
**      8                     0            Integer constant 0
**      9                     0            Integer constant 1
**     10,11                               reserved for expansion
**    N>=12 and even       (N-12)/2        BLOB
**    N>=13 and odd        (N-13)/2        text
**
** The 8 and 9 types were added in 3.3.0, file format 4.  Prior versions
** of SQLite will not understand those serial types.
*/

/*
** Return the serial-type for the value stored in pMem.
*/
u32 sqlite3VdbeSerialType(Mem *pMem, int file_format){
  int flags = pMem->flags;
  int n;

  if( flags&MEM_Null ){
    return 0;
  }
  if( flags&MEM_Int ){
    /* Figure out whether to use 1, 2, 4, 6 or 8 bytes. */
#   define MAX_6BYTE ((((i64)0x00001000)<<32)-1)
    i64 i = pMem->u.i;
    u64 u;
    if( file_format>=4 && (i&1)==i ){
      return 8+i;
    }
    u = i<0 ? -i : i;
    if( u<=127 ) return 1;
    if( u<=32767 ) return 2;
    if( u<=8388607 ) return 3;
    if( u<=2147483647 ) return 4;
    if( u<=MAX_6BYTE ) return 5;
    return 6;
  }
  if( flags&MEM_Real ){
    return 7;
  }
  assert( flags&(MEM_Str|MEM_Blob) );
  n = pMem->n;
  if( flags & MEM_Zero ){
    n += pMem->u.i;
  }
  assert( n>=0 );
  return ((n*2) + 12 + ((flags&MEM_Str)!=0));
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
** If we are on an architecture with mixed-endian floating 
** points (ex: ARM7) then swap the lower 4 bytes with the 
** upper 4 bytes.  Return the result.
**
** For most architectures, this is a no-op.
**
** (later):  It is reported to me that the mixed-endian problem
** on ARM7 is an issue with GCC, not with the ARM7 chip.  It seems
** that early versions of GCC stored the two words of a 64-bit
** float in the wrong order.  And that error has been propagated
** ever since.  The blame is not necessarily with GCC, though.
** GCC might have just copying the problem from a prior compiler.
** I am also told that newer versions of GCC that follow a different
** ABI get the byte order right.
**
** Developers using SQLite on an ARM7 should compile and run their
** application using -DSQLITE_DEBUG=1 at least once.  With DEBUG
** enabled, some asserts below will ensure that the byte order of
** floating point values is correct.
**
** (2007-08-30)  Frank van Vugt has studied this problem closely
** and has send his findings to the SQLite developers.  Frank
** writes that some Linux kernels offer floating point hardware
** emulation that uses only 32-bit mantissas instead of a full 
** 48-bits as required by the IEEE standard.  (This is the
** CONFIG_FPE_FASTFPE option.)  On such systems, floating point
** byte swapping becomes very complicated.  To avoid problems,
** the necessary byte swapping is carried out using a 64-bit integer
** rather than a 64-bit float.  Frank assures us that the code here
** works for him.  We, the developers, have no way to independently
** verify this, but Frank seems to know what he is talking about
** so we trust him.
*/
#ifdef SQLITE_MIXED_ENDIAN_64BIT_FLOAT
static u64 floatSwap(u64 in){
  union {
    u64 r;
    u32 i[2];
  } u;
  u32 t;

  u.r = in;
  t = u.i[0];
  u.i[0] = u.i[1];
  u.i[1] = t;
  return u.r;
}
# define swapMixedEndianFloat(X)  X = floatSwap(X)
#else
# define swapMixedEndianFloat(X)
#endif

/*
** Write the serialized data blob for the value stored in pMem into 
** buf. It is assumed that the caller has allocated sufficient space.
** Return the number of bytes written.
**
** nBuf is the amount of space left in buf[].  nBuf must always be
** large enough to hold the entire field.  Except, if the field is
** a blob with a zero-filled tail, then buf[] might be just the right
** size to hold everything except for the zero-filled tail.  If buf[]
** is only big enough to hold the non-zero prefix, then only write that
** prefix into buf[].  But if buf[] is large enough to hold both the
** prefix and the tail then write the prefix and set the tail to all
** zeros.
**
** Return the number of bytes actually written into buf[].  The number
** of bytes in the zero-filled tail is included in the return value only
** if those bytes were zeroed in buf[].
*/ 
int sqlite3VdbeSerialPut(u8 *buf, int nBuf, Mem *pMem, int file_format){
  u32 serial_type = sqlite3VdbeSerialType(pMem, file_format);
  int len;

  /* Integer and Real */
  if( serial_type<=7 && serial_type>0 ){
    u64 v;
    int i;
    if( serial_type==7 ){
      assert( sizeof(v)==sizeof(pMem->r) );
      memcpy(&v, &pMem->r, sizeof(v));
      swapMixedEndianFloat(v);
    }else{
      v = pMem->u.i;
    }
    len = i = sqlite3VdbeSerialTypeLen(serial_type);
    assert( len<=nBuf );
    while( i-- ){
      buf[i] = (v&0xFF);
      v >>= 8;
    }
    return len;
  }

  /* String or blob */
  if( serial_type>=12 ){
    assert( pMem->n + ((pMem->flags & MEM_Zero)?pMem->u.i:0)
             == sqlite3VdbeSerialTypeLen(serial_type) );
    assert( pMem->n<=nBuf );
    len = pMem->n;
    memcpy(buf, pMem->z, len);
    if( pMem->flags & MEM_Zero ){
      len += pMem->u.i;
      if( len>nBuf ){
        len = nBuf;
      }
      memset(&buf[pMem->n], 0, len-pMem->n);
    }
    return len;
  }

  /* NULL or constants 0 or 1 */
  return 0;
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
  switch( serial_type ){
    case 10:   /* Reserved for future use */
    case 11:   /* Reserved for future use */
    case 0: {  /* NULL */
      pMem->flags = MEM_Null;
      break;
    }
    case 1: { /* 1-byte signed integer */
      pMem->u.i = (signed char)buf[0];
      pMem->flags = MEM_Int;
      return 1;
    }
    case 2: { /* 2-byte signed integer */
      pMem->u.i = (((signed char)buf[0])<<8) | buf[1];
      pMem->flags = MEM_Int;
      return 2;
    }
    case 3: { /* 3-byte signed integer */
      pMem->u.i = (((signed char)buf[0])<<16) | (buf[1]<<8) | buf[2];
      pMem->flags = MEM_Int;
      return 3;
    }
    case 4: { /* 4-byte signed integer */
      pMem->u.i = (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
      pMem->flags = MEM_Int;
      return 4;
    }
    case 5: { /* 6-byte signed integer */
      u64 x = (((signed char)buf[0])<<8) | buf[1];
      u32 y = (buf[2]<<24) | (buf[3]<<16) | (buf[4]<<8) | buf[5];
      x = (x<<32) | y;
      pMem->u.i = *(i64*)&x;
      pMem->flags = MEM_Int;
      return 6;
    }
    case 6:   /* 8-byte signed integer */
    case 7: { /* IEEE floating point */
      u64 x;
      u32 y;
#if !defined(NDEBUG) && !defined(SQLITE_OMIT_FLOATING_POINT)
      /* Verify that integers and floating point values use the same
      ** byte order.  Or, that if SQLITE_MIXED_ENDIAN_64BIT_FLOAT is
      ** defined that 64-bit floating point values really are mixed
      ** endian.
      */
      static const u64 t1 = ((u64)0x3ff00000)<<32;
      static const double r1 = 1.0;
      u64 t2 = t1;
      swapMixedEndianFloat(t2);
      assert( sizeof(r1)==sizeof(t2) && memcmp(&r1, &t2, sizeof(r1))==0 );
#endif

      x = (buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8) | buf[3];
      y = (buf[4]<<24) | (buf[5]<<16) | (buf[6]<<8) | buf[7];
      x = (x<<32) | y;
      if( serial_type==6 ){
        pMem->u.i = *(i64*)&x;
        pMem->flags = MEM_Int;
      }else{
        assert( sizeof(x)==8 && sizeof(pMem->r)==8 );
        swapMixedEndianFloat(x);
        memcpy(&pMem->r, &x, sizeof(x));
        pMem->flags = MEM_Real;
      }
      return 8;
    }
    case 8:    /* Integer 0 */
    case 9: {  /* Integer 1 */
      pMem->u.i = serial_type-8;
      pMem->flags = MEM_Int;
      return 0;
    }
    default: {
      int len = (serial_type-12)/2;
      pMem->z = (char *)buf;
      pMem->n = len;
      pMem->xDel = 0;
      if( serial_type&0x01 ){
        pMem->flags = MEM_Str | MEM_Ephem;
      }else{
        pMem->flags = MEM_Blob | MEM_Ephem;
      }
      return len;
    }
  }
  return 0;
}

/*
** The header of a record consists of a sequence variable-length integers.
** These integers are almost always small and are encoded as a single byte.
** The following macro takes advantage this fact to provide a fast decode
** of the integers in a record header.  It is faster for the common case
** where the integer is a single byte.  It is a little slower when the
** integer is two or more bytes.  But overall it is faster.
**
** The following expressions are equivalent:
**
**     x = sqlite3GetVarint32( A, &B );
**
**     x = GetVarint( A, B );
**
*/
#define GetVarint(A,B)  ((B = *(A))<=0x7f ? 1 : sqlite3GetVarint32(A, &B))

/*
** This function compares the two table rows or index records specified by 
** {nKey1, pKey1} and {nKey2, pKey2}, returning a negative, zero
** or positive integer if {nKey1, pKey1} is less than, equal to or 
** greater than {nKey2, pKey2}.  Both Key1 and Key2 must be byte strings
** composed by the OP_MakeRecord opcode of the VDBE.
**
** Key1 and Key2 do not have to contain the same number of fields.
** But if the lengths differ, Key2 must be the shorter of the two.
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
  mem1.db = pKeyInfo->db;
  mem2.enc = pKeyInfo->enc;
  mem2.db = pKeyInfo->db;
  
  idx1 = GetVarint(aKey1, szHdr1);
  d1 = szHdr1;
  idx2 = GetVarint(aKey2, szHdr2);
  d2 = szHdr2;
  nField = pKeyInfo->nField;
  while( idx1<szHdr1 && idx2<szHdr2 ){
    u32 serial_type1;
    u32 serial_type2;

    /* Read the serial types for the next element in each key. */
    idx1 += GetVarint( aKey1+idx1, serial_type1 );
    if( d1>=nKey1 && sqlite3VdbeSerialTypeLen(serial_type1)>0 ) break;
    idx2 += GetVarint( aKey2+idx2, serial_type2 );
    if( d2>=nKey2 && sqlite3VdbeSerialTypeLen(serial_type2)>0 ) break;

    /* Extract the values to be compared.
    */
    d1 += sqlite3VdbeSerialGet(&aKey1[d1], serial_type1, &mem1);
    d2 += sqlite3VdbeSerialGet(&aKey2[d2], serial_type2, &mem2);

    /* Do the comparison
    */
    rc = sqlite3MemCompare(&mem1, &mem2, i<nField ? pKeyInfo->aColl[i] : 0);
    if( mem1.flags & MEM_Dyn ) sqlite3VdbeMemRelease(&mem1);
    if( mem2.flags & MEM_Dyn ) sqlite3VdbeMemRelease(&mem2);
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
    }else if( !pKeyInfo->prefixIsEqual ){
      if( d1<nKey1 ){
        rc = 1;
      }else if( d2<nKey2 ){
        rc = -1;  /* Only occurs on a corrupt database file */
      }
    }
  }else if( pKeyInfo->aSortOrder && i<pKeyInfo->nField
               && pKeyInfo->aSortOrder[i] ){
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
int sqlite3VdbeIdxRowidLen(const u8 *aKey){
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
  i64 nCellKey = 0;
  int rc;
  u32 szHdr;        /* Size of the header */
  u32 typeRowid;    /* Serial type of the rowid */
  u32 lenRowid;     /* Size of the rowid */
  Mem m, v;

  sqlite3BtreeKeySize(pCur, &nCellKey);
  if( nCellKey<=0 ){
    return SQLITE_CORRUPT_BKPT;
  }
  rc = sqlite3VdbeMemFromBtree(pCur, 0, nCellKey, 1, &m);
  if( rc ){
    return rc;
  }
  sqlite3GetVarint32((u8*)m.z, &szHdr);
  sqlite3GetVarint32((u8*)&m.z[szHdr-1], &typeRowid);
  lenRowid = sqlite3VdbeSerialTypeLen(typeRowid);
  sqlite3VdbeSerialGet((u8*)&m.z[m.n-lenRowid], typeRowid, &v);
  *rowid = v.u.i;
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
  i64 nCellKey = 0;
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
  lenRowid = sqlite3VdbeIdxRowidLen((u8*)m.z);
  *res = sqlite3VdbeRecordCompare(pC->pKeyInfo, m.n-lenRowid, m.z, nKey, pKey);
  sqlite3VdbeMemRelease(&m);
  return SQLITE_OK;
}

/*
** This routine sets the value to be returned by subsequent calls to
** sqlite3_changes() on the database handle 'db'. 
*/
void sqlite3VdbeSetChanges(sqlite3 *db, int nChange){
  assert( sqlite3_mutex_held(db->mutex) );
  db->nChange = nChange;
  db->nTotalChange += nChange;
}

/*
** Set a flag in the vdbe to update the change counter when it is finalised
** or reset.
*/
void sqlite3VdbeCountChanges(Vdbe *v){
  v->changeCntOn = 1;
}

/*
** Mark every prepared statement associated with a database connection
** as expired.
**
** An expired statement means that recompilation of the statement is
** recommend.  Statements expire when things happen that make their
** programs obsolete.  Removing user-defined functions or collating
** sequences, or changing an authorization function are the types of
** things that make prepared statements obsolete.
*/
void sqlite3ExpirePreparedStatements(sqlite3 *db){
  Vdbe *p;
  for(p = db->pVdbe; p; p=p->pNext){
    p->expired = 1;
  }
}

/*
** Return the database associated with the Vdbe.
*/
sqlite3 *sqlite3VdbeDb(Vdbe *v){
  return v->db;
}
