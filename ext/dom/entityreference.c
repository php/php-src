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
#include "dom_properties.h"

/*
* class DOMEntityReference extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-11C98490
* Since:
*/

/* {{{ */
PHP_METHOD(DOMEntityReference, __construct)
{
	xmlNode *node;
	xmlNodePtr oldnode = NULL;
	dom_object *intern;
	char *name;
	size_t name_len, name_valid;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	name_valid = xmlValidateName(BAD_CAST name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	node = xmlNewReference(NULL, BAD_CAST name);

	if (!node) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, node, (void *)intern);
}
/* }}} end DOMEntityReference::__construct */

/* The following property handlers are necessary because of special lifetime management with entities and entity
 * references. The issue is that entity references hold a reference to an entity declaration, but don't
 * register that reference anywhere. When the entity declaration disappears we have no way of notifying the
 * entity references. Override the property handlers for the declaration-accessing properties to fix this problem. */

xmlEntityPtr dom_entity_reference_fetch_and_sync_declaration(xmlNodePtr reference)
{
	xmlEntityPtr entity = xmlGetDocEntity(reference->doc, reference->name);
	reference->children = (xmlNodePtr) entity;
	reference->last = (xmlNodePtr) entity;
	reference->content = entity ? entity->content : NULL;
	return entity;
}

zend_result dom_entity_reference_child_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	xmlEntityPtr entity = dom_entity_reference_fetch_and_sync_declaration(nodep);

	php_dom_create_nullable_object((xmlNodePtr) entity, retval, obj);
	return SUCCESS;
}

zend_result dom_entity_reference_text_content_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	dom_entity_reference_fetch_and_sync_declaration(nodep);
	return dom_node_text_content_read(obj, retval);
}

zend_result dom_entity_reference_child_nodes_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	dom_entity_reference_fetch_and_sync_declaration(nodep);
	return dom_node_child_nodes_read(obj, retval);
}

#endif
