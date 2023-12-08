%require "3.0"
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
   |          Nikita Popov <nikic@php.net>                                |
   +----------------------------------------------------------------------+
*/

%code top {
#include "zend.h"
#include "zend_list.h"
#include "zend_globals.h"
#include "zend_API.h"
#include "zend_constants.h"
#include "zend_language_scanner.h"
#include "zend_exceptions.h"

#define YYSIZE_T size_t
#define yytnamerr zend_yytnamerr
static YYSIZE_T zend_yytnamerr(char*, const char*);

#ifdef _MSC_VER
#define YYMALLOC malloc
#define YYFREE free
#endif
}

%code requires {
#include "zend_compile.h"
}

%define api.prefix {zend}
%define api.pure full
%define api.value.type {zend_parser_stack_elem}
%define parse.error verbose
%expect 0

%destructor { zend_ast_destroy($$); } <ast>
%destructor { if ($$) zend_string_release_ex($$, 0); } <str>

%precedence T_THROW
%precedence PREC_ARROW_FUNCTION
%precedence T_INCLUDE T_INCLUDE_ONCE T_REQUIRE T_REQUIRE_ONCE
%left T_LOGICAL_OR
%left T_LOGICAL_XOR
%left T_LOGICAL_AND
%precedence T_PRINT
%precedence T_YIELD
%precedence T_DOUBLE_ARROW
%precedence T_YIELD_FROM
%precedence '=' T_PLUS_EQUAL T_MINUS_EQUAL T_MUL_EQUAL T_DIV_EQUAL T_CONCAT_EQUAL T_MOD_EQUAL T_AND_EQUAL T_OR_EQUAL T_XOR_EQUAL T_SL_EQUAL T_SR_EQUAL T_POW_EQUAL T_COALESCE_EQUAL
%left '?' ':'
%right T_COALESCE
%left T_BOOLEAN_OR
%left T_BOOLEAN_AND
%left '|'
%left '^'
%left T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG
%nonassoc T_IS_EQUAL T_IS_NOT_EQUAL T_IS_IDENTICAL T_IS_NOT_IDENTICAL T_SPACESHIP
%nonassoc '<' T_IS_SMALLER_OR_EQUAL '>' T_IS_GREATER_OR_EQUAL T_THIN_ARROW_LEFT
%left '.'
%left T_SL T_SR
%left '+' '-'
%left '*' '/' '%'
%precedence '!'
%precedence T_INSTANCEOF
%precedence '~' T_INT_CAST T_DOUBLE_CAST T_STRING_CAST T_ARRAY_CAST T_OBJECT_CAST T_BOOL_CAST T_UNSET_CAST '@'
%right T_POW
%precedence T_CLONE

/* Resolve danging else conflict */
%precedence T_NOELSE
%precedence T_ELSEIF
%precedence T_ELSE

%token <ast> T_LNUMBER   "integer"
%token <ast> T_DNUMBER   "floating-point number"
%token <ast> T_STRING    "identifier"
%token <ast> T_NAME_FULLY_QUALIFIED "fully qualified name"
%token <ast> T_NAME_RELATIVE "namespace-relative name"
%token <ast> T_NAME_QUALIFIED "namespaced name"
%token <ast> T_VARIABLE  "variable"
%token <ast> T_INLINE_HTML
%token <ast> T_ENCAPSED_AND_WHITESPACE  "string content"
%token <ast> T_CONSTANT_ENCAPSED_STRING "quoted string"
%token <ast> T_STRING_VARNAME "variable name"
%token <ast> T_NUM_STRING "number"

%token <ident> T_INCLUDE       "'include'"
%token <ident> T_INCLUDE_ONCE  "'include_once'"
%token <ident> T_EVAL          "'eval'"
%token <ident> T_REQUIRE       "'require'"
%token <ident> T_REQUIRE_ONCE  "'require_once'"
%token <ident> T_LOGICAL_OR    "'or'"
%token <ident> T_LOGICAL_XOR   "'xor'"
%token <ident> T_LOGICAL_AND   "'and'"
%token <ident> T_PRINT         "'print'"
%token <ident> T_YIELD         "'yield'"
%token <ident> T_YIELD_FROM    "'yield from'"
%token <ident> T_INSTANCEOF    "'instanceof'"
%token <ident> T_NEW           "'new'"
%token <ident> T_CLONE         "'clone'"
%token <ident> T_EXIT          "'exit'"
%token <ident> T_IF            "'if'"
%token <ident> T_ELSEIF        "'elseif'"
%token <ident> T_ELSE          "'else'"
%token <ident> T_ENDIF         "'endif'"
%token <ident> T_ECHO          "'echo'"
%token <ident> T_DO            "'do'"
%token <ident> T_WHILE         "'while'"
%token <ident> T_ENDWHILE      "'endwhile'"
%token <ident> T_FOR           "'for'"
%token <ident> T_ENDFOR        "'endfor'"
%token <ident> T_FOREACH       "'foreach'"
%token <ident> T_ENDFOREACH    "'endforeach'"
%token <ident> T_DECLARE       "'declare'"
%token <ident> T_ENDDECLARE    "'enddeclare'"
%token <ident> T_AS            "'as'"
%token <ident> T_SWITCH        "'switch'"
%token <ident> T_ENDSWITCH     "'endswitch'"
%token <ident> T_CASE          "'case'"
%token <ident> T_DEFAULT       "'default'"
%token <ident> T_MATCH         "'match'"
%token <ident> T_BREAK         "'break'"
%token <ident> T_CONTINUE      "'continue'"
%token <ident> T_GOTO          "'goto'"
%token <ident> T_FUNCTION      "'function'"
%token <ident> T_FN            "'fn'"
%token <ident> T_CONST         "'const'"
%token <ident> T_RETURN        "'return'"
%token <ident> T_TRY           "'try'"
%token <ident> T_CATCH         "'catch'"
%token <ident> T_FINALLY       "'finally'"
%token <ident> T_THROW         "'throw'"
%token <ident> T_USE           "'use'"
%token <ident> T_INSTEADOF     "'insteadof'"
%token <ident> T_GLOBAL        "'global'"
%token <ident> T_STATIC        "'static'"
%token <ident> T_ABSTRACT      "'abstract'"
%token <ident> T_FINAL         "'final'"
%token <ident> T_PRIVATE       "'private'"
%token <ident> T_PROTECTED     "'protected'"
%token <ident> T_PUBLIC        "'public'"
%token <ident> T_READONLY      "'readonly'"
%token <ident> T_VAR           "'var'"
%token <ident> T_UNSET         "'unset'"
%token <ident> T_ISSET         "'isset'"
%token <ident> T_EMPTY         "'empty'"
%token <ident> T_HALT_COMPILER "'__halt_compiler'"
%token <ident> T_CLASS         "'class'"
%token <ident> T_TRAIT         "'trait'"
%token <ident> T_INTERFACE     "'interface'"
%token <ident> T_ENUM          "'enum'"
%token <ident> T_EXTENDS       "'extends'"
%token <ident> T_IMPLEMENTS    "'implements'"
%token <ident> T_NAMESPACE     "'namespace'"
%token <ident> T_LIST            "'list'"
%token <ident> T_ARRAY           "'array'"
%token <ident> T_CALLABLE        "'callable'"
%token <ident> T_LINE            "'__LINE__'"
%token <ident> T_FILE            "'__FILE__'"
%token <ident> T_DIR             "'__DIR__'"
%token <ident> T_CLASS_C         "'__CLASS__'"
%token <ident> T_TRAIT_C         "'__TRAIT__'"
%token <ident> T_METHOD_C        "'__METHOD__'"
%token <ident> T_FUNC_C          "'__FUNCTION__'"
%token <ident> T_NS_C            "'__NAMESPACE__'"

%token END 0 "end of file"
%token T_ATTRIBUTE    "'#['"
%token T_PLUS_EQUAL   "'+='"
%token T_MINUS_EQUAL  "'-='"
%token T_MUL_EQUAL    "'*='"
%token T_DIV_EQUAL    "'/='"
%token T_CONCAT_EQUAL "'.='"
%token T_MOD_EQUAL    "'%='"
%token T_AND_EQUAL    "'&='"
%token T_OR_EQUAL     "'|='"
%token T_XOR_EQUAL    "'^='"
%token T_SL_EQUAL     "'<<='"
%token T_SR_EQUAL     "'>>='"
%token T_COALESCE_EQUAL "'??='"
%token T_BOOLEAN_OR   "'||'"
%token T_BOOLEAN_AND  "'&&'"
%token T_IS_EQUAL     "'=='"
%token T_IS_NOT_EQUAL "'!='"
%token T_IS_IDENTICAL "'==='"
%token T_IS_NOT_IDENTICAL "'!=='"
%token T_IS_SMALLER_OR_EQUAL "'<='"
%token T_IS_GREATER_OR_EQUAL "'>='"
%token T_SPACESHIP "'<=>'"
%token T_SL "'<<'"
%token T_SR "'>>'"
%token T_INC "'++'"
%token T_DEC "'--'"
%token T_INT_CAST    "'(int)'"
%token T_DOUBLE_CAST "'(double)'"
%token T_STRING_CAST "'(string)'"
%token T_ARRAY_CAST  "'(array)'"
%token T_OBJECT_CAST "'(object)'"
%token T_BOOL_CAST   "'(bool)'"
%token T_UNSET_CAST  "'(unset)'"
%token T_OBJECT_OPERATOR "'->'"
%token T_NULLSAFE_OBJECT_OPERATOR "'?->'"
%token T_DOUBLE_ARROW    "'=>'"
%token T_THIN_ARROW_LEFT "'<-'"
%token T_COMMENT         "comment"
%token T_DOC_COMMENT     "doc comment"
%token T_OPEN_TAG        "open tag"
%token T_OPEN_TAG_WITH_ECHO "'<?='"
%token T_CLOSE_TAG       "'?>'"
%token T_WHITESPACE      "whitespace"
%token T_START_HEREDOC   "heredoc start"
%token T_END_HEREDOC     "heredoc end"
%token T_DOLLAR_OPEN_CURLY_BRACES "'${'"
%token T_CURLY_OPEN      "'{$'"
%token T_PAAMAYIM_NEKUDOTAYIM "'::'"
%token T_NS_SEPARATOR    "'\\'"
%token T_ELLIPSIS        "'...'"
%token T_COALESCE        "'??'"
%token T_POW             "'**'"
%token T_POW_EQUAL       "'**='"
/* We need to split the & token in two to avoid a shift/reduce conflict. For T1&$v and T1&T2,
 * with only one token lookahead, bison does not know whether to reduce T1 as a complete type,
 * or shift to continue parsing an intersection type. */
