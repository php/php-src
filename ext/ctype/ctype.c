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
   | Authors: Hartmut Holzgraefe <hartmut@six.de>                         |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

#include "php.h"
#include "php_ini.h"
#include "php_ctype.h"
#include "SAPI.h"
#include "ext/standard/info.h"

#include <ctype.h>

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#if HAVE_CTYPE

/* If you declare any globals in php_ctype.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(ctype)
*/

/* True global resources - no need for thread safety here */
static int le_ctype;

/* Every user visible function must have an entry in ctype_functions[].
*/
function_entry ctype_functions[] = {
	PHP_FE(confirm_ctype_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(ctype_alnum,	NULL)
	PHP_FE(ctype_alpha,	NULL)
	PHP_FE(ctype_cntrl,	NULL)
	PHP_FE(ctype_digit,	NULL)
	PHP_FE(ctype_lower,	NULL)
	PHP_FE(ctype_graph,	NULL)
	PHP_FE(ctype_print,	NULL)
	PHP_FE(ctype_punct,	NULL)
	PHP_FE(ctype_space,	NULL)
	PHP_FE(ctype_upper,	NULL)
	PHP_FE(ctype_xdigit,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in ctype_functions[] */
};

zend_module_entry ctype_module_entry = {
	"ctype",
	ctype_functions,
	PHP_MINIT(ctype),
	PHP_MSHUTDOWN(ctype),
	PHP_RINIT(ctype),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(ctype),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(ctype),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CTYPE
ZEND_GET_MODULE(ctype)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

#ifndef PHP_EXPERIMENTAL
#define PHP_EXPERIMENTAL(x,y)
#endif 


PHP_MINIT_FUNCTION(ctype)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ctype)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(ctype)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(ctype)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(ctype)
{
	ELS_FETCH();
	SLS_FETCH();

	php_info_print_table_start();
	php_info_print_table_row(2, "ctype functions", "enabled (experimental)");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_ctype_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_ctype_compiled)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/ctype/config.m4, module %s is compiled into PHP", Z_STRVAL_PP(arg));
	RETURN_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
*/

static int ctype(int (*iswhat)(int),zval **c) 
{
	switch ((*c)->type) {
	case IS_LONG:
		return iswhat((*c)->value.lval);
	case IS_STRING:
		{
			char *p;
			int n,len;
			convert_to_string_ex(c);
			p=(*c)->value.str.val;
			len = (*c)->value.str.len;
			for(n=0;n<len;n++) {
				if(!iswhat(*p++)) return 0;
			}
			return 1;
		}
	default:
		break;
	}
	return 0;
}

/* {{{ proto bool isalnum(mixed c)
    Check for alphanumeric character(s) */
PHP_FUNCTION(ctype_alnum)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	
	if(ctype(isalnum,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isalpha(mixed c)
    Check for alphabetic character(s) */
PHP_FUNCTION(ctype_alpha)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isalpha,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool iscntrl(mixed c)
    Check for control character(s) */
PHP_FUNCTION(ctype_cntrl)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(iscntrl,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isdigit(mixed c)
   Check for numeric character(s) */
PHP_FUNCTION(ctype_digit)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isdigit,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool islower(mixed c)
   Check for lowercase character(s)  */
PHP_FUNCTION(ctype_lower)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(islower,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isgraph(mixed c)
    Check for any printable character(s) except space */
PHP_FUNCTION(ctype_graph)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isgraph,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isprint(mixed c)
    Check for printable character(s) */
PHP_FUNCTION(ctype_print)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isprint,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool ispunct(mixed c)
    Check for any printable character which is not a space or an alphanumeric character */
PHP_FUNCTION(ctype_punct)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(ispunct,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isspace(mixed c)
    Check for whitespace character(s)*/
PHP_FUNCTION(ctype_space)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isspace,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isupper(mixed c)
    Check for uppercase character(s) */
PHP_FUNCTION(ctype_upper)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isupper,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */

/* {{{ proto bool isxdigit(mixed c)
    Check for character(s) representing a hexadecimal digit */
PHP_FUNCTION(ctype_xdigit)
{
	PHP_EXPERIMENTAL("4.0.4dev",NULL)
	zval **c;
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &c) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	if(ctype(isxdigit,c)) {
	   RETURN_TRUE;
	}

	RETURN_FALSE;		
}
/* }}} */


#endif	/* HAVE_CTYPE */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
