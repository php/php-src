/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2004 The PHP Group                                |
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
* class domdocument extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-i-Document
* Since: 
*/

zend_function_entry php_dom_document_class_functions[] = {
	PHP_FALIAS(createElement, dom_document_create_element, NULL)
	PHP_FALIAS(createDocumentFragment, dom_document_create_document_fragment, NULL)
	PHP_FALIAS(createTextNode, dom_document_create_text_node, NULL)
	PHP_FALIAS(createComment, dom_document_create_comment, NULL)
	PHP_FALIAS(createCDATASection, dom_document_create_cdatasection, NULL)
	PHP_FALIAS(createProcessingInstruction, dom_document_create_processing_instruction, NULL)
	PHP_FALIAS(createAttribute, dom_document_create_attribute, NULL)
	PHP_FALIAS(createEntityReference, dom_document_create_entity_reference, NULL)
	PHP_FALIAS(getElementsByTagName, dom_document_get_elements_by_tag_name, NULL)
	PHP_FALIAS(importNode, dom_document_import_node, NULL)
	PHP_FALIAS(createElementNS, dom_document_create_element_ns, NULL)
	PHP_FALIAS(createAttributeNS, dom_document_create_attribute_ns, NULL)
	PHP_FALIAS(getElementsByTagNameNS, dom_document_get_elements_by_tag_name_ns, NULL)
	PHP_FALIAS(getElementById, dom_document_get_element_by_id, NULL)
	PHP_FALIAS(adoptNode, dom_document_adopt_node, NULL)
	PHP_FALIAS(normalizeDocument, dom_document_normalize_document, NULL)
	PHP_FALIAS(renameNode, dom_document_rename_node, NULL)
	PHP_FALIAS(load, dom_document_load, NULL)
	PHP_FALIAS(save, dom_document_save, NULL)
	PHP_FALIAS(loadXML, dom_document_loadxml, NULL)
	PHP_FALIAS(saveXML, dom_document_savexml, NULL)
	PHP_FALIAS(domdocument, dom_document_document, NULL)
	PHP_FALIAS(validate, dom_document_validate, NULL)
	PHP_FALIAS(xinclude, dom_document_xinclude, NULL)
#if defined(LIBXML_HTML_ENABLED)
	PHP_FALIAS(loadHTML, dom_document_load_html, NULL)
	PHP_FALIAS(loadHTMLFile, dom_document_load_html_file, NULL)
	PHP_FALIAS(saveHTML, dom_document_save_html, NULL)
	PHP_FALIAS(saveHTMLFile, dom_document_save_html_file, NULL)
#endif  /* defined(LIBXML_HTML_ENABLED) */
#if defined(LIBXML_SCHEMAS_ENABLED)
	PHP_FALIAS(schemaValidate, dom_document_schema_validate_file, NULL)
	PHP_FALIAS(schemaValidateSource, dom_document_schema_validate_xml, NULL)
	PHP_FALIAS(relaxNGValidate, dom_document_relaxNG_validate_file, NULL)
	PHP_FALIAS(relaxNGValidateSource, dom_document_relaxNG_validate_xml, NULL)
#endif
	{NULL, NULL, NULL}
};

/* {{{ proto doctype	documenttype	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-B63ED1A31
Since: 
*/
int dom_document_doctype_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	xmlDtdPtr dtdptr;
	int ret;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	dtdptr = xmlGetIntSubset(docp);
	if (!dtdptr) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (NULL == (*retval = php_dom_create_object((xmlNodePtr) dtdptr, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
	
}

/* }}} */



/* {{{ proto implementation	domimplementation	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1B793EBA
Since: 
*/
int dom_document_implementation_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	php_dom_create_implementation(retval TSRMLS_CC);
	return SUCCESS;
}

/* }}} */



/* {{{ proto domelement document_element	documentElement	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-87CD092
Since: 
*/
int dom_document_document_element_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	xmlNode *root;
	int ret;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	root = xmlDocGetRootElement(docp);
	if (!root) {
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (NULL == (*retval = php_dom_create_object(root, &ret, NULL, *retval, obj TSRMLS_CC))) {
		php_error(E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */


/* {{{ proto actual_encoding	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-actualEncoding
Since: DOM Level 3
*/
/* READ ONLY FOR NOW USING ENCODING PROPERTY
int dom_document_actual_encoding_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_NULL(*retval);
	return SUCCESS;
}

int dom_document_actual_encoding_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	return SUCCESS;
}
*/

/* }}} */

/* {{{ proto encoding	string
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-encoding
Since: DOM Level 3
*/
int dom_document_encoding_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *encoding;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	encoding = (char *) docp->encoding;
	ALLOC_ZVAL(*retval);

	if (encoding != NULL) {
		ZVAL_STRING(*retval, encoding, 1);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

int dom_document_encoding_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlDoc *docp;
	xmlCharEncodingHandlerPtr handler;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	handler = xmlFindCharEncodingHandler(Z_STRVAL_P(newval));

    if (handler != NULL) {
		xmlCharEncCloseFunc(handler);
		if (docp->encoding != NULL) {
			xmlFree((xmlChar *)docp->encoding);
		}
		docp->encoding = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));
		return SUCCESS;
    } else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Document Encoding");
    }

	return SUCCESS;
}

/* }}} */



/* {{{ proto standalone	boolean	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-standalone
Since: DOM Level 3
*/
int dom_document_standalone_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	int standalone;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	ALLOC_ZVAL(*retval);
	standalone = docp->standalone;
	ZVAL_BOOL(*retval, standalone);

	return SUCCESS;
}

