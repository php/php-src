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
  | Author: Christian Stocker <chregu@php.net>                           |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_execute_locks.h"
#include "ext/standard/info.h"
#include "php_xsl.h"

/* If you declare any globals in php_xsl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(xsl)
*/

/* True global resources - no need for thread safety here */

static HashTable classes;
/* not used right now  
static zend_object_handlers xsl_object_handlers;
static HashTable xsl_xsltprocessor_prop_handlers;
*/

/* {{{ xsl_functions[]
 *
 * Every user visible function must have an entry in xsl_functions[].
 */
function_entry xsl_functions[] = {
	{NULL, NULL, NULL}  /* Must be the last line in xsl_functions[] */
};
/* }}} */

/* {{{ xsl_module_entry
 */
zend_module_entry xsl_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"xsl",
	xsl_functions,
	PHP_MINIT(xsl),
	PHP_MSHUTDOWN(xsl),
	PHP_RINIT(xsl),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(xsl),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(xsl),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XSL
ZEND_GET_MODULE(xsl)
#endif


/************
* copied from php_dom .... should be in a common file...
**********/


typedef int (*dom_read_t)(xsl_object *obj, zval **retval TSRMLS_DC);
typedef int (*dom_write_t)(xsl_object *obj, zval *newval TSRMLS_DC);

typedef struct _dom_prop_handler {
	dom_read_t read_func;
	dom_write_t write_func;
} dom_prop_handler;

/* end copied from php_dom.c */
	
/* {{{ xsl_objects_dtor */
void xsl_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
	/* TODO */
	xsl_object *intern = (xsl_object *)object;

	zend_hash_destroy(intern->std.properties);
	FREE_HASHTABLE(intern->std.properties);

	zend_hash_destroy(intern->parameter);
	FREE_HASHTABLE(intern->parameter);
	
	if (intern->ptr) {
		/* free wrapper */
		if (((xsltStylesheetPtr) intern->ptr)->_private != NULL) {
			((xsltStylesheetPtr) intern->ptr)->_private = NULL;   
		}
		/* libxslt uses _private for itself, so turning of the deregistering is maybe a solution 
		   we copied the doc at import, so it shouldn't be possible to be used from php land */
		xsltFreeStylesheet((xsltStylesheetPtr) intern->ptr);
		intern->ptr = NULL;
	}
	efree(object);
}
/* }}} */
/* {{{ xsl_objects_new */
zend_object_value xsl_objects_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value retval;
	xsl_object *intern;
	zend_class_entry *base_class;
	zval *tmp;

	intern = emalloc(sizeof(xsl_object));
	intern->std.ce = class_type;
	intern->std.in_get = 0;
	intern->std.in_set = 0;
	intern->ptr = NULL;
	intern->node_list = NULL;
	intern->prop_handler = NULL;
	intern->parameter = NULL;
	
	base_class = class_type;
	while(base_class->type != ZEND_INTERNAL_CLASS && base_class->parent != NULL) {
		base_class = base_class->parent;
	}

	zend_hash_find(&classes, base_class->name , base_class->name_length + 1, &intern->prop_handler);

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	ALLOC_HASHTABLE(intern->parameter);
	zend_hash_init(intern->parameter, 0, NULL, ZVAL_PTR_DTOR, 0);
	retval.handle = zend_objects_store_put(intern, xsl_objects_dtor, dom_objects_clone TSRMLS_CC);
	retval.handlers = &dom_object_handlers;
	return retval;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xsl)
{
	
	zend_class_entry ce;
	zend_hash_init(&classes, 0, NULL, NULL, 1);
	
	memcpy(&dom_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	dom_object_handlers.read_property = dom_read_property;
	dom_object_handlers.write_property = dom_write_property;
	
	REGISTER_XSL_CLASS(ce, "xsltprocessor", NULL, php_xsl_xsltprocessor_class_functions, xsl_xsltprocessor_class_entry);
	return SUCCESS;
}
/* }}} */

/* {{{ xsl_object_get_data */
zval *xsl_object_get_data(void *obj)
{
	zval *dom_wrapper;
	dom_wrapper = ((xsltStylesheetPtr) obj)->_private;
	return dom_wrapper;
}
/* }}} */

/* {{{ xsl_object_set_data */
static void xsl_object_set_data(void *obj, zval *wrapper TSRMLS_DC)
{
	((xsltStylesheetPtr) obj)->_private = wrapper;
}
/* }}} */

/* {{{ php_xsl_set_object */
void php_xsl_set_object(zval *wrapper, void *obj TSRMLS_DC)
{
	xsl_object *object;

	object = (xsl_object *)zend_objects_get_address(wrapper TSRMLS_CC);
	object->ptr = obj;
	xsl_object_set_data(obj, wrapper TSRMLS_CC);
}
/* }}} */

/* {{{ php_xsl_create_object */
zval *php_xsl_create_object(xsltStylesheetPtr obj, int *found, zval *wrapper_in, zval *return_value  TSRMLS_DC)
{
	zval *wrapper;
	zend_class_entry *ce;

	*found = 0;

	if (!obj) {
		if(!wrapper_in) {
			ALLOC_ZVAL(wrapper);
		} else {
			wrapper = wrapper_in;
		}
		ZVAL_NULL(wrapper);
		return wrapper;
	}

	if ((wrapper = (zval *) xsl_object_get_data((void *) obj))) {
		zval_add_ref(&wrapper);
		*found = 1;
		return wrapper;
	}

	if(!wrapper_in) {
		wrapper = return_value;
	} else {
		wrapper = wrapper_in;
	}

	
	ce = xsl_xsltprocessor_class_entry;

	if(!wrapper_in) {
		object_init_ex(wrapper, ce);
	}
	php_xsl_set_object(wrapper, (void *) obj TSRMLS_CC);
	return (wrapper);
}
/* }}} */




/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(xsl)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	xsltCleanupGlobals();

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(xsl)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(xsl)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(xsl)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "XML/XSLT", "enabled");
	{
		char buffer[128];
		int major, minor, subminor;

		php_info_print_table_row(2, "DOM/XSLT", "enabled");
		major = xsltLibxsltVersion/10000;
		minor = (xsltLibxsltVersion - major * 10000) / 100;
		subminor = (xsltLibxsltVersion - major * 10000 - minor * 100);
		snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
		php_info_print_table_row(2, "libxslt Version", buffer);
		major = xsltLibxmlVersion/10000;
		minor = (xsltLibxmlVersion - major * 10000) / 100;
		subminor = (xsltLibxmlVersion - major * 10000 - minor * 100);
		snprintf(buffer, 128, "%d.%d.%d", major, minor, subminor);
		php_info_print_table_row(2, "libxslt compiled against libxml Version", buffer);
	}
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
