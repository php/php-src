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
#include "internal_helpers.h"

const dom_ns_magic_token *dom_ns_is_html_magic_token = (const dom_ns_magic_token *) DOM_XHTML_NS_URI;
const dom_ns_magic_token *dom_ns_is_mathml_magic_token = (const dom_ns_magic_token *) DOM_MATHML_NS_URI;
const dom_ns_magic_token *dom_ns_is_svg_magic_token = (const dom_ns_magic_token *) DOM_SVG_NS_URI;
const dom_ns_magic_token *dom_ns_is_xlink_magic_token = (const dom_ns_magic_token *) DOM_XLINK_NS_URI;
const dom_ns_magic_token *dom_ns_is_xml_magic_token = (const dom_ns_magic_token *) DOM_XML_NS_URI;
const dom_ns_magic_token *dom_ns_is_xmlns_magic_token = (const dom_ns_magic_token *) DOM_XMLNS_NS_URI;

struct _dom_libxml_ns_mapper {
	php_libxml_private_data_header header;
	/* This is used almost all the time for HTML documents, so it makes sense to cache this. */
	xmlNsPtr html_ns;
	/* Used for every prefixless namespace declaration in XML, so also very common. */
	xmlNsPtr prefixless_xmlns_ns;
	HashTable uri_to_prefix_map;
};

static void dom_libxml_ns_mapper_prefix_map_element_dtor(zval *zv)
{
	if (DOM_Z_IS_OWNED(zv)) {
		efree(Z_PTR_P(zv));
	}
}

static HashTable *dom_libxml_ns_mapper_ensure_prefix_map(dom_libxml_ns_mapper *mapper, zend_string **uri)
{
	zval *zv = zend_hash_find(&mapper->uri_to_prefix_map, *uri);
	HashTable *prefix_map;
	if (zv == NULL) {
		prefix_map = emalloc(sizeof(HashTable));
		zend_hash_init(prefix_map, 0, NULL, dom_libxml_ns_mapper_prefix_map_element_dtor, false);
		zval zv;
		ZVAL_ARR(&zv, prefix_map);
		zend_hash_add_new(&mapper->uri_to_prefix_map, *uri, &zv);
	} else {
		/* cast to Bucket* only works if this holds, I would prefer a static assert but we're stuck at C99. */
		ZEND_ASSERT(XtOffsetOf(Bucket, val) == 0);
		ZEND_ASSERT(Z_TYPE_P(zv) == IS_ARRAY);
		Bucket *bucket = (Bucket *) zv;
		/* Make sure we take the value from the key string that lives long enough. */
		*uri = bucket->key;
		prefix_map = Z_ARRVAL_P(zv);
	}
	return prefix_map;
}

static void dom_libxml_ns_mapper_header_destroy(php_libxml_private_data_header *header)
{
	dom_libxml_ns_mapper_destroy((dom_libxml_ns_mapper *) header);
}

dom_libxml_ns_mapper *dom_libxml_ns_mapper_create(void)
{
	dom_libxml_ns_mapper *mapper = emalloc(sizeof(*mapper));
	mapper->header.dtor = dom_libxml_ns_mapper_header_destroy;
	mapper->html_ns = NULL;
	mapper->prefixless_xmlns_ns = NULL;
	zend_hash_init(&mapper->uri_to_prefix_map, 0, NULL, ZVAL_PTR_DTOR, false);
	return mapper;
}

void dom_libxml_ns_mapper_destroy(dom_libxml_ns_mapper *mapper)
{
	zend_hash_destroy(&mapper->uri_to_prefix_map);
	efree(mapper);
}

static xmlNsPtr dom_libxml_ns_mapper_ensure_cached_ns(dom_libxml_ns_mapper *mapper, xmlNsPtr *ptr, const char *uri, size_t length, const dom_ns_magic_token *token)
{
	if (EXPECTED(*ptr != NULL)) {
		return *ptr;
	}

	zend_string *uri_str = zend_string_init(uri, length, false);
	*ptr = dom_libxml_ns_mapper_get_ns(mapper, NULL, uri_str);
	(*ptr)->_private = (void *) token;
	zend_string_release_ex(uri_str, false);
	return *ptr;
}

xmlNsPtr dom_libxml_ns_mapper_ensure_html_ns(dom_libxml_ns_mapper *mapper)
{
	return dom_libxml_ns_mapper_ensure_cached_ns(mapper, &mapper->html_ns, DOM_XHTML_NS_URI, sizeof(DOM_XHTML_NS_URI) - 1, dom_ns_is_html_magic_token);
}

