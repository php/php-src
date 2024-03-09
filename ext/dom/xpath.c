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
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"

#define PHP_DOM_XPATH_QUERY 0
#define PHP_DOM_XPATH_EVALUATE 1

/*
* class DOMXPath
*/

#ifdef LIBXML_XPATH_ENABLED

void dom_xpath_objects_free_storage(zend_object *object)
{
	dom_xpath_object *intern = php_xpath_obj_from_obj(object);

	zend_object_std_dtor(&intern->dom.std);

	if (intern->dom.ptr != NULL) {
		xmlXPathFreeContext((xmlXPathContextPtr) intern->dom.ptr);
		php_libxml_decrement_doc_ref((php_libxml_node_object *) &intern->dom);
	}

	php_dom_xpath_callbacks_dtor(&intern->xpath_callbacks);
}

HashTable *dom_xpath_get_gc(zend_object *object, zval **table, int *n)
{
	dom_xpath_object *intern = php_xpath_obj_from_obj(object);
	return php_dom_xpath_callbacks_get_gc_for_whole_object(&intern->xpath_callbacks, object, table, n);
}

static void dom_xpath_proxy_factory(xmlNodePtr node, zval *child, dom_object *intern, xmlXPathParserContextPtr ctxt)
{
	ZEND_IGNORE_VALUE(ctxt);

	ZEND_ASSERT(node->type != XML_NAMESPACE_DECL);

	php_dom_create_object(node, child, intern);
}

static dom_xpath_object *dom_xpath_ext_fetch_intern(xmlXPathParserContextPtr ctxt)
{
	if (UNEXPECTED(!zend_is_executing())) {
		xmlGenericError(xmlGenericErrorContext,
		"xmlExtFunctionTest: Function called from outside of PHP\n");
		return NULL;
	}

	dom_xpath_object *intern = (dom_xpath_object *) ctxt->context->userData;
	if (UNEXPECTED(intern == NULL)) {
		xmlGenericError(xmlGenericErrorContext,
		"xmlExtFunctionTest: failed to get the internal object\n");
		return NULL;
	}

	return intern;
}

static void dom_xpath_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, php_dom_xpath_nodeset_evaluation_mode evaluation_mode) /* {{{ */
{
	dom_xpath_object *intern = dom_xpath_ext_fetch_intern(ctxt);
	if (!intern) {
		php_dom_xpath_callbacks_clean_argument_stack(ctxt, nargs);
	} else {
		php_dom_xpath_callbacks_call_php_ns(&intern->xpath_callbacks, ctxt, nargs, evaluation_mode, &intern->dom, dom_xpath_proxy_factory);
	}
}
/* }}} */

static void dom_xpath_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	dom_xpath_ext_function_php(ctxt, nargs, PHP_DOM_XPATH_EVALUATE_NODESET_TO_STRING);
}
/* }}} */

static void dom_xpath_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	dom_xpath_ext_function_php(ctxt, nargs, PHP_DOM_XPATH_EVALUATE_NODESET_TO_NODESET);
}
/* }}} */

static void dom_xpath_ext_function_trampoline(xmlXPathParserContextPtr ctxt, int nargs)
{
	dom_xpath_object *intern = dom_xpath_ext_fetch_intern(ctxt);
	if (!intern) {
		php_dom_xpath_callbacks_clean_argument_stack(ctxt, nargs);
	} else {
		php_dom_xpath_callbacks_call_custom_ns(&intern->xpath_callbacks, ctxt, nargs, PHP_DOM_XPATH_EVALUATE_NODESET_TO_NODESET, &intern->dom, dom_xpath_proxy_factory);
	}
}

