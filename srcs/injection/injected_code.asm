bits 64

section .text
	global _start

_start:
	;	Save l'etat des registres utilisés
	push	rax
	push	rbx
	push	rcx
	push	rdx
	push	rdi
	push	rsi

	;	Puisqu'on est pas surs qu'il y aura un "...WOODY...\n" dans le .data, on peut "juste" le push nous meme
	;	pour etre SUR qu'on l'a dans notre stack quelque part. Le probleme de ca c'est que l'instruction push
	;	ne peux que prendre en operande 4 bits max alors qu'elle push 8 bits, du coup au final on se retrouve avec
	;	du padding filled par des 0x0. On peut quand meme print notre message puisque les \0 ne se print pas a
	;	l'écran. En tout cas ca fonctionne.
	push	0x0a2e2e2e	;	...\n
	push	0x59444f4f	;	OODY
	push	0x572e2e2e	;	...W

	;	write(stderr, "...WOODY...\n", 20)
	mov		rax, 1		;	syscall number
	mov		rdi, 2		;	stderr
	mov		rsi, rsp	;	"...W\0\0\0\0OODY\0\0\0\0...\n", precedement push dans la stack
	mov		rdx, 20		;	20
	syscall

	;	Clear la stack pour pas avoir de problemes
	pop	rax
	pop	rax
	pop	rax

	;	Il faut pouvoir pouvoir avoir un pointeur sur le debut du code a XOR, et un pointeur sur la fin.
	;	Pour ca j'utilise rel qui va renvoyer rip avec un offset, ici du symbole _start ou .end.
	;	Ces valeurs seront changées par woodpacker et remplacées par un offest plus grand qui pointera
	;	sur le debut du code a XOR et la fin du code a XOR.
	;	La clé aussi devra etre changée par woodpacker pour avoir la même qu'a l'encryption.

	;	rax -> current		-> pointeur sur le debut du code, que je vais XOR. rax sera incrementé pour
	;							pointer sur les prochaines instructions
	;	rbx -> end			-> pointeur sur la fin du code a XOR, pour savoir quand s'arreter
	;	rcx -> encrypted	-> le code XOR par la clé
	;	rdx -> key			-> la clé
	lea		rax, [rel _start]
	lea		rbx, [rel .end]
	xor		rcx, rcx
	mov		rdx, 0x0

.loop:
	cmp		rax, rbx		;	while (current < end)
	jae		.exit			;	{

	mov	dword	ecx, [rax]	;		encrypted = (unsigned int)*current;
	xor	dword	ecx, edx	;		encrypted ^= key;
	mov	dword	[rax], ecx	;		*current = encrypted;
	add			rax, 4		;		current += 4;
	jmp			.loop		;	}

.exit:
	;	Remet les registres a leur etat de base
	pop	rsi
	pop	rdi
	pop	rdx
	pop	rcx
	pop	rbx
	pop	rax
	jmp	_start

.end:
