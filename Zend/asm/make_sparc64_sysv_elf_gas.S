/*
       Copyright Claudio Jeker 2024
   Distributed under the Boost Software License, Version 1.0.
      (See accompanying file LICENSE_1_0.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

/*
 * fcontext_t *make_fcontext(void *sp, size_t size, void (*fn)(transfer_t));
 */
#define CC64FSZ 176
#define BIAS 2047
#define FP 112
#define SP 128
#define I7 136

.file "make_sparc64_sysv_elf_gas.S"
.text
.align  4
.global make_fcontext
.type   make_fcontext, %function
make_fcontext:
	save	%sp, -CC64FSZ, %sp

	# shift address in %i0 (allocated stack) to lower 16 byte boundary
	and	%i0, -0xf, %i0

	# reserve space for two frames on the stack
	# the first frame is for the call the second one holds the data
	# for jump_fcontext
	sub	%i0, 2 * CC64FSZ, %i0

	# third argument of make_fcontext() is the context-function to call
	# store it in the first stack frame, also clear %fp there to indicate
	# the end of the stack.
	stx	%i2, [%i0 + CC64FSZ + I7]
	stx	%g0, [%i0 + CC64FSZ + FP]

	# On OpenBSD stackghost prevents overriding the return address on
	# a stack frame. So this code uses an extra trampoline to load
	# to call the context-function and then do the _exit(0) dance.
	# Extract the full address of the trampoline via pc relative addressing
1:
	rd	%pc, %l0
	add	%l0, (trampoline - 1b - 8), %l0
	stx	%l0, [%i0 + I7]

	# Save framepointer to first stack frame but first substract the BIAS
	add	%i0, CC64FSZ - BIAS, %l0
	stx	%l0, [%i0 + SP]

	# Return context-data which is also includes the BIAS
	ret
	 restore %i0, -BIAS, %o0

trampoline:
	ldx	[%sp + BIAS + I7], %l0

	# no need to setup transfer_t, already in %o0 and %o1
	jmpl	%l0, %o7
	 nop

	call	_exit
	 clr	%o0
	unimp
.size	make_fcontext,.-make_fcontext
# Mark that we don't need executable stack.
.section .note.GNU-stack,"",%progbits
