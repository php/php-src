/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Dmitry Stogov <dmitry@zend.com>                              |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ffi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define yy_buf  FFI_G(buf)
#define yy_end  FFI_G(end)
#define yy_pos  FFI_G(pos)
#define yy_text FFI_G(text)
#define yy_line FFI_G(line)

/* forward declarations */
static void yy_error(const char *msg);
static void yy_error_sym(const char *msg, int sym);

#define YYPOS cpos
#define YYEND cend

#define YY_EOF 0
#define YY__COMMA 1
#define YY__SEMICOLON 2
#define YY_TYPEDEF 3
#define YY_EXTERN 4
#define YY_STATIC 5
#define YY_AUTO 6
#define YY_REGISTER 7
#define YY_INLINE 8
#define YY___INLINE 9
#define YY___INLINE__ 10
#define YY__NORETURN 11
#define YY___CDECL 12
#define YY___STDCALL 13
#define YY___FASTCALL 14
#define YY___THISCALL 15
#define YY__ALIGNAS 16
#define YY__LPAREN 17
#define YY__RPAREN 18
#define YY_CONST 19
#define YY___CONST 20
#define YY___CONST__ 21
#define YY_RESTRICT 22
#define YY___RESTICT 23
#define YY___RESTRICT__ 24
#define YY_VOLATILE 25
#define YY___VOLATILE 26
#define YY___VOLATILE__ 27
#define YY__ATOMIC 28
#define YY_VOID 29
#define YY_CHAR 30
#define YY_SHORT 31
#define YY_INT 32
#define YY_LONG 33
#define YY_FLOAT 34
#define YY_DOUBLE 35
#define YY_SIGNED 36
#define YY_UNSIGNED 37
#define YY__BOOL 38
#define YY__COMPLEX 39
#define YY_COMPLEX 40
#define YY___COMPLEX 41
#define YY___COMPLEX__ 42
#define YY_STRUCT 43
#define YY_UNION 44
#define YY__LBRACE 45
#define YY__RBRACE 46
#define YY__COLON 47
#define YY_ENUM 48
#define YY__EQUAL 49
#define YY__STAR 50
#define YY__LBRACK 51
#define YY__RBRACK 52
#define YY__POINT_POINT_POINT 53
#define YY___ATTRIBUTE 54
#define YY___ATTRIBUTE__ 55
#define YY___DECLSPEC 56
#define YY__POINT 57
#define YY__QUERY 58
#define YY__BAR_BAR 59
#define YY__AND_AND 60
#define YY__BAR 61
#define YY__UPARROW 62
#define YY__AND 63
#define YY__EQUAL_EQUAL 64
#define YY__BANG_EQUAL 65
#define YY__LESS 66
#define YY__GREATER 67
#define YY__LESS_EQUAL 68
#define YY__GREATER_EQUAL 69
#define YY__LESS_LESS 70
#define YY__GREATER_GREATER 71
#define YY__PLUS 72
#define YY__MINUS 73
#define YY__SLASH 74
#define YY__PERCENT 75
#define YY__MINUS_GREATER 76
#define YY__PLUS_PLUS 77
#define YY__MINUS_MINUS 78
#define YY__TILDE 79
#define YY__BANG 80
#define YY_SIZEOF 81
#define YY__ALIGNOF 82
#define YY___ALIGNOF 83
#define YY___ALIGNOF__ 84
#define YY_ID 85
#define YY_OCTNUMBER 86
#define YY_DECNUMBER 87
#define YY_HEXNUMBER 88
#define YY_FLOATNUMBER 89
#define YY_STRING 90
#define YY_CHARACTER 91
#define YY_EOL 92
#define YY_WS 93
#define YY_ONE_LINE_COMMENT 94
#define YY_COMMENT 95

static const char * sym_name[] = {
	"<EOF>",
	",",
	";",
	"typedef",
	"extern",
	"static",
	"auto",
	"register",
	"inline",
	"__inline",
	"__inline__",
	"_Noreturn",
	"__cdecl",
	"__stdcall",
	"__fastcall",
	"__thiscall",
	"_Alignas",
	"(",
	")",
	"const",
	"__const",
	"__const__",
	"restrict",
	"__restict",
	"__restrict__",
	"volatile",
	"__volatile",
	"__volatile__",
	"_Atomic",
	"void",
	"char",
	"short",
	"int",
	"long",
	"float",
	"double",
	"signed",
	"unsigned",
	"_Bool",
	"_Complex",
	"complex",
	"__complex",
	"__complex__",
	"struct",
	"union",
	"{",
	"}",
	":",
	"enum",
	"=",
	"*",
	"[",
	"]",
	"...",
	"__attribute",
	"__attribute__",
	"__declspec",
	".",
	"?",
	"||",
	"&&",
	"|",
	"^",
	"&",
	"==",
	"!=",
	"<",
	">",
	"<=",
	">=",
	"<<",
	">>",
	"+",
	"-",
	"/",
	"%",
	"->",
	"++",
	"--",
	"~",
	"!",
	"sizeof",
	"_Alignof",
	"__alignof",
	"__alignof__",
	"<ID>",
	"<OCTNUMBER>",
	"<DECNUMBER>",
	"<HEXNUMBER>",
	"<FLOATNUMBER>",
	"<STRING>",
	"<CHARACTER>",
	"<EOL>",
	"<WS>",
	"<ONE_LINE_COMMENT>",
	"<COMMENT>",
	NULL
};

#define YY_IN_SET(sym, set, bitset) \
	(bitset[sym>>3] & (1 << (sym & 0x7)))

static int skip_EOL(int sym);
static int skip_WS(int sym);
static int skip_ONE_LINE_COMMENT(int sym);
static int skip_COMMENT(int sym);
static int get_sym(void);
static int check_specifier_qualifier_list(int sym);
static int check_type_qualifier_list(int sym);
static int check_type_qualifier(int sym);
static int check_type_specifier(int sym);
static int check_struct_or_union_specifier(int sym);
static int check_struct_contents(int sym);
static int check_struct_declaration(int sym);
static int check_struct_declarator(int sym);
static int check_enum_specifier(int sym);
static int check_enumerator_list(int sym);
static int check_enumerator(int sym);
static int check_declarator(int sym);
static int check_abstract_declarator(int sym);
static int check_nested_abstract_declarator(int sym);
static int check_pointer(int sym);
static int check_array_or_function_declarators(int sym);
static int check_parameter_declaration(int sym);
static int check_type_name(int sym);
static int check_attributes(int sym);
static int check_attrib(int sym);
static int check_expr_list(int sym);
static int check_expression(int sym);
static int check_assignment_expression(int sym);
static int check_constant_expression(int sym);
static int check_conditional_expression(int sym);
static int check_logical_or_expression(int sym);
static int check_logical_and_expression(int sym);
static int check_inclusive_or_expression(int sym);
static int check_exclusive_or_expression(int sym);
static int check_and_expression(int sym);
static int check_equality_expression(int sym);
static int check_relational_expression(int sym);
static int check_shift_expression(int sym);
static int check_additive_expression(int sym);
static int check_multiplicative_expression(int sym);
static int check_cast_expression(int sym);
static int check_unary_expression(int sym);
static int check_ID(int sym);
static int check_OCTNUMBER(int sym);
static int check_DECNUMBER(int sym);
static int check_HEXNUMBER(int sym);
static int check_FLOATNUMBER(int sym);
static int check_STRING(int sym);
static int check_CHARACTER(int sym);
static int parse_declarations(int sym);
static int parse_declaration_specifiers(int sym, zend_ffi_dcl *dcl);
static int parse_specifier_qualifier_list(int sym, zend_ffi_dcl *dcl);
static int parse_type_qualifier_list(int sym, zend_ffi_dcl *dcl);
static int parse_type_qualifier(int sym, zend_ffi_dcl *dcl);
static int parse_type_specifier(int sym, zend_ffi_dcl *dcl);
static int parse_struct_or_union_specifier(int sym, zend_ffi_dcl *dcl);
static int parse_struct_contents(int sym, zend_ffi_dcl *dcl);
static int parse_struct_declaration(int sym, zend_ffi_dcl *struct_dcl);
static int parse_struct_declarator(int sym, zend_ffi_dcl *struct_dcl, zend_ffi_dcl *field_dcl);
static int parse_enum_specifier(int sym, zend_ffi_dcl *dcl);
static int parse_enumerator_list(int sym, zend_ffi_dcl *enum_dcl);
static int parse_enumerator(int sym, zend_ffi_dcl *enum_dcl, int64_t *min, int64_t *max, int64_t *last);
static int parse_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len);
static int parse_abstract_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len);
static int parse_nested_abstract_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len);
static int parse_pointer(int sym, zend_ffi_dcl *dcl);
static int parse_array_or_function_declarators(int sym, zend_ffi_dcl *dcl);
static int parse_parameter_declaration(int sym, HashTable **args);
static int parse_type_name(int sym, zend_ffi_dcl *dcl);
static int parse_attributes(int sym, zend_ffi_dcl *dcl);
static int parse_attrib(int sym, zend_ffi_dcl *dcl);
static int parse_initializer(int sym);
static int parse_designation(int sym);
static int parse_expr_list(int sym);
static int parse_expression(int sym, zend_ffi_val *val);
static int parse_assignment_expression(int sym, zend_ffi_val *val);
static int parse_constant_expression(int sym, zend_ffi_val *val);
static int parse_conditional_expression(int sym, zend_ffi_val *val);
static int parse_logical_or_expression(int sym, zend_ffi_val *val);
static int parse_logical_and_expression(int sym, zend_ffi_val *val);
static int parse_inclusive_or_expression(int sym, zend_ffi_val *val);
static int parse_exclusive_or_expression(int sym, zend_ffi_val *val);
static int parse_and_expression(int sym, zend_ffi_val *val);
static int parse_equality_expression(int sym, zend_ffi_val *val);
static int parse_relational_expression(int sym, zend_ffi_val *val);
static int parse_shift_expression(int sym, zend_ffi_val *val);
static int parse_additive_expression(int sym, zend_ffi_val *val);
static int parse_multiplicative_expression(int sym, zend_ffi_val *val);
static int parse_cast_expression(int sym, zend_ffi_val *val);
static int parse_unary_expression(int sym, zend_ffi_val *val);
static int parse_ID(int sym, const char **name, size_t *name_len);
static int parse_OCTNUMBER(int sym, zend_ffi_val *val);
static int parse_DECNUMBER(int sym, zend_ffi_val *val);
static int parse_HEXNUMBER(int sym, zend_ffi_val *val);
static int parse_FLOATNUMBER(int sym, zend_ffi_val *val);
static int parse_STRING(int sym, zend_ffi_val *val);
static int parse_CHARACTER(int sym, zend_ffi_val *val);
static int synpred_1(int sym);
static int synpred_2(int sym);
static int synpred_3(int sym);
static int synpred_4(int sym);
static int synpred_5(int sym);
static int synpred_6(int sym);
static int synpred_7(int sym);

