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
** and 5 operands.  Operands P1, P2, and P3 are integers.  Operand P4 
** is a null-terminated string.  Operand P5 is an unsigned character.
** Few opcodes use all 5 operands.
**
** Computation results are stored on a set of registers numbered beginning
** with 1 and going up to Vdbe.nMem.  Each register can store
** either an integer, a null-terminated string, a floating point
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
** The next global variable records the size of the largest MEM_Blob
** or MEM_Str that has been used by a VDBE opcode.  The test procedures
** use this information to make sure that the zero-blob functionality
** is working correctly.   This variable has no function other than to
** help verify the correct operation of the library.
*/
#ifdef SQLITE_TEST
int sqlite3_max_blobsize = 0;
static void updateMaxBlobsize(Mem *p){
  if( (p->flags & (MEM_Str|MEM_Blob))!=0 && p->n>sqlite3_max_blobsize ){
    sqlite3_max_blobsize = p->n;
  }
}
#endif

/*
** Test a register to see if it exceeds the current maximum blob size.
** If it does, record the new maximum blob size.
*/
#ifdef SQLITE_TEST
# define UPDATE_MAX_BLOBSIZE(P)  updateMaxBlobsize(P)
#else
# define UPDATE_MAX_BLOBSIZE(P)
#endif

/*
** Release the memory associated with a register.  This
** leaves the Mem.flags field in an inconsistent state.
*/
#define Release(P) if((P)->flags&MEM_Dyn){ sqlite3VdbeMemRelease(P); }

/*
** Convert the given register into a string if it isn't one
** already. Return non-zero if a malloc() fails.
*/
#define Stringify(P, enc) \
   if(((P)->flags&(MEM_Str|MEM_Blob))==0 && sqlite3VdbeMemStringify(P,enc)) \
     { goto no_mem; }

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
** is responsible for deallocating that string.  Because the register
** does not control the string, it might be deleted without the register
** knowing it.
**
** This routine converts an ephemeral string into a dynamically allocated
** string that the register itself controls.  In other words, it
** converts an MEM_Ephem string into an MEM_Dyn string.
*/
#define Deephemeralize(P) \
   if( ((P)->flags&MEM_Ephem)!=0 \
       && sqlite3VdbeMemMakeWriteable(P) ){ goto no_mem;}

/*
** Call sqlite3VdbeMemExpandBlob() on the supplied value (type Mem*)
** P if required.
*/
#define ExpandBlob(P) (((P)->flags&MEM_Zero)?sqlite3VdbeMemExpandBlob(P):0)

/*
** Argument pMem points at a regiser that will be passed to a
** user-defined function or returned to the user as the result of a query.
** The second argument, 'db_enc' is the text encoding used by the vdbe for
** register variables.  This routine sets the pMem->enc and pMem->type
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
** Properties of opcodes.  The OPFLG_INITIALIZER macro is
** created by mkopcodeh.awk during compilation.  Data is obtained
** from the comments following the "case OP_xxxx:" statements in
** this file.  
*/
static unsigned char opcodeProperty[] = OPFLG_INITIALIZER;

