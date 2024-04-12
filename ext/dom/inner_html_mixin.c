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
#include "dom_properties.h"
#include "html5_parser.h"
#include "html5_serializer.h"
#include "xml_serializer.h"
#include "domexception.h"
#include <libxml/xmlsave.h>
#include <lexbor/dom/interfaces/element.h>
#include <lexbor/html/interfaces/document.h>
#include <lexbor/tag/tag.h>
#include <lexbor/encoding/encoding.h>

/* Spec date: 2024-04-14 */

static zend_result dom_inner_html_write_string(void *application_data, const char *buf)
{
	smart_str *output = application_data;
	smart_str_appends(output, buf);
	return SUCCESS;
}

static zend_result dom_inner_html_write_string_len(void *application_data, const char *buf, size_t len)
{
	smart_str *output = application_data;
	smart_str_appendl(output, buf, len);
	return SUCCESS;
}

static int dom_write_smart_str(void *context, const char *buffer, int len)
{
	smart_str *str = context;
	smart_str_appendl(str, buffer, len);
	return len;
}

/* https://w3c.github.io/DOM-Parsing/#the-innerhtml-mixin
 * and https://w3c.github.io/DOM-Parsing/#dfn-fragment-serializing-algorithm */
zend_result dom_element_inner_html_read(dom_object *obj, zval *retval)
{
	DOM_PROP_NODE(xmlNodePtr, node, obj);

	/* 1. Let context document be the value of node's node document. */
	const xmlDoc *context_document = node->doc;

	/* 2. If context document is an HTML document, return an HTML serialization of node. */
	if (context_document->type == XML_HTML_DOCUMENT_NODE) {
		smart_str output = {0};
		dom_html5_serialize_context ctx;
		ctx.application_data = &output;
		ctx.write_string = dom_inner_html_write_string;
		ctx.write_string_len = dom_inner_html_write_string_len;
		dom_html5_serialize(&ctx, node);
		ZVAL_STR(retval, smart_str_extract(&output));
	}
	/* 3. Otherwise, context document is an XML document; return an XML serialization of node passing the flag require well-formed. */
	else {
		ZEND_ASSERT(context_document->type == XML_DOCUMENT_NODE);

		int status = -1;
		smart_str str = {0};
		/* No need to check buf's return value, as xmlSaveToBuffer() will fail instead. */
		xmlSaveCtxtPtr ctxt = xmlSaveToIO(dom_write_smart_str, NULL, &str, "UTF-8", XML_SAVE_AS_XML);
		if (EXPECTED(ctxt != NULL)) {
			xmlCharEncodingHandlerPtr handler = xmlFindCharEncodingHandler("UTF-8");
			xmlOutputBufferPtr out = xmlOutputBufferCreateIO(dom_write_smart_str, NULL, &str, handler);
			if (EXPECTED(out != NULL)) {
				/* Note: the innerHTML mixin sets the well-formed flag to true. */
				xmlNodePtr child = node->children;
				status = 0;
				while (child != NULL && status == 0) {
					status = dom_xml_serialize(ctxt, out, child, false, true);
					child = child->next;
				}
				status |= xmlOutputBufferFlush(out);
				status |= xmlOutputBufferClose(out);
			}
			(void) xmlSaveClose(ctxt);
			xmlCharEncCloseFunc(handler);
		}
		if (UNEXPECTED(status < 0)) {
			smart_str_free_ex(&str, false);
			php_dom_throw_error_with_message(SYNTAX_ERR, "The resulting XML serialization is not well-formed", true);
			return FAILURE;
		}
		ZVAL_STR(retval, smart_str_extract(&str));
	}

	return SUCCESS;
}

