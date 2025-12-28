.code

; ZEND_API void execute_ex(zend_execute_data *ex)
PUBLIC execute_ex

EXTERN execute_ex_real:PROC

; Assembly wrapper around the real execute_ex function, so that we can
; save the preserved registers when re-entering the VM from JIT code.
; See GH-18136.
execute_ex PROC EXPORT FRAME
    ; 10 floating points numbers
    ; 32 bytes shadow space
    ; 8 bytes to align after the return address
    sub rsp, 8*10 + 32 + 8
    .allocstack 8*10 + 32 + 8
    .endprolog
    movsd qword ptr [rsp + 32 + 8*0], xmm6
    movsd qword ptr [rsp + 32 + 8*1], xmm7
    movsd qword ptr [rsp + 32 + 8*2], xmm8
    movsd qword ptr [rsp + 32 + 8*3], xmm9
    movsd qword ptr [rsp + 32 + 8*4], xmm10
    movsd qword ptr [rsp + 32 + 8*5], xmm11
    movsd qword ptr [rsp + 32 + 8*6], xmm12
    movsd qword ptr [rsp + 32 + 8*7], xmm13
    movsd qword ptr [rsp + 32 + 8*8], xmm14
    movsd qword ptr [rsp + 32 + 8*9], xmm15
    call execute_ex_real
    movsd xmm6, qword ptr [rsp + 32 + 8*0]
    movsd xmm7, qword ptr [rsp + 32 + 8*1]
    movsd xmm8, qword ptr [rsp + 32 + 8*2]
    movsd xmm9, qword ptr [rsp + 32 + 8*3]
    movsd xmm10, qword ptr [rsp + 32 + 8*4]
    movsd xmm11, qword ptr [rsp + 32 + 8*5]
    movsd xmm12, qword ptr [rsp + 32 + 8*6]
    movsd xmm13, qword ptr [rsp + 32 + 8*7]
    movsd xmm14, qword ptr [rsp + 32 + 8*8]
    movsd xmm15, qword ptr [rsp + 32 + 8*9]
    add rsp, 8*10 + 32 + 8
    ret
execute_ex ENDP

END
