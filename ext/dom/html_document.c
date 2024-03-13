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
#include "html5_parser.h"
#include "html5_serializer.h"
#include "namespace_compat.h"
#include "dom_properties.h"
#include <Zend/zend_smart_string.h>
#include <lexbor/html/encoding.h>
#include <lexbor/encoding/encoding.h>

/* Implementation defined, but as HTML5 defaults in all other cases to UTF-8, we'll do the same. */
#define DOM_FALLBACK_ENCODING_NAME "UTF-8"
#define DOM_FALLBACK_ENCODING_ID LXB_ENCODING_UTF_8

typedef struct _dom_line_column_cache {
	size_t last_line;
	size_t last_column;
	size_t last_offset;
} dom_line_column_cache;

typedef struct _dom_lexbor_libxml2_bridge_application_data {
	const char *input_name;
	const lxb_codepoint_t *current_input_codepoints;
	const char *current_input_characters;
	size_t current_input_length;
	size_t current_total_offset;
	dom_line_column_cache cache_tokenizer;
	bool html_no_implied;
} dom_lexbor_libxml2_bridge_application_data;

typedef struct _dom_character_encoding_data {
	const lxb_encoding_data_t *encoding_data;
	size_t bom_shift;
} dom_character_encoding_data;

typedef zend_result (*dom_write_output)(void*, const char *, size_t);

typedef struct _dom_output_ctx {
	const lxb_encoding_data_t *encoding_data;
	const lxb_encoding_data_t *decoding_data;
	lxb_encoding_encode_t *encode;
	lxb_encoding_decode_t *decode;
	lxb_codepoint_t *codepoints;
	lxb_char_t *encoding_output;
	void *output_data;
	dom_write_output write_output;
} dom_output_ctx;

typedef struct _dom_decoding_encoding_ctx {
	/* We can skip some conversion if the input and output encoding are both UTF-8,
	 * we only have to validate and substitute replacement characters */
	bool fast_path; /* Put first, near the encode & decode structures, for cache locality */
	lxb_encoding_encode_t encode;
	lxb_encoding_decode_t decode;
	const lxb_encoding_data_t *encode_data;
	const lxb_encoding_data_t *decode_data;
	lxb_char_t encoding_output[4096];
	lxb_codepoint_t codepoints[4096];
} dom_decoding_encoding_ctx;

/* https://dom.spec.whatwg.org/#dom-document-implementation */
zend_result dom_modern_document_implementation_read(dom_object *obj, zval *retval)
{
	const uint32_t PROP_INDEX = 14;

#if ZEND_DEBUG
	zend_string *implementation_str = ZSTR_INIT_LITERAL("implementation", false);
	const zend_property_info *prop_info = zend_get_property_info(dom_abstract_base_document_class_entry, implementation_str, 0);
	zend_string_release_ex(implementation_str, false);
	ZEND_ASSERT(OBJ_PROP_TO_NUM(prop_info->offset) == PROP_INDEX);
#endif

	zval *cached_implementation = OBJ_PROP_NUM(&obj->std, PROP_INDEX);
	if (Z_ISUNDEF_P(cached_implementation)) {
		php_dom_create_implementation(cached_implementation, true);
	}

	ZVAL_OBJ_COPY(retval, Z_OBJ_P(cached_implementation));

	return SUCCESS;
}

static void dom_decoding_encoding_ctx_init(dom_decoding_encoding_ctx *ctx)
{
	ctx->encode_data = lxb_encoding_data(LXB_ENCODING_UTF_8);
	ctx->decode_data = NULL;
	/* Set fast path on by default so that the decoder finishing is skipped if this was never initialised properly. */
	ctx->fast_path = true;
	(void) lxb_encoding_encode_init(
		&ctx->encode,
		ctx->encode_data,
		ctx->encoding_output,
		sizeof(ctx->encoding_output) / sizeof(*ctx->encoding_output)
	);
	(void) lxb_encoding_encode_replace_set(&ctx->encode, LXB_ENCODING_REPLACEMENT_BYTES, LXB_ENCODING_REPLACEMENT_SIZE);
}

static const char *dom_lexbor_tokenizer_error_code_to_string(lxb_html_tokenizer_error_id_t id)
{
	switch (id) {
		case LXB_HTML_TOKENIZER_ERROR_ABCLOFEMCO: return "abrupt-closing-of-empty-comment";
		case LXB_HTML_TOKENIZER_ERROR_ABDOPUID: return "abrupt-doctype-public-identifier";
		case LXB_HTML_TOKENIZER_ERROR_ABDOSYID: return "abrupt-doctype-system-identifier";
		case LXB_HTML_TOKENIZER_ERROR_ABOFDIINNUCHRE: return "absence-of-digits-in-numeric-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_CDINHTCO: return "cdata-in-html-content";
		case LXB_HTML_TOKENIZER_ERROR_CHREOUUNRA: return "character-reference-outside-unicode-range";
		case LXB_HTML_TOKENIZER_ERROR_COCHININST: return "control-character-in-input-stream";
		case LXB_HTML_TOKENIZER_ERROR_COCHRE: return "control-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_ENTAWIAT: return "end-tag-with-attributes";
		case LXB_HTML_TOKENIZER_ERROR_DUAT: return "duplicate-attribute";
		case LXB_HTML_TOKENIZER_ERROR_ENTAWITRSO: return "end-tag-with-trailing-solidus";
		case LXB_HTML_TOKENIZER_ERROR_EOBETANA: return "eof-before-tag-name";
		case LXB_HTML_TOKENIZER_ERROR_EOINCD: return "eof-in-cdata";
		case LXB_HTML_TOKENIZER_ERROR_EOINCO: return "eof-in-comment";
		case LXB_HTML_TOKENIZER_ERROR_EOINDO: return "eof-in-doctype";
		case LXB_HTML_TOKENIZER_ERROR_EOINSCHTCOLITE: return "eof-in-script-html-comment-like-text";
		case LXB_HTML_TOKENIZER_ERROR_EOINTA: return "eof-in-tag";
		case LXB_HTML_TOKENIZER_ERROR_INCLCO: return "incorrectly-closed-comment";
		case LXB_HTML_TOKENIZER_ERROR_INOPCO: return "incorrectly-opened-comment";
		case LXB_HTML_TOKENIZER_ERROR_INCHSEAFDONA: return "invalid-character-sequence-after-doctype-name";
		case LXB_HTML_TOKENIZER_ERROR_INFICHOFTANA: return "invalid-first-character-of-tag-name";
		case LXB_HTML_TOKENIZER_ERROR_MIATVA: return "missing-attribute-value";
		case LXB_HTML_TOKENIZER_ERROR_MIDONA: return "missing-doctype-name";
		case LXB_HTML_TOKENIZER_ERROR_MIDOPUID: return "missing-doctype-public-identifier";
		case LXB_HTML_TOKENIZER_ERROR_MIDOSYID: return "missing-doctype-system-identifier";
		case LXB_HTML_TOKENIZER_ERROR_MIENTANA: return "missing-end-tag-name";
		case LXB_HTML_TOKENIZER_ERROR_MIQUBEDOPUID: return "missing-quote-before-doctype-public-identifier";
		case LXB_HTML_TOKENIZER_ERROR_MIQUBEDOSYID: return "missing-quote-before-doctype-system-identifier";
		case LXB_HTML_TOKENIZER_ERROR_MISEAFCHRE: return "missing-semicolon-after-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_MIWHAFDOPUKE: return "missing-whitespace-after-doctype-public-keyword";
		case LXB_HTML_TOKENIZER_ERROR_MIWHAFDOSYKE: return "missing-whitespace-after-doctype-system-keyword";
		case LXB_HTML_TOKENIZER_ERROR_MIWHBEDONA: return "missing-whitespace-before-doctype-name";
		case LXB_HTML_TOKENIZER_ERROR_MIWHBEAT: return "missing-whitespace-between-attributes";
		case LXB_HTML_TOKENIZER_ERROR_MIWHBEDOPUANSYID: return "missing-whitespace-between-doctype-public-and-system-identifiers";
		case LXB_HTML_TOKENIZER_ERROR_NECO: return "nested-comment";
		case LXB_HTML_TOKENIZER_ERROR_NOCHRE: return "noncharacter-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_NOININST: return "noncharacter-in-input-stream";
		case LXB_HTML_TOKENIZER_ERROR_NOVOHTELSTTAWITRSO: return "non-void-html-element-start-tag-with-trailing-solidus";
		case LXB_HTML_TOKENIZER_ERROR_NUCHRE: return "null-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_SUCHRE: return "surrogate-character-reference";
		case LXB_HTML_TOKENIZER_ERROR_SUININST: return "surrogate-in-input-stream";
		case LXB_HTML_TOKENIZER_ERROR_UNCHAFDOSYID: return "unexpected-character-after-doctype-system-identifier";
		case LXB_HTML_TOKENIZER_ERROR_UNCHINATNA: return "unexpected-character-in-attribute-name";
		case LXB_HTML_TOKENIZER_ERROR_UNCHINUNATVA: return "unexpected-character-in-unquoted-attribute-value";
		case LXB_HTML_TOKENIZER_ERROR_UNEQSIBEATNA: return "unexpected-equals-sign-before-attribute-name";
		case LXB_HTML_TOKENIZER_ERROR_UNNUCH: return "unexpected-null-character";
		case LXB_HTML_TOKENIZER_ERROR_UNQUMAINOFTANA: return "unexpected-question-mark-instead-of-tag-name";
		case LXB_HTML_TOKENIZER_ERROR_UNSOINTA: return "unexpected-solidus-in-tag";
		case LXB_HTML_TOKENIZER_ERROR_UNNACHRE: return "unknown-named-character-reference";
		default: return "unknown error";
	}
}

