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
#include "xml_serializer.h"
#include "namespace_compat.h"
#include "serialize_common.h"

// TODO: implement iterative approach instead of recursive?

/* This file implements the XML serialization algorithm.
 * https://w3c.github.io/DOM-Parsing/#dom-xmlserializer-serializetostring (Date 2021-05-02)
 *
 * The following are spec issues that were fixed in this implementation, but are not yet fixed
 * in the spec itself:
 * https://github.com/w3c/DOM-Parsing/issues/28
 * https://github.com/w3c/DOM-Parsing/issues/29
 * https://github.com/w3c/DOM-Parsing/issues/38
 * https://github.com/w3c/DOM-Parsing/issues/43
 * https://github.com/w3c/DOM-Parsing/issues/44
 * https://github.com/w3c/DOM-Parsing/issues/45
 * https://github.com/w3c/DOM-Parsing/issues/47
 * https://github.com/w3c/DOM-Parsing/issues/50
 * https://github.com/w3c/DOM-Parsing/issues/52
 * https://github.com/w3c/DOM-Parsing/issues/59
 * https://github.com/w3c/DOM-Parsing/issues/71
 */

#define TRY(x) do { if (UNEXPECTED(x < 0)) { return -1; } } while (0)
#define TRY_OR_CLEANUP(x) do { if (UNEXPECTED(x < 0)) { goto cleanup; } } while (0)

/* We're using the type flags of the zval to store an extra flag. */
#define DOM_Z_OWNED(z, v)	ZVAL_PTR(z, v)
#define DOM_Z_UNOWNED(z, v)	ZVAL_INDIRECT(z, v)
#define DOM_Z_IS_OWNED(z)	(Z_TYPE_P(z) == IS_PTR)

/* https://w3c.github.io/DOM-Parsing/#dfn-namespace-prefix-map
 * This associates a namespace uri with a list of possible prefixes. */
typedef struct {
	HashTable *ht;
} dom_xml_ns_prefix_map;

/* https://w3c.github.io/DOM-Parsing/#dfn-local-prefixes-map */
typedef struct {
	HashTable ht;
} dom_xml_local_prefix_map;

typedef struct {
	const xmlChar *prefix, *name;
} dom_qname_pair;

static int dom_xml_serialization_algorithm(
	xmlSaveCtxtPtr ctxt,
	xmlOutputBufferPtr out,
	dom_xml_ns_prefix_map *namespace_prefix_map,
	xmlNodePtr node,
	const xmlChar *namespace,
	unsigned int *prefix_index,
	int indent
);

static bool dom_xml_str_equals_treat_nulls_as_empty(const xmlChar *s1, const xmlChar *s2)
{
	if (s1 == s2) {
		return true;
	}
	if (s1 == NULL) {
		return s2 == NULL || *s2 == '\0';
	}
	if (s2 == NULL) {
		/* Note: at this point we know that s1 != NULL. */
		return *s1 == '\0';
	}
	return strcmp((const char *) s1, (const char *) s2) == 0;
}

static zend_always_inline bool dom_xml_str_equals_treat_nulls_as_nulls(const xmlChar *s1, const xmlChar *s2)
{
	if (s1 == s2) {
		return true;
	}
	if (s1 == NULL || s2 == NULL) {
		return false;
	}
	return strcmp((const char *) s1, (const char *) s2) == 0;
}

static zend_always_inline void dom_xml_ns_prefix_map_ctor(dom_xml_ns_prefix_map *map)
{
	ALLOC_HASHTABLE(map->ht);
	zend_hash_init(map->ht, 8, NULL, NULL, false);
}

static void dom_xml_ns_prefix_map_destroy(dom_xml_ns_prefix_map *map)
{
	HashTable *list;
	ZEND_HASH_MAP_FOREACH_PTR(map->ht, list) {
		if (GC_DELREF(list) == 0) {
			zval *tmp;
			ZEND_HASH_PACKED_FOREACH_VAL(list, tmp) {
				if (DOM_Z_IS_OWNED(tmp)) {
					efree(Z_PTR_P(tmp));
				}
			} ZEND_HASH_FOREACH_END();

			zend_hash_destroy(list);
			efree(list);
		}
	} ZEND_HASH_FOREACH_END();

	zend_hash_destroy(map->ht);
	efree(map->ht);
	map->ht = NULL;
}

static zend_always_inline void dom_xml_ns_prefix_map_dtor(dom_xml_ns_prefix_map *map)
{
	if (GC_DELREF(map->ht) == 0) {
		dom_xml_ns_prefix_map_destroy(map);
	}
}

static zend_always_inline void dom_xml_ns_prefix_map_copy(dom_xml_ns_prefix_map *dst, const dom_xml_ns_prefix_map *src)
{
	dst->ht = src->ht;
	GC_ADDREF(dst->ht);
}

static zend_always_inline void dom_xml_local_prefix_map_ctor(dom_xml_local_prefix_map *map)
{
	zend_hash_init(&map->ht, 8, NULL, NULL, false);
}

static zend_always_inline void dom_xml_local_prefix_map_dtor(dom_xml_local_prefix_map *map)
{
	zend_hash_destroy(&map->ht);
}

static zend_always_inline void dom_xml_local_prefix_map_add(
	dom_xml_local_prefix_map *map,
	const xmlChar *prefix,
	size_t prefix_len,
	const xmlChar *ns
)
{
	ZEND_ASSERT(prefix != NULL);
	zend_hash_str_add_ptr(&map->ht, (const char *) prefix, prefix_len, (void *) ns);
}

static zend_always_inline const xmlChar *dom_xml_local_prefix_map_find(
	const dom_xml_local_prefix_map *map,
	const xmlChar *prefix,
	size_t prefix_len
)
{
	ZEND_ASSERT(prefix != NULL);
	return zend_hash_str_find_ptr(&map->ht, (const char *) prefix, prefix_len);
}

static zend_always_inline bool dom_xml_local_prefix_map_conflicts(
	const dom_xml_local_prefix_map *map,
	const xmlChar *prefix,
	size_t prefix_len,
	const xmlChar *ns
)
{
	const xmlChar *result = dom_xml_local_prefix_map_find(map, prefix, prefix_len);
	if (result == NULL) {
		return false;
	}
	return !dom_xml_str_equals_treat_nulls_as_empty(result, ns);
}

