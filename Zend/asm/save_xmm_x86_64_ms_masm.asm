.code

; ZEND_API void execute_ex(zend_execute_data *ex)
PUBLIC execute_ex

EXTERN execute_ex_real:PROC

; Assembly wrapper around the real execute_ex function, so that we can
; save the preserved registers when re-entering the VM from JIT code.
; See GH-18136.
execute_ex PROC EXPORT FRAME
    ; 10 XMM registers
    ; 32 bytes shadow space
    ; 8 bytes to align after the return address
    sub rsp, 16*10 + 32 + 8
    .allocstack 16*10 + 32 + 8
    .endprolog
    movaps xmmword ptr [rsp + 32 + 16*0], xmm6
    movaps xmmword ptr [rsp + 32 + 16*1], xmm7
    movaps xmmword ptr [rsp + 32 + 16*2], xmm8
    movaps xmmword ptr [rsp + 32 + 16*3], xmm9
    movaps xmmword ptr [rsp + 32 + 16*4], xmm10
    movaps xmmword ptr [rsp + 32 + 16*5], xmm11
    movaps xmmword ptr [rsp + 32 + 16*6], xmm12
    movaps xmmword ptr [rsp + 32 + 16*7], xmm13
    movaps xmmword ptr [rsp + 32 + 16*8], xmm14
    movaps xmmword ptr [rsp + 32 + 16*9], xmm15
    call execute_ex_real
    movaps xmm6, xmmword ptr [rsp + 32 + 16*0]
    movaps xmm7, xmmword ptr [rsp + 32 + 16*1]
    movaps xmm8, xmmword ptr [rsp + 32 + 16*2]
    movaps xmm9, xmmword ptr [rsp + 32 + 16*3]
    movaps xmm10, xmmword ptr [rsp + 32 + 16*4]
    movaps xmm11, xmmword ptr [rsp + 32 + 16*5]
    movaps xmm12, xmmword ptr [rsp + 32 + 16*6]
    movaps xmm13, xmmword ptr [rsp + 32 + 16*7]
    movaps xmm14, xmmword ptr [rsp + 32 + 16*8]
    movaps xmm15, xmmword ptr [rsp + 32 + 16*9]
    add rsp, 16*10 + 32 + 8
    ret
execute_ex ENDP

END
