section .data
    x dq 10
    y dq 5
    z dq 0
    fmt db 'Result: %lld', 10, 0

section .text
    global main
    extern printf

main:
    ; if (x > y)
    mov rax, [rel x]
    mov rbx, [rel y]
    cmp rax, rbx
    jle .else_branch
    ; z = x - y
    mov rax, [rel x]
    sub rax, [rel y]
    mov [rel z], rax
    jmp .after_if
.else_branch:
    ; z = y - x
    mov rax, [rel y]
    sub rax, [rel x]
    mov [rel z], rax
.after_if:

    ; while (z < 20)
.while_start:
    mov rax, [rel z]
    cmp rax, 20
    jge .while_end
    ; z = z + 1
    mov rax, [rel z]
    add rax, 1
    mov [rel z], rax
    jmp .while_start
.while_end:

    ; Print z using printf("Result: %lld\n", z)
    sub rsp, 40                ; 32 for shadow space + 8 for alignment
    mov rdx, [rel z]           ; 2nd arg: z (in rdx)
    lea rcx, [rel fmt]         ; 1st arg: format string (in rcx)
    call printf
    add rsp, 40
    ret