%token T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG     "'&'"
/* Bison warns on duplicate token literals, so use a different dummy value here.
 * It will be fixed up by zend_yytnamerr() later. */
%token T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG "amp"
%token T_BAD_CHARACTER   "invalid character"

/* Token used to force a parse error from the lexer */
%token T_ERROR

%type <ast> top_statement namespace_name name statement function_declaration_statement
%type <ast> class_declaration_statement trait_declaration_statement legacy_namespace_name
%type <ast> interface_declaration_statement interface_extends_list
%type <ast> group_use_declaration inline_use_declarations inline_use_declaration
%type <ast> mixed_group_use_declaration use_declaration unprefixed_use_declaration
%type <ast> unprefixed_use_declarations const_decl inner_statement
%type <ast> expr optional_expr while_statement for_statement foreach_variable
%type <ast> foreach_statement declare_statement finally_statement unset_variable variable
%type <ast> extends_from parameter optional_type_without_static argument global_var
%type <ast> static_var class_statement trait_adaptation trait_precedence trait_alias
%type <ast> absolute_trait_method_reference trait_method_reference property echo_expr
%type <ast> new_expr anonymous_class class_name class_name_reference simple_variable
%type <ast> internal_functions_in_yacc
%type <ast> exit_expr scalar backticks_expr lexical_var function_call member_name property_name
%type <ast> variable_class_name dereferenceable_scalar constant class_constant
%type <ast> fully_dereferenceable array_object_dereferenceable
%type <ast> callable_expr callable_variable static_member new_variable
%type <ast> encaps_var encaps_var_offset isset_variables
%type <ast> top_statement_list use_declarations const_list inner_statement_list if_stmt
%type <ast> alt_if_stmt for_exprs switch_case_list global_var_list static_var_list
%type <ast> echo_expr_list unset_variables catch_name_list catch_list optional_variable parameter_list class_statement_list
%type <ast> implements_list case_list if_stmt_without_else
%type <ast> non_empty_parameter_list argument_list non_empty_argument_list property_list
%type <ast> class_const_list class_const_decl class_name_list trait_adaptations method_body non_empty_for_exprs
%type <ast> ctor_arguments alt_if_stmt_without_else trait_adaptation_list lexical_vars
%type <ast> lexical_var_list encaps_list
%type <ast> array_pair non_empty_array_pair_list array_pair_list possible_array_pair
%type <ast> isset_variable type return_type type_expr type_without_static
%type <ast> identifier type_expr_without_static union_type_without_static_element union_type_without_static intersection_type_without_static
%type <ast> inline_function union_type_element union_type intersection_type
%type <ast> attributed_statement attributed_class_statement attributed_parameter
%type <ast> attribute_decl attribute attributes attribute_group namespace_declaration_name
%type <ast> match match_arm_list non_empty_match_arm_list match_arm match_arm_cond_list match_arm_body block_expr
%type <ast> enum_declaration_statement enum_backing_type enum_case enum_case_expr
%type <ast> function_name non_empty_member_modifiers

%type <num> returns_ref function fn is_reference is_variadic property_modifiers
%type <num> method_modifiers class_const_modifiers member_modifier optional_cpp_modifiers
%type <num> class_modifiers class_modifier anonymous_class_modifiers anonymous_class_modifiers_optional use_type backup_fn_flags

%type <ptr> backup_lex_pos
%type <str> backup_doc_comment

%type <ident> reserved_non_modifiers semi_reserved

%% /* Rules */

start:
	top_statement_list	{ CG(ast) = $1; (void) zendnerrs; }
;

reserved_non_modifiers:
	  T_INCLUDE | T_INCLUDE_ONCE | T_EVAL | T_REQUIRE | T_REQUIRE_ONCE | T_LOGICAL_OR | T_LOGICAL_XOR | T_LOGICAL_AND
	| T_INSTANCEOF | T_NEW | T_CLONE | T_EXIT | T_IF | T_ELSEIF | T_ELSE | T_ENDIF | T_ECHO | T_DO | T_WHILE | T_ENDWHILE
	| T_FOR | T_ENDFOR | T_FOREACH | T_ENDFOREACH | T_DECLARE | T_ENDDECLARE | T_AS | T_TRY | T_CATCH | T_FINALLY
	| T_THROW | T_USE | T_INSTEADOF | T_GLOBAL | T_VAR | T_UNSET | T_ISSET | T_EMPTY | T_CONTINUE | T_GOTO
	| T_FUNCTION | T_CONST | T_RETURN | T_PRINT | T_YIELD | T_LIST | T_SWITCH | T_ENDSWITCH | T_CASE | T_DEFAULT | T_BREAK
	| T_ARRAY | T_CALLABLE | T_EXTENDS | T_IMPLEMENTS | T_NAMESPACE | T_TRAIT | T_INTERFACE | T_CLASS
	| T_CLASS_C | T_TRAIT_C | T_FUNC_C | T_METHOD_C | T_LINE | T_FILE | T_DIR | T_NS_C | T_FN | T_MATCH | T_ENUM
;

semi_reserved:
	  reserved_non_modifiers
	| T_STATIC | T_ABSTRACT | T_FINAL | T_PRIVATE | T_PROTECTED | T_PUBLIC | T_READONLY
;

ampersand:
		T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG
	|	T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG
;

identifier:
		T_STRING { $$ = $1; }
	| 	semi_reserved  {
			zval zv;
			if (zend_lex_tstring(&zv, $1) == FAILURE) { YYABORT; }
			$$ = zend_ast_create_zval(&zv);
		}
;

top_statement_list:
		top_statement_list top_statement { $$ = zend_ast_list_add($1, $2); }
	|	%empty { $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;

/* Name usable in a namespace declaration. */
namespace_declaration_name:
		identifier								{ $$ = $1; }
	|	T_NAME_QUALIFIED						{ $$ = $1; }
;

/* Name usable in "use" declarations (leading separator forbidden). */
namespace_name:
		T_STRING								{ $$ = $1; }
	|	T_NAME_QUALIFIED						{ $$ = $1; }
;

/* Name usable in "use" declarations (leading separator allowed). */
legacy_namespace_name:
		namespace_name							{ $$ = $1; }
	|	T_NAME_FULLY_QUALIFIED					{ $$ = $1; }
;

name:
		T_STRING									{ $$ = $1; $$->attr = ZEND_NAME_NOT_FQ; }
	|	T_NAME_QUALIFIED							{ $$ = $1; $$->attr = ZEND_NAME_NOT_FQ; }
	|	T_NAME_FULLY_QUALIFIED						{ $$ = $1; $$->attr = ZEND_NAME_FQ; }
	|	T_NAME_RELATIVE								{ $$ = $1; $$->attr = ZEND_NAME_RELATIVE; }
;

attribute_decl:
		class_name
			{ $$ = zend_ast_create(ZEND_AST_ATTRIBUTE, $1, NULL); }
	|	class_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_ATTRIBUTE, $1, $2); }
;

attribute_group:
		attribute_decl
			{ $$ = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_GROUP, $1); }
	|	attribute_group ',' attribute_decl
			{ $$ = zend_ast_list_add($1, $3); }
;

attribute:
		T_ATTRIBUTE attribute_group possible_comma ']'	{ $$ = $2; }
;

attributes:
		attribute				{ $$ = zend_ast_create_list(1, ZEND_AST_ATTRIBUTE_LIST, $1); }
	|	attributes attribute	{ $$ = zend_ast_list_add($1, $2); }
;

attributed_statement:
		function_declaration_statement		{ $$ = $1; }
	|	class_declaration_statement			{ $$ = $1; }
	|	trait_declaration_statement			{ $$ = $1; }
	|	interface_declaration_statement		{ $$ = $1; }
	|	enum_declaration_statement			{ $$ = $1; }
;

top_statement:
		statement							{ $$ = $1; }
	|	attributed_statement					{ $$ = $1; }
	|	attributes attributed_statement		{ $$ = zend_ast_with_attributes($2, $1); }
	|	T_HALT_COMPILER '(' ')' ';'
			{ $$ = zend_ast_create(ZEND_AST_HALT_COMPILER,
			      zend_ast_create_zval_from_long(zend_get_scanned_file_offset()));
			  zend_stop_lexing(); }
	|	T_NAMESPACE namespace_declaration_name ';'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, $2, NULL);
			  RESET_DOC_COMMENT(); }
	|	T_NAMESPACE namespace_declaration_name { RESET_DOC_COMMENT(); }
		'{' top_statement_list '}'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, $2, $5); }
	|	T_NAMESPACE { RESET_DOC_COMMENT(); }
		'{' top_statement_list '}'
			{ $$ = zend_ast_create(ZEND_AST_NAMESPACE, NULL, $4); }
	|	T_USE mixed_group_use_declaration ';'		{ $$ = $2; }
	|	T_USE use_type group_use_declaration ';'	{ $$ = $3; $$->attr = $2; }
	|	T_USE use_declarations ';'					{ $$ = $2; $$->attr = ZEND_SYMBOL_CLASS; }
	|	T_USE use_type use_declarations ';'			{ $$ = $3; $$->attr = $2; }
	|	T_CONST const_list ';'						{ $$ = $2; }
;

use_type:
	 	T_FUNCTION 		{ $$ = ZEND_SYMBOL_FUNCTION; }
	| 	T_CONST 		{ $$ = ZEND_SYMBOL_CONST; }
;

group_use_declaration:
		legacy_namespace_name T_NS_SEPARATOR '{' unprefixed_use_declarations possible_comma '}'
			{ $$ = zend_ast_create(ZEND_AST_GROUP_USE, $1, $4); }
