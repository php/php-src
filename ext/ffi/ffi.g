/*
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

/*
To generate ffi_parser.c use llk <https://github.com/dstogov/llk>:
php llk.php ffi.g
*/

%start          declarations
%sub-start      type_name
%case-sensetive true
%global-vars    false
%output         "ffi_parser.c"
%language       "c"
%indent         "\t"

%{
/*
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

%}

declarations:
	(
		{zend_ffi_dcl common_dcl = ZEND_FFI_ATTR_INIT;}
		"__extension__"?
		declaration_specifiers(&common_dcl)
		(
			{const char *name;}
			{size_t name_len;}
			{zend_ffi_dcl dcl;}
			{dcl = common_dcl;}
			declarator(&dcl, &name, &name_len)
			(
				{zend_ffi_val asm_str;}
				"__asm__"
				"("
				STRING(&asm_str)+
				/*TODO*/
				")"
			)?
			attributes(&dcl)?
			initializer?
			{zend_ffi_declare(name, name_len, &dcl);}
			(	","
				{dcl = common_dcl;}
				declarator(&dcl, &name, &name_len)
				attributes(&dcl)?
				initializer?
				{zend_ffi_declare(name, name_len, &dcl);}
			)*
		)?
		";"
	)*
;

declaration_specifiers(zend_ffi_dcl *dcl):
	(	?{sym != YY_ID || !(dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS)}
		(	{if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);}
			"typedef"
			{dcl->flags |= ZEND_FFI_DCL_TYPEDEF;}
		|	{if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);}
			"extern"
			{dcl->flags |= ZEND_FFI_DCL_EXTERN;}
		|	{if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);}
			"static"
			{dcl->flags |= ZEND_FFI_DCL_STATIC;}
		|	{if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);}
			"auto"
			{dcl->flags |= ZEND_FFI_DCL_AUTO;}
		|	{if (dcl->flags & ZEND_FFI_DCL_STORAGE_CLASS) yy_error_sym("unexpected", sym);}
			"register"
			{dcl->flags |= ZEND_FFI_DCL_REGISTER;}
//		|	"_Thread_local" // TODO: not-implemented ???
		|	("inline"|"__inline"|"__inline__")
			{dcl->flags |= ZEND_FFI_DCL_INLINE;}
		|	"_Noreturn"
			{dcl->flags |= ZEND_FFI_DCL_NO_RETURN;}
		|	"_Alignas"
			"("
			(	&type_name_start
				{zend_ffi_dcl align_dcl = ZEND_FFI_ATTR_INIT;}
				type_name(&align_dcl)
				{zend_ffi_align_as_type(dcl, &align_dcl);}
			|	{zend_ffi_val align_val;}
				constant_expression(&align_val)
				{zend_ffi_align_as_val(dcl, &align_val);}
			)
			")"
		|	attributes(dcl)
		|	type_qualifier(dcl)
		|	type_specifier(dcl)
		)
	)+
;

specifier_qualifier_list(zend_ffi_dcl *dcl):
	"__extension__"?
	(	?{sym != YY_ID || zend_ffi_is_typedef_name((const char*)yy_text, yy_pos - yy_text) || (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) == 0}
		(	type_specifier(dcl)
		|	type_qualifier(dcl)
		|	attributes(dcl)
		)
	)+
;

type_qualifier_list(zend_ffi_dcl *dcl):
	(	type_qualifier(dcl)
	|	attributes(dcl)
	)+
;

type_qualifier(zend_ffi_dcl *dcl):
		("const"|"__const"|"__const__")
		{dcl->flags |= ZEND_FFI_DCL_CONST;}
		{dcl->attr |= ZEND_FFI_ATTR_CONST;}
	|	("restrict"|"__restrict"|"__restrict__")
		{dcl->flags |= ZEND_FFI_DCL_RESTRICT;}
	|	("volatile"|"__volatile"|"__volatile__")
		{dcl->flags |= ZEND_FFI_DCL_VOLATILE;}
	|	"_Atomic"
		{dcl->flags |= ZEND_FFI_DCL_ATOMIC;}
