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
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"
#include "private_data.h"
#include "xml_serializer.h"
#include "internal_helpers.h"
#include "dom_properties.h"
#include <libxml/SAX.h>
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/relaxng.h>
#include <libxml/xmlschemas.h>
#endif

/*
* class DOMDocument extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-i-Document
* Since:
*/

/* {{{ docType	DOMDocumentType
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-B63ED1A31
Since:
*/
zend_result dom_document_doctype_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	xmlDtdPtr dtdptr = xmlGetIntSubset(docp);

	php_dom_create_nullable_object((xmlNodePtr) dtdptr, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ implementation	DOMImplementation
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1B793EBA
Since:
*/
zend_result dom_document_implementation_read(dom_object *obj, zval *retval)
{
	php_dom_create_implementation(retval, false);
	return SUCCESS;
}
/* }}} */

/* {{{ documentElement	DOMElement
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-87CD092
Since:
*/
zend_result dom_document_document_element_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	xmlNodePtr root = xmlDocGetRootElement(docp);

	php_dom_create_nullable_object(root, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ encoding	string
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-encoding
Since: DOM Level 3
*/
zend_result dom_document_encoding_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	const char *encoding = (const char *) docp->encoding;

	if (encoding != NULL) {
		ZVAL_STRING(retval, encoding);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result dom_document_actual_encoding_read(dom_object *obj, zval *retval)
{
	zend_error(E_DEPRECATED, "Property DOMDocument::$actualEncoding is deprecated");
	if (UNEXPECTED(EG(exception))) {
		return FAILURE;
	}

	return dom_document_encoding_read(obj, retval);
}

zend_result dom_document_encoding_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	/* Typed property, can only be IS_STRING or IS_NULL. */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING || Z_TYPE_P(newval) == IS_NULL);

	if (Z_TYPE_P(newval) == IS_NULL) {
		goto invalid_encoding;
	}

	const zend_string *str = Z_STR_P(newval);

	xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler(ZSTR_VAL(str));

	if (handler != NULL) {
		xmlCharEncCloseFunc(handler);
		if (docp->encoding != NULL) {
			xmlFree(BAD_CAST docp->encoding);
		}
		docp->encoding = xmlStrdup((const xmlChar *) ZSTR_VAL(str));
	} else {
		goto invalid_encoding;
	}

	return SUCCESS;

invalid_encoding:
	zend_value_error("Invalid document encoding");
	return FAILURE;
}

/* }}} */

/* {{{ standalone	boolean
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-standalone
Since: DOM Level 3
*/
zend_result dom_document_standalone_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);
	ZVAL_BOOL(retval, docp->standalone > 0);
	return SUCCESS;
}

zend_result dom_document_standalone_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	ZEND_ASSERT(Z_TYPE_P(newval) == IS_TRUE || Z_TYPE_P(newval) == IS_FALSE);
	docp->standalone = Z_TYPE_P(newval) == IS_TRUE;

	return SUCCESS;
}

/* }}} */

/* {{{ version	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-version
Since: DOM Level 3
*/
zend_result dom_document_version_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	const char *version = (const char *) docp->version;

	if (version != NULL) {
		ZVAL_STRING(retval, version);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result dom_document_version_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	/* Cannot fail because the type is either null or a string. */
	zend_string *str = zval_get_string(newval);

	if (php_dom_follow_spec_intern(obj)) {
		if (!zend_string_equals_literal(str, "1.0") && !zend_string_equals_literal(str, "1.1")) {
			zend_value_error("Invalid XML version");
			zend_string_release_ex_noinline(str, 0);
			return FAILURE;
		}
	}

	if (docp->version != NULL) {
		xmlFree(BAD_CAST docp->version);
	}

	docp->version = xmlStrdup((const xmlChar *) ZSTR_VAL(str));

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ strictErrorChecking	boolean
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-strictErrorChecking
Since: DOM Level 3
*/
zend_result dom_document_strict_error_checking_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->stricterror);
	return SUCCESS;
}

zend_result dom_document_strict_error_checking_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->stricterror = zend_is_true(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ formatOutput	boolean
readonly=no
*/
zend_result dom_document_format_output_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->formatoutput);
	return SUCCESS;
}

zend_result dom_document_format_output_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->formatoutput = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ validateOnParse	boolean
readonly=no
*/
zend_result	dom_document_validate_on_parse_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->validateonparse);
	return SUCCESS;
}

zend_result dom_document_validate_on_parse_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->validateonparse = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ resolveExternals	boolean
readonly=no
*/
zend_result dom_document_resolve_externals_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->resolveexternals);
	return SUCCESS;
}

zend_result dom_document_resolve_externals_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->resolveexternals = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ preserveWhiteSpace	boolean
readonly=no
*/
zend_result dom_document_preserve_whitespace_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->preservewhitespace);
	return SUCCESS;
}

zend_result dom_document_preserve_whitespace_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->preservewhitespace = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ recover	boolean
readonly=no
*/
zend_result dom_document_recover_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->recover);
	return SUCCESS;
}

zend_result dom_document_recover_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->recover = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ substituteEntities	boolean
readonly=no
*/
zend_result dom_document_substitue_entities_read(dom_object *obj, zval *retval)
{
	libxml_doc_props const* doc_prop = dom_get_doc_props_read_only(obj->document);
	ZVAL_BOOL(retval, doc_prop->substituteentities);
	return SUCCESS;
}

