/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   |   Jan Kneschke <jan.kneschke@phpdoc.de>                              |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "php_phpdoc.h"


/*
** <!-- 
** taken from zend_language_scanner.c
** perhaps this a LICENSE CONFLICT, don't know.
*/

typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef unsigned int yy_size_t;
struct yy_buffer_state
	{
	FILE *yy_input_file;
 
	char *yy_ch_buf;		/* input buffer */
	char *yy_buf_pos;	       /* current position in input buffer */
 
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

#ifndef ZTS
extern char *zendtext;
extern int zendleng;
#else
#define zendtext ((char *) zend_get_zendtext(CLS_C))
#define zendleng zend_get_zendleng(CLS_C)
#endif

/*
** -->
*/

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_PHPDOC

/* If you declare any globals in php_phpdoc.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(phpdoc)
*/

/* True global resources - no need for thread safety here */
static int le_phpdoc;

/* Every user visible function must have an entry in phpdoc_functions[].
*/
function_entry phpdoc_functions[] = {
	PHP_FE(confirm_phpdoc_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(phpdoc_xml_from_string, NULL)
	{NULL, NULL, NULL}	/* Must be the last line in phpdoc_functions[] */
};

zend_module_entry phpdoc_module_entry = {
	"phpdoc",
	phpdoc_functions,
	PHP_MINIT(phpdoc),
	PHP_MSHUTDOWN(phpdoc),
	PHP_RINIT(phpdoc),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(phpdoc),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(phpdoc),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHPDOC
ZEND_GET_MODULE(phpdoc)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(phpdoc)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(phpdoc)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(phpdoc)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(phpdoc)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(phpdoc)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "phpdoc support", "enabled");
	php_info_print_table_row(2, "PHPDoc Version", "IPv6" );
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_phpdoc_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_FUNCTION(confirm_phpdoc_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/phpdoc/config.m4, module %s is compiled into PHP", Z_STRVAL_PP(arg));
	RETURN_STRINGL(string, len, 1);
}
/* }}} */







int find_something(int *stack, int stack_pos) {
#if 0
	int i = stack_pos -1;

	zend_printf("L:%d:",i);
	for (; i >= 0; i--) {
		zend_printf("[%d]",stack[i]);
	}
#endif
	return (stack_pos > 0) ? stack[stack_pos-1] : -1;
}

#define PHPDOC_STRCAT(x,y) \
	x->value.str.len += strlen(y); \
	if (x->value.str.val != empty_string) {\
		x->value.str.val = erealloc(x->value.str.val, x->value.str.len+1); \
		strcat(x->value.str.val, y);\
	} else {\
		x->value.str.val = emalloc(x->value.str.len+1); \
		strcpy(x->value.str.val, y);\
	}

void sprint_xml(zval *new_string, const char *s) {
	char buffer[32];

	while (*s) {
		switch(*s) {
			case '<':
				zend_sprintf(buffer, "&lt;");
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case '>':
				zend_sprintf(buffer, "&gt;");
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case '"':
				zend_sprintf(buffer, "&quot;");
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case '&':
				zend_sprintf(buffer, "&amp;");
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case '\'':
				zend_sprintf(buffer, "&apos;");
				PHPDOC_STRCAT(new_string, buffer);
				break;
			default:
				zend_sprintf(buffer, "%c",*s);
				PHPDOC_STRCAT(new_string, buffer);
		}
		s++;
	}
}


#define T_FUNCTION_PARAM		INT_MAX-1
#define T_FUNCTION_PARAM_DEFAULT	INT_MAX-2
#define T_FUNCTION_PARAM_LIST		INT_MAX-3
#define T_BLOCK				INT_MAX-4

typedef struct {
	int	*stack;
	int	pos;
	int	size;
	int	increment;
} phpdoc_stack;

zval *gen_xml(int debug)
{
	zval token;
	int token_type;
	
	phpdoc_stack st;
	
	zval *new_string;
	
	char buffer[256];
	
	CLS_FETCH();
	
	MAKE_STD_ZVAL(new_string);
	ZVAL_EMPTY_STRING(new_string);
	
	/* init the stack */
	st.size		= 256;
	st.increment	= 128;
	st.pos		= 0;
	st.stack	= emalloc(sizeof(int) * st.size);

	PHPDOC_STRCAT(new_string, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n");
	
	PHPDOC_STRCAT(new_string, "<php>\n");

	st.stack[0] = -1; 
	
	token.type = 0;
	while ((token_type=CG(lang_scanner).lex_scan(&token CLS_CC))) {
		if (debug)
			zend_printf("// %s(%d)\n",zendtext, token_type);
			
		switch (token_type) {
		case T_INCLUDE:
			PHPDOC_STRCAT(new_string, "<uses type=\"include\" file=\"");
			st.stack[st.pos++] = token_type;
			break;
		case T_INCLUDE_ONCE:
			PHPDOC_STRCAT(new_string, "<uses type=\"include once\" file=\"");
			st.stack[st.pos++] = token_type;
			break;
		case T_REQUIRE:
			PHPDOC_STRCAT(new_string, "<uses type=\"require\" file=\"");
			st.stack[st.pos++] = token_type;
			break;
		case T_REQUIRE_ONCE:
			PHPDOC_STRCAT(new_string, "<uses type=\"require once\" file=\"");
			st.stack[st.pos++] = token_type;
			break;
		case T_CLASS:
			PHPDOC_STRCAT(new_string, "<class ");
			st.stack[st.pos++] = token_type;
			break;
		case T_FUNCTION:
			PHPDOC_STRCAT(new_string, "<function ");
			st.stack[st.pos++] = token_type;
			break;
		case T_VAR:
			PHPDOC_STRCAT(new_string, "<variable ");
			st.stack[st.pos++] = token_type;
			break;
		case T_GLOBAL:
			st.stack[st.pos++] = token_type;
			break;
		case T_ML_COMMENT:
		case T_COMMENT:
			PHPDOC_STRCAT(new_string, "<comment>");
			
			sprint_xml(new_string, zendtext);
			
			PHPDOC_STRCAT(new_string, "</comment>\n");
			break;
		case T_EXTENDS:
			switch(find_something(st.stack, st.pos)) {
			case T_CLASS:
				st.stack[st.pos++] = token_type;
				break;
			default:
				zend_printf("ALARM: extends w/o class\n");
				break;
			}
			break;
		case T_VARIABLE:
			switch(find_something(st.stack, st.pos)) {
			case T_VAR:
				zend_sprintf(buffer, " name=\"%s\"", zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case -T_FUNCTION:
				st.stack[st.pos++] = token_type;
				break;
			case T_FUNCTION_PARAM_LIST:
				st.stack[st.pos++] = T_FUNCTION_PARAM;
				zend_sprintf(buffer, "<args name=\"%s\"", zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case T_GLOBAL:
				zend_sprintf(buffer, "<global name=\"%s\"/>",zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				break;
			default:
				break;
			}
			break;
			
		case T_STRING:
			switch(find_something(st.stack, st.pos)) {
			case T_CLASS:
			case T_FUNCTION:
			case T_VAR:
				zend_sprintf(buffer, " name=\"%s\"", zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				break;
			case T_EXTENDS:
				zend_sprintf(buffer, " extends=\"%s\"", zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				st.pos--;
				break;
			case T_INCLUDE:
			case T_INCLUDE_ONCE:
			case T_REQUIRE:
			case T_REQUIRE_ONCE:
				PHPDOC_STRCAT(new_string, zendtext);
				break;
			}
			break;
		case T_DNUMBER:
			switch(find_something(st.stack, st.pos)) {
			case T_FUNCTION_PARAM_DEFAULT:
				PHPDOC_STRCAT(new_string, zendtext);
				PHPDOC_STRCAT(new_string, "\"");
				st.pos--;
				break;
			default:
				break;
			}
			break;
		case T_LNUMBER:
			switch(find_something(st.stack, st.pos)) {
			case T_FUNCTION_PARAM_DEFAULT:
				PHPDOC_STRCAT(new_string, zendtext);
				PHPDOC_STRCAT(new_string, "\"");
				st.pos--;
				break;
			default:
				break;
			}
			break;
		case '-':
			switch(find_something(st.stack, st.pos)) {
			case T_FUNCTION_PARAM_DEFAULT:
				/* a DNUMBER or LNUMBER will follow */
				PHPDOC_STRCAT(new_string, zendtext);
				
				break;
			default:
				break;
			}
			break;
		case '{':
			switch (find_something(st.stack, st.pos)) {
			case T_CLASS:
			case T_FUNCTION:
				PHPDOC_STRCAT(new_string, ">");
				st.stack[st.pos-1] = -st.stack[st.pos-1];
				break;
			case T_FUNCTION_PARAM_LIST:
				st.pos--;
				
				/* and finish the function-head */
				st.stack[st.pos-1] = -st.stack[st.pos-1];
				break;
			default:
				st.stack[st.pos++] = T_BLOCK;
				break;
			}
			break;
		case '}':
			switch (find_something(st.stack, st.pos)) {
			case -T_CLASS:
				PHPDOC_STRCAT(new_string, "</class>\n");
				st.pos--;
				break;
			case -T_FUNCTION:
				PHPDOC_STRCAT(new_string, "</function>\n");
				st.pos--;
				break;
			case T_BLOCK:
				st.pos--;
				break;
			}
			break;
		case '(':
			switch (find_something(st.stack, st.pos)) {
			case T_FUNCTION:
				/* terminate the function-head */
				PHPDOC_STRCAT(new_string, ">");
				st.stack[st.pos++] = T_FUNCTION_PARAM_LIST;
				break;
			}
			break;
		case ')':
			switch (find_something(st.stack, st.pos)) {
			case T_FUNCTION_PARAM:
				/* remove the param-list from the st.stack */
				PHPDOC_STRCAT(new_string, "/>");
				st.pos--;
				break;
			case T_INCLUDE:
			case T_INCLUDE_ONCE:
			case T_REQUIRE:
			case T_REQUIRE_ONCE:
				zend_sprintf(buffer, "\"/>\n");
				PHPDOC_STRCAT(new_string, buffer);
				st.pos--;
				break;
			}
			break;
		case '=':
			switch (find_something(st.stack, st.pos)) {
			case T_VAR:
				zend_sprintf(buffer, ">");
				PHPDOC_STRCAT(new_string, buffer);
				st.stack[st.pos-1] = -st.stack[st.pos-1];
				break;
			case T_FUNCTION_PARAM:
				zend_sprintf(buffer, " default=\"");
				PHPDOC_STRCAT(new_string, buffer);
				st.stack[st.pos++] = T_FUNCTION_PARAM_DEFAULT;
				
				break;
			}
			break;
		case ';':
			switch (find_something(st.stack, st.pos)) {
			case T_VAR:
				zend_sprintf(buffer, ">");
				PHPDOC_STRCAT(new_string, buffer);
			case -T_VAR:
				zend_sprintf(buffer, "</variable>\n");
				PHPDOC_STRCAT(new_string, buffer);
				st.pos--;
				break;
			case T_GLOBAL:
			case T_VARIABLE:
				st.pos--;
				break;
			case T_INCLUDE:
			case T_INCLUDE_ONCE:
			case T_REQUIRE:
			case T_REQUIRE_ONCE:
				zend_sprintf(buffer, "\"/>\n");
				PHPDOC_STRCAT(new_string, buffer);
				st.pos--;
				break;
			}
			break;
		case ',':
		case T_ARRAY:
			switch (find_something(st.stack, st.pos)) {
			case T_FUNCTION_PARAM:
				zend_sprintf(buffer, "/>");
				PHPDOC_STRCAT(new_string, buffer);
				st.pos--;
				break;
			}
		case T_CONSTANT_ENCAPSED_STRING:
			switch (find_something(st.stack, st.pos)) {
			case T_INCLUDE:
			case T_INCLUDE_ONCE:
			case T_REQUIRE:
			case T_REQUIRE_ONCE:
				sprint_xml(new_string, zendtext);
				break;
			case T_FUNCTION_PARAM_DEFAULT:
				sprint_xml(new_string, zendtext);
				PHPDOC_STRCAT(new_string, "\"");
				st.pos--;
				break;
			}
			break;
		case '.':
			switch (find_something(st.stack, st.pos)) {
			case T_INCLUDE:
			case T_INCLUDE_ONCE:
			case T_REQUIRE:
			case T_REQUIRE_ONCE:
				zend_sprintf(buffer, "%s",zendtext);
				PHPDOC_STRCAT(new_string, buffer);
				break;
			}
			break;
		default:
			break;
		}
		
		if (st.pos >= st.size - 5) {
			st.size += st.increment;
			st.stack = erealloc(st.stack, st.size);
		}
		
		if (st.pos < 0) {
			char *c = NULL;
			
			/* i don't really know how to handle it. gooing down hard seem to be to best way */
			
			*c = 'V';
		}
		
		if (token_type == T_END_HEREDOC) {
			zend_bool has_semicolon=(strchr(token.value.str.val, ';')?1:0);

			efree(token.value.str.val);
			if (has_semicolon) {
				lex_scan(&token CLS_CC);
			}
		}
		token.type = 0;
	}
	
	efree(st.stack);
	
	PHPDOC_STRCAT(new_string, "</php>\n");
	
	zval_copy_ctor(new_string);
	return new_string;
}

/* {{{ proto string phpdoc_xml_from_string(string arg)
   Generates XML for PHPDoc from a usersupplied string */

PHP_FUNCTION(phpdoc_xml_from_string)
{
	zval **arg;
	zend_lex_state original_lex_state;
	
#ifdef ZTS
	istrstream *input_stream;
#endif

	CLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	convert_to_string_ex(arg);

	CG(lang_scanner).save_lexical_state(&original_lex_state CLS_CC);
	
#ifndef ZTS
	if (prepare_string_for_scanning(*arg, "dummy.php")==FAILURE) {
#else
	if (prepare_string_for_scanning(*arg, &input_stream, "dummy.php" CLS_CC)==FAILURE) {
#endif
		RETURN_EMPTY_STRING();
	}
#if 1
	*return_value = *gen_xml(0);
#else
	MAKE_STD_ZVAL(return_value);
	ZVAL_EMPTY_STRING(return_value);
#endif
	CG(lang_scanner).restore_lexical_state(&original_lex_state CLS_CC);
#ifdef ZTS
	delete input_stream;
#endif

	zval_copy_ctor(return_value);
	return;
	
}
/* }}} */


#endif	/* HAVE_PHPDOC */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
