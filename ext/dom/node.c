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
#include "namespace_compat.h"
#include "internal_helpers.h"

/*
* class DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1950641247
* Since:
*/

zend_string *dom_node_concatenated_name_helper(size_t name_len, const char *name, size_t prefix_len, const char *prefix)
{
	if (UNEXPECTED(prefix_len > ZSTR_MAX_LEN / 2 - 1 || name_len > ZSTR_MAX_LEN / 2 - 1)) {
		return zend_empty_string;
	}
	zend_string *str = zend_string_alloc(prefix_len + 1 + name_len, false);
	memcpy(ZSTR_VAL(str), prefix, prefix_len);
	ZSTR_VAL(str)[prefix_len] = ':';
	memcpy(ZSTR_VAL(str) + prefix_len + 1, name, name_len + 1 /* include \0 */);
	return str;
}

zend_string *dom_node_get_node_name_attribute_or_element(const xmlNode *nodep, bool uppercase)
{
	zend_string *ret;
	size_t name_len = strlen((const char *) nodep->name);
	if (nodep->ns != NULL && nodep->ns->prefix != NULL) {
		ret = dom_node_concatenated_name_helper(name_len, (const char *) nodep->name, strlen((const char *) nodep->ns->prefix), (const char *) nodep->ns->prefix);
	} else {
		ret = zend_string_init((const char *) nodep->name, name_len, false);
	}
	if (uppercase) {
		zend_str_toupper(ZSTR_VAL(ret), ZSTR_LEN(ret));
	}
	return ret;
}

bool php_dom_is_node_connected(const xmlNode *node)
{
	ZEND_ASSERT(node != NULL);
	do {
		if (node->type == XML_DOCUMENT_NODE || node->type == XML_HTML_DOCUMENT_NODE) {
			return true;
		}
		node = node->parent;
	} while (node != NULL);
	return false;
}

/* {{{ nodeName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D095
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-nodename
Since:
*/
zend_result dom_node_node_name_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	bool uppercase = false;

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
			uppercase = php_dom_follow_spec_intern(obj) && dom_ns_is_html_and_document_is_html(nodep);
			ZEND_FALLTHROUGH;
		case XML_ATTRIBUTE_NODE:
			ZVAL_STR(retval, dom_node_get_node_name_attribute_or_element(nodep, uppercase));
			break;
		case XML_NAMESPACE_DECL: {
			xmlNsPtr ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				zend_string *str = dom_node_concatenated_name_helper(strlen((const char *) ns->prefix), (const char *) ns->prefix, strlen("xmlns"), "xmlns");
				ZVAL_STR(retval, str);
			} else {
				ZVAL_STRING(retval, (const char *) nodep->name);
			}
			break;
		}
		case XML_DOCUMENT_TYPE_NODE:
		case XML_DTD_NODE:
		case XML_PI_NODE:
		case XML_ENTITY_DECL:
		case XML_ENTITY_REF_NODE:
		case XML_NOTATION_NODE:
			ZVAL_STRING(retval, (char *) nodep->name);
			break;
		case XML_CDATA_SECTION_NODE:
			ZVAL_STRING(retval, "#cdata-section");
			break;
		case XML_COMMENT_NODE:
			ZVAL_STRING(retval, "#comment");
			break;
		case XML_HTML_DOCUMENT_NODE:
		case XML_DOCUMENT_NODE:
			ZVAL_STRING(retval, "#document");
			break;
		case XML_DOCUMENT_FRAG_NODE:
			ZVAL_STRING(retval, "#document-fragment");
			break;
		case XML_TEXT_NODE:
			ZVAL_STRING(retval, "#text");
			break;
		EMPTY_SWITCH_DEFAULT_CASE();
	}

	return SUCCESS;
}

/* }}} */

/* {{{ nodeValue	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68D080
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-nodevalue
Since:
*/
zend_result dom_node_node_value_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Access to Element node is implemented as a convenience method */
	switch (nodep->type) {
		case XML_ELEMENT_NODE: {
			if (php_dom_follow_spec_intern(obj)) {
				ZVAL_NULL(retval);
				break;
			}
			ZEND_FALLTHROUGH;
		}
		case XML_ATTRIBUTE_NODE:
		case XML_TEXT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			php_dom_get_content_into_zval(nodep, retval, true);
			break;
		case XML_NAMESPACE_DECL: {
			char *str = (char *) xmlNodeGetContent(nodep->children);
			if (str != NULL) {
				ZVAL_STRING(retval, str);
				xmlFree(str);
			} else {
				ZVAL_NULL(retval);
			}
			break;
		}
		default:
			ZVAL_NULL(retval);
			break;
	}

	return SUCCESS;
}

zend_result dom_node_node_value_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	zend_string *str;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	str = zval_try_get_string(newval);
	if (UNEXPECTED(!str)) {
		return FAILURE;
	}

	/* Access to Element node is implemented as a convenience method */
	switch (nodep->type) {
		case XML_ATTRIBUTE_NODE:
			if (php_dom_follow_spec_intern(obj)) {
				dom_remove_all_children(nodep);
				xmlAddChild(nodep, xmlNewTextLen((xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str)));
				break;
			}
			ZEND_FALLTHROUGH;
		case XML_ELEMENT_NODE:
			dom_remove_all_children(nodep);
			ZEND_FALLTHROUGH;
		case XML_TEXT_NODE:
		case XML_COMMENT_NODE:
		case XML_CDATA_SECTION_NODE:
		case XML_PI_NODE:
			xmlNodeSetContentLen(nodep, (xmlChar *) ZSTR_VAL(str), ZSTR_LEN(str));
			break;
		default:
			break;
	}

	php_libxml_invalidate_node_list_cache(obj->document);

	zend_string_release_ex(str, 0);
	return SUCCESS;
}

/* }}} */

/* {{{ nodeType	int
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-111237558
Since:
*/
zend_result dom_node_node_type_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	/* Specs dictate that they are both type XML_DOCUMENT_TYPE_NODE */
	if (nodep->type == XML_DTD_NODE) {
		ZVAL_LONG(retval, XML_DOCUMENT_TYPE_NODE);
	} else {
		ZVAL_LONG(retval, nodep->type);
	}

	return SUCCESS;
}

/* }}} */

static zend_result dom_node_parent_get(dom_object *obj, zval *retval, bool only_element)
{
	xmlNodePtr nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	xmlNodePtr nodeparent = nodep->parent;
	if (!nodeparent || (only_element && nodeparent->type != XML_ELEMENT_NODE)) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nodeparent, retval, obj);
	return SUCCESS;
}

/* {{{ parentNode	?DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1060184317
Since:
*/
zend_result dom_node_parent_node_read(dom_object *obj, zval *retval)
{
	return dom_node_parent_get(obj, retval, false);
}

/* }}} */

/* {{{ parentElement	?DomElement
readonly=yes
URL: https://dom.spec.whatwg.org/#parent-element
Since:
*/
zend_result dom_node_parent_element_read(dom_object *obj, zval *retval)
{
	return dom_node_parent_get(obj, retval, true);
}

/* }}} */

/* {{{ childNodes	DomNodeList
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1451460987
Since:
*/
zend_result dom_node_child_nodes_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	dom_object *intern;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	php_dom_create_iterator(retval, DOM_NODELIST, php_dom_follow_spec_intern(obj));
	intern = Z_DOMOBJ_P(retval);
	dom_namednode_iter(obj, XML_ELEMENT_NODE, intern, NULL, NULL, 0, NULL, 0);

	return SUCCESS;
}
/* }}} */