/*
** Return true if an opcode has any of the OPFLG_xxx properties
** specified by mask.
*/
int sqlite3VdbeOpcodeHasProperty(int opcode, int mask){
  assert( opcode>0 && opcode<sizeof(opcodeProperty) );
  return (opcodeProperty[opcode]&mask)!=0;
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
  p->apCsr[iCur] = pCx = sqlite3MallocZero( sizeof(Cursor) );
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
      if( !realnum && sqlite3Atoi64(pRec->z, &value) ){
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
static void applyAffinity(
  Mem *pRec,          /* The value to apply affinity to */
  char affinity,      /* The affinity to be applied */
  u8 enc              /* Use this text encoding */
){
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
void sqlite3ValueApplyAffinity(
  sqlite3_value *pVal, 
  u8 affinity, 
  u8 enc
){
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

    sqlite3_snprintf(100, zCsr, "%c", c);
    zCsr += strlen(zCsr);
    sqlite3_snprintf(100, zCsr, "%d[", pMem->n);
    zCsr += strlen(zCsr);
    for(i=0; i<16 && i<pMem->n; i++){
      sqlite3_snprintf(100, zCsr, "%02X", ((int)pMem->z[i] & 0xFF));
      zCsr += strlen(zCsr);
    }
    for(i=0; i<16 && i<pMem->n; i++){
      char z = pMem->z[i];
      if( z<32 || z>126 ) *zCsr++ = '.';
      else *zCsr++ = z;
    }

    sqlite3_snprintf(100, zCsr, "]%s", encnames[pMem->enc]);
    zCsr += strlen(zCsr);
    if( f & MEM_Zero ){
      sqlite3_snprintf(100, zCsr,"+%lldz",pMem->u.i);
      zCsr += strlen(zCsr);
    }
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
    sqlite3_snprintf(100, &zBuf[k], "%d", pMem->n);
    k += strlen(&zBuf[k]);
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
    sqlite3_snprintf(100,&zBuf[k], encnames[pMem->enc]);
    k += strlen(&zBuf[k]);
    zBuf[k++] = 0;
  }
}
#endif

#ifdef SQLITE_DEBUG
/*
** Print the value of a register for tracing purposes:
*/
static void memTracePrint(FILE *out, Mem *p){
  if( p->flags & MEM_Null ){
    fprintf(out, " NULL");
  }else if( (p->flags & (MEM_Int|MEM_Str))==(MEM_Int|MEM_Str) ){
    fprintf(out, " si:%lld", p->u.i);
  }else if( p->flags & MEM_Int ){
    fprintf(out, " i:%lld", p->u.i);
  }else if( p->flags & MEM_Real ){
    fprintf(out, " r:%g", p->r);
  }else{
    char zBuf[200];
    sqlite3VdbeMemPrettyPrint(p, zBuf);
    fprintf(out, " ");
    fprintf(out, "%s", zBuf);
  }
}
static void registerTrace(FILE *out, int iReg, Mem *p){
  fprintf(out, "REG[%d] = ", iReg);
  memTracePrint(out, p);
  fprintf(out, "\n");
}
#endif

#ifdef SQLITE_DEBUG
#  define REGISTER_TRACE(R,M) if(p->trace&&R>0)registerTrace(p->trace,R,M)
#else
#  define REGISTER_TRACE(R,M)
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
** from sqlite3_malloc() and p->zErrMsg is made to point to that memory.
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
  Mem *pIn1, *pIn2, *pIn3;   /* Input operands */
  Mem *pOut;                 /* Output operand */
  u8 opProperty;
#ifdef VDBE_PROFILE
  unsigned long long start;  /* CPU clock count at start of opcode */
  int origPc;                /* Program counter at start of opcode */
#endif
#ifndef SQLITE_OMIT_PROGRESS_CALLBACK
  int nProgressOps = 0;      /* Opcodes executed since progress callback. */
#endif

  assert( p->magic==VDBE_MAGIC_RUN );  /* sqlite3_step() verifies this */
  assert( db->magic==SQLITE_MAGIC_BUSY );
  sqlite3BtreeMutexArrayEnter(&p->aMutex);
  if( p->rc==SQLITE_NOMEM ){
    /* This happens if a malloc() inside a call to sqlite3_column_text() or
    ** sqlite3_column_text16() failed.  */
    goto no_mem;
  }
  assert( p->rc==SQLITE_OK || p->rc==SQLITE_BUSY );
  p->rc = SQLITE_OK;
  assert( p->explain==0 );
  p->pResultSet = 0;
  db->busyHandler.nBusy = 0;
  CHECK_FOR_INTERRUPT;
  sqlite3VdbeIOTraceSql(p);
#ifdef SQLITE_DEBUG
  if( p->pc==0 && ((p->db->flags & SQLITE_VdbeListing)!=0
    || sqlite3OsAccess(db->pVfs, "vdbe_explain", SQLITE_ACCESS_EXISTS))
  ){
    int i;
    printf("VDBE Program Listing:\n");
    sqlite3VdbePrintSql(p);
    for(i=0; i<p->nOp; i++){
      sqlite3VdbePrintOp(stdout, i, &p->aOp[i]);
    }
  }
  if( sqlite3OsAccess(db->pVfs, "vdbe_trace", SQLITE_ACCESS_EXISTS) ){
    p->trace = stdout;
  }
#endif
  for(pc=p->pc; rc==SQLITE_OK; pc++){
    assert( pc>=0 && pc<p->nOp );
    if( db->mallocFailed ) goto no_mem;
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
    if( p->trace==0 && pc==0 
     && sqlite3OsAccess(db->pVfs, "vdbe_sqltrace", SQLITE_ACCESS_EXISTS) ){
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
        int prc;
        if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
        prc =db->xProgress(db->pProgressArg);
        if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
        if( prc!=0 ){
          rc = SQLITE_INTERRUPT;
          goto vdbe_error_halt;
        }
        nProgressOps = 0;
      }
      nProgressOps++;
    }
#endif

    /* Do common setup processing for any opcode that is marked
    ** with the "out2-prerelease" tag.  Such opcodes have a single
    ** output which is specified by the P2 parameter.  The P2 register
    ** is initialized to a NULL.
    */
    opProperty = opcodeProperty[pOp->opcode];
    if( (opProperty & OPFLG_OUT2_PRERELEASE)!=0 ){
      assert( pOp->p2>0 );
      assert( pOp->p2<=p->nMem );
      pOut = &p->aMem[pOp->p2];
      sqlite3VdbeMemRelease(pOut);
      pOut->flags = MEM_Null;
    }else
 
    /* Do common setup for opcodes marked with one of the following
    ** combinations of properties.
    **
    **           in1
    **           in1 in2
    **           in1 in2 out3
    **           in1 in3
    **
    ** Variables pIn1, pIn2, and pIn3 are made to point to appropriate
    ** registers for inputs.  Variable pOut points to the output register.
    */
    if( (opProperty & OPFLG_IN1)!=0 ){
      assert( pOp->p1>0 );
      assert( pOp->p1<=p->nMem );
      pIn1 = &p->aMem[pOp->p1];
      REGISTER_TRACE(pOp->p1, pIn1);
      if( (opProperty & OPFLG_IN2)!=0 ){
        assert( pOp->p2>0 );
        assert( pOp->p2<=p->nMem );
        pIn2 = &p->aMem[pOp->p2];
        REGISTER_TRACE(pOp->p2, pIn2);
        if( (opProperty & OPFLG_OUT3)!=0 ){
          assert( pOp->p3>0 );
          assert( pOp->p3<=p->nMem );
          pOut = &p->aMem[pOp->p3];
        }
      }else if( (opProperty & OPFLG_IN3)!=0 ){
        assert( pOp->p3>0 );
        assert( pOp->p3<=p->nMem );
        pIn3 = &p->aMem[pOp->p3];
        REGISTER_TRACE(pOp->p3, pIn3);
      }
    }else if( (opProperty & OPFLG_IN2)!=0 ){
      assert( pOp->p2>0 );
      assert( pOp->p2<=p->nMem );
      pIn2 = &p->aMem[pOp->p2];
      REGISTER_TRACE(pOp->p2, pIn2);
    }else if( (opProperty & OPFLG_IN3)!=0 ){
      assert( pOp->p3>0 );
      assert( pOp->p3<=p->nMem );
      pIn3 = &p->aMem[pOp->p3];
      REGISTER_TRACE(pOp->p3, pIn3);
    }

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
** Other keywords in the comment that follows each case are used to
** construct the OPFLG_INITIALIZER value that initializes opcodeProperty[].
** Keywords include: in1, in2, in3, out2_prerelease, out2, out3.  See
** the mkopcodeh.awk script for additional information.
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

/* Opcode:  Goto * P2 * * *
**
** An unconditional jump to address P2.
** The next instruction executed will be 
** the one at index P2 from the beginning of
** the program.
*/
case OP_Goto: {             /* jump */
  CHECK_FOR_INTERRUPT;
  pc = pOp->p2 - 1;
  break;
}

/* Opcode:  Gosub * P2 * * *
**
** Push the current address plus 1 onto the return address stack
** and then jump to address P2.
**
** The return address stack is of limited depth.  If too many
** OP_Gosub operations occur without intervening OP_Returns, then
** the return address stack will fill up and processing will abort
** with a fatal error.
*/
case OP_Gosub: {            /* jump */
  assert( p->returnDepth<sizeof(p->returnStack)/sizeof(p->returnStack[0]) );
  p->returnStack[p->returnDepth++] = pc+1;
  pc = pOp->p2 - 1;
  break;
}

/* Opcode:  Return * * * * *
**
** Jump immediately to the next instruction after the last unreturned
** OP_Gosub.  If an OP_Return has occurred for all OP_Gosubs, then
** processing aborts with a fatal error.
*/
case OP_Return: {
  assert( p->returnDepth>0 );
  p->returnDepth--;
  pc = p->returnStack[p->returnDepth] - 1;
  break;
}

/* Opcode:  Halt P1 P2 * P4 *
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
** If P4 is not null then it is an error message string.
**
** There is an implied "Halt 0 0 0" instruction inserted at the very end of
** every program.  So a jump past the last instruction of the program
** is the same as executing Halt.
*/
case OP_Halt: {
  p->rc = pOp->p1;
  p->pc = pc;
  p->errorAction = pOp->p2;
  if( pOp->p4.z ){
    sqlite3SetString(&p->zErrMsg, pOp->p4.z, (char*)0);
  }
  rc = sqlite3VdbeHalt(p);
  assert( rc==SQLITE_BUSY || rc==SQLITE_OK );
  if( rc==SQLITE_BUSY ){
    p->rc = rc = SQLITE_BUSY;
  }else{
    rc = p->rc ? SQLITE_ERROR : SQLITE_DONE;
  }
  goto vdbe_return;
}

/* Opcode: Integer P1 P2 * * *
**
** The 32-bit integer value P1 is written into register P2.
*/
case OP_Integer: {         /* out2-prerelease */
  pOut->flags = MEM_Int;
  pOut->u.i = pOp->p1;
  break;
}

/* Opcode: Int64 * P2 * P4 *
**
** P4 is a pointer to a 64-bit integer value.
** Write that value into register P2.
*/
case OP_Int64: {           /* out2-prerelease */
  assert( pOp->p4.pI64!=0 );
  pOut->flags = MEM_Int;
  pOut->u.i = *pOp->p4.pI64;
  break;
}

/* Opcode: Real * P2 * P4 *
**
** P4 is a pointer to a 64-bit floating point value.
** Write that value into register P2.
*/
case OP_Real: {            /* same as TK_FLOAT, out2-prerelease */
  pOut->flags = MEM_Real;
  pOut->r = *pOp->p4.pReal;
  break;
}

/* Opcode: String8 * P2 * P4 *
**
** P4 points to a nul terminated UTF-8 string. This opcode is transformed 
** into an OP_String before it is executed for the first time.
*/
case OP_String8: {         /* same as TK_STRING, out2-prerelease */
  assert( pOp->p4.z!=0 );
  pOp->opcode = OP_String;
  pOp->p1 = strlen(pOp->p4.z);

#ifndef SQLITE_OMIT_UTF16
  if( encoding!=SQLITE_UTF8 ){
    sqlite3VdbeMemSetStr(pOut, pOp->p4.z, -1, SQLITE_UTF8, SQLITE_STATIC);
    if( SQLITE_OK!=sqlite3VdbeChangeEncoding(pOut, encoding) ) goto no_mem;
    if( SQLITE_OK!=sqlite3VdbeMemDynamicify(pOut) ) goto no_mem;
    pOut->flags &= ~(MEM_Dyn);
    pOut->flags |= MEM_Static;
    if( pOp->p4type==P4_DYNAMIC ){
      sqlite3_free(pOp->p4.z);
    }
    pOp->p4type = P4_DYNAMIC;
    pOp->p4.z = pOut->z;
    pOp->p1 = pOut->n;
    if( pOp->p1>SQLITE_MAX_LENGTH ){
      goto too_big;
    }
    UPDATE_MAX_BLOBSIZE(pOut);
    break;
  }
#endif
  if( pOp->p1>SQLITE_MAX_LENGTH ){
    goto too_big;
  }
  /* Fall through to the next case, OP_String */
}
  
/* Opcode: String P1 P2 * P4 *
**
** The string value P4 of length P1 (bytes) is stored in register P2.
*/
case OP_String: {          /* out2-prerelease */
  assert( pOp->p4.z!=0 );
  pOut->flags = MEM_Str|MEM_Static|MEM_Term;
  pOut->z = pOp->p4.z;
  pOut->n = pOp->p1;
  pOut->enc = encoding;
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: Null * P2 * * *
**
** Write a NULL into register P2.
*/
case OP_Null: {           /* out2-prerelease */
  break;
}


#ifndef SQLITE_OMIT_BLOB_LITERAL
/* Opcode: Blob P1 P2 * P4
**
** P4 points to a blob of data P1 bytes long.  Store this
** blob in register P2. This instruction is not coded directly
** by the compiler. Instead, the compiler layer specifies
** an OP_HexBlob opcode, with the hex string representation of
** the blob as P4. This opcode is transformed to an OP_Blob
** the first time it is executed.
*/
case OP_Blob: {                /* out2-prerelease */
  assert( pOp->p1 <= SQLITE_MAX_LENGTH );
  sqlite3VdbeMemSetStr(pOut, pOp->p4.z, pOp->p1, 0, 0);
  pOut->enc = encoding;
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}
#endif /* SQLITE_OMIT_BLOB_LITERAL */

/* Opcode: Variable P1 P2 * * *
**
** The value of variable P1 is written into register P2. A variable is
** an unknown in the original SQL string as handed to sqlite3_compile().
** Any occurance of the '?' character in the original SQL is considered
** a variable.  Variables in the SQL string are number from left to
** right beginning with 1.  The values of variables are set using the
** sqlite3_bind() API.
*/
case OP_Variable: {           /* out2-prerelease */
  int j = pOp->p1 - 1;
  Mem *pVar;
  assert( j>=0 && j<p->nVar );

  pVar = &p->aVar[j];
  if( sqlite3VdbeMemTooBig(pVar) ){
    goto too_big;
  }
  sqlite3VdbeMemShallowCopy(pOut, &p->aVar[j], MEM_Static);
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: Move P1 P2 * * *
**
** Move the value in register P1 over into register P2.  Register P1
** is left holding a NULL.  It is an error for P1 and P2 to be the
** same register.
*/
/* Opcode: Copy P1 P2 * * *
**
** Make a copy of register P1 into register P2.
**
** This instruction makes a deep copy of the value.  A duplicate
** is made of any string or blob constant.  See also OP_SCopy.
*/
/* Opcode: SCopy P1 P2 * * *
**
** Make a shallow copy of register P1 into register P2.
**
** This instruction makes a shallow copy of the value.  If the value
** is a string or blob, then the copy is only a pointer to the
** original and hence if the original changes so will the copy.
** Worse, if the original is deallocated, the copy becomes invalid.
** Thus the program must guarantee that the original will not change
** during the lifetime of the copy.  Use OP_Copy to make a complete
** copy.
*/
case OP_Move:
case OP_Copy:
case OP_SCopy: {
  assert( pOp->p1>0 );
  assert( pOp->p1<=p->nMem );
  pIn1 = &p->aMem[pOp->p1];
  REGISTER_TRACE(pOp->p1, pIn1);
  assert( pOp->p2>0 );
  assert( pOp->p2<=p->nMem );
  pOut = &p->aMem[pOp->p2];
  assert( pOut!=pIn1 );
  if( pOp->opcode==OP_Move ){
    sqlite3VdbeMemMove(pOut, pIn1);
  }else{
    sqlite3VdbeMemShallowCopy(pOut, pIn1, MEM_Ephem);
    if( pOp->opcode==OP_Copy ){
      Deephemeralize(pOut);
    }
  }
  REGISTER_TRACE(pOp->p2, pOut);
  break;
}

/* Opcode: ResultRow P1 P2 * * *
**
** The registers P1 throught P1+P2-1 contain a single row of
** results. This opcode causes the sqlite3_step() call to terminate
** with an SQLITE_ROW return code and it sets up the sqlite3_stmt
** structure to provide access to the top P1 values as the result
** row.
*/
case OP_ResultRow: {
  Mem *pMem;
  int i;
  assert( p->nResColumn==pOp->p2 );
  assert( pOp->p1>0 );
  assert( pOp->p1+pOp->p2<=p->nMem );

  /* Invalidate all ephemeral cursor row caches */
  p->cacheCtr = (p->cacheCtr + 2)|1;

  /* Make sure the results of the current row are \000 terminated
  ** and have an assigned type.  The results are deephemeralized as
  ** as side effect.
  */
  pMem = p->pResultSet = &p->aMem[pOp->p1];
  for(i=0; i<pOp->p2; i++){
    sqlite3VdbeMemNulTerminate(&pMem[i]);
    storeTypeInfo(&pMem[i], encoding);
  }

  /* Return SQLITE_ROW
  */
  p->nCallback++;
  p->pc = pc + 1;
  rc = SQLITE_ROW;
  goto vdbe_return;
}

/* Opcode: Concat P1 P2 P3 * *
**
** Add the text in register P1 onto the end of the text in
** register P2 and store the result in register P3.
** If either the P1 or P2 text are NULL then store NULL in P3.
*/
case OP_Concat: {           /* same as TK_CONCAT, in1, in2, out3 */
  char *zNew;
  i64 nByte;

  if( (pIn1->flags | pIn2->flags) & MEM_Null ){
    Release(pOut);
    pOut->flags = MEM_Null;
    break;
  }
  ExpandBlob(pIn1);
  Stringify(pIn1, encoding);
  ExpandBlob(pIn2);
  Stringify(pIn2, encoding);
  nByte = pIn1->n + pIn2->n;
  if( nByte>SQLITE_MAX_LENGTH ){
    goto too_big;
  }
  zNew = sqlite3DbMallocRaw(db, nByte+2);
  if( zNew==0 ){
    goto no_mem;
  }
  memcpy(zNew, pIn2->z, pIn2->n);
  memcpy(&zNew[pIn2->n], pIn1->z, pIn1->n);
  zNew[nByte] = 0;
  zNew[nByte+1] = 0;
  Release(pOut);
  pOut->n = nByte;
  pOut->flags = MEM_Str|MEM_Dyn|MEM_Term;
  pOut->xDel = 0;
  pOut->enc = encoding;
  pOut->z = zNew;
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: Add P1 P2 P3 * *
**
** Add the value in register P1 to the value in register P2
** and store the result in regiser P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: Multiply P1 P2 P3 * *
**
**
** Multiply the value in regiser P1 by the value in regiser P2
** and store the result in register P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: Subtract P1 P2 P3 * *
**
** Subtract the value in register P1 from the value in register P2
** and store the result in register P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: Divide P1 P2 P3 * *
**
** Divide the value in register P1 by the value in register P2
** and store the result in register P3.  If the value in register P2
** is zero, then the result is NULL.
** If either input is NULL, the result is NULL.
*/
/* Opcode: Remainder P1 P2 P3 * *
**
** Compute the remainder after integer division of the value in
** register P1 by the value in register P2 and store the result in P3. 
** If the value in register P2 is zero the result is NULL.
** If either operand is NULL, the result is NULL.
*/
case OP_Add:                   /* same as TK_PLUS, in1, in2, out3 */
case OP_Subtract:              /* same as TK_MINUS, in1, in2, out3 */
case OP_Multiply:              /* same as TK_STAR, in1, in2, out3 */
case OP_Divide:                /* same as TK_SLASH, in1, in2, out3 */
case OP_Remainder: {           /* same as TK_REM, in1, in2, out3 */
  int flags;
  flags = pIn1->flags | pIn2->flags;
  if( (flags & MEM_Null)!=0 ) goto arithmetic_result_is_null;
  if( (pIn1->flags & pIn2->flags & MEM_Int)==MEM_Int ){
    i64 a, b;
    a = pIn1->u.i;
    b = pIn2->u.i;
    switch( pOp->opcode ){
      case OP_Add:         b += a;       break;
      case OP_Subtract:    b -= a;       break;
      case OP_Multiply:    b *= a;       break;
      case OP_Divide: {
        if( a==0 ) goto arithmetic_result_is_null;
        /* Dividing the largest possible negative 64-bit integer (1<<63) by 
        ** -1 returns an integer to large to store in a 64-bit data-type. On
        ** some architectures, the value overflows to (1<<63). On others,
        ** a SIGFPE is issued. The following statement normalizes this
        ** behaviour so that all architectures behave as if integer 
        ** overflow occured.
        */
        if( a==-1 && b==(((i64)1)<<63) ) a = 1;
        b /= a;
        break;
      }
      default: {
        if( a==0 ) goto arithmetic_result_is_null;
        if( a==-1 ) a = 1;
        b %= a;
        break;
      }
    }
    Release(pOut);
    pOut->u.i = b;
    pOut->flags = MEM_Int;
  }else{
    double a, b;
    a = sqlite3VdbeRealValue(pIn1);
    b = sqlite3VdbeRealValue(pIn2);
    switch( pOp->opcode ){
      case OP_Add:         b += a;       break;
      case OP_Subtract:    b -= a;       break;
      case OP_Multiply:    b *= a;       break;
      case OP_Divide: {
        if( a==0.0 ) goto arithmetic_result_is_null;
        b /= a;
        break;
      }
      default: {
        i64 ia = (i64)a;
        i64 ib = (i64)b;
        if( ia==0 ) goto arithmetic_result_is_null;
        if( ia==-1 ) ia = 1;
        b = ib % ia;
        break;
      }
    }
    if( sqlite3_isnan(b) ){
      goto arithmetic_result_is_null;
    }
    Release(pOut);
    pOut->r = b;
    pOut->flags = MEM_Real;
    if( (flags & MEM_Real)==0 ){
      sqlite3VdbeIntegerAffinity(pOut);
    }
  }
  break;

arithmetic_result_is_null:
  sqlite3VdbeMemSetNull(pOut);
  break;
}

/* Opcode: CollSeq * * P4
**
** P4 is a pointer to a CollSeq struct. If the next call to a user function
** or aggregate calls sqlite3GetFuncCollSeq(), this collation sequence will
** be returned. This is used by the built-in min(), max() and nullif()
** functions.
**
** The interface used by the implementation of the aforementioned functions
** to retrieve the collation sequence set by this opcode is not available
** publicly, only to user functions defined in func.c.
*/
case OP_CollSeq: {
  assert( pOp->p4type==P4_COLLSEQ );
  break;
}

/* Opcode: Function P1 P2 P3 P4 P5
**
** Invoke a user function (P4 is a pointer to a Function structure that
** defines the function) with P5 arguments taken from register P2 and
** successors.  The result of the function is stored in register P3.
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
  int n = pOp->p5;

  apVal = p->apArg;
  assert( apVal || n==0 );

  assert( n==0 || (pOp->p2>0 && pOp->p2+n<=p->nMem) );
  pArg = &p->aMem[pOp->p2];
  for(i=0; i<n; i++, pArg++){
    apVal[i] = pArg;
    storeTypeInfo(pArg, encoding);
    REGISTER_TRACE(pOp->p2, pArg);
  }

  assert( pOp->p4type==P4_FUNCDEF || pOp->p4type==P4_VDBEFUNC );
  if( pOp->p4type==P4_FUNCDEF ){
    ctx.pFunc = pOp->p4.pFunc;
    ctx.pVdbeFunc = 0;
  }else{
    ctx.pVdbeFunc = (VdbeFunc*)pOp->p4.pVdbeFunc;
    ctx.pFunc = ctx.pVdbeFunc->pFunc;
  }

  ctx.s.flags = MEM_Null;
  ctx.s.z = 0;
  ctx.s.xDel = 0;
  ctx.s.db = db;
  ctx.isError = 0;
  if( ctx.pFunc->needCollSeq ){
    assert( pOp>p->aOp );
    assert( pOp[-1].p4type==P4_COLLSEQ );
    assert( pOp[-1].opcode==OP_CollSeq );
    ctx.pColl = pOp[-1].p4.pColl;
  }
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  (*ctx.pFunc->xFunc)(&ctx, n, apVal);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  if( db->mallocFailed ){
    /* Even though a malloc() has failed, the implementation of the
    ** user function may have called an sqlite3_result_XXX() function
    ** to return a value. The following call releases any resources
    ** associated with such a value.
    **
    ** Note: Maybe MemRelease() should be called if sqlite3SafetyOn()
    ** fails also (the if(...) statement above). But if people are
    ** misusing sqlite, they have bigger problems than a leaked value.
    */
    sqlite3VdbeMemRelease(&ctx.s);
    goto no_mem;
  }

  /* If any auxilary data functions have been called by this user function,
  ** immediately call the destructor for any non-static values.
  */
  if( ctx.pVdbeFunc ){
    sqlite3VdbeDeleteAuxData(ctx.pVdbeFunc, pOp->p1);
    pOp->p4.pVdbeFunc = ctx.pVdbeFunc;
    pOp->p4type = P4_VDBEFUNC;
  }

  /* If the function returned an error, throw an exception */
  if( ctx.isError ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(&ctx.s), (char*)0);
    rc = ctx.isError;
  }

  /* Copy the result of the function into register P3 */
  sqlite3VdbeChangeEncoding(&ctx.s, encoding);
  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  pOut = &p->aMem[pOp->p3];
  sqlite3VdbeMemMove(pOut, &ctx.s);
  if( sqlite3VdbeMemTooBig(pOut) ){
    goto too_big;
  }
  REGISTER_TRACE(pOp->p3, pOut);
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: BitAnd P1 P2 P3 * *
**
** Take the bit-wise AND of the values in register P1 and P2 and
** store the result in register P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: BitOr P1 P2 P3 * *
**
** Take the bit-wise OR of the values in register P1 and P2 and
** store the result in register P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: ShiftLeft P1 P2 P3 * *
**
** Shift the integer value in register P2 to the left by the
** number of bits specified by the integer in regiser P1.
** Store the result in register P3.
** If either input is NULL, the result is NULL.
*/
/* Opcode: ShiftRight P1 P2 P3 * *
**
** Shift the integer value in register P2 to the right by the
** number of bits specified by the integer in register P1.
** Store the result in register P3.
** If either input is NULL, the result is NULL.
*/
case OP_BitAnd:                 /* same as TK_BITAND, in1, in2, out3 */
case OP_BitOr:                  /* same as TK_BITOR, in1, in2, out3 */
case OP_ShiftLeft:              /* same as TK_LSHIFT, in1, in2, out3 */
case OP_ShiftRight: {           /* same as TK_RSHIFT, in1, in2, out3 */
  i64 a, b;

  if( (pIn1->flags | pIn2->flags) & MEM_Null ){
    sqlite3VdbeMemSetNull(pOut);
    break;
  }
  a = sqlite3VdbeIntValue(pIn2);
  b = sqlite3VdbeIntValue(pIn1);
  switch( pOp->opcode ){
    case OP_BitAnd:      a &= b;     break;
    case OP_BitOr:       a |= b;     break;
    case OP_ShiftLeft:   a <<= b;    break;
    default:  assert( pOp->opcode==OP_ShiftRight );
                         a >>= b;    break;
  }
  Release(pOut);
  pOut->u.i = a;
  pOut->flags = MEM_Int;
  break;
}

/* Opcode: AddImm  P1 P2 * * *
** 
** Add the constant P2 the value in register P1.
** The result is always an integer.
**
** To force any register to be an integer, just add 0.
*/
case OP_AddImm: {            /* in1 */
  sqlite3VdbeMemIntegerify(pIn1);
  pIn1->u.i += pOp->p2;
  break;
}

/* Opcode: ForceInt P1 P2 P3 * *
**
** Convert value in register P1 into an integer.  If the value 
** in P1 is not numeric (meaning that is is a NULL or a string that
** does not look like an integer or floating point number) then
** jump to P2.  If the value in P1 is numeric then
** convert it into the least integer that is greater than or equal to its
** current value if P3==0, or to the least integer that is strictly
** greater than its current value if P3==1.
*/
case OP_ForceInt: {            /* jump, in1 */
  i64 v;
  applyAffinity(pIn1, SQLITE_AFF_NUMERIC, encoding);
  if( (pIn1->flags & (MEM_Int|MEM_Real))==0 ){
    pc = pOp->p2 - 1;
    break;
  }
  if( pIn1->flags & MEM_Int ){
    v = pIn1->u.i + (pOp->p3!=0);
  }else{
    assert( pIn1->flags & MEM_Real );
    v = (sqlite3_int64)pIn1->r;
    if( pIn1->r>(double)v ) v++;
    if( pOp->p3 && pIn1->r==(double)v ) v++;
  }
  Release(pIn1);
  pIn1->u.i = v;
  pIn1->flags = MEM_Int;
  break;
}

/* Opcode: MustBeInt P1 P2 * * *
** 
** Force the value in register P1 to be an integer.  If the value
** in P1 is not an integer and cannot be converted into an integer
** without data loss, then jump immediately to P2, or if P2==0
** raise an SQLITE_MISMATCH exception.
*/
case OP_MustBeInt: {            /* jump, in1 */
  applyAffinity(pIn1, SQLITE_AFF_NUMERIC, encoding);
  if( (pIn1->flags & MEM_Int)==0 ){
    if( pOp->p2==0 ){
      rc = SQLITE_MISMATCH;
      goto abort_due_to_error;
    }else{
      pc = pOp->p2 - 1;
    }
  }else{
    Release(pIn1);
    pIn1->flags = MEM_Int;
  }
  break;
}

/* Opcode: RealAffinity P1 * * * *
**
** If register P1 holds an integer convert it to a real value.
**
** This opcode is used when extracting information from a column that
** has REAL affinity.  Such column values may still be stored as
** integers, for space efficiency, but after extraction we want them
** to have only a real value.
*/
case OP_RealAffinity: {                  /* in1 */
  if( pIn1->flags & MEM_Int ){
    sqlite3VdbeMemRealify(pIn1);
  }
  break;
}

#ifndef SQLITE_OMIT_CAST
/* Opcode: ToText P1 * * * *
**
** Force the value in register P1 to be text.
** If the value is numeric, convert it to a string using the
** equivalent of printf().  Blob values are unchanged and
** are afterwards simply interpreted as text.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToText: {                  /* same as TK_TO_TEXT, in1 */
  if( pIn1->flags & MEM_Null ) break;
  assert( MEM_Str==(MEM_Blob>>3) );
  pIn1->flags |= (pIn1->flags&MEM_Blob)>>3;
  applyAffinity(pIn1, SQLITE_AFF_TEXT, encoding);
  rc = ExpandBlob(pIn1);
  assert( pIn1->flags & MEM_Str );
  pIn1->flags &= ~(MEM_Int|MEM_Real|MEM_Blob);
  UPDATE_MAX_BLOBSIZE(pIn1);
  break;
}

/* Opcode: ToBlob P1 * * * *
**
** Force the value in register P1 to be a BLOB.
** If the value is numeric, convert it to a string first.
** Strings are simply reinterpreted as blobs with no change
** to the underlying data.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToBlob: {                  /* same as TK_TO_BLOB, in1 */
  if( pIn1->flags & MEM_Null ) break;
  if( (pIn1->flags & MEM_Blob)==0 ){
    applyAffinity(pIn1, SQLITE_AFF_TEXT, encoding);
    assert( pIn1->flags & MEM_Str );
    pIn1->flags |= MEM_Blob;
  }
  pIn1->flags &= ~(MEM_Int|MEM_Real|MEM_Str);
  UPDATE_MAX_BLOBSIZE(pIn1);
  break;
}

/* Opcode: ToNumeric P1 * * * *
**
** Force the value in register P1 to be numeric (either an
** integer or a floating-point number.)
** If the value is text or blob, try to convert it to an using the
** equivalent of atoi() or atof() and store 0 if no such conversion 
** is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToNumeric: {                  /* same as TK_TO_NUMERIC, in1 */
  if( (pIn1->flags & (MEM_Null|MEM_Int|MEM_Real))==0 ){
    sqlite3VdbeMemNumerify(pIn1);
  }
  break;
}
#endif /* SQLITE_OMIT_CAST */

