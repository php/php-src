;      Copyright Edward Nevill + Oliver Kowalke 2015
;  Distributed under the Boost Software License, Version 1.0.
;     (See accompanying file LICENSE_1_0.txt or copy at
;         http://www.boost.org/LICENSE_1_0.txt)

;*******************************************************
;*                                                     *
;*  -------------------------------------------------  *
;*  |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  *
;*  -------------------------------------------------  *
;*  | 0x0 | 0x4 | 0x8 | 0xc | 0x10| 0x14| 0x18| 0x1c|  *
;*  -------------------------------------------------  *
;*  |    d8     |    d9     |    d10    |    d11    |  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  8  |  9  |  10 |  11 |  12 |  13 |  14 |  15 |  *
;*  -------------------------------------------------  *
;*  | 0x20| 0x24| 0x28| 0x2c| 0x30| 0x34| 0x38| 0x3c|  *
;*  -------------------------------------------------  *
;*  |    d12    |    d13    |    d14    |    d15    |  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  16 |  17 |  18 |  19 |  20 |  21 |  22 |  23 |  *
;*  -------------------------------------------------  *
;*  | 0x40| 0x44| 0x48| 0x4c| 0x50| 0x54| 0x58| 0x5c|  *
;*  -------------------------------------------------  *
;*  |    x19    |    x20    |    x21    |    x22    |  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  24 |  25 |  26 |  27 |  28 |  29 |  30 |  31 |  *
;*  -------------------------------------------------  *
;*  | 0x60| 0x64| 0x68| 0x6c| 0x70| 0x74| 0x78| 0x7c|  *
;*  -------------------------------------------------  *
;*  |    x23    |    x24    |    x25    |    x26    |  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  32 |  33 |  34 |  35 |  36 |  37 |  38 |  39 |  *
;*  -------------------------------------------------  *
;*  | 0x80| 0x84| 0x88| 0x8c| 0x90| 0x94| 0x98| 0x9c|  *
;*  -------------------------------------------------  *
;*  |    x27    |    x28    |    FP     |     LR    |  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  40 |  41 |  42 |  43 |  44 |  45 |  46 |  47 |  *
;*  -------------------------------------------------  *
;*  | 0xa0| 0xa4| 0xa8| 0xac| 0xb0| 0xb4| 0xb8| 0xbc|  *
;*  -------------------------------------------------  *
;*  |   base    |   limit   |  dealloc  | fiber data|  *
;*  -------------------------------------------------  *
;*  -------------------------------------------------  *
;*  |  48 |  49 |  50 | 51  |           |           |  *
;*  -------------------------------------------------  *
;*  | 0xc0| 0xc4| 0xc8| 0xcc|           |           |  *
;*  -------------------------------------------------  *
;*  |     PC    |   align   |           |           |  *
;*  -------------------------------------------------  *
;*                                                     *
;*******************************************************

    AREA |.text|, CODE, READONLY, ALIGN=4, CODEALIGN
    EXPORT make_fcontext
    IMPORT _exit

make_fcontext proc
    ; save stack top address to x3
    mov x3, x0

    ; shift address in x0 (allocated stack) to lower 16 byte boundary
    and x0, x0, ~0xF

    ; reserve space for context-data on context-stack
    sub  x0, x0, #0xd0

    ; save top address of context_stack as 'base'
    str  x3, [x0, #0xa0]
    ; save bottom address of context-stack as 'limit' and 'dealloction stack'
    sub  x3, x3, x1
    stp  x3, x3, [x0, #0xa8]
    ; save 0 as 'fiber data'
    str  xzr, [x0, #0xb8]

    ; third arg of make_fcontext() == address of context-function
    ; store address as x19 for trampoline
    str  x2, [x0, #0x40]
    ; store trampoline address as pc
    adr  x2, trampoline
    str  x2, [x0, #0xc0]

    ; save address of finish as return-address for context-function
    ; will be entered after context-function returns (LR register)
    adr  x1, finish
    str  x1, [x0, #0x98]

    ret  x30 ; return pointer to context-data (x0)

trampoline
    stp  fp, lr, [sp, #-0x10]!
    mov  fp, sp
    blr x19

finish
    ; exit code is zero
    mov  x0, #0
    ; exit application
    bl  _exit
    ENDP
    END
