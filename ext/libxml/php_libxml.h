/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Shane Caraveo <shane@php.net>                               |
   |          Wez Furlong <wez@thebrainroom.com>                          |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LIBXML_H
#define PHP_LIBXML_H

#if HAVE_LIBXML
extern zend_module_entry libxml_module_entry;
#define libxml_module_ptr &libxml_module_entry

#ifdef PHP_WIN32
#	define PHP_LIBXML_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LIBXML_API __attribute__ ((visibility("default")))
#else
#	define PHP_LIBXML_API
#endif

#include "ext/standard/php_smart_str.h"
#include <libxml/tree.h>

#define LIBXML_SAVE_NOEMPTYTAG 1<<2

ZEND_BEGIN_MODULE_GLOBALS(libxml)
	zval *stream_context;
	smart_str error_buffer;
	zend_llist *error_list;
	struct _php_libxml_entity_resolver {
		zend_fcall_info			fci;
		zend_fcall_info_cache	fcc;
	} entity_loader;
	zend_bool entity_loader_disabled;
ZEND_END_MODULE_GLOBALS(libxml)

typedef struct _libxml_doc_props {
	int formatoutput;
	int validateonparse;
	int resolveexternals;
	int preservewhitespace;
	int substituteentities;
	int stricterror;
	int recover;
	HashTable *classmap;
} libxml_doc_props;

typedef struct _php_libxml_ref_obj {
	void *ptr;
	int   refcount;
	libxml_doc_props *doc_props;
} php_libxml_ref_obj;

typedef struct _php_libxml_node_ptr {
	xmlNodePtr node;
	int	refcount;
	void *_private;
} php_libxml_node_ptr;

typedef struct _php_libxml_node_object {
	zend_object  std;
	php_libxml_node_ptr *node;
	php_libxml_ref_obj *document;
	HashTable *properties;
} php_libxml_node_object;

typedef void * (*php_libxml_export_node) (zval *object TSRMLS_DC);

PHP_LIBXML_API int php_libxml_increment_node_ptr(php_libxml_node_object *object, xmlNodePtr node, void *private_data TSRMLS_DC);
PHP_LIBXML_API int php_libxml_decrement_node_ptr(php_libxml_node_object *object TSRMLS_DC);
PHP_LIBXML_API int php_libxml_increment_doc_ref(php_libxml_node_object *object, xmlDocPtr docp TSRMLS_DC);
PHP_LIBXML_API int php_libxml_decrement_doc_ref(php_libxml_node_object *object TSRMLS_DC);
PHP_LIBXML_API xmlNodePtr php_libxml_import_node(zval *object TSRMLS_DC);
PHP_LIBXML_API int php_libxml_register_export(zend_class_entry *ce, php_libxml_export_node export_function);
/* When an explicit freeing of node and children is required */
PHP_LIBXML_API void php_libxml_node_free_resource(xmlNodePtr node TSRMLS_DC);
/* When object dtor is called as node may still be referenced */
PHP_LIBXML_API void php_libxml_node_decrement_resource(php_libxml_node_object *object TSRMLS_DC);
PHP_LIBXML_API void php_libxml_error_handler(void *ctx, const char *msg, ...);
PHP_LIBXML_API void php_libxml_ctx_warning(void *ctx, const char *msg, ...);
PHP_LIBXML_API void php_libxml_ctx_error(void *ctx, const char *msg, ...);
PHP_LIBXML_API int php_libxml_xmlCheckUTF8(const unsigned char *s);
PHP_LIBXML_API zval *php_libxml_switch_context(zval *context TSRMLS_DC);
PHP_LIBXML_API void php_libxml_issue_error(int level, const char *msg TSRMLS_DC);
PHP_LIBXML_API zend_bool php_libxml_disable_entity_loader(zend_bool disable TSRMLS_DC);

/* Init/shutdown functions*/
PHP_LIBXML_API void php_libxml_initialize(void);
PHP_LIBXML_API void php_libxml_shutdown(void);

#ifdef ZTS
#define LIBXML(v) TSRMG(libxml_globals_id, zend_libxml_globals *, v)
#else
#define LIBXML(v) (libxml_globals.v)
#endif

#else /* HAVE_LIBXML */
#define libxml_module_ptr NULL
#endif

#define phpext_libxml_ptr libxml_module_ptr

#endif /* PHP_LIBXML_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
