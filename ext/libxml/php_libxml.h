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
   | Authors: Shane Caraveo <shane@php.net>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_LIBXML_H
#define PHP_LIBXML_H

#ifdef HAVE_LIBXML
extern zend_module_entry libxml_module_entry;
#define libxml_module_ptr &libxml_module_entry

#include "php_version.h"
#define PHP_LIBXML_VERSION PHP_VERSION

#ifdef PHP_WIN32
#	define PHP_LIBXML_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LIBXML_API __attribute__ ((visibility("default")))
#else
#	define PHP_LIBXML_API
#endif

#include "zend_smart_str.h"
#include <libxml/tree.h>

#define LIBXML_SAVE_NOEMPTYTAG 1<<2

ZEND_BEGIN_MODULE_GLOBALS(libxml)
	zval stream_context;
	smart_str error_buffer;
	zend_llist *error_list;
	zend_fcall_info_cache entity_loader_callback;
	bool entity_loader_disabled;
ZEND_END_MODULE_GLOBALS(libxml)

typedef struct _libxml_doc_props {
	HashTable *classmap;
	bool formatoutput;
	bool validateonparse;
	bool resolveexternals;
	bool preservewhitespace;
	bool substituteentities;
	bool stricterror;
	bool recover;
} libxml_doc_props;

typedef struct {
	size_t modification_nr;
} php_libxml_cache_tag;

typedef struct _php_libxml_ref_obj {
	void *ptr;
	libxml_doc_props *doc_props;
	php_libxml_cache_tag cache_tag;
	int refcount;
	bool is_modern_api_class;
} php_libxml_ref_obj;

typedef struct _php_libxml_node_ptr {
	xmlNodePtr node;
	int	refcount;
	void *_private;
} php_libxml_node_ptr;

typedef struct _php_libxml_node_object {
	php_libxml_node_ptr *node;
	php_libxml_ref_obj *document;
	HashTable *properties;
	zend_object  std;
} php_libxml_node_object;


static inline php_libxml_node_object *php_libxml_node_fetch_object(zend_object *obj) {
	return (php_libxml_node_object *)((char*)(obj) - obj->handlers->offset);
}

static zend_always_inline void php_libxml_invalidate_node_list_cache(php_libxml_ref_obj *doc_ptr)
{
	if (!doc_ptr) {
		return;
	}
#if SIZEOF_SIZE_T == 8
	/* If one operation happens every nanosecond, then it would still require 584 years to overflow
	 * the counter. So we'll just assume this never happens. */
	doc_ptr->cache_tag.modification_nr++;
#else
	size_t new_modification_nr = doc_ptr->cache_tag.modification_nr + 1;
	if (EXPECTED(new_modification_nr > 0)) { /* unsigned overflow; checking after addition results in one less instruction */
		doc_ptr->cache_tag.modification_nr = new_modification_nr;
	}
#endif
}

static zend_always_inline void php_libxml_invalidate_node_list_cache_from_doc(xmlDocPtr docp)
{
	if (docp && docp->_private) { /* docp is NULL for detached nodes */
		php_libxml_node_ptr *node_private = (php_libxml_node_ptr *) docp->_private;
		php_libxml_node_object *object_private = (php_libxml_node_object *) node_private->_private;
		if (object_private) {
			php_libxml_invalidate_node_list_cache(object_private->document);
		}
	}
}

#define Z_LIBXML_NODE_P(zv) php_libxml_node_fetch_object(Z_OBJ_P((zv)))

typedef void * (*php_libxml_export_node) (zval *object);

