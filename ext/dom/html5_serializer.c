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
#include "html5_serializer.h"
#include "namespace_compat.h"
#include <lexbor/encoding/encoding.h>

#define TRY(x) do { if (UNEXPECTED((x) != SUCCESS)) { return FAILURE; } } while (0)

static bool dom_is_ns(const xmlNode *node, const char *uri)
{
	return node->ns != NULL && strcmp((const char *) node->ns->href, uri) == 0;
}

static bool dom_is_html_ns(const xmlNode *node)
{
	return node->ns == NULL || dom_is_ns(node, DOM_XHTML_NS_URI);
}

static bool dom_local_name_compare_ex(const xmlNode *node, const char *tag, size_t tag_length, size_t name_length)
{
	return name_length == tag_length && zend_binary_strcmp((const char *) node->name, name_length, tag, tag_length) == 0;
}

static zend_result dom_html5_serialize_doctype(dom_html5_serialize_context *ctx, const xmlDtd *dtd)
{
	TRY(ctx->write_string_len(ctx->application_data, "<!DOCTYPE ", strlen("<!DOCTYPE ")));
	TRY(ctx->write_string(ctx->application_data, (const char *) dtd->name));
	return ctx->write_string_len(ctx->application_data, ">", strlen(">"));
}

static zend_result dom_html5_serialize_comment(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	TRY(ctx->write_string_len(ctx->application_data, "<!--", strlen("<!--")));
	TRY(ctx->write_string(ctx->application_data, (const char *) node->content));
	return ctx->write_string_len(ctx->application_data, "-->", strlen("-->"));
}

static zend_result dom_html5_serialize_processing_instruction(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	TRY(ctx->write_string_len(ctx->application_data, "<?", strlen("<?")));
	TRY(ctx->write_string(ctx->application_data, (const char *) node->name));
	TRY(ctx->write_string_len(ctx->application_data, " ", strlen(" ")));
	TRY(ctx->write_string(ctx->application_data, (const char *) node->content));
	return ctx->write_string_len(ctx->application_data, ">", strlen(">"));
}

/* https://html.spec.whatwg.org/multipage/parsing.html#escapingString */
static zend_result dom_html5_escape_string(dom_html5_serialize_context *ctx, const char *content, bool attribute_mode)
{
	const char *last_output = content;

	while (*content != '\0') {
		switch (*content) {
			/* Step 1 */
			case '&': {
				TRY(ctx->write_string_len(ctx->application_data, last_output, content - last_output));
				TRY(ctx->write_string_len(ctx->application_data, "&amp;", strlen("&amp;")));
				last_output = content + 1;
				break;
			}

			/* Step 2 (non-breaking space) (note: uses UTF-8 internally) */
			case '\xC2': {
				if (content[1] == '\xA0') {
					TRY(ctx->write_string_len(ctx->application_data, last_output, content - last_output));
					TRY(ctx->write_string_len(ctx->application_data, "&nbsp;", strlen("&nbsp;")));
					content++; /* Consume A0 too */
					last_output = content + 1;
				}
				break;
			}

			/* Step 3 */
			case '"': {
				if (attribute_mode) {
					TRY(ctx->write_string_len(ctx->application_data, last_output, content - last_output));
					TRY(ctx->write_string_len(ctx->application_data, "&quot;", strlen("&quot;")));
					last_output = content + 1;
				}
				break;
			}

			/* Step 4 */
			case '<': {
				if (!attribute_mode) {
					TRY(ctx->write_string_len(ctx->application_data, last_output, content - last_output));
					TRY(ctx->write_string_len(ctx->application_data, "&lt;", strlen("&lt;")));
					last_output = content + 1;
				}
				break;
			}
			case '>': {
				if (!attribute_mode) {
					TRY(ctx->write_string_len(ctx->application_data, last_output, content - last_output));
					TRY(ctx->write_string_len(ctx->application_data, "&gt;", strlen("&gt;")));
					last_output = content + 1;
				}
				break;
			}
		}

		content++;
	}

	return ctx->write_string_len(ctx->application_data, last_output, content - last_output);
}

static zend_result dom_html5_serialize_text_node(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	if (node->parent->type == XML_ELEMENT_NODE && dom_is_html_ns(node->parent)) {
		const xmlNode *parent = node->parent;
		size_t name_length = strlen((const char *) parent->name);
		/* Spec tells us to only emit noscript content as-is if scripting is enabled.
		 * However, the user agent (PHP) does not support (JS) scripting.
		 * Furthermore, if actually consumed by a browser then we should err on the safe side and not emit the content as-is. */
		if (dom_local_name_compare_ex(parent, "style", strlen("style"), name_length)
			|| dom_local_name_compare_ex(parent, "script", strlen("script"), name_length)
			|| dom_local_name_compare_ex(parent, "xmp", strlen("xmp"), name_length)
			|| dom_local_name_compare_ex(parent, "iframe", strlen("iframe"), name_length)
			|| dom_local_name_compare_ex(parent, "noembed", strlen("noembed"), name_length)
			|| dom_local_name_compare_ex(parent, "noframes", strlen("noframes"), name_length)
			|| dom_local_name_compare_ex(parent, "plaintext", strlen("plaintext"), name_length)) {
			return ctx->write_string(ctx->application_data, (const char *) node->content);
		}
	}

	return dom_html5_escape_string(ctx, (const char *) node->content, false);
}