static lxb_dom_node_t *dom_html_fragment_lexbor_parse(lxb_html_document_t *document, lxb_dom_element_t *element, const zend_string *input)
{
	lxb_status_t status = lxb_html_document_parse_fragment_chunk_begin(document, element);
	if (status != LXB_STATUS_OK) {
		return NULL;
	}

	const lxb_encoding_data_t *encoding_data = lxb_encoding_data(LXB_ENCODING_UTF_8);
	lxb_encoding_decode_t decode;
	lxb_encoding_decode_init_single(&decode, encoding_data);

	const lxb_char_t *buf_ref = (const lxb_char_t *) ZSTR_VAL(input);
	if (ZSTR_IS_VALID_UTF8(input)) {
		/* If we know the input is valid UTF-8, we don't have to perform checks and replace invalid sequences. */
		status = lxb_html_document_parse_fragment_chunk(document, buf_ref, ZSTR_LEN(input));
		if (UNEXPECTED(status != LXB_STATUS_OK)) {
			return NULL;
		}
	} else {
		/* See dom_decode_encode_fast_path(), simplified version for in-memory use-case. */
		const lxb_char_t *buf_end = buf_ref + ZSTR_LEN(input);
		const lxb_char_t *last_output = buf_ref;
		while (buf_ref < buf_end) {
			if (decode.u.utf_8.need == 0 && *buf_ref < 0x80) {
				buf_ref++;
				continue;
			}

			const lxb_char_t *buf_ref_backup = buf_ref;
			lxb_codepoint_t codepoint = lxb_encoding_decode_utf_8_single(&decode, &buf_ref, buf_end);
			if (UNEXPECTED(codepoint > LXB_ENCODING_MAX_CODEPOINT)) {
				status = lxb_html_document_parse_fragment_chunk(document, last_output, buf_ref_backup - last_output);
				if (UNEXPECTED(status != LXB_STATUS_OK)) {
					return NULL;
				}

				status = lxb_html_document_parse_fragment_chunk(document, LXB_ENCODING_REPLACEMENT_BYTES, LXB_ENCODING_REPLACEMENT_SIZE);
				if (UNEXPECTED(status != LXB_STATUS_OK)) {
					return NULL;
				}

				last_output = buf_ref;
			}
		}

		if (buf_ref != last_output) {
			status = lxb_html_document_parse_fragment_chunk(document, last_output, buf_ref - last_output);
			if (UNEXPECTED(status != LXB_STATUS_OK)) {
				return NULL;
			}
		}
	}

	return lxb_html_document_parse_fragment_chunk_end(document);
}

static lxb_dom_document_cmode_t dom_translate_quirks_mode(php_libxml_quirks_mode quirks_mode)
{
	switch (quirks_mode) {
		case PHP_LIBXML_NO_QUIRKS: return LXB_DOM_DOCUMENT_CMODE_NO_QUIRKS;
		case PHP_LIBXML_LIMITED_QUIRKS: return LXB_DOM_DOCUMENT_CMODE_LIMITED_QUIRKS;
		case PHP_LIBXML_QUIRKS: return LXB_DOM_DOCUMENT_CMODE_QUIRKS;
		EMPTY_SWITCH_DEFAULT_CASE();
	}
}