zend_result dom_document_substitue_entities_write(dom_object *obj, zval *newval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		doc_prop->substituteentities = zend_is_true(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ documentURI	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-documentURI
Since: DOM Level 3
*/
zend_result dom_document_document_uri_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	const char *url = (const char *) docp->URL;
	if (url != NULL) {
		ZVAL_STRING(retval, url);
	} else {
		if (php_dom_follow_spec_intern(obj)) {
			ZVAL_STRING(retval, "about:blank");
		} else {
			ZVAL_NULL(retval);
		}
	}

	return SUCCESS;
}

zend_result dom_document_document_uri_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	/* Cannot fail because the type is either null or a string. */
	zend_string *str = zval_get_string(newval);

	if (docp->URL != NULL) {
		xmlFree(BAD_CAST docp->URL);
	}

	docp->URL = xmlStrdup((const xmlChar *) ZSTR_VAL(str));

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ config	DOMConfiguration
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-config
Since: DOM Level 3
*/
zend_result dom_document_config_read(dom_object *obj, zval *retval)
{
	zend_error(E_DEPRECATED, "Property DOMDocument::$config is deprecated");
	if (UNEXPECTED(EG(exception))) {
		return FAILURE;
	}

	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-2141741547
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-createelement
Since:
*/
PHP_METHOD(DOMDocument, createElement)
{
	xmlDocPtr docp;
	dom_object *intern;
	size_t value_len;
	char *value = NULL;
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STR(name)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(value, value_len)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	if (xmlValidateName(BAD_CAST ZSTR_VAL(name), 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	xmlNodePtr node = xmlNewDocNode(docp, NULL, BAD_CAST ZSTR_VAL(name), BAD_CAST value);

	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}

PHP_METHOD(Dom_Document, createElement)
{
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	zend_string *name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	if (xmlValidateName(BAD_CAST ZSTR_VAL(name), 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	if (docp->type == XML_HTML_DOCUMENT_NODE) {
		php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(intern);
		char *lower = zend_str_tolower_dup_ex(ZSTR_VAL(name), ZSTR_LEN(name));
		node = xmlNewDocRawNode(docp, php_dom_libxml_ns_mapper_ensure_html_ns(ns_mapper), BAD_CAST (lower ? lower : ZSTR_VAL(name)), NULL);
		efree(lower);
	} else {
		node = xmlNewDocNode(docp, NULL, BAD_CAST ZSTR_VAL(name), NULL);
	}

	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}
/* }}} end dom_document_create_element */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-35CB04B5
Since:
*/
PHP_METHOD(DOMDocument, createDocumentFragment)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocFragment(docp);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}
/* }}} end dom_document_create_document_fragment */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1975348127
Since:
*/
PHP_METHOD(DOMDocument, createTextNode)
{
	xmlNode *node;
	xmlDocPtr docp;
	size_t value_len;
	dom_object *intern;
	char *value;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(value, value_len)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	node = xmlNewDocText(docp, BAD_CAST value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}
/* }}} end dom_document_create_text_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1334481328
Since:
*/
PHP_METHOD(DOMDocument, createComment)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	size_t value_len;
	dom_object *intern;
	char *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocComment(docp, BAD_CAST value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}
/* }}} end dom_document_create_comment */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D26C0AF8
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-createcdatasection
Since:
*/
PHP_METHOD(DOMDocument, createCDATASection)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	size_t value_len;
	dom_object *intern;
	char *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (php_dom_follow_spec_intern(intern)) {
		if (docp->type == XML_HTML_DOCUMENT_NODE) {
			php_dom_throw_error_with_message(NOT_SUPPORTED_ERR, "This operation is not supported for HTML documents", /* strict */ true);
			RETURN_THROWS();
		}

		if (zend_memnstr(value, "]]>", strlen("]]>"), value + value_len) != NULL) {
			php_dom_throw_error_with_message(INVALID_CHARACTER_ERR, "Invalid character sequence \"]]>\" in CDATA section", /* strict */ true);
			RETURN_THROWS();
		}
	}

	node = xmlNewCDataBlock(docp, BAD_CAST value, value_len);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}
/* }}} end dom_document_create_cdatasection */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-135944439
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-createprocessinginstruction
Since:
*/
static void dom_document_create_processing_instruction(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	xmlNode *node;
	xmlDocPtr docp;
	size_t value_len, name_len = 0;
	dom_object *intern;
	char *name, *value = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), modern ? "ss" : "s|s", &name, &name_len, &value, &value_len) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	if (xmlValidateName(BAD_CAST name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (modern) {
		if (value != NULL && zend_memnstr(value, "?>", strlen("?>"), value + value_len) != NULL) {
			php_dom_throw_error_with_message(INVALID_CHARACTER_ERR, "Invalid character sequence \"?>\" in processing instruction", /* strict */ true);
			RETURN_THROWS();
		}
	}

	node = xmlNewDocPI(docp, BAD_CAST name, BAD_CAST value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, intern);
}

PHP_METHOD(DOMDocument, createProcessingInstruction)
{
	dom_document_create_processing_instruction(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Dom_Document, createProcessingInstruction)
{
	dom_document_create_processing_instruction(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_document_create_processing_instruction */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1084891198
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-createattribute
Since:
*/
PHP_METHOD(DOMDocument, createAttribute)
{
	zval *id;
	xmlAttrPtr node;
	xmlDocPtr docp;
	dom_object *intern;
	zend_string *name;

	id = ZEND_THIS;
	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_PATH_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName(BAD_CAST ZSTR_VAL(name), 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (docp->type == XML_HTML_DOCUMENT_NODE && php_dom_follow_spec_intern(intern)) {
		char *lower = zend_str_tolower_dup_ex(ZSTR_VAL(name), ZSTR_LEN(name));
		node = xmlNewDocProp(docp, BAD_CAST (lower ? lower : ZSTR_VAL(name)), NULL);
		efree(lower);
	} else {
		node = xmlNewDocProp(docp, BAD_CAST ZSTR_VAL(name), NULL);
	}

	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ((xmlNodePtr) node, intern);

}
/* }}} end dom_document_create_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-392B75AE
Since:
*/
PHP_METHOD(DOMDocument, createEntityReference)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	dom_object *intern;
	size_t name_len;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName(BAD_CAST name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	node = xmlNewReference(docp, BAD_CAST name);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ((xmlNodePtr) node, intern);
}
/* }}} end dom_document_create_entity_reference */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Core-Document-importNode
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-importnode
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, importNode)
{
	zval *node;
	xmlDocPtr docp;
	xmlNodePtr nodep, retnodep;
	dom_object *intern, *nodeobj;
	bool recursive = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &node, dom_node_class_entry, &recursive) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	DOM_GET_OBJ(nodep, node, xmlNodePtr, nodeobj);

	if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE) {
		php_error_docref(NULL, E_WARNING, "Cannot import: Node Type Not Supported");
		RETURN_FALSE;
	}

	if (nodep->doc == docp) {
		retnodep = nodep;
	} else {
		retnodep = dom_clone_node(NULL, nodep, docp, recursive);
		if (!retnodep) {
			RETURN_FALSE;
		}

		if (retnodep->type == XML_ATTRIBUTE_NODE && nodep->ns != NULL && retnodep->ns == NULL) {
			xmlNsPtr nsptr = NULL;
			xmlNodePtr root = xmlDocGetRootElement(docp);

			nsptr = xmlSearchNsByHref (nodep->doc, root, nodep->ns->href);
			if (nsptr == NULL || nsptr->prefix == NULL) {
				int errorcode;
				nsptr = dom_get_ns(root, (char *) nodep->ns->href, &errorcode, (char *) nodep->ns->prefix);

				/* If there is no root, the namespace cannot be attached to it, so we have to attach it to the old list. */
				if (nsptr != NULL && root == NULL) {
					php_libxml_set_old_ns(nodep->doc, nsptr);
				}
			}
			retnodep->ns = nsptr;
		}
	}

	DOM_RET_OBJ(retnodep, intern);
}

static void dom_modern_document_import_node(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *node_ce)
{
	zval *node;
	xmlDocPtr docp;
	xmlNodePtr nodep, retnodep;
	dom_object *intern, *nodeobj;
	bool recursive = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &node, node_ce, &recursive) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	DOM_GET_OBJ(nodep, node, xmlNodePtr, nodeobj);

	if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE) {
		php_dom_throw_error(NOT_SUPPORTED_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	if (nodep->doc == docp) {
		retnodep = nodep;
	} else {
		retnodep = dom_clone_node(php_dom_get_ns_mapper(intern), nodep, docp, recursive);
		if (!retnodep) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
			RETURN_THROWS();
		}
	}

	DOM_RET_OBJ(retnodep, intern);
}

PHP_METHOD(Dom_Document, importNode)
{
	dom_modern_document_import_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_node_class_entry);
}
/* }}} end dom_document_import_node */

PHP_METHOD(Dom_Document, importLegacyNode)
{
	dom_modern_document_import_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_node_class_entry);
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrElNS
Modern spec URL: https://dom.spec.whatwg.org/#internal-createelementns-steps
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, createElementNS)
{
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL;
	size_t value_len = 0;
	char *value = NULL;
	int errorcode;
	dom_object *intern;
	zend_string *name = NULL, *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S!S|s", &uri, &name, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	char *localname = NULL, *prefix = NULL;
	errorcode = dom_check_qname(ZSTR_VAL(name), &localname, &prefix, uri ? ZSTR_LEN(uri) : 0, ZSTR_LEN(name));

	if (errorcode == 0) {
		if (xmlValidateName(BAD_CAST localname, 0) == 0) {
			nodep = xmlNewDocNode(docp, NULL, BAD_CAST localname, BAD_CAST value);
			if (UNEXPECTED(nodep == NULL)) {
				php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
				RETURN_THROWS();
			}

			if (uri != NULL) {
				xmlNsPtr nsptr = xmlSearchNsByHref(nodep->doc, nodep, BAD_CAST ZSTR_VAL(uri));
				if (nsptr == NULL) {
					nsptr = dom_get_ns(nodep, ZSTR_VAL(uri), &errorcode, prefix);
				}
				nodep->ns = nsptr;
			}
		} else {
			errorcode = INVALID_CHARACTER_ERR;
		}
	}

	xmlFree(localname);
	xmlFree(prefix);

	if (errorcode != 0) {
		xmlFreeNode(nodep);
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	DOM_RET_OBJ(nodep, intern);
}

PHP_METHOD(Dom_Document, createElementNS)
{
	xmlDocPtr docp;
	dom_object *intern;
	zend_string *name, *uri;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STR_OR_NULL(uri)
		Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	xmlChar *localname = NULL, *prefix = NULL;
	int errorcode = dom_validate_and_extract(uri, name, &localname, &prefix);

	if (errorcode == 0) {
		php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(intern);
		xmlNsPtr ns = php_dom_libxml_ns_mapper_get_ns_raw_prefix_string(ns_mapper, prefix, xmlStrlen(prefix), uri);

		/* Try to create the node with the local name interned. */
		const xmlChar *interned_localname = xmlDictLookup(docp->dict, localname, -1);
		xmlNodePtr nodep;
		if (interned_localname == NULL) {
			nodep = xmlNewDocNodeEatName(docp, ns, localname, NULL);
		} else {
			xmlFree(localname);
			nodep = xmlNewDocNodeEatName(docp, ns, BAD_CAST interned_localname, NULL);
		}

		if (UNEXPECTED(nodep == NULL)) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		} else {
			DOM_RET_OBJ(nodep, intern);
		}
	} else {
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document));
		xmlFree(localname);
	}

	xmlFree(prefix);
}
/* }}} end dom_document_create_element_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrAttrNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-createattributens
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, createAttributeNS)
{
	zval *id;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL, root;
	xmlNsPtr nsptr;
	zend_string *name, *uri;
	xmlChar *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S!S", &uri, &name) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	root = xmlDocGetRootElement(docp);
	if (root != NULL || php_dom_follow_spec_intern(intern)) {
		errorcode = dom_validate_and_extract(uri, name, &localname, &prefix);
		if (UNEXPECTED(errorcode != 0)) {
			if (!php_dom_follow_spec_intern(intern)) {
				/* legacy behaviour */
				errorcode = NAMESPACE_ERR;
			}
			goto error;
		}

		nodep = (xmlNodePtr) xmlNewDocProp(docp, localname, NULL);
		if (UNEXPECTED(nodep == NULL)) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
			RETURN_THROWS();
		}

		if (uri != NULL && ZSTR_LEN(uri) > 0) {
			if (php_dom_follow_spec_intern(intern)) {
				php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(intern);
				nsptr = php_dom_libxml_ns_mapper_get_ns_raw_prefix_string(ns_mapper, prefix, xmlStrlen(prefix), uri);
			} else {
				nsptr = xmlSearchNsByHref(docp, root, BAD_CAST ZSTR_VAL(uri));

				if (zend_string_equals_literal(name, "xmlns") || xmlStrEqual(BAD_CAST prefix, BAD_CAST "xml")) {
					if (nsptr == NULL) {
						nsptr = xmlNewNs(NULL, BAD_CAST ZSTR_VAL(uri), BAD_CAST prefix);
						php_libxml_set_old_ns(docp, nsptr);
					}
				} else {
					if (nsptr == NULL || nsptr->prefix == NULL) {
						nsptr = dom_get_ns_unchecked(root, ZSTR_VAL(uri), prefix ? (char *) prefix : "default");
						if (UNEXPECTED(nsptr == NULL)) {
							errorcode = NAMESPACE_ERR;
						}
					}
				}
			}
			nodep->ns = nsptr;
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Document Missing Root Element");
		RETURN_FALSE;
	}

