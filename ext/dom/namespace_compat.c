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

typedef struct {
	/* Fast lookup for created mappings. */
	HashTable mapper;
	/* It is common that the last created mapping will be used for a while,
	 * cache it too to bypass the hash table. */
	xmlNsPtr last_mapped_src, last_mapped_dst;
} dom_libxml_reconcile_ctx;

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
			/* Can already start at the second child because the first child is always the xml namespace. */
			xmlNsPtr tmp = docp->oldNs->next;
			while (tmp != NULL) {
				if (dom_ns_is_fast_ex(tmp, dom_ns_is_html_magic_token)) {
					nsptr = tmp;
					break;
				}
				tmp = tmp->next;
			}
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

static zend_always_inline void dom_libxml_reconcile_modern_single_attribute_node(xmlAttrPtr attr)
{
	attr->ns = dom_ns_create_local_as_is(
		attr->doc, attr->parent, attr->parent, (const char *) attr->ns->href, attr->ns->prefix
	);
}

/* resolve_prefix_conflict will rename prefixes if there is a declaration with the same prefix but different uri. */
void php_dom_reconcile_attribute_namespace_after_insertion(xmlAttrPtr attrp, bool resolve_prefix_conflict)
{
	ZEND_ASSERT(attrp != NULL);

	if (attrp->ns != NULL) {
		if (resolve_prefix_conflict) {
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
		} else {
			dom_libxml_reconcile_modern_single_attribute_node(attrp);
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
	xmlNsPtr new_ns = zend_hash_index_find_ptr(&ctx->mapper, dom_mangle_pointer_for_key(node->ns));
	if (new_ns == NULL) {
		/* We have to create an alternative declaration, and we'll add it to the map.
		 * This can only really fail on OOM, which hopefully doesn't happen.
		 * If it happens anyway, the namespace will be set to NULL. */
		new_ns = dom_ns_create_local_as_is(node->doc, ns_holder, ns_holder, (const char *) node->ns->href, node->ns->prefix);
		zend_hash_index_add_new_ptr(&ctx->mapper, dom_mangle_pointer_for_key(node->ns), new_ns);
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
		zend_hash_index_add_ptr(&ctx->mapper, dom_mangle_pointer_for_key(ns), ns);
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

void dom_libxml_reconcile_modern(xmlNodePtr node)
{
	if (node->type != XML_ELEMENT_NODE || dom_libxml_reconcile_fast_element_skip(node)) {
		return;
	}

	dom_libxml_reconcile_ctx ctx;
	zend_hash_init(&ctx.mapper, 0, NULL, NULL, 0);
	ctx.last_mapped_src = NULL;
	ctx.last_mapped_dst = NULL;

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

	zend_hash_destroy(&ctx.mapper);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