/* https://html.spec.whatwg.org/#html-fragment-parsing-algorithm */
static xmlNodePtr dom_html_fragment_parsing_algorithm(dom_object *obj, xmlNodePtr context_node, const zend_string *input, php_libxml_quirks_mode quirks_mode)
{
	/* The whole algorithm is implemented in Lexbor, we just have to be the adapter between the
	 * data structures used in PHP and what Lexbor expects. */

	lxb_html_document_t *document = lxb_html_document_create();
	document->dom_document.compat_mode = dom_translate_quirks_mode(quirks_mode);
	lxb_dom_element_t *element = lxb_dom_element_interface_create(&document->dom_document);

	const lxb_tag_data_t *tag_data = lxb_tag_data_by_name(document->dom_document.tags, (lxb_char_t *) context_node->name, xmlStrlen(context_node->name));
	element->node.local_name = tag_data == NULL ? LXB_TAG__UNDEF : tag_data->tag_id;

	const lxb_char_t *ns_uri;
	size_t ns_uri_len;
	if (context_node->ns == NULL || context_node->ns->href == NULL) {
		ns_uri = (lxb_char_t *) "";
		ns_uri_len = 0;
	} else {
		ns_uri = context_node->ns->href;
		ns_uri_len = xmlStrlen(ns_uri);
	}
	const lxb_ns_data_t *ns_data = lxb_ns_data_by_link(document->dom_document.ns, ns_uri, ns_uri_len);
	element->node.ns = ns_data == NULL ? LXB_NS__UNDEF : ns_data->ns_id;

	lxb_dom_node_t *node = dom_html_fragment_lexbor_parse(document, element, input);
	xmlNodePtr fragment = NULL;
	if (node != NULL) {
		/* node->last_child could be NULL, but that is allowed. */
		lexbor_libxml2_bridge_status status = lexbor_libxml2_bridge_convert_fragment(node->last_child, context_node->doc, &fragment, true, true, php_dom_get_ns_mapper(obj));
		if (UNEXPECTED(status != LEXBOR_LIBXML2_BRIDGE_STATUS_OK)) {
			php_dom_throw_error(INVALID_STATE_ERR, true);
		}
	} else {
		php_dom_throw_error(INVALID_STATE_ERR, true);
	}

	lxb_html_document_destroy(document);

	return fragment;
}

static void dom_xml_parser_tag_name(const xmlNode *context_node, xmlParserCtxtPtr parser)
{
	if (context_node->ns != NULL && context_node->ns->prefix != NULL) {
		xmlParseChunk(parser, (const char *) context_node->ns->prefix, xmlStrlen(context_node->ns->prefix), 0);
		xmlParseChunk(parser, ":", 1, 0);
	}

	xmlParseChunk(parser, (const char *) context_node->name, xmlStrlen(context_node->name), 0);
}

static void dom_xml_fragment_parsing_algorithm_parse(php_dom_libxml_ns_mapper *ns_mapper, const xmlNode *context_node, const zend_string *input, xmlParserCtxtPtr parser)
{
	xmlParseChunk(parser, "<", 1, 0);
	dom_xml_parser_tag_name(context_node, parser);

	/* Namespaces: we have to declare all in-scope namespaces including the default namespace */
	/* xmlns attributes */
	php_dom_in_scope_ns in_scope_ns = php_dom_get_in_scope_ns(ns_mapper, context_node, true);
	for (size_t i = 0; i < in_scope_ns.count; i++) {
		const xmlNs *ns = in_scope_ns.list[i];
		xmlParseChunk(parser, " xmlns:", 7, 0);
		ZEND_ASSERT(ns->prefix != NULL);
		xmlParseChunk(parser, (const char *) ns->prefix, xmlStrlen(ns->prefix), 0);
		xmlParseChunk(parser, "=\"", 2, 0);
		xmlParseChunk(parser, (const char *) ns->href, xmlStrlen(ns->href), 0);
		xmlParseChunk(parser, "\"", 1, 0);
	}
	php_dom_in_scope_ns_destroy(&in_scope_ns);
	/* default namespace */
	const char *default_ns = dom_locate_a_namespace(context_node, NULL);
	if (default_ns != NULL) {
		xmlParseChunk(parser, " xmlns=\"", 8, 0);
		xmlParseChunk(parser, default_ns, strlen(default_ns), 0);
		xmlParseChunk(parser, "\"", 1, 0);
	}

	xmlParseChunk(parser, ">", 1, 0);

	xmlParseChunk(parser, (const char *) ZSTR_VAL(input), ZSTR_LEN(input), 0);

	xmlParseChunk(parser, "</", 2, 0);
	dom_xml_parser_tag_name(context_node, parser);
	xmlParseChunk(parser, ">", 1, 1);
}

