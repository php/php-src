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


/*
* class DOMEntity extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-527DCFF2
* Since:
*/

/* {{{ publicId	string
readonly=yes
URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D7303025
Since:
*/
zend_result dom_entity_public_id_read(dom_object *obj, zval *retval)
{
	xmlEntity *nodep = (xmlEntity *) dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->etype != XML_EXTERNAL_GENERAL_UNPARSED_ENTITY || !nodep->ExternalID) {
		ZVAL_NULL(retval);
	} else {
		ZVAL_STRING(retval, (char *) (nodep->ExternalID));
	}

	return SUCCESS;
}

/* }}} */

/* {{{ systemId	string
readonly=yes
URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D7C29F3E
Since:
*/
zend_result dom_entity_system_id_read(dom_object *obj, zval *retval)
{
	xmlEntity *nodep = (xmlEntity *) dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->etype != XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		ZVAL_NULL(retval);
	} else {
		ZVAL_STRING(retval, (char *) (nodep->SystemID));
	}

	return SUCCESS;
}

/* }}} */

/* {{{ notationName	string
readonly=yes
URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6ABAEB38
Since:
*/
zend_result dom_entity_notation_name_read(dom_object *obj, zval *retval)
{
	xmlEntity *nodep = (xmlEntity *) dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->etype != XML_EXTERNAL_GENERAL_UNPARSED_ENTITY) {
		ZVAL_NULL(retval);
	} else {
		/* According to spec, NULL is only allowed for unparsed entities, if it's not set we should use the empty string. */
		if (!nodep->content) {
			ZVAL_EMPTY_STRING(retval);
		} else {
			ZVAL_STRING(retval, (const char *) nodep->content);
		}
	}

	return SUCCESS;
}

/* }}} */

/* {{{ actualEncoding	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Entity3-actualEncoding
Since: DOM Level 3
*/
zend_result dom_entity_actual_encoding_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ encoding	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Entity3-encoding
Since: DOM Level 3
*/
zend_result dom_entity_encoding_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* {{{ version	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Entity3-version
Since: DOM Level 3
*/
zend_result dom_entity_version_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

#endif
