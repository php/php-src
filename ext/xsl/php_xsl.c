/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | https://www.php.net/license/3_01.txt                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Christian Stocker <chregu@php.net>                           |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xsl.h"
#include "php_xsl_arginfo.h"

zend_class_entry *xsl_xsltprocessor_class_entry;
static zend_object_handlers xsl_object_handlers;

static const zend_module_dep xsl_deps[] = {
	ZEND_MOD_REQUIRED("libxml")
	ZEND_MOD_REQUIRED("dom")
	ZEND_MOD_END
};

/* {{{ xsl_module_entry */
zend_module_entry xsl_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	xsl_deps,
	"xsl",
	NULL,
	PHP_MINIT(xsl),
	PHP_MSHUTDOWN(xsl),
	NULL,
	NULL,
	PHP_MINFO(xsl),
	PHP_XSL_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XSL
ZEND_GET_MODULE(xsl)
#endif

static HashTable *xsl_objects_get_gc(zend_object *object, zval **table, int *n)
{
	xsl_object *intern = php_xsl_fetch_object(object);
	return php_dom_xpath_callbacks_get_gc_for_whole_object(&intern->xpath_callbacks, object, table, n);
}

/* {{{ xsl_objects_free_storage */
void xsl_objects_free_storage(zend_object *object)
{
	xsl_object *intern = php_xsl_fetch_object(object);

	zend_object_std_dtor(&intern->std);

	if (intern->parameter) {
		zend_hash_destroy(intern->parameter);
		FREE_HASHTABLE(intern->parameter);
	}

	php_dom_xpath_callbacks_dtor(&intern->xpath_callbacks);

	if (intern->doc) {
		php_libxml_decrement_doc_ref(intern->doc);
		efree(intern->doc);
	}

	if (intern->ptr) {
		/* free wrapper */
		if (((xsltStylesheetPtr) intern->ptr)->_private != NULL) {
			((xsltStylesheetPtr) intern->ptr)->_private = NULL;
		}

		xsltFreeStylesheet((xsltStylesheetPtr) intern->ptr);
		intern->ptr = NULL;
	}
	if (intern->profiling) {
		efree(intern->profiling);
	}
}
/* }}} */

/* {{{ xsl_objects_new */
zend_object *xsl_objects_new(zend_class_entry *class_type)
{
	xsl_object *intern;

	intern = zend_object_alloc(sizeof(xsl_object), class_type);
	intern->securityPrefs = XSL_SECPREF_DEFAULT;

	zend_object_std_init(&intern->std, class_type);
	object_properties_init(&intern->std, class_type);
	intern->parameter = zend_new_array(0);
	php_dom_xpath_callbacks_ctor(&intern->xpath_callbacks);

	return &intern->std;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(xsl)
{
	memcpy(&xsl_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	xsl_object_handlers.offset = XtOffsetOf(xsl_object, std);
	xsl_object_handlers.clone_obj = NULL;
	xsl_object_handlers.free_obj = xsl_objects_free_storage;
	xsl_object_handlers.get_gc = xsl_objects_get_gc;

	xsl_xsltprocessor_class_entry = register_class_XSLTProcessor();
	xsl_xsltprocessor_class_entry->create_object = xsl_objects_new;
	xsl_xsltprocessor_class_entry->default_object_handlers = &xsl_object_handlers;

#ifdef HAVE_XSL_EXSLT
	exsltRegisterAll();
#endif

	xsltRegisterExtModuleFunction ((const xmlChar *) "functionString",
				   (const xmlChar *) "http://php.net/xsl",
				   xsl_ext_function_string_php);
	xsltRegisterExtModuleFunction ((const xmlChar *) "function",
				   (const xmlChar *) "http://php.net/xsl",
				   xsl_ext_function_object_php);
	xsltSetGenericErrorFunc(NULL, php_libxml_error_handler);

	register_php_xsl_symbols(module_number);

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
static void xsl_object_set_data(void *obj, zval *wrapper)
{
	((xsltStylesheetPtr) obj)->_private = wrapper;
}
/* }}} */

/* {{{ php_xsl_set_object */
void php_xsl_set_object(zval *wrapper, void *obj)
{
	xsl_object *object;

	object = Z_XSL_P(wrapper);
	object->ptr = obj;
	xsl_object_set_data(obj, wrapper);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(xsl)
{
	xsltUnregisterExtModuleFunction ((const xmlChar *) "functionString",
				   (const xmlChar *) "http://php.net/xsl");
	xsltUnregisterExtModuleFunction ((const xmlChar *) "function",
				   (const xmlChar *) "http://php.net/xsl");
	xsltSetGenericErrorFunc(NULL, NULL);
	xsltCleanupGlobals();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(xsl)
{
	php_info_print_table_start();
	{
		char buffer[128];
		int major, minor, subminor;

		php_info_print_table_row(2, "XSL", "enabled");
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
#ifdef HAVE_XSL_EXSLT
	php_info_print_table_row(2, "EXSLT", "enabled");
	php_info_print_table_row(2, "libexslt Version", LIBEXSLT_DOTTED_VERSION);
#endif
	php_info_print_table_end();
}
/* }}} */