static const char *dom_lexbor_tree_error_code_to_string(lxb_html_tree_error_id_t id)
{
	switch (id) {
		case LXB_HTML_RULES_ERROR_UNTO: return "unexpected-token";
		case LXB_HTML_RULES_ERROR_UNCLTO: return "unexpected-closed-token";
		case LXB_HTML_RULES_ERROR_NUCH: return "null-character";
		case LXB_HTML_RULES_ERROR_UNCHTO: return "unexpected-character-token";
		case LXB_HTML_RULES_ERROR_UNTOININMO: return "unexpected-token-in-initial-mode";
		case LXB_HTML_RULES_ERROR_BADOTOININMO: return "bad-doctype-token-in-initial-mode";
		case LXB_HTML_RULES_ERROR_DOTOINBEHTMO: return "doctype-token-in-before-html-mode";
		case LXB_HTML_RULES_ERROR_UNCLTOINBEHTMO: return "unexpected-closed-token-in-before-html-mode";
		case LXB_HTML_RULES_ERROR_DOTOINBEHEMO: return "doctype-token-in-before-head-mode";
		case LXB_HTML_RULES_ERROR_UNCLTOINBEHEMO: return "unexpected-closed_token-in-before-head-mode";
		case LXB_HTML_RULES_ERROR_DOTOINHEMO: return "doctype-token-in-head-mode";
		case LXB_HTML_RULES_ERROR_NOVOHTELSTTAWITRSO: return "non-void-html-element-start-tag-with-trailing-solidus";
		case LXB_HTML_RULES_ERROR_HETOINHEMO: return "head-token-in-head-mode";
		case LXB_HTML_RULES_ERROR_UNCLTOINHEMO: return "unexpected-closed-token-in-head-mode";
		case LXB_HTML_RULES_ERROR_TECLTOWIOPINHEMO: return "template-closed-token-without-opening-in-head-mode";
		case LXB_HTML_RULES_ERROR_TEELISNOCUINHEMO: return "template-element-is-not-current-in-head-mode";
		case LXB_HTML_RULES_ERROR_DOTOINHENOMO: return "doctype-token-in-head-noscript-mode";
		case LXB_HTML_RULES_ERROR_DOTOAFHEMO: return "doctype-token-after-head-mode";
		case LXB_HTML_RULES_ERROR_HETOAFHEMO: return "head-token-after-head-mode";
		case LXB_HTML_RULES_ERROR_DOTOINBOMO: return "doctype-token-in-body-mode";
		case LXB_HTML_RULES_ERROR_BAENOPELISWR: return "bad-ending-open-elements-is-wrong";
		case LXB_HTML_RULES_ERROR_OPELISWR: return "open-elements-is-wrong";
		case LXB_HTML_RULES_ERROR_UNELINOPELST: return "unexpected-element-in-open-elements-stack";
		case LXB_HTML_RULES_ERROR_MIELINOPELST: return "missing-element-in-open-elements-stack";
		case LXB_HTML_RULES_ERROR_NOBOELINSC: return "no-body-element-in-scope";
		case LXB_HTML_RULES_ERROR_MIELINSC: return "missing-element-in-scope";
		case LXB_HTML_RULES_ERROR_UNELINSC: return "unexpected-element-in-scope";
		case LXB_HTML_RULES_ERROR_UNELINACFOST: return "unexpected-element-in-active-formatting-stack";
		case LXB_HTML_RULES_ERROR_UNENOFFI: return "unexpected-end-of-file";
		case LXB_HTML_RULES_ERROR_CHINTATE: return "characters-in-table-text";
		case LXB_HTML_RULES_ERROR_DOTOINTAMO: return "doctype-token-in-table-mode";
		case LXB_HTML_RULES_ERROR_DOTOINSEMO: return "doctype-token-in-select-mode";
		case LXB_HTML_RULES_ERROR_DOTOAFBOMO: return "doctype-token-after-body-mode";
		case LXB_HTML_RULES_ERROR_DOTOINFRMO: return "doctype-token-in-frameset-mode";
		case LXB_HTML_RULES_ERROR_DOTOAFFRMO: return "doctype-token-after-frameset-mode";
		case LXB_HTML_RULES_ERROR_DOTOFOCOMO: return "doctype-token-foreign-content-mode";
		default: return "unknown error";
	}
}