error:
	xmlFree(localname);
	xmlFree(prefix);

	if (errorcode != 0) {
		xmlFreeProp((xmlAttrPtr) nodep);
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	DOM_RET_OBJ(nodep, intern);
}
/* }}} end dom_document_create_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBId
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, getElementById)
{
	xmlDocPtr docp;
	size_t idname_len;
	dom_object *intern;
	char *idname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(idname, idname_len)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	/* If the document has not been manipulated yet, the ID cache will be in sync and we can trust its result.
	 * This check mainly exists because a lot of times people just query a document without modifying it,
	 * and we can allow quick access to IDs in that case. */
	if (!dom_is_document_cache_modified_since_parsing(intern->document)) {
		const xmlAttr *attrp = xmlGetID(docp, BAD_CAST idname);
		if (attrp && attrp->parent) {
			DOM_RET_OBJ(attrp->parent, intern);
		}
	} else {
		/* From the moment an ID is created, libxml2's behaviour is to cache that element, even
		 * if that element is not yet attached to the document. Similarly, only upon destruction of
		 * the element the ID is actually removed by libxml2. Since libxml2 has such behaviour deeply
		 * ingrained in the library, and uses the cache for various purposes, it seems like a bad
		 * idea and lost cause to fight it. */

		const xmlNode *base = (const xmlNode *) docp;
		const xmlNode *node = base->children;
		while (node != NULL) {
			if (node->type == XML_ELEMENT_NODE) {
				for (const xmlAttr *attr = node->properties; attr != NULL; attr = attr->next) {
					if (attr->atype == XML_ATTRIBUTE_ID && dom_compare_value(attr, BAD_CAST idname)) {
						DOM_RET_OBJ((xmlNodePtr) node, intern);
						return;
					}
				}
			}

			node = php_dom_next_in_tree_order(node, base);
		}
	}
}
/* }}} end dom_document_get_element_by_id */

