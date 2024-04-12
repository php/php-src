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
#include "dom_properties.h"

/*
* class DOMElement extends DOMNode
*
* URL: https://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-745549614
* Since:
*/

/* {{{ */
PHP_METHOD(DOMElement, __construct)
{
	xmlNodePtr nodep = NULL, oldnode = NULL;
	dom_object *intern;
	char *name, *value = NULL, *uri = NULL;
	char *localname = NULL, *prefix = NULL;
	int errorcode = 0;
	size_t name_len, value_len = 0, uri_len = 0;
	int name_valid;
	xmlNsPtr nsptr = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|s!s", &name, &name_len, &value, &value_len, &uri, &uri_len) == FAILURE) {
		RETURN_THROWS();
	}

	name_valid = xmlValidateName(BAD_CAST name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	/* Namespace logic is separate and only when uri passed in to insure no BC breakage */
	if (uri_len > 0) {
		errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);
		if (errorcode == 0) {
			nodep = xmlNewNode (NULL, BAD_CAST localname);
			if (nodep != NULL && uri != NULL) {
				nsptr = dom_get_ns(nodep, uri, &errorcode, prefix);
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
			php_dom_throw_error(errorcode, true);
			RETURN_THROWS();
		}
	} else {
	    /* If you don't pass a namespace uri, then you can't set a prefix */
	    localname = (char *) xmlSplitQName2(BAD_CAST name, (xmlChar **) &prefix);
	    if (prefix != NULL) {
			xmlFree(localname);
			xmlFree(prefix);
	        php_dom_throw_error(NAMESPACE_ERR, true);
	        RETURN_THROWS();
	    }
		nodep = xmlNewNode(NULL, BAD_CAST name);
	}

	if (!nodep) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		RETURN_THROWS();
	}

	if (value_len > 0) {
		xmlNodeSetContentLen(nodep, BAD_CAST value, value_len);
	}

	intern = Z_DOMOBJ_P(ZEND_THIS);
	oldnode = dom_object_get_node(intern);
	if (oldnode != NULL) {
		php_libxml_node_decrement_resource((php_libxml_node_object *)intern);
	}
	php_libxml_increment_node_ptr((php_libxml_node_object *)intern, nodep, (void *)intern);
}
/* }}} end DOMElement::__construct */

/* {{{ tagName	string
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-104682815
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-tagname
Since:
*/
zend_result dom_element_tag_name_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	bool uppercase = php_dom_follow_spec_intern(obj) && php_dom_ns_is_html_and_document_is_html(nodep);

	zend_string *result = dom_node_get_node_name_attribute_or_element((const xmlNode *) nodep, uppercase);
	ZVAL_NEW_STR(retval, result);

	return SUCCESS;
}

/* }}} */

static zend_result dom_element_reflected_attribute_read(dom_object *obj, zval *retval, const char *name)
{
	DOM_PROP_NODE(xmlNodePtr, nodep, obj);

	xmlChar *content = xmlGetNoNsProp(nodep, (const xmlChar *) name);
	if (content == NULL) {
		ZVAL_EMPTY_STRING(retval);
		return SUCCESS;
	}

	ZVAL_STRING(retval, (const char *) content);
	xmlFree(content);

	return SUCCESS;
}

static xmlAttrPtr dom_element_reflected_attribute_write(dom_object *obj, zval *newval, const char *name)
{
	xmlNode *nodep = dom_object_get_node(obj);

	if (nodep == NULL) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		return NULL;
	}

	/* Typed property, so it is a string already */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING);
	return xmlSetNsProp(nodep, NULL, (const xmlChar *) name, (const xmlChar *) Z_STRVAL_P(newval));
}

/* {{{ className	string
URL: https://dom.spec.whatwg.org/#dom-element-classname
Since:
*/
zend_result dom_element_class_name_read(dom_object *obj, zval *retval)
{
	return dom_element_reflected_attribute_read(obj, retval, "class");
}

zend_result dom_element_class_name_write(dom_object *obj, zval *newval)
{
	if (dom_element_reflected_attribute_write(obj, newval, "class")) {
		return SUCCESS;
	}
	return FAILURE;
}
/* }}} */

/* {{{ id	string
URL: https://dom.spec.whatwg.org/#dom-element-id
Since:
*/
zend_result dom_element_id_read(dom_object *obj, zval *retval)
{
	return dom_element_reflected_attribute_read(obj, retval, "id");
}

static void php_set_attribute_id(xmlAttrPtr attrp, bool is_id);

zend_result dom_element_id_write(dom_object *obj, zval *newval)
{
	xmlAttrPtr attr = dom_element_reflected_attribute_write(obj, newval, "id");
	if (!attr) {
		return FAILURE;
	}
	php_set_attribute_id(attr, true);
	return SUCCESS;
}
/* }}} */

/* {{{ schemaTypeInfo	typeinfo
readonly=yes
URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#Element-schemaTypeInfo
Since: DOM Level 3
*/
zend_result dom_element_schema_type_info_read(dom_object *obj, zval *retval)
{
	ZVAL_NULL(retval);
	return SUCCESS;
}

/* }}} */

