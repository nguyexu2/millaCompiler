	.text
	.file	"whileTest"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$0, a(%rip)
	cmpl	$10, a(%rip)
	je	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movl	a(%rip), %edi
	callq	writeln
	addl	$2, a(%rip)
	cmpl	$10, a(%rip)
	jne	.LBB0_2
.LBB0_3:                                # %functionReturn
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	a,@object               # @a
	.bss
	.globl	a
	.p2align	2
a:
	.long	0                       # 0x0
	.size	a, 4


	.section	".note.GNU-stack","",@progbits
