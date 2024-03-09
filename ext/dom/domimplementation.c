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
Modern URL: https://dom.spec.whatwg.org/#dom-domimplementation-createdocumenttype
Since: DOM Level 2
*/
PHP_METHOD(DOMImplementation, createDocumentType)
{
	xmlDtd *doctype;
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
			php_dom_throw_error(NAMESPACE_ERR, true);
			xmlFreeURI(uri);
			xmlFree(localname);
			RETURN_FALSE;
		}
	} else {
		localname = xmlStrdup((xmlChar *) name);
	}

	if (uri) {
		xmlFreeURI(uri);
	}

	doctype = xmlCreateIntSubset(NULL, localname, pch1, pch2);
	xmlFree(localname);

	if (doctype == NULL) {
		php_error_docref(NULL, E_WARNING, "Unable to create DocumentType");
		RETURN_FALSE;
	}

	DOM_RET_OBJ((xmlNodePtr) doctype, NULL);
}

PHP_METHOD(DOM_Implementation, createDocumentType)
{
	size_t name_len, publicid_len = 0, systemid_len = 0;
	const char *name, *publicid = NULL, *systemid = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ppp", &name, &name_len, &publicid, &publicid_len, &systemid, &systemid_len) != SUCCESS) {
		RETURN_THROWS();
	}

	/* 1. Validate qualifiedName. */
	if (xmlValidateQName(BAD_CAST name, 0) != 0) {
		php_dom_throw_error(NAMESPACE_ERR, true);
		RETURN_THROWS();
	}

	/* 2. Return a new doctype, with qualifiedName as its name, publicId as its public ID, and systemId as its system ID ... */
	xmlDtdPtr doctype = xmlCreateIntSubset(
		NULL,
		BAD_CAST name,
		publicid_len ? BAD_CAST publicid : NULL,
		systemid_len ? BAD_CAST systemid : NULL
	);
	if (UNEXPECTED(doctype == NULL)) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}

	php_dom_instantiate_object_helper(
		return_value,
		dom_modern_documenttype_class_entry,
		(xmlNodePtr) doctype,
		NULL
	);
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
	int errorcode = 0;
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
			/* As the new document is the context node, and the default for strict error checking
			 * is true, this will always throw. */
			php_dom_throw_error(WRONG_DOCUMENT_ERR, true);
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
		php_dom_throw_error(errorcode, true);
		RETURN_THROWS();
	}

	/* currently letting libxml2 set the version string */
	docp = xmlNewDoc(NULL);
	if (!docp) {
		if (localname != NULL) {
			xmlFree(localname);
		}
		/* See above for strict error checking argument. */
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
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
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
			RETURN_THROWS();
		}

		nodep->nsDef = nsptr;

		xmlDocSetRootElement(docp, nodep);
		xmlFree(localname);
	}

	DOM_RET_OBJ((xmlNodePtr) docp, NULL);

	if (doctobj != NULL) {
		doctobj->document = ((dom_object *)((php_libxml_node_ptr *)docp->_private)->_private)->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)doctobj, docp);
	}
}

PHP_METHOD(DOM_Implementation, createDocument)
{
	zval *dtd = NULL;
	xmlDtdPtr doctype = NULL;
	zend_string *uri = NULL, *qualified_name = zend_empty_string;
	dom_object *doctobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "P!P|O!", &uri, &qualified_name, &dtd, dom_modern_documenttype_class_entry) != SUCCESS) {
		RETURN_THROWS();
	}

	if (dtd != NULL) {
		DOM_GET_OBJ(doctype, dtd, xmlDtdPtr, doctobj);
	}

	xmlDocPtr document = NULL;
	xmlChar *localname = NULL, *prefix = NULL;
	php_dom_libxml_ns_mapper *ns_mapper = php_dom_libxml_ns_mapper_create();

	/* 1. Let document be a new XMLDocument. */
	document = xmlNewDoc(BAD_CAST "1.0");
	if (UNEXPECTED(document == NULL)) {
		goto oom;
	}
	document->encoding = xmlStrdup(BAD_CAST "UTF-8");

	/* 2. Let element be null. */
	xmlNodePtr element = NULL;

	/* 3. If qualifiedName is not the empty string, then set element to the result of running the internal createElementNS steps. */
	if (ZSTR_LEN(qualified_name) != 0) {
		int errorcode = dom_validate_and_extract(uri, qualified_name, &localname, &prefix);

		if (EXPECTED(errorcode == 0)) {
			xmlNsPtr ns = php_dom_libxml_ns_mapper_get_ns_raw_prefix_string(ns_mapper, prefix, xmlStrlen(prefix), uri);
			element = xmlNewDocNode(document, ns, localname, NULL);
			if (UNEXPECTED(element == NULL)) {
				goto oom;
			}
			xmlFree(localname);
			xmlFree(prefix);
			localname = NULL;
			prefix = NULL;
		} else {
			php_dom_throw_error(errorcode, /* strict */ true);
			goto error;
		}
	}

	/* 8. Return document.
	 *    => This is done here already to gain access to the dom_object */
	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_xml_document_class_entry,
		(xmlNodePtr) document,
		NULL
	);
	intern->document->class_type = PHP_LIBXML_CLASS_MODERN;
	intern->document->private_data = php_dom_libxml_ns_mapper_header(ns_mapper);

	/* 4. If doctype is non-null, append doctype to document. */
	if (doctype != NULL) {
		php_dom_adopt_node((xmlNodePtr) doctype, intern, document);
		xmlAddChild((xmlNodePtr) document, (xmlNodePtr) doctype);
		doctype->doc = document;
		document->intSubset = (xmlDtdPtr) doctype;
		ZEND_ASSERT(doctype->parent == document);
	}

	/* 5. If element is non-null, append element to document. */
	if (element != NULL) {
		xmlAddChild((xmlNodePtr) document, element);
	}

	/* 6. document’s origin is this’s associated document’s origin.
	 *    => We don't store the origin in ext/dom. */

	/* 7. document’s content type is determined by namespace:
	 *    => We don't store the content type in ext/dom. */

	return;

