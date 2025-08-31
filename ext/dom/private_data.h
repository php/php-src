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

#ifndef PRIVATE_DATA_H
#define PRIVATE_DATA_H

#include "xml_common.h"

struct php_dom_libxml_ns_mapper {
	/* This is used almost all the time for HTML documents, so it makes sense to cache this. */
	xmlNsPtr html_ns;
	/* Used for every prefixless namespace declaration in XML, so also very common. */
	xmlNsPtr prefixless_xmlns_ns;
	HashTable uri_to_prefix_map;
};

typedef struct php_dom_private_data {
	php_libxml_private_data_header header;
	struct php_dom_libxml_ns_mapper ns_mapper;
	HashTable *template_fragments;
} php_dom_private_data;

typedef struct php_libxml_private_data_header php_libxml_private_data_header;
struct php_libxml_private_data_header;

struct php_dom_private_data;
typedef struct php_dom_private_data php_dom_private_data;

struct php_dom_libxml_ns_mapper;
typedef struct php_dom_libxml_ns_mapper php_dom_libxml_ns_mapper;

php_libxml_private_data_header *php_dom_libxml_private_data_header(php_dom_private_data *private_data);
php_dom_libxml_ns_mapper *php_dom_ns_mapper_from_private(php_dom_private_data *private_data);
php_dom_private_data *php_dom_private_data_create(void);
void php_dom_private_data_destroy(php_dom_private_data *data);
void php_dom_add_templated_content(php_dom_private_data *private_data, const xmlNode *template_node, xmlNodePtr fragment);
xmlNodePtr php_dom_retrieve_templated_content(php_dom_private_data *private_data, const xmlNode *template_node);
xmlNodePtr php_dom_ensure_templated_content(php_dom_private_data *private_data, xmlNodePtr template_node);
void php_dom_remove_templated_content(php_dom_private_data *private_data, const xmlNode *template_node);
uint32_t php_dom_get_template_count(const php_dom_private_data *private_data);
void dom_add_element_ns_hook(php_dom_private_data *private_data, xmlNodePtr element);

#endif
