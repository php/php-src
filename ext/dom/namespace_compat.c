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

bool dom_ns_is_also_an_attribute(const xmlNs *ns) {
    return ns->_private != NULL;
}

void dom_ns_compat_mark_attribute(xmlNsPtr ns) {
    ns->_private = (void *) 1;
}

void dom_ns_compat_mark_attribute_list(xmlNsPtr ns) {
    while (ns != NULL) {
        dom_ns_compat_mark_attribute(ns);
        ns = ns->next;
    }
}

void dom_ns_compat_copy_attribute_list_mark(xmlNsPtr copy, const xmlNs *original) {
    /* It's possible that the original list is shorter than the copy list
     * because of additional namespace copies from within a fragment. */
    while (original != NULL) {
        ZEND_ASSERT(copy != NULL);
        if (dom_ns_is_also_an_attribute(original)) {
            dom_ns_compat_mark_attribute(copy);
        }
        copy = copy->next;
        original = original->next;
    }
}

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
