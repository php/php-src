/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Andrei Zmievski <andrei@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_tokenizer.h"

typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef unsigned int yy_size_t;
struct yy_buffer_state
	{
	FILE *yy_input_file;
 
	char *yy_ch_buf;   /* input buffer */
	char *yy_buf_pos;  /* current position in input buffer */
 
	/* Size of input buffer in bytes, not including room for EOB
	 * characters.
	 */
	yy_size_t yy_buf_size;
 
	/* Number of characters read into yy_ch_buf, not including EOB
	 * characters.
	 */
	int yy_n_chars;
 
	/* Whether we "own" the buffer - i.e., we know we created it,
	 * and can realloc() it to grow it, and should free() it to
	 * delete it.
	 */
	int yy_is_our_buffer;
 
	/* Whether this is an "interactive" input source; if so, and
	 * if we're using stdio for input, then we want to use getc()
	 * instead of fread(), to make sure we stop fetching input after
	 * each newline.
	 */
	int yy_is_interactive;
 
	/* Whether we're considered to be at the beginning of a line.
	 * If so, '^' rules will be active on the next match, otherwise
	 * not.
	 */
	int yy_at_bol;
 
	/* Whether to try to fill the input buffer when we reach the
	 * end of it.
	 */
	int yy_fill_buffer;
 
	int yy_buffer_status;
#define YY_BUFFER_NEW 0
#define YY_BUFFER_NORMAL 1
	/* When an EOF's been seen but there's still some text to process
	 * then we mark the buffer as YY_EOF_PENDING, to indicate that we
	 * shouldn't try reading from the input source any more.  We might
	 * still have a bunch of tokens to match, though, because of
	 * possible backing-up.
	 *
	 * When we actually see the EOF, we change the status to "new"
	 * (via yyrestart()), so that the user can continue scanning by
	 * just pointing yyin at a new input file.
	 */
#define YY_BUFFER_EOF_PENDING 2
	};

#include "zend.h"
#include "zend_language_scanner.h"
#include "zend_language_parser.h"

#define zendtext LANG_SCNG(yy_text)
#define zendleng LANG_SCNG(yy_leng)
/* If you declare any globals in php_tokenizer.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(tokenizer)
*/

/* True global resources - no need for thread safety here */
/* static int le_tokenizer; */

/* {{{ tokenizer_functions[]
 *
 * Every user visible function must have an entry in tokenizer_functions[].
 */