;

mixed_group_use_declaration:
		legacy_namespace_name T_NS_SEPARATOR '{' inline_use_declarations possible_comma '}'
			{ $$ = zend_ast_create(ZEND_AST_GROUP_USE, $1, $4);}
;

possible_comma:
		%empty
	|	','
;

inline_use_declarations:
		inline_use_declarations ',' inline_use_declaration
			{ $$ = zend_ast_list_add($1, $3); }
	|	inline_use_declaration
			{ $$ = zend_ast_create_list(1, ZEND_AST_USE, $1); }
;

unprefixed_use_declarations:
		unprefixed_use_declarations ',' unprefixed_use_declaration
			{ $$ = zend_ast_list_add($1, $3); }
	|	unprefixed_use_declaration
			{ $$ = zend_ast_create_list(1, ZEND_AST_USE, $1); }
;

use_declarations:
		use_declarations ',' use_declaration
			{ $$ = zend_ast_list_add($1, $3); }
	|	use_declaration
			{ $$ = zend_ast_create_list(1, ZEND_AST_USE, $1); }
;

inline_use_declaration:
		unprefixed_use_declaration { $$ = $1; $$->attr = ZEND_SYMBOL_CLASS; }
	|	use_type unprefixed_use_declaration { $$ = $2; $$->attr = $1; }
;

unprefixed_use_declaration:
		namespace_name
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, NULL); }
	|	namespace_name T_AS T_STRING
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, $3); }
;

use_declaration:
		legacy_namespace_name
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, NULL); }
	|	legacy_namespace_name T_AS T_STRING
			{ $$ = zend_ast_create(ZEND_AST_USE_ELEM, $1, $3); }
;

const_list:
		const_list ',' const_decl { $$ = zend_ast_list_add($1, $3); }
	|	const_decl { $$ = zend_ast_create_list(1, ZEND_AST_CONST_DECL, $1); }
;

inner_statement_list:
		inner_statement_list inner_statement
			{ $$ = zend_ast_list_add($1, $2); }
	|	%empty
			{ $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


inner_statement:
		statement { $$ = $1; }
	|	attributed_statement					{ $$ = $1; }
	|	attributes attributed_statement		{ $$ = zend_ast_with_attributes($2, $1); }
	|	T_HALT_COMPILER '(' ')' ';'
			{ $$ = NULL; zend_throw_exception(zend_ce_compile_error,
			      "__HALT_COMPILER() can only be used from the outermost scope", 0); YYERROR; }
;


statement:
		'{' inner_statement_list '}' { $$ = $2; }
	|	if_stmt { $$ = $1; }
	|	alt_if_stmt { $$ = $1; }
	|	T_WHILE '(' expr ')' while_statement
			{ $$ = zend_ast_create(ZEND_AST_WHILE, $3, $5); }
	|	T_DO statement T_WHILE '(' expr ')' ';'
			{ $$ = zend_ast_create(ZEND_AST_DO_WHILE, $2, $5); }
	|	T_FOR '(' for_exprs ';' for_exprs ';' for_exprs ')' for_statement
			{ $$ = zend_ast_create(ZEND_AST_FOR, $3, $5, $7, $9); }
	|	T_SWITCH '(' expr ')' switch_case_list
			{ $$ = zend_ast_create(ZEND_AST_SWITCH, $3, $5); }
	|	T_BREAK optional_expr ';'		{ $$ = zend_ast_create(ZEND_AST_BREAK, $2); }
	|	T_CONTINUE optional_expr ';'	{ $$ = zend_ast_create(ZEND_AST_CONTINUE, $2); }
	|	T_RETURN optional_expr ';'		{ $$ = zend_ast_create(ZEND_AST_RETURN, $2); }
	|	T_GLOBAL global_var_list ';'	{ $$ = $2; }
	|	T_STATIC static_var_list ';'	{ $$ = $2; }
	|	T_ECHO echo_expr_list ';'		{ $$ = $2; }
	|	T_INLINE_HTML { $$ = zend_ast_create(ZEND_AST_ECHO, $1); }
	|	expr ';' { $$ = $1; }
	|	T_UNSET '(' unset_variables possible_comma ')' ';' { $$ = $3; }
	|	T_FOREACH '(' expr T_AS foreach_variable ')' foreach_statement
			{ $$ = zend_ast_create(ZEND_AST_FOREACH, $3, $5, NULL, $7); }
	|	T_FOREACH '(' expr T_AS foreach_variable T_DOUBLE_ARROW foreach_variable ')'
		foreach_statement
			{ $$ = zend_ast_create(ZEND_AST_FOREACH, $3, $7, $5, $9); }
	|	T_DECLARE '(' const_list ')'
			{ if (!zend_handle_encoding_declaration($3)) { YYERROR; } }
		declare_statement
			{ $$ = zend_ast_create(ZEND_AST_DECLARE, $3, $6); }
	|	';'	/* empty statement */ { $$ = NULL; }
	|	T_TRY '{' inner_statement_list '}' catch_list finally_statement
			{ $$ = zend_ast_create(ZEND_AST_TRY, $3, $5, $6); }
	|	T_GOTO T_STRING ';' { $$ = zend_ast_create(ZEND_AST_GOTO, $2); }
	|	T_STRING ':' { $$ = zend_ast_create(ZEND_AST_LABEL, $1); }
;

catch_list:
		%empty
			{ $$ = zend_ast_create_list(0, ZEND_AST_CATCH_LIST); }
	|	catch_list T_CATCH '(' catch_name_list optional_variable ')' '{' inner_statement_list '}'
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_CATCH, $4, $5, $8)); }
;

catch_name_list:
		class_name { $$ = zend_ast_create_list(1, ZEND_AST_NAME_LIST, $1); }
	|	catch_name_list '|' class_name { $$ = zend_ast_list_add($1, $3); }
;

optional_variable:
		%empty { $$ = NULL; }
	|	T_VARIABLE { $$ = $1; }
;

finally_statement:
		%empty { $$ = NULL; }
	|	T_FINALLY '{' inner_statement_list '}' { $$ = $3; }
;

unset_variables:
		unset_variable { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
	|	unset_variables ',' unset_variable { $$ = zend_ast_list_add($1, $3); }
;

unset_variable:
		variable { $$ = zend_ast_create(ZEND_AST_UNSET, $1); }
;

function_name:
		T_STRING { $$ = $1; }
	|	T_READONLY {
			zval zv;
			if (zend_lex_tstring(&zv, $1) == FAILURE) { YYABORT; }
			$$ = zend_ast_create_zval(&zv);
		}
;

function_declaration_statement:
	function returns_ref function_name backup_doc_comment '(' parameter_list ')' return_type
	backup_fn_flags '{' inner_statement_list '}' backup_fn_flags
		{ $$ = zend_ast_create_decl(ZEND_AST_FUNC_DECL, $2 | $13, $1, $4,
		      zend_ast_get_str($3), $6, NULL, $11, $8, NULL); CG(extra_fn_flags) = $9; }
;

is_reference:
		%empty	{ $$ = 0; }
	|	T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG	{ $$ = ZEND_PARAM_REF; }
;

is_variadic:
		%empty { $$ = 0; }
	|	T_ELLIPSIS  { $$ = ZEND_PARAM_VARIADIC; }
;

class_declaration_statement:
		class_modifiers T_CLASS { $<num>$ = CG(zend_lineno); }
		T_STRING extends_from implements_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, $1, $<num>3, $7, zend_ast_get_str($4), $5, $6, $9, NULL, NULL); }
	|	T_CLASS { $<num>$ = CG(zend_lineno); }
		T_STRING extends_from implements_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, 0, $<num>2, $6, zend_ast_get_str($3), $4, $5, $8, NULL, NULL); }
;

class_modifiers:
		class_modifier 					{ $$ = $1; }
	|	class_modifiers class_modifier
			{ $$ = zend_add_class_modifier($1, $2); if (!$$) { YYERROR; } }
;

anonymous_class_modifiers:
		class_modifier
			{ $$ = zend_add_anonymous_class_modifier(0, $1); if (!$$) { YYERROR; } }
	|	anonymous_class_modifiers class_modifier
			{ $$ = zend_add_anonymous_class_modifier($1, $2); if (!$$) { YYERROR; } }
;

anonymous_class_modifiers_optional:
		%empty				{ $$ = 0; }
	|	anonymous_class_modifiers	{ $$ = $1; }
;

class_modifier:
		T_ABSTRACT 		{ $$ = ZEND_ACC_EXPLICIT_ABSTRACT_CLASS; }
	|	T_FINAL 		{ $$ = ZEND_ACC_FINAL; }
	|	T_READONLY 		{ $$ = ZEND_ACC_READONLY_CLASS|ZEND_ACC_NO_DYNAMIC_PROPERTIES; }
;

trait_declaration_statement:
		T_TRAIT { $<num>$ = CG(zend_lineno); }
		T_STRING backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_TRAIT, $<num>2, $4, zend_ast_get_str($3), NULL, NULL, $6, NULL, NULL); }
;

interface_declaration_statement:
		T_INTERFACE { $<num>$ = CG(zend_lineno); }
		T_STRING interface_extends_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_INTERFACE, $<num>2, $5, zend_ast_get_str($3), NULL, $4, $7, NULL, NULL); }
;

enum_declaration_statement:
		T_ENUM { $<num>$ = CG(zend_lineno); }
		T_STRING enum_backing_type implements_list backup_doc_comment '{' class_statement_list '}'
			{ $$ = zend_ast_create_decl(ZEND_AST_CLASS, ZEND_ACC_ENUM|ZEND_ACC_FINAL, $<num>2, $6, zend_ast_get_str($3), NULL, $5, $8, NULL, $4); }
;

enum_backing_type:
		%empty	{ $$ = NULL; }
	|	':' type_expr { $$ = $2; }
;

enum_case:
		T_CASE backup_doc_comment identifier enum_case_expr ';'
			{ $$ = zend_ast_create(ZEND_AST_ENUM_CASE, $3, $4, ($2 ? zend_ast_create_zval_from_str($2) : NULL), NULL); }
