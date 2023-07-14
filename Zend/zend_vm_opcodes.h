/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_VM_OPCODES_H
#define ZEND_VM_OPCODES_H

#define ZEND_VM_SPEC		1
#define ZEND_VM_LINES		0
#define ZEND_VM_KIND_CALL	1
#define ZEND_VM_KIND_SWITCH	2
#define ZEND_VM_KIND_GOTO	3
#define ZEND_VM_KIND_HYBRID	4
/* HYBRID requires support for computed GOTO and global register variables*/
#if (defined(__GNUC__) && defined(HAVE_GCC_GLOBAL_REGS))
# define ZEND_VM_KIND		ZEND_VM_KIND_HYBRID
#else
# define ZEND_VM_KIND		ZEND_VM_KIND_CALL
#endif

#if (ZEND_VM_KIND == ZEND_VM_KIND_HYBRID) && !defined(__SANITIZE_ADDRESS__)
# if ((defined(i386) && !defined(__PIC__)) || defined(__x86_64__) || defined(_M_X64))
#  define ZEND_VM_HYBRID_JIT_RED_ZONE_SIZE 16
# endif
#endif

#define ZEND_VM_OP_SPEC          0x00000001
#define ZEND_VM_OP_CONST         0x00000002
#define ZEND_VM_OP_TMPVAR        0x00000004
#define ZEND_VM_OP_TMPVARCV      0x00000008
#define ZEND_VM_OP_MASK          0x000000f0
#define ZEND_VM_OP_NUM           0x00000010
#define ZEND_VM_OP_JMP_ADDR      0x00000020
#define ZEND_VM_OP_TRY_CATCH     0x00000030
#define ZEND_VM_OP_THIS          0x00000050
#define ZEND_VM_OP_NEXT          0x00000060
#define ZEND_VM_OP_CLASS_FETCH   0x00000070
#define ZEND_VM_OP_CONSTRUCTOR   0x00000080
#define ZEND_VM_OP_CONST_FETCH   0x00000090
#define ZEND_VM_OP_CACHE_SLOT    0x000000a0
#define ZEND_VM_EXT_VAR_FETCH    0x00010000
#define ZEND_VM_EXT_ISSET        0x00020000
#define ZEND_VM_EXT_CACHE_SLOT   0x00040000
#define ZEND_VM_EXT_ARRAY_INIT   0x00080000
#define ZEND_VM_EXT_REF          0x00100000
#define ZEND_VM_EXT_FETCH_REF    0x00200000
#define ZEND_VM_EXT_DIM_WRITE    0x00400000
#define ZEND_VM_EXT_MASK         0x0f000000
#define ZEND_VM_EXT_NUM          0x01000000
#define ZEND_VM_EXT_LAST_CATCH   0x02000000
#define ZEND_VM_EXT_JMP_ADDR     0x03000000
#define ZEND_VM_EXT_OP           0x04000000
#define ZEND_VM_EXT_TYPE         0x07000000
#define ZEND_VM_EXT_EVAL         0x08000000
#define ZEND_VM_EXT_TYPE_MASK    0x09000000
#define ZEND_VM_EXT_SRC          0x0b000000
#define ZEND_VM_NO_CONST_CONST   0x40000000
#define ZEND_VM_COMMUTATIVE      0x80000000
#define ZEND_VM_OP1_FLAGS(flags) (flags & 0xff)
#define ZEND_VM_OP2_FLAGS(flags) ((flags >> 8) & 0xff)

BEGIN_EXTERN_C()

ZEND_API const char* ZEND_FASTCALL zend_get_opcode_name(uint8_t opcode);
ZEND_API uint32_t ZEND_FASTCALL zend_get_opcode_flags(uint8_t opcode);
ZEND_API uint8_t zend_get_opcode_id(const char *name, size_t length);

END_EXTERN_C()

