/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
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

/*
* class DOMDocumentType extends DOMNode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-412266927
* Since: 
*/

const zend_function_entry php_dom_documenttype_class_functions[] = {
	PHP_FE_END
};

/* {{{ name	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1844763134
Since: 
*/
int dom_documenttype_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, (char *) (dtdptr->name), 1);

	return SUCCESS;
}

/* }}} */

/* {{{ entities	DOMNamedNodeMap	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1788794630
Since: 
*/
int dom_documenttype_entities_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr doctypep;
	xmlHashTable *entityht;
	dom_object *intern;

	doctypep = (xmlDtdPtr) dom_object_get_node(obj);

	if (doctypep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	MAKE_STD_ZVAL(*retval);
	php_dom_create_interator(*retval, DOM_NAMEDNODEMAP TSRMLS_CC);

	entityht = (xmlHashTable *) doctypep->entities;

	intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
	dom_namednode_iter(obj, XML_ENTITY_NODE, intern, entityht, NULL, NULL TSRMLS_CC);

	return SUCCESS;
}

/* }}} */

/* {{{ notations	DOMNamedNodeMap	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D46829EF
Since: 
*/
int dom_documenttype_notations_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr doctypep;
	xmlHashTable *notationht;
	dom_object *intern;

	doctypep = (xmlDtdPtr) dom_object_get_node(obj);

	if (doctypep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	MAKE_STD_ZVAL(*retval);
	php_dom_create_interator(*retval, DOM_NAMEDNODEMAP TSRMLS_CC);

	notationht = (xmlHashTable *) doctypep->notations;

	intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
	dom_namednode_iter(obj, XML_NOTATION_NODE, intern, notationht, NULL, NULL TSRMLS_CC);

	return SUCCESS;
}

/* }}} */

/* {{{ publicId	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-publicId
Since: DOM Level 2
*/
int dom_documenttype_public_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (dtdptr->ExternalID) {
		ZVAL_STRING(*retval, (char *) (dtdptr->ExternalID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	return SUCCESS;

}

/* }}} */

/* {{{ systemId	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-systemId
Since: DOM Level 2
*/
int dom_documenttype_system_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);
	if (dtdptr->SystemID) {
		ZVAL_STRING(*retval, (char *) (dtdptr->SystemID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	return SUCCESS;
}

/* }}} */

/* {{{ internalSubset	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-internalSubset
Since: DOM Level 2
*/
int dom_documenttype_internal_subset_read(dom_object *obj, zval **retval TSRMLS_DC)
{

	xmlDtdPtr dtdptr;
	xmlDtd *intsubset;
	xmlOutputBuffer *buff = NULL;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0 TSRMLS_CC);
		return FAILURE;
	}

	ALLOC_ZVAL(*retval);

	if (dtdptr->doc != NULL && ((intsubset = dtdptr->doc->intSubset) != NULL)) {
		buff = xmlAllocOutputBuffer(NULL);
		if (buff != NULL) {
			xmlNodeDumpOutput (buff, NULL, (xmlNodePtr) intsubset, 0, 0, NULL);
			xmlOutputBufferFlush(buff);
#ifdef LIBXML2_NEW_BUFFER
			ZVAL_STRINGL(*retval, xmlOutputBufferGetContent(buff), xmlOutputBufferGetSize(buff), 1);
#else
			ZVAL_STRINGL(*retval, buff->buffer->content, buff->buffer->use, 1);
#endif
			(void)xmlOutputBufferClose(buff);
			return SUCCESS;
		}
	}

	ZVAL_EMPTY_STRING(*retval);

	return SUCCESS;

}

/* }}} */

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