;

enum_case_expr:
		%empty	{ $$ = NULL; }
	|	'=' expr { $$ = $2; }
;

extends_from:
		%empty				{ $$ = NULL; }
	|	T_EXTENDS class_name	{ $$ = $2; }
;

interface_extends_list:
		%empty			        { $$ = NULL; }
	|	T_EXTENDS class_name_list	{ $$ = $2; }
;

implements_list:
		%empty		        		{ $$ = NULL; }
	|	T_IMPLEMENTS class_name_list	{ $$ = $2; }
;

foreach_variable:
		variable			{ $$ = $1; }
	|	ampersand variable	{ $$ = zend_ast_create(ZEND_AST_REF, $2); }
	|	T_LIST '(' array_pair_list ')' { $$ = $3; $$->attr = ZEND_ARRAY_SYNTAX_LIST; }
	|	'[' array_pair_list ']' { $$ = $2; $$->attr = ZEND_ARRAY_SYNTAX_SHORT; }
;

for_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDFOR ';' { $$ = $2; }
;

foreach_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDFOREACH ';' { $$ = $2; }
;

declare_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDDECLARE ';' { $$ = $2; }
;

switch_case_list:
		'{' case_list '}'					{ $$ = $2; }
	|	'{' ';' case_list '}'				{ $$ = $3; }
	|	':' case_list T_ENDSWITCH ';'		{ $$ = $2; }
	|	':' ';' case_list T_ENDSWITCH ';'	{ $$ = $3; }
;

case_list:
		%empty { $$ = zend_ast_create_list(0, ZEND_AST_SWITCH_LIST); }
	|	case_list T_CASE expr case_separator inner_statement_list
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_SWITCH_CASE, $3, $5)); }
	|	case_list T_DEFAULT case_separator inner_statement_list
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_SWITCH_CASE, NULL, $4)); }
;

case_separator:
		':'
	|	';'
;


match:
		T_MATCH '(' expr ')' '{' match_arm_list '}'
			{ $$ = zend_ast_create(ZEND_AST_MATCH, $3, $6); };
;

match_arm_list:
		%empty { $$ = zend_ast_create_list(0, ZEND_AST_MATCH_ARM_LIST); }
	|	non_empty_match_arm_list possible_comma { $$ = $1; }
;

non_empty_match_arm_list:
		match_arm { $$ = zend_ast_create_list(1, ZEND_AST_MATCH_ARM_LIST, $1); }
	|	non_empty_match_arm_list ',' match_arm { $$ = zend_ast_list_add($1, $3); }
;

match_arm:
		match_arm_cond_list possible_comma T_DOUBLE_ARROW match_arm_body
			{ $$ = zend_ast_create(ZEND_AST_MATCH_ARM, $1, $4); }
	|	T_DEFAULT possible_comma T_DOUBLE_ARROW match_arm_body
			{ $$ = zend_ast_create(ZEND_AST_MATCH_ARM, NULL, $4); }
;

match_arm_cond_list:
		expr { $$ = zend_ast_create_list(1, ZEND_AST_EXPR_LIST, $1); }
	|	match_arm_cond_list ',' expr { $$ = zend_ast_list_add($1, $3); }
;

match_arm_body:
		expr { $$ = $1; }
	|	block_expr { $$ = $1; }
;

block_expr:
		'{' inner_statement_list optional_expr '}' { $$ = zend_ast_create(ZEND_AST_BLOCK_EXPR, $2, $3); }
;

while_statement:
		statement { $$ = $1; }
	|	':' inner_statement_list T_ENDWHILE ';' { $$ = $2; }
;


if_stmt_without_else:
		T_IF '(' expr ')' statement
			{ $$ = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create(ZEND_AST_IF_ELEM, $3, $5)); }
	|	if_stmt_without_else T_ELSEIF '(' expr ')' statement
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, $4, $6)); }
;

if_stmt:
		if_stmt_without_else %prec T_NOELSE { $$ = $1; }
	|	if_stmt_without_else T_ELSE statement
			{ $$ = zend_ast_list_add($1, zend_ast_create(ZEND_AST_IF_ELEM, NULL, $3)); }
;

alt_if_stmt_without_else:
		T_IF '(' expr ')' ':' inner_statement_list
			{ $$ = zend_ast_create_list(1, ZEND_AST_IF,
			      zend_ast_create(ZEND_AST_IF_ELEM, $3, $6)); }
	|	alt_if_stmt_without_else T_ELSEIF '(' expr ')' ':' inner_statement_list
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, $4, $7)); }
;

alt_if_stmt:
		alt_if_stmt_without_else T_ENDIF ';' { $$ = $1; }
	|	alt_if_stmt_without_else T_ELSE ':' inner_statement_list T_ENDIF ';'
			{ $$ = zend_ast_list_add($1,
			      zend_ast_create(ZEND_AST_IF_ELEM, NULL, $4)); }
;

parameter_list:
		non_empty_parameter_list possible_comma { $$ = $1; }
	|	%empty	{ $$ = zend_ast_create_list(0, ZEND_AST_PARAM_LIST); }
;


non_empty_parameter_list:
		attributed_parameter
			{ $$ = zend_ast_create_list(1, ZEND_AST_PARAM_LIST, $1); }
	|	non_empty_parameter_list ',' attributed_parameter
			{ $$ = zend_ast_list_add($1, $3); }
;

attributed_parameter:
		attributes parameter	{ $$ = zend_ast_with_attributes($2, $1); }
	|	parameter				{ $$ = $1; }
;

optional_cpp_modifiers:
		%empty
			{ $$ = 0; }
	|	non_empty_member_modifiers
			{ $$ = zend_modifier_list_to_flags(ZEND_MODIFIER_TARGET_CPP, $1);
			  if (!$$) { YYERROR; } }
;

parameter:
		optional_cpp_modifiers optional_type_without_static
		is_reference is_variadic T_VARIABLE backup_doc_comment
			{ $$ = zend_ast_create_ex(ZEND_AST_PARAM, $1 | $3 | $4, $2, $5, NULL,
					NULL, $6 ? zend_ast_create_zval_from_str($6) : NULL); }
	|	optional_cpp_modifiers optional_type_without_static
		is_reference is_variadic T_VARIABLE backup_doc_comment '=' expr
			{ $$ = zend_ast_create_ex(ZEND_AST_PARAM, $1 | $3 | $4, $2, $5, $8,
					NULL, $6 ? zend_ast_create_zval_from_str($6) : NULL); }
;

optional_type_without_static:
		%empty	{ $$ = NULL; }
	|	type_expr_without_static	{ $$ = $1; }
;

type_expr:
		type				{ $$ = $1; }
	|	'?' type			{ $$ = $2; $$->attr |= ZEND_TYPE_NULLABLE; }
	|	union_type			{ $$ = $1; }
	|	intersection_type	{ $$ = $1; }
;

type:
		type_without_static	{ $$ = $1; }
	|	T_STATIC			{ $$ = zend_ast_create_ex(ZEND_AST_TYPE, IS_STATIC); }
;

union_type_element:
                type { $$ = $1; }
        |        '(' intersection_type ')' { $$ = $2; }
;

union_type:
		union_type_element '|' union_type_element
			{ $$ = zend_ast_create_list(2, ZEND_AST_TYPE_UNION, $1, $3); }
	|	union_type '|' union_type_element
			{ $$ = zend_ast_list_add($1, $3); }
;

intersection_type:
		type T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG type       { $$ = zend_ast_create_list(2, ZEND_AST_TYPE_INTERSECTION, $1, $3); }
	|	intersection_type T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG type { $$ = zend_ast_list_add($1, $3); }
;

/* Duplicate the type rules without "static",
 * to avoid conflicts with "static" modifier for properties. */

type_expr_without_static:
		type_without_static			{ $$ = $1; }
	|	'?' type_without_static		{ $$ = $2; $$->attr |= ZEND_TYPE_NULLABLE; }
	|	union_type_without_static	{ $$ = $1; }
	|	intersection_type_without_static	{ $$ = $1; }
;

type_without_static:
		T_ARRAY		{ $$ = zend_ast_create_ex(ZEND_AST_TYPE, IS_ARRAY); }
	|	T_CALLABLE	{ $$ = zend_ast_create_ex(ZEND_AST_TYPE, IS_CALLABLE); }
	|	name		{ $$ = $1; }
;

union_type_without_static_element:
                type_without_static { $$ = $1; }
        |        '(' intersection_type_without_static ')' { $$ = $2; }
;

union_type_without_static:
		union_type_without_static_element '|' union_type_without_static_element
			{ $$ = zend_ast_create_list(2, ZEND_AST_TYPE_UNION, $1, $3); }
	|	union_type_without_static '|' union_type_without_static_element
			{ $$ = zend_ast_list_add($1, $3); }
;

intersection_type_without_static:
		type_without_static T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG type_without_static
			{ $$ = zend_ast_create_list(2, ZEND_AST_TYPE_INTERSECTION, $1, $3); }
	|	intersection_type_without_static T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG type_without_static
			{ $$ = zend_ast_list_add($1, $3); }
;

return_type:
		%empty	{ $$ = NULL; }
	|	':' type_expr	{ $$ = $2; }
;

