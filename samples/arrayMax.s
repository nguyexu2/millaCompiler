	.text
	.file	"arrayMax"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movabsq	$283467841547, %rax     # imm = 0x420000000B
	movq	%rax, X(%rip)
	movabsq	$210453397632, %rax     # imm = 0x3100000080
	movq	%rax, X+8(%rip)
	movabsq	$197568495749, %rax     # imm = 0x2E00000085
	movq	%rax, X+16(%rip)
	movabsq	$373662154767, %rax     # imm = 0x570000000F
	movq	%rax, X+24(%rip)
	movabsq	$158913790007, %rax     # imm = 0x2500000037
	movq	%rax, X+32(%rip)
	movabsq	$188978561102, %rax     # imm = 0x2C0000004E
	movq	%rax, X+40(%rip)
	movabsq	$163208757281, %rax     # imm = 0x2600000021
	movq	%rax, X+48(%rip)
	movabsq	$25769803861, %rax      # imm = 0x600000055
	movq	%rax, X+56(%rip)
	movabsq	$17179869334, %rax      # imm = 0x400000096
	movq	%rax, X+64(%rip)
	movabsq	$236223201281, %rax     # imm = 0x3700000001
	movq	%rax, X+72(%rip)
	movl	$78, X+80(%rip)
	movl	$0, I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movslq	I(%rip), %rax
	movl	X(,%rax,4), %edi
	callq	writeln
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_2
.LBB0_3:                                # %after_loop
	movl	X(%rip), %eax
	movl	%eax, MAX(%rip)
	movl	$1, I(%rip)
	cmpl	$20, I(%rip)
	jle	.LBB0_5
	jmp	.LBB0_8
	.p2align	4, 0x90
.LBB0_7:                                # %after_if
                                        #   in Loop: Header=BB0_5 Depth=1
	incl	I(%rip)
	cmpl	$20, I(%rip)
	jg	.LBB0_8
.LBB0_5:                                # %loop4
                                        # =>This Inner Loop Header: Depth=1
	movl	MAX(%rip), %eax
	movslq	I(%rip), %rcx
	cmpl	X(,%rcx,4), %eax
	jge	.LBB0_7
# %bb.6:                                # %then
                                        #   in Loop: Header=BB0_5 Depth=1
	movslq	I(%rip), %rax
	movl	X(,%rax,4), %eax
	movl	%eax, MAX(%rip)
	jmp	.LBB0_7
.LBB0_8:                                # %functionReturn
	movl	MAX(%rip), %edi
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

	.type	MAX,@object             # @MAX
	.globl	MAX
	.p2align	2
MAX:
	.long	0                       # 0x0
	.size	MAX, 4

	.type	X,@object               # @X
	.globl	X
	.p2align	4
X:
	.zero	84
	.size	X, 84


	.section	".note.GNU-stack","",@progbits
