section .data
    msg db "Hello, world!", 10   ; string + newline
    len equ $ - msg              ; length of string

section .text
    global _start

;0	stdin	Standard input  (keyboard)
;1	stdout	Standard output (console)
;2	stderr	Standard error  (console, for errors)

_start:
    ; write(1, msg, len)
    mov rax, 1        ; syscall: write
    mov rdi, 1        ; fd = stdout
    mov rsi, msg      ; buffer address
    mov rdx, len      ; buffer length
    syscall

    ; exit(0)
    mov rax, 60       ; syscall: exit
    xor rdi, rdi      ; set status = 0
    syscall