;

type_specifier(zend_ffi_dcl *dcl):
    {const char *name;}
    {size_t name_len;}
	(	{if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);}
		"void"
		{dcl->flags |= ZEND_FFI_DCL_VOID;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED))) yy_error_sym("unexpected", sym);}
		"char"
		{dcl->flags |= ZEND_FFI_DCL_CHAR;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);}
		"short"
		{dcl->flags |= ZEND_FFI_DCL_SHORT;}
	|   {if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG))) yy_error_sym("unexpected", sym);}
		"int"
		{dcl->flags |= ZEND_FFI_DCL_INT;}
	|	{
			if (dcl->flags & ZEND_FFI_DCL_LONG) {
				if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);
				dcl->flags |= ZEND_FFI_DCL_LONG_LONG;
			} else {
				if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_SIGNED|ZEND_FFI_DCL_UNSIGNED|ZEND_FFI_DCL_INT|ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);
				dcl->flags |= ZEND_FFI_DCL_LONG;
			}
		}
		"long"
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);}
		"float"
		{dcl->flags |= ZEND_FFI_DCL_FLOAT;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_COMPLEX))) yy_error_sym("unexpected", sym);}
		"double"
		{dcl->flags |= ZEND_FFI_DCL_DOUBLE;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);}
		"signed"
		{dcl->flags |= ZEND_FFI_DCL_SIGNED;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_CHAR|ZEND_FFI_DCL_SHORT|ZEND_FFI_DCL_LONG|ZEND_FFI_DCL_LONG_LONG|ZEND_FFI_DCL_INT))) yy_error_sym("unexpected", sym);}
		"unsigned"
		{dcl->flags |= ZEND_FFI_DCL_UNSIGNED;}
	|	{if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);}
		"_Bool"
		{dcl->flags |= ZEND_FFI_DCL_BOOL;}
	|	{if (dcl->flags & (ZEND_FFI_DCL_TYPE_SPECIFIERS-(ZEND_FFI_DCL_FLOAT|ZEND_FFI_DCL_DOUBLE|ZEND_FFI_DCL_LONG))) yy_error_sym("unexpected", sym);}
		("_Complex"|"complex"|"__complex"|"__complex__")
		{dcl->flags |= ZEND_FFI_DCL_COMPLEX;}
//	|	"_Atomic" "(" type_name ")" // TODO: not-implemented ???
	|	{if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);}
		struct_or_union_specifier(dcl)
	|	{if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);}
		enum_specifier(dcl)
	|   {if (dcl->flags & ZEND_FFI_DCL_TYPE_SPECIFIERS) yy_error_sym("unexpected", sym);}
		{/*redeclaration of '%.*s' ??? */}
		ID(&name, &name_len)
		{dcl->flags |= ZEND_FFI_DCL_TYPEDEF_NAME;}
		{zend_ffi_resolve_typedef(name, name_len, dcl);}
	)
;

struct_or_union_specifier(zend_ffi_dcl *dcl):
	(	"struct"
		{dcl->flags |= ZEND_FFI_DCL_STRUCT;}
	| 	"union"
		{dcl->flags |= ZEND_FFI_DCL_UNION;}
	)
	attributes(dcl)?
	(   {const char *name;}
		{size_t name_len;}
		ID(&name, &name_len)
		{zend_ffi_declare_tag(name, name_len, dcl, 1);}
		(	struct_contents(dcl)
			{zend_ffi_declare_tag(name, name_len, dcl, 0);}
		)?
	|	{zend_ffi_make_struct_type(dcl);}
		struct_contents(dcl)
	)
;

struct_contents(zend_ffi_dcl *dcl):
	"{"
	(	struct_declaration(dcl)
		(	";"
			struct_declaration(dcl)
		)*
		(";")?
	)?
    "}"
	attributes(dcl)?+
	{zend_ffi_adjust_struct_size(dcl);}
