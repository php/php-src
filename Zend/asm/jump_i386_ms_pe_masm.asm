
;           Copyright Oliver Kowalke 2009.
;  Distributed under the Boost Software License, Version 1.0.
;     (See accompanying file LICENSE_1_0.txt or copy at
;           http://www.boost.org/LICENSE_1_0.txt)

;  ---------------------------------------------------------------------------------
;  |    0    |    1    |    2    |    3    |    4    |    5    |    6    |    7    |
;  ---------------------------------------------------------------------------------
;  |    0h   |   04h   |   08h   |   0ch   |   010h  |   014h  |   018h  |   01ch  |
;  ---------------------------------------------------------------------------------
;  | fc_mxcsr|fc_x87_cw| fc_strg |fc_deallo|  limit  |   base  |  fc_seh |   EDI   |
;  ---------------------------------------------------------------------------------
;  ---------------------------------------------------------------------------------
;  |    8    |    9    |   10    |    11   |    12   |    13   |    14   |    15   |
;  ---------------------------------------------------------------------------------
;  |   020h  |  024h   |  028h   |   02ch  |   030h  |   034h  |   038h  |   03ch  |
;  ---------------------------------------------------------------------------------
;  |   ESI   |   EBX   |   EBP   |   EIP   |    to   |   data  |  EH NXT |SEH HNDLR|
;  ---------------------------------------------------------------------------------

.386
.XMM
.model flat, c
.code

jump_fcontext PROC BOOST_CONTEXT_EXPORT
    ; prepare stack
    lea  esp, [esp-02ch]

IFNDEF BOOST_USE_TSX
    ; save MMX control- and status-word
    stmxcsr  [esp]
    ; save x87 control-word
    fnstcw  [esp+04h]
ENDIF

    assume  fs:nothing
    ; load NT_TIB into ECX
    mov  edx, fs:[018h]
    assume  fs:error
    ; load fiber local storage
    mov  eax, [edx+010h]
    mov  [esp+08h], eax
    ; load current deallocation stack
    mov  eax, [edx+0e0ch]
    mov  [esp+0ch], eax
    ; load current stack limit
    mov  eax, [edx+08h]
    mov  [esp+010h], eax
    ; load current stack base
    mov  eax, [edx+04h]
    mov  [esp+014h], eax
    ; load current SEH exception list
    mov  eax, [edx]
    mov  [esp+018h], eax

    mov  [esp+01ch], edi  ; save EDI 
    mov  [esp+020h], esi  ; save ESI 
    mov  [esp+024h], ebx  ; save EBX 
    mov  [esp+028h], ebp  ; save EBP 

    ; store ESP (pointing to context-data) in EAX
    mov  eax, esp

    ; firstarg of jump_fcontext() == fcontext to jump to
    mov  ecx, [esp+030h]
    
    ; restore ESP (pointing to context-data) from ECX
    mov  esp, ecx

IFNDEF BOOST_USE_TSX
    ; restore MMX control- and status-word
    ldmxcsr  [esp]
    ; restore x87 control-word
    fldcw  [esp+04h]
ENDIF

    assume  fs:nothing
    ; load NT_TIB into EDX
    mov  edx, fs:[018h]
    assume  fs:error
    ; restore fiber local storage
    mov  ecx, [esp+08h]
    mov  [edx+010h], ecx
    ; restore current deallocation stack
    mov  ecx, [esp+0ch]
    mov  [edx+0e0ch], ecx
    ; restore current stack limit
    mov  ecx, [esp+010h]
    mov  [edx+08h], ecx
    ; restore current stack base
    mov  ecx, [esp+014h]
    mov  [edx+04h], ecx
    ; restore current SEH exception list
    mov  ecx, [esp+018h]
    mov  [edx], ecx

    mov  ecx, [esp+02ch]  ; restore EIP

    mov  edi, [esp+01ch]  ; restore EDI 
    mov  esi, [esp+020h]  ; restore ESI 
    mov  ebx, [esp+024h]  ; restore EBX 
    mov  ebp, [esp+028h]  ; restore EBP 

    ; prepare stack
    lea  esp, [esp+030h]

    ; return transfer_t
    ; FCTX == EAX, DATA == EDX
    mov  edx, [eax+034h]

    ; jump to context
    jmp ecx
jump_fcontext ENDP
END
