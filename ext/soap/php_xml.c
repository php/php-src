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
  | Authors: Brad Lafountain <rodif_bl@yahoo.com>                        |
  |          Shane Caraveo <shane@caraveo.com>                           |
  |          Dmitry Stogov <dmitry@php.net>                              |
  +----------------------------------------------------------------------+
*/

#include "php_soap.h"
#include "ext/libxml/php_libxml.h"
#include "libxml/parser.h"
#include "libxml/parserInternals.h"

/* Channel libxml file io layer through the PHP streams subsystem.
 * This allows use of ftps:// and https:// urls */

static bool is_blank(const xmlChar* str)
{
	while (*str != '\0') {
		if (*str != ' '  && *str != 0x9 && *str != 0xa && *str != 0xd) {
			return false;
		}
		str++;
	}
	return true;
}

/* removes all empty text, comments and other insignificant nodes.
 * Iterative because recursion overflows the stack on a deep document. */
static void cleanup_xml_node(xmlNodePtr node)
{
	xmlNodePtr parent = node;
	xmlNodePtr trav = node->children;

	while (trav != NULL) {
		xmlNodePtr next = trav->next;

		if (trav->type == XML_TEXT_NODE) {
			if (is_blank(trav->content)) {
				xmlUnlinkNode(trav);
				xmlFreeNode(trav);
			}
		} else if ((trav->type != XML_ELEMENT_NODE) &&
		           (trav->type != XML_CDATA_SECTION_NODE)) {
			xmlUnlinkNode(trav);
			xmlFreeNode(trav);
		} else if (trav->children != NULL) {
			parent = trav;
			trav = trav->children;
			continue;
		}

		while (next == NULL) {
			if (parent == node) {
				return;
			}
			next = parent->next;
			parent = parent->parent;
		}
		trav = next;
	}
}

#if LIBXML_VERSION < 21300
static int is_nesting_too_deep(xmlNodePtr node)
{
	xmlNodePtr trav = node->children;
	unsigned int depth = 0;

	while (trav != NULL) {
		/* An entity reference borrows its declaration as child list, and that
		 * declaration hangs off the DTD, so descending leaves the document. */
		if (trav->children != NULL &&
		    trav->type != XML_ENTITY_REF_NODE &&
		    trav->type != XML_DTD_NODE) {
			if (++depth > SOAP_MAX_XML_DEPTH) {
				return TRUE;
			}
			trav = trav->children;
			continue;
		}
		while (trav->next == NULL) {
			trav = trav->parent;
			if (trav == node) {
				return FALSE;
			}
			depth--;
		}
		trav = trav->next;
	}
	return FALSE;
}
#endif

static void soap_ignorableWhitespace(void *ctx, const xmlChar *ch, int len)
{
}

static void soap_Comment(void *ctx, const xmlChar *value)
{
}

/* Consumes `ctxt` */
static xmlDocPtr soap_xmlParse_ex(xmlParserCtxtPtr ctxt)
{
	xmlDocPtr ret;
	if (ctxt) {
#if LIBXML_VERSION >= 21300
		xmlCtxtSetOptions(ctxt, XML_PARSE_HUGE | XML_PARSE_NO_XXE | XML_PARSE_NONET | XML_PARSE_NOBLANKS);
#else
		php_libxml_sanitize_parse_ctxt_options(ctxt);
		ZEND_DIAGNOSTIC_IGNORED_START("-Wdeprecated-declarations")
		ctxt->keepBlanks = 0;
		ctxt->options |= XML_PARSE_HUGE;
		ZEND_DIAGNOSTIC_IGNORED_END
#endif
		ctxt->sax->ignorableWhitespace = soap_ignorableWhitespace;
		ctxt->sax->comment = soap_Comment;
		ctxt->sax->warning = NULL;
		ctxt->sax->error = NULL;
		/*ctxt->sax->fatalError = NULL;*/
		bool old = php_libxml_disable_entity_loader(true);
		xmlParseDocument(ctxt);
		php_libxml_disable_entity_loader(old);
		if (ctxt->wellFormed) {
			ret = ctxt->myDoc;
			if (ret->URL == NULL && ctxt->directory != NULL) {
				ret->URL = xmlCharStrdup(ctxt->directory);
			}
		} else {
			ret = NULL;
			xmlFreeDoc(ctxt->myDoc);
			ctxt->myDoc = NULL;
		}
		xmlFreeParserCtxt(ctxt);
	} else {
		ret = NULL;
	}
	return ret;
}