/* Note: the object returned is not necessarily a node, but can be an attribute or a namespace declaration. */
static xmlNodePtr dom_get_attribute_or_nsdecl(dom_object *intern, xmlNodePtr elem, const xmlChar *name, size_t name_len) /* {{{ */
{
	if (!php_dom_follow_spec_intern(intern)) {
		int len;
		const xmlChar *nqname = xmlSplitQName3(name, &len);

		if (nqname != NULL) {
			xmlNsPtr ns;
			if (strncmp((const char *) name, "xmlns:", len + 1) == 0) {
				ns = elem->nsDef;
				while (ns) {
					if (xmlStrEqual(ns->prefix, nqname)) {
						break;
					}
					ns = ns->next;
				}
				return (xmlNodePtr)ns;
			}
			xmlChar *prefix = xmlStrndup(name, len);
			ns = xmlSearchNs(elem->doc, elem, prefix);
			if (prefix != NULL) {
				xmlFree(prefix);
			}
			if (ns != NULL) {
				return (xmlNodePtr)xmlHasNsProp(elem, nqname, ns->href);
			}
		} else {
			if (xmlStrEqual(name, BAD_CAST "xmlns")) {
				xmlNsPtr nsPtr = elem->nsDef;
				while (nsPtr) {
					if (nsPtr->prefix == NULL) {
						return (xmlNodePtr)nsPtr;
					}
					nsPtr = nsPtr->next;
				}
				return NULL;
			}
		}
		return (xmlNodePtr) xmlHasNsProp(elem, name, NULL);
	} else {
		return (xmlNodePtr) php_dom_get_attribute_node(elem, name, name_len);
	}
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-666EE0F9
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-getattribute
Since:
*/
PHP_METHOD(DOMElement, getAttribute)
{
	zval *id;
	xmlNode *nodep;
	char *name;
	xmlChar *value = NULL;
	dom_object *intern;
	xmlNodePtr attr;
	size_t name_len;
	bool should_free = false;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attr = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
	if (attr) {
		switch (attr->type) {
			case XML_ATTRIBUTE_NODE:
				value = xmlNodeListGetString(attr->doc, attr->children, 1);
				should_free = true;
				break;
			case XML_NAMESPACE_DECL:
				value = BAD_CAST ((xmlNsPtr)attr)->href;
				should_free = false;
				break;
			default:
				value = BAD_CAST ((xmlAttributePtr)attr)->defaultValue;
				should_free = false;
		}
	}

	if (value == NULL) {
		if (php_dom_follow_spec_intern(intern)) {
			RETURN_NULL();
		}
		RETURN_EMPTY_STRING();
	} else {
		RETVAL_STRING((char *)value);
		if (should_free) {
			xmlFree(value);
		}
	}
}
/* }}} end dom_element_get_attribute */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-element-getattributenames
Since:
*/
PHP_METHOD(DOMElement, getAttributeNames)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	zval tmp;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(nodep, id, xmlNodePtr, intern);

	array_init(return_value);
	HashTable *ht = Z_ARRVAL_P(return_value);
	zend_hash_real_init_packed(ht);

	if (!php_dom_follow_spec_intern(intern)) {
		for (xmlNsPtr nsptr = nodep->nsDef; nsptr; nsptr = nsptr->next) {
			const char *prefix = (const char *) nsptr->prefix;
			ZVAL_NEW_STR(&tmp, dom_node_concatenated_name_helper(strlen(prefix), prefix, strlen("xmlns"), (const char *) "xmlns"));
			zend_hash_next_index_insert(ht, &tmp);
		}
	}

	for (xmlAttrPtr attr = nodep->properties; attr; attr = attr->next) {
		ZVAL_NEW_STR(&tmp, dom_node_get_node_name_attribute_or_element((const xmlNode *) attr, false));
		zend_hash_next_index_insert(ht, &tmp);
	}
}
/* }}} end DOMElement::getAttributeNames() */