static zend_always_inline bool dom_xml_local_prefix_map_contains(
	const dom_xml_local_prefix_map *map,
	const xmlChar *prefix,
	size_t prefix_len
)
{
	return dom_xml_local_prefix_map_find(map, prefix, prefix_len) != NULL;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-add */
static void dom_xml_ns_prefix_map_add(
	dom_xml_ns_prefix_map *map,
	const xmlChar *prefix,
	bool prefix_owned,
	const xmlChar *ns,
	size_t ns_length
)
{
	ZEND_ASSERT(map->ht != NULL);
	ZEND_ASSERT(prefix != NULL);

	if (ns == NULL) {
		ns = BAD_CAST "";
	}

	if (GC_REFCOUNT(map->ht) > 1) {
		GC_DELREF(map->ht);
		map->ht = zend_array_dup(map->ht);

		HashTable *list;
		ZEND_HASH_MAP_FOREACH_PTR(map->ht, list) {
			GC_ADDREF(list);
		} ZEND_HASH_FOREACH_END();
	}

	/* 1. Let candidates list be the result of retrieving a list from map where there exists a key in map
	*     that matches the value of ns
	 *    or if there is no such key, then let candidates list be null. */
	HashTable *list = zend_hash_str_find_ptr(map->ht, (const char *) ns, ns_length);

	/* 2. If candidates list is null, then create a new list with prefix as the only item in the list,
	 *    and associate that list with a new key ns in map. */
	if (list == NULL) {
		ALLOC_HASHTABLE(list);
		zend_hash_init(list, 8, NULL, NULL, false);
		zend_hash_str_add_new_ptr(map->ht, (const char *) ns, ns_length, list);
	} else if (GC_REFCOUNT(list) > 1) {
		GC_DELREF(list);
		list = zend_array_dup(list);
		zend_hash_str_update_ptr(map->ht, (const char *) ns, ns_length, list);
	}

	/* 3. (Otherwise), append prefix to the end of candidates list. */
	zval tmp;
	if (prefix_owned) {
		DOM_Z_OWNED(&tmp, (void *) prefix);
	} else {
		DOM_Z_UNOWNED(&tmp, (void *) prefix);
	}
	zend_hash_next_index_insert_new(list, &tmp);
}

/* https://w3c.github.io/DOM-Parsing/#dfn-found */
static zend_always_inline HashTable *dom_get_candidates_list(dom_xml_ns_prefix_map *map, const xmlChar *ns, size_t ns_length)
{
	ZEND_ASSERT(map->ht != NULL);

	/* 1. Let candidates list be the result of retrieving a list from map where there exists a key in map that matches
	 *    the value of ns
	 *    or if there is no such key, then let candidates list be null. */
	return zend_hash_str_find_ptr(map->ht, (const char *) ns, ns_length);
}

/* https://w3c.github.io/DOM-Parsing/#dfn-found */
static zend_always_inline bool dom_prefix_in_candidate_list(const HashTable *list, const xmlChar *prefix)
{
	ZEND_ASSERT(prefix != NULL);

	if (list == NULL) {
		return false;
	}

	/* 2. If the value of prefix occurs at least once in candidates list, return true, otherwise return false. */
	const char *tmp;
	ZEND_HASH_PACKED_FOREACH_PTR(list, tmp) {
		if (dom_xml_str_equals_treat_nulls_as_empty(BAD_CAST tmp, prefix)) {
			return true;
		}
	} ZEND_HASH_FOREACH_END();

	return false;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-found */
static zend_always_inline bool dom_prefix_found_in_ns_prefix_map(
	dom_xml_ns_prefix_map *map,
	const xmlChar *prefix,
	const xmlChar *ns,
	size_t ns_length
)
{
	ZEND_ASSERT(ns != NULL);
	HashTable *list = dom_get_candidates_list(map, ns, ns_length);
	return dom_prefix_in_candidate_list(list, prefix);
}

/* Helper to get the attribute value, will return "" instead of NULL for empty values, to mimic getAttribute()'s behaviour. */
static zend_always_inline const xmlChar *dom_get_attribute_value(const xmlAttr *attr)
{
	if (attr->children == NULL) {
		return BAD_CAST "";
	}
	return attr->children->content ? attr->children->content : BAD_CAST "";
}

/* https://w3c.github.io/DOM-Parsing/#dfn-recording-the-namespace-information */
static const xmlChar *dom_recording_the_namespace_information(
	dom_xml_ns_prefix_map *namespace_prefix_map,
	dom_xml_local_prefix_map *local_prefixes_map,
	xmlNodePtr element
)
{
	ZEND_ASSERT(element->type == XML_ELEMENT_NODE);

	/* 1. Let default namespace attr value be null. */
	const xmlChar *default_namespace_attr_value = NULL;

	/* 2. [MAIN] For each attribute attr in element's attributes, in the order they are specified in the element's attribute list: */
	for (xmlAttrPtr attr = element->properties; attr != NULL; attr = attr->next) {
		/* Steps 2.1-2.2 fetch namespace information from the attribute, but let's defer that for simplicity to the if's body. */

		/* 2.3. If the attribute namespace is the XMLNS namespace, then: */
		if (dom_ns_is_fast((xmlNodePtr) attr, dom_ns_is_xmlns_magic_token)) {
			/* 2.3.1. If attribute prefix is null, then attr is a default namespace declaration.
			 *        Set the default namespace attr value to attr's value and stop running these steps,
			 *        returning to Main to visit the next attribute. */
			if (attr->ns->prefix == NULL) {
				default_namespace_attr_value = dom_get_attribute_value(attr);
				continue;
			}
			/* 2.3.2. Otherwise, the attribute prefix is not null and attr is a namespace prefix definition.
			 *        Run the following steps: */
			else {
				/* 2.3.2.1. Let prefix definition be the value of attr's localName. */
				const xmlChar *prefix_definition = attr->name;
				ZEND_ASSERT(prefix_definition != NULL);

				/* 2.3.2.2. Let namespace definition be the value of attr's value. */
				const xmlChar *namespace_definition = dom_get_attribute_value(attr);
				ZEND_ASSERT(namespace_definition != NULL);

				/* 2.3.2.3. If namespace definition is the XML namespace, then stop running these steps,
				 *          and return to Main to visit the next attribute. */
				if (strcmp((const char *) namespace_definition, DOM_XML_NS_URI) == 0) {
					continue;
				}

				/* 2.3.2.4. If namespace definition is the empty string (the declarative form of having no namespace),
				 *          then let namespace definition be null instead. */
				if (*namespace_definition == '\0') {
					namespace_definition = NULL;
				}

				size_t namespace_definition_length = namespace_definition == NULL ? 0 : strlen((const char *) namespace_definition);

				/* 2.3.2.5. If prefix definition is found in map given the namespace namespace definition,
				 *          then stop running these steps, and return to Main to visit the next attribute. */
				if (dom_prefix_found_in_ns_prefix_map(namespace_prefix_map, prefix_definition, namespace_definition, namespace_definition_length)) {
					continue;
				}

				/* 2.3.2.6. Add the prefix prefix definition to map given namespace namespace definition. */
				dom_xml_ns_prefix_map_add(namespace_prefix_map, prefix_definition, false, namespace_definition, namespace_definition_length);

				/* 2.3.2.7. Add the value of prefix definition as a new key to the local prefixes map,
				 *          with the namespace definition as the key's value replacing the value of null with the empty string if applicable. */
				size_t prefix_definition_length = strlen((const char *) prefix_definition);
				namespace_definition = namespace_definition == NULL ? BAD_CAST "" : namespace_definition;
				dom_xml_local_prefix_map_add(local_prefixes_map, prefix_definition, prefix_definition_length, namespace_definition);
			}
		}
	}

	/* 3. Return the value of default namespace attr value. */
	return default_namespace_attr_value;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-retrieving-a-preferred-prefix-string */
static const xmlChar *dom_retrieve_a_preferred_prefix_string(
	dom_xml_ns_prefix_map *namespace_prefix_map,
	dom_xml_local_prefix_map *local_prefixes_map,
	const xmlChar *preferred_prefix,
	const xmlChar *ns,
	size_t ns_length
)
{
	ZEND_ASSERT(namespace_prefix_map->ht != NULL);

	if (ns == NULL) {
		ns = BAD_CAST "";
	}

	/* 1. Let candidates list be the result of retrieving a list from map where there exists a key in map that matches
	 *    the value of ns or if there is no such key, then stop running these steps, and return the null value. */
	HashTable *list = dom_get_candidates_list(namespace_prefix_map, ns, ns_length);
	if (list == NULL) {
		return NULL;
	}

	/* 2. Otherwise, for each prefix value prefix in candidates list, iterating from beginning to end: */
	const xmlChar *prefix = NULL;
	const xmlChar *last_non_conflicting_in_list = NULL;

	/* Reverse so that the "nearest" ns gets priority: https://github.com/w3c/DOM-Parsing/issues/45 */
	ZEND_HASH_PACKED_REVERSE_FOREACH_PTR(list, prefix) {
		ZEND_ASSERT(prefix != NULL);

		/* 2.1. If prefix matches preferred prefix, then stop running these steps and return prefix. */
		/* Adapted for https://github.com/w3c/DOM-Parsing/issues/45 */
		if (!dom_xml_local_prefix_map_conflicts(local_prefixes_map, prefix, strlen((const char *) prefix), ns)) {
			if (dom_xml_str_equals_treat_nulls_as_empty(preferred_prefix, prefix)) {
				return prefix;
			}

			if (last_non_conflicting_in_list == NULL) {
				last_non_conflicting_in_list = prefix;
			}
		}
	} ZEND_HASH_FOREACH_END();

	/* 2.2. If prefix is the last item in the candidates list, then stop running these steps and return prefix. */
	/* Note: previously the last item was "prefix", but we loop backwards now. */
	return last_non_conflicting_in_list;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-generating-a-prefix */
static xmlChar *dom_xml_generate_a_prefix(
	dom_xml_ns_prefix_map *map,
	dom_xml_local_prefix_map *local_prefixes_map,
	const xmlChar *new_namespace,
	size_t new_namespace_length,
	unsigned int *prefix_index
)
{
	/* 1. Let generated prefix be the concatenation of the string "ns" and the current numerical value of prefix index. */
	char buffer[32];
	buffer[0] = 'n';
	buffer[1] = 's';
	size_t length;
	do {
		length = snprintf(buffer + 2, sizeof(buffer) - 2, "%u", *prefix_index) + 2;

		/* 2. Let the value of prefix index be incremented by one. */
		(*prefix_index)++;

		/* Loop condition is for https://github.com/w3c/DOM-Parsing/issues/44 */
	} while (dom_xml_local_prefix_map_contains(local_prefixes_map, (const xmlChar *) buffer, length));

	xmlChar *generated_prefix = emalloc(length + 1);
	memcpy(generated_prefix, buffer, length + 1);

	/* 3. Add to map the generated prefix given the new namespace namespace. */
	dom_xml_ns_prefix_map_add(map, generated_prefix, true, new_namespace, new_namespace_length);
	/* Continuation of https://github.com/w3c/DOM-Parsing/issues/44 */
	dom_xml_local_prefix_map_add(local_prefixes_map, generated_prefix, length, new_namespace);

	/* 4. Return the value of generated prefix. */
	return generated_prefix;
}

static int dom_xml_output_qname(xmlOutputBufferPtr out, const dom_qname_pair *qname)
{
	if (qname->prefix != NULL) {
		TRY(xmlOutputBufferWriteString(out, (const char *) qname->prefix));
		TRY(xmlOutputBufferWrite(out, strlen(":"), ":"));
	}
	return xmlOutputBufferWriteString(out, (const char *) qname->name);
}

/* This is a utility method used by both
 * https://w3c.github.io/DOM-Parsing/#dfn-xml-serializing-an-element-node
 * and https://w3c.github.io/DOM-Parsing/#dfn-serializing-an-attribute-value */
static int dom_xml_common_text_serialization(xmlOutputBufferPtr out, const char *content, bool attribute_mode)
{
	if (content == NULL) {
		return 0;
	}

	const char *last_output = content;
	const char *mask = attribute_mode ? "&<>\"\t\n\r" : "&<>";

	while (true) {
		size_t chunk_length = strcspn(content, mask);

		content += chunk_length;
		if (*content == '\0') {
			break;
		}

		TRY(xmlOutputBufferWrite(out, content - last_output, last_output));

		switch (*content) {
			case '&': {
				TRY(xmlOutputBufferWrite(out, strlen("&amp;"), "&amp;"));
				break;
			}

			case '<': {
				TRY(xmlOutputBufferWrite(out, strlen("&lt;"), "&lt;"));
				break;
			}

			case '>': {
				TRY(xmlOutputBufferWrite(out, strlen("&gt;"), "&gt;"));
				break;
			}

			case '"': {
				TRY(xmlOutputBufferWrite(out, strlen("&quot;"), "&quot;"));
				break;
			}
			
			/* The following three are added to address https://github.com/w3c/DOM-Parsing/issues/59 */

			case '\t': {
				TRY(xmlOutputBufferWrite(out, strlen("&#9;"), "&#9;"));
				break;
			}

			case '\n': {
				TRY(xmlOutputBufferWrite(out, strlen("&#10;"), "&#10;"));
				break;
			}

			case '\r': {
				TRY(xmlOutputBufferWrite(out, strlen("&#13;"), "&#13;"));
				break;
			}
		}

		content++;
		last_output = content;
	}

	return xmlOutputBufferWrite(out, content - last_output, last_output);
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serializing-an-element-node */
static zend_always_inline int dom_xml_serialize_text_node(xmlOutputBufferPtr out, xmlNodePtr text)
{
	/* 1. If the require well-formed flag is set ...
	 *    => N/A */

	return dom_xml_common_text_serialization(out, (const char *) text->content, false);
}

/* Spec says to do nothing, but that's inconsistent/wrong, see https://github.com/w3c/DOM-Parsing/issues/28 */
static int dom_xml_serialize_attribute_node(xmlOutputBufferPtr out, xmlNodePtr attr)
{
	if (attr->ns != NULL && attr->ns->prefix != NULL) {
		TRY(xmlOutputBufferWriteString(out, (const char *) attr->ns->prefix));
		TRY(xmlOutputBufferWrite(out, strlen(":"), ":"));
	}
	TRY(xmlOutputBufferWriteString(out, (const char *) attr->name));
	TRY(xmlOutputBufferWrite(out, strlen("=\""), "=\""));
	TRY(dom_xml_common_text_serialization(out, (const char *) dom_get_attribute_value((xmlAttrPtr) attr), true));
	return xmlOutputBufferWrite(out, strlen("\""), "\"");
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serializing-a-comment-node */
static int dom_xml_serialize_comment_node(xmlOutputBufferPtr out, xmlNodePtr comment)
{
	/* 1. If the require well-formed flag is set ...
	 *    => N/A */

	TRY(xmlOutputBufferWrite(out, strlen("<!--"), "<!--"));
	if (EXPECTED(comment->content != NULL)) {
		TRY(xmlOutputBufferWriteString(out, (const char *) comment->content));
	}
	return xmlOutputBufferWrite(out, strlen("-->"), "-->");
}

/* https://w3c.github.io/DOM-Parsing/#xml-serializing-a-processinginstruction-node */
static int dom_xml_serialize_processing_instruction(xmlOutputBufferPtr out, xmlNodePtr pi)
{
	/* Steps 1-2 deal with well-formed flag
	 *    => N/A */

	TRY(xmlOutputBufferWrite(out, strlen("<?"), "<?"));
	TRY(xmlOutputBufferWriteString(out, (const char *) pi->name));
	TRY(xmlOutputBufferWrite(out, strlen(" "), " "));
	if (EXPECTED(pi->content != NULL)) {
		TRY(xmlOutputBufferWriteString(out, (const char *) pi->content));
	}
	return xmlOutputBufferWrite(out, strlen("?>"), "?>");
}

/* https://github.com/w3c/DOM-Parsing/issues/38
 * and https://github.com/w3c/DOM-Parsing/blob/ab8d1ac9699ed43ae6de9f4be2b0f3cfc5f3709e/index.html#L1510 */
static int dom_xml_serialize_cdata_section_node(xmlOutputBufferPtr out, xmlNodePtr cdata)
{
	TRY(xmlOutputBufferWrite(out, strlen("<![CDATA["), "<![CDATA["));
	if (EXPECTED(cdata->content != NULL)) {
		TRY(xmlOutputBufferWriteString(out, (const char *) cdata->content));
	}
	return xmlOutputBufferWrite(out, strlen("]]>"), "]]>");
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serialization-of-the-attributes */
static int dom_xml_serialize_attributes(
	xmlOutputBufferPtr out,
	xmlNodePtr element,
	dom_xml_ns_prefix_map *map,
	dom_xml_local_prefix_map *local_prefixes_map,
	unsigned int *prefix_index,
	bool ignore_namespace_definition_attribute
)
{
	/* 1. Let result be the empty string.
	 *    => We're going to write directly to the output buffer. */

	/* 2. Let localname set be a new empty namespace localname set.
	 *    => N/A this is only required for well-formedness */

	/* 3. [LOOP] For each attribute attr in element's attributes, in the order they are specified in the element's attribute list: */
	for (xmlAttrPtr attr = element->properties; attr != NULL; attr = attr->next) {
		/* 3.1. If the require well-formed flag is set ...
		 *      => N/A */

		/* 3.2. Create a new tuple consisting of attr's namespaceURI attribute and localName attribute, and add it to the localname set.
		 *      => N/A this is only required for well-formedness */

		/* 3.3. Let attribute namespace be the value of attr's namespaceURI value. */
		const xmlChar *attribute_namespace = attr->ns == NULL ? NULL : attr->ns->href;

		/* 3.4. Let candidate prefix be null. */
		const xmlChar *candidate_prefix = NULL;

		/* 3.5. If attribute namespace is not null, then run these sub-steps: */
		if (attribute_namespace != NULL) {
			/* 3.5.1. Let candidate prefix be the result of retrieving a preferred prefix string from map
			 *        given namespace attribute namespace with preferred prefix being attr's prefix value. */
			candidate_prefix = dom_retrieve_a_preferred_prefix_string(
				map,
				local_prefixes_map,
				attr->ns->prefix,
				attribute_namespace,
				strlen((const char *) attribute_namespace)
			);

			/* 3.5.2. If the value of attribute namespace is the XMLNS namespace, then run these steps: */
			if (dom_ns_is_fast((xmlNodePtr) attr, dom_ns_is_xmlns_magic_token)) {
				const xmlChar *attr_value = dom_get_attribute_value(attr);

				/* 3.5.2.1. If any of the following are true, then stop running these steps and goto Loop to visit the next attribute: */
				/* the attr's value is the XML namespace; */
				if (strcmp((const char *) attr_value, DOM_XML_NS_URI) == 0) {
					continue;
				}
				/* the attr's prefix is null and the ignore namespace definition attribute flag is true */
				if (ignore_namespace_definition_attribute && attr->ns->prefix == NULL) {
					/* https://github.com/w3c/DOM-Parsing/issues/47 */
					if (!dom_xml_str_equals_treat_nulls_as_empty(element->ns == NULL ? NULL : element->ns->href, attr_value)) {
						continue;
					}
				}
				/* the attr's prefix is not null and either */
				if (attr->ns->prefix != NULL) {
					/* the attr's localName is not a key contained in the local prefixes map
					 * or the attr's localName is present in the local prefixes map but the value of the key does not match attr's value
					 * and furthermore that the attr's localName (as the prefix to find) is found in the namespace prefix map
					 * given the namespace consisting of the attr's value */
					const xmlChar *value = dom_xml_local_prefix_map_find(local_prefixes_map, attr->name, strlen((const char *) attr->name));
					if (value == NULL || strcmp((const char *) value, (const char *) attr_value) != 0) {
						if (dom_prefix_found_in_ns_prefix_map(map, attr->name, attr_value, strlen((const char *) attr_value))) {
							continue;
						}
					}
				}

				/* 3.5.2.2. If the require well-formed flag is set ...
		 		 *      => N/A */
				/* 3.5.2.3. If the require well-formed flag is set ...
		 		 *      => N/A */

				/* 3.5.2.4. the attr's prefix matches the string "xmlns", then let candidate prefix be the string "xmlns". */
				if (attr->ns->prefix != NULL && strcmp((const char *) attr->ns->prefix, "xmlns") == 0) {
					candidate_prefix = BAD_CAST "xmlns";
				}
			}
			/* 3.5.3. Otherwise, the attribute namespace in not the XMLNS namespace. Run these steps: */
			else if (candidate_prefix == NULL) { /* https://github.com/w3c/DOM-Parsing/issues/29 */
				/* Continuation of https://github.com/w3c/DOM-Parsing/issues/29 */
				if (attr->ns->prefix == NULL
					|| dom_xml_local_prefix_map_contains(local_prefixes_map, attr->ns->prefix, strlen((const char *) attr->ns->prefix))) {
					/* 3.5.3.1. Let candidate prefix be the result of generating a prefix providing map,
					 *          attribute namespace, and prefix index as input. */
					candidate_prefix = dom_xml_generate_a_prefix(
						map,
						local_prefixes_map,
						attribute_namespace,
						strlen((const char *) attribute_namespace),
						prefix_index
					);
				} else {
					candidate_prefix = attr->ns->prefix;
					/* Continuation of https://github.com/w3c/DOM-Parsing/issues/29 */
					dom_xml_ns_prefix_map_add(
						map,
						candidate_prefix,
						false,
						attribute_namespace,
						strlen((const char *) attribute_namespace)
					);
					dom_xml_local_prefix_map_add(
						local_prefixes_map,
						candidate_prefix,
						strlen((const char *) candidate_prefix),
						attribute_namespace
					);
				}

				/* 3.5.3.2. Append the following to result, in the order listed: */
				TRY(xmlOutputBufferWrite(out, strlen(" xmlns:"), " xmlns:"));
				TRY(xmlOutputBufferWriteString(out, (const char *) candidate_prefix));
				TRY(xmlOutputBufferWrite(out, strlen("=\""), "=\""));
				TRY(dom_xml_common_text_serialization(out, (const char *) attribute_namespace, true));
				TRY(xmlOutputBufferWrite(out, strlen("\""), "\""));
			}
		}

		/* 3.6. Append a " " (U+0020 SPACE) to result. */
		TRY(xmlOutputBufferWrite(out, strlen(" "), " "));

		/* 3.7. If candidate prefix is not null, then append to result the concatenation of candidate prefix with ":" (U+003A COLON). */
		if (candidate_prefix != NULL) {
			TRY(xmlOutputBufferWriteString(out, (const char *) candidate_prefix));
			TRY(xmlOutputBufferWrite(out, strlen(":"), ":"));
		}

		/* 3.8. If the require well-formed flag is set ...
		 *      => N/A */

		/* 3.9. Append the following strings to result, in the order listed: */
		TRY(xmlOutputBufferWriteString(out, (const char *) attr->name));
		TRY(xmlOutputBufferWrite(out, strlen("=\""), "=\""));
		TRY(dom_xml_common_text_serialization(out, (const char *) dom_get_attribute_value(attr), true));
		TRY(xmlOutputBufferWrite(out, strlen("\""), "\""));
	}

	/* 4. Return the value of result.
	 *    => We're writing directly to the output buffer. */
	return 0;
}

/* Only format output if there are no text/entityrefs/cdata nodes as children. */
static bool dom_xml_should_format_element(xmlNodePtr element)
{
	xmlNodePtr child = element->children;
	ZEND_ASSERT(child != NULL);
	do {
		if (child->type == XML_TEXT_NODE || child->type == XML_ENTITY_REF_NODE || child->type == XML_CDATA_SECTION_NODE) {
			return false;
		}
		child = child->next;
	} while (child != NULL);
	return true;
}

static int dom_xml_output_indents(xmlOutputBufferPtr out, int indent)
{
	TRY(xmlOutputBufferWrite(out, strlen("\n"), "\n"));
	for (int i = 0; i < indent; i++) {
		TRY(xmlOutputBufferWrite(out, strlen("  "), "  "));
	}
	return 0;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serializing-an-element-node */
static int dom_xml_serialize_element_node(
	xmlSaveCtxtPtr ctxt,
	xmlOutputBufferPtr out,
	const xmlChar *namespace,
	dom_xml_ns_prefix_map *namespace_prefix_map,
	xmlNodePtr element,
	unsigned int *prefix_index,
	int indent
)
{
	bool should_format = indent >= 0 && element->children != NULL && dom_xml_should_format_element(element);

	/* 1. If the require well-formed flag is set ...
	 *    => N/A */

	/* 2. Let markup be the string "<" (U+003C LESS-THAN SIGN). */
	TRY(xmlOutputBufferWrite(out, strlen("<"), "<"));

	/* 3. Let qualified name be an empty string.
	 *    => We're going to do it a bit differently.
	 *       To avoid string allocations, we're going to store the qualified name separately as prefix+name.
	 *       If the prefix is NULL then the qualified name will be == name, otherwise == prefix:name. */
	dom_qname_pair qualified_name = { NULL, NULL };

	/* 4. Let skip end tag be a boolean flag with value false. */
	bool skip_end_tag = false;

	/* 5. Let ignore namespace definition attribute be a boolean flag with value false. */
	bool ignore_namespace_definition_attribute = false;

	/* 6. Given prefix map, copy a namespace prefix map and let map be the result. */
	dom_xml_ns_prefix_map map;
	dom_xml_ns_prefix_map_copy(&map, namespace_prefix_map);

	/* 7. Let local prefixes map be an empty map. */
	dom_xml_local_prefix_map local_prefixes_map;
	dom_xml_local_prefix_map_ctor(&local_prefixes_map);

	/* 8. Let local default namespace be the result of recording the namespace information for node given map and local prefixes map. */
	const xmlChar *local_default_namespace = dom_recording_the_namespace_information(&map, &local_prefixes_map, element);

	/* 9. Let inherited ns be a copy of namespace. */
	const xmlChar *inherited_ns = namespace;

	/* 10. Let ns be the value of node's namespaceURI attribute. */
	const xmlChar *const ns = element->ns == NULL ? NULL : element->ns->href;

	/* 11. If inherited ns is equal to ns, then: */
	if (dom_xml_str_equals_treat_nulls_as_nulls(inherited_ns, ns)) {
		/* 11.1. If local default namespace is not null, then set ignore namespace definition attribute to true. */
		if (local_default_namespace != NULL) {
			ignore_namespace_definition_attribute = true;
		}

		/* 11.2. If ns is the XML namespace,
		 *       then append to qualified name the concatenation of the string "xml:" and the value of node's localName. */
		if (dom_ns_is_fast(element, dom_ns_is_xml_magic_token)) {
			qualified_name.prefix = BAD_CAST "xml";
			qualified_name.name = element->name;
		}
		/* 11.3. Otherwise, append to qualified name the value of node's localName. */
		else {
			qualified_name.name = element->name;
		}

		/* 11.4. Append the value of qualified name to markup. */
		TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));
	}
	/* 12. Otherwise, inherited ns is not equal to ns */
	else {
		/* 12.1. Let prefix be the value of node's prefix attribute. */
		const xmlChar *prefix = element->ns == NULL ? NULL : element->ns->prefix;

		/* 12.2. Let candidate prefix be the result of retrieving a preferred prefix string prefix from map given namespace ns. */
		/* https://github.com/w3c/DOM-Parsing/issues/52 */
		const xmlChar *candidate_prefix;
		if (prefix == NULL && dom_xml_str_equals_treat_nulls_as_empty(ns, local_default_namespace)) {
			candidate_prefix = NULL;
		} else {
			size_t ns_length = ns == NULL ? 0 : strlen((const char *) ns);
			candidate_prefix = dom_retrieve_a_preferred_prefix_string(&map, &local_prefixes_map, prefix, ns, ns_length);
		}

		/* 12.3. If the value of prefix matches "xmlns", then run the following steps: */
		if (prefix != NULL && strcmp((const char *) prefix, "xmlns") == 0) {
			/* Step 1 deals with well-formedness, which we don't implement here. */

			/* 12.3.2. Let candidate prefix be the value of prefix. */
			candidate_prefix = prefix;
		}

		/* 12.4. if candidate prefix is not null (a namespace prefix is defined which maps to ns), then: */
		if (candidate_prefix != NULL) {
			/* 12.4.1. Append to qualified name the concatenation of candidate prefix, ":" (U+003A COLON), and node's localName. */
			qualified_name.prefix = candidate_prefix;
			qualified_name.name = element->name;

			/* 12.4.2. If the local default namespace is not null (there exists a locally-defined default namespace declaration attribute)
			 *         and its value is not the XML namespace ... */
			if (local_default_namespace != NULL && strcmp((const char *) local_default_namespace, DOM_XML_NS_URI) != 0) {
				if (*local_default_namespace == '\0') {
					inherited_ns = NULL;
				} else {
					inherited_ns = local_default_namespace;
				}
			}

			/* 12.4.3. Append the value of qualified name to markup. */
			TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));
		}
		/* 12.5. Otherwise, if prefix is not null, then: */
		else if (prefix != NULL) {
			size_t ns_length = ns == NULL ? 0 : strlen((const char *) ns);

			/* 12.5.1. If the local prefixes map contains a key matching prefix, ... */
			size_t prefix_length = strlen((const char *) prefix);
			if (dom_xml_local_prefix_map_contains(&local_prefixes_map, prefix, prefix_length)) {
				prefix = dom_xml_generate_a_prefix(&map, &local_prefixes_map, ns, ns_length, prefix_index);
			} else { /* else branch fixes spec issue: generating a prefix already adds it to the maps. */
				/* 12.5.2. Add prefix to map given namespace ns. */
				dom_xml_ns_prefix_map_add(&map, prefix, false, ns, ns_length);
				/* This is not spelled out in spec, but we have to do this to avoid conflicts (see default_namespace_move.phpt). */
				dom_xml_local_prefix_map_add(&local_prefixes_map, prefix, prefix_length, ns);
			}

			/* 12.5.3. Append to qualified name the concatenation of prefix, ":" (U+003A COLON), and node's localName. */
			qualified_name.prefix = prefix;
			qualified_name.name = element->name;

			/* 12.5.4. Append the value of qualified name to markup. */
			TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));

			/* 12.5.5. Append the following to markup, in the order listed: ... */
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen(" xmlns:"), " xmlns:"));
			TRY_OR_CLEANUP(xmlOutputBufferWriteString(out, (const char *) prefix));
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen("=\""), "=\""));
			TRY_OR_CLEANUP(dom_xml_common_text_serialization(out, (const char *) ns, true));
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen("\""), "\""));

			/* 12.5.6. If local default namespace is not null ... (editorial numbering error: https://github.com/w3c/DOM-Parsing/issues/43) */
			if (local_default_namespace != NULL) {
				if (*local_default_namespace == '\0') {
					inherited_ns = NULL;
				} else {
					inherited_ns = local_default_namespace;
				}
			}
		}
		/* 12.6. Otherwise, if local default namespace is null, or local default namespace is not null and its value is not equal to ns, then: */
		/* Note: https://github.com/w3c/DOM-Parsing/issues/47 */
		else if (local_default_namespace == NULL || !dom_xml_str_equals_treat_nulls_as_empty(local_default_namespace, ns)) {
			/* 12.6.1. Set the ignore namespace definition attribute flag to true. */
			ignore_namespace_definition_attribute = true;

			/* 12.6.2. Append to qualified name the value of node's localName. */
			qualified_name.name = element->name;

			/* 12.6.3. Let the value of inherited ns be ns. */
			inherited_ns = ns;

			/* 12.6.4. Append the value of qualified name to markup. */
			TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));

			/* 12.6.5. Append the following to markup, in the order listed: ... */
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen(" xmlns=\""), " xmlns=\""));
			TRY_OR_CLEANUP(dom_xml_common_text_serialization(out, (const char *) ns, true));
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen("\""), "\""));
		}
		/* 12.7. Otherwise, the node has a local default namespace that matches ns ... */
		else {
			qualified_name.name = element->name;
			inherited_ns = ns;
			TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));
		}
	}

	/* 13. Append to markup the result of the XML serialization of node's attributes given map, prefix index,
	 *     local prefixes map, ignore namespace definition attribute flag, and require well-formed flag. */
	TRY_OR_CLEANUP(dom_xml_serialize_attributes(out, element, &map, &local_prefixes_map, prefix_index, ignore_namespace_definition_attribute));

	/* 14. If ns is the HTML namespace, and the node's list of children is empty, and the node's localName matches
	 *     any one of the following void elements: ... */
	if (element->children == NULL) {
		if (xmlSaveNoEmptyTags) {
			/* Do nothing, use the <x></x> closing style. */
		} else if (dom_ns_is_fast(element, dom_ns_is_html_magic_token)) {
			size_t name_length = strlen((const char *) element->name);
			if (dom_local_name_compare_ex(element, "area", strlen("area"), name_length)
				|| dom_local_name_compare_ex(element, "base", strlen("base"), name_length)
				|| dom_local_name_compare_ex(element, "basefont", strlen("basefont"), name_length)
				|| dom_local_name_compare_ex(element, "bgsound", strlen("bgsound"), name_length)
				|| dom_local_name_compare_ex(element, "br", strlen("br"), name_length)
				|| dom_local_name_compare_ex(element, "col", strlen("col"), name_length)
				|| dom_local_name_compare_ex(element, "embed", strlen("embed"), name_length)
				|| dom_local_name_compare_ex(element, "frame", strlen("frame"), name_length)
				|| dom_local_name_compare_ex(element, "hr", strlen("hr"), name_length)
				|| dom_local_name_compare_ex(element, "img", strlen("img"), name_length)
				|| dom_local_name_compare_ex(element, "input", strlen("input"), name_length)
				|| dom_local_name_compare_ex(element, "keygen", strlen("keygen"), name_length)
				|| dom_local_name_compare_ex(element, "link", strlen("link"), name_length)
				|| dom_local_name_compare_ex(element, "menuitem", strlen("menuitem"), name_length)
				|| dom_local_name_compare_ex(element, "meta", strlen("meta"), name_length)
				|| dom_local_name_compare_ex(element, "param", strlen("param"), name_length)
				|| dom_local_name_compare_ex(element, "source", strlen("source"), name_length)
				|| dom_local_name_compare_ex(element, "track", strlen("track"), name_length)
				|| dom_local_name_compare_ex(element, "wbr", strlen("wbr"), name_length)) {
				TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen(" /"), " /"));
				skip_end_tag = true;
			}
		} else {
			/* 15. If ns is not the HTML namespace, and the node's list of children is empty,
			 *     then append "/" (U+002F SOLIDUS) to markup and set the skip end tag flag to true. */
			TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen("/"), "/"));
			skip_end_tag = true;
		}
	}

	/* 16. Append ">" (U+003E GREATER-THAN SIGN) to markup. */
	TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen(">"), ">"));

	/* 17. If the value of skip end tag is true, then return the value of markup and skip the remaining steps. */
	if (!skip_end_tag) {
		/* Step 18 deals with template elements which we don't support. */

		if (should_format) {
			indent++;
		} else {
			indent = -1;
		}

		/* 19. Otherwise, append to markup the result of running the XML serialization algorithm on each of node's children. */
		for (xmlNodePtr child = element->children; child != NULL; child = child->next) {
			if (should_format) {
				TRY_OR_CLEANUP(dom_xml_output_indents(out, indent));
			}
			TRY_OR_CLEANUP(dom_xml_serialization_algorithm(ctxt, out, &map, child, inherited_ns, prefix_index, indent));
		}

		if (should_format) {
			indent--;
			TRY_OR_CLEANUP(dom_xml_output_indents(out, indent));
		}

		/* 20. Append the following to markup, in the order listed: */
		TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen("</"), "</"));
		TRY_OR_CLEANUP(dom_xml_output_qname(out, &qualified_name));
		TRY_OR_CLEANUP(xmlOutputBufferWrite(out, strlen(">"), ">"));
	}

	/* 21. Return the value of markup.
	 *     => We use the output buffer instead. */
	dom_xml_ns_prefix_map_dtor(&map);
	dom_xml_local_prefix_map_dtor(&local_prefixes_map);
	return 0;