static zend_always_inline void php_dom_transfer_document_ref_single_node(xmlNodePtr node, php_libxml_ref_obj *new_document)
{
	php_libxml_node_ptr *iteration_object_ptr = node->_private;
	if (iteration_object_ptr) {
		php_libxml_node_object *iteration_object = iteration_object_ptr->_private;
		ZEND_ASSERT(iteration_object != NULL);
		/* Must increase refcount first because we could be the last reference holder, and the document may be equal. */
		new_document->refcount++;
		php_libxml_decrement_doc_ref(iteration_object);
		iteration_object->document = new_document;
	}
}

static void php_dom_transfer_document_ref(xmlNodePtr node, php_libxml_ref_obj *new_document)
{
	if (node->children) {
		php_dom_transfer_document_ref(node->children, new_document);
	}

	while (node) {
		if (node->type == XML_ELEMENT_NODE) {
			for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
				php_dom_transfer_document_ref_single_node((xmlNodePtr) attr, new_document);
			}
		}

		php_dom_transfer_document_ref_single_node(node, new_document);
		node = node->next;
	}
}

/* Workaround for bug that was fixed in https://github.com/GNOME/libxml2/commit/4bc3ebf3eaba352fbbce2ef70ad00a3c7752478a */
#if LIBXML_VERSION < 21000
static xmlChar *libxml_copy_dicted_string(xmlDictPtr src_dict, xmlDictPtr dst_dict, xmlChar *str)
{
	if (str == NULL) {
		return NULL;
	}
	if (xmlDictOwns(src_dict, str) == 1) {
		if (dst_dict == NULL) {
			return xmlStrdup(str);
		}
		return BAD_CAST xmlDictLookup(dst_dict, str, -1);
	}
	return str;
}

static void libxml_fixup_name_and_content(xmlDocPtr src_doc, xmlDocPtr dst_doc, xmlNodePtr node)
{
	if (src_doc != NULL && dst_doc != src_doc && src_doc->dict != NULL) {
		node->name = libxml_copy_dicted_string(src_doc->dict, dst_doc->dict, BAD_CAST node->name);
		node->content = libxml_copy_dicted_string(src_doc->dict, NULL, node->content);
	}
}

static void libxml_fixup_name_and_content_element(xmlDocPtr src_doc, xmlDocPtr dst_doc, xmlNodePtr node)
{
	libxml_fixup_name_and_content(src_doc, dst_doc, node);
	for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		libxml_fixup_name_and_content(src_doc, dst_doc, (xmlNodePtr) attr);
	}

	for (xmlNodePtr child = node->children; child != NULL; child = child->next) {
		libxml_fixup_name_and_content_element(src_doc, dst_doc, child);
	}
}
#endif

bool php_dom_adopt_node(xmlNodePtr nodep, dom_object *dom_object_new_document, xmlDocPtr new_document)
{
	xmlDocPtr original_document = nodep->doc;
	php_libxml_invalidate_node_list_cache_from_doc(original_document);
	if (nodep->doc != new_document) {
		php_libxml_invalidate_node_list_cache(dom_object_new_document->document);

		/* Note for ATTRIBUTE_NODE: specified is always true in ext/dom,
		 * and since this unlink it; the owner element will be unset (i.e. parentNode). */
		if (php_dom_follow_spec_intern(dom_object_new_document)) {
			xmlUnlinkNode(nodep);
			xmlSetTreeDoc(nodep, new_document);
			php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(dom_object_new_document);
			php_dom_libxml_reconcile_modern(ns_mapper, nodep);
#if LIBXML_VERSION < 21000
			libxml_fixup_name_and_content_element(original_document, new_document, nodep);
#endif
		} else {
			int ret = xmlDOMWrapAdoptNode(NULL, original_document, nodep, new_document, NULL, /* options, unused */ 0);
			if (UNEXPECTED(ret != 0)) {
				return false;
			}
		}

		php_dom_transfer_document_ref(nodep, dom_object_new_document->document);
	} else {
		xmlUnlinkNode(nodep);
	}
	return true;
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-adoptNode
Since: DOM Level 3
Modern spec URL: https://dom.spec.whatwg.org/#dom-document-adoptnode
*/
static void dom_document_adopt_node(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zval *node_zval;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node_zval, dom_get_node_ce(modern)) == FAILURE) {
		RETURN_THROWS();
	}

	xmlNodePtr nodep;
	dom_object *dom_object_nodep;
	DOM_GET_OBJ(nodep, node_zval, xmlNodePtr, dom_object_nodep);

	if (UNEXPECTED(nodep->type == XML_DOCUMENT_NODE
		|| nodep->type == XML_HTML_DOCUMENT_NODE
		|| nodep->type == XML_DOCUMENT_TYPE_NODE
		|| nodep->type == XML_DTD_NODE
		|| nodep->type == XML_ENTITY_NODE
		|| nodep->type == XML_NOTATION_NODE)) {
		php_dom_throw_error(NOT_SUPPORTED_ERR, dom_get_strict_error(dom_object_nodep->document));
		RETURN_FALSE;
	}

	xmlDocPtr new_document;
	dom_object *dom_object_new_document;
	zval *new_document_zval = ZEND_THIS;
	DOM_GET_OBJ(new_document, new_document_zval, xmlDocPtr, dom_object_new_document);

	if (!php_dom_adopt_node(nodep, dom_object_new_document, new_document)) {
		if (modern) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
			RETURN_THROWS();
		}
		RETURN_FALSE;
	}

	RETURN_OBJ_COPY(&dom_object_nodep->std);
}

PHP_METHOD(DOMDocument, adoptNode)
{
	dom_document_adopt_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Dom_Document, adoptNode)
{
	dom_document_adopt_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_document_adopt_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-normalizeDocument
Since: DOM Level 3
*/
PHP_METHOD(DOMDocument, normalizeDocument)
{
	zval *id;
	xmlDocPtr docp;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	php_dom_normalize_legacy((xmlNodePtr) docp);
}
/* }}} end dom_document_normalize_document */

/* {{{ */
PHP_METHOD(DOMDocument, __construct)
{
	xmlDoc *docp = NULL, *olddoc;
	dom_object *intern;
	char *encoding, *version = NULL;
	size_t encoding_len = 0, version_len = 0;
	unsigned int refcount;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ss", &version, &version_len, &encoding, &encoding_len) == FAILURE) {
		RETURN_THROWS();
	}

	docp = xmlNewDoc(BAD_CAST version);

	if (!docp) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		return;
	}

	if (encoding_len > 0) {
		docp->encoding = (const xmlChar *) xmlStrdup(BAD_CAST encoding);
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	olddoc = (xmlDocPtr) dom_object_get_node(intern);
	if (olddoc != NULL) {
		php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
		refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);
		if (refcount != 0) {
			olddoc->_private = NULL;
		}
	}
	intern->document = NULL;
	if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, docp) == -1) {
		/* docp is always non-null so php_libxml_increment_doc_ref() never returns -1 */
		ZEND_UNREACHABLE();
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)docp, (void *)intern);
}
/* }}} end DOMDocument::__construct */

