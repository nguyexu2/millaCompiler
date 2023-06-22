	.text
	.file	"arrayAverage"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$-20, I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movl	I(%rip), %eax
	addl	$20, %eax
	cltq
	movl	$0, X(,%rax,4)
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_2
.LBB0_3:                                # %after_loop
	movl	$NUM, %edi
	callq	readln
	movl	$0, I(%rip)
	jmp	.LBB0_4
	.p2align	4, 0x90
.LBB0_5:                                # %loop4
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	$TEMP, %edi
	callq	readln
	movl	TEMP(%rip), %eax
	addl	$20, %eax
	cltq
	incl	X(,%rax,4)
	incl	I(%rip)
.LBB0_4:                                # %condition3
                                        # =>This Inner Loop Header: Depth=1
	movl	NUM(%rip), %eax
	decl	%eax
	cmpl	%eax, I(%rip)
	jle	.LBB0_5
# %bb.6:                                # %after_loop5
	movl	$0, SUM(%rip)
	movl	$20, I(%rip)
	cmpl	$-20, I(%rip)
	jl	.LBB0_9
	.p2align	4, 0x90
.LBB0_8:                                # %loop13
                                        # =>This Inner Loop Header: Depth=1
	movl	I(%rip), %eax
	leal	20(%rax), %ecx
	movslq	%ecx, %rcx
	movl	X(,%rcx,4), %ecx
	imull	%eax, %ecx
	addl	%ecx, SUM(%rip)
	decl	%eax
	movl	%eax, I(%rip)
	cmpl	$-20, I(%rip)
	jge	.LBB0_8
.LBB0_9:                                # %after_loop14
	movl	SUM(%rip), %eax
	cltd
	idivl	NUM(%rip)
	movl	%eax, %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	I,@object               # @I
	.bss
	.globl	I
	.p2align	2
I:
	.long	0                       # 0x0
	.size	I, 4

	.type	TEMP,@object            # @TEMP
	.globl	TEMP
	.p2align	2
TEMP:
	.long	0                       # 0x0
	.size	TEMP, 4

	.type	NUM,@object             # @NUM
	.globl	NUM
	.p2align	2
NUM:
	.long	0                       # 0x0
	.size	NUM, 4

	.type	SUM,@object             # @SUM
	.globl	SUM
	.p2align	2
SUM:
	.long	0                       # 0x0
	.size	SUM, 4

	.type	X,@object               # @X
	.globl	X
	.p2align	4
X:
	.zero	164
	.size	X, 164


	.section	".note.GNU-stack","",@progbits