function_entry tokenizer_functions[] = {
	PHP_FE(token_get_all,	NULL)
	PHP_FE(token_name,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in tokenizer_functions[] */
};
/* }}} */

/* {{{ tokenizer_module_entry
 */
zend_module_entry tokenizer_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"tokenizer",
	tokenizer_functions,
	PHP_MINIT(tokenizer),
	PHP_MSHUTDOWN(tokenizer),
	PHP_RINIT(tokenizer),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(tokenizer),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(tokenizer),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TOKENIZER
ZEND_GET_MODULE(tokenizer)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("tokenizer.global_value",      "42", PHP_INI_ALL, OnUpdateInt, global_value, zend_tokenizer_globals, tokenizer_globals)
	STD_PHP_INI_ENTRY("tokenizer.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_tokenizer_globals, tokenizer_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_tokenizer_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_tokenizer_init_globals(zend_tokenizer_globals *tokenizer_globals)
{
	tokenizer_globals->global_value = 0;
	tokenizer_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(tokenizer)
{
	/* If you have INI entries, uncomment these lines 
	ZEND_INIT_MODULE_GLOBALS(tokenizer, php_tokenizer_init_globals, NULL);
	REGISTER_INI_ENTRIES();
	*/

	REGISTER_LONG_CONSTANT("T_INCLUDE", T_INCLUDE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_INCLUDE_ONCE", T_INCLUDE_ONCE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_EVAL", T_EVAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_REQUIRE", T_REQUIRE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_REQUIRE_ONCE", T_REQUIRE_ONCE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LOGICAL_OR", T_LOGICAL_OR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LOGICAL_XOR", T_LOGICAL_XOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LOGICAL_AND", T_LOGICAL_AND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_PRINT", T_PRINT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_PLUS_EQUAL", T_PLUS_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_MINUS_EQUAL", T_MINUS_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_MUL_EQUAL", T_MUL_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DIV_EQUAL", T_DIV_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CONCAT_EQUAL", T_CONCAT_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_MOD_EQUAL", T_MOD_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_AND_EQUAL", T_AND_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_OR_EQUAL", T_OR_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_XOR_EQUAL", T_XOR_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_SL_EQUAL", T_SL_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_SR_EQUAL", T_SR_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_BOOLEAN_OR", T_BOOLEAN_OR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_BOOLEAN_AND", T_BOOLEAN_AND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_EQUAL", T_IS_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_NOT_EQUAL", T_IS_NOT_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_IDENTICAL", T_IS_IDENTICAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_NOT_IDENTICAL", T_IS_NOT_IDENTICAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_SMALLER_OR_EQUAL", T_IS_SMALLER_OR_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IS_GREATER_OR_EQUAL", T_IS_GREATER_OR_EQUAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_SL", T_SL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_SR", T_SR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_INC", T_INC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DEC", T_DEC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_INT_CAST", T_INT_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DOUBLE_CAST", T_DOUBLE_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_STRING_CAST", T_STRING_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ARRAY_CAST", T_ARRAY_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_OBJECT_CAST", T_OBJECT_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_BOOL_CAST", T_BOOL_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_UNSET_CAST", T_UNSET_CAST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_NEW", T_NEW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_EXIT", T_EXIT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_IF", T_IF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ELSEIF", T_ELSEIF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ELSE", T_ELSE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDIF", T_ENDIF, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LNUMBER", T_LNUMBER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DNUMBER", T_DNUMBER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_STRING", T_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_STRING_VARNAME", T_STRING_VARNAME, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_VARIABLE", T_VARIABLE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_NUM_STRING", T_NUM_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_INLINE_HTML", T_INLINE_HTML, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CHARACTER", T_CHARACTER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_BAD_CHARACTER", T_BAD_CHARACTER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENCAPSED_AND_WHITESPACE", T_ENCAPSED_AND_WHITESPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CONSTANT_ENCAPSED_STRING", T_CONSTANT_ENCAPSED_STRING, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ECHO", T_ECHO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DO", T_DO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_WHILE", T_WHILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDWHILE", T_ENDWHILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_FOR", T_FOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDFOR", T_ENDFOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_FOREACH", T_FOREACH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDFOREACH", T_ENDFOREACH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DECLARE", T_DECLARE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDDECLARE", T_ENDDECLARE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_AS", T_AS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_SWITCH", T_SWITCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ENDSWITCH", T_ENDSWITCH, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CASE", T_CASE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DEFAULT", T_DEFAULT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_BREAK", T_BREAK, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CONTINUE", T_CONTINUE, CONST_CS | CONST_PERSISTENT);
#ifndef ZEND_ENGINE_2
	REGISTER_LONG_CONSTANT("T_OLD_FUNCTION", T_OLD_FUNCTION, CONST_CS | CONST_PERSISTENT);
#endif
	REGISTER_LONG_CONSTANT("T_FUNCTION", T_FUNCTION, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CONST", T_CONST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_RETURN", T_RETURN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_USE", T_USE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_GLOBAL", T_GLOBAL, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_STATIC", T_STATIC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_VAR", T_VAR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_UNSET", T_UNSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ISSET", T_ISSET, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_EMPTY", T_EMPTY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CLASS", T_CLASS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_EXTENDS", T_EXTENDS, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_OBJECT_OPERATOR", T_OBJECT_OPERATOR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DOUBLE_ARROW", T_DOUBLE_ARROW, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LIST", T_LIST, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ARRAY", T_ARRAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CLASS_C", T_CLASS_C, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_FUNC_C", T_FUNC_C, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_LINE", T_LINE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_FILE", T_FILE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_COMMENT", T_COMMENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_ML_COMMENT", T_ML_COMMENT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_OPEN_TAG", T_OPEN_TAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_OPEN_TAG_WITH_ECHO", T_OPEN_TAG_WITH_ECHO, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CLOSE_TAG", T_CLOSE_TAG, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_WHITESPACE", T_WHITESPACE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_START_HEREDOC", T_START_HEREDOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_END_HEREDOC", T_END_HEREDOC, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DOLLAR_OPEN_CURLY_BRACES", T_DOLLAR_OPEN_CURLY_BRACES, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_CURLY_OPEN", T_CURLY_OPEN, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_PAAMAYIM_NEKUDOTAYIM", T_PAAMAYIM_NEKUDOTAYIM, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("T_DOUBLE_COLON", T_PAAMAYIM_NEKUDOTAYIM, CONST_CS | CONST_PERSISTENT);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(tokenizer)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(tokenizer)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(tokenizer)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(tokenizer)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "Tokenizer Support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

static void tokenize(zval *return_value TSRMLS_DC)
{
	zval token;
	zval *keyword;
	int token_type;
	zend_bool destroy;

	array_init(return_value);

	ZVAL_NULL(&token);
	while ((token_type = lex_scan(&token TSRMLS_CC))) {
		destroy = 1;
		switch (token_type) {
			case T_OPEN_TAG:
			case T_OPEN_TAG_WITH_ECHO:
			case T_WHITESPACE:
			case T_COMMENT:
			case T_CLOSE_TAG:
				destroy = 0;
				break;
		}

		if (token_type >= 256) {
			MAKE_STD_ZVAL(keyword);
			array_init(keyword);
			add_next_index_long(keyword, token_type);
			add_next_index_stringl(keyword, zendtext, zendleng, 1);
			add_next_index_zval(return_value, keyword);
		} else {
			add_next_index_stringl(return_value, zendtext, zendleng, 1);
		}
		if (destroy && Z_TYPE(token) != IS_NULL) {
			zval_dtor(&token);
		}
		ZVAL_NULL(&token);
	}
}

static char *
get_token_type_name(int token_type)
{
	switch (token_type) {
		case T_INCLUDE: return "T_INCLUDE";
		case T_INCLUDE_ONCE: return "T_INCLUDE_ONCE";
		case T_EVAL: return "T_EVAL";
		case T_REQUIRE: return "T_REQUIRE";
		case T_REQUIRE_ONCE: return "T_REQUIRE_ONCE";
		case T_LOGICAL_OR: return "T_LOGICAL_OR";
		case T_LOGICAL_XOR: return "T_LOGICAL_XOR";
		case T_LOGICAL_AND: return "T_LOGICAL_AND";
		case T_PRINT: return "T_PRINT";
		case T_PLUS_EQUAL: return "T_PLUS_EQUAL";
		case T_MINUS_EQUAL: return "T_MINUS_EQUAL";
		case T_MUL_EQUAL: return "T_MUL_EQUAL";
		case T_DIV_EQUAL: return "T_DIV_EQUAL";
		case T_CONCAT_EQUAL: return "T_CONCAT_EQUAL";
		case T_MOD_EQUAL: return "T_MOD_EQUAL";
		case T_AND_EQUAL: return "T_AND_EQUAL";
		case T_OR_EQUAL: return "T_OR_EQUAL";
		case T_XOR_EQUAL: return "T_XOR_EQUAL";
		case T_SL_EQUAL: return "T_SL_EQUAL";
		case T_SR_EQUAL: return "T_SR_EQUAL";
		case T_BOOLEAN_OR: return "T_BOOLEAN_OR";
		case T_BOOLEAN_AND: return "T_BOOLEAN_AND";
		case T_IS_EQUAL: return "T_IS_EQUAL";
		case T_IS_NOT_EQUAL: return "T_IS_NOT_EQUAL";
		case T_IS_IDENTICAL: return "T_IS_IDENTICAL";
		case T_IS_NOT_IDENTICAL: return "T_IS_NOT_IDENTICAL";
		case T_IS_SMALLER_OR_EQUAL: return "T_IS_SMALLER_OR_EQUAL";
		case T_IS_GREATER_OR_EQUAL: return "T_IS_GREATER_OR_EQUAL";
		case T_SL: return "T_SL";
		case T_SR: return "T_SR";
		case T_INC: return "T_INC";
		case T_DEC: return "T_DEC";
		case T_INT_CAST: return "T_INT_CAST";
		case T_DOUBLE_CAST: return "T_DOUBLE_CAST";
		case T_STRING_CAST: return "T_STRING_CAST";
		case T_ARRAY_CAST: return "T_ARRAY_CAST";
		case T_OBJECT_CAST: return "T_OBJECT_CAST";
		case T_BOOL_CAST: return "T_BOOL_CAST";
		case T_UNSET_CAST: return "T_UNSET_CAST";
		case T_NEW: return "T_NEW";
		case T_EXIT: return "T_EXIT";
		case T_IF: return "T_IF";
		case T_ELSEIF: return "T_ELSEIF";
		case T_ELSE: return "T_ELSE";
		case T_ENDIF: return "T_ENDIF";
		case T_LNUMBER: return "T_LNUMBER";
		case T_DNUMBER: return "T_DNUMBER";
		case T_STRING: return "T_STRING";
		case T_STRING_VARNAME: return "T_STRING_VARNAME";
		case T_VARIABLE: return "T_VARIABLE";
		case T_NUM_STRING: return "T_NUM_STRING";
		case T_INLINE_HTML: return "T_INLINE_HTML";
		case T_CHARACTER: return "T_CHARACTER";
		case T_BAD_CHARACTER: return "T_BAD_CHARACTER";
		case T_ENCAPSED_AND_WHITESPACE: return "T_ENCAPSED_AND_WHITESPACE";
		case T_CONSTANT_ENCAPSED_STRING: return "T_CONSTANT_ENCAPSED_STRING";
		case T_ECHO: return "T_ECHO";
		case T_DO: return "T_DO";
		case T_WHILE: return "T_WHILE";
		case T_ENDWHILE: return "T_ENDWHILE";
		case T_FOR: return "T_FOR";
		case T_ENDFOR: return "T_ENDFOR";
		case T_FOREACH: return "T_FOREACH";
		case T_ENDFOREACH: return "T_ENDFOREACH";
		case T_DECLARE: return "T_DECLARE";
		case T_ENDDECLARE: return "T_ENDDECLARE";
		case T_AS: return "T_AS";
		case T_SWITCH: return "T_SWITCH";
		case T_ENDSWITCH: return "T_ENDSWITCH";
		case T_CASE: return "T_CASE";
		case T_DEFAULT: return "T_DEFAULT";
		case T_BREAK: return "T_BREAK";
		case T_CONTINUE: return "T_CONTINUE";
#ifndef ZEND_ENGINE_2
		case T_OLD_FUNCTION: return "T_OLD_FUNCTION";
#endif
		case T_FUNCTION: return "T_FUNCTION";
		case T_CONST: return "T_CONST";
		case T_RETURN: return "T_RETURN";
		case T_USE: return "T_USE";
		case T_GLOBAL: return "T_GLOBAL";
		case T_STATIC: return "T_STATIC";
		case T_VAR: return "T_VAR";
		case T_UNSET: return "T_UNSET";
		case T_ISSET: return "T_ISSET";
		case T_EMPTY: return "T_EMPTY";
		case T_CLASS: return "T_CLASS";
		case T_EXTENDS: return "T_EXTENDS";
		case T_OBJECT_OPERATOR: return "T_OBJECT_OPERATOR";
		case T_DOUBLE_ARROW: return "T_DOUBLE_ARROW";
		case T_LIST: return "T_LIST";
		case T_ARRAY: return "T_ARRAY";
		case T_CLASS_C: return "T_CLASS_C";
		case T_FUNC_C: return "T_FUNC_C";
		case T_LINE: return "T_LINE";
		case T_FILE: return "T_FILE";
		case T_COMMENT: return "T_COMMENT";
		case T_ML_COMMENT: return "T_ML_COMMENT";
		case T_OPEN_TAG: return "T_OPEN_TAG";
		case T_OPEN_TAG_WITH_ECHO: return "T_OPEN_TAG_WITH_ECHO";
		case T_CLOSE_TAG: return "T_CLOSE_TAG";
		case T_WHITESPACE: return "T_WHITESPACE";
		case T_START_HEREDOC: return "T_START_HEREDOC";
		case T_END_HEREDOC: return "T_END_HEREDOC";
		case T_DOLLAR_OPEN_CURLY_BRACES: return "T_DOLLAR_OPEN_CURLY_BRACES";
		case T_CURLY_OPEN: return "T_CURLY_OPEN";
		case T_PAAMAYIM_NEKUDOTAYIM: return "T_DOUBLE_COLON";
	}
	return "UNKNOWN";
}

/* {{{ proto array token_get_all(string source)
 */
PHP_FUNCTION(token_get_all)
{
	char *source = NULL;
	int argc = ZEND_NUM_ARGS();
	int source_len;
	zval source_z;
	zend_lex_state original_lex_state;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &source, &source_len) == FAILURE) 
		return;

	ZVAL_STRINGL(&source_z, source, source_len, 1);
	zend_save_lexical_state(&original_lex_state TSRMLS_CC);

	if (zend_prepare_string_for_scanning(&source_z, "" TSRMLS_CC) == FAILURE) {
		RETURN_EMPTY_STRING();
	}

	tokenize(return_value TSRMLS_CC);
	
	zend_restore_lexical_state(&original_lex_state TSRMLS_CC);
	zval_dtor(&source_z);
}
/* }}} */

/* {{{ proto string token_name(int type)
 */
PHP_FUNCTION(token_name)
{
	int argc = ZEND_NUM_ARGS();
	long type;

	if (zend_parse_parameters(argc TSRMLS_CC, "l", &type) == FAILURE) {
		return;
	}
	RETVAL_STRING(get_token_type_name(type), 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
