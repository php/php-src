/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Niels Dossche <nielsdos@php.net>                            |
   +----------------------------------------------------------------------+
*/

#ifndef HTML5_PARSER_H
#define HTML5_PARSER_H

#include "namespace_compat.h"
#include <lexbor/html/parser.h>
#include <libxml/tree.h>
#include <Zend/zend_portability.h>

typedef enum {
    LEXBOR_LIBXML2_BRIDGE_STATUS_OK = 0,
    LEXBOR_LIBXML2_BRIDGE_STATUS_CANNOT_INIT,
    LEXBOR_LIBXML2_BRIDGE_STATUS_FATAL_PARSE,
    LEXBOR_LIBXML2_BRIDGE_STATUS_OVERFLOW,
    LEXBOR_LIBXML2_BRIDGE_STATUS_OOM,
} lexbor_libxml2_bridge_status;

typedef void (*lexbor_libxml2_bridge_tokenizer_error_reporter)(
    void *application_data,
    lxb_html_tokenizer_error_t *error,
    size_t offset
);
typedef void (*lexbor_libxml2_bridge_tree_error_reporter)(
    void *application_data,
    lxb_html_tree_error_t *error,
    size_t line,
    size_t column,
    size_t len
);

typedef struct lexbor_libxml2_bridge_extracted_observations {
    bool has_explicit_html_tag;
    bool has_explicit_head_tag;
    bool has_explicit_body_tag;
    php_libxml_quirks_mode quirks_mode;
} lexbor_libxml2_bridge_extracted_observations;

typedef struct lexbor_libxml2_bridge_parse_context {
    /* Private fields */
    lexbor_libxml2_bridge_tokenizer_error_reporter tokenizer_error_reporter;
    lexbor_libxml2_bridge_tree_error_reporter tree_error_reporter;
    /* Public fields */
    lexbor_libxml2_bridge_extracted_observations observations;
    /* Application data, do what you want with this */
    void *application_data;
} lexbor_libxml2_bridge_parse_context;

void lexbor_libxml2_bridge_parse_context_init(lexbor_libxml2_bridge_parse_context *ctx);
void lexbor_libxml2_bridge_parse_set_error_callbacks(
    lexbor_libxml2_bridge_parse_context *ctx,
    lexbor_libxml2_bridge_tokenizer_error_reporter tokenizer_error_reporter,
    lexbor_libxml2_bridge_tree_error_reporter tree_error_reporter
);
lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert_document(
    lxb_html_document_t *document,
    xmlDocPtr *doc_out,
    bool compact_text_nodes,
    bool create_default_ns,
    php_dom_private_data *private_data
);
lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert_fragment(
    lxb_dom_node_t *start_node,
    xmlDocPtr lxml_doc,
    xmlNodePtr *fragment_out,
    bool compact_text_nodes,
    bool create_default_ns,
    php_dom_private_data *private_data
);
void lexbor_libxml2_bridge_report_errors(
    const lexbor_libxml2_bridge_parse_context *ctx,
    lxb_html_parser_t *parser,
    const lxb_char_t *input_html,
    size_t chunk_offset,
    size_t *error_index_offset_tokenizer,
    size_t *error_index_offset_tree
);
void lexbor_libxml2_bridge_copy_observations(
    lxb_html_tree_t *tree,
    lexbor_libxml2_bridge_extracted_observations *observations
);

#endif
