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

#include "xml_common.h"

/* https://infra.spec.whatwg.org/#namespaces */
#define DOM_XHTML_NS_URI "http://www.w3.org/1999/xhtml"
#define DOM_MATHML_NS_URI "http://www.w3.org/1998/Math/MathML"
#define DOM_SVG_NS_URI "http://www.w3.org/2000/svg"
#define DOM_XLINK_NS_URI "http://www.w3.org/1999/xlink"
#define DOM_XML_NS_URI "http://www.w3.org/XML/1998/namespace"
#define DOM_XMLNS_NS_URI "http://www.w3.org/2000/xmlns/"

struct php_dom_ns_magic_token;
typedef struct php_dom_ns_magic_token php_dom_ns_magic_token;

struct _php_dom_libxml_ns_mapper;
typedef struct _php_dom_libxml_ns_mapper php_dom_libxml_ns_mapper;

PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_html_magic_token;
PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_mathml_magic_token;
PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_svg_magic_token;
PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_xlink_magic_token;
PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_xml_magic_token;
PHP_DOM_EXPORT extern const php_dom_ns_magic_token *php_dom_ns_is_xmlns_magic_token;

typedef struct _php_libxml_private_data_header php_libxml_private_data_header;
struct _php_libxml_private_data_header;

/* These functions make it possible to make a namespace declaration also visible as an attribute by
 * creating an equivalent attribute node. */

PHP_DOM_EXPORT php_dom_libxml_ns_mapper *php_dom_libxml_ns_mapper_create(void);
PHP_DOM_EXPORT void php_dom_libxml_ns_mapper_destroy(php_dom_libxml_ns_mapper *mapper);
PHP_DOM_EXPORT xmlNsPtr php_dom_libxml_ns_mapper_ensure_html_ns(php_dom_libxml_ns_mapper *mapper);
PHP_DOM_EXPORT xmlNsPtr php_dom_libxml_ns_mapper_ensure_prefixless_xmlns_ns(php_dom_libxml_ns_mapper *mapper);
PHP_DOM_EXPORT xmlNsPtr php_dom_libxml_ns_mapper_get_ns(php_dom_libxml_ns_mapper *mapper, zend_string *prefix, zend_string *uri);
PHP_DOM_EXPORT xmlNsPtr php_dom_libxml_ns_mapper_get_ns_raw_prefix_string(php_dom_libxml_ns_mapper *mapper, const xmlChar *prefix, size_t prefix_len, zend_string *uri);
PHP_DOM_EXPORT xmlNsPtr php_dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(php_dom_libxml_ns_mapper *mapper, const char *prefix, const char *uri);

PHP_DOM_EXPORT php_libxml_private_data_header *php_dom_libxml_ns_mapper_header(php_dom_libxml_ns_mapper *mapper);
PHP_DOM_EXPORT void php_dom_ns_compat_mark_attribute_list(php_dom_libxml_ns_mapper *mapper, xmlNodePtr node);
PHP_DOM_EXPORT void php_dom_libxml_reconcile_modern(php_dom_libxml_ns_mapper *ns_mapper, xmlNodePtr node);
PHP_DOM_EXPORT void php_dom_reconcile_attribute_namespace_after_insertion(xmlAttrPtr attrp);
PHP_DOM_EXPORT xmlAttrPtr php_dom_ns_compat_mark_attribute(php_dom_libxml_ns_mapper *mapper, xmlNodePtr node, xmlNsPtr ns);

PHP_DOM_EXPORT bool php_dom_ns_is_fast(const xmlNode *nodep, const php_dom_ns_magic_token *magic_token);
PHP_DOM_EXPORT bool php_dom_ns_is_fast_ex(xmlNsPtr ns, const php_dom_ns_magic_token *magic_token);
PHP_DOM_EXPORT bool php_dom_ns_is_html_and_document_is_html(const xmlNode *nodep);

typedef struct _php_dom_in_scope_ns {
   xmlNsPtr *list;
   size_t count;
   bool origin_is_ns_compat;
} php_dom_in_scope_ns;

PHP_DOM_EXPORT php_dom_in_scope_ns php_dom_get_in_scope_ns(php_dom_libxml_ns_mapper *ns_mapper, const xmlNode *node);
PHP_DOM_EXPORT php_dom_in_scope_ns php_dom_get_in_scope_ns_legacy(const xmlNode *node);
PHP_DOM_EXPORT void php_dom_in_scope_ns_destroy(php_dom_in_scope_ns *in_scope_ns);

#endif
