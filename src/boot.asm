[org 0x7c00]

; Store boot drive
mov [BOOT_DRIVE], dl

; Print B
mov ah, 0x0e
mov al, 'B'
int 0x10

; Load kernel (sector 2 onward)
mov ax, 0x1000
mov es, ax
xor bx, bx
mov ah, 0x02
mov al, KERNEL_SECTORS
mov ch, 0
mov cl, 2
mov dh, 0
mov dl, [BOOT_DRIVE]
int 0x13
jc disk_fail

; Print D
mov ah, 0x0e
mov al, 'D'
int 0x10

; Enter protected mode
cli
lgdt [gdt_desc]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp 0x08:protected_mode

disk_fail:
mov ah, 0x0e
mov al, 'E'
int 0x10
jmp $

gdt_start:
dq 0x0
dq 0x00cf9a000000ffff    ; How do you possibly find these addresses?
dq 0x00cf92000000ffff
gdt_end:

gdt_desc:
dw gdt_end - gdt_start - 1
dd gdt_start

[bits 32]
protected_mode:
mov ax, 0x10
mov ds, ax
mov es, ax
mov ss, ax
mov esp, 0x90000  ; stack
mov byte [0xb8004], 'P'  ; print P
jmp 0x00010000           ; jump to kernel

BOOT_DRIVE: db 0
times 510-($-$$) db 0
dw 0xaa55