int dom_document_standalone_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlDoc *docp;
	int standalone;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	standalone = Z_LVAL_P(newval);
    if (standalone > 0) {
        docp->standalone = 1;
    }
    else if (standalone < 0) {
        docp->standalone = -1;
    }
    else {
        docp->standalone = 0;
    }

	return SUCCESS;
}

/* }}} */



/* {{{ proto version	string	
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-version
Since: DOM Level 3
*/
int dom_document_version_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *version;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	version = (char *) docp->version;
	ALLOC_ZVAL(*retval);

	if (version != NULL) {
		ZVAL_STRING(*retval, version, 1);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

int dom_document_version_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlDoc *docp;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	if (docp->version != NULL) {
		xmlFree((xmlChar *) docp->version );
	}

	docp->version = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));

	return SUCCESS;
}

/* }}} */

/* {{{ proto strict_error_checking	boolean	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-strictErrorChecking
Since: DOM Level 3
*/
int dom_document_strict_error_checking_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->stricterror);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_strict_error_checking_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->stricterror = Z_LVAL_P(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ proto formatOutput	boolean	
readonly=no
*/
int dom_document_format_output_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->formatoutput);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_format_output_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->formatoutput = Z_LVAL_P(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ proto validateonParse	boolean	
readonly=no
*/
int	dom_document_validate_on_parse_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->validateonparse);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_validate_on_parse_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->validateonparse = Z_LVAL_P(newval);
	}

	return SUCCESS;
}
/* }}} */


/* {{{ proto resolveExternals	boolean	
readonly=no
*/
int dom_document_resolve_externals_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->resolveexternals);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_resolve_externals_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->resolveexternals = Z_LVAL_P(newval);
	}

	return SUCCESS;
}
/* }}} */


/* {{{ proto preserveWhiteSpace	boolean	
readonly=no
*/
int dom_document_preserve_whitespace_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->preservewhitespace);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_preserve_whitespace_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->preservewhitespace = Z_LVAL_P(newval);
	}

	return SUCCESS;
}
/* }}} */


/* {{{ proto substituteEntities	boolean	
readonly=no
*/
int dom_document_substitue_entities_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->substituteentities);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_substitue_entities_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	dom_doc_props *doc_prop;

	if (obj->document && newval->type == IS_BOOL) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->substituteentities = Z_LVAL_P(newval);
	}

	return SUCCESS;
}
/* }}} */


/* {{{ proto document_uri	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-documentURI
Since: DOM Level 3
*/
int dom_document_document_uri_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *url;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	url = (char *) docp->URL;
	if (url != NULL) {
		ZVAL_STRING(*retval, url, 1);
	} else {
		ZVAL_NULL(*retval);
	}

	return SUCCESS;
}

int dom_document_document_uri_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	xmlDoc *docp;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	if (docp->URL != NULL) {
		xmlFree((xmlChar *) docp->URL);
	}

	docp->URL = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));

	return SUCCESS;
}

/* }}} */



/* {{{ proto config	domconfiguration	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-config
Since: DOM Level 3
*/
int dom_document_config_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	ALLOC_ZVAL(*retval);
	ZVAL_NULL(*retval);
	return SUCCESS;
}

/* }}} */



