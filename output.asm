section .data
__return_value dq 0
fmt db 'Result: %lld', 10, 0
b dq 0
a dq 0
section .text
global main
extern printf
main:
    push rbp
    mov rbp, rsp
    sub rsp, 16
    ; declare a
    mov rax, 5
    mov [rel a], rax
    ; declare b
    mov rax, 3
    mov [rel b], rax
    mov rax, [rel a]
    push rax
    mov rax, [rel b]
    mov rbx, rax
    pop rax
    add rax, rbx
    mov [rel __return_value], rax
    mov rdx, qword [rel __return_value]
    lea rcx, [rel fmt]
    call printf
    pop rbp
    mov rdx, qword [rel __return_value]
    lea rcx, [rel fmt]
    call printf
    ret
