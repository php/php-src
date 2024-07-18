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
#include <config.h>
#endif

#include "php.h"
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

void xsl_free_sheet(xsl_object *intern)
{
	if (intern->ptr) {
		xsltStylesheetPtr sheet = intern->ptr;

		/* Free wrapper */
		if (sheet->_private != NULL) {
			sheet->_private = NULL;
		}

		xsltFreeStylesheet(sheet);
		intern->ptr = NULL;
	}
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

	xsl_free_sheet(intern);

	if (intern->doc) {
		php_libxml_decrement_doc_ref(intern->doc);
		efree(intern->doc);
	}

	if (intern->sheet_ref_obj) {
		php_libxml_decrement_doc_ref_directly(intern->sheet_ref_obj);
	}

	if (intern->profiling) {
		zend_string_release(intern->profiling);
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

	/* Default initialize properties that could not be default initialized at the stub because they depend on library
	 * configuration parameters. */
	ZVAL_LONG(xsl_prop_max_template_depth(&intern->std), xsltMaxDepth);
	ZVAL_LONG(xsl_prop_max_template_vars(&intern->std), xsltMaxVars);

	return &intern->std;
}
/* }}} */

#if ZEND_DEBUG
# define XSL_DEFINE_PROP_ACCESSOR(c_name, php_name, prop_index) \
	zval *xsl_prop_##c_name(zend_object *object) \
	{ \
		zend_string *prop_name = ZSTR_INIT_LITERAL(php_name, false); \
		const zend_property_info *prop_info = zend_get_property_info(xsl_xsltprocessor_class_entry, prop_name, 0); \
		zend_string_release_ex(prop_name, false); \
		ZEND_ASSERT(OBJ_PROP_TO_NUM(prop_info->offset) == (prop_index)); \
		return OBJ_PROP_NUM(object, (prop_index)); \
	}
#else
# define XSL_DEFINE_PROP_ACCESSOR(c_name, php_name, prop_index) \
	zval *xsl_prop_##c_name(zend_object *object) \
	{ \
		return OBJ_PROP_NUM(object, (prop_index)); \
	}
#endif

XSL_DEFINE_PROP_ACCESSOR(max_template_depth, "maxTemplateDepth", 2)
XSL_DEFINE_PROP_ACCESSOR(max_template_vars, "maxTemplateVars", 3)

static zval *xsl_objects_write_property_with_validation(zend_object *object, zend_string *member, zval *value, void **cache_slot, zval *property)
{
	/* Read old value so we can restore it if necessary. The value is not refcounted as its type is IS_LONG. */
	ZEND_ASSERT(Z_TYPE_P(property) == IS_LONG);
	zend_long old_property_value = Z_LVAL_P(property);

	/* Write new property, which will also potentially perform coercions. */
	zend_std_write_property(object, member, value, NULL);

	/* Validate value *after* coercions have been performed, and restore the old value if necessary. */
	if (UNEXPECTED(Z_LVAL_P(property) < 0)) {
		Z_LVAL_P(property) = old_property_value;
		zend_value_error("%s::$%s must be greater than or equal to 0", ZSTR_VAL(object->ce->name), ZSTR_VAL(member));
		return &EG(error_zval);
	}

	return property;
}

static zval *xsl_objects_write_property(zend_object *object, zend_string *member, zval *value, void **cache_slot)
{
	/* Extra validation for maxTemplateDepth and maxTemplateVars */
	if (zend_string_equals_literal(member, "maxTemplateDepth")) {
		zval *property = xsl_prop_max_template_depth(object);
		return xsl_objects_write_property_with_validation(object, member, value, cache_slot, property);
	} else if (zend_string_equals_literal(member, "maxTemplateVars")) {
		zval *property = xsl_prop_max_template_vars(object);
		return xsl_objects_write_property_with_validation(object, member, value, cache_slot, property);
	} else {
		return zend_std_write_property(object, member, value, cache_slot);
	}
}

static bool xsl_is_validated_property(const zend_string *member)
{
	return zend_string_equals_literal(member, "maxTemplateDepth") || zend_string_equals_literal(member, "maxTemplateVars");
}

static zval *xsl_objects_get_property_ptr_ptr(zend_object *object, zend_string *member, int type, void **cache_slot)
{
	if (xsl_is_validated_property(member)) {
		return NULL;
	}

	return zend_std_get_property_ptr_ptr(object, member, type, cache_slot);
}

static zval *xsl_objects_read_property(zend_object *object, zend_string *member, int type, void **cache_slot, zval *rv)
{
	/* read handler is being called as a fallback after get_property_ptr_ptr returned NULL */
	if (type != BP_VAR_IS && type != BP_VAR_R && xsl_is_validated_property(member)) {
		zend_throw_error(NULL, "Indirect modification of %s::$%s is not allowed", ZSTR_VAL(object->ce->name), ZSTR_VAL(member));
		return &EG(uninitialized_zval);
	}

	return zend_std_read_property(object, member, type, cache_slot, rv);
}

static void xsl_objects_unset_property(zend_object *object, zend_string *member, void **cache_slot)
{
	if (xsl_is_validated_property(member)) {
		zend_throw_error(NULL, "Cannot unset %s::$%s", ZSTR_VAL(object->ce->name), ZSTR_VAL(member));
		return;
	}

	zend_std_unset_property(object, member, cache_slot);
}

/* Tries to output an error message where a part was replaced by another string.
 * Returns true if the search string was found and the error message with replacement was outputted.
 * Return false otherwise. */
static bool xsl_try_output_replaced_error_message(
	void *ctx,
	const char *msg,
	const char *search,
	size_t search_len,
	const char *replace
)
{
	const char *msg_replace_location = strstr(msg, search);
	if (msg_replace_location != NULL) {
		php_libxml_ctx_error(ctx, "%.*s%s%s", (int) (msg_replace_location - msg), msg, replace, msg_replace_location + search_len);
		return true;
	}
	return false;
}

/* Helper macro so the string length doesn't need to be passed separately.
 * Only allows literal strings for `search` and `replace`. */
#define XSL_TRY_OUTPUT_REPLACED_ERROR_MESSAGE(ctx, msg, search, replace) \
	xsl_try_output_replaced_error_message(ctx, msg, "" search, sizeof("" search) - 1, "" replace)

/* We want to output PHP-tailored error messages for some libxslt error messages, such that
 * the errors refer to PHP properties instead of libxslt-specific fields. */
static void xsl_libxslt_error_handler(void *ctx, const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	if (strcmp(msg, "%s") == 0) {
		/* Adjust error message to be more descriptive */
		const char *msg_arg = va_arg(args, const char *);
		bool output = XSL_TRY_OUTPUT_REPLACED_ERROR_MESSAGE(ctx, msg_arg, "xsltMaxDepth (--maxdepth)", "$maxTemplateDepth")
				   || XSL_TRY_OUTPUT_REPLACED_ERROR_MESSAGE(ctx, msg_arg, "maxTemplateVars (--maxvars)", "$maxTemplateVars");

		if (!output) {
			php_libxml_ctx_error(ctx, "%s", msg_arg);
		}
	} else {
		php_libxml_error_handler_va(PHP_LIBXML_ERROR, ctx, msg, args);
	}

	va_end(args);
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(xsl)
{
	memcpy(&xsl_object_handlers, &std_object_handlers, sizeof(zend_object_handlers));
	xsl_object_handlers.offset = XtOffsetOf(xsl_object, std);
	xsl_object_handlers.clone_obj = NULL;
	xsl_object_handlers.free_obj = xsl_objects_free_storage;
	xsl_object_handlers.get_gc = xsl_objects_get_gc;
	xsl_object_handlers.write_property = xsl_objects_write_property;
	xsl_object_handlers.get_property_ptr_ptr = xsl_objects_get_property_ptr_ptr;
	xsl_object_handlers.read_property = xsl_objects_read_property;
	xsl_object_handlers.unset_property = xsl_objects_unset_property;

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
	xsltSetGenericErrorFunc(NULL, xsl_libxslt_error_handler);

	register_php_xsl_symbols(module_number);

	return SUCCESS;
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
