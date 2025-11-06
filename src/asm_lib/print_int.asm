
_print_int:
    push rbp
    mov rbp, rsp
    sub rsp, 25             ; Allocate buffer
    
    mov byte [rbp - 25], 0
    mov rax, [rbp + 16]     ; RAX = Number

    shr rax, 63
    cmp rax, 0
    mov rax, [rbp + 16]
    jz print_int_L1

    not rax
    inc rax
    mov byte [rbp - 25], 1

print_int_L1:
    mov rbx, 10             ; RBX = 10
    
    ; R8 will track the position of the last digit (End of buffer)
    mov r8, rbp
    sub r8, 1               ; R8 points to RSP + 23 (The end of the buffer)

    mov rcx, r8             ; RCX is the writing pointer
    mov byte [rcx], 10      ; Store byte
    dec rcx

print_int_L0:
    xor rdx, rdx            
    div rbx                 ; RDX = digit
    
    add dl, '0'             ; Convert to ASCII
    mov byte [rcx], dl      ; Store byte
    dec rcx                 ; Move back
    
    cmp rax, 0
    jnz print_int_L0

    mov al, [rbp - 25]
    cmp al, 0
    jz print_int_L2
    mov byte [rcx], '-'      ; Store byte
    dec rcx

print_int_L2:
    inc rcx                 ; RCX now points to the first digit ('1')

    ; --- Syscall to print the number ---
    mov rax, 1              ; syscall number for write
    mov rdi, 1              ; file descriptor 1 (stdout)
    mov rsi, rcx            ; RSI = Start of the string
    
    ; RDX = Length = R8 (last digit pos) - RCX (first digit pos) + 1
    mov rdx, r8             
    sub rdx, rsi            
    inc rdx                 
    
    syscall                 ; Print the string
    
    ; --- Cleanup ---
    mov rsp, rbp            
    pop rbp
    ret
