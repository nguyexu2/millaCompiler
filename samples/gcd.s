	.text
	.file	"gcd"
	.globl	gcdi                    # -- Begin function gcdi
	.p2align	4, 0x90
	.type	gcdi,@function
gcdi:                                   # @gcdi
	.cfi_startproc
# %bb.0:                                # %entry
	movl	%edi, -16(%rsp)
	movl	%esi, -12(%rsp)
	cmpl	$0, -12(%rsp)
	je	.LBB0_3
	.p2align	4, 0x90
.LBB0_2:                                # %loop
                                        # =>This Inner Loop Header: Depth=1
	movl	-12(%rsp), %ecx
	movl	%ecx, -4(%rsp)
	movl	-16(%rsp), %eax
	cltd
	idivl	%ecx
	movl	%edx, -12(%rsp)
	movl	%ecx, -16(%rsp)
	cmpl	$0, -12(%rsp)
	jne	.LBB0_2
.LBB0_3:                                # %after_loop
	movl	-16(%rsp), %eax
	movl	%eax, -8(%rsp)
	movl	-8(%rsp), %eax
	retq
.Lfunc_end0:
	.size	gcdi, .Lfunc_end0-gcdi
	.cfi_endproc
                                        # -- End function
	.globl	gcdr                    # -- Begin function gcdr
	.p2align	4, 0x90
	.type	gcdr,@function
gcdr:                                   # @gcdr
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, 20(%rsp)
	movl	%esi, 8(%rsp)
	movl	%edi, %eax
	cltd
	idivl	%esi
	movl	%edx, 16(%rsp)
	testl	%edx, %edx
	jne	.LBB1_2
# %bb.1:                                # %then
	movl	8(%rsp), %eax
	jmp	.LBB1_3
.LBB1_2:                                # %after_if
	movl	8(%rsp), %edi
	movl	16(%rsp), %esi
	callq	gcdr
.LBB1_3:                                # %functionReturn
	movl	%eax, 12(%rsp)
	movl	12(%rsp), %eax
	addq	$24, %rsp
	retq
.Lfunc_end1:
	.size	gcdr, .Lfunc_end1-gcdr
	.cfi_endproc
                                        # -- End function
	.globl	gcdr_guessing_inner     # -- Begin function gcdr_guessing_inner
	.p2align	4, 0x90
	.type	gcdr_guessing_inner,@function
gcdr_guessing_inner:                    # @gcdr_guessing_inner
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edx, %ecx
	movl	%edi, 16(%rsp)
	movl	%esi, 20(%rsp)
	movl	%ecx, 8(%rsp)
	movl	%edi, %eax
	cltd
	idivl	%ecx
	movl	%edx, %edi
	movl	%esi, %eax
	cltd
	idivl	%ecx
	orl	%edi, %edx
	jne	.LBB2_2
# %bb.1:                                # %then
	movl	8(%rsp), %eax
	jmp	.LBB2_3
.LBB2_2:                                # %after_if
	movl	16(%rsp), %edi
	movl	20(%rsp), %esi
	movl	8(%rsp), %edx
	decl	%edx
	callq	gcdr_guessing_inner
.LBB2_3:                                # %functionReturn
	movl	%eax, 12(%rsp)
	movl	12(%rsp), %eax
	addq	$24, %rsp
	retq
.Lfunc_end2:
	.size	gcdr_guessing_inner, .Lfunc_end2-gcdr_guessing_inner
	.cfi_endproc
                                        # -- End function
	.globl	gcdr_guessing           # -- Begin function gcdr_guessing
	.p2align	4, 0x90
	.type	gcdr_guessing,@function
gcdr_guessing:                          # @gcdr_guessing
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$24, %rsp
	.cfi_def_cfa_offset 32
	movl	%edi, 16(%rsp)
	movl	%esi, 20(%rsp)
	movl	%esi, %edx
	callq	gcdr_guessing_inner
	movl	%eax, 12(%rsp)
	movl	12(%rsp), %eax
	addq	$24, %rsp
	retq
.Lfunc_end3:
	.size	gcdr_guessing, .Lfunc_end3-gcdr_guessing
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
	movl	$54, %edi
	movl	$81, %esi
	callq	gcdi
	movl	%eax, %edi
	callq	writeln
	movl	$54, %edi
	movl	$81, %esi
	callq	gcdr
	movl	%eax, %edi
	callq	writeln
	movl	$54, %edi
	movl	$81, %esi
	callq	gcdr_guessing
	movl	%eax, %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end4:
	.size	main, .Lfunc_end4-main
	.cfi_endproc
                                        # -- End function

	.section	".note.GNU-stack","",@progbits
