/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
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
* class DOMText extends DOMCharacterData
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#ID-1312295772
* Since:
*/

/* {{{ */
PHP_METHOD(DOMText, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *value = NULL;
	size_t value_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|s", &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	nodep = xmlNewText(BAD_CAST value);

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMText::__construct */

/* {{{ wholeText	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-wholeText
Since: DOM Level 3
*/
zend_result dom_text_whole_text_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, node, obj);

	smart_str str = {0};

	/* Find starting text node */
	while (node->prev && ((node->prev->type == XML_TEXT_NODE) || (node->prev->type == XML_CDATA_SECTION_NODE))) {
		node = node->prev;
	}

	/* concatenate all adjacent text and cdata nodes */
	while (node && ((node->type == XML_TEXT_NODE) || (node->type == XML_CDATA_SECTION_NODE))) {
		if (node->content) {
			smart_str_appends(&str, (const char *) node->content);
		}
		node = node->next;
	}

	ZVAL_STR(retval, smart_str_extract(&str));

	return SUCCESS;
}

/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-38853C1D
Modern spec URL: https://dom.spec.whatwg.org/#dom-text-splittext
Since:
*/
PHP_METHOD(DOMText, splitText)
{
	xmlChar    *first;
	xmlChar    *second;
	xmlNodePtr  node;
	xmlNodePtr  nnode;
	zend_long        offset;
	int         length;
	dom_object	*intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &offset) == FAILURE) {
		RETURN_THROWS();
	}
	DOM_GET_OBJ(node, ZEND_THIS, xmlNodePtr, intern);

	if (offset < 0) {
		zend_argument_value_error(1, "must be greater than or equal to 0");
		RETURN_THROWS();
	}

	const xmlChar *cur = php_dom_get_content_or_empty(node);
	length = xmlUTF8Strlen(cur);

	if (ZEND_LONG_INT_OVFL(offset) || (int)offset > length) {
		if (php_dom_follow_spec_intern(intern)) {
			php_dom_throw_error(INDEX_SIZE_ERR, /* strict */ true);
		}
		RETURN_FALSE;
	}

	first = xmlUTF8Strndup(cur, (int)offset);
	second = xmlUTF8Strsub(cur, (int)offset, (int)(length - offset));

	xmlNodeSetContent(node, NULL);
	node->content = first;
	nnode = xmlNewDocText(node->doc, NULL);

	if (nnode == NULL) {
		xmlFree(second);
		php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		RETURN_THROWS();
	}

	nnode->content = second;

	if (node->parent != NULL) {
		nnode->type = XML_ELEMENT_NODE;
		xmlAddNextSibling(node, nnode);
		nnode->type = XML_TEXT_NODE;
	}

	php_dom_create_object(nnode, return_value, intern);
}
/* }}} end dom_text_split_text */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Text3-isWhitespaceInElementContent
Since: DOM Level 3
*/
PHP_METHOD(DOMText, isWhitespaceInElementContent)
{
	xmlNodePtr  node;
	dom_object	*intern;
	ZEND_PARSE_PARAMETERS_NONE();
	DOM_GET_OBJ(node, ZEND_THIS, xmlNodePtr, intern);
	RETURN_BOOL(xmlIsBlankNode(node));
}
/* }}} end dom_text_is_whitespace_in_element_content */

#endif
