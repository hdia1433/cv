.section __DATA, __bss
.lcomm _x, 4



.section __TEXT, __text
.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	bl _static_init

	adrp x0, _x@PAGE
	add x0, x0, :lo12:_x@PAGEOFF
	ldr w0, [x0]
	bl _exit

_static_init:
		mov w0, #4
		sub w0, w0, #2
		mov w28, w0
		adrp x27, _x@PAGE
		add x27, x27, :lo12:_x@PAGEOFF
		str w28, [x27]