static xmlNodePtr dom_create_attribute(xmlNodePtr nodep, const char *name, const char* value)
{
	if (xmlStrEqual(BAD_CAST name, BAD_CAST "xmlns")) {
		return (xmlNodePtr) xmlNewNs(nodep, BAD_CAST value, NULL);
	} else {
		return (xmlNodePtr) xmlSetProp(nodep, BAD_CAST name, BAD_CAST value);
	}
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-F68F082
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-setattribute
Since:
*/
PHP_METHOD(DOMElement, setAttribute)
{
	zval *id;
	xmlNode *nodep;
	xmlNodePtr attr = NULL;
	int name_valid;
	size_t name_len, value_len;
	dom_object *intern;
	char *name, *value;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &name, &name_len, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (name_len == 0) {
		zend_argument_value_error(1, "cannot be empty");
		RETURN_THROWS();
	}

	name_valid = xmlValidateName(BAD_CAST name, 0);
	if (name_valid != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	if (php_dom_follow_spec_intern(intern)) {
		xmlChar *name_processed = BAD_CAST name;
		if (php_dom_ns_is_html_and_document_is_html(nodep)) {
			char *lowercase_copy = zend_str_tolower_dup_ex(name, name_len);
			if (lowercase_copy != NULL) {
				name_processed = BAD_CAST lowercase_copy;
			}
		}

		/* Can't use xmlSetNsProp unconditionally here because that doesn't take into account the qualified name matching... */
		attr = (xmlNodePtr) php_dom_get_attribute_node(nodep, BAD_CAST name, name_len);
		if (attr != NULL) {
			dom_remove_all_children(attr);
			xmlNodePtr node = xmlNewDocText(attr->doc, BAD_CAST value);
			xmlAddChild(attr, node);
		} else {
			attr = (xmlNodePtr) xmlSetNsProp(nodep, NULL, name_processed, BAD_CAST value);
		}

		if (name_processed != BAD_CAST name) {
			efree(name_processed);
		}
	} else {
		attr = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
		if (attr != NULL) {
			switch (attr->type) {
				case XML_ATTRIBUTE_NODE:
					node_list_unlink(attr->children);
					break;
				case XML_NAMESPACE_DECL:
					RETURN_FALSE;
				EMPTY_SWITCH_DEFAULT_CASE();
			}
		}

		attr = dom_create_attribute(nodep, name, value);
		if (!attr) {
			zend_argument_value_error(1, "must be a valid XML attribute");
			RETURN_THROWS();
		}
		if (attr->type == XML_NAMESPACE_DECL) {
			RETURN_TRUE;
		}

		DOM_RET_OBJ(attr, intern);
	}
}
/* }}} end dom_element_set_attribute */

typedef struct _dom_deep_ns_redef_item {
	xmlNodePtr current_node;
	xmlNsPtr defined_ns;
} dom_deep_ns_redef_item;

/* Reconciliation for a *single* namespace, but reconciles *closest* to the subtree needing it. */
static void dom_deep_ns_redef(xmlNodePtr node, xmlNsPtr ns_to_redefine)
{
	size_t worklist_capacity = 128;
	dom_deep_ns_redef_item *worklist = emalloc(sizeof(dom_deep_ns_redef_item) * worklist_capacity);
	worklist[0].current_node = node;
	worklist[0].defined_ns = NULL;
	size_t worklist_size = 1;

	while (worklist_size > 0) {
		worklist_size--;
		dom_deep_ns_redef_item *current_worklist_item = &worklist[worklist_size];
		ZEND_ASSERT(current_worklist_item->current_node->type == XML_ELEMENT_NODE);
		xmlNsPtr defined_ns = current_worklist_item->defined_ns;

		if (current_worklist_item->current_node->ns == ns_to_redefine) {
			if (defined_ns == NULL) {
				defined_ns = xmlNewNs(current_worklist_item->current_node, ns_to_redefine->href, ns_to_redefine->prefix);
			}
			current_worklist_item->current_node->ns = defined_ns;
		}

		for (xmlAttrPtr attr = current_worklist_item->current_node->properties; attr; attr = attr->next) {
			if (attr->ns == ns_to_redefine) {
				if (defined_ns == NULL) {
					defined_ns = xmlNewNs(current_worklist_item->current_node, ns_to_redefine->href, ns_to_redefine->prefix);
				}
				attr->ns = defined_ns;
			}
		}

		for (xmlNodePtr child = current_worklist_item->current_node->children; child; child = child->next) {
			if (child->type != XML_ELEMENT_NODE) {
				continue;
			}
			if (worklist_size == worklist_capacity) {
				if (UNEXPECTED(worklist_capacity >= SIZE_MAX / 3 * 2 / sizeof(dom_deep_ns_redef_item))) {
					/* Shouldn't be possible to hit, but checked for safety anyway */
					goto out;
				}
				worklist_capacity = worklist_capacity * 3 / 2;
				worklist = erealloc(worklist, sizeof(dom_deep_ns_redef_item) * worklist_capacity);
			}
			worklist[worklist_size].current_node = child;
			worklist[worklist_size].defined_ns = defined_ns;
			worklist_size++;
		}
	}

out:
	efree(worklist);
}

static bool dom_remove_attribute(xmlNodePtr thisp, xmlNodePtr attrp)
{
	ZEND_ASSERT(thisp != NULL);
	ZEND_ASSERT(attrp != NULL);

	switch (attrp->type) {
		case XML_ATTRIBUTE_NODE:
			if (php_dom_object_get_data(attrp) == NULL) {
				node_list_unlink(attrp->children);
				xmlUnlinkNode(attrp);
				xmlFreeProp((xmlAttrPtr)attrp);
			} else {
				xmlUnlinkNode(attrp);
			}
			break;
		case XML_NAMESPACE_DECL: {
			/* They will always be removed, but can be re-added.
			 *
			 * If any reference was left to the namespace, the only effect is that
			 * the definition is potentially moved closer to the element using it.
			 * If no reference was left, it is actually removed. */
			xmlNsPtr ns = (xmlNsPtr) attrp;
			if (thisp->nsDef == ns) {
				thisp->nsDef = ns->next;
			} else if (thisp->nsDef != NULL) {
				xmlNsPtr prev = thisp->nsDef;
				xmlNsPtr cur = prev->next;
				while (cur) {
					if (cur == ns) {
						prev->next = cur->next;
						break;
					}
					prev = cur;
					cur = cur->next;
				}
			} else {
				/* defensive: attrp not defined in thisp ??? */
#if ZEND_DEBUG
				ZEND_UNREACHABLE();
#endif
				break; /* defensive */
			}

			ns->next = NULL;
			php_libxml_set_old_ns(thisp->doc, ns); /* note: can't deallocate as it might be referenced by a "fake namespace node" */
			/* xmlReconciliateNs() redefines at the top of the tree instead of closest to the child, own reconciliation here.
			 * Similarly, the DOM version has other issues too (see dom_libxml_reconcile_ensure_namespaces_are_declared). */
			dom_deep_ns_redef(thisp, ns);

			break;
		}
		EMPTY_SWITCH_DEFAULT_CASE();
	}
	return true;
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-6D6AC0F9
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-removeattribute
Since:
*/
PHP_METHOD(DOMElement, removeAttribute)
{
	xmlNodePtr nodep, attrp;
	dom_object *intern;
	size_t name_len;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, ZEND_THIS, xmlNodePtr, intern);

	attrp = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
	if (attrp == NULL) {
		RETURN_FALSE;
	}

	RETURN_BOOL(dom_remove_attribute(nodep, attrp));
}

PHP_METHOD(DOM_Element, removeAttribute)
{
	xmlNodePtr nodep, attrp;
	dom_object *intern;
	size_t name_len;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, ZEND_THIS, xmlNodePtr, intern);

	attrp = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
	if (attrp != NULL) {
		dom_remove_attribute(nodep, attrp);
	}
}
/* }}} end dom_element_remove_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-217A91B8
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-getattributenode
Since:
*/
PHP_METHOD(DOMElement, getAttributeNode)
{
	zval *id;
	xmlNodePtr nodep, attrp;
	size_t name_len;
	dom_object *intern;
	char *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attrp = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
	if (attrp == NULL) {
		if (php_dom_follow_spec_intern(intern)) {
			RETURN_NULL();
		}
		RETURN_FALSE;
	}

	if (attrp->type == XML_NAMESPACE_DECL) {
		xmlNsPtr original = (xmlNsPtr) attrp;
		/* Keep parent alive, because we're a fake child. */
		GC_ADDREF(&intern->std);
		(void) php_dom_create_fake_namespace_decl(nodep, original, return_value, intern);
	} else {
		DOM_RET_OBJ((xmlNodePtr) attrp, intern);
	}
}
/* }}} end dom_element_get_attribute_node */

static void dom_element_set_attribute_node_common(INTERNAL_FUNCTION_PARAMETERS, bool use_ns, bool modern)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlNs *nsp;
	xmlAttr *attrp, *existattrp = NULL;
	dom_object *intern, *attrobj, *oldobj;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, dom_get_node_ce(modern)) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	/* ZPP Guarantees that a DOMAttr class is given, as it is converted to a xmlAttr
	 * to pass to libxml (see http://www.xmlsoft.org/html/libxml-tree.html#xmlAttr)
	 * if it is not of type XML_ATTRIBUTE_NODE it indicates a bug somewhere */
	ZEND_ASSERT(attrp->type == XML_ATTRIBUTE_NODE);

	if (modern) {
		if (attrp->parent != NULL && attrp->parent != nodep) {
			php_dom_throw_error(INUSE_ATTRIBUTE_ERR, /* strict */ true);
			RETURN_THROWS();
		}
		if (attrp->doc != NULL && attrp->doc != nodep->doc) {
			php_dom_adopt_node((xmlNodePtr) attrp, intern, nodep->doc);
		}
	} else {
		if (!(attrp->doc == NULL || attrp->doc == nodep->doc)) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
			RETURN_FALSE;
		}
	}

	nsp = attrp->ns;
	if (use_ns && nsp != NULL) {
		existattrp = xmlHasNsProp(nodep, attrp->name, nsp->href);
	} else {
		existattrp = xmlHasProp(nodep, attrp->name);
	}

	if (existattrp != NULL && existattrp->type != XML_ATTRIBUTE_DECL) {
		if ((oldobj = php_dom_object_get_data((xmlNodePtr) existattrp)) != NULL &&
			((php_libxml_node_ptr *)oldobj->ptr)->node == (xmlNodePtr) attrp)
		{
			RETURN_NULL();
		}
		xmlUnlinkNode((xmlNodePtr) existattrp);
	}

	if (attrp->parent != NULL) {
		xmlUnlinkNode((xmlNodePtr) attrp);
	}

	if (attrp->doc == NULL && nodep->doc != NULL) {
		attrobj->document = intern->document;
		php_libxml_increment_doc_ref((php_libxml_node_object *)attrobj, NULL);
	}

	xmlAddChild(nodep, (xmlNodePtr) attrp);
	if (!modern) {
		php_dom_reconcile_attribute_namespace_after_insertion(attrp);
	}

	/* Returns old property if removed otherwise NULL */
	if (existattrp != NULL) {
		DOM_RET_OBJ((xmlNodePtr) existattrp, intern);
	} else {
		RETURN_NULL();
	}
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-887236154
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-setattributenode
Since:
*/
PHP_METHOD(DOMElement, setAttributeNode)
{
	dom_element_set_attribute_node_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, /* use_ns */ false, /* modern */ false);
}
/* }}} end dom_element_set_attribute_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-D589198
Since:
*/
static void dom_element_remove_attribute_node(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *node_ce)
{
	zval *node;
	xmlNode *nodep;
	xmlAttr *attrp;
	dom_object *intern, *attrobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &node, node_ce) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, ZEND_THIS, xmlNodePtr, intern);

	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	ZEND_ASSERT(attrp->type == XML_ATTRIBUTE_NODE);

	if (attrp->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
		RETURN_FALSE;
	}

	xmlUnlinkNode((xmlNodePtr) attrp);

	DOM_RET_OBJ((xmlNodePtr) attrp, intern);
}