/* {{{ firstChild DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-169727388
Since:
*/
zend_result dom_node_first_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *first = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep)) {
		first = nodep->children;
	}

	if (!first) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(first, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ lastChild	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-61AD09FB
Since:
*/
zend_result dom_node_last_child_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *last = NULL;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (dom_node_children_valid(nodep)) {
		last = nodep->last;
	}

	if (!last) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(last, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ previousSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-640FB3C8
Since:
*/
zend_result dom_node_previous_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *prevsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	prevsib = nodep->prev;
	if (!prevsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(prevsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ nextSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6AC54C2F
Since:
*/
zend_result dom_node_next_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *nextsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	nextsib = nodep->next;
	if (!nextsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nextsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ previousElementSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-640FB3C8
Since:
*/
zend_result dom_node_previous_element_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *prevsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	prevsib = nodep->prev;

	while (prevsib && prevsib->type != XML_ELEMENT_NODE) {
		prevsib = prevsib->prev;
	}

	if (!prevsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(prevsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ nextElementSibling	DomNode
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6AC54C2F
Since:
*/
zend_result dom_node_next_element_sibling_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep, *nextsib;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	nextsib = nodep->next;

	while (nextsib != NULL && nextsib->type != XML_ELEMENT_NODE) {
		nextsib = nextsib->next;
	}

	if (!nextsib) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	php_dom_create_object(nextsib, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ attributes	DomNamedNodeMap
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-84CF096
Since:
*/
zend_result dom_node_attributes_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	dom_object *intern;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_ELEMENT_NODE) {
		php_dom_create_iterator(retval, DOM_NAMEDNODEMAP, php_dom_follow_spec_intern(obj));
		intern = Z_DOMOBJ_P(retval);
		dom_namednode_iter(obj, XML_ATTRIBUTE_NODE, intern, NULL, NULL, 0, NULL, 0);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ isConnected	boolean
readonly=yes
URL: https://dom.spec.whatwg.org/#dom-node-isconnected
Since:
*/
zend_result dom_node_is_connected_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	ZVAL_BOOL(retval, php_dom_is_node_connected(nodep));
	return SUCCESS;
}
/* }}} */

/* {{{ ownerDocument	DomDocument
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-node-ownerDoc
Since:
*/
zend_result dom_node_owner_document_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlDocPtr docp;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
		ZVAL_NULL(retval);
		return SUCCESS;
	}

	docp = nodep->doc;
	if (!docp) {
		return FAILURE;
	}

	php_dom_create_object((xmlNodePtr) docp, retval, obj);
	return SUCCESS;
}

/* }}} */

/* {{{ namespaceUri	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSname
Since: DOM Level 2
*/
zend_result dom_node_namespace_uri_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
		case XML_NAMESPACE_DECL:
			if (nodep->ns != NULL) {
				str = (char *) nodep->ns->href;
			}
			break;
		default:
			str = NULL;
			break;
	}

	if (str != NULL) {
		ZVAL_STRING(retval, str);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ prefix	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSPrefix
Modern spec URL: https://dom.spec.whatwg.org/#concept-element-namespace-prefix
Since: DOM Level 2
*/
zend_result dom_node_prefix_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlNsPtr ns;
	char *str = NULL;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
		case XML_ATTRIBUTE_NODE:
		case XML_NAMESPACE_DECL:
			ns = nodep->ns;
			if (ns != NULL && ns->prefix) {
				str = (char *) ns->prefix;
			}
			break;
		default:
			str = NULL;
			break;
	}

	if (str == NULL) {
		ZVAL_EMPTY_STRING(retval);
	} else {
		ZVAL_STRING(retval, str);
	}
	return SUCCESS;
}

zend_result dom_modern_node_prefix_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	xmlNsPtr ns = nodep->ns;
	if (ns != NULL && ns->prefix != NULL) {
		ZVAL_STRING(retval, (const char *) ns->prefix);
	} else {
		ZVAL_NULL(retval);
	}
	return SUCCESS;
}

zend_result dom_node_prefix_write(dom_object *obj, zval *newval)
{
	zend_string *prefix_str;
	xmlNode *nodep, *nsnode = NULL;
	xmlNsPtr ns = NULL, curns;
	char *strURI;
	char *prefix;

	nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	switch (nodep->type) {
		case XML_ELEMENT_NODE:
			nsnode = nodep;
			ZEND_FALLTHROUGH;
		case XML_ATTRIBUTE_NODE:
			if (nsnode == NULL) {
				nsnode = nodep->parent;
				if (nsnode == NULL) {
					nsnode = xmlDocGetRootElement(nodep->doc);
				}
			}
			/* Typed property, this is already a string */
			ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
			prefix_str = Z_STR_P(newval);

			prefix = ZSTR_VAL(prefix_str);
			if (*prefix == '\0') {
				/* The empty string namespace prefix does not exist.
				 * We should fall back to the default namespace in this case. */
				prefix = NULL;
			}
			if (nsnode && nodep->ns != NULL && !xmlStrEqual(nodep->ns->prefix, (xmlChar *)prefix)) {
				strURI = (char *) nodep->ns->href;
				/* Validate namespace naming constraints */
				if (strURI == NULL ||
					(zend_string_equals_literal(prefix_str, "xml") && strcmp(strURI, (char *) XML_XML_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && zend_string_equals_literal(prefix_str, "xmlns") &&
					 strcmp(strURI, (char *) DOM_XMLNS_NAMESPACE)) ||
					(nodep->type == XML_ATTRIBUTE_NODE && !strcmp((char *) nodep->name, "xmlns"))) {
					php_dom_throw_error(NAMESPACE_ERR, dom_get_strict_error(obj->document));
					return FAILURE;
				} else {
					curns = nsnode->nsDef;
					while (curns != NULL) {
						if (xmlStrEqual((xmlChar *)prefix, curns->prefix) && xmlStrEqual(nodep->ns->href, curns->href)) {
							ns = curns;
							break;
						}
						curns = curns->next;
					}
					if (ns == NULL) {
						ns = xmlNewNs(nsnode, nodep->ns->href, (xmlChar *)prefix);
						/* Sadly, we cannot distinguish between OOM and namespace conflict.
						 * But OOM will almost never happen. */
						if (UNEXPECTED(ns == NULL)) {
							php_dom_throw_error(NAMESPACE_ERR, /* strict */ true);
							return FAILURE;
						}
					}
				}

				xmlSetNs(nodep, ns);
			}
			break;
		default:
			break;
	}

	return SUCCESS;
}

/* }}} */

/* {{{ localName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeNSLocalN
Since: DOM Level 2
*/
zend_result dom_node_local_name_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (nodep->type == XML_ELEMENT_NODE || nodep->type == XML_ATTRIBUTE_NODE || nodep->type == XML_NAMESPACE_DECL) {
		ZVAL_STRING(retval, (char *) (nodep->name));
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ baseURI	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-baseURI
Since: DOM Level 3
*/
zend_result dom_node_base_uri_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);
	xmlChar *baseuri;

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	baseuri = xmlNodeGetBase(nodep->doc, nodep);
	if (baseuri) {
		ZVAL_STRING(retval, (char *) (baseuri));
		xmlFree(baseuri);
	} else {
		ZVAL_NULL(retval);
	}

	return SUCCESS;
}

/* }}} */

/* {{{ textContent	string
readonly=no
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-textContent
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-textcontent
Since: DOM Level 3
*/
/* Determines when the operation is a no-op. */
static bool dom_skip_text_content(dom_object *obj, xmlNodePtr nodep)
{
	if (php_dom_follow_spec_intern(obj)) {
		int type = nodep->type;
		if (type != XML_DOCUMENT_FRAG_NODE && type != XML_ELEMENT_NODE && type != XML_ATTRIBUTE_NODE
			&& type != XML_TEXT_NODE && type != XML_CDATA_SECTION_NODE && type != XML_COMMENT_NODE && type != XML_PI_NODE) {
			/* Yes, success... It's a no-op for these cases. */
			return true;
		}
	}
	return false;
}

zend_result dom_node_text_content_read(dom_object *obj, zval *retval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	if (dom_skip_text_content(obj, nodep)) {
		ZVAL_NULL(retval);
	} else {
		php_dom_get_content_into_zval(nodep, retval, false);
	}

	return SUCCESS;
}

zend_result dom_node_text_content_write(dom_object *obj, zval *newval)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, 1);
		return FAILURE;
	}

	php_libxml_invalidate_node_list_cache(obj->document);

	/* Typed property, this is already a string */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING || Z_TYPE_P(newval) == IS_NULL);
	const xmlChar *xmlChars;
	size_t len;
	if (Z_TYPE_P(newval) == IS_NULL) {
		xmlChars = (const xmlChar *) "";
		len = 0;
	} else {
		xmlChars = (const xmlChar *) Z_STRVAL_P(newval);
		len = Z_STRLEN_P(newval);
	}

	int type = nodep->type;

	/* We can't directly call xmlNodeSetContent, because it might encode the string through
	 * xmlStringLenGetNodeList for types XML_DOCUMENT_FRAG_NODE, XML_ELEMENT_NODE, XML_ATTRIBUTE_NODE.
	 * See tree.c:xmlNodeSetContent in libxml.
	 * In these cases we need to use a text node to avoid the encoding.
	 * For the other cases, we *can* rely on xmlNodeSetContent because it is either a no-op, or handles
	 * the content without encoding. */
	if (type == XML_DOCUMENT_FRAG_NODE || type == XML_ELEMENT_NODE || type == XML_ATTRIBUTE_NODE) {
		dom_remove_all_children(nodep);
		xmlNode *textNode = xmlNewDocTextLen(nodep->doc, xmlChars, len);
		xmlAddChild(nodep, textNode);
	} else {
		xmlNodeSetContent(nodep, xmlChars);
	}

	return SUCCESS;
}

/* }}} */

