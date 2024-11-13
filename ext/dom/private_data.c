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
#include <config.h>
#endif

#include "php.h"
#if defined(HAVE_LIBXML) && defined(HAVE_DOM)
#include "php_dom.h"
#include "private_data.h"
#include "internal_helpers.h"

static void php_dom_libxml_private_data_destroy(php_libxml_private_data_header *header)
{
	php_dom_private_data_destroy((php_dom_private_data *) header);
}

static void php_dom_libxml_private_data_ns_hook(php_libxml_private_data_header *header, xmlNodePtr node)
{
	php_dom_remove_templated_content((php_dom_private_data *) header, node);
}

php_libxml_private_data_header *php_dom_libxml_private_data_header(php_dom_private_data *private_data)
{
	return private_data == NULL ? NULL : &private_data->header;
}

php_dom_libxml_ns_mapper *php_dom_ns_mapper_from_private(php_dom_private_data *private_data)
{
	return private_data == NULL ? NULL : &private_data->ns_mapper;
}

php_dom_private_data *php_dom_private_data_create(void)
{
	php_dom_private_data *private_data = emalloc(sizeof(*private_data));
	private_data->header.dtor = php_dom_libxml_private_data_destroy;
	private_data->header.ns_hook = php_dom_libxml_private_data_ns_hook;
	private_data->ns_mapper.html_ns = NULL;
	private_data->ns_mapper.prefixless_xmlns_ns = NULL;
	zend_hash_init(&private_data->ns_mapper.uri_to_prefix_map, 0, NULL, ZVAL_PTR_DTOR, false);
	private_data->template_fragments = NULL;
	return private_data;
}

void php_dom_private_data_destroy(php_dom_private_data *data)
{
	zend_hash_destroy(&data->ns_mapper.uri_to_prefix_map);
	if (data->template_fragments != NULL) {
		xmlNodePtr node;
		ZEND_HASH_MAP_FOREACH_PTR(data->template_fragments, node) {
			xmlFreeNode(node);
		} ZEND_HASH_FOREACH_END();
		zend_hash_destroy(data->template_fragments);
		FREE_HASHTABLE(data->template_fragments);
	}
	efree(data);
}

static void php_dom_free_templated_content(php_dom_private_data *private_data, xmlNodePtr base)
{
	/* Note: it's not possible to obtain a userland reference to these yet, so we can just free them without worrying
	 *       about their proxies.
	 * Note 2: it's possible to have nested template content. */

	if (zend_hash_num_elements(private_data->template_fragments) > 0) {
		/* There's more templated content, try to free it. */
		xmlNodePtr current = base->children;
		while (current != NULL) {
			if (current->type == XML_ELEMENT_NODE) {
				php_dom_remove_templated_content(private_data, current);
			}

			current = php_dom_next_in_tree_order(current, base);
		}
	}

	xmlFreeNode(base);
}

void php_dom_add_templated_content(php_dom_private_data *private_data, const xmlNode *template_node, xmlNodePtr fragment)
{
	if (private_data->template_fragments == NULL) {
		ALLOC_HASHTABLE(private_data->template_fragments);
		zend_hash_init(private_data->template_fragments, 0, NULL, NULL, false);
		zend_hash_real_init_mixed(private_data->template_fragments);
	}

	zend_hash_index_add_new_ptr(private_data->template_fragments, dom_mangle_pointer_for_key(template_node), fragment);
}

xmlNodePtr php_dom_retrieve_templated_content(php_dom_private_data *private_data, const xmlNode *template_node)
{
	if (private_data->template_fragments == NULL) {
		return NULL;
	}

	return zend_hash_index_find_ptr(private_data->template_fragments, dom_mangle_pointer_for_key(template_node));
}

xmlNodePtr php_dom_ensure_templated_content(php_dom_private_data *private_data, xmlNodePtr template_node)
{
	xmlNodePtr result = php_dom_retrieve_templated_content(private_data, template_node);
	if (result == NULL) {
		result = xmlNewDocFragment(template_node->doc);
		if (EXPECTED(result != NULL)) {
			result->parent = template_node;
			dom_add_element_ns_hook(private_data, template_node);
			php_dom_add_templated_content(private_data, template_node, result);
		}
	}
	return result;
}

void php_dom_remove_templated_content(php_dom_private_data *private_data, const xmlNode *template_node)
{
	if (private_data->template_fragments != NULL) {
		/* Deletion needs to be done not via a destructor because we can't access private_data from there. */
		zval *zv = zend_hash_index_find(private_data->template_fragments, dom_mangle_pointer_for_key(template_node));
		if (zv != NULL) {
			xmlNodePtr node = Z_PTR_P(zv);
			ZEND_ASSERT(offsetof(Bucket, val) == 0 && "Type cast only works if this is true");
			Bucket* bucket = (Bucket*) zv;
			/* First remove it from the bucket before freeing the content, otherwise recursion could make the bucket
			 * pointer invalid due to hash table structure changes. */
			zend_hash_del_bucket(private_data->template_fragments, bucket);
			php_dom_free_templated_content(private_data, node);
		}
	}
}

uint32_t php_dom_get_template_count(const php_dom_private_data *private_data)
{
	if (private_data->template_fragments != NULL) {
		return zend_hash_num_elements(private_data->template_fragments);
	} else {
		return 0;
	}
}

void dom_add_element_ns_hook(php_dom_private_data *private_data, xmlNodePtr element)
{
	xmlNsPtr ns = pemalloc(sizeof(*ns), true);

	/* The private data is a tagged data structure where only tag 1 is defined by ext/libxml to register a hook. */
	memset(ns, 0, sizeof(*ns));
	ns->prefix = xmlStrdup(element->ns->prefix);
	ns->href = xmlStrdup(element->ns->href);
	ns->type = XML_LOCAL_NAMESPACE;
	ns->_private = (void *) ((uintptr_t) private_data | LIBXML_NS_TAG_HOOK);
	element->ns = ns;

	php_libxml_set_old_ns(element->doc, ns);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
