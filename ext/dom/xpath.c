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

#define PHP_DOM_XPATH_QUERY 0
#define PHP_DOM_XPATH_EVALUATE 1

/*
* class DOMXPath
*/

#ifdef LIBXML_XPATH_ENABLED

static void dom_xpath_ext_function_php(xmlXPathParserContextPtr ctxt, int nargs, int type) /* {{{ */
{
	zval retval;
	int result, i;
	int error = 0;
	zend_fcall_info fci;
	xmlXPathObjectPtr obj;
	char *str;
	zend_string *callable = NULL;
	dom_xpath_object *intern;


	if (! zend_is_executing()) {
		xmlGenericError(xmlGenericErrorContext,
		"xmlExtFunctionTest: Function called from outside of PHP\n");
		error = 1;
	} else {
		intern = (dom_xpath_object *) ctxt->context->userData;
		if (intern == NULL) {
			xmlGenericError(xmlGenericErrorContext,
			"xmlExtFunctionTest: failed to get the internal object\n");
			error = 1;
		}
		else if (intern->registerPhpFunctions == 0) {
			xmlGenericError(xmlGenericErrorContext,
			"xmlExtFunctionTest: PHP Object did not register PHP functions\n");
			error = 1;
		}
	}

	if (error == 1) {
		for (i = nargs - 1; i >= 0; i--) {
			obj = valuePop(ctxt);
			xmlXPathFreeObject(obj);
		}
		return;
	}

	if (UNEXPECTED(nargs == 0)) {
		zend_throw_error(NULL, "Function name must be passed as the first argument");
		return;
	}

	fci.param_count = nargs - 1;
	if (fci.param_count > 0) {
		fci.params = safe_emalloc(fci.param_count, sizeof(zval), 0);
	}
	/* Reverse order to pop values off ctxt stack */
	for (i = fci.param_count - 1; i >= 0; i--) {
		obj = valuePop(ctxt);
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(&fci.params[i],  (char *)obj->stringval);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(&fci.params[i],  obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(&fci.params[i], obj->floatval);
				break;
			case XPATH_NODESET:
				if (type == 1) {
					str = (char *)xmlXPathCastToString(obj);
					ZVAL_STRING(&fci.params[i], str);
					xmlFree(str);
				} else if (type == 2) {
					int j;
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
						array_init_size(&fci.params[i], obj->nodesetval->nodeNr);
						zend_hash_real_init_packed(Z_ARRVAL_P(&fci.params[i]));
						for (j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval child;
							if (node->type == XML_NAMESPACE_DECL) {
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
							add_next_index_zval(&fci.params[i], &child);
						}
					} else {
						ZVAL_EMPTY_ARRAY(&fci.params[i]);
					}
				}
				break;
			default:
			ZVAL_STRING(&fci.params[i], (char *)xmlXPathCastToString(obj));
		}
		xmlXPathFreeObject(obj);
	}

	fci.size = sizeof(fci);

	/* Last element of the stack is the function name */
	obj = valuePop(ctxt);
	if (obj->stringval == NULL) {
		zend_type_error("Handler name must be a string");
		xmlXPathFreeObject(obj);
		goto cleanup_no_callable;
	}
	ZVAL_STRING(&fci.function_name, (char *) obj->stringval);
	xmlXPathFreeObject(obj);

	fci.object = NULL;
	fci.named_params = NULL;
	fci.retval = &retval;

	if (!zend_make_callable(&fci.function_name, &callable)) {
		zend_throw_error(NULL, "Unable to call handler %s()", ZSTR_VAL(callable));
		goto cleanup;
	} else if (intern->registerPhpFunctions == 2 && zend_hash_exists(intern->registered_phpfunctions, callable) == 0) {
		zend_throw_error(NULL, "Not allowed to call handler '%s()'.", ZSTR_VAL(callable));
		goto cleanup;
	} else {
		result = zend_call_function(&fci, NULL);
		if (result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
			if (Z_TYPE(retval) == IS_OBJECT && instanceof_function(Z_OBJCE(retval), dom_node_class_entry)) {
				xmlNode *nodep;
				dom_object *obj;
				if (intern->node_list == NULL) {
					intern->node_list = zend_new_array(0);
				}
				Z_ADDREF(retval);
				zend_hash_next_index_insert(intern->node_list, &retval);
				obj = Z_DOMOBJ_P(&retval);
				nodep = dom_object_get_node(obj);
				valuePush(ctxt, xmlXPathNewNodeSet(nodep));
			} else if (Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE) {
				valuePush(ctxt, xmlXPathNewBoolean(Z_TYPE(retval) == IS_TRUE));
			} else if (Z_TYPE(retval) == IS_OBJECT) {
				zend_type_error("A PHP Object cannot be converted to a XPath-string");
				return;
			} else {
				zend_string *str = zval_get_string(&retval);
				valuePush(ctxt, xmlXPathNewString((xmlChar *) ZSTR_VAL(str)));
				zend_string_release_ex(str, 0);
			}
			zval_ptr_dtor(&retval);
		}
	}
cleanup:
	zend_string_release_ex(callable, 0);
	zval_ptr_dtor_nogc(&fci.function_name);
cleanup_no_callable:
	if (fci.param_count > 0) {
		for (i = 0; i < nargs - 1; i++) {
			zval_ptr_dtor(&fci.params[i]);
		}
		efree(fci.params);
	}
}
/* }}} */

