/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"


/*
* class domtypeinfo
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#TypeInfo
* Since: DOM Level 3
*/

const zend_function_entry php_dom_typeinfo_class_functions[] = {
	PHP_FE_END
};

/* {{{ attribute protos, not implemented yet */

/* {{{ type_name	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#TypeInfo-typeName
Since:
*/
int dom_typeinfo_type_name_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ type_namespace	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#TypeInfo-typeNamespace
Since:
*/
int dom_typeinfo_type_namespace_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