static int get_skip_sym(void) {
	int ch;
	int ret;
	int accept = -1;
	const unsigned char *accept_pos;
	const unsigned char *cpos = yy_pos;
	const unsigned char *cend = yy_end;

_yy_state_start:
	yy_text = YYPOS;
	ch = *YYPOS;
	switch (ch) {
		case 't':
			ch = *++YYPOS;
			if (ch != 'y') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'p') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'e') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'd') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'e') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'f') goto _yy_tunnel_15;
			ret = YY_TYPEDEF;
			goto _yy_state_366;
		case 'e':
			ch = *++YYPOS;
			if (ch == 'x') {
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'e') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'r') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'n') goto _yy_tunnel_15;
				ret = YY_EXTERN;
				goto _yy_state_366;
			} else if (ch == 'n') {
				ch = *++YYPOS;
				if (ch != 'u') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'm') goto _yy_tunnel_15;
				ret = YY_ENUM;
				goto _yy_state_366;
			} else {
				goto _yy_tunnel_15;
			}
		case 's':
			ch = *++YYPOS;
			if (ch == 't') {
				ch = *++YYPOS;
				if (ch == 'a') {
					ch = *++YYPOS;
					if (ch != 't') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'i') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'c') goto _yy_tunnel_15;
					ret = YY_STATIC;
					goto _yy_state_366;
				} else if (ch == 'r') {
					ch = *++YYPOS;
					if (ch != 'u') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'c') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 't') goto _yy_tunnel_15;
					ret = YY_STRUCT;
					goto _yy_state_366;
				} else {
					goto _yy_tunnel_15;
				}
			} else if (ch == 'h') {
				ch = *++YYPOS;
				if (ch != 'o') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'r') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ret = YY_SHORT;
				goto _yy_state_366;
			} else if (ch == 'i') {
				ch = *++YYPOS;
				if (ch == 'g') {
					ch = *++YYPOS;
					if (ch != 'n') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'd') goto _yy_tunnel_15;
					ret = YY_SIGNED;
					goto _yy_state_366;
				} else if (ch == 'z') {
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'f') goto _yy_tunnel_15;
					ret = YY_SIZEOF;
					goto _yy_state_366;
				} else {
					goto _yy_tunnel_15;
				}
			} else {
				goto _yy_tunnel_15;
			}
		case 'a':
			ch = *++YYPOS;
			if (ch != 'u') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 't') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'o') goto _yy_tunnel_15;
			ret = YY_AUTO;
			goto _yy_state_366;
		case 'r':
			ch = *++YYPOS;
			if (ch != 'e') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch == 'g') {
				ch = *++YYPOS;
				if (ch != 'i') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 's') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'e') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'r') goto _yy_tunnel_15;
				ret = YY_REGISTER;
				goto _yy_state_366;
			} else if (ch == 's') {
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'r') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'i') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'c') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ret = YY_RESTRICT;
				goto _yy_state_366;
			} else {
				goto _yy_tunnel_15;
			}
		case 'i':
			ch = *++YYPOS;
			if (ch != 'n') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch == 'l') {
				ch = *++YYPOS;
				if (ch != 'i') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'n') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'e') goto _yy_tunnel_15;
				ret = YY_INLINE;
				goto _yy_state_366;
			} else if (ch == 't') {
				ret = YY_INT;
				goto _yy_state_366;
			} else {
				goto _yy_tunnel_15;
			}
		case '_':
			ch = *++YYPOS;
			switch (ch) {
				case '_':
					ch = *++YYPOS;
					switch (ch) {
						case 'i':
							ch = *++YYPOS;
							if (ch != 'n') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'i') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'n') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'e') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != '_') {ret = YY___INLINE; goto _yy_tunnel_366;}
							ch = *++YYPOS;
							if (ch != '_') goto _yy_tunnel_15;
							ret = YY___INLINE__;
							goto _yy_state_366;
						case 'c':
							ch = *++YYPOS;
							if (ch == 'd') {
								ch = *++YYPOS;
								if (ch != 'e') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'c') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'l') goto _yy_tunnel_15;
								ret = YY___CDECL;
								goto _yy_state_366;
							} else if (ch == 'o') {
								ch = *++YYPOS;
								if (ch == 'm') {
									ch = *++YYPOS;
									if (ch != 'p') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != 'l') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != 'e') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != 'x') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != '_') {ret = YY___COMPLEX; goto _yy_tunnel_366;}
									ch = *++YYPOS;
									if (ch != '_') goto _yy_tunnel_15;
									ret = YY___COMPLEX__;
									goto _yy_state_366;
								} else if (ch == 'n') {
									ch = *++YYPOS;
									if (ch != 's') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != 't') goto _yy_tunnel_15;
									ch = *++YYPOS;
									if (ch != '_') {ret = YY___CONST; goto _yy_tunnel_366;}
									ch = *++YYPOS;
									if (ch != '_') goto _yy_tunnel_15;
									ret = YY___CONST__;
									goto _yy_state_366;
								} else {
									goto _yy_tunnel_15;
								}
							} else {
								goto _yy_tunnel_15;
							}
						case 's':
							ch = *++YYPOS;
							if (ch != 't') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'd') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'c') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'a') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ret = YY___STDCALL;
							goto _yy_state_366;
						case 'f':
							ch = *++YYPOS;
							if (ch != 'a') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 's') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 't') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'c') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'a') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ret = YY___FASTCALL;
							goto _yy_state_366;
						case 't':
							ch = *++YYPOS;
							if (ch != 'h') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'i') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 's') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'c') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'a') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ret = YY___THISCALL;
							goto _yy_state_366;
						case 'a':
							ch = *++YYPOS;
							if (ch == 't') {
								ch = *++YYPOS;
								if (ch != 't') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'r') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'i') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'b') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'u') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 't') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'e') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != '_') {ret = YY___ATTRIBUTE; goto _yy_tunnel_366;}
								ch = *++YYPOS;
								if (ch != '_') goto _yy_tunnel_15;
								ret = YY___ATTRIBUTE__;
								goto _yy_state_366;
							} else if (ch == 'l') {
								ch = *++YYPOS;
								if (ch != 'i') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'g') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'n') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'o') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'f') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != '_') {ret = YY___ALIGNOF; goto _yy_tunnel_366;}
								ch = *++YYPOS;
								if (ch != '_') goto _yy_tunnel_15;
								ret = YY___ALIGNOF__;
								goto _yy_state_366;
							} else {
								goto _yy_tunnel_15;
							}
						case 'd':
							ch = *++YYPOS;
							if (ch != 'e') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'c') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 's') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'p') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'e') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'c') goto _yy_tunnel_15;
							ret = YY___DECLSPEC;
							goto _yy_state_366;
						case 'r':
							ch = *++YYPOS;
							if (ch != 'e') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 's') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 't') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch == 'i') {
								ch = *++YYPOS;
								if (ch != 'c') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 't') goto _yy_tunnel_15;
								ret = YY___RESTICT;
								goto _yy_state_366;
							} else if (ch == 'r') {
								ch = *++YYPOS;
								if (ch != 'i') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 'c') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != 't') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != '_') goto _yy_tunnel_15;
								ch = *++YYPOS;
								if (ch != '_') goto _yy_tunnel_15;
								ret = YY___RESTRICT__;
								goto _yy_state_366;
							} else {
								goto _yy_tunnel_15;
							}
						case 'v':
							ch = *++YYPOS;
							if (ch != 'o') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'a') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 't') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'i') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'l') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != 'e') goto _yy_tunnel_15;
							ch = *++YYPOS;
							if (ch != '_') {ret = YY___VOLATILE; goto _yy_tunnel_366;}
							ch = *++YYPOS;
							if (ch != '_') goto _yy_tunnel_15;
							ret = YY___VOLATILE__;
							goto _yy_state_366;
						default:
							goto _yy_tunnel_15;
					}
				case 'N':
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'r') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 't') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'u') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'r') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'n') goto _yy_tunnel_15;
					ret = YY__NORETURN;
					goto _yy_state_366;
				case 'A':
					ch = *++YYPOS;
					if (ch == 'l') {
						ch = *++YYPOS;
						if (ch != 'i') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch != 'g') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch != 'n') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch == 'a') {
							ch = *++YYPOS;
							if (ch != 's') goto _yy_tunnel_15;
							ret = YY__ALIGNAS;
							goto _yy_state_366;
						} else if (ch == 'o') {
							ch = *++YYPOS;
							if (ch != 'f') goto _yy_tunnel_15;
							ret = YY__ALIGNOF;
							goto _yy_state_366;
						} else {
							goto _yy_tunnel_15;
						}
					} else if (ch == 't') {
						ch = *++YYPOS;
						if (ch != 'o') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch != 'm') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch != 'i') goto _yy_tunnel_15;
						ch = *++YYPOS;
						if (ch != 'c') goto _yy_tunnel_15;
						ret = YY__ATOMIC;
						goto _yy_state_366;
					} else {
						goto _yy_tunnel_15;
					}
				case 'B':
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'l') goto _yy_tunnel_15;
					ret = YY__BOOL;
					goto _yy_state_366;
				case 'C':
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'm') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'p') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'l') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'x') goto _yy_tunnel_15;
					ret = YY__COMPLEX;
					goto _yy_state_366;
				default:
					goto _yy_tunnel_15;
			}
		case '(':
			YYPOS++;
			ret = YY__LPAREN;
			goto _yy_fin;
		case 'v':
			ch = *++YYPOS;
			if (ch != 'o') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch == 'i') {
				ch = *++YYPOS;
				if (ch != 'd') goto _yy_tunnel_15;
				ret = YY_VOID;
				goto _yy_state_366;
			} else if (ch == 'l') {
				ch = *++YYPOS;
				if (ch != 'a') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 't') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'i') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'l') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'e') goto _yy_tunnel_15;
				ret = YY_VOLATILE;
				goto _yy_state_366;
			} else {
				goto _yy_tunnel_15;
			}
		case 'c':
			ch = *++YYPOS;
			if (ch == 'h') {
				ch = *++YYPOS;
				if (ch != 'a') goto _yy_tunnel_15;
				ch = *++YYPOS;
				if (ch != 'r') goto _yy_tunnel_15;
				ret = YY_CHAR;
				goto _yy_state_366;
			} else if (ch == 'o') {
				ch = *++YYPOS;
				if (ch == 'm') {
					ch = *++YYPOS;
					if (ch != 'p') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'l') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'x') goto _yy_tunnel_15;
					ret = YY_COMPLEX;
					goto _yy_state_366;
				} else if (ch == 'n') {
					ch = *++YYPOS;
					if (ch != 's') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 't') goto _yy_tunnel_15;
					ret = YY_CONST;
					goto _yy_state_366;
				} else {
					goto _yy_tunnel_15;
				}
			} else {
				goto _yy_tunnel_15;
			}
		case 'l':
			ch = *++YYPOS;
			if (ch != 'o') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'n') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'g') goto _yy_tunnel_15;
			ret = YY_LONG;
			goto _yy_state_366;
		case 'f':
			ch = *++YYPOS;
			if (ch != 'l') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'o') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'a') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 't') goto _yy_tunnel_15;
			ret = YY_FLOAT;
			goto _yy_state_366;
		case 'd':
			ch = *++YYPOS;
			if (ch != 'o') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'u') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'b') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'l') goto _yy_tunnel_15;
			ch = *++YYPOS;
			if (ch != 'e') goto _yy_tunnel_15;
			ret = YY_DOUBLE;
			goto _yy_state_366;
		case 'u':
			ch = *++YYPOS;
			if (ch == 'n') {
				ch = *++YYPOS;
				if (ch == 's') {
					ch = *++YYPOS;
					if (ch != 'i') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'g') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'n') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'e') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'd') goto _yy_tunnel_15;
					ret = YY_UNSIGNED;
					goto _yy_state_366;
				} else if (ch == 'i') {
					ch = *++YYPOS;
					if (ch != 'o') goto _yy_tunnel_15;
					ch = *++YYPOS;
					if (ch != 'n') goto _yy_tunnel_15;
					ret = YY_UNION;
					goto _yy_state_366;
				} else {
					goto _yy_tunnel_15;
				}
			} else if (ch == '8') {
				ch = *++YYPOS;
				if (ch != '"') goto _yy_tunnel_15;
				goto _yy_state_27;
			} else if (ch == '"') {
				goto _yy_state_27;
			} else if (ch == '\'') {
				goto _yy_state_28;
			} else {
				goto _yy_tunnel_15;
			}
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case 'b':
		case 'g':
		case 'h':
		case 'j':
		case 'k':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
			goto _yy_state_15;
		case 'L':
		case 'U':
			ch = *++YYPOS;
			if (ch == '"') {
				goto _yy_state_27;
			} else if (ch == '\'') {
				goto _yy_state_28;
			} else {
				goto _yy_tunnel_15;
			}
		case ')':
			YYPOS++;
			ret = YY__RPAREN;
			goto _yy_fin;
		case '[':
			YYPOS++;
			ret = YY__LBRACK;
			goto _yy_fin;
		case ',':
			YYPOS++;
			ret = YY__COMMA;
			goto _yy_fin;
		case ']':
			YYPOS++;
			ret = YY__RBRACK;
			goto _yy_fin;
		case '.':
			ch = *++YYPOS;
			accept = YY__POINT;
			accept_pos = yy_pos;
			if ((ch >= '0' && ch <= '9')) {
				goto _yy_state_73;
			} else if (ch == '.') {
				ch = *++YYPOS;
				if (ch == '.') {
					YYPOS++;
					ret = YY__POINT_POINT_POINT;
					goto _yy_fin;
				} else {
					goto _yy_state_error;
				}
			} else {
				ret = YY__POINT;
				goto _yy_fin;
			}
		case '-':
			ch = *++YYPOS;
			if (ch == '>') {
				YYPOS++;
				ret = YY__MINUS_GREATER;
				goto _yy_fin;
			} else if (ch == '-') {
				YYPOS++;
				ret = YY__MINUS_MINUS;
				goto _yy_fin;
			} else {
				ret = YY__MINUS;
				goto _yy_fin;
			}
		case '+':
			ch = *++YYPOS;
			if (ch == '+') {
				YYPOS++;
				ret = YY__PLUS_PLUS;
				goto _yy_fin;
			} else {
				ret = YY__PLUS;
				goto _yy_fin;
			}
		case '0':
			ch = *++YYPOS;
			if (ch != 'X' && ch != 'x') goto _yy_tunnel_78;
			ch = *++YYPOS;
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
				goto _yy_state_155;
			} else {
				goto _yy_state_error;
			}
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			goto _yy_state_26;
		case '"':
			goto _yy_state_27;
		case '\'':
			goto _yy_state_28;
		case '&':
			ch = *++YYPOS;
			if (ch == '&') {
				YYPOS++;
				ret = YY__AND_AND;
				goto _yy_fin;
			} else {
				ret = YY__AND;
				goto _yy_fin;
			}
		case '*':
			YYPOS++;
			ret = YY__STAR;
			goto _yy_fin;
		case '~':
			YYPOS++;
			ret = YY__TILDE;
			goto _yy_fin;
		case '!':
			ch = *++YYPOS;
			if (ch == '=') {
				YYPOS++;
				ret = YY__BANG_EQUAL;
				goto _yy_fin;
			} else {
				ret = YY__BANG;
				goto _yy_fin;
			}
		case '/':
			ch = *++YYPOS;
			accept = YY__SLASH;
			accept_pos = yy_pos;
			if (ch == '*') {
				goto _yy_state_99;
			} else if (ch == '/') {
				goto _yy_state_48;
			} else {
				ret = YY__SLASH;
				goto _yy_fin;
			}
		case '%':
			YYPOS++;
			ret = YY__PERCENT;
			goto _yy_fin;
		case '<':
			ch = *++YYPOS;
			if (ch == '<') {
				YYPOS++;
				ret = YY__LESS_LESS;
				goto _yy_fin;
			} else if (ch == '=') {
				YYPOS++;
				ret = YY__LESS_EQUAL;
				goto _yy_fin;
			} else {
				ret = YY__LESS;
				goto _yy_fin;
			}
		case '>':
			ch = *++YYPOS;
			if (ch == '>') {
				YYPOS++;
				ret = YY__GREATER_GREATER;
				goto _yy_fin;
			} else if (ch == '=') {
				YYPOS++;
				ret = YY__GREATER_EQUAL;
				goto _yy_fin;
			} else {
				ret = YY__GREATER;
				goto _yy_fin;
			}
		case '=':
			ch = *++YYPOS;
			if (ch == '=') {
				YYPOS++;
				ret = YY__EQUAL_EQUAL;
				goto _yy_fin;
			} else {
				ret = YY__EQUAL;
				goto _yy_fin;
			}
		case '^':
			YYPOS++;
			ret = YY__UPARROW;
			goto _yy_fin;
		case '|':
			ch = *++YYPOS;
			if (ch == '|') {
				YYPOS++;
				ret = YY__BAR_BAR;
				goto _yy_fin;
			} else {
				ret = YY__BAR;
				goto _yy_fin;
			}
		case '?':
			YYPOS++;
			ret = YY__QUERY;
			goto _yy_fin;
		case ':':
			YYPOS++;
			ret = YY__COLON;
			goto _yy_fin;
		case '{':
			YYPOS++;
			ret = YY__LBRACE;
			goto _yy_fin;
		case ';':
			YYPOS++;
			ret = YY__SEMICOLON;
			goto _yy_fin;
		case '}':
			YYPOS++;
			ret = YY__RBRACE;
			goto _yy_fin;
		case '\r':
			ch = *++YYPOS;
			if (ch == '\n') {
				yy_line++;
				YYPOS++;
				ret = YY_EOL;
				goto _yy_fin;
			} else {
				ret = YY_EOL;
				goto _yy_fin;
			}
		case '\n':
			yy_line++;
			YYPOS++;
			ret = YY_EOL;
			goto _yy_fin;
		case ' ':
		case '\t':
		case '\f':
		case '\v':
			goto _yy_state_47;
		case '#':
			goto _yy_state_48;
		case '\0':
			if (ch == 0 && YYPOS < YYEND) goto _yy_state_error;
			YYPOS++;
			ret = YY_EOF;
			goto _yy_fin;
		default:
			goto _yy_state_error;
	}
_yy_state_15:
	ch = *++YYPOS;
_yy_tunnel_15:
	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {
		goto _yy_state_15;
	} else {
		ret = YY_ID;
		goto _yy_fin;
	}