static void dom_xpath_ext_function_string_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	dom_xpath_ext_function_php(ctxt, nargs, 1);
}
/* }}} */

static void dom_xpath_ext_function_object_php(xmlXPathParserContextPtr ctxt, int nargs) /* {{{ */
{
	dom_xpath_ext_function_php(ctxt, nargs, 2);
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMXPath, __construct)
{
	zval *doc;
	bool register_node_ns = 1;
	xmlDocPtr docp = NULL;
	dom_object *docobj;
	dom_xpath_object *intern;
	xmlXPathContextPtr ctx, oldctx;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &doc, dom_document_class_entry, &register_node_ns) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, doc, xmlDocPtr, docobj);

	ctx = xmlXPathNewContext(docp);
	if (ctx == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	intern = Z_XPATHOBJ_P(ZEND_THIS);
	if (intern != NULL) {
		oldctx = (xmlXPathContextPtr)intern->dom.ptr;
		if (oldctx != NULL) {
			php_libxml_decrement_doc_ref((php_libxml_node_object *) &intern->dom);
			xmlXPathFreeContext(oldctx);
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
	zval *id;
	xmlXPathContextPtr ctxp;
	size_t prefix_len, ns_uri_len;
	dom_xpath_object *intern;
	unsigned char *prefix, *ns_uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	intern = Z_XPATHOBJ_P(id);

	ctxp = (xmlXPathContextPtr) intern->dom.ptr;
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

static void php_xpath_eval(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id, retval, *context = NULL;
	xmlXPathContextPtr ctxp;
	xmlNodePtr nodep = NULL;
	xmlXPathObjectPtr xpathobjp;
	size_t expr_len, nsnbr = 0, xpath_type;
	dom_xpath_object *intern;
	dom_object *nodeobj;
	char *expr;
	xmlDoc *docp = NULL;
	xmlNsPtr *ns = NULL;
	bool register_node_ns;

	id = ZEND_THIS;
	intern = Z_XPATHOBJ_P(id);
	register_node_ns = intern->register_node_ns;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|O!b", &expr, &expr_len, &context, dom_node_class_entry, &register_node_ns) == FAILURE) {
		RETURN_THROWS();
	}

	ctxp = (xmlXPathContextPtr) intern->dom.ptr;
	if (ctxp == NULL) {
		zend_throw_error(NULL, "Invalid XPath Context");
		RETURN_THROWS();
	}

	docp = (xmlDocPtr) ctxp->doc;
	if (docp == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid XPath Document Pointer");
		RETURN_FALSE;
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

	if (register_node_ns) {
		/* Register namespaces in the node */
		ns = xmlGetNsList(docp, nodep);

		if (ns != NULL) {
			while (ns[nsnbr] != NULL)
			nsnbr++;
		}
	}


	ctxp->namespaces = ns;
	ctxp->nsNr = nsnbr;

	xpathobjp = xmlXPathEvalExpression((xmlChar *) expr, ctxp);
	ctxp->node = NULL;

	if (ns != NULL) {
		xmlFree(ns);
		ctxp->namespaces = NULL;
		ctxp->nsNr = 0;
	}

	if (! xpathobjp) {
		/* TODO Add Warning? */
		RETURN_FALSE;
	}

	if (type == PHP_DOM_XPATH_QUERY) {
		xpath_type = XPATH_NODESET;
	} else {
		xpath_type = xpathobjp->type;
	}

	switch (xpath_type) {

		case  XPATH_NODESET:
		{
			int i;
			xmlNodeSetPtr nodesetp;

			if (xpathobjp->type == XPATH_NODESET && NULL != (nodesetp = xpathobjp->nodesetval) && nodesetp->nodeNr) {
				array_init_size(&retval, nodesetp->nodeNr);
				zend_hash_real_init_packed(Z_ARRVAL_P(&retval));
				for (i = 0; i < nodesetp->nodeNr; i++) {
					xmlNodePtr node = nodesetp->nodeTab[i];
					zval child;

					if (node->type == XML_NAMESPACE_DECL) {
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
			php_dom_create_iterator(return_value, DOM_NODELIST);
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
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_QUERY);
}
/* }}} end dom_xpath_query */

/* {{{ */
PHP_METHOD(DOMXPath, evaluate)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_EVALUATE);
}
/* }}} end dom_xpath_evaluate */

/* {{{ */
PHP_METHOD(DOMXPath, registerPhpFunctions)
{
	zval *id = ZEND_THIS;
	dom_xpath_object *intern = Z_XPATHOBJ_P(id);
	zval *entry, new_string;
	zend_string *name = NULL;
	HashTable *ht = NULL;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(ht, name)
	ZEND_PARSE_PARAMETERS_END();

	if (ht) {
		ZEND_HASH_FOREACH_VAL(ht, entry) {
			zend_string *str = zval_get_string(entry);
			ZVAL_LONG(&new_string, 1);
			zend_hash_update(intern->registered_phpfunctions, str, &new_string);
			zend_string_release_ex(str, 0);
		} ZEND_HASH_FOREACH_END();
		intern->registerPhpFunctions = 2;
	} else if (name) {
		ZVAL_LONG(&new_string, 1);
		zend_hash_update(intern->registered_phpfunctions, name, &new_string);
		intern->registerPhpFunctions = 2;
	} else {
		intern->registerPhpFunctions = 1;
	}

}
/* }}} end dom_xpath_register_php_functions */

#endif /* LIBXML_XPATH_ENABLED */

#endif
