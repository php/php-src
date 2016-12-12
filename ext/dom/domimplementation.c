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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementation_get_feature, 0, 0, 2)
	ZEND_ARG_INFO(0, feature)
	ZEND_ARG_INFO(0, version)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementation_has_feature, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementation_create_documenttype, 0, 0, 3)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_INFO(0, publicId)
	ZEND_ARG_INFO(0, systemId)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_implementation_create_document, 0, 0, 3)
	ZEND_ARG_INFO(0, namespaceURI)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_OBJ_INFO(0, docType, DOMDocumentType, 0)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMImplementation
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-102161490
* Since:
*/

const zend_function_entry php_dom_domimplementation_class_functions[] = {
	PHP_ME(domimplementation, getFeature, arginfo_dom_implementation_get_feature, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(domimplementation, hasFeature, arginfo_dom_implementation_has_feature, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(domimplementation, createDocumentType, arginfo_dom_implementation_create_documenttype, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(domimplementation, createDocument, arginfo_dom_implementation_create_document, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_FE_END
};

/* {{{ proto boolean dom_domimplementation_has_feature(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-5CED94D7
Since:
*/
PHP_METHOD(domimplementation, hasFeature)
{
	size_t feature_len, version_len;
	char *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &feature, &feature_len, &version, &version_len) == FAILURE) {
		return;
	}

	if (dom_has_feature(feature, version)) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_domimplementation_has_feature */

/* {{{ proto DOMDocumentType dom_domimplementation_create_document_type(string qualifiedName, string publicId, string systemId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocType
Since: DOM Level 2
*/
PHP_METHOD(domimplementation, createDocumentType)
{
	xmlDtd *doctype;
	int ret;
	size_t name_len = 0, publicid_len = 0, systemid_len = 0;
	char *name = NULL, *publicid = NULL, *systemid = NULL;
	xmlChar *pch1 = NULL, *pch2 = NULL, *localname = NULL;
	xmlURIPtr uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|sss", &name, &name_len, &publicid, &publicid_len, &systemid, &systemid_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL, E_WARNING, "qualifiedName is required");
		RETURN_FALSE;
	}

	if (publicid_len > 0) {
		pch1 = (xmlChar *) publicid;
	}
	if (systemid_len > 0) {
		pch2 = (xmlChar *) systemid;
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

/* {{{ proto DOMDocument dom_domimplementation_create_document(string namespaceURI, string qualifiedName, DOMDocumentType doctype);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocument
Since: DOM Level 2
*/
PHP_METHOD(domimplementation, createDocument)
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

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ssO", &uri, &uri_len, &name, &name_len, &node, dom_documenttype_class_entry) == FAILURE) {
		return;
	}

	if (node != NULL) {
		DOM_GET_OBJ(doctype, node, xmlDtdPtr, doctobj);
		if (doctype->type == XML_DOCUMENT_TYPE_NODE) {
			php_error_docref(NULL, E_WARNING, "Invalid DocumentType object");
			RETURN_FALSE;
		}
		if (doctype->doc != NULL) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, 1);
			RETURN_FALSE;
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
		RETURN_FALSE;
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
			/* Need some type of error here */
			php_error_docref(NULL, E_WARNING, "Unexpected Error");
			RETURN_FALSE;
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

/* {{{ proto DOMNode dom_domimplementation_get_feature(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMImplementation3-getFeature
Since: DOM Level 3
*/
PHP_METHOD(domimplementation, getFeature)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementation_get_feature */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