/* {{{ proto domelement dom_document_create_element(string tagName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-2141741547
Since: 
*/
PHP_FUNCTION(dom_document_create_element)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	int ret, name_len, value_len;
	char *name, *value = NULL;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewDocNode(docp, NULL, name, value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_element */


/* {{{ proto domdocumentfragment dom_document_create_document_fragment();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-35CB04B5
Since: 
*/
PHP_FUNCTION(dom_document_create_document_fragment)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	int ret;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	node =  xmlNewDocFragment(docp);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_document_fragment */


/* {{{ proto domtext dom_document_create_text_node(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1975348127
Since: 
*/
PHP_FUNCTION(dom_document_create_text_node)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewDocText(docp, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_text_node */


/* {{{ proto domcomment dom_document_create_comment(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1334481328
Since: 
*/
PHP_FUNCTION(dom_document_create_comment)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewDocComment(docp, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_comment */


/* {{{ proto domcdatasection dom_document_create_cdatasection(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D26C0AF8
Since: 
*/
PHP_FUNCTION(dom_document_create_cdatasection)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewCDataBlock(docp, (xmlChar *) value, value_len);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_cdatasection */


/* {{{ proto domprocessinginstruction dom_document_create_processing_instruction(string target, string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-135944439
Since: 
*/
PHP_FUNCTION(dom_document_create_processing_instruction)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len, name_len;
	dom_object *intern;
	char *name, *value = NULL;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	node = xmlNewPI((xmlChar *) name, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	node->doc = docp;

	DOM_RET_OBJ(rv, node, &ret, intern);
}
/* }}} end dom_document_create_processing_instruction */


/* {{{ proto domattr dom_document_create_attribute(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1084891198
Since: 
*/
PHP_FUNCTION(dom_document_create_attribute)
{
	zval *id, *rv = NULL;
	xmlAttrPtr node;
	xmlDocPtr docp;
	int ret, name_len;
	dom_object *intern;
	char *name;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	if (name_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Attribute name is required");
		RETURN_FALSE;
	}

	node = xmlNewDocProp(docp, (xmlChar *) name, NULL);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) node, &ret, intern);

}
/* }}} end dom_document_create_attribute */


/* {{{ proto domentityreference dom_document_create_entity_reference(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-392B75AE
Since: 
*/
PHP_FUNCTION(dom_document_create_entity_reference)
{
	zval *id, *rv = NULL;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	dom_object *intern;
	int ret, name_len;
	char *name;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	node = xmlNewReference(docp, name);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) node, &ret, intern);
}
/* }}} end dom_document_create_entity_reference */


/* {{{ proto domnodelist dom_document_get_elements_by_tag_name(string tagname);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-A6C9094
Since: 
*/
PHP_FUNCTION(dom_document_get_elements_by_tag_name)
{
	zval *id;
	xmlDocPtr docp;
	int name_len;
	dom_object *intern, *namednode;
	char *name;
	xmlChar *local;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	php_dom_create_interator(return_value, DOM_NODELIST TSRMLS_CC);
	namednode = (dom_object *)zend_objects_get_address(return_value TSRMLS_CC);
	local = xmlCharStrndup(name, name_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, NULL);
}
/* }}} end dom_document_get_elements_by_tag_name */


/* {{{ proto domnode dom_document_import_node(node importedNode, boolean deep);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Core-Document-importNode
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_import_node)
{
	zval *rv = NULL;
	zval *id, *node;
	xmlDocPtr docp;
	xmlNodePtr nodep, retnodep;
	dom_object *intern, *nodeobj;
	int ret; 
	long recursive = 0;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o|l", &node, &recursive) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(nodep, node, xmlNodePtr, nodeobj);

	if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE 
		|| nodep->type == XML_DOCUMENT_TYPE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot import: Node Type Not Supported");
		RETURN_FALSE;
	}

	if (nodep->doc == docp) {
		retnodep = nodep;
	} else {
		retnodep = xmlDocCopyNode(nodep, docp, recursive);
		if (!retnodep) {
			RETURN_FALSE;
		}
	}

	DOM_RET_OBJ(rv, (xmlNodePtr) retnodep, &ret, intern);
}
/* }}} end dom_document_import_node */


