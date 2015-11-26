/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2015 The PHP Group                                |
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
int dom_documenttype_name_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	ZVAL_STRING(retval, (char *) (dtdptr->name));

	return SUCCESS;
}

/* }}} */

/* {{{ entities	DOMNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1788794630
Since:
*/
int dom_documenttype_entities_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr doctypep = (xmlDtdPtr) dom_object_get_node(obj);
	xmlHashTable *entityht;
	dom_object *intern;

	if (doctypep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	php_dom_create_interator(retval, DOM_NAMEDNODEMAP);

	entityht = (xmlHashTable *) doctypep->entities;

	intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_ENTITY_NODE, intern, entityht, NULL, NULL);

	return SUCCESS;
}

/* }}} */

/* {{{ notations	DOMNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D46829EF
Since:
*/
int dom_documenttype_notations_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr doctypep = (xmlDtdPtr) dom_object_get_node(obj);
	xmlHashTable *notationht;
	dom_object *intern;

	if (doctypep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	php_dom_create_interator(retval, DOM_NAMEDNODEMAP);

	notationht = (xmlHashTable *) doctypep->notations;

	intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_NOTATION_NODE, intern, notationht, NULL, NULL);

	return SUCCESS;
}

/* }}} */

/* {{{ publicId	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-publicId
Since: DOM Level 2
*/
int dom_documenttype_public_id_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dtdptr->ExternalID) {
		ZVAL_STRING(retval, (char *) (dtdptr->ExternalID));
	} else {
		ZVAL_EMPTY_STRING(retval);
	}
	return SUCCESS;

}

/* }}} */

/* {{{ systemId	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-systemId
Since: DOM Level 2
*/
int dom_documenttype_system_id_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr dtdptr = (xmlDtdPtr) dom_object_get_node(obj);

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dtdptr->SystemID) {
		ZVAL_STRING(retval, (char *) (dtdptr->SystemID));
	} else {
		ZVAL_EMPTY_STRING(retval);
	}
	return SUCCESS;
}

/* }}} */

/* {{{ internalSubset	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-internalSubset
Since: DOM Level 2
*/
int dom_documenttype_internal_subset_read(dom_object *obj, zval *retval)
{
	xmlDtdPtr dtdptr = (xmlDtdPtr) dom_object_get_node(obj);
	xmlDtdPtr intsubset;

	if (dtdptr == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 0);
		return FAILURE;
	}

	if (dtdptr->doc != NULL && ((intsubset = xmlGetIntSubset(dtdptr->doc)) != NULL)) {
		smart_str ret_buf = {0};
		xmlNodePtr cur = intsubset->children;

		while (cur != NULL) {
			xmlOutputBuffer *buff = xmlAllocOutputBuffer(NULL);

			if (buff != NULL) {
				xmlNodeDumpOutput (buff, NULL, cur, 0, 0, NULL);
				xmlOutputBufferFlush(buff);

#ifdef LIBXML2_NEW_BUFFER
				smart_str_appendl(&ret_buf, (const char *) xmlOutputBufferGetContent(buff), xmlOutputBufferGetSize(buff));
#else
				smart_str_appendl(&ret_buf, (char *) buff->buffer->content, buff->buffer->use);
#endif

				(void)xmlOutputBufferClose(buff);
			}

			cur = cur->next;
		}

		if (ret_buf.s) {
			smart_str_0(&ret_buf);
			ZVAL_NEW_STR(retval, ret_buf.s);
			return SUCCESS;
		}
	}

	ZVAL_NULL(retval);

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
