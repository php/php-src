/*
** 2001 September 15
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** The code in this file implements execution method of the 
** Virtual Database Engine (VDBE).  A separate file ("vdbeaux.c")
** handles housekeeping details such as creating and deleting
** VDBE instances.  This file is solely interested in executing
** the VDBE program.
**
** In the external interface, an "sqlite3_stmt*" is an opaque pointer
** to a VDBE.
**
** The SQL parser generates a program which is then executed by
** the VDBE to do the work of the SQL statement.  VDBE programs are 
** similar in form to assembly language.  The program consists of
** a linear sequence of operations.  Each operation has an opcode 
** and 3 operands.  Operands P1 and P2 are integers.  Operand P3 
** is a null-terminated string.   The P2 operand must be non-negative.
** Opcodes will typically ignore one or more operands.  Many opcodes
** ignore all three operands.
**
** Computation results are stored on a stack.  Each entry on the
** stack is either an integer, a null-terminated string, a floating point
** number, or the SQL "NULL" value.  An inplicit conversion from one
** type to the other occurs as necessary.
** 
** Most of the code in this file is taken up by the sqlite3VdbeExec()
** function which does the work of interpreting a VDBE program.
** But other routines are also provided to help in building up
** a program instruction by instruction.
**
** Various scripts scan this source file in order to generate HTML
** documentation, headers files, or other derived files.  The formatting
** of the code in this file is, therefore, important.  See other comments
** in this file for details.  If in doubt, do not deviate from existing
** commenting and indentation practices when changing or adding code.
**
** $Id$
*/
#include "sqliteInt.h"
#include "os.h"
#include <ctype.h>
#include "vdbeInt.h"

/*
** The following global variable is incremented every time a cursor
** moves, either by the OP_MoveXX, OP_Next, or OP_Prev opcodes.  The test
** procedures use this information to make sure that indices are
** working correctly.  This variable has no function other than to
** help verify the correct operation of the library.
*/
#ifdef SQLITE_TEST
int sqlite3_search_count = 0;
#endif

/*
** When this global variable is positive, it gets decremented once before
** each instruction in the VDBE.  When reaches zero, the u1.isInterrupted
** field of the sqlite3 structure is set in order to simulate and interrupt.
**
** This facility is used for testing purposes only.  It does not function
** in an ordinary build.
*/
#ifdef SQLITE_TEST
int sqlite3_interrupt_count = 0;
#endif

/*
** The next global variable is incremented each type the OP_Sort opcode
** is executed.  The test procedures use this information to make sure that
** sorting is occurring or not occuring at appropriate times.   This variable
** has no function other than to help verify the correct operation of the
** library.
*/
#ifdef SQLITE_TEST
int sqlite3_sort_count = 0;
#endif

/*
** Release the memory associated with the given stack level.  This
** leaves the Mem.flags field in an inconsistent state.
*/
#define Release(P) if((P)->flags&MEM_Dyn){ sqlite3VdbeMemRelease(P); }

/*
** Convert the given stack entity into a string if it isn't one
** already. Return non-zero if a malloc() fails.
*/
#define Stringify(P, enc) \
   if(((P)->flags&(MEM_Str|MEM_Blob))==0 && sqlite3VdbeMemStringify(P,enc)) \
     { goto no_mem; }

/*
** Convert the given stack entity into a string that has been obtained
** from sqliteMalloc().  This is different from Stringify() above in that
** Stringify() will use the NBFS bytes of static string space if the string
** will fit but this routine always mallocs for space.
** Return non-zero if we run out of memory.
*/
#define Dynamicify(P,enc) sqlite3VdbeMemDynamicify(P)

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
** An ephemeral string value (signified by the MEM_Ephem flag) contains
** a pointer to a dynamically allocated string where some other entity
** is responsible for deallocating that string.  Because the stack entry
** does not control the string, it might be deleted without the stack
** entry knowing it.
**
** This routine converts an ephemeral string into a dynamically allocated
** string that the stack entry itself controls.  In other words, it
** converts an MEM_Ephem string into an MEM_Dyn string.
*/
#define Deephemeralize(P) \
   if( ((P)->flags&MEM_Ephem)!=0 \
       && sqlite3VdbeMemMakeWriteable(P) ){ goto no_mem;}

/*
** Argument pMem points at a memory cell that will be passed to a
** user-defined function or returned to the user as the result of a query.
** The second argument, 'db_enc' is the text encoding used by the vdbe for
** stack variables.  This routine sets the pMem->enc and pMem->type
** variables used by the sqlite3_value_*() routines.
*/
#define storeTypeInfo(A,B) _storeTypeInfo(A)
static void _storeTypeInfo(Mem *pMem){
  int flags = pMem->flags;
  if( flags & MEM_Null ){
    pMem->type = SQLITE_NULL;
  }
  else if( flags & MEM_Int ){
    pMem->type = SQLITE_INTEGER;
  }
  else if( flags & MEM_Real ){
    pMem->type = SQLITE_FLOAT;
  }
  else if( flags & MEM_Str ){
    pMem->type = SQLITE_TEXT;
  }else{
    pMem->type = SQLITE_BLOB;
  }
}

/*
** Pop the stack N times.
*/
static void popStack(Mem **ppTos, int N){
  Mem *pTos = *ppTos;
  while( N>0 ){
    N--;
    Release(pTos);
    pTos--;
  }
  *ppTos = pTos;
}

/*
** Allocate cursor number iCur.  Return a pointer to it.  Return NULL
** if we run out of memory.
*/
static Cursor *allocateCursor(Vdbe *p, int iCur, int iDb){
  Cursor *pCx;
  assert( iCur<p->nCursor );
  if( p->apCsr[iCur] ){
    sqlite3VdbeFreeCursor(p, p->apCsr[iCur]);
  }
  p->apCsr[iCur] = pCx = sqliteMalloc( sizeof(Cursor) );
  if( pCx ){
    pCx->iDb = iDb;
  }
  return pCx;
}

/*
** Try to convert a value into a numeric representation if we can
** do so without loss of information.  In other words, if the string
** looks like a number, convert it into a number.  If it does not
** look like a number, leave it alone.
*/
static void applyNumericAffinity(Mem *pRec){
  if( (pRec->flags & (MEM_Real|MEM_Int))==0 ){
    int realnum;
    sqlite3VdbeMemNulTerminate(pRec);
    if( (pRec->flags&MEM_Str)
         && sqlite3IsNumber(pRec->z, &realnum, pRec->enc) ){
      i64 value;
      sqlite3VdbeChangeEncoding(pRec, SQLITE_UTF8);
      if( !realnum && sqlite3atoi64(pRec->z, &value) ){
        sqlite3VdbeMemRelease(pRec);
        pRec->u.i = value;
        pRec->flags = MEM_Int;
      }else{
        sqlite3VdbeMemRealify(pRec);
      }
    }
  }
}

/*
** Processing is determine by the affinity parameter:
**
** SQLITE_AFF_INTEGER:
** SQLITE_AFF_REAL:
** SQLITE_AFF_NUMERIC:
**    Try to convert pRec to an integer representation or a 
**    floating-point representation if an integer representation
**    is not possible.  Note that the integer representation is
**    always preferred, even if the affinity is REAL, because
**    an integer representation is more space efficient on disk.
**
** SQLITE_AFF_TEXT:
**    Convert pRec to a text representation.
**
** SQLITE_AFF_NONE:
**    No-op.  pRec is unchanged.
*/
static void applyAffinity(Mem *pRec, char affinity, u8 enc){
  if( affinity==SQLITE_AFF_TEXT ){
    /* Only attempt the conversion to TEXT if there is an integer or real
    ** representation (blob and NULL do not get converted) but no string
    ** representation.
    */
    if( 0==(pRec->flags&MEM_Str) && (pRec->flags&(MEM_Real|MEM_Int)) ){
      sqlite3VdbeMemStringify(pRec, enc);
    }
    pRec->flags &= ~(MEM_Real|MEM_Int);
  }else if( affinity!=SQLITE_AFF_NONE ){
    assert( affinity==SQLITE_AFF_INTEGER || affinity==SQLITE_AFF_REAL
             || affinity==SQLITE_AFF_NUMERIC );
    applyNumericAffinity(pRec);
    if( pRec->flags & MEM_Real ){
      sqlite3VdbeIntegerAffinity(pRec);
    }
  }
}

/*
** Try to convert the type of a function argument or a result column
** into a numeric representation.  Use either INTEGER or REAL whichever
** is appropriate.  But only do the conversion if it is possible without
** loss of information and return the revised type of the argument.
**
** This is an EXPERIMENTAL api and is subject to change or removal.
*/
int sqlite3_value_numeric_type(sqlite3_value *pVal){
  Mem *pMem = (Mem*)pVal;
  applyNumericAffinity(pMem);
  storeTypeInfo(pMem, 0);
  return pMem->type;
}

/*
** Exported version of applyAffinity(). This one works on sqlite3_value*, 
** not the internal Mem* type.
*/
void sqlite3ValueApplyAffinity(sqlite3_value *pVal, u8 affinity, u8 enc){
  applyAffinity((Mem *)pVal, affinity, enc);
}

#ifdef SQLITE_DEBUG
/*
** Write a nice string representation of the contents of cell pMem
** into buffer zBuf, length nBuf.
*/
void sqlite3VdbeMemPrettyPrint(Mem *pMem, char *zBuf){
  char *zCsr = zBuf;
  int f = pMem->flags;

  static const char *const encnames[] = {"(X)", "(8)", "(16LE)", "(16BE)"};

  if( f&MEM_Blob ){
    int i;
    char c;
    if( f & MEM_Dyn ){
      c = 'z';
      assert( (f & (MEM_Static|MEM_Ephem))==0 );
    }else if( f & MEM_Static ){
      c = 't';
      assert( (f & (MEM_Dyn|MEM_Ephem))==0 );
    }else if( f & MEM_Ephem ){
      c = 'e';
      assert( (f & (MEM_Static|MEM_Dyn))==0 );
    }else{
      c = 's';
    }

    zCsr += sprintf(zCsr, "%c", c);
    zCsr += sprintf(zCsr, "%d[", pMem->n);
    for(i=0; i<16 && i<pMem->n; i++){
      zCsr += sprintf(zCsr, "%02X ", ((int)pMem->z[i] & 0xFF));
    }
    for(i=0; i<16 && i<pMem->n; i++){
      char z = pMem->z[i];
      if( z<32 || z>126 ) *zCsr++ = '.';
      else *zCsr++ = z;
    }

    zCsr += sprintf(zCsr, "]");
    *zCsr = '\0';
  }else if( f & MEM_Str ){
    int j, k;
    zBuf[0] = ' ';
    if( f & MEM_Dyn ){
      zBuf[1] = 'z';
      assert( (f & (MEM_Static|MEM_Ephem))==0 );
    }else if( f & MEM_Static ){
      zBuf[1] = 't';
      assert( (f & (MEM_Dyn|MEM_Ephem))==0 );
    }else if( f & MEM_Ephem ){
      zBuf[1] = 'e';
      assert( (f & (MEM_Static|MEM_Dyn))==0 );
    }else{
      zBuf[1] = 's';
    }
    k = 2;
    k += sprintf(&zBuf[k], "%d", pMem->n);
    zBuf[k++] = '[';
    for(j=0; j<15 && j<pMem->n; j++){
      u8 c = pMem->z[j];
      if( c>=0x20 && c<0x7f ){
        zBuf[k++] = c;
      }else{
        zBuf[k++] = '.';
      }
    }
    zBuf[k++] = ']';
    k += sprintf(&zBuf[k], encnames[pMem->enc]);
    zBuf[k++] = 0;
  }
}
#endif


#ifdef VDBE_PROFILE
/*
** The following routine only works on pentium-class processors.
** It uses the RDTSC opcode to read the cycle count value out of the
** processor and returns that value.  This can be used for high-res
** profiling.
*/
__inline__ unsigned long long int hwtime(void){
  unsigned long long int x;
  __asm__("rdtsc\n\t"
          "mov %%edx, %%ecx\n\t"
          :"=A" (x));
  return x;
}
#endif

/*
** The CHECK_FOR_INTERRUPT macro defined here looks to see if the
** sqlite3_interrupt() routine has been called.  If it has been, then
** processing of the VDBE program is interrupted.
**
** This macro added to every instruction that does a jump in order to
** implement a loop.  This test used to be on every single instruction,
** but that meant we more testing that we needed.  By only testing the
** flag on jump instructions, we get a (small) speed improvement.
*/
#define CHECK_FOR_INTERRUPT \
   if( db->u1.isInterrupted ) goto abort_due_to_interrupt;


