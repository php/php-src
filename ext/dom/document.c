/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2012 The PHP Group                                |
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

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_element, 0, 0, 1)
	ZEND_ARG_INFO(0, tagName)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_document_fragment, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_text_node, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_comment, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_cdatasection, 0, 0, 1)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_processing_instruction, 0, 0, 2)
	ZEND_ARG_INFO(0, target)
	ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_attribute, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_entity_reference, 0, 0, 1)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_get_elements_by_tag_name, 0, 0, 1)
	ZEND_ARG_INFO(0, tagName)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_import_node, 0, 0, 2)
	ZEND_ARG_OBJ_INFO(0, importedNode, DOMNode, 0)
	ZEND_ARG_INFO(0, deep)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_element_ns, 0, 0, 2)
	ZEND_ARG_INFO(0, namespaceURI)
	ZEND_ARG_INFO(0, qualifiedName)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_create_attribute_ns, 0, 0, 2)
	ZEND_ARG_INFO(0, namespaceURI)
	ZEND_ARG_INFO(0, qualifiedName)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_get_elements_by_tag_name_ns, 0, 0, 2)
	ZEND_ARG_INFO(0, namespaceURI)
	ZEND_ARG_INFO(0, localName)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_get_element_by_id, 0, 0, 1)
	ZEND_ARG_INFO(0, elementId)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_adopt_node, 0, 0, 1)
	ZEND_ARG_OBJ_INFO(0, source, DOMNode, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_normalize_document, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_rename_node, 0, 0, 3)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 0)
	ZEND_ARG_INFO(0, namespaceURI)
	ZEND_ARG_INFO(0, qualifiedName)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_load, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_save, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_loadxml, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_savexml, 0, 0, 0)
	ZEND_ARG_OBJ_INFO(0, node, DOMNode, 1)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_construct, 0, 0, 0)
	ZEND_ARG_INFO(0, version)
	ZEND_ARG_INFO(0, encoding)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_validate, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_xinclude, 0, 0, 0)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_loadhtml, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_loadhtmlfile, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
	ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_savehtml, 0, 0, 0)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_savehtmlfile, 0, 0, 1)
	ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_schema_validate_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_schema_validate_xml, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_relaxNG_validate_file, 0, 0, 1)
	ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_relaxNG_validate_xml, 0, 0, 1)
	ZEND_ARG_INFO(0, source)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_dom_document_registernodeclass, 0, 0, 2)
	ZEND_ARG_INFO(0, baseClass)
	ZEND_ARG_INFO(0, extendedClass)
ZEND_END_ARG_INFO();
/* }}} */

/*
* class DOMDocument extends DOMNode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-i-Document
* Since: 
*/

