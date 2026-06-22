.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	add x28, sp, #0

	mov w28, #1
	str w28, [x0]

	add x28, sp, #0
	add x0, x28, #4

	mov w28, #2
	str w28, [x0]

	add x28, sp, #0
	add x0, x28, #8

	mov w28, #3
	str w28, [x0]

	mov w1, #1
	mov w28, #4
	mul w1, w1, w28

	add x28, sp, #0
	add x1, x28, w1

	mov w28, #5
	str w28, [x1]

	mov w2, #0
	mov w28, #4
	mul w2, w2, w28

	add x28, sp, #0
	add x2, x28, w2

	bl _exit

