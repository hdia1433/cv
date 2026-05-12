.section __DATA, __data
_x:
	.long 4
.section __DATA, __bss
.lcomm _y, 4

.lcomm _z, 4



.section __TEXT, __text
.global _main
_main:
	stp x29, x30, [sp, #-16]!
	mov x29, sp

	bl _static_init

	adrp x0, _z@PAGE
	add x0, x0, :lo12:_z@PAGEOFF
	ldr w0, [x0]
	bl _exit

_static_init:
	adrp x28, _x@PAGE
	add x28, x28, :lo12:_x@PAGEOFF
	ldr w28, [x28]
	adrp x27, _y@PAGE
	add x27, x27, :lo12:_y@PAGEOFF
	str w28, [x27]
	adrp x28, _y@PAGE
	add x28, x28, :lo12:_y@PAGEOFF
	ldr w28, [x28]
	add w0, w28, #5
	mov w28, w0
	adrp x27, _z@PAGE
	add x27, x27, :lo12:_z@PAGEOFF
	str w28, [x27]

	ret
