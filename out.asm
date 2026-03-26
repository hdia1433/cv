.text


.global _main
_main:

	
	sub sp, sp, #16
	mov w0, #6
	str w0, [sp, #0]

	
	ldr w0, [sp, #0]
	bl _exit
	add sp, sp, #16


	mov x0, 0
	ret