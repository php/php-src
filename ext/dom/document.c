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
#include "php_dom.h"

typedef struct _idsIterator idsIterator;
struct _idsIterator {
	xmlChar *elementId;
	xmlNode *element;
};

#define DOM_LOAD_STRING 0
#define DOM_LOAD_FILE 1

static void idsHashScanner(void *payload, void *data, xmlChar *name)
{
	idsIterator *priv = (idsIterator *) data;

	if (priv->element == NULL && xmlStrEqual (name, priv->elementId)) {
		priv->element = ((xmlNode *)((xmlID *)payload)->attr)->parent;
	}
}

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
	{NULL, NULL, NULL}
};

/* {{{ void add_domdocument_properties(zval *id) */
void add_domdocument_properties(zval *id TSRMLS_DC) {
	add_property_bool(id, "formatOutput", 0);
	add_property_bool(id, "validateOnParse", 0);
	add_property_bool(id, "resolveExternals", 0);
	add_property_bool(id, "preserveWhiteSpace", 1);
	add_property_bool(id, "substituteEntities", 0);
}
/* }}} end add_domdocument_properties */

/* {{{ static int dom_document_get_property_int(zval *id, char *property TSRMLS_DC) */
static int dom_document_get_property_int(zval *id, char *property TSRMLS_DC) {
	zval *format, *member;
	zend_object_handlers *std_hnd;
	int retformat = 0;

	MAKE_STD_ZVAL(member);
	ZVAL_STRING(member, property, 1);

	std_hnd = zend_get_std_object_handlers();
	format = std_hnd->read_property(id, member, 0 TSRMLS_CC);

	if (format->type == IS_BOOL) {
		retformat = Z_BVAL_P(format);
	}

	zval_dtor(member);
	FREE_ZVAL(member);

	return retformat;
}
/* }}} end dom_document_get_property_int */

/* {{{ static void php_dom_ctx_error(void *ctx, const char *msg, ...) */
static void php_dom_ctx_error(void *ctx, const char *msg, ...) {
	va_list ap;
	char *buf;
	int len;
	xmlParserCtxtPtr parser;

	TSRMLS_FETCH();

	parser = (xmlParserCtxtPtr) ctx;

	va_start(ap, msg);
	len = vspprintf(&buf, 0, msg, ap);
	va_end(ap);
	
	/* remove any trailing \n */
	while (len && buf[--len] == '\n') {
		buf[len] = '\0';
	}

	php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s in %s, line: %d", buf, parser->input->filename, parser->input->line);
	efree(buf);
}
/* }}} end php_dom_ctx_error */

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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
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
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Cannot create required DOM object");
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
	int charset;

	docp = (xmlDocPtr) dom_object_get_node(obj);
	if (docp->encoding != NULL) {
		xmlFree((xmlChar *)docp->encoding);
	}

	docp->encoding = xmlStrdup((const xmlChar *) Z_STRVAL_P(newval));
    charset = (int)xmlParseCharEncoding((const char*) docp->encoding);
    if (charset > 0) {
		return SUCCESS;
    } else {
        /* TODO: ERROR XML_CHAR_ENCODING_ERROR */
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
	ALLOC_ZVAL(*retval);
	ZVAL_NULL(*retval);
	return SUCCESS;
}

int dom_document_strict_error_checking_write(dom_object *obj, zval *newval TSRMLS_DC)
{
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
	xmlXPathContextPtr ctxp;
	xmlDocPtr docp;
	xmlXPathObjectPtr xpathobjp;
	dom_object *intern;

	int name_len, ret;
	char *str,*name;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE) {
		return;
	}

	ctxp = xmlXPathNewContext(docp);

	ctxp->node = NULL;
	str = (char*) emalloc((name_len+3) * sizeof(char)) ;
	sprintf(str ,"//%s",name);

	xpathobjp = xmlXPathEval(str, ctxp);
	efree(str);
	ctxp->node = NULL;

	if (!xpathobjp) {
		RETURN_FALSE;
	}

	if (xpathobjp->type ==  XPATH_NODESET) {
		int i;
		xmlNodeSetPtr nodesetp;

		if (NULL == (nodesetp = xpathobjp->nodesetval)) {
			xmlXPathFreeObject (xpathobjp);
			xmlXPathFreeContext(ctxp);
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
	xmlXPathFreeContext(ctxp);
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		nodep = xmlNewDocNode (docp, NULL, localname, NULL);
		if (nodep != NULL) {
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
		php_dom_throw_error(errorcode, &return_value TSRMLS_CC);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	root = xmlDocGetRootElement(docp);
	if (root != NULL) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			nodep = (xmlNodePtr) xmlNewDocProp(docp, localname, NULL);
			if (nodep != NULL) {
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
		php_dom_throw_error(errorcode, &return_value TSRMLS_CC);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid Namespace");
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
	xmlNodePtr elemp;
	int uri_len, name_len;
	dom_object *intern;
	char *uri, *name;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &uri, &uri_len, &name, &name_len) == FAILURE) {
		return;
	}

	array_init(return_value);
	elemp = xmlDocGetRootElement(docp);

	dom_get_elements_by_tag_name_ns_raw(elemp, uri, name, &return_value, intern TSRMLS_CC);
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
	idsIterator iter;
	xmlHashTable *ids = NULL;
	int ret,idname_len;
	dom_object *intern;
	char *idname;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &idname, &idname_len) == FAILURE) {
		return;
	}

	ids = (xmlHashTable *) docp->ids;
	if (ids) {
		iter.elementId = (xmlChar *) idname;
		iter.element = NULL;
		xmlHashScan(ids, (void *)idsHashScanner, &iter);
		DOM_RET_OBJ(rv, (xmlNodePtr) iter.element, &ret, intern);
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
			decrement_node_ptr(intern TSRMLS_CC);
			refcount = decrement_document_reference(intern TSRMLS_CC);
			if (refcount != 0) {
				olddoc->_private = NULL;
			}
		}
		intern->document = NULL;
		increment_document_reference(intern, docp TSRMLS_CC);

		php_dom_set_object(intern, (xmlNodePtr) docp TSRMLS_CC);
	}

	add_domdocument_properties(id TSRMLS_CC);
}
/* }}} end dom_document_document */