xmlNsPtr dom_libxml_ns_mapper_ensure_prefixless_xmlns_ns(dom_libxml_ns_mapper *mapper)
{
	return dom_libxml_ns_mapper_ensure_cached_ns(mapper, &mapper->prefixless_xmlns_ns, DOM_XMLNS_NS_URI, sizeof(DOM_XMLNS_NS_URI) - 1, dom_ns_is_xmlns_magic_token);
}

static xmlNsPtr dom_create_owned_ns(zend_string *prefix, zend_string *uri)
{
	ZEND_ASSERT(prefix != NULL);
	ZEND_ASSERT(uri != NULL);

	xmlNsPtr ns = emalloc(sizeof(*ns));
	memset(ns, 0, sizeof(*ns));
	ns->type = XML_LOCAL_NAMESPACE;
	/* These two strings are kept alive because they're the hash table keys that lead to this entry. */
	ns->prefix = ZSTR_LEN(prefix) == 0 ? NULL : BAD_CAST ZSTR_VAL(prefix);
	ns->href = BAD_CAST ZSTR_VAL(uri);
	/* Note ns->context is unused in libxml2 at the moment, and if it were used it would be for
	 * LIBXML_NAMESPACE_DICT which is opt-in anyway. */

	return ns;
}

xmlNsPtr dom_libxml_ns_mapper_get_ns(dom_libxml_ns_mapper *mapper, zend_string *prefix, zend_string *uri)
{
	if (uri == NULL) {
		uri = zend_empty_string;
	}

	if (prefix == NULL) {
		prefix = zend_empty_string;
	}

	if (ZSTR_LEN(prefix) == 0 && ZSTR_LEN(uri) == 0) {
		return NULL;
	}

	HashTable *prefix_map = dom_libxml_ns_mapper_ensure_prefix_map(mapper, &uri);
	xmlNsPtr found = zend_hash_find_ptr(prefix_map, prefix);
	if (found != NULL) {
		return found;
	}

	xmlNsPtr ns = dom_create_owned_ns(prefix, uri);

	zval new_zv;
	DOM_Z_OWNED(&new_zv, ns);
	zend_hash_add_new(prefix_map, prefix, &new_zv);

	return ns;
}

xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_prefix_string(dom_libxml_ns_mapper *mapper, const xmlChar *prefix, size_t prefix_len, zend_string *uri)
{
	xmlNsPtr ns;
	if (prefix_len == 0) {
		/* Fast path */
		ns = dom_libxml_ns_mapper_get_ns(mapper, zend_empty_string, uri);
	} else {
		zend_string *prefix_str = zend_string_init((const char *) prefix, prefix_len, false);
		ns = dom_libxml_ns_mapper_get_ns(mapper, prefix_str, uri);
		zend_string_release_ex(prefix_str, false);
	}
	return ns;
}

static xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_strings_ex(dom_libxml_ns_mapper *mapper, const char *prefix, size_t prefix_len, const char *uri, size_t uri_len)
{
	zend_string *prefix_str = zend_string_init(prefix, prefix_len, false);
	zend_string *uri_str = zend_string_init(uri, uri_len, false);
	xmlNsPtr ns = dom_libxml_ns_mapper_get_ns(mapper, prefix_str, uri_str);
	zend_string_release_ex(prefix_str, false);
	zend_string_release_ex(uri_str, false);
	return ns;
}

static zend_always_inline xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_strings(dom_libxml_ns_mapper *mapper, const char *prefix, const char *uri)
{
	return dom_libxml_ns_mapper_get_ns_raw_strings_ex(mapper, prefix, strlen(prefix), uri, strlen(uri));
}

xmlNsPtr dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(dom_libxml_ns_mapper *mapper, const char *prefix, const char *uri)
{
	if (prefix == NULL) {
		prefix = "";
	}
	if (uri == NULL) {
		uri = "";
	}
	return dom_libxml_ns_mapper_get_ns_raw_strings(mapper, prefix, uri);
}

static xmlNsPtr dom_libxml_ns_mapper_store_and_normalize_parsed_ns(dom_libxml_ns_mapper *mapper, xmlNsPtr ns)
{
	ZEND_ASSERT(ns != NULL);

	zend_string *href_str = zend_string_init((const char *) ns->href, xmlStrlen(ns->href), false);
	zend_string *href_str_orig = href_str;
	HashTable *prefix_map = dom_libxml_ns_mapper_ensure_prefix_map(mapper, &href_str);
	zend_string_release_ex(href_str_orig, false);

	const char *prefix = (const char *) ns->prefix;
	size_t prefix_len;
	if (prefix == NULL) {
		prefix = "";
		prefix_len = 0;
	} else {
		prefix_len = xmlStrlen(ns->prefix);
	}

	zval *zv = zend_hash_str_find_ptr(prefix_map, prefix, prefix_len);
	if (zv != NULL) {
		return Z_PTR_P(zv);
	}

	zval new_zv;
	DOM_Z_UNOWNED(&new_zv, ns);
	zend_hash_str_add_new(prefix_map, prefix, prefix_len, &new_zv);

	return ns;
}

