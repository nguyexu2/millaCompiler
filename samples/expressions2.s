	.text
	.file	"expressions2"
	.globl	main                    # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$x, %edi
	callq	readln
	movl	$y, %edi
	callq	readln
	movl	y(%rip), %eax
	movl	x(%rip), %edi
	leal	(%rdi,%rax), %ecx
	movl	%ecx, a(%rip)
	subl	%edi, %eax
	movl	%eax, b(%rip)
                                        # kill: def %edi killed %edi killed %rdi
	callq	writeln
	movl	y(%rip), %edi
	callq	writeln
	movl	a(%rip), %edi
	callq	writeln
	movl	b(%rip), %edi
	callq	writeln
	movl	x(%rip), %eax
	addl	a(%rip), %eax
	movl	y(%rip), %edi
	subl	b(%rip), %edi
	imull	%eax, %edi
	movl	%edi, c(%rip)
	callq	writeln
	movl	a(%rip), %eax
	cltd
	idivl	b(%rip)
	movl	%edx, d(%rip)
	movl	%edx, %edi
	callq	writeln
	xorl	%eax, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	x,@object               # @x
	.bss
	.globl	x
	.p2align	2
x:
	.long	0                       # 0x0
	.size	x, 4

	.type	y,@object               # @y
	.globl	y
	.p2align	2
y:
	.long	0                       # 0x0
	.size	y, 4

	.type	a,@object               # @a
	.globl	a
	.p2align	2
a:
	.long	0                       # 0x0
	.size	a, 4

	.type	b,@object               # @b
	.globl	b
	.p2align	2
b:
	.long	0                       # 0x0
	.size	b, 4

	.type	c,@object               # @c
	.globl	c
	.p2align	2
c:
	.long	0                       # 0x0
	.size	c, 4

	.type	d,@object               # @d
	.globl	d
	.p2align	2
d:
	.long	0                       # 0x0
	.size	d, 4


	.section	".note.GNU-stack","",@progbits