static const char *dom_lexbor_libxml2_bridge_status_code_to_string(lexbor_libxml2_bridge_status status)
{
	switch (status) {
		case LEXBOR_LIBXML2_BRIDGE_STATUS_CANNOT_INIT: return "cannot initialize data structures";
		case LEXBOR_LIBXML2_BRIDGE_STATUS_FATAL_PARSE: return "fatal error in parsing";
		case LEXBOR_LIBXML2_BRIDGE_STATUS_OVERFLOW: return "string length overflow";
		case LEXBOR_LIBXML2_BRIDGE_STATUS_OOM: return "out of memory";
		default: return "unknown error";
	}
}

static void dom_reset_line_column_cache(dom_line_column_cache *cache)
{
	cache->last_line = 1;
	cache->last_column = 1;
	cache->last_offset = 0;
}

static void dom_find_line_and_column_using_cache(
	const dom_lexbor_libxml2_bridge_application_data *application_data,
	dom_line_column_cache *cache,
	size_t offset
)
{
	offset -= application_data->current_total_offset;
	if (offset > application_data->current_input_length) {
		/* Possible with empty input, also just good for general safety */
		offset = application_data->current_input_length;
	}

	size_t last_column = cache->last_column;
	size_t last_line = cache->last_line;
	size_t last_offset = cache->last_offset;

	/* Either unicode or UTF-8 data */
	if (application_data->current_input_codepoints != NULL) {
		while (last_offset < offset) {
			if (application_data->current_input_codepoints[last_offset] == 0x000A /* Unicode codepoint for line feed */) {
				last_line++;
				last_column = 1;
			} else {
				last_column++;
			}
			last_offset++;
		}
	} else {
		while (last_offset < offset) {
			const lxb_char_t current = application_data->current_input_characters[last_offset];
			if (current == '\n') {
				last_line++;
				last_column = 1;
				last_offset++;
			} else {
				/* See Lexbor tokenizer patch
				 * Note for future self: branchlessly computing the length and jumping by the length would be nice,
				 * however it takes so many instructions to do so that it is slower than this naive method. */
				if ((current & 0b11000000) != 0b10000000) {
					last_column++;
				}
				last_offset++;
			}
		}
	}

	cache->last_column = last_column;
	cache->last_line = last_line;
	cache->last_offset = last_offset;
}

static void dom_lexbor_libxml2_bridge_tokenizer_error_reporter(
	void *application_data_voidptr,
	lxb_html_tokenizer_error_t *error,
	size_t offset
)
{
	dom_lexbor_libxml2_bridge_application_data *application_data = application_data_voidptr;
	dom_find_line_and_column_using_cache(application_data, &application_data->cache_tokenizer, offset);
	php_libxml_pretend_ctx_error_ex(application_data->input_name, application_data->cache_tokenizer.last_line, application_data->cache_tokenizer.last_column, "tokenizer error %s in %s, line: %zu, column: %zu\n", dom_lexbor_tokenizer_error_code_to_string(error->id), application_data->input_name, application_data->cache_tokenizer.last_line, application_data->cache_tokenizer.last_column);
}

static void dom_lexbor_libxml2_bridge_tree_error_reporter(
	void *application_data_voidptr,
	lxb_html_tree_error_t *error,
	size_t line,
	size_t column,
	size_t len
)
{
	dom_lexbor_libxml2_bridge_application_data *application_data = application_data_voidptr;

	if (line == 1 && application_data->html_no_implied && error->id == LXB_HTML_RULES_ERROR_UNTOININMO) {
		/* For no implied mode, we want to mimick libxml's behaviour of not reporting an error for a lacking doctype. */
		return;
	}

	if (len <= 1) {
		/* Possible with EOF, or single-character tokens, don't use a range in the error display in this case */
		php_libxml_pretend_ctx_error_ex(
			application_data->input_name,
			line,
			column,
			"tree error %s in %s, line: %zu, column: %zu\n",
			dom_lexbor_tree_error_code_to_string(error->id),
			application_data->input_name,
			line,
			column
		);
	} else {
		php_libxml_pretend_ctx_error_ex(
			application_data->input_name,
			line,
			column,
			"tree error %s in %s, line: %zu, column: %zu-%zu\n",
			dom_lexbor_tree_error_code_to_string(error->id),
			application_data->input_name,
			line,
			column,
			column + len - 1
		);
	}
}