PHP_METHOD(DOMElement, removeAttributeNode)
{
	dom_element_remove_attribute_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_node_class_entry);
}

PHP_METHOD(DOM_Element, removeAttributeNode)
{
	dom_element_remove_attribute_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_node_class_entry);
}
/* }}} end dom_element_remove_attribute_node */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-1938918D
Modern spec URL: https://dom.spec.whatwg.org/#concept-getelementsbytagname
Since:
*/
static void dom_element_get_elements_by_tag_name(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	size_t name_len;
	dom_object *intern, *namednode;
	char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	if (modern) {
		php_dom_create_iterator(return_value, DOM_HTMLCOLLECTION, true);
	} else {
		php_dom_create_iterator(return_value, DOM_NODELIST, false);
	}
	namednode = Z_DOMOBJ_P(return_value);
	dom_namednode_iter(intern, 0, namednode, NULL, name, name_len, NULL, 0);
}

PHP_METHOD(DOMElement, getElementsByTagName)
{
	dom_element_get_elements_by_tag_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Element, getElementsByTagName)
{
	dom_element_get_elements_by_tag_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_element_get_elements_by_tag_name */

/* should_free_result must be initialized to false */
static const xmlChar *dom_get_attribute_ns(dom_object *intern, xmlNodePtr elemp, const char *uri, size_t uri_len, const char *name, bool *should_free_result)
{
	bool follow_spec = php_dom_follow_spec_intern(intern);
	if (follow_spec && uri_len == 0) {
		uri = NULL;
	}

	xmlChar *strattr = xmlGetNsProp(elemp, BAD_CAST name, BAD_CAST uri);

	if (strattr != NULL) {
		*should_free_result = true;
		return strattr;
	} else {
		if (!follow_spec && xmlStrEqual(BAD_CAST uri, BAD_CAST DOM_XMLNS_NS_URI)) {
			xmlNsPtr nsptr = dom_get_nsdecl(elemp, BAD_CAST name);
			if (nsptr != NULL) {
				return nsptr->href;
			} else {
				return NULL;
			}
		} else {
			return NULL;
		}
	}
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAttrNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-getattributens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, getAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	dom_object *intern;
	size_t uri_len = 0, name_len = 0;
	char *uri, *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	bool should_free_result = false;
	const xmlChar *result = dom_get_attribute_ns(intern, elemp, uri, uri_len, name, &should_free_result);
	if (result == NULL) {
		if (php_dom_follow_spec_intern(intern)) {
			RETURN_NULL();
		}
		RETURN_EMPTY_STRING();
	} else {
		RETVAL_STRING((const char *) result);
		if (should_free_result) {
			xmlFree(BAD_CAST result);
		}
	}
}
/* }}} end dom_element_get_attribute_ns */

static void dom_set_attribute_ns_legacy(dom_object *intern, xmlNodePtr elemp, char *uri, size_t uri_len, char *name, size_t name_len, const char *value)
{
	if (name_len == 0) {
		zend_argument_value_error(2, "cannot be empty");
		return;
	}

	xmlNodePtr nodep = NULL;
	xmlNsPtr nsptr;
	xmlAttr *attr;
	char *localname = NULL, *prefix = NULL;
	int is_xmlns = 0, name_valid;
	bool stricterror = dom_get_strict_error(intern->document);

	int errorcode = dom_check_qname(name, &localname, &prefix, uri_len, name_len);

	if (errorcode == 0) {
		if (uri_len > 0) {
			nodep = (xmlNodePtr) xmlHasNsProp(elemp, BAD_CAST localname, BAD_CAST uri);
			if (nodep != NULL && nodep->type != XML_ATTRIBUTE_DECL) {
				node_list_unlink(nodep->children);
			}

			if ((xmlStrEqual(BAD_CAST prefix, BAD_CAST "xmlns") ||
				(prefix == NULL && xmlStrEqual(BAD_CAST localname, BAD_CAST "xmlns"))) &&
				xmlStrEqual(BAD_CAST uri, BAD_CAST DOM_XMLNS_NS_URI)) {
				is_xmlns = 1;
				if (prefix == NULL) {
					nsptr = dom_get_nsdecl(elemp, NULL);
				} else {
					nsptr = dom_get_nsdecl(elemp, BAD_CAST localname);
				}
			} else {
				nsptr = xmlSearchNsByHref(elemp->doc, elemp, BAD_CAST uri);
				if (nsptr && nsptr->prefix == NULL) {
					xmlNsPtr tmpnsptr;

					tmpnsptr = nsptr->next;
					while (tmpnsptr) {
						if ((tmpnsptr->prefix != NULL) && (tmpnsptr->href != NULL) &&
							(xmlStrEqual(tmpnsptr->href, BAD_CAST uri))) {
							nsptr = tmpnsptr;
							break;
						}
						tmpnsptr = tmpnsptr->next;
					}
					if (tmpnsptr == NULL) {
						nsptr = dom_get_ns_resolve_prefix_conflict(elemp, (const char *) nsptr->href);
					}
				}
			}

			if (nsptr == NULL) {
				if (is_xmlns == 1) {
					xmlNewNs(elemp, BAD_CAST value, prefix == NULL ? NULL : BAD_CAST localname);
				} else {
					nsptr = dom_get_ns(elemp, uri, &errorcode, prefix);
				}
				xmlReconciliateNs(elemp->doc, elemp);
			} else {
				if (is_xmlns == 1) {
					if (nsptr->href) {
						xmlFree(BAD_CAST nsptr->href);
					}
					nsptr->href = xmlStrdup(BAD_CAST value);
				}
			}

			if (errorcode == 0 && is_xmlns == 0) {
				xmlSetNsProp(elemp, nsptr, BAD_CAST localname, BAD_CAST value);
			}
		} else {
			name_valid = xmlValidateName(BAD_CAST localname, 0);
			if (name_valid != 0) {
				errorcode = INVALID_CHARACTER_ERR;
				stricterror = 1;
			} else {
				attr = xmlHasProp(elemp, BAD_CAST localname);
				if (attr != NULL && attr->type != XML_ATTRIBUTE_DECL) {
					node_list_unlink(attr->children);
				}
				xmlSetProp(elemp, BAD_CAST localname, BAD_CAST value);
			}
		}
	}

	xmlFree(localname);
	if (prefix != NULL) {
		xmlFree(prefix);
	}

	if (errorcode != 0) {
		php_dom_throw_error(errorcode, stricterror);
	}
}

/* https://dom.spec.whatwg.org/#dom-element-setattributens */
static void dom_set_attribute_ns_modern(dom_object *intern, xmlNodePtr elemp, zend_string *uri, const zend_string *name, const char *value)
{
	xmlChar *localname = NULL, *prefix = NULL;
	int errorcode = dom_validate_and_extract(uri, name, &localname, &prefix);

	if (errorcode == 0) {
		php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(intern);
		xmlNsPtr ns = php_dom_libxml_ns_mapper_get_ns_raw_prefix_string(ns_mapper, prefix, xmlStrlen(prefix), uri);
		if (UNEXPECTED(xmlSetNsProp(elemp, ns, localname, BAD_CAST value) == NULL)) {
			php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
		}
	} else {
		php_dom_throw_error(errorcode, /* strict */ true);
	}

	xmlFree(localname);
	xmlFree(prefix);
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAttrNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-setattributens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, setAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	size_t value_len = 0;
	char *value;
	zend_string *uri;
	zend_string *name = NULL;
	dom_object *intern;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S!Ss", &uri, &name, &value, &value_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	if (php_dom_follow_spec_intern(intern)) {
		dom_set_attribute_ns_modern(intern, elemp, uri, name, value);
	} else {
		dom_set_attribute_ns_legacy(intern, elemp, uri ? ZSTR_VAL(uri) : NULL, uri ? ZSTR_LEN(uri) : 0, ZSTR_VAL(name), ZSTR_LEN(name), value);
	}
}
/* }}} end dom_element_set_attribute_ns */

