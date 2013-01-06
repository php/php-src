/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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
	zval **args;
	zval *retval;
	int result, i, ret;
	int error = 0;
	zend_fcall_info fci;
	zval handler;
	xmlXPathObjectPtr obj;
	char *str;
	char *callable = NULL;
	dom_xpath_object *intern;
	
	TSRMLS_FETCH();

	if (! zend_is_executing(TSRMLS_C)) {
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
		fci.params = safe_emalloc(fci.param_count, sizeof(zval**), 0);
		args = safe_emalloc(fci.param_count, sizeof(zval *), 0);
	}
	/* Reverse order to pop values off ctxt stack */
	for (i = nargs - 2; i >= 0; i--) {
		obj = valuePop(ctxt);
		MAKE_STD_ZVAL(args[i]);
		switch (obj->type) {
			case XPATH_STRING:
				ZVAL_STRING(args[i],  (char *)obj->stringval, 1);
				break;
			case XPATH_BOOLEAN:
				ZVAL_BOOL(args[i],  obj->boolval);
				break;
			case XPATH_NUMBER:
				ZVAL_DOUBLE(args[i], obj->floatval);
				break;
			case XPATH_NODESET:
				if (type == 1) {
					str = (char *)xmlXPathCastToString(obj);
					ZVAL_STRING(args[i], str, 1);
					xmlFree(str);
				} else if (type == 2) {
					int j;
					array_init(args[i]);
					if (obj->nodesetval && obj->nodesetval->nodeNr > 0) {
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
							child = php_dom_create_object(node, &ret, child, (dom_object *)intern TSRMLS_CC);
							add_next_index_zval(args[i], child);
						}
					}
				}
				break;
			default:
			ZVAL_STRING(args[i], (char *)xmlXPathCastToString(obj), 1);
		}
		xmlXPathFreeObject(obj);
		fci.params[i] = &args[i];
	}
	
	fci.size = sizeof(fci);
	fci.function_table = EG(function_table);
	
	obj = valuePop(ctxt);
	if (obj->stringval == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Handler name must be a string");
		xmlXPathFreeObject(obj);
		if (fci.param_count > 0) {
			for (i = 0; i < nargs - 1; i++) {
				zval_ptr_dtor(&args[i]);
			}
			efree(args);
			efree(fci.params);
		}
		return; 
	}
	INIT_PZVAL(&handler);
	ZVAL_STRING(&handler, obj->stringval, 1);
	xmlXPathFreeObject(obj);

	fci.function_name = &handler;
	fci.symbol_table = NULL;
	fci.object_ptr = NULL;
	fci.retval_ptr_ptr = &retval;
	fci.no_separation = 0;

	if (!zend_make_callable(&handler, &callable TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", callable);
		
	} else if ( intern->registerPhpFunctions == 2 && zend_hash_exists(intern->registered_phpfunctions, callable, strlen(callable) + 1) == 0) { 
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Not allowed to call handler '%s()'.", callable);
		/* Push an empty string, so that we at least have an xslt result... */
		valuePush(ctxt, xmlXPathNewString((xmlChar *)""));
	} else {
		result = zend_call_function(&fci, NULL TSRMLS_CC);
		if (result == FAILURE) {
			if (Z_TYPE(handler) == IS_STRING) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to call handler %s()", Z_STRVAL_P(&handler));
			}
		/* retval is == NULL, when an exception occurred, don't report anything, because PHP itself will handle that */
		} else if (retval == NULL) {
		} else {
			if (retval->type == IS_OBJECT && instanceof_function( Z_OBJCE_P(retval), dom_node_class_entry TSRMLS_CC)) {
				xmlNode *nodep;
				dom_object *obj;
				if (intern->node_list == NULL) {
					ALLOC_HASHTABLE(intern->node_list);
					zend_hash_init(intern->node_list, 0, NULL, ZVAL_PTR_DTOR, 0);
				}
				zval_add_ref(&retval);
				zend_hash_next_index_insert(intern->node_list, &retval, sizeof(zval *), NULL);
				obj = (dom_object *)zend_object_store_get_object(retval TSRMLS_CC);
				nodep = dom_object_get_node(obj);
				valuePush(ctxt, xmlXPathNewNodeSet(nodep));
			} else if (retval->type == IS_BOOL) {
				valuePush(ctxt, xmlXPathNewBoolean(retval->value.lval));
			} else if (retval->type == IS_OBJECT) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "A PHP Object cannot be converted to a XPath-string");
				valuePush(ctxt, xmlXPathNewString((xmlChar *)""));
			} else {
				convert_to_string_ex(&retval);
				valuePush(ctxt, xmlXPathNewString( Z_STRVAL_P(retval)));
			}
			zval_ptr_dtor(&retval);
		}
	}
	efree(callable);
	zval_dtor(&handler);
	if (fci.param_count > 0) {
		for (i = 0; i < nargs - 1; i++) {
			zval_ptr_dtor(&args[i]);
		}
		efree(args);
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
	zval *id, *doc;
	xmlDocPtr docp = NULL;
	dom_object *docobj;
	dom_xpath_object *intern;
	xmlXPathContextPtr ctx, oldctx;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, dom_domexception_class_entry, &error_handling TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &id, dom_xpath_class_entry, &doc, dom_document_class_entry) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
	DOM_GET_OBJ(docp, doc, xmlDocPtr, docobj);

	ctx = xmlXPathNewContext(docp);
	if (ctx == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

	intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldctx = (xmlXPathContextPtr)intern->ptr;
		if (oldctx != NULL) {
			php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
			xmlXPathFreeContext(oldctx);
		}

		xmlXPathRegisterFuncNS (ctx, (const xmlChar *) "functionString",
					   (const xmlChar *) "http://php.net/xpath",
					   dom_xpath_ext_function_string_php);
		xmlXPathRegisterFuncNS (ctx, (const xmlChar *) "function",
					   (const xmlChar *) "http://php.net/xpath",
					   dom_xpath_ext_function_object_php);

		intern->ptr = ctx;
		ctx->userData = (void *)intern;
		intern->document = docobj->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)intern, docp TSRMLS_CC);
	}
}
/* }}} end DOMXPath::__construct */