const zend_function_entry php_dom_document_class_functions[] = { /* {{{ */
	PHP_FALIAS(createElement, dom_document_create_element, arginfo_dom_document_create_element)
	PHP_FALIAS(createDocumentFragment, dom_document_create_document_fragment, arginfo_dom_document_create_document_fragment)
	PHP_FALIAS(createTextNode, dom_document_create_text_node, arginfo_dom_document_create_text_node)
	PHP_FALIAS(createComment, dom_document_create_comment, arginfo_dom_document_create_comment)
	PHP_FALIAS(createCDATASection, dom_document_create_cdatasection, arginfo_dom_document_create_cdatasection)
	PHP_FALIAS(createProcessingInstruction, dom_document_create_processing_instruction, arginfo_dom_document_create_processing_instruction)
	PHP_FALIAS(createAttribute, dom_document_create_attribute, arginfo_dom_document_create_attribute)
	PHP_FALIAS(createEntityReference, dom_document_create_entity_reference, arginfo_dom_document_create_entity_reference)
	PHP_FALIAS(getElementsByTagName, dom_document_get_elements_by_tag_name, arginfo_dom_document_get_elements_by_tag_name)
	PHP_FALIAS(importNode, dom_document_import_node, arginfo_dom_document_import_node)
	PHP_FALIAS(createElementNS, dom_document_create_element_ns, arginfo_dom_document_create_element_ns)
	PHP_FALIAS(createAttributeNS, dom_document_create_attribute_ns, arginfo_dom_document_create_attribute_ns)
	PHP_FALIAS(getElementsByTagNameNS, dom_document_get_elements_by_tag_name_ns, arginfo_dom_document_get_elements_by_tag_name_ns)
	PHP_FALIAS(getElementById, dom_document_get_element_by_id, arginfo_dom_document_get_element_by_id)
	PHP_FALIAS(adoptNode, dom_document_adopt_node, arginfo_dom_document_adopt_node)
	PHP_FALIAS(normalizeDocument, dom_document_normalize_document, arginfo_dom_document_normalize_document)
	PHP_FALIAS(renameNode, dom_document_rename_node, arginfo_dom_document_rename_node)
	PHP_ME(domdocument, load, arginfo_dom_document_load, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_FALIAS(save, dom_document_save, arginfo_dom_document_save)
	PHP_ME(domdocument, loadXML, arginfo_dom_document_loadxml, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_FALIAS(saveXML, dom_document_savexml, arginfo_dom_document_savexml)
	PHP_ME(domdocument, __construct, arginfo_dom_document_construct, ZEND_ACC_PUBLIC)
	PHP_FALIAS(validate, dom_document_validate, arginfo_dom_document_validate)
	PHP_FALIAS(xinclude, dom_document_xinclude, arginfo_dom_document_xinclude)
#if defined(LIBXML_HTML_ENABLED)
	PHP_ME(domdocument, loadHTML, arginfo_dom_document_loadhtml, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_ME(domdocument, loadHTMLFile, arginfo_dom_document_loadhtmlfile, ZEND_ACC_PUBLIC|ZEND_ACC_ALLOW_STATIC)
	PHP_FALIAS(saveHTML, dom_document_save_html, arginfo_dom_document_savehtml)
	PHP_FALIAS(saveHTMLFile, dom_document_save_html_file, arginfo_dom_document_savehtmlfile)
#endif  /* defined(LIBXML_HTML_ENABLED) */
#if defined(LIBXML_SCHEMAS_ENABLED)
	PHP_FALIAS(schemaValidate, dom_document_schema_validate_file, arginfo_dom_document_schema_validate_file)
	PHP_FALIAS(schemaValidateSource, dom_document_schema_validate_xml, arginfo_dom_document_schema_validate_xml)
	PHP_FALIAS(relaxNGValidate, dom_document_relaxNG_validate_file, arginfo_dom_document_relaxNG_validate_file)
	PHP_FALIAS(relaxNGValidateSource, dom_document_relaxNG_validate_xml, arginfo_dom_document_relaxNG_validate_xml)
#endif
	PHP_ME(domdocument, registerNodeClass, arginfo_dom_document_registernodeclass, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

/* {{{ docType	DOMDocumentType	
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

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	dtdptr = xmlGetIntSubset(docp);
	if (!dtdptr) {
		ZVAL_NULL(*retval);
		return SUCCESS;
	}

	if (NULL == (*retval = php_dom_create_object((xmlNodePtr) dtdptr, &ret, *retval, obj TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
	
}

/* }}} */

/* {{{ implementation	DOMImplementation	
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

/* {{{ documentElement	DOMElement	
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

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	root = xmlDocGetRootElement(docp);
	if (!root) {
		ZVAL_NULL(*retval);
		return SUCCESS;
	}

	if (NULL == (*retval = php_dom_create_object(root, &ret, *retval, obj TSRMLS_CC))) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
		return FAILURE;
	}
	return SUCCESS;
}

/* }}} */

/* {{{ encoding	string
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-encoding
Since: DOM Level 3
*/
int dom_document_encoding_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *encoding;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

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
	zval value_copy;
	xmlDoc *docp;
	xmlCharEncodingHandlerPtr handler;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	if (newval->type != IS_STRING) {
		if(Z_REFCOUNT_P(newval) > 1) {
			value_copy = *newval;
			zval_copy_ctor(&value_copy);
			newval = &value_copy;
		}
		convert_to_string(newval);
	}

	handler = xmlFindCharEncodingHandler(Z_STRVAL_P(newval));

    if (handler != NULL) {
		xmlCharEncCloseFunc(handler);
		if (docp->encoding != NULL) {
			xmlFree((xmlChar *)docp->encoding);
		}
		docp->encoding = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));
    } else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Document Encoding");
    }

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ standalone	boolean	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-standalone
Since: DOM Level 3
*/
int dom_document_standalone_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	int standalone;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	standalone = docp->standalone;
	ZVAL_BOOL(*retval, standalone);

	return SUCCESS;
}

int dom_document_standalone_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	zval value_copy;
	xmlDoc *docp;
	int standalone;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_long(newval);

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

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ version	string	
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-version
Since: DOM Level 3
*/
int dom_document_version_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *version;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

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
	zval value_copy;
	xmlDoc *docp;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	if (docp->version != NULL) {
		xmlFree((xmlChar *) docp->version );
	}

	if (newval->type != IS_STRING) {
		if(Z_REFCOUNT_P(newval) > 1) {
			value_copy = *newval;
			zval_copy_ctor(&value_copy);
			newval = &value_copy;
		}
		convert_to_string(newval);
	}

	docp->version = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ strictErrorChecking	boolean	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-strictErrorChecking
Since: DOM Level 3
*/
int dom_document_strict_error_checking_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->stricterror = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ formatOutput	boolean	
readonly=no
*/
int dom_document_format_output_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->formatoutput = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ validateOnParse	boolean	
readonly=no
*/
int	dom_document_validate_on_parse_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->validateonparse = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ resolveExternals	boolean	
readonly=no
*/
int dom_document_resolve_externals_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->resolveexternals = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ preserveWhiteSpace	boolean	
readonly=no
*/
int dom_document_preserve_whitespace_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->preservewhitespace = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ recover	boolean	
readonly=no
*/
int dom_document_recover_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

	ALLOC_ZVAL(*retval);
	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		ZVAL_BOOL(*retval, doc_prop->recover);
	} else {
		ZVAL_FALSE(*retval);
	}
	return SUCCESS;
}