php_libxml_private_data_header *dom_libxml_ns_mapper_header(dom_libxml_ns_mapper *mapper)
{
	return mapper == NULL ? NULL : &mapper->header;
}

typedef struct {
	/* Fast lookup for created mappings. */
	HashTable old_ns_to_new_ns_ptr;
	/* It is common that the last created mapping will be used for a while,
	 * cache it too to bypass the hash table. */
	xmlNsPtr last_mapped_src, last_mapped_dst;
	dom_libxml_ns_mapper *ns_mapper;
} dom_libxml_reconcile_ctx;

xmlAttrPtr dom_ns_compat_mark_attribute(dom_libxml_ns_mapper *mapper, xmlNodePtr node, xmlNsPtr ns)
{
	xmlNsPtr xmlns_ns;
	const xmlChar *name;
	if (ns->prefix != NULL) {
		xmlns_ns = dom_libxml_ns_mapper_get_ns_raw_strings(mapper, "xmlns", DOM_XMLNS_NS_URI);
		name = ns->prefix;
	} else {
		xmlns_ns = dom_libxml_ns_mapper_ensure_prefixless_xmlns_ns(mapper);
		name = BAD_CAST "xmlns";
	}

	ZEND_ASSERT(xmlns_ns != NULL);

	return xmlSetNsProp(node, xmlns_ns, name, ns->href);
}

void dom_ns_compat_mark_attribute_list(dom_libxml_ns_mapper *mapper, xmlNodePtr node)
{
	if (node->nsDef == NULL) {
		return;
	}

	/* We want to prepend at the front, but in order of the namespace definitions.
	 * So temporarily unlink the existing properties and add them again at the end. */
	xmlAttrPtr attr = node->properties;
	node->properties = NULL;

	xmlNsPtr ns = node->nsDef;
	xmlAttrPtr last_added = NULL;
	do {
		last_added = dom_ns_compat_mark_attribute(mapper, node, ns);
		dom_libxml_ns_mapper_store_and_normalize_parsed_ns(mapper, ns);
		xmlNsPtr next = ns->next;
		ns->next = NULL;
		php_libxml_set_old_ns(node->doc, ns);
		ns = next;
	} while (ns != NULL);

	if (last_added != NULL) {
		/* node->properties now points to the first namespace declaration attribute. */
		if (attr != NULL) {
			last_added->next = attr;
			attr->prev = last_added;
		}
	} else {
		/* Nothing added, so nothing changed. Only really possible on OOM. */
		node->properties = attr;
	}

	node->nsDef = NULL;
}

static bool dom_ns_is_fast_ex(xmlNsPtr ns, const dom_ns_magic_token *magic_token)
{
	ZEND_ASSERT(ns != NULL);
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
	return false;
}

bool dom_ns_is_fast(const xmlNode *nodep, const dom_ns_magic_token *magic_token)
{
	ZEND_ASSERT(nodep != NULL);
	xmlNsPtr ns = nodep->ns;
	if (ns != NULL) {
		return dom_ns_is_fast_ex(ns, magic_token);
	}
	return false;
}

bool dom_ns_is_html_and_document_is_html(const xmlNode *nodep)
{
	ZEND_ASSERT(nodep != NULL);
	return nodep->doc && nodep->doc->type == XML_HTML_DOCUMENT_NODE && dom_ns_is_fast(nodep, dom_ns_is_html_magic_token);
}

/* will rename prefixes if there is a declaration with the same prefix but different uri. */
void php_dom_reconcile_attribute_namespace_after_insertion(xmlAttrPtr attrp)
{
	ZEND_ASSERT(attrp != NULL);

	if (attrp->ns != NULL) {
		/* Try to link to an existing namespace. If that won't work, reconcile. */
		xmlNodePtr nodep = attrp->parent;
		xmlNsPtr matching_ns = xmlSearchNs(nodep->doc, nodep, attrp->ns->prefix);
		if (matching_ns && xmlStrEqual(matching_ns->href, attrp->ns->href)) {
			/* Doesn't leak because this doesn't define the declaration. */
			attrp->ns = matching_ns;
		} else {
			if (attrp->ns->prefix != NULL) {
				/* Note: explicitly use the legacy reconciliation as it mostly (i.e. as good as it gets for legacy DOM)
				* does the right thing for attributes. */
				xmlReconciliateNs(nodep->doc, nodep);
			}
		}
	}
}