/* {{{ document DOMDocument*/
int dom_xpath_document_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp = NULL;
	xmlXPathContextPtr ctx;
	int ret;
	zval *tmp;

	ctx = (xmlXPathContextPtr) obj->ptr;

	if (ctx) {
		docp = (xmlDocPtr) ctx->doc;
	}

	ALLOC_ZVAL(*retval);
	tmp = *retval;

	if (NULL == (*retval = php_dom_create_object((xmlNodePtr) docp, &ret, *retval, obj TSRMLS_CC))) {
		FREE_ZVAL(tmp);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	if (tmp != *retval) {
		FREE_ZVAL(tmp);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ proto boolean dom_xpath_register_ns(string prefix, string uri); */
PHP_FUNCTION(dom_xpath_register_ns)
{
	zval *id;
	xmlXPathContextPtr ctxp;
	int prefix_len, ns_uri_len;
	dom_xpath_object *intern;
	unsigned char *prefix, *ns_uri;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oss", &id, dom_xpath_class_entry, &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		return;
	}

	intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);

	ctxp = (xmlXPathContextPtr) intern->ptr;
	if (ctxp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid XPath Context");
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
	dom_nnodemap_object *mapptr;

	mapptr = (dom_nnodemap_object *)intern->ptr;
	mapptr->baseobjptr = baseobj;
	mapptr->nodetype = DOM_NODESET;

}
/* }}} */

static void php_xpath_eval(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id, *retval, *context = NULL;
	xmlXPathContextPtr ctxp;
	xmlNodePtr nodep = NULL;
	xmlXPathObjectPtr xpathobjp;
	int expr_len, ret, nsnbr = 0, xpath_type;
	dom_xpath_object *intern;
	dom_object *nodeobj;
	char *expr;
	xmlDoc *docp = NULL;
	xmlNsPtr *ns = NULL;
	zend_bool register_node_ns = 1;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|O!b", &id, dom_xpath_class_entry, &expr, &expr_len, &context, dom_node_class_entry, &register_node_ns) == FAILURE) {
		return;
	}

	intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);

	ctxp = (xmlXPathContextPtr) intern->ptr;
	if (ctxp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid XPath Context");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) ctxp->doc;
	if (docp == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid XPath Document Pointer");
		RETURN_FALSE;
	}

	if (context != NULL) {
		DOM_GET_OBJ(nodep, context, xmlNodePtr, nodeobj);
	}

	if (!nodep) {
		nodep = xmlDocGetRootElement(docp);
	}

	if (nodep && docp != nodep->doc) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Node From Wrong Document");
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

	xpathobjp = xmlXPathEvalExpression(expr, ctxp);
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

			MAKE_STD_ZVAL(retval);
			array_init(retval);

			if (xpathobjp->type == XPATH_NODESET && NULL != (nodesetp = xpathobjp->nodesetval)) {

				for (i = 0; i < nodesetp->nodeNr; i++) {
					xmlNodePtr node = nodesetp->nodeTab[i];
					zval *child;

					MAKE_STD_ZVAL(child);
					
					if (node->type == XML_NAMESPACE_DECL) {
						xmlNsPtr curns;
						xmlNodePtr nsparent;

						nsparent = node->_private;
						curns = xmlNewNs(NULL, node->name, NULL);
						if (node->children) {
							curns->prefix = xmlStrdup((char *) node->children);
						}
						if (node->children) {
							node = xmlNewDocNode(docp, NULL, (char *) node->children, node->name);
						} else {
							node = xmlNewDocNode(docp, NULL, "xmlns", node->name);
						}
						node->type = XML_NAMESPACE_DECL;
						node->parent = nsparent;
						node->ns = curns;
					}
					child = php_dom_create_object(node, &ret, child, (dom_object *)intern TSRMLS_CC);
					add_next_index_zval(retval, child);
				}
			}
			php_dom_create_interator(return_value, DOM_NODELIST TSRMLS_CC);
			nodeobj = (dom_object *)zend_objects_get_address(return_value TSRMLS_CC);
			dom_xpath_iter(retval, nodeobj);
			break;
		}

		case XPATH_BOOLEAN:
			RETVAL_BOOL(xpathobjp->boolval);
			break;

		case XPATH_NUMBER:
			RETVAL_DOUBLE(xpathobjp->floatval)
			break;

		case XPATH_STRING:
			RETVAL_STRING(xpathobjp->stringval, 1);
			break;

		default:
			RETVAL_NULL();
			break;
	}

	xmlXPathFreeObject(xpathobjp);
}
/* }}} */