static void dom_remove_eliminated_ns_single_element(xmlNodePtr node, xmlNsPtr eliminatedNs)
{
	ZEND_ASSERT(node->type == XML_ELEMENT_NODE);
	if (node->ns == eliminatedNs) {
		node->ns = NULL;
	}

	for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		if (attr->ns == eliminatedNs) {
			attr->ns = NULL;
		}
	}
}

static void dom_remove_eliminated_ns(xmlNodePtr node, xmlNsPtr eliminatedNs)
{
	dom_remove_eliminated_ns_single_element(node, eliminatedNs);

	xmlNodePtr base = node;
	node = node->children;
	while (node != NULL) {
		ZEND_ASSERT(node != base);

		if (node->type == XML_ELEMENT_NODE) {
			dom_remove_eliminated_ns_single_element(node, eliminatedNs);

			if (node->children) {
				node = node->children;
				continue;
			}
		}

		node = php_dom_next_in_tree_order(node, base);
	}
}

static void dom_eliminate_ns(xmlNodePtr nodep, xmlNsPtr nsptr)
{
	if (nsptr->href != NULL) {
		xmlFree((char *) nsptr->href);
		nsptr->href = NULL;
	}
	if (nsptr->prefix != NULL) {
		xmlFree((char *) nsptr->prefix);
		nsptr->prefix = NULL;
	}

	/* Remove it from the list and move it to the old ns list */
	xmlNsPtr current_ns = nodep->nsDef;
	if (current_ns == nsptr) {
		nodep->nsDef = nsptr->next;
	} else {
		do {
			if (current_ns->next == nsptr) {
				current_ns->next = nsptr->next;
				break;
			}
			current_ns = current_ns->next;
		} while (current_ns != NULL);
	}
	nsptr->next = NULL;
	php_libxml_set_old_ns(nodep->doc, nsptr);

	dom_remove_eliminated_ns(nodep, nsptr);
}

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElRemAtNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-removeattributens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, removeAttributeNS)
{
	zval *id;
	xmlNode *nodep;
	xmlAttr *attrp;
	xmlNsPtr nsptr;
	dom_object *intern;
	size_t name_len, uri_len;
	char *name, *uri;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	bool follow_spec = php_dom_follow_spec_intern(intern);
	if (follow_spec && uri_len == 0) {
		uri = NULL;
	}

	attrp = xmlHasNsProp(nodep, BAD_CAST name, BAD_CAST uri);

	if (!follow_spec) {
		nsptr = dom_get_nsdecl(nodep, BAD_CAST name);
		if (nsptr != NULL) {
			if (xmlStrEqual(BAD_CAST uri, nsptr->href)) {
				dom_eliminate_ns(nodep, nsptr);
			} else {
				return;
			}
		}
	}

	if (attrp && attrp->type != XML_ATTRIBUTE_DECL) {
		if (php_dom_object_get_data((xmlNodePtr) attrp) == NULL) {
			node_list_unlink(attrp->children);
			xmlUnlinkNode((xmlNodePtr) attrp);
			xmlFreeProp(attrp);
		} else {
			xmlUnlinkNode((xmlNodePtr) attrp);
		}
	}
}
/* }}} end dom_element_remove_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElGetAtNodeNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-getattributenodens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, getAttributeNodeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlAttrPtr attrp;
	dom_object *intern;
	size_t uri_len, name_len;
	char *uri, *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	bool follow_spec = php_dom_follow_spec_intern(intern);
	if (follow_spec && uri_len == 0) {
		uri = NULL;
	}

	attrp = xmlHasNsProp(elemp, BAD_CAST name, BAD_CAST uri);

	if (attrp == NULL) {
		if (!follow_spec && xmlStrEqual(BAD_CAST uri, BAD_CAST DOM_XMLNS_NS_URI)) {
			xmlNsPtr nsptr;
			nsptr = dom_get_nsdecl(elemp, BAD_CAST name);
			if (nsptr != NULL) {
				/* Keep parent alive, because we're a fake child. */
				GC_ADDREF(&intern->std);
				(void) php_dom_create_fake_namespace_decl(elemp, nsptr, return_value, intern);
			} else {
				RETURN_NULL();
			}
		} else {
			RETURN_NULL();
		}
	} else {
		DOM_RET_OBJ((xmlNodePtr) attrp, intern);
	}

}
/* }}} end dom_element_get_attribute_node_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetAtNodeNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-setattributenodens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, setAttributeNodeNS)
{
	dom_element_set_attribute_node_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, /* use_ns */ true, /* modern */ false);
}

