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
.global	jump_fcontext
.hidden jump_fcontext
.type	jump_fcontext, @function

#define ARG_OFFSET         0
#define GR_OFFSET          16
#define R14_OFFSET         88
#define FP_OFFSET          96
#define FPC_OFFSET         160
#define PC_OFFSET          168
#define CONTEXT_SIZE       176

#define REG_SAVE_AREA_SIZE 160

/*

typedef void*   fcontext_t;

struct transfer_t {
   fcontext_t  fctx;
   void    *   data;
};

transfer_t jump_fcontext( fcontext_t const to,
			  void * data);

Incoming args
r2 - Hidden argument to the location where the return transfer_t needs to be returned
r3 - Context we want to switch to
r4 - Data pointer

*/

jump_fcontext:
	.machine "z10"
	/* Reserve stack space to store the current context.  */
	aghi	%r15,-CONTEXT_SIZE

	/* Save the argument register holding the location of the return value.  */
	stg	%r2,GR_OFFSET(%r15)

	/* Save the call-saved general purpose registers.  */
	stmg	%r6,%r14,GR_OFFSET+8(%r15)

	/* Save call-saved floating point registers.  */
	std	%f8,FP_OFFSET(%r15)
	std	%f9,FP_OFFSET+8(%r15)
	std	%f10,FP_OFFSET+16(%r15)
	std	%f11,FP_OFFSET+24(%r15)
	std	%f12,FP_OFFSET+32(%r15)
	std	%f13,FP_OFFSET+40(%r15)
	std	%f14,FP_OFFSET+48(%r15)
	std	%f15,FP_OFFSET+56(%r15)

	/* Save the return address as current pc.  */
	stg	%r14,PC_OFFSET(%r15)

	/* Save the floating point control register.  */
	stfpc	FPC_OFFSET(%r15)

	/* Backup the stack pointer pointing to the old context-data into r1.  */
	lgr	 %r1,%r15

	/* Load the new context pointer as stack pointer.  */
	lgr	%r15,%r3

	/* Restore the call-saved GPRs from the new context.  */
	lmg	%r6,%r14,GR_OFFSET+8(%r15)

	/* Restore call-saved floating point registers.  */
	ld	%f8,FP_OFFSET(%r15)
	ld	%f9,FP_OFFSET+8(%r15)
	ld	%f10,FP_OFFSET+16(%r15)
	ld	%f11,FP_OFFSET+24(%r15)
	ld	%f12,FP_OFFSET+32(%r15)
	ld	%f13,FP_OFFSET+40(%r15)
	ld	%f14,FP_OFFSET+48(%r15)
	ld	%f15,FP_OFFSET+56(%r15)

	/* Load the floating point control register.  */
	lfpc	FPC_OFFSET(%r15)

	/* Restore PC - the location where we will jump to at the end.  */
	lg	%r5,PC_OFFSET(%r15)

	ltg	%r2,GR_OFFSET(%r15)
	jnz	use_return_slot

	/* We're restoring a context created by make_fcontext.
	   This is going to be the argument of the entry point
	   of the fiber. We're placing it on top of the ABI
	   defined register save area of the fiber's own stack. */
	la	%r2,REG_SAVE_AREA_SIZE(%r15)

	/* REG_SAVE_AREA_SIZE + sizeof(transfer_t) */
	aghi	%r15,-(REG_SAVE_AREA_SIZE+16)

use_return_slot:
	/* Save the two fields in transfer_t.  When calling a
	   make_fcontext function this becomes the function argument of
	   the target function, otherwise it will be the return value of
	   jump_fcontext.  */
	stg	%r1,0(%r2)
	stg	%r4,8(%r2)

	/* Free the restored context.  */
	aghi	%r15,CONTEXT_SIZE

	/* Jump to the PC loaded from the new context.  */
	br	%r5


.size   jump_fcontext,.-jump_fcontext
.section .note.GNU-stack,"",%progbits
