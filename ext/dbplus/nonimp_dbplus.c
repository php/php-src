/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
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
#include "php_dbplus.h"


/* {{{ proto int dbplus_change(int handle, string domain)
    */
PHP_FUNCTION(dbplus_change)
{
	zval **handle, **domain;
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &handle, &domain) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(handle);
	convert_to_string_ex(domain);


	php_error(E_WARNING, "dbplus_change: not yet implemented");
}
/* }}} */





/* {{{ proto int dbplus_rcreate(string name, string domlist, int flag)
    */
PHP_FUNCTION(dbplus_rcreate)
{
	zval **name, **domlist, **flag;
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &name, &domlist, &flag) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	convert_to_string_ex(domlist);
	convert_to_long_ex(flag);


	php_error(E_WARNING, "dbplus_rcreate: not yet implemented");
}
/* }}} */

/* {{{ proto int dbplus_rcrtexact(string name, int handle, int flag)
    */
PHP_FUNCTION(dbplus_rcrtexact)
{
	zval **name, **handle, **flag;
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &name, &handle, &flag) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	convert_to_long_ex(handle);
	convert_to_long_ex(flag);


	php_error(E_WARNING, "dbplus_rcrtexact: not yet implemented");
}
/* }}} */

/* {{{ proto int dbplus_rcrtlike(string name, int handle, int flag)
    */
PHP_FUNCTION(dbplus_rcrtlike)
{
	zval **name, **handle, **flag;
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &name, &handle, &flag) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	convert_to_long_ex(handle);
	convert_to_long_ex(flag);


	php_error(E_WARNING, "dbplus_rcrtlike: not yet implemented");
}
/* }}} */

/* {{{ proto int dbplus_resolve(string name, string vname)
    */
PHP_FUNCTION(dbplus_resolve)
{
	zval **name, **vname;
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &name, &vname) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(name);
	convert_to_string_ex(vname);


	php_error(E_WARNING, "dbplus_resolve: not yet implemented");
}
/* }}} */

/* {{{ proto int dbplus_tcl(int sid, string script)
    */
PHP_FUNCTION(dbplus_tcl)
{
	zval **sid, **script;
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &sid, &script) == FAILURE){
		WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(sid);
	convert_to_string_ex(script);


	php_error(E_WARNING, "dbplus_tcl: not yet implemented");
}
/* }}} */



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