PHP_METHOD(DOM_Element, setAttributeNodeNS)
{
	dom_element_set_attribute_node_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, /* use_ns */ true, /* modern */ true);
}
/* }}} end dom_element_set_attribute_node_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-A6C90942
Modern spec URL: https://dom.spec.whatwg.org/#concept-getelementsbytagnamens
Since: DOM Level 2
*/
static void dom_element_get_elements_by_tag_name_ns(INTERNAL_FUNCTION_PARAMETERS, bool modern)
{
	size_t uri_len, name_len;
	dom_object *intern, *namednode;
	char *uri, *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	if (modern) {
		php_dom_create_iterator(return_value, DOM_HTMLCOLLECTION, true);
	} else {
		php_dom_create_iterator(return_value, DOM_NODELIST, false);
	}
	namednode = Z_DOMOBJ_P(return_value);
	dom_namednode_iter(intern, 0, namednode, NULL, name, name_len, uri ? uri : "", uri_len);
}

PHP_METHOD(DOMElement, getElementsByTagNameNS)
{
	dom_element_get_elements_by_tag_name_ns(INTERNAL_FUNCTION_PARAM_PASSTHRU, false);
}

PHP_METHOD(DOM_Element, getElementsByTagNameNS)
{
	dom_element_get_elements_by_tag_name_ns(INTERNAL_FUNCTION_PARAM_PASSTHRU, true);
}
/* }}} end dom_element_get_elements_by_tag_name_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttr
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-hasattribute
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, hasAttribute)
{
	zval *id;
	xmlNode *nodep;
	dom_object *intern;
	char *name;
	size_t name_len;
	xmlNodePtr attr;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attr = dom_get_attribute_or_nsdecl(intern, nodep, BAD_CAST name, name_len);
	if (attr == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}
/* }}} end dom_element_has_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElHasAttrNS
Modern spec URL: https://dom.spec.whatwg.org/#dom-element-hasattributens
Since: DOM Level 2
*/
PHP_METHOD(DOMElement, hasAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	dom_object *intern;
	size_t uri_len, name_len;
	char *uri, *name;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s!s", &uri, &uri_len, &name, &name_len) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	bool should_free_result = false;
	const xmlChar *result = dom_get_attribute_ns(intern, elemp, uri, uri_len, name, &should_free_result);
	if (result == NULL) {
		RETURN_FALSE;
	} else {
		if (should_free_result) {
			xmlFree(BAD_CAST result);
		}
		RETURN_TRUE;
	}
}
/* }}} end dom_element_has_attribute_ns */

