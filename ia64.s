// rb_ia64_flushrs and rb_ia64_bsp is written in IA64 assembly language
// because Intel Compiler for IA64 doesn't support inline assembly.
//
// This file is based on following C program compiled by gcc.
//
//   void rb_ia64_flushrs(void) { __builtin_ia64_flushrs(); }
//   void *rb_ia64_bsp(void) { return __builtin_ia64_bsp(); }
// 
// Note that rb_ia64_flushrs and rb_ia64_bsp works in its own stack frame.
// It's because BSP is updated by br.call/brl.call (not alloc instruction).
// So rb_ia64_flushrs flushes stack frames including caller's one.
// rb_ia64_bsp returns the address next to caller's register stack frame. 
//
// See also
// Intel Itanium Architecture Software Developer's Manual
// Volume 2: System Architecture.
//
	.file	"ia64.c"
	.text
	.align 16
	.global rb_ia64_flushrs#
	.proc rb_ia64_flushrs#
rb_ia64_flushrs:
	.prologue
	.body
	flushrs
	;;
	nop.i 0
	br.ret.sptk.many b0
	.endp rb_ia64_flushrs#
	.align 16
	.global rb_ia64_bsp#
	.proc rb_ia64_bsp#
rb_ia64_bsp:
	.prologue
	.body
	nop.m 0
	;;
	mov r8 = ar.bsp
	br.ret.sptk.many b0
	.endp rb_ia64_bsp#
	.ident	"GCC: (GNU) 3.3.5 (Debian 1:3.3.5-13)"
