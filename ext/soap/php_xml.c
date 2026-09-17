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

/* removes all empty text, comments and other insignoficant nodes.
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

xmlDocPtr soap_xmlParseFile(const char *filename)
{
	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr ret;
	bool old_allow_url_fopen;

/*
	xmlInitParser();
*/

	old_allow_url_fopen = PG(allow_url_fopen);
	PG(allow_url_fopen) = 1;
	ctxt = xmlCreateFileParserCtxt(filename);
	PG(allow_url_fopen) = old_allow_url_fopen;
	if (ctxt) {
		bool old;

		php_libxml_sanitize_parse_ctxt_options(ctxt);
		/* TODO: In libxml2 2.14.0 change this to the new options API so we don't rely on deprecated APIs. */
		ZEND_DIAGNOSTIC_IGNORED_START("-Wdeprecated-declarations")
		ctxt->keepBlanks = 0;
		ctxt->options |= XML_PARSE_HUGE;
		ZEND_DIAGNOSTIC_IGNORED_END
		ctxt->sax->ignorableWhitespace = soap_ignorableWhitespace;
		ctxt->sax->comment = soap_Comment;
		ctxt->sax->warning = NULL;
		ctxt->sax->error = NULL;
		/*ctxt->sax->fatalError = NULL;*/
		old = php_libxml_disable_entity_loader(1);
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

/*
	xmlCleanupParser();
*/

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
	xmlParserCtxtPtr ctxt = NULL;
	xmlDocPtr ret;


/*
	xmlInitParser();
*/
	ctxt = xmlCreateMemoryParserCtxt(buf, buf_size);
	if (ctxt) {
		bool old;

		php_libxml_sanitize_parse_ctxt_options(ctxt);
		ctxt->sax->ignorableWhitespace = soap_ignorableWhitespace;
		ctxt->sax->comment = soap_Comment;
		ctxt->sax->warning = NULL;
		ctxt->sax->error = NULL;
		/*ctxt->sax->fatalError = NULL;*/
		/* TODO: In libxml2 2.14.0 change this to the new options API so we don't rely on deprecated APIs. */
		ZEND_DIAGNOSTIC_IGNORED_START("-Wdeprecated-declarations")
		ctxt->options |= XML_PARSE_HUGE;
		ZEND_DIAGNOSTIC_IGNORED_END
		old = php_libxml_disable_entity_loader(1);
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

/*
	xmlCleanupParser();
*/

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

xmlNsPtr attr_find_ns(xmlAttrPtr node)
{
	if (node->ns) {
		return node->ns;
	} else if (node->parent->ns) {
		return node->parent->ns;
	} else {
		return xmlSearchNs(node->doc, node->parent, NULL);
	}
}

xmlNsPtr node_find_ns(xmlNodePtr node)
{
	if (node->ns) {
		return node->ns;
	} else {
		return xmlSearchNs(node->doc, node, NULL);
	}
}

int attr_is_equal_ex(xmlAttrPtr node, char *name, char *ns)
{
	if (name == NULL || ((node->name) && strcmp((char*)node->name, name) == 0)) {
		if (ns) {
			xmlNsPtr nsPtr = attr_find_ns(node);
			if (nsPtr) {
				return (strcmp((char*)nsPtr->href, ns) == 0);
			} else {
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

int node_is_equal_ex(xmlNodePtr node, char *name, char *ns)
{
	if (name == NULL || ((node->name) && strcmp((char*)node->name, name) == 0)) {
		if (ns) {
			xmlNsPtr nsPtr = node_find_ns(node);
			if (nsPtr) {
				return (strcmp((char*)nsPtr->href, ns) == 0);
			} else {
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}


xmlAttrPtr get_attribute_ex(xmlAttrPtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (attr_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node!=NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_recurisve_ex(xmlNodePtr node, char *name, char *ns)
{
	while (node != NULL) {
		if (node_is_equal_ex(node, name, ns)) {
			return node;
		} else if (node->children != NULL) {
			xmlNodePtr tmp = get_node_recurisve_ex(node->children, name, ns);
			if (tmp) {
				return tmp;
			}
		}
		node = node->next;
	}
	return NULL;
}

xmlNodePtr get_node_with_attribute_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
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

xmlNodePtr get_node_with_attribute_recursive_ex(xmlNodePtr node, char *name, char *name_ns, char *attribute, char *value, char *attr_ns)
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