_yy_state_26:
	ch = *++YYPOS;
	accept = YY_DECNUMBER;
	accept_pos = yy_pos;
	switch (ch) {
		case 'U':
		case 'u':
			ch = *++YYPOS;
			if (ch == 'L') {
				ch = *++YYPOS;
				if (ch == 'L') {
					YYPOS++;
					ret = YY_DECNUMBER;
					goto _yy_fin;
				} else {
					ret = YY_DECNUMBER;
					goto _yy_fin;
				}
			} else if (ch == 'l') {
				YYPOS++;
				ret = YY_DECNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_DECNUMBER;
				goto _yy_fin;
			}
		case 'L':
			ch = *++YYPOS;
			accept = YY_DECNUMBER;
			accept_pos = yy_pos;
			if (ch == 'L') {
				goto _yy_state_162;
			} else if (ch == 'U' || ch == 'u') {
				YYPOS++;
				ret = YY_DECNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_DECNUMBER;
				goto _yy_fin;
			}
		case 'l':
			ch = *++YYPOS;
			accept = YY_DECNUMBER;
			accept_pos = yy_pos;
			if (ch == 'U' || ch == 'u') {
				YYPOS++;
				ret = YY_DECNUMBER;
				goto _yy_fin;
			} else if (ch == 'l') {
				goto _yy_state_162;
			} else {
				ret = YY_DECNUMBER;
				goto _yy_fin;
			}
		case 'E':
		case 'e':
			goto _yy_state_85;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			goto _yy_state_26;
		case '.':
			goto _yy_state_73;
		default:
			ret = YY_DECNUMBER;
			goto _yy_fin;
	}
_yy_state_27:
	ch = *++YYPOS;
	if (ch == '\\') {
		ch = *++YYPOS;
		if (YYPOS < YYEND) {
			if (ch == '\n') {
				yy_line++;
			}
			goto _yy_state_27;
		} else {
			goto _yy_state_error;
		}
	} else if (ch == '"') {
		YYPOS++;
		ret = YY_STRING;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '!' || (ch >= '#' && ch <= '[') || ch >= ']')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_27;
	} else {
		goto _yy_state_error;
	}
_yy_state_28:
	ch = *++YYPOS;
	if (ch == '\\') {
		ch = *++YYPOS;
		if (YYPOS < YYEND) {
			if (ch == '\n') {
				yy_line++;
			}
			goto _yy_state_28;
		} else {
			goto _yy_state_error;
		}
	} else if (ch == '\'') {
		YYPOS++;
		ret = YY_CHARACTER;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '&' || (ch >= '(' && ch <= '[') || ch >= ']')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_28;
	} else {
		goto _yy_state_error;
	}
_yy_state_47:
	ch = *++YYPOS;
	if (ch == '\t' || ch == '\v' || ch == '\f' || ch == ' ') {
		goto _yy_state_47;
	} else {
		ret = YY_WS;
		goto _yy_fin;
	}
_yy_state_48:
	ch = *++YYPOS;
	if (ch == '\r') {
		ch = *++YYPOS;
		if (ch == '\n') {
			yy_line++;
			YYPOS++;
			ret = YY_ONE_LINE_COMMENT;
			goto _yy_fin;
		} else {
			ret = YY_ONE_LINE_COMMENT;
			goto _yy_fin;
		}
	} else if (ch == '\n') {
		yy_line++;
		YYPOS++;
		ret = YY_ONE_LINE_COMMENT;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= '\t' || ch == '\v' || ch == '\f' || ch >= '\016')) {
		goto _yy_state_48;
	} else {
		goto _yy_state_error;
	}
_yy_state_73:
	ch = *++YYPOS;
	accept = YY_FLOATNUMBER;
	accept_pos = yy_pos;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_73;
	} else if (ch == 'F' || ch == 'L' || ch == 'f' || ch == 'l') {
		YYPOS++;
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	} else if (ch == 'E' || ch == 'e') {
		goto _yy_state_85;
	} else {
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	}
_yy_state_78:
	ch = *++YYPOS;
_yy_tunnel_78:
	accept = YY_OCTNUMBER;
	accept_pos = yy_pos;
	switch (ch) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			goto _yy_state_78;
		case 'U':
		case 'u':
			ch = *++YYPOS;
			if (ch == 'L') {
				ch = *++YYPOS;
				if (ch == 'L') {
					YYPOS++;
					ret = YY_OCTNUMBER;
					goto _yy_fin;
				} else {
					ret = YY_OCTNUMBER;
					goto _yy_fin;
				}
			} else if (ch == 'l') {
				YYPOS++;
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			}
		case 'L':
			ch = *++YYPOS;
			accept = YY_OCTNUMBER;
			accept_pos = yy_pos;
			if (ch == 'L') {
				goto _yy_state_153;
			} else if (ch == 'U' || ch == 'u') {
				YYPOS++;
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			}
		case 'l':
			ch = *++YYPOS;
			accept = YY_OCTNUMBER;
			accept_pos = yy_pos;
			if (ch == 'l') {
				goto _yy_state_153;
			} else if (ch == 'U' || ch == 'u') {
				YYPOS++;
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_OCTNUMBER;
				goto _yy_fin;
			}
		case '8':
		case '9':
			goto _yy_state_83;
		case 'E':
		case 'e':
			goto _yy_state_85;
		case '.':
			goto _yy_state_73;
		default:
			ret = YY_OCTNUMBER;
			goto _yy_fin;
	}
_yy_state_83:
	ch = *++YYPOS;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_83;
	} else if (ch == 'E' || ch == 'e') {
		goto _yy_state_85;
	} else if (ch == '.') {
		goto _yy_state_73;
	} else {
		goto _yy_state_error;
	}
_yy_state_85:
	ch = *++YYPOS;
	if (ch == '+' || ch == '-') {
		ch = *++YYPOS;
		if ((ch >= '0' && ch <= '9')) {
			goto _yy_state_158;
		} else {
			goto _yy_state_error;
		}
	} else if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_158;
	} else {
		goto _yy_state_error;
	}
_yy_state_99:
	ch = *++YYPOS;
_yy_tunnel_99:
	if (ch == '*') {
		ch = *++YYPOS;
		if (ch != '/') goto _yy_tunnel_99;
		YYPOS++;
		ret = YY_COMMENT;
		goto _yy_fin;
	} else if (YYPOS < YYEND && (ch <= ')' || ch >= '+')) {
		if (ch == '\n') {
			yy_line++;
		}
		goto _yy_state_99;
	} else {
		goto _yy_state_error;
	}
_yy_state_153:
	ch = *++YYPOS;
	if (ch == 'U' || ch == 'u') {
		YYPOS++;
		ret = YY_OCTNUMBER;
		goto _yy_fin;
	} else {
		goto _yy_state_error;
	}
_yy_state_155:
	ch = *++YYPOS;
	if (ch == 'U' || ch == 'u') {
		ch = *++YYPOS;
		if (ch == 'L') {
			ch = *++YYPOS;
			if (ch == 'L') {
				YYPOS++;
				ret = YY_HEXNUMBER;
				goto _yy_fin;
			} else {
				ret = YY_HEXNUMBER;
				goto _yy_fin;
			}
		} else if (ch == 'l') {
			YYPOS++;
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		} else {
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		}
	} else if (ch == 'L') {
		ch = *++YYPOS;
		accept = YY_HEXNUMBER;
		accept_pos = yy_pos;
		if (ch == 'L') {
			goto _yy_state_258;
		} else if (ch == 'U' || ch == 'u') {
			YYPOS++;
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		} else {
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		}
	} else if (ch == 'l') {
		ch = *++YYPOS;
		accept = YY_HEXNUMBER;
		accept_pos = yy_pos;
		if (ch == 'U' || ch == 'u') {
			YYPOS++;
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		} else if (ch == 'l') {
			goto _yy_state_258;
		} else {
			ret = YY_HEXNUMBER;
			goto _yy_fin;
		}
	} else if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) {
		goto _yy_state_155;
	} else {
		ret = YY_HEXNUMBER;
		goto _yy_fin;
	}
_yy_state_158:
	ch = *++YYPOS;
	if ((ch >= '0' && ch <= '9')) {
		goto _yy_state_158;
	} else if (ch == 'F' || ch == 'L' || ch == 'f' || ch == 'l') {
		YYPOS++;
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	} else {
		ret = YY_FLOATNUMBER;
		goto _yy_fin;
	}
_yy_state_162:
	ch = *++YYPOS;
	if (ch == 'U' || ch == 'u') {
		YYPOS++;
		ret = YY_DECNUMBER;
		goto _yy_fin;
	} else {
		goto _yy_state_error;
	}
_yy_state_258:
	ch = *++YYPOS;
	if (ch == 'U' || ch == 'u') {
		YYPOS++;
		ret = YY_HEXNUMBER;
		goto _yy_fin;
	} else {
		goto _yy_state_error;
	}
_yy_state_366:
	ch = *++YYPOS;
_yy_tunnel_366:
	if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {
		goto _yy_state_15;
	} else {
		goto _yy_fin;
	}
_yy_state_error:
	if (accept >= 0) {
		yy_pos = accept_pos;
		return accept;
	}
	if (YYPOS >= YYEND) {
		yy_error("unexpected <EOF>");
	} else if (YYPOS == yy_text) {
		yy_error("unexpected character 'escape_char(ch)'");
	} else {
		yy_error("unexpected sequence 'escape_string(yy_text, 1 + YYPOS - yy_text))'");
	}
	YYPOS++;
	goto _yy_state_start;
_yy_fin:
	yy_pos = YYPOS;
	return ret;
}