const char *dom_get_valid_file_path(const char *source, char *resolved_path, int resolved_path_len ) /* {{{ */
{
	xmlURI *uri;
	xmlChar *escsource;
	const char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	if (uri == NULL) {
		return NULL;
	}
	escsource = xmlURIEscapeStr(BAD_CAST source, BAD_CAST ":");
	xmlParseURIReference(uri, (char *) escsource);
	xmlFree(escsource);

	if (uri->scheme != NULL) {
		/* absolute file uris - libxml only supports localhost or empty host */
#ifdef PHP_WIN32
		if (strncasecmp(source, "file://",7) == 0 && ':' == source[8]) {
			isFileUri = 1;
			source += 7;
		} else
#endif
		if (strncasecmp(source, "file:///",8) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 8;
#else
			source += 7;
#endif
		} else if (strncasecmp(source, "file://localhost/",17) == 0) {
			isFileUri = 1;
#ifdef PHP_WIN32
			source += 17;
#else
			source += 16;
#endif
		}
	}

	file_dest = source;

	if ((uri->scheme == NULL || isFileUri)) {
		/* XXX possible buffer overflow if VCWD_REALPATH does not know size of resolved_path */
		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path)) {
			xmlFreeURI(uri);
			return NULL;
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}
/* }}} */

xmlDocPtr dom_document_parser(zval *id, dom_load_mode mode, const char *source, size_t source_len, size_t options, xmlCharEncodingHandlerPtr encoding) /* {{{ */
{
	xmlDocPtr ret;
	xmlParserCtxtPtr ctxt = NULL;
	int validate, recover, resolve_externals, keep_blanks, substitute_ent;
	int resolved_path_len;
	int old_error_reporting = 0;
	char *directory=NULL, resolved_path[MAXPATHLEN + 1];

	libxml_doc_props const* doc_props;
	if (id == NULL) {
		doc_props = dom_get_doc_props_read_only(NULL);
	} else {
		dom_object *intern = Z_DOMOBJ_P(id);
		php_libxml_ref_obj *document = intern->document;
		doc_props = dom_get_doc_props_read_only(document);
	}
	validate = doc_props->validateonparse;
	resolve_externals = doc_props->resolveexternals;
	keep_blanks = doc_props->preservewhitespace;
	substitute_ent = doc_props->substituteentities;
	recover = doc_props->recover || (options & XML_PARSE_RECOVER) == XML_PARSE_RECOVER;

	xmlInitParser();

	if (mode == DOM_LOAD_FILE) {
		if (CHECK_NULL_PATH(source, source_len)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			return NULL;
		}
		const char *file_dest = dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
		if (file_dest) {
			ctxt = xmlCreateFileParserCtxt(file_dest);
		}
	} else {
		ctxt = xmlCreateMemoryParserCtxt(source, source_len);
	}

	if (ctxt == NULL) {
		return(NULL);
	}

	if (encoding != NULL) {
		/* Note: libxml 2.12+ doesn't handle NULL encoding well. */
		(void) xmlSwitchToEncoding(ctxt, encoding);
	}

	/* If loading from memory, we need to set the base directory for the document */
	if (mode != DOM_LOAD_FILE) {
#ifdef HAVE_GETCWD
		directory = VCWD_GETCWD(resolved_path, MAXPATHLEN);
#elif defined(HAVE_GETWD)
		directory = VCWD_GETWD(resolved_path);
#endif
		if (directory) {
			if(ctxt->directory != NULL) {
				xmlFree((char *) ctxt->directory);
			}
			resolved_path_len = strlen(resolved_path);
			if (resolved_path[resolved_path_len - 1] != DEFAULT_SLASH) {
				resolved_path[resolved_path_len] = DEFAULT_SLASH;
				resolved_path[++resolved_path_len] = '\0';
			}
			ctxt->directory = (char *) xmlCanonicPath((const xmlChar *) resolved_path);
		}
	}

	ctxt->vctxt.error = php_libxml_ctx_error;
	ctxt->vctxt.warning = php_libxml_ctx_warning;

	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_libxml_ctx_error;
		ctxt->sax->warning = php_libxml_ctx_warning;
	}

	if (validate && ! (options & XML_PARSE_DTDVALID)) {
		options |= XML_PARSE_DTDVALID;
	}
	if (resolve_externals && ! (options & XML_PARSE_DTDATTR)) {
		options |= XML_PARSE_DTDATTR;
	}
	if (substitute_ent && ! (options & XML_PARSE_NOENT)) {
		options |= XML_PARSE_NOENT;
	}
	if (keep_blanks == 0 && ! (options & XML_PARSE_NOBLANKS)) {
		options |= XML_PARSE_NOBLANKS;
	}
	if (recover) {
		options |= XML_PARSE_RECOVER;
	}

	php_libxml_sanitize_parse_ctxt_options(ctxt);
	xmlCtxtUseOptions(ctxt, options);

	if (recover) {
		old_error_reporting = EG(error_reporting);
		EG(error_reporting) = old_error_reporting | E_WARNING;
	}

	xmlParseDocument(ctxt);

	if (ctxt->wellFormed || recover) {
		ret = ctxt->myDoc;
		if (recover) {
			EG(error_reporting) = old_error_reporting;
		}
		/* If loading from memory, set the base reference uri for the document */
		if (ret && ret->URL == NULL && ctxt->directory != NULL) {
			ret->URL = xmlStrdup(BAD_CAST ctxt->directory);
		}
	} else {
		ret = DOM_DOCUMENT_MALFORMED;
		xmlFreeDoc(ctxt->myDoc);
		ctxt->myDoc = NULL;
	}

	xmlFreeParserCtxt(ctxt);

	return(ret);
}
/* }}} */

static void php_dom_finish_loading_document(zval *this, zval *return_value, xmlDocPtr newdoc)
{
	if (!newdoc)
		RETURN_FALSE;

	dom_object *intern = Z_DOMOBJ_P(this);
	size_t old_modification_nr = 0;
	if (intern != NULL) {
		php_libxml_class_type class_type = PHP_LIBXML_CLASS_LEGACY;
		xmlDocPtr docp = (xmlDocPtr) dom_object_get_node(intern);
		dom_doc_propsptr doc_prop = NULL;
		if (docp != NULL) {
			const php_libxml_ref_obj *doc_ptr = intern->document;
			ZEND_ASSERT(doc_ptr != NULL); /* Must exist, we have a document */
			class_type = doc_ptr->class_type;
			old_modification_nr = doc_ptr->cache_tag.modification_nr;
			php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
			doc_prop = intern->document->doc_props;
			intern->document->doc_props = NULL;
			unsigned int refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);
			if (refcount != 0) {
				docp->_private = NULL;
			}
		}
		intern->document = NULL;
		if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc) == -1) {
			RETURN_FALSE;
		}
		intern->document->doc_props = doc_prop;
		intern->document->class_type = class_type;
	}

	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern);
	/* Since iterators should invalidate, we need to start the modification number from the old counter */
	if (old_modification_nr != 0) {
		intern->document->cache_tag.modification_nr = old_modification_nr;
		php_libxml_invalidate_node_list_cache(intern->document);
	}

	RETURN_TRUE;
}

