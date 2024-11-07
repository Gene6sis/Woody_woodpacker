BITS 64  ; Indique que nous travaillons en mode 64 bits

section .text
global _start

_start:
    ; Ecrire "Hello from injected code!\n" sur stdout
    mov rax, 1                    ; syscall write
    mov rdi, 1                    ; file descriptor (stdout)
    lea rsi, [rel message]        ; adresse du message
    mov rdx, message_len          ; longueur du message
    syscall                       ; appeler le syscall

    ; Sortir proprement
    mov rax, 60                   ; syscall exit
    xor rdi, rdi                  ; code de retour 0
    syscall                       ; appeler le syscall

message db "Hello from injected code!", 0x0A
message_len equ $ - message       ; longueur du message
