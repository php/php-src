/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
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
#include "php_dom.h"

/*
* class domimplementation 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-102161490
* Since: 
*/

zend_function_entry php_dom_domimplementation_class_functions[] = {
	PHP_FALIAS(hasFeature, dom_domimplementation_has_feature, NULL)
	PHP_FALIAS(createDocumentType, dom_domimplementation_create_document_type, NULL)
	PHP_FALIAS(createDocument, dom_domimplementation_create_document, NULL)
	PHP_FALIAS(getFeature, dom_domimplementation_get_feature, NULL)
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */


/* {{{ proto boolean dom_domimplementation_has_feature(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-5CED94D7
Since: 
*/
PHP_FUNCTION(dom_domimplementation_has_feature)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementation_has_feature */


/* {{{ proto domdocumenttype dom_domimplementation_create_document_type(string qualifiedName, string publicId, string systemId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocType
Since: DOM Level 2
*/
PHP_FUNCTION(dom_domimplementation_create_document_type)
{
	zval *rv = NULL;
	xmlDtd *doctype;
	int ret, name_len, publicid_len, systemid_len;
	char *name, *publicid, *systemid;
	xmlChar *pch1 = NULL, *pch2 = NULL, *localname = NULL;
	xmlURIPtr uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sss", &name, &name_len, &publicid, &publicid_len, &systemid, &systemid_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "qualifiedName is required");
		RETURN_FALSE;
	}

	if (publicid_len > 0)
		pch1 = publicid;
	if (systemid_len > 0)
		pch2 = systemid;

	uri = xmlParseURI(name);
	if (uri->opaque != NULL) {
		localname = xmlStrdup(uri->opaque);
		if (xmlStrchr(localname, (xmlChar) ':') != NULL) {
			php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
			xmlFreeURI(uri);
			xmlFree(localname);
			RETURN_FALSE;
		}
	} else {
		localname = xmlStrdup(name);
	}

	/* TODO: Test that localname has no invalid chars 
	php_dom_throw_error(INVALID_CHARACTER_ERR, TSRMLS_CC);
	*/

	xmlFreeURI(uri);

	doctype = xmlCreateIntSubset(NULL, localname, pch1, pch2);
	xmlFree(localname);

	DOM_RET_OBJ(rv, (xmlNodePtr) doctype, &ret);
}
/* }}} end dom_domimplementation_create_document_type */


/* {{{ proto domdocument dom_domimplementation_create_document(string namespaceURI, string qualifiedName, documenttype doctype);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Level-2-Core-DOM-createDocument
Since: DOM Level 2
*/
PHP_FUNCTION(dom_domimplementation_create_document)
{
	zval *node = NULL, *rv = NULL;
	xmlDoc *docp;
	xmlNode *nodep;
	xmlDtdPtr doctype = NULL, dtd = NULL;
	xmlNsPtr nsptr = NULL;
	int ret, uri_len = 0, name_len = 0;
	char *uri, *name;
	xmlChar *prefix = NULL, *localname = NULL;
	xmlURIPtr uristruct;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|sso", &uri, &uri_len, &name, &name_len, &node) == FAILURE) {
		return;
	}

	if (doctype != NULL) {
		DOM_GET_OBJ(doctype, node, xmlDtdPtr);
		if (doctype->type == XML_DOCUMENT_TYPE_NODE) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid DocumentType object");
			RETURN_FALSE;
		}
		if (doctype->doc != NULL) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, &return_value TSRMLS_CC);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "DocumentType: Wrong Document");
			RETURN_FALSE;
		}
	}

	if (uri_len > 0 || name_len > 0 || doctype != NULL) {
		if (name_len == 0 && uri_len > 0) {
			php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
		}
		if (name_len > 0) {
			uristruct = xmlParseURI(name);
			if (uristruct->opaque != NULL) {
				prefix = xmlStrdup(uristruct->scheme);
				localname = xmlStrdup(uristruct->opaque);
				if (xmlStrchr(localname, (xmlChar) ':') != NULL) {
					php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
					xmlFreeURI(uristruct);
					xmlFree(prefix);
					xmlFree(localname);
					RETURN_FALSE;
				}
				if (!strcmp (prefix, "xml") && strcmp(uri, XML_XML_NAMESPACE)) {
					php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
					xmlFreeURI(uristruct);
					xmlFree(prefix);
					xmlFree(localname);
					RETURN_FALSE;
				}
			} else {
				localname = xmlStrdup(name);
			}

			/* TODO: Test that localname has no invalid chars 
			php_dom_throw_error(INVALID_CHARACTER_ERR, TSRMLS_CC);
			*/

			xmlFreeURI(uristruct);

			if (uri_len > 0) {
				if (prefix == NULL) {
					php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
					xmlFree(localname);
					RETURN_FALSE;
				} else {
					if ((nsptr = xmlNewNs(NULL, uri, prefix)) == NULL) {
						php_dom_throw_error(NAMESPACE_ERR, &return_value TSRMLS_CC);
						xmlFree(prefix);
						xmlFree(localname);
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
						RETURN_FALSE;	
					}
				}
			}
			if (prefix != NULL) {
				xmlFree(prefix);
			}
		}
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
		dtd = xmlCreateIntSubset (docp, doctype->name, 
			doctype->ExternalID, doctype->SystemID);
	}

	if (localname != NULL) {
		nodep = xmlNewDocNode (docp, nsptr, localname, NULL);
		if (!nodep) {
			xmlFreeDoc(docp);
			xmlFree(localname);
			/* Need some type of error here */
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unexpected Error");
			RETURN_FALSE;
		}
		xmlDocSetRootElement(docp, nodep);
		xmlFree(localname);
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) docp, &ret);
}
/* }}} end dom_domimplementation_create_document */


/* {{{ proto domnode dom_domimplementation_get_feature(string feature, string version);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#DOMImplementation3-getFeature
Since: DOM Level 3
*/
PHP_FUNCTION(dom_domimplementation_get_feature)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_domimplementation_get_feature */