int dom_document_recover_write(dom_object *obj, zval *newval TSRMLS_DC)
{
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->recover = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ substituteEntities	boolean	
readonly=no
*/
int dom_document_substitue_entities_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	dom_doc_propsptr doc_prop;

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
	zval value_copy;
	dom_doc_propsptr doc_prop;

	if(Z_REFCOUNT_P(newval) > 1) {
		value_copy = *newval;
		zval_copy_ctor(&value_copy);
		newval = &value_copy;
	}
	convert_to_boolean(newval);

	if (obj->document) {
		doc_prop = dom_get_doc_props(obj->document);
		doc_prop->substituteentities = Z_LVAL_P(newval);
	}

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ documentURI	string	
readonly=no 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-documentURI
Since: DOM Level 3
*/
int dom_document_document_uri_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDoc *docp;
	char *url;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

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
	zval value_copy;
	xmlDoc *docp;

	docp = (xmlDocPtr) dom_object_get_node(obj);

	if (docp == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	if (docp->URL != NULL) {
		xmlFree((xmlChar *) docp->URL);
	}

	if (newval->type != IS_STRING) {
		if(Z_REFCOUNT_P(newval) > 1) {
			value_copy = *newval;
			zval_copy_ctor(&value_copy);
			newval = &value_copy;
		}
		convert_to_string(newval);
	}

	docp->URL = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));

	if (newval == &value_copy) {
		zval_dtor(newval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ config	DOMConfiguration	
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

/* {{{ proto DOMElement dom_document_create_element(string tagName [, string value]);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-2141741547
Since: 
*/
PHP_FUNCTION(dom_document_create_element)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	int ret, name_len, value_len;
	char *name, *value = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|s", &id, dom_document_class_entry, &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	node = xmlNewDocNode(docp, NULL, name, value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_element */

/* {{{ proto DOMDocumentFragment dom_document_create_document_fragment();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-35CB04B5
Since: 
*/
PHP_FUNCTION(dom_document_create_document_fragment)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	dom_object *intern;
	int ret;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &id, dom_document_class_entry) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node =  xmlNewDocFragment(docp);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_document_fragment */

/* {{{ proto DOMText dom_document_create_text_node(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1975348127
Since: 
*/
PHP_FUNCTION(dom_document_create_text_node)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocText(docp, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_text_node */

/* {{{ proto DOMComment dom_document_create_comment(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1334481328
Since: 
*/
PHP_FUNCTION(dom_document_create_comment)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewDocComment(docp, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_comment */

/* {{{ proto DOMCdataSection dom_document_create_cdatasection(string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D26C0AF8
Since: 
*/
PHP_FUNCTION(dom_document_create_cdatasection)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len;
	dom_object *intern;
	char *value;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	node = xmlNewCDataBlock(docp, (xmlChar *) value, value_len);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_cdatasection */

/* {{{ proto DOMProcessingInstruction dom_document_create_processing_instruction(string target, string data);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-135944439
Since: 
*/
PHP_FUNCTION(dom_document_create_processing_instruction)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp;
	int ret, value_len, name_len = 0;
	dom_object *intern;
	char *name, *value = NULL;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|s", &id, dom_document_class_entry, &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	node = xmlNewPI((xmlChar *) name, (xmlChar *) value);
	if (!node) {
		RETURN_FALSE;
	}

	node->doc = docp;

	DOM_RET_OBJ(node, &ret, intern);
}
/* }}} end dom_document_create_processing_instruction */

/* {{{ proto DOMAttr dom_document_create_attribute(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1084891198
Since: 
*/
PHP_FUNCTION(dom_document_create_attribute)
{
	zval *id;
	xmlAttrPtr node;
	xmlDocPtr docp;
	int ret, name_len;
	dom_object *intern;
	char *name;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &name, &name_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	node = xmlNewDocProp(docp, (xmlChar *) name, NULL);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ((xmlNodePtr) node, &ret, intern);

}
/* }}} end dom_document_create_attribute */

/* {{{ proto DOMEntityReference dom_document_create_entity_reference(string name);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-392B75AE
Since: 
*/
PHP_FUNCTION(dom_document_create_entity_reference)
{
	zval *id;
	xmlNode *node;
	xmlDocPtr docp = NULL;
	dom_object *intern;
	int ret, name_len;
	char *name;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &name, &name_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	if (xmlValidateName((xmlChar *) name, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	node = xmlNewReference(docp, name);
	if (!node) {
		RETURN_FALSE;
	}

	DOM_RET_OBJ((xmlNodePtr) node, &ret, intern);
}
/* }}} end dom_document_create_entity_reference */

/* {{{ proto DOMNodeList dom_document_get_elements_by_tag_name(string tagname);
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &name, &name_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	php_dom_create_interator(return_value, DOM_NODELIST TSRMLS_CC);
	namednode = (dom_object *)zend_objects_get_address(return_value TSRMLS_CC);
	local = xmlCharStrndup(name, name_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, NULL TSRMLS_CC);
}
/* }}} end dom_document_get_elements_by_tag_name */

/* {{{ proto DOMNode dom_document_import_node(DOMNode importedNode, boolean deep);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Core-Document-importNode
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_import_node)
{
	zval *id, *node;
	xmlDocPtr docp;
	xmlNodePtr nodep, retnodep;
	dom_object *intern, *nodeobj;
	int ret; 
	long recursive = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO|l", &id, dom_document_class_entry, &node, dom_node_class_entry, &recursive) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	DOM_GET_OBJ(nodep, node, xmlNodePtr, nodeobj);

	if (nodep->type == XML_HTML_DOCUMENT_NODE || nodep->type == XML_DOCUMENT_NODE 
		|| nodep->type == XML_DOCUMENT_TYPE_NODE) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot import: Node Type Not Supported");
		RETURN_FALSE;
	}

	if (nodep->doc == docp) {
		retnodep = nodep;
	} else {
		if ((recursive == 0) && (nodep->type == XML_ELEMENT_NODE)) {
			recursive = 2;
		}
		retnodep = xmlDocCopyNode(nodep, docp, recursive);
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

/* {{{ proto DOMElement dom_document_create_element_ns(string namespaceURI, string qualifiedName [,string value]);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrElNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_create_element_ns)
{
	zval *id;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL;
	xmlNsPtr nsptr = NULL;
	int ret, uri_len = 0, name_len = 0, value_len = 0;
	char *uri, *name, *value = NULL;
	char *localname = NULL, *prefix = NULL;
	int errorcode;
	dom_object *intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os!s|s", &id, dom_document_class_entry, &uri, &uri_len, &name, &name_len, &value, &value_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		if (xmlValidateName((xmlChar *) localname, 0) == 0) {
			nodep = xmlNewDocNode (docp, NULL, localname, value);
			if (nodep != NULL && uri != NULL) {
				nsptr = xmlSearchNsByHref (nodep->doc, nodep, uri);
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
		php_dom_throw_error(errorcode, dom_get_strict_error(intern->document) TSRMLS_CC);
		RETURN_FALSE;
	}

	if (nodep == NULL) {
		RETURN_FALSE;
	}

	
	nodep->ns = nsptr;

	DOM_RET_OBJ(nodep, &ret, intern);
}
/* }}} end dom_document_create_element_ns */

/* {{{ proto DOMAttr dom_document_create_attribute_ns(string namespaceURI, string qualifiedName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-DocCrAttrNS
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_create_attribute_ns)
{
	zval *id;
	xmlDocPtr docp;
	xmlNodePtr nodep = NULL, root;
	xmlNsPtr nsptr;
	int ret, uri_len = 0, name_len = 0;
	char *uri, *name;
	char *localname = NULL, *prefix = NULL;
	dom_object *intern;
	int errorcode;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os!s", &id, dom_document_class_entry, &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	root = xmlDocGetRootElement(docp);
	if (root != NULL) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			if (xmlValidateName((xmlChar *) localname, 0) == 0) {
				nodep = (xmlNodePtr) xmlNewDocProp(docp, localname, NULL);
				if (nodep != NULL && uri_len > 0) {
					nsptr = xmlSearchNsByHref (nodep->doc, root, uri);
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

	DOM_RET_OBJ(nodep, &ret, intern);
}
/* }}} end dom_document_create_attribute_ns */

/* {{{ proto DOMNodeList dom_document_get_elements_by_tag_name_ns(string namespaceURI, string localName);
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oss", &id, dom_document_class_entry, &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	php_dom_create_interator(return_value, DOM_NODELIST TSRMLS_CC);
	namednode = (dom_object *)zend_objects_get_address(return_value TSRMLS_CC);
	local = xmlCharStrndup(name, name_len);
	nsuri = xmlCharStrndup(uri, uri_len);
	dom_namednode_iter(intern, 0, namednode, NULL, local, nsuri TSRMLS_CC);
}
/* }}} end dom_document_get_elements_by_tag_name_ns */

/* {{{ proto DOMElement dom_document_get_element_by_id(string elementId);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-getElBId
Since: DOM Level 2
*/
PHP_FUNCTION(dom_document_get_element_by_id)
{
	zval *id;
	xmlDocPtr docp;
	xmlAttrPtr  attrp;
	int ret, idname_len;
	dom_object *intern;
	char *idname;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &idname, &idname_len) == FAILURE) {
		return;
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

/* {{{ proto DOMNode dom_document_adopt_node(DOMNode source);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-adoptNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_adopt_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_document_adopt_node */

/* {{{ proto void dom_document_normalize_document();
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-normalizeDocument
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_normalize_document)
{
	zval *id;
	xmlDocPtr docp;
	dom_object *intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &id, dom_document_class_entry) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	dom_normalize((xmlNodePtr) docp TSRMLS_CC);
}
/* }}} end dom_document_normalize_document */

/* {{{ proto DOMNode dom_document_rename_node(node n, string namespaceURI, string qualifiedName);
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Document3-renameNode
Since: DOM Level 3
*/
PHP_FUNCTION(dom_document_rename_node)
{
 DOM_NOT_IMPLEMENTED();
}
/* }}} end dom_document_rename_node */

/* {{{ proto void DOMDocument::__construct([string version], [string encoding]); */
PHP_METHOD(domdocument, __construct)
{

	zval *id;
	xmlDoc *docp = NULL, *olddoc;
	dom_object *intern;
	char *encoding, *version = NULL;
	int encoding_len = 0, version_len = 0, refcount;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, dom_domexception_class_entry, &error_handling TSRMLS_CC);
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|ss", &id, dom_document_class_entry, &version, &version_len, &encoding, &encoding_len) == FAILURE) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}

	zend_restore_error_handling(&error_handling TSRMLS_CC);
	docp = xmlNewDoc(version);

	if (!docp) {
		php_dom_throw_error(INVALID_STATE_ERR, 1 TSRMLS_CC);
		RETURN_FALSE;
	}

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
		if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, docp TSRMLS_CC) == -1) {
			RETURN_FALSE;
		}
		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)docp, (void *)intern TSRMLS_CC);
	}
}
/* }}} end DOMDocument::__construct */

