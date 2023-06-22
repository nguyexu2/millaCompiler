	.text
	.file	"factorialCycle"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$1, f(%rip)
	movl	$n, %edi
	callq	readln
	cmpl	$2, n(%rip)
	jl	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movl	n(%rip), %eax
	movl	f(%rip), %ecx
	imull	%eax, %ecx
	movl	%ecx, f(%rip)
	decl	%eax
	movl	%eax, n(%rip)
	cmpl	$2, n(%rip)
	jge	.LBB0_2
.LBB0_3:                                # %after_loop
	movl	f(%rip), %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	n,@object               # @n
	.bss
	.globl	n
	.p2align	2
n:
	.long	0                       # 0x0
	.size	n, 4

	.type	f,@object               # @f
	.globl	f
	.p2align	2
f:
	.long	0                       # 0x0
	.size	f, 4


	.section	".note.GNU-stack","",@progbits
