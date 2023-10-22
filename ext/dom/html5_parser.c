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
#include "html5_parser.h"
#include "namespace_compat.h"
#include <lexbor/html/parser.h>
#include <lexbor/html/interfaces/element.h>
#include <libxml/tree.h>
#include <libxml/parserInternals.h>
#include <libxml/HTMLtree.h>
#include <Zend/zend.h>

typedef struct {
    lxb_dom_node_t *node;
    uintptr_t current_active_namespace;
    xmlNodePtr lxml_parent;
    xmlNsPtr lxml_ns;
} work_list_item;

static void lexbor_libxml2_bridge_work_list_item_push(lexbor_array_obj_t *array, lxb_dom_node_t *node, uintptr_t current_active_namespace, xmlNodePtr lxml_parent, xmlNsPtr lxml_ns)
{
    work_list_item *item = (work_list_item *) lexbor_array_obj_push_wo_cls(array);
    item->node = node;
    item->current_active_namespace = current_active_namespace;
    item->lxml_parent = lxml_parent;
    item->lxml_ns = lxml_ns;
}

static unsigned short sanitize_line_nr(size_t line)
{
    if (line > USHRT_MAX) {
        return USHRT_MAX;
    }
    return (unsigned short) line;
}

static const xmlChar *get_libxml_namespace_href(uintptr_t lexbor_namespace)
{
    if (lexbor_namespace == LXB_NS_SVG) {
        return (const xmlChar *) DOM_SVG_NS_URI;
    } else if (lexbor_namespace == LXB_NS_MATH) {
        return (const xmlChar *) DOM_MATHML_NS_URI;
    } else {
        return (const xmlChar *) DOM_XHTML_NS_URI;
    }
}

static lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert(lxb_dom_node_t *start_node, xmlDocPtr lxml_doc, bool compact_text_nodes, bool create_default_ns)
{
    lexbor_libxml2_bridge_status retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OK;

    lexbor_array_obj_t work_list;
    lexbor_array_obj_init(&work_list, 128, sizeof(work_list_item));

    for (lxb_dom_node_t *node = start_node; node != NULL; node = node->prev) {
        lexbor_libxml2_bridge_work_list_item_push(&work_list, node, LXB_NS__UNDEF, (xmlNodePtr) lxml_doc, NULL);
    }

    work_list_item *current_stack_item;
    while ((current_stack_item = lexbor_array_obj_pop(&work_list)) != NULL) {
        lxb_dom_node_t *node = current_stack_item->node;
        xmlNodePtr lxml_parent = current_stack_item->lxml_parent;

        /* CDATA section and processing instructions don't occur in parsed HTML documents.
         * The historical types are not emitted by the parser either. */
        if (node->type == LXB_DOM_NODE_TYPE_ELEMENT) {
            /* Note: HTML isn't exactly XML-namespace-aware; as this is an HTML parser we only care about the local name.
             * If a prefix:name format is used, then the local name will be "prefix:name" and the prefix will be empty.
             * There is however still somewhat of a concept of namespaces. There are three: HTML (the default), SVG, and MATHML. */
            lxb_dom_element_t *element = lxb_dom_interface_element(node);
            const lxb_char_t *name = lxb_dom_element_local_name(element, NULL);
            xmlNodePtr lxml_element = xmlNewDocNode(lxml_doc, NULL, name, NULL);
            if (UNEXPECTED(lxml_element == NULL)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                goto out;
            }
            xmlAddChild(lxml_parent, lxml_element);
            lxml_element->line = sanitize_line_nr(node->line);

            /* Namespaces, note: namespace switches are uncommon */
            uintptr_t entering_namespace = element->node.ns;
            xmlNsPtr current_lxml_ns = current_stack_item->lxml_ns;
            if (create_default_ns && UNEXPECTED(entering_namespace != current_stack_item->current_active_namespace)) {
                current_lxml_ns = xmlNewNs(lxml_element, get_libxml_namespace_href(entering_namespace), NULL);
            }
            lxml_element->ns = current_lxml_ns; /* Instead of xmlSetNs() because we know the arguments are valid. Prevents overhead. */

            for (lxb_dom_node_t *child_node = element->node.last_child; child_node != NULL; child_node = child_node->prev) {
                lexbor_libxml2_bridge_work_list_item_push(&work_list, child_node, entering_namespace, lxml_element, current_lxml_ns);
            }

            for (lxb_dom_attr_t *attr = element->last_attr; attr != NULL; attr = attr->prev) {
                lexbor_libxml2_bridge_work_list_item_push(&work_list, (lxb_dom_node_t *) attr, entering_namespace, lxml_element, current_lxml_ns);
            }
        } else if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
            lxb_dom_text_t *text = lxb_dom_interface_text(node);
            const lxb_char_t *data = text->char_data.data.data;
            size_t data_length = text->char_data.data.length;
            if (UNEXPECTED(data_length >= INT_MAX)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OVERFLOW;
                goto out;
            }
            xmlNodePtr lxml_text;
            if (compact_text_nodes && data_length < sizeof(void *) * 2) {
                /* See xmlSAX2TextNode() in libxml2 */
                lxml_text = xmlMalloc(sizeof(xmlNode));
                if (UNEXPECTED(lxml_text == NULL)) {
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                    goto out;
                }
                memset(lxml_text, 0, sizeof(xmlNode));
                lxml_text->name = xmlStringText;
                lxml_text->type = XML_TEXT_NODE;
                lxml_text->doc = lxml_doc;
                lxml_text->content = (xmlChar *) &lxml_text->properties;
                memcpy(lxml_text->content, data, data_length + 1 /* include '\0' */);
            } else {
                lxml_text = xmlNewDocTextLen(lxml_doc, (const xmlChar *) data, data_length);
                if (UNEXPECTED(lxml_text == NULL)) {
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                    goto out;
                }
            }
            xmlAddChild(lxml_parent, lxml_text);
            if (node->line >= USHRT_MAX) {
                lxml_text->line = USHRT_MAX;
                lxml_text->psvi = (void *) (ptrdiff_t) node->line;
            } else {
                lxml_text->line = (unsigned short) node->line;
            }
        } else if (node->type == LXB_DOM_NODE_TYPE_DOCUMENT_TYPE) {
            lxb_dom_document_type_t *doctype = lxb_dom_interface_document_type(node);
            const lxb_char_t *name = lxb_dom_document_type_name(doctype, NULL);
            size_t public_id_len, system_id_len;
            const lxb_char_t *public_id = lxb_dom_document_type_public_id(doctype, &public_id_len);
            const lxb_char_t *system_id = lxb_dom_document_type_system_id(doctype, &system_id_len);
            xmlDtdPtr lxml_dtd = xmlCreateIntSubset(lxml_doc, name, public_id_len ? public_id : NULL, system_id_len ? system_id : NULL);
            if (UNEXPECTED(lxml_dtd == NULL)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                goto out;
            }
            /* libxml2 doesn't support line numbers on this anyway, it returns -1 instead, so don't bother */
        } else if (node->type == LXB_DOM_NODE_TYPE_ATTRIBUTE) {
            lxb_dom_attr_t *attr = lxb_dom_interface_attr(node);
            do {
                /* Same namespace remark as for elements */
                const lxb_char_t *local_name = lxb_dom_attr_local_name(attr, NULL);
                const lxb_char_t *value = lxb_dom_attr_value(attr, NULL);
                xmlAttrPtr lxml_attr = xmlSetNsProp(lxml_parent, NULL, local_name, value);
                if (UNEXPECTED(lxml_attr == NULL)) {
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                    goto out;
                }
                attr = attr->next;
                /* libxml2 doesn't support line numbers on this anyway, it derives them instead, so don't bother */
            } while (attr);
        } else if (node->type == LXB_DOM_NODE_TYPE_COMMENT) {
            lxb_dom_comment_t *comment = lxb_dom_interface_comment(node);
            xmlNodePtr lxml_comment = xmlNewDocComment(lxml_doc, comment->char_data.data.data);
            if (UNEXPECTED(lxml_comment == NULL)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                goto out;
            }
            xmlAddChild(lxml_parent, lxml_comment);
            lxml_comment->line = sanitize_line_nr(node->line);
        }
    }

