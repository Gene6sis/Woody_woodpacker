bits 64

section .text
	global _start

_start:
	push	0x0a216948

	mov		rax, 1
	mov		rdi, 1
	mov		rsi, rsp
	mov		rdx, 4
	syscall

	mov rax, 60
	mov rdi, 0
	syscall