/* {{{ */
static void dom_xpath_construct(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *document_ce)
{
	zval *doc;
	bool register_node_ns = true;
	xmlDocPtr docp = NULL;
	dom_object *docobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &doc, document_ce, &register_node_ns) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, doc, xmlDocPtr, docobj);

	xmlXPathContextPtr ctx = xmlXPathNewContext(docp);
	if (ctx == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	dom_xpath_object *intern = Z_XPATHOBJ_P(ZEND_THIS);
	xmlXPathContextPtr oldctx = intern->dom.ptr;
	if (oldctx != NULL) {
		php_libxml_decrement_doc_ref((php_libxml_node_object *) &intern->dom);
		xmlXPathFreeContext(oldctx);
		php_dom_xpath_callbacks_dtor(&intern->xpath_callbacks);
		php_dom_xpath_callbacks_ctor(&intern->xpath_callbacks);
	}

	xmlXPathRegisterFuncNS (ctx, (const xmlChar *) "functionString",
					(const xmlChar *) "http://php.net/xpath",
					dom_xpath_ext_function_string_php);
	xmlXPathRegisterFuncNS (ctx, (const xmlChar *) "function",
					(const xmlChar *) "http://php.net/xpath",
					dom_xpath_ext_function_object_php);

	intern->dom.ptr = ctx;
	ctx->userData = (void *)intern;
	intern->dom.document = docobj->document;
	intern->register_node_ns = register_node_ns;
	php_libxml_increment_doc_ref((php_libxml_node_object *) &intern->dom, docp);
}

PHP_METHOD(DOMXPath, __construct)
{
	dom_xpath_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_document_class_entry);
}

PHP_METHOD(DOM_XPath, __construct)
{
	dom_xpath_construct(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_abstract_base_document_class_entry);
}
/* }}} end DOMXPath::__construct */

/* {{{ document DOMDocument*/
zend_result dom_xpath_document_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = NULL;
	xmlXPathContextPtr ctx = (xmlXPathContextPtr) obj->ptr;

	if (ctx) {
		docp = (xmlDocPtr) ctx->doc;
	}

	php_dom_create_object((xmlNodePtr) docp, retval, obj);
	return SUCCESS;
}
/* }}} */

/* {{{ registerNodeNamespaces bool*/
static inline dom_xpath_object *php_xpath_obj_from_dom_obj(dom_object *obj) {
	return (dom_xpath_object*)((char*)(obj) - XtOffsetOf(dom_xpath_object, dom));
}

zend_result dom_xpath_register_node_ns_read(dom_object *obj, zval *retval)
{
	ZVAL_BOOL(retval, php_xpath_obj_from_dom_obj(obj)->register_node_ns);

	return SUCCESS;
}

