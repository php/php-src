	.text
.globl ___chkstk
___chkstk:
	pushq	%rax
	movq	%rax, %rcx
	movq	%rsp, %rdx
	call	_ruby_alloca_chkstk
	popq	%rax
	subq	%rax, %rsp
	ret