static int skip_EOL(int sym) {
	if (sym != YY_EOL) {
		yy_error_sym("<EOL> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_WS(int sym) {
	if (sym != YY_WS) {
		yy_error_sym("<WS> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_ONE_LINE_COMMENT(int sym) {
	if (sym != YY_ONE_LINE_COMMENT) {
		yy_error_sym("<ONE_LINE_COMMENT> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int skip_COMMENT(int sym) {
	if (sym != YY_COMMENT) {
		yy_error_sym("<COMMENT> expected, got", sym);
	}
	sym = get_skip_sym();
	return sym;
}

static int get_sym(void) {
	int sym;
	sym = get_skip_sym();
	while (sym == YY_EOL || sym == YY_WS || sym == YY_ONE_LINE_COMMENT || sym == YY_COMMENT) {
		if (sym == YY_EOL) {
			sym = skip_EOL(sym);
		} else if (sym == YY_WS) {
			sym = skip_WS(sym);
		} else if (sym == YY_ONE_LINE_COMMENT) {
			sym = skip_ONE_LINE_COMMENT(sym);
		} else if (sym == YY_COMMENT) {
			sym = skip_COMMENT(sym);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int check_specifier_qualifier_list(int sym) {
	do {
		if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID), "\000\000\000\340\377\037\001\000\000\000\040\000")) {
			sym = check_type_specifier(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC), "\000\000\370\037\000\000\000\000\000\000\000\000")) {
			sym = check_type_qualifier(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = check_attributes(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	} while ((YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) && (sym != YY_ID || zend_ffi_is_typedef_name((const char*)yy_text, yy_pos - yy_text)));
	return sym;
}

static int check_type_qualifier_list(int sym) {
	do {
		if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC), "\000\000\370\037\000\000\000\000\000\000\000\000")) {
			sym = check_type_qualifier(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = check_attributes(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	} while (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000"));
	return sym;
}

static int check_type_qualifier(int sym) {
	if (sym == YY_CONST || sym == YY___CONST || sym == YY___CONST__) {
		if (sym == YY_CONST) {
			sym = get_sym();
		} else if (sym == YY___CONST) {
			sym = get_sym();
		} else if (sym == YY___CONST__) {
			sym = get_sym();
		} else {
			return -1;
		}
	} else if (sym == YY_RESTRICT || sym == YY___RESTICT || sym == YY___RESTRICT__) {
		if (sym == YY_RESTRICT) {
			sym = get_sym();
		} else if (sym == YY___RESTICT) {
			sym = get_sym();
		} else if (sym == YY___RESTRICT__) {
			sym = get_sym();
		} else {
			return -1;
		}
	} else if (sym == YY_VOLATILE || sym == YY___VOLATILE || sym == YY___VOLATILE__) {
		if (sym == YY_VOLATILE) {
			sym = get_sym();
		} else if (sym == YY___VOLATILE) {
			sym = get_sym();
		} else if (sym == YY___VOLATILE__) {
			sym = get_sym();
		} else {
			return -1;
		}
	} else if (sym == YY__ATOMIC) {
		sym = get_sym();
	} else {
		return -1;
	}
	return sym;
}

static int check_type_specifier(int sym) {
	switch (sym) {
		case YY_VOID:
			sym = get_sym();
			break;
		case YY_CHAR:
			sym = get_sym();
			break;
		case YY_SHORT:
			sym = get_sym();
			break;
		case YY_INT:
			sym = get_sym();
			break;
		case YY_LONG:
			sym = get_sym();
			break;
		case YY_FLOAT:
			sym = get_sym();
			break;
		case YY_DOUBLE:
			sym = get_sym();
			break;
		case YY_SIGNED:
			sym = get_sym();
			break;
		case YY_UNSIGNED:
			sym = get_sym();
			break;
		case YY__BOOL:
			sym = get_sym();
			break;
		case YY__COMPLEX:
		case YY_COMPLEX:
		case YY___COMPLEX:
		case YY___COMPLEX__:
			if (sym == YY__COMPLEX) {
				sym = get_sym();
			} else if (sym == YY_COMPLEX) {
				sym = get_sym();
			} else if (sym == YY___COMPLEX) {
				sym = get_sym();
			} else if (sym == YY___COMPLEX__) {
				sym = get_sym();
			} else {
				return -1;
			}
			break;
		case YY_STRUCT:
		case YY_UNION:
			sym = check_struct_or_union_specifier(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_ENUM:
			sym = check_enum_specifier(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_ID:
			sym = check_ID(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		default:
			return -1;
	}
	return sym;
}

static int check_struct_or_union_specifier(int sym) {
	if (sym == YY_STRUCT) {
		sym = get_sym();
	} else if (sym == YY_UNION) {
		sym = get_sym();
	} else {
		return -1;
	}
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = check_attributes(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if (sym == YY_ID) {
		sym = check_ID(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym == YY__LBRACE) {
			sym = check_struct_contents(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else if (sym == YY__LBRACE) {
		sym = check_struct_contents(sym);
		if (sym == -1) {
			return -1;
		}
	} else {
		return -1;
	}
	return sym;
}

static int check_struct_contents(int sym) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt2;
	if (sym != YY__LBRACE) {
		return -1;
	}
	sym = get_sym();
	if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
		sym = check_struct_declaration(sym);
		if (sym == -1) {
			return -1;
		}
		while (1) {
			save_pos  = yy_pos;
			save_text = yy_text;
			save_line = yy_line;
			alt2 = -2;
			sym2 = sym;
			if (sym2 == YY__SEMICOLON) {
				sym2 = get_sym();
				goto _yy_state_2_1;
			} else if (sym2 == YY__RBRACE) {
				alt2 = 6;
				goto _yy_state_2;
			} else {
				return -1;
			}
_yy_state_2_1:
			if (YY_IN_SET(sym2, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
				alt2 = 3;
				goto _yy_state_2;
			} else if (sym2 == YY__RBRACE) {
				alt2 = 5;
				goto _yy_state_2;
			} else {
				return -1;
			}
_yy_state_2:
			yy_pos  = save_pos;
			yy_text = save_text;
			yy_line = save_line;
			if (alt2 != 3) {
				break;
			}
			sym = get_sym();
			sym = check_struct_declaration(sym);
			if (sym == -1) {
				return -1;
			}
		}
		if (alt2 == 5) {
			sym = get_sym();
		}
	}
	if (sym != YY__RBRACE) {
		return -1;
	}
	sym = get_sym();
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = check_attributes(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_struct_declaration(int sym) {
	sym = check_specifier_qualifier_list(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym == YY__SEMICOLON || sym == YY__RBRACE) {
	} else if (sym == YY__STAR || sym == YY_ID || sym == YY__LPAREN || sym == YY__COLON) {
		sym = check_struct_declarator(sym);
		if (sym == -1) {
			return -1;
		}
		while (sym == YY__COMMA) {
			sym = get_sym();
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = check_attributes(sym);
				if (sym == -1) {
					return -1;
				}
			}
			sym = check_struct_declarator(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else {
		return -1;
	}
	return sym;
}

static int check_struct_declarator(int sym) {
	if (sym == YY__STAR || sym == YY_ID || sym == YY__LPAREN) {
		sym = check_declarator(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym == YY__COLON) {
			sym = get_sym();
			sym = check_constant_expression(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = check_attributes(sym);
				if (sym == -1) {
					return -1;
				}
			}
		} else if (YY_IN_SET(sym, (YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY__COMMA,YY__SEMICOLON,YY__RBRACE), "\006\000\000\000\000\100\300\001\000\000\000\000")) {
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = check_attributes(sym);
				if (sym == -1) {
					return -1;
				}
			}
		} else {
			return -1;
		}
	} else if (sym == YY__COLON) {
		sym = get_sym();
		sym = check_constant_expression(sym);
		if (sym == -1) {
			return -1;
		}
	} else {
		return -1;
	}
	return sym;
}

static int check_enum_specifier(int sym) {
	if (sym != YY_ENUM) {
		return -1;
	}
	sym = get_sym();
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = check_attributes(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if (sym == YY_ID) {
		sym = check_ID(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym == YY__LBRACE) {
			sym = get_sym();
			sym = check_enumerator_list(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym != YY__RBRACE) {
				return -1;
			}
			sym = get_sym();
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = check_attributes(sym);
				if (sym == -1) {
					return -1;
				}
			}
		} else if (YY_IN_SET(sym, (YY_TYPEDEF,YY_EXTERN,YY_STATIC,YY_AUTO,YY_REGISTER,YY_INLINE,YY___INLINE,YY___INLINE__,YY__NORETURN,YY___CDECL,YY___STDCALL,YY___FASTCALL,YY___THISCALL,YY__ALIGNAS,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY__STAR,YY__LPAREN,YY__SEMICOLON,YY__COLON,YY__LBRACK,YY__RBRACE,YY__COMMA,YY__RPAREN,YY_EOF), "\377\377\377\377\377\337\315\001\000\000\040\000")) {
		} else {
			return -1;
		}
	} else if (sym == YY__LBRACE) {
		sym = get_sym();
		sym = check_enumerator_list(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym != YY__RBRACE) {
			return -1;
		}
		sym = get_sym();
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = check_attributes(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else {
		return -1;
	}
	return sym;
}

static int check_enumerator_list(int sym) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt243;
	sym = check_enumerator(sym);
	if (sym == -1) {
		return -1;
	}
	while (1) {
		save_pos  = yy_pos;
		save_text = yy_text;
		save_line = yy_line;
		alt243 = -2;
		sym2 = sym;
		if (sym2 == YY__COMMA) {
			sym2 = get_sym();
			goto _yy_state_243_1;
		} else if (sym2 == YY__RBRACE) {
			alt243 = -1;
			goto _yy_state_243;
		} else {
			return -1;
		}
_yy_state_243_1:
		if (sym2 == YY_ID) {
			alt243 = 244;
			goto _yy_state_243;
		} else if (sym2 == YY__RBRACE) {
			alt243 = 246;
			goto _yy_state_243;
		} else {
			return -1;
		}
_yy_state_243:
		yy_pos  = save_pos;
		yy_text = save_text;
		yy_line = save_line;
		if (alt243 != 244) {
			break;
		}
		sym = get_sym();
		sym = check_enumerator(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if (alt243 == 246) {
		sym = get_sym();
	}
	return sym;
}

static int check_enumerator(int sym) {
	sym = check_ID(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym == YY__EQUAL) {
		sym = get_sym();
		sym = check_constant_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_declarator(int sym) {
	if (sym == YY__STAR) {
		sym = check_pointer(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if (sym == YY_ID) {
		sym = check_ID(sym);
		if (sym == -1) {
			return -1;
		}
	} else if (sym == YY__LPAREN) {
		sym = get_sym();
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = check_attributes(sym);
			if (sym == -1) {
				return -1;
			}
		}
		sym = check_declarator(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym != YY__RPAREN) {
			return -1;
		}
		sym = get_sym();
	} else {
		return -1;
	}
	if (sym == YY__LBRACK || sym == YY__LPAREN) {
		sym = check_array_or_function_declarators(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_abstract_declarator(int sym) {
	if (sym == YY__STAR) {
		sym = check_pointer(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if ((sym == YY__LPAREN) && synpred_2(sym)) {
		sym = check_nested_abstract_declarator(sym);
		if (sym == -1) {
			return -1;
		}
	} else if (sym == YY_ID) {
		sym = check_ID(sym);
		if (sym == -1) {
			return -1;
		}
	} else if (YY_IN_SET(sym, (YY__LBRACK,YY__LPAREN,YY__COMMA,YY__RPAREN,YY_EOF), "\003\000\006\000\000\000\010\000\000\000\000\000")) {
	} else {
		return -1;
	}
	if (sym == YY__LBRACK || sym == YY__LPAREN) {
		sym = check_array_or_function_declarators(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_nested_abstract_declarator(int sym) {
	if (sym != YY__LPAREN) {
		return -1;
	}
	sym = get_sym();
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = check_attributes(sym);
		if (sym == -1) {
			return -1;
		}
	}
	if (sym == YY__STAR) {
		sym = check_pointer(sym);
		if (sym == -1) {
			return -1;
		}
		if ((sym == YY__LPAREN) && synpred_3(sym)) {
			sym = check_nested_abstract_declarator(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY_ID) {
			sym = check_ID(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__LBRACK || sym == YY__LPAREN || sym == YY__RPAREN) {
		} else {
			return -1;
		}
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = check_array_or_function_declarators(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else if (sym == YY__LPAREN || sym == YY_ID || sym == YY__LBRACK) {
		if ((sym == YY__LPAREN) && synpred_4(sym)) {
			sym = check_nested_abstract_declarator(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym == YY__LBRACK || sym == YY__LPAREN) {
				sym = check_array_or_function_declarators(sym);
				if (sym == -1) {
					return -1;
				}
			}
		} else if (sym == YY_ID) {
			sym = check_ID(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym == YY__LBRACK || sym == YY__LPAREN) {
				sym = check_array_or_function_declarators(sym);
				if (sym == -1) {
					return -1;
				}
			}
		} else if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = check_array_or_function_declarators(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	} else {
		return -1;
	}
	if (sym != YY__RPAREN) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_pointer(int sym) {
	if (sym != YY__STAR) {
		return -1;
	}
	do {
		sym = get_sym();
		if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
			sym = check_type_qualifier_list(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} while (sym == YY__STAR);
	return sym;
}

static int check_array_or_function_declarators(int sym) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt114;
	int alt110;
	int alt124;
	if (sym == YY__LBRACK) {
		sym = get_sym();
		save_pos  = yy_pos;
		save_text = yy_text;
		save_line = yy_line;
		alt110 = -2;
		sym2 = sym;
		if (sym2 == YY_STATIC) {
			alt110 = 111;
			goto _yy_state_110;
		} else if (YY_IN_SET(sym2, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
			alt110 = 114;
			goto _yy_state_110;
		} else if (sym2 == YY__STAR) {
			sym2 = get_sym();
			goto _yy_state_110_15;
		} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER), "\000\000\002\000\000\000\000\200\000\343\377\017")) {
			alt110 = 120;
			goto _yy_state_110;
		} else if (sym2 == YY__RBRACK) {
			alt110 = 121;
			goto _yy_state_110;
		} else {
			return -1;
		}
_yy_state_110_15:
		if (sym2 == YY__RBRACK) {
			alt110 = 119;
			goto _yy_state_110;
		} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__STAR), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
			alt110 = 120;
			goto _yy_state_110;
		} else {
			return -1;
		}
_yy_state_110:
		yy_pos  = save_pos;
		yy_text = save_text;
		yy_line = save_line;
		if (alt110 == 111) {
			sym = get_sym();
			if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
				sym = check_type_qualifier_list(sym);
				if (sym == -1) {
					return -1;
				}
			}
			sym = check_assignment_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (alt110 == 114) {
			sym = check_type_qualifier_list(sym);
			if (sym == -1) {
				return -1;
			}
			save_pos  = yy_pos;
			save_text = yy_text;
			save_line = yy_line;
			alt114 = -2;
			sym2 = sym;
			if (sym2 == YY_STATIC) {
				alt114 = 115;
				goto _yy_state_114;
			} else if (sym2 == YY__STAR) {
				sym2 = get_sym();
				goto _yy_state_114_2;
			} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER), "\000\000\002\000\000\000\000\200\000\343\377\017")) {
				alt114 = 118;
				goto _yy_state_114;
			} else if (sym2 == YY__RBRACK) {
				alt114 = 121;
				goto _yy_state_114;
			} else {
				return -1;
			}
_yy_state_114_2:
			if (sym2 == YY__RBRACK) {
				alt114 = 117;
				goto _yy_state_114;
			} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__STAR), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				alt114 = 118;
				goto _yy_state_114;
			} else {
				return -1;
			}
_yy_state_114:
			yy_pos  = save_pos;
			yy_text = save_text;
			yy_line = save_line;
			if (alt114 == 115) {
				sym = get_sym();
				sym = check_assignment_expression(sym);
				if (sym == -1) {
					return -1;
				}
			} else if (alt114 == 121) {
			} else if (alt114 == 117) {
				sym = get_sym();
			} else if (alt114 == 118) {
				sym = check_assignment_expression(sym);
				if (sym == -1) {
					return -1;
				}
			} else {
				return -1;
			}
		} else if (alt110 == 121 || alt110 == 119 || alt110 == 120) {
			if (alt110 == 121) {
			} else if (alt110 == 119) {
				sym = get_sym();
			} else if (alt110 == 120) {
				sym = check_assignment_expression(sym);
				if (sym == -1) {
					return -1;
				}
			} else {
				return -1;
			}
		} else {
			return -1;
		}
		if (sym != YY__RBRACK) {
			return -1;
		}
		sym = get_sym();
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = check_array_or_function_declarators(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else if (sym == YY__LPAREN) {
		sym = get_sym();
		if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY__POINT_POINT_POINT), "\000\000\370\377\377\037\341\001\000\000\040\000")) {
			if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
				sym = check_parameter_declaration(sym);
				if (sym == -1) {
					return -1;
				}
				while (1) {
					save_pos  = yy_pos;
					save_text = yy_text;
					save_line = yy_line;
					alt124 = -2;
					sym2 = sym;
					if (sym2 == YY__COMMA) {
						sym2 = get_sym();
						goto _yy_state_124_1;
					} else if (sym2 == YY__RPAREN) {
						alt124 = 130;
						goto _yy_state_124;
					} else {
						return -1;
					}
_yy_state_124_1:
					if (YY_IN_SET(sym2, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
						alt124 = 125;
						goto _yy_state_124;
					} else if (sym2 == YY__POINT_POINT_POINT) {
						alt124 = 127;
						goto _yy_state_124;
					} else {
						return -1;
					}
_yy_state_124:
					yy_pos  = save_pos;
					yy_text = save_text;
					yy_line = save_line;
					if (alt124 != 125) {
						break;
					}
					sym = get_sym();
					sym = check_parameter_declaration(sym);
					if (sym == -1) {
						return -1;
					}
				}
				if (alt124 == 127) {
					sym = get_sym();
					if (sym != YY__POINT_POINT_POINT) {
						return -1;
					}
					sym = get_sym();
				}
			} else if (sym == YY__POINT_POINT_POINT) {
				sym = get_sym();
			} else {
				return -1;
			}
		}
		if (sym != YY__RPAREN) {
			return -1;
		}
		sym = get_sym();
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = check_array_or_function_declarators(sym);
			if (sym == -1) {
				return -1;
			}
		}
	} else {
		return -1;
	}
	return sym;
}

static int check_parameter_declaration(int sym) {
	sym = check_specifier_qualifier_list(sym);
	if (sym == -1) {
		return -1;
	}
	sym = check_abstract_declarator(sym);
	if (sym == -1) {
		return -1;
	}
	return sym;
}

static int check_type_name(int sym) {
	sym = check_specifier_qualifier_list(sym);
	if (sym == -1) {
		return -1;
	}
	sym = check_abstract_declarator(sym);
	if (sym == -1) {
		return -1;
	}
	return sym;
}

static int check_attributes(int sym) {
	do {
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__) {
			if (sym == YY___ATTRIBUTE) {
				sym = get_sym();
			} else if (sym == YY___ATTRIBUTE__) {
				sym = get_sym();
			} else {
				return -1;
			}
			if (sym != YY__LPAREN) {
				return -1;
			}
			sym = get_sym();
			if (sym != YY__LPAREN) {
				return -1;
			}
			sym = get_sym();
			sym = check_attrib(sym);
			if (sym == -1) {
				return -1;
			}
			while (sym == YY__COMMA) {
				sym = get_sym();
				sym = check_attrib(sym);
				if (sym == -1) {
					return -1;
				}
			}
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
		} else if (sym == YY___DECLSPEC) {
			sym = get_sym();
			if (sym != YY__LPAREN) {
				return -1;
			}
			sym = get_sym();
			do {
				sym = check_ID(sym);
				if (sym == -1) {
					return -1;
				}
				if (sym == YY__LPAREN) {
					sym = get_sym();
					sym = check_assignment_expression(sym);
					if (sym == -1) {
						return -1;
					}
					if (sym != YY__RPAREN) {
						return -1;
					}
					sym = get_sym();
				}
			} while (sym == YY_ID);
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
		} else {
			return -1;
		}
	} while (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC);
	return sym;
}

static int check_attrib(int sym) {
	if (sym == YY_ID) {
		sym = check_ID(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym == YY__COMMA || sym == YY__RPAREN) {
		} else if (sym == YY__LPAREN) {
			sym = get_sym();
			sym = check_assignment_expression(sym);
			if (sym == -1) {
				return -1;
			}
			while (sym == YY__COMMA) {
				sym = get_sym();
				sym = check_assignment_expression(sym);
				if (sym == -1) {
					return -1;
				}
			}
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_expr_list(int sym) {
	sym = check_assignment_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__COMMA) {
		sym = get_sym();
		sym = check_assignment_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_expression(int sym) {
	sym = check_assignment_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__COMMA) {
		sym = get_sym();
		sym = check_assignment_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_assignment_expression(int sym) {
	sym = check_conditional_expression(sym);
	if (sym == -1) {
		return -1;
	}
	return sym;
}

static int check_constant_expression(int sym) {
	sym = check_conditional_expression(sym);
	if (sym == -1) {
		return -1;
	}
	return sym;
}

static int check_conditional_expression(int sym) {
	sym = check_logical_or_expression(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym == YY__QUERY) {
		sym = get_sym();
		sym = check_expression(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym != YY__COLON) {
			return -1;
		}
		sym = get_sym();
		sym = check_conditional_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_logical_or_expression(int sym) {
	sym = check_logical_and_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__BAR_BAR) {
		sym = get_sym();
		sym = check_logical_and_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_logical_and_expression(int sym) {
	sym = check_inclusive_or_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__AND_AND) {
		sym = get_sym();
		sym = check_inclusive_or_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_inclusive_or_expression(int sym) {
	sym = check_exclusive_or_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__BAR) {
		sym = get_sym();
		sym = check_exclusive_or_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_exclusive_or_expression(int sym) {
	sym = check_and_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__UPARROW) {
		sym = get_sym();
		sym = check_and_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_and_expression(int sym) {
	sym = check_equality_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__AND) {
		sym = get_sym();
		sym = check_equality_expression(sym);
		if (sym == -1) {
			return -1;
		}
	}
	return sym;
}

static int check_equality_expression(int sym) {
	sym = check_relational_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__EQUAL_EQUAL || sym == YY__BANG_EQUAL) {
		if (sym == YY__EQUAL_EQUAL) {
			sym = get_sym();
			sym = check_relational_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__BANG_EQUAL) {
			sym = get_sym();
			sym = check_relational_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_relational_expression(int sym) {
	sym = check_shift_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__LESS || sym == YY__GREATER || sym == YY__LESS_EQUAL || sym == YY__GREATER_EQUAL) {
		if (sym == YY__LESS) {
			sym = get_sym();
			sym = check_shift_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__GREATER) {
			sym = get_sym();
			sym = check_shift_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__LESS_EQUAL) {
			sym = get_sym();
			sym = check_shift_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__GREATER_EQUAL) {
			sym = get_sym();
			sym = check_shift_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_shift_expression(int sym) {
	sym = check_additive_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__LESS_LESS || sym == YY__GREATER_GREATER) {
		if (sym == YY__LESS_LESS) {
			sym = get_sym();
			sym = check_additive_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__GREATER_GREATER) {
			sym = get_sym();
			sym = check_additive_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_additive_expression(int sym) {
	sym = check_multiplicative_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__PLUS || sym == YY__MINUS) {
		if (sym == YY__PLUS) {
			sym = get_sym();
			sym = check_multiplicative_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__MINUS) {
			sym = get_sym();
			sym = check_multiplicative_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_multiplicative_expression(int sym) {
	sym = check_cast_expression(sym);
	if (sym == -1) {
		return -1;
	}
	while (sym == YY__STAR || sym == YY__SLASH || sym == YY__PERCENT) {
		if (sym == YY__STAR) {
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__SLASH) {
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else if (sym == YY__PERCENT) {
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
		} else {
			return -1;
		}
	}
	return sym;
}

static int check_cast_expression(int sym) {
	if ((sym == YY__LPAREN) && synpred_5(sym)) {
		sym = get_sym();
		sym = check_type_name(sym);
		if (sym == -1) {
			return -1;
		}
		if (sym != YY__RPAREN) {
			return -1;
		}
		sym = get_sym();
	}
	sym = check_unary_expression(sym);
	if (sym == -1) {
		return -1;
	}
	return sym;
}

static int check_unary_expression(int sym) {
	switch (sym) {
		case YY_ID:
			sym = check_ID(sym);
			if (sym == -1) {
				return -1;
			}
			while (YY_IN_SET(sym, (YY__LBRACK,YY__LPAREN,YY__POINT,YY__MINUS_GREATER,YY__PLUS_PLUS,YY__MINUS_MINUS), "\000\000\002\000\000\000\010\002\000\160\000\000")) {
				switch (sym) {
					case YY__LBRACK:
						sym = get_sym();
						sym = check_expr_list(sym);
						if (sym == -1) {
							return -1;
						}
						if (sym != YY__RBRACK) {
							return -1;
						}
						sym = get_sym();
						break;
					case YY__LPAREN:
						sym = get_sym();
						if (YY_IN_SET(sym, (YY__LPAREN,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
							sym = check_expr_list(sym);
							if (sym == -1) {
								return -1;
							}
						}
						if (sym != YY__RPAREN) {
							return -1;
						}
						sym = get_sym();
						break;
					case YY__POINT:
						sym = get_sym();
						sym = check_ID(sym);
						if (sym == -1) {
							return -1;
						}
						break;
					case YY__MINUS_GREATER:
						sym = get_sym();
						sym = check_ID(sym);
						if (sym == -1) {
							return -1;
						}
						break;
					case YY__PLUS_PLUS:
						sym = get_sym();
						break;
					case YY__MINUS_MINUS:
						sym = get_sym();
						break;
					default:
						return -1;
				}
			}
			break;
		case YY_OCTNUMBER:
			sym = check_OCTNUMBER(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_DECNUMBER:
			sym = check_DECNUMBER(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_HEXNUMBER:
			sym = check_HEXNUMBER(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_FLOATNUMBER:
			sym = check_FLOATNUMBER(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_STRING:
			sym = check_STRING(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_CHARACTER:
			sym = check_CHARACTER(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__LPAREN:
			sym = get_sym();
			sym = check_expression(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
			break;
		case YY__PLUS_PLUS:
			sym = get_sym();
			sym = check_unary_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__MINUS_MINUS:
			sym = get_sym();
			sym = check_unary_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__AND:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__STAR:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__PLUS:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__MINUS:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__TILDE:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY__BANG:
			sym = get_sym();
			sym = check_cast_expression(sym);
			if (sym == -1) {
				return -1;
			}
			break;
		case YY_SIZEOF:
			sym = get_sym();
			if ((sym == YY__LPAREN) && synpred_6(sym)) {
				sym = get_sym();
				sym = check_type_name(sym);
				if (sym == -1) {
					return -1;
				}
				if (sym != YY__RPAREN) {
					return -1;
				}
				sym = get_sym();
			} else if (YY_IN_SET(sym, (YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				sym = check_unary_expression(sym);
				if (sym == -1) {
					return -1;
				}
			} else {
				return -1;
			}
			break;
		case YY__ALIGNOF:
			sym = get_sym();
			if (sym != YY__LPAREN) {
				return -1;
			}
			sym = get_sym();
			sym = check_type_name(sym);
			if (sym == -1) {
				return -1;
			}
			if (sym != YY__RPAREN) {
				return -1;
			}
			sym = get_sym();
			break;
		case YY___ALIGNOF:
		case YY___ALIGNOF__:
			if (sym == YY___ALIGNOF) {
				sym = get_sym();
			} else if (sym == YY___ALIGNOF__) {
				sym = get_sym();
			} else {
				return -1;
			}
			if ((sym == YY__LPAREN) && synpred_7(sym)) {
				sym = get_sym();
				sym = check_type_name(sym);
				if (sym == -1) {
					return -1;
				}
				if (sym != YY__RPAREN) {
					return -1;
				}
				sym = get_sym();
			} else if (YY_IN_SET(sym, (YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				sym = check_unary_expression(sym);
				if (sym == -1) {
					return -1;
				}
			} else {
				return -1;
			}
			break;
		default:
			return -1;
	}
	return sym;
}

static int check_ID(int sym) {
	if (sym != YY_ID) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_OCTNUMBER(int sym) {
	if (sym != YY_OCTNUMBER) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_DECNUMBER(int sym) {
	if (sym != YY_DECNUMBER) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_HEXNUMBER(int sym) {
	if (sym != YY_HEXNUMBER) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_FLOATNUMBER(int sym) {
	if (sym != YY_FLOATNUMBER) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_STRING(int sym) {
	if (sym != YY_STRING) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int check_CHARACTER(int sym) {
	if (sym != YY_CHARACTER) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int synpred_1(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = check_type_name(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int synpred_2(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = check_nested_abstract_declarator(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int synpred_3(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = check_nested_abstract_declarator(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int synpred_4(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = check_nested_abstract_declarator(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int _synpred_5(int sym) {
	if (sym != YY__LPAREN) {
		return -1;
	}
	sym = get_sym();
	sym = check_type_name(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym != YY__RPAREN) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int synpred_5(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = _synpred_5(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int _synpred_6(int sym) {
	if (sym != YY__LPAREN) {
		return -1;
	}
	sym = get_sym();
	sym = check_type_name(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym != YY__RPAREN) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int synpred_6(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = _synpred_6(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int _synpred_7(int sym) {
	if (sym != YY__LPAREN) {
		return -1;
	}
	sym = get_sym();
	sym = check_type_name(sym);
	if (sym == -1) {
		return -1;
	}
	if (sym != YY__RPAREN) {
		return -1;
	}
	sym = get_sym();
	return sym;
}

static int synpred_7(int sym) {
	int ret;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;

	save_pos  = yy_pos;
	save_text = yy_text;
	save_line = yy_line;
	ret = _synpred_7(sym) != -1;
	yy_pos  = save_pos;
	yy_text = save_text;
	yy_line = save_line;
	return ret;
}

static int parse_declarations(int sym) {
	while (YY_IN_SET(sym, (YY_TYPEDEF,YY_EXTERN,YY_STATIC,YY_AUTO,YY_REGISTER,YY_INLINE,YY___INLINE,YY___INLINE__,YY__NORETURN,YY___CDECL,YY___STDCALL,YY___FASTCALL,YY___THISCALL,YY__ALIGNAS,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID), "\370\377\371\377\377\037\301\001\000\000\040\000")) {
		zend_ffi_dcl common_dcl = ZEND_FFI_ATTR_INIT;
		sym = parse_declaration_specifiers(sym, &common_dcl);
		if (sym == YY__STAR || sym == YY_ID || sym == YY__LPAREN) {
			const char *name;
			size_t name_len;
			zend_ffi_dcl dcl;
			dcl = common_dcl;
			sym = parse_declarator(sym, &dcl, &name, &name_len);
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = parse_attributes(sym, &dcl);
			}
			if (sym == YY__EQUAL) {
				sym = parse_initializer(sym);
			}
			zend_ffi_declare(name, name_len, &dcl);
			while (sym == YY__COMMA) {
				sym = get_sym();
				dcl = common_dcl;
				sym = parse_declarator(sym, &dcl, &name, &name_len);
				if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
					sym = parse_attributes(sym, &dcl);
				}
				if (sym == YY__EQUAL) {
					sym = parse_initializer(sym);
				}
				zend_ffi_declare(name, name_len, &dcl);
			}
		}
		if (sym != YY__SEMICOLON) {
			yy_error_sym("';' expected, got", sym);
		}
		sym = get_sym();
	}
	return sym;
}

static int parse_declaration_specifiers(int sym, zend_ffi_dcl *dcl) {
	do {
		switch (sym) {
			case YY_TYPEDEF:
				if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_TYPEDEF;
				break;
			case YY_EXTERN:
				if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_EXTERN;
				break;
			case YY_STATIC:
				if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_STATIC;
				break;
			case YY_AUTO:
				if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_AUTO;
				break;
			case YY_REGISTER:
				if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_REGISTER;
				break;
			case YY_INLINE:
			case YY___INLINE:
			case YY___INLINE__:
				if (sym == YY_INLINE) {
					sym = get_sym();
				} else if (sym == YY___INLINE) {
					sym = get_sym();
				} else if (sym == YY___INLINE__) {
					sym = get_sym();
				} else {
					yy_error_sym("unexpected", sym);
				}
				dcl->flags |= ZEND_FFI_DCL_INLINE;
				break;
			case YY__NORETURN:
				sym = get_sym();
				dcl->flags |= ZEND_FFI_DCL_NO_RETURN;
				break;
			case YY___CDECL:
				sym = get_sym();
				zend_ffi_set_abi(dcl, ZEND_FFI_ABI_CDECL);
				break;
			case YY___STDCALL:
				sym = get_sym();
				zend_ffi_set_abi(dcl, ZEND_FFI_ABI_STDCALL);
				break;
			case YY___FASTCALL:
				sym = get_sym();
				zend_ffi_set_abi(dcl, ZEND_FFI_ABI_FASTCALL);
				break;
			case YY___THISCALL:
				sym = get_sym();
				zend_ffi_set_abi(dcl, ZEND_FFI_ABI_THISCALL);
				break;
			case YY__ALIGNAS:
				sym = get_sym();
				if (sym != YY__LPAREN) {
					yy_error_sym("'(' expected, got", sym);
				}
				sym = get_sym();
				if ((YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) && synpred_1(sym)) {
					zend_ffi_dcl align_dcl = ZEND_FFI_ATTR_INIT;
					sym = parse_type_name(sym, &align_dcl);
					zend_ffi_align_as_type(dcl, &align_dcl);
				} else if (YY_IN_SET(sym, (YY__LPAREN,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
					zend_ffi_val align_val;
					sym = parse_constant_expression(sym, &align_val);
					zend_ffi_align_as_val(dcl, &align_val);
				} else {
					yy_error_sym("unexpected", sym);
				}
				if (sym != YY__RPAREN) {
					yy_error_sym("')' expected, got", sym);
				}
				sym = get_sym();
				break;
			case YY___ATTRIBUTE:
			case YY___ATTRIBUTE__:
			case YY___DECLSPEC:
				sym = parse_attributes(sym, dcl);
				break;
			case YY_CONST:
			case YY___CONST:
			case YY___CONST__:
			case YY_RESTRICT:
			case YY___RESTICT:
			case YY___RESTRICT__:
			case YY_VOLATILE:
			case YY___VOLATILE:
			case YY___VOLATILE__:
			case YY__ATOMIC:
				sym = parse_type_qualifier(sym, dcl);
				break;
			case YY_VOID:
			case YY_CHAR:
			case YY_SHORT:
			case YY_INT:
			case YY_LONG:
			case YY_FLOAT:
			case YY_DOUBLE:
			case YY_SIGNED:
			case YY_UNSIGNED:
			case YY__BOOL:
			case YY__COMPLEX:
			case YY_COMPLEX:
			case YY___COMPLEX:
			case YY___COMPLEX__:
			case YY_STRUCT:
			case YY_UNION:
			case YY_ENUM:
			case YY_ID:
				sym = parse_type_specifier(sym, dcl);
				break;
			default:
				yy_error_sym("unexpected", sym);
		}
	} while ((YY_IN_SET(sym, (YY_TYPEDEF,YY_EXTERN,YY_STATIC,YY_AUTO,YY_REGISTER,YY_INLINE,YY___INLINE,YY___INLINE__,YY__NORETURN,YY___CDECL,YY___STDCALL,YY___FASTCALL,YY___THISCALL,YY__ALIGNAS,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID), "\370\377\371\377\377\037\301\001\000\000\040\000")) && (sym != YY_ID || !(dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS)));
	return sym;
}

static int parse_specifier_qualifier_list(int sym, zend_ffi_dcl *dcl) {
	do {
		if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID), "\000\000\000\340\377\037\001\000\000\000\040\000")) {
			sym = parse_type_specifier(sym, dcl);
		} else if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC), "\000\000\370\037\000\000\000\000\000\000\000\000")) {
			sym = parse_type_qualifier(sym, dcl);
		} else if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = parse_attributes(sym, dcl);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} while ((YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) && (sym != YY_ID || zend_ffi_is_typedef_name((const char*)yy_text, yy_pos - yy_text)));
	return sym;
}

static int parse_type_qualifier_list(int sym, zend_ffi_dcl *dcl) {
	do {
		if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC), "\000\000\370\037\000\000\000\000\000\000\000\000")) {
			sym = parse_type_qualifier(sym, dcl);
		} else if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = parse_attributes(sym, dcl);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} while (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000"));
	return sym;
}

static int parse_type_qualifier(int sym, zend_ffi_dcl *dcl) {
	if (sym == YY_CONST || sym == YY___CONST || sym == YY___CONST__) {
		if (sym == YY_CONST) {
			sym = get_sym();
		} else if (sym == YY___CONST) {
			sym = get_sym();
		} else if (sym == YY___CONST__) {
			sym = get_sym();
		} else {
			yy_error_sym("unexpected", sym);
		}
		dcl->flags |= ZEND_FFI_DCL_CONST;
		dcl->attr |= ZEND_FFI_ATTR_CONST;
	} else if (sym == YY_RESTRICT || sym == YY___RESTICT || sym == YY___RESTRICT__) {
		if (sym == YY_RESTRICT) {
			sym = get_sym();
		} else if (sym == YY___RESTICT) {
			sym = get_sym();
		} else if (sym == YY___RESTRICT__) {
			sym = get_sym();
		} else {
			yy_error_sym("unexpected", sym);
		}
		dcl->flags |= ZEND_FFI_DCL_RESTRICT;
	} else if (sym == YY_VOLATILE || sym == YY___VOLATILE || sym == YY___VOLATILE__) {
		if (sym == YY_VOLATILE) {
			sym = get_sym();
		} else if (sym == YY___VOLATILE) {
			sym = get_sym();
		} else if (sym == YY___VOLATILE__) {
			sym = get_sym();
		} else {
			yy_error_sym("unexpected", sym);
		}
		dcl->flags |= ZEND_FFI_DCL_VOLATILE;
	} else if (sym == YY__ATOMIC) {
		sym = get_sym();
		dcl->flags |= ZEND_FFI_DCL_ATOMIC;
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_type_specifier(int sym, zend_ffi_dcl *dcl) {
	const char *name;
	size_t name_len;
	switch (sym) {
		case YY_VOID:
			if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_VOID;
			break;
		case YY_CHAR:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_CHAR;
			break;
		case YY_SHORT:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_SHORT;
			break;
		case YY_INT:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_INT;
			break;
		case YY_LONG:
			if (dcl->flags & ZEND_FFI_DCL_LONG) {
				if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);
				dcl->flags |= ZEND_FFI_DCL_LONG_LONG;
			} else {
				if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);
				dcl->flags |= ZEND_FFI_DCL_LONG;
			}
			sym = get_sym();
			break;
		case YY_FLOAT:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_FLOAT;
			break;
		case YY_DOUBLE:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_DOUBLE;
			break;
		case YY_SIGNED:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_SIGNED;
			break;
		case YY_UNSIGNED:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_UNSIGNED;
			break;
		case YY__BOOL:
			if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);
			sym = get_sym();
			dcl->flags |= ZEND_FFI_DCL_BOOL;
			break;
		case YY__COMPLEX:
		case YY_COMPLEX:
		case YY___COMPLEX:
		case YY___COMPLEX__:
			if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_LONG))) yy_error_sym("Unexpected '%s'", sym);
			if (sym == YY__COMPLEX) {
				sym = get_sym();
			} else if (sym == YY_COMPLEX) {
				sym = get_sym();
			} else if (sym == YY___COMPLEX) {
				sym = get_sym();
			} else if (sym == YY___COMPLEX__) {
				sym = get_sym();
			} else {
				yy_error_sym("unexpected", sym);
			}
			dcl->flags |= ZEND_FFI_DCL_COMPLEX;
			break;
		case YY_STRUCT:
		case YY_UNION:
			if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);
			sym = parse_struct_or_union_specifier(sym, dcl);
			break;
		case YY_ENUM:
			if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);
			sym = parse_enum_specifier(sym, dcl);
			break;
		case YY_ID:
			if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);
			/*redeclaration of '%.*s' ??? */
			sym = parse_ID(sym, &name, &name_len);
			dcl->flags |= ZEND_FFI_DCL_TYPEDEF_NAME;
			zend_ffi_resolve_typedef(name, name_len, dcl);
			break;
		default:
			yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_struct_or_union_specifier(int sym, zend_ffi_dcl *dcl) {
	if (sym == YY_STRUCT) {
		sym = get_sym();
		dcl->flags |= ZEND_FFI_DCL_STRUCT;
	} else if (sym == YY_UNION) {
		sym = get_sym();
		dcl->flags |= ZEND_FFI_DCL_UNION;
	} else {
		yy_error_sym("unexpected", sym);
	}
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = parse_attributes(sym, dcl);
	}
	if (sym == YY_ID) {
		const char *name;
		size_t name_len;
		sym = parse_ID(sym, &name, &name_len);
		zend_ffi_declare_tag(name, name_len, dcl, 1);
		if (sym == YY__LBRACE) {
			sym = parse_struct_contents(sym, dcl);
			zend_ffi_declare_tag(name, name_len, dcl, 0);
		}
	} else if (sym == YY__LBRACE) {
		zend_ffi_make_struct_type(dcl);
		sym = parse_struct_contents(sym, dcl);
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_struct_contents(int sym, zend_ffi_dcl *dcl) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt2;
	if (sym != YY__LBRACE) {
		yy_error_sym("'{' expected, got", sym);
	}
	sym = get_sym();
	if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
		sym = parse_struct_declaration(sym, dcl);
		while (1) {
			save_pos  = yy_pos;
			save_text = yy_text;
			save_line = yy_line;
			alt2 = -2;
			sym2 = sym;
			if (sym2 == YY__SEMICOLON) {
				sym2 = get_sym();
				goto _yy_state_2_1;
			} else if (sym2 == YY__RBRACE) {
				alt2 = 6;
				goto _yy_state_2;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_2_1:
			if (YY_IN_SET(sym2, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
				alt2 = 3;
				goto _yy_state_2;
			} else if (sym2 == YY__RBRACE) {
				alt2 = 5;
				goto _yy_state_2;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_2:
			yy_pos  = save_pos;
			yy_text = save_text;
			yy_line = save_line;
			if (alt2 != 3) {
				break;
			}
			sym = get_sym();
			sym = parse_struct_declaration(sym, dcl);
		}
		if (alt2 == 5) {
			sym = get_sym();
		}
	}
	if (sym != YY__RBRACE) {
		yy_error_sym("'}' expected, got", sym);
	}
	sym = get_sym();
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = parse_attributes(sym, dcl);
	}
	zend_ffi_adjust_struct_size(dcl);
	return sym;
}

static int parse_struct_declaration(int sym, zend_ffi_dcl *struct_dcl) {
	zend_ffi_dcl common_field_dcl = ZEND_FFI_ATTR_INIT;
	sym = parse_specifier_qualifier_list(sym, &common_field_dcl);
	if (sym == YY__SEMICOLON || sym == YY__RBRACE) {
		zend_ffi_add_anonymous_field(struct_dcl, &common_field_dcl);
	} else if (sym == YY__STAR || sym == YY_ID || sym == YY__LPAREN || sym == YY__COLON) {
		sym = parse_struct_declarator(sym, struct_dcl, &common_field_dcl);
		while (sym == YY__COMMA) {
			sym = get_sym();
			zend_ffi_dcl field_dcl = common_field_dcl;
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = parse_attributes(sym, &field_dcl);
			}
			sym = parse_struct_declarator(sym, struct_dcl, &field_dcl);
		}
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_struct_declarator(int sym, zend_ffi_dcl *struct_dcl, zend_ffi_dcl *field_dcl) {
	const char *name = NULL;
	size_t name_len = 0;
	zend_ffi_val bits;
	if (sym == YY__STAR || sym == YY_ID || sym == YY__LPAREN) {
		sym = parse_declarator(sym, field_dcl, &name, &name_len);
		if (sym == YY__COLON) {
			sym = get_sym();
			sym = parse_constant_expression(sym, &bits);
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = parse_attributes(sym, field_dcl);
			}
			zend_ffi_add_bit_field(struct_dcl, name, name_len, field_dcl, &bits);
		} else if (YY_IN_SET(sym, (YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY__COMMA,YY__SEMICOLON,YY__RBRACE), "\006\000\000\000\000\100\300\001\000\000\000\000")) {
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = parse_attributes(sym, field_dcl);
			}
			zend_ffi_add_field(struct_dcl, name, name_len, field_dcl);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} else if (sym == YY__COLON) {
		sym = get_sym();
		sym = parse_constant_expression(sym, &bits);
		zend_ffi_add_bit_field(struct_dcl, NULL, 0, field_dcl, &bits);
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_enum_specifier(int sym, zend_ffi_dcl *dcl) {
	if (sym != YY_ENUM) {
		yy_error_sym("'enum' expected, got", sym);
	}
	sym = get_sym();
	dcl->flags |= ZEND_FFI_DCL_ENUM;
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = parse_attributes(sym, dcl);
	}
	if (sym == YY_ID) {
		const char *name;
		size_t name_len;
		sym = parse_ID(sym, &name, &name_len);
		if (sym == YY__LBRACE) {
			zend_ffi_declare_tag(name, name_len, dcl, 0);
			sym = get_sym();
			sym = parse_enumerator_list(sym, dcl);
			if (sym != YY__RBRACE) {
				yy_error_sym("'}' expected, got", sym);
			}
			sym = get_sym();
			if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
				sym = parse_attributes(sym, dcl);
			}
		} else if (YY_IN_SET(sym, (YY_TYPEDEF,YY_EXTERN,YY_STATIC,YY_AUTO,YY_REGISTER,YY_INLINE,YY___INLINE,YY___INLINE__,YY__NORETURN,YY___CDECL,YY___STDCALL,YY___FASTCALL,YY___THISCALL,YY__ALIGNAS,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY__STAR,YY__LPAREN,YY__SEMICOLON,YY__COLON,YY__LBRACK,YY__RBRACE,YY__COMMA,YY__RPAREN,YY_EOF), "\377\377\377\377\377\337\315\001\000\000\040\000")) {
			zend_ffi_declare_tag(name, name_len, dcl, 1);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} else if (sym == YY__LBRACE) {
		sym = get_sym();
		zend_ffi_make_enum_type(dcl);
		sym = parse_enumerator_list(sym, dcl);
		if (sym != YY__RBRACE) {
			yy_error_sym("'}' expected, got", sym);
		}
		sym = get_sym();
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = parse_attributes(sym, dcl);
		}
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_enumerator_list(int sym, zend_ffi_dcl *enum_dcl) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt243;
	int64_t min = 0, max = 0, last = -1;
	sym = parse_enumerator(sym, enum_dcl, &min, &max, &last);
	while (1) {
		save_pos  = yy_pos;
		save_text = yy_text;
		save_line = yy_line;
		alt243 = -2;
		sym2 = sym;
		if (sym2 == YY__COMMA) {
			sym2 = get_sym();
			goto _yy_state_243_1;
		} else if (sym2 == YY__RBRACE) {
			alt243 = -1;
			goto _yy_state_243;
		} else {
			yy_error_sym("unexpected", sym2);
		}
_yy_state_243_1:
		if (sym2 == YY_ID) {
			alt243 = 244;
			goto _yy_state_243;
		} else if (sym2 == YY__RBRACE) {
			alt243 = 246;
			goto _yy_state_243;
		} else {
			yy_error_sym("unexpected", sym2);
		}
_yy_state_243:
		yy_pos  = save_pos;
		yy_text = save_text;
		yy_line = save_line;
		if (alt243 != 244) {
			break;
		}
		sym = get_sym();
		sym = parse_enumerator(sym, enum_dcl, &min, &max, &last);
	}
	if (alt243 == 246) {
		sym = get_sym();
	}
	return sym;
}

static int parse_enumerator(int sym, zend_ffi_dcl *enum_dcl, int64_t *min, int64_t *max, int64_t *last) {
	const char *name;
	size_t name_len;
	zend_ffi_val val = {.kind = ZEND_FFI_VAL_EMPTY};
	sym = parse_ID(sym, &name, &name_len);
	if (sym == YY__EQUAL) {
		sym = get_sym();
		sym = parse_constant_expression(sym, &val);
	}
	zend_ffi_add_enum_val(enum_dcl, name, name_len, &val, min, max, last);
	return sym;
}

static int parse_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len) {
	zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};
	zend_bool nested = 0;
	if (sym == YY__STAR) {
		sym = parse_pointer(sym, dcl);
	}
	if (sym == YY_ID) {
		sym = parse_ID(sym, name, name_len);
	} else if (sym == YY__LPAREN) {
		sym = get_sym();
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
			sym = parse_attributes(sym, &nested_dcl);
		}
		sym = parse_declarator(sym, &nested_dcl, name, name_len);
		if (sym != YY__RPAREN) {
			yy_error_sym("')' expected, got", sym);
		}
		sym = get_sym();
		nested = 1;
	} else {
		yy_error_sym("unexpected", sym);
	}
	if (sym == YY__LBRACK || sym == YY__LPAREN) {
		sym = parse_array_or_function_declarators(sym, dcl);
	}
	if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);
	return sym;
}

static int parse_abstract_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len) {
	zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};
	zend_bool nested = 0;
	if (sym == YY__STAR) {
		sym = parse_pointer(sym, dcl);
	}
	if ((sym == YY__LPAREN) && synpred_2(sym)) {
		sym = parse_nested_abstract_declarator(sym, &nested_dcl, name, name_len);
		nested = 1;
	} else if (sym == YY_ID) {
		sym = parse_ID(sym, name, name_len);
	} else if (YY_IN_SET(sym, (YY__LBRACK,YY__LPAREN,YY__COMMA,YY__RPAREN,YY_EOF), "\003\000\006\000\000\000\010\000\000\000\000\000")) {
	} else {
		yy_error_sym("unexpected", sym);
	}
	if (sym == YY__LBRACK || sym == YY__LPAREN) {
		sym = parse_array_or_function_declarators(sym, dcl);
	}
	if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);
	return sym;
}

static int parse_nested_abstract_declarator(int sym, zend_ffi_dcl *dcl, const char **name, size_t *name_len) {
	zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};
	zend_bool nested = 0;
	if (sym != YY__LPAREN) {
		yy_error_sym("'(' expected, got", sym);
	}
	sym = get_sym();
	if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC) {
		sym = parse_attributes(sym, &nested_dcl);
	}
	if (sym == YY__STAR) {
		sym = parse_pointer(sym, dcl);
		if ((sym == YY__LPAREN) && synpred_3(sym)) {
			sym = parse_nested_abstract_declarator(sym, &nested_dcl, name, name_len);
			nested = 1;
		} else if (sym == YY_ID) {
			sym = parse_ID(sym, name, name_len);
		} else if (sym == YY__LBRACK || sym == YY__LPAREN || sym == YY__RPAREN) {
		} else {
			yy_error_sym("unexpected", sym);
		}
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = parse_array_or_function_declarators(sym, dcl);
		}
	} else if (sym == YY__LPAREN || sym == YY_ID || sym == YY__LBRACK) {
		if ((sym == YY__LPAREN) && synpred_4(sym)) {
			sym = parse_nested_abstract_declarator(sym, &nested_dcl, name, name_len);
			if (sym == YY__LBRACK || sym == YY__LPAREN) {
				sym = parse_array_or_function_declarators(sym, dcl);
			}
			nested = 1;
		} else if (sym == YY_ID) {
			sym = parse_ID(sym, name, name_len);
			if (sym == YY__LBRACK || sym == YY__LPAREN) {
				sym = parse_array_or_function_declarators(sym, dcl);
			}
		} else if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = parse_array_or_function_declarators(sym, dcl);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} else {
		yy_error_sym("unexpected", sym);
	}
	if (sym != YY__RPAREN) {
		yy_error_sym("')' expected, got", sym);
	}
	sym = get_sym();
	if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);
	return sym;
}

static int parse_pointer(int sym, zend_ffi_dcl *dcl) {
	if (sym != YY__STAR) {
		yy_error_sym("'*' expected, got", sym);
	}
	do {
		sym = get_sym();
		zend_ffi_make_pointer_type(dcl);
		if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
			sym = parse_type_qualifier_list(sym, dcl);
		}
	} while (sym == YY__STAR);
	return sym;
}

static int parse_array_or_function_declarators(int sym, zend_ffi_dcl *dcl) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt114;
	int alt110;
	int alt124;
	zend_ffi_dcl dummy = ZEND_FFI_ATTR_INIT;
	zend_ffi_val len = {.kind = ZEND_FFI_VAL_EMPTY};
	HashTable *args = NULL;
	uint32_t attr = 0;
	if (sym == YY__LBRACK) {
		sym = get_sym();
		save_pos  = yy_pos;
		save_text = yy_text;
		save_line = yy_line;
		alt110 = -2;
		sym2 = sym;
		if (sym2 == YY_STATIC) {
			alt110 = 111;
			goto _yy_state_110;
		} else if (YY_IN_SET(sym2, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
			alt110 = 114;
			goto _yy_state_110;
		} else if (sym2 == YY__STAR) {
			sym2 = get_sym();
			goto _yy_state_110_15;
		} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER), "\000\000\002\000\000\000\000\200\000\343\377\017")) {
			alt110 = 120;
			goto _yy_state_110;
		} else if (sym2 == YY__RBRACK) {
			alt110 = 121;
			goto _yy_state_110;
		} else {
			yy_error_sym("unexpected", sym2);
		}
_yy_state_110_15:
		if (sym2 == YY__RBRACK) {
			alt110 = 119;
			goto _yy_state_110;
		} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__STAR), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
			alt110 = 120;
			goto _yy_state_110;
		} else {
			yy_error_sym("unexpected", sym2);
		}
_yy_state_110:
		yy_pos  = save_pos;
		yy_text = save_text;
		yy_line = save_line;
		if (alt110 == 111) {
			sym = get_sym();
			if (YY_IN_SET(sym, (YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\037\000\000\300\001\000\000\000\000")) {
				sym = parse_type_qualifier_list(sym, &dummy);
			}
			sym = parse_assignment_expression(sym, &len);
		} else if (alt110 == 114) {
			sym = parse_type_qualifier_list(sym, &dummy);
			save_pos  = yy_pos;
			save_text = yy_text;
			save_line = yy_line;
			alt114 = -2;
			sym2 = sym;
			if (sym2 == YY_STATIC) {
				alt114 = 115;
				goto _yy_state_114;
			} else if (sym2 == YY__STAR) {
				sym2 = get_sym();
				goto _yy_state_114_2;
			} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER), "\000\000\002\000\000\000\000\200\000\343\377\017")) {
				alt114 = 118;
				goto _yy_state_114;
			} else if (sym2 == YY__RBRACK) {
				alt114 = 121;
				goto _yy_state_114;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_114_2:
			if (sym2 == YY__RBRACK) {
				alt114 = 117;
				goto _yy_state_114;
			} else if (YY_IN_SET(sym2, (YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__STAR), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				alt114 = 118;
				goto _yy_state_114;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_114:
			yy_pos  = save_pos;
			yy_text = save_text;
			yy_line = save_line;
			if (alt114 == 115) {
				sym = get_sym();
				sym = parse_assignment_expression(sym, &len);
			} else if (alt114 == 121) {
				attr |= ZEND_FFI_ATTR_INCOMPLETE_ARRAY;
			} else if (alt114 == 117) {
				sym = get_sym();
				attr |= ZEND_FFI_ATTR_VLA;
			} else if (alt114 == 118) {
				sym = parse_assignment_expression(sym, &len);
			} else {
				yy_error_sym("unexpected", sym);
			}
		} else if (alt110 == 121 || alt110 == 119 || alt110 == 120) {
			if (alt110 == 121) {
				attr |= ZEND_FFI_ATTR_INCOMPLETE_ARRAY;
			} else if (alt110 == 119) {
				sym = get_sym();
				attr |= ZEND_FFI_ATTR_VLA;
			} else if (alt110 == 120) {
				sym = parse_assignment_expression(sym, &len);
			} else {
				yy_error_sym("unexpected", sym);
			}
		} else {
			yy_error_sym("unexpected", sym);
		}
		if (sym != YY__RBRACK) {
			yy_error_sym("']' expected, got", sym);
		}
		sym = get_sym();
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = parse_array_or_function_declarators(sym, dcl);
		}
		dcl->attr |= attr;
		zend_ffi_make_array_type(dcl, &len);
	} else if (sym == YY__LPAREN) {
		sym = get_sym();
		if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC,YY__POINT_POINT_POINT), "\000\000\370\377\377\037\341\001\000\000\040\000")) {
			if (YY_IN_SET(sym, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
				sym = parse_parameter_declaration(sym, &args);
				while (1) {
					save_pos  = yy_pos;
					save_text = yy_text;
					save_line = yy_line;
					alt124 = -2;
					sym2 = sym;
					if (sym2 == YY__COMMA) {
						sym2 = get_sym();
						goto _yy_state_124_1;
					} else if (sym2 == YY__RPAREN) {
						alt124 = 130;
						goto _yy_state_124;
					} else {
						yy_error_sym("unexpected", sym2);
					}
_yy_state_124_1:
					if (YY_IN_SET(sym2, (YY_VOID,YY_CHAR,YY_SHORT,YY_INT,YY_LONG,YY_FLOAT,YY_DOUBLE,YY_SIGNED,YY_UNSIGNED,YY__BOOL,YY__COMPLEX,YY_COMPLEX,YY___COMPLEX,YY___COMPLEX__,YY_STRUCT,YY_UNION,YY_ENUM,YY_ID,YY_CONST,YY___CONST,YY___CONST__,YY_RESTRICT,YY___RESTICT,YY___RESTRICT__,YY_VOLATILE,YY___VOLATILE,YY___VOLATILE__,YY__ATOMIC,YY___ATTRIBUTE,YY___ATTRIBUTE__,YY___DECLSPEC), "\000\000\370\377\377\037\301\001\000\000\040\000")) {
						alt124 = 125;
						goto _yy_state_124;
					} else if (sym2 == YY__POINT_POINT_POINT) {
						alt124 = 127;
						goto _yy_state_124;
					} else {
						yy_error_sym("unexpected", sym2);
					}
_yy_state_124:
					yy_pos  = save_pos;
					yy_text = save_text;
					yy_line = save_line;
					if (alt124 != 125) {
						break;
					}
					sym = get_sym();
					sym = parse_parameter_declaration(sym, &args);
				}
				if (alt124 == 127) {
					sym = get_sym();
					if (sym != YY__POINT_POINT_POINT) {
						yy_error_sym("'...' expected, got", sym);
					}
					sym = get_sym();
					attr |= ZEND_FFI_ATTR_VARIADIC;
				}
			} else if (sym == YY__POINT_POINT_POINT) {
				sym = get_sym();
				attr |= ZEND_FFI_ATTR_VARIADIC;
			} else {
				yy_error_sym("unexpected", sym);
			}
		}
		if (sym != YY__RPAREN) {
			yy_error_sym("')' expected, got", sym);
		}
		sym = get_sym();
		if (sym == YY__LBRACK || sym == YY__LPAREN) {
			sym = parse_array_or_function_declarators(sym, dcl);
		}
		dcl->attr |= attr;
		zend_ffi_make_func_type(dcl, args);
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_parameter_declaration(int sym, HashTable **args) {
	const char *name = NULL;
	size_t name_len = 0;
	zend_bool old_allow_vla = FFI_G(allow_vla);
	FFI_G(allow_vla) = 1;
	zend_ffi_dcl param_dcl = ZEND_FFI_ATTR_INIT;
	sym = parse_specifier_qualifier_list(sym, &param_dcl);
	sym = parse_abstract_declarator(sym, &param_dcl, &name, &name_len);
	zend_ffi_add_arg(args, name, name_len, &param_dcl);
	FFI_G(allow_vla) = old_allow_vla;
	return sym;
}

static int parse_type_name(int sym, zend_ffi_dcl *dcl) {
	const char *name = NULL;
	size_t name_len = 0;
	sym = parse_specifier_qualifier_list(sym, dcl);
	sym = parse_abstract_declarator(sym, dcl, &name, &name_len);
	return sym;
}

static int parse_attributes(int sym, zend_ffi_dcl *dcl) {
	const char *name;
	size_t name_len;
	zend_ffi_val val;
	do {
		if (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__) {
			if (sym == YY___ATTRIBUTE) {
				sym = get_sym();
			} else if (sym == YY___ATTRIBUTE__) {
				sym = get_sym();
			} else {
				yy_error_sym("unexpected", sym);
			}
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			sym = parse_attrib(sym, dcl);
			while (sym == YY__COMMA) {
				sym = get_sym();
				sym = parse_attrib(sym, dcl);
			}
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
		} else if (sym == YY___DECLSPEC) {
			sym = get_sym();
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			do {
				sym = parse_ID(sym, &name, &name_len);
				if (sym == YY__LPAREN) {
					sym = get_sym();
					sym = parse_assignment_expression(sym, &val);
					zend_ffi_add_msvc_attribute_value(dcl, name, name_len, &val);
					if (sym != YY__RPAREN) {
						yy_error_sym("')' expected, got", sym);
					}
					sym = get_sym();
				}
			} while (sym == YY_ID);
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
		} else {
			yy_error_sym("unexpected", sym);
		}
	} while (sym == YY___ATTRIBUTE || sym == YY___ATTRIBUTE__ || sym == YY___DECLSPEC);
	return sym;
}

static int parse_attrib(int sym, zend_ffi_dcl *dcl) {
	const char *name;
	size_t name_len;
	int n;
	zend_ffi_val val;
	if (sym == YY_ID) {
		sym = parse_ID(sym, &name, &name_len);
		if (sym == YY__COMMA || sym == YY__RPAREN) {
			zend_ffi_add_attribute(dcl, name, name_len);
		} else if (sym == YY__LPAREN) {
			sym = get_sym();
			sym = parse_assignment_expression(sym, &val);
			zend_ffi_add_attribute_value(dcl, name, name_len, 0, &val);
			n = 0;
			while (sym == YY__COMMA) {
				sym = get_sym();
				sym = parse_assignment_expression(sym, &val);
				zend_ffi_add_attribute_value(dcl, name, name_len, ++n, &val);
			}
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_initializer(int sym) {
	int   sym2;
	const unsigned char *save_pos;
	const unsigned char *save_text;
	int   save_line;
	int alt336;
	zend_ffi_val dummy;
	if (sym != YY__EQUAL) {
		yy_error_sym("'=' expected, got", sym);
	}
	sym = get_sym();
	if (YY_IN_SET(sym, (YY__LPAREN,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
		sym = parse_assignment_expression(sym, &dummy);
	} else if (sym == YY__LBRACE) {
		sym = get_sym();
		if (sym == YY__LBRACK || sym == YY__POINT) {
			sym = parse_designation(sym);
		}
		sym = parse_initializer(sym);
		while (1) {
			save_pos  = yy_pos;
			save_text = yy_text;
			save_line = yy_line;
			alt336 = -2;
			sym2 = sym;
			if (sym2 == YY__COMMA) {
				sym2 = get_sym();
				goto _yy_state_336_1;
			} else if (sym2 == YY__RBRACE) {
				alt336 = 341;
				goto _yy_state_336;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_336_1:
			if (sym2 == YY__LBRACK || sym2 == YY__POINT || sym2 == YY__EQUAL) {
				alt336 = 337;
				goto _yy_state_336;
			} else if (sym2 == YY__RBRACE) {
				alt336 = 340;
				goto _yy_state_336;
			} else {
				yy_error_sym("unexpected", sym2);
			}
_yy_state_336:
			yy_pos  = save_pos;
			yy_text = save_text;
			yy_line = save_line;
			if (alt336 != 337) {
				break;
			}
			sym = get_sym();
			if (sym == YY__LBRACK || sym == YY__POINT) {
				sym = parse_designation(sym);
			}
			sym = parse_initializer(sym);
		}
		if (alt336 == 340) {
			sym = get_sym();
		}
		if (sym != YY__RBRACE) {
			yy_error_sym("'}' expected, got", sym);
		}
		sym = get_sym();
	} else {
		yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_designation(int sym) {
	const char *name;
	size_t name_len;
	zend_ffi_val dummy;
	do {
		if (sym == YY__LBRACK) {
			sym = get_sym();
			sym = parse_constant_expression(sym, &dummy);
			if (sym != YY__RBRACK) {
				yy_error_sym("']' expected, got", sym);
			}
			sym = get_sym();
		} else if (sym == YY__POINT) {
			sym = get_sym();
			sym = parse_ID(sym, &name, &name_len);
		} else {
			yy_error_sym("unexpected", sym);
		}
	} while (sym == YY__LBRACK || sym == YY__POINT);
	if (sym != YY__EQUAL) {
		yy_error_sym("'=' expected, got", sym);
	}
	sym = get_sym();
	return sym;
}

static int parse_expr_list(int sym) {
	zend_ffi_val dummy;
	sym = parse_assignment_expression(sym, &dummy);
	while (sym == YY__COMMA) {
		sym = get_sym();
		sym = parse_assignment_expression(sym, &dummy);
	}
	return sym;
}

static int parse_expression(int sym, zend_ffi_val *val) {
	sym = parse_assignment_expression(sym, val);
	while (sym == YY__COMMA) {
		sym = get_sym();
		sym = parse_assignment_expression(sym, val);
	}
	return sym;
}

static int parse_assignment_expression(int sym, zend_ffi_val *val) {
	sym = parse_conditional_expression(sym, val);
	return sym;
}

static int parse_constant_expression(int sym, zend_ffi_val *val) {
	sym = parse_conditional_expression(sym, val);
	return sym;
}

static int parse_conditional_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2, op3;
	sym = parse_logical_or_expression(sym, val);
	if (sym == YY__QUERY) {
		sym = get_sym();
		sym = parse_expression(sym, &op2);
		if (sym != YY__COLON) {
			yy_error_sym("':' expected, got", sym);
		}
		sym = get_sym();
		sym = parse_conditional_expression(sym, &op3);
		zend_ffi_expr_conditional(val, &op2, &op3);
	}
	return sym;
}

static int parse_logical_or_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_logical_and_expression(sym, val);
	while (sym == YY__BAR_BAR) {
		sym = get_sym();
		sym = parse_logical_and_expression(sym, &op2);
		zend_ffi_expr_bool_or(val, &op2);
	}
	return sym;
}

static int parse_logical_and_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_inclusive_or_expression(sym, val);
	while (sym == YY__AND_AND) {
		sym = get_sym();
		sym = parse_inclusive_or_expression(sym, &op2);
		zend_ffi_expr_bool_and(val, &op2);
	}
	return sym;
}

static int parse_inclusive_or_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_exclusive_or_expression(sym, val);
	while (sym == YY__BAR) {
		sym = get_sym();
		sym = parse_exclusive_or_expression(sym, &op2);
		zend_ffi_expr_bw_or(val, &op2);
	}
	return sym;
}

static int parse_exclusive_or_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_and_expression(sym, val);
	while (sym == YY__UPARROW) {
		sym = get_sym();
		sym = parse_and_expression(sym, &op2);
		zend_ffi_expr_bw_xor(val, &op2);
	}
	return sym;
}

static int parse_and_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_equality_expression(sym, val);
	while (sym == YY__AND) {
		sym = get_sym();
		sym = parse_equality_expression(sym, &op2);
		zend_ffi_expr_bw_and(val, &op2);
	}
	return sym;
}

static int parse_equality_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_relational_expression(sym, val);
	while (sym == YY__EQUAL_EQUAL || sym == YY__BANG_EQUAL) {
		if (sym == YY__EQUAL_EQUAL) {
			sym = get_sym();
			sym = parse_relational_expression(sym, &op2);
			zend_ffi_expr_is_equal(val, &op2);
		} else if (sym == YY__BANG_EQUAL) {
			sym = get_sym();
			sym = parse_relational_expression(sym, &op2);
			zend_ffi_expr_is_not_equal(val, &op2);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_relational_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_shift_expression(sym, val);
	while (sym == YY__LESS || sym == YY__GREATER || sym == YY__LESS_EQUAL || sym == YY__GREATER_EQUAL) {
		if (sym == YY__LESS) {
			sym = get_sym();
			sym = parse_shift_expression(sym, &op2);
			zend_ffi_expr_is_less(val, &op2);
		} else if (sym == YY__GREATER) {
			sym = get_sym();
			sym = parse_shift_expression(sym, &op2);
			zend_ffi_expr_is_greater(val, &op2);
		} else if (sym == YY__LESS_EQUAL) {
			sym = get_sym();
			sym = parse_shift_expression(sym, &op2);
			zend_ffi_expr_is_less_or_equal(val, &op2);
		} else if (sym == YY__GREATER_EQUAL) {
			sym = get_sym();
			sym = parse_shift_expression(sym, &op2);
			zend_ffi_expr_is_greater_or_equal(val, &op2);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_shift_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_additive_expression(sym, val);
	while (sym == YY__LESS_LESS || sym == YY__GREATER_GREATER) {
		if (sym == YY__LESS_LESS) {
			sym = get_sym();
			sym = parse_additive_expression(sym, &op2);
			zend_ffi_expr_shift_left(val, &op2);
		} else if (sym == YY__GREATER_GREATER) {
			sym = get_sym();
			sym = parse_additive_expression(sym, &op2);
			zend_ffi_expr_shift_right(val, &op2);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_additive_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_multiplicative_expression(sym, val);
	while (sym == YY__PLUS || sym == YY__MINUS) {
		if (sym == YY__PLUS) {
			sym = get_sym();
			sym = parse_multiplicative_expression(sym, &op2);
			zend_ffi_expr_add(val, &op2);
		} else if (sym == YY__MINUS) {
			sym = get_sym();
			sym = parse_multiplicative_expression(sym, &op2);
			zend_ffi_expr_sub(val, &op2);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_multiplicative_expression(int sym, zend_ffi_val *val) {
	zend_ffi_val op2;
	sym = parse_cast_expression(sym, val);
	while (sym == YY__STAR || sym == YY__SLASH || sym == YY__PERCENT) {
		if (sym == YY__STAR) {
			sym = get_sym();
			sym = parse_cast_expression(sym, &op2);
			zend_ffi_expr_mul(val, &op2);
		} else if (sym == YY__SLASH) {
			sym = get_sym();
			sym = parse_cast_expression(sym, &op2);
			zend_ffi_expr_div(val, &op2);
		} else if (sym == YY__PERCENT) {
			sym = get_sym();
			sym = parse_cast_expression(sym, &op2);
			zend_ffi_expr_mod(val, &op2);
		} else {
			yy_error_sym("unexpected", sym);
		}
	}
	return sym;
}

static int parse_cast_expression(int sym, zend_ffi_val *val) {
	int do_cast = 0;
	zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;
	if ((sym == YY__LPAREN) && synpred_5(sym)) {
		sym = get_sym();
		sym = parse_type_name(sym, &dcl);
		if (sym != YY__RPAREN) {
			yy_error_sym("')' expected, got", sym);
		}
		sym = get_sym();
		do_cast = 1;
	}
	sym = parse_unary_expression(sym, val);
	if (do_cast) zend_ffi_expr_cast(val, &dcl);
	return sym;
}

static int parse_unary_expression(int sym, zend_ffi_val *val) {
	const char *name;
	size_t name_len;
	zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;
	switch (sym) {
		case YY_ID:
			sym = parse_ID(sym, &name, &name_len);
			zend_ffi_resolve_const(name, name_len, val);
			while (YY_IN_SET(sym, (YY__LBRACK,YY__LPAREN,YY__POINT,YY__MINUS_GREATER,YY__PLUS_PLUS,YY__MINUS_MINUS), "\000\000\002\000\000\000\010\002\000\160\000\000")) {
				switch (sym) {
					case YY__LBRACK:
						sym = get_sym();
						sym = parse_expr_list(sym);
						if (sym != YY__RBRACK) {
							yy_error_sym("']' expected, got", sym);
						}
						sym = get_sym();
						break;
					case YY__LPAREN:
						sym = get_sym();
						if (YY_IN_SET(sym, (YY__LPAREN,YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
							sym = parse_expr_list(sym);
						}
						if (sym != YY__RPAREN) {
							yy_error_sym("')' expected, got", sym);
						}
						sym = get_sym();
						break;
					case YY__POINT:
						sym = get_sym();
						sym = parse_ID(sym, &name, &name_len);
						break;
					case YY__MINUS_GREATER:
						sym = get_sym();
						sym = parse_ID(sym, &name, &name_len);
						break;
					case YY__PLUS_PLUS:
						sym = get_sym();
						break;
					case YY__MINUS_MINUS:
						sym = get_sym();
						break;
					default:
						yy_error_sym("unexpected", sym);
				}
				zend_ffi_val_error(val);
			}
			break;
		case YY_OCTNUMBER:
			sym = parse_OCTNUMBER(sym, val);
			break;
		case YY_DECNUMBER:
			sym = parse_DECNUMBER(sym, val);
			break;
		case YY_HEXNUMBER:
			sym = parse_HEXNUMBER(sym, val);
			break;
		case YY_FLOATNUMBER:
			sym = parse_FLOATNUMBER(sym, val);
			break;
		case YY_STRING:
			sym = parse_STRING(sym, val);
			break;
		case YY_CHARACTER:
			sym = parse_CHARACTER(sym, val);
			break;
		case YY__LPAREN:
			sym = get_sym();
			sym = parse_expression(sym, val);
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
			break;
		case YY__PLUS_PLUS:
			sym = get_sym();
			sym = parse_unary_expression(sym, val);
			zend_ffi_val_error(val);
			break;
		case YY__MINUS_MINUS:
			sym = get_sym();
			sym = parse_unary_expression(sym, val);
			zend_ffi_val_error(val);
			break;
		case YY__AND:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_val_error(val);
			break;
		case YY__STAR:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_val_error(val);
			break;
		case YY__PLUS:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_expr_plus(val);
			break;
		case YY__MINUS:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_expr_neg(val);
			break;
		case YY__TILDE:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_expr_bw_not(val);
			break;
		case YY__BANG:
			sym = get_sym();
			sym = parse_cast_expression(sym, val);
			zend_ffi_expr_bool_not(val);
			break;
		case YY_SIZEOF:
			sym = get_sym();
			if ((sym == YY__LPAREN) && synpred_6(sym)) {
				sym = get_sym();
				sym = parse_type_name(sym, &dcl);
				if (sym != YY__RPAREN) {
					yy_error_sym("')' expected, got", sym);
				}
				sym = get_sym();
				zend_ffi_expr_sizeof_type(val, &dcl);
			} else if (YY_IN_SET(sym, (YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				sym = parse_unary_expression(sym, val);
				zend_ffi_expr_sizeof_val(val);
			} else {
				yy_error_sym("unexpected", sym);
			}
			break;
		case YY__ALIGNOF:
			sym = get_sym();
			if (sym != YY__LPAREN) {
				yy_error_sym("'(' expected, got", sym);
			}
			sym = get_sym();
			sym = parse_type_name(sym, &dcl);
			if (sym != YY__RPAREN) {
				yy_error_sym("')' expected, got", sym);
			}
			sym = get_sym();
			zend_ffi_expr_alignof_type(val, &dcl);
			break;
		case YY___ALIGNOF:
		case YY___ALIGNOF__:
			if (sym == YY___ALIGNOF) {
				sym = get_sym();
			} else if (sym == YY___ALIGNOF__) {
				sym = get_sym();
			} else {
				yy_error_sym("unexpected", sym);
			}
			if ((sym == YY__LPAREN) && synpred_7(sym)) {
				sym = get_sym();
				sym = parse_type_name(sym, &dcl);
				if (sym != YY__RPAREN) {
					yy_error_sym("')' expected, got", sym);
				}
				sym = get_sym();
				zend_ffi_expr_alignof_type(val, &dcl);
			} else if (YY_IN_SET(sym, (YY_ID,YY_OCTNUMBER,YY_DECNUMBER,YY_HEXNUMBER,YY_FLOATNUMBER,YY_STRING,YY_CHARACTER,YY__LPAREN,YY__PLUS_PLUS,YY__MINUS_MINUS,YY__AND,YY__STAR,YY__PLUS,YY__MINUS,YY__TILDE,YY__BANG,YY_SIZEOF,YY__ALIGNOF,YY___ALIGNOF,YY___ALIGNOF__), "\000\000\002\000\000\000\004\200\000\343\377\017")) {
				sym = parse_unary_expression(sym, val);
				zend_ffi_expr_alignof_val(val);
			} else {
				yy_error_sym("unexpected", sym);
			}
			break;
		default:
			yy_error_sym("unexpected", sym);
	}
	return sym;
}

static int parse_ID(int sym, const char **name, size_t *name_len) {
	if (sym != YY_ID) {
		yy_error_sym("<ID> expected, got", sym);
	}
	*name = (const char*)yy_text; *name_len = yy_pos - yy_text;
	sym = get_sym();
	return sym;
}

static int parse_OCTNUMBER(int sym, zend_ffi_val *val) {
	if (sym != YY_OCTNUMBER) {
		yy_error_sym("<OCTNUMBER> expected, got", sym);
	}
	zend_ffi_val_number(val, 8, (const char*)yy_text, yy_pos - yy_text);
	sym = get_sym();
	return sym;
}

static int parse_DECNUMBER(int sym, zend_ffi_val *val) {
	if (sym != YY_DECNUMBER) {
		yy_error_sym("<DECNUMBER> expected, got", sym);
	}
	zend_ffi_val_number(val, 10, (const char*)yy_text, yy_pos - yy_text);
	sym = get_sym();
	return sym;
}

static int parse_HEXNUMBER(int sym, zend_ffi_val *val) {
	if (sym != YY_HEXNUMBER) {
		yy_error_sym("<HEXNUMBER> expected, got", sym);
	}
	zend_ffi_val_number(val, 16, (const char*)yy_text + 2, yy_pos - yy_text - 2);
	sym = get_sym();
	return sym;
}

static int parse_FLOATNUMBER(int sym, zend_ffi_val *val) {
	if (sym != YY_FLOATNUMBER) {
		yy_error_sym("<FLOATNUMBER> expected, got", sym);
	}
	zend_ffi_val_float_number(val, (const char*)yy_text, yy_pos - yy_text);
	sym = get_sym();
	return sym;
}

static int parse_STRING(int sym, zend_ffi_val *val) {
	if (sym != YY_STRING) {
		yy_error_sym("<STRING> expected, got", sym);
	}
	zend_ffi_val_string(val, (const char*)yy_text, yy_pos - yy_text);
	sym = get_sym();
	return sym;
}

static int parse_CHARACTER(int sym, zend_ffi_val *val) {
	if (sym != YY_CHARACTER) {
		yy_error_sym("<CHARACTER> expected, got", sym);
	}
	zend_ffi_val_character(val, (const char*)yy_text, yy_pos - yy_text);
	sym = get_sym();
	return sym;
}

static void parse(void) {
	int sym;

	yy_pos = yy_text = yy_buf;
	yy_line = 1;
	sym = parse_declarations(get_sym());
	if (sym != YY_EOF) {
		yy_error_sym("<EOF> expected, got", sym);
	}
}

int zend_ffi_parse_decl(const char *str, size_t len) {
	if (SETJMP(FFI_G(bailout))==0) {
		FFI_G(allow_vla) = 0;
		yy_buf = (unsigned char*)str;
		yy_end = yy_buf + len;
		parse();
		return SUCCESS;
	} else {
		return FAILURE;
	}
}

int zend_ffi_parse_type(const char *str, size_t len, zend_ffi_dcl *dcl) {
	int sym;

	if (SETJMP(FFI_G(bailout))==0) {
		FFI_G(allow_vla) = 0;
		yy_pos = yy_text = yy_buf = (unsigned char*)str;
		yy_end = yy_buf + len;
		yy_line = 1;
		sym = parse_type_name(get_sym(), dcl);
		if (sym != YY_EOF) {
			yy_error_sym("<EOF> expected, got", sym);
		}
		zend_ffi_validate_type_name(dcl);
		return SUCCESS;
	} else {
		return FAILURE;
	};
}

static void yy_error(const char *msg) {
	zend_ffi_parser_error("%s at line %d", msg, yy_line);
}

static void yy_error_sym(const char *msg, int sym) {
	zend_ffi_parser_error("%s '%s' at line %d", msg, sym_name[sym], yy_line);
}