PHP_LIBXML_API int php_libxml_increment_node_ptr(php_libxml_node_object *object, xmlNodePtr node, void *private_data);
PHP_LIBXML_API int php_libxml_decrement_node_ptr(php_libxml_node_object *object);
PHP_LIBXML_API int php_libxml_increment_doc_ref(php_libxml_node_object *object, xmlDocPtr docp);
PHP_LIBXML_API int php_libxml_decrement_doc_ref(php_libxml_node_object *object);
PHP_LIBXML_API xmlNodePtr php_libxml_import_node(zval *object);
PHP_LIBXML_API zval *php_libxml_register_export(zend_class_entry *ce, php_libxml_export_node export_function);
/* When an explicit freeing of node and children is required */
PHP_LIBXML_API void php_libxml_node_free_list(xmlNodePtr node);
PHP_LIBXML_API void php_libxml_node_free_resource(xmlNodePtr node);
/* When object dtor is called as node may still be referenced */
PHP_LIBXML_API void php_libxml_node_decrement_resource(php_libxml_node_object *object);
PHP_LIBXML_API void php_libxml_error_handler(void *ctx, const char *msg, ...);
PHP_LIBXML_API void php_libxml_ctx_warning(void *ctx, const char *msg, ...);
PHP_LIBXML_API void php_libxml_pretend_ctx_error_ex(int line, int column, const char *msg,...);
PHP_LIBXML_API void php_libxml_ctx_error(void *ctx, const char *msg, ...);
PHP_LIBXML_API int php_libxml_xmlCheckUTF8(const unsigned char *s);
PHP_LIBXML_API void php_libxml_switch_context(zval *context, zval *oldcontext);
PHP_LIBXML_API void php_libxml_issue_error(int level, const char *msg);
PHP_LIBXML_API bool php_libxml_disable_entity_loader(bool disable);
PHP_LIBXML_API void php_libxml_set_old_ns(xmlDocPtr doc, xmlNsPtr ns);

/* Init/shutdown functions*/
PHP_LIBXML_API void php_libxml_initialize(void);
PHP_LIBXML_API void php_libxml_shutdown(void);

#define LIBXML(v) ZEND_MODULE_GLOBALS_ACCESSOR(libxml, v)

#if defined(ZTS) && defined(COMPILE_DL_LIBXML)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

/* Other extension may override the global state options, these global options
 * are copied initially to ctxt->options. Set the options to a known good value.
 * See libxml2 globals.c and parserInternals.c.
 * The unique_name argument allows multiple sanitizes and restores within the
 * same function, even nested is necessary. */
#define PHP_LIBXML_SANITIZE_GLOBALS(unique_name) \
	int xml_old_loadsubset_##unique_name = xmlLoadExtDtdDefaultValue; \
	xmlLoadExtDtdDefaultValue = 0; \
	int xml_old_validate_##unique_name = xmlDoValidityCheckingDefaultValue; \
	xmlDoValidityCheckingDefaultValue = 0; \
	int xml_old_pedantic_##unique_name = xmlPedanticParserDefault(0); \
	int xml_old_substitute_##unique_name = xmlSubstituteEntitiesDefault(0); \
	int xml_old_linenrs_##unique_name = xmlLineNumbersDefault(0); \
	int xml_old_blanks_##unique_name = xmlKeepBlanksDefault(1);

#define PHP_LIBXML_RESTORE_GLOBALS(unique_name) \
	xmlLoadExtDtdDefaultValue = xml_old_loadsubset_##unique_name; \
	xmlDoValidityCheckingDefaultValue = xml_old_validate_##unique_name; \
	(void) xmlPedanticParserDefault(xml_old_pedantic_##unique_name); \
	(void) xmlSubstituteEntitiesDefault(xml_old_substitute_##unique_name); \
	(void) xmlLineNumbersDefault(xml_old_linenrs_##unique_name); \
	(void) xmlKeepBlanksDefault(xml_old_blanks_##unique_name);

/* Alternative for above, working directly on the context and not setting globals.
 * Generally faster because no locking is involved, and this has the advantage that it sets the options to a known good value. */
static zend_always_inline void php_libxml_sanitize_parse_ctxt_options(xmlParserCtxtPtr ctxt)
{
	ctxt->loadsubset = 0;
	ctxt->validate = 0;
	ctxt->pedantic = 0;
	ctxt->replaceEntities = 0;
	ctxt->linenumbers = 0;
	ctxt->keepBlanks = 1;
	ctxt->options = 0;
}

#else /* HAVE_LIBXML */
#define libxml_module_ptr NULL
#endif

#define phpext_libxml_ptr libxml_module_ptr

#endif /* PHP_LIBXML_H */
