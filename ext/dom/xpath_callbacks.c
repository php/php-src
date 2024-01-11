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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   |          Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)

#include "php_dom.h"
#include <libxml/parserInternals.h>

static void xpath_callbacks_entry_dtor(zval *zv)
{
	zend_fcall_info_cache *fcc = Z_PTR_P(zv);
	zend_fcc_dtor(fcc);
	efree(fcc);
}

PHP_DOM_EXPORT void php_dom_xpath_callback_ns_ctor(php_dom_xpath_callback_ns *ns)
{
	zend_hash_init(&ns->functions, 0, NULL, xpath_callbacks_entry_dtor, false);
	ns->mode = PHP_DOM_REG_FUNC_MODE_NONE;
}

PHP_DOM_EXPORT void php_dom_xpath_callback_ns_dtor(php_dom_xpath_callback_ns *ns)
{
	zend_hash_destroy(&ns->functions);
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_ctor(php_dom_xpath_callbacks *registry)
{
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_clean_node_list(php_dom_xpath_callbacks *registry)
{
	if (registry->node_list) {
		zend_hash_destroy(registry->node_list);
		FREE_HASHTABLE(registry->node_list);
		registry->node_list = NULL;
	}
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_clean_argument_stack(xmlXPathParserContextPtr ctxt, uint32_t num_args)
{
	for (uint32_t i = 0; i < num_args; i++) {
		xmlXPathObjectPtr obj = valuePop(ctxt);
		xmlXPathFreeObject(obj);
	}

	/* Push sentinel value */
	valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_dtor(php_dom_xpath_callbacks *registry)
{
	if (registry->php_ns) {
		php_dom_xpath_callback_ns_dtor(registry->php_ns);
		efree(registry->php_ns);
	}
	if (registry->namespaces) {
		php_dom_xpath_callback_ns *ns;
		ZEND_HASH_MAP_FOREACH_PTR(registry->namespaces, ns) {
			php_dom_xpath_callback_ns_dtor(ns);
			efree(ns);
		} ZEND_HASH_FOREACH_END();

		zend_hash_destroy(registry->namespaces);
		FREE_HASHTABLE(registry->namespaces);
	}
	php_dom_xpath_callbacks_clean_node_list(registry);
}

static void php_dom_xpath_callback_ns_get_gc(php_dom_xpath_callback_ns *ns, zend_get_gc_buffer *gc_buffer)
{
	zend_fcall_info_cache *entry;
	ZEND_HASH_MAP_FOREACH_PTR(&ns->functions, entry) {
		zend_get_gc_buffer_add_fcc(gc_buffer, entry);
	} ZEND_HASH_FOREACH_END();
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_get_gc(php_dom_xpath_callbacks *registry, zend_get_gc_buffer *gc_buffer)
{
	if (registry->php_ns) {
		php_dom_xpath_callback_ns_get_gc(registry->php_ns, gc_buffer);
	}
	if (registry->namespaces) {
		php_dom_xpath_callback_ns *ns;
		ZEND_HASH_MAP_FOREACH_PTR(registry->namespaces, ns) {
			php_dom_xpath_callback_ns_get_gc(ns, gc_buffer);
		} ZEND_HASH_FOREACH_END();
	}
}

PHP_DOM_EXPORT HashTable *php_dom_xpath_callbacks_get_gc_for_whole_object(php_dom_xpath_callbacks *registry, zend_object *object, zval **table, int *n)
{
	if (registry->php_ns || registry->namespaces) {
		zend_get_gc_buffer *gc_buffer = zend_get_gc_buffer_create();
		php_dom_xpath_callbacks_get_gc(registry, gc_buffer);
		zend_get_gc_buffer_use(gc_buffer, table, n);

		if (object->properties == NULL && object->ce->default_properties_count == 0) {
			return NULL;
		} else {
			return zend_std_get_properties(object);
		}
	} else {
		return zend_std_get_gc(object, table, n);
	}
}

static bool php_dom_xpath_is_callback_name_valid(const zend_string *name, php_dom_xpath_callback_name_validation name_validation)
{
	if (ZSTR_LEN(name) == 0) {
		return false;
	}

	if (name_validation == PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NULLS || name_validation == PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NCNAME) {
		if (zend_str_has_nul_byte(name)) {
			return false;
		}
	}

	if (name_validation == PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NCNAME) {
		if (xmlValidateNCName((xmlChar *) ZSTR_VAL(name), /* pass 0 to disallow spaces */ 0) != 0) {
			return false;
		}
	}

	return true;
}

static bool php_dom_xpath_is_callback_name_valid_and_throw(const zend_string *name, php_dom_xpath_callback_name_validation name_validation, bool is_array)
{
	if (!php_dom_xpath_is_callback_name_valid(name, name_validation)) {
		if (is_array) {
			zend_argument_value_error(1, "must be an array containing valid callback names");
		} else {
			zend_argument_value_error(2, "must be a valid callback name");
		}
		return false;
	}
	return true;
}

PHP_DOM_EXPORT void php_dom_xpath_callbacks_delayed_lib_registration(const php_dom_xpath_callbacks* registry, void *ctxt, php_dom_xpath_callbacks_register_func_ctx register_func)
{
	if (registry->namespaces) {
		zend_string *namespace;
		php_dom_xpath_callback_ns *ns;
		ZEND_HASH_MAP_FOREACH_STR_KEY_PTR(registry->namespaces, namespace, ns) {
			zend_string *name;
			ZEND_HASH_MAP_FOREACH_STR_KEY(&ns->functions, name) {
				register_func(ctxt, namespace, name);
			} ZEND_HASH_FOREACH_END();
		} ZEND_HASH_FOREACH_END();
	}
}

static zend_result php_dom_xpath_callback_ns_update_method_handler(
	php_dom_xpath_callback_ns* ns,
	xmlXPathContextPtr ctxt,
	const zend_string *namespace,
	zend_string *name,
	const HashTable *callable_ht,
	php_dom_xpath_callback_name_validation name_validation,
	php_dom_xpath_callbacks_register_func_ctx register_func
)
{
	zval *entry, registered_value;

	if (callable_ht) {
		zend_string *key;
		ZEND_HASH_FOREACH_STR_KEY_VAL(callable_ht, key, entry) {
			zend_fcall_info_cache* fcc = emalloc(sizeof(zend_fcall_info));
			char *error;
			if (!zend_is_callable_ex(entry, NULL, 0, NULL, fcc, &error)) {
				zend_argument_type_error(1, "must be an array with valid callbacks as values, %s", error);
				efree(fcc);
				efree(error);
				return FAILURE;
			}

			zend_fcc_addref(fcc);
			ZVAL_PTR(&registered_value, fcc);

			if (!key) {
				zend_string *str = zval_try_get_string(entry);
				if (str && php_dom_xpath_is_callback_name_valid_and_throw(str, name_validation, true)) {
					zend_hash_update(&ns->functions, str, &registered_value);
					if (register_func) {
						register_func(ctxt, namespace, str);
					}
					zend_string_release_ex(str, false);
				} else {
					zend_fcc_dtor(fcc);
					efree(fcc);
					return FAILURE;
				}
			} else {
				if (!php_dom_xpath_is_callback_name_valid_and_throw(key, name_validation, true)) {
					zend_fcc_dtor(fcc);
					efree(fcc);
					return FAILURE;
				}
				zend_hash_update(&ns->functions, key, &registered_value);
				if (register_func) {
					register_func(ctxt, namespace, key);
				}
			}
		} ZEND_HASH_FOREACH_END();
		ns->mode = PHP_DOM_REG_FUNC_MODE_SET;
	} else if (name) {
		if (!php_dom_xpath_is_callback_name_valid(name, name_validation)) {
			zend_argument_value_error(1, "must be a valid callback name");
			return FAILURE;
		}
		zend_fcall_info_cache* fcc = emalloc(sizeof(zend_fcall_info));
		char *error;
		zval tmp;
		ZVAL_STR(&tmp, name);
		if (!zend_is_callable_ex(&tmp, NULL, 0, NULL, fcc, &error)) {
			zend_argument_type_error(1, "must be a callable, %s", error);
			efree(fcc);
			efree(error);
			return FAILURE;
		}
		zend_fcc_addref(fcc);
		ZVAL_PTR(&registered_value, fcc);
		zend_hash_update(&ns->functions, name, &registered_value);
		if (register_func) {
			register_func(ctxt, namespace, name);
		}
		ns->mode = PHP_DOM_REG_FUNC_MODE_SET;
	} else {
		ns->mode = PHP_DOM_REG_FUNC_MODE_ALL;
	}

	return SUCCESS;
}

static php_dom_xpath_callback_ns *php_dom_xpath_callbacks_ensure_ns(php_dom_xpath_callbacks *registry, zend_string *ns)
{
	if (ns == NULL) {
		if (!registry->php_ns) {
			registry->php_ns = emalloc(sizeof(php_dom_xpath_callback_ns));
			php_dom_xpath_callback_ns_ctor(registry->php_ns);
		}
		return registry->php_ns;
	} else {
		if (!registry->namespaces) {
			/* In most cases probably only a single namespace is registered. */
			registry->namespaces = zend_new_array(1);
		}
		php_dom_xpath_callback_ns *namespace = zend_hash_find_ptr(registry->namespaces, ns);
		if (namespace == NULL) {
			namespace = emalloc(sizeof(php_dom_xpath_callback_ns));
			php_dom_xpath_callback_ns_ctor(namespace);
			zend_hash_add_new_ptr(registry->namespaces, ns, namespace);
		}
		return namespace;
	}
}

PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_update_method_handler(php_dom_xpath_callbacks* registry, xmlXPathContextPtr ctxt, zend_string *ns, zend_string *name, const HashTable *callable_ht, php_dom_xpath_callback_name_validation name_validation, php_dom_xpath_callbacks_register_func_ctx register_func)
{
	php_dom_xpath_callback_ns *namespace = php_dom_xpath_callbacks_ensure_ns(registry, ns);
	return php_dom_xpath_callback_ns_update_method_handler(namespace, ctxt, ns, name, callable_ht, name_validation, register_func);
}

PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_update_single_method_handler(php_dom_xpath_callbacks* registry, xmlXPathContextPtr ctxt, zend_string *ns, zend_string *name, const zend_fcall_info_cache *fcc, php_dom_xpath_callback_name_validation name_validation, php_dom_xpath_callbacks_register_func_ctx register_func)
{
	if (!php_dom_xpath_is_callback_name_valid_and_throw(name, name_validation, false)) {
		return FAILURE;
	}

	php_dom_xpath_callback_ns *namespace = php_dom_xpath_callbacks_ensure_ns(registry, ns);
	zend_fcall_info_cache* allocated_fcc = emalloc(sizeof(zend_fcall_info));
	zend_fcc_dup(allocated_fcc, fcc);

	zval registered_value;
	ZVAL_PTR(&registered_value, allocated_fcc);

	zend_hash_update(&namespace->functions, name, &registered_value);
	if (register_func) {
		register_func(ctxt, ns, name);
	}

	namespace->mode = PHP_DOM_REG_FUNC_MODE_SET;

	return SUCCESS;
}

static zval *php_dom_xpath_callback_fetch_args(xmlXPathParserContextPtr ctxt, uint32_t param_count, php_dom_xpath_nodeset_evaluation_mode evaluation_mode, dom_object *intern, php_dom_xpath_callbacks_proxy_factory proxy_factory)
{
	if (param_count == 0) {
		return NULL;
	}

	zval *params = safe_emalloc(param_count, sizeof(zval), 0);

	/* Reverse order to pop values off ctxt stack */
	for (zval *param = params + param_count - 1; param >= params; param--) {
		xmlXPathObjectPtr obj = valuePop(ctxt);
		ZEND_ASSERT(obj != NULL);
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(param, (char *)obj->stringval);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(param, obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(param, obj->floatval);
				break;
			case XPATH_NODESET:
				if (evaluation_mode == PHP_DOM_XPATH_EVALUATE_NODESET_TO_STRING) {
					char *str = (char *)xmlXPathCastToString(obj);
					ZVAL_STRING(param, str);
					xmlFree(str);
				} else if (evaluation_mode == PHP_DOM_XPATH_EVALUATE_NODESET_TO_NODESET) {
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
						array_init_size(param, obj->nodesetval->nodeNr);
						zend_hash_real_init_packed(Z_ARRVAL_P(param));
						for (int j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval child;
							if (UNEXPECTED(node->type == XML_NAMESPACE_DECL)) {
								xmlNodePtr nsparent = node->_private;
								xmlNsPtr original = (xmlNsPtr) node;

								/* Make sure parent dom object exists, so we can take an extra reference. */
								zval parent_zval; /* don't destroy me, my lifetime is transfered to the fake namespace decl */
								php_dom_create_object(nsparent, &parent_zval, intern);
								dom_object *parent_intern = Z_DOMOBJ_P(&parent_zval);

								php_dom_create_fake_namespace_decl(nsparent, original, &child, parent_intern);
							} else {
								proxy_factory(node, &child, intern, ctxt);
							}
							zend_hash_next_index_insert_new(Z_ARRVAL_P(param), &child);
						}
					} else {
						ZVAL_EMPTY_ARRAY(param);
					}
				}
				break;
			default:
				ZVAL_STRING(param, (char *)xmlXPathCastToString(obj));
				break;
		}
		xmlXPathFreeObject(obj);
	}

	return params;
}

static void php_dom_xpath_callback_cleanup_args(zval *params, uint32_t param_count)
{
	if (params) {
		for (uint32_t i = 0; i < param_count; i++) {
			zval_ptr_dtor(&params[i]);
		}
		efree(params);
	}
}

static zend_result php_dom_xpath_callback_dispatch(php_dom_xpath_callbacks *xpath_callbacks, php_dom_xpath_callback_ns *ns, xmlXPathParserContextPtr ctxt, zval *params, uint32_t param_count, const char *function_name, size_t function_name_length)
{
    zval callback_retval;

	if (UNEXPECTED(ns == NULL)) {
		zend_throw_error(NULL, "No callbacks were registered");
		return FAILURE;
	}

	if (ns->mode == PHP_DOM_REG_FUNC_MODE_ALL) {
		zend_fcall_info fci;
		fci.size = sizeof(fci);
		fci.object = NULL;
		fci.retval = &callback_retval;
		fci.param_count = param_count;
		fci.params = params;
		fci.named_params = NULL;
		ZVAL_STRINGL(&fci.function_name, function_name, function_name_length);

		zend_call_function(&fci, NULL);
		zend_string_release_ex(Z_STR(fci.function_name), false);
		if (UNEXPECTED(EG(exception))) {
			return FAILURE;
		}
	} else {
		ZEND_ASSERT(ns->mode == PHP_DOM_REG_FUNC_MODE_SET);

		zval *fcc_wrapper = zend_hash_str_find(&ns->functions, function_name, function_name_length);
		if (fcc_wrapper) {
			zend_call_known_fcc(Z_PTR_P(fcc_wrapper), &callback_retval, param_count, params, NULL);
		} else {
			zend_throw_error(NULL, "No callback handler \"%s\" registered", function_name);
			return FAILURE;
		}
	}

	if (Z_TYPE(callback_retval) != IS_UNDEF) {
		if (Z_TYPE(callback_retval) == IS_OBJECT && instanceof_function(Z_OBJCE(callback_retval), dom_node_class_entry)) {
			xmlNode *nodep;
			dom_object *obj;
			if (xpath_callbacks->node_list == NULL) {
				xpath_callbacks->node_list = zend_new_array(0);
			}
			Z_ADDREF_P(&callback_retval);
			zend_hash_next_index_insert_new(xpath_callbacks->node_list, &callback_retval);
			obj = Z_DOMOBJ_P(&callback_retval);
			nodep = dom_object_get_node(obj);
			valuePush(ctxt, xmlXPathNewNodeSet(nodep));
		} else if (Z_TYPE(callback_retval) == IS_FALSE || Z_TYPE(callback_retval) == IS_TRUE) {
			valuePush(ctxt, xmlXPathNewBoolean(Z_TYPE(callback_retval) == IS_TRUE));
		} else if (Z_TYPE(callback_retval) == IS_OBJECT) {
			zend_type_error("Only objects that are instances of DOMNode can be converted to an XPath expression");
			zval_ptr_dtor(&callback_retval);
			return FAILURE;
		} else {
			zend_string *str = zval_get_string(&callback_retval);
			valuePush(ctxt, xmlXPathNewString((xmlChar *) ZSTR_VAL(str)));
			zend_string_release_ex(str, 0);
		}
		zval_ptr_dtor(&callback_retval);
	}

	return SUCCESS;
}

PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_call_php_ns(php_dom_xpath_callbacks *xpath_callbacks, xmlXPathParserContextPtr ctxt, int num_args, php_dom_xpath_nodeset_evaluation_mode evaluation_mode, dom_object *intern, php_dom_xpath_callbacks_proxy_factory proxy_factory)
{
	zend_result result = FAILURE;

	if (UNEXPECTED(num_args == 0)) {
		zend_throw_error(NULL, "Function name must be passed as the first argument");
		goto cleanup_no_obj;
	}

	uint32_t param_count = num_args - 1;
	zval *params = php_dom_xpath_callback_fetch_args(ctxt, param_count, evaluation_mode, intern, proxy_factory);

	/* Last element of the stack is the function name */
	xmlXPathObjectPtr obj = valuePop(ctxt);
	if (UNEXPECTED(obj->stringval == NULL)) {
		zend_type_error("Handler name must be a string");
		goto cleanup;
	}

	const char *function_name = (const char *) obj->stringval;
	size_t function_name_length = strlen(function_name);

	result = php_dom_xpath_callback_dispatch(xpath_callbacks, xpath_callbacks->php_ns, ctxt, params, param_count, function_name, function_name_length);

cleanup:
	xmlXPathFreeObject(obj);
	php_dom_xpath_callback_cleanup_args(params, param_count);
cleanup_no_obj:
	if (UNEXPECTED(result != SUCCESS)) {
		/* Push sentinel value */
		valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
	}

	return result;
}

PHP_DOM_EXPORT zend_result php_dom_xpath_callbacks_call_custom_ns(php_dom_xpath_callbacks *xpath_callbacks, xmlXPathParserContextPtr ctxt, int num_args, php_dom_xpath_nodeset_evaluation_mode evaluation_mode, dom_object *intern, php_dom_xpath_callbacks_proxy_factory proxy_factory)
{
	uint32_t param_count = num_args;
	zval *params = php_dom_xpath_callback_fetch_args(ctxt, param_count, evaluation_mode, intern, proxy_factory);

	const char *namespace = (const char *) ctxt->context->functionURI;
	/* Impossible because it wouldn't have been registered inside the context. */
	ZEND_ASSERT(xpath_callbacks->namespaces != NULL);

	php_dom_xpath_callback_ns *ns = zend_hash_str_find_ptr(xpath_callbacks->namespaces, namespace, strlen(namespace));
	/* Impossible because it wouldn't have been registered inside the context. */
	ZEND_ASSERT(ns != NULL);

	const char *function_name = (const char *) ctxt->context->function;
	size_t function_name_length = strlen(function_name);

	zend_result result = php_dom_xpath_callback_dispatch(xpath_callbacks, ns, ctxt, params, param_count, function_name, function_name_length);

	php_dom_xpath_callback_cleanup_args(params, param_count);
	if (UNEXPECTED(result != SUCCESS)) {
		/* Push sentinel value */
		valuePush(ctxt, xmlXPathNewString((const xmlChar *) ""));
	}

	return result;
}

#endif
