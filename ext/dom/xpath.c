/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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
   | Authors: Christian Stocker <chregu@php.net>                          |
   |          Rob Richards <rrichards@php.net>                            |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"

#define PHP_DOM_XPATH_QUERY 0
#define PHP_DOM_XPATH_EVALUATE 1

/*
* class DOMXPath
*/

#if defined(LIBXML_XPATH_ENABLED)

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_xpath_construct, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, doc, DOMDocument, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_xpath_register_ns, 0, 0, 2)
	ZEND_ARG_INFO(0, prefix)
	ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_xpath_query, 0, 0, 1)
	ZEND_ARG_INFO(0, expr)
	ZEND_ARG_OBJ_INFO(0, context, DOMNode, 1)
	ZEND_ARG_INFO(0, registerNodeNS)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_xpath_evaluate, 0, 0, 1)
	ZEND_ARG_INFO(0, expr)
	ZEND_ARG_OBJ_INFO(0, context, DOMNode, 1)
	ZEND_ARG_INFO(0, registerNodeNS)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_xpath_register_php_functions, 0, 0, 0)
ZEND_END_ARG_INFO();
/* }}} */

const zend_function_entry php_dom_xpath_class_functions[] = {
	PHP_ME(domxpath, __construct, arginfo_dom_xpath_construct, ZEND_ACC_PUBLIC)
	PHP_FALIAS(registerNamespace, dom_xpath_register_ns, arginfo_dom_xpath_register_ns)
	PHP_FALIAS(query, dom_xpath_query, arginfo_dom_xpath_query)
	PHP_FALIAS(evaluate, dom_xpath_evaluate, arginfo_dom_xpath_evaluate)
	PHP_FALIAS(registerPhpFunctions, dom_xpath_register_php_functions, arginfo_dom_xpath_register_php_functions)
	PHP_FE_END
};


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

	fci.param_count = nargs - 1;
	if (fci.param_count > 0) {
		fci.params = safe_emalloc(fci.param_count, sizeof(zval), 0);
	}
	/* Reverse order to pop values off ctxt stack */
	for (i = nargs - 2; i >= 0; i--) {
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
					array_init(&fci.params[i]);
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
						for (j = 0; j < obj->nodesetval->nodeNr; j++) {
							xmlNodePtr node = obj->nodesetval->nodeTab[j];
							zval child;
							/* not sure, if we need this... it's copied from xpath.c */
							if (node->type == XML_NAMESPACE_DECL) {
								xmlNsPtr curns;
								xmlNodePtr nsparent;

								nsparent = node->_private;
								curns = xmlNewNs(NULL, node->name, NULL);
								if (node->children) {
									curns->prefix = xmlStrdup((xmlChar *) node->children);
								}
								if (node->children) {
									node = xmlNewDocNode(node->doc, NULL, (xmlChar *) node->children, node->name);
								} else {
									node = xmlNewDocNode(node->doc, NULL, (xmlChar *) "xmlns", node->name);
								}
								node->type = XML_NAMESPACE_DECL;
								node->parent = nsparent;
								node->ns = curns;
							}
							php_dom_create_object(node, &child, &intern->dom);
							add_next_index_zval(&fci.params[i], &child);
						}
					}
				}
				break;
			default:
			ZVAL_STRING(&fci.params[i], (char *)xmlXPathCastToString(obj));
		}
		xmlXPathFreeObject(obj);
	}

	fci.size = sizeof(fci);

	obj = valuePop(ctxt);
	if (obj->stringval == NULL) {
		php_error_docref(NULL, E_WARNING, "Handler name must be a string");
		xmlXPathFreeObject(obj);
		if (fci.param_count > 0) {
			for (i = 0; i < nargs - 1; i++) {
				zval_ptr_dtor(&fci.params[i]);
			}
			efree(fci.params);
		}
		return;
	}
	ZVAL_STRING(&fci.function_name, (char *) obj->stringval);
	xmlXPathFreeObject(obj);

	fci.object = NULL;
	fci.retval = &retval;
	fci.no_separation = 0;

	if (!zend_make_callable(&fci.function_name, &callable)) {
		php_error_docref(NULL, E_WARNING, "Unable to call handler %s()", ZSTR_VAL(callable));
	} else if (intern->registerPhpFunctions == 2 && zend_hash_exists(intern->registered_phpfunctions, callable) == 0) {
		php_error_docref(NULL, E_WARNING, "Not allowed to call handler '%s()'.", ZSTR_VAL(callable));
		/* Push an empty string, so that we at least have an xslt result... */
		valuePush(ctxt, xmlXPathNewString((xmlChar *)""));
	} else {
		result = zend_call_function(&fci, NULL);
		if (result == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
			if (Z_TYPE(retval) == IS_OBJECT && instanceof_function(Z_OBJCE(retval), dom_node_class_entry)) {
				xmlNode *nodep;
				dom_object *obj;
				if (intern->node_list == NULL) {
					ALLOC_HASHTABLE(intern->node_list);
					zend_hash_init(intern->node_list, 0, NULL, ZVAL_PTR_DTOR, 0);
				}
				Z_ADDREF(retval);
				zend_hash_next_index_insert(intern->node_list, &retval);
				obj = Z_DOMOBJ_P(&retval);
				nodep = dom_object_get_node(obj);
				valuePush(ctxt, xmlXPathNewNodeSet(nodep));
			} else if (Z_TYPE(retval) == IS_FALSE || Z_TYPE(retval) == IS_TRUE) {
				valuePush(ctxt, xmlXPathNewBoolean(Z_TYPE(retval) == IS_TRUE));
			} else if (Z_TYPE(retval) == IS_OBJECT) {
				php_error_docref(NULL, E_WARNING, "A PHP Object cannot be converted to a XPath-string");
				valuePush(ctxt, xmlXPathNewString((xmlChar *)""));
			} else {
				zend_string *str = zval_get_string(&retval);
				valuePush(ctxt, xmlXPathNewString((xmlChar *) ZSTR_VAL(str)));
				zend_string_release(str);
			}
			zval_ptr_dtor(&retval);
		}
	}
	zend_string_release(callable);
	zval_dtor(&fci.function_name);
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