/* https://html.spec.whatwg.org/#xml-fragment-parsing-algorithm */
static xmlNodePtr dom_xml_fragment_parsing_algorithm(dom_object *obj, const xmlNode *context_node, const zend_string *input)
{
	/* Steps 1-4 below */
	xmlParserCtxtPtr parser = xmlCreatePushParserCtxt(NULL, NULL, NULL, 0, NULL);
	if (UNEXPECTED(parser == NULL)) {
		php_dom_throw_error(INVALID_STATE_ERR, true);
		return NULL;
	}

	/* This is not only good to avoid a performance cost of changing the tree, but also to work around an old bug
	 * in xmlSetTreeDoc(). */
	xmlDictFree(parser->dict);
	if (context_node->doc->dict == NULL) {
		context_node->doc->dict = xmlDictCreate();
		xmlDictSetLimit(context_node->doc->dict, XML_MAX_DICTIONARY_LIMIT);
	}
	parser->dict = context_node->doc->dict;

	php_libxml_sanitize_parse_ctxt_options(parser);
	xmlCtxtUseOptions(parser, XML_PARSE_IGNORE_ENC | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);

	xmlCharEncodingHandlerPtr encoding = xmlFindCharEncodingHandler("UTF-8");
	(void) xmlSwitchToEncoding(parser, encoding);

	php_dom_libxml_ns_mapper *ns_mapper = php_dom_get_ns_mapper(obj);
	dom_xml_fragment_parsing_algorithm_parse(ns_mapper, context_node, input, parser);

	/* 5. If there is an XML well-formedness or XML namespace well-formedness error, then throw a "SyntaxError" DOMException. */
	if (!parser->wellFormed || !parser->nsWellFormed) {
		parser->dict = NULL;
		xmlFreeDoc(parser->myDoc);
		xmlFreeParserCtxt(parser);
		php_dom_throw_error_with_message(SYNTAX_ERR, "XML fragment is not well-formed", true);
		return NULL;
	}

	xmlDocPtr doc = parser->myDoc;
	xmlFreeParserCtxt(parser);

	if (EXPECTED(doc != NULL)) {
		doc->dict = NULL;

		/* 6. If the document element of the resulting Document has any sibling nodes, then throw a "SyntaxError" DOMException. */
		xmlNodePtr document_element = doc->children;
		if (document_element == NULL || document_element->next != NULL) {
			xmlFreeDoc(doc);
			php_dom_throw_error_with_message(SYNTAX_ERR, "XML fragment is not well-formed", true);
			return NULL;
		}

		/* 7. Return the child nodes of the document element of the resulting Document, in tree order. */
		xmlNodePtr fragment = xmlNewDocFragment(context_node->doc);
		if (EXPECTED(fragment != NULL)) {
			xmlNodePtr child = document_element->children;
			/* Yes, we have to call both xmlSetTreeDoc() prior to xmlAddChildList()
			 * because xmlAddChildList() _only_ sets the tree for the topmost elements in the subtree! */
			xmlSetTreeDoc(document_element, context_node->doc);
			xmlAddChildList(fragment, child);
			dom_mark_namespaces_as_attributes_too(ns_mapper, doc);
			document_element->children = NULL;
			document_element->last = NULL;
		}
		xmlFreeDoc(doc);
		return fragment;
	}
	return NULL;
}

/* https://w3c.github.io/DOM-Parsing/#the-innerhtml-mixin
 * and https://w3c.github.io/DOM-Parsing/#dfn-fragment-parsing-algorithm */
zend_result dom_element_inner_html_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlNodePtr, context_node, obj);

	xmlNodePtr fragment;
	if (context_node->doc->type == XML_DOCUMENT_NODE) {
		fragment = dom_xml_fragment_parsing_algorithm(obj, context_node, Z_STR_P(newval));
	} else {
		fragment = dom_html_fragment_parsing_algorithm(obj, context_node, Z_STR_P(newval), obj->document->quirks_mode);
	}

	if (fragment == NULL) {
		return FAILURE;
	}

	/* We skip the steps involving the template element as context node since we don't do special handling for that. */
	dom_remove_all_children(context_node);
	return php_dom_pre_insert(obj->document, fragment, context_node, NULL) ? SUCCESS : FAILURE;
}

#endif
