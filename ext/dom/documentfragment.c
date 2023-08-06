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
* class DOMDocumentFragment extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-B63ED1A3
* Since:
*/

/* {{{ */
PHP_METHOD(DOMDocumentFragment, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	nodep = xmlNewDocFragment(NULL);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMDocumentFragment::__construct */

/* php_dom_xmlSetTreeDoc is a custom implementation of xmlSetTreeDoc
 needed for hack in appendXML due to libxml bug - no need to share this function */
static void php_dom_xmlSetTreeDoc(xmlNodePtr tree, xmlDocPtr doc) /* {{{ */
{
	xmlAttrPtr prop;
	xmlNodePtr cur;

	if (tree) {
		if(tree->type == XML_ELEMENT_NODE) {
			prop = tree->properties;
			while (prop != NULL) {
				prop->doc = doc;
				if (prop->children) {
					cur = prop->children;
					while (cur != NULL) {
						php_dom_xmlSetTreeDoc(cur, doc);
						cur = cur->next;
					}
				}
				prop = prop->next;
			}
		}
		if (tree->children != NULL) {
			cur = tree->children;
			while (cur != NULL) {
				php_dom_xmlSetTreeDoc(cur, doc);
				cur = cur->next;
			}
		}
		tree->doc = doc;
	}
}
/* }}} */

/* {{{ */
PHP_METHOD(DOMDocumentFragment, appendXML) {
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *data = NULL;
	size_t data_len = 0;
	int err;
	xmlNodePtr lst;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &data, &data_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (dom_node_is_read_only(nodep) == SUCCESS) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	if (data) {
		PHP_LIBXML_SANITIZE_GLOBALS(parse);
		err = xmlParseBalancedChunkMemory(nodep->doc, NULL, NULL, 0, (xmlChar *) data, &lst);
		PHP_LIBXML_RESTORE_GLOBALS(parse);
		if (err != 0) {
			RETURN_FALSE;
		}
		/* Following needed due to bug in libxml2 <= 2.6.14
		ifdef after next libxml release as bug is fixed in their cvs */
		php_dom_xmlSetTreeDoc(lst, nodep->doc);
		/* End stupid hack */

		xmlAddChildList(nodep,lst);
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-append
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMDocumentFragment, append)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
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
PHP_METHOD(DOMDocumentFragment, prepend)
{
	int argc = 0;
	zval *args, *id;
	dom_object *intern;
	xmlNode *context;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	id = ZEND_THIS;
	DOM_GET_OBJ(context, id, xmlNodePtr, intern);

	dom_parent_node_prepend(intern, args, argc);
}
/* }}} */

#endif