static xmlNodePtr _php_dom_insert_fragment(xmlNodePtr nodep, xmlNodePtr prevsib, xmlNodePtr nextsib, xmlNodePtr fragment, dom_object *intern) /* {{{ */
{
	xmlNodePtr newchild, node;

	newchild = fragment->children;

	if (newchild) {
		if (prevsib == NULL) {
			nodep->children = newchild;
		} else {
			prevsib->next = newchild;
		}
		newchild->prev = prevsib;
		if (nextsib == NULL) {
			nodep->last = fragment->last;
		} else {
			fragment->last->next = nextsib;
			nextsib->prev = fragment->last;
		}

		node = newchild;
		while (node != NULL) {
			node->parent = nodep;
			if (node->doc != nodep->doc) {
				xmlSetTreeDoc(node, nodep->doc);
				dom_object *childobj = node->_private;
				if (childobj != NULL) {
					childobj->document = intern->document;
					php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
				}
			}
			if (node == fragment->last) {
				break;
			}
			node = node->next;
		}

		fragment->children = NULL;
		fragment->last = NULL;
	}

	return newchild;
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-952280727
Since:
*/
static void dom_node_insert_before_legacy(zval *return_value, zval *ref, dom_object *intern, dom_object *childobj, xmlNodePtr parentp, xmlNodePtr child)
{
	if (!dom_node_children_valid(parentp)) {
		RETURN_FALSE;
	}

	xmlNodePtr new_child = NULL;
	int stricterror = dom_get_strict_error(intern->document);
	int ret;

	if (dom_node_is_read_only(parentp) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (dom_hierarchy(parentp, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->doc != parentp->doc && child->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->type == XML_DOCUMENT_FRAG_NODE && child->children == NULL) {
		/* TODO Drop Warning? */
		php_error_docref(NULL, E_WARNING, "Document Fragment is empty");
		RETURN_FALSE;
	}

	if (child->doc == NULL && parentp->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
	}

	php_libxml_invalidate_node_list_cache(intern->document);

	if (ref != NULL) {
		xmlNodePtr refp;
		dom_object *refpobj;
		DOM_GET_OBJ(refp, ref, xmlNodePtr, refpobj);
		if (refp->parent != parentp) {
			php_dom_throw_error(NOT_FOUND_ERR, stricterror);
			RETURN_FALSE;
		}

		if (child->parent != NULL) {
			xmlUnlinkNode(child);
		}

		if (child->type == XML_TEXT_NODE && (refp->type == XML_TEXT_NODE ||
			(refp->prev != NULL && refp->prev->type == XML_TEXT_NODE))) {
			if (child->doc == NULL) {
				xmlSetTreeDoc(child, parentp->doc);
			}
			new_child = child;
			new_child->parent = refp->parent;
			new_child->next = refp;
			new_child->prev = refp->prev;
			refp->prev = new_child;
			if (new_child->prev != NULL) {
				new_child->prev->next = new_child;
			}
			if (new_child->parent != NULL) {
				if (new_child->parent->children == refp) {
					new_child->parent->children = new_child;
				}
			}

		} else if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(refp->parent, child->name);
			else
				lastattr = xmlHasNsProp(refp->parent, child->name, child->ns->href);
			if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr);
				} else {
					DOM_RET_OBJ(child, &ret, intern);
					return;
				}
			}
			new_child = xmlAddPrevSibling(refp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
		} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
			xmlNodePtr last = child->last;
			new_child = _php_dom_insert_fragment(parentp, refp->prev, refp, child, intern);
			dom_reconcile_ns_list(parentp->doc, new_child, last);
		} else {
			new_child = xmlAddPrevSibling(refp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
			dom_reconcile_ns(parentp->doc, new_child);
		}
	} else {
		if (child->parent != NULL){
			xmlUnlinkNode(child);
		}
		if (child->type == XML_TEXT_NODE && parentp->last != NULL && parentp->last->type == XML_TEXT_NODE) {
			child->parent = parentp;
			if (child->doc == NULL) {
				xmlSetTreeDoc(child, parentp->doc);
			}
			new_child = child;
			if (parentp->children == NULL) {
				parentp->children = child;
				parentp->last = child;
			} else {
				child = parentp->last;
				child->next = new_child;
				new_child->prev = child;
				parentp->last = new_child;
			}
		} else 	if (child->type == XML_ATTRIBUTE_NODE) {
			xmlAttrPtr lastattr;

			if (child->ns == NULL)
				lastattr = xmlHasProp(parentp, child->name);
			else
				lastattr = xmlHasNsProp(parentp, child->name, child->ns->href);
			if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
				if (lastattr != (xmlAttrPtr) child) {
					xmlUnlinkNode((xmlNodePtr) lastattr);
					php_libxml_node_free_resource((xmlNodePtr) lastattr);
				} else {
					DOM_RET_OBJ(child, &ret, intern);
					return;
				}
			}
			new_child = xmlAddChild(parentp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
		} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
			xmlNodePtr last = child->last;
			new_child = _php_dom_insert_fragment(parentp, parentp->last, NULL, child, intern);
			dom_reconcile_ns_list(parentp->doc, new_child, last);
		} else {
			new_child = xmlAddChild(parentp, child);
			if (UNEXPECTED(NULL == new_child)) {
				goto cannot_add;
			}
			dom_reconcile_ns(parentp->doc, new_child);
		}
	}

	DOM_RET_OBJ(new_child, &ret, intern);
	return;
cannot_add:
	zend_throw_error(NULL, "Cannot add newnode as the previous sibling of refnode");
	RETURN_THROWS();
}
/* }}} end dom_node_insert_before */

/* https://dom.spec.whatwg.org/#dom-node-insertbefore */
static void dom_node_insert_before_modern(zval *return_value, zval *ref, dom_object *intern, dom_object *childobj, xmlNodePtr parentp, xmlNodePtr child)
{
	int ret;
	xmlNodePtr refp = NULL;
	dom_object *refobjp;
	if (php_dom_pre_insert_is_parent_invalid(parentp)) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, /* strict */ true);
		RETURN_THROWS();
	}
	if (ref != NULL) {
		DOM_GET_OBJ(refp, ref, xmlNodePtr, refobjp);
	}
	php_libxml_invalidate_node_list_cache(intern->document);
	php_dom_pre_insert(intern->document, child, parentp, refp);
	DOM_RET_OBJ(child, &ret, intern);
}

static void dom_node_insert_before(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zval *id, *node, *ref = NULL;
	xmlNodePtr child, parentp;
	dom_object *intern, *childobj;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O|O!", &node, dom_get_node_ce(modern), &ref, dom_get_node_ce(modern)) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(parentp, id, xmlNodePtr, intern);

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	if (modern) {
		dom_node_insert_before_modern(return_value, ref, intern, childobj, parentp, child);
	} else {
		dom_node_insert_before_legacy(return_value, ref, intern, childobj, parentp, child);
	}
}

PHP_METHOD(DOMNode, insertBefore)
{
	dom_node_insert_before(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Node, insertBefore)
{
	dom_node_insert_before(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}

/* https://dom.spec.whatwg.org/#concept-node-replace */
static zend_result dom_replace_node_validity_checks(xmlNodePtr parent, xmlNodePtr node, xmlNodePtr child)
{
	/* 1. If parent is not a Document, DocumentFragment, or Element node, then throw a "HierarchyRequestError" DOMException. */
	if (php_dom_pre_insert_is_parent_invalid(parent)) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, /* strict */ true);
		return FAILURE;
	}

	/* 2. If node is a host-including inclusive ancestor of parent, then throw a "HierarchyRequestError" DOMException. */
	if (dom_hierarchy(parent, node) != SUCCESS) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, /* strict */ true);
		return FAILURE;
	}

	/* 3. If child’s parent is not parent, then throw a "NotFoundError" DOMException. */
	if (child->parent != parent) {
		php_dom_throw_error(NOT_FOUND_ERR, /* strict */ true);
		return FAILURE;
	}

	/* 4. If node is not a DocumentFragment, DocumentType, Element, or CharacterData node, then throw a "HierarchyRequestError" DOMException. */
	if (node->type != XML_DOCUMENT_FRAG_NODE
		&& node->type != XML_DTD_NODE
		&& node->type != XML_ELEMENT_NODE
		&& node->type != XML_TEXT_NODE
		&& node->type != XML_CDATA_SECTION_NODE
		&& node->type != XML_COMMENT_NODE
		&& node->type != XML_PI_NODE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, /* strict */ true);
		return FAILURE;
	}

	/* 5. If either node is a Text node and parent is a document, or node is a doctype and parent is not a document,
	 *    then throw a "HierarchyRequestError" DOMException. */
	bool parent_is_document = parent->type == XML_DOCUMENT_NODE || parent->type == XML_HTML_DOCUMENT_NODE;
	if (parent_is_document && (node->type == XML_TEXT_NODE || node->type == XML_CDATA_SECTION_NODE)) {
		php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert text as a child of a document", /* strict */ true);
		return FAILURE;
	}
	if (!parent_is_document && node->type == XML_DTD_NODE) {
		php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot insert a document type into anything other than a document", /* strict */ true);
		return FAILURE;
	}

	/* 6. If parent is a document, and any of the statements below, switched on the interface node implements, are true,
	 *    then throw a "HierarchyRequestError" DOMException.
	 *    Spec note: These statements _slightly_ differ from the pre-insert algorithm. */
	if (parent_is_document) {
		/* DocumentFragment */
		if (node->type == XML_DOCUMENT_FRAG_NODE) {
			if (!php_dom_fragment_insertion_hierarchy_check_replace(parent, node, child)) {
				return FAILURE;
			}
		}
		/* Element */
		else if (node->type == XML_ELEMENT_NODE) {
			/* parent has an element child that is not child ... */
			if (xmlDocGetRootElement((xmlDocPtr) parent) != child) {
				php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Cannot have more than one element child in a document", /* strict */ true);
				return FAILURE;
			}
			/* ... or a doctype is following child. */
			if (php_dom_has_sibling_following_node(child, XML_DTD_NODE)) {
				php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
				return FAILURE;
			}
		}
		/* DocumentType */
		else if (node->type == XML_DTD_NODE) {
			/* parent has a doctype child that is not child, or an element is preceding child. */
			xmlDocPtr doc = (xmlDocPtr) parent;
			if (doc->intSubset != (xmlDtdPtr) child || php_dom_has_sibling_preceding_node(child, XML_ELEMENT_NODE)) {
				php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", /* strict */ true);
				return FAILURE;
			}
		}
	}

	/* Steps 7 and onward perform the removal and insertion, and also track changes for mutation records.
	 * We don't implement mutation records so we can just skip straight to the replace part. */

	return SUCCESS;
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-785887307
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-replacechild
Since:
*/
static void dom_node_replace_child(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zval *id, *newnode, *oldnode;
	xmlNodePtr newchild, oldchild, nodep;
	dom_object *intern, *newchildobj, *oldchildobj;

	int ret;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "OO", &newnode, dom_get_node_ce(modern), &oldnode, dom_get_node_ce(modern)) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_GET_OBJ(newchild, newnode, xmlNodePtr, newchildobj);
	DOM_GET_OBJ(oldchild, oldnode, xmlNodePtr, oldchildobj);

	int stricterror = dom_get_strict_error(intern->document);

	if (newchild->doc != nodep->doc && newchild->doc != NULL) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (modern) {
		if (dom_replace_node_validity_checks(nodep, newchild, oldchild) != SUCCESS) {
			RETURN_THROWS();
		}
	} else {
		if (!dom_node_children_valid(nodep)) {
			RETURN_FALSE;
		}

		if (!nodep->children) {
			RETURN_FALSE;
		}

		if (dom_node_is_read_only(nodep) == SUCCESS ||
			(newchild->parent != NULL && dom_node_is_read_only(newchild->parent) == SUCCESS)) {
			php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
			RETURN_FALSE;
		}

		if (dom_hierarchy(nodep, newchild) == FAILURE) {
			php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
			RETURN_FALSE;
		}

		if (oldchild->parent != nodep) {
			php_dom_throw_error(NOT_FOUND_ERR, stricterror);
			RETURN_FALSE;
		}
	}

	if (newchild->type == XML_DOCUMENT_FRAG_NODE) {
		xmlNodePtr prevsib, nextsib;
		prevsib = oldchild->prev;
		nextsib = oldchild->next;

		xmlUnlinkNode(oldchild);

		xmlNodePtr last = newchild->last;
		newchild = _php_dom_insert_fragment(nodep, prevsib, nextsib, newchild, intern);
		if (newchild && !modern) {
			dom_reconcile_ns_list(nodep->doc, newchild, last);
		}
	} else if (oldchild != newchild) {
		xmlDtdPtr intSubset = xmlGetIntSubset(nodep->doc);
		bool replacedoctype = (intSubset == (xmlDtd *) oldchild);

		if (newchild->doc == NULL && nodep->doc != NULL) {
			xmlSetTreeDoc(newchild, nodep->doc);
			newchildobj->document = intern->document;
			php_libxml_increment_doc_ref((php_libxml_node_object *)newchildobj, NULL);
		}
		xmlReplaceNode(oldchild, newchild);
		if (!modern) {
			dom_reconcile_ns(nodep->doc, newchild);
		}

		if (replacedoctype) {
			nodep->doc->intSubset = (xmlDtd *) newchild;
		}
	}
	php_libxml_invalidate_node_list_cache(intern->document);
	DOM_RET_OBJ(oldchild, &ret, intern);
}

