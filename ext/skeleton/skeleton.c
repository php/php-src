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
   +----------------------------------------------------------------------+
 */
 
/* $Id: */

#include "php.h"
#include "php_ini.h"
#include "php__extname_.h"

/* You should tweak config.m4 so this symbol (or some else suitable)
   gets defined.
*/
#ifdef HAVE__EXTNAME_

#ifdef ZTS
int _extname__globals_id;
#else
php__extname__globals _extname__globals;
#endif

/* Every user visible function must have an entry in _extname_functions[].
*/
function_entry _extname__functions[] = {
	PHP_FE(_extname__test,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in $extension_functions[] */
};

zend_module_entry _extname__module_entry = {
	"_extname_",
	_extname__functions,
	PHP_MINIT(_extname_),
	PHP_MSHUTDOWN(_extname_),
	PHP_RINIT(_extname_),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(_extname_),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(_extname_),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL__EXTNAME_
ZEND_GET_MODULE(_extname_)
#endif

/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
PHP_INI_END()
*/

PHP_MINIT_FUNCTION(_extname_)
{
/* Remove comments if you have entries in php.ini
	REGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(_extname_)
{
/* Remove comments if you have entries in php.ini
	UNREGISTER_INI_ENTRIES();
*/
	return SUCCESS;
}

/* Remove if there's nothing to do at request start */
PHP_RINIT_FUNCTION(_extname_)
{
	return SUCCESS;
}

/* Remove if there's nothing to do at request end */
PHP_RSHUTDOWN_FUNCTION(_extname_)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(_extname_)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "_extname_ support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes.
*/

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string _extname__test(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(_extname__test)
{
	zval **arg;
	int len;
	char string[256];

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg);

	len = sprintf(string, "Congratulations, you have successfully modified ext/_extname_/config.m4, module %s is compiled in PHP", Z_STRVAL_PP(arg));
	RETVAL_STRINGL(string, len, 1);
}
/* }}} */
/* The previous line is meant for emacs, so it can correctly fold and unfold
   functions in source code. See the corresponding marks just before function
   definition, where the functions purpose is also documented. Please follow
   this convention for the convenience of others editing your code.
*/

#endif	/* HAVE__EXTNAME_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
