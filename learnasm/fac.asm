global _start
_start:
    push 6
    call _fac
    mov rdi, rax
    mov rax, 60
    syscall

_fac:
    push rbp
    mov rbp, rsp
    mov rax, [rbp+16]

    cmp rax, 1
    jle _base_case

    push rax
    sub rax, 1
    push rax

    call _fac
    pop rbx
    pop rbx
    mul rbx
    jmp _cleanup

_base_case:
    mov rax, 1

_cleanup:
    mov rsp, rbp
    pop rbp
    ret