;


struct_declaration(zend_ffi_dcl *struct_dcl):
	{zend_ffi_dcl common_field_dcl = ZEND_FFI_ATTR_INIT;}
	specifier_qualifier_list(&common_field_dcl)
	(	/* empty */
		{zend_ffi_add_anonymous_field(struct_dcl, &common_field_dcl);}
	|	struct_declarator(struct_dcl, &common_field_dcl)
		(	","
			{zend_ffi_dcl field_dcl = common_field_dcl;}
			attributes(&field_dcl)?
			struct_declarator(struct_dcl, &field_dcl)
		)*
	)
;

struct_declarator(zend_ffi_dcl *struct_dcl, zend_ffi_dcl *field_dcl):
	{const char *name = NULL;}
	{size_t name_len = 0;}
	{zend_ffi_val bits;}
	(	declarator(field_dcl, &name, &name_len)
		(	":"
			constant_expression(&bits)
			attributes(field_dcl)?
			{zend_ffi_add_bit_field(struct_dcl, name, name_len, field_dcl, &bits);}
		|	/*empty */
			attributes(field_dcl)?
			{zend_ffi_add_field(struct_dcl, name, name_len, field_dcl);}
		)
	|	":"
		constant_expression(&bits)
		{zend_ffi_add_bit_field(struct_dcl, NULL, 0, field_dcl, &bits);}
	)
;

enum_specifier(zend_ffi_dcl *dcl):
	"enum"
	{dcl->flags |= ZEND_FFI_DCL_ENUM;}
	attributes(dcl)?
	(   {const char *name;}
		{size_t name_len;}
		ID(&name, &name_len)
		(	{zend_ffi_declare_tag(name, name_len, dcl, 0);}
			"{"
			enumerator_list(dcl)
			"}"
			attributes(dcl)?+
		|	{zend_ffi_declare_tag(name, name_len, dcl, 1);}
		)
	|	"{"
		{zend_ffi_make_enum_type(dcl);}
		enumerator_list(dcl)
		"}"
		attributes(dcl)?+
	)
;

enumerator_list(zend_ffi_dcl *enum_dcl):
	{int64_t min = 0, max = 0, last = -1;}
	enumerator(enum_dcl, &min, &max, &last)
	(	","
		enumerator(enum_dcl, &min, &max, &last)
	)*
	","?
;

enumerator(zend_ffi_dcl *enum_dcl, int64_t *min, int64_t *max, int64_t *last):
	{const char *name;}
	{size_t name_len;}
	{zend_ffi_val val = {.kind = ZEND_FFI_VAL_EMPTY};}
	ID(&name, &name_len)
	(	"="
		constant_expression(&val)
	)?
	{zend_ffi_add_enum_val(enum_dcl, name, name_len, &val, min, max, last);}
;

declarator(zend_ffi_dcl *dcl, const char **name, size_t *name_len):
	{zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};}
	{zend_bool nested = 0;}
	pointer(dcl)?
	(	ID(name, name_len)
	|	"("
		attributes(&nested_dcl)?
		declarator(&nested_dcl, name, name_len)
		")"
		{nested = 1;}
	)
	array_or_function_declarators(dcl, &nested_dcl)?
	{if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);}
;

abstract_declarator(zend_ffi_dcl *dcl):
	{zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};}
	{zend_bool nested = 0;}
	pointer(dcl)?
	(	&nested_declarator_start
		"("
		attributes(&nested_dcl)?
		abstract_declarator(&nested_dcl)
		")"
		{nested = 1;}
	)?
	array_or_function_declarators(dcl, &nested_dcl)?
	{if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);}
;