char *_dom_get_valid_file_path(char *source, char *resolved_path, int resolved_path_len  TSRMLS_DC) /* {{{ */
{
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
		if (!VCWD_REALPATH(source, resolved_path) && !expand_filepath(source, resolved_path TSRMLS_CC)) {
			xmlFreeURI(uri);
			return NULL;
		}
		file_dest = resolved_path;
	}

	xmlFreeURI(uri);

	return file_dest;
}
/* }}} */

static xmlDocPtr dom_document_parser(zval *id, int mode, char *source, int source_len, int options TSRMLS_DC) /* {{{ */
{
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt = NULL;
	dom_doc_propsptr doc_props;
	dom_object *intern;
	php_libxml_ref_obj *document = NULL;
	int validate, recover, resolve_externals, keep_blanks, substitute_ent;
	int resolved_path_len;
	int old_error_reporting = 0;
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
	recover = doc_props->recover;

	if (document == NULL) {
		efree(doc_props);
	}

	xmlInitParser();

	if (mode == DOM_LOAD_FILE) {
		char *file_dest = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
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
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_doc_propsptr doc_prop;
	dom_object *intern;
	char *source;
	int source_len, refcount, ret;
	long options = 0;

	id = getThis();
	if (id != NULL && ! instanceof_function(Z_OBJCE_P(id), dom_document_class_entry TSRMLS_CC)) {
		id = NULL;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &source, &source_len, &options) == FAILURE) {
		return;
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
		RETURN_FALSE;
	}

	newdoc = dom_document_parser(id, mode, source, source_len, options TSRMLS_CC);

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
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} end dom_parser_document */

