/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Hartmut Holzgraefe <hartmut@six.de>                          |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
/* static int le_ctype; */

/* {{{ ctype_functions[]
 * Every user visible function must have an entry in ctype_functions[].
 */
function_entry ctype_functions[] = {
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
/* }}} */

/* {{{ ctype_mpodule_entry
 */
zend_module_entry ctype_module_entry = {
	STANDARD_MODULE_HEADER,
	"ctype",
	ctype_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(ctype),
    NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CTYPE
ZEND_GET_MODULE(ctype)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ctype)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "ctype functions", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ ctype
 */
#define CTYPE(iswhat) \
	zval *c, tmp; \
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &c) == FAILURE) \
		return; \
	if (Z_TYPE_P(c) == IS_LONG) { \
		if (Z_LVAL_P(c) <= 255 && Z_LVAL_P(c) >= 0) { \
			RETURN_BOOL(iswhat(Z_LVAL_P(c))); \
		} else if (Z_LVAL_P(c) >= -128 && Z_LVAL_P(c) < 0) { \
			RETURN_BOOL(iswhat(Z_LVAL_P(c) + 256)); \
		} \
		tmp = *c; \
		zval_copy_ctor(&tmp); \
		convert_to_string(&tmp); \
	} else { \
		tmp = *c; \
	} \
	if (Z_TYPE(tmp) == IS_STRING) { \
		char *p = Z_STRVAL(tmp), *e = Z_STRVAL(tmp) + Z_STRLEN(tmp); \
		if (e == p) {	\
			if (Z_TYPE_P(c) == IS_LONG) zval_dtor(&tmp); \
			RETURN_FALSE;	\
		}	\
		while (p < e) { \
			if(!iswhat((int)*(unsigned char *)(p++))) { \
				if (Z_TYPE_P(c) == IS_LONG) zval_dtor(&tmp); \
				RETURN_FALSE; \
			} \
		} \
		if (Z_TYPE_P(c) == IS_LONG) zval_dtor(&tmp); \
		RETURN_TRUE; \
	} else { \
		RETURN_FALSE; \
	} \

/* }}} */

/* {{{ proto bool ctype_alnum(mixed c)
   Checks for alphanumeric character(s) */
PHP_FUNCTION(ctype_alnum)
{
	CTYPE(isalnum);
}
/* }}} */

/* {{{ proto bool ctype_alpha(mixed c)
   Checks for alphabetic character(s) */
PHP_FUNCTION(ctype_alpha)
{
	CTYPE(isalpha);
}
/* }}} */

/* {{{ proto bool ctype_cntrl(mixed c)
   Checks for control character(s) */
PHP_FUNCTION(ctype_cntrl)
{
	CTYPE(iscntrl);
}
/* }}} */

/* {{{ proto bool ctype_digit(mixed c)
   Checks for numeric character(s) */
PHP_FUNCTION(ctype_digit)
{
	CTYPE(isdigit);
}
/* }}} */

/* {{{ proto bool ctype_lower(mixed c)
   Checks for lowercase character(s)  */
PHP_FUNCTION(ctype_lower)
{
	CTYPE(islower);
}
/* }}} */

/* {{{ proto bool ctype_graph(mixed c)
   Checks for any printable character(s) except space */
PHP_FUNCTION(ctype_graph)
{
	CTYPE(isgraph);
}
/* }}} */

/* {{{ proto bool ctype_print(mixed c)
   Checks for printable character(s) */
PHP_FUNCTION(ctype_print)
{
	CTYPE(isprint);
}
/* }}} */

/* {{{ proto bool ctype_punct(mixed c)
   Checks for any printable character which is not whitespace or an alphanumeric character */
PHP_FUNCTION(ctype_punct)
{
	CTYPE(ispunct);
}
/* }}} */

/* {{{ proto bool ctype_space(mixed c)
   Checks for whitespace character(s)*/
PHP_FUNCTION(ctype_space)
{
	CTYPE(isspace);
}
/* }}} */

/* {{{ proto bool ctype_upper(mixed c)
   Checks for uppercase character(s) */
PHP_FUNCTION(ctype_upper)
{
	CTYPE(isupper);
}
/* }}} */

/* {{{ proto bool ctype_xdigit(mixed c)
   Checks for character(s) representing a hexadecimal digit */
PHP_FUNCTION(ctype_xdigit)
{
	CTYPE(isxdigit);
}
/* }}} */

#endif	/* HAVE_CTYPE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