/* {{{ proto DOMNodeList dom_xpath_query(string expr [,DOMNode context [, boolean registerNodeNS]]); */
PHP_FUNCTION(dom_xpath_query)
{
	php_xpath_eval(INTERNAL_FUNCTION_PARAM_PASSTHRU, PHP_DOM_XPATH_QUERY);
}
/* }}} end dom_xpath_query */

/* {{{ proto mixed dom_xpath_evaluate(string expr [,DOMNode context [, boolean registerNodeNS]]); */
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
	zval *array_value, **entry, *new_string;
	int  name_len = 0;
	char *name;

	DOM_GET_THIS(id);
	
	if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "a",  &array_value) == SUCCESS) {
		intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(array_value));

		while (zend_hash_get_current_data(Z_ARRVAL_P(array_value), (void **)&entry) == SUCCESS) {
			SEPARATE_ZVAL(entry);
			convert_to_string_ex(entry);

			MAKE_STD_ZVAL(new_string);
			ZVAL_LONG(new_string,1);
		
			zend_hash_update(intern->registered_phpfunctions, Z_STRVAL_PP(entry), Z_STRLEN_PP(entry) + 1, &new_string, sizeof(zval*), NULL);
			zend_hash_move_forward(Z_ARRVAL_P(array_value));
		}
		intern->registerPhpFunctions = 2;
		RETURN_TRUE;

	} else if (zend_parse_parameters_ex(ZEND_PARSE_PARAMS_QUIET, ZEND_NUM_ARGS() TSRMLS_CC, "s",  &name, &name_len) == SUCCESS) {
		intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);
		
		MAKE_STD_ZVAL(new_string);
		ZVAL_LONG(new_string,1);
		zend_hash_update(intern->registered_phpfunctions, name, name_len + 1, &new_string, sizeof(zval*), NULL);
		intern->registerPhpFunctions = 2;
		
	} else {
		intern = (dom_xpath_object *)zend_object_store_get_object(id TSRMLS_CC);
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