PHP_METHOD(DOMNode, replaceChild)
{
	dom_node_replace_child(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Node, replaceChild)
{
	dom_node_replace_child(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_node_replace_child */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1734834066
Since:
*/
static void dom_node_remove_child(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *node_ce)
{
	zval *id, *node;
	xmlNodePtr child, nodep;
	dom_object *intern, *childobj;
	int ret, stricterror;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, node_ce) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (!dom_node_children_valid(nodep)) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (!nodep->children || child->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, stricterror);
		RETURN_FALSE;
	}

	xmlUnlinkNode(child);
	php_libxml_invalidate_node_list_cache(intern->document);
	DOM_RET_OBJ(child, &ret, intern);
}

PHP_METHOD(DOMNode, removeChild)
{
	dom_node_remove_child(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_node_class_entry);
}

PHP_METHOD(DOM_Node, removeChild)
{
	dom_node_remove_child(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_node_class_entry);
}
/* }}} end dom_node_remove_child */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-184E7107
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-appendchild
Since:
*/
static void dom_node_append_child_legacy(zval *return_value, dom_object *intern, dom_object *childobj, xmlNodePtr nodep, xmlNodePtr child)
{
	xmlNodePtr new_child = NULL;
	int ret, stricterror;

	if (!dom_node_children_valid(nodep)) {
		RETURN_FALSE;
	}

	stricterror = dom_get_strict_error(intern->document);

	if (dom_node_is_read_only(nodep) == SUCCESS ||
		(child->parent != NULL && dom_node_is_read_only(child->parent) == SUCCESS)) {
		php_dom_throw_error(NO_MODIFICATION_ALLOWED_ERR, stricterror);
		RETURN_FALSE;
	}

	if (dom_hierarchy(nodep, child) == FAILURE) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, stricterror);
		RETURN_FALSE;
	}

	if (!(child->doc == NULL || child->doc == nodep->doc)) {
		php_dom_throw_error(WRONG_DOCUMENT_ERR, stricterror);
		RETURN_FALSE;
	}

	if (child->type == XML_DOCUMENT_FRAG_NODE && child->children == NULL) {
		/* TODO Drop Warning? */
		php_error_docref(NULL, E_WARNING, "Document Fragment is empty");
		RETURN_FALSE;
	}

	if (child->doc == NULL && nodep->doc != NULL) {
		childobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)childobj, NULL);
	}

	if (child->parent != NULL){
		xmlUnlinkNode(child);
	}

	if (child->type == XML_TEXT_NODE && nodep->last != NULL && nodep->last->type == XML_TEXT_NODE) {
		child->parent = nodep;
		if (child->doc == NULL) {
			xmlSetTreeDoc(child, nodep->doc);
		}
		new_child = child;
		if (nodep->children == NULL) {
			nodep->children = child;
			nodep->last = child;
		} else {
			child = nodep->last;
			child->next = new_child;
			new_child->prev = child;
			nodep->last = new_child;
		}
	} else 	if (child->type == XML_ATTRIBUTE_NODE) {
		xmlAttrPtr lastattr;

		if (child->ns == NULL)
			lastattr = xmlHasProp(nodep, child->name);
		else
			lastattr = xmlHasNsProp(nodep, child->name, child->ns->href);
		if (lastattr != NULL && lastattr->type != XML_ATTRIBUTE_DECL) {
			if (lastattr != (xmlAttrPtr) child) {
				xmlUnlinkNode((xmlNodePtr) lastattr);
				php_libxml_node_free_resource((xmlNodePtr) lastattr);
			}
		}
		new_child = xmlAddChild(nodep, child);
		if (UNEXPECTED(new_child == NULL)) {
			goto cannot_add;
		}
		php_dom_reconcile_attribute_namespace_after_insertion((xmlAttrPtr) new_child);
	} else if (child->type == XML_DOCUMENT_FRAG_NODE) {
		xmlNodePtr last = child->last;
		new_child = _php_dom_insert_fragment(nodep, nodep->last, NULL, child, intern);
		dom_reconcile_ns_list(nodep->doc, new_child, last);
	} else if (child->type == XML_DTD_NODE) {
		if (nodep->doc->intSubset != NULL) {
			php_dom_throw_error_with_message(HIERARCHY_REQUEST_ERR, "Document types must be the first child in a document", stricterror);
			RETURN_FALSE;
		}
		new_child = xmlAddChild(nodep, child);
		if (UNEXPECTED(new_child == NULL)) {
			goto cannot_add;
		}
		nodep->doc->intSubset = (xmlDtdPtr) new_child;
	} else {
		new_child = xmlAddChild(nodep, child);
		if (UNEXPECTED(new_child == NULL)) {
			goto cannot_add;
		}
		dom_reconcile_ns(nodep->doc, new_child);
	}

	php_libxml_invalidate_node_list_cache(intern->document);

	DOM_RET_OBJ(new_child, &ret, intern);
	return;
cannot_add:
	php_dom_throw_error(INVALID_STATE_ERR, stricterror);
	RETURN_FALSE;
}
/* }}} end dom_node_append_child */

PHP_METHOD(DOMNode, appendChild)
{
	zval *node;
	xmlNodePtr nodep, child;
	dom_object *intern, *childobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(node, dom_node_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(nodep, ZEND_THIS, xmlNodePtr, intern);
	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	dom_node_append_child_legacy(return_value, intern, childobj, nodep, child);
}

PHP_METHOD(DOM_Node, appendChild)
{
	zval *node;
	xmlNodePtr nodep, child;
	dom_object *intern, *childobj;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(node, dom_modern_node_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	DOM_GET_OBJ(nodep, ZEND_THIS, xmlNodePtr, intern);
	DOM_GET_OBJ(child, node, xmlNodePtr, childobj);

	/* Parent check from pre-insertion validation done here:
	 * If parent is not a Document, DocumentFragment, or Element node, then throw a "HierarchyRequestError" DOMException. */
	if (php_dom_pre_insert_is_parent_invalid(nodep)) {
		php_dom_throw_error(HIERARCHY_REQUEST_ERR, /* strict */ true);
		RETURN_THROWS();
	}
	/* Append, this doesn't do the parent check so we do it here. */
	php_libxml_invalidate_node_list_cache(intern->document);
	php_dom_node_append(intern->document, child, nodep);
	int ret;
	DOM_RET_OBJ(child, &ret, intern);
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-810594187
Since:
*/
PHP_METHOD(DOMNode, hasChildNodes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (!dom_node_children_valid(nodep)) {
		RETURN_FALSE;
	}

	if (nodep->children) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_has_child_nodes */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-3A0ED0A4
Since:
*/
PHP_METHOD(DOMNode, cloneNode)
{
	zval *id;
	xmlNode *n, *node;
	int ret;
	dom_object *intern;
	bool recursive = 0;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|b", &recursive) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(n, id, xmlNodePtr, intern);

	dom_libxml_ns_mapper *ns_mapper = NULL;
	bool clone_document = n->type == XML_DOCUMENT_NODE || n->type == XML_HTML_DOCUMENT_NODE;
	if (php_dom_follow_spec_intern(intern)) {
		if (clone_document) {
			ns_mapper = dom_libxml_ns_mapper_create();
		} else {
			ns_mapper = php_dom_get_ns_mapper(intern);
		}
	}

	node = dom_clone_node(ns_mapper, n, n->doc, recursive);

	if (!node) {
		if (clone_document && ns_mapper != NULL) {
			dom_libxml_ns_mapper_destroy(ns_mapper);
		}
		RETURN_FALSE;
	}

	/* If document cloned we want a new document proxy */
	if (clone_document) {
		dom_object *new_intern;
		if (ns_mapper) {
			/* We have the issue here that we can't create a modern node without an intern.
			 * Fortunately, it's impossible to have a custom document class for the modern DOM (final base class),
			 * so we can solve this by invoking the instantiation helper directly. */
			zend_class_entry *ce = n->type == XML_DOCUMENT_NODE ? dom_xml_document_class_entry : dom_html_document_class_entry;
			new_intern = php_dom_instantiate_object_helper(return_value, ce, node, NULL);
		} else {
			DOM_RET_OBJ(node, &ret, NULL);
			new_intern = Z_DOMOBJ_P(return_value);
		}
		php_dom_update_document_after_clone(intern, n, new_intern, node);
		ZEND_ASSERT(new_intern->document->private_data == NULL);
		new_intern->document->private_data = dom_libxml_ns_mapper_header(ns_mapper);
	} else {
		if (node->type == XML_ATTRIBUTE_NODE && n->ns != NULL && node->ns == NULL) {
			/* Let reconciliation deal with this. The lifetime of the namespace poses no problem
			 * because we're increasing the refcount of the document proxy at the return.
			 * libxml2 doesn't set the ns because it can't know that this is safe. */
			node->ns = n->ns;
		}

		DOM_RET_OBJ(node, &ret, intern);
	}
}
/* }}} end dom_node_clone_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-normalize
Since:
*/
PHP_METHOD(DOMNode, normalize)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	php_libxml_invalidate_node_list_cache(intern->document);

	if (php_dom_follow_spec_intern(intern)) {
		php_dom_normalize_modern(nodep);
	} else {
		php_dom_normalize_legacy(nodep);
	}
}
/* }}} end dom_node_normalize */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-Level-2-Core-Node-supports
Since: DOM Level 2
*/
PHP_METHOD(DOMNode, isSupported)
{
	zend_string *feature, *version;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &feature, &version) == FAILURE) {
		RETURN_THROWS();
	}

	RETURN_BOOL(dom_has_feature(feature, version));
}
/* }}} end dom_node_is_supported */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-NodeHasAttrs
Since: DOM Level 2
*/
PHP_METHOD(DOMNode, hasAttributes)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (nodep->type != XML_ELEMENT_NODE)
		RETURN_FALSE;

	if (nodep->properties) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} end dom_node_has_attributes */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-isSameNode
