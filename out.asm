.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	mov x0, #14
	bl _exit
