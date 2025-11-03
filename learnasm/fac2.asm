section .text
	global _start

_start:
	call _main

	mov rdi, rax
	mov rax, 60
	syscall

_main:
	push rbp
	mov rbp, rsp

	push 8
	call _fibo
	add rsp, 8

	mov rsp, rbp
	pop rbp
	ret

_fibo:
	push rbp
	mov rbp, rsp

	mov rax, qword [rbp + 16]
	cmp rax, 2
	jl _base_case

	push rax
	mov rax, 1
	pop rbx
	sub rbx, rax
	mov rax, rbx

	push rax
	call _fibo
	add rsp, 8

	push rax

	push qword [rbp + 16]
	mov rax, 2
	pop rbx
	sub rbx, rax
	mov rax, rbx

	push rax
	call _fibo
	add rsp, 8

	pop rbx
	add rax, rbx
	jmp _cleanup

_base_case:
	mov rax, qword [rbp + 16]

_cleanup:
	mov rsp, rbp
	pop rbp
	ret