/*
** Execute as much of a VDBE program as we can then return.
**
** sqlite3VdbeMakeReady() must be called before this routine in order to
** close the program with a final OP_Halt and to set up the callbacks
** and the error message pointer.
**
** Whenever a row or result data is available, this routine will either
** invoke the result callback (if there is one) or return with
** SQLITE_ROW.
**
** If an attempt is made to open a locked database, then this routine
** will either invoke the busy callback (if there is one) or it will
** return SQLITE_BUSY.
**
** If an error occurs, an error message is written to memory obtained
** from sqliteMalloc() and p->zErrMsg is made to point to that memory.
** The error code is stored in p->rc and this routine returns SQLITE_ERROR.
**
** If the callback ever returns non-zero, then the program exits
** immediately.  There will be no error message but the p->rc field is
** set to SQLITE_ABORT and this routine will return SQLITE_ERROR.
**
** A memory allocation error causes p->rc to be set to SQLITE_NOMEM and this
** routine to return SQLITE_ERROR.
**
** Other fatal errors return SQLITE_ERROR.
**
** After this routine has finished, sqlite3VdbeFinalize() should be
** used to clean up the mess that was left behind.
*/
int sqlite3VdbeExec(
  Vdbe *p                    /* The VDBE */
){
  int pc;                    /* The program counter */
  Op *pOp;                   /* Current operation */
  int rc = SQLITE_OK;        /* Value to return */
  sqlite3 *db = p->db;       /* The database */
  u8 encoding = ENC(db);     /* The database encoding */
  Mem *pTos;                 /* Top entry in the operand stack */
#ifdef VDBE_PROFILE
  unsigned long long start;  /* CPU clock count at start of opcode */
  int origPc;                /* Program counter at start of opcode */
#endif
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  int nProgressOps = 0;      /* Opcodes executed since progress callback. */
#endif
#ifndef NDEBUG
  Mem *pStackLimit;
#endif

  if( p->magic!=VDBE_MAGIC_RUN ) return SQLITE_MISUSE;
  assert( db->magic==SQLITE_MAGIC_BUSY );
  pTos = p->pTos;
  if( p->rc==SQLITE_NOMEM ){
    /* This happens if a malloc() inside a call to sqlite3_column_text() or
    ** sqlite3_column_text16() failed.  */
    goto no_mem;
  }
  assert( p->rc==SQLITE_OK || p->rc==SQLITE_BUSY );
  p->rc = SQLITE_OK;
  assert( p->explain==0 );
  if( p->popStack ){
    popStack(&pTos, p->popStack);
    p->popStack = 0;
  }
  p->resOnStack = 0;
  db->busyHandler.nBusy = 0;
  CHECK_FOR_INTERRUPT;
  sqlite3VdbeIOTraceSql(p);
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
  for(pc=p->pc; rc==SQLITE_OK; pc++){
    assert( pc>=0 && pc<p->nOp );
    assert( pTos<=&p->aStack[pc] );
    if( sqlite3MallocFailed() ) goto no_mem;
#ifdef VDBE_PROFILE
    origPc = pc;
    start = hwtime();
#endif
    pOp = &p->aOp[pc];

    /* Only allow tracing if SQLITE_DEBUG is defined.
    */
#ifdef SQLITE_DEBUG
    if( p->trace ){
      if( pc==0 ){
        printf("VDBE Execution Trace:\n");
        sqlite3VdbePrintSql(p);
      }
      sqlite3VdbePrintOp(p->trace, pc, pOp);
    }
    if( p->trace==0 && pc==0 && sqlite3OsFileExists("vdbe_sqltrace") ){
      sqlite3VdbePrintSql(p);
    }
#endif
      

    /* Check to see if we need to simulate an interrupt.  This only happens
    ** if we have a special test build.
    */
#ifdef SQLITE_TEST
    if( sqlite3_interrupt_count>0 ){
      sqlite3_interrupt_count--;
      if( sqlite3_interrupt_count==0 ){
        sqlite3_interrupt(db);
      }
    }
#endif

#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
    /* Call the progress callback if it is configured and the required number
    ** of VDBE ops have been executed (either since this invocation of
    ** sqlite3VdbeExec() or since last time the progress callback was called).
    ** If the progress callback returns non-zero, exit the virtual machine with
    ** a return code SQLITE_ABORT.
    */
    if( db->xProgress ){
      if( db->nProgressOps==nProgressOps ){
        if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
        if( db->xProgress(db->pProgressArg)!=0 ){
          sqlite3SafetyOn(db);
          rc = SQLITE_ABORT;
          continue; /* skip to the next iteration of the for loop */
        }
        nProgressOps = 0;
        if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
      }
      nProgressOps++;
    }
#endif

#ifndef NDEBUG
    /* This is to check that the return value of static function
    ** opcodeNoPush() (see vdbeaux.c) returns values that match the
    ** implementation of the virtual machine in this file. If
    ** opcodeNoPush() returns non-zero, then the stack is guarenteed
    ** not to grow when the opcode is executed. If it returns zero, then
    ** the stack may grow by at most 1.
    **
    ** The global wrapper function sqlite3VdbeOpcodeUsesStack() is not 
    ** available if NDEBUG is defined at build time.
    */ 
    pStackLimit = pTos;
    if( !sqlite3VdbeOpcodeNoPush(pOp->opcode) ){
      pStackLimit++;
    }
#endif

    switch( pOp->opcode ){

/*****************************************************************************
** What follows is a massive switch statement where each case implements a
** separate instruction in the virtual machine.  If we follow the usual
** indentation conventions, each case should be indented by 6 spaces.  But
** that is a lot of wasted space on the left margin.  So the code within
** the switch statement will break with convention and be flush-left. Another
** big comment (similar to this one) will mark the point in the code where
** we transition back to normal indentation.
**
** The formatting of each case is important.  The makefile for SQLite
** generates two C files "opcodes.h" and "opcodes.c" by scanning this
** file looking for lines that begin with "case OP_".  The opcodes.h files
** will be filled with #defines that give unique integer values to each
** opcode and the opcodes.c file is filled with an array of strings where
** each string is the symbolic name for the corresponding opcode.  If the
** case statement is followed by a comment of the form "/# same as ... #/"
** that comment is used to determine the particular value of the opcode.
**
** If a comment on the same line as the "case OP_" construction contains
** the word "no-push", then the opcode is guarenteed not to grow the 
** vdbe stack when it is executed. See function opcode() in
** vdbeaux.c for details.
**
** Documentation about VDBE opcodes is generated by scanning this file
** for lines of that contain "Opcode:".  That line and all subsequent
** comment lines are used in the generation of the opcode.html documentation
** file.
**
** SUMMARY:
**
**     Formatting is important to scripts that scan this file.
**     Do not deviate from the formatting style currently in use.
**
*****************************************************************************/

/* Opcode:  Goto * P2 *
**
** An unconditional jump to address P2.
** The next instruction executed will be 
** the one at index P2 from the beginning of
** the program.
*/
case OP_Goto: {             /* no-push */
  CHECK_FOR_INTERRUPT;
  pc = pOp->p2 - 1;
  break;
}

/* Opcode:  Gosub * P2 *
**
** Push the current address plus 1 onto the return address stack
** and then jump to address P2.
**
** The return address stack is of limited depth.  If too many
** OP_Gosub operations occur without intervening OP_Returns, then
** the return address stack will fill up and processing will abort
** with a fatal error.
*/
case OP_Gosub: {            /* no-push */
  assert( p->returnDepth<sizeof(p->returnStack)/sizeof(p->returnStack[0]) );
  p->returnStack[p->returnDepth++] = pc+1;
  pc = pOp->p2 - 1;
  break;
}

/* Opcode:  Return * * *
**
** Jump immediately to the next instruction after the last unreturned
** OP_Gosub.  If an OP_Return has occurred for all OP_Gosubs, then
** processing aborts with a fatal error.
*/
case OP_Return: {           /* no-push */
  assert( p->returnDepth>0 );
  p->returnDepth--;
  pc = p->returnStack[p->returnDepth] - 1;
  break;
}

/* Opcode:  Halt P1 P2 P3
**
** Exit immediately.  All open cursors, Fifos, etc are closed
** automatically.
**
** P1 is the result code returned by sqlite3_exec(), sqlite3_reset(),
** or sqlite3_finalize().  For a normal halt, this should be SQLITE_OK (0).
** For errors, it can be some other value.  If P1!=0 then P2 will determine
** whether or not to rollback the current transaction.  Do not rollback
** if P2==OE_Fail. Do the rollback if P2==OE_Rollback.  If P2==OE_Abort,
** then back out all changes that have occurred during this execution of the
** VDBE, but do not rollback the transaction. 
**
** If P3 is not null then it is an error message string.
**
** There is an implied "Halt 0 0 0" instruction inserted at the very end of
** every program.  So a jump past the last instruction of the program
** is the same as executing Halt.
*/
case OP_Halt: {            /* no-push */
  p->pTos = pTos;
  p->rc = pOp->p1;
  p->pc = pc;
  p->errorAction = pOp->p2;
  if( pOp->p3 ){
    sqlite3SetString(&p->zErrMsg, pOp->p3, (char*)0);
  }
  rc = sqlite3VdbeHalt(p);
  assert( rc==SQLITE_BUSY || rc==SQLITE_OK );
  if( rc==SQLITE_BUSY ){
    p->rc = SQLITE_BUSY;
    return SQLITE_BUSY;
  }
  return p->rc ? SQLITE_ERROR : SQLITE_DONE;
}

/* Opcode: Integer P1 * *
**
** The 32-bit integer value P1 is pushed onto the stack.
*/
case OP_Integer: {
  pTos++;
  pTos->flags = MEM_Int;
  pTos->u.i = pOp->p1;
  break;
}

/* Opcode: Int64 * * P3
**
** P3 is a string representation of an integer.  Convert that integer
** to a 64-bit value and push it onto the stack.
*/
case OP_Int64: {
  pTos++;
  assert( pOp->p3!=0 );
  pTos->flags = MEM_Str|MEM_Static|MEM_Term;
  pTos->z = pOp->p3;
  pTos->n = strlen(pTos->z);
  pTos->enc = SQLITE_UTF8;
  pTos->u.i = sqlite3VdbeIntValue(pTos);
  pTos->flags |= MEM_Int;
  break;
}

/* Opcode: Real * * P3
**
** The string value P3 is converted to a real and pushed on to the stack.
*/
case OP_Real: {            /* same as TK_FLOAT, */
  pTos++;
  pTos->flags = MEM_Str|MEM_Static|MEM_Term;
  pTos->z = pOp->p3;
  pTos->n = strlen(pTos->z);
  pTos->enc = SQLITE_UTF8;
  pTos->r = sqlite3VdbeRealValue(pTos);
  pTos->flags |= MEM_Real;
  sqlite3VdbeChangeEncoding(pTos, encoding);
  break;
}

/* Opcode: String8 * * P3
**
** P3 points to a nul terminated UTF-8 string. This opcode is transformed 
** into an OP_String before it is executed for the first time.
*/
case OP_String8: {         /* same as TK_STRING */
  assert( pOp->p3!=0 );
  pOp->opcode = OP_String;
  pOp->p1 = strlen(pOp->p3);

#ifndef SQLITE_OMIT_UTF16
  if( encoding!=SQLITE_UTF8 ){
    pTos++;
    sqlite3VdbeMemSetStr(pTos, pOp->p3, -1, SQLITE_UTF8, SQLITE_STATIC);
    if( SQLITE_OK!=sqlite3VdbeChangeEncoding(pTos, encoding) ) goto no_mem;
    if( SQLITE_OK!=sqlite3VdbeMemDynamicify(pTos) ) goto no_mem;
    pTos->flags &= ~(MEM_Dyn);
    pTos->flags |= MEM_Static;
    if( pOp->p3type==P3_DYNAMIC ){
      sqliteFree(pOp->p3);
    }
    pOp->p3type = P3_DYNAMIC;
    pOp->p3 = pTos->z;
    pOp->p1 = pTos->n;
    break;
  }
#endif
  /* Otherwise fall through to the next case, OP_String */
}
  
/* Opcode: String P1 * P3
**
** The string value P3 of length P1 (bytes) is pushed onto the stack.
*/
case OP_String: {
  pTos++;
  assert( pOp->p3!=0 );
  pTos->flags = MEM_Str|MEM_Static|MEM_Term;
  pTos->z = pOp->p3;
  pTos->n = pOp->p1;
  pTos->enc = encoding;
  break;
}

/* Opcode: Null * * *
**
** Push a NULL onto the stack.
*/
case OP_Null: {
  pTos++;
  pTos->flags = MEM_Null;
  pTos->n = 0;
  break;
}


#ifndef SQLITE_OMIT_BLOB_LITERAL
/* Opcode: HexBlob * * P3
**
** P3 is an UTF-8 SQL hex encoding of a blob. The blob is pushed onto the
** vdbe stack.
**
** The first time this instruction executes, in transforms itself into a
** 'Blob' opcode with a binary blob as P3.
*/
case OP_HexBlob: {            /* same as TK_BLOB */
  pOp->opcode = OP_Blob;
  pOp->p1 = strlen(pOp->p3)/2;
  if( pOp->p1 ){
    char *zBlob = sqlite3HexToBlob(pOp->p3);
    if( !zBlob ) goto no_mem;
    if( pOp->p3type==P3_DYNAMIC ){
      sqliteFree(pOp->p3);
    }
    pOp->p3 = zBlob;
    pOp->p3type = P3_DYNAMIC;
  }else{
    if( pOp->p3type==P3_DYNAMIC ){
      sqliteFree(pOp->p3);
    }
    pOp->p3type = P3_STATIC;
    pOp->p3 = "";
  }

  /* Fall through to the next case, OP_Blob. */
}

/* Opcode: Blob P1 * P3
**
** P3 points to a blob of data P1 bytes long. Push this
** value onto the stack. This instruction is not coded directly
** by the compiler. Instead, the compiler layer specifies
** an OP_HexBlob opcode, with the hex string representation of
** the blob as P3. This opcode is transformed to an OP_Blob
** the first time it is executed.
*/
case OP_Blob: {
  pTos++;
  sqlite3VdbeMemSetStr(pTos, pOp->p3, pOp->p1, 0, 0);
  break;
}
#endif /* SQLITE_OMIT_BLOB_LITERAL */

/* Opcode: Variable P1 * *
**
** Push the value of variable P1 onto the stack.  A variable is
** an unknown in the original SQL string as handed to sqlite3_compile().
** Any occurance of the '?' character in the original SQL is considered
** a variable.  Variables in the SQL string are number from left to
** right beginning with 1.  The values of variables are set using the
** sqlite3_bind() API.
*/
case OP_Variable: {
  int j = pOp->p1 - 1;
  assert( j>=0 && j<p->nVar );

  pTos++;
  sqlite3VdbeMemShallowCopy(pTos, &p->aVar[j], MEM_Static);
  break;
}

/* Opcode: Pop P1 * *
**
** P1 elements are popped off of the top of stack and discarded.
*/
case OP_Pop: {            /* no-push */
  assert( pOp->p1>=0 );
  popStack(&pTos, pOp->p1);
  assert( pTos>=&p->aStack[-1] );
  break;
}

/* Opcode: Dup P1 P2 *
**
** A copy of the P1-th element of the stack 
** is made and pushed onto the top of the stack.
** The top of the stack is element 0.  So the
** instruction "Dup 0 0 0" will make a copy of the
** top of the stack.
**
** If the content of the P1-th element is a dynamically
** allocated string, then a new copy of that string
** is made if P2==0.  If P2!=0, then just a pointer
** to the string is copied.
**
** Also see the Pull instruction.
*/
case OP_Dup: {
  Mem *pFrom = &pTos[-pOp->p1];
  assert( pFrom<=pTos && pFrom>=p->aStack );
  pTos++;
  sqlite3VdbeMemShallowCopy(pTos, pFrom, MEM_Ephem);
  if( pOp->p2 ){
    Deephemeralize(pTos);
  }
  break;
}

/* Opcode: Pull P1 * *
**
** The P1-th element is removed from its current location on 
** the stack and pushed back on top of the stack.  The
** top of the stack is element 0, so "Pull 0 0 0" is
** a no-op.  "Pull 1 0 0" swaps the top two elements of
** the stack.
**
** See also the Dup instruction.
*/
case OP_Pull: {            /* no-push */
  Mem *pFrom = &pTos[-pOp->p1];
  int i;
  Mem ts;

  ts = *pFrom;
  Deephemeralize(pTos);
  for(i=0; i<pOp->p1; i++, pFrom++){
    Deephemeralize(&pFrom[1]);
    assert( (pFrom->flags & MEM_Ephem)==0 );
    *pFrom = pFrom[1];
    if( pFrom->flags & MEM_Short ){
      assert( pFrom->flags & (MEM_Str|MEM_Blob) );
      assert( pFrom->z==pFrom[1].zShort );
      pFrom->z = pFrom->zShort;
    }
  }
  *pTos = ts;
  if( pTos->flags & MEM_Short ){
    assert( pTos->flags & (MEM_Str|MEM_Blob) );
    assert( pTos->z==pTos[-pOp->p1].zShort );
    pTos->z = pTos->zShort;
  }
  break;
}

/* Opcode: Push P1 * *
**
** Overwrite the value of the P1-th element down on the
** stack (P1==0 is the top of the stack) with the value
** of the top of the stack.  Then pop the top of the stack.
*/
case OP_Push: {            /* no-push */
  Mem *pTo = &pTos[-pOp->p1];

  assert( pTo>=p->aStack );
  sqlite3VdbeMemMove(pTo, pTos);
  pTos--;
  break;
}

/* Opcode: Callback P1 * *
**
** The top P1 values on the stack represent a single result row from
** a query.  This opcode causes the sqlite3_step() call to terminate
** with an SQLITE_ROW return code and it sets up the sqlite3_stmt
** structure to provide access to the top P1 values as the result
** row.  When the sqlite3_step() function is run again, the top P1
** values will be automatically popped from the stack before the next
** instruction executes.
*/
case OP_Callback: {            /* no-push */
  Mem *pMem;
  Mem *pFirstColumn;
  assert( p->nResColumn==pOp->p1 );

  /* Data in the pager might be moved or changed out from under us
  ** in between the return from this sqlite3_step() call and the
  ** next call to sqlite3_step().  So deephermeralize everything on 
  ** the stack.  Note that ephemeral data is never stored in memory 
  ** cells so we do not have to worry about them.
  */
  pFirstColumn = &pTos[0-pOp->p1];
  for(pMem = p->aStack; pMem<pFirstColumn; pMem++){
    Deephemeralize(pMem);
  }

  /* Invalidate all ephemeral cursor row caches */
  p->cacheCtr = (p->cacheCtr + 2)|1;

  /* Make sure the results of the current row are \000 terminated
  ** and have an assigned type.  The results are deephemeralized as
  ** as side effect.
  */
  for(; pMem<=pTos; pMem++ ){
    sqlite3VdbeMemNulTerminate(pMem);
    storeTypeInfo(pMem, encoding);
  }

  /* Set up the statement structure so that it will pop the current
  ** results from the stack when the statement returns.
  */
  p->resOnStack = 1;
  p->nCallback++;
  p->popStack = pOp->p1;
  p->pc = pc + 1;
  p->pTos = pTos;
  return SQLITE_ROW;
}

/* Opcode: Concat P1 P2 *
**
** Look at the first P1+2 elements of the stack.  Append them all 
** together with the lowest element first.  The original P1+2 elements
** are popped from the stack if P2==0 and retained if P2==1.  If
** any element of the stack is NULL, then the result is NULL.
**
** When P1==1, this routine makes a copy of the top stack element
** into memory obtained from sqliteMalloc().
*/
case OP_Concat: {           /* same as TK_CONCAT */
  char *zNew;
  int nByte;
  int nField;
  int i, j;
  Mem *pTerm;

  /* Loop through the stack elements to see how long the result will be. */
  nField = pOp->p1 + 2;
  pTerm = &pTos[1-nField];
  nByte = 0;
  for(i=0; i<nField; i++, pTerm++){
    assert( pOp->p2==0 || (pTerm->flags&MEM_Str) );
    if( pTerm->flags&MEM_Null ){
      nByte = -1;
      break;
    }
    Stringify(pTerm, encoding);
    nByte += pTerm->n;
  }

  if( nByte<0 ){
    /* If nByte is less than zero, then there is a NULL value on the stack.
    ** In this case just pop the values off the stack (if required) and
    ** push on a NULL.
    */
    if( pOp->p2==0 ){
      popStack(&pTos, nField);
    }
    pTos++;
    pTos->flags = MEM_Null;
  }else{
    /* Otherwise malloc() space for the result and concatenate all the
    ** stack values.
    */
    zNew = sqliteMallocRaw( nByte+2 );
    if( zNew==0 ) goto no_mem;
    j = 0;
    pTerm = &pTos[1-nField];
    for(i=j=0; i<nField; i++, pTerm++){
      int n = pTerm->n;
      assert( pTerm->flags & (MEM_Str|MEM_Blob) );
      memcpy(&zNew[j], pTerm->z, n);
      j += n;
    }
    zNew[j] = 0;
    zNew[j+1] = 0;
    assert( j==nByte );

    if( pOp->p2==0 ){
      popStack(&pTos, nField);
    }
    pTos++;
    pTos->n = j;
    pTos->flags = MEM_Str|MEM_Dyn|MEM_Term;
    pTos->xDel = 0;
    pTos->enc = encoding;
    pTos->z = zNew;
  }
  break;
}

/* Opcode: Add * * *
**
** Pop the top two elements from the stack, add them together,
** and push the result back onto the stack.  If either element
** is a string then it is converted to a double using the atof()
** function before the addition.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: Multiply * * *
**
** Pop the top two elements from the stack, multiply them together,
** and push the result back onto the stack.  If either element
** is a string then it is converted to a double using the atof()
** function before the multiplication.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: Subtract * * *
**
** Pop the top two elements from the stack, subtract the
** first (what was on top of the stack) from the second (the
** next on stack)
** and push the result back onto the stack.  If either element
** is a string then it is converted to a double using the atof()
** function before the subtraction.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: Divide * * *
**
** Pop the top two elements from the stack, divide the
** first (what was on top of the stack) from the second (the
** next on stack)
** and push the result back onto the stack.  If either element
** is a string then it is converted to a double using the atof()
** function before the division.  Division by zero returns NULL.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: Remainder * * *
**
** Pop the top two elements from the stack, divide the
** first (what was on top of the stack) from the second (the
** next on stack)
** and push the remainder after division onto the stack.  If either element
** is a string then it is converted to a double using the atof()
** function before the division.  Division by zero returns NULL.
** If either operand is NULL, the result is NULL.
*/
case OP_Add:                   /* same as TK_PLUS, no-push */
case OP_Subtract:              /* same as TK_MINUS, no-push */
case OP_Multiply:              /* same as TK_STAR, no-push */
case OP_Divide:                /* same as TK_SLASH, no-push */
case OP_Remainder: {           /* same as TK_REM, no-push */
  Mem *pNos = &pTos[-1];
  int flags;
  assert( pNos>=p->aStack );
  flags = pTos->flags | pNos->flags;
  if( (flags & MEM_Null)!=0 ){
    Release(pTos);
    pTos--;
    Release(pTos);
    pTos->flags = MEM_Null;
  }else if( (pTos->flags & pNos->flags & MEM_Int)==MEM_Int ){
    i64 a, b;
    a = pTos->u.i;
    b = pNos->u.i;
    switch( pOp->opcode ){
      case OP_Add:         b += a;       break;
      case OP_Subtract:    b -= a;       break;
      case OP_Multiply:    b *= a;       break;
      case OP_Divide: {
        if( a==0 ) goto divide_by_zero;
        b /= a;
        break;
      }
      default: {
        if( a==0 ) goto divide_by_zero;
        b %= a;
        break;
      }
    }
    Release(pTos);
    pTos--;
    Release(pTos);
    pTos->u.i = b;
    pTos->flags = MEM_Int;
  }else{
    double a, b;
    a = sqlite3VdbeRealValue(pTos);
    b = sqlite3VdbeRealValue(pNos);
    switch( pOp->opcode ){
      case OP_Add:         b += a;       break;
      case OP_Subtract:    b -= a;       break;
      case OP_Multiply:    b *= a;       break;
      case OP_Divide: {
        if( a==0.0 ) goto divide_by_zero;
        b /= a;
        break;
      }
      default: {
        int ia = (int)a;
        int ib = (int)b;
        if( ia==0.0 ) goto divide_by_zero;
        b = ib % ia;
        break;
      }
    }
    Release(pTos);
    pTos--;
    Release(pTos);
    pTos->r = b;
    pTos->flags = MEM_Real;
    if( (flags & MEM_Real)==0 ){
      sqlite3VdbeIntegerAffinity(pTos);
    }
  }
  break;

divide_by_zero:
  Release(pTos);
  pTos--;
  Release(pTos);
  pTos->flags = MEM_Null;
  break;
}

/* Opcode: CollSeq * * P3
**
** P3 is a pointer to a CollSeq struct. If the next call to a user function
** or aggregate calls sqlite3GetFuncCollSeq(), this collation sequence will
** be returned. This is used by the built-in min(), max() and nullif()
** functions.
**
** The interface used by the implementation of the aforementioned functions
** to retrieve the collation sequence set by this opcode is not available
** publicly, only to user functions defined in func.c.
*/
case OP_CollSeq: {             /* no-push */
  assert( pOp->p3type==P3_COLLSEQ );
  break;
}

/* Opcode: Function P1 P2 P3
**
** Invoke a user function (P3 is a pointer to a Function structure that
** defines the function) with P2 arguments taken from the stack.  Pop all
** arguments from the stack and push back the result.
**
** P1 is a 32-bit bitmask indicating whether or not each argument to the 
** function was determined to be constant at compile time. If the first
** argument was constant then bit 0 of P1 is set. This is used to determine
** whether meta data associated with a user function argument using the
** sqlite3_set_auxdata() API may be safely retained until the next
** invocation of this opcode.
**
** See also: AggStep and AggFinal
*/
case OP_Function: {
  int i;
  Mem *pArg;
  sqlite3_context ctx;
  sqlite3_value **apVal;
  int n = pOp->p2;

  apVal = p->apArg;
  assert( apVal || n==0 );

  pArg = &pTos[1-n];
  for(i=0; i<n; i++, pArg++){
    apVal[i] = pArg;
    storeTypeInfo(pArg, encoding);
  }

  assert( pOp->p3type==P3_FUNCDEF || pOp->p3type==P3_VDBEFUNC );
  if( pOp->p3type==P3_FUNCDEF ){
    ctx.pFunc = (FuncDef*)pOp->p3;
    ctx.pVdbeFunc = 0;
  }else{
    ctx.pVdbeFunc = (VdbeFunc*)pOp->p3;
    ctx.pFunc = ctx.pVdbeFunc->pFunc;
  }

  ctx.s.flags = MEM_Null;
  ctx.s.z = 0;
  ctx.s.xDel = 0;
  ctx.isError = 0;
  if( ctx.pFunc->needCollSeq ){
    assert( pOp>p->aOp );
    assert( pOp[-1].p3type==P3_COLLSEQ );
    assert( pOp[-1].opcode==OP_CollSeq );
    ctx.pColl = (CollSeq *)pOp[-1].p3;
  }
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  (*ctx.pFunc->xFunc)(&ctx, n, apVal);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  if( sqlite3MallocFailed() ) goto no_mem;
  popStack(&pTos, n);

  /* If any auxilary data functions have been called by this user function,
  ** immediately call the destructor for any non-static values.
  */
  if( ctx.pVdbeFunc ){
    sqlite3VdbeDeleteAuxData(ctx.pVdbeFunc, pOp->p1);
    pOp->p3 = (char *)ctx.pVdbeFunc;
    pOp->p3type = P3_VDBEFUNC;
  }

  /* If the function returned an error, throw an exception */
  if( ctx.isError ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(&ctx.s), (char*)0);
    rc = SQLITE_ERROR;
  }

  /* Copy the result of the function to the top of the stack */
  sqlite3VdbeChangeEncoding(&ctx.s, encoding);
  pTos++;
  pTos->flags = 0;
  sqlite3VdbeMemMove(pTos, &ctx.s);
  break;
}

/* Opcode: BitAnd * * *
**
** Pop the top two elements from the stack.  Convert both elements
** to integers.  Push back onto the stack the bit-wise AND of the
** two elements.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: BitOr * * *
**
** Pop the top two elements from the stack.  Convert both elements
** to integers.  Push back onto the stack the bit-wise OR of the
** two elements.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: ShiftLeft * * *
**
** Pop the top two elements from the stack.  Convert both elements
** to integers.  Push back onto the stack the second element shifted
** left by N bits where N is the top element on the stack.
** If either operand is NULL, the result is NULL.
*/
/* Opcode: ShiftRight * * *
**
** Pop the top two elements from the stack.  Convert both elements
** to integers.  Push back onto the stack the second element shifted
** right by N bits where N is the top element on the stack.
** If either operand is NULL, the result is NULL.
*/
case OP_BitAnd:                 /* same as TK_BITAND, no-push */
case OP_BitOr:                  /* same as TK_BITOR, no-push */
case OP_ShiftLeft:              /* same as TK_LSHIFT, no-push */
case OP_ShiftRight: {           /* same as TK_RSHIFT, no-push */
  Mem *pNos = &pTos[-1];
  i64 a, b;

  assert( pNos>=p->aStack );
  if( (pTos->flags | pNos->flags) & MEM_Null ){
    popStack(&pTos, 2);
    pTos++;
    pTos->flags = MEM_Null;
    break;
  }
  a = sqlite3VdbeIntValue(pNos);
  b = sqlite3VdbeIntValue(pTos);
  switch( pOp->opcode ){
    case OP_BitAnd:      a &= b;     break;
    case OP_BitOr:       a |= b;     break;
    case OP_ShiftLeft:   a <<= b;    break;
    case OP_ShiftRight:  a >>= b;    break;
    default:   /* CANT HAPPEN */     break;
  }
  Release(pTos);
  pTos--;
  Release(pTos);
  pTos->u.i = a;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: AddImm  P1 * *
** 
** Add the value P1 to whatever is on top of the stack.  The result
** is always an integer.
**
** To force the top of the stack to be an integer, just add 0.
*/
case OP_AddImm: {            /* no-push */
  assert( pTos>=p->aStack );
  sqlite3VdbeMemIntegerify(pTos);
  pTos->u.i += pOp->p1;
  break;
}

/* Opcode: ForceInt P1 P2 *
**
** Convert the top of the stack into an integer.  If the current top of
** the stack is not numeric (meaning that is is a NULL or a string that
** does not look like an integer or floating point number) then pop the
** stack and jump to P2.  If the top of the stack is numeric then
** convert it into the least integer that is greater than or equal to its
** current value if P1==0, or to the least integer that is strictly
** greater than its current value if P1==1.
*/
case OP_ForceInt: {            /* no-push */
  i64 v;
  assert( pTos>=p->aStack );
  applyAffinity(pTos, SQLITE_AFF_NUMERIC, encoding);
  if( (pTos->flags & (MEM_Int|MEM_Real))==0 ){
    Release(pTos);
    pTos--;
    pc = pOp->p2 - 1;
    break;
  }
  if( pTos->flags & MEM_Int ){
    v = pTos->u.i + (pOp->p1!=0);
  }else{
    /* FIX ME:  should this not be assert( pTos->flags & MEM_Real ) ??? */
    sqlite3VdbeMemRealify(pTos);
    v = (int)pTos->r;
    if( pTos->r>(double)v ) v++;
    if( pOp->p1 && pTos->r==(double)v ) v++;
  }
  Release(pTos);
  pTos->u.i = v;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: MustBeInt P1 P2 *
** 
** Force the top of the stack to be an integer.  If the top of the
** stack is not an integer and cannot be converted into an integer
** with out data loss, then jump immediately to P2, or if P2==0
** raise an SQLITE_MISMATCH exception.
**
** If the top of the stack is not an integer and P2 is not zero and
** P1 is 1, then the stack is popped.  In all other cases, the depth
** of the stack is unchanged.
*/
case OP_MustBeInt: {            /* no-push */
  assert( pTos>=p->aStack );
  applyAffinity(pTos, SQLITE_AFF_NUMERIC, encoding);
  if( (pTos->flags & MEM_Int)==0 ){
    if( pOp->p2==0 ){
      rc = SQLITE_MISMATCH;
      goto abort_due_to_error;
    }else{
      if( pOp->p1 ) popStack(&pTos, 1);
      pc = pOp->p2 - 1;
    }
  }else{
    Release(pTos);
    pTos->flags = MEM_Int;
  }
  break;
}

/* Opcode: RealAffinity * * *
**
** If the top of the stack is an integer, convert it to a real value.
**
** This opcode is used when extracting information from a column that
** has REAL affinity.  Such column values may still be stored as
** integers, for space efficiency, but after extraction we want them
** to have only a real value.
*/
case OP_RealAffinity: {                  /* no-push */
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Int ){
    sqlite3VdbeMemRealify(pTos);
  }
  break;
}

#ifndef SQLITE_OMIT_CAST
/* Opcode: ToText * * *
**
** Force the value on the top of the stack to be text.
** If the value is numeric, convert it to a string using the
** equivalent of printf().  Blob values are unchanged and
** are afterwards simply interpreted as text.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToText: {                  /* same as TK_TO_TEXT, no-push */
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Null ) break;
  assert( MEM_Str==(MEM_Blob>>3) );
  pTos->flags |= (pTos->flags&MEM_Blob)>>3;
  applyAffinity(pTos, SQLITE_AFF_TEXT, encoding);
  assert( pTos->flags & MEM_Str );
  pTos->flags &= ~(MEM_Int|MEM_Real|MEM_Blob);
  break;
}

/* Opcode: ToBlob * * *
**
** Force the value on the top of the stack to be a BLOB.
** If the value is numeric, convert it to a string first.
** Strings are simply reinterpreted as blobs with no change
** to the underlying data.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToBlob: {                  /* same as TK_TO_BLOB, no-push */
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Null ) break;
  if( (pTos->flags & MEM_Blob)==0 ){
    applyAffinity(pTos, SQLITE_AFF_TEXT, encoding);
    assert( pTos->flags & MEM_Str );
    pTos->flags |= MEM_Blob;
  }
  pTos->flags &= ~(MEM_Int|MEM_Real|MEM_Str);
  break;
}

