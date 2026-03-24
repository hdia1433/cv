.text


.global _main
_main:

	mov x0, 42
	mov x16, 1
	svc #0

	mov x0, 0
	ret