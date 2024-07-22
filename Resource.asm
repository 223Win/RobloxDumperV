section .text
global _start

_start:
    ; Save the state of the registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11

    ; Move the function address and arguments from the stack into registers
    mov rax, [rsp + 8*16]  ; Function address (the stack is 16*8 bytes deep because of saved registers)
    mov rcx, [rsp + 8*17]  ; Argument 1
    mov rdx, [rsp + 8*18]  ; Argument 2
    mov r8,  [rsp + 8*19]  ; Argument 3
    mov r9,  [rsp + 8*20]  ; Argument 4

    ; Call the function
    call rax

    ; Restore the state of the registers
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Exit the process
    ret