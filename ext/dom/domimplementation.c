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

/*
* class DOMImplementation
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-102161490
* Since:
*/

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-5CED94D7
Since:
*/
PHP_METHOD(DOMImplementation, hasFeature)
{
	zend_string *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &feature, &version) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(dom_has_feature(feature, version));
}
/* }}} end dom_domimplementation_has_feature */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocType
Since: DOM Level 2
*/
PHP_METHOD(DOMImplementation, createDocumentType)
{
	xmlDtd *doctype;
	int ret;
	size_t name_len = 0, publicid_len = 0, systemid_len = 0;
	char *name = NULL, *publicid = NULL, *systemid = NULL;
	xmlChar *pch1 = NULL, *pch2 = NULL, *localname = NULL;
	xmlURIPtr uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|ss", &name, &name_len, &publicid, &publicid_len, &systemid, &systemid_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	if (publicid_len > 0) {
		pch1 = (xmlChar *) publicid;
	}
	if (systemid_len > 0) {
		pch2 = (xmlChar *) systemid;
	}

	if (strstr(name, "%00")) {
		php_error_docref(NULL, E_WARNING, "URI must not contain percent-encoded NUL bytes");
		RETURN_FALSE;
	}

	uri = xmlParseURI(name);
	if (uri != NULL && uri->opaque != NULL) {
		localname = xmlStrdup((xmlChar *) uri->opaque);
		if (xmlStrchr(localname, (xmlChar) ':') != NULL) {
			php_dom_throw_error(NAMESPACE_ERR, 1);
			xmlFreeURI(uri);
			xmlFree(localname);
			RETURN_FALSE;
		}
	} else {
		localname = xmlStrdup((xmlChar *) name);
	}

	/* TODO: Test that localname has no invalid chars
	php_dom_throw_error(INVALID_CHARACTER_ERR,);
	*/

	if (uri) {
		xmlFreeURI(uri);
	}

	doctype = xmlCreateIntSubset(NULL, localname, pch1, pch2);
	xmlFree(localname);

	if (doctype == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create DocumentType");
		RETURN_FALSE;
	}

	DOM_RET_OBJ((xmlNodePtr) doctype, &ret, NULL);
}
/* }}} end dom_domimplementation_create_document_type */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocument
Since: DOM Level 2
*/
PHP_METHOD(DOMImplementation, createDocument)
{
	zval *node = NULL;
	xmlDoc *docp;
	xmlNode *nodep;
	xmlDtdPtr doctype = NULL;
	xmlNsPtr nsptr = NULL;
	int ret, errorcode = 0;
	size_t uri_len = 0, name_len = 0;
	char *uri = NULL, *name = NULL;
	char *prefix = NULL, *localname = NULL;
	dom_object *doctobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s!sO!", &uri, &uri_len, &name, &name_len, &node, dom_documenttype_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	if (node != NULL) {
		DOM_GET_OBJ(doctype, node, xmlDtdPtr, doctobj);
		if (doctype->type == XML_DOCUMENT_TYPE_NODE) {
			zend_argument_value_error(3, "is an invalid DocumentType object");
			RETURN_THROWS();
		}
		if (doctype->doc != NULL) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, 1);
			RETURN_THROWS();
		}
	} else {
		doctobj = NULL;
	}

	if (name_len > 0) {
		errorcode = dom_check_qname(name, &localname, &prefix, 1, name_len);
		if (errorcode == 0 && uri_len > 0
			&& ((nsptr = xmlNewNs(NULL, (xmlChar *) uri, (xmlChar *) prefix)) == NULL)
		) {
			errorcode = NAMESPACE_ERR;
		}
	}

	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		if (localname != NULL) {
			xmlFree(localname);
		}
		php_dom_throw_error(errorcode, 1);
		RETURN_THROWS();
	}

	/* currently letting libxml2 set the version string */
	docp = xmlNewDoc(NULL);
	if (!docp) {
		if (localname != NULL) {
			xmlFree(localname);
		}
		RETURN_FALSE;
	}

	if (doctype != NULL) {
		docp->intSubset = doctype;
		doctype->parent = docp;
		doctype->doc = docp;
		docp->children = (xmlNodePtr) doctype;
		docp->last = (xmlNodePtr) doctype;
	}

	if (localname != NULL) {
		nodep = xmlNewDocNode(docp, nsptr, (xmlChar *) localname, NULL);
		if (!nodep) {
			if (doctype != NULL) {
				docp->intSubset = NULL;
				doctype->parent = NULL;
				doctype->doc = NULL;
				docp->children = NULL;
				docp->last = NULL;
			}
			xmlFreeDoc(docp);
			xmlFree(localname);
			/* Need some better type of error here */
			php_dom_throw_error(PHP_ERR, 1);
			RETURN_THROWS();
		}

		nodep->nsDef = nsptr;

		xmlDocSetRootElement(docp, nodep);
		xmlFree(localname);
	}

	DOM_RET_OBJ((xmlNodePtr) docp, &ret, NULL);

	if (doctobj != NULL) {
		doctobj->document = ((dom_object *)((php_libxml_node_ptr *)docp->_private)->_private)->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)doctobj, docp);
	}
}
/* }}} end dom_domimplementation_create_document */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMImplementation3-getFeature
Since: DOM Level 3
*/
PHP_METHOD(DOMImplementation, getFeature)
{
	size_t feature_len, version_len;
	char *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &feature, &feature_len, &version, &version_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementation_get_feature */

#endif
