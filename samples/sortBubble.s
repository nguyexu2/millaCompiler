	.text
	.file	"sortBubble"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$0, I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movslq	I(%rip), %rax
	movl	$20, %ecx
	subl	%eax, %ecx
	movl	%ecx, X(,%rax,4)
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_2
.LBB0_3:                                # %after_loop
	movl	$0, I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_6
	.p2align	4, 0x90
.LBB0_5:                                # %loop5
                                        # =>This Inner Loop Header: Depth=1
	movslq	I(%rip), %rax
	movl	X(,%rax,4), %edi
	callq	writeln
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_5
.LBB0_6:                                # %after_loop6
	movl	$1, I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_8
	jmp	.LBB0_13
	.p2align	4, 0x90
.LBB0_17:                               # %after_loop19
                                        #   in Loop: Header=BB0_8 Depth=1
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_13
.LBB0_8:                                # %loop13
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_9 Depth 2
	movl	$20, J(%rip)
	jmp	.LBB0_9
	.p2align	4, 0x90
.LBB0_12:                               # %after_if
                                        #   in Loop: Header=BB0_9 Depth=2
	decl	J(%rip)
.LBB0_9:                                # %condition17
                                        #   Parent Loop BB0_8 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	J(%rip), %eax
	cmpl	I(%rip), %eax
	jl	.LBB0_17
# %bb.10:                               # %loop18
                                        #   in Loop: Header=BB0_9 Depth=2
	movslq	J(%rip), %rax
	movl	X(,%rax,4), %ecx
	leal	-1(%rax), %eax
	cltq
	cmpl	X(,%rax,4), %ecx
	jge	.LBB0_12
# %bb.11:                               # %then
                                        #   in Loop: Header=BB0_9 Depth=2
	movl	J(%rip), %eax
	decl	%eax
	cltq
	movl	X(,%rax,4), %eax
	movl	%eax, TEMP(%rip)
	movslq	J(%rip), %rax
	leal	-1(%rax), %ecx
	movslq	%ecx, %rcx
	movl	X(,%rax,4), %eax
	movl	%eax, X(,%rcx,4)
	movslq	J(%rip), %rax
	movl	TEMP(%rip), %ecx
	movl	%ecx, X(,%rax,4)
	jmp	.LBB0_12
.LBB0_13:                               # %after_loop14
	movl	$0, I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_16
	.p2align	4, 0x90
.LBB0_15:                               # %loop35
                                        # =>This Inner Loop Header: Depth=1
	movslq	I(%rip), %rax
	movl	X(,%rax,4), %edi
	callq	writeln
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_15
.LBB0_16:                               # %functionReturn
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

	.type	J,@object               # @J
	.globl	J
	.p2align	2
J:
	.long	0                       # 0x0
	.size	J, 4

	.type	TEMP,@object            # @TEMP
	.globl	TEMP
	.p2align	2
TEMP:
	.long	0                       # 0x0
	.size	TEMP, 4

	.type	X,@object               # @X
	.globl	X
	.p2align	4
X:
	.zero	84
	.size	X, 84


	.section	".note.GNU-stack","",@progbits