/* {{{ proto DOMNode dom_document_load(string source [, int options]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-load
Since: DOM Level 3
*/
PHP_METHOD(domdocument, load)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load */

/* {{{ proto DOMNode dom_document_loadxml(string source [, int options]);
URL: http://www.w3.org/TR/DOM-Level-3-LS/load-save.html#LS-DocumentLS-loadXML
Since: DOM Level 3
*/
PHP_METHOD(domdocument, loadXML)
{
	dom_parse_document(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_loadxml */

/* {{{ proto int dom_document_save(string file);
Convenience method to save to file
*/
PHP_FUNCTION(dom_document_save)
{
	zval *id;
	xmlDoc *docp;
	int file_len = 0, bytes, format, saveempty = 0;
	dom_object *intern;
	dom_doc_propsptr doc_props;
	char *file;
	long options = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os|l", &id, dom_document_class_entry, &file, &file_len, &options) == FAILURE) {
		return;
	}

	if (file_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Filename");
		RETURN_FALSE;
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

/* {{{ proto string dom_document_savexml([node n]);
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
	dom_doc_propsptr doc_props;
	int size, format, saveempty = 0;
	long options = 0;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|O!l", &id, dom_document_class_entry, &nodep, dom_node_class_entry, &options) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not fetch buffer");
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
		RETVAL_STRING(mem, 1);
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
		if (!size) {
			RETURN_FALSE;
		}
		RETVAL_STRINGL(mem, size, 1);
		xmlFree(mem);
	}
}
/* }}} end dom_document_savexml */

static xmlNodePtr php_dom_free_xinclude_node(xmlNodePtr cur TSRMLS_DC) /* {{{ */
{
	xmlNodePtr xincnode;

	xincnode = cur;
	cur = cur->next;
	xmlUnlinkNode(xincnode);
	php_libxml_node_free_resource(xincnode TSRMLS_CC);

	return cur;
}
/* }}} */

static void php_dom_remove_xinclude_nodes(xmlNodePtr cur TSRMLS_DC) /* {{{ */
{
	while(cur) {
		if (cur->type == XML_XINCLUDE_START) {
			cur = php_dom_free_xinclude_node(cur TSRMLS_CC);

			/* XML_XINCLUDE_END node will be a sibling of XML_XINCLUDE_START */
			while(cur && cur->type != XML_XINCLUDE_END) {
				/* remove xinclude processing nodes from recursive xincludes */
				if (cur->type == XML_ELEMENT_NODE) {
					   php_dom_remove_xinclude_nodes(cur->children TSRMLS_CC);
				}
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
/* }}} */

/* {{{ proto int dom_document_xinclude([int options])
   Substitutues xincludes in a DomDocument */
PHP_FUNCTION(dom_document_xinclude)
{
	zval *id;
	xmlDoc *docp;
	xmlNodePtr root;
	long flags = 0; 
	int err;
	dom_object *intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O|l", &id, dom_document_class_entry, &flags) == FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	err = xmlXIncludeProcessFlags(docp, flags);

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
/* }}} */

/* {{{ proto boolean dom_document_validate();
Since: DOM extended
*/
PHP_FUNCTION(dom_document_validate)
{
	zval *id;
	xmlDoc *docp;
	dom_object *intern;
	xmlValidCtxt *cvp;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &id, dom_document_class_entry) == FAILURE) {
		return;
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

#if defined(LIBXML_SCHEMAS_ENABLED)
static void _dom_document_schema_validate(INTERNAL_FUNCTION_PARAMETERS, int type) /* {{{ */
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

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Op", &id, dom_document_class_entry, &source, &source_len) == FAILURE) {
		return;
	}

	if (source_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema source");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema file source");
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema");
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
/* }}} */

/* {{{ proto boolean dom_document_schema_validate_file(string filename); */
PHP_FUNCTION(dom_document_schema_validate_file)
{
	_dom_document_schema_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_schema_validate_file */

/* {{{ proto boolean dom_document_schema_validate(string source); */
PHP_FUNCTION(dom_document_schema_validate_xml)
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
	int source_len = 0;
	xmlRelaxNGParserCtxtPtr parser;
	xmlRelaxNGPtr           sptr;
	xmlRelaxNGValidCtxtPtr  vptr;
	int                     is_valid;
	char resolved_path[MAXPATHLEN + 1];

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Op", &id, dom_document_class_entry, &source, &source_len) == FAILURE) {
		return;
	}

	if (source_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Schema source");
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	switch (type) {
	case DOM_LOAD_FILE:
		valid_file = _dom_get_valid_file_path(source, resolved_path, MAXPATHLEN  TSRMLS_CC);
		if (!valid_file) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid RelaxNG file source");
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid RelaxNG");
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
/* }}} */

/* {{{ proto boolean dom_document_relaxNG_validate_file(string filename); */
PHP_FUNCTION(dom_document_relaxNG_validate_file)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_relaxNG_validate_file */

/* {{{ proto boolean dom_document_relaxNG_validate_xml(string source); */
PHP_FUNCTION(dom_document_relaxNG_validate_xml)
{
	_dom_document_relaxNG_validate(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_relaxNG_validate_xml */

#endif

#if defined(LIBXML_HTML_ENABLED)

static void dom_load_html(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zval *id;
	xmlDoc *docp = NULL, *newdoc;
	dom_object *intern;
	dom_doc_propsptr doc_prop;
	char *source;
	int source_len, refcount, ret;
	long options = 0;
	htmlParserCtxtPtr ctxt;
	
	id = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &source, &source_len, &options) == FAILURE) {
		return;
	}

	if (!source_len) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Empty string supplied as input");
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

	if (options) {
		htmlCtxtUseOptions(ctxt, options);
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

	if (id != NULL && instanceof_function(Z_OBJCE_P(id), dom_document_class_entry TSRMLS_CC)) {
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
			if (php_libxml_increment_doc_ref((php_libxml_node_object *)intern, newdoc TSRMLS_CC) == -1) {
				RETURN_FALSE;
			}
			intern->document->doc_props = doc_prop;
		}

		php_libxml_increment_node_ptr((php_libxml_node_object *)intern, (xmlNodePtr)newdoc, (void *)intern TSRMLS_CC);

		RETURN_TRUE;
	} else {
		DOM_RET_OBJ((xmlNodePtr) newdoc, &ret, NULL);
	}
}
/* }}} */

/* {{{ proto DOMNode dom_document_load_html_file(string source);
Since: DOM extended
*/
PHP_METHOD(domdocument, loadHTMLFile)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_FILE);
}
/* }}} end dom_document_load_html_file */

/* {{{ proto DOMNode dom_document_load_html(string source);
Since: DOM extended
*/
PHP_METHOD(domdocument, loadHTML)
{
	dom_load_html(INTERNAL_FUNCTION_PARAM_PASSTHRU, DOM_LOAD_STRING);
}
/* }}} end dom_document_load_html */

/* {{{ proto int dom_document_save_html_file(string file);
Convenience method to save to file as html
*/
PHP_FUNCTION(dom_document_save_html_file)
{
	zval *id;
	xmlDoc *docp;
	int file_len, bytes, format;
	dom_object *intern;
	dom_doc_propsptr doc_props;
	char *file;
	const char *encoding;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Os", &id, dom_document_class_entry, &file, &file_len) == FAILURE) {
		return;
	}

	if (file_len == 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Filename");
		RETURN_FALSE;
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

/* {{{ proto string dom_document_save_html();
Convenience method to output as html
*/
PHP_FUNCTION(dom_document_save_html)
{
	zval *id, *nodep = NULL;
	xmlDoc *docp;
	xmlNode *node;
	xmlBufferPtr buf;
	dom_object *intern, *nodeobj;
	xmlChar *mem = NULL;
	int size, format;
	dom_doc_propsptr doc_props;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(),
		"O|O!", &id, dom_document_class_entry, &nodep, dom_node_class_entry)
		== FAILURE) {
		return;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

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
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not fetch buffer");
			RETURN_FALSE;
		}
		
		size = htmlNodeDump(buf, docp, node);
		if (size >= 0) {
			mem = (xmlChar*) xmlBufferContent(buf);
			if (!mem) {
				RETVAL_FALSE;
			} else {
				RETVAL_STRINGL((const char*) mem, size, 1);
			}
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Error dumping HTML node");
			RETVAL_FALSE;
		}
		xmlBufferFree(buf);
	} else {
#if LIBXML_VERSION >= 20623
		htmlDocDumpMemoryFormat(docp, &mem, &size, format);
#else
		htmlDocDumpMemory(docp, &mem, &size);
#endif
		if (!size) {
			RETVAL_FALSE;
		} else {
			RETVAL_STRINGL((const char*) mem, size, 1);
		}
		if (mem)
			xmlFree(mem);
	}

}
/* }}} end dom_document_save_html */

#endif  /* defined(LIBXML_HTML_ENABLED) */

/* {{{ proto boolean DOMDocument::registerNodeClass(string baseclass, string extendedclass);
   Register extended class used to create base node type */
PHP_METHOD(domdocument, registerNodeClass)
{
	zval *id;
	xmlDoc *docp;
	char *baseclass = NULL, *extendedclass = NULL;
	int baseclass_len = 0, extendedclass_len = 0;
	zend_class_entry *basece = NULL, *ce = NULL;
	dom_object *intern;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oss!", &id, dom_document_class_entry, &baseclass, &baseclass_len, &extendedclass, &extendedclass_len) == FAILURE) {
		return;
	}

	if (baseclass_len) {
		zend_class_entry **pce;
		if (zend_lookup_class(baseclass, baseclass_len, &pce TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s does not exist", baseclass);
			return;
		}
		basece = *pce;
	}

	if (basece == NULL || ! instanceof_function(basece, dom_node_class_entry TSRMLS_CC)) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s is not derived from DOMNode.", baseclass);
		return;
	}

	if (extendedclass_len) {
		zend_class_entry **pce;
		if (zend_lookup_class(extendedclass, extendedclass_len, &pce TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s does not exist", extendedclass);
		}
		ce = *pce;
	}

	if (ce == NULL || instanceof_function(ce, basece TSRMLS_CC)) {

		DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

		if (dom_set_doc_classmap(intern->document, basece, ce TSRMLS_CC) == FAILURE) {
			php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s could not be registered.", extendedclass);
		}
		RETURN_TRUE;
	} else {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Class %s is not derived from %s.", extendedclass, baseclass);
	}

	RETURN_FALSE;
}
/* }}} */

#endif  /* HAVE_LIBXML && HAVE_DOM */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
