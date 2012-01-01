/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"

/*
* class DOMNotation extends DOMNode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-5431D1B9
* Since: 
*/

const zend_function_entry php_dom_notation_class_functions[] = {
	PHP_FE_END
};

/* {{{ attribute protos, not implemented yet */

/* {{{ publicId	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-54F2B4D0
Since: 
*/
int dom_notation_public_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlEntityPtr nodep;

	nodep = (xmlEntityPtr) dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (nodep->ExternalID) {
		ZVAL_STRING(*retval, (char *) (nodep->ExternalID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ systemId	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-E8AAB1D0
Since: 
*/
int dom_notation_system_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlEntityPtr nodep;

	nodep = (xmlEntityPtr) dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (nodep->SystemID) {
		ZVAL_STRING(*retval, (char *) (nodep->SystemID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}

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