/* {{{ proto domelement dom_document_create_element_ns(string namespaceURI, string qualifiedName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrElNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_create_element_ns)
{
	zval *id, *rv = NULL;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL;
	xmlNsPtr nsptr = NULL;
	int ret, uri_len = 0, name_len = 0;
	char *uri, *name;
	char *localname = NULL, *prefix = NULL;
	int errorcode;
	dom_object *intern;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		nodep = xmlNewDocNode (docp, NULL, localname, NULL);
		if (nodep != NULL && uri != NULL) {
			nsptr = xmlSearchNsByHref (nodep->doc, nodep, uri);
			if (nsptr == NULL) {
				nsptr = dom_get_ns(nodep, uri, &errorcode, prefix);
			}
			xmlSetNs(nodep, nsptr);
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
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}

	
	nodep->ns = nsptr;

	DOM_RET_OBJ(rv, nodep, &ret, intern);
}
/* }}} end dom_document_create_element_ns */


/* {{{ proto domattr dom_document_create_attribute_ns(string namespaceURI, string qualifiedName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrAttrNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_create_attribute_ns)
{
	zval *id, *rv = NULL;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL, root;
	xmlNsPtr nsptr;
	int ret, uri_len = 0, name_len = 0;
	char *uri, *name;
	char *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	root = xmlDocGetRootElement(docp);
	if (root != NULL) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			nodep = (xmlNodePtr) xmlNewDocProp(docp, localname, NULL);
			if (nodep != NULL && uri_len > 0) {
				nsptr = xmlSearchNsByHref (nodep->doc, root, uri);
				if (nsptr == NULL) {
					nsptr = dom_get_ns(root, uri, &errorcode, prefix);
				}
				xmlSetNs(nodep, nsptr);
			}
		}
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Document Missing Root Element");
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
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(rv, nodep, &ret, intern);
}
/* }}} end dom_document_create_attribute_ns */


/* {{{ proto domnodelist dom_document_get_elements_by_tag_name_ns(string namespaceURI, string localName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBTNNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_get_elements_by_tag_name_ns)
{
	zval *id;
	xmlDocPtr docp;
	int uri_len, name_len;
	dom_object *intern, *namednode;
	char *uri, *name;
	xmlChar *local, *nsuri;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	php_dom_create_interator(return_value, DOM_NODELIST TSRMLS_CC);
	namednode = (dom_object *)zend_objects_get_address(return_value TSRMLS_CC);
	local = xmlCharStrndup(name, name_len);
	nsuri = xmlCharStrndup(uri, uri_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, nsuri);
}
/* }}} end dom_document_get_elements_by_tag_name_ns */


/* {{{ proto domelement dom_document_get_element_by_id(string elementId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBId
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_get_element_by_id)
{
	zval *id, *rv = NULL;
	xmlDocPtr docp;
	xmlAttrPtr  attrp;
	int ret, idname_len;
	dom_object *intern;
	char *idname;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &idname, &idname_len) == FAILURE) {
		return;
	}
	attrp = xmlGetID(docp, (xmlChar *) idname);

	if (attrp && attrp->parent) {
		DOM_RET_OBJ(rv, (xmlNodePtr) attrp->parent, &ret, intern);
	} else {
		RETVAL_NULL();
	}

}
/* }}} end dom_document_get_element_by_id */


/* {{{ proto domnode dom_document_adopt_node(node source);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-adoptNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_adopt_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_document_adopt_node */


/* {{{ proto dom_void dom_document_normalize_document();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-normalizeDocument
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_normalize_document)
{
	zval *id;
	xmlDocPtr docp;
	dom_object *intern;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	DOM_NO_ARGS();

	dom_normalize((xmlNodePtr) docp TSRMLS_CC);
}
/* }}} end dom_document_normalize_document */


/* {{{ proto domnode dom_document_rename_node(node n, string namespaceURI, string qualifiedName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-renameNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_rename_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_document_rename_node */

/* {{{ proto domnode dom_document_document([string version], [string encoding]); */
PHP_FUNCTION(dom_document_document)
{

	zval *id;
	xmlDoc *docp = NULL, *olddoc;
	dom_object *intern;
	char *encoding, *version = NULL;
	int encoding_len = 0, version_len = 0, refcount;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|ss", &version, &version_len, &encoding, &encoding_len) == FAILURE) {
		return;
	}

	docp = xmlNewDoc(version);
	if (!docp)
		RETURN_FALSE;

	if (encoding_len > 0) {
		docp->encoding = (const xmlChar*)xmlStrdup(encoding);
	}

	intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
	if (intern != NULL) {
		olddoc = (xmlDocPtr) dom_object_get_node(intern);
		if (olddoc != NULL) {
			php_libxml_decrement_node_ptr((php_libxml_node_object *) intern TSRMLS_CC);
			refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
			if (refcount != 0) {
				olddoc->_private = NULL;
			}
		}
		intern->document = NULL;
		php_libxml_increment_doc_ref((php_libxml_node_object *)intern, docp TSRMLS_CC);
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)docp, (void *)intern TSRMLS_CC);
	}
}
/* }}} end dom_document_document */