static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	char *source;
	size_t source_len;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &source, &source_len, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (!source_len) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}
	if (ZEND_SIZE_T_INT_OVFL(source_len)) {
		php_error_docref(NULL, E_WARNING, "Input string is too long");
		RETURN_FALSE;
	}
	if (ZEND_LONG_EXCEEDS_INT(options)) {
		php_error_docref(NULL, E_WARNING, "Invalid options");
		RETURN_FALSE;
	}

	xmlDocPtr newdoc = dom_document_parser(ZEND_THIS, mode, source, source_len, options, NULL);
	if (newdoc == DOM_DOCUMENT_MALFORMED) {
		newdoc = NULL;
	}
	php_dom_finish_loading_document(ZEND_THIS, return_value, newdoc);
}

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-load
Since: DOM Level 3
*/
PHP_METHOD(DOMDocument, load)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load */

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-loadXML
Since: DOM Level 3
*/
PHP_METHOD(DOMDocument, loadXML)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_loadxml */

/* {{{ Convenience method to save to file */
PHP_METHOD(DOMDocument, save)
{
	zval *id;
	xmlDoc *docp;
	size_t file_len = 0;
	int saveempty = 0;
	dom_object *intern;
	char *file;
	zend_long options = 0;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p|l", &file, &file_len, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (file_len == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	/* encoding handled by property on doc */

	libxml_doc_props const* doc_props = dom_get_doc_props_read_only(intern->document);
	bool format = doc_props->formatoutput;
	if (options & LIBXML_SAVE_NOEMPTYTAG) {
		saveempty = xmlSaveNoEmptyTags;
		xmlSaveNoEmptyTags = 1;
	}
	zend_long bytes = intern->document->handlers->dump_doc_to_file(file, docp, format, (const char *) docp->encoding);
	if (options & LIBXML_SAVE_NOEMPTYTAG) {
		xmlSaveNoEmptyTags = saveempty;
	}
	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} end dom_document_save */

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-saveXML
Since: DOM Level 3
*/
static void dom_document_save_xml(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *node_ce)
{
	zval *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	dom_object *intern, *nodeobj;
	int old_xml_save_no_empty_tags;
	zend_long options = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!l", &nodep, node_ce, &options) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	libxml_doc_props const* doc_props = dom_get_doc_props_read_only(intern->document);
	bool format = doc_props->formatoutput;

	zend_string *res;
	if (nodep != NULL) {
		/* Dump contents of Node */
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
			RETURN_FALSE;
		}

		/* Save libxml2 global, override its value, and restore after saving (don't move me or risk breaking the state
		 * w.r.t. the implicit return in DOM_GET_OBJ). */
		old_xml_save_no_empty_tags = xmlSaveNoEmptyTags;
		xmlSaveNoEmptyTags = (options & LIBXML_SAVE_NOEMPTYTAG) ? 1 : 0;
		res = intern->document->handlers->dump_node_to_str(docp, node, format, (const char *) docp->encoding);
		xmlSaveNoEmptyTags = old_xml_save_no_empty_tags;
	} else {
		int converted_options = XML_SAVE_AS_XML;
		if (options & XML_SAVE_NO_DECL) {
			converted_options |= XML_SAVE_NO_DECL;
		}
		if (format) {
			converted_options |= XML_SAVE_FORMAT;
		}

		/* Save libxml2 global, override its value, and restore after saving. */
		old_xml_save_no_empty_tags = xmlSaveNoEmptyTags;
		xmlSaveNoEmptyTags = (options & LIBXML_SAVE_NOEMPTYTAG) ? 1 : 0;
		res = intern->document->handlers->dump_doc_to_str(docp, converted_options, (const char *) docp->encoding);
		xmlSaveNoEmptyTags = old_xml_save_no_empty_tags;
	}

	if (!res) {
		php_error_docref(NULL, E_WARNING, "Could not save document");
		RETURN_FALSE;
	} else {
		RETURN_NEW_STR(res);
	}
}

PHP_METHOD(DOMDocument, saveXML)
{
	dom_document_save_xml(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_node_class_entry);
}

PHP_METHOD(Dom_XMLDocument, saveXml)
{
	dom_document_save_xml(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_node_class_entry);
}
/* }}} end dom_document_savexml */

static xmlNodePtr php_dom_free_xinclude_node(xmlNodePtr cur) /* {{{ */
{
	xmlNodePtr xincnode;

	xincnode = cur;
	cur = cur->next;
	xmlUnlinkNode(xincnode);
	php_libxml_node_free_resource(xincnode);

	return cur;
}
/* }}} */

static void php_dom_remove_xinclude_nodes(xmlNodePtr cur) /* {{{ */
{
	while(cur) {
		if (cur->type == XML_XINCLUDE_START) {
			cur = php_dom_free_xinclude_node(cur);

			/* XML_XINCLUDE_END node will be a sibling of XML_XINCLUDE_START */
			while(cur && cur->type != XML_XINCLUDE_END) {
				/* remove xinclude processing nodes from recursive xincludes */
				if (cur->type == XML_ELEMENT_NODE) {
					   php_dom_remove_xinclude_nodes(cur->children);
				}
				cur = cur->next;
			}

			if (cur && cur->type == XML_XINCLUDE_END) {
				cur = php_dom_free_xinclude_node(cur);
			}
		} else {
			if (cur->type == XML_ELEMENT_NODE) {
				php_dom_remove_xinclude_nodes(cur->children);
			}
			cur = cur->next;
		}
	}
}
/* }}} */

static void dom_xinclude_strip_references(xmlNodePtr basep)
{
	php_libxml_node_free_resource(basep);

	xmlNodePtr current = basep->children;

	while (current) {
		php_libxml_node_free_resource(current);
		current = php_dom_next_in_tree_order(current, basep);
	}
}

/* See GH-14702.
 * We have to remove userland references to xinclude fallback nodes because libxml2 will make clones of these
 * and remove the original nodes. If the originals are removed while there are still userland references
 * this will cause memory corruption. */
static void dom_xinclude_strip_fallback_references(const xmlNode *basep)
{
	xmlNodePtr current = basep->children;

	while (current) {
		if (current->type == XML_ELEMENT_NODE && current->ns != NULL && current->_private != NULL
			&& xmlStrEqual(current->name, XINCLUDE_FALLBACK)
			&& (xmlStrEqual(current->ns->href, XINCLUDE_NS) || xmlStrEqual(current->ns->href, XINCLUDE_OLD_NS))) {
			dom_xinclude_strip_references(current);
		}

		current = php_dom_next_in_tree_order(current, basep);
	}
}