Since: DOM Level 3
*/
static void dom_node_is_same_node(INTERNAL_FUNCTION_PARAMETERS, zval *node)
{
	zval *id;
	xmlNodePtr nodeotherp, nodep;
	dom_object *intern, *nodeotherobj;

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	DOM_GET_OBJ(nodeotherp, node, xmlNodePtr, nodeotherobj);

	if (nodep == nodeotherp) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}

PHP_METHOD(DOMNode, isSameNode)
{
	zval *node;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_node_class_entry) != SUCCESS) {
		RETURN_THROWS();
	}

	dom_node_is_same_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, node);
}

PHP_METHOD(DOM_Node, isSameNode)
{
	zval *node;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O!", &node, dom_modern_node_class_entry) != SUCCESS) {
		RETURN_THROWS();
	}

	if (node == NULL) {
		RETURN_FALSE;
	}

	dom_node_is_same_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, node);
}
/* }}} end dom_node_is_same_node */

static bool php_dom_node_is_content_equal(const xmlNode *this, const xmlNode *other)
{
	xmlChar *this_content = xmlNodeGetContent(this);
	xmlChar *other_content = xmlNodeGetContent(other);
	bool result = xmlStrEqual(this_content, other_content);
	xmlFree(this_content);
	xmlFree(other_content);
	return result;
}

static bool php_dom_node_is_ns_uri_equal(const xmlNode *this, const xmlNode *other)
{
	const xmlChar *this_ns = this->ns ? this->ns->href : NULL;
	const xmlChar *other_ns = other->ns ? other->ns->href : NULL;
	return xmlStrEqual(this_ns, other_ns);
}

static bool php_dom_node_is_ns_prefix_equal(const xmlNode *this, const xmlNode *other)
{
	const xmlChar *this_ns = this->ns ? this->ns->prefix : NULL;
	const xmlChar *other_ns = other->ns ? other->ns->prefix : NULL;
	return xmlStrEqual(this_ns, other_ns);
}

static bool php_dom_node_is_equal_node(const xmlNode *this, const xmlNode *other, bool spec_compliant);

#define PHP_DOM_FUNC_CAT(prefix, suffix) prefix##_##suffix
/* xmlNode and xmlNs have incompatible struct layouts, i.e. the next field is in a different offset */
#define PHP_DOM_DEFINE_LIST_COUNTER_HELPER(type)																							\
	static size_t PHP_DOM_FUNC_CAT(php_dom_node_count_list_size, type)(const type *node)													\
	{																																		\
		size_t counter = 0;																													\
		while (node) {																														\
			counter++;																														\
			node = node->next;																												\
		}																																	\
		return counter;																														\
	}
#define PHP_DOM_DEFINE_LIST_EQUALITY_ORDERED_HELPER(type)																					\
	static bool PHP_DOM_FUNC_CAT(php_dom_node_list_equality_check_ordered, type)(const type *list1, const type *list2, bool spec_compliant)	\
	{																																		\
		size_t count = PHP_DOM_FUNC_CAT(php_dom_node_count_list_size, type)(list1);															\
		if (count != PHP_DOM_FUNC_CAT(php_dom_node_count_list_size, type)(list2)) {															\
			return false;																													\
		}																																	\
		for (size_t i = 0; i < count; i++) {																								\
			if (!php_dom_node_is_equal_node((const xmlNode *) list1, (const xmlNode *) list2, spec_compliant)) {							\
				return false;																												\
			}																																\
			list1 = list1->next;																											\
			list2 = list2->next;																											\
		}																																	\
		return true;																														\
	}
#define PHP_DOM_DEFINE_LIST_EQUALITY_UNORDERED_HELPER(type)																					\
	static bool PHP_DOM_FUNC_CAT(php_dom_node_list_equality_check_unordered, type)(const type *list1, const type *list2, bool spec_compliant)\
	{																																		\
		size_t count = PHP_DOM_FUNC_CAT(php_dom_node_count_list_size, type)(list1);															\
		if (count != PHP_DOM_FUNC_CAT(php_dom_node_count_list_size, type)(list2)) {															\
			return false;																													\
		}																																	\
		for (const type *n1 = list1; n1 != NULL; n1 = n1->next) {																			\
			bool found = false;																												\
			for (const type *n2 = list2; n2 != NULL && !found; n2 = n2->next) {																\
				if (php_dom_node_is_equal_node((const xmlNode *) n1, (const xmlNode *) n2, spec_compliant)) {								\
					found = true;																											\
				}																															\
			}																																\
			if (!found) {																													\
				return false;																												\
			}																																\
		}																																	\
		return true;																														\
	}

PHP_DOM_DEFINE_LIST_COUNTER_HELPER(xmlNode)
PHP_DOM_DEFINE_LIST_COUNTER_HELPER(xmlNs)
PHP_DOM_DEFINE_LIST_EQUALITY_ORDERED_HELPER(xmlNode)
PHP_DOM_DEFINE_LIST_EQUALITY_UNORDERED_HELPER(xmlNode)
PHP_DOM_DEFINE_LIST_EQUALITY_UNORDERED_HELPER(xmlNs)

static bool php_dom_is_equal_attr(const xmlAttr *this_attr, const xmlAttr *other_attr)
{
	ZEND_ASSERT(this_attr != NULL);
	ZEND_ASSERT(other_attr != NULL);
	return xmlStrEqual(this_attr->name, other_attr->name)
		&& php_dom_node_is_ns_uri_equal((const xmlNode *) this_attr, (const xmlNode *) other_attr)
		&& php_dom_node_is_content_equal((const xmlNode *) this_attr, (const xmlNode *) other_attr);
}

static bool php_dom_node_is_equal_node(const xmlNode *this, const xmlNode *other, bool spec_compliant)
{
	ZEND_ASSERT(this != NULL);
	ZEND_ASSERT(other != NULL);

	if (this->type != other->type) {
		return false;
	}

	/* Notes:
	 *   - XML_DOCUMENT_TYPE_NODE is no longer created by libxml2, we only have to support XML_DTD_NODE.
	 *   - element and attribute declarations are not exposed as nodes in DOM, so no comparison is needed for those. */
	if (this->type == XML_ELEMENT_NODE) {
		return xmlStrEqual(this->name, other->name)
			&& php_dom_node_is_ns_prefix_equal(this, other)
			&& php_dom_node_is_ns_uri_equal(this, other)
			/* Check attributes first, then namespace declarations, then children */
			&& php_dom_node_list_equality_check_unordered_xmlNode((const xmlNode *) this->properties, (const xmlNode *) other->properties, spec_compliant)
			&& (spec_compliant || php_dom_node_list_equality_check_unordered_xmlNs(this->nsDef, other->nsDef, false))
			&& php_dom_node_list_equality_check_ordered_xmlNode(this->children, other->children, spec_compliant);
	} else if (this->type == XML_DTD_NODE) {
		/* Note: in the living spec entity declarations and notations are no longer compared because they're considered obsolete. */
		const xmlDtd *this_dtd = (const xmlDtd *) this;
		const xmlDtd *other_dtd = (const xmlDtd *) other;
		return xmlStrEqual(this_dtd->name, other_dtd->name)
			&& xmlStrEqual(this_dtd->ExternalID, other_dtd->ExternalID)
			&& xmlStrEqual(this_dtd->SystemID, other_dtd->SystemID);
	} else if (this->type == XML_PI_NODE) {
		return xmlStrEqual(this->name, other->name) && xmlStrEqual(this->content, other->content);
	} else if (this->type == XML_TEXT_NODE || this->type == XML_COMMENT_NODE || this->type == XML_CDATA_SECTION_NODE) {
		return xmlStrEqual(this->content, other->content);
	} else if (this->type == XML_ATTRIBUTE_NODE) {
		const xmlAttr *this_attr = (const xmlAttr *) this;
		const xmlAttr *other_attr = (const xmlAttr *) other;
		return php_dom_is_equal_attr(this_attr, other_attr);
	} else if (this->type == XML_ENTITY_REF_NODE) {
		return xmlStrEqual(this->name, other->name);
	} else if (this->type == XML_ENTITY_DECL || this->type == XML_NOTATION_NODE || this->type == XML_ENTITY_NODE) {
		const xmlEntity *this_entity = (const xmlEntity *) this;
		const xmlEntity *other_entity = (const xmlEntity *) other;
		return this_entity->etype == other_entity->etype
			&& xmlStrEqual(this_entity->name, other_entity->name)
			&& xmlStrEqual(this_entity->ExternalID, other_entity->ExternalID)
			&& xmlStrEqual(this_entity->SystemID, other_entity->SystemID)
			&& php_dom_node_is_content_equal(this, other);
	} else if (this->type == XML_NAMESPACE_DECL) {
		const xmlNs *this_ns = (const xmlNs *) this;
		const xmlNs *other_ns = (const xmlNs *) other;
		return xmlStrEqual(this_ns->prefix, other_ns->prefix) && xmlStrEqual(this_ns->href, other_ns->href);
	} else if (this->type == XML_DOCUMENT_FRAG_NODE || this->type == XML_HTML_DOCUMENT_NODE || this->type == XML_DOCUMENT_NODE) {
		return php_dom_node_list_equality_check_ordered_xmlNode(this->children, other->children, spec_compliant);
	}

	return false;
}