zend_result dom_xpath_register_node_ns_write(dom_object *obj, zval *newval)
{
	php_xpath_obj_from_dom_obj(obj)->register_node_ns = zend_is_true(newval);

	return SUCCESS;
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMXPath, registerNamespace)
{
	size_t prefix_len, ns_uri_len;
	unsigned char *prefix, *ns_uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	dom_xpath_object *intern = Z_XPATHOBJ_P(ZEND_THIS);

	xmlXPathContextPtr ctxp = intern->dom.ptr;
	if (ctxp == NULL) {
		zend_throw_error(NULL, "Invalid XPath Context");
		RETURN_THROWS();
	}

	if (xmlXPathRegisterNs(ctxp, prefix, ns_uri) != 0) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

static void dom_xpath_iter(zval *baseobj, dom_object *intern) /* {{{ */
{
	dom_nnodemap_object *mapptr = (dom_nnodemap_object *) intern->ptr;

	ZVAL_COPY_VALUE(&mapptr->baseobj_zv, baseobj);
	mapptr->nodetype = DOM_NODESET;
}
/* }}} */

static void php_xpath_eval(INTERNAL_FUNCTION_PARAMETERS, int type, bool modern) /* {{{ */
{
	zval *context = NULL;
	xmlNodePtr nodep = NULL;
	size_t expr_len, xpath_type;
	dom_object *nodeobj;
	char *expr;

	dom_xpath_object *intern = Z_XPATHOBJ_P(ZEND_THIS);
	bool register_node_ns = intern->register_node_ns;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|O!b", &expr, &expr_len, &context, modern ? dom_modern_node_class_entry : dom_node_class_entry, &register_node_ns) == FAILURE) {
		RETURN_THROWS();
	}

	xmlXPathContextPtr ctxp = intern->dom.ptr;
	if (ctxp == NULL) {
		zend_throw_error(NULL, "Invalid XPath Context");
		RETURN_THROWS();
	}

	xmlDocPtr docp = ctxp->doc;
	if (docp == NULL) {
		if (modern) {
			zend_throw_error(NULL, "Invalid XPath Document Pointer");
			RETURN_THROWS();
		} else {
			php_error_docref(NULL, E_WARNING, "Invalid XPath Document Pointer");
			RETURN_FALSE;
		}
	}

	if (context != NULL) {
		DOM_GET_OBJ(nodep, context, xmlNodePtr, nodeobj);
	}

	if (!nodep) {
		nodep = xmlDocGetRootElement(docp);
	}

	if (nodep && docp != nodep->doc) {
		zend_throw_error(NULL, "Node from wrong document");
		RETURN_THROWS();
	}

	ctxp->node = nodep;

	php_dom_in_scope_ns in_scope_ns;
	if (register_node_ns && nodep != NULL) {
		if (modern) {
			php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(&intern->dom);
			in_scope_ns = php_dom_get_in_scope_ns(ns_mapper, nodep);
		} else {
			in_scope_ns = php_dom_get_in_scope_ns_legacy(nodep);
		}
		ctxp->namespaces = in_scope_ns.list;
		ctxp->nsNr = in_scope_ns.count;
	}

	xmlXPathObjectPtr xpathobjp = xmlXPathEvalExpression((xmlChar *) expr, ctxp);
	ctxp->node = NULL;

	if (register_node_ns && nodep != NULL) {
		php_dom_in_scope_ns_destroy(&in_scope_ns);
		ctxp->namespaces = NULL;
		ctxp->nsNr = 0;
	}

	if (! xpathobjp) {
		if (modern) {
			if (!EG(exception)) {
				zend_throw_error(NULL, "Could not evaluate XPath expression");
			}
			RETURN_THROWS();
		} else {
			/* Should have already emit a warning by libxml */
			RETURN_FALSE;
		}
	}

	if (type == PHP_DOM_XPATH_QUERY) {
		xpath_type = XPATH_NODESET;
	} else {
		xpath_type = xpathobjp->type;
	}

	switch (xpath_type) {

		case  XPATH_NODESET:
		{
			xmlNodeSetPtr nodesetp;
			zval retval;

			if (xpathobjp->type == XPATH_NODESET && NULL != (nodesetp = xpathobjp->nodesetval) && nodesetp->nodeNr) {
				array_init_size(&retval, nodesetp->nodeNr);
				zend_hash_real_init_packed(Z_ARRVAL_P(&retval));
				for (int i = 0; i < nodesetp->nodeNr; i++) {
					xmlNodePtr node = nodesetp->nodeTab[i];
					zval child;

					if (node->type == XML_NAMESPACE_DECL) {
						if (modern) {
							continue;
						}

						xmlNodePtr nsparent = node->_private;
						xmlNsPtr original = (xmlNsPtr) node;

						/* Make sure parent dom object exists, so we can take an extra reference. */
						zval parent_zval; /* don't destroy me, my lifetime is transfered to the fake namespace decl */
						php_dom_create_object(nsparent, &parent_zval, &intern->dom);
						dom_object *parent_intern = Z_DOMOBJ_P(&parent_zval);

						node = php_dom_create_fake_namespace_decl(nsparent, original, &child, parent_intern);
					} else {
						php_dom_create_object(node, &child, &intern->dom);
					}
					add_next_index_zval(&retval, &child);
				}
			} else {
				ZVAL_EMPTY_ARRAY(&retval);
			}
			php_dom_create_iterator(return_value, DOM_NODELIST, modern);
			nodeobj = Z_DOMOBJ_P(return_value);
			dom_xpath_iter(&retval, nodeobj);
			break;
		}

		case XPATH_BOOLEAN:
			RETVAL_BOOL(xpathobjp->boolval);
			break;

		case XPATH_NUMBER:
			RETVAL_DOUBLE(xpathobjp->floatval);
			break;

		case XPATH_STRING:
			RETVAL_STRING((char *) xpathobjp->stringval);
			break;

		default:
			RETVAL_NULL();
			break;
	}

	xmlXPathFreeObject(xpathobjp);
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMXPath, query)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_QUERY, false);
}

PHP_METHOD(DOM_XPath, query)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_QUERY, true);
}
/* }}} end dom_xpath_query */

/* {{{ */
PHP_METHOD(DOMXPath, evaluate)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_EVALUATE, false);
}

PHP_METHOD(DOM_XPath, evaluate)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_EVALUATE, true);
}
/* }}} end dom_xpath_evaluate */

