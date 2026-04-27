[bits 32]
global _start
extern c_start

_start:
    mov esp, 0x90000   ; stack for C
    call c_start       ; jump to C kernel
    cli
    hlt
    jmp $