/* Opcode: ToNumeric * * *
**
** Force the value on the top of the stack to be numeric (either an
** integer or a floating-point number.)
** If the value is text or blob, try to convert it to an using the
** equivalent of atoi() or atof() and store 0 if no such conversion 
** is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToNumeric: {                  /* same as TK_TO_NUMERIC, no-push */
  assert( pTos>=p->aStack );
  if( (pTos->flags & MEM_Null)==0 ){
    sqlite3VdbeMemNumerify(pTos);
  }
  break;
}
#endif /* SQLITE_OMIT_CAST */

/* Opcode: ToInt * * *
**
** Force the value on the top of the stack to be an integer.  If
** The value is currently a real number, drop its fractional part.
** If the value is text or blob, try to convert it to an integer using the
** equivalent of atoi() and store 0 if no such conversion is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToInt: {                  /* same as TK_TO_INT, no-push */
  assert( pTos>=p->aStack );
  if( (pTos->flags & MEM_Null)==0 ){
    sqlite3VdbeMemIntegerify(pTos);
  }
  break;
}

#ifndef SQLITE_OMIT_CAST
/* Opcode: ToReal * * *
**
** Force the value on the top of the stack to be a floating point number.
** If The value is currently an integer, convert it.
** If the value is text or blob, try to convert it to an integer using the
** equivalent of atoi() and store 0 if no such conversion is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToReal: {                  /* same as TK_TO_REAL, no-push */
  assert( pTos>=p->aStack );
  if( (pTos->flags & MEM_Null)==0 ){
    sqlite3VdbeMemRealify(pTos);
  }
  break;
}
#endif /* SQLITE_OMIT_CAST */

/* Opcode: Eq P1 P2 P3
**
** Pop the top two elements from the stack.  If they are equal, then
** jump to instruction P2.  Otherwise, continue to the next instruction.
**
** If the 0x100 bit of P1 is true and either operand is NULL then take the
** jump.  If the 0x100 bit of P1 is clear then fall thru if either operand
** is NULL.
**
** If the 0x200 bit of P1 is set and either operand is NULL then
** both operands are converted to integers prior to comparison.
** NULL operands are converted to zero and non-NULL operands are
** converted to 1.  Thus, for example, with 0x200 set,  NULL==NULL is true
** whereas it would normally be NULL.  Similarly,  NULL==123 is false when
** 0x200 is set but is NULL when the 0x200 bit of P1 is clear.
**
** The least significant byte of P1 (mask 0xff) must be an affinity character -
** SQLITE_AFF_TEXT, SQLITE_AFF_INTEGER, and so forth. An attempt is made 
** to coerce both values
** according to the affinity before the comparison is made. If the byte is
** 0x00, then numeric affinity is used.
**
** Once any conversions have taken place, and neither value is NULL, 
** the values are compared. If both values are blobs, or both are text,
** then memcmp() is used to determine the results of the comparison. If
** both values are numeric, then a numeric comparison is used. If the
** two values are of different types, then they are inequal.
**
** If P2 is zero, do not jump.  Instead, push an integer 1 onto the
** stack if the jump would have been taken, or a 0 if not.  Push a
** NULL if either operand was NULL.
**
** If P3 is not NULL it is a pointer to a collating sequence (a CollSeq
** structure) that defines how to compare text.
*/
/* Opcode: Ne P1 P2 P3
**
** This works just like the Eq opcode except that the jump is taken if
** the operands from the stack are not equal.  See the Eq opcode for
** additional information.
*/
/* Opcode: Lt P1 P2 P3
**
** This works just like the Eq opcode except that the jump is taken if
** the 2nd element down on the stack is less than the top of the stack.
** See the Eq opcode for additional information.
*/
/* Opcode: Le P1 P2 P3
**
** This works just like the Eq opcode except that the jump is taken if
** the 2nd element down on the stack is less than or equal to the
** top of the stack.  See the Eq opcode for additional information.
*/
/* Opcode: Gt P1 P2 P3
**
** This works just like the Eq opcode except that the jump is taken if
** the 2nd element down on the stack is greater than the top of the stack.
** See the Eq opcode for additional information.
*/
/* Opcode: Ge P1 P2 P3
**
** This works just like the Eq opcode except that the jump is taken if
** the 2nd element down on the stack is greater than or equal to the
** top of the stack.  See the Eq opcode for additional information.
*/
case OP_Eq:               /* same as TK_EQ, no-push */
case OP_Ne:               /* same as TK_NE, no-push */
case OP_Lt:               /* same as TK_LT, no-push */
case OP_Le:               /* same as TK_LE, no-push */
case OP_Gt:               /* same as TK_GT, no-push */
case OP_Ge: {             /* same as TK_GE, no-push */
  Mem *pNos;
  int flags;
  int res;
  char affinity;

  pNos = &pTos[-1];
  flags = pTos->flags|pNos->flags;

  /* If either value is a NULL P2 is not zero, take the jump if the least
  ** significant byte of P1 is true. If P2 is zero, then push a NULL onto
  ** the stack.
  */
  if( flags&MEM_Null ){
    if( (pOp->p1 & 0x200)!=0 ){
      /* The 0x200 bit of P1 means, roughly "do not treat NULL as the
      ** magic SQL value it normally is - treat it as if it were another
      ** integer".
      **
      ** With 0x200 set, if either operand is NULL then both operands
      ** are converted to integers prior to being passed down into the
      ** normal comparison logic below.  NULL operands are converted to
      ** zero and non-NULL operands are converted to 1.  Thus, for example,
      ** with 0x200 set,  NULL==NULL is true whereas it would normally
      ** be NULL.  Similarly,  NULL!=123 is true.
      */
      sqlite3VdbeMemSetInt64(pTos, (pTos->flags & MEM_Null)==0);
      sqlite3VdbeMemSetInt64(pNos, (pNos->flags & MEM_Null)==0);
    }else{
      /* If the 0x200 bit of P1 is clear and either operand is NULL then
      ** the result is always NULL.  The jump is taken if the 0x100 bit
      ** of P1 is set.
      */
      popStack(&pTos, 2);
      if( pOp->p2 ){
        if( pOp->p1 & 0x100 ){
          pc = pOp->p2-1;
        }
      }else{
        pTos++;
        pTos->flags = MEM_Null;
      }
      break;
    }
  }

  affinity = pOp->p1 & 0xFF;
  if( affinity ){
    applyAffinity(pNos, affinity, encoding);
    applyAffinity(pTos, affinity, encoding);
  }

  assert( pOp->p3type==P3_COLLSEQ || pOp->p3==0 );
  res = sqlite3MemCompare(pNos, pTos, (CollSeq*)pOp->p3);
  switch( pOp->opcode ){
    case OP_Eq:    res = res==0;     break;
    case OP_Ne:    res = res!=0;     break;
    case OP_Lt:    res = res<0;      break;
    case OP_Le:    res = res<=0;     break;
    case OP_Gt:    res = res>0;      break;
    default:       res = res>=0;     break;
  }

  popStack(&pTos, 2);
  if( pOp->p2 ){
    if( res ){
      pc = pOp->p2-1;
    }
  }else{
    pTos++;
    pTos->flags = MEM_Int;
    pTos->u.i = res;
  }
  break;
}

/* Opcode: And * * *
**
** Pop two values off the stack.  Take the logical AND of the
** two values and push the resulting boolean value back onto the
** stack. 
*/
/* Opcode: Or * * *
**
** Pop two values off the stack.  Take the logical OR of the
** two values and push the resulting boolean value back onto the
** stack. 
*/
case OP_And:              /* same as TK_AND, no-push */
case OP_Or: {             /* same as TK_OR, no-push */
  Mem *pNos = &pTos[-1];
  int v1, v2;    /* 0==TRUE, 1==FALSE, 2==UNKNOWN or NULL */

  assert( pNos>=p->aStack );
  if( pTos->flags & MEM_Null ){
    v1 = 2;
  }else{
    sqlite3VdbeMemIntegerify(pTos);
    v1 = pTos->u.i==0;
  }
  if( pNos->flags & MEM_Null ){
    v2 = 2;
  }else{
    sqlite3VdbeMemIntegerify(pNos);
    v2 = pNos->u.i==0;
  }
  if( pOp->opcode==OP_And ){
    static const unsigned char and_logic[] = { 0, 1, 2, 1, 1, 1, 2, 1, 2 };
    v1 = and_logic[v1*3+v2];
  }else{
    static const unsigned char or_logic[] = { 0, 0, 0, 0, 1, 2, 0, 2, 2 };
    v1 = or_logic[v1*3+v2];
  }
  popStack(&pTos, 2);
  pTos++;
  if( v1==2 ){
    pTos->flags = MEM_Null;
  }else{
    pTos->u.i = v1==0;
    pTos->flags = MEM_Int;
  }
  break;
}

/* Opcode: Negative * * *
**
** Treat the top of the stack as a numeric quantity.  Replace it
** with its additive inverse.  If the top of the stack is NULL
** its value is unchanged.
*/
/* Opcode: AbsValue * * *
**
** Treat the top of the stack as a numeric quantity.  Replace it
** with its absolute value. If the top of the stack is NULL
** its value is unchanged.
*/
case OP_Negative:              /* same as TK_UMINUS, no-push */
case OP_AbsValue: {
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Real ){
    neg_abs_real_case:
    Release(pTos);
    if( pOp->opcode==OP_Negative || pTos->r<0.0 ){
      pTos->r = -pTos->r;
    }
    pTos->flags = MEM_Real;
  }else if( pTos->flags & MEM_Int ){
    Release(pTos);
    if( pOp->opcode==OP_Negative || pTos->u.i<0 ){
      pTos->u.i = -pTos->u.i;
    }
    pTos->flags = MEM_Int;
  }else if( pTos->flags & MEM_Null ){
    /* Do nothing */
  }else{
    sqlite3VdbeMemNumerify(pTos);
    goto neg_abs_real_case;
  }
  break;
}

/* Opcode: Not * * *
**
** Interpret the top of the stack as a boolean value.  Replace it
** with its complement.  If the top of the stack is NULL its value
** is unchanged.
*/
case OP_Not: {                /* same as TK_NOT, no-push */
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Null ) break;  /* Do nothing to NULLs */
  sqlite3VdbeMemIntegerify(pTos);
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos->u.i = !pTos->u.i;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: BitNot * * *
**
** Interpret the top of the stack as an value.  Replace it
** with its ones-complement.  If the top of the stack is NULL its
** value is unchanged.
*/
case OP_BitNot: {             /* same as TK_BITNOT, no-push */
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Null ) break;  /* Do nothing to NULLs */
  sqlite3VdbeMemIntegerify(pTos);
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos->u.i = ~pTos->u.i;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: Noop * * *
**
** Do nothing.  This instruction is often useful as a jump
** destination.
*/
/*
** The magic Explain opcode are only inserted when explain==2 (which
** is to say when the EXPLAIN QUERY PLAN syntax is used.)
** This opcode records information from the optimizer.  It is the
** the same as a no-op.  This opcodesnever appears in a real VM program.
*/
case OP_Explain:
case OP_Noop: {            /* no-push */
  break;
}

/* Opcode: If P1 P2 *
**
** Pop a single boolean from the stack.  If the boolean popped is
** true, then jump to p2.  Otherwise continue to the next instruction.
** An integer is false if zero and true otherwise.  A string is
** false if it has zero length and true otherwise.
**
** If the value popped of the stack is NULL, then take the jump if P1
** is true and fall through if P1 is false.
*/
/* Opcode: IfNot P1 P2 *
**
** Pop a single boolean from the stack.  If the boolean popped is
** false, then jump to p2.  Otherwise continue to the next instruction.
** An integer is false if zero and true otherwise.  A string is
** false if it has zero length and true otherwise.
**
** If the value popped of the stack is NULL, then take the jump if P1
** is true and fall through if P1 is false.
*/
case OP_If:                 /* no-push */
case OP_IfNot: {            /* no-push */
  int c;
  assert( pTos>=p->aStack );
  if( pTos->flags & MEM_Null ){
    c = pOp->p1;
  }else{
#ifdef SQLITE_OMIT_FLOATING_POINT
    c = sqlite3VdbeIntValue(pTos);
#else
    c = sqlite3VdbeRealValue(pTos)!=0.0;
#endif
    if( pOp->opcode==OP_IfNot ) c = !c;
  }
  Release(pTos);
  pTos--;
  if( c ) pc = pOp->p2-1;
  break;
}

/* Opcode: IsNull P1 P2 *
**
** Check the top of the stack and jump to P2 if the top of the stack
** is NULL.  If P1 is positive, then pop P1 elements from the stack
** regardless of whether or not the jump is taken.  If P1 is negative,
** pop -P1 elements from the stack only if the jump is taken and leave
** the stack unchanged if the jump is not taken.
*/
case OP_IsNull: {            /* same as TK_ISNULL, no-push */
  if( pTos->flags & MEM_Null ){
    pc = pOp->p2-1;
    if( pOp->p1<0 ){
      popStack(&pTos, -pOp->p1);
    }
  }
  if( pOp->p1>0 ){
    popStack(&pTos, pOp->p1);
  }
  break;
}

/* Opcode: NotNull P1 P2 *
**
** Jump to P2 if the top abs(P1) values on the stack are all not NULL.  
** Regardless of whether or not the jump is taken, pop the stack
** P1 times if P1 is greater than zero.  But if P1 is negative,
** leave the stack unchanged.
*/
case OP_NotNull: {            /* same as TK_NOTNULL, no-push */
  int i, cnt;
  cnt = pOp->p1;
  if( cnt<0 ) cnt = -cnt;
  assert( &pTos[1-cnt] >= p->aStack );
  for(i=0; i<cnt && (pTos[1+i-cnt].flags & MEM_Null)==0; i++){}
  if( i>=cnt ) pc = pOp->p2-1;
  if( pOp->p1>0 ) popStack(&pTos, cnt);
  break;
}

/* Opcode: SetNumColumns P1 P2 *
**
** Before the OP_Column opcode can be executed on a cursor, this
** opcode must be called to set the number of fields in the table.
**
** This opcode sets the number of columns for cursor P1 to P2.
**
** If OP_KeyAsData is to be applied to cursor P1, it must be executed
** before this op-code.
*/
case OP_SetNumColumns: {       /* no-push */
  Cursor *pC;
  assert( (pOp->p1)<p->nCursor );
  assert( p->apCsr[pOp->p1]!=0 );
  pC = p->apCsr[pOp->p1];
  pC->nField = pOp->p2;
  break;
}

