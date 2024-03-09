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
#include "dom_properties.h"

/* {{{ name	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1844763134
Since:
*/
zend_result dom_documenttype_name_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);
	ZVAL_STRING(retval, dtdptr->name ? (char *) (dtdptr->name) : "");
	return SUCCESS;
}

/* }}} */

/* {{{ entities	DOMNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1788794630
Since:
*/
zend_result dom_documenttype_entities_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);

	php_dom_create_iterator(retval, DOM_DTD_NAMEDNODEMAP, php_dom_follow_spec_intern(obj));

	xmlHashTable *entityht = (xmlHashTable *) dtdptr->entities;

	dom_object *intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_ENTITY_NODE, intern, entityht, NULL, 0, NULL, 0);

	return SUCCESS;
}

/* }}} */

/* {{{ notations	DOMNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D46829EF
Since:
*/
zend_result dom_documenttype_notations_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);

	php_dom_create_iterator(retval, DOM_DTD_NAMEDNODEMAP, php_dom_follow_spec_intern(obj));

	xmlHashTable *notationht = (xmlHashTable *) dtdptr->notations;

	dom_object *intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_NOTATION_NODE, intern, notationht, NULL, 0, NULL, 0);

	return SUCCESS;
}

/* }}} */

/* {{{ publicId	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-Core-DocType-publicId
Since: DOM Level 2
*/
zend_result dom_documenttype_public_id_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);

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
zend_result dom_documenttype_system_id_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);

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
zend_result dom_documenttype_internal_subset_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlDtdPtr, dtdptr, obj);

	xmlDtdPtr intsubset;
	if (dtdptr->doc != NULL && ((intsubset = xmlGetIntSubset(dtdptr->doc)) != NULL)) {
		smart_str ret_buf = {0};
		xmlNodePtr cur = intsubset->children;

		while (cur != NULL) {
			xmlOutputBuffer *buff = xmlAllocOutputBuffer(NULL);

			if (buff != NULL) {
				xmlNodeDumpOutput (buff, NULL, cur, 0, 0, NULL);
				xmlOutputBufferFlush(buff);

				smart_str_appendl(&ret_buf, (const char *) xmlOutputBufferGetContent(buff), xmlOutputBufferGetSize(buff));

				(void)xmlOutputBufferClose(buff);
			}

			cur = cur->next;
		}

		if (ret_buf.s) {
			ZVAL_NEW_STR(retval, smart_str_extract(&ret_buf));
			return SUCCESS;
		}
	}

	ZVAL_NULL(retval);

	return SUCCESS;
}

/* }}} */

#endif