static xmlNodePtr dom_search_child(xmlNodePtr parent, const char *searching_for)
{
	xmlNodePtr node = parent->children;
	while (node != NULL) {
		if (node->type == XML_ELEMENT_NODE && strcmp((const char *) node->name, searching_for) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static void dom_place_remove_element_and_hoist_children(xmlNodePtr parent, const char *searching_for)
{
	xmlNodePtr node = dom_search_child(parent, searching_for);
	if (node != NULL) {
		xmlUnlinkNode(node);

		xmlNodePtr child = node->children;
		while (child != NULL) {
			xmlUnlinkNode(child);
			xmlAddChild(parent, child);
			child = node->children;
		}

		xmlFreeNode(node);
	}
}

static void dom_post_process_html5_loading(
	xmlDocPtr lxml_doc,
	zend_long options,
	const lexbor_libxml2_bridge_extracted_observations *observations
)
{
	if (options & HTML_PARSE_NOIMPLIED) {
		xmlNodePtr html_node = dom_search_child((xmlNodePtr) lxml_doc, "html");
		if (!observations->has_explicit_head_tag) {
			dom_place_remove_element_and_hoist_children(html_node, "head");
		}
		if (!observations->has_explicit_body_tag) {
			dom_place_remove_element_and_hoist_children(html_node, "body");
		}
		if (!observations->has_explicit_html_tag) {
			dom_place_remove_element_and_hoist_children((xmlNodePtr) lxml_doc, "html");
		}
	}
}

/* https://html.spec.whatwg.org/multipage/parsing.html#determining-the-character-encoding */
static dom_character_encoding_data dom_determine_encoding(const char *source, size_t source_len)
{
	dom_character_encoding_data result;

	/* BOM sniffing */
	if (source_len >= 3 && source[0] == '\xEF' && source[1] == '\xBB' && source[2] == '\xBF') {
		result.encoding_data = lxb_encoding_data(LXB_ENCODING_UTF_8);
		result.bom_shift = 3;
		return result;
	} else if (source_len >= 2) {
		if (source[0] == '\xFE' && source[1] == '\xFF') {
			result.encoding_data = lxb_encoding_data(LXB_ENCODING_UTF_16BE);
			result.bom_shift = 2;
			return result;
		} else if (source[0] == '\xFF' && source[1] == '\xFE') {
			result.encoding_data = lxb_encoding_data(LXB_ENCODING_UTF_16LE);
			result.bom_shift = 2;
			return result;
		}
	}

	/* Perform prescan */
	lxb_html_encoding_t encoding;
	lxb_status_t status = lxb_html_encoding_init(&encoding);
	if (status != LXB_STATUS_OK) {
		goto fallback_uninit;
	}
	/* This is the "wait either for 1024 bytes or 500ms" part */
	if (source_len > 1024) {
		source_len = 1024;
	}
	status = lxb_html_encoding_determine(&encoding, (const lxb_char_t *) source, (const lxb_char_t *) source + source_len);
	if (status != LXB_STATUS_OK) {
		goto fallback;
	}
	lxb_html_encoding_entry_t *entry = lxb_html_encoding_meta_entry(&encoding, 0);
	if (entry == NULL) {
		goto fallback;
	}
	result.encoding_data = lxb_encoding_data_by_pre_name(entry->name, entry->end - entry->name);
	if (!result.encoding_data) {
		goto fallback;
	}
	result.bom_shift = 0;
	lxb_html_encoding_destroy(&encoding, false);
	return result;

fallback:
	lxb_html_encoding_destroy(&encoding, false);
fallback_uninit:
	result.encoding_data = lxb_encoding_data(DOM_FALLBACK_ENCODING_ID);
	result.bom_shift = 0;
	return result;
}

static void dom_setup_parser_encoding_manually(const lxb_char_t *buf_start, const lxb_encoding_data_t *encoding_data, dom_decoding_encoding_ctx *decoding_encoding_ctx, dom_lexbor_libxml2_bridge_application_data *application_data)
{
	static const lxb_codepoint_t replacement_codepoint = LXB_ENCODING_REPLACEMENT_CODEPOINT;

	decoding_encoding_ctx->decode_data = encoding_data;

	(void) lxb_encoding_decode_init(
		&decoding_encoding_ctx->decode,
		decoding_encoding_ctx->decode_data,
		decoding_encoding_ctx->codepoints,
		sizeof(decoding_encoding_ctx->codepoints) / sizeof(*decoding_encoding_ctx->codepoints)
	);
	(void) lxb_encoding_decode_replace_set(
		&decoding_encoding_ctx->decode,
		&replacement_codepoint,
		LXB_ENCODING_REPLACEMENT_BUFFER_LEN
	);
	/* Note: encode_data is for UTF-8 */
	decoding_encoding_ctx->fast_path = decoding_encoding_ctx->decode_data == decoding_encoding_ctx->encode_data;

	if (decoding_encoding_ctx->fast_path) {
		application_data->current_input_codepoints = NULL;
		application_data->current_input_characters = (const char *) buf_start;
	} else {
		application_data->current_input_codepoints = decoding_encoding_ctx->codepoints;
		application_data->current_input_characters = NULL;
	}
}

static void dom_setup_parser_encoding_implicitly(
	const lxb_char_t **buf_ref,
	size_t *read,
	dom_decoding_encoding_ctx *decoding_encoding_ctx,
	dom_lexbor_libxml2_bridge_application_data *application_data
)
{
	const char *buf_start = (const char *) *buf_ref;
	dom_character_encoding_data dom_encoding_data = dom_determine_encoding(buf_start, *read);
	*buf_ref += dom_encoding_data.bom_shift;
	*read -= dom_encoding_data.bom_shift;
	dom_setup_parser_encoding_manually((const lxb_char_t *) buf_start, dom_encoding_data.encoding_data, decoding_encoding_ctx, application_data);
}

static bool dom_process_parse_chunk(
	lexbor_libxml2_bridge_parse_context *ctx,
	lxb_html_document_t *document,
	lxb_html_parser_t *parser,
	size_t encoded_length,
	const lxb_char_t *encoding_output,
	size_t input_buffer_length,
	size_t *tokenizer_error_offset,
	size_t *tree_error_offset
)
{
	dom_lexbor_libxml2_bridge_application_data *application_data = ctx->application_data;
	application_data->current_input_length = input_buffer_length;
	lexbor_status_t lexbor_status = lxb_html_document_parse_chunk(document, encoding_output, encoded_length);
	if (UNEXPECTED(lexbor_status != LXB_STATUS_OK)) {
		return false;
	}
	if (ctx->tokenizer_error_reporter || ctx->tree_error_reporter) {
		lexbor_libxml2_bridge_report_errors(ctx, parser, encoding_output, application_data->current_total_offset, tokenizer_error_offset, tree_error_offset);
		dom_find_line_and_column_using_cache(application_data, &application_data->cache_tokenizer, application_data->current_total_offset + input_buffer_length);
	}
	application_data->current_total_offset += input_buffer_length;
	application_data->cache_tokenizer.last_offset = 0;
	return true;
}

static bool dom_decode_encode_fast_path(
	lexbor_libxml2_bridge_parse_context *ctx,
	lxb_html_document_t *document,
	lxb_html_parser_t *parser,
	const lxb_char_t **buf_ref_ref,
	const lxb_char_t *buf_end,
	dom_decoding_encoding_ctx *decoding_encoding_ctx,
	size_t *tokenizer_error_offset,
	size_t *tree_error_offset
)
{
	const lxb_char_t *buf_ref = *buf_ref_ref;
	const lxb_char_t *last_output = buf_ref;
	while (buf_ref != buf_end) {
		/* Fast path converts non-validated UTF-8 -> validated UTF-8 */
		if (decoding_encoding_ctx->decode.u.utf_8.need == 0 && *buf_ref < 0x80) {
			/* Fast path within the fast path: try to skip non-mb bytes in bulk if we are not in a state where we
			 * need more UTF-8 bytes to complete a sequence.
			 * It might be tempting to use SIMD here, but it turns out that this is less efficient because
			 * we need to process the same byte multiple times sometimes when mixing ASCII with multibyte. */
			buf_ref++;
			continue;
		}
		const lxb_char_t *buf_ref_backup = buf_ref;
		lxb_codepoint_t codepoint = lxb_encoding_decode_utf_8_single(&decoding_encoding_ctx->decode, &buf_ref, buf_end);
		if (UNEXPECTED(codepoint > LXB_ENCODING_MAX_CODEPOINT)) {
			size_t skip = buf_ref - buf_ref_backup; /* Skip invalid data, it's replaced by the UTF-8 replacement bytes */
			if (!dom_process_parse_chunk(
				ctx,
				document,
				parser,
				buf_ref - last_output - skip,
				last_output,
				buf_ref - last_output,
				tokenizer_error_offset,
				tree_error_offset
			)) {
				goto fail_oom;
			}
			if (!dom_process_parse_chunk(
				ctx,
				document,
				parser,
				LXB_ENCODING_REPLACEMENT_SIZE,
				LXB_ENCODING_REPLACEMENT_BYTES,
				0,
				tokenizer_error_offset,
				tree_error_offset
			)) {
				goto fail_oom;
			}
			last_output = buf_ref;
		}
	}
	if (buf_ref != last_output
		&& !dom_process_parse_chunk(
			ctx,
			document,
			parser,
			buf_ref - last_output,
			last_output,
			buf_ref - last_output,
			tokenizer_error_offset,
			tree_error_offset
	)) {
		goto fail_oom;
	}
	*buf_ref_ref = buf_ref;
	return true;
fail_oom:
	*buf_ref_ref = buf_ref;
	return false;
}

static bool dom_decode_encode_slow_path(
	lexbor_libxml2_bridge_parse_context *ctx,
	lxb_html_document_t *document,
	lxb_html_parser_t *parser,
	const lxb_char_t **buf_ref_ref,
	const lxb_char_t *buf_end,
	dom_decoding_encoding_ctx *decoding_encoding_ctx,
	size_t *tokenizer_error_offset,
	size_t *tree_error_offset
)
{
	const lxb_char_t *buf_ref = *buf_ref_ref;
	lexbor_status_t decode_status, encode_status;
	do {
		decode_status = decoding_encoding_ctx->decode_data->decode(&decoding_encoding_ctx->decode, &buf_ref, buf_end);

		const lxb_codepoint_t *codepoints_ref = (const lxb_codepoint_t *) decoding_encoding_ctx->codepoints;
		size_t decoding_buffer_used = lxb_encoding_decode_buf_used(&decoding_encoding_ctx->decode);
		const lxb_codepoint_t *codepoints_end = decoding_encoding_ctx->codepoints + decoding_buffer_used;
		do {
			encode_status = decoding_encoding_ctx->encode_data->encode(&decoding_encoding_ctx->encode, &codepoints_ref, codepoints_end);
			ZEND_ASSERT(encode_status != LXB_STATUS_ERROR && "parameters and replacements should be valid");
			if (!dom_process_parse_chunk(
				ctx,
				document,
				parser,
				lxb_encoding_encode_buf_used(&decoding_encoding_ctx->encode),
				decoding_encoding_ctx->encoding_output,
				decoding_buffer_used,
				tokenizer_error_offset,
				tree_error_offset
			)) {
				goto fail_oom;
			}
			lxb_encoding_encode_buf_used_set(&decoding_encoding_ctx->encode, 0);
		} while (encode_status == LXB_STATUS_SMALL_BUFFER);
		lxb_encoding_decode_buf_used_set(&decoding_encoding_ctx->decode, 0);
	} while (decode_status == LXB_STATUS_SMALL_BUFFER);
	*buf_ref_ref = buf_ref;
	return true;
fail_oom:
	*buf_ref_ref = buf_ref;
	return false;
}

static bool dom_parse_decode_encode_step(
	lexbor_libxml2_bridge_parse_context *ctx,
	lxb_html_document_t *document,
	lxb_html_parser_t *parser,
	const lxb_char_t **buf_ref_ref,
	const lxb_char_t *buf_end,
	dom_decoding_encoding_ctx *decoding_encoding_ctx,
	size_t *tokenizer_error_offset,
	size_t *tree_error_offset
)
{
	if (decoding_encoding_ctx->fast_path) {
		return dom_decode_encode_fast_path(
			ctx,
			document,
			parser,
			buf_ref_ref,
			buf_end,
			decoding_encoding_ctx,
			tokenizer_error_offset,
			tree_error_offset
		);
	} else {
		return dom_decode_encode_slow_path(
			ctx,
			document,
			parser,
			buf_ref_ref,
			buf_end,
			decoding_encoding_ctx,
			tokenizer_error_offset,
			tree_error_offset
		);
	}
}

static bool dom_parse_decode_encode_finish(
	lexbor_libxml2_bridge_parse_context *ctx,
	lxb_html_document_t *document,
	lxb_html_parser_t *parser,
	dom_decoding_encoding_ctx *decoding_encoding_ctx,
	size_t *tokenizer_error_offset,
	size_t *tree_error_offset
)
{
	if (!decoding_encoding_ctx->fast_path) {
		/* Fast path handles codepoints one by one, so this part is not applicable in that case */
		(void) lxb_encoding_decode_finish(&decoding_encoding_ctx->decode);
		size_t decoding_buffer_size = lxb_encoding_decode_buf_used(&decoding_encoding_ctx->decode);
		if (decoding_buffer_size > 0) {
			const lxb_codepoint_t *codepoints_ref = (const lxb_codepoint_t *) decoding_encoding_ctx->codepoints;
			const lxb_codepoint_t *codepoints_end = codepoints_ref + decoding_buffer_size;
			(void) decoding_encoding_ctx->encode_data->encode(&decoding_encoding_ctx->encode, &codepoints_ref, codepoints_end);
			if (!dom_process_parse_chunk(
				ctx,
				document,
				parser,
				lxb_encoding_encode_buf_used(&decoding_encoding_ctx->encode),
				decoding_encoding_ctx->encoding_output,
				decoding_buffer_size,
				tokenizer_error_offset,
				tree_error_offset
			)) {
				return false;
			}
		}
	}
	(void) lxb_encoding_encode_finish(&decoding_encoding_ctx->encode);
	if (lxb_encoding_encode_buf_used(&decoding_encoding_ctx->encode)
		&& !dom_process_parse_chunk(
			ctx,
			document,
			parser,
			lxb_encoding_encode_buf_used(&decoding_encoding_ctx->encode),
			decoding_encoding_ctx->encoding_output,
			lxb_encoding_decode_buf_used(&decoding_encoding_ctx->decode),
			tokenizer_error_offset,
			tree_error_offset
	)) {
		return false;
	}
	return true;
}

static bool check_options_validity(uint32_t arg_num, zend_long options)
{
	const zend_long VALID_OPTIONS = XML_PARSE_NOERROR | XML_PARSE_COMPACT | HTML_PARSE_NOIMPLIED | DOM_HTML_NO_DEFAULT_NS;
	if ((options & ~VALID_OPTIONS) != 0) {
		zend_argument_value_error(arg_num, "contains invalid flags (allowed flags: "
										   "LIBXML_NOERROR, "
										   "LIBXML_COMPACT, "
										   "LIBXML_HTML_NOIMPLIED, "
										   "DOM\\NO_DEFAULT_NS)");
		return false;
	}
	return true;
}

PHP_METHOD(DOM_HTMLDocument, createEmpty)
{
	const char *encoding = "UTF-8";
	size_t encoding_len = strlen("UTF-8");
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|p", &encoding, &encoding_len) == FAILURE) {
		RETURN_THROWS();
	}

	const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name((const lxb_char_t *) encoding, encoding_len);

	if (encoding_data == NULL) {
		zend_argument_value_error(1, "must be a valid document encoding");
		RETURN_THROWS();
	}

	xmlDocPtr lxml_doc = php_dom_create_html_doc();
	if (UNEXPECTED(lxml_doc == NULL)) {
		goto oom;
	}

	lxml_doc->encoding = xmlStrdup((const xmlChar *) encoding);

	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_html_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	intern->document->class_type = PHP_LIBXML_CLASS_MODERN;
	intern->document->private_data = php_dom_libxml_ns_mapper_header(php_dom_libxml_ns_mapper_create());
	return;

oom:
	php_dom_throw_error(INVALID_STATE_ERR, true);
	RETURN_THROWS();
}

PHP_METHOD(DOM_HTMLDocument, createFromString)
{
	const char *source, *override_encoding = NULL;
	size_t source_len, override_encoding_len;
	zend_long options = 0;
	if (zend_parse_parameters(
		ZEND_NUM_ARGS(),
		"s|lp!",
		&source,
		&source_len,
		&options,
		&override_encoding,
		&override_encoding_len
	) == FAILURE) {
		RETURN_THROWS();
	}

	if (!check_options_validity(2, options)) {
		RETURN_THROWS();
	}

	dom_lexbor_libxml2_bridge_application_data application_data;
	application_data.input_name = "Entity";
	application_data.current_total_offset = 0;
	application_data.html_no_implied = options & HTML_PARSE_NOIMPLIED;
	dom_reset_line_column_cache(&application_data.cache_tokenizer);
	lexbor_libxml2_bridge_parse_context ctx;
	lexbor_libxml2_bridge_parse_context_init(&ctx);
	if (!(options & XML_PARSE_NOERROR)) {
		lexbor_libxml2_bridge_parse_set_error_callbacks(
			&ctx,
			dom_lexbor_libxml2_bridge_tokenizer_error_reporter,
			dom_lexbor_libxml2_bridge_tree_error_reporter
		);
	}
	ctx.application_data = &application_data;

	size_t tokenizer_error_offset = 0;
	size_t tree_error_offset = 0;

	/* Setup everything encoding & decoding related */
	const lxb_char_t *buf_ref = (const lxb_char_t *) source;
	dom_decoding_encoding_ctx decoding_encoding_ctx;
	dom_decoding_encoding_ctx_init(&decoding_encoding_ctx);
	if (override_encoding != NULL) {
		const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name(
			(const lxb_char_t *) override_encoding,
			override_encoding_len
		);
		if (!encoding_data) {
			zend_argument_value_error(3, "must be a valid document encoding");
			RETURN_THROWS();
		}
		dom_setup_parser_encoding_manually(buf_ref, encoding_data, &decoding_encoding_ctx, &application_data);
	} else {
		dom_setup_parser_encoding_implicitly(&buf_ref, &source_len, &decoding_encoding_ctx, &application_data);
	}

	lxb_html_document_t *document = lxb_html_document_create();
	if (UNEXPECTED(document == NULL)) {
		goto fail_oom;
	}

	lxb_status_t lexbor_status = lxb_html_document_parse_chunk_begin(document);
	if (UNEXPECTED(lexbor_status != LXB_STATUS_OK)) {
		goto fail_oom;
	}

	lxb_html_parser_t *parser = document->dom_document.parser;

	while (source_len > 0) {
		size_t chunk_size = source_len;
		const size_t MAX_CHUNK_SIZE = sizeof(decoding_encoding_ctx.encoding_output) / sizeof(*decoding_encoding_ctx.encoding_output);
		if (chunk_size > MAX_CHUNK_SIZE) {
			chunk_size = MAX_CHUNK_SIZE;
		}
		source_len -= chunk_size;

		const lxb_char_t *buf_end = buf_ref + chunk_size;
		bool result = dom_parse_decode_encode_step(
			&ctx,
			document,
			parser,
			&buf_ref,
			buf_end,
			&decoding_encoding_ctx,
			&tokenizer_error_offset,
			&tree_error_offset
		);
		if (!result) {
			goto fail_oom;
		}
	}

	if (!dom_parse_decode_encode_finish(&ctx, document, parser, &decoding_encoding_ctx, &tokenizer_error_offset, &tree_error_offset)) {
		goto fail_oom;
	}

	lexbor_status = lxb_html_document_parse_chunk_end(document);
	if (lexbor_status != LXB_STATUS_OK) {
		goto fail_oom;
	}

	php_dom_libxml_ns_mapper *ns_mapper = php_dom_libxml_ns_mapper_create();

	xmlDocPtr lxml_doc;
	lexbor_libxml2_bridge_status bridge_status = lexbor_libxml2_bridge_convert_document(
		document,
		&lxml_doc,
		options & XML_PARSE_COMPACT,
		!(options & DOM_HTML_NO_DEFAULT_NS),
		ns_mapper
	);
	lexbor_libxml2_bridge_copy_observations(parser->tree, &ctx.observations);
	if (UNEXPECTED(bridge_status != LEXBOR_LIBXML2_BRIDGE_STATUS_OK)) {
		php_dom_libxml_ns_mapper_destroy(ns_mapper);
		php_libxml_ctx_error(
			NULL,
			"%s in %s",
			dom_lexbor_libxml2_bridge_status_code_to_string(bridge_status),
			application_data.input_name
		);
		lxb_html_document_destroy(document);
		RETURN_FALSE;
	}
	lxb_html_document_destroy(document);

	dom_post_process_html5_loading(lxml_doc, options, &ctx.observations);

	if (decoding_encoding_ctx.decode_data) {
		lxml_doc->encoding = xmlStrdup((const xmlChar *) decoding_encoding_ctx.decode_data->name);
	} else {
		lxml_doc->encoding = xmlStrdup((const xmlChar *) "UTF-8");
	}

	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_html_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	intern->document->class_type = PHP_LIBXML_CLASS_MODERN;
	intern->document->private_data = php_dom_libxml_ns_mapper_header(ns_mapper);
	return;

fail_oom:
	lxb_html_document_destroy(document);
	php_dom_throw_error(INVALID_STATE_ERR, true);
	RETURN_THROWS();
}

PHP_METHOD(DOM_HTMLDocument, createFromFile)
{
	const char *filename, *override_encoding = NULL;
	php_dom_libxml_ns_mapper *ns_mapper = NULL;
	size_t filename_len, override_encoding_len;
	zend_long options = 0;
	php_stream *stream = NULL;
	if (zend_parse_parameters(
		ZEND_NUM_ARGS(),
		"p|lp!",
		&filename,
		&filename_len,
		&options,
		&override_encoding,
		&override_encoding_len
	) == FAILURE) {
		RETURN_THROWS();
	}

	/* See php_libxml_streams_IO_open_wrapper(), apparently this caused issues in the past. */
	if (strstr(filename, "%00")) {
		zend_argument_value_error(1, "must not contain percent-encoded NUL bytes");
		RETURN_THROWS();
	}

	if (!check_options_validity(2, options)) {
		RETURN_THROWS();
	}

	dom_lexbor_libxml2_bridge_application_data application_data;
	application_data.input_name = filename;
	application_data.current_total_offset = 0;
	application_data.html_no_implied = options & HTML_PARSE_NOIMPLIED;
	dom_reset_line_column_cache(&application_data.cache_tokenizer);
	lexbor_libxml2_bridge_parse_context ctx;
	lexbor_libxml2_bridge_parse_context_init(&ctx);
	if (!(options & XML_PARSE_NOERROR)) {
		lexbor_libxml2_bridge_parse_set_error_callbacks(
			&ctx,
			dom_lexbor_libxml2_bridge_tokenizer_error_reporter,
			dom_lexbor_libxml2_bridge_tree_error_reporter
		);
	}
	ctx.application_data = &application_data;

	char buf[4096];

	/* Setup everything encoding & decoding related */
	dom_decoding_encoding_ctx decoding_encoding_ctx;
	dom_decoding_encoding_ctx_init(&decoding_encoding_ctx);
	bool should_determine_encoding_implicitly = true; /* First read => determine encoding implicitly */
	if (override_encoding != NULL) {
		const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name(
			(const lxb_char_t *) override_encoding,
			override_encoding_len
		);
		if (!encoding_data) {
			zend_argument_value_error(3, "must be a valid document encoding");
			RETURN_THROWS();
		}
		should_determine_encoding_implicitly = false;
		dom_setup_parser_encoding_manually((const lxb_char_t *) buf, encoding_data, &decoding_encoding_ctx, &application_data);
	}

	zend_string *opened_path = NULL;
	stream = php_stream_open_wrapper_ex(filename, "rb", REPORT_ERRORS, &opened_path, php_libxml_get_stream_context());
	if (!stream) {
		if (!EG(exception)) {
			zend_throw_exception_ex(NULL, 0, "Cannot open file '%s'", filename);
		}
		RETURN_THROWS();
	}

	/* MIME sniff */
	if (should_determine_encoding_implicitly) {
		zend_string *charset = php_libxml_sniff_charset_from_stream(stream);
		if (charset != NULL) {
			const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name(
				(const lxb_char_t *) ZSTR_VAL(charset),
				ZSTR_LEN(charset)
			);
			if (encoding_data != NULL) {
				should_determine_encoding_implicitly = false;
				dom_setup_parser_encoding_manually(
					(const lxb_char_t *) buf,
					encoding_data,
					&decoding_encoding_ctx,
					&application_data
				);
			}
			zend_string_release_ex(charset, false);
		}
	}

	lxb_html_document_t *document = lxb_html_document_create();
	if (UNEXPECTED(document == NULL)) {
		goto fail_oom;
	}

	lxb_status_t lexbor_status = lxb_html_document_parse_chunk_begin(document);
	if (UNEXPECTED(lexbor_status != LXB_STATUS_OK)) {
		goto fail_oom;
	}

	size_t tokenizer_error_offset = 0;
	size_t tree_error_offset = 0;
	ssize_t read;
	lxb_html_parser_t *parser = document->dom_document.parser;

	while ((read = php_stream_read(stream, buf, sizeof(buf))) > 0) {
		const lxb_char_t *buf_ref = (const lxb_char_t *) buf;

		if (should_determine_encoding_implicitly) {
			should_determine_encoding_implicitly = false;
			dom_setup_parser_encoding_implicitly(&buf_ref, (size_t *) &read, &decoding_encoding_ctx, &application_data);
		}

		const lxb_char_t *buf_end = buf_ref + read;
		bool result = dom_parse_decode_encode_step(
			&ctx,
			document,
			parser,
			&buf_ref,
			buf_end,
			&decoding_encoding_ctx,
			&tokenizer_error_offset,
			&tree_error_offset
		);
		if (!result) {
			goto fail_oom;
		}
	}

	if (!dom_parse_decode_encode_finish(&ctx, document, parser, &decoding_encoding_ctx, &tokenizer_error_offset, &tree_error_offset)) {
		goto fail_oom;
	}

	lexbor_status = lxb_html_document_parse_chunk_end(document);
	if (lexbor_status != LXB_STATUS_OK) {
		goto fail_oom;
	}

	ns_mapper = php_dom_libxml_ns_mapper_create();

	xmlDocPtr lxml_doc;
	lexbor_libxml2_bridge_status bridge_status = lexbor_libxml2_bridge_convert_document(
		document,
		&lxml_doc,
		options & XML_PARSE_COMPACT,
		!(options & DOM_HTML_NO_DEFAULT_NS),
		ns_mapper
	);
	lexbor_libxml2_bridge_copy_observations(parser->tree, &ctx.observations);
	if (UNEXPECTED(bridge_status != LEXBOR_LIBXML2_BRIDGE_STATUS_OK)) {
		php_libxml_ctx_error(NULL, "%s in %s", dom_lexbor_libxml2_bridge_status_code_to_string(bridge_status), filename);
		RETVAL_FALSE;
		goto fail_general;
	}
	lxb_html_document_destroy(document);

	dom_post_process_html5_loading(lxml_doc, options, &ctx.observations);

	if (decoding_encoding_ctx.decode_data) {
		lxml_doc->encoding = xmlStrdup((const xmlChar *) decoding_encoding_ctx.decode_data->name);
	} else {
		lxml_doc->encoding = xmlStrdup((const xmlChar *) "UTF-8");
	}

	if (stream->wrapper == &php_plain_files_wrapper && opened_path != NULL) {
		xmlChar *converted = xmlPathToURI((const xmlChar *) ZSTR_VAL(opened_path));
		if (UNEXPECTED(!converted)) {
			goto fail_oom;
		}
		/* Check for "file:/" instead of "file://" because of libxml2 quirk */
		if (strncmp((const char *) converted, "file:/", sizeof("file:/") - 1) != 0) {
			xmlChar *buffer = xmlStrdup((const xmlChar *) "file://");
			if (UNEXPECTED(!buffer)) {
				xmlFree(converted);
				goto fail_oom;
			}
			xmlChar *new_buffer = xmlStrcat(buffer, converted);
			if (UNEXPECTED(!new_buffer)) {
				xmlFree(buffer);
				xmlFree(converted);
				goto fail_oom;
			}
			xmlFree(converted);
			lxml_doc->URL = new_buffer;
		} else {
#if PHP_WIN32
			converted = php_dom_libxml_fix_file_path(converted);
#endif
			lxml_doc->URL = converted;
		}
	} else {
		lxml_doc->URL = xmlStrdup((const xmlChar *) filename);
	}

	if (opened_path != NULL) {
		zend_string_release_ex(opened_path, false);
	}
	php_stream_close(stream);
	stream = NULL;

	dom_object *intern = php_dom_instantiate_object_helper(
		return_value,
		dom_html_document_class_entry,
		(xmlNodePtr) lxml_doc,
		NULL
	);
	intern->document->class_type = PHP_LIBXML_CLASS_MODERN;
	intern->document->private_data = php_dom_libxml_ns_mapper_header(ns_mapper);
	return;

fail_oom:
	php_dom_throw_error(INVALID_STATE_ERR, true);
fail_general:
	if (ns_mapper != NULL) {
		php_dom_libxml_ns_mapper_destroy(ns_mapper);
	}
	lxb_html_document_destroy(document);
	php_stream_close(stream);
	if (opened_path != NULL) {
		zend_string_release_ex(opened_path, false);
	}
}

static zend_result dom_write_output_smart_str(void *ctx, const char *buf, size_t size)
{
	smart_str_appendl((smart_str *) ctx, buf, size);
	return SUCCESS;
}

static zend_result dom_write_output_stream(void *application_data, const char *buf, size_t len)
{
	php_stream *stream = (php_stream *) application_data;
	if (UNEXPECTED(php_stream_write(stream, buf, len) < 0)) {
		return FAILURE;
	}
	return SUCCESS;
}

static zend_result dom_saveHTML_write_string_len(void *application_data, const char *buf, size_t len)
{
	dom_output_ctx *output = (dom_output_ctx *) application_data;
	lxb_status_t decode_status, encode_status;
	const lxb_char_t *buf_ref = (const lxb_char_t *) buf;
	const lxb_char_t *buf_end = buf_ref + len;

	do {
		decode_status = output->decoding_data->decode(output->decode, &buf_ref, buf_end);

		const lxb_codepoint_t *codepoints_ref = output->codepoints;
		const lxb_codepoint_t *codepoints_end = codepoints_ref + lxb_encoding_decode_buf_used(output->decode);
		do {
			encode_status = output->encoding_data->encode(output->encode, &codepoints_ref, codepoints_end);
			if (UNEXPECTED(output->write_output(
				output->output_data,
				(const char *) output->encoding_output,
				lxb_encoding_encode_buf_used(output->encode)
			) != SUCCESS)) {
				return FAILURE;
			}
			lxb_encoding_encode_buf_used_set(output->encode, 0);
		} while (encode_status == LXB_STATUS_SMALL_BUFFER);
		lxb_encoding_decode_buf_used_set(output->decode, 0);
	} while (decode_status == LXB_STATUS_SMALL_BUFFER);

	return SUCCESS;
}

static zend_result dom_saveHTML_write_string(void *application_data, const char *buf)
{
	return dom_saveHTML_write_string_len(application_data, buf, strlen(buf));
}

static zend_result dom_common_save(dom_output_ctx *output_ctx, const xmlDoc *docp, const xmlNode *node)
{
	/* Initialize everything related to encoding & decoding */
	const lxb_encoding_data_t *decoding_data = lxb_encoding_data(LXB_ENCODING_UTF_8);
	const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name(
		(const lxb_char_t *) docp->encoding,
		strlen((const char *) docp->encoding)
	);
	lxb_encoding_encode_t encode;
	lxb_encoding_decode_t decode;
	lxb_char_t encoding_output[4096];
	lxb_codepoint_t codepoints[4096];
	(void) lxb_encoding_encode_init(&encode, encoding_data, encoding_output, sizeof(encoding_output) / sizeof(*encoding_output));
	(void) lxb_encoding_decode_init(&decode, decoding_data, codepoints, sizeof(codepoints) / sizeof(*codepoints));
	if (encoding_data->encoding == LXB_ENCODING_UTF_8) {
		lxb_encoding_encode_replace_set(&encode, LXB_ENCODING_REPLACEMENT_BYTES, LXB_ENCODING_REPLACEMENT_SIZE);
	} else {
		/* Fallback if there is no replacement by default */
		lxb_encoding_encode_replace_set(&encode, (const lxb_char_t *) "?", 1);
	}
	lxb_encoding_decode_replace_set(&decode, LXB_ENCODING_REPLACEMENT_BUFFER, LXB_ENCODING_REPLACEMENT_BUFFER_LEN);

	output_ctx->encoding_data = encoding_data;
	output_ctx->decoding_data = decoding_data;
	output_ctx->encode = &encode;
	output_ctx->decode = &decode;
	output_ctx->codepoints = codepoints;
	output_ctx->encoding_output = encoding_output;

	dom_html5_serialize_context ctx;
	ctx.write_string_len = dom_saveHTML_write_string_len;
	ctx.write_string = dom_saveHTML_write_string;
	ctx.application_data = output_ctx;
	if (UNEXPECTED(dom_html5_serialize_outer(&ctx, node) != SUCCESS)) {
		return FAILURE;
	}

	(void) lxb_encoding_decode_finish(&decode);
	if (lxb_encoding_decode_buf_used(&decode)) {
		const lxb_codepoint_t *codepoints_ref = (const lxb_codepoint_t *) codepoints;
		(void) encoding_data->encode(&encode, &codepoints_ref, codepoints_ref + lxb_encoding_decode_buf_used(&decode));
		if (UNEXPECTED(output_ctx->write_output(
			output_ctx->output_data,
			(const char *) encoding_output,
			lxb_encoding_encode_buf_used(&encode)) != SUCCESS
		)) {
			return FAILURE;
		}
	}
	(void) lxb_encoding_encode_finish(&encode);
	if (lxb_encoding_encode_buf_used(&encode)) {
		if (UNEXPECTED(output_ctx->write_output(
			output_ctx->output_data,
			(const char *) encoding_output,
			lxb_encoding_encode_buf_used(&encode)) != SUCCESS
		)) {
			return FAILURE;
		}
	}

	return SUCCESS;
}

PHP_METHOD(DOM_HTMLDocument, saveHTMLFile)
{
	zval *id;
	xmlDoc *docp;
	size_t file_len;
	dom_object *intern;
	char *file;

	id = ZEND_THIS;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "p", &file, &file_len) == FAILURE) {
		RETURN_THROWS();
	}

	if (file_len == 0) {
		zend_argument_value_error(1, "must not be empty");
		RETURN_THROWS();
	}

	php_stream *stream = php_stream_open_wrapper_ex(file, "wb", REPORT_ERRORS, /* opened_path */ NULL, php_libxml_get_stream_context());
	if (!stream) {
		RETURN_FALSE;
	}

	DOM_GET_OBJ(docp, id, xmlDocPtr, intern);

	dom_output_ctx output_ctx;
	output_ctx.output_data = stream;
	output_ctx.write_output = dom_write_output_stream;
	if (UNEXPECTED(dom_common_save(&output_ctx, docp, (const xmlNode *) docp) != SUCCESS)) {
		php_stream_close(stream);
		RETURN_FALSE;
	}

	zend_long bytes = php_stream_tell(stream);
	php_stream_close(stream);

	RETURN_LONG(bytes);
}