/* Opcode: Column P1 P2 P3
**
** Interpret the data that cursor P1 points to as a structure built using
** the MakeRecord instruction.  (See the MakeRecord opcode for additional
** information about the format of the data.) Push onto the stack the value
** of the P2-th column contained in the data. If there are less that (P2+1) 
** values in the record, push a NULL onto the stack.
**
** If the KeyAsData opcode has previously executed on this cursor, then the
** field might be extracted from the key rather than the data.
**
** If the column contains fewer than P2 fields, then push a NULL.  Or
** if P3 is of type P3_MEM, then push the P3 value.  The P3 value will
** be default value for a column that has been added using the ALTER TABLE
** ADD COLUMN command.  If P3 is an ordinary string, just push a NULL.
** When P3 is a string it is really just a comment describing the value
** to be pushed, not a default value.
*/
case OP_Column: {
  u32 payloadSize;   /* Number of bytes in the record */
  int p1 = pOp->p1;  /* P1 value of the opcode */
  int p2 = pOp->p2;  /* column number to retrieve */
  Cursor *pC = 0;    /* The VDBE cursor */
  char *zRec;        /* Pointer to complete record-data */
  BtCursor *pCrsr;   /* The BTree cursor */
  u32 *aType;        /* aType[i] holds the numeric type of the i-th column */
  u32 *aOffset;      /* aOffset[i] is offset to start of data for i-th column */
  u32 nField;        /* number of fields in the record */
  int len;           /* The length of the serialized data for the column */
  int i;             /* Loop counter */
  char *zData;       /* Part of the record being decoded */
  Mem sMem;          /* For storing the record being decoded */

  sMem.flags = 0;
  assert( p1<p->nCursor );
  pTos++;
  pTos->flags = MEM_Null;

  /* This block sets the variable payloadSize to be the total number of
  ** bytes in the record.
  **
  ** zRec is set to be the complete text of the record if it is available.
  ** The complete record text is always available for pseudo-tables
  ** If the record is stored in a cursor, the complete record text
  ** might be available in the  pC->aRow cache.  Or it might not be.
  ** If the data is unavailable,  zRec is set to NULL.
  **
  ** We also compute the number of columns in the record.  For cursors,
  ** the number of columns is stored in the Cursor.nField element.  For
  ** records on the stack, the next entry down on the stack is an integer
  ** which is the number of records.
  */
  pC = p->apCsr[p1];
#ifndef SQLITE_OMIT_VIRTUALTABLE
  assert( pC->pVtabCursor==0 );
#endif
  assert( pC!=0 );
  if( pC->pCursor!=0 ){
    /* The record is stored in a B-Tree */
    rc = sqlite3VdbeCursorMoveto(pC);
    if( rc ) goto abort_due_to_error;
    zRec = 0;
    pCrsr = pC->pCursor;
    if( pC->nullRow ){
      payloadSize = 0;
    }else if( pC->cacheStatus==p->cacheCtr ){
      payloadSize = pC->payloadSize;
      zRec = (char*)pC->aRow;
    }else if( pC->isIndex ){
      i64 payloadSize64;
      sqlite3BtreeKeySize(pCrsr, &payloadSize64);
      payloadSize = payloadSize64;
    }else{
      sqlite3BtreeDataSize(pCrsr, &payloadSize);
    }
    nField = pC->nField;
  }else if( pC->pseudoTable ){
    /* The record is the sole entry of a pseudo-table */
    payloadSize = pC->nData;
    zRec = pC->pData;
    pC->cacheStatus = CACHE_STALE;
    assert( payloadSize==0 || zRec!=0 );
    nField = pC->nField;
    pCrsr = 0;
  }else{
    zRec = 0;
    payloadSize = 0;
    pCrsr = 0;
    nField = 0;
  }

  /* If payloadSize is 0, then just push a NULL onto the stack. */
  if( payloadSize==0 ){
    assert( pTos->flags==MEM_Null );
    break;
  }

  assert( p2<nField );

  /* Read and parse the table header.  Store the results of the parse
  ** into the record header cache fields of the cursor.
  */
  if( pC && pC->cacheStatus==p->cacheCtr ){
    aType = pC->aType;
    aOffset = pC->aOffset;
  }else{
    u8 *zIdx;        /* Index into header */
    u8 *zEndHdr;     /* Pointer to first byte after the header */
    u32 offset;      /* Offset into the data */
    int szHdrSz;     /* Size of the header size field at start of record */
    int avail;       /* Number of bytes of available data */

    aType = pC->aType;
    if( aType==0 ){
      pC->aType = aType = sqliteMallocRaw( 2*nField*sizeof(aType) );
    }
    if( aType==0 ){
      goto no_mem;
    }
    pC->aOffset = aOffset = &aType[nField];
    pC->payloadSize = payloadSize;
    pC->cacheStatus = p->cacheCtr;

    /* Figure out how many bytes are in the header */
    if( zRec ){
      zData = zRec;
    }else{
      if( pC->isIndex ){
        zData = (char*)sqlite3BtreeKeyFetch(pCrsr, &avail);
      }else{
        zData = (char*)sqlite3BtreeDataFetch(pCrsr, &avail);
      }
      /* If KeyFetch()/DataFetch() managed to get the entire payload,
      ** save the payload in the pC->aRow cache.  That will save us from
      ** having to make additional calls to fetch the content portion of
      ** the record.
      */
      if( avail>=payloadSize ){
        zRec = zData;
        pC->aRow = (u8*)zData;
      }else{
        pC->aRow = 0;
      }
    }
    /* The following assert is true in all cases accept when
    ** the database file has been corrupted externally.
    **    assert( zRec!=0 || avail>=payloadSize || avail>=9 ); */
    szHdrSz = GetVarint((u8*)zData, offset);

    /* The KeyFetch() or DataFetch() above are fast and will get the entire
    ** record header in most cases.  But they will fail to get the complete
    ** record header if the record header does not fit on a single page
    ** in the B-Tree.  When that happens, use sqlite3VdbeMemFromBtree() to
    ** acquire the complete header text.
    */
    if( !zRec && avail<offset ){
      rc = sqlite3VdbeMemFromBtree(pCrsr, 0, offset, pC->isIndex, &sMem);
      if( rc!=SQLITE_OK ){
        goto op_column_out;
      }
      zData = sMem.z;
    }
    zEndHdr = (u8 *)&zData[offset];
    zIdx = (u8 *)&zData[szHdrSz];

    /* Scan the header and use it to fill in the aType[] and aOffset[]
    ** arrays.  aType[i] will contain the type integer for the i-th
    ** column and aOffset[i] will contain the offset from the beginning
    ** of the record to the start of the data for the i-th column
    */
    for(i=0; i<nField; i++){
      if( zIdx<zEndHdr ){
        aOffset[i] = offset;
        zIdx += GetVarint(zIdx, aType[i]);
        offset += sqlite3VdbeSerialTypeLen(aType[i]);
      }else{
        /* If i is less that nField, then there are less fields in this
        ** record than SetNumColumns indicated there are columns in the
        ** table. Set the offset for any extra columns not present in
        ** the record to 0. This tells code below to push a NULL onto the
        ** stack instead of deserializing a value from the record.
        */
        aOffset[i] = 0;
      }
    }
    Release(&sMem);
    sMem.flags = MEM_Null;

    /* If we have read more header data than was contained in the header,
    ** or if the end of the last field appears to be past the end of the
    ** record, then we must be dealing with a corrupt database.
    */
    if( zIdx>zEndHdr || offset>payloadSize ){
      rc = SQLITE_CORRUPT_BKPT;
      goto op_column_out;
    }
  }

  /* Get the column information. If aOffset[p2] is non-zero, then 
  ** deserialize the value from the record. If aOffset[p2] is zero,
  ** then there are not enough fields in the record to satisfy the
  ** request.  In this case, set the value NULL or to P3 if P3 is
  ** a pointer to a Mem object.
  */
  if( aOffset[p2] ){
    assert( rc==SQLITE_OK );
    if( zRec ){
      zData = &zRec[aOffset[p2]];
    }else{
      len = sqlite3VdbeSerialTypeLen(aType[p2]);
      rc = sqlite3VdbeMemFromBtree(pCrsr, aOffset[p2], len, pC->isIndex,&sMem);
      if( rc!=SQLITE_OK ){
        goto op_column_out;
      }
      zData = sMem.z;
    }
    sqlite3VdbeSerialGet((u8*)zData, aType[p2], pTos);
    pTos->enc = encoding;
  }else{
    if( pOp->p3type==P3_MEM ){
      sqlite3VdbeMemShallowCopy(pTos, (Mem *)(pOp->p3), MEM_Static);
    }else{
      pTos->flags = MEM_Null;
    }
  }

  /* If we dynamically allocated space to hold the data (in the
  ** sqlite3VdbeMemFromBtree() call above) then transfer control of that
  ** dynamically allocated space over to the pTos structure.
  ** This prevents a memory copy.
  */
  if( (sMem.flags & MEM_Dyn)!=0 ){
    assert( pTos->flags & MEM_Ephem );
    assert( pTos->flags & (MEM_Str|MEM_Blob) );
    assert( pTos->z==sMem.z );
    assert( sMem.flags & MEM_Term );
    pTos->flags &= ~MEM_Ephem;
    pTos->flags |= MEM_Dyn|MEM_Term;
  }

  /* pTos->z might be pointing to sMem.zShort[].  Fix that so that we
  ** can abandon sMem */
  rc = sqlite3VdbeMemMakeWriteable(pTos);

op_column_out:
  break;
}

/* Opcode: MakeRecord P1 P2 P3
**
** Convert the top abs(P1) entries of the stack into a single entry
** suitable for use as a data record in a database table or as a key
** in an index.  The details of the format are irrelavant as long as
** the OP_Column opcode can decode the record later and as long as the
** sqlite3VdbeRecordCompare function will correctly compare two encoded
** records.  Refer to source code comments for the details of the record
** format.
**
** The original stack entries are popped from the stack if P1>0 but
** remain on the stack if P1<0.
**
** If P2 is not zero and one or more of the entries are NULL, then jump
** to the address given by P2.  This feature can be used to skip a
** uniqueness test on indices.
**
** P3 may be a string that is P1 characters long.  The nth character of the
** string indicates the column affinity that should be used for the nth
** field of the index key (i.e. the first character of P3 corresponds to the
** lowest element on the stack).
**
** The mapping from character to affinity is given by the SQLITE_AFF_
** macros defined in sqliteInt.h.
**
** If P3 is NULL then all index fields have the affinity NONE.
**
** See also OP_MakeIdxRec
*/
/* Opcode: MakeIdxRec P1 P2 P3
**
** This opcode works just OP_MakeRecord except that it reads an extra
** integer from the stack (thus reading a total of abs(P1+1) entries)
** and appends that extra integer to the end of the record as a varint.
** This results in an index key.
*/
case OP_MakeIdxRec:
case OP_MakeRecord: {
  /* Assuming the record contains N fields, the record format looks
  ** like this:
  **
  ** ------------------------------------------------------------------------
  ** | hdr-size | type 0 | type 1 | ... | type N-1 | data0 | ... | data N-1 | 
  ** ------------------------------------------------------------------------
  **
  ** Data(0) is taken from the lowest element of the stack and data(N-1) is
  ** the top of the stack.
  **
  ** Each type field is a varint representing the serial type of the 
  ** corresponding data element (see sqlite3VdbeSerialType()). The
  ** hdr-size field is also a varint which is the offset from the beginning
  ** of the record to data0.
  */
  unsigned char *zNewRecord;
  unsigned char *zCsr;
  Mem *pRec;
  Mem *pRowid = 0;
  int nData = 0;         /* Number of bytes of data space */
  int nHdr = 0;          /* Number of bytes of header space */
  int nByte = 0;         /* Space required for this record */
  int nVarint;           /* Number of bytes in a varint */
  u32 serial_type;       /* Type field */
  int containsNull = 0;  /* True if any of the data fields are NULL */
  char zTemp[NBFS];      /* Space to hold small records */
  Mem *pData0;

  int leaveOnStack;      /* If true, leave the entries on the stack */
  int nField;            /* Number of fields in the record */
  int jumpIfNull;        /* Jump here if non-zero and any entries are NULL. */
  int addRowid;          /* True to append a rowid column at the end */
  char *zAffinity;       /* The affinity string for the record */
  int file_format;       /* File format to use for encoding */

  leaveOnStack = ((pOp->p1<0)?1:0);
  nField = pOp->p1 * (leaveOnStack?-1:1);
  jumpIfNull = pOp->p2;
  addRowid = pOp->opcode==OP_MakeIdxRec;
  zAffinity = pOp->p3;

  pData0 = &pTos[1-nField];
  assert( pData0>=p->aStack );
  containsNull = 0;
  file_format = p->minWriteFileFormat;

  /* Loop through the elements that will make up the record to figure
  ** out how much space is required for the new record.
  */
  for(pRec=pData0; pRec<=pTos; pRec++){
    if( zAffinity ){
      applyAffinity(pRec, zAffinity[pRec-pData0], encoding);
    }
    if( pRec->flags&MEM_Null ){
      containsNull = 1;
    }
    serial_type = sqlite3VdbeSerialType(pRec, file_format);
    nData += sqlite3VdbeSerialTypeLen(serial_type);
    nHdr += sqlite3VarintLen(serial_type);
  }

  /* If we have to append a varint rowid to this record, set 'rowid'
  ** to the value of the rowid and increase nByte by the amount of space
  ** required to store it and the 0x00 seperator byte.
  */
  if( addRowid ){
    pRowid = &pTos[0-nField];
    assert( pRowid>=p->aStack );
    sqlite3VdbeMemIntegerify(pRowid);
    serial_type = sqlite3VdbeSerialType(pRowid, 0);
    nData += sqlite3VdbeSerialTypeLen(serial_type);
    nHdr += sqlite3VarintLen(serial_type);
  }

  /* Add the initial header varint and total the size */
  nHdr += nVarint = sqlite3VarintLen(nHdr);
  if( nVarint<sqlite3VarintLen(nHdr) ){
    nHdr++;
  }
  nByte = nHdr+nData;

  /* Allocate space for the new record. */
  if( nByte>sizeof(zTemp) ){
    zNewRecord = sqliteMallocRaw(nByte);
    if( !zNewRecord ){
      goto no_mem;
    }
  }else{
    zNewRecord = (u8*)zTemp;
  }

  /* Write the record */
  zCsr = zNewRecord;
  zCsr += sqlite3PutVarint(zCsr, nHdr);
  for(pRec=pData0; pRec<=pTos; pRec++){
    serial_type = sqlite3VdbeSerialType(pRec, file_format);
    zCsr += sqlite3PutVarint(zCsr, serial_type);      /* serial type */
  }
  if( addRowid ){
    zCsr += sqlite3PutVarint(zCsr, sqlite3VdbeSerialType(pRowid, 0));
  }
  for(pRec=pData0; pRec<=pTos; pRec++){
    zCsr += sqlite3VdbeSerialPut(zCsr, pRec, file_format);  /* serial data */
  }
  if( addRowid ){
    zCsr += sqlite3VdbeSerialPut(zCsr, pRowid, 0);
  }
  assert( zCsr==(zNewRecord+nByte) );

  /* Pop entries off the stack if required. Push the new record on. */
  if( !leaveOnStack ){
    popStack(&pTos, nField+addRowid);
  }
  pTos++;
  pTos->n = nByte;
  if( nByte<=sizeof(zTemp) ){
    assert( zNewRecord==(unsigned char *)zTemp );
    pTos->z = pTos->zShort;
    memcpy(pTos->zShort, zTemp, nByte);
    pTos->flags = MEM_Blob | MEM_Short;
  }else{
    assert( zNewRecord!=(unsigned char *)zTemp );
    pTos->z = (char*)zNewRecord;
    pTos->flags = MEM_Blob | MEM_Dyn;
    pTos->xDel = 0;
  }
  pTos->enc = SQLITE_UTF8;  /* In case the blob is ever converted to text */

  /* If a NULL was encountered and jumpIfNull is non-zero, take the jump. */
  if( jumpIfNull && containsNull ){
    pc = jumpIfNull - 1;
  }
  break;
}

/* Opcode: Statement P1 * *
**
** Begin an individual statement transaction which is part of a larger
** BEGIN..COMMIT transaction.  This is needed so that the statement
** can be rolled back after an error without having to roll back the
** entire transaction.  The statement transaction will automatically
** commit when the VDBE halts.
**
** The statement is begun on the database file with index P1.  The main
** database file has an index of 0 and the file used for temporary tables
** has an index of 1.
*/
case OP_Statement: {       /* no-push */
  int i = pOp->p1;
  Btree *pBt;
  if( i>=0 && i<db->nDb && (pBt = db->aDb[i].pBt)!=0 && !(db->autoCommit) ){
    assert( sqlite3BtreeIsInTrans(pBt) );
    if( !sqlite3BtreeIsInStmt(pBt) ){
      rc = sqlite3BtreeBeginStmt(pBt);
    }
  }
  break;
}

/* Opcode: AutoCommit P1 P2 *
**
** Set the database auto-commit flag to P1 (1 or 0). If P2 is true, roll
** back any currently active btree transactions. If there are any active
** VMs (apart from this one), then the COMMIT or ROLLBACK statement fails.
**
** This instruction causes the VM to halt.
*/
case OP_AutoCommit: {       /* no-push */
  u8 i = pOp->p1;
  u8 rollback = pOp->p2;

  assert( i==1 || i==0 );
  assert( i==1 || rollback==0 );

  assert( db->activeVdbeCnt>0 );  /* At least this one VM is active */

  if( db->activeVdbeCnt>1 && i && !db->autoCommit ){
    /* If this instruction implements a COMMIT or ROLLBACK, other VMs are
    ** still running, and a transaction is active, return an error indicating
    ** that the other VMs must complete first. 
    */
    sqlite3SetString(&p->zErrMsg, "cannot ", rollback?"rollback":"commit", 
        " transaction - SQL statements in progress", (char*)0);
    rc = SQLITE_ERROR;
  }else if( i!=db->autoCommit ){
    if( pOp->p2 ){
      assert( i==1 );
      sqlite3RollbackAll(db);
      db->autoCommit = 1;
    }else{
      db->autoCommit = i;
      if( sqlite3VdbeHalt(p)==SQLITE_BUSY ){
        p->pTos = pTos;
        p->pc = pc;
        db->autoCommit = 1-i;
        p->rc = SQLITE_BUSY;
        return SQLITE_BUSY;
      }
    }
    if( p->rc==SQLITE_OK ){
      return SQLITE_DONE;
    }else{
      return SQLITE_ERROR;
    }
  }else{
    sqlite3SetString(&p->zErrMsg,
        (!i)?"cannot start a transaction within a transaction":(
        (rollback)?"cannot rollback - no transaction is active":
                   "cannot commit - no transaction is active"), (char*)0);
         
    rc = SQLITE_ERROR;
  }
  break;
}

/* Opcode: Transaction P1 P2 *
**
** Begin a transaction.  The transaction ends when a Commit or Rollback
** opcode is encountered.  Depending on the ON CONFLICT setting, the
** transaction might also be rolled back if an error is encountered.
**
** P1 is the index of the database file on which the transaction is
** started.  Index 0 is the main database file and index 1 is the
** file used for temporary tables.
**
** If P2 is non-zero, then a write-transaction is started.  A RESERVED lock is
** obtained on the database file when a write-transaction is started.  No
** other process can start another write transaction while this transaction is
** underway.  Starting a write transaction also creates a rollback journal. A
** write transaction must be started before any changes can be made to the
** database.  If P2 is 2 or greater then an EXCLUSIVE lock is also obtained
** on the file.
**
** If P2 is zero, then a read-lock is obtained on the database file.
*/
case OP_Transaction: {       /* no-push */
  int i = pOp->p1;
  Btree *pBt;

  assert( i>=0 && i<db->nDb );
  pBt = db->aDb[i].pBt;

  if( pBt ){
    rc = sqlite3BtreeBeginTrans(pBt, pOp->p2);
    if( rc==SQLITE_BUSY ){
      p->pc = pc;
      p->rc = SQLITE_BUSY;
      p->pTos = pTos;
      return SQLITE_BUSY;
    }
    if( rc!=SQLITE_OK && rc!=SQLITE_READONLY /* && rc!=SQLITE_BUSY */ ){
      goto abort_due_to_error;
    }
  }
  break;
}