static void php_set_attribute_id(xmlAttrPtr attrp, bool is_id) /* {{{ */
{
	if (is_id == 1 && attrp->atype != XML_ATTRIBUTE_ID) {
		xmlChar *id_val;

		id_val = xmlNodeListGetString(attrp->doc, attrp->children, 1);
		if (id_val != NULL) {
			xmlAddID(NULL, attrp->doc, id_val, attrp);
			xmlFree(id_val);
		}
	} else if (is_id == 0 && attrp->atype == XML_ATTRIBUTE_ID) {
		xmlRemoveID(attrp->doc, attrp);
		attrp->atype = 0;
	}
}
/* }}} */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttr
Since: DOM Level 3
*/
PHP_METHOD(DOMElement, setIdAttribute)
{
	zval *id;
	xmlNode *nodep;
	xmlAttrPtr attrp;
	dom_object *intern;
	char *name;
	size_t name_len;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "sb", &name, &name_len, &is_id) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);

	attrp = xmlHasNsProp(nodep, BAD_CAST name, NULL);
	if (attrp == NULL || attrp->type == XML_ATTRIBUTE_DECL) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_id_attribute */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNS
Since: DOM Level 3
*/
PHP_METHOD(DOMElement, setIdAttributeNS)
{
	zval *id;
	xmlNodePtr elemp;
	xmlAttrPtr attrp;
	dom_object *intern;
	size_t uri_len, name_len;
	char *uri, *name;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "ssb", &uri, &uri_len, &name, &name_len, &is_id) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(elemp, id, xmlNodePtr, intern);

	attrp = xmlHasNsProp(elemp, BAD_CAST name, BAD_CAST uri);
	if (attrp == NULL || attrp->type == XML_ATTRIBUTE_DECL) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}
/* }}} end dom_element_set_id_attribute_ns */

/* {{{ URL: http://www.w3.org/TR/2003/WD-DOM-Level-3-Core-20030226/DOM3-Core.html#core-ID-ElSetIdAttrNode
Since: DOM Level 3
*/
static void dom_element_set_id_attribute_node(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *attr_ce)
{
	zval *id, *node;
	xmlNode *nodep;
	xmlAttrPtr attrp;
	dom_object *intern, *attrobj;
	bool is_id;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "Ob", &node, attr_ce, &is_id) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(nodep, id, xmlNodePtr, intern);
	DOM_GET_OBJ(attrp, node, xmlAttrPtr, attrobj);

	if (attrp->parent != nodep) {
		php_dom_throw_error(NOT_FOUND_ERR, dom_get_strict_error(intern->document));
	} else {
		php_set_attribute_id(attrp, is_id);
	}

	RETURN_NULL();
}

PHP_METHOD(DOMElement, setIdAttributeNode)
{
	dom_element_set_id_attribute_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_attr_class_entry);
}

PHP_METHOD(DOM_Element, setIdAttributeNode)
{
	dom_element_set_id_attribute_node(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_attr_class_entry);
}
/* }}} end dom_element_set_id_attribute_node */

/* {{{ URL:
Since:
*/
PHP_METHOD(DOMElement, remove)
{
	dom_object *intern;

	if (zend_parse_parameters_none() == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_child_node_remove(intern);
}
/* }}} end DOMElement::remove */

PHP_METHOD(DOMElement, after)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_after(intern, args, argc);
}

PHP_METHOD(DOMElement, before)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_before(intern, args, argc);
}

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-append
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, append)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_append(intern, args, argc);
}
/* }}} end DOMElement::append */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-prepend
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, prepend)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_prepend(intern, args, argc);
}
/* }}} end DOMElement::prepend */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-replacechildren
Since: DOM Living Standard (DOM4)
*/
PHP_METHOD(DOMElement, replaceWith)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_child_replace_with(intern, args, argc);
}
/* }}} end DOMElement::prepend */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-parentnode-replacechildren
Since:
*/
PHP_METHOD(DOMElement, replaceChildren)
{
	uint32_t argc = 0;
	zval *args;
	dom_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "*", &args, &argc) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_INTERN(intern);

	dom_parent_node_replace_children(intern, args, argc);
}
/* }}} */

#define INSERT_ADJACENT_RES_ADOPT_FAILED ((void*) -1)
#define INSERT_ADJACENT_RES_SYNTAX_FAILED INSERT_ADJACENT_RES_ADOPT_FAILED
#define INSERT_ADJACENT_RES_PRE_INSERT_FAILED ((void*) -2)

static xmlNodePtr dom_insert_adjacent(const zend_string *where, xmlNodePtr thisp, dom_object *this_intern, xmlNodePtr otherp)
{
	if (zend_string_equals_literal_ci(where, "beforebegin")) {
		if (thisp->parent == NULL) {
			return NULL;
		}
		if (!php_dom_adopt_node(otherp, this_intern, thisp->doc)) {
			return INSERT_ADJACENT_RES_ADOPT_FAILED;
		}
		if (!php_dom_pre_insert(this_intern->document, otherp, thisp->parent, thisp)) {
			return INSERT_ADJACENT_RES_PRE_INSERT_FAILED;
		}
	} else if (zend_string_equals_literal_ci(where, "afterbegin")) {
		if (!php_dom_adopt_node(otherp, this_intern, thisp->doc)) {
			return INSERT_ADJACENT_RES_ADOPT_FAILED;
		}
		if (!php_dom_pre_insert(this_intern->document, otherp, thisp, thisp->children)) {
			return INSERT_ADJACENT_RES_PRE_INSERT_FAILED;
		}
	} else if (zend_string_equals_literal_ci(where, "beforeend")) {
		if (!php_dom_adopt_node(otherp, this_intern, thisp->doc)) {
			return INSERT_ADJACENT_RES_ADOPT_FAILED;
		}
		if (!php_dom_pre_insert(this_intern->document, otherp, thisp, NULL)) {
			return INSERT_ADJACENT_RES_PRE_INSERT_FAILED;
		}
	} else if (zend_string_equals_literal_ci(where, "afterend")) {
		if (thisp->parent == NULL) {
			return NULL;
		}
		if (!php_dom_adopt_node(otherp, this_intern, thisp->doc)) {
			return INSERT_ADJACENT_RES_ADOPT_FAILED;
		}
		if (!php_dom_pre_insert(this_intern->document, otherp, thisp->parent, thisp->next))  {
			return INSERT_ADJACENT_RES_PRE_INSERT_FAILED;
		}
	} else {
		php_dom_throw_error(SYNTAX_ERR, dom_get_strict_error(this_intern->document));
		return INSERT_ADJACENT_RES_SYNTAX_FAILED;
	}
	return otherp;
}