/* Opcode: ToInt P1 * * * *
**
** Force the value in register P1 be an integer.  If
** The value is currently a real number, drop its fractional part.
** If the value is text or blob, try to convert it to an integer using the
** equivalent of atoi() and store 0 if no such conversion is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToInt: {                  /* same as TK_TO_INT, in1 */
  if( (pIn1->flags & MEM_Null)==0 ){
    sqlite3VdbeMemIntegerify(pIn1);
  }
  break;
}

#ifndef SQLITE_OMIT_CAST
/* Opcode: ToReal P1 * * * *
**
** Force the value in register P1 to be a floating point number.
** If The value is currently an integer, convert it.
** If the value is text or blob, try to convert it to an integer using the
** equivalent of atoi() and store 0.0 if no such conversion is possible.
**
** A NULL value is not changed by this routine.  It remains NULL.
*/
case OP_ToReal: {                  /* same as TK_TO_REAL, in1 */
  if( (pIn1->flags & MEM_Null)==0 ){
    sqlite3VdbeMemRealify(pIn1);
  }
  break;
}
#endif /* SQLITE_OMIT_CAST */

/* Opcode: Lt P1 P2 P3 P4 P5
**
** Compare the values in register P1 and P3.  If reg(P3)<reg(P1) then
** jump to address P2.  
**
** If the SQLITE_JUMPIFNULL bit of P5 is set and either reg(P1) or
** reg(P3) is NULL then take the jump.  If the SQLITE_JUMPIFNULL 
** bit is clear then fall thru if either operand is NULL.
**
** If the SQLITE_NULLEQUAL bit of P5 is set then treat NULL operands
** as being equal to one another.  Normally NULLs are not equal to 
** anything including other NULLs.
**
** The SQLITE_AFF_MASK portion of P5 must be an affinity character -
** SQLITE_AFF_TEXT, SQLITE_AFF_INTEGER, and so forth. An attempt is made 
** to coerce both inputs according to this affinity before the
** comparison is made. If the SQLITE_AFF_MASK is 0x00, then numeric
** affinity is used. Note that the affinity conversions are stored
** back into the input registers P1 and P3.  So this opcode can cause
** persistent changes to registers P1 and P3.
**
** Once any conversions have taken place, and neither value is NULL, 
** the values are compared. If both values are blobs then memcmp() is
** used to determine the results of the comparison.  If both values
** are text, then the appropriate collating function specified in
** P4 is  used to do the comparison.  If P4 is not specified then
** memcmp() is used to compare text string.  If both values are
** numeric, then a numeric comparison is used. If the two values
** are of different types, then numbers are considered less than
** strings and strings are considered less than blobs.
**
** If the SQLITE_STOREP2 bit of P5 is set, then do not jump.  Instead,
** store a boolean result (either 0, or 1, or NULL) in register P2.
*/
/* Opcode: Ne P1 P2 P3 P4 P5
**
** This works just like the Lt opcode except that the jump is taken if
** the operands in registers P1 and P3 are not equal.  See the Lt opcode for
** additional information.
*/
/* Opcode: Eq P1 P2 P3 P4 P5
**
** This works just like the Lt opcode except that the jump is taken if
** the operands in registers P1 and P3 are equal.
** See the Lt opcode for additional information.
*/
/* Opcode: Le P1 P2 P3 P4 P5
**
** This works just like the Lt opcode except that the jump is taken if
** the content of register P3 is less than or equal to the content of
** register P1.  See the Lt opcode for additional information.
*/
/* Opcode: Gt P1 P2 P3 P4 P5
**
** This works just like the Lt opcode except that the jump is taken if
** the content of register P3 is greater than the content of
** register P1.  See the Lt opcode for additional information.
*/
/* Opcode: Ge P1 P2 P3 P4 P5
**
** This works just like the Lt opcode except that the jump is taken if
** the content of register P3 is greater than or equal to the content of
** register P1.  See the Lt opcode for additional information.
*/
case OP_Eq:               /* same as TK_EQ, jump, in1, in3 */
case OP_Ne:               /* same as TK_NE, jump, in1, in3 */
case OP_Lt:               /* same as TK_LT, jump, in1, in3 */
case OP_Le:               /* same as TK_LE, jump, in1, in3 */
case OP_Gt:               /* same as TK_GT, jump, in1, in3 */
case OP_Ge: {             /* same as TK_GE, jump, in1, in3 */
  int flags;
  int res;
  char affinity;
  Mem x1, x3;

  flags = pIn1->flags|pIn3->flags;

  if( flags&MEM_Null ){
    if( (pOp->p5 & SQLITE_NULLEQUAL)!=0 ){
      /*
      ** When SQLITE_NULLEQUAL set and either operand is NULL
      ** then both operands are converted to integers prior to being 
      ** passed down into the normal comparison logic below.  
      ** NULL operands are converted to zero and non-NULL operands
      ** are converted to 1.  Thus, for example, with SQLITE_NULLEQUAL
      ** set,  NULL==NULL is true whereas it would normally NULL.
      ** Similarly,  NULL!=123 is true.
      */
      x1.flags = MEM_Int;
      x1.u.i = (pIn1->flags & MEM_Null)==0;
      pIn1 = &x1;
      x3.flags = MEM_Int;
      x3.u.i = (pIn3->flags & MEM_Null)==0;
      pIn3 = &x3;
    }else{
      /* If the SQLITE_NULLEQUAL bit is clear and either operand is NULL then
      ** the result is always NULL.  The jump is taken if the 
      ** SQLITE_JUMPIFNULL bit is set.
      */
      if( pOp->p5 & SQLITE_STOREP2 ){
        pOut = &p->aMem[pOp->p2];
        Release(pOut);
        pOut->flags = MEM_Null;
        REGISTER_TRACE(pOp->p2, pOut);
      }else if( pOp->p5 & SQLITE_JUMPIFNULL ){
        pc = pOp->p2-1;
      }
      break;
    }
  }

  affinity = pOp->p5 & SQLITE_AFF_MASK;
  if( affinity ){
    applyAffinity(pIn1, affinity, encoding);
    applyAffinity(pIn3, affinity, encoding);
  }

  assert( pOp->p4type==P4_COLLSEQ || pOp->p4.pColl==0 );
  ExpandBlob(pIn1);
  ExpandBlob(pIn3);
  res = sqlite3MemCompare(pIn3, pIn1, pOp->p4.pColl);
  switch( pOp->opcode ){
    case OP_Eq:    res = res==0;     break;
    case OP_Ne:    res = res!=0;     break;
    case OP_Lt:    res = res<0;      break;
    case OP_Le:    res = res<=0;     break;
    case OP_Gt:    res = res>0;      break;
    default:       res = res>=0;     break;
  }

  if( pOp->p5 & SQLITE_STOREP2 ){
    pOut = &p->aMem[pOp->p2];
    Release(pOut);
    pOut->flags = MEM_Int;
    pOut->u.i = res;
    REGISTER_TRACE(pOp->p2, pOut);
  }else if( res ){
    pc = pOp->p2-1;
  }
  break;
}

/* Opcode: And P1 P2 P3 * *
**
** Take the logical AND of the values in registers P1 and P2 and
** write the result into register P3.
**
** If either P1 or P2 is 0 (false) then the result is 0 even if
** the other input is NULL.  A NULL and true or two NULLs give
** a NULL output.
*/
/* Opcode: Or P1 P2 P3 * *
**
** Take the logical OR of the values in register P1 and P2 and
** store the answer in register P3.
**
** If either P1 or P2 is nonzero (true) then the result is 1 (true)
** even if the other input is NULL.  A NULL and false or two NULLs
** give a NULL output.
*/
case OP_And:              /* same as TK_AND, in1, in2, out3 */
case OP_Or: {             /* same as TK_OR, in1, in2, out3 */
  int v1, v2;    /* 0==FALSE, 1==TRUE, 2==UNKNOWN or NULL */

  if( pIn1->flags & MEM_Null ){
    v1 = 2;
  }else{
    v1 = sqlite3VdbeIntValue(pIn1)!=0;
  }
  if( pIn2->flags & MEM_Null ){
    v2 = 2;
  }else{
    v2 = sqlite3VdbeIntValue(pIn2)!=0;
  }
  if( pOp->opcode==OP_And ){
    static const unsigned char and_logic[] = { 0, 0, 0, 0, 1, 2, 0, 2, 2 };
    v1 = and_logic[v1*3+v2];
  }else{
    static const unsigned char or_logic[] = { 0, 1, 2, 1, 1, 1, 2, 1, 2 };
    v1 = or_logic[v1*3+v2];
  }
  Release(pOut);
  if( v1==2 ){
    pOut->flags = MEM_Null;
  }else{
    pOut->u.i = v1;
    pOut->flags = MEM_Int;
  }
  break;
}

/* Opcode: Not P1 * * * *
**
** Interpret the value in register P1 as a boolean value.  Replace it
** with its complement.  If the value in register P1 is NULL its value
** is unchanged.
*/
case OP_Not: {                /* same as TK_NOT, in1 */
  if( pIn1->flags & MEM_Null ) break;  /* Do nothing to NULLs */
  sqlite3VdbeMemIntegerify(pIn1);
  pIn1->u.i = !pIn1->u.i;
  assert( pIn1->flags==MEM_Int );
  break;
}

/* Opcode: BitNot P1 * * * *
**
** Interpret the content of register P1 as an integer.  Replace it
** with its ones-complement.  If the value is originally NULL, leave
** it unchanged.
*/
case OP_BitNot: {             /* same as TK_BITNOT, in1 */
  if( pIn1->flags & MEM_Null ) break;  /* Do nothing to NULLs */
  sqlite3VdbeMemIntegerify(pIn1);
  pIn1->u.i = ~pIn1->u.i;
  assert( pIn1->flags==MEM_Int );
  break;
}

/* Opcode: If P1 P2 P3 * *
**
** Jump to P2 if the value in register P1 is true.  The value is
** is considered true if it is numeric and non-zero.  If the value
** in P1 is NULL then take the jump if P3 is true.
*/
/* Opcode: IfNot P1 P2 P3 * *
**
** Jump to P2 if the value in register P1 is False.  The value is
** is considered true if it has a numeric value of zero.  If the value
** in P1 is NULL then take the jump if P3 is true.
*/
case OP_If:                 /* jump, in1 */
case OP_IfNot: {            /* jump, in1 */
  int c;
  if( pIn1->flags & MEM_Null ){
    c = pOp->p3;
  }else{
#ifdef SQLITE_OMIT_FLOATING_POINT
    c = sqlite3VdbeIntValue(pIn1);
#else
    c = sqlite3VdbeRealValue(pIn1)!=0.0;
#endif
    if( pOp->opcode==OP_IfNot ) c = !c;
  }
  if( c ){
    pc = pOp->p2-1;
  }
  break;
}

/* Opcode: IsNull P1 P2 P3 * *
**
** Jump to P2 if the value in register P1 is NULL.  If P3 is greater
** than zero, then check all values reg(P1), reg(P1+1), 
** reg(P1+2), ..., reg(P1+P3-1).
*/
case OP_IsNull: {            /* same as TK_ISNULL, jump, in1 */
  int n = pOp->p3;
  assert( pOp->p3==0 || pOp->p1>0 );
  do{
    if( (pIn1->flags & MEM_Null)!=0 ){
      pc = pOp->p2 - 1;
      break;
    }
    pIn1++;
  }while( --n > 0 );
  break;
}