/* Opcode: ReadCookie P1 P2 *
**
** Read cookie number P2 from database P1 and push it onto the stack.
** P2==0 is the schema version.  P2==1 is the database format.
** P2==2 is the recommended pager cache size, and so forth.  P1==0 is
** the main database file and P1==1 is the database file used to store
** temporary tables.
**
** There must be a read-lock on the database (either a transaction
** must be started or there must be an open cursor) before
** executing this instruction.
*/
case OP_ReadCookie: {
  int iMeta;
  assert( pOp->p2<SQLITE_N_BTREE_META );
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  assert( db->aDb[pOp->p1].pBt!=0 );
  /* The indexing of meta values at the schema layer is off by one from
  ** the indexing in the btree layer.  The btree considers meta[0] to
  ** be the number of free pages in the database (a read-only value)
  ** and meta[1] to be the schema cookie.  The schema layer considers
  ** meta[1] to be the schema cookie.  So we have to shift the index
  ** by one in the following statement.
  */
  rc = sqlite3BtreeGetMeta(db->aDb[pOp->p1].pBt, 1 + pOp->p2, (u32 *)&iMeta);
  pTos++;
  pTos->u.i = iMeta;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: SetCookie P1 P2 *
**
** Write the top of the stack into cookie number P2 of database P1.
** P2==0 is the schema version.  P2==1 is the database format.
** P2==2 is the recommended pager cache size, and so forth.  P1==0 is
** the main database file and P1==1 is the database file used to store
** temporary tables.
**
** A transaction must be started before executing this opcode.
*/
case OP_SetCookie: {       /* no-push */
  Db *pDb;
  assert( pOp->p2<SQLITE_N_BTREE_META );
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  pDb = &db->aDb[pOp->p1];
  assert( pDb->pBt!=0 );
  assert( pTos>=p->aStack );
  sqlite3VdbeMemIntegerify(pTos);
  /* See note about index shifting on OP_ReadCookie */
  rc = sqlite3BtreeUpdateMeta(pDb->pBt, 1+pOp->p2, (int)pTos->u.i);
  if( pOp->p2==0 ){
    /* When the schema cookie changes, record the new cookie internally */
    pDb->pSchema->schema_cookie = pTos->u.i;
    db->flags |= SQLITE_InternChanges;
  }else if( pOp->p2==1 ){
    /* Record changes in the file format */
    pDb->pSchema->file_format = pTos->u.i;
  }
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos--;
  if( pOp->p1==1 ){
    /* Invalidate all prepared statements whenever the TEMP database
    ** schema is changed.  Ticket #1644 */
    sqlite3ExpirePreparedStatements(db);
  }
  break;
}

/* Opcode: VerifyCookie P1 P2 *
**
** Check the value of global database parameter number 0 (the
** schema version) and make sure it is equal to P2.  
** P1 is the database number which is 0 for the main database file
** and 1 for the file holding temporary tables and some higher number
** for auxiliary databases.
**
** The cookie changes its value whenever the database schema changes.
** This operation is used to detect when that the cookie has changed
** and that the current process needs to reread the schema.
**
** Either a transaction needs to have been started or an OP_Open needs
** to be executed (to establish a read lock) before this opcode is
** invoked.
*/
case OP_VerifyCookie: {       /* no-push */
  int iMeta;
  Btree *pBt;
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  pBt = db->aDb[pOp->p1].pBt;
  if( pBt ){
    rc = sqlite3BtreeGetMeta(pBt, 1, (u32 *)&iMeta);
  }else{
    rc = SQLITE_OK;
    iMeta = 0;
  }
  if( rc==SQLITE_OK && iMeta!=pOp->p2 ){
    sqlite3SetString(&p->zErrMsg, "database schema has changed", (char*)0);
    /* If the schema-cookie from the database file matches the cookie 
    ** stored with the in-memory representation of the schema, do
    ** not reload the schema from the database file.
    **
    ** If virtual-tables are in use, this is not just an optimisation.
    ** Often, v-tables store their data in other SQLite tables, which
    ** are queried from within xNext() and other v-table methods using
    ** prepared queries. If such a query is out-of-date, we do not want to
    ** discard the database schema, as the user code implementing the
    ** v-table would have to be ready for the sqlite3_vtab structure itself
    ** to be invalidated whenever sqlite3_step() is called from within 
    ** a v-table method.
    */
    if( db->aDb[pOp->p1].pSchema->schema_cookie!=iMeta ){
      sqlite3ResetInternalSchema(db, pOp->p1);
    }

    sqlite3ExpirePreparedStatements(db);
    rc = SQLITE_SCHEMA;
  }
  break;
}

/* Opcode: OpenRead P1 P2 P3
**
** Open a read-only cursor for the database table whose root page is
** P2 in a database file.  The database file is determined by an 
** integer from the top of the stack.  0 means the main database and
** 1 means the database used for temporary tables.  Give the new 
** cursor an identifier of P1.  The P1 values need not be contiguous
** but all P1 values should be small integers.  It is an error for
** P1 to be negative.
**
** If P2==0 then take the root page number from the next of the stack.
**
** There will be a read lock on the database whenever there is an
** open cursor.  If the database was unlocked prior to this instruction
** then a read lock is acquired as part of this instruction.  A read
** lock allows other processes to read the database but prohibits
** any other process from modifying the database.  The read lock is
** released when all cursors are closed.  If this instruction attempts
** to get a read lock but fails, the script terminates with an
** SQLITE_BUSY error code.
**
** The P3 value is a pointer to a KeyInfo structure that defines the
** content and collating sequence of indices.  P3 is NULL for cursors
** that are not pointing to indices.
**
** See also OpenWrite.
*/
/* Opcode: OpenWrite P1 P2 P3
**
** Open a read/write cursor named P1 on the table or index whose root
** page is P2.  If P2==0 then take the root page number from the stack.
**
** The P3 value is a pointer to a KeyInfo structure that defines the
** content and collating sequence of indices.  P3 is NULL for cursors
** that are not pointing to indices.
**
** This instruction works just like OpenRead except that it opens the cursor
** in read/write mode.  For a given table, there can be one or more read-only
** cursors or a single read/write cursor but not both.
**
** See also OpenRead.
*/
case OP_OpenRead:          /* no-push */
case OP_OpenWrite: {       /* no-push */
  int i = pOp->p1;
  int p2 = pOp->p2;
  int wrFlag;
  Btree *pX;
  int iDb;
  Cursor *pCur;
  Db *pDb;
  
  assert( pTos>=p->aStack );
  sqlite3VdbeMemIntegerify(pTos);
  iDb = pTos->u.i;
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos--;
  assert( iDb>=0 && iDb<db->nDb );
  pDb = &db->aDb[iDb];
  pX = pDb->pBt;
  assert( pX!=0 );
  if( pOp->opcode==OP_OpenWrite ){
    wrFlag = 1;
    if( pDb->pSchema->file_format < p->minWriteFileFormat ){
      p->minWriteFileFormat = pDb->pSchema->file_format;
    }
  }else{
    wrFlag = 0;
  }
  if( p2<=0 ){
    assert( pTos>=p->aStack );
    sqlite3VdbeMemIntegerify(pTos);
    p2 = pTos->u.i;
    assert( (pTos->flags & MEM_Dyn)==0 );
    pTos--;
    assert( p2>=2 );
  }
  assert( i>=0 );
  pCur = allocateCursor(p, i, iDb);
  if( pCur==0 ) goto no_mem;
  pCur->nullRow = 1;
  if( pX==0 ) break;
  /* We always provide a key comparison function.  If the table being
  ** opened is of type INTKEY, the comparision function will be ignored. */
  rc = sqlite3BtreeCursor(pX, p2, wrFlag,
           sqlite3VdbeRecordCompare, pOp->p3,
           &pCur->pCursor);
  if( pOp->p3type==P3_KEYINFO ){
    pCur->pKeyInfo = (KeyInfo*)pOp->p3;
    pCur->pIncrKey = &pCur->pKeyInfo->incrKey;
    pCur->pKeyInfo->enc = ENC(p->db);
  }else{
    pCur->pKeyInfo = 0;
    pCur->pIncrKey = &pCur->bogusIncrKey;
  }
  switch( rc ){
    case SQLITE_BUSY: {
      p->pc = pc;
      p->rc = SQLITE_BUSY;
      p->pTos = &pTos[1 + (pOp->p2<=0)]; /* Operands must remain on stack */
      return SQLITE_BUSY;
    }
    case SQLITE_OK: {
      int flags = sqlite3BtreeFlags(pCur->pCursor);
      /* Sanity checking.  Only the lower four bits of the flags byte should
      ** be used.  Bit 3 (mask 0x08) is unpreditable.  The lower 3 bits
      ** (mask 0x07) should be either 5 (intkey+leafdata for tables) or
      ** 2 (zerodata for indices).  If these conditions are not met it can
      ** only mean that we are dealing with a corrupt database file
      */
      if( (flags & 0xf0)!=0 || ((flags & 0x07)!=5 && (flags & 0x07)!=2) ){
        rc = SQLITE_CORRUPT_BKPT;
        goto abort_due_to_error;
      }
      pCur->isTable = (flags & BTREE_INTKEY)!=0;
      pCur->isIndex = (flags & BTREE_ZERODATA)!=0;
      /* If P3==0 it means we are expected to open a table.  If P3!=0 then
      ** we expect to be opening an index.  If this is not what happened,
      ** then the database is corrupt
      */
      if( (pCur->isTable && pOp->p3type==P3_KEYINFO)
       || (pCur->isIndex && pOp->p3type!=P3_KEYINFO) ){
        rc = SQLITE_CORRUPT_BKPT;
        goto abort_due_to_error;
      }
      break;
    }
    case SQLITE_EMPTY: {
      pCur->isTable = pOp->p3type!=P3_KEYINFO;
      pCur->isIndex = !pCur->isTable;
      rc = SQLITE_OK;
      break;
    }
    default: {
      goto abort_due_to_error;
    }
  }
  break;
}

/* Opcode: OpenEphemeral P1 P2 P3
**
** Open a new cursor P1 to a transient table.
** The cursor is always opened read/write even if 
** the main database is read-only.  The transient or virtual
** table is deleted automatically when the cursor is closed.
**
** P2 is the number of columns in the virtual table.
** The cursor points to a BTree table if P3==0 and to a BTree index
** if P3 is not 0.  If P3 is not NULL, it points to a KeyInfo structure
** that defines the format of keys in the index.
**
** This opcode was once called OpenTemp.  But that created
** confusion because the term "temp table", might refer either
** to a TEMP table at the SQL level, or to a table opened by
** this opcode.  Then this opcode was call OpenVirtual.  But
** that created confusion with the whole virtual-table idea.
*/
case OP_OpenEphemeral: {       /* no-push */
  int i = pOp->p1;
  Cursor *pCx;
  assert( i>=0 );
  pCx = allocateCursor(p, i, -1);
  if( pCx==0 ) goto no_mem;
  pCx->nullRow = 1;
  rc = sqlite3BtreeFactory(db, 0, 1, TEMP_PAGES, &pCx->pBt);
  if( rc==SQLITE_OK ){
    rc = sqlite3BtreeBeginTrans(pCx->pBt, 1);
  }
  if( rc==SQLITE_OK ){
    /* If a transient index is required, create it by calling
    ** sqlite3BtreeCreateTable() with the BTREE_ZERODATA flag before
    ** opening it. If a transient table is required, just use the
    ** automatically created table with root-page 1 (an INTKEY table).
    */
    if( pOp->p3 ){
      int pgno;
      assert( pOp->p3type==P3_KEYINFO );
      rc = sqlite3BtreeCreateTable(pCx->pBt, &pgno, BTREE_ZERODATA); 
      if( rc==SQLITE_OK ){
        assert( pgno==MASTER_ROOT+1 );
        rc = sqlite3BtreeCursor(pCx->pBt, pgno, 1, sqlite3VdbeRecordCompare,
            pOp->p3, &pCx->pCursor);
        pCx->pKeyInfo = (KeyInfo*)pOp->p3;
        pCx->pKeyInfo->enc = ENC(p->db);
        pCx->pIncrKey = &pCx->pKeyInfo->incrKey;
      }
      pCx->isTable = 0;
    }else{
      rc = sqlite3BtreeCursor(pCx->pBt, MASTER_ROOT, 1, 0, 0, &pCx->pCursor);
      pCx->isTable = 1;
      pCx->pIncrKey = &pCx->bogusIncrKey;
    }
  }
  pCx->nField = pOp->p2;
  pCx->isIndex = !pCx->isTable;
  break;
}

/* Opcode: OpenPseudo P1 * *
**
** Open a new cursor that points to a fake table that contains a single
** row of data.  Any attempt to write a second row of data causes the
** first row to be deleted.  All data is deleted when the cursor is
** closed.
**
** A pseudo-table created by this opcode is useful for holding the
** NEW or OLD tables in a trigger.  Also used to hold the a single
** row output from the sorter so that the row can be decomposed into
** individual columns using the OP_Column opcode.
*/
case OP_OpenPseudo: {       /* no-push */
  int i = pOp->p1;
  Cursor *pCx;
  assert( i>=0 );
  pCx = allocateCursor(p, i, -1);
  if( pCx==0 ) goto no_mem;
  pCx->nullRow = 1;
  pCx->pseudoTable = 1;
  pCx->pIncrKey = &pCx->bogusIncrKey;
  pCx->isTable = 1;
  pCx->isIndex = 0;
  break;
}

/* Opcode: Close P1 * *
**
** Close a cursor previously opened as P1.  If P1 is not
** currently open, this instruction is a no-op.
*/
case OP_Close: {       /* no-push */
  int i = pOp->p1;
  if( i>=0 && i<p->nCursor ){
    sqlite3VdbeFreeCursor(p, p->apCsr[i]);
    p->apCsr[i] = 0;
  }
  break;
}

/* Opcode: MoveGe P1 P2 *
**
** Pop the top of the stack and use its value as a key.  Reposition
** cursor P1 so that it points to the smallest entry that is greater
** than or equal to the key that was popped ffrom the stack.
** If there are no records greater than or equal to the key and P2 
** is not zero, then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveLt, MoveGt, MoveLe
*/
/* Opcode: MoveGt P1 P2 *
**
** Pop the top of the stack and use its value as a key.  Reposition
** cursor P1 so that it points to the smallest entry that is greater
** than the key from the stack.
** If there are no records greater than the key and P2 is not zero,
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveLt, MoveGe, MoveLe
*/
/* Opcode: MoveLt P1 P2 *
**
** Pop the top of the stack and use its value as a key.  Reposition
** cursor P1 so that it points to the largest entry that is less
** than the key from the stack.
** If there are no records less than the key and P2 is not zero,
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveGt, MoveGe, MoveLe
*/
/* Opcode: MoveLe P1 P2 *
**
** Pop the top of the stack and use its value as a key.  Reposition
** cursor P1 so that it points to the largest entry that is less than
** or equal to the key that was popped from the stack.
** If there are no records less than or eqal to the key and P2 is not zero,
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveGt, MoveGe, MoveLt
*/
case OP_MoveLt:         /* no-push */
case OP_MoveLe:         /* no-push */
case OP_MoveGe:         /* no-push */
case OP_MoveGt: {       /* no-push */
  int i = pOp->p1;
  Cursor *pC;

  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  if( pC->pCursor!=0 ){
    int res, oc;
    oc = pOp->opcode;
    pC->nullRow = 0;
    *pC->pIncrKey = oc==OP_MoveGt || oc==OP_MoveLe;
    if( pC->isTable ){
      i64 iKey;
      sqlite3VdbeMemIntegerify(pTos);
      iKey = intToKey(pTos->u.i);
      if( pOp->p2==0 && pOp->opcode==OP_MoveGe ){
        pC->movetoTarget = iKey;
        pC->deferredMoveto = 1;
        assert( (pTos->flags & MEM_Dyn)==0 );
        pTos--;
        break;
      }
      rc = sqlite3BtreeMoveto(pC->pCursor, 0, (u64)iKey, 0, &res);
      if( rc!=SQLITE_OK ){
        goto abort_due_to_error;
      }
      pC->lastRowid = pTos->u.i;
      pC->rowidIsValid = res==0;
    }else{
      assert( pTos->flags & MEM_Blob );
      /* Stringify(pTos, encoding); */
      rc = sqlite3BtreeMoveto(pC->pCursor, pTos->z, pTos->n, 0, &res);
      if( rc!=SQLITE_OK ){
        goto abort_due_to_error;
      }
      pC->rowidIsValid = 0;
    }
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
    *pC->pIncrKey = 0;
#ifdef SQLITE_TEST
    sqlite3_search_count++;
#endif
    if( oc==OP_MoveGe || oc==OP_MoveGt ){
      if( res<0 ){
        rc = sqlite3BtreeNext(pC->pCursor, &res);
        if( rc!=SQLITE_OK ) goto abort_due_to_error;
        pC->rowidIsValid = 0;
      }else{
        res = 0;
      }
    }else{
      assert( oc==OP_MoveLt || oc==OP_MoveLe );
      if( res>=0 ){
        rc = sqlite3BtreePrevious(pC->pCursor, &res);
        if( rc!=SQLITE_OK ) goto abort_due_to_error;
        pC->rowidIsValid = 0;
      }else{
        /* res might be negative because the table is empty.  Check to
        ** see if this is the case.
        */
        res = sqlite3BtreeEof(pC->pCursor);
      }
    }
    if( res ){
      if( pOp->p2>0 ){
        pc = pOp->p2 - 1;
      }else{
        pC->nullRow = 1;
      }
    }
  }
  Release(pTos);
  pTos--;
  break;
}

/* Opcode: Distinct P1 P2 *
**
** Use the top of the stack as a record created using MakeRecord.  P1 is a
** cursor on a table that declared as an index.  If that table contains an
** entry that matches the top of the stack fall thru.  If the top of the stack
** matches no entry in P1 then jump to P2.
**
** The cursor is left pointing at the matching entry if it exists.  The
** record on the top of the stack is not popped.
**
** This instruction is similar to NotFound except that this operation
** does not pop the key from the stack.
**
** The instruction is used to implement the DISTINCT operator on SELECT
** statements.  The P1 table is not a true index but rather a record of
** all results that have produced so far.  
**
** See also: Found, NotFound, MoveTo, IsUnique, NotExists
*/
/* Opcode: Found P1 P2 *
**
** Top of the stack holds a blob constructed by MakeRecord.  P1 is an index.
** If an entry that matches the top of the stack exists in P1 then
** jump to P2.  If the top of the stack does not match any entry in P1
** then fall thru.  The P1 cursor is left pointing at the matching entry
** if it exists.  The blob is popped off the top of the stack.
**
** This instruction is used to implement the IN operator where the
** left-hand side is a SELECT statement.  P1 is not a true index but
** is instead a temporary index that holds the results of the SELECT
** statement.  This instruction just checks to see if the left-hand side
** of the IN operator (stored on the top of the stack) exists in the
** result of the SELECT statement.
**
** See also: Distinct, NotFound, MoveTo, IsUnique, NotExists
*/
/* Opcode: NotFound P1 P2 *
**
** The top of the stack holds a blob constructed by MakeRecord.  P1 is
** an index.  If no entry exists in P1 that matches the blob then jump
** to P2.  If an entry does existing, fall through.  The cursor is left
** pointing to the entry that matches.  The blob is popped from the stack.
**
** The difference between this operation and Distinct is that
** Distinct does not pop the key from the stack.
**
** See also: Distinct, Found, MoveTo, NotExists, IsUnique
*/
case OP_Distinct:       /* no-push */
case OP_NotFound:       /* no-push */
case OP_Found: {        /* no-push */
  int i = pOp->p1;
  int alreadyExists = 0;
  Cursor *pC;
  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pC = p->apCsr[i])->pCursor!=0 ){
    int res, rx;
    assert( pC->isTable==0 );
    Stringify(pTos, encoding);
    rx = sqlite3BtreeMoveto(pC->pCursor, pTos->z, pTos->n, 0, &res);
    alreadyExists = rx==SQLITE_OK && res==0;
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
  }
  if( pOp->opcode==OP_Found ){
    if( alreadyExists ) pc = pOp->p2 - 1;
  }else{
    if( !alreadyExists ) pc = pOp->p2 - 1;
  }
  if( pOp->opcode!=OP_Distinct ){
    Release(pTos);
    pTos--;
  }
  break;
}

/* Opcode: IsUnique P1 P2 *
**
** The top of the stack is an integer record number.  Call this
** record number R.  The next on the stack is an index key created
** using MakeIdxRec.  Call it K.  This instruction pops R from the
** stack but it leaves K unchanged.
**
** P1 is an index.  So it has no data and its key consists of a
** record generated by OP_MakeRecord where the last field is the 
** rowid of the entry that the index refers to.
** 
** This instruction asks if there is an entry in P1 where the
** fields matches K but the rowid is different from R.
** If there is no such entry, then there is an immediate
** jump to P2.  If any entry does exist where the index string
** matches K but the record number is not R, then the record
** number for that entry is pushed onto the stack and control
** falls through to the next instruction.
**
** See also: Distinct, NotFound, NotExists, Found
*/
case OP_IsUnique: {        /* no-push */
  int i = pOp->p1;
  Mem *pNos = &pTos[-1];
  Cursor *pCx;
  BtCursor *pCrsr;
  i64 R;

  /* Pop the value R off the top of the stack
  */
  assert( pNos>=p->aStack );
  sqlite3VdbeMemIntegerify(pTos);
  R = pTos->u.i;
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos--;
  assert( i>=0 && i<p->nCursor );
  pCx = p->apCsr[i];
  assert( pCx!=0 );
  pCrsr = pCx->pCursor;
  if( pCrsr!=0 ){
    int res;
    i64 v;         /* The record number on the P1 entry that matches K */
    char *zKey;    /* The value of K */
    int nKey;      /* Number of bytes in K */
    int len;       /* Number of bytes in K without the rowid at the end */
    int szRowid;   /* Size of the rowid column at the end of zKey */

    /* Make sure K is a string and make zKey point to K
    */
    Stringify(pNos, encoding);
    zKey = pNos->z;
    nKey = pNos->n;

    szRowid = sqlite3VdbeIdxRowidLen((u8*)zKey);
    len = nKey-szRowid;

    /* Search for an entry in P1 where all but the last four bytes match K.
    ** If there is no such entry, jump immediately to P2.
    */
    assert( pCx->deferredMoveto==0 );
    pCx->cacheStatus = CACHE_STALE;
    rc = sqlite3BtreeMoveto(pCrsr, zKey, len, 0, &res);
    if( rc!=SQLITE_OK ){
      goto abort_due_to_error;
    }
    if( res<0 ){
      rc = sqlite3BtreeNext(pCrsr, &res);
      if( res ){
        pc = pOp->p2 - 1;
        break;
      }
    }
    rc = sqlite3VdbeIdxKeyCompare(pCx, len, (u8*)zKey, &res); 
    if( rc!=SQLITE_OK ) goto abort_due_to_error;
    if( res>0 ){
      pc = pOp->p2 - 1;
      break;
    }

    /* At this point, pCrsr is pointing to an entry in P1 where all but
    ** the final entry (the rowid) matches K.  Check to see if the
    ** final rowid column is different from R.  If it equals R then jump
    ** immediately to P2.
    */
    rc = sqlite3VdbeIdxRowid(pCrsr, &v);
    if( rc!=SQLITE_OK ){
      goto abort_due_to_error;
    }
    if( v==R ){
      pc = pOp->p2 - 1;
      break;
    }

    /* The final varint of the key is different from R.  Push it onto
    ** the stack.  (The record number of an entry that violates a UNIQUE
    ** constraint.)
    */
    pTos++;
    pTos->u.i = v;
    pTos->flags = MEM_Int;
  }
  break;
}