out:
    lexbor_array_obj_destroy(&work_list, false);
    return retval;
}

void lexbor_libxml2_bridge_parse_context_init(lexbor_libxml2_bridge_parse_context *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
}

void lexbor_libxml2_bridge_parse_set_error_callbacks(lexbor_libxml2_bridge_parse_context *ctx, lexbor_libxml2_bridge_tokenizer_error_reporter tokenizer_error_reporter, lexbor_libxml2_bridge_tree_error_reporter tree_error_reporter)
{
    ctx->tokenizer_error_reporter = tokenizer_error_reporter;
    ctx->tree_error_reporter = tree_error_reporter;
}

lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert_document(lxb_html_document_t *document, xmlDocPtr *doc_out, bool compact_text_nodes, bool create_default_ns)
{
#ifdef LIBXML_HTML_ENABLED
    xmlDocPtr lxml_doc = htmlNewDocNoDtD(NULL, NULL);
#else
    /* If HTML support is not enabled, then htmlNewDocNoDtD() is not available.
     * This code mimics the behaviour. */
    xmlDocPtr lxml_doc = xmlNewDoc((const xmlChar *) "1.0");
    lxml_doc->type = XML_HTML_DOCUMENT_NODE;
#endif
    if (!lxml_doc) {
        return LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
    }
    lexbor_libxml2_bridge_status status = lexbor_libxml2_bridge_convert(lxb_dom_interface_node(document)->last_child, lxml_doc, compact_text_nodes, create_default_ns);
    if (status != LEXBOR_LIBXML2_BRIDGE_STATUS_OK) {
        xmlFreeDoc(lxml_doc);
        return status;
    }
    *doc_out = lxml_doc;
    return LEXBOR_LIBXML2_BRIDGE_STATUS_OK;
}

void lexbor_libxml2_bridge_report_errors(const lexbor_libxml2_bridge_parse_context *ctx, lxb_html_parser_t *parser, const lxb_char_t *input_html, size_t chunk_offset, size_t *error_index_offset_tokenizer, size_t *error_index_offset_tree)
{
    void *error;

    /* Tokenizer errors */
    lexbor_array_obj_t *parse_errors = lxb_html_parser_tokenizer(parser)->parse_errors;
    size_t index = *error_index_offset_tokenizer;
    while ((error = lexbor_array_obj_get(parse_errors, index)) != NULL) {
        /* See https://github.com/lexbor/lexbor/blob/master/source/lexbor/html/tokenizer/error.h */
        lxb_html_tokenizer_error_t *token_error = error;
        if (ctx->tokenizer_error_reporter) {
            ctx->tokenizer_error_reporter(ctx->application_data, token_error, token_error->pos - input_html + chunk_offset);
        }
        index++;
    }
    *error_index_offset_tokenizer = index;

    /* Tree parser errors */
    parse_errors = lxb_html_parser_tree(parser)->parse_errors;
    index = *error_index_offset_tree;
    while ((error = lexbor_array_obj_get(parse_errors, index)) != NULL) {
        /* See https://github.com/lexbor/lexbor/blob/master/source/lexbor/html/tree/error.h */
        lxb_html_tree_error_t *tree_error = error;
        if (ctx->tree_error_reporter) {
            ctx->tree_error_reporter(ctx->application_data, tree_error, tree_error->line + 1, tree_error->column + 1, tree_error->length);
        }
        index++;
    }
    *error_index_offset_tree = index;
}

void lexbor_libxml2_bridge_copy_observations(lxb_html_tree_t *tree, lexbor_libxml2_bridge_extracted_observations *observations)
{
    observations->has_explicit_html_tag = tree->has_explicit_html_tag;
    observations->has_explicit_head_tag = tree->has_explicit_head_tag;
    observations->has_explicit_body_tag = tree->has_explicit_body_tag;
}

#endif  /* HAVE_LIBXML && HAVE_DOM */