xmlDocPtr soap_xmlParseFile(const char *filename)
{
	bool old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = true;
	xmlParserCtxtPtr ctxt = xmlCreateFileParserCtxt(filename);
	PG(allow_url_fopen) = old_allow_url_fopen;

	xmlDocPtr ret = soap_xmlParse_ex(ctxt);

	if (ret) {
#if LIBXML_VERSION < 21300
		if (is_nesting_too_deep((xmlNodePtr)ret)) {
			/* php_sdl.c reports xmlGetLastError() as the reason, and libxml2 did
			 * not fail here, so drop the error an earlier parse left behind. */
			xmlResetLastError();
			xmlFreeDoc(ret);
			return NULL;
		}
#endif
		cleanup_xml_node((xmlNodePtr)ret);
	}
	return ret;
}

xmlDocPtr soap_xmlParseMemory(const void *buf, size_t buf_size)
{
	xmlParserCtxtPtr ctxt = xmlCreateMemoryParserCtxt(buf, buf_size);
	xmlDocPtr ret = soap_xmlParse_ex(ctxt);

#if LIBXML_VERSION < 21300
	if (ret && is_nesting_too_deep((xmlNodePtr)ret)) {
		xmlFreeDoc(ret);
		ret = NULL;
	}
#endif

/*
	if (ret) {
		cleanup_xml_node((xmlNodePtr)ret);
	}
*/
	return ret;
}

xmlNsPtr node_find_ns(xmlNodePtr node)
{
	if (node->ns) {
		return node->ns;
	} else {
		return xmlSearchNs(node->doc, node, NULL);
	}
}

bool attr_is_equal_ex(xmlAttrPtr node, const char *name, const char *ns)
{
	if (node->name && strcmp((const char *) node->name, name) == 0) {
		xmlNsPtr nsPtr = node->ns;
		if (ns) {
			if (nsPtr) {
				return (strcmp((const char *) nsPtr->href, ns) == 0);
			} else {
				return false;
			}
		} else if (nsPtr) {
			return false;
		}
		return true;
	}
	return false;
}

bool node_is_equal_ex(xmlNodePtr node, const char *name, const char *ns)
{
	if (name == NULL || ((node->name) && strcmp((char*)node->name, name) == 0)) {
		if (ns) {
			xmlNsPtr nsPtr = node_find_ns(node);
			if (nsPtr) {
				return strcmp((const char *) nsPtr->href, ns) == 0;
			} else {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool node_is_equal_ex_one_of(xmlNodePtr node, const char *name, const char *const *namespaces)
{
	if ((node->name) && strcmp((char*)node->name, name) == 0) {
		xmlNsPtr nsPtr = node_find_ns(node);
		if (nsPtr) {
			do {
				if (strcmp((const char *) nsPtr->href, *namespaces) == 0) {
					return true;
				}
				namespaces++;
			} while (*namespaces != NULL);
		}
		return false;
	}
	return false;
}

xmlAttrPtr get_attribute_any_ns(xmlAttrPtr node, const char *name)
{
	while (node!=NULL) {
		if (node->name && strcmp((const char *) node->name, name) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

/* Finds an attribute by name and namespace.
 * If ns is NULL, the attribute must not be in any namespace.
 * If ns is not NULL, the attribute must be in the specified namespace.
 */
xmlAttrPtr get_attribute_ex(xmlAttrPtr node, const char *name, const char *ns)
{
	while (node != NULL) {
		if (attr_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_ex(xmlNodePtr node, const char *name, const char *ns)
{
	while (node!=NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, const char *name, const char *name_ns, const char *attribute, const char *value, const char *attr_ns)
{
	xmlAttrPtr attr;

	while (node != NULL) {
		if (name != NULL) {
			node = get_node_ex(node, name, name_ns);
			if (node==NULL) {
				return NULL;
			}
		}

		attr = get_attribute_ex(node->properties, attribute, attr_ns);
		if (attr != NULL && strcmp((char*)attr->children->content, value) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, const char *name, const char *name_ns, const char *attribute, const char *value, const char *attr_ns)
{
	unsigned int depth = 0;

	while (node != NULL) {
		if (node_is_equal_ex(node, name, name_ns)) {
			xmlAttrPtr attr = get_attribute_ex(node->properties, attribute, attr_ns);
			if (attr != NULL && strcmp((char*)attr->children->content, value) == 0) {
				return node;
			}
		}
		if (node->children != NULL &&
		    node->type != XML_ENTITY_REF_NODE &&
		    node->type != XML_DTD_NODE) {
			node = node->children;
			depth++;
			continue;
		}
		while (node->next == NULL) {
			if (depth == 0) {
				return NULL;
			}
			node = node->parent;
			depth--;
		}
		node = node->next;
	}
	return NULL;
}

/* namespace is either a copy or NULL, value is never NULL and never a copy. */
void parse_namespace(const xmlChar *inval, const char **value, char **namespace)
{
	const char *found = strrchr((const char *) inval, ':');

	if (found != NULL && found != (const char *) inval) {
		(*namespace) = estrndup((const char *) inval, found - (const char *) inval);
		(*value) = ++found;
	} else {
		(*value) = (const char *) inval;
		(*namespace) = NULL;
	}
}