static zend_result dom_html5_serialize_element_tag_name(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	/* Note: it is not the serializer's responsibility to care about uppercase/lowercase (see createElement() note) */
	if (node->ns != NULL && node->ns->prefix != NULL
		&& !(dom_is_html_ns(node) || dom_is_ns(node, DOM_MATHML_NS_URI) || dom_is_ns(node, DOM_SVG_NS_URI))) {
		TRY(ctx->write_string(ctx->application_data, (const char *) node->ns->prefix));
		TRY(ctx->write_string_len(ctx->application_data, ":", strlen(":")));
	}
	return ctx->write_string(ctx->application_data, (const char *) node->name);
}

static zend_result dom_html5_serialize_element_start(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	TRY(ctx->write_string_len(ctx->application_data, "<", strlen("<")));
	TRY(dom_html5_serialize_element_tag_name(ctx, node));

	/* We don't support the "is" value during element creation, so no handling here. */

	/* Some namespace declarations are also attributes (see https://html.spec.whatwg.org/multipage/parsing.html#create-an-element-for-the-token) */
	for (const xmlNs *ns = node->nsDef; ns != NULL; ns = ns->next) {
		if (!dom_ns_is_also_an_attribute(ns)) {
			continue;
		}

		if (ns->prefix != NULL) {
			TRY(ctx->write_string_len(ctx->application_data, " xmlns:", strlen(" xmlns:")));
			TRY(ctx->write_string(ctx->application_data, (const char *) ns->prefix));
			TRY(ctx->write_string_len(ctx->application_data, "=\"", strlen("=\"")));
		} else {
			TRY(ctx->write_string_len(ctx->application_data, " xmlns=\"", strlen(" xmlns=\"")));
		}
		TRY(ctx->write_string(ctx->application_data, (const char *) ns->href));
		TRY(ctx->write_string_len(ctx->application_data, "\"", strlen("\"")));
	}

	for (const xmlAttr *attr = node->properties; attr; attr = attr->next) {
		TRY(ctx->write_string_len(ctx->application_data, " ", strlen(" ")));
		if (attr->ns == NULL) {
			TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
		} else {
			if (dom_is_ns((const xmlNode *) attr, DOM_XML_NS_URI)) {
				TRY(ctx->write_string_len(ctx->application_data, "xml:", strlen("xml:")));
				TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
			} else if (dom_is_ns((const xmlNode *) attr, DOM_XMLNS_NS_URI)) {
				/* Compatibility for real attributes */
				if (strcmp((const char *) attr->name, "xmlns") == 0) {
					TRY(ctx->write_string_len(ctx->application_data, "xmlns", strlen("xmlns")));
				} else {
					TRY(ctx->write_string_len(ctx->application_data, "xmlns:", strlen("xmlns:")));
					TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
				}
			} else if (dom_is_ns((const xmlNode *) attr, DOM_XLINK_NS_URI)) {
				TRY(ctx->write_string_len(ctx->application_data, "xlink:", strlen("xlink:")));
				TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
			} else if (attr->ns->prefix == NULL) {
				TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
			} else {
				TRY(ctx->write_string(ctx->application_data, (const char *) attr->ns->prefix));
				TRY(ctx->write_string_len(ctx->application_data, ":", strlen(":")));
				TRY(ctx->write_string(ctx->application_data, (const char *) attr->name));
			}
		}
		TRY(ctx->write_string_len(ctx->application_data, "=\"", strlen("=\"")));
		xmlChar *content = xmlNodeGetContent((const xmlNode *) attr);
		if (content != NULL) {
			zend_result result = dom_html5_escape_string(ctx, (const char *) content, true);
			xmlFree(content);
			TRY(result);
		}
		TRY(ctx->write_string_len(ctx->application_data, "\"", strlen("\"")));
	}

	return ctx->write_string_len(ctx->application_data, ">", strlen(">"));

	/* Note: "continue on to the next child if the element is void" is handled in the iteration and dom_html5_serialize_element_end() */
}

/* https://html.spec.whatwg.org/multipage/syntax.html#void-elements
 * https://html.spec.whatwg.org/multipage/parsing.html#serializes-as-void */
