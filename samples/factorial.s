	.text
	.file	"factorial"
	.globl	facti                   # -- Begin function facti
	.p2align	4, 0x90
	.type	facti,@function
facti:                                  # @facti
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -8(%rsp)
	movl	$1, -4(%rsp)
	jmp	.LBB0_1
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        #   in Loop: Header=BB0_1 Depth=1
	movl	-8(%rsp), %eax
	movl	-4(%rsp), %ecx
	imull	%eax, %ecx
	movl	%ecx, -4(%rsp)
	decl	%eax
	movl	%eax, -8(%rsp)
.LBB0_1:                                # %condition
                                        # =>This Inner Loop Header: Depth=1
	movl	-8(%rsp), %eax
	cmpl	$2, %eax
	jge	.LBB0_2
# %bb.3:                                # %functionReturn
	movl	-4(%rsp), %eax
	retq
.Lfunc_end0:
	.size	facti, .Lfunc_end0-facti
	.cfi_endproc
                                        # -- End function
	.globl	factr                   # -- Begin function factr
	.p2align	4, 0x90
	.type	factr,@function
factr:                                  # @factr
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rbx
	.cfi_def_cfa_offset 16
	subq	$16, %rsp
	.cfi_def_cfa_offset 32
	.cfi_offset %rbx, -16
	movl	%edi, 12(%rsp)
	cmpl	$1, %edi
	jne	.LBB1_2
# %bb.1:                                # %then
	movl	$1, 8(%rsp)
	jmp	.LBB1_3
.LBB1_2:                                # %else
	movl	12(%rsp), %ebx
	leal	-1(%rbx), %edi
	callq	factr
	imull	%ebx, %eax
	movl	%eax, 8(%rsp)
.LBB1_3:                                # %functionReturn
	movl	8(%rsp), %eax
	addq	$16, %rsp
	popq	%rbx
	retq
.Lfunc_end1:
	.size	factr, .Lfunc_end1-factr
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
	movl	$5, %edi
	callq	facti
	movl	%eax, %edi
	callq	writeln
	movl	$5, %edi
	callq	factr
	movl	%eax, %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end2:
	.size	main, .Lfunc_end2-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