/* {{{ */
PHP_METHOD(DOMXPath, registerPhpFunctions)
{
	dom_xpath_object *intern = Z_XPATHOBJ_P(ZEND_THIS);

	zend_string *callable_name = NULL;
	HashTable *callable_ht = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(callable_ht, callable_name)
	ZEND_PARSE_PARAMETERS_END();

	php_dom_xpath_callbacks_update_method_handler(
		&intern->xpath_callbacks,
		intern->dom.ptr,
		NULL,
		callable_name,
		callable_ht,
		PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NULLS,
		NULL
	);
}
/* }}} end dom_xpath_register_php_functions */

static void dom_xpath_register_func_in_ctx(void *ctxt, const zend_string *ns, const zend_string *name)
{
	xmlXPathRegisterFuncNS((xmlXPathContextPtr) ctxt, (const xmlChar *) ZSTR_VAL(name), (const xmlChar *) ZSTR_VAL(ns), dom_xpath_ext_function_trampoline);
}

PHP_METHOD(DOMXPath, registerPhpFunctionNS)
{
	dom_xpath_object *intern = Z_XPATHOBJ_P(ZEND_THIS);

	zend_string *namespace, *name;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_PATH_STR(namespace)
		Z_PARAM_PATH_STR(name)
		Z_PARAM_FUNC_NO_TRAMPOLINE_FREE(fci, fcc)
	ZEND_PARSE_PARAMETERS_END();

	if (zend_string_equals_literal(namespace, "http://php.net/xpath")) {
		zend_argument_value_error(1, "must not be \"http://php.net/xpath\" because it is reserved by PHP");
		RETURN_THROWS();
	}

	php_dom_xpath_callbacks_update_single_method_handler(
		&intern->xpath_callbacks,
		intern->dom.ptr,
		namespace,
		name,
		&fcc,
		PHP_DOM_XPATH_CALLBACK_NAME_VALIDATE_NCNAME,
		dom_xpath_register_func_in_ctx
	);
}

/* {{{ */
PHP_METHOD(DOMXPath, quote) {
	const char *input;
	size_t input_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &input, &input_len) == FAILURE) {
		RETURN_THROWS();
	}
	if (memchr(input, '\'', input_len) == NULL) {
		zend_string *const output = zend_string_safe_alloc(1, input_len, 2, false);
		output->val[0] = '\'';
		memcpy(output->val + 1, input, input_len);
		output->val[input_len + 1] = '\'';
		output->val[input_len + 2] = '\0';
		RETURN_STR(output);
	} else if (memchr(input, '"', input_len) == NULL) {
		zend_string *const output = zend_string_safe_alloc(1, input_len, 2, false);
		output->val[0] = '"';
		memcpy(output->val + 1, input, input_len);
		output->val[input_len + 1] = '"';
		output->val[input_len + 2] = '\0';
		RETURN_STR(output);
	} else {
		smart_str output = {0};
		// need to use the concat() trick published by Robert Rossney at https://stackoverflow.com/a/1352556/1067003
		smart_str_appendl(&output, "concat(", 7);
		const char *ptr = input;
		const char *const end = input + input_len;
		while (ptr < end) {
			const char *const single_quote_ptr = memchr(ptr, '\'', end - ptr);
			const char *const double_quote_ptr = memchr(ptr, '"', end - ptr);
			const size_t distance_to_single_quote = single_quote_ptr ? single_quote_ptr - ptr : end - ptr;
			const size_t distance_to_double_quote = double_quote_ptr ? double_quote_ptr - ptr : end - ptr;
			const size_t bytes_until_quote = MAX(distance_to_single_quote, distance_to_double_quote);
			const char quote_method = (distance_to_single_quote > distance_to_double_quote) ? '\'' : '"';
			smart_str_appendc(&output, quote_method);
			smart_str_appendl(&output, ptr, bytes_until_quote);
			smart_str_appendc(&output, quote_method);
			ptr += bytes_until_quote;
			smart_str_appendc(&output, ',');
		}
		ZEND_ASSERT(ptr == end);
		output.s->val[output.s->len - 1] = ')';
		RETURN_STR(smart_str_extract(&output));
	}
}
/* }}} */

#endif /* LIBXML_XPATH_ENABLED */

#endif