/* Opcode: NotExists P1 P2 *
**
** Use the top of the stack as a integer key.  If a record with that key
** does not exist in table of P1, then jump to P2.  If the record
** does exist, then fall thru.  The cursor is left pointing to the
** record if it exists.  The integer key is popped from the stack.
**
** The difference between this operation and NotFound is that this
** operation assumes the key is an integer and that P1 is a table whereas
** NotFound assumes key is a blob constructed from MakeRecord and
** P1 is an index.
**
** See also: Distinct, Found, MoveTo, NotFound, IsUnique
*/
case OP_NotExists: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    int res;
    u64 iKey;
    assert( pTos->flags & MEM_Int );
    assert( p->apCsr[i]->isTable );
    iKey = intToKey(pTos->u.i);
    rc = sqlite3BtreeMoveto(pCrsr, 0, iKey, 0,&res);
    pC->lastRowid = pTos->u.i;
    pC->rowidIsValid = res==0;
    pC->nullRow = 0;
    pC->cacheStatus = CACHE_STALE;
    /* res might be uninitialized if rc!=SQLITE_OK.  But if rc!=SQLITE_OK
    ** processing is about to abort so we really do not care whether or not
    ** the following jump is taken.  (In other words, do not stress over
    ** the error that valgrind sometimes shows on the next statement when
    ** running ioerr.test and similar failure-recovery test scripts.) */
    if( res!=0 ){
      pc = pOp->p2 - 1;
      pC->rowidIsValid = 0;
    }
  }
  Release(pTos);
  pTos--;
  break;
}

/* Opcode: Sequence P1 * *
**
** Push an integer onto the stack which is the next available
** sequence number for cursor P1.  The sequence number on the
** cursor is incremented after the push.
*/
case OP_Sequence: {
  int i = pOp->p1;
  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  pTos++;
  pTos->u.i = p->apCsr[i]->seqCount++;
  pTos->flags = MEM_Int;
  break;
}


/* Opcode: NewRowid P1 P2 *
**
** Get a new integer record number (a.k.a "rowid") used as the key to a table.
** The record number is not previously used as a key in the database
** table that cursor P1 points to.  The new record number is pushed 
** onto the stack.
**
** If P2>0 then P2 is a memory cell that holds the largest previously
** generated record number.  No new record numbers are allowed to be less
** than this value.  When this value reaches its maximum, a SQLITE_FULL
** error is generated.  The P2 memory cell is updated with the generated
** record number.  This P2 mechanism is used to help implement the
** AUTOINCREMENT feature.
*/
case OP_NewRowid: {
  int i = pOp->p1;
  i64 v = 0;
  Cursor *pC;
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pC = p->apCsr[i])->pCursor==0 ){
    /* The zero initialization above is all that is needed */
  }else{
    /* The next rowid or record number (different terms for the same
    ** thing) is obtained in a two-step algorithm.
    **
    ** First we attempt to find the largest existing rowid and add one
    ** to that.  But if the largest existing rowid is already the maximum
    ** positive integer, we have to fall through to the second
    ** probabilistic algorithm
    **
    ** The second algorithm is to select a rowid at random and see if
    ** it already exists in the table.  If it does not exist, we have
    ** succeeded.  If the random rowid does exist, we select a new one
    ** and try again, up to 1000 times.
    **
    ** For a table with less than 2 billion entries, the probability
    ** of not finding a unused rowid is about 1.0e-300.  This is a 
    ** non-zero probability, but it is still vanishingly small and should
    ** never cause a problem.  You are much, much more likely to have a
    ** hardware failure than for this algorithm to fail.
    **
    ** The analysis in the previous paragraph assumes that you have a good
    ** source of random numbers.  Is a library function like lrand48()
    ** good enough?  Maybe. Maybe not. It's hard to know whether there
    ** might be subtle bugs is some implementations of lrand48() that
    ** could cause problems. To avoid uncertainty, SQLite uses its own 
    ** random number generator based on the RC4 algorithm.
    **
    ** To promote locality of reference for repetitive inserts, the
    ** first few attempts at chosing a random rowid pick values just a little
    ** larger than the previous rowid.  This has been shown experimentally
    ** to double the speed of the COPY operation.
    */
    int res, rx=SQLITE_OK, cnt;
    i64 x;
    cnt = 0;
    if( (sqlite3BtreeFlags(pC->pCursor)&(BTREE_INTKEY|BTREE_ZERODATA)) !=
          BTREE_INTKEY ){
      rc = SQLITE_CORRUPT_BKPT;
      goto abort_due_to_error;
    }
    assert( (sqlite3BtreeFlags(pC->pCursor) & BTREE_INTKEY)!=0 );
    assert( (sqlite3BtreeFlags(pC->pCursor) & BTREE_ZERODATA)==0 );

#ifdef SQLITE_32BIT_ROWID
#   define MAX_ROWID 0x7fffffff
#else
    /* Some compilers complain about constants of the form 0x7fffffffffffffff.
    ** Others complain about 0x7ffffffffffffffffLL.  The following macro seems
    ** to provide the constant while making all compilers happy.
    */
#   define MAX_ROWID  ( (((u64)0x7fffffff)<<32) | (u64)0xffffffff )
#endif

    if( !pC->useRandomRowid ){
      if( pC->nextRowidValid ){
        v = pC->nextRowid;
      }else{
        rc = sqlite3BtreeLast(pC->pCursor, &res);
        if( rc!=SQLITE_OK ){
          goto abort_due_to_error;
        }
        if( res ){
          v = 1;
        }else{
          sqlite3BtreeKeySize(pC->pCursor, &v);
          v = keyToInt(v);
          if( v==MAX_ROWID ){
            pC->useRandomRowid = 1;
          }else{
            v++;
          }
        }
      }

#ifndef SQLITE_OMIT_AUTOINCREMENT
      if( pOp->p2 ){
        Mem *pMem;
        assert( pOp->p2>0 && pOp->p2<p->nMem );  /* P2 is a valid memory cell */
        pMem = &p->aMem[pOp->p2];
        sqlite3VdbeMemIntegerify(pMem);
        assert( (pMem->flags & MEM_Int)!=0 );  /* mem(P2) holds an integer */
        if( pMem->u.i==MAX_ROWID || pC->useRandomRowid ){
          rc = SQLITE_FULL;
          goto abort_due_to_error;
        }
        if( v<pMem->u.i+1 ){
          v = pMem->u.i + 1;
        }
        pMem->u.i = v;
      }
#endif

      if( v<MAX_ROWID ){
        pC->nextRowidValid = 1;
        pC->nextRowid = v+1;
      }else{
        pC->nextRowidValid = 0;
      }
    }
    if( pC->useRandomRowid ){
      assert( pOp->p2==0 );  /* SQLITE_FULL must have occurred prior to this */
      v = db->priorNewRowid;
      cnt = 0;
      do{
        if( v==0 || cnt>2 ){
          sqlite3Randomness(sizeof(v), &v);
          if( cnt<5 ) v &= 0xffffff;
        }else{
          unsigned char r;
          sqlite3Randomness(1, &r);
          v += r + 1;
        }
        if( v==0 ) continue;
        x = intToKey(v);
        rx = sqlite3BtreeMoveto(pC->pCursor, 0, (u64)x, 0, &res);
        cnt++;
      }while( cnt<1000 && rx==SQLITE_OK && res==0 );
      db->priorNewRowid = v;
      if( rx==SQLITE_OK && res==0 ){
        rc = SQLITE_FULL;
        goto abort_due_to_error;
      }
    }
    pC->rowidIsValid = 0;
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
  }
  pTos++;
  pTos->u.i = v;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: Insert P1 P2 P3
**
** Write an entry into the table of cursor P1.  A new entry is
** created if it doesn't already exist or the data for an existing
** entry is overwritten.  The data is the value on the top of the
** stack.  The key is the next value down on the stack.  The key must
** be an integer.  The stack is popped twice by this instruction.
**
** If the OPFLAG_NCHANGE flag of P2 is set, then the row change count is
** incremented (otherwise not).  If the OPFLAG_LASTROWID flag of P2 is set,
** then rowid is stored for subsequent return by the
** sqlite3_last_insert_rowid() function (otherwise it's unmodified).
**
** Parameter P3 may point to a string containing the table-name, or
** may be NULL. If it is not NULL, then the update-hook 
** (sqlite3.xUpdateCallback) is invoked following a successful insert.
**
** This instruction only works on tables.  The equivalent instruction
** for indices is OP_IdxInsert.
*/
case OP_Insert: {         /* no-push */
  Mem *pNos = &pTos[-1];
  int i = pOp->p1;
  Cursor *pC;
  assert( pNos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( ((pC = p->apCsr[i])->pCursor!=0 || pC->pseudoTable) ){
    i64 iKey;   /* The integer ROWID or key for the record to be inserted */

    assert( pNos->flags & MEM_Int );
    assert( pC->isTable );
    iKey = intToKey(pNos->u.i);

    if( pOp->p2 & OPFLAG_NCHANGE ) p->nChange++;
    if( pOp->p2 & OPFLAG_LASTROWID ) db->lastRowid = pNos->u.i;
    if( pC->nextRowidValid && pNos->u.i>=pC->nextRowid ){
      pC->nextRowidValid = 0;
    }
    if( pTos->flags & MEM_Null ){
      pTos->z = 0;
      pTos->n = 0;
    }else{
      assert( pTos->flags & (MEM_Blob|MEM_Str) );
    }
    if( pC->pseudoTable ){
      sqliteFree(pC->pData);
      pC->iKey = iKey;
      pC->nData = pTos->n;
      if( pTos->flags & MEM_Dyn ){
        pC->pData = pTos->z;
        pTos->flags = MEM_Null;
      }else{
        pC->pData = sqliteMallocRaw( pC->nData+2 );
        if( !pC->pData ) goto no_mem;
        memcpy(pC->pData, pTos->z, pC->nData);
        pC->pData[pC->nData] = 0;
        pC->pData[pC->nData+1] = 0;
      }
      pC->nullRow = 0;
    }else{
      rc = sqlite3BtreeInsert(pC->pCursor, 0, iKey,
                              pTos->z, pTos->n,
                              pOp->p2 & OPFLAG_APPEND);
    }
    
    pC->rowidIsValid = 0;
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;

    /* Invoke the update-hook if required. */
    if( rc==SQLITE_OK && db->xUpdateCallback && pOp->p3 ){
      const char *zDb = db->aDb[pC->iDb].zName;
      const char *zTbl = pOp->p3;
      int op = ((pOp->p2 & OPFLAG_ISUPDATE) ? SQLITE_UPDATE : SQLITE_INSERT);
      assert( pC->isTable );
      db->xUpdateCallback(db->pUpdateArg, op, zDb, zTbl, iKey);
      assert( pC->iDb>=0 );
    }
  }
  popStack(&pTos, 2);

  break;
}

/* Opcode: Delete P1 P2 P3
**
** Delete the record at which the P1 cursor is currently pointing.
**
** The cursor will be left pointing at either the next or the previous
** record in the table. If it is left pointing at the next record, then
** the next Next instruction will be a no-op.  Hence it is OK to delete
** a record from within an Next loop.
**
** If the OPFLAG_NCHANGE flag of P2 is set, then the row change count is
** incremented (otherwise not).
**
** If P1 is a pseudo-table, then this instruction is a no-op.
*/
case OP_Delete: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  if( pC->pCursor!=0 ){
    i64 iKey;

    /* If the update-hook will be invoked, set iKey to the rowid of the
    ** row being deleted.
    */
    if( db->xUpdateCallback && pOp->p3 ){
      assert( pC->isTable );
      if( pC->rowidIsValid ){
        iKey = pC->lastRowid;
      }else{
        rc = sqlite3BtreeKeySize(pC->pCursor, &iKey);
        if( rc ){
          goto abort_due_to_error;
        }
        iKey = keyToInt(iKey);
      }
    }

    rc = sqlite3VdbeCursorMoveto(pC);
    if( rc ) goto abort_due_to_error;
    rc = sqlite3BtreeDelete(pC->pCursor);
    pC->nextRowidValid = 0;
    pC->cacheStatus = CACHE_STALE;

    /* Invoke the update-hook if required. */
    if( rc==SQLITE_OK && db->xUpdateCallback && pOp->p3 ){
      const char *zDb = db->aDb[pC->iDb].zName;
      const char *zTbl = pOp->p3;
      db->xUpdateCallback(db->pUpdateArg, SQLITE_DELETE, zDb, zTbl, iKey);
      assert( pC->iDb>=0 );
    }
  }
  if( pOp->p2 & OPFLAG_NCHANGE ) p->nChange++;
  break;
}

/* Opcode: ResetCount P1 * *
**
** This opcode resets the VMs internal change counter to 0. If P1 is true,
** then the value of the change counter is copied to the database handle
** change counter (returned by subsequent calls to sqlite3_changes())
** before it is reset. This is used by trigger programs.
*/
case OP_ResetCount: {        /* no-push */
  if( pOp->p1 ){
    sqlite3VdbeSetChanges(db, p->nChange);
  }
  p->nChange = 0;
  break;
}

/* Opcode: RowData P1 * *
**
** Push onto the stack the complete row data for cursor P1.
** There is no interpretation of the data.  It is just copied
** onto the stack exactly as it is found in the database file.
**
** If the cursor is not pointing to a valid row, a NULL is pushed
** onto the stack.
*/
/* Opcode: RowKey P1 * *
**
** Push onto the stack the complete row key for cursor P1.
** There is no interpretation of the key.  It is just copied
** onto the stack exactly as it is found in the database file.
**
** If the cursor is not pointing to a valid row, a NULL is pushed
** onto the stack.
*/
case OP_RowKey:
case OP_RowData: {
  int i = pOp->p1;
  Cursor *pC;
  u32 n;

  /* Note that RowKey and RowData are really exactly the same instruction */
  pTos++;
  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC->isTable || pOp->opcode==OP_RowKey );
  assert( pC->isIndex || pOp->opcode==OP_RowData );
  assert( pC!=0 );
  if( pC->nullRow ){
    pTos->flags = MEM_Null;
  }else if( pC->pCursor!=0 ){
    BtCursor *pCrsr = pC->pCursor;
    rc = sqlite3VdbeCursorMoveto(pC);
    if( rc ) goto abort_due_to_error;
    if( pC->nullRow ){
      pTos->flags = MEM_Null;
      break;
    }else if( pC->isIndex ){
      i64 n64;
      assert( !pC->isTable );
      sqlite3BtreeKeySize(pCrsr, &n64);
      n = n64;
    }else{
      sqlite3BtreeDataSize(pCrsr, &n);
    }
    pTos->n = n;
    if( n<=NBFS ){
      pTos->flags = MEM_Blob | MEM_Short;
      pTos->z = pTos->zShort;
    }else{
      char *z = sqliteMallocRaw( n );
      if( z==0 ) goto no_mem;
      pTos->flags = MEM_Blob | MEM_Dyn;
      pTos->xDel = 0;
      pTos->z = z;
    }
    if( pC->isIndex ){
      rc = sqlite3BtreeKey(pCrsr, 0, n, pTos->z);
    }else{
      rc = sqlite3BtreeData(pCrsr, 0, n, pTos->z);
    }
  }else if( pC->pseudoTable ){
    pTos->n = pC->nData;
    pTos->z = pC->pData;
    pTos->flags = MEM_Blob|MEM_Ephem;
  }else{
    pTos->flags = MEM_Null;
  }
  pTos->enc = SQLITE_UTF8;  /* In case the blob is ever cast to text */
  break;
}

/* Opcode: Rowid P1 * *
**
** Push onto the stack an integer which is the key of the table entry that
** P1 is currently point to.
*/
case OP_Rowid: {
  int i = pOp->p1;
  Cursor *pC;
  i64 v;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  rc = sqlite3VdbeCursorMoveto(pC);
  if( rc ) goto abort_due_to_error;
  pTos++;
  if( pC->rowidIsValid ){
    v = pC->lastRowid;
  }else if( pC->pseudoTable ){
    v = keyToInt(pC->iKey);
  }else if( pC->nullRow || pC->pCursor==0 ){
    pTos->flags = MEM_Null;
    break;
  }else{
    assert( pC->pCursor!=0 );
    sqlite3BtreeKeySize(pC->pCursor, &v);
    v = keyToInt(v);
  }
  pTos->u.i = v;
  pTos->flags = MEM_Int;
  break;
}

/* Opcode: NullRow P1 * *
**
** Move the cursor P1 to a null row.  Any OP_Column operations
** that occur while the cursor is on the null row will always push 
** a NULL onto the stack.
*/
case OP_NullRow: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  pC->nullRow = 1;
  pC->rowidIsValid = 0;
  break;
}

/* Opcode: Last P1 P2 *
**
** The next use of the Rowid or Column or Next instruction for P1 
** will refer to the last entry in the database table or index.
** If the table or index is empty and P2>0, then jump immediately to P2.
** If P2 is 0 or if the table or index is not empty, fall through
** to the following instruction.
*/
case OP_Last: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  if( (pCrsr = pC->pCursor)!=0 ){
    int res;
    rc = sqlite3BtreeLast(pCrsr, &res);
    pC->nullRow = res;
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
    if( res && pOp->p2>0 ){
      pc = pOp->p2 - 1;
    }
  }else{
    pC->nullRow = 0;
  }
  break;
}


/* Opcode: Sort P1 P2 *
**
** This opcode does exactly the same thing as OP_Rewind except that
** it increments an undocumented global variable used for testing.
**
** Sorting is accomplished by writing records into a sorting index,
** then rewinding that index and playing it back from beginning to
** end.  We use the OP_Sort opcode instead of OP_Rewind to do the
** rewinding so that the global variable will be incremented and
** regression tests can determine whether or not the optimizer is
** correctly optimizing out sorts.
*/
case OP_Sort: {        /* no-push */
#ifdef SQLITE_TEST
  sqlite3_sort_count++;
  sqlite3_search_count--;
#endif
  /* Fall through into OP_Rewind */
}
/* Opcode: Rewind P1 P2 *
**
** The next use of the Rowid or Column or Next instruction for P1 
** will refer to the first entry in the database table or index.
** If the table or index is empty and P2>0, then jump immediately to P2.
** If P2 is 0 or if the table or index is not empty, fall through
** to the following instruction.
*/
case OP_Rewind: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  int res;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  if( (pCrsr = pC->pCursor)!=0 ){
    rc = sqlite3BtreeFirst(pCrsr, &res);
    pC->atFirst = res==0;
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
  }else{
    res = 1;
  }
  pC->nullRow = res;
  if( res && pOp->p2>0 ){
    pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: Next P1 P2 *
**
** Advance cursor P1 so that it points to the next key/data pair in its
** table or index.  If there are no more key/value pairs then fall through
** to the following instruction.  But if the cursor advance was successful,
** jump immediately to P2.
**
** See also: Prev
*/
/* Opcode: Prev P1 P2 *
**
** Back up cursor P1 so that it points to the previous key/data pair in its
** table or index.  If there is no previous key/value pairs then fall through
** to the following instruction.  But if the cursor backup was successful,
** jump immediately to P2.
*/
case OP_Prev:          /* no-push */
case OP_Next: {        /* no-push */
  Cursor *pC;
  BtCursor *pCrsr;

  CHECK_FOR_INTERRUPT;
  assert( pOp->p1>=0 && pOp->p1<p->nCursor );
  pC = p->apCsr[pOp->p1];
  if( pC==0 ){
    break;  /* See ticket #2273 */
  }
  if( (pCrsr = pC->pCursor)!=0 ){
    int res;
    if( pC->nullRow ){
      res = 1;
    }else{
      assert( pC->deferredMoveto==0 );
      rc = pOp->opcode==OP_Next ? sqlite3BtreeNext(pCrsr, &res) :
                                  sqlite3BtreePrevious(pCrsr, &res);
      pC->nullRow = res;
      pC->cacheStatus = CACHE_STALE;
    }
    if( res==0 ){
      pc = pOp->p2 - 1;
#ifdef SQLITE_TEST
      sqlite3_search_count++;
#endif
    }
  }else{
    pC->nullRow = 1;
  }
  pC->rowidIsValid = 0;
  break;
}

/* Opcode: IdxInsert P1 P2 *
**
** The top of the stack holds a SQL index key made using either the
** MakeIdxRec or MakeRecord instructions.  This opcode writes that key
** into the index P1.  Data for the entry is nil.
**
** P2 is a flag that provides a hint to the b-tree layer that this
** insert is likely to be an append.
**
** This instruction only works for indices.  The equivalent instruction
** for tables is OP_Insert.
*/
case OP_IdxInsert: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  assert( pTos->flags & MEM_Blob );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    int nKey = pTos->n;
    const char *zKey = pTos->z;
    assert( pC->isTable==0 );
    rc = sqlite3BtreeInsert(pCrsr, zKey, nKey, "", 0, pOp->p2);
    assert( pC->deferredMoveto==0 );
    pC->cacheStatus = CACHE_STALE;
  }
  Release(pTos);
  pTos--;
  break;
}

