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
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "namespace_compat.h"

const dom_ns_magic_token *dom_ns_is_html_magic_token = (const dom_ns_magic_token *) DOM_XHTML_NS_URI;
const dom_ns_magic_token *dom_ns_is_mathml_magic_token = (const dom_ns_magic_token *) DOM_MATHML_NS_URI;
const dom_ns_magic_token *dom_ns_is_svg_magic_token = (const dom_ns_magic_token *) DOM_SVG_NS_URI;
const dom_ns_magic_token *dom_ns_is_xlink_magic_token = (const dom_ns_magic_token *) DOM_XLINK_NS_URI;
const dom_ns_magic_token *dom_ns_is_xml_magic_token = (const dom_ns_magic_token *) DOM_XML_NS_URI;
const dom_ns_magic_token *dom_ns_is_xmlns_magic_token = (const dom_ns_magic_token *) DOM_XMLNS_NS_URI;

static void dom_ns_compat_mark_attribute(xmlNodePtr node, xmlNsPtr ns)
{
	const xmlChar *name, *prefix;
	if (ns->prefix != NULL) {
		prefix = BAD_CAST "xmlns";
		name = ns->prefix;
	} else {
		prefix = NULL;
		name = BAD_CAST "xmlns";
	}

	xmlNsPtr xmlns_ns = dom_ns_create_local_as_is(node->doc, xmlDocGetRootElement(node->doc), node, DOM_XMLNS_NS_URI, prefix);
	xmlSetNsProp(node, xmlns_ns, name, ns->href);
}

void dom_ns_compat_mark_attribute_list(xmlNodePtr node)
{
	xmlNsPtr ns = node->nsDef;
	while (ns != NULL) {
		dom_ns_compat_mark_attribute(node, ns);
		ns = ns->next;
	}
}

bool dom_ns_is_fast(const xmlNode *nodep, const dom_ns_magic_token *magic_token)
{
	ZEND_ASSERT(nodep != NULL);
	xmlNsPtr ns = nodep->ns;
	if (ns != NULL) {
		/* cached for fast checking */
		if (ns->_private == magic_token) {
			return true;
		} else if (ns->_private != NULL) {
			/* Other token stored */
			return false;
		}
		/* Slow path */
		if (xmlStrEqual(ns->href, BAD_CAST magic_token)) {
			ns->_private = (void *) magic_token;
			return true;
		}
	}
	return false;
}

bool dom_ns_is_html_and_document_is_html(const xmlNode *nodep)
{
	ZEND_ASSERT(nodep != NULL);
	return nodep->doc && nodep->doc->type == XML_HTML_DOCUMENT_NODE && dom_ns_is_fast(nodep, dom_ns_is_html_magic_token);
}

static xmlNsPtr dom_ns_create_local_as_is_unchecked(xmlDocPtr doc, xmlNodePtr parent, const char *href, xmlChar *prefix)
{
	xmlNsPtr ns = xmlMalloc(sizeof(xmlNs));
	if (UNEXPECTED(ns == NULL)) {
		return NULL;
	}
	memset(ns, 0, sizeof(xmlNs));
	ns->type = XML_LOCAL_NAMESPACE;
	if (href != NULL) {
		ns->href = xmlStrdup(BAD_CAST href);
		if (UNEXPECTED(ns->href == NULL)) {
			xmlFreeNs(ns);
			return NULL;
		}
	}
	if (prefix != NULL) {
		ns->prefix = xmlStrdup(BAD_CAST prefix);
		if (UNEXPECTED(ns->prefix == NULL)) {
			xmlFreeNs(ns);
			return NULL;
		}
	}

	if (parent != NULL) {
		/* Order doesn't matter for spec-compliant mode, insert at front for fast insertion. */
		ns->next = parent->nsDef;
		parent->nsDef = ns;
	} else {
		ZEND_ASSERT(doc != NULL);
		php_libxml_set_old_ns(doc, ns);
	}

	return ns;
}

xmlNsPtr dom_ns_create_local_as_is(xmlDocPtr doc, xmlNodePtr ns_holder, xmlNodePtr parent, const char *uri, const xmlChar *prefix)
{
	if (uri == NULL || uri[0] == '\0') {
		return NULL;
	}

	if (parent != NULL) {
		xmlNsPtr existing = xmlSearchNs(doc, parent, BAD_CAST prefix);
		if (existing != NULL && xmlStrEqual(existing->href, BAD_CAST uri)) {
			return existing;
		}
	}

	return dom_ns_create_local_as_is_unchecked(doc, ns_holder, uri, BAD_CAST prefix);
}

xmlNsPtr dom_ns_fast_get_html_ns(xmlDocPtr docp)
{
	xmlNsPtr nsptr = NULL;
	xmlNodePtr root = xmlDocGetRootElement(docp);

	if (root != NULL) {
		/* Fast path: if the root element itself is in the HTML namespace, return its namespace.
		 * This is the most common case. */
		if (dom_ns_is_fast(root, dom_ns_is_html_magic_token)) {
			return root->ns;
		}

		/* Otherwise, we search all the namespaces that the root declares to avoid creating duplicates. */
		nsptr = xmlSearchNsByHref(docp, root, BAD_CAST DOM_XHTML_NS_URI);
		if (nsptr == NULL) {
			nsptr = dom_ns_create_local_as_is_unchecked(docp, root, DOM_XHTML_NS_URI, NULL);
			if (UNEXPECTED(nsptr == NULL)) {
				/* Only happens on OOM. */
				php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
				return NULL;
			}
		}
		nsptr->_private = (void *) dom_ns_is_html_magic_token;
	} else {
		/* If there is no root we fall back to the old namespace list. */
		if (docp->oldNs != NULL) {
			xmlNsPtr tmp = docp->oldNs;
			do {
				if (tmp->_private == dom_ns_is_html_magic_token || xmlStrEqual(tmp->href, BAD_CAST DOM_XHTML_NS_URI)) {
					nsptr = tmp;
					nsptr->_private = (void *) dom_ns_is_html_magic_token;
					break;
				}
				tmp = tmp->next;
			} while (tmp != NULL);
		}

		/* If there was no old namespace list, or the HTML namespace was not in there, create a new one. */
		if (nsptr == NULL) {
			nsptr = dom_ns_create_local_as_is_unchecked(docp, NULL, DOM_XHTML_NS_URI, NULL);
			if (UNEXPECTED(nsptr == NULL)) {
				/* Only happens on OOM. */
				php_dom_throw_error(INVALID_STATE_ERR, /* strict */ true);
				return NULL;
			}
			nsptr->_private = (void *) dom_ns_is_html_magic_token;
		}
	}

	return nsptr;
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
