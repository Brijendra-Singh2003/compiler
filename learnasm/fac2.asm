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

	sub rsp, 8
	mov qword [rbp - 8], 8
	or qword [rbp - 8], 2
	mov rax, [rbp - 8]

	mov rsp, rbp
	pop rbp
	ret