argument_list:
		'(' ')'	{ $$ = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	'(' non_empty_argument_list possible_comma ')' { $$ = $2; }
	|	'(' T_ELLIPSIS ')' { $$ = zend_ast_create(ZEND_AST_CALLABLE_CONVERT); }
;

non_empty_argument_list:
		argument
			{ $$ = zend_ast_create_list(1, ZEND_AST_ARG_LIST, $1); }
	|	non_empty_argument_list ',' argument
			{ $$ = zend_ast_list_add($1, $3); }
;

argument:
		expr				{ $$ = $1; }
	|	identifier ':' expr
			{ $$ = zend_ast_create(ZEND_AST_NAMED_ARG, $1, $3); }
	|	T_ELLIPSIS expr	{ $$ = zend_ast_create(ZEND_AST_UNPACK, $2); }
;

global_var_list:
		global_var_list ',' global_var { $$ = zend_ast_list_add($1, $3); }
	|	global_var { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;

global_var:
	simple_variable
		{ $$ = zend_ast_create(ZEND_AST_GLOBAL, zend_ast_create(ZEND_AST_VAR, $1)); }
;


static_var_list:
		static_var_list ',' static_var { $$ = zend_ast_list_add($1, $3); }
	|	static_var { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;

static_var:
		T_VARIABLE			{ $$ = zend_ast_create(ZEND_AST_STATIC, $1, NULL); }
	|	T_VARIABLE '=' expr	{ $$ = zend_ast_create(ZEND_AST_STATIC, $1, $3); }
;

class_statement_list:
		class_statement_list class_statement
			{ $$ = zend_ast_list_add($1, $2); }
	|	%empty
			{ $$ = zend_ast_create_list(0, ZEND_AST_STMT_LIST); }
;


attributed_class_statement:
		property_modifiers optional_type_without_static property_list ';'
			{ $$ = zend_ast_create(ZEND_AST_PROP_GROUP, $2, $3, NULL);
			  $$->attr = $1; }
	|	class_const_modifiers T_CONST class_const_list ';'
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST_GROUP, $3, NULL, NULL);
			  $$->attr = $1; }
	|	class_const_modifiers T_CONST type_expr class_const_list ';'
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST_GROUP, $4, NULL, $3);
			  $$->attr = $1; }
	|	method_modifiers function returns_ref identifier backup_doc_comment '(' parameter_list ')'
		return_type backup_fn_flags method_body backup_fn_flags
			{ $$ = zend_ast_create_decl(ZEND_AST_METHOD, $3 | $1 | $12, $2, $5,
				  zend_ast_get_str($4), $7, NULL, $11, $9, NULL); CG(extra_fn_flags) = $10; }
	|	enum_case { $$ = $1; }
;

class_statement:
		attributed_class_statement { $$ = $1; }
	|	attributes attributed_class_statement { $$ = zend_ast_with_attributes($2, $1); }
	|	T_USE class_name_list trait_adaptations
			{ $$ = zend_ast_create(ZEND_AST_USE_TRAIT, $2, $3); }
;

class_name_list:
		class_name { $$ = zend_ast_create_list(1, ZEND_AST_NAME_LIST, $1); }
	|	class_name_list ',' class_name { $$ = zend_ast_list_add($1, $3); }
;

trait_adaptations:
		';'								{ $$ = NULL; }
	|	'{' '}'							{ $$ = NULL; }
	|	'{' trait_adaptation_list '}'	{ $$ = $2; }
;

trait_adaptation_list:
		trait_adaptation
			{ $$ = zend_ast_create_list(1, ZEND_AST_TRAIT_ADAPTATIONS, $1); }
	|	trait_adaptation_list trait_adaptation
			{ $$ = zend_ast_list_add($1, $2); }
;

trait_adaptation:
		trait_precedence ';'	{ $$ = $1; }
	|	trait_alias ';'			{ $$ = $1; }
;

trait_precedence:
	absolute_trait_method_reference T_INSTEADOF class_name_list
		{ $$ = zend_ast_create(ZEND_AST_TRAIT_PRECEDENCE, $1, $3); }
;

trait_alias:
		trait_method_reference T_AS T_STRING
			{ $$ = zend_ast_create(ZEND_AST_TRAIT_ALIAS, $1, $3); }
	|	trait_method_reference T_AS reserved_non_modifiers
			{ zval zv;
			  if (zend_lex_tstring(&zv, $3) == FAILURE) { YYABORT; }
			  $$ = zend_ast_create(ZEND_AST_TRAIT_ALIAS, $1, zend_ast_create_zval(&zv)); }
	|	trait_method_reference T_AS member_modifier identifier
			{ uint32_t modifiers = zend_modifier_token_to_flag(ZEND_MODIFIER_TARGET_METHOD, $3);
			  $$ = zend_ast_create_ex(ZEND_AST_TRAIT_ALIAS, modifiers, $1, $4);
			  /* identifier nonterminal can cause allocations, so we need to free the node */
			  if (!modifiers) { zend_ast_destroy($$); YYERROR; } }
	|	trait_method_reference T_AS member_modifier
			{ uint32_t modifiers = zend_modifier_token_to_flag(ZEND_MODIFIER_TARGET_METHOD, $3);
			  $$ = zend_ast_create_ex(ZEND_AST_TRAIT_ALIAS, modifiers, $1, NULL);
			  /* identifier nonterminal can cause allocations, so we need to free the node */
			  if (!modifiers) { zend_ast_destroy($$); YYERROR; } }
;

trait_method_reference:
		identifier
			{ $$ = zend_ast_create(ZEND_AST_METHOD_REFERENCE, NULL, $1); }
	|	absolute_trait_method_reference { $$ = $1; }
;

absolute_trait_method_reference:
	class_name T_PAAMAYIM_NEKUDOTAYIM identifier
		{ $$ = zend_ast_create(ZEND_AST_METHOD_REFERENCE, $1, $3); }
;

method_body:
		';' /* abstract method */		{ $$ = NULL; }
	|	'{' inner_statement_list '}'	{ $$ = $2; }
;

property_modifiers:
		non_empty_member_modifiers
			{ $$ = zend_modifier_list_to_flags(ZEND_MODIFIER_TARGET_PROPERTY, $1);
			  if (!$$) { YYERROR; } }
	|	T_VAR
			{ $$ = ZEND_ACC_PUBLIC; }
;

