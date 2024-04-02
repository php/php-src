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
#include <lexbor/html/parser.h>
#include <lexbor/html/interfaces/element.h>
#include <libxml/parserInternals.h>
#include <libxml/HTMLtree.h>
#include <Zend/zend.h>

#define WORK_LIST_INIT_SIZE 128
/* libxml2 reserves 2 pointer-sized words for interned strings */
#define LXML_INTERNED_STRINGS_SIZE (sizeof(void *) * 2)

typedef struct _work_list_item {
    lxb_dom_node_t *node;
    uintptr_t current_active_namespace;
    xmlNodePtr lxml_parent;
    xmlNsPtr lxml_ns;
} work_list_item;

static void lexbor_libxml2_bridge_work_list_item_push(
    lexbor_array_obj_t *array,
    lxb_dom_node_t *node,
    uintptr_t current_active_namespace,
    xmlNodePtr lxml_parent,
    xmlNsPtr lxml_ns
)
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

static const php_dom_ns_magic_token *get_libxml_namespace_href(uintptr_t lexbor_namespace)
{
    if (lexbor_namespace == LXB_NS_SVG) {
        return php_dom_ns_is_svg_magic_token;
    } else if (lexbor_namespace == LXB_NS_MATH) {
        return php_dom_ns_is_mathml_magic_token;
    } else {
        return php_dom_ns_is_html_magic_token;
    }
}

static xmlNodePtr lexbor_libxml2_bridge_new_text_node_fast(xmlDocPtr lxml_doc, const lxb_char_t *data, size_t data_length, bool compact_text_nodes)
{
    if (compact_text_nodes && data_length < LXML_INTERNED_STRINGS_SIZE) {
        /* See xmlSAX2TextNode() in libxml2 */
        xmlNodePtr lxml_text = xmlMalloc(sizeof(*lxml_text));
        if (UNEXPECTED(lxml_text == NULL)) {
            return NULL;
        }
        memset(lxml_text, 0, sizeof(*lxml_text));
        lxml_text->name = xmlStringText;
        lxml_text->type = XML_TEXT_NODE;
        lxml_text->doc = lxml_doc;
        lxml_text->content = BAD_CAST &lxml_text->properties;
        if (data != NULL) {
            memcpy(lxml_text->content, data, data_length);
        }
        return lxml_text;
    } else {
        return xmlNewDocTextLen(lxml_doc, (const xmlChar *) data, data_length);
    }
}

static lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert(
    lxb_dom_node_t *start_node,
    xmlDocPtr lxml_doc,
    bool compact_text_nodes,
    bool create_default_ns,
    php_dom_libxml_ns_mapper *ns_mapper
)
{
    lexbor_libxml2_bridge_status retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OK;

    xmlNsPtr html_ns = php_dom_libxml_ns_mapper_ensure_html_ns(ns_mapper);
    xmlNsPtr xlink_ns = NULL;
    xmlNsPtr prefixed_xmlns_ns = NULL;

    lexbor_array_obj_t work_list;
    lexbor_array_obj_init(&work_list, WORK_LIST_INIT_SIZE, sizeof(work_list_item));

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
                if (entering_namespace == LXB_NS_HTML) {
                    current_lxml_ns = html_ns;
                } else {
                    const php_dom_ns_magic_token *magic_token = get_libxml_namespace_href(entering_namespace);
                    zend_string *uri = zend_string_init((char *) magic_token, strlen((char *) magic_token), false);
                    current_lxml_ns = php_dom_libxml_ns_mapper_get_ns(ns_mapper, NULL, uri);
                    zend_string_release_ex(uri, false);
                    if (EXPECTED(current_lxml_ns != NULL)) {
                        current_lxml_ns->_private = (void *) magic_token;
                    }
                }
            }
            /* Instead of xmlSetNs() because we know the arguments are valid. Prevents overhead. */
            lxml_element->ns = current_lxml_ns;

            for (lxb_dom_node_t *child_node = element->node.last_child; child_node != NULL; child_node = child_node->prev) {
                lexbor_libxml2_bridge_work_list_item_push(
                    &work_list,
                    child_node,
                    entering_namespace,
                    lxml_element,
                    current_lxml_ns
                );
            }

            xmlAttrPtr last_added_attr = NULL;
            for (lxb_dom_attr_t *attr = element->first_attr; attr != NULL; attr = attr->next) {
                /* Same namespace remark as for elements */
                size_t local_name_length, value_length;
                const lxb_char_t *local_name = lxb_dom_attr_local_name(attr, &local_name_length);
                const lxb_char_t *value = lxb_dom_attr_value(attr, &value_length);

                if (UNEXPECTED(local_name_length >= INT_MAX || value_length >= INT_MAX)) {
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OVERFLOW;
                    goto out;
                }

                xmlAttrPtr lxml_attr = xmlMalloc(sizeof(xmlAttr));
                if (UNEXPECTED(lxml_attr == NULL)) {
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                    goto out;
                }

                memset(lxml_attr, 0, sizeof(xmlAttr));
                lxml_attr->type = XML_ATTRIBUTE_NODE;
                lxml_attr->parent = lxml_element;
                lxml_attr->name = xmlDictLookup(lxml_doc->dict, local_name, local_name_length);
                lxml_attr->doc = lxml_doc;
                xmlNodePtr lxml_text = lexbor_libxml2_bridge_new_text_node_fast(lxml_doc, value, value_length, true /* Always true for optimization purposes */);
                if (UNEXPECTED(lxml_text == NULL)) {
                    xmlFreeProp(lxml_attr);
                    retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                    goto out;
                }

                lxml_attr->children = lxml_attr->last = lxml_text;
                lxml_text->parent = (xmlNodePtr) lxml_attr;

                if (attr->node.ns == LXB_NS_XMLNS) {
                    if (strcmp((const char *) local_name, "xmlns") != 0) {
                        if (prefixed_xmlns_ns == NULL) {
                            prefixed_xmlns_ns = php_dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(ns_mapper, "xmlns", DOM_XMLNS_NS_URI);
                        }
                        lxml_attr->ns = prefixed_xmlns_ns;
                    } else {
                        lxml_attr->ns = php_dom_libxml_ns_mapper_ensure_prefixless_xmlns_ns(ns_mapper);
                    }
                    lxml_attr->ns->_private = (void *) php_dom_ns_is_xmlns_magic_token;
                } else if (attr->node.ns == LXB_NS_XLINK) {
                    if (xlink_ns == NULL) {
                        xlink_ns = php_dom_libxml_ns_mapper_get_ns_raw_strings_nullsafe(ns_mapper, "xlink", DOM_XLINK_NS_URI);
                        xlink_ns->_private = (void *) php_dom_ns_is_xlink_magic_token;
                    }
                    lxml_attr->ns = xlink_ns;
                }

                if (last_added_attr == NULL) {
                    lxml_element->properties = lxml_attr;
                } else {
                    last_added_attr->next = lxml_attr;
                    lxml_attr->prev = last_added_attr;
                }
                last_added_attr = lxml_attr;

                /* xmlIsID does some other stuff too that is irrelevant here. */
                if (local_name_length == 2 && local_name[0] == 'i' && local_name[1] == 'd') {
                    xmlAddID(NULL, lxml_doc, value, lxml_attr);
                }

                /* libxml2 doesn't support line numbers on this anyway, it derives them instead, so don't bother */
            }
        } else if (node->type == LXB_DOM_NODE_TYPE_TEXT) {
            lxb_dom_text_t *text = lxb_dom_interface_text(node);
            const lxb_char_t *data = text->char_data.data.data;
            size_t data_length = text->char_data.data.length;
            if (UNEXPECTED(data_length >= INT_MAX)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OVERFLOW;
                goto out;
            }
            xmlNodePtr lxml_text = lexbor_libxml2_bridge_new_text_node_fast(lxml_doc, data, data_length, compact_text_nodes);
            if (UNEXPECTED(lxml_text == NULL)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                goto out;
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
            xmlDtdPtr lxml_dtd = xmlCreateIntSubset(
                lxml_doc,
                name,
                public_id_len ? public_id : NULL,
                system_id_len ? system_id : NULL
            );
            if (UNEXPECTED(lxml_dtd == NULL)) {
                retval = LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
                goto out;
            }
            /* libxml2 doesn't support line numbers on this anyway, it returns -1 instead, so don't bother */
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

void lexbor_libxml2_bridge_parse_set_error_callbacks(
    lexbor_libxml2_bridge_parse_context *ctx,
    lexbor_libxml2_bridge_tokenizer_error_reporter tokenizer_error_reporter,
    lexbor_libxml2_bridge_tree_error_reporter tree_error_reporter
)
{
    ctx->tokenizer_error_reporter = tokenizer_error_reporter;
    ctx->tree_error_reporter = tree_error_reporter;
}

lexbor_libxml2_bridge_status lexbor_libxml2_bridge_convert_document(
    lxb_html_document_t *document,
    xmlDocPtr *doc_out,
    bool compact_text_nodes,
    bool create_default_ns,
    php_dom_libxml_ns_mapper *ns_mapper
)
{
    xmlDocPtr lxml_doc = php_dom_create_html_doc();
    if (UNEXPECTED(!lxml_doc)) {
        return LEXBOR_LIBXML2_BRIDGE_STATUS_OOM;
    }
    lexbor_libxml2_bridge_status status = lexbor_libxml2_bridge_convert(
        lxb_dom_interface_node(document)->last_child,
        lxml_doc,
        compact_text_nodes,
        create_default_ns,
        ns_mapper
    );
    if (status != LEXBOR_LIBXML2_BRIDGE_STATUS_OK) {
        xmlFreeDoc(lxml_doc);
        return status;
    }
    *doc_out = lxml_doc;
    return LEXBOR_LIBXML2_BRIDGE_STATUS_OK;
}

void lexbor_libxml2_bridge_report_errors(
    const lexbor_libxml2_bridge_parse_context *ctx,
    lxb_html_parser_t *parser,
    const lxb_char_t *input_html,
    size_t chunk_offset,
    size_t *error_index_offset_tokenizer,
    size_t *error_index_offset_tree
)
{
    void *error;

    /* Tokenizer errors */
    lexbor_array_obj_t *parse_errors = lxb_html_parser_tokenizer(parser)->parse_errors;
    size_t index = *error_index_offset_tokenizer;
    while ((error = lexbor_array_obj_get(parse_errors, index)) != NULL) {
        /* See https://github.com/lexbor/lexbor/blob/master/source/lexbor/html/tokenizer/error.h */
        lxb_html_tokenizer_error_t *token_error = error;
        if (ctx->tokenizer_error_reporter) {
            ctx->tokenizer_error_reporter(
                ctx->application_data,
                token_error,
                token_error->pos - input_html + chunk_offset
            );
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
            ctx->tree_error_reporter(
                ctx->application_data,
                tree_error,
                tree_error->line + 1,
                tree_error->column + 1,
                tree_error->length
            );
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