parameter_declarator(zend_ffi_dcl *dcl, const char **name, size_t *name_len):
	{zend_ffi_dcl nested_dcl = {ZEND_FFI_DCL_CHAR, 0, 0, 0, NULL};}
	{zend_bool nested = 0;}
	pointer(dcl)?
	(	&nested_declarator_start
		"("
		attributes(&nested_dcl)?
		parameter_declarator(&nested_dcl, name, name_len)
		")"
		{nested = 1;}
	|	ID(name, name_len)
	|	/* empty */
	)
	array_or_function_declarators(dcl, &nested_dcl)?
	{if (nested) zend_ffi_nested_declaration(dcl, &nested_dcl);}
;

pointer(zend_ffi_dcl *dcl):
	(	"*"
		{zend_ffi_make_pointer_type(dcl);}
		type_qualifier_list(dcl)?
	)+
;

array_or_function_declarators(zend_ffi_dcl *dcl, zend_ffi_dcl *nested_dcl):
	{zend_ffi_dcl dummy = ZEND_FFI_ATTR_INIT;}
	{zend_ffi_val len = {.kind = ZEND_FFI_VAL_EMPTY};}
	{HashTable *args = NULL;}
	{uint32_t attr = 0;}
	(	"["
	    (	"static"
			type_qualifier_list(&dummy)?
			assignment_expression(&len)
		|	type_qualifier_list(&dummy)
			(	"static" assignment_expression(&len)
			|	/* empty */
				{attr |= ZEND_FFI_ATTR_INCOMPLETE_ARRAY;}
			|	"*"
				{attr |= ZEND_FFI_ATTR_VLA;}
			|	assignment_expression(&len)
			)
		|	(	/* empty */
				{attr |= ZEND_FFI_ATTR_INCOMPLETE_ARRAY;}
			|	"*"
				{attr |= ZEND_FFI_ATTR_VLA;}
			|	assignment_expression(&len)
			)
		)
		"]"
		array_or_function_declarators(dcl, nested_dcl)?
		{dcl->attr |= attr;}
		{zend_ffi_make_array_type(dcl, &len);}
	|	"("
		(
			parameter_declaration(&args)
			(	","
				parameter_declaration(&args)
			)*
			(
				","
				"..."
				{attr |= ZEND_FFI_ATTR_VARIADIC;}
			)?
		|	"..."
			{attr |= ZEND_FFI_ATTR_VARIADIC;}
		)?
		")"
		array_or_function_declarators(dcl, nested_dcl)?
		{dcl->attr |= attr;}
		{zend_ffi_make_func_type(dcl, args, nested_dcl);}
//	|	"(" (ID ("," ID)*)? ")" // TODO: ANSI function not-implemented ???
	)
;

parameter_declaration(HashTable **args):
	{const char *name = NULL;}
	{size_t name_len = 0;}
	{zend_bool old_allow_vla = FFI_G(allow_vla);}
	{FFI_G(allow_vla) = 1;}
	{zend_ffi_dcl param_dcl = ZEND_FFI_ATTR_INIT;}
	specifier_qualifier_list(&param_dcl)
	parameter_declarator(&param_dcl, &name, &name_len)
	/*attributes(&param_dcl)? conflict ???*/
	{zend_ffi_add_arg(args, name, name_len, &param_dcl);}
	{FFI_G(allow_vla) = old_allow_vla;}
;

type_name(zend_ffi_dcl *dcl):
	specifier_qualifier_list(dcl)
	abstract_declarator(dcl)
;

attributes(zend_ffi_dcl *dcl):
	{const char *name;}
	{size_t name_len;}
	{zend_ffi_val val;}
	(
		("__attribute"|"__attribute__")
		"("
		"("
		attrib(dcl)
		(	","
			attrib(dcl)
		)*
		")"
		")"
	|	"__declspec"
		"("
			(	ID(&name, &name_len)
				(
					"("
						assignment_expression(&val)
						{zend_ffi_add_msvc_attribute_value(dcl, name, name_len, &val);}
					")"
				)?
			)+
		")"
	|	"__cdecl"
		{zend_ffi_set_abi(dcl, ZEND_FFI_ABI_CDECL);}
	|	"__stdcall"
		{zend_ffi_set_abi(dcl, ZEND_FFI_ABI_STDCALL);}
	|	"__fastcall"
		{zend_ffi_set_abi(dcl, ZEND_FFI_ABI_FASTCALL);}
	|	"__thiscall"
		{zend_ffi_set_abi(dcl, ZEND_FFI_ABI_THISCALL);}
	|	"__vectorcall"
		{zend_ffi_set_abi(dcl, ZEND_FFI_ABI_VECTORCALL);}
	)++
