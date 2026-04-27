; I don't need this file but yes

[org 0x10000]
[bits 32]

VGA_BASE equ 0xb8000

; Print X at next cell after B D P
mov edi, VGA_BASE + 6
mov byte [edi], 'X'
mov byte [edi+1], 0x0f

; Print message
mov esi, message
mov edi, VGA_BASE + 8
print_loop:
lodsb
cmp al, 0
je done
mov ah, 0x0f
stosw
jmp print_loop
done:
hlt
jmp done

message db " Hello kernel world!",0