/* {{{ proto void DOMXPath::__construct(DOMDocument doc) U */
PHP_METHOD(domxpath, __construct)
{
	zval *id = getThis(), *doc;
	xmlDocPtr docp = NULL;
	dom_object *docobj;
	dom_xpath_object *intern;
	xmlXPathContextPtr ctx, oldctx;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "O", &doc, dom_document_class_entry) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, doc, xmlDocPtr, docobj);

	ctx = xmlXPathNewContext(docp);
	if (ctx == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_FALSE;
	}

	intern = Z_XPATHOBJ_P(id);
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
		php_libxml_increment_doc_ref((php_libxml_node_object *) &intern->dom, docp);
	}
}
/* }}} end DOMXPath::__construct */

/* {{{ document DOMDocument*/
int dom_xpath_document_read(dom_object *obj, zval *retval)
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

/* {{{ proto boolean dom_xpath_register_ns(string prefix, string uri) */
PHP_FUNCTION(dom_xpath_register_ns)
{
	zval *id;
	xmlXPathContextPtr ctxp;
	size_t prefix_len, ns_uri_len;
	dom_xpath_object *intern;
	unsigned char *prefix, *ns_uri;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Oss", &id, dom_xpath_class_entry, &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	intern = Z_XPATHOBJ_P(id);

	ctxp = (xmlXPathContextPtr) intern->dom.ptr;
	if (ctxp == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid XPath Context");
		RETURN_FALSE;
	}

	if (xmlXPathRegisterNs(ctxp, prefix, ns_uri) != 0) {
		RETURN_FALSE
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
	zend_bool register_node_ns = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS(), getThis(), "Os|O!b", &id, dom_xpath_class_entry, &expr, &expr_len, &context, dom_node_class_entry, &register_node_ns) == FAILURE) {
		return;
	}

	intern = Z_XPATHOBJ_P(id);

	ctxp = (xmlXPathContextPtr) intern->dom.ptr;
	if (ctxp == NULL) {
		php_error_docref(NULL, E_WARNING, "Invalid XPath Context");
		RETURN_FALSE;
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
		php_error_docref(NULL, E_WARNING, "Node From Wrong Document");
		RETURN_FALSE;
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

			array_init(&retval);

			if (xpathobjp->type == XPATH_NODESET && NULL != (nodesetp = xpathobjp->nodesetval)) {

				for (i = 0; i < nodesetp->nodeNr; i++) {
					xmlNodePtr node = nodesetp->nodeTab[i];
					zval child;

					if (node->type == XML_NAMESPACE_DECL) {
						xmlNsPtr curns;
						xmlNodePtr nsparent;

						nsparent = node->_private;
						curns = xmlNewNs(NULL, node->name, NULL);
						if (node->children) {
							curns->prefix = xmlStrdup((xmlChar *) node->children);
						}
						if (node->children) {
							node = xmlNewDocNode(docp, NULL, (xmlChar *) node->children, node->name);
						} else {
							node = xmlNewDocNode(docp, NULL, (xmlChar *) "xmlns", node->name);
						}
						node->type = XML_NAMESPACE_DECL;
						node->parent = nsparent;
						node->ns = curns;
					}
					php_dom_create_object(node, &child, &intern->dom);
					add_next_index_zval(&retval, &child);
				}
			}
			php_dom_create_interator(return_value, DOM_NODELIST);
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

/* {{{ proto DOMNodeList dom_xpath_query(string expr [,DOMNode context [, boolean registerNodeNS]]) */
PHP_FUNCTION(dom_xpath_query)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_QUERY);
}
/* }}} end dom_xpath_query */

/* {{{ proto mixed dom_xpath_evaluate(string expr [,DOMNode context [, boolean registerNodeNS]]) */
PHP_FUNCTION(dom_xpath_evaluate)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_EVALUATE);
}
/* }}} end dom_xpath_evaluate */

/* {{{ proto void dom_xpath_register_php_functions() */
PHP_FUNCTION(dom_xpath_register_php_functions)
{
	zval *id;
	dom_xpath_object *intern;
	zval *array_value, *entry, new_string;
	zend_string *name;

	DOM_GET_THIS(id);

	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "a",  &array_value) == SUCCESS) {
		intern = Z_XPATHOBJ_P(id);
		ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array_value), entry) {
			zend_string *str = zval_get_string(entry);
			ZVAL_LONG(&new_string,1);
			zend_hash_update(intern->registered_phpfunctions, str, &new_string);
			zend_string_release(str);
		} ZEND_HASH_FOREACH_END();
		intern->registerPhpFunctions = 2;
		RETURN_TRUE;

	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS(), "S",  &name) == SUCCESS) {
		intern = Z_XPATHOBJ_P(id);

		ZVAL_LONG(&new_string, 1);
		zend_hash_update(intern->registered_phpfunctions, name, &new_string);
		intern->registerPhpFunctions = 2;
	} else {
		intern = Z_XPATHOBJ_P(id);
		intern->registerPhpFunctions = 1;
	}

}
/* }}} end dom_xpath_register_php_functions */

#endif /* LIBXML_XPATH_ENABLED */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