;

attrib(zend_ffi_dcl *dcl):
	{const char *name;}
	{size_t name_len;}
	{int n;}
	{zend_ffi_val val;}
	{zend_bool orig_attribute_parsing;}
	(   ID(&name, &name_len)
		(	/* empty */
			{zend_ffi_add_attribute(dcl, name, name_len);}
		|	"("
			{orig_attribute_parsing = FFI_G(attribute_parsing);}
			{FFI_G(attribute_parsing) = 1;}
			assignment_expression(&val)
			{zend_ffi_add_attribute_value(dcl, name, name_len, 0, &val);}
			{n = 0;}
			(	","
				assignment_expression(&val)
				{zend_ffi_add_attribute_value(dcl, name, name_len, ++n, &val);}
			)*
			{FFI_G(attribute_parsing) = orig_attribute_parsing;}
			")"
		)
	|	"const"
	|	"__const"
	|	"__const__"
	)?
;

initializer:
	{zend_ffi_val dummy;}
	"="
	(	assignment_expression(&dummy)
	|	"{" designation? initializer ( "," designation? initializer)* ","? "}"
	)
;

designation:
	{const char *name;}
	{size_t name_len;}
	{zend_ffi_val dummy;}
	(	"[" constant_expression(&dummy) "]"
	|	"." ID(&name, &name_len)
	)+
	"="
;

expr_list:
	{zend_ffi_val dummy;}
	assignment_expression(&dummy)
	(	","
		assignment_expression(&dummy)
	)*
;

expression(zend_ffi_val *val):
	assignment_expression(val)
	(	","
		assignment_expression(val)
	)*
;

assignment_expression(zend_ffi_val *val):
//	(	unary_expression
//		("="|"*="|"/="|"%="|"+="|"-="|"<<="|">>="|"&="|"^="|"|=")
//	)* // TODO: not-implemented ???
	conditional_expression(val)
;

constant_expression(zend_ffi_val *val):
	conditional_expression(val)
;

conditional_expression(zend_ffi_val *val):
	{zend_ffi_val op2, op3;}
	logical_or_expression(val)
	(	"?"
		expression(&op2)
		":"
		conditional_expression(&op3)
		{zend_ffi_expr_conditional(val, &op2, &op3);}
	)?
;

logical_or_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	logical_and_expression(val)
	(   "||"
		logical_and_expression(&op2)
		{zend_ffi_expr_bool_or(val, &op2);}
	)*
;

logical_and_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	inclusive_or_expression(val)
	(   "&&"
		inclusive_or_expression(&op2)
		{zend_ffi_expr_bool_and(val, &op2);}
	)*
;

inclusive_or_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	exclusive_or_expression(val)
	(	"|"
		exclusive_or_expression(&op2)
		{zend_ffi_expr_bw_or(val, &op2);}
	)*
;

exclusive_or_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	and_expression(val)
	(	"^"
		and_expression(&op2)
		{zend_ffi_expr_bw_xor(val, &op2);}
	)*
;

and_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	equality_expression(val)
	(	"&"
		equality_expression(&op2)
		{zend_ffi_expr_bw_and(val, &op2);}
	)*
;

equality_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	relational_expression(val)
	(	"=="
		relational_expression(&op2)
		{zend_ffi_expr_is_equal(val, &op2);}
	|	"!="
		relational_expression(&op2)
		{zend_ffi_expr_is_not_equal(val, &op2);}
	)*
;

