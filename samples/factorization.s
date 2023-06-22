	.text
	.file	"factorization"
	.globl	factorization           # -- Begin function factorization
	.p2align	4, 0x90
	.type	factorization,@function
factorization:                          # @factorization
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, 12(%rsp)
	callq	writeln
	movl	12(%rsp), %eax
	cmpl	$4, %eax
	jge	.LBB0_2
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_3:                                # %loop
                                        #   in Loop: Header=BB0_2 Depth=1
	movl	$2, %edi
	callq	writeln
	movl	12(%rsp), %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	sarl	%ecx
	movl	%ecx, 12(%rsp)
.LBB0_2:                                # %condition
                                        # =>This Inner Loop Header: Depth=1
	movl	12(%rsp), %eax
	movl	%eax, %ecx
	shrl	$31, %ecx
	addl	%eax, %ecx
	andl	$-2, %ecx
	cmpl	%ecx, %eax
	je	.LBB0_3
	jmp	.LBB0_4
	.p2align	4, 0x90
.LBB0_5:                                # %loop8
                                        #   in Loop: Header=BB0_4 Depth=1
	movl	$3, %edi
	callq	writeln
	movslq	12(%rsp), %rax
	imulq	$1431655766, %rax, %rax # imm = 0x55555556
	movq	%rax, %rcx
	shrq	$63, %rcx
	shrq	$32, %rax
	addl	%ecx, %eax
	movl	%eax, 12(%rsp)
.LBB0_4:                                # %condition7
                                        # =>This Inner Loop Header: Depth=1
	movslq	12(%rsp), %rax
	imulq	$1431655766, %rax, %rcx # imm = 0x55555556
	movq	%rcx, %rdx
	shrq	$63, %rdx
	shrq	$32, %rcx
	addl	%edx, %ecx
	leal	(%rcx,%rcx,2), %ecx
	cmpl	%ecx, %eax
	je	.LBB0_5
# %bb.6:                                # %after_loop9
	movl	12(%rsp), %eax
	movl	%eax, 20(%rsp)
	movl	$5, 16(%rsp)
	jmp	.LBB0_7
	.p2align	4, 0x90
.LBB0_15:                               # %after_loop35
                                        #   in Loop: Header=BB0_7 Depth=1
	addl	$4, 16(%rsp)
.LBB0_7:                                # %condition16
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB0_10 Depth 2
                                        #     Child Loop BB0_13 Depth 2
	movl	16(%rsp), %eax
	cmpl	20(%rsp), %eax
	jle	.LBB0_10
	jmp	.LBB0_8
	.p2align	4, 0x90
.LBB0_11:                               # %loop22
                                        #   in Loop: Header=BB0_10 Depth=2
	movl	16(%rsp), %edi
	callq	writeln
	movl	12(%rsp), %eax
	cltd
	idivl	16(%rsp)
	movl	%eax, 12(%rsp)
.LBB0_10:                               # %condition21
                                        #   Parent Loop BB0_7 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	12(%rsp), %eax
	cltd
	idivl	16(%rsp)
	testl	%edx, %edx
	je	.LBB0_11
# %bb.12:                               # %after_loop23
                                        #   in Loop: Header=BB0_7 Depth=1
	addl	$2, 16(%rsp)
	jmp	.LBB0_13
	.p2align	4, 0x90
.LBB0_14:                               # %loop34
                                        #   in Loop: Header=BB0_13 Depth=2
	movl	16(%rsp), %edi
	callq	writeln
	movl	12(%rsp), %eax
	cltd
	idivl	16(%rsp)
	movl	%eax, 12(%rsp)
.LBB0_13:                               # %condition33
                                        #   Parent Loop BB0_7 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movl	12(%rsp), %eax
	cltd
	idivl	16(%rsp)
	testl	%edx, %edx
	je	.LBB0_14
	jmp	.LBB0_15
.LBB0_8:                                # %after_loop18
	movl	12(%rsp), %eax
	cmpl	$1, %eax
	je	.LBB0_9
.LBB0_1:                                # %then
	movl	12(%rsp), %edi
	callq	writeln
	addq	$24, %rsp
	retq
.LBB0_9:                                # %functionReturn
	addq	$24, %rsp
	retq
.Lfunc_end0:
	.size	factorization, .Lfunc_end0-factorization
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
	callq	factorization
	movl	$1, %edi
	callq	factorization
	movl	$2, %edi
	callq	factorization
	movl	$3, %edi
	callq	factorization
	movl	$4, %edi
	callq	factorization
	movl	$5, %edi
	callq	factorization
	movl	$6, %edi
	callq	factorization
	movl	$7, %edi
	callq	factorization
	movl	$8, %edi
	callq	factorization
	movl	$9, %edi
	callq	factorization
	movl	$10, %edi
	callq	factorization
	movl	$11, %edi
	callq	factorization
	movl	$12, %edi
	callq	factorization
	movl	$13, %edi
	callq	factorization
	movl	$14, %edi
	callq	factorization
	movl	$15, %edi
	callq	factorization
	movl	$16, %edi
	callq	factorization
	movl	$17, %edi
	callq	factorization
	movl	$100, %edi
	callq	factorization
	movl	$131, %edi
	callq	factorization
	movl	$133, %edi
	callq	factorization
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