PHP_METHOD(DOM_HTMLDocument, saveHTML)
{
	zval *nodep = NULL;
	const xmlDoc *docp;
	const xmlNode *node;
	dom_object *intern, *nodeobj;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|O!", &nodep, dom_modern_node_class_entry) == FAILURE) {
		RETURN_THROWS();
	}

	DOM_GET_OBJ(docp, ZEND_THIS, xmlDocPtr, intern);

	if (nodep != NULL) {
		DOM_GET_OBJ(node, nodep, xmlNodePtr, nodeobj);
		if (node->doc != docp) {
			php_dom_throw_error(WRONG_DOCUMENT_ERR, dom_get_strict_error(intern->document));
			RETURN_FALSE;
		}
	} else {
		node = (const xmlNode *) docp;
	}

	smart_str buf = {0};
	dom_output_ctx output_ctx;
	output_ctx.output_data = &buf;
	output_ctx.write_output = dom_write_output_smart_str;
	/* Can't fail because dom_write_output_smart_str() can't fail. */
	zend_result result = dom_common_save(&output_ctx, docp, node);
	ZEND_ASSERT(result == SUCCESS);

	RETURN_STR(smart_str_extract(&buf));
}

zend_result dom_html_document_encoding_write(dom_object *obj, zval *newval)
{
	DOM_PROP_NODE(xmlDocPtr, docp, obj);

	/* Typed property, can only be IS_STRING or IS_NULL. */
	ZEND_ASSERT(Z_TYPE_P(newval) == IS_STRING || Z_TYPE_P(newval) == IS_NULL);

	if (Z_TYPE_P(newval) == IS_NULL) {
		goto invalid_encoding;
	}

	zend_string *str = Z_STR_P(newval);
	const lxb_encoding_data_t *encoding_data = lxb_encoding_data_by_name((const lxb_char_t *) ZSTR_VAL(str), ZSTR_LEN(str));

	if (encoding_data != NULL) {
		xmlFree(BAD_CAST docp->encoding);
		docp->encoding = xmlStrdup((const xmlChar *) encoding_data->name);
	} else {
		goto invalid_encoding;
	}

	return SUCCESS;

invalid_encoding:
	zend_value_error("Invalid document encoding");
	return FAILURE;
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