/* {{{ URL: https://dom.spec.whatwg.org/#dom-node-isequalnode (for everything still in the living spec)
*      URL: https://www.w3.org/TR/2004/REC-DOM-Level-3-Core-20040407/DOM3-Core.html#core-Node3-isEqualNode (for old nodes removed from the living spec)
Since: DOM Level 3
*/
static void dom_node_is_equal_node_common(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zval *id, *node;
	xmlNodePtr otherp, nodep;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O!", &node, dom_get_node_ce(modern)) == FAILURE) {
		RETURN_THROWS();
	}

	if (node == NULL) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(otherp, node, xmlNodePtr, intern);
	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (nodep == otherp) {
		RETURN_TRUE;
	}

	/* Empty fragments/documents only match if they're both empty */
	if (nodep == NULL || otherp == NULL) {
		RETURN_BOOL(nodep == NULL && otherp == NULL);
	}

	RETURN_BOOL(php_dom_node_is_equal_node(nodep, otherp, modern));
}

PHP_METHOD(DOMNode, isEqualNode)
{
	dom_node_is_equal_node_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Node, isEqualNode)
{
	dom_node_is_equal_node_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end DOMNode::isEqualNode */

/* https://dom.spec.whatwg.org/#locate-a-namespace-prefix */
static const xmlChar *dom_locate_a_namespace_prefix(xmlNodePtr elem, const char *uri)
{
	do {
		/* 1. If element’s namespace is namespace and its namespace prefix is non-null, then return its namespace prefix. */
		if (elem->ns != NULL && elem->ns->prefix != NULL && xmlStrEqual(elem->ns->href, BAD_CAST uri)) {
			return elem->ns->prefix;
		}

		/* 2. If element has an attribute whose namespace prefix is "xmlns" and value is namespace,
		*     then return element’s first such attribute’s local name. */
		for (xmlAttrPtr attr = elem->properties; attr != NULL; attr = attr->next) {
			if (attr->ns != NULL && attr->children != NULL
				&& xmlStrEqual(attr->ns->prefix, BAD_CAST "xmlns") && xmlStrEqual(attr->children->content, BAD_CAST uri)) {
				return attr->name;
			}
		}

		/* 3. If element’s parent element is not null, then return the result of running locate a namespace prefix on that element using namespace. */
		elem = elem->parent;
	} while (elem != NULL && elem->type == XML_ELEMENT_NODE);

	/* 4. Return null. */
	return NULL;
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Node3-lookupNamespacePrefix
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-lookupprefix
Since: DOM Level 3
*/
static void dom_node_lookup_prefix(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	zval *id;
	xmlNodePtr nodep, lookupp = NULL;
	dom_object *intern;
	xmlNsPtr nsptr;
	size_t uri_len = 0;
	char *uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), modern ? "s!" : "s", &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	/* 1. If namespace is null or the empty string, then return null. */
	if (uri_len > 0) {
		/* 2. Switch on the interface this implements: */
		switch (nodep->type) {
			case XML_ELEMENT_NODE:
				lookupp = nodep;
				break;
			case XML_DOCUMENT_NODE:
			case XML_HTML_DOCUMENT_NODE:
				lookupp = xmlDocGetRootElement((xmlDocPtr) nodep);
				break;
			case XML_ENTITY_NODE :
			case XML_NOTATION_NODE:
			case XML_DOCUMENT_FRAG_NODE:
			case XML_DOCUMENT_TYPE_NODE:
			case XML_DTD_NODE:
				RETURN_NULL();
				break;
			default:
				lookupp = nodep->parent;
		}

		if (lookupp != NULL) {
			if (modern) {
				const char * result = (const char *) dom_locate_a_namespace_prefix(lookupp, uri);
				if (result != NULL) {
					RETURN_STRING(result);
				}
			} else {
				nsptr = xmlSearchNsByHref(lookupp->doc, lookupp, (xmlChar *) uri);
				if (nsptr && nsptr->prefix != NULL) {
					RETURN_STRING((const char *) nsptr->prefix);
				}
			}
		}
	}

	RETURN_NULL();
}

PHP_METHOD(DOMNode, lookupPrefix)
{
	dom_node_lookup_prefix(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Node, lookupPrefix)
{
	dom_node_lookup_prefix(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_node_lookup_prefix */

/* https://dom.spec.whatwg.org/#locate-a-namespace */
static const char *dom_locate_a_namespace(xmlNodePtr node, const zend_string *prefix)
{
	/* switch on the interface node implements: */
	if (node->type == XML_ELEMENT_NODE) {
		if (prefix != NULL) {
			/* 1. If prefix is "xml", then return the XML namespace. */
			if (zend_string_equals_literal_ci(prefix, "xml")) {
				return DOM_XML_NS_URI;
			}

			/* 2. If prefix is "xmlns", then return the XMLNS namespace. */
			if (zend_string_equals_literal_ci(prefix, "xmlns")) {
				return DOM_XMLNS_NS_URI;
			}
		}

		do {
			/* 3. If its namespace is non-null and its namespace prefix is prefix, then return namespace. */
			if (node->ns != NULL && xmlStrEqual(node->ns->prefix, BAD_CAST (prefix ? ZSTR_VAL(prefix) : NULL))) {
				return (const char *) node->ns->href;
			}

			/* 4. If it has an attribute whose namespace is the XMLNS namespace, namespace prefix is "xmlns", and local name is prefix,
			*     or if prefix is null and it has an attribute whose namespace is the XMLNS namespace, namespace prefix is null, and local name is "xmlns",
			*     then return its value if it is not the empty string, and null otherwise. */
			for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
				if (attr->ns == NULL || !xmlStrEqual(attr->ns->href, BAD_CAST DOM_XMLNS_NS_URI)) {
					continue;
				}
				if ((prefix != NULL && xmlStrEqual(attr->ns->prefix, BAD_CAST "xmlns") && xmlStrEqual(attr->name, BAD_CAST ZSTR_VAL(prefix)))
					|| (prefix == NULL && attr->ns->prefix == NULL && xmlStrEqual(attr->name, BAD_CAST "xmlns"))) {
					if (attr->children != NULL && attr->children->content[0] != '\0') {
						return (const char *) attr->children->content;
					} else {
						return NULL;
					}
				}
			}

			/* 5. If its parent element is null, then return null. */
			if (node->parent == NULL || node->parent->type != XML_ELEMENT_NODE) {
				return NULL;
			}

			/* 6. Return the result of running locate a namespace on its parent element using prefix. */
			node = node->parent;
		} while (true);
	} else if (node->type == XML_DOCUMENT_NODE || node->type == XML_HTML_DOCUMENT_NODE) {
		/* 1. If its document element is null, then return null. */
		node = xmlDocGetRootElement((xmlDocPtr) node);
		if (UNEXPECTED(node == NULL)) {
			return NULL;
		}

		/* 2. Return the result of running locate a namespace on its document element using prefix. */
		return dom_locate_a_namespace(node, prefix);
	} else if (node->type == XML_DTD_NODE || node->type == XML_DOCUMENT_FRAG_NODE) {
		return NULL;
	} else {
		/* 1. If its element is null, then return null / If its parent element is null, then return null. */
		if (node->parent == NULL || node->parent->type != XML_ELEMENT_NODE) {
			return NULL;
		}

		/* 2. Return the result of running locate a namespace on its element using prefix. */
		return dom_locate_a_namespace(node->parent, prefix);
	}
}

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-isDefaultNamespace
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-isdefaultnamespace
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, isDefaultNamespace)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	size_t uri_len = 0;
	char *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &uri, &uri_len) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (uri_len > 0) {
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
			if (nodep == NULL) {
				RETURN_FALSE;
			}
		}

		nsptr = xmlSearchNs(nodep->doc, nodep, NULL);
		if (nsptr && xmlStrEqual(nsptr->href, (xmlChar *) uri)) {
			RETURN_TRUE;
		}
	}

	RETURN_FALSE;
}