relational_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	shift_expression(val)
	(	"<"
		shift_expression(&op2)
		{zend_ffi_expr_is_less(val, &op2);}
	|	">"
		shift_expression(&op2)
		{zend_ffi_expr_is_greater(val, &op2);}
	|	"<="
		shift_expression(&op2)
		{zend_ffi_expr_is_less_or_equal(val, &op2);}
	|	">="
		shift_expression(&op2)
		{zend_ffi_expr_is_greater_or_equal(val, &op2);}
	)*
;

shift_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	additive_expression(val)
	(	"<<"
		additive_expression(&op2)
		{zend_ffi_expr_shift_left(val, &op2);}
	|	">>"
		additive_expression(&op2)
		{zend_ffi_expr_shift_right(val, &op2);}
	)*
;

additive_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	multiplicative_expression(val)
	(	"+"
		multiplicative_expression(&op2)
		{zend_ffi_expr_add(val, &op2);}
	|	"-"
		multiplicative_expression(&op2)
		{zend_ffi_expr_sub(val, &op2);}
	)*
;

multiplicative_expression(zend_ffi_val *val):
	{zend_ffi_val op2;}
	cast_expression(val)
	(	"*"
		cast_expression(&op2)
		{zend_ffi_expr_mul(val, &op2);}
	|	"/"
		cast_expression(&op2)
		{zend_ffi_expr_div(val, &op2);}
	|	"%"
		cast_expression(&op2)
		{zend_ffi_expr_mod(val, &op2);}
	)*
;

cast_expression(zend_ffi_val *val):
	{int do_cast = 0;}
	{zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;}
	(	&( "(" type_name_start )
		"("
		type_name(&dcl)
		")"
		{do_cast = 1;}
	)?
	unary_expression(val)
	{if (do_cast) zend_ffi_expr_cast(val, &dcl);}
;

unary_expression(zend_ffi_val *val):
	{const char *name;}
	{size_t name_len;}
	{zend_ffi_dcl dcl = ZEND_FFI_ATTR_INIT;}
	(	ID(&name, &name_len)
		{zend_ffi_resolve_const(name, name_len, val);}
		(
			(	"["
				expr_list
				"]"
			|	"("
				expr_list?
				")"
			|	"."
				ID(&name, &name_len)
			|	"->"
				ID(&name, &name_len)
		    |	"++"
			|	"--"
			)
			{zend_ffi_val_error(val);}
		)*
	|	OCTNUMBER(val)
	|	DECNUMBER(val)
	|	HEXNUMBER(val)
	|	FLOATNUMBER(val)
	|	STRING(val)
	|	CHARACTER(val)
	|	"("
		expression(val)
		")"
	|	"++"
		unary_expression(val)
		{zend_ffi_val_error(val);}
	|	"--"
		unary_expression(val)
		{zend_ffi_val_error(val);}
	|	"&"
		cast_expression(val)
		{zend_ffi_val_error(val);}
	|	"*"
		cast_expression(val)
		{zend_ffi_val_error(val);}
	|	"+"
		cast_expression(val)
		{zend_ffi_expr_plus(val);}
	|	"-"
		cast_expression(val)
		{zend_ffi_expr_neg(val);}
	|	"~"
		cast_expression(val)
		{zend_ffi_expr_bw_not(val);}
	|	"!"
		cast_expression(val)
		{zend_ffi_expr_bool_not(val);}
	|	"sizeof"
		(	&( "(" type_name_start )
			"("
			type_name(&dcl)
			")"
			{zend_ffi_expr_sizeof_type(val, &dcl);}
		|	unary_expression(val)
			{zend_ffi_expr_sizeof_val(val);}
		)
	|	"_Alignof"
		"("
		type_name(&dcl)
		")"
		{zend_ffi_expr_alignof_type(val, &dcl);}
	|	("__alignof"|"__alignof__")
		(	&( "(" type_name_start )
			"("
			type_name(&dcl)
			")"
			{zend_ffi_expr_alignof_type(val, &dcl);}
		|	unary_expression(val)
			{zend_ffi_expr_alignof_val(val);}
		)
	)
