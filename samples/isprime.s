	.text
	.file	"isprime"
	.globl	isprime                 # -- Begin function isprime
	.p2align	4, 0x90
	.type	isprime,@function
isprime:                                # @isprime
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -8(%rsp)
	cmpl	$2, %edi
	jl	.LBB0_10
# %bb.1:                                # %after_if
	movl	-8(%rsp), %eax
	cmpl	$3, %eax
	jg	.LBB0_3
# %bb.2:                                # %then3
	movl	$1, -12(%rsp)
	movl	-12(%rsp), %eax
	retq
.LBB0_3:                                # %after_if5
	movslq	-8(%rsp), %rax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	andl	$-2, %ecx
	cmpl	%ecx, %eax
	sete	%cl
	imulq	$1431655766, %rax, %rdx # imm = 0x55555556
	movq	%rdx, %rsi
	shrq	$63, %rsi
	shrq	$32, %rdx
	addl	%esi, %edx
	leal	(%rdx,%rdx,2), %edx
	cmpl	%edx, %eax
	sete	%al
	orb	%cl, %al
	cmpb	$1, %al
	jne	.LBB0_4
.LBB0_10:                               # %then49
	movl	$0, -12(%rsp)
.LBB0_11:                               # %functionReturn
	movl	-12(%rsp), %eax
	retq
.LBB0_4:                                # %after_if12
	movl	$1, -12(%rsp)
	movl	-8(%rsp), %eax
	movl	%eax, -4(%rsp)
	movl	$5, -16(%rsp)
	jmp	.LBB0_5
	.p2align	4, 0x90
.LBB0_12:                               # %after_if51
                                        #   in Loop: Header=BB0_5 Depth=1
	addl	$4, -16(%rsp)
.LBB0_5:                                # %condition
                                        # =>This Inner Loop Header: Depth=1
	movl	-16(%rsp), %eax
	cmpl	-4(%rsp), %eax
	jge	.LBB0_11
# %bb.6:                                # %loop
                                        #   in Loop: Header=BB0_5 Depth=1
	movl	-16(%rsp), %eax
	cmpl	-4(%rsp), %eax
	jl	.LBB0_11
# %bb.7:                                # %after_if25
                                        #   in Loop: Header=BB0_5 Depth=1
	movl	-8(%rsp), %eax
	cltd
	idivl	-16(%rsp)
	testl	%edx, %edx
	je	.LBB0_10
# %bb.8:                                # %after_if33
                                        #   in Loop: Header=BB0_5 Depth=1
	movl	-16(%rsp), %eax
	addl	$2, %eax
	movl	%eax, -16(%rsp)
	cmpl	-4(%rsp), %eax
	jl	.LBB0_11
# %bb.9:                                # %after_if43
                                        #   in Loop: Header=BB0_5 Depth=1
	movl	-8(%rsp), %eax
	cltd
	idivl	-16(%rsp)
	testl	%edx, %edx
	jne	.LBB0_12
	jmp	.LBB0_10
.Lfunc_end0:
	.size	isprime, .Lfunc_end0-isprime
	.cfi_endproc
                                        # -- End function
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	xorl	%edi, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$1, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$2, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$3, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$4, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$5, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$6, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$7, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$8, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$9, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$10, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$11, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$12, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$13, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$14, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$15, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$16, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	movl	$17, %edi
	callq	isprime
	movl	%eax, %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
