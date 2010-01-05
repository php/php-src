/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2010 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#define ZEND_NOP                       0
#define ZEND_ADD                       1
#define ZEND_SUB                       2
#define ZEND_MUL                       3
#define ZEND_DIV                       4
#define ZEND_MOD                       5
#define ZEND_SL                        6
#define ZEND_SR                        7
#define ZEND_CONCAT                    8
#define ZEND_BW_OR                     9
#define ZEND_BW_AND                   10
#define ZEND_BW_XOR                   11
#define ZEND_BW_NOT                   12
#define ZEND_BOOL_NOT                 13
#define ZEND_BOOL_XOR                 14
#define ZEND_IS_IDENTICAL             15
#define ZEND_IS_NOT_IDENTICAL         16
#define ZEND_IS_EQUAL                 17
#define ZEND_IS_NOT_EQUAL             18
#define ZEND_IS_SMALLER               19
#define ZEND_IS_SMALLER_OR_EQUAL      20
#define ZEND_CAST                     21
#define ZEND_QM_ASSIGN                22
#define ZEND_ASSIGN_ADD               23
#define ZEND_ASSIGN_SUB               24
#define ZEND_ASSIGN_MUL               25
#define ZEND_ASSIGN_DIV               26
#define ZEND_ASSIGN_MOD               27
#define ZEND_ASSIGN_SL                28
#define ZEND_ASSIGN_SR                29
#define ZEND_ASSIGN_CONCAT            30
#define ZEND_ASSIGN_BW_OR             31
#define ZEND_ASSIGN_BW_AND            32
#define ZEND_ASSIGN_BW_XOR            33
#define ZEND_PRE_INC                  34
#define ZEND_PRE_DEC                  35
#define ZEND_POST_INC                 36
#define ZEND_POST_DEC                 37
#define ZEND_ASSIGN                   38
#define ZEND_ASSIGN_REF               39
#define ZEND_ECHO                     40
#define ZEND_PRINT                    41
#define ZEND_JMP                      42
#define ZEND_JMPZ                     43
#define ZEND_JMPNZ                    44
#define ZEND_JMPZNZ                   45
#define ZEND_JMPZ_EX                  46
#define ZEND_JMPNZ_EX                 47
#define ZEND_CASE                     48
#define ZEND_SWITCH_FREE              49
#define ZEND_BRK                      50
#define ZEND_CONT                     51
#define ZEND_BOOL                     52
#define ZEND_INIT_STRING              53
#define ZEND_ADD_CHAR                 54
#define ZEND_ADD_STRING               55
#define ZEND_ADD_VAR                  56
#define ZEND_BEGIN_SILENCE            57
#define ZEND_END_SILENCE              58
#define ZEND_INIT_FCALL_BY_NAME       59
#define ZEND_DO_FCALL                 60
#define ZEND_DO_FCALL_BY_NAME         61
#define ZEND_RETURN                   62
#define ZEND_RECV                     63
#define ZEND_RECV_INIT                64
#define ZEND_SEND_VAL                 65
#define ZEND_SEND_VAR                 66
#define ZEND_SEND_REF                 67
#define ZEND_NEW                      68
#define ZEND_FREE                     70
#define ZEND_INIT_ARRAY               71
#define ZEND_ADD_ARRAY_ELEMENT        72
#define ZEND_INCLUDE_OR_EVAL          73
#define ZEND_UNSET_VAR                74
#define ZEND_UNSET_DIM                75
#define ZEND_UNSET_OBJ                76
#define ZEND_FE_RESET                 77
#define ZEND_FE_FETCH                 78
#define ZEND_EXIT                     79
#define ZEND_FETCH_R                  80
#define ZEND_FETCH_DIM_R              81
#define ZEND_FETCH_OBJ_R              82
#define ZEND_FETCH_W                  83
#define ZEND_FETCH_DIM_W              84
#define ZEND_FETCH_OBJ_W              85
#define ZEND_FETCH_RW                 86
#define ZEND_FETCH_DIM_RW             87
#define ZEND_FETCH_OBJ_RW             88
#define ZEND_FETCH_IS                 89
#define ZEND_FETCH_DIM_IS             90
#define ZEND_FETCH_OBJ_IS             91
#define ZEND_FETCH_FUNC_ARG           92
#define ZEND_FETCH_DIM_FUNC_ARG       93
#define ZEND_FETCH_OBJ_FUNC_ARG       94
#define ZEND_FETCH_UNSET              95
#define ZEND_FETCH_DIM_UNSET          96
#define ZEND_FETCH_OBJ_UNSET          97
#define ZEND_FETCH_DIM_TMP_VAR        98
#define ZEND_FETCH_CONSTANT           99
#define ZEND_EXT_STMT                101
#define ZEND_EXT_FCALL_BEGIN         102
#define ZEND_EXT_FCALL_END           103
#define ZEND_EXT_NOP                 104
#define ZEND_TICKS                   105
#define ZEND_SEND_VAR_NO_REF         106
#define ZEND_CATCH                   107
#define ZEND_THROW                   108
#define ZEND_FETCH_CLASS             109
#define ZEND_CLONE                   110
#define ZEND_INIT_METHOD_CALL        112
#define ZEND_INIT_STATIC_METHOD_CALL 113
#define ZEND_ISSET_ISEMPTY_VAR       114
#define ZEND_ISSET_ISEMPTY_DIM_OBJ   115
#define ZEND_PRE_INC_OBJ             132
#define ZEND_PRE_DEC_OBJ             133
#define ZEND_POST_INC_OBJ            134
#define ZEND_POST_DEC_OBJ            135
#define ZEND_ASSIGN_OBJ              136
#define ZEND_INSTANCEOF              138
#define ZEND_DECLARE_CLASS           139
#define ZEND_DECLARE_INHERITED_CLASS 140
#define ZEND_DECLARE_FUNCTION        141
#define ZEND_RAISE_ABSTRACT_ERROR    142
#define ZEND_ADD_INTERFACE           144
#define ZEND_VERIFY_ABSTRACT_CLASS   146
#define ZEND_ASSIGN_DIM              147
#define ZEND_ISSET_ISEMPTY_PROP_OBJ  148
#define ZEND_HANDLE_EXCEPTION        149
#define ZEND_USER_OPCODE             150