/* Opcode: NotNull P1 P2 * * *
**
** Jump to P2 if the value in register P1 is not NULL.  
*/
case OP_NotNull: {            /* same as TK_NOTNULL, jump, in1 */
  if( (pIn1->flags & MEM_Null)==0 ){
    pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: SetNumColumns P1 P2 * * *
**
** Before the OP_Column opcode can be executed on a cursor, this
** opcode must be called to set the number of fields in the table.
**
** This opcode sets the number of columns for cursor P1 to P2.
**
** If OP_KeyAsData is to be applied to cursor P1, it must be executed
** before this op-code.
*/
case OP_SetNumColumns: {
  Cursor *pC;
  assert( (pOp->p1)<p->nCursor );
  assert( p->apCsr[pOp->p1]!=0 );
  pC = p->apCsr[pOp->p1];
  pC->nField = pOp->p2;
  break;
}

/* Opcode: Column P1 P2 P3 P4 *
**
** Interpret the data that cursor P1 points to as a structure built using
** the MakeRecord instruction.  (See the MakeRecord opcode for additional
** information about the format of the data.)  Extract the P2-th column
** from this record.  If there are less that (P2+1) 
** values in the record, extract a NULL.
**
** The value extracted is stored in register P3.
**
** If the KeyAsData opcode has previously executed on this cursor, then the
** field might be extracted from the key rather than the data.
**
** If the column contains fewer than P2 fields, then extract a NULL.  Or,
** if the P4 argument is a P4_MEM use the value of the P4 argument as
** the result.
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
  Mem *pDest;        /* Where to write the extracted value */
  Mem sMem;          /* For storing the record being decoded */

  sMem.flags = 0;
  assert( p1<p->nCursor );
  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  pDest = &p->aMem[pOp->p3];
  sqlite3VdbeMemSetNull(pDest);

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
  ** the number of columns is stored in the Cursor.nField element.
  */
  pC = p->apCsr[p1];
  assert( pC!=0 );
#ifndef SQLITE_OMIT_VIRTUALTABLE
  assert( pC->pVtabCursor==0 );
#endif
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
  }else{
    assert( pC->pseudoTable );
    /* The record is the sole entry of a pseudo-table */
    payloadSize = pC->nData;
    zRec = pC->pData;
    pC->cacheStatus = CACHE_STALE;
    assert( payloadSize==0 || zRec!=0 );
    nField = pC->nField;
    pCrsr = 0;
  }

  /* If payloadSize is 0, then just store a NULL */
  if( payloadSize==0 ){
    assert( pDest->flags==MEM_Null );
    goto op_column_out;
  }
  if( payloadSize>SQLITE_MAX_LENGTH ){
    goto too_big;
  }

  assert( p2<nField );

  /* Read and parse the table header.  Store the results of the parse
  ** into the record header cache fields of the cursor.
  */
  if( pC->cacheStatus==p->cacheCtr ){
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
      pC->aType = aType = sqlite3DbMallocRaw(db, 2*nField*sizeof(aType) );
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
        ** the record to 0. This tells code below to store a NULL
        ** instead of deserializing a value from the record.
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
  ** request.  In this case, set the value NULL or to P4 if P4 is
  ** a pointer to a Mem object.
  */
  if( aOffset[p2] ){
    assert( rc==SQLITE_OK );
    if( zRec ){
      zData = &zRec[aOffset[p2]];
    }else{
      len = sqlite3VdbeSerialTypeLen(aType[p2]);
      rc = sqlite3VdbeMemFromBtree(pCrsr, aOffset[p2], len, pC->isIndex, &sMem);
      if( rc!=SQLITE_OK ){
        goto op_column_out;
      }
      zData = sMem.z;
    }
    sqlite3VdbeSerialGet((u8*)zData, aType[p2], pDest);
    pDest->enc = encoding;
  }else{
    if( pOp->p4type==P4_MEM ){
      sqlite3VdbeMemShallowCopy(pDest, pOp->p4.pMem, MEM_Static);
    }else{
      assert( pDest->flags==MEM_Null );
    }
  }

  /* If we dynamically allocated space to hold the data (in the
  ** sqlite3VdbeMemFromBtree() call above) then transfer control of that
  ** dynamically allocated space over to the pDest structure.
  ** This prevents a memory copy.
  */
  if( (sMem.flags & MEM_Dyn)!=0 ){
    assert( pDest->flags & MEM_Ephem );
    assert( pDest->flags & (MEM_Str|MEM_Blob) );
    assert( pDest->z==sMem.z );
    assert( sMem.flags & MEM_Term );
    pDest->flags &= ~MEM_Ephem;
    pDest->flags |= MEM_Dyn|MEM_Term;
  }

  /* pDest->z might be pointing to sMem.zShort[].  Fix that so that we
  ** can abandon sMem */
  rc = sqlite3VdbeMemMakeWriteable(pDest);

op_column_out:
  UPDATE_MAX_BLOBSIZE(pDest);
  REGISTER_TRACE(pOp->p3, pDest);
  break;
}

/* Opcode: MakeRecord P1 P2 P3 P4 *
**
** Convert P2 registers beginning with P1 into a single entry
** suitable for use as a data record in a database table or as a key
** in an index.  The details of the format are irrelavant as long as
** the OP_Column opcode can decode the record later and as long as the
** sqlite3VdbeRecordCompare function will correctly compare two encoded
** records.  Refer to source code comments for the details of the record
** format.
**
** P4 may be a string that is P1 characters long.  The nth character of the
** string indicates the column affinity that should be used for the nth
** field of the index key.
**
** The mapping from character to affinity is given by the SQLITE_AFF_
** macros defined in sqliteInt.h.
**
** If P4 is NULL then all index fields have the affinity NONE.
*/
case OP_MakeRecord: {
  /* Assuming the record contains N fields, the record format looks
  ** like this:
  **
  ** ------------------------------------------------------------------------
  ** | hdr-size | type 0 | type 1 | ... | type N-1 | data0 | ... | data N-1 | 
  ** ------------------------------------------------------------------------
  **
  ** Data(0) is taken from register P1.  Data(1) comes from register P1+1
  ** and so froth.
  **
  ** Each type field is a varint representing the serial type of the 
  ** corresponding data element (see sqlite3VdbeSerialType()). The
  ** hdr-size field is also a varint which is the offset from the beginning
  ** of the record to data0.
  */
  u8 *zNewRecord;        /* A buffer to hold the data for the new record */
  Mem *pRec;             /* The new record */
  u64 nData = 0;         /* Number of bytes of data space */
  int nHdr = 0;          /* Number of bytes of header space */
  u64 nByte = 0;         /* Data space required for this record */
  int nZero = 0;         /* Number of zero bytes at the end of the record */
  int nVarint;           /* Number of bytes in a varint */
  u32 serial_type;       /* Type field */
  Mem *pData0;           /* First field to be combined into the record */
  Mem *pLast;            /* Last field of the record */
  int nField;            /* Number of fields in the record */
  char *zAffinity;       /* The affinity string for the record */
  int file_format;       /* File format to use for encoding */
  int i;                 /* Space used in zNewRecord[] */
  char zTemp[NBFS];      /* Space to hold small records */

  nField = pOp->p1;
  zAffinity = pOp->p4.z;
  assert( nField>0 && pOp->p2>0 && pOp->p2+nField<=p->nMem );
  pData0 = &p->aMem[nField];
  nField = pOp->p2;
  pLast = &pData0[nField-1];
  file_format = p->minWriteFileFormat;

  /* Loop through the elements that will make up the record to figure
  ** out how much space is required for the new record.
  */
  for(pRec=pData0; pRec<=pLast; pRec++){
    int len;
    if( zAffinity ){
      applyAffinity(pRec, zAffinity[pRec-pData0], encoding);
    }
    if( pRec->flags&MEM_Zero && pRec->n>0 ){
      sqlite3VdbeMemExpandBlob(pRec);
    }
    serial_type = sqlite3VdbeSerialType(pRec, file_format);
    len = sqlite3VdbeSerialTypeLen(serial_type);
    nData += len;
    nHdr += sqlite3VarintLen(serial_type);
    if( pRec->flags & MEM_Zero ){
      /* Only pure zero-filled BLOBs can be input to this Opcode.
      ** We do not allow blobs with a prefix and a zero-filled tail. */
      nZero += pRec->u.i;
    }else if( len ){
      nZero = 0;
    }
  }

  /* Add the initial header varint and total the size */
  nHdr += nVarint = sqlite3VarintLen(nHdr);
  if( nVarint<sqlite3VarintLen(nHdr) ){
    nHdr++;
  }
  nByte = nHdr+nData-nZero;
  if( nByte>SQLITE_MAX_LENGTH ){
    goto too_big;
  }

  /* Allocate space for the new record. */
  if( nByte>sizeof(zTemp) ){
    zNewRecord = sqlite3DbMallocRaw(db, nByte);
    if( !zNewRecord ){
      goto no_mem;
    }
  }else{
    zNewRecord = (u8*)zTemp;
  }

  /* Write the record */
  i = sqlite3PutVarint(zNewRecord, nHdr);
  for(pRec=pData0; pRec<=pLast; pRec++){
    serial_type = sqlite3VdbeSerialType(pRec, file_format);
    i += sqlite3PutVarint(&zNewRecord[i], serial_type);      /* serial type */
  }
  for(pRec=pData0; pRec<=pLast; pRec++){  /* serial data */
    i += sqlite3VdbeSerialPut(&zNewRecord[i], nByte-i, pRec, file_format);
  }
  assert( i==nByte );

  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  pOut = &p->aMem[pOp->p3];
  Release(pOut);
  pOut->n = nByte;
  if( nByte<=sizeof(zTemp) ){
    assert( zNewRecord==(unsigned char *)zTemp );
    pOut->z = pOut->zShort;
    memcpy(pOut->zShort, zTemp, nByte);
    pOut->flags = MEM_Blob | MEM_Short;
  }else{
    assert( zNewRecord!=(unsigned char *)zTemp );
    pOut->z = (char*)zNewRecord;
    pOut->flags = MEM_Blob | MEM_Dyn;
    pOut->xDel = 0;
  }
  if( nZero ){
    pOut->u.i = nZero;
    pOut->flags |= MEM_Zero;
  }
  pOut->enc = SQLITE_UTF8;  /* In case the blob is ever converted to text */
  REGISTER_TRACE(pOp->p3, pOut);
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: Statement P1 * * * *
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
case OP_Statement: {
  if( db->autoCommit==0 || db->activeVdbeCnt>1 ){
    int i = pOp->p1;
    Btree *pBt;
    assert( i>=0 && i<db->nDb );
    assert( db->aDb[i].pBt!=0 );
    pBt = db->aDb[i].pBt;
    assert( sqlite3BtreeIsInTrans(pBt) );
    assert( (p->btreeMask & (1<<i))!=0 );
    if( !sqlite3BtreeIsInStmt(pBt) ){
      rc = sqlite3BtreeBeginStmt(pBt);
      p->openedStatement = 1;
    }
  }
  break;
}

/* Opcode: AutoCommit P1 P2 * * *
**
** Set the database auto-commit flag to P1 (1 or 0). If P2 is true, roll
** back any currently active btree transactions. If there are any active
** VMs (apart from this one), then the COMMIT or ROLLBACK statement fails.
**
** This instruction causes the VM to halt.
*/
case OP_AutoCommit: {
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
        p->pc = pc;
        db->autoCommit = 1-i;
        p->rc = rc = SQLITE_BUSY;
        goto vdbe_return;
      }
    }
    if( p->rc==SQLITE_OK ){
      rc = SQLITE_DONE;
    }else{
      rc = SQLITE_ERROR;
    }
    goto vdbe_return;
  }else{
    sqlite3SetString(&p->zErrMsg,
        (!i)?"cannot start a transaction within a transaction":(
        (rollback)?"cannot rollback - no transaction is active":
                   "cannot commit - no transaction is active"), (char*)0);
         
    rc = SQLITE_ERROR;
  }
  break;
}

/* Opcode: Transaction P1 P2 * * *
**
** Begin a transaction.  The transaction ends when a Commit or Rollback
** opcode is encountered.  Depending on the ON CONFLICT setting, the
** transaction might also be rolled back if an error is encountered.
**
** P1 is the index of the database file on which the transaction is
** started.  Index 0 is the main database file and index 1 is the
** file used for temporary tables.  Indices of 2 or more are used for
** attached databases.
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
case OP_Transaction: {
  int i = pOp->p1;
  Btree *pBt;

  assert( i>=0 && i<db->nDb );
  assert( (p->btreeMask & (1<<i))!=0 );
  pBt = db->aDb[i].pBt;

  if( pBt ){
    rc = sqlite3BtreeBeginTrans(pBt, pOp->p2);
    if( rc==SQLITE_BUSY ){
      p->pc = pc;
      p->rc = rc = SQLITE_BUSY;
      goto vdbe_return;
    }
    if( rc!=SQLITE_OK && rc!=SQLITE_READONLY /* && rc!=SQLITE_BUSY */ ){
      goto abort_due_to_error;
    }
  }
  break;
}

/* Opcode: ReadCookie P1 P2 P3 * *
**
** Read cookie number P3 from database P1 and write it into register P2.
** P3==0 is the schema version.  P3==1 is the database format.
** P3==2 is the recommended pager cache size, and so forth.  P1==0 is
** the main database file and P1==1 is the database file used to store
** temporary tables.
**
** If P1 is negative, then this is a request to read the size of a
** databases free-list. P3 must be set to 1 in this case. The actual
** database accessed is ((P1+1)*-1). For example, a P1 parameter of -1
** corresponds to database 0 ("main"), a P1 of -2 is database 1 ("temp").
**
** There must be a read-lock on the database (either a transaction
** must be started or there must be an open cursor) before
** executing this instruction.
*/
case OP_ReadCookie: {               /* out2-prerelease */
  int iMeta;
  int iDb = pOp->p1;
  int iCookie = pOp->p3;

  assert( pOp->p3<SQLITE_N_BTREE_META );
  if( iDb<0 ){
    iDb = (-1*(iDb+1));
    iCookie *= -1;
  }
  assert( iDb>=0 && iDb<db->nDb );
  assert( db->aDb[iDb].pBt!=0 );
  assert( (p->btreeMask & (1<<iDb))!=0 );
  /* The indexing of meta values at the schema layer is off by one from
  ** the indexing in the btree layer.  The btree considers meta[0] to
  ** be the number of free pages in the database (a read-only value)
  ** and meta[1] to be the schema cookie.  The schema layer considers
  ** meta[1] to be the schema cookie.  So we have to shift the index
  ** by one in the following statement.
  */
  rc = sqlite3BtreeGetMeta(db->aDb[iDb].pBt, 1 + iCookie, (u32 *)&iMeta);
  pOut->u.i = iMeta;
  pOut->flags = MEM_Int;
  break;
}