method_modifiers:
		%empty
			{ $$ = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers
			{ $$ = zend_modifier_list_to_flags(ZEND_MODIFIER_TARGET_METHOD, $1);
			  if (!$$) { YYERROR; }
			  if (!($$ & ZEND_ACC_PPP_MASK)) { $$ |= ZEND_ACC_PUBLIC; } }
;

class_const_modifiers:
		%empty
			{ $$ = ZEND_ACC_PUBLIC; }
	|	non_empty_member_modifiers
			{ $$ = zend_modifier_list_to_flags(ZEND_MODIFIER_TARGET_CONSTANT, $1);
			  if (!$$) { YYERROR; }
			  if (!($$ & ZEND_ACC_PPP_MASK)) { $$ |= ZEND_ACC_PUBLIC; } }
;

non_empty_member_modifiers:
		member_modifier
			{ $$ = zend_ast_create_list(1, ZEND_AST_MODIFIER_LIST, zend_ast_create_zval_from_long($1)); }
	|	non_empty_member_modifiers member_modifier
			{ $$ = zend_ast_list_add($1, zend_ast_create_zval_from_long($2)); }
;

member_modifier:
		T_PUBLIC				{ $$ = T_PUBLIC; }
	|	T_PROTECTED				{ $$ = T_PROTECTED; }
	|	T_PRIVATE				{ $$ = T_PRIVATE; }
	|	T_STATIC				{ $$ = T_STATIC; }
	|	T_ABSTRACT				{ $$ = T_ABSTRACT; }
	|	T_FINAL					{ $$ = T_FINAL; }
	|	T_READONLY				{ $$ = T_READONLY; }
;

property_list:
		property_list ',' property { $$ = zend_ast_list_add($1, $3); }
	|	property { $$ = zend_ast_create_list(1, ZEND_AST_PROP_DECL, $1); }
;

property:
		T_VARIABLE backup_doc_comment
			{ $$ = zend_ast_create(ZEND_AST_PROP_ELEM, $1, NULL, ($2 ? zend_ast_create_zval_from_str($2) : NULL)); }
	|	T_VARIABLE '=' expr backup_doc_comment
			{ $$ = zend_ast_create(ZEND_AST_PROP_ELEM, $1, $3, ($4 ? zend_ast_create_zval_from_str($4) : NULL)); }
;

class_const_list:
		class_const_list ',' class_const_decl { $$ = zend_ast_list_add($1, $3); }
	|	class_const_decl { $$ = zend_ast_create_list(1, ZEND_AST_CLASS_CONST_DECL, $1); }
;

class_const_decl:
		T_STRING '=' expr backup_doc_comment { $$ = zend_ast_create(ZEND_AST_CONST_ELEM, $1, $3, ($4 ? zend_ast_create_zval_from_str($4) : NULL)); }
	|	semi_reserved '=' expr backup_doc_comment {
			zval zv;
			if (zend_lex_tstring(&zv, $1) == FAILURE) { YYABORT; }
			$$ = zend_ast_create(ZEND_AST_CONST_ELEM, zend_ast_create_zval(&zv), $3, ($4 ? zend_ast_create_zval_from_str($4) : NULL));
		}
;

const_decl:
	T_STRING '=' expr backup_doc_comment { $$ = zend_ast_create(ZEND_AST_CONST_ELEM, $1, $3, ($4 ? zend_ast_create_zval_from_str($4) : NULL)); }
;

echo_expr_list:
		echo_expr_list ',' echo_expr { $$ = zend_ast_list_add($1, $3); }
	|	echo_expr { $$ = zend_ast_create_list(1, ZEND_AST_STMT_LIST, $1); }
;
echo_expr:
	expr { $$ = zend_ast_create(ZEND_AST_ECHO, $1); }
;

for_exprs:
		%empty			{ $$ = NULL; }
	|	non_empty_for_exprs	{ $$ = $1; }
;

non_empty_for_exprs:
		non_empty_for_exprs ',' expr { $$ = zend_ast_list_add($1, $3); }
	|	expr { $$ = zend_ast_create_list(1, ZEND_AST_EXPR_LIST, $1); }
;

anonymous_class:
		anonymous_class_modifiers_optional T_CLASS { $<num>$ = CG(zend_lineno); } ctor_arguments
		extends_from implements_list backup_doc_comment '{' class_statement_list '}' {
			zend_ast *decl = zend_ast_create_decl(
				ZEND_AST_CLASS, ZEND_ACC_ANON_CLASS | $1, $<num>3, $7, NULL,
				$5, $6, $9, NULL, NULL);
			$$ = zend_ast_create(ZEND_AST_NEW, decl, $4);
		}
;

new_expr:
		T_NEW class_name_reference ctor_arguments
			{ $$ = zend_ast_create(ZEND_AST_NEW, $2, $3); }
	|	T_NEW anonymous_class
			{ $$ = $2; }
	|	T_NEW attributes anonymous_class
			{ zend_ast_with_attributes($3->child[0], $2); $$ = $3; }
;

expr:
		variable
			{ $$ = $1; }
	|	T_LIST '(' array_pair_list ')' '=' expr
			{ $3->attr = ZEND_ARRAY_SYNTAX_LIST; $$ = zend_ast_create(ZEND_AST_ASSIGN, $3, $6); }
	|	'[' array_pair_list ']' '=' expr
			{ $2->attr = ZEND_ARRAY_SYNTAX_SHORT; $$ = zend_ast_create(ZEND_AST_ASSIGN, $2, $5); }
	|	variable '=' expr
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN, $1, $3); }
	|	variable '=' ampersand variable
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN_REF, $1, $4); }
	|	T_CLONE expr { $$ = zend_ast_create(ZEND_AST_CLONE, $2); }
	|	variable T_PLUS_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_ADD, $1, $3); }
	|	variable T_MINUS_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_SUB, $1, $3); }
	|	variable T_MUL_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_MUL, $1, $3); }
	|	variable T_POW_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_POW, $1, $3); }
	|	variable T_DIV_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_DIV, $1, $3); }
	|	variable T_CONCAT_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_CONCAT, $1, $3); }
	|	variable T_MOD_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_MOD, $1, $3); }
	|	variable T_AND_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_BW_AND, $1, $3); }
	|	variable T_OR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_BW_OR, $1, $3); }
	|	variable T_XOR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_BW_XOR, $1, $3); }
	|	variable T_SL_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_SL, $1, $3); }
	|	variable T_SR_EQUAL expr
			{ $$ = zend_ast_create_assign_op(ZEND_SR, $1, $3); }
	|	variable T_COALESCE_EQUAL expr
			{ $$ = zend_ast_create(ZEND_AST_ASSIGN_COALESCE, $1, $3); }
	|	variable T_INC { $$ = zend_ast_create(ZEND_AST_POST_INC, $1); }
	|	T_INC variable { $$ = zend_ast_create(ZEND_AST_PRE_INC, $2); }
	|	variable T_DEC { $$ = zend_ast_create(ZEND_AST_POST_DEC, $1); }
	|	T_DEC variable { $$ = zend_ast_create(ZEND_AST_PRE_DEC, $2); }
	|	expr T_BOOLEAN_OR expr
			{ $$ = zend_ast_create(ZEND_AST_OR, $1, $3); }
	|	expr T_BOOLEAN_AND expr
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
	|	expr T_LOGICAL_OR expr
			{ $$ = zend_ast_create(ZEND_AST_OR, $1, $3); }
	|	expr T_LOGICAL_AND expr
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
	|	expr T_LOGICAL_XOR expr
			{ $$ = zend_ast_create_binary_op(ZEND_BOOL_XOR, $1, $3); }
	|	expr '|' expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_OR, $1, $3); }
	|	expr T_AMPERSAND_NOT_FOLLOWED_BY_VAR_OR_VARARG expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_AND, $1, $3); }
	|	expr T_AMPERSAND_FOLLOWED_BY_VAR_OR_VARARG expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_AND, $1, $3); }
	|	expr '^' expr	{ $$ = zend_ast_create_binary_op(ZEND_BW_XOR, $1, $3); }
	|	expr '.' expr 	{ $$ = zend_ast_create_concat_op($1, $3); }
	|	expr '+' expr 	{ $$ = zend_ast_create_binary_op(ZEND_ADD, $1, $3); }
	|	expr '-' expr 	{ $$ = zend_ast_create_binary_op(ZEND_SUB, $1, $3); }
	|	expr '*' expr	{ $$ = zend_ast_create_binary_op(ZEND_MUL, $1, $3); }
	|	expr T_POW expr	{ $$ = zend_ast_create_binary_op(ZEND_POW, $1, $3); }
	|	expr '/' expr	{ $$ = zend_ast_create_binary_op(ZEND_DIV, $1, $3); }
	|	expr '%' expr 	{ $$ = zend_ast_create_binary_op(ZEND_MOD, $1, $3); }
	| 	expr T_SL expr	{ $$ = zend_ast_create_binary_op(ZEND_SL, $1, $3); }
	|	expr T_SR expr	{ $$ = zend_ast_create_binary_op(ZEND_SR, $1, $3); }
	|	'+' expr %prec '~' { $$ = zend_ast_create(ZEND_AST_UNARY_PLUS, $2); }
	|	'-' expr %prec '~' { $$ = zend_ast_create(ZEND_AST_UNARY_MINUS, $2); }
	|	'!' expr { $$ = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BOOL_NOT, $2); }
	|	'~' expr { $$ = zend_ast_create_ex(ZEND_AST_UNARY_OP, ZEND_BW_NOT, $2); }
	|	expr T_IS_IDENTICAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_IDENTICAL, $1, $3); }
	|	expr T_IS_NOT_IDENTICAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_NOT_IDENTICAL, $1, $3); }
	|	expr T_IS_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_EQUAL, $1, $3); }
	|	expr T_IS_NOT_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_NOT_EQUAL, $1, $3); }
	|	expr '<' expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1, $3); }
	/* BC for $foo<-$bar */
	|	expr T_THIN_ARROW_LEFT expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_SMALLER, $1, zend_ast_create(ZEND_AST_UNARY_MINUS, $3)); }
	|	expr T_IS_SMALLER_OR_EQUAL expr
			{ $$ = zend_ast_create_binary_op(ZEND_IS_SMALLER_OR_EQUAL, $1, $3); }
	|	expr '>' expr
			{ $$ = zend_ast_create(ZEND_AST_GREATER, $1, $3); }
	|	expr T_IS_GREATER_OR_EQUAL expr
			{ $$ = zend_ast_create(ZEND_AST_GREATER_EQUAL, $1, $3); }
	|	expr T_SPACESHIP expr
			{ $$ = zend_ast_create_binary_op(ZEND_SPACESHIP, $1, $3); }
	|	expr T_INSTANCEOF class_name_reference
			{ $$ = zend_ast_create(ZEND_AST_INSTANCEOF, $1, $3); }
	|	'(' expr ')' {
			$$ = $2;
			if ($$->kind == ZEND_AST_CONDITIONAL) $$->attr = ZEND_PARENTHESIZED_CONDITIONAL;
		}
	|	new_expr { $$ = $1; }
	|	expr '?' expr ':' expr
			{ $$ = zend_ast_create(ZEND_AST_CONDITIONAL, $1, $3, $5); }
	|	expr '?' ':' expr
			{ $$ = zend_ast_create(ZEND_AST_CONDITIONAL, $1, NULL, $4); }
	|	expr T_COALESCE expr
			{ $$ = zend_ast_create(ZEND_AST_COALESCE, $1, $3); }
	|	internal_functions_in_yacc { $$ = $1; }
	|	T_INT_CAST expr		{ $$ = zend_ast_create_cast(IS_LONG, $2); }
	|	T_DOUBLE_CAST expr	{ $$ = zend_ast_create_cast(IS_DOUBLE, $2); }
	|	T_STRING_CAST expr	{ $$ = zend_ast_create_cast(IS_STRING, $2); }
	|	T_ARRAY_CAST expr	{ $$ = zend_ast_create_cast(IS_ARRAY, $2); }
	|	T_OBJECT_CAST expr	{ $$ = zend_ast_create_cast(IS_OBJECT, $2); }
	|	T_BOOL_CAST expr	{ $$ = zend_ast_create_cast(_IS_BOOL, $2); }
	|	T_UNSET_CAST expr	{ $$ = zend_ast_create_cast(IS_NULL, $2); }
	|	T_EXIT exit_expr	{ $$ = zend_ast_create(ZEND_AST_EXIT, $2); }
	|	'@' expr			{ $$ = zend_ast_create(ZEND_AST_SILENCE, $2); }
	|	scalar { $$ = $1; }
	|	'`' backticks_expr '`' { $$ = zend_ast_create(ZEND_AST_SHELL_EXEC, $2); }
	|	T_PRINT expr { $$ = zend_ast_create(ZEND_AST_PRINT, $2); }
	|	T_YIELD { $$ = zend_ast_create(ZEND_AST_YIELD, NULL, NULL); CG(extra_fn_flags) |= ZEND_ACC_GENERATOR; }
	|	T_YIELD expr { $$ = zend_ast_create(ZEND_AST_YIELD, $2, NULL); CG(extra_fn_flags) |= ZEND_ACC_GENERATOR; }
	|	T_YIELD expr T_DOUBLE_ARROW expr { $$ = zend_ast_create(ZEND_AST_YIELD, $4, $2); CG(extra_fn_flags) |= ZEND_ACC_GENERATOR; }
	|	T_YIELD_FROM expr { $$ = zend_ast_create(ZEND_AST_YIELD_FROM, $2); CG(extra_fn_flags) |= ZEND_ACC_GENERATOR; }
	|	T_THROW expr { $$ = zend_ast_create(ZEND_AST_THROW, $2); }
	|	inline_function { $$ = $1; }
	|	attributes inline_function { $$ = zend_ast_with_attributes($2, $1); }
	|	T_STATIC inline_function { $$ = $2; ((zend_ast_decl *) $$)->flags |= ZEND_ACC_STATIC; }
	|	attributes T_STATIC inline_function
			{ $$ = zend_ast_with_attributes($3, $1); ((zend_ast_decl *) $$)->flags |= ZEND_ACC_STATIC; }
	|	match { $$ = $1; }
;


inline_function:
		function returns_ref backup_doc_comment '(' parameter_list ')' lexical_vars return_type
		backup_fn_flags '{' inner_statement_list '}' backup_fn_flags
			{ $$ = zend_ast_create_decl(ZEND_AST_CLOSURE, $2 | $13, $1, $3,
				  ZSTR_INIT_LITERAL("{closure}", 0),
				  $5, $7, $11, $8, NULL); CG(extra_fn_flags) = $9; }
	|	fn returns_ref backup_doc_comment '(' parameter_list ')' return_type
		T_DOUBLE_ARROW backup_fn_flags backup_lex_pos expr backup_fn_flags
			{ $$ = zend_ast_create_decl(ZEND_AST_ARROW_FUNC, $2 | $12, $1, $3,
				  ZSTR_INIT_LITERAL("{closure}", 0), $5, NULL, $11, $7, NULL);
				  CG(extra_fn_flags) = $9; }