/* Opcode: IdxDelete P1 * *
**
** The top of the stack is an index key built using the either the
** MakeIdxRec or MakeRecord opcodes.
** This opcode removes that entry from the index.
*/
case OP_IdxDelete: {        /* no-push */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( pTos>=p->aStack );
  assert( pTos->flags & MEM_Blob );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    int res;
    rc = sqlite3BtreeMoveto(pCrsr, pTos->z, pTos->n, 0, &res);
    if( rc==SQLITE_OK && res==0 ){
      rc = sqlite3BtreeDelete(pCrsr);
    }
    assert( pC->deferredMoveto==0 );
    pC->cacheStatus = CACHE_STALE;
  }
  Release(pTos);
  pTos--;
  break;
}

/* Opcode: IdxRowid P1 * *
**
** Push onto the stack an integer which is the last entry in the record at
** the end of the index key pointed to by cursor P1.  This integer should be
** the rowid of the table entry to which this index entry points.
**
** See also: Rowid, MakeIdxRec.
*/
case OP_IdxRowid: {
  int i = pOp->p1;
  BtCursor *pCrsr;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  pTos++;
  pTos->flags = MEM_Null;
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    i64 rowid;

    assert( pC->deferredMoveto==0 );
    assert( pC->isTable==0 );
    if( pC->nullRow ){
      pTos->flags = MEM_Null;
    }else{
      rc = sqlite3VdbeIdxRowid(pCrsr, &rowid);
      if( rc!=SQLITE_OK ){
        goto abort_due_to_error;
      }
      pTos->flags = MEM_Int;
      pTos->u.i = rowid;
    }
  }
  break;
}

/* Opcode: IdxGT P1 P2 *
**
** The top of the stack is an index entry that omits the ROWID.  Compare
** the top of stack against the index that P1 is currently pointing to.
** Ignore the ROWID on the P1 index.
**
** The top of the stack might have fewer columns that P1.
**
** If the P1 index entry is greater than the top of the stack
** then jump to P2.  Otherwise fall through to the next instruction.
** In either case, the stack is popped once.
*/
/* Opcode: IdxGE P1 P2 P3
**
** The top of the stack is an index entry that omits the ROWID.  Compare
** the top of stack against the index that P1 is currently pointing to.
** Ignore the ROWID on the P1 index.
**
** If the P1 index entry is greater than or equal to the top of the stack
** then jump to P2.  Otherwise fall through to the next instruction.
** In either case, the stack is popped once.
**
** If P3 is the "+" string (or any other non-NULL string) then the
** index taken from the top of the stack is temporarily increased by
** an epsilon prior to the comparison.  This make the opcode work
** like IdxGT except that if the key from the stack is a prefix of
** the key in the cursor, the result is false whereas it would be
** true with IdxGT.
*/
/* Opcode: IdxLT P1 P2 P3
**
** The top of the stack is an index entry that omits the ROWID.  Compare
** the top of stack against the index that P1 is currently pointing to.
** Ignore the ROWID on the P1 index.
**
** If the P1 index entry is less than  the top of the stack
** then jump to P2.  Otherwise fall through to the next instruction.
** In either case, the stack is popped once.
**
** If P3 is the "+" string (or any other non-NULL string) then the
** index taken from the top of the stack is temporarily increased by
** an epsilon prior to the comparison.  This makes the opcode work
** like IdxLE.
*/
case OP_IdxLT:          /* no-push */
case OP_IdxGT:          /* no-push */
case OP_IdxGE: {        /* no-push */
  int i= pOp->p1;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  assert( pTos>=p->aStack );
  if( (pC = p->apCsr[i])->pCursor!=0 ){
    int res;
 
    assert( pTos->flags & MEM_Blob );  /* Created using OP_Make*Key */
    Stringify(pTos, encoding);
    assert( pC->deferredMoveto==0 );
    *pC->pIncrKey = pOp->p3!=0;
    assert( pOp->p3==0 || pOp->opcode!=OP_IdxGT );
    rc = sqlite3VdbeIdxKeyCompare(pC, pTos->n, (u8*)pTos->z, &res);
    *pC->pIncrKey = 0;
    if( rc!=SQLITE_OK ){
      break;
    }
    if( pOp->opcode==OP_IdxLT ){
      res = -res;
    }else if( pOp->opcode==OP_IdxGE ){
      res++;
    }
    if( res>0 ){
      pc = pOp->p2 - 1 ;
    }
  }
  Release(pTos);
  pTos--;
  break;
}

/* Opcode: Destroy P1 P2 *
**
** Delete an entire database table or index whose root page in the database
** file is given by P1.
**
** The table being destroyed is in the main database file if P2==0.  If
** P2==1 then the table to be clear is in the auxiliary database file
** that is used to store tables create using CREATE TEMPORARY TABLE.
**
** If AUTOVACUUM is enabled then it is possible that another root page
** might be moved into the newly deleted root page in order to keep all
** root pages contiguous at the beginning of the database.  The former
** value of the root page that moved - its value before the move occurred -
** is pushed onto the stack.  If no page movement was required (because
** the table being dropped was already the last one in the database) then
** a zero is pushed onto the stack.  If AUTOVACUUM is disabled
** then a zero is pushed onto the stack.
**
** See also: Clear
*/
case OP_Destroy: {
  int iMoved;
  int iCnt;
#ifndef SQLITE_OMIT_VIRTUALTABLE
  Vdbe *pVdbe;
  iCnt = 0;
  for(pVdbe=db->pVdbe; pVdbe; pVdbe=pVdbe->pNext){
    if( pVdbe->magic==VDBE_MAGIC_RUN && pVdbe->inVtabMethod<2 && pVdbe->pc>=0 ){
      iCnt++;
    }
  }
#else
  iCnt = db->activeVdbeCnt;
#endif
  if( iCnt>1 ){
    rc = SQLITE_LOCKED;
  }else{
    assert( iCnt==1 );
    rc = sqlite3BtreeDropTable(db->aDb[pOp->p2].pBt, pOp->p1, &iMoved);
    pTos++;
    pTos->flags = MEM_Int;
    pTos->u.i = iMoved;
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( rc==SQLITE_OK && iMoved!=0 ){
      sqlite3RootPageMoved(&db->aDb[pOp->p2], iMoved, pOp->p1);
    }
#endif
  }
  break;
}

/* Opcode: Clear P1 P2 *
**
** Delete all contents of the database table or index whose root page
** in the database file is given by P1.  But, unlike Destroy, do not
** remove the table or index from the database file.
**
** The table being clear is in the main database file if P2==0.  If
** P2==1 then the table to be clear is in the auxiliary database file
** that is used to store tables create using CREATE TEMPORARY TABLE.
**
** See also: Destroy
*/
case OP_Clear: {        /* no-push */

  /* For consistency with the way other features of SQLite operate
  ** with a truncate, we will also skip the update callback.
  */
#if 0
  Btree *pBt = db->aDb[pOp->p2].pBt;
  if( db->xUpdateCallback && pOp->p3 ){
    const char *zDb = db->aDb[pOp->p2].zName;
    const char *zTbl = pOp->p3;
    BtCursor *pCur = 0;
    int fin = 0;

    rc = sqlite3BtreeCursor(pBt, pOp->p1, 0, 0, 0, &pCur);
    if( rc!=SQLITE_OK ){
      goto abort_due_to_error;
    }
    for(
      rc=sqlite3BtreeFirst(pCur, &fin); 
      rc==SQLITE_OK && !fin; 
      rc=sqlite3BtreeNext(pCur, &fin)
    ){
      i64 iKey;
      rc = sqlite3BtreeKeySize(pCur, &iKey);
      if( rc ){
        break;
      }
      iKey = keyToInt(iKey);
      db->xUpdateCallback(db->pUpdateArg, SQLITE_DELETE, zDb, zTbl, iKey);
    }
    sqlite3BtreeCloseCursor(pCur);
    if( rc!=SQLITE_OK ){
      goto abort_due_to_error;
    }
  }
#endif
  rc = sqlite3BtreeClearTable(db->aDb[pOp->p2].pBt, pOp->p1);
  break;
}

/* Opcode: CreateTable P1 * *
**
** Allocate a new table in the main database file if P2==0 or in the
** auxiliary database file if P2==1.  Push the page number
** for the root page of the new table onto the stack.
**
** The difference between a table and an index is this:  A table must
** have a 4-byte integer key and can have arbitrary data.  An index
** has an arbitrary key but no data.
**
** See also: CreateIndex
*/
/* Opcode: CreateIndex P1 * *
**
** Allocate a new index in the main database file if P2==0 or in the
** auxiliary database file if P2==1.  Push the page number of the
** root page of the new index onto the stack.
**
** See documentation on OP_CreateTable for additional information.
*/
case OP_CreateIndex:
case OP_CreateTable: {
  int pgno;
  int flags;
  Db *pDb;
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  pDb = &db->aDb[pOp->p1];
  assert( pDb->pBt!=0 );
  if( pOp->opcode==OP_CreateTable ){
    /* flags = BTREE_INTKEY; */
    flags = BTREE_LEAFDATA|BTREE_INTKEY;
  }else{
    flags = BTREE_ZERODATA;
  }
  rc = sqlite3BtreeCreateTable(pDb->pBt, &pgno, flags);
  pTos++;
  if( rc==SQLITE_OK ){
    pTos->u.i = pgno;
    pTos->flags = MEM_Int;
  }else{
    pTos->flags = MEM_Null;
  }
  break;
}

/* Opcode: ParseSchema P1 P2 P3
**
** Read and parse all entries from the SQLITE_MASTER table of database P1
** that match the WHERE clause P3.  P2 is the "force" flag.   Always do
** the parsing if P2 is true.  If P2 is false, then this routine is a
** no-op if the schema is not currently loaded.  In other words, if P2
** is false, the SQLITE_MASTER table is only parsed if the rest of the
** schema is already loaded into the symbol table.
**
** This opcode invokes the parser to create a new virtual machine,
** then runs the new virtual machine.  It is thus a reentrant opcode.
*/
case OP_ParseSchema: {        /* no-push */
  char *zSql;
  int iDb = pOp->p1;
  const char *zMaster;
  InitData initData;

  assert( iDb>=0 && iDb<db->nDb );
  if( !pOp->p2 && !DbHasProperty(db, iDb, DB_SchemaLoaded) ){
    break;
  }
  zMaster = SCHEMA_TABLE(iDb);
  initData.db = db;
  initData.iDb = pOp->p1;
  initData.pzErrMsg = &p->zErrMsg;
  zSql = sqlite3MPrintf(
     "SELECT name, rootpage, sql FROM '%q'.%s WHERE %s",
     db->aDb[iDb].zName, zMaster, pOp->p3);
  if( zSql==0 ) goto no_mem;
  sqlite3SafetyOff(db);
  assert( db->init.busy==0 );
  db->init.busy = 1;
  assert( !sqlite3MallocFailed() );
  rc = sqlite3_exec(db, zSql, sqlite3InitCallback, &initData, 0);
  if( rc==SQLITE_ABORT ) rc = initData.rc;
  sqliteFree(zSql);
  db->init.busy = 0;
  sqlite3SafetyOn(db);
  if( rc==SQLITE_NOMEM ){
    sqlite3FailedMalloc();
    goto no_mem;
  }
  break;  
}

#if !defined(SQLITE_OMIT_ANALYZE) && !defined(SQLITE_OMIT_PARSER)
/* Opcode: LoadAnalysis P1 * *
**
** Read the sqlite_stat1 table for database P1 and load the content
** of that table into the internal index hash table.  This will cause
** the analysis to be used when preparing all subsequent queries.
*/
case OP_LoadAnalysis: {        /* no-push */
  int iDb = pOp->p1;
  assert( iDb>=0 && iDb<db->nDb );
  sqlite3AnalysisLoad(db, iDb);
  break;  
}
#endif /* !defined(SQLITE_OMIT_ANALYZE) && !defined(SQLITE_OMIT_PARSER)  */

/* Opcode: DropTable P1 * P3
**
** Remove the internal (in-memory) data structures that describe
** the table named P3 in database P1.  This is called after a table
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropTable: {        /* no-push */
  sqlite3UnlinkAndDeleteTable(db, pOp->p1, pOp->p3);
  break;
}

/* Opcode: DropIndex P1 * P3
**
** Remove the internal (in-memory) data structures that describe
** the index named P3 in database P1.  This is called after an index
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropIndex: {        /* no-push */
  sqlite3UnlinkAndDeleteIndex(db, pOp->p1, pOp->p3);
  break;
}

/* Opcode: DropTrigger P1 * P3
**
** Remove the internal (in-memory) data structures that describe
** the trigger named P3 in database P1.  This is called after a trigger
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropTrigger: {        /* no-push */
  sqlite3UnlinkAndDeleteTrigger(db, pOp->p1, pOp->p3);
  break;
}


#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/* Opcode: IntegrityCk P1 P2 *
**
** Do an analysis of the currently open database.  Push onto the
** stack the text of an error message describing any problems.
** If no problems are found, push a NULL onto the stack.
**
** P1 is the address of a memory cell that contains the maximum
** number of allowed errors.  At most mem[P1] errors will be reported.
** In other words, the analysis stops as soon as mem[P1] errors are 
** seen.  Mem[P1] is updated with the number of errors remaining.
**
** The root page numbers of all tables in the database are integer
** values on the stack.  This opcode pulls as many integers as it
** can off of the stack and uses those numbers as the root pages.
**
** If P2 is not zero, the check is done on the auxiliary database
** file, not the main database file.
**
** This opcode is used to implement the integrity_check pragma.
*/
case OP_IntegrityCk: {
  int nRoot;
  int *aRoot;
  int j;
  int nErr;
  char *z;
  Mem *pnErr;

  for(nRoot=0; &pTos[-nRoot]>=p->aStack; nRoot++){
    if( (pTos[-nRoot].flags & MEM_Int)==0 ) break;
  }
  assert( nRoot>0 );
  aRoot = sqliteMallocRaw( sizeof(int*)*(nRoot+1) );
  if( aRoot==0 ) goto no_mem;
  j = pOp->p1;
  assert( j>=0 && j<p->nMem );
  pnErr = &p->aMem[j];
  assert( (pnErr->flags & MEM_Int)!=0 );
  for(j=0; j<nRoot; j++){
    Mem *pMem = &pTos[-j];
    aRoot[j] = pMem->u.i;
  }
  aRoot[j] = 0;
  popStack(&pTos, nRoot);
  pTos++;
  z = sqlite3BtreeIntegrityCheck(db->aDb[pOp->p2].pBt, aRoot, nRoot,
                                 pnErr->u.i, &nErr);
  pnErr->u.i -= nErr;
  if( nErr==0 ){
    assert( z==0 );
    pTos->flags = MEM_Null;
  }else{
    pTos->z = z;
    pTos->n = strlen(z);
    pTos->flags = MEM_Str | MEM_Dyn | MEM_Term;
    pTos->xDel = 0;
  }
  pTos->enc = SQLITE_UTF8;
  sqlite3VdbeChangeEncoding(pTos, encoding);
  sqliteFree(aRoot);
  break;
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

/* Opcode: FifoWrite * * *
**
** Write the integer on the top of the stack
** into the Fifo.
*/
case OP_FifoWrite: {        /* no-push */
  assert( pTos>=p->aStack );
  sqlite3VdbeMemIntegerify(pTos);
  sqlite3VdbeFifoPush(&p->sFifo, pTos->u.i);
  assert( (pTos->flags & MEM_Dyn)==0 );
  pTos--;
  break;
}

/* Opcode: FifoRead * P2 *
**
** Attempt to read a single integer from the Fifo
** and push it onto the stack.  If the Fifo is empty
** push nothing but instead jump to P2.
*/
case OP_FifoRead: {
  i64 v;
  CHECK_FOR_INTERRUPT;
  if( sqlite3VdbeFifoPop(&p->sFifo, &v)==SQLITE_DONE ){
    pc = pOp->p2 - 1;
  }else{
    pTos++;
    pTos->u.i = v;
    pTos->flags = MEM_Int;
  }
  break;
}

#ifndef SQLITE_OMIT_TRIGGER
/* Opcode: ContextPush * * * 
**
** Save the current Vdbe context such that it can be restored by a ContextPop
** opcode. The context stores the last insert row id, the last statement change
** count, and the current statement change count.
*/
case OP_ContextPush: {        /* no-push */
  int i = p->contextStackTop++;
  Context *pContext;

  assert( i>=0 );
  /* FIX ME: This should be allocated as part of the vdbe at compile-time */
  if( i>=p->contextStackDepth ){
    p->contextStackDepth = i+1;
    p->contextStack = sqliteReallocOrFree(p->contextStack,
                                          sizeof(Context)*(i+1));
    if( p->contextStack==0 ) goto no_mem;
  }
  pContext = &p->contextStack[i];
  pContext->lastRowid = db->lastRowid;
  pContext->nChange = p->nChange;
  pContext->sFifo = p->sFifo;
  sqlite3VdbeFifoInit(&p->sFifo);
  break;
}

/* Opcode: ContextPop * * * 
**
** Restore the Vdbe context to the state it was in when contextPush was last
** executed. The context stores the last insert row id, the last statement
** change count, and the current statement change count.
*/
case OP_ContextPop: {        /* no-push */
  Context *pContext = &p->contextStack[--p->contextStackTop];
  assert( p->contextStackTop>=0 );
  db->lastRowid = pContext->lastRowid;
  p->nChange = pContext->nChange;
  sqlite3VdbeFifoClear(&p->sFifo);
  p->sFifo = pContext->sFifo;
  break;
}
#endif /* #ifndef SQLITE_OMIT_TRIGGER */

/* Opcode: MemStore P1 P2 *
**
** Write the top of the stack into memory location P1.
** P1 should be a small integer since space is allocated
** for all memory locations between 0 and P1 inclusive.
**
** After the data is stored in the memory location, the
** stack is popped once if P2 is 1.  If P2 is zero, then
** the original data remains on the stack.
*/
case OP_MemStore: {        /* no-push */
  assert( pTos>=p->aStack );
  assert( pOp->p1>=0 && pOp->p1<p->nMem );
  rc = sqlite3VdbeMemMove(&p->aMem[pOp->p1], pTos);
  pTos--;

  /* If P2 is 0 then fall thru to the next opcode, OP_MemLoad, that will
  ** restore the top of the stack to its original value.
  */
  if( pOp->p2 ){
    break;
  }
}
/* Opcode: MemLoad P1 * *
**
** Push a copy of the value in memory location P1 onto the stack.
**
** If the value is a string, then the value pushed is a pointer to
** the string that is stored in the memory location.  If the memory
** location is subsequently changed (using OP_MemStore) then the
** value pushed onto the stack will change too.
*/
case OP_MemLoad: {
  int i = pOp->p1;
  assert( i>=0 && i<p->nMem );
  pTos++;
  sqlite3VdbeMemShallowCopy(pTos, &p->aMem[i], MEM_Ephem);
  break;
}

#ifndef SQLITE_OMIT_AUTOINCREMENT
/* Opcode: MemMax P1 * *
**
** Set the value of memory cell P1 to the maximum of its current value
** and the value on the top of the stack.  The stack is unchanged.
**
** This instruction throws an error if the memory cell is not initially
** an integer.
*/
case OP_MemMax: {        /* no-push */
  int i = pOp->p1;
  Mem *pMem;
  assert( pTos>=p->aStack );
  assert( i>=0 && i<p->nMem );
  pMem = &p->aMem[i];
  sqlite3VdbeMemIntegerify(pMem);
  sqlite3VdbeMemIntegerify(pTos);
  if( pMem->u.i<pTos->u.i){
    pMem->u.i = pTos->u.i;
  }
  break;
}
#endif /* SQLITE_OMIT_AUTOINCREMENT */

/* Opcode: MemIncr P1 P2 *
**
** Increment the integer valued memory cell P2 by the value in P1.
**
** It is illegal to use this instruction on a memory cell that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_MemIncr: {        /* no-push */
  int i = pOp->p2;
  Mem *pMem;
  assert( i>=0 && i<p->nMem );
  pMem = &p->aMem[i];
  assert( pMem->flags==MEM_Int );
  pMem->u.i += pOp->p1;
  break;
}

