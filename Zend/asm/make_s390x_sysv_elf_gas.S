/*******************************************************
 *  -------------------------------------------------  *
 *  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  *
 *  -------------------------------------------------  *
 *  |     0     |     8     |    16     |    24     |  *
 *  -------------------------------------------------  *
 *  |   t.fctx  |   t.data  |    r2     |    r6     |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
 *  -------------------------------------------------  *
 *  |     32    |    40     |     48    |     56    |  *
 *  -------------------------------------------------  *
 *  |     r7    |     r8    |     r9    |    r10    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
 *  -------------------------------------------------  *
 *  |     64    |     72    |     80    |     88    |  *
 *  -------------------------------------------------  *
 *  |    r11    |    r12    |    r13    |    r14    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  24 |  25 |  26 |  27 |  28 | 29  |  30 |  31 |  *
 *  -------------------------------------------------  *
 *  |     96    |    104    |    112    |    120    |  *
 *  -------------------------------------------------  *
 *  |     f8    |     f9    |    f10    |    f11    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  32 |  33 |  34 |  35 |  36 |  37 |  38 |  39 |  *
 *  -------------------------------------------------  *
 *  |    128    |    136    |    144    |    152    |  *
 *  -------------------------------------------------  *
 *  |    f12    |    f13    |    f14    |    f15    |  *
 *  -------------------------------------------------  *
 *  -------------------------------------------------  *
 *  |  40 |  41 |  42 |  43 |  44 |  45 |  46 |  47 |  *
 *  -------------------------------------------------  *
 *  |    160    |    168    |    176    |           |  *
 *  -------------------------------------------------  *
 *  |    fpc    |     pc    |           |           |  *
 *  -------------------------------------------------  *
 *******************************************************/

.text
.align	8
.global	make_fcontext
.hidden make_fcontext
.type	make_fcontext, @function

#define ARG_OFFSET         0
#define GR_OFFSET          16
#define R14_OFFSET         88
#define FP_OFFSET          96
#define FPC_OFFSET         160
#define PC_OFFSET          168
#define CONTEXT_SIZE       176

/*

fcontext_t make_fcontext( void * sp, std::size_t size, void (* fn)( transfer_t) );

Create and return a context below SP to call FN.

Incoming args
r2 - The stack location where to create the context
r3 - The size of the context
r4 - The address of the context function

*/

make_fcontext:
	.machine "z10"
	/* Align the stack to an 8 byte boundary.  */
	nill    %r2,0xfff8

	/* Allocate stack space for the context.  */
	aghi	%r2,-CONTEXT_SIZE

	/* Set the r2 save slot to zero.  This indicates jump_fcontext
	   that this is a special context.  */
	mvghi	GR_OFFSET(%r2),0

	/* Save the floating point control register.  */
	stfpc	FPC_OFFSET(%r2)

	/* Store the address of the target function as new pc.  */
	stg	%r4,PC_OFFSET(%r2)

	/* Store a pointer to the finish routine as r14. If a function
	   called via context routines just returns that value will be
	   loaded and used as return address.  Hence the program will
	   just exit.  */
	larl	%r1,finish
	stg	%r1,R14_OFFSET(%r2)

	/* Return as usual with the new context returned in r2.  */
	br	%r14

finish:
	/* In finish tasks, you load the exit code and exit the
	   make_fcontext This is called when the context-function is
	   entirely executed.  */
	lghi	%r2,0
	brasl	%r14,_exit@PLT

.size   make_fcontext,.-make_fcontext
.section .note.GNU-stack,"",%progbits
