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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_dom.h"


/*
* class domexception 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-17189187
* Since: 
*/

extern zend_class_entry *dom_domexception_class_entry;

zend_function_entry php_dom_domexception_class_functions[] = {
	{NULL, NULL, NULL}
};

/* {{{ php_dom_throw_error */
void php_dom_throw_error(int error_code, zval **retval TSRMLS_DC)
{
	zval *dom_exception;
	char *error_message;

	ALLOC_ZVAL(dom_exception);
	Z_TYPE_P(dom_exception) = IS_OBJECT;
	object_init_ex(dom_exception, dom_domexception_class_entry);
	dom_exception->refcount = 1;
	dom_exception->is_ref = 1;
	switch (error_code)
	{
		case INDEX_SIZE_ERR:
			error_message = "Index Size Error";
			break;
		case DOMSTRING_SIZE_ERR:
			error_message = "DOM String Size Error";
			break;
		case HIERARCHY_REQUEST_ERR:
			error_message = "Hierarchy Request Error";
			break;
		case WRONG_DOCUMENT_ERR:
			error_message = "Wrong Document Error";
			break;
		case INVALID_CHARACTER_ERR:
			error_message = "Invalid Character Error";
			break;
		case NO_DATA_ALLOWED_ERR:
			error_message = "No Data Allowed Error";
			break;
		case NO_MODIFICATION_ALLOWED_ERR:
			error_message = "No Modification Allowed Error";
			break;
		case NOT_FOUND_ERR:
			error_message = "Not Found Error";
			break;
		case NOT_SUPPORTED_ERR:
			error_message = "Not Supported Error";
			break;
		case INUSE_ATTRIBUTE_ERR:
			error_message = "Inuse Attribute Error";
			break;
		case INVALID_STATE_ERR:
			error_message = "Invalid State Error";
			break;
		case SYNTAX_ERR:
			error_message = "Syntax Error";
			break;
		case INVALID_MODIFICATION_ERR:
			error_message = "Invalid Modification Error";
			break;
		case NAMESPACE_ERR:
			error_message = "Namespace Error";
			break;
		case INVALID_ACCESS_ERR:
			error_message = "Invalid Access Error";
			break;
		case VALIDATION_ERR:
			error_message = "Validation Error";
			break;
		default:
			error_message = "Unhandled Error";
	}

	add_property_long(dom_exception, "code", error_code);
	add_property_stringl(dom_exception, "message", error_message, strlen(error_message), 1);
	EG(exception) = dom_exception;
}
/* }}} end php_dom_throw_error */