static int dom_perform_xinclude(xmlDocPtr docp, dom_object *intern, zend_long flags)
{
	dom_xinclude_strip_fallback_references((const xmlNode *) docp);

	PHP_LIBXML_SANITIZE_GLOBALS(xinclude);
	int err = xmlXIncludeProcessFlags(docp, (int)flags);
	PHP_LIBXML_RESTORE_GLOBALS(xinclude);

	/* XML_XINCLUDE_START and XML_XINCLUDE_END nodes need to be removed as these
	are added via xmlXIncludeProcess to mark beginning and ending of xincluded document
	but are not wanted in resulting document - must be done even if err as it could fail after
	having processed some xincludes */
	xmlNodePtr root = docp->children;
	while (root && root->type != XML_ELEMENT_NODE && root->type != XML_XINCLUDE_START) {
		root = root->next;
	}
	if (root) {
		php_dom_remove_xinclude_nodes(root);
	}

	php_libxml_invalidate_node_list_cache(intern->document);

	return err;
}

/* {{{ Substitutues xincludes in a DomDocument */
PHP_METHOD(DOMDocument, xinclude)
{
	xmlDoc *docp;
	zend_long flags = 0;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZEND_LONG_EXCEEDS_INT(flags)) {
		php_error_docref(NULL, E_WARNING, "Invalid flags");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	int err = dom_perform_xinclude(docp, intern, flags);

	if (err) {
		RETVAL_LONG(err);
	} else {
		RETVAL_FALSE;
	}
}

PHP_METHOD(Dom_XMLDocument, xinclude)
{
	xmlDoc *docp;
	zend_long flags = 0;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZEND_LONG_EXCEEDS_INT(flags)) {
		zend_argument_value_error(1, "is too large");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	int err = dom_perform_xinclude(docp, intern, flags);

	if (err < 0) {
		php_dom_throw_error(INVALID_MODIFICATION_ERR, /* strict */ true);
	} else {
		RETURN_LONG(err);
	}
}
/* }}} */

/* {{{ Since: DOM extended */
PHP_METHOD(DOMDocument, validate)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	xmlValidCtxt *cvp;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	PHP_LIBXML_SANITIZE_GLOBALS(validate);
	cvp = xmlNewValidCtxt();

	cvp->userData = NULL;
	cvp->error    = (xmlValidityErrorFunc) php_libxml_error_handler;
	cvp->warning  = (xmlValidityErrorFunc) php_libxml_error_handler;

	if (xmlValidateDocument(cvp, docp)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}
	PHP_LIBXML_RESTORE_GLOBALS(validate);

	xmlFreeValidCtxt(cvp);

}
/* }}} */