#define ZEND_NOP                          0
#define ZEND_ADD                          1
#define ZEND_SUB                          2
#define ZEND_MUL                          3
#define ZEND_DIV                          4
#define ZEND_MOD                          5
#define ZEND_SL                           6
#define ZEND_SR                           7
#define ZEND_CONCAT                       8
#define ZEND_BW_OR                        9
#define ZEND_BW_AND                      10
#define ZEND_BW_XOR                      11
#define ZEND_POW                         12
#define ZEND_BW_NOT                      13
#define ZEND_BOOL_NOT                    14
#define ZEND_BOOL_XOR                    15
#define ZEND_IS_IDENTICAL                16
#define ZEND_IS_NOT_IDENTICAL            17
#define ZEND_IS_EQUAL                    18
#define ZEND_IS_NOT_EQUAL                19
#define ZEND_IS_SMALLER                  20
#define ZEND_IS_SMALLER_OR_EQUAL         21
#define ZEND_ASSIGN                      22
#define ZEND_ASSIGN_DIM                  23
#define ZEND_ASSIGN_OBJ                  24
#define ZEND_ASSIGN_STATIC_PROP          25
#define ZEND_ASSIGN_OP                   26
#define ZEND_ASSIGN_DIM_OP               27
#define ZEND_ASSIGN_OBJ_OP               28
#define ZEND_ASSIGN_STATIC_PROP_OP       29
#define ZEND_ASSIGN_REF                  30
#define ZEND_QM_ASSIGN                   31
#define ZEND_ASSIGN_OBJ_REF              32
#define ZEND_ASSIGN_STATIC_PROP_REF      33
#define ZEND_PRE_INC                     34
#define ZEND_PRE_DEC                     35
#define ZEND_POST_INC                    36
#define ZEND_POST_DEC                    37
#define ZEND_PRE_INC_STATIC_PROP         38
#define ZEND_PRE_DEC_STATIC_PROP         39
#define ZEND_POST_INC_STATIC_PROP        40
#define ZEND_POST_DEC_STATIC_PROP        41
#define ZEND_JMP                         42
#define ZEND_JMPZ                        43
#define ZEND_JMPNZ                       44
#define ZEND_JMPZ_EX                     46
#define ZEND_JMPNZ_EX                    47
#define ZEND_CASE                        48
#define ZEND_CHECK_VAR                   49
#define ZEND_SEND_VAR_NO_REF_EX          50
#define ZEND_CAST                        51
#define ZEND_BOOL                        52
#define ZEND_FAST_CONCAT                 53
#define ZEND_ROPE_INIT                   54
#define ZEND_ROPE_ADD                    55
#define ZEND_ROPE_END                    56
#define ZEND_BEGIN_SILENCE               57
#define ZEND_END_SILENCE                 58
#define ZEND_INIT_FCALL_BY_NAME          59
#define ZEND_DO_FCALL                    60
#define ZEND_INIT_FCALL                  61
#define ZEND_RETURN                      62
#define ZEND_RECV                        63
#define ZEND_RECV_INIT                   64
#define ZEND_SEND_VAL                    65
#define ZEND_SEND_VAR_EX                 66
#define ZEND_SEND_REF                    67
#define ZEND_NEW                         68
#define ZEND_INIT_NS_FCALL_BY_NAME       69
#define ZEND_FREE                        70
#define ZEND_INIT_ARRAY                  71
#define ZEND_ADD_ARRAY_ELEMENT           72
#define ZEND_INCLUDE_OR_EVAL             73
#define ZEND_UNSET_VAR                   74
#define ZEND_UNSET_DIM                   75
#define ZEND_UNSET_OBJ                   76
#define ZEND_FE_RESET_R                  77
#define ZEND_FE_FETCH_R                  78
#define ZEND_EXIT                        79
#define ZEND_FETCH_R                     80
#define ZEND_FETCH_DIM_R                 81
#define ZEND_FETCH_OBJ_R                 82
#define ZEND_FETCH_W                     83
#define ZEND_FETCH_DIM_W                 84
#define ZEND_FETCH_OBJ_W                 85
#define ZEND_FETCH_RW                    86
#define ZEND_FETCH_DIM_RW                87
#define ZEND_FETCH_OBJ_RW                88
#define ZEND_FETCH_IS                    89
#define ZEND_FETCH_DIM_IS                90
#define ZEND_FETCH_OBJ_IS                91
#define ZEND_FETCH_FUNC_ARG              92
#define ZEND_FETCH_DIM_FUNC_ARG          93
#define ZEND_FETCH_OBJ_FUNC_ARG          94
#define ZEND_FETCH_UNSET                 95
#define ZEND_FETCH_DIM_UNSET             96
#define ZEND_FETCH_OBJ_UNSET             97
#define ZEND_FETCH_LIST_R                98
#define ZEND_FETCH_CONSTANT              99
#define ZEND_CHECK_FUNC_ARG             100
#define ZEND_EXT_STMT                   101
#define ZEND_EXT_FCALL_BEGIN            102
#define ZEND_EXT_FCALL_END              103
#define ZEND_EXT_NOP                    104
#define ZEND_TICKS                      105
#define ZEND_SEND_VAR_NO_REF            106
#define ZEND_CATCH                      107
#define ZEND_THROW                      108
#define ZEND_FETCH_CLASS                109
#define ZEND_CLONE                      110
#define ZEND_RETURN_BY_REF              111
#define ZEND_INIT_METHOD_CALL           112
#define ZEND_INIT_STATIC_METHOD_CALL    113
#define ZEND_ISSET_ISEMPTY_VAR          114
#define ZEND_ISSET_ISEMPTY_DIM_OBJ      115
#define ZEND_SEND_VAL_EX                116
#define ZEND_SEND_VAR                   117
#define ZEND_INIT_USER_CALL             118
#define ZEND_SEND_ARRAY                 119
#define ZEND_SEND_USER                  120
#define ZEND_STRLEN                     121
#define ZEND_DEFINED                    122
#define ZEND_TYPE_CHECK                 123
#define ZEND_VERIFY_RETURN_TYPE         124
#define ZEND_FE_RESET_RW                125
#define ZEND_FE_FETCH_RW                126
#define ZEND_FE_FREE                    127
#define ZEND_INIT_DYNAMIC_CALL          128
#define ZEND_DO_ICALL                   129
#define ZEND_DO_UCALL                   130
#define ZEND_DO_FCALL_BY_NAME           131
#define ZEND_PRE_INC_OBJ                132
#define ZEND_PRE_DEC_OBJ                133
#define ZEND_POST_INC_OBJ               134
#define ZEND_POST_DEC_OBJ               135
#define ZEND_ECHO                       136
#define ZEND_OP_DATA                    137
#define ZEND_INSTANCEOF                 138
#define ZEND_GENERATOR_CREATE           139
#define ZEND_MAKE_REF                   140
#define ZEND_DECLARE_FUNCTION           141
#define ZEND_DECLARE_LAMBDA_FUNCTION    142
#define ZEND_DECLARE_CONST              143
#define ZEND_DECLARE_CLASS              144
#define ZEND_DECLARE_CLASS_DELAYED      145
#define ZEND_DECLARE_ANON_CLASS         146
#define ZEND_ADD_ARRAY_UNPACK           147
#define ZEND_ISSET_ISEMPTY_PROP_OBJ     148
#define ZEND_HANDLE_EXCEPTION           149
#define ZEND_USER_OPCODE                150
#define ZEND_ASSERT_CHECK               151
#define ZEND_JMP_SET                    152
#define ZEND_UNSET_CV                   153
#define ZEND_ISSET_ISEMPTY_CV           154
#define ZEND_FETCH_LIST_W               155
#define ZEND_SEPARATE                   156
#define ZEND_FETCH_CLASS_NAME           157
#define ZEND_CALL_TRAMPOLINE            158
#define ZEND_DISCARD_EXCEPTION          159
#define ZEND_YIELD                      160
#define ZEND_GENERATOR_RETURN           161
#define ZEND_FAST_CALL                  162
#define ZEND_FAST_RET                   163
#define ZEND_RECV_VARIADIC              164
#define ZEND_SEND_UNPACK                165
#define ZEND_YIELD_FROM                 166
#define ZEND_COPY_TMP                   167
#define ZEND_BIND_GLOBAL                168
#define ZEND_COALESCE                   169
#define ZEND_SPACESHIP                  170
#define ZEND_FUNC_NUM_ARGS              171
#define ZEND_FUNC_GET_ARGS              172
#define ZEND_FETCH_STATIC_PROP_R        173
#define ZEND_FETCH_STATIC_PROP_W        174
#define ZEND_FETCH_STATIC_PROP_RW       175
#define ZEND_FETCH_STATIC_PROP_IS       176
#define ZEND_FETCH_STATIC_PROP_FUNC_ARG 177
#define ZEND_FETCH_STATIC_PROP_UNSET    178
#define ZEND_UNSET_STATIC_PROP          179
#define ZEND_ISSET_ISEMPTY_STATIC_PROP  180
#define ZEND_FETCH_CLASS_CONSTANT       181
#define ZEND_BIND_LEXICAL               182
#define ZEND_BIND_STATIC                183
#define ZEND_FETCH_THIS                 184
#define ZEND_SEND_FUNC_ARG              185
#define ZEND_ISSET_ISEMPTY_THIS         186
#define ZEND_SWITCH_LONG                187
#define ZEND_SWITCH_STRING              188
#define ZEND_IN_ARRAY                   189
#define ZEND_COUNT                      190
#define ZEND_GET_CLASS                  191
#define ZEND_GET_CALLED_CLASS           192
#define ZEND_GET_TYPE                   193
#define ZEND_ARRAY_KEY_EXISTS           194
#define ZEND_MATCH                      195
#define ZEND_CASE_STRICT                196
#define ZEND_MATCH_ERROR                197
#define ZEND_JMP_NULL                   198
#define ZEND_CHECK_UNDEF_ARGS           199
#define ZEND_FETCH_GLOBALS              200
#define ZEND_VERIFY_NEVER_TYPE          201
#define ZEND_CALLABLE_CONVERT           202
#define ZEND_BIND_INIT_STATIC_OR_JMP    203

#define ZEND_VM_LAST_OPCODE             203

#endif