oom:
	php_dom_throw_error(INVALID_STATE_ERR, true);
error:
	xmlFree(localname);
	xmlFree(prefix);
	xmlFreeDoc(document);
	php_dom_libxml_ns_mapper_destroy(ns_mapper);
	RETURN_THROWS();
}
/* }}} end dom_domimplementation_create_document */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-domimplementation-createhtmldocument */
PHP_METHOD(DOM_Implementation, createHTMLDocument)
{
	const char *title = NULL;
	size_t title_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|p!", &title, &title_len) != SUCCESS) {
		RETURN_THROWS();
	}

	/* 1. Let doc be a new document that is an HTML document. */
	xmlDocPtr doc = php_dom_create_html_doc();
	if (UNEXPECTED(doc == NULL)) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}
	doc->encoding = xmlStrdup(BAD_CAST "UTF-8");

	/* 2. Set doc’s content type to "text/html".
	 *    => We don't store the content type in ext/dom. */

	/* 3. Append a new doctype, with "html" as its name and with its node document set to doc, to doc. */
	xmlDtdPtr dtd = xmlCreateIntSubset(doc, BAD_CAST "html", NULL, NULL);

	php_dom_libxml_ns_mapper *ns_mapper = php_dom_libxml_ns_mapper_create();
	xmlNsPtr html_ns = php_dom_libxml_ns_mapper_ensure_html_ns(ns_mapper);

	/* 4. Append the result of creating an element given doc, html, and the HTML namespace, to doc. */
	xmlNodePtr html_element = xmlNewDocRawNode(doc, html_ns, BAD_CAST "html", NULL);
	xmlAddChild((xmlNodePtr) doc, html_element);

	/* 5. Append the result of creating an element given doc, head, and the HTML namespace, to the html element created earlier. */
	xmlNodePtr head_element = xmlNewDocRawNode(doc, html_ns, BAD_CAST "head", NULL);
	xmlAddChild(html_element, head_element);

	/* 6. If title is given: */
	xmlNodePtr title_element = NULL;
	if (title != NULL) {
		/* 6.1. Append the result of creating an element given doc, title, and the HTML namespace, to the head element created earlier. */
		/* 6.2. Append the result of creating a text node given doc and title, to the title element created earlier. */
		title_element = xmlNewDocRawNode(doc, html_ns, BAD_CAST "title", BAD_CAST title);
		xmlAddChild(head_element, title_element);
	}

	/* 7. Append the result of creating an element given doc, body, and the HTML namespace, to the html element created earlier. */
	xmlNodePtr body_element = xmlNewDocRawNode(doc, html_ns, BAD_CAST "body", NULL);
	xmlAddChild(html_element, body_element);

	/* 8. doc’s origin is this’s associated document’s origin.
	 *    => We don't store the origin in ext/dom. */

	if (UNEXPECTED(dtd == NULL || html_element == NULL || head_element == NULL || (title != NULL && title_element == NULL) || body_element == NULL)) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		xmlFreeDoc(doc);
		php_dom_libxml_ns_mapper_destroy(ns_mapper);
		RETURN_THROWS();
	}

	/* 9. Return doc. */
	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_html_document_class_entry,
		(xmlNodePtr) doc,
		NULL
	);
	intern->document->class_type = PHP_LIBXML_CLASS_MODERN;
	intern->document->private_data = php_dom_libxml_ns_mapper_header(ns_mapper);
}
/* }}} */

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

	zend_throw_error(NULL, "Not yet implemented");
	RETURN_THROWS();
}
/* }}} end dom_domimplementation_get_feature */

#endif