char *_dom_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) {
	xmlURI *uri;
	xmlChar *escsource;
	char *file_dest;
	int isFileUri = 0;

	uri = xmlCreateURI();
	escsource = xmlURIEscapeStr(source, ":");
	xmlParseURIReference(uri, escsource);
	xmlFree(escsource);

	if (uri->scheme != NULL) {
		/* absolute file uris - libxml only supports localhost or empty host */
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
		if (! VCWD_REALPATH(source, resolved_path)) {
			expand_filepath(source, resolved_path TSRMLS_CC);
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}


/* {{{ */
static xmlDocPtr dom_document_parser(zval *id, int mode, char *source TSRMLS_DC) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt = NULL;
	dom_doc_props *doc_props;
	dom_object *intern;
	php_libxml_ref_obj *document = NULL;
	int validate, resolve_externals, keep_blanks, substitute_ent;
	int resolved_path_len;
	char *directory=NULL, resolved_path[MAXPATHLEN];

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		document = intern->document;
	}

	doc_props = dom_get_doc_props(document);
	validate = doc_props->validateonparse;
	resolve_externals = doc_props->resolveexternals;
	keep_blanks = doc_props->preservewhitespace;
	substitute_ent = doc_props->substituteentities;

	if (document == NULL) {
		efree(doc_props);
	}

	xmlInitParser();

	keep_blanks = xmlKeepBlanksDefault(keep_blanks);

	if (mode == DOM_LOAD_FILE) {
		char *file_dest = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (file_dest) {
			ctxt = xmlCreateFileParserCtxt(file_dest);
		}
		
	} else {
		ctxt = xmlCreateDocParserCtxt(source);
	}

	xmlKeepBlanksDefault(keep_blanks);
	/* xmlIndentTreeOutput default is changed in xmlKeepBlanksDefault
	reset back to 1 which is default value */

	xmlIndentTreeOutput = 1;

	if (ctxt == NULL) {
		return(NULL);
	}

	/* If loading from memory, we need to set the base directory for the document */
	if (mode != DOM_LOAD_FILE) {
#if HAVE_GETCWD
		directory = VCWD_GETCWD(resolved_path, MAXPATHLEN);
#elif HAVE_GETWD
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

	ctxt->recovery = 0;
	ctxt->validate = validate;
    ctxt->loadsubset = (resolve_externals * XML_COMPLETE_ATTRS);
	ctxt->replaceEntities = substitute_ent;

	ctxt->vctxt.error = php_libxml_ctx_error;
	ctxt->vctxt.warning = php_libxml_ctx_warning;

	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_libxml_ctx_error;
		ctxt->sax->warning = php_libxml_ctx_warning;
	}

	xmlParseDocument(ctxt);

	if (ctxt->wellFormed) {
		ret = ctxt->myDoc;
		/* If loading from memory, set the base reference uri for the document */
		if (ret->URL == NULL && ctxt->directory != NULL) {
			ret->URL = xmlStrdup(ctxt->directory);
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
	zval *id, *rv = NULL;
	xmlDoc *docp = NULL, *newdoc;
	dom_doc_props *doc_prop;
	dom_object *intern;
	char *source;
	int source_len, refcount, ret;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	newdoc = dom_document_parser(id, mode, source TSRMLS_CC);

	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern TSRMLS_CC);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC);
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ(rv, (xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} end dom_parser_document */

/* {{{ proto boolean domnode dom_document_load(string source);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-load
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_load)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load */

/* {{{ proto boolean domnode dom_document_loadxml(string source);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-loadXML
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_loadxml)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_loadxml */

/* {{{ proto long domnode dom_document_save(string file);
Convenience method to save to file
*/
PHP_FUNCTION(dom_document_save)
{
	zval *id;
	xmlDoc *docp;
	int file_len, bytes, format;
	dom_object *intern;
	dom_doc_props *doc_props;
	char *file;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		return;
	}

	/* encoding handled by property on doc */

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;
	bytes = xmlSaveFormatFileEnc(file, docp, NULL, format);

	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} end dom_document_save */

/* {{{ proto string domnode dom_document_savexml([node n]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-saveXML
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_savexml)
{
	zval *id, *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	xmlBufferPtr buf;
	xmlChar *mem;
	dom_object *intern, *nodeobj;
	dom_doc_props *doc_props;
	int size, format;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &nodep) == FAILURE) {
		return;
	}

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;

	if (nodep != NULL) {
		/* Dump contents of Node */
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
			RETURN_FALSE;
		}
		buf = xmlBufferCreate();
		if (!buf) {
			php_error(E_WARNING, "Could not fetch buffer");
			RETURN_FALSE;
		}

		xmlNodeDump(buf, docp, node, 0, format);
		mem = (xmlChar*) xmlBufferContent(buf);
		if (!mem) {
			xmlBufferFree(buf);
			RETURN_FALSE;
		}
		RETVAL_STRING(mem,  1);
		xmlBufferFree(buf);
	} else {
		/* Encoding is handled from the encoding property set on the document */
		xmlDocDumpFormatMemory(docp, &mem, &size, format);
		if (!size) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL(mem, size, 1);
		xmlFree(mem);
	}
}
/* }}} end dom_document_savexml */