static zend_always_inline zend_long dom_mangle_pointer_for_key(void *ptr)
{
	zend_ulong value = (zend_ulong) (uintptr_t) ptr;
	/* Shift 3/4 for better hash distribution because the low 3/4 bits are always 0. */
#if SIZEOF_ZEND_LONG == 8
	return value >> 4;
#else
	return value >> 3;
#endif
}

static zend_always_inline void dom_libxml_reconcile_modern_single_node(dom_libxml_reconcile_ctx *ctx, xmlNodePtr ns_holder, xmlNodePtr node)
{
	ZEND_ASSERT(node->ns != NULL);

	if (node->ns == ctx->last_mapped_src) {
		node->ns = ctx->last_mapped_dst;
		return;
	}

	/* If the namespace is the same as in the map, we're good. */
	xmlNsPtr new_ns = zend_hash_index_find_ptr(&ctx->old_ns_to_new_ns_ptr, dom_mangle_pointer_for_key(node->ns));
	if (new_ns == NULL) {
		/* We have to create an alternative declaration, and we'll add it to the map. */
		const char *prefix = (const char *) node->ns->prefix;
		const char *href = (const char *) node->ns->href;
		new_ns = dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(ctx->ns_mapper, prefix, href);
		zend_hash_index_add_new_ptr(&ctx->old_ns_to_new_ns_ptr, dom_mangle_pointer_for_key(node->ns), new_ns);
		ctx->last_mapped_src = node->ns;
		ctx->last_mapped_dst = new_ns;
		node->ns = new_ns;
	} else if (node->ns != new_ns) {
		/* The namespace is different, so we have to replace it. */
		node->ns = new_ns;
	}
}

static zend_always_inline bool dom_libxml_reconcile_fast_element_skip(xmlNodePtr node)
{
	/* Fast path: this is a lone element and the namespace is defined by the node (or the namespace is NULL). */
	ZEND_ASSERT(node->type == XML_ELEMENT_NODE);
	return node->children == NULL && node->properties == NULL && node->ns == node->nsDef;
}

static zend_always_inline void dom_libxml_reconcile_modern_single_element_node(dom_libxml_reconcile_ctx *ctx, xmlNodePtr node)
{
	ZEND_ASSERT(node->type == XML_ELEMENT_NODE);

	/* Add the defined namespaces to the mapper. */
	for (xmlNsPtr ns = node->nsDef; ns != NULL; ns = ns->next) {
		zend_hash_index_add_ptr(&ctx->old_ns_to_new_ns_ptr, dom_mangle_pointer_for_key(ns), ns);
	}

	if (node->ns != NULL) {
		dom_libxml_reconcile_modern_single_node(ctx, node, node);
	}

	for (xmlAttrPtr attr = node->properties; attr != NULL; attr = attr->next) {
		if (attr->ns != NULL) {
			dom_libxml_reconcile_modern_single_node(ctx, node, (xmlNodePtr) attr);
		}
	}
}

void dom_libxml_reconcile_modern(dom_libxml_ns_mapper *ns_mapper, xmlNodePtr node)
{
	if (node->type == XML_ATTRIBUTE_NODE) {
		if (node->ns != NULL) {
			node->ns = dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(ns_mapper, (const char *) node->ns->prefix, (const char *) node->ns->href);
		}
		return;
	}

	if (node->type != XML_ELEMENT_NODE || dom_libxml_reconcile_fast_element_skip(node)) {
		return;
	}

	dom_libxml_reconcile_ctx ctx;
	zend_hash_init(&ctx.old_ns_to_new_ns_ptr, 0, NULL, NULL, 0);
	ctx.last_mapped_src = NULL;
	ctx.last_mapped_dst = NULL;
	ctx.ns_mapper = ns_mapper;

	dom_libxml_reconcile_modern_single_element_node(&ctx, node);

	xmlNodePtr base = node;
	node = node->children;
	while (node != NULL) {
		ZEND_ASSERT(node != base);

		if (node->type == XML_ELEMENT_NODE) {
			dom_libxml_reconcile_modern_single_element_node(&ctx, node);

			if (node->children) {
				node = node->children;
				continue;
			}
		}

		node = php_dom_next_in_tree_order(node, base);
	}

	zend_hash_destroy(&ctx.old_ns_to_new_ns_ptr);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
