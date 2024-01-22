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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_DOM_XPATH_CALLBACKS_H
#define PHP_DOM_XPATH_CALLBACKS_H

#if defined(HAVE_LIBXML) && defined(HAVE_DOM)

#include <libxml/xpathInternals.h>
#include "xml_common.h"

typedef enum {
	PHP_DOM_REG_FUNC_MODE_NONE = 0,
	PHP_DOM_REG_FUNC_MODE_ALL,
	PHP_DOM_REG_FUNC_MODE_SET,
} php_dom_register_functions_mode;

typedef enum {
	PHP_DOM_XPATH_EVALUATE_NODESET_TO_STRING,
	PHP_DOM_XPATH_EVALUATE_NODESET_TO_NODESET,
} php_dom_xpath_nodeset_evaluation_mode;

typedef void (*php_dom_xpath_callbacks_proxy_factory)(xmlNodePtr node, zval *proxy, dom_object *intern, xmlXPathParserContextPtr ctxt);
typedef void (*php_dom_xpath_callbacks_register_func_ctx)(void *ctxt, const zend_string *ns, const zend_string *name);

typedef struct {
	HashTable functions;
	php_dom_register_functions_mode mode;
} php_dom_xpath_callback_ns;

typedef struct {
	php_dom_xpath_callback_ns *php_ns;
	HashTable *namespaces;
	HashTable *node_list;
} php_dom_xpath_callbacks;

typedef enum {
	PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NULLS,
	PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NCNAME,
} php_dom_xpath_callback_name_validation;

PHP_DOM_EXPORT void php_dom_xpath_callbacks_ctor(php_dom_xpath_callbacks *registry);
PHP_DOM_EXPORT void php_dom_xpath_callbacks_dtor(php_dom_xpath_callbacks *registry);
PHP_DOM_EXPORT void php_dom_xpath_callbacks_clean_node_list(php_dom_xpath_callbacks *registry);
PHP_DOM_EXPORT void php_dom_xpath_callbacks_clean_argument_stack(xmlXPathParserContextPtr ctxt, uint32_t num_args);
PHP_DOM_EXPORT void php_dom_xpath_callbacks_get_gc(php_dom_xpath_callbacks *registry, zend_get_gc_buffer *gc_buffer);
PHP_DOM_EXPORT HashTable *php_dom_xpath_callbacks_get_gc_for_whole_object(php_dom_xpath_callbacks *registry, zend_object *object, zval **table, int *n);
PHP_DOM_EXPORT void php_dom_xpath_callbacks_delayed_lib_registration(const php_dom_xpath_callbacks* registry, void *ctxt, php_dom_xpath_callbacks_register_func_ctx register_func);
PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_update_method_handler(php_dom_xpath_callbacks* registry, xmlXPathContextPtr ctxt, zend_string *ns, zend_string *name, const HashTable *callable_ht, php_dom_xpath_callback_name_validation name_validation, php_dom_xpath_callbacks_register_func_ctx register_func);
PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_update_single_method_handler(php_dom_xpath_callbacks* registry, xmlXPathContextPtr ctxt, zend_string *ns, zend_string *name, const zend_fcall_info_cache *fcc, php_dom_xpath_callback_name_validation name_validation, php_dom_xpath_callbacks_register_func_ctx register_func);
PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_call_php_ns(php_dom_xpath_callbacks *xpath_callbacks, xmlXPathParserContextPtr ctxt, int num_args, php_dom_xpath_nodeset_evaluation_mode evaluation_mode, dom_object *intern, php_dom_xpath_callbacks_proxy_factory proxy_factory);
PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_call_custom_ns(php_dom_xpath_callbacks *xpath_callbacks, xmlXPathParserContextPtr ctxt, int num_args, php_dom_xpath_nodeset_evaluation_mode evaluation_mode, dom_object *intern, php_dom_xpath_callbacks_proxy_factory proxy_factory);

#endif
#endif
