section .text 
    global _start

jmp _start

;------------------------------------------------------
; Scan double number from stdin
; 
; We will use the stack as an intermediate storage, so as not to use the data section
; DESTROY: rax, rbx, rdi, rsi, rdx, r8, r9, rcx, xmm0, xmm1
;------------------------------------------------------
SCAN:
    sub rsp, 64 ; Stack-based memory allocation

;=======READ NUMBER========
    mov rax, 0
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 64
    syscall
;==========================

    add rsp, 64 ; Stack Alignment
    
    ;   rsi - index
    ;   rdi - number of digits after dot
    xor rax, rax ; the number itself
    xor rcx, rcx ; current symbol
    xor r8, r8   ; check minus 
    xor r9, r9   ; check dot
    mov rbx, 10

;=========RESEARCH OUR NUMBER===========
_research:
    cmp byte [rsi], '-'
    je _minus

    cmp byte [rsi], '.'
    je _dot

    cmp byte [rsi], '0'
    jb _end_research

    cmp byte [rsi], '9'
    ja _end_research

    cmp r9, 0
    je  _dec_before_dot
    jmp _dec_after_dot
   

_minus: 
    inc rsi
    inc r8
    jmp _research

_dot:
    inc rsi
    inc r9
    jmp _research


_dec_after_dot:    
    inc rdi

_dec_before_dot:    

    mul rbx
    mov cl, byte [rsi]
    sub cl, '0'
    add rax, rcx

    inc rsi

    jmp _research

;==============END RESEARCH=================
_end_research:
    cmp r8, 1
    jne _plus

    not rax ; additional code
    inc rax

_plus:
    mov r9, rax
    mov rax, 1

;=======GET EXPONENT==========
    mov rbx, 10

_exp:
    cmp rdi, 0
    je _end

    mul rbx
    dec rdi
    jmp _exp

;======EXPONENT IN RAX========
_end:

    cvtsi2sd xmm0, r9
    cvtsi2sd xmm1, rax

    divsd xmm0, xmm1

    pop r9 ; save return address

    sub rsp, 8
    movsd  qword [rsp], xmm0 ; push our number

    push r9 ; push return address
    ret



;------------------------------------------------------
; Print double number in stdout in the format x.xxx (our number in stack)
; 
; We will use the stack as an intermediate storage, so as not to use the data section
; DESTROY: rax, rbx, rdi, rsi, rdx, r8, r9, rcx, xmm0, xmm1
;------------------------------------------------------
PRINTF:
    pop r9
    movsd xmm0, qword [rsp]
    mov r8, 1000
    cvtsi2sd xmm1, r8
    mulsd xmm0, xmm1
    cvtsd2si rax, xmm0

    push r9

    xor r8, r8

    cmp rax, 0xFFFFFFFFFFFFFFFF ; 
    jg _convert

    dec rax
    not rax
   
    mov r8, 1

_convert:
    mov rsi, rsp
    sub rsp, 64

    mov rcx, 0
    mov rdi, 10

    dec rsi
    mov byte [rsi], 0xa
    inc rcx

    mov rdx, 0
    div rdi
    add rdx, '0'
    dec rsi
    mov byte [rsi], dl
    inc rcx

    mov rdx, 0
    div rdi
    add rdx, '0'
    dec rsi
    mov byte [rsi], dl
    inc rcx

    mov rdx, 0
    div rdi
    add rdx, '0'
    dec rsi
    mov byte [rsi], dl
    inc rcx

    dec rsi
    mov byte [rsi], '.'
    inc rcx

_dec: 
    mov rdx, 0
    div rdi
    add rdx, '0'
    dec rsi
    mov byte [rsi], dl
    inc rcx

    cmp rax, 0
    jne _dec

    cmp r8, 0
    je _write

    dec rsi
    mov byte [rsi], '-'
    inc rcx

_write:

    mov rax, 1
    mov rdi, 1  
    mov rdx, rcx 
    syscall

    add rsp, 64

    ret


;------------------------------------------------------
; Print double number in stdout in the format x (our number in stack)
; 
; We will use the stack as an intermediate storage, so as not to use the data section
; DESTROY: rax, rbx, rdi, rsi, rdx, r8, r9, rcx, xmm0,
;------------------------------------------------------
PRINT:
    pop r9

    movsd xmm0, qword [rsp]
    cvtsd2si rax, xmm0

    push r9

    xor r8, r8

    cmp rax, 0x0  
    jge _convert_int

    dec rax
    not rax
    
    mov r8, 1

_convert_int: 
    mov rsi, rsp
    sub rsp, 64

    mov rcx, 0
    mov rdi, 10

    dec rsi
    mov byte [rsi], 0xa
    inc rcx

    jmp _dec


; EXIT
HLT:
    mov rax, 60
    xor rdi, rdi
    syscall

_start:

call SCAN 
call PRINT
call PRINTF
call HLT
nop
nop
nop