cleanup:
	dom_xml_ns_prefix_map_dtor(&map);
	dom_xml_local_prefix_map_dtor(&local_prefixes_map);
	return -1;
}

/* https://w3c.github.io/DOM-Parsing/#xml-serializing-a-documentfragment-node */
static int dom_xml_serializing_a_document_fragment_node(
	xmlSaveCtxtPtr ctxt,
	xmlOutputBufferPtr out,
	dom_xml_ns_prefix_map *namespace_prefix_map,
	xmlNodePtr node,
	const xmlChar *namespace,
	unsigned int *prefix_index,
	int indent
)
{
	/* 1. Let markup the empty string. 
	 *    => We use the output buffer instead. */

	/* 2. For each child child of node, in tree order, run the XML serialization algorithm on the child ... */
	xmlNodePtr child = node->children;
	while (child != NULL) {
		TRY(dom_xml_serialization_algorithm(ctxt, out, namespace_prefix_map, child, namespace, prefix_index, indent));
		child = child->next;
	}

	/* 3. Return the value of markup
	 *    => We use the output buffer instead. */
	return 0;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serializing-a-document-node */
static int dom_xml_serializing_a_document_node(
	xmlSaveCtxtPtr ctxt,
	xmlOutputBufferPtr out,
	dom_xml_ns_prefix_map *namespace_prefix_map,
	xmlNodePtr node,
	const xmlChar *namespace,
	unsigned int *prefix_index,
	int indent
)
{
	/* 1. Let serialized document be an empty string.
	 *    => We use the output buffer instead. */

	xmlNodePtr child = node->children;
	node->children = NULL;

	/* https://github.com/w3c/DOM-Parsing/issues/50 */
	TRY(xmlOutputBufferFlush(out));
	TRY(xmlSaveDoc(ctxt, (xmlDocPtr) node));
	TRY(xmlSaveFlush(ctxt));

	node->children = child;

	/* 2. For each child child of node, in tree order, run the XML serialization algorithm on the child passing along the provided arguments,
	 *    and append the result to serialized document. */
	while (child != NULL) {
		TRY(dom_xml_serialization_algorithm(ctxt, out, namespace_prefix_map, child, namespace, prefix_index, indent));
		child = child->next;
	}

	/* 3. Return the value of serialized document.
	 *    => We use the output buffer instead. */
	return 0;
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serialization-algorithm */
static int dom_xml_serialization_algorithm(
	xmlSaveCtxtPtr ctxt,
	xmlOutputBufferPtr out,
	dom_xml_ns_prefix_map *namespace_prefix_map,
	xmlNodePtr node,
	const xmlChar *namespace,
	unsigned int *prefix_index,
	int indent
)
{
	/* If node's interface is: */
	switch (node->type) {
		case XML_ELEMENT_NODE:
			return dom_xml_serialize_element_node(ctxt, out, namespace, namespace_prefix_map, node, prefix_index, indent);

		case XML_DOCUMENT_FRAG_NODE:
			return dom_xml_serializing_a_document_fragment_node(ctxt, out, namespace_prefix_map, node, namespace, prefix_index, indent);

		case XML_HTML_DOCUMENT_NODE:
		case XML_DOCUMENT_NODE:
			return dom_xml_serializing_a_document_node(ctxt, out, namespace_prefix_map, node, namespace, prefix_index, indent);

		case XML_TEXT_NODE:
			return dom_xml_serialize_text_node(out, node);

		case XML_COMMENT_NODE:
			return dom_xml_serialize_comment_node(out, node);

		case XML_PI_NODE:
			return dom_xml_serialize_processing_instruction(out, node);

		case XML_CDATA_SECTION_NODE:
			return dom_xml_serialize_cdata_section_node(out, node);

		case XML_ATTRIBUTE_NODE:
			return dom_xml_serialize_attribute_node(out, node);

		default:
			TRY(xmlOutputBufferFlush(out));
			TRY(xmlSaveTree(ctxt, node));
			TRY(xmlSaveFlush(ctxt));
			if (node->type == XML_DTD_NODE) {
				return xmlOutputBufferWrite(out, strlen("\n"), "\n");
			}
			return 0;
	}

	ZEND_UNREACHABLE();
}

/* https://w3c.github.io/DOM-Parsing/#dfn-xml-serialization
 * Assumes well-formed == false. */
int dom_xml_serialize(xmlSaveCtxtPtr ctxt, xmlOutputBufferPtr out, xmlNodePtr node, bool format)
{
	/* 1. Let namespace be a context namespace with value null. */
	const xmlChar *namespace = NULL;

	/* 2. Let prefix map be a new namespace prefix map. */
	dom_xml_ns_prefix_map namespace_prefix_map;
	dom_xml_ns_prefix_map_ctor(&namespace_prefix_map);

	/* 3. Add the XML namespace with prefix value "xml" to prefix map. */
	dom_xml_ns_prefix_map_add(&namespace_prefix_map, BAD_CAST "xml", false, BAD_CAST DOM_XML_NS_URI, strlen(DOM_XML_NS_URI));

	/* 4. Let prefix index be a generated namespace prefix index with value 1. */
	unsigned int prefix_index = 1;

	/* 5. Return the result of running the XML serialization algorithm ... */
	int indent = format ? 0 : -1;
	int result = dom_xml_serialization_algorithm(ctxt, out, &namespace_prefix_map, node, namespace, &prefix_index, indent);

	dom_xml_ns_prefix_map_dtor(&namespace_prefix_map);

	return result;
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