/* Opcode: IfMemPos P1 P2 *
**
** If the value of memory cell P1 is 1 or greater, jump to P2.
**
** It is illegal to use this instruction on a memory cell that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfMemPos: {        /* no-push */
  int i = pOp->p1;
  Mem *pMem;
  assert( i>=0 && i<p->nMem );
  pMem = &p->aMem[i];
  assert( pMem->flags==MEM_Int );
  if( pMem->u.i>0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: IfMemNeg P1 P2 *
**
** If the value of memory cell P1 is less than zero, jump to P2. 
**
** It is illegal to use this instruction on a memory cell that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfMemNeg: {        /* no-push */
  int i = pOp->p1;
  Mem *pMem;
  assert( i>=0 && i<p->nMem );
  pMem = &p->aMem[i];
  assert( pMem->flags==MEM_Int );
  if( pMem->u.i<0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: IfMemZero P1 P2 *
**
** If the value of memory cell P1 is exactly 0, jump to P2. 
**
** It is illegal to use this instruction on a memory cell that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfMemZero: {        /* no-push */
  int i = pOp->p1;
  Mem *pMem;
  assert( i>=0 && i<p->nMem );
  pMem = &p->aMem[i];
  assert( pMem->flags==MEM_Int );
  if( pMem->u.i==0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: MemNull P1 * *
**
** Store a NULL in memory cell P1
*/
case OP_MemNull: {
  assert( pOp->p1>=0 && pOp->p1<p->nMem );
  sqlite3VdbeMemSetNull(&p->aMem[pOp->p1]);
  break;
}

/* Opcode: MemInt P1 P2 *
**
** Store the integer value P1 in memory cell P2.
*/
case OP_MemInt: {
  assert( pOp->p2>=0 && pOp->p2<p->nMem );
  sqlite3VdbeMemSetInt64(&p->aMem[pOp->p2], pOp->p1);
  break;
}

/* Opcode: MemMove P1 P2 *
**
** Move the content of memory cell P2 over to memory cell P1.
** Any prior content of P1 is erased.  Memory cell P2 is left
** containing a NULL.
*/
case OP_MemMove: {
  assert( pOp->p1>=0 && pOp->p1<p->nMem );
  assert( pOp->p2>=0 && pOp->p2<p->nMem );
  rc = sqlite3VdbeMemMove(&p->aMem[pOp->p1], &p->aMem[pOp->p2]);
  break;
}

/* Opcode: AggStep P1 P2 P3
**
** Execute the step function for an aggregate.  The
** function has P2 arguments.  P3 is a pointer to the FuncDef
** structure that specifies the function.  Use memory location
** P1 as the accumulator.
**
** The P2 arguments are popped from the stack.
*/
case OP_AggStep: {        /* no-push */
  int n = pOp->p2;
  int i;
  Mem *pMem, *pRec;
  sqlite3_context ctx;
  sqlite3_value **apVal;

  assert( n>=0 );
  pRec = &pTos[1-n];
  assert( pRec>=p->aStack );
  apVal = p->apArg;
  assert( apVal || n==0 );
  for(i=0; i<n; i++, pRec++){
    apVal[i] = pRec;
    storeTypeInfo(pRec, encoding);
  }
  ctx.pFunc = (FuncDef*)pOp->p3;
  assert( pOp->p1>=0 && pOp->p1<p->nMem );
  ctx.pMem = pMem = &p->aMem[pOp->p1];
  pMem->n++;
  ctx.s.flags = MEM_Null;
  ctx.s.z = 0;
  ctx.s.xDel = 0;
  ctx.isError = 0;
  ctx.pColl = 0;
  if( ctx.pFunc->needCollSeq ){
    assert( pOp>p->aOp );
    assert( pOp[-1].p3type==P3_COLLSEQ );
    assert( pOp[-1].opcode==OP_CollSeq );
    ctx.pColl = (CollSeq *)pOp[-1].p3;
  }
  (ctx.pFunc->xStep)(&ctx, n, apVal);
  popStack(&pTos, n);
  if( ctx.isError ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(&ctx.s), (char*)0);
    rc = SQLITE_ERROR;
  }
  sqlite3VdbeMemRelease(&ctx.s);
  break;
}

/* Opcode: AggFinal P1 P2 P3
**
** Execute the finalizer function for an aggregate.  P1 is
** the memory location that is the accumulator for the aggregate.
**
** P2 is the number of arguments that the step function takes and
** P3 is a pointer to the FuncDef for this function.  The P2
** argument is not used by this opcode.  It is only there to disambiguate
** functions that can take varying numbers of arguments.  The
** P3 argument is only needed for the degenerate case where
** the step function was not previously called.
*/
case OP_AggFinal: {        /* no-push */
  Mem *pMem;
  assert( pOp->p1>=0 && pOp->p1<p->nMem );
  pMem = &p->aMem[pOp->p1];
  assert( (pMem->flags & ~(MEM_Null|MEM_Agg))==0 );
  rc = sqlite3VdbeMemFinalize(pMem, (FuncDef*)pOp->p3);
  if( rc==SQLITE_ERROR ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(pMem), (char*)0);
  }
  break;
}


#if !defined(SQLITE_OMIT_VACUUM) && !defined(SQLITE_OMIT_ATTACH)
/* Opcode: Vacuum * * *
**
** Vacuum the entire database.  This opcode will cause other virtual
** machines to be created and run.  It may not be called from within
** a transaction.
*/
case OP_Vacuum: {        /* no-push */
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse; 
  rc = sqlite3RunVacuum(&p->zErrMsg, db);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  break;
}
#endif

/* Opcode: Expire P1 * *
**
** Cause precompiled statements to become expired. An expired statement
** fails with an error code of SQLITE_SCHEMA if it is ever executed 
** (via sqlite3_step()).
** 
** If P1 is 0, then all SQL statements become expired. If P1 is non-zero,
** then only the currently executing statement is affected. 
*/
case OP_Expire: {        /* no-push */
  if( !pOp->p1 ){
    sqlite3ExpirePreparedStatements(db);
  }else{
    p->expired = 1;
  }
  break;
}

#ifndef SQLITE_OMIT_SHARED_CACHE
/* Opcode: TableLock P1 P2 P3
**
** Obtain a lock on a particular table. This instruction is only used when
** the shared-cache feature is enabled. 
**
** If P1 is not negative, then it is the index of the database
** in sqlite3.aDb[] and a read-lock is required. If P1 is negative, a 
** write-lock is required. In this case the index of the database is the 
** absolute value of P1 minus one (iDb = abs(P1) - 1;) and a write-lock is
** required. 
**
** P2 contains the root-page of the table to lock.
**
** P3 contains a pointer to the name of the table being locked. This is only
** used to generate an error message if the lock cannot be obtained.
*/
case OP_TableLock: {        /* no-push */
  int p1 = pOp->p1; 
  u8 isWriteLock = (p1<0);
  if( isWriteLock ){
    p1 = (-1*p1)-1;
  }
  rc = sqlite3BtreeLockTable(db->aDb[p1].pBt, pOp->p2, isWriteLock);
  if( rc==SQLITE_LOCKED ){
    const char *z = (const char *)pOp->p3;
    sqlite3SetString(&p->zErrMsg, "database table is locked: ", z, (char*)0);
  }
  break;
}
#endif /* SQLITE_OMIT_SHARED_CACHE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VBegin * * P3
**
** P3 a pointer to an sqlite3_vtab structure. Call the xBegin method 
** for that table.
*/
case OP_VBegin: {   /* no-push */
  rc = sqlite3VtabBegin(db, (sqlite3_vtab *)pOp->p3);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VCreate P1 * P3
**
** P3 is the name of a virtual table in database P1. Call the xCreate method
** for that table.
*/
case OP_VCreate: {   /* no-push */
  rc = sqlite3VtabCallCreate(db, pOp->p1, pOp->p3, &p->zErrMsg);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VDestroy P1 * P3
**
** P3 is the name of a virtual table in database P1.  Call the xDestroy method
** of that table.
*/
case OP_VDestroy: {   /* no-push */
  p->inVtabMethod = 2;
  rc = sqlite3VtabCallDestroy(db, pOp->p1, pOp->p3);
  p->inVtabMethod = 0;
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VOpen P1 * P3
**
** P3 is a pointer to a virtual table object, an sqlite3_vtab structure.
** P1 is a cursor number.  This opcode opens a cursor to the virtual
** table and stores that cursor in P1.
*/
case OP_VOpen: {   /* no-push */
  Cursor *pCur = 0;
  sqlite3_vtab_cursor *pVtabCursor = 0;

  sqlite3_vtab *pVtab = (sqlite3_vtab *)(pOp->p3);
  sqlite3_module *pModule = (sqlite3_module *)pVtab->pModule;

  assert(pVtab && pModule);
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  rc = pModule->xOpen(pVtab, &pVtabCursor);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  if( SQLITE_OK==rc ){
    /* Initialise sqlite3_vtab_cursor base class */
    pVtabCursor->pVtab = pVtab;

    /* Initialise vdbe cursor object */
    pCur = allocateCursor(p, pOp->p1, -1);
    if( pCur ){
      pCur->pVtabCursor = pVtabCursor;
      pCur->pModule = pVtabCursor->pVtab->pModule;
    }else{
      pModule->xClose(pVtabCursor);
    }
  }
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VFilter P1 P2 P3
**
** P1 is a cursor opened using VOpen.  P2 is an address to jump to if
** the filtered result set is empty.
**
** P3 is either NULL or a string that was generated by the xBestIndex
** method of the module.  The interpretation of the P3 string is left
** to the module implementation.
**
** This opcode invokes the xFilter method on the virtual table specified
** by P1.  The integer query plan parameter to xFilter is the top of the
** stack.  Next down on the stack is the argc parameter.  Beneath the
** next of stack are argc additional parameters which are passed to
** xFilter as argv. The topmost parameter (i.e. 3rd element popped from
** the stack) becomes argv[argc-1] when passed to xFilter.
**
** The integer query plan parameter, argc, and all argv stack values 
** are popped from the stack before this instruction completes.
**
** A jump is made to P2 if the result set after filtering would be 
** empty.
*/
case OP_VFilter: {   /* no-push */
  int nArg;

  const sqlite3_module *pModule;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  pModule = pCur->pVtabCursor->pVtab->pModule;

  /* Grab the index number and argc parameters off the top of the stack. */
  assert( (&pTos[-1])>=p->aStack );
  assert( (pTos[0].flags&MEM_Int)!=0 && pTos[-1].flags==MEM_Int );
  nArg = pTos[-1].u.i;

  /* Invoke the xFilter method */
  {
    int res = 0;
    int i;
    Mem **apArg = p->apArg;
    for(i = 0; i<nArg; i++){
      apArg[i] = &pTos[i+1-2-nArg];
      storeTypeInfo(apArg[i], 0);
    }

    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    p->inVtabMethod = 1;
    rc = pModule->xFilter(pCur->pVtabCursor, pTos->u.i, pOp->p3, nArg, apArg);
    p->inVtabMethod = 0;
    if( rc==SQLITE_OK ){
      res = pModule->xEof(pCur->pVtabCursor);
    }
    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;

    if( res ){
      pc = pOp->p2 - 1;
    }
  }

  /* Pop the index number, argc value and parameters off the stack */
  popStack(&pTos, 2+nArg);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VRowid P1 * *
**
** Push an integer onto the stack which is the rowid of
** the virtual-table that the P1 cursor is pointing to.
*/
case OP_VRowid: {
  const sqlite3_module *pModule;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  pModule = pCur->pVtabCursor->pVtab->pModule;
  if( pModule->xRowid==0 ){
    sqlite3SetString(&p->zErrMsg, "Unsupported module operation: xRowid", 0);
    rc = SQLITE_ERROR;
  } else {
    sqlite_int64 iRow;

    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    rc = pModule->xRowid(pCur->pVtabCursor, &iRow);
    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;

    pTos++;
    pTos->flags = MEM_Int;
    pTos->u.i = iRow;
  }

  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VColumn P1 P2 *
**
** Push onto the stack the value of the P2-th column of
** the row of the virtual-table that the P1 cursor is pointing to.
*/
case OP_VColumn: {
  const sqlite3_module *pModule;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  pModule = pCur->pVtabCursor->pVtab->pModule;
  if( pModule->xColumn==0 ){
    sqlite3SetString(&p->zErrMsg, "Unsupported module operation: xColumn", 0);
    rc = SQLITE_ERROR;
  } else {
    sqlite3_context sContext;
    memset(&sContext, 0, sizeof(sContext));
    sContext.s.flags = MEM_Null;
    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    rc = pModule->xColumn(pCur->pVtabCursor, &sContext, pOp->p2);

    /* Copy the result of the function to the top of the stack. We
    ** do this regardless of whether or not an error occured to ensure any
    ** dynamic allocation in sContext.s (a Mem struct) is  released.
    */
    sqlite3VdbeChangeEncoding(&sContext.s, encoding);
    pTos++;
    pTos->flags = 0;
    sqlite3VdbeMemMove(pTos, &sContext.s);

    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  }
  
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VNext P1 P2 *
**
** Advance virtual table P1 to the next row in its result set and
** jump to instruction P2.  Or, if the virtual table has reached
** the end of its result set, then fall through to the next instruction.
*/
case OP_VNext: {   /* no-push */
  const sqlite3_module *pModule;
  int res = 0;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  pModule = pCur->pVtabCursor->pVtab->pModule;
  if( pModule->xNext==0 ){
    sqlite3SetString(&p->zErrMsg, "Unsupported module operation: xNext", 0);
    rc = SQLITE_ERROR;
  } else {
    /* Invoke the xNext() method of the module. There is no way for the
    ** underlying implementation to return an error if one occurs during
    ** xNext(). Instead, if an error occurs, true is returned (indicating that 
    ** data is available) and the error code returned when xColumn or
    ** some other method is next invoked on the save virtual table cursor.
    */
    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    p->inVtabMethod = 1;
    rc = pModule->xNext(pCur->pVtabCursor);
    p->inVtabMethod = 0;
    if( rc==SQLITE_OK ){
      res = pModule->xEof(pCur->pVtabCursor);
    }
    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;

    if( !res ){
      /* If there is data, jump to P2 */
      pc = pOp->p2 - 1;
    }
  }

  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */


#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VUpdate P1 P2 P3
**
** P3 is a pointer to a virtual table object, an sqlite3_vtab structure.
** This opcode invokes the corresponding xUpdate method. P2 values
** are taken from the stack to pass to the xUpdate invocation. The
** value on the top of the stack corresponds to the p2th element 
** of the argv array passed to xUpdate.
**
** The xUpdate method will do a DELETE or an INSERT or both.
** The argv[0] element (which corresponds to the P2-th element down
** on the stack) is the rowid of a row to delete.  If argv[0] is
** NULL then no deletion occurs.  The argv[1] element is the rowid
** of the new row.  This can be NULL to have the virtual table
** select the new rowid for itself.  The higher elements in the
** stack are the values of columns in the new row.
**
** If P2==1 then no insert is performed.  argv[0] is the rowid of
** a row to delete.
**
** P1 is a boolean flag. If it is set to true and the xUpdate call
** is successful, then the value returned by sqlite3_last_insert_rowid() 
** is set to the value of the rowid for the row just inserted.
*/
case OP_VUpdate: {   /* no-push */
  sqlite3_vtab *pVtab = (sqlite3_vtab *)(pOp->p3);
  sqlite3_module *pModule = (sqlite3_module *)pVtab->pModule;
  int nArg = pOp->p2;
  assert( pOp->p3type==P3_VTAB );
  if( pModule->xUpdate==0 ){
    sqlite3SetString(&p->zErrMsg, "read-only table", 0);
    rc = SQLITE_ERROR;
  }else{
    int i;
    sqlite_int64 rowid;
    Mem **apArg = p->apArg;
    Mem *pX = &pTos[1-nArg];
    for(i = 0; i<nArg; i++, pX++){
      storeTypeInfo(pX, 0);
      apArg[i] = pX;
    }
    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    sqlite3VtabLock(pVtab);
    rc = pModule->xUpdate(pVtab, nArg, apArg, &rowid);
    sqlite3VtabUnlock(db, pVtab);
    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
    if( pOp->p1 && rc==SQLITE_OK ){
      assert( nArg>1 && apArg[0] && (apArg[0]->flags&MEM_Null) );
      db->lastRowid = rowid;
    }
  }
  popStack(&pTos, nArg);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

/* An other opcode is illegal...
*/
default: {
  assert( 0 );
  break;
}

/*****************************************************************************
** The cases of the switch statement above this line should all be indented
** by 6 spaces.  But the left-most 6 spaces have been removed to improve the
** readability.  From this point on down, the normal indentation rules are
** restored.
*****************************************************************************/
    }

    /* Make sure the stack limit was not exceeded */
    assert( pTos<=pStackLimit );

#ifdef VDBE_PROFILE
    {
      long long elapse = hwtime() - start;
      pOp->cycles += elapse;
      pOp->cnt++;
#if 0
        fprintf(stdout, "%10lld ", elapse);
        sqlite3VdbePrintOp(stdout, origPc, &p->aOp[origPc]);
#endif
    }
#endif

    /* The following code adds nothing to the actual functionality
    ** of the program.  It is only here for testing and debugging.
    ** On the other hand, it does burn CPU cycles every time through
    ** the evaluator loop.  So we can leave it out when NDEBUG is defined.
    */
#ifndef NDEBUG
    /* Sanity checking on the top element of the stack. If the previous
    ** instruction was VNoChange, then the flags field of the top
    ** of the stack is set to 0. This is technically invalid for a memory
    ** cell, so avoid calling MemSanity() in this case.
    */
    if( pTos>=p->aStack && pTos->flags ){
      sqlite3VdbeMemSanity(pTos);
    }
    assert( pc>=-1 && pc<p->nOp );
#ifdef SQLITE_DEBUG
    /* Code for tracing the vdbe stack. */
    if( p->trace && pTos>=p->aStack ){
      int i;
      fprintf(p->trace, "Stack:");
      for(i=0; i>-5 && &pTos[i]>=p->aStack; i--){
        if( pTos[i].flags & MEM_Null ){
          fprintf(p->trace, " NULL");
        }else if( (pTos[i].flags & (MEM_Int|MEM_Str))==(MEM_Int|MEM_Str) ){
          fprintf(p->trace, " si:%lld", pTos[i].u.i);
        }else if( pTos[i].flags & MEM_Int ){
          fprintf(p->trace, " i:%lld", pTos[i].u.i);
        }else if( pTos[i].flags & MEM_Real ){
          fprintf(p->trace, " r:%g", pTos[i].r);
        }else{
          char zBuf[100];
          sqlite3VdbeMemPrettyPrint(&pTos[i], zBuf);
          fprintf(p->trace, " ");
          fprintf(p->trace, "%s", zBuf);
        }
      }
      if( rc!=0 ) fprintf(p->trace," rc=%d",rc);
      fprintf(p->trace,"\n");
    }
#endif  /* SQLITE_DEBUG */
#endif  /* NDEBUG */
  }  /* The end of the for(;;) loop the loops through opcodes */

  /* If we reach this point, it means that execution is finished.
  */
vdbe_halt:
  if( rc ){
    p->rc = rc;
    rc = SQLITE_ERROR;
  }else{
    rc = SQLITE_DONE;
  }
  sqlite3VdbeHalt(p);
  p->pTos = pTos;
  return rc;

  /* Jump to here if a malloc() fails.  It's hard to get a malloc()
  ** to fail on a modern VM computer, so this code is untested.
  */
no_mem:
  sqlite3SetString(&p->zErrMsg, "out of memory", (char*)0);
  rc = SQLITE_NOMEM;
  goto vdbe_halt;

  /* Jump to here for an SQLITE_MISUSE error.
  */
abort_due_to_misuse:
  rc = SQLITE_MISUSE;
  /* Fall thru into abort_due_to_error */

  /* Jump to here for any other kind of fatal error.  The "rc" variable
  ** should hold the error number.
  */
abort_due_to_error:
  if( p->zErrMsg==0 ){
    if( sqlite3MallocFailed() ) rc = SQLITE_NOMEM;
    sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(rc), (char*)0);
  }
  goto vdbe_halt;

  /* Jump to here if the sqlite3_interrupt() API sets the interrupt
  ** flag.
  */
abort_due_to_interrupt:
  assert( db->u1.isInterrupted );
  if( db->magic!=SQLITE_MAGIC_BUSY ){
    rc = SQLITE_MISUSE;
  }else{
    rc = SQLITE_INTERRUPT;
  }
  p->rc = rc;
  sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(rc), (char*)0);
  goto vdbe_halt;
}