static xmlNodePtr php_dom_free_xinclude_node(xmlNodePtr cur TSRMLS_DC) {
	xmlNodePtr xincnode;

	xincnode = cur;
	cur = cur->next;
	xmlUnlinkNode(xincnode);
	php_libxml_node_free_resource(xincnode TSRMLS_CC);

	return cur;
}

static void php_dom_remove_xinclude_nodes(xmlNodePtr cur TSRMLS_DC) {
	while(cur) {
		if (cur->type == XML_XINCLUDE_START) {
			cur = php_dom_free_xinclude_node(cur TSRMLS_CC);

			/* XML_XINCLUDE_END node will be a sibling of XML_XINCLUDE_START */
			while(cur && cur->type != XML_XINCLUDE_END) {
				cur = cur->next;
			}

			if (cur && cur->type == XML_XINCLUDE_END) {
				cur = php_dom_free_xinclude_node(cur TSRMLS_CC);
			}
		} else {
			if (cur->type == XML_ELEMENT_NODE) {
				php_dom_remove_xinclude_nodes(cur->children TSRMLS_CC);
			}
			cur = cur->next;
		}
	}
}

/* {{{ proto int dom_document_xinclude()
   Substitutues xincludes in a DomDocument */
PHP_FUNCTION(dom_document_xinclude)
{
	zval *id;
	xmlDoc *docp;
	xmlNodePtr root;
	int err; 
	dom_object *intern;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	err = xmlXIncludeProcess (docp);

	/* XML_XINCLUDE_START and XML_XINCLUDE_END nodes need to be removed as these
	are added via xmlXIncludeProcess to mark beginning and ending of xincluded document 
	but are not wanted in resulting document - must be done even if err as it could fail after
	having processed some xincludes */
	root = (xmlNodePtr) docp->children;
	while(root && root->type != XML_ELEMENT_NODE && root->type != XML_XINCLUDE_START) {
		root = root->next;
	}
	if (root) {
		php_dom_remove_xinclude_nodes(root TSRMLS_CC);
	}

	if (err) {
		RETVAL_LONG(err);
	} else {
		RETVAL_FALSE;
	}
    
}


/* {{{ proto string domnode dom_document_validate();
Since: DOM extended
*/
PHP_FUNCTION(dom_document_validate)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	xmlValidCtxt *cvp;
	
	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);
	
	if (docp->intSubset == NULL) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "No DTD given in XML-Document");
	}
	
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
 

#if defined(LIBXML_SCHEMAS_ENABLED)
static void
_dom_document_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
	int source_len = 0;
	xmlSchemaParserCtxtPtr  parser;
	xmlSchemaPtr            sptr;
	xmlSchemaValidCtxtPtr   vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error(E_WARNING, "Invalid Schema file source");
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
		php_error(E_WARNING, "Invalid Schema");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlSchemaNewValidCtxt(sptr);
	if (!vptr) {
		xmlSchemaFree(sptr);
		php_error(E_ERROR, "Invalid Schema Validation Context");
		RETURN_FALSE;
	}

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