static bool dom_html5_serializes_as_void(const xmlNode *node)
{
	if (dom_is_html_ns(node)) {
		size_t name_length = strlen((const char *) node->name);
		if (/* These are the void elements from https://html.spec.whatwg.org/multipage/syntax.html#void-elements */
			dom_local_name_compare_ex(node, "area", strlen("area"), name_length)
			|| dom_local_name_compare_ex(node, "base", strlen("base"), name_length)
			|| dom_local_name_compare_ex(node, "br", strlen("br"), name_length)
			|| dom_local_name_compare_ex(node, "col", strlen("col"), name_length)
			|| dom_local_name_compare_ex(node, "embed", strlen("embed"), name_length)
			|| dom_local_name_compare_ex(node, "hr", strlen("hr"), name_length)
			|| dom_local_name_compare_ex(node, "img", strlen("img"), name_length)
			|| dom_local_name_compare_ex(node, "input", strlen("input"), name_length)
			|| dom_local_name_compare_ex(node, "link", strlen("link"), name_length)
			|| dom_local_name_compare_ex(node, "meta", strlen("meta"), name_length)
			|| dom_local_name_compare_ex(node, "source", strlen("source"), name_length)
			|| dom_local_name_compare_ex(node, "track", strlen("track"), name_length)
			|| dom_local_name_compare_ex(node, "wbr", strlen("wbr"), name_length)
			/* These are the additional names from https://html.spec.whatwg.org/multipage/parsing.html#serializes-as-void */
			|| dom_local_name_compare_ex(node, "basefont", strlen("basefont"), name_length)
			|| dom_local_name_compare_ex(node, "bgsound", strlen("bgsound"), name_length)
			|| dom_local_name_compare_ex(node, "frame", strlen("frame"), name_length)
			|| dom_local_name_compare_ex(node, "keygen", strlen("keygen"), name_length)
			|| dom_local_name_compare_ex(node, "param", strlen("param"), name_length)) {
			return true;
		}
	}
	return false;
}

static zend_result dom_html5_serialize_element_end(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	if (!dom_html5_serializes_as_void(node)) {
		TRY(ctx->write_string_len(ctx->application_data, "</", strlen("</")));
		TRY(dom_html5_serialize_element_tag_name(ctx, node));
		return ctx->write_string_len(ctx->application_data, ">", strlen(">"));
	}
	return SUCCESS;
}

/* https://html.spec.whatwg.org/multipage/parsing.html#html-fragment-serialisation-algorithm */
static zend_result dom_html5_serialize_node(dom_html5_serialize_context *ctx, const xmlNode *node, const xmlNode *bound)
{
	while (node != NULL) {
		switch (node->type) {
			case XML_DTD_NODE: {
				TRY(dom_html5_serialize_doctype(ctx, (const xmlDtd *) node));
				break;
			}

			case XML_CDATA_SECTION_NODE:
			case XML_TEXT_NODE: {
				TRY(dom_html5_serialize_text_node(ctx, node));
				break;
			}

			case XML_PI_NODE: {
				TRY(dom_html5_serialize_processing_instruction(ctx, node));
				break;
			}

			case XML_COMMENT_NODE: {
				TRY(dom_html5_serialize_comment(ctx, node));
				break;
			}

			case XML_ELEMENT_NODE: {
				TRY(dom_html5_serialize_element_start(ctx, node));
				if (node->children) {
					if (!dom_html5_serializes_as_void(node)) {
						node = node->children;
						continue;
					}
				} else {
					/* Not descended, so wouldn't put the closing tag as it's normally only done when going back upwards. */
					TRY(dom_html5_serialize_element_end(ctx, node));
				}
				break;
			}

			default:
				break;
		}

		if (node->next) {
			node = node->next;
		} else {
			/* Go upwards, until we find a parent node with a next sibling, or until we hit the bound. */
			do {
				node = node->parent;
				if (node == bound) {
					return SUCCESS;
				}
				if (node->type == XML_ELEMENT_NODE) {
					TRY(dom_html5_serialize_element_end(ctx, node));
				}
			} while (node->next == NULL);
			node = node->next;
		}
	}

	return SUCCESS;
}

/* https://html.spec.whatwg.org/multipage/parsing.html#serialising-html-fragments (Date 2023-10-18)
 * Note: this serializes the _children_, excluding the node itself! */
zend_result dom_html5_serialize(dom_html5_serialize_context *ctx, const xmlNode *node)
{
	/* Step 1. Note that this algorithm serializes children. Only elements, documents, and fragments can have children. */
	if (node->type != XML_ELEMENT_NODE && node->type != XML_DOCUMENT_FRAG_NODE && node->type != XML_DOCUMENT_NODE && node->type != XML_HTML_DOCUMENT_NODE) {
		return SUCCESS;
	}
	if (node->type == XML_ELEMENT_NODE && dom_html5_serializes_as_void(node)) {
		return SUCCESS;
	}

	/* Step 2 not needed because we're not using a string to store the serialized data */
	/* Step 3 not needed because we don't support template contents yet */

	/* Step 4 */
	return dom_html5_serialize_node(ctx, node->children, node);
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
