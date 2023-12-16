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

#define DOM_NS_IS_HTML_MAGIC ((void *) 1)

static void dom_ns_compat_mark_attribute(xmlNodePtr node, xmlNsPtr ns)
{
    xmlChar *name;
    if (ns->prefix) {
        /* Can't overflow in practice because it would require the prefix to occupy almost the entire address space. */
        size_t prefix_len = strlen((char *) ns->prefix);
        size_t len = strlen("xmlns:") + prefix_len;
        name = xmlMalloc(len + 1);
        if (UNEXPECTED(name == NULL)) {
            return;
        }
        memcpy(name, "xmlns:", strlen("xmlns:"));
        memcpy(name + strlen("xmlns:"), ns->prefix, prefix_len + 1 /* including '\0' */);
    } else {
        name = xmlStrdup(BAD_CAST "xmlns");
        if (UNEXPECTED(name == NULL)) {
            return;
        }
    }

    xmlSetNsProp(node, NULL, name, ns->href);
    xmlFree(name);
}

void dom_ns_compat_mark_attribute_list(xmlNodePtr node)
{
    xmlNsPtr ns = node->nsDef;
    while (ns != NULL) {
        dom_ns_compat_mark_attribute(node, ns);
        ns = ns->next;
    }
}

// TODO: extend this to other namespaces too?
bool dom_ns_is_html(const xmlNode *nodep)
{
	ZEND_ASSERT(nodep != NULL);
	xmlNsPtr ns = nodep->ns;
	if (ns != NULL) {
		/* cached for fast checking */
		if (ns->_private == DOM_NS_IS_HTML_MAGIC) {
			return true;
		}
		if (xmlStrEqual(ns->href, BAD_CAST "http://www.w3.org/1999/xhtml")) {
			ns->_private = DOM_NS_IS_HTML_MAGIC;
			return true;
		}
	}
	return false;
}

bool dom_ns_is_html_and_document_is_html(const xmlNode *nodep)
{
    ZEND_ASSERT(nodep != NULL);
    return nodep->doc && nodep->doc->type == XML_HTML_DOCUMENT_NODE && dom_ns_is_html(nodep);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