#ifdef LIBXML_SCHEMAS_ENABLED
static void dom_document_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL;
	const char *valid_file = NULL;
	size_t source_len = 0;
	int valid_opts = 0;
	zend_long flags = 0;
	xmlSchemaParserCtxtPtr  parser;
	xmlSchemaPtr            sptr;
	xmlSchemaValidCtxtPtr   vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &source, &source_len, &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (!source_len) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	PHP_LIBXML_SANITIZE_GLOBALS(new_parser_ctxt);

	switch (type) {
	case DOM_LOAD_FILE:
		if (CHECK_NULL_PATH(source, source_len)) {
			PHP_LIBXML_RESTORE_GLOBALS(new_parser_ctxt);
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		valid_file = dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
		if (!valid_file) {
			PHP_LIBXML_RESTORE_GLOBALS(new_parser_ctxt);
			php_error_docref(NULL, E_WARNING, "Invalid Schema file source");
			RETURN_FALSE;
		}
		parser = xmlSchemaNewParserCtxt(valid_file);
		break;
	case DOM_LOAD_STRING:
		parser = xmlSchemaNewMemParserCtxt(source, source_len);
		/* If loading from memory, we need to set the base directory for the document
		   but it is not apparent how to do that for schema's */
		break;
	default:
		return;
	}

	xmlSchemaSetParserErrors(parser,
		(xmlSchemaValidityErrorFunc) php_libxml_error_handler,
		(xmlSchemaValidityWarningFunc) php_libxml_error_handler,
		parser);
	sptr = xmlSchemaParse(parser);
	xmlSchemaFreeParserCtxt(parser);
	PHP_LIBXML_RESTORE_GLOBALS(new_parser_ctxt);
	if (!sptr) {
		if (!EG(exception)) {
			php_error_docref(NULL, E_WARNING, "Invalid Schema");
		}
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlSchemaNewValidCtxt(sptr);
	if (!vptr) {
		xmlSchemaFree(sptr);
		zend_throw_error(NULL, "Invalid Schema Validation Context");
		RETURN_THROWS();
	}

	if (flags & XML_SCHEMA_VAL_VC_I_CREATE) {
		valid_opts |= XML_SCHEMA_VAL_VC_I_CREATE;
	}

	PHP_LIBXML_SANITIZE_GLOBALS(validate);
	xmlSchemaSetValidOptions(vptr, valid_opts);
	xmlSchemaSetValidErrors(vptr, php_libxml_error_handler, php_libxml_error_handler, vptr);
	is_valid = xmlSchemaValidateDoc(vptr, docp);
	xmlSchemaFree(sptr);
	xmlSchemaFreeValidCtxt(vptr);
	PHP_LIBXML_RESTORE_GLOBALS(validate);

	if (is_valid == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMDocument, schemaValidate)
{
	dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_schema_validate_file */

/* {{{ */
PHP_METHOD(DOMDocument, schemaValidateSource)
{
	dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_schema_validate */

static void dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL;
	const char *valid_file = NULL;
	size_t source_len = 0;
	xmlRelaxNGParserCtxtPtr parser;
	xmlRelaxNGPtr           sptr;
	xmlRelaxNGValidCtxtPtr  vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &source, &source_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (!source_len) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		if (CHECK_NULL_PATH(source, source_len)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		valid_file = dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
		if (!valid_file) {
			php_error_docref(NULL, E_WARNING, "Invalid RelaxNG file source");
			RETURN_FALSE;
		}
		parser = xmlRelaxNGNewParserCtxt(valid_file);
		break;
	case DOM_LOAD_STRING:
		parser = xmlRelaxNGNewMemParserCtxt(source, source_len);
		/* If loading from memory, we need to set the base directory for the document
		   but it is not apparent how to do that for schema's */
		break;
	default:
		return;
	}

	PHP_LIBXML_SANITIZE_GLOBALS(parse);
	xmlRelaxNGSetParserErrors(parser,
		(xmlRelaxNGValidityErrorFunc) php_libxml_error_handler,
		(xmlRelaxNGValidityWarningFunc) php_libxml_error_handler,
		parser);
	sptr = xmlRelaxNGParse(parser);
	xmlRelaxNGFreeParserCtxt(parser);
	PHP_LIBXML_RESTORE_GLOBALS(parse);
	if (!sptr) {
		php_error_docref(NULL, E_WARNING, "Invalid RelaxNG");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlRelaxNGNewValidCtxt(sptr);
	if (!vptr) {
		xmlRelaxNGFree(sptr);
		zend_throw_error(NULL, "Invalid RelaxNG Validation Context");
		RETURN_THROWS();
	}

	xmlRelaxNGSetValidErrors(vptr, php_libxml_error_handler, php_libxml_error_handler, vptr);
	is_valid = xmlRelaxNGValidateDoc(vptr, docp);
	xmlRelaxNGFree(sptr);
	xmlRelaxNGFreeValidCtxt(vptr);

	if (is_valid == 0) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMDocument, relaxNGValidate)
{
	dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_relaxNG_validate_file */

/* {{{ */
PHP_METHOD(DOMDocument, relaxNGValidateSource)
{
	dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_relaxNG_validate_xml */

#endif

#ifdef LIBXML_HTML_ENABLED

static void dom_load_html(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	char *source;
	size_t source_len;
	zend_long options = 0;
	htmlParserCtxtPtr ctxt;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &source, &source_len, &options) == FAILURE) {
		RETURN_THROWS();
	}

	if (!source_len) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	if (ZEND_LONG_EXCEEDS_INT(options)) {
		php_error_docref(NULL, E_WARNING, "Invalid options");
		RETURN_FALSE;
	}

	if (mode == DOM_LOAD_FILE) {
		if (CHECK_NULL_PATH(source, source_len)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		ctxt = htmlCreateFileParserCtxt(source, NULL);
	} else {
		if (ZEND_SIZE_T_INT_OVFL(source_len)) {
			php_error_docref(NULL, E_WARNING, "Input string is too long");
			RETURN_FALSE;
		}
		ctxt = htmlCreateMemoryParserCtxt(source, (int)source_len);
	}

	if (!ctxt) {
		RETURN_FALSE;
	}


	ctxt->vctxt.error = php_libxml_ctx_error;
	ctxt->vctxt.warning = php_libxml_ctx_warning;
	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_libxml_ctx_error;
		ctxt->sax->warning = php_libxml_ctx_warning;
	}
	php_libxml_sanitize_parse_ctxt_options(ctxt);
	if (options) {
		htmlCtxtUseOptions(ctxt, (int)options);
	}
	htmlParseDocument(ctxt);
	xmlDocPtr newdoc = ctxt->myDoc;
	htmlFreeParserCtxt(ctxt);

	php_dom_finish_loading_document(ZEND_THIS, return_value, newdoc);
}
/* }}} */

/* {{{ Since: DOM extended */
PHP_METHOD(DOMDocument, loadHTMLFile)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load_html_file */

/* {{{ Since: DOM extended */
PHP_METHOD(DOMDocument, loadHTML)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_load_html */

/* {{{ Convenience method to save to file as html */
PHP_METHOD(DOMDocument, saveHTMLFile)
{
	zval *id;
	xmlDoc *docp;
	size_t file_len;
	int bytes, format;
	dom_object *intern;
	char *file;
	const char *encoding;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (file_len == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);


	encoding = (const char *) htmlGetMetaEncoding(docp);

	libxml_doc_props const* doc_props = dom_get_doc_props_read_only(intern->document);
	format = doc_props->formatoutput;
	bytes = htmlSaveFileFormat(file, docp, encoding, format);

	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} end dom_document_save_html_file */

/* {{{ Convenience method to output as html */
PHP_METHOD(DOMDocument, saveHTML)
{
	zval *id, *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	xmlOutputBufferPtr outBuf;
	xmlBufferPtr buf;
	dom_object *intern, *nodeobj;
	int format;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(),
		"|O!", &nodep, dom_node_class_entry)
		== FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	libxml_doc_props const* doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;

	if (nodep != NULL) {
		/* Dump contents of Node */
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
			RETURN_FALSE;
		}

		buf = xmlBufferCreate();
		if (!buf) {
			php_error_docref(NULL, E_WARNING, "Could not fetch buffer");
			RETURN_FALSE;
		}
		outBuf = xmlOutputBufferCreateBuffer(buf, NULL);
		if (!outBuf) {
			xmlBufferFree(buf);
			php_error_docref(NULL, E_WARNING, "Could not fetch output buffer");
			RETURN_FALSE;
		}

		if (node->type == XML_DOCUMENT_FRAG_NODE) {
			for (node = node->children; node; node = node->next) {
				htmlNodeDumpFormatOutput(outBuf, docp, node, NULL, format);
				if (outBuf->error) {
					break;
				}
			}
		} else {
			htmlNodeDumpFormatOutput(outBuf, docp, node, NULL, format);
		}
		if (!outBuf->error) {
			xmlOutputBufferFlush(outBuf);
			const xmlChar *mem = xmlBufferContent(buf);
			if (!mem) {
				RETVAL_FALSE;
			} else {
				int size = xmlBufferLength(buf);
				RETVAL_STRINGL((const char*) mem, size);
			}
		} else {
			php_error_docref(NULL, E_WARNING, "Error dumping HTML node");
			RETVAL_FALSE;
		}
		xmlOutputBufferClose(outBuf);
		xmlBufferFree(buf);
	} else {
		xmlChar *mem = NULL;
		int size = 0;
		htmlDocDumpMemoryFormat(docp, &mem, &size, format);
		if (!size || !mem) {
			RETVAL_FALSE;
		} else {
			RETVAL_STRINGL((const char*) mem, size);
		}
		xmlFree(mem);
	}

}
/* }}} end dom_document_save_html */

#endif  /* defined(LIBXML_HTML_ENABLED) */

/* {{{ Register extended class used to create base node type */
static void dom_document_register_node_class(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zend_class_entry *basece = dom_get_node_ce(modern), *ce = NULL;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "CC!", &basece, &ce) == FAILURE) {
		RETURN_THROWS();
	}

	if (basece->ce_flags & ZEND_ACC_ABSTRACT) {
		zend_argument_value_error(1, "must not be an abstract class");
		RETURN_THROWS();
	}

	if (ce == NULL || instanceof_function(ce, basece)) {
		if (UNEXPECTED(ce != NULL && (ce->ce_flags & ZEND_ACC_ABSTRACT))) {
			zend_argument_value_error(2, "must not be an abstract class");
			RETURN_THROWS();
		}
		DOM_GET_THIS_INTERN(intern);
		dom_set_doc_classmap(intern->document, basece, ce);
		if (!modern) {
			RETVAL_TRUE;
		}
		return;
	}

	zend_argument_error(NULL, 2, "must be a class name derived from %s or null, %s given", ZSTR_VAL(basece->name), ZSTR_VAL(ce->name));
	RETURN_THROWS();
}

PHP_METHOD(DOMDocument, registerNodeClass)
{
	dom_document_register_node_class(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(Dom_Document, registerNodeClass)
{
	dom_document_register_node_class(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-replacechildren
Since:
*/
PHP_METHOD(DOMDocument, replaceChildren)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_replace_children(intern, args, argc);
}
/* }}} */

#endif  /* HAVE_LIBXML && HAVE_DOM */
