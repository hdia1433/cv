.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp
	add sp, sp, #16

	mov w0, #4
	add w0, w0, #2

	mov w28, w0
	str w28, [sp, #0]

	ldr w28, [sp, #0]
	add w1, w28, #8

	mov w0, w1
	bl _exit

