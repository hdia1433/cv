.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp
	sub sp, sp, #16

	mov w28, #'h'
	str w27, [sp, #0]


	add sp, sp, #16
	mov sp, x29
	ldp x29, x30, [sp, #16]
	ret