/* {{{ URL: https://dom.spec.whatwg.org/#dom-element-insertadjacentelement
Since:
*/
static void dom_element_insert_adjacent_element(INTERNAL_FUNCTION_PARAMETERS, zend_class_entry *element_ce)
{
	zend_string *where;
	zval *element_zval, *id;
	xmlNodePtr thisp, otherp;
	dom_object *this_intern, *other_intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SO", &where, &element_zval, element_ce) != SUCCESS) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(thisp, id, xmlNodePtr, this_intern);
	DOM_GET_OBJ(otherp, element_zval, xmlNodePtr, other_intern);

	xmlNodePtr result = dom_insert_adjacent(where, thisp, this_intern, otherp);
	if (result == NULL) {
		RETURN_NULL();
	} else if (result != INSERT_ADJACENT_RES_ADOPT_FAILED && result != INSERT_ADJACENT_RES_PRE_INSERT_FAILED) {
		DOM_RET_OBJ(otherp, other_intern);
	} else {
		RETURN_THROWS();
	}
}

PHP_METHOD(DOMElement, insertAdjacentElement)
{
	dom_element_insert_adjacent_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_element_class_entry);
}

PHP_METHOD(DOM_Element, insertAdjacentElement)
{
	dom_element_insert_adjacent_element(INTERNAL_FUNCTION_PARAM_PASSTHRU, dom_modern_element_class_entry);
}
/* }}} end DOMElement::insertAdjacentElement */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-element-insertadjacenttext
Since:
*/
PHP_METHOD(DOMElement, insertAdjacentText)
{
	zend_string *where, *data;
	dom_object *this_intern;
	zval *id;
	xmlNodePtr thisp;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "SS", &where, &data) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(thisp, id, xmlNodePtr, this_intern);

	if (UNEXPECTED(ZEND_SIZE_T_INT_OVFL(ZSTR_LEN(data)))) {
		zend_argument_value_error(2, "is too long");
		RETURN_THROWS();
	}

	xmlNodePtr otherp = xmlNewDocTextLen(thisp->doc, (const xmlChar *) ZSTR_VAL(data), ZSTR_LEN(data));
	xmlNodePtr result = dom_insert_adjacent(where, thisp, this_intern, otherp);
	if (result == NULL || result == INSERT_ADJACENT_RES_ADOPT_FAILED) {
		xmlFreeNode(otherp);
	}
}
/* }}} end DOMElement::insertAdjacentText */

/* {{{ URL: https://dom.spec.whatwg.org/#dom-element-toggleattribute
Since:
*/
PHP_METHOD(DOMElement, toggleAttribute)
{
	char *qname, *qname_tmp = NULL;
	size_t qname_length;
	bool force, force_is_null = true;
	xmlNodePtr thisp;
	zval *id;
	dom_object *intern;
	bool retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|b!", &qname, &qname_length, &force, &force_is_null) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_THIS_OBJ(thisp, id, xmlNodePtr, intern);

	/* Step 1 */
	if (xmlValidateName(BAD_CAST qname, 0) != 0) {
		php_dom_throw_error(INVALID_CHARACTER_ERR, true);
		RETURN_THROWS();
	}

	bool follow_spec = php_dom_follow_spec_intern(intern);

	/* Step 2 */
	if (thisp->doc != NULL && thisp->doc->type == XML_HTML_DOCUMENT_NODE
		&& ((!follow_spec && thisp->ns == NULL) || (thisp->ns != NULL && xmlStrEqual(thisp->ns->href, BAD_CAST DOM_XHTML_NS_URI)))) {
		qname_tmp = zend_str_tolower_dup_ex(qname, qname_length);
		if (qname_tmp != NULL) {
			qname = qname_tmp;
		}
	}

	/* Step 3 */
	xmlNodePtr attribute = dom_get_attribute_or_nsdecl(intern, thisp, BAD_CAST qname, qname_length);

	/* Step 4 */
	if (attribute == NULL) {
		/* Step 4.1 */
		if (force_is_null || force) {
			if (follow_spec) {
				xmlSetNsProp(thisp, NULL, BAD_CAST qname, NULL);
			} else {
				/* The behaviour for namespaces isn't defined by spec, but this is based on observing browers behaviour.
				* It follows the same rules when you'd manually add an attribute using the other APIs. */
				int len;
				const xmlChar *split = xmlSplitQName3((const xmlChar *) qname, &len);
				if (split == NULL || strncmp(qname, "xmlns:", len + 1 /* +1 for matching ':' too */) != 0) {
					/* unqualified name, or qualified name with no xml namespace declaration */
					dom_create_attribute(thisp, qname, "");
				} else {
					/* qualified name with xml namespace declaration */
					xmlNewNs(thisp, (const xmlChar *) "", (const xmlChar *) (qname + len + 1));
				}
			}
			retval = true;
			goto out;
		}
		/* Step 4.2 */
		retval = false;
		goto out;
	}

	/* Step 5 */
	if (force_is_null || !force) {
		dom_remove_attribute(thisp, attribute);
		retval = false;
		goto out;
	}

	/* Step 6 */
	retval = true;

out:
	if (qname_tmp) {
		efree(qname_tmp);
	}
	RETURN_BOOL(retval);
}
/* }}} end DOMElement::prepend */

#endif