PHP_METHOD(DOM_Node, isDefaultNamespace)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	size_t uri_len = 0;
	char *uri;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!", &uri, &uri_len) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	if (uri_len == 0) {
		uri = NULL;
	}
	const char *ns_uri = dom_locate_a_namespace(nodep, NULL);
	RETURN_BOOL(xmlStrEqual(BAD_CAST uri, BAD_CAST ns_uri));
}
/* }}} end dom_node_is_default_namespace */

/* {{{ URL: http://www.w3.org/TR/DOM-Level-3-Core/core.html#Node3-lookupNamespaceURI
Modern spec URL: https://dom.spec.whatwg.org/#dom-node-lookupnamespaceuri
Since: DOM Level 3
*/
PHP_METHOD(DOMNode, lookupNamespaceURI)
{
	zval *id;
	xmlNodePtr nodep;
	dom_object *intern;
	xmlNsPtr nsptr;
	zend_string *prefix;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S!", &prefix) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (php_dom_follow_spec_intern(intern)) {
		if (prefix != NULL && ZSTR_LEN(prefix) == 0) {
			prefix = NULL;
		}
		const char *ns_uri = dom_locate_a_namespace(nodep, prefix);
		if (ns_uri == NULL) {
			RETURN_NULL();
		} else {
			RETURN_STRING(ns_uri);
		}
	} else {
		if (nodep->type == XML_DOCUMENT_NODE || nodep->type == XML_HTML_DOCUMENT_NODE) {
			nodep = xmlDocGetRootElement((xmlDocPtr) nodep);
			if (nodep == NULL) {
				RETURN_NULL();
			}
		}

		nsptr = xmlSearchNs(nodep->doc, nodep, BAD_CAST (prefix ? ZSTR_VAL(prefix) : NULL));
		if (nsptr && nsptr->href != NULL) {
			RETURN_STRING((char *) nsptr->href);
		}
	}

	RETURN_NULL();
}
/* }}} end dom_node_lookup_namespace_uri */

static int dom_canonicalize_node_parent_lookup_cb(void *user_data, xmlNodePtr node, xmlNodePtr parent)
{
	xmlNodePtr root = user_data;
	/* We have to unroll the first iteration because node->parent
	 * is not necessarily equal to parent due to libxml2 tree rules (ns decls out of the tree for example). */
	if (node == root) {
		return 1;
	}
	node = parent;
	while (node != NULL) {
		if (node == root) {
			return 1;
		}
		node = node->parent;
	}

	return 0;
}

static void dom_canonicalization(INTERNAL_FUNCTION_PARAMETERS, int mode) /* {{{ */
{
	zval *id;
	zval *xpath_array=NULL, *ns_prefixes=NULL;
	xmlNodePtr nodep;
	xmlDocPtr docp;
	xmlNodeSetPtr nodeset = NULL;
	dom_object *intern;
	bool exclusive=0, with_comments=0;
	xmlChar **inclusive_ns_prefixes = NULL;
	char *file = NULL;
	int ret = -1;
	size_t file_len = 0;
	xmlOutputBufferPtr buf;
	xmlXPathContextPtr ctxp=NULL;
	xmlXPathObjectPtr xpathobjp=NULL;

	id = ZEND_THIS;
	if (mode == 0) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(),
			"|bba!a!", &exclusive, &with_comments,
			&xpath_array, &ns_prefixes) == FAILURE) {
			RETURN_THROWS();
		}
	} else {
		if (zend_parse_parameters(ZEND_NUM_ARGS(),
			"s|bba!a!", &file, &file_len, &exclusive,
			&with_comments, &xpath_array, &ns_prefixes) == FAILURE) {
			RETURN_THROWS();
		}
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	docp = nodep->doc;

	if (! docp) {
		zend_throw_error(NULL, "Node must be associated with a document");
		RETURN_THROWS();
	}

	bool simple_node_parent_lookup_callback = false;
	if (xpath_array == NULL) {
		/* Optimization: if the node is a document, all nodes may be included, no extra filtering or nodeset necessary. */
		if (nodep->type != XML_DOCUMENT_NODE && nodep->type != XML_HTML_DOCUMENT_NODE) {
			simple_node_parent_lookup_callback = true;
		}
	} else {
		/*xpath query from xpath_array */
		HashTable *ht = Z_ARRVAL_P(xpath_array);
		zval *tmp;
		char *xquery;

		/* Find "query" key */
		tmp = zend_hash_find(ht, ZSTR_KNOWN(ZEND_STR_QUERY));
		if (!tmp) {
			/* if mode == 0 then $xpath arg is 3, if mode == 1 then $xpath is 4 */
			zend_argument_value_error(3 + mode, "must have a \"query\" key");
			RETURN_THROWS();
		}
		if (Z_TYPE_P(tmp) != IS_STRING) {
			/* if mode == 0 then $xpath arg is 3, if mode == 1 then $xpath is 4 */
			zend_argument_type_error(3 + mode, "\"query\" option must be a string, %s given", zend_zval_value_name(tmp));
			RETURN_THROWS();
		}
		xquery = Z_STRVAL_P(tmp);

		ctxp = xmlXPathNewContext(docp);
		ctxp->node = nodep;

		tmp = zend_hash_str_find(ht, "namespaces", sizeof("namespaces")-1);
		if (tmp && Z_TYPE_P(tmp) == IS_ARRAY && !HT_IS_PACKED(Z_ARRVAL_P(tmp))) {
			zval *tmpns;
			zend_string *prefix;

			ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(tmp), prefix, tmpns) {
				if (Z_TYPE_P(tmpns) == IS_STRING) {
					if (prefix) {
						xmlXPathRegisterNs(ctxp, (xmlChar *) ZSTR_VAL(prefix), (xmlChar *) Z_STRVAL_P(tmpns));
					}
				}
			} ZEND_HASH_FOREACH_END();
		}

		xpathobjp = xmlXPathEvalExpression((xmlChar *) xquery, ctxp);
		ctxp->node = NULL;
		if (xpathobjp && xpathobjp->type == XPATH_NODESET) {
			nodeset = xpathobjp->nodesetval;
		} else {
			if (xpathobjp) {
				xmlXPathFreeObject(xpathobjp);
			}
			xmlXPathFreeContext(ctxp);
			zend_throw_error(NULL, "XPath query did not return a nodeset");
			RETURN_THROWS();
		}
	}

	if (ns_prefixes != NULL) {
		if (exclusive) {
			zval *tmpns;
			int nscount = 0;

			inclusive_ns_prefixes = safe_emalloc(zend_hash_num_elements(Z_ARRVAL_P(ns_prefixes)) + 1,
				sizeof(xmlChar *), 0);
			ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(ns_prefixes), tmpns) {
				if (Z_TYPE_P(tmpns) == IS_STRING) {
					inclusive_ns_prefixes[nscount++] = (xmlChar *) Z_STRVAL_P(tmpns);
				}
			} ZEND_HASH_FOREACH_END();
			inclusive_ns_prefixes[nscount] = NULL;
		} else {
			php_error_docref(NULL, E_NOTICE,
				"Inclusive namespace prefixes only allowed in exclusive mode.");
		}
	}

	if (mode == 1) {
		buf = xmlOutputBufferCreateFilename(file, NULL, 0);
	} else {
		buf = xmlAllocOutputBuffer(NULL);
	}

	if (buf != NULL) {
		if (simple_node_parent_lookup_callback) {
			ret = xmlC14NExecute(docp, dom_canonicalize_node_parent_lookup_cb, nodep, exclusive, inclusive_ns_prefixes, with_comments, buf);
		} else {
			ret = xmlC14NDocSaveTo(docp, nodeset, exclusive, inclusive_ns_prefixes, with_comments, buf);
		}
	}

	if (inclusive_ns_prefixes != NULL) {
		efree(inclusive_ns_prefixes);
	}
	if (xpathobjp != NULL) {
		xmlXPathFreeObject(xpathobjp);
	}
	if (ctxp != NULL) {
		xmlXPathFreeContext(ctxp);
	}

	if (buf == NULL || ret < 0) {
		RETVAL_FALSE;
	} else {
		if (mode == 0) {
			ret = xmlOutputBufferGetSize(buf);
			if (ret > 0) {
				RETVAL_STRINGL((char *) xmlOutputBufferGetContent(buf), ret);
			} else {
				RETVAL_EMPTY_STRING();
			}
		}
	}

	if (buf) {
		int bytes;

		bytes = xmlOutputBufferClose(buf);
		if (mode == 1 && (ret >= 0)) {
			RETURN_LONG(bytes);
		}
	}
}
/* }}} */

/* {{{ Canonicalize nodes to a string */
PHP_METHOD(DOMNode, C14N)
{
	dom_canonicalization(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}
/* }}} */

/* {{{ Canonicalize nodes to a file */
PHP_METHOD(DOMNode, C14NFile)
{
	dom_canonicalization(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}
/* }}} */

/* {{{ Gets an xpath for a node */
static void dom_node_get_node_path(INTERNAL_FUNCTION_PARAMETERS, bool throw)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *value;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	value = (char *) xmlGetNodePath(nodep);
	if (value == NULL) {
		/* This is only possible when an invalid argument is passed (e.g. namespace declaration), or on allocation failure. */
		if (throw) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
			RETURN_THROWS();
		}
		RETURN_NULL();
	} else {
		RETVAL_STRING(value);
		xmlFree(value);
	}
}

