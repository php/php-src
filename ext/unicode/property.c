/*
   +----------------------------------------------------------------------+
   | PHP Version 6                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andrei Zmievski <andrei@php.net>                            |
   +----------------------------------------------------------------------+
 */

/* $Id$ */ 

#include "php_unicode.h"

typedef UBool (*prop_check_func_t)(UChar32 ch);

static void check_property_impl(INTERNAL_FUNCTION_PARAMETERS, prop_check_func_t checker)
{
	UChar	   *str;
	int			str_len;
	zend_bool	result = 1;
	int			offset = 0;
	UChar32		ch;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "u", &str, &str_len) == FAILURE) {
		return;
	}

	if (str_len == 0) {
		RETURN_FALSE;
	}

	while (offset < str_len && result) {
		U16_NEXT(str, offset, str_len, ch);
		result = checker(ch);
	}

	RETURN_BOOL(result);
}


/* {{{ C/POSIX migration functions */

PHP_FUNCTION(unicode_is_lower)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_islower);
}

PHP_FUNCTION(unicode_is_upper)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isupper);
}

PHP_FUNCTION(unicode_is_digit)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isdigit);
}

PHP_FUNCTION(unicode_is_alpha)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isalpha);
}

PHP_FUNCTION(unicode_is_alnum)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isalnum);
}

PHP_FUNCTION(unicode_is_xdigit)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isxdigit);
}

PHP_FUNCTION(unicode_is_punct)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_ispunct);
}

PHP_FUNCTION(unicode_is_graph)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isgraph);
}

PHP_FUNCTION(unicode_is_blank)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isblank);
}

PHP_FUNCTION(unicode_is_space)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isspace);
}

PHP_FUNCTION(unicode_is_cntrl)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_iscntrl);
}

PHP_FUNCTION(unicode_is_print)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isprint);
}

/* }}} */

/*
 * Additional binary property functions
 */

PHP_FUNCTION(unicode_is_title)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_istitle);
}

PHP_FUNCTION(unicode_is_defined)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isdefined);
}

PHP_FUNCTION(unicode_is_id_start)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDStart);
}

PHP_FUNCTION(unicode_is_id_part)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDPart);
}

PHP_FUNCTION(unicode_is_id_ignorable)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isIDIgnorable);
}

PHP_FUNCTION(unicode_is_iso_control)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isISOControl);
}

PHP_FUNCTION(unicode_is_mirrored)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isMirrored);
}

PHP_FUNCTION(unicode_is_base)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isbase);
}

PHP_FUNCTION(unicode_is_whitespace)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isWhitespace);
}

PHP_FUNCTION(unicode_is_u_whitespace)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUWhiteSpace);
}

PHP_FUNCTION(unicode_is_u_alphabetic)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUAlphabetic);
}

PHP_FUNCTION(unicode_is_u_uppercase)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isUUppercase);
}

PHP_FUNCTION(unicode_is_u_lowercase)
{
	check_property_impl(INTERNAL_FUNCTION_PARAM_PASSTHRU, u_isULowercase);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
