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
* class domlocator
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Interfaces-DOMLocator
* Since: DOM Level 3
*/

const zend_function_entry php_dom_domlocator_class_functions[] = {
	PHP_FE_END
};

/* {{{ attribute protos, not implemented yet */

/* {{{ line_number	long
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMLocator-line-number
Since:
*/
int dom_domlocator_line_number_read(dom_object *obj, zval *retval)
{
	ZVAL_STRING(retval, "TEST");
	return SUCCESS;
}

/* }}} */

/* {{{ column_number	long
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMLocator-column-number
Since:
*/
int dom_domlocator_column_number_read(dom_object *obj, zval *retval)
{
	ZVAL_STRING(retval, "TEST");
	return SUCCESS;
}

/* }}} */

/* {{{ offset	long
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMLocator-offset
Since:
*/
int dom_domlocator_offset_read(dom_object *obj, zval *retval)
{
	ZVAL_STRING(retval, "TEST");
	return SUCCESS;
}

/* }}} */

/* {{{ related_node	node
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMLocator-node
Since:
*/
int dom_domlocator_related_node_read(dom_object *obj, zval *retval)
{
	ZVAL_STRING(retval, "TEST");
	return SUCCESS;
}

/* }}} */

/* {{{ uri	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMLocator-uri
Since:
*/
int dom_domlocator_uri_read(dom_object *obj, zval *retval)
{
	ZVAL_STRING(retval, "TEST");
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