/* {{{ proto boolean domnode _dom_document_schema_validate(string filename); */
PHP_FUNCTION(dom_document_schema_validate_file)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end _dom_document_schema_validate */

/* {{{ proto boolean domnode _dom_document_schema_validate(string source); */
PHP_FUNCTION(dom_document_schema_validate_xml)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end _dom_document_schema_validate */


static void
_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAMETERS, int type)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	char *source = NULL, *valid_file = NULL;
	int source_len = 0;
	xmlRelaxNGParserCtxtPtr parser;
	xmlRelaxNGPtr           sptr;
	xmlRelaxNGValidCtxtPtr  vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error(E_WARNING, "Invalid RelaxNG file source");
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
		php_error(E_WARNING, "Invalid RelaxNG");
		RETURN_FALSE;
	}

	docp = (xmlDocPtr) dom_object_get_node(intern);

	vptr = xmlRelaxNGNewValidCtxt(sptr);
	if (!vptr) {
		xmlRelaxNGFree(sptr);
		php_error(E_ERROR, "Invalid RelaxNG Validation Context");
		RETURN_FALSE;
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

/* {{{ proto boolean domnode dom_document_relaxNG_validate_file(string filename); */
PHP_FUNCTION(dom_document_relaxNG_validate_file)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_relaxNG_validate_file */

/* {{{ proto boolean domnode dom_document_relaxNG_validate_xml(string source); */
PHP_FUNCTION(dom_document_relaxNG_validate_xml)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_relaxNG_validate_xml */

#endif

#if defined(LIBXML_HTML_ENABLED)

static void dom_load_html(INTERNAL_FUNCTION_PARAMETERS, int mode)
{
	zval *id, *rv = NULL;
	xmlDoc *docp = NULL, *newdoc;
	dom_object *intern;
	dom_doc_props *doc_prop;
	char *source;
	int source_len, refcount, ret;
	htmlParserCtxtPtr ctxt;
	
	id = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	if (!source_len) {
		RETURN_FALSE;
	}

	if (mode == DOM_LOAD_FILE) {
		ctxt = htmlCreateFileParserCtxt(source, NULL);
	} else {
		source_len = xmlStrlen(source);
		ctxt = htmlCreateMemoryParserCtxt(source, source_len);
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
	htmlParseDocument(ctxt);
	newdoc = ctxt->myDoc;
	htmlFreeParserCtxt(ctxt);
	
	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			doc_prop = NULL;
			if (docp != NULL) {
				php_libxml_decrement_node_ptr((php_libxml_node_object *) intern TSRMLS_CC);
				doc_prop = intern->document->doc_props;
				intern->document->doc_props = NULL;
				refcount = php_libxml_decrement_doc_ref((php_libxml_node_object *)intern TSRMLS_CC);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC);
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ(rv, (xmlNodePtr) newdoc, &ret, NULL);
	}
}

/* {{{ proto boolean domnode dom_document_load_html_file(string source);
Since: DOM extended
*/
PHP_FUNCTION(dom_document_load_html_file)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load_html_file */

/* {{{ proto boolean domnode dom_document_load_html(string source);
Since: DOM extended
*/
PHP_FUNCTION(dom_document_load_html)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_load_html */

/* {{{ proto long dom_document_save_html_file(string file);
Convenience method to save to file as html
*/
PHP_FUNCTION(dom_document_save_html_file)
{
	zval *id;
	xmlDoc *docp;
	int file_len, bytes, format;
	dom_object *intern;
	dom_doc_props *doc_props;
	char *file;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		return;
	}

	/* encoding handled by property on doc */

	doc_props = dom_get_doc_props(intern->document);
	format = doc_props->formatoutput;
	bytes = htmlSaveFileFormat(file, docp, NULL, format);

	if (bytes == -1) {
		RETURN_FALSE;
	}
	RETURN_LONG(bytes);
}
/* }}} end dom_document_save_html_file */

/* {{{ proto string dom_document_save_html();
Convenience method to output as html
*/
PHP_FUNCTION(dom_document_save_html)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	xmlChar *mem;
	int size;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	htmlDocDumpMemory(docp, &mem, &size);
	if (!size) {
		if (mem)
			xmlFree(mem);
		RETURN_FALSE;
	}
	RETVAL_STRINGL(mem, size, 1);
	xmlFree(mem);
}
/* }}} end dom_document_save_html */

#endif  /* defined(LIBXML_HTML_ENABLED) */

#endif  /* HAVE_LIBXML && HAVE_DOM */
