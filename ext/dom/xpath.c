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


/*
* class domxpath 
*/

#if defined(LIBXML_XPATH_ENABLED)

zend_function_entry php_dom_xpath_class_functions[] = {
	PHP_FALIAS(domxpath, dom_xpath_xpath, NULL)
	PHP_FALIAS(register_namespace, dom_xpath_register_ns, NULL)
	PHP_FALIAS(query, dom_xpath_query, NULL)
	{NULL, NULL, NULL}
};

/* {{{ proto domxpath dom_xpath_xpath(domDocument doc); */
PHP_FUNCTION(dom_xpath_xpath)
{
	zval *id, *doc;
	xmlDocPtr docp = NULL;
	dom_object *docobj, *intern;
	xmlXPathContextPtr ctx, oldctx;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &doc) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, doc, xmlDocPtr, docobj);

	ctx = xmlXPathNewContext(docp);
	if (ctx == NULL) {
		RETURN_FALSE;
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		oldctx = (xmlXPathContextPtr)intern->ptr;
		if (oldctx != NULL) {
			decrement_document_reference(intern TSRMLS_CC);
			xmlXPathFreeContext(oldctx);
		}
		intern->ptr = ctx;
		intern->document = docobj->document;
		increment_document_reference(intern, docp TSRMLS_CC);
	}
}
/* }}} end dom_xpath_xpath */

/* {{{ proto domdocument document	document */
int dom_xpath_document_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp = NULL;
	xmlXPathContextPtr ctx;
	int ret;

	ctx = (xmlXPathContextPtr) obj->ptr;

	if (ctx) {
		docp = (xmlDocPtr) ctx->doc;
	} else {
		printf("NONE");
	}

	ALLOC_ZVAL(*retval);
	if (NULL == (*retval = php_dom_create_object((xmlNodePtr) docp, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* {{{ proto boolean dom_xpath_register_ns(string prefix, string uri); */
PHP_FUNCTION(dom_xpath_register_ns)
{
	zval *id;
	xmlXPathContextPtr ctxp;
	int prefix_len, ns_uri_len;
	dom_object *intern;
	unsigned char *prefix, *ns_uri;

	DOM_GET_THIS(id);

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);

	ctxp = (xmlXPathContextPtr) intern->ptr;
	if (ctxp == NULL) {
		php_error(E_WARNING, "Invalid XPath Context");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &prefix, &prefix_len, &ns_uri, &ns_uri_len) == FAILURE) {
		RETURN_FALSE;
	}
	fprintf(stderr,"register %s=%s\n",prefix, ns_uri);
	if (xmlXPathRegisterNs(ctxp, prefix, ns_uri) != 0) {
		RETURN_FALSE
	}
	RETURN_TRUE;
}

/* {{{ proto domnodelist dom_xpath_query(string expr [,domNode context]); */
PHP_FUNCTION(dom_xpath_query)
{
	zval *id, *context = NULL;
	xmlXPathContextPtr ctxp;
	xmlNodePtr nodep = NULL;
	xmlXPathObjectPtr xpathobjp;
	int expr_len, ret, nsnbr = 0;
	dom_object *intern, *nodeobj;
	char *expr;
	xmlDoc *docp = NULL;
	xmlNsPtr *ns;

	DOM_GET_THIS(id);

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);

	ctxp = (xmlXPathContextPtr) intern->ptr;
	if (ctxp == NULL) {
		php_error(E_WARNING, "Invalid XPath Context");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) ctxp->doc;
	if (docp == NULL) {
		php_error(E_WARNING, "Invalid XPath Document Pointer");
		RETURN_FALSE;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|o", &expr, &expr_len, &context) == FAILURE) {
		RETURN_FALSE;
	}

	if (context != NULL) {
		DOM_GET_OBJ(nodep, context, xmlNodePtr, nodeobj);
	}

	if (!nodep) {
		nodep = xmlDocGetRootElement(docp);
	}

	if (nodep && docp != nodep->doc) {
		php_error(E_WARNING, "Node From Wrong Document");
		RETURN_FALSE;
	}

	ctxp->node = nodep;

	/* Register namespaces in the node */
	ns = xmlGetNsList(docp, nodep);

    if (ns != NULL) {
        while (ns[nsnbr] != NULL)
	    nsnbr++;
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

	if (xpathobjp->type ==  XPATH_NODESET) {
		int i;
		xmlNodeSetPtr nodesetp;

		if (NULL == (nodesetp = xpathobjp->nodesetval)) {
			xmlXPathFreeObject (xpathobjp);
			RETURN_FALSE;
		}

		array_init(return_value);

		for (i = 0; i < nodesetp->nodeNr; i++) {
			xmlNodePtr node = nodesetp->nodeTab[i];
			zval *child;
			MAKE_STD_ZVAL(child);

			child = php_dom_create_object(node, &ret, NULL, child, intern TSRMLS_CC);
			add_next_index_zval(return_value, child);
		}
	}

	xmlXPathFreeObject(xpathobjp);
}
/* }}} end dom_xpath_query */

#endif /* LIBXML_XPATH_ENABLED */

/* }}} */
#endif