;

/* lookahead rules */
nested_declarator_start:
    "("
	(   ?{!zend_ffi_is_typedef_name((const char*)yy_text, yy_pos - yy_text)}
		ID
	|	"__attribute"
	|	"__attribute__"
	|	"__declspec"
	|	"*"
	|	"("
	|	"["
	)
;

type_name_start:
	(	?{zend_ffi_is_typedef_name((const char*)yy_text, yy_pos - yy_text)}
		ID
	|	"void"
	|	"char"
	|	"short"
	|   "int"
	|	"long"
	|	"float"
	|	"double"
	|	"signed"
	|	"unsigned"
	|	"_Bool"
	|	"_Complex"
	|	"complex"
	|	"__complex"
	|	"__complex__"
	|	"struct"
	|	"union"
	|	"enum"
	|	"const"
	|	"__const"
	|	"__const__"
	|	"restrict"
	|	"__restict"
	|	"__restrict__"
	|	"volatile"
	|	"__volatile"
	|	"__volatile__"
	|	"_Atomic"
	|	"__attribute"
	|	"__attribute__"
	|	"__declspec"
	)
;

/* scanner rules */
ID(const char **name, size_t *name_len):
	/[A-Za-z_][A-Za-z_0-9]*/
	{*name = (const char*)yy_text; *name_len = yy_pos - yy_text;}
;

OCTNUMBER(zend_ffi_val *val):
	/0[0-7]*([Uu](L|l|LL|l)?|[Ll][Uu]?|(LL|ll)[Uu])?/
	{zend_ffi_val_number(val, 8, (const char*)yy_text, yy_pos - yy_text);}
;

DECNUMBER(zend_ffi_val *val):
	/[1-9][0-9]*([Uu](L|l|LL|l)?|[Ll][Uu]?|(LL|ll)[Uu])?/
	{zend_ffi_val_number(val, 10, (const char*)yy_text, yy_pos - yy_text);}
;

HEXNUMBER(zend_ffi_val *val):
	/0[xX][0-9A-Fa-f][0-9A-Fa-f]*([Uu](L|l|LL|l)?|[Ll][Uu]?|(LL|ll)[Uu])?/
	{zend_ffi_val_number(val, 16, (const char*)yy_text + 2, yy_pos - yy_text - 2);}
;

FLOATNUMBER(zend_ffi_val *val):
	/([0-9]*\.[0-9]+([Ee][\+\-]?[0-9]+)?|[0-9]+\.([Ee][\+\-]?[0-9]+)?|[0-9]+[Ee][\+\-]?[0-9]+)[flFL]?/
	{zend_ffi_val_float_number(val, (const char*)yy_text, yy_pos - yy_text);}
;

STRING(zend_ffi_val *val):
	/(u8|u|U|L)?"([^"\\]|\\.)*"/
	{zend_ffi_val_string(val, (const char*)yy_text, yy_pos - yy_text);}
;

CHARACTER(zend_ffi_val *val):
	/[LuU]?'([^'\\]|\\.)*'/
	{zend_ffi_val_character(val, (const char*)yy_text, yy_pos - yy_text);}
;

EOL: /\r\n|\r|\n/;
WS: /[ \t\f\v]+/;
ONE_LINE_COMMENT: /(\/\/|#)[^\r\n]*(\r\n|\r|\n)/;
COMMENT: /\/\*([^\*]|\*+[^\*\/])*\*+\//;

SKIP: ( EOL | WS | ONE_LINE_COMMENT | COMMENT )*;

%%
int zend_ffi_parse_decl(const char *str, size_t len) {
	if (SETJMP(FFI_G(bailout))==0) {
		FFI_G(allow_vla) = 0;
		FFI_G(attribute_parsing) = 0;
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
		FFI_G(attribute_parsing) = 0;
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
