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
#if HAVE_LIBXML && HAVE_DOM
#include "php_dom.h"

/*
* class domdocumenttype extends domnode 
*
* URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-412266927
* Since: 
*/

zend_function_entry php_dom_documenttype_class_functions[] = {
	{NULL, NULL, NULL}
};

/* {{{ attribute protos, not implemented yet */

/* {{{ proto name	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1844763134
Since: 
*/
int dom_documenttype_name_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);
	ALLOC_ZVAL(*retval);
	ZVAL_STRING(*retval, (char *) (dtdptr->name), 1);

	return SUCCESS;
}

/* }}} */



/* {{{ proto entities	namednodemap	
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

	MAKE_STD_ZVAL(*retval);
	php_dom_create_interator(*retval, DOM_NAMEDNODEMAP TSRMLS_CC);

	entityht = (xmlHashTable *) doctypep->entities;

	intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
	dom_namednode_iter(obj, XML_ENTITY_NODE, intern, entityht, NULL, NULL);

	return SUCCESS;
}

/* }}} */



/* {{{ proto notations	namednodemap	
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
	notationht = (xmlHashTable *) doctypep->notations;

	intern = (dom_object *)zend_objects_get_address(*retval TSRMLS_CC);
	dom_namednode_iter(obj, XML_NOTATION_NODE, intern, notationht, NULL, NULL);

	return SUCCESS;
}

/* }}} */



/* {{{ proto public_id	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-publicId
Since: DOM Level 2
*/
int dom_documenttype_public_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	if (dtdptr->ExternalID) {
		ZVAL_STRING(*retval, (char *) (dtdptr->ExternalID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	return SUCCESS;

}

/* }}} */



/* {{{ proto system_id	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-systemId
Since: DOM Level 2
*/
int dom_documenttype_system_id_read(dom_object *obj, zval **retval TSRMLS_DC)
{
	xmlDtdPtr dtdptr;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);
	if (dtdptr->SystemID) {
		ZVAL_STRING(*retval, (char *) (dtdptr->ExternalID), 1);
	} else {
		ZVAL_EMPTY_STRING(*retval);
	}
	return SUCCESS;
}

/* }}} */



/* {{{ proto internal_subset	string	
readonly=yes 
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-internalSubset
Since: DOM Level 2
*/
int dom_documenttype_internal_subset_read(dom_object *obj, zval **retval TSRMLS_DC)
{

	xmlDtdPtr dtdptr;
	xmlDtd *intsubset;
	xmlOutputBuffer *buff = NULL;
	xmlChar *strintsubset;

	dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	ALLOC_ZVAL(*retval);

	if (dtdptr->doc != NULL && ((intsubset = dtdptr->doc->intSubset) != NULL)) {
		buff = xmlAllocOutputBuffer(NULL);
		if (buff != NULL) {
			xmlNodeDumpOutput (buff, NULL, (xmlNodePtr) intsubset, 0, 0, NULL);
			xmlOutputBufferFlush(buff);
			strintsubset = xmlStrndup(buff->buffer->content, buff->buffer->use);
			(void)xmlOutputBufferClose(buff);
			ZVAL_STRING(*retval, (char *) strintsubset, 1);
			return SUCCESS;
		}
	}

	ZVAL_EMPTY_STRING(*retval);

	return SUCCESS;

}

/* }}} */

#endif