/* Opcode: SetCookie P1 P2 P3 * *
**
** Write the content of register P3 (interpreted as an integer)
** into cookie number P2 of database P1.
** P2==0 is the schema version.  P2==1 is the database format.
** P2==2 is the recommended pager cache size, and so forth.  P1==0 is
** the main database file and P1==1 is the database file used to store
** temporary tables.
**
** A transaction must be started before executing this opcode.
*/
case OP_SetCookie: {       /* in3 */
  Db *pDb;
  assert( pOp->p2<SQLITE_N_BTREE_META );
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  assert( (p->btreeMask & (1<<pOp->p1))!=0 );
  pDb = &db->aDb[pOp->p1];
  assert( pDb->pBt!=0 );
  sqlite3VdbeMemIntegerify(pIn3);
  /* See note about index shifting on OP_ReadCookie */
  rc = sqlite3BtreeUpdateMeta(pDb->pBt, 1+pOp->p2, (int)pIn3->u.i);
  if( pOp->p2==0 ){
    /* When the schema cookie changes, record the new cookie internally */
    pDb->pSchema->schema_cookie = pIn3->u.i;
    db->flags |= SQLITE_InternChanges;
  }else if( pOp->p2==1 ){
    /* Record changes in the file format */
    pDb->pSchema->file_format = pIn3->u.i;
  }
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
case OP_VerifyCookie: {
  int iMeta;
  Btree *pBt;
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  assert( (p->btreeMask & (1<<pOp->p1))!=0 );
  pBt = db->aDb[pOp->p1].pBt;
  if( pBt ){
    rc = sqlite3BtreeGetMeta(pBt, 1, (u32 *)&iMeta);
  }else{
    rc = SQLITE_OK;
    iMeta = 0;
  }
  if( rc==SQLITE_OK && iMeta!=pOp->p2 ){
    sqlite3_free(p->zErrMsg);
    p->zErrMsg = sqlite3DbStrDup(db, "database schema has changed");
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

/* Opcode: OpenRead P1 P2 P3 P4 P5
**
** Open a read-only cursor for the database table whose root page is
** P2 in a database file.  The database file is determined by P3. 
** P3==0 means the main database, P3==1 means the database used for 
** temporary tables, and P3>1 means used the corresponding attached
** database.  Give the new cursor an identifier of P1.  The P1
** values need not be contiguous but all P1 values should be small integers.
** It is an error for P1 to be negative.
**
** If P5!=0 then use the content of register P2 as the root page, not
** the value of P2 itself.
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
** The P4 value is a pointer to a KeyInfo structure that defines the
** content and collating sequence of indices.  P4 is NULL for cursors
** that are not pointing to indices.
**
** See also OpenWrite.
*/
/* Opcode: OpenWrite P1 P2 P3 P4 P5
**
** Open a read/write cursor named P1 on the table or index whose root
** page is P2.  Or if P5!=0 use the content of register P2 to find the
** root page.
**
** The P4 value is a pointer to a KeyInfo structure that defines the
** content and collating sequence of indices.  P4 is NULL for cursors
** that are not pointing to indices.
**
** This instruction works just like OpenRead except that it opens the cursor
** in read/write mode.  For a given table, there can be one or more read-only
** cursors or a single read/write cursor but not both.
**
** See also OpenRead.
*/
case OP_OpenRead:
case OP_OpenWrite: {
  int i = pOp->p1;
  int p2 = pOp->p2;
  int iDb = pOp->p3;
  int wrFlag;
  Btree *pX;
  Cursor *pCur;
  Db *pDb;
  
  assert( iDb>=0 && iDb<db->nDb );
  assert( (p->btreeMask & (1<<iDb))!=0 );
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
  if( pOp->p5 ){
    assert( p2>0 );
    assert( p2<=p->nMem );
    pIn2 = &p->aMem[p2];
    sqlite3VdbeMemIntegerify(pIn2);
    p2 = pIn2->u.i;
    assert( p2>=2 );
  }
  assert( i>=0 );
  pCur = allocateCursor(p, i, iDb);
  if( pCur==0 ) goto no_mem;
  pCur->nullRow = 1;
  /* We always provide a key comparison function.  If the table being
  ** opened is of type INTKEY, the comparision function will be ignored. */
  rc = sqlite3BtreeCursor(pX, p2, wrFlag,
           sqlite3VdbeRecordCompare, pOp->p4.p,
           &pCur->pCursor);
  if( pOp->p4type==P4_KEYINFO ){
    pCur->pKeyInfo = pOp->p4.pKeyInfo;
    pCur->pIncrKey = &pCur->pKeyInfo->incrKey;
    pCur->pKeyInfo->enc = ENC(p->db);
  }else{
    pCur->pKeyInfo = 0;
    pCur->pIncrKey = &pCur->bogusIncrKey;
  }
  switch( rc ){
    case SQLITE_BUSY: {
      p->pc = pc;
      p->rc = rc = SQLITE_BUSY;
      goto vdbe_return;
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
      /* If P4==0 it means we are expected to open a table.  If P4!=0 then
      ** we expect to be opening an index.  If this is not what happened,
      ** then the database is corrupt
      */
      if( (pCur->isTable && pOp->p4type==P4_KEYINFO)
       || (pCur->isIndex && pOp->p4type!=P4_KEYINFO) ){
        rc = SQLITE_CORRUPT_BKPT;
        goto abort_due_to_error;
      }
      break;
    }
    case SQLITE_EMPTY: {
      pCur->isTable = pOp->p4type!=P4_KEYINFO;
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

/* Opcode: OpenEphemeral P1 P2 * P4 *
**
** Open a new cursor P1 to a transient table.
** The cursor is always opened read/write even if 
** the main database is read-only.  The transient or virtual
** table is deleted automatically when the cursor is closed.
**
** P2 is the number of columns in the virtual table.
** The cursor points to a BTree table if P4==0 and to a BTree index
** if P4 is not 0.  If P4 is not NULL, it points to a KeyInfo structure
** that defines the format of keys in the index.
**
** This opcode was once called OpenTemp.  But that created
** confusion because the term "temp table", might refer either
** to a TEMP table at the SQL level, or to a table opened by
** this opcode.  Then this opcode was call OpenVirtual.  But
** that created confusion with the whole virtual-table idea.
*/
case OP_OpenEphemeral: {
  int i = pOp->p1;
  Cursor *pCx;
  static const int openFlags = 
      SQLITE_OPEN_READWRITE |
      SQLITE_OPEN_CREATE |
      SQLITE_OPEN_EXCLUSIVE |
      SQLITE_OPEN_DELETEONCLOSE |
      SQLITE_OPEN_TRANSIENT_DB;

  assert( i>=0 );
  pCx = allocateCursor(p, i, -1);
  if( pCx==0 ) goto no_mem;
  pCx->nullRow = 1;
  rc = sqlite3BtreeFactory(db, 0, 1, SQLITE_DEFAULT_TEMP_CACHE_SIZE, openFlags,
                           &pCx->pBt);
  if( rc==SQLITE_OK ){
    rc = sqlite3BtreeBeginTrans(pCx->pBt, 1);
  }
  if( rc==SQLITE_OK ){
    /* If a transient index is required, create it by calling
    ** sqlite3BtreeCreateTable() with the BTREE_ZERODATA flag before
    ** opening it. If a transient table is required, just use the
    ** automatically created table with root-page 1 (an INTKEY table).
    */
    if( pOp->p4.pKeyInfo ){
      int pgno;
      assert( pOp->p4type==P4_KEYINFO );
      rc = sqlite3BtreeCreateTable(pCx->pBt, &pgno, BTREE_ZERODATA); 
      if( rc==SQLITE_OK ){
        assert( pgno==MASTER_ROOT+1 );
        rc = sqlite3BtreeCursor(pCx->pBt, pgno, 1, sqlite3VdbeRecordCompare,
            pOp->p4.z, &pCx->pCursor);
        pCx->pKeyInfo = pOp->p4.pKeyInfo;
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

/* Opcode: OpenPseudo P1 * * * *
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
case OP_OpenPseudo: {
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

/* Opcode: Close P1 * * * *
**
** Close a cursor previously opened as P1.  If P1 is not
** currently open, this instruction is a no-op.
*/
case OP_Close: {
  int i = pOp->p1;
  assert( i>=0 && i<p->nCursor );
  sqlite3VdbeFreeCursor(p, p->apCsr[i]);
  p->apCsr[i] = 0;
  break;
}

/* Opcode: MoveGe P1 P2 P3 * *
**
** Use the value in register P3 as a key.  Reposition
** cursor P1 so that it points to the smallest entry that is greater
** than or equal to the key in register P3.
** If there are no records greater than or equal to the key and P2 
** is not zero, then jump to P2.
**
** A special feature of this opcode (and different from the
** related OP_MoveGt, OP_MoveLt, and OP_MoveLe) is that if P2 is
** zero and P1 is an SQL table (a b-tree with integer keys) then
** the seek is deferred until it is actually needed.  It might be
** the case that the cursor is never accessed.  By deferring the
** seek, we avoid unnecessary seeks.
**
** See also: Found, NotFound, Distinct, MoveLt, MoveGt, MoveLe
*/
/* Opcode: MoveGt P1 P2 P3 * *
**
** Use the value in register P3 as a key.  Reposition
** cursor P1 so that it points to the smallest entry that is greater
** than the key in register P3.
** If there are no records greater than the key 
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveLt, MoveGe, MoveLe
*/
/* Opcode: MoveLt P1 P2 P3 * * 
**
** Use the value in register P3 as a key.  Reposition
** cursor P1 so that it points to the largest entry that is less
** than the key in register P3.
** If there are no records less than the key
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveGt, MoveGe, MoveLe
*/
/* Opcode: MoveLe P1 P2 P3 * *
**
** Use the value in register P3 as a key.  Reposition
** cursor P1 so that it points to the largest entry that is less than
** or equal to the key.
** If there are no records less than or eqal to the key
** then jump to P2.
**
** See also: Found, NotFound, Distinct, MoveGt, MoveGe, MoveLt
*/
case OP_MoveLt:         /* jump, in3 */
case OP_MoveLe:         /* jump, in3 */
case OP_MoveGe:         /* jump, in3 */
case OP_MoveGt: {       /* jump, in3 */
  int i = pOp->p1;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  if( pC->pCursor!=0 ){
    int res, oc;
    oc = pOp->opcode;
    pC->nullRow = 0;
    *pC->pIncrKey = oc==OP_MoveGt || oc==OP_MoveLe;
    if( pC->isTable ){
      i64 iKey = sqlite3VdbeIntValue(pIn3);
      if( pOp->p2==0 ){
        assert( pOp->opcode==OP_MoveGe );
        pC->movetoTarget = iKey;
        pC->rowidIsValid = 0;
        pC->deferredMoveto = 1;
        break;
      }
      rc = sqlite3BtreeMoveto(pC->pCursor, 0, (u64)iKey, 0, &res);
      if( rc!=SQLITE_OK ){
        goto abort_due_to_error;
      }
      pC->lastRowid = iKey;
      pC->rowidIsValid = res==0;
    }else{
      assert( pIn3->flags & MEM_Blob );
      ExpandBlob(pIn3);
      rc = sqlite3BtreeMoveto(pC->pCursor, pIn3->z, pIn3->n, 0, &res);
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
    assert( pOp->p2>0 );
    if( res ){
      pc = pOp->p2 - 1;
    }
  }
  break;
}

/* Opcode: Found P1 P2 P3 * *
**
** Register P3 holds a blob constructed by MakeRecord.  P1 is an index.
** If an entry that matches the value in register p3 exists in P1 then
** jump to P2.  If the P3 value does not match any entry in P1
** then fall thru.  The P1 cursor is left pointing at the matching entry
** if it exists.
**
** This instruction is used to implement the IN operator where the
** left-hand side is a SELECT statement.  P1 may be a true index, or it
** may be a temporary index that holds the results of the SELECT
** statement.   This instruction is also used to implement the
** DISTINCT keyword in SELECT statements.
**
** This instruction checks if index P1 contains a record for which 
** the first N serialised values exactly match the N serialised values
** in the record in register P3, where N is the total number of values in
** the P3 record (the P3 record is a prefix of the P1 record). 
**
** See also: NotFound, MoveTo, IsUnique, NotExists
*/
/* Opcode: NotFound P1 P2 P3 * *
**
** Register P3 holds a blob constructed by MakeRecord.  P1 is
** an index.  If no entry exists in P1 that matches the blob then jump
** to P2.  If an entry does existing, fall through.  The cursor is left
** pointing to the entry that matches.
**
** See also: Found, MoveTo, NotExists, IsUnique
*/
case OP_NotFound:       /* jump, in3 */
case OP_Found: {        /* jump, in3 */
  int i = pOp->p1;
  int alreadyExists = 0;
  Cursor *pC;
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pC = p->apCsr[i])->pCursor!=0 ){
    int res;
    assert( pC->isTable==0 );
    assert( pIn3->flags & MEM_Blob );
    if( pOp->opcode==OP_Found ){
      pC->pKeyInfo->prefixIsEqual = 1;
    }
    rc = sqlite3BtreeMoveto(pC->pCursor, pIn3->z, pIn3->n, 0, &res);
    pC->pKeyInfo->prefixIsEqual = 0;
    if( rc!=SQLITE_OK ){
      break;
    }
    alreadyExists = (res==0);
    pC->deferredMoveto = 0;
    pC->cacheStatus = CACHE_STALE;
  }
  if( pOp->opcode==OP_Found ){
    if( alreadyExists ) pc = pOp->p2 - 1;
  }else{
    if( !alreadyExists ) pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: IsUnique P1 P2 P3 P4 *
**
** The P3 register contains an integer record number.  Call this
** record number R.  The P4 register contains an index key created
** using MakeIdxRec.  Call it K.
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
** number for that entry is written into P3 and control
** falls through to the next instruction.
**
** See also: NotFound, NotExists, Found
*/
case OP_IsUnique: {        /* jump, in3 */
  int i = pOp->p1;
  Cursor *pCx;
  BtCursor *pCrsr;
  Mem *pK;
  i64 R;

  /* Pop the value R off the top of the stack
  */
  assert( pOp->p4type==P4_INT32 );
  assert( pOp->p4.i>0 && pOp->p4.i<=p->nMem );
  pK = &p->aMem[pOp->p4.i];
  sqlite3VdbeMemIntegerify(pIn3);
  R = pIn3->u.i;
  assert( (pIn3->flags & MEM_Dyn)==0 );
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
    assert( pK->flags & MEM_Blob );
    zKey = pK->z;
    nKey = pK->n;

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

    /* The final varint of the key is different from R.  Store it back
    ** into register R3.  (The record number of an entry that violates
    ** a UNIQUE constraint.)
    */
    pIn3->u.i = v;
    assert( pIn3->flags==MEM_Int );
  }
  break;
}

/* Opcode: NotExists P1 P2 P3 * *
**
** Use the content of register P3 as a integer key.  If a record 
** with that key does not exist in table of P1, then jump to P2. 
** If the record does exist, then fall thru.  The cursor is left 
** pointing to the record if it exists.
**
** The difference between this operation and NotFound is that this
** operation assumes the key is an integer and that P1 is a table whereas
** NotFound assumes key is a blob constructed from MakeRecord and
** P1 is an index.
**
** See also: Found, MoveTo, NotFound, IsUnique
*/
case OP_NotExists: {        /* jump, in3 */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    int res;
    u64 iKey;
    assert( pIn3->flags & MEM_Int );
    assert( p->apCsr[i]->isTable );
    iKey = intToKey(pIn3->u.i);
    rc = sqlite3BtreeMoveto(pCrsr, 0, iKey, 0,&res);
    pC->lastRowid = pIn3->u.i;
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
      assert( pC->rowidIsValid==0 );
    }
  }
  break;
}

/* Opcode: Sequence P1 P2 * * *
**
** Find the next available sequence number for cursor P1.
** Write the sequence number into register P2.
** The sequence number on the cursor is incremented after this
** instruction.  
*/
case OP_Sequence: {           /* out2-prerelease */
  int i = pOp->p1;
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  pOut->u.i = p->apCsr[i]->seqCount++;
  pOut->flags = MEM_Int;
  break;
}


/* Opcode: NewRowid P1 P2 P3 * *
**
** Get a new integer record number (a.k.a "rowid") used as the key to a table.
** The record number is not previously used as a key in the database
** table that cursor P1 points to.  The new record number is written
** written to register P2.
**
** If P3>0 then P3 is a register that holds the largest previously
** generated record number.  No new record numbers are allowed to be less
** than this value.  When this value reaches its maximum, a SQLITE_FULL
** error is generated.  The P3 register is updated with the generated
** record number.  This P3 mechanism is used to help implement the
** AUTOINCREMENT feature.
*/
case OP_NewRowid: {           /* out2-prerelease */
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
      if( pOp->p3 ){
        Mem *pMem;
        assert( pOp->p3>0 && pOp->p3<=p->nMem ); /* P3 is a valid memory cell */
        pMem = &p->aMem[pOp->p3];
	REGISTER_TRACE(pOp->p3, pMem);
        sqlite3VdbeMemIntegerify(pMem);
        assert( (pMem->flags & MEM_Int)!=0 );  /* mem(P3) holds an integer */
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
      assert( pOp->p3==0 );  /* SQLITE_FULL must have occurred prior to this */
      v = db->priorNewRowid;
      cnt = 0;
      do{
        if( cnt==0 && (v&0xffffff)==v ){
          v++;
        }else{
          sqlite3Randomness(sizeof(v), &v);
          if( cnt<5 ) v &= 0xffffff;
        }
        if( v==0 ) continue;
        x = intToKey(v);
        rx = sqlite3BtreeMoveto(pC->pCursor, 0, (u64)x, 0, &res);
        cnt++;
      }while( cnt<100 && rx==SQLITE_OK && res==0 );
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
  pOut->flags = MEM_Int;
  pOut->u.i = v;
  break;
}

/* Opcode: Insert P1 P2 P3 P4 P5
**
** Write an entry into the table of cursor P1.  A new entry is
** created if it doesn't already exist or the data for an existing
** entry is overwritten.  The data is the value stored register
** number P2. The key is stored in register P3. The key must
** be an integer.
**
** If the OPFLAG_NCHANGE flag of P5 is set, then the row change count is
** incremented (otherwise not).  If the OPFLAG_LASTROWID flag of P5 is set,
** then rowid is stored for subsequent return by the
** sqlite3_last_insert_rowid() function (otherwise it is unmodified).
**
** Parameter P4 may point to a string containing the table-name, or
** may be NULL. If it is not NULL, then the update-hook 
** (sqlite3.xUpdateCallback) is invoked following a successful insert.
**
** (WARNING/TODO: If P1 is a pseudo-cursor and P2 is dynamically
** allocated, then ownership of P2 is transferred to the pseudo-cursor
** and register P2 becomes ephemeral.  If the cursor is changed, the
** value of register P2 will then change.  Make sure this does not
** cause any problems.)
**
** This instruction only works on tables.  The equivalent instruction
** for indices is OP_IdxInsert.
*/
case OP_Insert: {
  Mem *pData = &p->aMem[pOp->p2];
  Mem *pKey = &p->aMem[pOp->p3];

  i64 iKey;   /* The integer ROWID or key for the record to be inserted */
  int i = pOp->p1;
  Cursor *pC;
  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  assert( pC->pCursor!=0 || pC->pseudoTable );
  assert( pKey->flags & MEM_Int );
  assert( pC->isTable );
  REGISTER_TRACE(pOp->p2, pData);
  REGISTER_TRACE(pOp->p3, pKey);

  iKey = intToKey(pKey->u.i);
  if( pOp->p5 & OPFLAG_NCHANGE ) p->nChange++;
  if( pOp->p5 & OPFLAG_LASTROWID ) db->lastRowid = pKey->u.i;
  if( pC->nextRowidValid && pKey->u.i>=pC->nextRowid ){
    pC->nextRowidValid = 0;
  }
  if( pData->flags & MEM_Null ){
    pData->z = 0;
    pData->n = 0;
  }else{
    assert( pData->flags & (MEM_Blob|MEM_Str) );
  }
  if( pC->pseudoTable ){
    sqlite3_free(pC->pData);
    pC->iKey = iKey;
    pC->nData = pData->n;
    if( pData->flags & MEM_Dyn ){
      pC->pData = pData->z;
      pData->flags &= ~MEM_Dyn;
      pData->flags |= MEM_Ephem;
    }else{
      pC->pData = sqlite3_malloc( pC->nData+2 );
      if( !pC->pData ) goto no_mem;
      memcpy(pC->pData, pData->z, pC->nData);
      pC->pData[pC->nData] = 0;
      pC->pData[pC->nData+1] = 0;
    }
    pC->nullRow = 0;
  }else{
    int nZero;
    if( pData->flags & MEM_Zero ){
      nZero = pData->u.i;
    }else{
      nZero = 0;
    }
    rc = sqlite3BtreeInsert(pC->pCursor, 0, iKey,
                            pData->z, pData->n, nZero,
                            pOp->p5 & OPFLAG_APPEND);
  }
  
  pC->rowidIsValid = 0;
  pC->deferredMoveto = 0;
  pC->cacheStatus = CACHE_STALE;

  /* Invoke the update-hook if required. */
  if( rc==SQLITE_OK && db->xUpdateCallback && pOp->p4.z ){
    const char *zDb = db->aDb[pC->iDb].zName;
    const char *zTbl = pOp->p4.z;
    int op = ((pOp->p5 & OPFLAG_ISUPDATE) ? SQLITE_UPDATE : SQLITE_INSERT);
    assert( pC->isTable );
    db->xUpdateCallback(db->pUpdateArg, op, zDb, zTbl, iKey);
    assert( pC->iDb>=0 );
  }
  break;
}

/* Opcode: Delete P1 P2 * P4 *
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
** P1 must not be pseudo-table.  It has to be a real table with
** multiple rows.
**
** If P4 is not NULL, then it is the name of the table that P1 is
** pointing to.  The update hook will be invoked, if it exists.
** If P4 is not NULL then the P1 cursor must have been positioned
** using OP_NotFound prior to invoking this opcode.
*/
case OP_Delete: {
  int i = pOp->p1;
  i64 iKey;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  assert( pC->pCursor!=0 );  /* Only valid for real tables, no pseudotables */

  /* If the update-hook will be invoked, set iKey to the rowid of the
  ** row being deleted.
  */
  if( db->xUpdateCallback && pOp->p4.z ){
    assert( pC->isTable );
    assert( pC->rowidIsValid );  /* lastRowid set by previous OP_NotFound */
    iKey = pC->lastRowid;
  }

  rc = sqlite3VdbeCursorMoveto(pC);
  if( rc ) goto abort_due_to_error;
  rc = sqlite3BtreeDelete(pC->pCursor);
  pC->nextRowidValid = 0;
  pC->cacheStatus = CACHE_STALE;

  /* Invoke the update-hook if required. */
  if( rc==SQLITE_OK && db->xUpdateCallback && pOp->p4.z ){
    const char *zDb = db->aDb[pC->iDb].zName;
    const char *zTbl = pOp->p4.z;
    db->xUpdateCallback(db->pUpdateArg, SQLITE_DELETE, zDb, zTbl, iKey);
    assert( pC->iDb>=0 );
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
case OP_ResetCount: {
  if( pOp->p1 ){
    sqlite3VdbeSetChanges(db, p->nChange);
  }
  p->nChange = 0;
  break;
}

/* Opcode: RowData P1 P2 * * *
**
** Write into register P2 the complete row data for cursor P1.
** There is no interpretation of the data.  
** It is just copied onto the P2 register exactly as 
** it is found in the database file.
**
** If the P1 cursor must be pointing to a valid row (not a NULL row)
** of a real table, not a pseudo-table.
*/
/* Opcode: RowKey P1 P2 * * *
**
** Write into register P2 the complete row key for cursor P1.
** There is no interpretation of the data.  
** The key is copied onto the P3 register exactly as 
** it is found in the database file.
**
** If the P1 cursor must be pointing to a valid row (not a NULL row)
** of a real table, not a pseudo-table.
*/
case OP_RowKey:             /* out2-prerelease */
case OP_RowData: {          /* out2-prerelease */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  u32 n;

  /* Note that RowKey and RowData are really exactly the same instruction */
  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC->isTable || pOp->opcode==OP_RowKey );
  assert( pC->isIndex || pOp->opcode==OP_RowData );
  assert( pC!=0 );
  assert( pC->nullRow==0 );
  assert( pC->pseudoTable==0 );
  assert( pC->pCursor!=0 );
  pCrsr = pC->pCursor;
  rc = sqlite3VdbeCursorMoveto(pC);
  if( rc ) goto abort_due_to_error;
  if( pC->isIndex ){
    i64 n64;
    assert( !pC->isTable );
    sqlite3BtreeKeySize(pCrsr, &n64);
    if( n64>SQLITE_MAX_LENGTH ){
      goto too_big;
    }
    n = n64;
  }else{
    sqlite3BtreeDataSize(pCrsr, &n);
    if( n>SQLITE_MAX_LENGTH ){
      goto too_big;
    }
  }
  pOut->n = n;
  if( n<=NBFS ){
    pOut->flags = MEM_Blob | MEM_Short;
    pOut->z = pOut->zShort;
  }else{
    char *z = sqlite3_malloc( n );
    if( z==0 ) goto no_mem;
    pOut->flags = MEM_Blob | MEM_Dyn;
    pOut->xDel = 0;
    pOut->z = z;
  }
  if( pC->isIndex ){
    rc = sqlite3BtreeKey(pCrsr, 0, n, pOut->z);
  }else{
    rc = sqlite3BtreeData(pCrsr, 0, n, pOut->z);
  }
  pOut->enc = SQLITE_UTF8;  /* In case the blob is ever cast to text */
  UPDATE_MAX_BLOBSIZE(pOut);
  break;
}

/* Opcode: Rowid P1 P2 * * *
**
** Store in register P2 an integer which is the key of the table entry that
** P1 is currently point to.  If p2==0 then push the integer.
*/
case OP_Rowid: {                 /* out2-prerelease */
  int i = pOp->p1;
  Cursor *pC;
  i64 v;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  rc = sqlite3VdbeCursorMoveto(pC);
  if( rc ) goto abort_due_to_error;
  if( pC->rowidIsValid ){
    v = pC->lastRowid;
  }else if( pC->pseudoTable ){
    v = keyToInt(pC->iKey);
  }else if( pC->nullRow ){
    /* Leave the rowid set to a NULL */
    break;
  }else{
    assert( pC->pCursor!=0 );
    sqlite3BtreeKeySize(pC->pCursor, &v);
    v = keyToInt(v);
  }
  pOut->u.i = v;
  pOut->flags = MEM_Int;
  break;
}

/* Opcode: NullRow P1 * * * *
**
** Move the cursor P1 to a null row.  Any OP_Column operations
** that occur while the cursor is on the null row will always
** write a NULL.
*/
case OP_NullRow: {
  int i = pOp->p1;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  pC->nullRow = 1;
  pC->rowidIsValid = 0;
  break;
}

/* Opcode: Last P1 P2 * * *
**
** The next use of the Rowid or Column or Next instruction for P1 
** will refer to the last entry in the database table or index.
** If the table or index is empty and P2>0, then jump immediately to P2.
** If P2 is 0 or if the table or index is not empty, fall through
** to the following instruction.
*/
case OP_Last: {        /* jump */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  int res;

  assert( i>=0 && i<p->nCursor );
  pC = p->apCsr[i];
  assert( pC!=0 );
  pCrsr = pC->pCursor;
  assert( pCrsr!=0 );
  rc = sqlite3BtreeLast(pCrsr, &res);
  pC->nullRow = res;
  pC->deferredMoveto = 0;
  pC->cacheStatus = CACHE_STALE;
  if( res && pOp->p2>0 ){
    pc = pOp->p2 - 1;
  }
  break;
}


/* Opcode: Sort P1 P2 * * *
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
case OP_Sort: {        /* jump */
#ifdef SQLITE_TEST
  sqlite3_sort_count++;
  sqlite3_search_count--;
#endif
  /* Fall through into OP_Rewind */
}
/* Opcode: Rewind P1 P2 * * *
**
** The next use of the Rowid or Column or Next instruction for P1 
** will refer to the first entry in the database table or index.
** If the table or index is empty and P2>0, then jump immediately to P2.
** If P2 is 0 or if the table or index is not empty, fall through
** to the following instruction.
*/
case OP_Rewind: {        /* jump */
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
  assert( pOp->p2>0 && pOp->p2<p->nOp );
  if( res ){
    pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: Next P1 P2 * * *
**
** Advance cursor P1 so that it points to the next key/data pair in its
** table or index.  If there are no more key/value pairs then fall through
** to the following instruction.  But if the cursor advance was successful,
** jump immediately to P2.
**
** The P1 cursor must be for a real table, not a pseudo-table.
**
** See also: Prev
*/
/* Opcode: Prev P1 P2 * * *
**
** Back up cursor P1 so that it points to the previous key/data pair in its
** table or index.  If there is no previous key/value pairs then fall through
** to the following instruction.  But if the cursor backup was successful,
** jump immediately to P2.
**
** The P1 cursor must be for a real table, not a pseudo-table.
*/
case OP_Prev:          /* jump */
case OP_Next: {        /* jump */
  Cursor *pC;
  BtCursor *pCrsr;
    int res;

  CHECK_FOR_INTERRUPT;
  assert( pOp->p1>=0 && pOp->p1<p->nCursor );
  pC = p->apCsr[pOp->p1];
  if( pC==0 ){
    break;  /* See ticket #2273 */
  }
  pCrsr = pC->pCursor;
  assert( pCrsr );
  if( pC->nullRow ){
    res = 1;
  }else{
    assert( pC->deferredMoveto==0 );
    rc = pOp->opcode==OP_Next ? sqlite3BtreeNext(pCrsr, &res) :
                                sqlite3BtreePrevious(pCrsr, &res);
    pC->nullRow = res;
    pC->cacheStatus = CACHE_STALE;
    if( res==0 ){
      pc = pOp->p2 - 1;
#ifdef SQLITE_TEST
      sqlite3_search_count++;
#endif
    }
  }
  pC->rowidIsValid = 0;
  break;
}

/* Opcode: IdxInsert P1 P2 P3 * *
**
** Register P2 holds a SQL index key made using the
** MakeIdxRec instructions.  This opcode writes that key
** into the index P1.  Data for the entry is nil.
**
** P3 is a flag that provides a hint to the b-tree layer that this
** insert is likely to be an append.
**
** This instruction only works for indices.  The equivalent instruction
** for tables is OP_Insert.
*/
case OP_IdxInsert: {        /* in2 */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  assert( pIn2->flags & MEM_Blob );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    assert( pC->isTable==0 );
    rc = ExpandBlob(pIn2);
    if( rc==SQLITE_OK ){
      int nKey = pIn2->n;
      const char *zKey = pIn2->z;
      rc = sqlite3BtreeInsert(pCrsr, zKey, nKey, "", 0, 0, pOp->p3);
      assert( pC->deferredMoveto==0 );
      pC->cacheStatus = CACHE_STALE;
    }
  }
  break;
}

/* Opcode: IdxDelete P1 P2 * * *
**
** The content of register P2 is an index key built using the either the
** MakeIdxRec opcode.  Removes that entry from the index.
*/
case OP_IdxDelete: {        /* in2 */
  int i = pOp->p1;
  Cursor *pC;
  BtCursor *pCrsr;
  assert( pIn2->flags & MEM_Blob );
  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    int res;
    rc = sqlite3BtreeMoveto(pCrsr, pIn2->z, pIn2->n, 0, &res);
    if( rc==SQLITE_OK && res==0 ){
      rc = sqlite3BtreeDelete(pCrsr);
    }
    assert( pC->deferredMoveto==0 );
    pC->cacheStatus = CACHE_STALE;
  }
  break;
}

/* Opcode: IdxRowid P1 P2 * * *
**
** Write into register P2 an integer which is the last entry in the record at
** the end of the index key pointed to by cursor P1.  This integer should be
** the rowid of the table entry to which this index entry points.
**
** See also: Rowid, MakeIdxRec.
*/
case OP_IdxRowid: {              /* out2-prerelease */
  int i = pOp->p1;
  BtCursor *pCrsr;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pCrsr = (pC = p->apCsr[i])->pCursor)!=0 ){
    i64 rowid;

    assert( pC->deferredMoveto==0 );
    assert( pC->isTable==0 );
    if( !pC->nullRow ){
      rc = sqlite3VdbeIdxRowid(pCrsr, &rowid);
      if( rc!=SQLITE_OK ){
        goto abort_due_to_error;
      }
      pOut->flags = MEM_Int;
      pOut->u.i = rowid;
    }
  }
  break;
}

/* Opcode: IdxGE P1 P2 P3 * P5
**
** The value in register P3 is an index entry that omits the ROWID.  Compare
** this value against the index that P1 is currently pointing to.
** Ignore the ROWID on the P1 index.
**
** If the P1 index entry is greater than or equal to the value in 
** register P3 then jump to P2.  Otherwise fall through to the next 
** instruction.
**
** If P5 is non-zero then the value in register P3 is temporarily
** increased by an epsilon prior to the comparison.  This make the opcode work
** like IdxGT except that if the key from register P3 is a prefix of
** the key in the cursor, the result is false whereas it would be
** true with IdxGT.
*/
/* Opcode: IdxLT P1 P2 P3 * P5
**
** The value in register P3 is an index entry that omits the ROWID.  Compare
** the this value against the index that P1 is currently pointing to.
** Ignore the ROWID on the P1 index.
**
** If the P1 index entry is less than the register P3 value
** then jump to P2.  Otherwise fall through to the next instruction.
**
** If P5 is non-zero then the
** index taken from register P3 is temporarily increased by
** an epsilon prior to the comparison.  This makes the opcode work
** like IdxLE.
*/
case OP_IdxLT:          /* jump, in3 */
case OP_IdxGE: {        /* jump, in3 */
  int i= pOp->p1;
  Cursor *pC;

  assert( i>=0 && i<p->nCursor );
  assert( p->apCsr[i]!=0 );
  if( (pC = p->apCsr[i])->pCursor!=0 ){
    int res;
 
    assert( pIn3->flags & MEM_Blob );  /* Created using OP_MakeRecord */
    assert( pC->deferredMoveto==0 );
    ExpandBlob(pIn3);
    assert( pOp->p5==0 || pOp->p5==1 );
    *pC->pIncrKey = pOp->p5;
    rc = sqlite3VdbeIdxKeyCompare(pC, pIn3->n, (u8*)pIn3->z, &res);
    *pC->pIncrKey = 0;
    if( rc!=SQLITE_OK ){
      break;
    }
    if( pOp->opcode==OP_IdxLT ){
      res = -res;
    }else{
      assert( pOp->opcode==OP_IdxGE );
      res++;
    }
    if( res>0 ){
      pc = pOp->p2 - 1 ;
    }
  }
  break;
}

/* Opcode: Destroy P1 P2 P3 * *
**
** Delete an entire database table or index whose root page in the database
** file is given by P1.
**
** The table being destroyed is in the main database file if P3==0.  If
** P3==1 then the table to be clear is in the auxiliary database file
** that is used to store tables create using CREATE TEMPORARY TABLE.
**
** If AUTOVACUUM is enabled then it is possible that another root page
** might be moved into the newly deleted root page in order to keep all
** root pages contiguous at the beginning of the database.  The former
** value of the root page that moved - its value before the move occurred -
** is stored in register P2.  If no page 
** movement was required (because the table being dropped was already 
** the last one in the database) then a zero is stored in register P2.
** If AUTOVACUUM is disabled then a zero is stored in register P2.
**
** See also: Clear
*/
case OP_Destroy: {     /* out2-prerelease */
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
    p->errorAction = OE_Abort;
  }else{
    int iDb = pOp->p3;
    assert( iCnt==1 );
    assert( (p->btreeMask & (1<<iDb))!=0 );
    rc = sqlite3BtreeDropTable(db->aDb[iDb].pBt, pOp->p1, &iMoved);
    pOut->flags = MEM_Int;
    pOut->u.i = iMoved;
#ifndef SQLITE_OMIT_AUTOVACUUM
    if( rc==SQLITE_OK && iMoved!=0 ){
      sqlite3RootPageMoved(&db->aDb[iDb], iMoved, pOp->p1);
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
case OP_Clear: {
  assert( (p->btreeMask & (1<<pOp->p2))!=0 );
  rc = sqlite3BtreeClearTable(db->aDb[pOp->p2].pBt, pOp->p1);
  break;
}

/* Opcode: CreateTable P1 P2 * * *
**
** Allocate a new table in the main database file if P1==0 or in the
** auxiliary database file if P1==1 or in an attached database if
** P1>1.  Write the root page number of the new table into
** register P2
**
** The difference between a table and an index is this:  A table must
** have a 4-byte integer key and can have arbitrary data.  An index
** has an arbitrary key but no data.
**
** See also: CreateIndex
*/
/* Opcode: CreateIndex P1 P2 * * *
**
** Allocate a new index in the main database file if P1==0 or in the
** auxiliary database file if P1==1 or in an attached database if
** P1>1.  Write the root page number of the new table into
** register P2.
**
** See documentation on OP_CreateTable for additional information.
*/
case OP_CreateIndex:            /* out2-prerelease */
case OP_CreateTable: {          /* out2-prerelease */
  int pgno;
  int flags;
  Db *pDb;
  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  assert( (p->btreeMask & (1<<pOp->p1))!=0 );
  pDb = &db->aDb[pOp->p1];
  assert( pDb->pBt!=0 );
  if( pOp->opcode==OP_CreateTable ){
    /* flags = BTREE_INTKEY; */
    flags = BTREE_LEAFDATA|BTREE_INTKEY;
  }else{
    flags = BTREE_ZERODATA;
  }
  rc = sqlite3BtreeCreateTable(pDb->pBt, &pgno, flags);
  if( rc==SQLITE_OK ){
    pOut->u.i = pgno;
    pOut->flags = MEM_Int;
  }
  break;
}

/* Opcode: ParseSchema P1 P2 * P4 *
**
** Read and parse all entries from the SQLITE_MASTER table of database P1
** that match the WHERE clause P4.  P2 is the "force" flag.   Always do
** the parsing if P2 is true.  If P2 is false, then this routine is a
** no-op if the schema is not currently loaded.  In other words, if P2
** is false, the SQLITE_MASTER table is only parsed if the rest of the
** schema is already loaded into the symbol table.
**
** This opcode invokes the parser to create a new virtual machine,
** then runs the new virtual machine.  It is thus a reentrant opcode.
*/
case OP_ParseSchema: {
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
  zSql = sqlite3MPrintf(db,
     "SELECT name, rootpage, sql FROM '%q'.%s WHERE %s",
     db->aDb[iDb].zName, zMaster, pOp->p4.z);
  if( zSql==0 ) goto no_mem;
  (void)sqlite3SafetyOff(db);
  assert( db->init.busy==0 );
  db->init.busy = 1;
  assert( !db->mallocFailed );
  rc = sqlite3_exec(db, zSql, sqlite3InitCallback, &initData, 0);
  if( rc==SQLITE_ABORT ) rc = initData.rc;
  sqlite3_free(zSql);
  db->init.busy = 0;
  (void)sqlite3SafetyOn(db);
  if( rc==SQLITE_NOMEM ){
    goto no_mem;
  }
  break;  
}

#if !defined(SQLITE_OMIT_ANALYZE) && !defined(SQLITE_OMIT_PARSER)
/* Opcode: LoadAnalysis P1 * * * *
**
** Read the sqlite_stat1 table for database P1 and load the content
** of that table into the internal index hash table.  This will cause
** the analysis to be used when preparing all subsequent queries.
*/
case OP_LoadAnalysis: {
  int iDb = pOp->p1;
  assert( iDb>=0 && iDb<db->nDb );
  rc = sqlite3AnalysisLoad(db, iDb);
  break;  
}
#endif /* !defined(SQLITE_OMIT_ANALYZE) && !defined(SQLITE_OMIT_PARSER)  */

/* Opcode: DropTable P1 * * P4 *
**
** Remove the internal (in-memory) data structures that describe
** the table named P4 in database P1.  This is called after a table
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropTable: {
  sqlite3UnlinkAndDeleteTable(db, pOp->p1, pOp->p4.z);
  break;
}

/* Opcode: DropIndex P1 * * P4 *
**
** Remove the internal (in-memory) data structures that describe
** the index named P4 in database P1.  This is called after an index
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropIndex: {
  sqlite3UnlinkAndDeleteIndex(db, pOp->p1, pOp->p4.z);
  break;
}

/* Opcode: DropTrigger P1 * * P4 *
**
** Remove the internal (in-memory) data structures that describe
** the trigger named P4 in database P1.  This is called after a trigger
** is dropped in order to keep the internal representation of the
** schema consistent with what is on disk.
*/
case OP_DropTrigger: {
  sqlite3UnlinkAndDeleteTrigger(db, pOp->p1, pOp->p4.z);
  break;
}


#ifndef SQLITE_OMIT_INTEGRITY_CHECK
/* Opcode: IntegrityCk P1 P2 P3 * P5
**
** Do an analysis of the currently open database.  Store in
** register P1 the text of an error message describing any problems.
** If no problems are found, store a NULL in register P1.
**
** The register P3 contains the maximum number of allowed errors.
** At most reg(P3) errors will be reported.
** In other words, the analysis stops as soon as reg(P1) errors are 
** seen.  Reg(P1) is updated with the number of errors remaining.
**
** The root page numbers of all tables in the database are integer
** stored in reg(P1), reg(P1+1), reg(P1+2), ....  There are P2 tables
** total.
**
** If P5 is not zero, the check is done on the auxiliary database
** file, not the main database file.
**
** This opcode is used to implement the integrity_check pragma.
*/
case OP_IntegrityCk: {
  int nRoot;      /* Number of tables to check.  (Number of root pages.) */
  int *aRoot;     /* Array of rootpage numbers for tables to be checked */
  int j;          /* Loop counter */
  int nErr;       /* Number of errors reported */
  char *z;        /* Text of the error report */
  Mem *pnErr;     /* Register keeping track of errors remaining */
  
  nRoot = pOp->p2;
  assert( nRoot>0 );
  aRoot = sqlite3_malloc( sizeof(int)*(nRoot+1) );
  if( aRoot==0 ) goto no_mem;
  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  pnErr = &p->aMem[pOp->p3];
  assert( (pnErr->flags & MEM_Int)!=0 );
  assert( (pnErr->flags & (MEM_Str|MEM_Blob))==0 );
  pIn1 = &p->aMem[pOp->p1];
  for(j=0; j<nRoot; j++){
    aRoot[j] = sqlite3VdbeIntValue(&pIn1[j]);
  }
  aRoot[j] = 0;
  assert( pOp->p5<db->nDb );
  assert( (p->btreeMask & (1<<pOp->p5))!=0 );
  z = sqlite3BtreeIntegrityCheck(db->aDb[pOp->p5].pBt, aRoot, nRoot,
                                 pnErr->u.i, &nErr);
  pnErr->u.i -= nErr;
  sqlite3VdbeMemSetNull(pIn1);
  if( nErr==0 ){
    assert( z==0 );
  }else{
    pIn1->z = z;
    pIn1->n = strlen(z);
    pIn1->flags = MEM_Str | MEM_Dyn | MEM_Term;
    pIn1->xDel = 0;
  }
  pIn1->enc = SQLITE_UTF8;
  UPDATE_MAX_BLOBSIZE(pIn1);
  sqlite3VdbeChangeEncoding(pIn1, encoding);
  sqlite3_free(aRoot);
  break;
}
#endif /* SQLITE_OMIT_INTEGRITY_CHECK */

/* Opcode: FifoWrite P1 * * * *
**
** Write the integer from register P1 into the Fifo.
*/
case OP_FifoWrite: {        /* in1 */
  if( sqlite3VdbeFifoPush(&p->sFifo, sqlite3VdbeIntValue(pIn1))==SQLITE_NOMEM ){
    goto no_mem;
  }
  break;
}

/* Opcode: FifoRead P1 P2 * * *
**
** Attempt to read a single integer from the Fifo.  Store that
** integer in register P1.
** 
** If the Fifo is empty jump to P2.
*/
case OP_FifoRead: {         /* jump */
  CHECK_FOR_INTERRUPT;
  assert( pOp->p1>0 && pOp->p1<=p->nMem );
  pOut = &p->aMem[pOp->p1];
  Release(pOut);
  pOut->flags = MEM_Int;
  if( sqlite3VdbeFifoPop(&p->sFifo, &pOut->u.i)==SQLITE_DONE ){
    pc = pOp->p2 - 1;
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
case OP_ContextPush: {
  int i = p->contextStackTop++;
  Context *pContext;

  assert( i>=0 );
  /* FIX ME: This should be allocated as part of the vdbe at compile-time */
  if( i>=p->contextStackDepth ){
    p->contextStackDepth = i+1;
    p->contextStack = sqlite3DbReallocOrFree(db, p->contextStack,
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
case OP_ContextPop: {
  Context *pContext = &p->contextStack[--p->contextStackTop];
  assert( p->contextStackTop>=0 );
  db->lastRowid = pContext->lastRowid;
  p->nChange = pContext->nChange;
  sqlite3VdbeFifoClear(&p->sFifo);
  p->sFifo = pContext->sFifo;
  break;
}
#endif /* #ifndef SQLITE_OMIT_TRIGGER */

#ifndef SQLITE_OMIT_AUTOINCREMENT
/* Opcode: MemMax P1 P2 * * *
**
** Set the value of register P1 to the maximum of its current value
** and the value in register P2.
**
** This instruction throws an error if the memory cell is not initially
** an integer.
*/
case OP_MemMax: {        /* in1, in2 */
  sqlite3VdbeMemIntegerify(pIn1);
  sqlite3VdbeMemIntegerify(pIn2);
  if( pIn1->u.i<pIn2->u.i){
    pIn1->u.i = pIn2->u.i;
  }
  break;
}
#endif /* SQLITE_OMIT_AUTOINCREMENT */

/* Opcode: IfPos P1 P2 * * *
**
** If the value of register P1 is 1 or greater, jump to P2.
**
** It is illegal to use this instruction on a register that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfPos: {        /* jump, in1 */
  assert( pIn1->flags==MEM_Int );
  if( pIn1->u.i>0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: IfNeg P1 P2 * * *
**
** If the value of register P1 is less than zero, jump to P2. 
**
** It is illegal to use this instruction on a register that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfNeg: {        /* jump, in1 */
  assert( pIn1->flags==MEM_Int );
  if( pIn1->u.i<0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: IfZero P1 P2 * * *
**
** If the value of register P1 is exactly 0, jump to P2. 
**
** It is illegal to use this instruction on a register that does
** not contain an integer.  An assertion fault will result if you try.
*/
case OP_IfZero: {        /* jump, in1 */
  assert( pIn1->flags==MEM_Int );
  if( pIn1->u.i==0 ){
     pc = pOp->p2 - 1;
  }
  break;
}

/* Opcode: AggStep * P2 P3 P4 P5
**
** Execute the step function for an aggregate.  The
** function has P5 arguments.   P4 is a pointer to the FuncDef
** structure that specifies the function.  Use register
** P3 as the accumulator.
**
** The P5 arguments are taken from register P2 and its
** successors.
*/
case OP_AggStep: {
  int n = pOp->p5;
  int i;
  Mem *pMem, *pRec;
  sqlite3_context ctx;
  sqlite3_value **apVal;

  assert( n>=0 );
  pRec = &p->aMem[pOp->p2];
  apVal = p->apArg;
  assert( apVal || n==0 );
  for(i=0; i<n; i++, pRec++){
    apVal[i] = pRec;
    storeTypeInfo(pRec, encoding);
  }
  ctx.pFunc = pOp->p4.pFunc;
  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  ctx.pMem = pMem = &p->aMem[pOp->p3];
  pMem->n++;
  ctx.s.flags = MEM_Null;
  ctx.s.z = 0;
  ctx.s.xDel = 0;
  ctx.s.db = db;
  ctx.isError = 0;
  ctx.pColl = 0;
  if( ctx.pFunc->needCollSeq ){
    assert( pOp>p->aOp );
    assert( pOp[-1].p4type==P4_COLLSEQ );
    assert( pOp[-1].opcode==OP_CollSeq );
    ctx.pColl = pOp[-1].p4.pColl;
  }
  (ctx.pFunc->xStep)(&ctx, n, apVal);
  if( ctx.isError ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(&ctx.s), (char*)0);
    rc = ctx.isError;
  }
  sqlite3VdbeMemRelease(&ctx.s);
  break;
}

/* Opcode: AggFinal P1 P2 * P4 *
**
** Execute the finalizer function for an aggregate.  P1 is
** the memory location that is the accumulator for the aggregate.
**
** P2 is the number of arguments that the step function takes and
** P4 is a pointer to the FuncDef for this function.  The P2
** argument is not used by this opcode.  It is only there to disambiguate
** functions that can take varying numbers of arguments.  The
** P4 argument is only needed for the degenerate case where
** the step function was not previously called.
*/
case OP_AggFinal: {
  Mem *pMem;
  assert( pOp->p1>0 && pOp->p1<=p->nMem );
  pMem = &p->aMem[pOp->p1];
  assert( (pMem->flags & ~(MEM_Null|MEM_Agg))==0 );
  rc = sqlite3VdbeMemFinalize(pMem, pOp->p4.pFunc);
  if( rc==SQLITE_ERROR ){
    sqlite3SetString(&p->zErrMsg, sqlite3_value_text(pMem), (char*)0);
  }
  UPDATE_MAX_BLOBSIZE(pMem);
  if( sqlite3VdbeMemTooBig(pMem) ){
    goto too_big;
  }
  break;
}


#if !defined(SQLITE_OMIT_VACUUM) && !defined(SQLITE_OMIT_ATTACH)
/* Opcode: Vacuum * * * * *
**
** Vacuum the entire database.  This opcode will cause other virtual
** machines to be created and run.  It may not be called from within
** a transaction.
*/
case OP_Vacuum: {
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse; 
  rc = sqlite3RunVacuum(&p->zErrMsg, db);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  break;
}
#endif

#if !defined(SQLITE_OMIT_AUTOVACUUM)
/* Opcode: IncrVacuum P1 P2 * * *
**
** Perform a single step of the incremental vacuum procedure on
** the P1 database. If the vacuum has finished, jump to instruction
** P2. Otherwise, fall through to the next instruction.
*/
case OP_IncrVacuum: {        /* jump */
  Btree *pBt;

  assert( pOp->p1>=0 && pOp->p1<db->nDb );
  assert( (p->btreeMask & (1<<pOp->p1))!=0 );
  pBt = db->aDb[pOp->p1].pBt;
  rc = sqlite3BtreeIncrVacuum(pBt);
  if( rc==SQLITE_DONE ){
    pc = pOp->p2 - 1;
    rc = SQLITE_OK;
  }
  break;
}
#endif

/* Opcode: Expire P1 * * * *
**
** Cause precompiled statements to become expired. An expired statement
** fails with an error code of SQLITE_SCHEMA if it is ever executed 
** (via sqlite3_step()).
** 
** If P1 is 0, then all SQL statements become expired. If P1 is non-zero,
** then only the currently executing statement is affected. 
*/
case OP_Expire: {
  if( !pOp->p1 ){
    sqlite3ExpirePreparedStatements(db);
  }else{
    p->expired = 1;
  }
  break;
}

#ifndef SQLITE_OMIT_SHARED_CACHE
/* Opcode: TableLock P1 P2 * P4 *
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
** P4 contains a pointer to the name of the table being locked. This is only
** used to generate an error message if the lock cannot be obtained.
*/
case OP_TableLock: {
  int p1 = pOp->p1; 
  u8 isWriteLock = (p1<0);
  if( isWriteLock ){
    p1 = (-1*p1)-1;
  }
  assert( p1>=0 && p1<db->nDb );
  assert( (p->btreeMask & (1<<p1))!=0 );
  rc = sqlite3BtreeLockTable(db->aDb[p1].pBt, pOp->p2, isWriteLock);
  if( rc==SQLITE_LOCKED ){
    const char *z = pOp->p4.z;
    sqlite3SetString(&p->zErrMsg, "database table is locked: ", z, (char*)0);
  }
  break;
}
#endif /* SQLITE_OMIT_SHARED_CACHE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VBegin * * * P4 *
**
** P4 a pointer to an sqlite3_vtab structure. Call the xBegin method 
** for that table.
*/
case OP_VBegin: {
  rc = sqlite3VtabBegin(db, pOp->p4.pVtab);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VCreate P1 * * P4 *
**
** P4 is the name of a virtual table in database P1. Call the xCreate method
** for that table.
*/
case OP_VCreate: {
  rc = sqlite3VtabCallCreate(db, pOp->p1, pOp->p4.z, &p->zErrMsg);
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VDestroy P1 * * P4 *
**
** P4 is the name of a virtual table in database P1.  Call the xDestroy method
** of that table.
*/
case OP_VDestroy: {
  p->inVtabMethod = 2;
  rc = sqlite3VtabCallDestroy(db, pOp->p1, pOp->p4.z);
  p->inVtabMethod = 0;
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VOpen P1 * * P4 *
**
** P4 is a pointer to a virtual table object, an sqlite3_vtab structure.
** P1 is a cursor number.  This opcode opens a cursor to the virtual
** table and stores that cursor in P1.
*/
case OP_VOpen: {
  Cursor *pCur = 0;
  sqlite3_vtab_cursor *pVtabCursor = 0;

  sqlite3_vtab *pVtab = pOp->p4.pVtab;
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
      db->mallocFailed = 1;
      pModule->xClose(pVtabCursor);
    }
  }
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VFilter P1 P2 P3 P4 *
**
** P1 is a cursor opened using VOpen.  P2 is an address to jump to if
** the filtered result set is empty.
**
** P4 is either NULL or a string that was generated by the xBestIndex
** method of the module.  The interpretation of the P4 string is left
** to the module implementation.
**
** This opcode invokes the xFilter method on the virtual table specified
** by P1.  The integer query plan parameter to xFilter is stored in register
** P3. Register P3+1 stores the argc parameter to be passed to the
** xFilter method. Registers P3+2..P3+1+argc are the argc additional
** parametersneath additional parameters which are passed to
** xFilter as argv. Register P3+2 becomes argv[0] when passed to xFilter.
**
** A jump is made to P2 if the result set after filtering would be empty.
*/
case OP_VFilter: {   /* jump */
  int nArg;
  int iQuery;
  const sqlite3_module *pModule;
  Mem *pQuery = &p->aMem[pOp->p3];
  Mem *pArgc = &pQuery[1];

  Cursor *pCur = p->apCsr[pOp->p1];

  REGISTER_TRACE(pOp->p3, pQuery);
  assert( pCur->pVtabCursor );
  pModule = pCur->pVtabCursor->pVtab->pModule;

  /* Grab the index number and argc parameters */
  assert( (pQuery->flags&MEM_Int)!=0 && pArgc->flags==MEM_Int );
  nArg = pArgc->u.i;
  iQuery = pQuery->u.i;

  /* Invoke the xFilter method */
  {
    int res = 0;
    int i;
    Mem **apArg = p->apArg;
    for(i = 0; i<nArg; i++){
      apArg[i] = &pArgc[i+1];
      storeTypeInfo(apArg[i], 0);
    }

    if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
    p->inVtabMethod = 1;
    rc = pModule->xFilter(pCur->pVtabCursor, iQuery, pOp->p4.z, nArg, apArg);
    p->inVtabMethod = 0;
    if( rc==SQLITE_OK ){
      res = pModule->xEof(pCur->pVtabCursor);
    }
    if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;

    if( res ){
      pc = pOp->p2 - 1;
    }
  }
  pCur->nullRow = 0;

  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VRowid P1 P2 * * *
**
** Store into register P2  the rowid of
** the virtual-table that the P1 cursor is pointing to.
*/
case OP_VRowid: {             /* out2-prerelease */
  const sqlite3_module *pModule;
  sqlite_int64 iRow;
  Cursor *pCur = p->apCsr[pOp->p1];

  assert( pCur->pVtabCursor );
  if( pCur->nullRow ){
    break;
  }
  pModule = pCur->pVtabCursor->pVtab->pModule;
  assert( pModule->xRowid );
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  rc = pModule->xRowid(pCur->pVtabCursor, &iRow);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;
  pOut->flags = MEM_Int;
  pOut->u.i = iRow;
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VColumn P1 P2 P3 * *
**
** Store the value of the P2-th column of
** the row of the virtual-table that the 
** P1 cursor is pointing to into register P3.
*/
case OP_VColumn: {
  const sqlite3_module *pModule;
  Mem *pDest;
  sqlite3_context sContext;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  assert( pOp->p3>0 && pOp->p3<=p->nMem );
  pDest = &p->aMem[pOp->p3];
  if( pCur->nullRow ){
    sqlite3VdbeMemSetNull(pDest);
    break;
  }
  pModule = pCur->pVtabCursor->pVtab->pModule;
  assert( pModule->xColumn );
  memset(&sContext, 0, sizeof(sContext));
  sContext.s.flags = MEM_Null;
  sContext.s.db = db;
  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  rc = pModule->xColumn(pCur->pVtabCursor, &sContext, pOp->p2);

  /* Copy the result of the function to the P3 register. We
  ** do this regardless of whether or not an error occured to ensure any
  ** dynamic allocation in sContext.s (a Mem struct) is  released.
  */
  sqlite3VdbeChangeEncoding(&sContext.s, encoding);
  REGISTER_TRACE(pOp->p3, pDest);
  sqlite3VdbeMemMove(pDest, &sContext.s);
  UPDATE_MAX_BLOBSIZE(pDest);

  if( sqlite3SafetyOn(db) ){
    goto abort_due_to_misuse;
  }
  if( sqlite3VdbeMemTooBig(pDest) ){
    goto too_big;
  }
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VNext P1 P2 * * *
**
** Advance virtual table P1 to the next row in its result set and
** jump to instruction P2.  Or, if the virtual table has reached
** the end of its result set, then fall through to the next instruction.
*/
case OP_VNext: {   /* jump */
  const sqlite3_module *pModule;
  int res = 0;

  Cursor *pCur = p->apCsr[pOp->p1];
  assert( pCur->pVtabCursor );
  if( pCur->nullRow ){
    break;
  }
  pModule = pCur->pVtabCursor->pVtab->pModule;
  assert( pModule->xNext );

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
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VRename P1 * * P4 *
**
** P4 is a pointer to a virtual table object, an sqlite3_vtab structure.
** This opcode invokes the corresponding xRename method. The value
** in register P1 is passed as the zName argument to the xRename method.
*/
case OP_VRename: {
  sqlite3_vtab *pVtab = pOp->p4.pVtab;
  Mem *pName = &p->aMem[pOp->p1];
  assert( pVtab->pModule->xRename );
  REGISTER_TRACE(pOp->p1, pName);

  Stringify(pName, encoding);

  if( sqlite3SafetyOff(db) ) goto abort_due_to_misuse;
  sqlite3VtabLock(pVtab);
  rc = pVtab->pModule->xRename(pVtab, pName->z);
  sqlite3VtabUnlock(db, pVtab);
  if( sqlite3SafetyOn(db) ) goto abort_due_to_misuse;

  break;
}
#endif

#ifndef SQLITE_OMIT_VIRTUALTABLE
/* Opcode: VUpdate P1 P2 P3 P4 *
**
** P4 is a pointer to a virtual table object, an sqlite3_vtab structure.
** This opcode invokes the corresponding xUpdate method. P2 values
** are contiguous memory cells starting at P3 to pass to the xUpdate 
** invocation. The value in register (P3+P2-1) corresponds to the 
** p2th element of the argv array passed to xUpdate.
**
** The xUpdate method will do a DELETE or an INSERT or both.
** The argv[0] element (which corresponds to memory cell P3)
** is the rowid of a row to delete.  If argv[0] is NULL then no 
** deletion occurs.  The argv[1] element is the rowid of the new 
** row.  This can be NULL to have the virtual table select the new 
** rowid for itself.  The subsequent elements in the array are 
** the values of columns in the new row.
**
** If P2==1 then no insert is performed.  argv[0] is the rowid of
** a row to delete.
**
** P1 is a boolean flag. If it is set to true and the xUpdate call
** is successful, then the value returned by sqlite3_last_insert_rowid() 
** is set to the value of the rowid for the row just inserted.
*/
case OP_VUpdate: {
  sqlite3_vtab *pVtab = pOp->p4.pVtab;
  sqlite3_module *pModule = (sqlite3_module *)pVtab->pModule;
  int nArg = pOp->p2;
  assert( pOp->p4type==P4_VTAB );
  if( pModule->xUpdate==0 ){
    sqlite3SetString(&p->zErrMsg, "read-only table", 0);
    rc = SQLITE_ERROR;
  }else{
    int i;
    sqlite_int64 rowid;
    Mem **apArg = p->apArg;
    Mem *pX = &p->aMem[pOp->p3];
    for(i=0; i<nArg; i++){
      storeTypeInfo(pX, 0);
      apArg[i] = pX;
      pX++;
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
  break;
}
#endif /* SQLITE_OMIT_VIRTUALTABLE */

#ifndef SQLITE_OMIT_TRACE
/* Opcode: Trace * * * P4 *
**
** If tracing is enabled (by the sqlite3_trace()) interface, then
** the UTF-8 string contained in P4 is emitted on the trace callback.
*/
case OP_Trace: {
  if( pOp->p4.z ){
    if( db->xTrace ){
      db->xTrace(db->pTraceArg, pOp->p4.z);
    }
#ifdef SQLITE_DEBUG
    if( (db->flags & SQLITE_SqlTrace)!=0 ){
      sqlite3DebugPrintf("SQL-trace: %s\n", pOp->p4.z);
    }
#endif /* SQLITE_DEBUG */
  }
  break;
}
#endif


/* Opcode: Noop * * * * *
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
default: {          /* This is really OP_Noop and OP_Explain */
  break;
}

/*****************************************************************************
** The cases of the switch statement above this line should all be indented
** by 6 spaces.  But the left-most 6 spaces have been removed to improve the
** readability.  From this point on down, the normal indentation rules are
** restored.
*****************************************************************************/
    }

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
    assert( pc>=-1 && pc<p->nOp );

#ifdef SQLITE_DEBUG
    if( p->trace ){
      if( rc!=0 ) fprintf(p->trace,"rc=%d\n",rc);
      if( opProperty & OPFLG_OUT2_PRERELEASE ){
        registerTrace(p->trace, pOp->p2, pOut);
      }
      if( opProperty & OPFLG_OUT3 ){
        registerTrace(p->trace, pOp->p3, pOut);
      }
    }
#endif  /* SQLITE_DEBUG */
#endif  /* NDEBUG */
  }  /* The end of the for(;;) loop the loops through opcodes */

  /* If we reach this point, it means that execution is finished with
  ** an error of some kind.
  */
vdbe_error_halt:
  assert( rc );
  p->rc = rc;
  rc = SQLITE_ERROR;
  sqlite3VdbeHalt(p);

  /* This is the only way out of this procedure.  We have to
  ** release the mutexes on btrees that were acquired at the
  ** top. */
vdbe_return:
  sqlite3BtreeMutexArrayLeave(&p->aMutex);
  return rc;

  /* Jump to here if a string or blob larger than SQLITE_MAX_LENGTH
  ** is encountered.
  */
too_big:
  sqlite3SetString(&p->zErrMsg, "string or blob too big", (char*)0);
  rc = SQLITE_TOOBIG;
  goto vdbe_error_halt;

  /* Jump to here if a malloc() fails.
  */
no_mem:
  db->mallocFailed = 1;
  sqlite3SetString(&p->zErrMsg, "out of memory", (char*)0);
  rc = SQLITE_NOMEM;
  goto vdbe_error_halt;

  /* Jump to here for an SQLITE_MISUSE error.
  */
abort_due_to_misuse:
  rc = SQLITE_MISUSE;
  /* Fall thru into abort_due_to_error */

  /* Jump to here for any other kind of fatal error.  The "rc" variable
  ** should hold the error number.
  */
abort_due_to_error:
  assert( p->zErrMsg==0 );
  if( db->mallocFailed ) rc = SQLITE_NOMEM;
  sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(rc), (char*)0);
  goto vdbe_error_halt;

  /* Jump to here if the sqlite3_interrupt() API sets the interrupt
  ** flag.
  */
abort_due_to_interrupt:
  assert( db->u1.isInterrupted );
  rc = SQLITE_INTERRUPT;
  p->rc = rc;
  sqlite3SetString(&p->zErrMsg, sqlite3ErrStr(rc), (char*)0);
  goto vdbe_error_halt;
}