/* {{{ static xmlDocPtr dom_document_parser(zval *id, int mode, char *source TSRMLS_DC) */
static xmlDocPtr dom_document_parser(zval *id, int mode, char *source TSRMLS_DC) {
    xmlDocPtr ret;
    xmlParserCtxtPtr ctxt;
	char *directory = NULL;
	int validate, resolve_externals, keep_blanks, substitute_ent;

	if (id != NULL) {
		validate = dom_document_get_property_int(id, "validateOnParse" TSRMLS_CC);
		resolve_externals = dom_document_get_property_int(id, "resolveExternals" TSRMLS_CC);
		keep_blanks = dom_document_get_property_int(id, "preserveWhiteSpace" TSRMLS_CC);
		substitute_ent = dom_document_get_property_int(id, "substituteEntities" TSRMLS_CC);
	} else {
		validate = 0;
		resolve_externals = 0;
		keep_blanks = 1;
		substitute_ent = 0;
	}

	xmlInitParser();

	if (mode == DOM_LOAD_FILE) {
		ctxt = xmlCreateFileParserCtxt(source);
	} else {
		ctxt = xmlCreateDocParserCtxt(source);
	}

	if (ctxt == NULL) {
		return(NULL);
	}

	if (mode == DOM_LOAD_FILE) {
		if ((ctxt->directory == NULL) && (directory == NULL))
			directory = xmlParserGetDirectory(source);
		if ((ctxt->directory == NULL) && (directory != NULL))
			ctxt->directory = (char *) xmlStrdup((xmlChar *) directory);
	}

	ctxt->recovery = 0;
	ctxt->validate = validate;
    ctxt->loadsubset = resolve_externals;
    ctxt->keepBlanks = keep_blanks;
    if (ctxt->keepBlanks == 0)
		ctxt->sax->ignorableWhitespace = ignorableWhitespace;
	ctxt->replaceEntities = substitute_ent;

	ctxt->vctxt.error = php_dom_ctx_error;
	ctxt->vctxt.warning = php_dom_ctx_error;
	if (ctxt->sax != NULL) {
		ctxt->sax->error = php_dom_ctx_error;
	}

	xmlParseDocument(ctxt);

	if (ctxt->wellFormed)
		ret = ctxt->myDoc;
	else {
		ret = NULL;
		xmlFreeDoc(ctxt->myDoc);
		ctxt->myDoc = NULL;
	}

	xmlFreeParserCtxt(ctxt);

	return(ret);
}
/* }}} end dom_parser_document */

/* {{{ static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) */
static void dom_parse_document(INTERNAL_FUNCTION_PARAMETERS, int mode) {
	zval *id, *rv = NULL;
	xmlDoc *docp = NULL, *newdoc;
	dom_object *intern;
	char *source;
	int source_len, refcount, ret;

	id = getThis();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source, &source_len) == FAILURE) {
		return;
	}

	if (mode == DOM_LOAD_FILE) {
		if ((PG(safe_mode) && (!php_checkuid(source, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(source TSRMLS_CC)) {
			RETURN_FALSE;
		}
	}

	newdoc = dom_document_parser(id, mode, source TSRMLS_CC);

	if (!newdoc)
		RETURN_FALSE;

	if (id != NULL) {
		intern = (dom_object *)zend_object_store_get_object(id TSRMLS_CC);
		if (intern != NULL) {
			docp = (xmlDocPtr) dom_object_get_node(intern);
			if (docp != NULL) {
				decrement_node_ptr(intern TSRMLS_CC);
				refcount = decrement_document_reference(intern TSRMLS_CC);
				if (refcount != 0) {
					docp->_private = NULL;
				}
			}
			intern->document = NULL;
			increment_document_reference(intern, newdoc TSRMLS_CC);
		}

		php_dom_set_object(intern, (xmlNodePtr) newdoc TSRMLS_CC);

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
	char *file;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &file, &file_len) == FAILURE) {
		return;
	}
	
	if ((PG(safe_mode) && (!php_checkuid(file, NULL, CHECKUID_CHECK_FILE_AND_DIR))) || php_check_open_basedir(file TSRMLS_CC)) {
		RETURN_FALSE;
	}

	/* encoding handled by property on doc */
	format = dom_document_get_property_int(id, "formatOutput" TSRMLS_CC);
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
	int size, format;

	DOM_GET_THIS_OBJ(docp, id, xmlDocPtr, intern);

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &nodep) == FAILURE) {
		return;
	}

	format = dom_document_get_property_int(id, "formatOutput" TSRMLS_CC);

	if (nodep != NULL) {
		/* Dump contents of Node */
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, &return_value TSRMLS_CC);
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Node not from same document");
			RETURN_FALSE;
		}
		buf = xmlBufferCreate();
		if (!buf) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not fetch buffer");
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