PHP_METHOD(DOMNode, getNodePath)
{
	dom_node_get_node_path(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Node, getNodePath)
{
	dom_node_get_node_path(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} */

/* {{{ Gets line number for a node */
PHP_METHOD(DOMNode, getLineNo)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	RETURN_LONG(xmlGetLineNo(nodep));
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-node-contains
Since:
*/
static void dom_node_contains(INTERNAL_FUNCTION_PARAMETERS, zval *other)
{
	zval *id;
	xmlNodePtr otherp, thisp;
	dom_object *unused_intern;

	ZEND_ASSERT(other != NULL);

	DOM_GET_OBJ(otherp, other, xmlNodePtr, unused_intern);
	DOM_GET_THIS_OBJ(thisp, id, xmlNodePtr, unused_intern);

	do {
		if (otherp == thisp) {
			RETURN_TRUE;
		}
		otherp = otherp->parent;
	} while (otherp);

	RETURN_FALSE;
}

PHP_METHOD(DOMNode, contains)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OR_NULL(other)
	ZEND_PARSE_PARAMETERS_END();

	if (other == NULL) {
		RETURN_FALSE;
	}

	if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(other), dom_node_class_entry) && !instanceof_function(Z_OBJCE_P(other), dom_namespace_node_class_entry))) {
		zend_argument_type_error(1, "must be of type DOMNode|DOMNameSpaceNode|null, %s given", zend_zval_value_name(other));
		RETURN_THROWS();
	}

	dom_node_contains(INTERNAL_FUNCTION_PARAM_PASSTHRU, other);
}

PHP_METHOD(DOM_Node, contains)
{
	zval *other;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS_OR_NULL(other, dom_modern_node_class_entry)
	ZEND_PARSE_PARAMETERS_END();

	if (other == NULL) {
		RETURN_FALSE;
	}

	dom_node_contains(INTERNAL_FUNCTION_PARAM_PASSTHRU, other);
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-node-getrootnode
Since:
*/
PHP_METHOD(DOMNode, getRootNode)
{
	zval *id;
	xmlNodePtr thisp;
	dom_object *intern;
	/* Unused now because we don't support the shadow DOM nodes. Options only influence shadow DOM nodes. */
	zval *options;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|a", &options) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(thisp, id, xmlNodePtr, intern);

	while (thisp->parent) {
		thisp = thisp->parent;
	}

	int ret;
	DOM_RET_OBJ(thisp, &ret, intern);
}
/* }}} */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-node-comparedocumentposition (last check date 2023-07-24)
Since:
*/

#define DOCUMENT_POSITION_DISCONNECTED 0x01
#define DOCUMENT_POSITION_PRECEDING 0x02
#define DOCUMENT_POSITION_FOLLOWING 0x04
#define DOCUMENT_POSITION_CONTAINS 0x08
#define DOCUMENT_POSITION_CONTAINED_BY 0x10
#define DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC 0x20

static void dom_node_compare_document_position(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *node_ce)
{
	zval *id, *node_zval;
	xmlNodePtr other, this;
	dom_object *this_intern, *other_intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node_zval, node_ce) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(this, id, xmlNodePtr, this_intern);
	DOM_GET_OBJ(other, node_zval, xmlNodePtr, other_intern);

	/* Step 1 */
	if (this == other) {
		RETURN_LONG(0);
	}

	/* Step 2 */
	xmlNodePtr node1 = other;
	xmlNodePtr node2 = this;

	/* Step 3 */
	xmlNodePtr attr1 = NULL;
	xmlNodePtr attr2 = NULL;

	/* Step 4 */
	if (node1->type == XML_ATTRIBUTE_NODE) {
		attr1 = node1;
		node1 = attr1->parent;
	}

	/* Step 5 */
	if (node2->type == XML_ATTRIBUTE_NODE) {
		/* 5.1 */
		attr2 = node2;
		node2 = attr2->parent;

		/* 5.2 */
		if (attr1 != NULL && node1 != NULL && node2 == node1) {
			for (const xmlAttr *attr = node2->properties; attr != NULL; attr = attr->next) {
				if (php_dom_is_equal_attr(attr, (const xmlAttr *) attr1)) {
					RETURN_LONG(DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOCUMENT_POSITION_PRECEDING);
				} else if (php_dom_is_equal_attr(attr, (const xmlAttr *) attr2)) {
					RETURN_LONG(DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | DOCUMENT_POSITION_FOLLOWING);
				}
			}
		}
	}

	/* Step 6 */
	/* We first check the first condition,
	 * and as we need the root later anyway we'll cache the root and perform the root check after this if. */
	if (node1 == NULL || node2 == NULL) {
		goto disconnected;
	}
	bool node2_is_ancestor_of_node1 = false;
	size_t node1_depth = 0;
	xmlNodePtr node1_root = node1;
	while (node1_root->parent) {
		node1_root = node1_root->parent;
		if (node1_root == node2) {
			node2_is_ancestor_of_node1 = true;
		}
		node1_depth++;
	}
	bool node1_is_ancestor_of_node2 = false;
	size_t node2_depth = 0;
	xmlNodePtr node2_root = node2;
	while (node2_root->parent) {
		node2_root = node2_root->parent;
		if (node2_root == node1) {
			node1_is_ancestor_of_node2 = true;
		}
		node2_depth++;
	}
	/* Second condition from step 6 */
	if (node1_root != node2_root) {
		goto disconnected;
	}

	/* Step 7 */
	if ((node1_is_ancestor_of_node2 && attr1 == NULL) || (node1 == node2 && attr2 != NULL)) {
		RETURN_LONG(DOCUMENT_POSITION_CONTAINS | DOCUMENT_POSITION_PRECEDING);
	}

	/* Step 8 */
	if ((node2_is_ancestor_of_node1 && attr2 == NULL) || (node1 == node2 && attr1 != NULL)) {
		RETURN_LONG(DOCUMENT_POSITION_CONTAINED_BY | DOCUMENT_POSITION_FOLLOWING);
	}

	/* Special case: comparing children and attributes.
	 * They belong to a different tree and are therefore hard to compare, but spec demands attributes to precede children
	 * according to the pre-order depth-first search ordering.
	 * Because their tree is different, the node parents only meet at the common element instead of earlier.
	 * Therefore, it seems that one is the ancestor of the other. */
	if (node1_is_ancestor_of_node2) {
		ZEND_ASSERT(attr1 != NULL); /* Would've been handled in step 7 otherwise */
		RETURN_LONG(DOCUMENT_POSITION_PRECEDING);
	} else if (node2_is_ancestor_of_node1) {
		ZEND_ASSERT(attr2 != NULL); /* Would've been handled in step 8 otherwise */
		RETURN_LONG(DOCUMENT_POSITION_FOLLOWING);
	}

	/* Step 9 */

	/* We'll use the following strategy (which was already prepared during step 6) to implement this efficiently:
	 * 1. Move nodes upwards such that they are at the same depth.
	 * 2. Then we move both nodes upwards simultaneously until their parents are equal.
	 * 3. If we then move node1 to the next entry repeatedly and we encounter node2,
	 *    then we know node1 precedes node2. Otherwise, node2 must precede node1. */
	/* 1. */
	if (node1_depth > node2_depth) {
		do {
			node1 = node1->parent;
			node1_depth--;
		} while (node1_depth > node2_depth);
	} else if (node2_depth > node1_depth) {
		do {
			node2 = node2->parent;
			node2_depth--;
		} while (node2_depth > node1_depth);
	}
	/* 2. */
	while (node1->parent != node2->parent) {
		node1 = node1->parent;
		node2 = node2->parent;
	}
	/* 3. */
	ZEND_ASSERT(node1 != node2);
	ZEND_ASSERT(node1 != NULL);
	ZEND_ASSERT(node2 != NULL);
	do {
		node1 = node1->next;
		if (node1 == node2) {
			RETURN_LONG(DOCUMENT_POSITION_PRECEDING);
		}
	} while (node1 != NULL);

	/* Step 10 */
	RETURN_LONG(DOCUMENT_POSITION_FOLLOWING);

disconnected:;
	zend_long ordering;
	if (node1 == node2) {
		/* Degenerate case, they're both NULL, but the ordering must be consistent... */
		ZEND_ASSERT(node1 == NULL);
		ordering = other_intern < this_intern ? DOCUMENT_POSITION_PRECEDING : DOCUMENT_POSITION_FOLLOWING;
	} else {
		ordering = node1 < node2 ? DOCUMENT_POSITION_PRECEDING : DOCUMENT_POSITION_FOLLOWING;
	}
	RETURN_LONG(DOCUMENT_POSITION_DISCONNECTED | DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC | ordering);
}

PHP_METHOD(DOMNode, compareDocumentPosition)
{
	dom_node_compare_document_position(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_node_class_entry);
}

PHP_METHOD(DOM_Node, compareDocumentPosition)
{
	dom_node_compare_document_position(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_node_class_entry);
}
/* }}} */

/**
 * We want to block the serialization and unserialization of DOM classes.
 * However, using @not-serializable makes the child classes also not serializable, even if the user implements the methods.
 * So instead, we implement the methods wherein we throw exceptions.
 * The reason we choose these methods is because:
 *   - If the user implements __serialize / __unserialize, the respective throwing methods are not called.
 *   - If the user implements __sleep / __wakeup, then it's also not a problem because they will not enter the throwing methods.
 */

PHP_METHOD(DOM_Node, __construct)
{
	ZEND_UNREACHABLE();
}

PHP_METHOD(DOMNode, __sleep)
{
	zend_throw_exception_ex(NULL, 0, "Serialization of '%s' is not allowed, unless serialization methods are implemented in a subclass", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
	RETURN_THROWS();
}

PHP_METHOD(DOMNode, __wakeup)
{
	zend_throw_exception_ex(NULL, 0, "Unserialization of '%s' is not allowed, unless unserialization methods are implemented in a subclass", ZSTR_VAL(Z_OBJCE_P(ZEND_THIS)->name));
	RETURN_THROWS();
}

#endif
