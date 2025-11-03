section .text
	global _start

_start:
	mov rbx, 69
	mov rax, 0xfc
	cmovz rax, rbx
	setnz al
	and rax, 1

_return:
	mov rdi, rax
	mov rax, 60
	syscall