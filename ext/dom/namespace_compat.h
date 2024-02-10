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

struct dom_ns_magic_token;
typedef struct dom_ns_magic_token dom_ns_magic_token;

struct _dom_libxml_ns_mapper;
typedef struct _dom_libxml_ns_mapper dom_libxml_ns_mapper;

extern const dom_ns_magic_token *dom_ns_is_html_magic_token;
extern const dom_ns_magic_token *dom_ns_is_mathml_magic_token;
extern const dom_ns_magic_token *dom_ns_is_svg_magic_token;
extern const dom_ns_magic_token *dom_ns_is_xlink_magic_token;
extern const dom_ns_magic_token *dom_ns_is_xml_magic_token;
extern const dom_ns_magic_token *dom_ns_is_xmlns_magic_token;

/* These functions make it possible to make a namespace declaration also visible as an attribute by
 * creating an equivalent attribute node. */

typedef struct _php_libxml_private_data_header php_libxml_private_data_header;
struct _php_libxml_private_data_header;

dom_libxml_ns_mapper *dom_libxml_ns_mapper_create(void);
void dom_libxml_ns_mapper_destroy(dom_libxml_ns_mapper *mapper);
xmlNsPtr dom_libxml_ns_mapper_ensure_html_ns(dom_libxml_ns_mapper *mapper);
xmlNsPtr dom_libxml_ns_mapper_ensure_prefixless_xmlns_ns(dom_libxml_ns_mapper *mapper);
xmlNsPtr dom_libxml_ns_mapper_get_ns(dom_libxml_ns_mapper *mapper, zend_string *prefix, zend_string *uri);
xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_prefix_string(dom_libxml_ns_mapper *mapper, const xmlChar *prefix, size_t prefix_len, zend_string *uri);
xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(dom_libxml_ns_mapper *mapper, const char *prefix, const char *uri);
php_libxml_private_data_header *dom_libxml_ns_mapper_header(dom_libxml_ns_mapper *mapper);

void dom_ns_compat_mark_attribute_list(dom_libxml_ns_mapper *mapper, xmlNodePtr node);
bool dom_ns_is_fast(const xmlNode *nodep, const dom_ns_magic_token *magic_token);
bool dom_ns_is_html_and_document_is_html(const xmlNode *nodep);
void dom_libxml_reconcile_modern(dom_libxml_ns_mapper *ns_mapper, xmlNodePtr node);
void php_dom_reconcile_attribute_namespace_after_insertion(xmlAttrPtr attrp);
xmlAttrPtr dom_ns_compat_mark_attribute(dom_libxml_ns_mapper *mapper, xmlNodePtr node, xmlNsPtr ns);

#endif