;

fn:
	T_FN { $$ = CG(zend_lineno); }
;

function:
	T_FUNCTION { $$ = CG(zend_lineno); }
;

backup_doc_comment:
	%empty { $$ = CG(doc_comment); CG(doc_comment) = NULL; }
;

backup_fn_flags:
	%prec PREC_ARROW_FUNCTION %empty { $$ = CG(extra_fn_flags); CG(extra_fn_flags) = 0; }
;

backup_lex_pos:
	%empty { $$ = LANG_SCNG(yy_text); }
;

returns_ref:
		%empty	{ $$ = 0; }
	|	ampersand	{ $$ = ZEND_ACC_RETURN_REFERENCE; }
;

lexical_vars:
		%empty { $$ = NULL; }
	|	T_USE '(' lexical_var_list possible_comma ')' { $$ = $3; }
;

lexical_var_list:
		lexical_var_list ',' lexical_var { $$ = zend_ast_list_add($1, $3); }
	|	lexical_var { $$ = zend_ast_create_list(1, ZEND_AST_CLOSURE_USES, $1); }
;

lexical_var:
		T_VARIABLE		{ $$ = $1; }
	|	ampersand T_VARIABLE	{ $$ = $2; $$->attr = ZEND_BIND_REF; }
;

function_call:
		name argument_list
			{ $$ = zend_ast_create(ZEND_AST_CALL, $1, $2); }
	|	T_READONLY argument_list {
			zval zv;
			if (zend_lex_tstring(&zv, $1) == FAILURE) { YYABORT; }
			$$ = zend_ast_create(ZEND_AST_CALL, zend_ast_create_zval(&zv), $2);
		}
	|	class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_STATIC_CALL, $1, $3, $4); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM member_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_STATIC_CALL, $1, $3, $4); }
	|	callable_expr { $<num>$ = CG(zend_lineno); } argument_list {
			$$ = zend_ast_create(ZEND_AST_CALL, $1, $3);
			$$->lineno = $<num>2;
		}
;

class_name:
		T_STATIC
			{ zval zv; ZVAL_INTERNED_STR(&zv, ZSTR_KNOWN(ZEND_STR_STATIC));
			  $$ = zend_ast_create_zval_ex(&zv, ZEND_NAME_NOT_FQ); }
	|	name { $$ = $1; }
;

class_name_reference:
		class_name		{ $$ = $1; }
	|	new_variable	{ $$ = $1; }
	|	'(' expr ')'	{ $$ = $2; }
;

exit_expr:
		%empty				{ $$ = NULL; }
	|	'(' optional_expr ')'	{ $$ = $2; }
;

backticks_expr:
		%empty
			{ $$ = zend_ast_create_zval_from_str(ZSTR_EMPTY_ALLOC()); }
	|	T_ENCAPSED_AND_WHITESPACE { $$ = $1; }
	|	encaps_list { $$ = $1; }
;


ctor_arguments:
		%empty	{ $$ = zend_ast_create_list(0, ZEND_AST_ARG_LIST); }
	|	argument_list { $$ = $1; }
;


dereferenceable_scalar:
		T_ARRAY '(' array_pair_list ')'	{ $$ = $3; $$->attr = ZEND_ARRAY_SYNTAX_LONG; }
	|	'[' array_pair_list ']'			{ $$ = $2; $$->attr = ZEND_ARRAY_SYNTAX_SHORT; }
	|	T_CONSTANT_ENCAPSED_STRING		{ $$ = $1; }
	|	'"' encaps_list '"'			 	{ $$ = $2; }
;

scalar:
		T_LNUMBER 	{ $$ = $1; }
	|	T_DNUMBER 	{ $$ = $1; }
	|	T_START_HEREDOC T_ENCAPSED_AND_WHITESPACE T_END_HEREDOC { $$ = $2; }
	|	T_START_HEREDOC T_END_HEREDOC
			{ $$ = zend_ast_create_zval_from_str(ZSTR_EMPTY_ALLOC()); }
	|	T_START_HEREDOC encaps_list T_END_HEREDOC { $$ = $2; }
	|	dereferenceable_scalar	{ $$ = $1; }
	|	constant				{ $$ = $1; }
	|	class_constant			{ $$ = $1; }
;

constant:
		name		{ $$ = zend_ast_create(ZEND_AST_CONST, $1); }
	|	T_LINE 		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_LINE); }
	|	T_FILE 		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_FILE); }
	|	T_DIR   	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_DIR); }
	|	T_TRAIT_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_TRAIT_C); }
	|	T_METHOD_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_METHOD_C); }
	|	T_FUNC_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_FUNC_C); }
	|	T_NS_C		{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_NS_C); }
	|	T_CLASS_C	{ $$ = zend_ast_create_ex(ZEND_AST_MAGIC_CONST, T_CLASS_C); }
;

class_constant:
		class_name T_PAAMAYIM_NEKUDOTAYIM identifier
			{ $$ = zend_ast_create_class_const_or_name($1, $3); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM identifier
			{ $$ = zend_ast_create_class_const_or_name($1, $3); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM '{' expr '}'
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST, $1, $4); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM '{' expr '}'
			{ $$ = zend_ast_create(ZEND_AST_CLASS_CONST, $1, $4); }
;

optional_expr:
		%empty	{ $$ = NULL; }
	|	expr		{ $$ = $1; }
;

variable_class_name:
		fully_dereferenceable { $$ = $1; }
;

fully_dereferenceable:
		variable				{ $$ = $1; }
	|	'(' expr ')'			{ $$ = $2; }
	|	dereferenceable_scalar	{ $$ = $1; }
	|	class_constant			{ $$ = $1; }
;

array_object_dereferenceable:
		fully_dereferenceable	{ $$ = $1; }
	|	constant				{ $$ = $1; }
;

callable_expr:
		callable_variable		{ $$ = $1; }
	|	'(' expr ')'			{ $$ = $2; }
	|	dereferenceable_scalar	{ $$ = $1; }
;

callable_variable:
		simple_variable
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	array_object_dereferenceable '[' optional_expr ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	array_object_dereferenceable '{' expr '}'
			{ $$ = zend_ast_create_ex(ZEND_AST_DIM, ZEND_DIM_ALTERNATIVE_SYNTAX, $1, $3); }
	|	array_object_dereferenceable T_OBJECT_OPERATOR property_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_METHOD_CALL, $1, $3, $4); }
	|	array_object_dereferenceable T_NULLSAFE_OBJECT_OPERATOR property_name argument_list
			{ $$ = zend_ast_create(ZEND_AST_NULLSAFE_METHOD_CALL, $1, $3, $4); }
	|	function_call { $$ = $1; }
;

variable:
		callable_variable
			{ $$ = $1; }
	|	static_member
			{ $$ = $1; }
	|	array_object_dereferenceable T_OBJECT_OPERATOR property_name
			{ $$ = zend_ast_create(ZEND_AST_PROP, $1, $3); }
	|	array_object_dereferenceable T_NULLSAFE_OBJECT_OPERATOR property_name
			{ $$ = zend_ast_create(ZEND_AST_NULLSAFE_PROP, $1, $3); }
;

simple_variable:
		T_VARIABLE			{ $$ = $1; }
	|	'$' '{' expr '}'	{ $$ = $3; }
	|	'$' simple_variable	{ $$ = zend_ast_create(ZEND_AST_VAR, $2); }
;

static_member:
		class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
	|	variable_class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
;

new_variable:
		simple_variable
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	new_variable '[' optional_expr ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM, $1, $3); }
	|	new_variable '{' expr '}'
			{ $$ = zend_ast_create_ex(ZEND_AST_DIM, ZEND_DIM_ALTERNATIVE_SYNTAX, $1, $3); }
	|	new_variable T_OBJECT_OPERATOR property_name
			{ $$ = zend_ast_create(ZEND_AST_PROP, $1, $3); }
	|	new_variable T_NULLSAFE_OBJECT_OPERATOR property_name
			{ $$ = zend_ast_create(ZEND_AST_NULLSAFE_PROP, $1, $3); }
	|	class_name T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
	|	new_variable T_PAAMAYIM_NEKUDOTAYIM simple_variable
			{ $$ = zend_ast_create(ZEND_AST_STATIC_PROP, $1, $3); }
;

member_name:
		identifier { $$ = $1; }
	|	'{' expr '}'	{ $$ = $2; }
	|	simple_variable	{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
;

property_name:
		T_STRING { $$ = $1; }
	|	'{' expr '}'	{ $$ = $2; }
	|	simple_variable	{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
;

array_pair_list:
		non_empty_array_pair_list
			{ /* allow single trailing comma */ $$ = zend_ast_list_rtrim($1); }
;

possible_array_pair:
		%empty { $$ = NULL; }
	|	array_pair  { $$ = $1; }
;

non_empty_array_pair_list:
		non_empty_array_pair_list ',' possible_array_pair
			{ $$ = zend_ast_list_add($1, $3); }
	|	possible_array_pair
			{ $$ = zend_ast_create_list(1, ZEND_AST_ARRAY, $1); }
;

array_pair:
		expr T_DOUBLE_ARROW expr
			{ $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $3, $1); }
	|	expr
			{ $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $1, NULL); }
	|	expr T_DOUBLE_ARROW ampersand variable
			{ $$ = zend_ast_create_ex(ZEND_AST_ARRAY_ELEM, 1, $4, $1); }
	|	ampersand variable
			{ $$ = zend_ast_create_ex(ZEND_AST_ARRAY_ELEM, 1, $2, NULL); }
	|	T_ELLIPSIS expr
			{ $$ = zend_ast_create(ZEND_AST_UNPACK, $2); }
	|	expr T_DOUBLE_ARROW T_LIST '(' array_pair_list ')'
			{ $5->attr = ZEND_ARRAY_SYNTAX_LIST;
			  $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $5, $1); }
	|	T_LIST '(' array_pair_list ')'
			{ $3->attr = ZEND_ARRAY_SYNTAX_LIST;
			  $$ = zend_ast_create(ZEND_AST_ARRAY_ELEM, $3, NULL); }
