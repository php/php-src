/*
  +----------------------------------------------------------------------+
  | PHP Version 4                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2003 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
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
#include "ext/standard/info.h"
#include "php_xsl.h"

static void xsl_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs);
static void xsl_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs);

/* If you declare any globals in php_xsl.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(xsl)
*/

static zend_object_handlers xsl_object_handlers;

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

/* {{{ xsl_objects_clone */
void xsl_objects_clone(void *object, void **object_clone TSRMLS_DC)
{
	/* TODO */
}
/* }}} */

/* {{{ xsl_objects_dtor */
void xsl_objects_dtor(void *object, zend_object_handle handle TSRMLS_DC)
{
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
	zval *tmp;

	intern = emalloc(sizeof(xsl_object));
	intern->std.ce = class_type;
	intern->std.in_get = 0;
	intern->std.in_set = 0;
	intern->ptr = NULL;
	intern->prop_handler = NULL;
	intern->parameter = NULL;

	ALLOC_HASHTABLE(intern->std.properties);
	zend_hash_init(intern->std.properties, 0, NULL, ZVAL_PTR_DTOR, 0);
	zend_hash_copy(intern->std.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	ALLOC_HASHTABLE(intern->parameter);
	zend_hash_init(intern->parameter, 0, NULL, ZVAL_PTR_DTOR, 0);
	retval.handle = zend_objects_store_put(intern, xsl_objects_dtor, xsl_objects_clone TSRMLS_CC);
	intern->handle = retval.handle;
	retval.handlers = &xsl_object_handlers;
	return retval;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(xsl)
{
	
	zend_class_entry ce;
	
	memcpy(&xsl_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	
	REGISTER_XSL_CLASS(ce, "xsltprocessor", NULL, php_xsl_xsltprocessor_class_functions, xsl_xsltprocessor_class_entry);
#if HAVE_XSL_EXSLT
	exsltRegisterAll();
#endif
	xsltRegisterExtModuleFunction ((const xmlChar *) "functionString",
				   (const xmlChar *) "http://php.net/xsl",
				   xsl_ext_function_string_php);
	xsltRegisterExtModuleFunction ((const xmlChar *) "function",
				   (const xmlChar *) "http://php.net/xsl",
				   xsl_ext_function_object_php);

	return SUCCESS;
}
/* }}} */


static void xsl_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, int type)
{
	xsltTransformContextPtr tctxt;
	zval **args;
	zval *retval;
	int result, i, ret;
	zend_fcall_info fci;
	zval handler;
	xmlXPathObjectPtr obj;
	char *str;

	TSRMLS_FETCH();

	tctxt = xsltXPathGetTransformContext(ctxt);
	if (tctxt == NULL) {
		xsltGenericError(xsltGenericErrorContext,
		"xsltExtFunctionTest: failed to get the transformation context\n");
		return;
	}

	args = safe_emalloc(sizeof(zval **), nargs - 1, 0);
	for (i = 0; i < nargs - 1; i++) {
		obj = valuePop(ctxt);
		MAKE_STD_ZVAL(args[i]);
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(args[i],  obj->stringval, 1);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(args[i],  obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(args[i], obj->floatval);
				break;
			case XPATH_NODESET:
				if (type == 1) {
					str = xmlXPathCastToString(obj);
					ZVAL_STRING(args[i], str, 1);
					xmlFree(str);
				} else if (type == 2) {
					int j;
					dom_object *intern;
					array_init(args[i]);
					if (obj->nodesetval->nodeNr > 0) {
						intern = (dom_object *) php_dom_object_get_data((void *) obj->nodesetval->nodeTab[0]->doc);
						for (j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval *child;
							
							MAKE_STD_ZVAL(child);
							/* not sure, if we need this... it's copied from xpath.c */
							if (node->type == XML_NAMESPACE_DECL) {
								xmlNsPtr curns;
								xmlNodePtr nsparent;
								
								nsparent = node->_private;
								curns = xmlNewNs(NULL, node->name, NULL);
								if (node->children) {
									curns->prefix = xmlStrdup((char *) node->children);
								}
								if (node->children) {
									node = xmlNewDocNode(node->doc, NULL, (char *) node->children, node->name);
								} else {
									node = xmlNewDocNode(node->doc, NULL, "xmlns", node->name);
								}
								node->type = XML_NAMESPACE_DECL;
								node->parent = nsparent;
								node->ns = curns;
							}
							child = php_dom_create_object(node, &ret, NULL, child, intern TSRMLS_CC);
							add_next_index_zval(args[i], child);
						}
					}
				}
				break;
			default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "php:function object type %d is not supported yet", obj->type);
			ZVAL_STRING(args[i], "", 0);
		}
		xmlXPathFreeObject(obj);
	}
	
	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	
	obj = valuePop(ctxt);
	INIT_PZVAL(&handler);
	ZVAL_STRING(&handler, obj->stringval, 1);
	xmlXPathFreeObject(obj);
	
	fci.function_name = &handler;
	fci.symbol_table = NULL;
	fci.object_pp = NULL;
	fci.retval_ptr_ptr = &retval;
	fci.param_count = nargs - 1;
	fci.params = &args;
	fci.no_separation = 0;
	/*fci.function_handler_cache = &function_ptr;*/
	
	result = zend_call_function(&fci, NULL TSRMLS_CC);
	if (result == FAILURE) {
		if (Z_TYPE(handler) == IS_STRING) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(&handler));
		} 
	} else {
		if (retval->type == IS_BOOL) {
			valuePush(ctxt, xmlXPathNewBoolean(retval->value.lval));
		} else {
			convert_to_string_ex(&retval);
			valuePush(ctxt, xmlXPathNewString( Z_STRVAL_P(retval)));
		}
		zval_ptr_dtor(&retval);
	}
	zval_dtor(&handler);
	for (i = 0; i < nargs - 1; i++) {
		zval_ptr_dtor(&args[i]);
	}
	efree(args);
}

static void
xsl_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs)
{
	xsl_ext_function_php(ctxt, nargs, 1);
}

static void
xsl_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs)
{
	xsl_ext_function_php(ctxt, nargs, 2);
}

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
#if HAVE_XSL_EXSLT
		php_info_print_table_row(2, "EXSLT", "enabled");
		php_info_print_table_row(2, "libexslt Version", LIBEXSLT_DOTTED_VERSION);
#endif
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
