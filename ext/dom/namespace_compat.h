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

#ifndef NAMESPACE_COMPAT_H
#define NAMESPACE_COMPAT_H

#include <libxml/tree.h>

/* https://infra.spec.whatwg.org/#namespaces */
#define DOM_XHTML_NS_URI "http://www.w3.org/1999/xhtml"
#define DOM_MATHML_NS_URI "http://www.w3.org/1998/Math/MathML"
#define DOM_SVG_NS_URI "http://www.w3.org/2000/svg"
#define DOM_XLINK_NS_URI "http://www.w3.org/1999/xlink"
#define DOM_XML_NS_URI "http://www.w3.org/XML/1998/namespace"
#define DOM_XMLNS_NS_URI "http://www.w3.org/2000/xmlns/"

/* These functions make it possible to make a namespace declaration also visible as an attribute by
 * creating an equivalent attribute node. */

void dom_ns_compat_mark_attribute_list(xmlNodePtr node);
bool dom_ns_is_html(const xmlNode *nodep);
bool dom_ns_is_html_and_document_is_html(const xmlNode *nodep);

#endif
