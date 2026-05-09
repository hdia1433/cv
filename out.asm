.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	mov w0, #12
	bl _exit