;

encaps_list:
		encaps_list encaps_var
			{ $$ = zend_ast_list_add($1, $2); }
	|	encaps_list T_ENCAPSED_AND_WHITESPACE
			{ $$ = zend_ast_list_add($1, $2); }
	|	encaps_var
			{ $$ = zend_ast_create_list(1, ZEND_AST_ENCAPS_LIST, $1); }
	|	T_ENCAPSED_AND_WHITESPACE encaps_var
			{ $$ = zend_ast_create_list(2, ZEND_AST_ENCAPS_LIST, $1, $2); }
;

encaps_var:
		T_VARIABLE
			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
	|	T_VARIABLE '[' encaps_var_offset ']'
			{ $$ = zend_ast_create(ZEND_AST_DIM,
			      zend_ast_create(ZEND_AST_VAR, $1), $3); }
	|	T_VARIABLE T_OBJECT_OPERATOR T_STRING
			{ $$ = zend_ast_create(ZEND_AST_PROP,
			      zend_ast_create(ZEND_AST_VAR, $1), $3); }
	|	T_VARIABLE T_NULLSAFE_OBJECT_OPERATOR T_STRING
			{ $$ = zend_ast_create(ZEND_AST_NULLSAFE_PROP,
			      zend_ast_create(ZEND_AST_VAR, $1), $3); }
	|	T_DOLLAR_OPEN_CURLY_BRACES expr '}'
			{ $$ = zend_ast_create_ex(ZEND_AST_VAR, ZEND_ENCAPS_VAR_DOLLAR_CURLY_VAR_VAR, $2); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '}'
			{ $$ = zend_ast_create_ex(ZEND_AST_VAR, ZEND_ENCAPS_VAR_DOLLAR_CURLY, $2); }
	|	T_DOLLAR_OPEN_CURLY_BRACES T_STRING_VARNAME '[' expr ']' '}'
			{ $$ = zend_ast_create_ex(ZEND_AST_DIM, ZEND_ENCAPS_VAR_DOLLAR_CURLY,
			      zend_ast_create(ZEND_AST_VAR, $2), $4); }
	|	T_CURLY_OPEN variable '}' { $$ = $2; }
;

encaps_var_offset:
		T_STRING			{ $$ = $1; }
	|	T_NUM_STRING		{ $$ = $1; }
	|	'-' T_NUM_STRING 	{ $$ = zend_negate_num_string($2); }
	|	T_VARIABLE			{ $$ = zend_ast_create(ZEND_AST_VAR, $1); }
;


internal_functions_in_yacc:
		T_ISSET '(' isset_variables possible_comma ')' { $$ = $3; }
	|	T_EMPTY '(' expr ')' { $$ = zend_ast_create(ZEND_AST_EMPTY, $3); }
	|	T_INCLUDE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE, $2); }
	|	T_INCLUDE_ONCE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_INCLUDE_ONCE, $2); }
	|	T_EVAL '(' expr ')'
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_EVAL, $3); }
	|	T_REQUIRE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE, $2); }
	|	T_REQUIRE_ONCE expr
			{ $$ = zend_ast_create_ex(ZEND_AST_INCLUDE_OR_EVAL, ZEND_REQUIRE_ONCE, $2); }
;

isset_variables:
		isset_variable { $$ = $1; }
	|	isset_variables ',' isset_variable
			{ $$ = zend_ast_create(ZEND_AST_AND, $1, $3); }
;

isset_variable:
		expr { $$ = zend_ast_create(ZEND_AST_ISSET, $1); }
;

%%

/* Over-ride Bison formatting routine to give better token descriptions.
   Copy to YYRES the contents of YYSTR for use in yyerror.
   YYSTR is taken from yytname, from the %token declaration.
   If YYRES is null, do not copy; instead, return the length of what
   the result would have been.  */
static YYSIZE_T zend_yytnamerr(char *yyres, const char *yystr)
{
	const char *toktype = yystr;
	size_t toktype_len = strlen(toktype);

	/* CG(parse_error) states:
	 * 0 => yyres = NULL, yystr is the unexpected token
	 * 1 => yyres = NULL, yystr is one of the expected tokens
	 * 2 => yyres != NULL, yystr is the unexpected token
	 * 3 => yyres != NULL, yystr is one of the expected tokens
	 */
	if (yyres && CG(parse_error) < 2) {
		CG(parse_error) = 2;
	}

	if (CG(parse_error) % 2 == 0) {
		/* The unexpected token */
		char buffer[120];
		const unsigned char *tokcontent, *tokcontent_end;
		size_t tokcontent_len;

		CG(parse_error)++;

		if (LANG_SCNG(yy_text)[0] == 0 &&
			LANG_SCNG(yy_leng) == 1 &&
			strcmp(toktype, "\"end of file\"") == 0) {
			if (yyres) {
				yystpcpy(yyres, "end of file");
			}
			return sizeof("end of file")-1;
		}

		/* Prevent the backslash getting doubled in the output (eugh) */
		if (strcmp(toktype, "\"'\\\\'\"") == 0) {
			if (yyres) {
				yystpcpy(yyres, "token \"\\\"");
			}
			return sizeof("token \"\\\"")-1;
		}

		/* We used "amp" as a dummy label to avoid a duplicate token literal warning. */
		if (strcmp(toktype, "\"amp\"") == 0) {
			if (yyres) {
				yystpcpy(yyres, "token \"&\"");
			}
			return sizeof("token \"&\"")-1;
		}

		/* Avoid unreadable """ */
		/* "'" would theoretically be just as bad, but is never currently parsed as a separate token */
		if (strcmp(toktype, "'\"'") == 0) {
			if (yyres) {
				yystpcpy(yyres, "double-quote mark");
			}
			return sizeof("double-quote mark")-1;
		}

		/* Strip off the outer quote marks */
		if (toktype_len >= 2 && *toktype == '"') {
			toktype++;
			toktype_len -= 2;
		}

		/* If the token always has one form, the %token line should have a single-quoted name */
		/* The parser rules also include single-character un-named tokens which will be single-quoted here */
		/* We re-format this with double quotes here to ensure everything's consistent */
		if (toktype_len > 0 && *toktype == '\'') {
			if (yyres) {
				snprintf(buffer, sizeof(buffer), "token \"%.*s\"", (int)toktype_len-2, toktype+1);
				yystpcpy(yyres, buffer);
			}
			return toktype_len + sizeof("token ")-1;
		}

		/* Fetch the content of the last seen token from global lexer state */
		tokcontent = LANG_SCNG(yy_text);
		tokcontent_len = LANG_SCNG(yy_leng);

		/* For T_BAD_CHARACTER, the content probably won't be a printable char */
		/* Also, "unexpected invalid character" sounds a bit redundant */
		if (tokcontent_len == 1 && strcmp(yystr, "\"invalid character\"") == 0) {
			if (yyres) {
				snprintf(buffer, sizeof(buffer), "character 0x%02hhX", *tokcontent);
				yystpcpy(yyres, buffer);
			}
			return sizeof("character 0x00")-1;
		}

		/* Truncate at line end to avoid messing up log formats */
		tokcontent_end = memchr(tokcontent, '\n', tokcontent_len);
		if (tokcontent_end != NULL) {
			tokcontent_len = (tokcontent_end - tokcontent);
		}

		/* Try to be helpful about what kind of string was found, before stripping the quotes */
		if (tokcontent_len > 0 && strcmp(yystr, "\"quoted string\"") == 0) {
			if (*tokcontent == '"') {
				toktype = "double-quoted string";
				toktype_len = sizeof("double-quoted string")-1;
			}
			else if (*tokcontent == '\'') {
				toktype = "single-quoted string";
				toktype_len = sizeof("single-quoted string")-1;
			}
		}

		/* For quoted strings, strip off another layer of quotes to avoid putting quotes inside quotes */
		if (tokcontent_len > 0 && (*tokcontent == '\'' || *tokcontent=='"'))  {
			tokcontent++;
			tokcontent_len--;
		}
		if (tokcontent_len > 0 && (tokcontent[tokcontent_len-1] == '\'' || tokcontent[tokcontent_len-1] == '"'))  {
			tokcontent_len--;
		}

		/* Truncate to 30 characters and add a ... */
		if (tokcontent_len > 30 + sizeof("...")-1) {
			if (yyres) {
				snprintf(buffer, sizeof(buffer), "%.*s \"%.*s...\"", (int)toktype_len, toktype, 30, tokcontent);
				yystpcpy(yyres, buffer);
			}
			return toktype_len + 30 + sizeof(" \"...\"")-1;
		}

		if (yyres) {
			snprintf(buffer, sizeof(buffer), "%.*s \"%.*s\"", (int)toktype_len, toktype, (int)tokcontent_len, tokcontent);
			yystpcpy(yyres, buffer);
		}
		return toktype_len + tokcontent_len + sizeof(" \"\"")-1;
	}

	/* One of the expected tokens */

	/* Prevent the backslash getting doubled in the output (eugh) */
	if (strcmp(toktype, "\"'\\\\'\"") == 0) {
		if (yyres) {
			yystpcpy(yyres, "\"\\\"");
		}
		return sizeof("\"\\\"")-1;
	}

	/* Strip off the outer quote marks */
	if (toktype_len >= 2 && *toktype == '"') {
		toktype++;
		toktype_len -= 2;
	}

	if (yyres) {
		YYSIZE_T yyn = 0;

		for (; yyn < toktype_len; ++yyn) {
			/* Replace single quotes with double for consistency */
			if (toktype[yyn] == '\'') {
				yyres[yyn] = '"';
			}
			else {
				yyres[yyn] = toktype[yyn];
			}
		}
		yyres[toktype_len] = '\0';
	}

	return toktype_len;
}
