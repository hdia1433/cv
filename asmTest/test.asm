.text

.global _main
_main:
    sub sp, sp, #16

    mov w1, #42
    str w1, [sp, #0]

    ldr w0, [sp, #0]

    bl _exit