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
#include <libxml/SAX.h>
#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/relaxng.h>
#include <libxml/xmlschemas.h>
#endif

typedef struct _idsIterator idsIterator;
struct _idsIterator {
	xmlChar *elementId;
	xmlNode *element;
};

#define DOM_LOAD_STRING 0
#define DOM_LOAD_FILE 1

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
int dom_document_doctype_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	xmlDtdPtr dtdptr;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	dtdptr = xmlGetIntSubset(docp);
	if (!dtdptr) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object((xmlNodePtr) dtdptr, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ implementation	DOMImplementation
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1B793EBA
Since:
*/
int dom_document_implementation_read(dom_object *obj, zval *retval)
{
	php_dom_create_implementation(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ documentElement	DOMElement
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-87CD092
Since:
*/
int dom_document_document_element_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	xmlNode *root;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	root = xmlDocGetRootElement(docp);
	if (!root) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(root, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ encoding	string
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-encoding
Since: DOM Level 3
*/
int dom_document_encoding_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	char *encoding;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	encoding = (char *) docp->encoding;

	if (encoding != NULL) {
		ZVAL_STRING(retval, encoding);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

zend_result dom_document_encoding_write(dom_object *obj, zval *newval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	zend_string *str;
	xmlCharEncodingHandlerPtr handler;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	handler = xmlFindCharEncodingHandler(ZSTR_VAL(str));

	if (handler != NULL) {
		xmlCharEncCloseFunc(handler);
		if (docp->encoding != NULL) {
			xmlFree((xmlChar *)docp->encoding);
		}
		docp->encoding = xmlStrdup((const xmlChar *) ZSTR_VAL(str));
	} else {
		zend_value_error("Invalid document encoding");
		return FAILURE;
	}

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ standalone	boolean
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-standalone
Since: DOM Level 3
*/
int dom_document_standalone_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	ZVAL_BOOL(retval, docp->standalone);
	return SUCCESS;
}

int dom_document_standalone_write(dom_object *obj, zval *newval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	zend_long standalone;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	standalone = zval_get_long(newval);
	docp->standalone = ZEND_NORMALIZE_BOOL(standalone);

	return SUCCESS;
}

/* }}} */

/* {{{ version	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-version
Since: DOM Level 3
*/
int dom_document_version_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	char *version;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	version = (char *) docp->version;

	if (version != NULL) {
		ZVAL_STRING(retval, version);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

int dom_document_version_write(dom_object *obj, zval *newval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	zend_string *str;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	if (docp->version != NULL) {
		xmlFree((xmlChar *) docp->version );
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
int dom_document_strict_error_checking_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->stricterror);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_strict_error_checking_write(dom_object *obj, zval *newval)
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
int dom_document_format_output_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->formatoutput);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_format_output_write(dom_object *obj, zval *newval)
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
int	dom_document_validate_on_parse_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->validateonparse);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_validate_on_parse_write(dom_object *obj, zval *newval)
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
int dom_document_resolve_externals_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->resolveexternals);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_resolve_externals_write(dom_object *obj, zval *newval)
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
int dom_document_preserve_whitespace_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->preservewhitespace);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_preserve_whitespace_write(dom_object *obj, zval *newval)
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
int dom_document_recover_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->recover);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_recover_write(dom_object *obj, zval *newval)
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
int dom_document_substitue_entities_read(dom_object *obj, zval *retval)
{
	if (obj->document) {
		dom_doc_propsptr doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(retval, doc_prop->substituteentities);
	} else {
		ZVAL_FALSE(retval);
	}
	return SUCCESS;
}

int dom_document_substitue_entities_write(dom_object *obj, zval *newval)
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
int dom_document_document_uri_read(dom_object *obj, zval *retval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	char *url;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	url = (char *) docp->URL;
	if (url != NULL) {
		ZVAL_STRING(retval, url);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

int dom_document_document_uri_write(dom_object *obj, zval *newval)
{
	xmlDoc *docp = (xmlDocPtr) dom_object_get_node(obj);
	zend_string *str;

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	if (docp->URL != NULL) {
		xmlFree((xmlChar *) docp->URL);
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
int dom_document_config_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-2141741547
Since:
*/
PHP_METHOD(DOMDocument, createElement)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	int ret;
	size_t name_len, value_len;
	char *name, *value = NULL;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	node = xmlNewDocNode(docp, NULL, (xmlChar *) name, (xmlChar *) value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, &ret, intern);
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
	int ret;

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

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_document_fragment */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1975348127
Since:
*/
PHP_METHOD(DOMDocument, createTextNode)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	size_t value_len;
	dom_object *intern;
	char *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocText(docp, (xmlChar *) value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, &ret, intern);
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
	int ret;
	size_t value_len;
	dom_object *intern;
	char *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocComment(docp, (xmlChar *) value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_comment */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D26C0AF8
Since:
*/
PHP_METHOD(DOMDocument, createCDATASection)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	size_t value_len;
	dom_object *intern;
	char *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewCDataBlock(docp, (xmlChar *) value, value_len);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_cdatasection */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-135944439
Since:
*/
PHP_METHOD(DOMDocument, createProcessingInstruction)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret;
	size_t value_len, name_len = 0;
	dom_object *intern;
	char *name, *value = NULL;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	node = xmlNewPI((xmlChar *) name, (xmlChar *) value);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	node->doc = docp;

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_processing_instruction */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1084891198
Since:
*/
PHP_METHOD(DOMDocument, createAttribute)
{
	zval *id;
	xmlAttrPtr node;
	xmlDocPtr docp;
	int ret;
	size_t name_len;
	dom_object *intern;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	node = xmlNewDocProp(docp, (xmlChar *) name, NULL);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ((xmlNodePtr) node, &ret, intern);

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
	int ret;
	size_t name_len;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	node = xmlNewReference(docp, (xmlChar *) name);
	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	DOM_RET_OBJ((xmlNodePtr) node, &ret, intern);
}
/* }}} end dom_document_create_entity_reference */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-A6C9094
Since:
*/
PHP_METHOD(DOMDocument, getElementsByTagName)
{
	zval *id;
	xmlDocPtr docp;
	size_t name_len;
	dom_object *intern, *namednode;
	char *name;
	xmlChar *local;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	php_dom_create_iterator(return_value, DOM_NODELIST);
	namednode = Z_DOMOBJ_P(return_value);
	local = xmlCharStrndup(name, name_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, NULL);
}
/* }}} end dom_document_get_elements_by_tag_name */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Core-Document-importNode
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, importNode)
{
	zval *id, *node;
	xmlDocPtr docp;
	xmlNodePtr nodep, retnodep;
	dom_object *intern, *nodeobj;
	int ret;
	bool recursive = 0;
	/* See http://www.xmlsoft.org/html/libxml-tree.html#xmlDocCopyNode for meaning of values */
	int extended_recursive;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|b", &node, dom_node_class_entry, &recursive) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	DOM_GET_OBJ(nodep, node, xmlNodePtr, nodeobj);

	if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE
		|| nodep->type == XML_DOCUMENT_TYPE_NODE) {
		php_error_docref(NULL, E_WARNING, "Cannot import: Node Type Not Supported");
		RETURN_FALSE;
	}

	if (nodep->doc == docp) {
		retnodep = nodep;
	} else {
		extended_recursive = recursive;
		if ((recursive == 0) && (nodep->type == XML_ELEMENT_NODE)) {
			extended_recursive = 2;
		}
		retnodep = xmlDocCopyNode(nodep, docp, extended_recursive);
		if (!retnodep) {
			RETURN_FALSE;
		}

		if ((retnodep->type == XML_ATTRIBUTE_NODE) && (nodep->ns != NULL)) {
			xmlNsPtr nsptr = NULL;
			xmlNodePtr root = xmlDocGetRootElement(docp);

			nsptr = xmlSearchNsByHref (nodep->doc, root, nodep->ns->href);
			if (nsptr == NULL) {
				int errorcode;
				nsptr = dom_get_ns(root, (char *) nodep->ns->href, &errorcode, (char *) nodep->ns->prefix);
			}
			xmlSetNs(retnodep, nsptr);
		}
	}

	DOM_RET_OBJ((xmlNodePtr) retnodep, &ret, intern);
}
/* }}} end dom_document_import_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrElNS
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, createElementNS)
{
	zval *id;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL;
	xmlNsPtr nsptr = NULL;
	int ret;
	size_t uri_len = 0, name_len = 0, value_len = 0;
	char *uri, *name, *value = NULL;
	char *localname = NULL, *prefix = NULL;
	int errorcode;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s|s", &uri, &uri_len, &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		if (xmlValidateName((xmlChar *) localname, 0) == 0) {
			nodep = xmlNewDocNode(docp, NULL, (xmlChar *) localname, (xmlChar *) value);
			if (nodep != NULL && uri != NULL) {
				nsptr = xmlSearchNsByHref(nodep->doc, nodep, (xmlChar *) uri);
				if (nsptr == NULL) {
					nsptr = dom_get_ns(nodep, uri, &errorcode, prefix);
				}
				xmlSetNs(nodep, nsptr);
			}
		} else {
			errorcode = INVALID_CHARACTER_ERR;
		}
	}

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		if (nodep != NULL) {
			xmlFreeNode(nodep);
		}
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}


	nodep->ns = nsptr;

	DOM_RET_OBJ(nodep, &ret, intern);
}
/* }}} end dom_document_create_element_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrAttrNS
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, createAttributeNS)
{
	zval *id;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL, root;
	xmlNsPtr nsptr;
	int ret;
	size_t uri_len = 0, name_len = 0;
	char *uri, *name;
	char *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	root = xmlDocGetRootElement(docp);
	if (root != NULL) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			if (xmlValidateName((xmlChar *) localname, 0) == 0) {
				nodep = (xmlNodePtr) xmlNewDocProp(docp, (xmlChar *) localname, NULL);
				if (nodep != NULL && uri_len > 0) {
					nsptr = xmlSearchNsByHref(nodep->doc, root, (xmlChar *) uri);
					if (nsptr == NULL) {
						nsptr = dom_get_ns(root, uri, &errorcode, prefix);
					}
					xmlSetNs(nodep, nsptr);
				}
			} else {
				errorcode = INVALID_CHARACTER_ERR;
			}
		}
	} else {
		php_error_docref(NULL, E_WARNING, "Document Missing Root Element");
		RETURN_FALSE;
	}

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		if (nodep != NULL) {
			xmlFreeProp((xmlAttrPtr) nodep);
		}
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(nodep, &ret, intern);
}
/* }}} end dom_document_create_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBTNNS
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, getElementsByTagNameNS)
{
	zval *id;
	xmlDocPtr docp;
	size_t uri_len, name_len;
	dom_object *intern, *namednode;
	char *uri, *name;
	xmlChar *local, *nsuri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	php_dom_create_iterator(return_value, DOM_NODELIST);
	namednode = Z_DOMOBJ_P(return_value);
	local = xmlCharStrndup(name, name_len);
	nsuri = xmlCharStrndup(uri ? uri : "", uri_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, nsuri);
}
/* }}} end dom_document_get_elements_by_tag_name_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBId
Since: DOM Level 2
*/
PHP_METHOD(DOMDocument, getElementById)
{
	zval *id;
	xmlDocPtr docp;
	xmlAttrPtr  attrp;
	int ret;
	size_t idname_len;
	dom_object *intern;
	char *idname;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &idname, &idname_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	attrp = xmlGetID(docp, (xmlChar *) idname);

	if (attrp && attrp->parent) {
		DOM_RET_OBJ((xmlNodePtr) attrp->parent, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_document_get_element_by_id */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-adoptNode
Since: DOM Level 3
*/
PHP_METHOD(DOMDocument, adoptNode)
{
	zval *nodep = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &nodep, dom_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_NOT_IMPLEMENTED();
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

	dom_normalize((xmlNodePtr) docp);
}
/* }}} end dom_document_normalize_document */

/* {{{ */
PHP_METHOD(DOMDocument, __construct)
{
	xmlDoc *docp = NULL, *olddoc;
	dom_object *intern;
	char *encoding, *version = NULL;
	size_t encoding_len = 0, version_len = 0;
	int refcount;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|ss", &version, &version_len, &encoding, &encoding_len) == FAILURE) {
		RETURN_THROWS();
	}

	docp = xmlNewDoc((xmlChar *) version);

	if (!docp) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return;
	}

	if (encoding_len > 0) {
		docp->encoding = (const xmlChar *) xmlStrdup((xmlChar *) encoding);
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	if (intern != NULL) {
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
}
/* }}} end DOMDocument::__construct */

char *_dom_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len ) /* {{{ */
{
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr((xmlChar *) source, (xmlChar *) ":");
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

static xmlDocPtr dom_document_parser(zval *id, int mode, char *source, size_t source_len, size_t options) /* {{{ */
{
	xmlDocPtr ret;
	xmlParserCtxtPtr ctxt = NULL;
	dom_doc_propsptr doc_props;
	dom_object *intern;
	php_libxml_ref_obj *document = NULL;
	int validate, recover, resolve_externals, keep_blanks, substitute_ent;
	int resolved_path_len;
	int old_error_reporting = 0;
	char *directory=NULL, resolved_path[MAXPATHLEN + 1];

	if (id != NULL) {
		intern = Z_DOMOBJ_P(id);
		document = intern->document;
	}

	doc_props = dom_get_doc_props(document);
	validate = doc_props->validateonparse;
	resolve_externals = doc_props->resolveexternals;
	keep_blanks = doc_props->preservewhitespace;
	substitute_ent = doc_props->substituteentities;
	recover = doc_props->recover;

	if (document == NULL) {
		efree(doc_props);
	}

	xmlInitParser();

	if (mode == DOM_LOAD_FILE) {
		char *file_dest;
		if (CHECK_NULL_PATH(source, source_len)) {
			zend_value_error("Path to document must not contain any null bytes");
			return NULL;
		}
		file_dest = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
		if (file_dest) {
			ctxt = xmlCreateFileParserCtxt(file_dest);
		}

	} else {
		ctxt = xmlCreateMemoryParserCtxt(source, source_len);
	}

	if (ctxt == NULL) {
		return(NULL);
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

	xmlCtxtUseOptions(ctxt, options);

	ctxt->recovery = recover;
	if (recover) {
		old_error_reporting = EG(error_reporting);
		EG(error_reporting) = old_error_reporting | E_WARNING;
	}

	xmlParseDocument(ctxt);

	if (ctxt->wellFormed || recover) {
		ret = ctxt->myDoc;
		if (ctxt->recovery) {
			EG(error_reporting) = old_error_reporting;
		}
		/* If loading from memory, set the base reference uri for the document */
		if (ret && ret->URL == NULL && ctxt->directory != NULL) {
			ret->URL = xmlStrdup((xmlChar *) ctxt->directory);
		}
	} else {
		ret = NULL;
		xmlFreeDoc(ctxt->myDoc);
		ctxt->myDoc = NULL;
	}

	xmlFreeParserCtxt(ctxt);

	return(ret);
}
/* }}} */

/* {{{ static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) */
static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) {
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_doc_propsptr doc_prop;
	dom_object *intern;
	char *source;
	size_t source_len;
	int refcount, ret;
	zend_long options = 0;

	id = getThis();
	if (id != NULL && ! instanceof_function(Z_OBJCE_P(id), dom_document_class_entry)) {
		id = NULL;
	}

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

	newdoc = dom_document_parser(id, mode, source, source_len, options);

	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL) {
		intern = Z_DOMOBJ_P(id);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} end dom_parser_document */

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
	int bytes, format, saveempty = 0;
	dom_object *intern;
	dom_doc_propsptr doc_props;
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

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;
	if (options & LIBXML_SAVE_NOEMPTYTAG) {
		saveempty = xmlSaveNoEmptyTags;
		xmlSaveNoEmptyTags = 1;
	}
	bytes = xmlSaveFormatFileEnc(file, docp, NULL, format);
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
PHP_METHOD(DOMDocument, saveXML)
{
	zval *id, *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	xmlBufferPtr buf;
	xmlChar *mem;
	dom_object *intern, *nodeobj;
	dom_doc_propsptr doc_props;
	int size, format, saveempty = 0;
	zend_long options = 0;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!l", &nodep, dom_node_class_entry, &options) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	doc_props = dom_get_doc_props(intern->document);
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
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			saveempty = xmlSaveNoEmptyTags;
			xmlSaveNoEmptyTags = 1;
		}
		xmlNodeDump(buf, docp, node, 0, format);
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			xmlSaveNoEmptyTags = saveempty;
		}
		mem = (xmlChar*) xmlBufferContent(buf);
		if (!mem) {
			xmlBufferFree(buf);
			RETURN_FALSE;
		}
		RETVAL_STRING((char *) mem);
		xmlBufferFree(buf);
	} else {
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			saveempty = xmlSaveNoEmptyTags;
			xmlSaveNoEmptyTags = 1;
		}
		/* Encoding is handled from the encoding property set on the document */
		xmlDocDumpFormatMemory(docp, &mem, &size, format);
		if (options & LIBXML_SAVE_NOEMPTYTAG) {
			xmlSaveNoEmptyTags = saveempty;
		}
		if (!size || !mem) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL((char *) mem, size);
		xmlFree(mem);
	}
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

/* {{{ Substitutues xincludes in a DomDocument */
PHP_METHOD(DOMDocument, xinclude)
{
	zval *id;
	xmlDoc *docp;
	xmlNodePtr root;
	zend_long flags = 0;
	int err;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|l", &flags) == FAILURE) {
		RETURN_THROWS();
	}

	if (ZEND_LONG_EXCEEDS_INT(flags)) {
		php_error_docref(NULL, E_WARNING, "Invalid flags");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	err = xmlXIncludeProcessFlags(docp, (int)flags);

	/* XML_XINCLUDE_START and XML_XINCLUDE_END nodes need to be removed as these
	are added via xmlXIncludeProcess to mark beginning and ending of xincluded document
	but are not wanted in resulting document - must be done even if err as it could fail after
	having processed some xincludes */
	root = (xmlNodePtr) docp->children;
	while(root && root->type != XML_ELEMENT_NODE && root->type != XML_XINCLUDE_START) {
		root = root->next;
	}
	if (root) {
		php_dom_remove_xinclude_nodes(root);
	}

	if (err) {
		RETVAL_LONG(err);
	} else {
		RETVAL_FALSE;
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

	cvp = xmlNewValidCtxt();

	cvp->userData = NULL;
	cvp->error    = (xmlValidityErrorFunc) php_libxml_error_handler;
	cvp->warning  = (xmlValidityErrorFunc) php_libxml_error_handler;

	if (xmlValidateDocument(cvp, docp)) {
		RETVAL_TRUE;
	} else {
		RETVAL_FALSE;
	}

	xmlFreeValidCtxt(cvp);

}
/* }}} */

#ifdef LIBXML_SCHEMAS_ENABLED
static void _dom_document_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
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

	switch (type) {
	case DOM_LOAD_FILE:
		if (CHECK_NULL_PATH(source, source_len)) {
			zend_argument_value_error(1, "must not contain any null bytes");
			RETURN_THROWS();
		}
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
		if (!valid_file) {
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

	xmlSchemaSetValidOptions(vptr, valid_opts);
	xmlSchemaSetValidErrors(vptr, php_libxml_error_handler, php_libxml_error_handler, vptr);
	is_valid = xmlSchemaValidateDoc(vptr, docp);
	xmlSchemaFree(sptr);
	xmlSchemaFreeValidCtxt(vptr);

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
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_schema_validate_file */

/* {{{ */
PHP_METHOD(DOMDocument, schemaValidateSource)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_schema_validate */

static void _dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
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
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN);
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

	xmlRelaxNGSetParserErrors(parser,
		(xmlRelaxNGValidityErrorFunc) php_libxml_error_handler,
		(xmlRelaxNGValidityWarningFunc) php_libxml_error_handler,
		parser);
	sptr = xmlRelaxNGParse(parser);
	xmlRelaxNGFreeParserCtxt(parser);
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
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_relaxNG_validate_file */

/* {{{ */
PHP_METHOD(DOMDocument, relaxNGValidateSource)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_relaxNG_validate_xml */

#endif

#ifdef LIBXML_HTML_ENABLED

static void dom_load_html(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_object *intern;
	dom_doc_propsptr doc_prop;
	char *source;
	size_t source_len;
	int refcount, ret;
	zend_long options = 0;
	htmlParserCtxtPtr ctxt;

	id = getThis();

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
	if (options) {
		htmlCtxtUseOptions(ctxt, (int)options);
	}
	htmlParseDocument(ctxt);
	newdoc = ctxt->myDoc;
	htmlFreeParserCtxt(ctxt);

	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL && instanceof_function(Z_OBJCE_P(id), dom_document_class_entry)) {
		intern = Z_DOMOBJ_P(id);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
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
	dom_doc_propsptr doc_props;
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

	doc_props = dom_get_doc_props(intern->document);
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
	xmlChar *mem = NULL;
	int format;
	dom_doc_propsptr doc_props;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(),
		"|O!", &nodep, dom_node_class_entry)
		== FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	doc_props = dom_get_doc_props(intern->document);
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
			mem = (xmlChar*) xmlBufferContent(buf);
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
		int size = 0;
		htmlDocDumpMemoryFormat(docp, &mem, &size, format);
		if (!size || !mem) {
			RETVAL_FALSE;
		} else {
			RETVAL_STRINGL((const char*) mem, size);
		}
		if (mem)
			xmlFree(mem);
	}

}
/* }}} end dom_document_save_html */

#endif  /* defined(LIBXML_HTML_ENABLED) */

/* {{{ Register extended class used to create base node type */
PHP_METHOD(DOMDocument, registerNodeClass)
{
	zval *id;
	xmlDoc *docp;
	zend_class_entry *basece = dom_node_class_entry, *ce = NULL;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "CC!", &basece, &ce) == FAILURE) {
		RETURN_THROWS();
	}

	if (ce == NULL || instanceof_function(ce, basece)) {
		DOM_GET_OBJ(docp, id, xmlDocPtr, intern);
		dom_set_doc_classmap(intern->document, basece, ce);
		RETURN_TRUE;
	}

	zend_argument_error(NULL, 2, "must be a class name derived from %s or null, %s given", ZSTR_VAL(basece->name), ZSTR_VAL(ce->name));
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-append
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMDocument, append)
{
	int argc;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_append(intern, args, argc);
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-prepend
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMDocument, prepend)
{
	int argc;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "+", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_prepend(intern, args, argc);
}
/* }}} */

#endif  /* HAVE_LIBXML && HAVE_DOM */
