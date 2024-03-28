/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/dom/interfaces/document.h"
#include "lexbor/dom/interfaces/element.h"
#include "lexbor/dom/interfaces/text.h"
#include "lexbor/dom/interfaces/document_fragment.h"
#include "lexbor/dom/interfaces/comment.h"
#include "lexbor/dom/interfaces/cdata_section.h"
#include "lexbor/dom/interfaces/cdata_section.h"
#include "lexbor/dom/interfaces/processing_instruction.h"


lxb_dom_document_t *
lxb_dom_document_interface_create(lxb_dom_document_t *document)
{
    lxb_dom_document_t *doc;

    doc = lexbor_mraw_calloc(document->mraw, sizeof(lxb_dom_document_t));
    if (doc == NULL) {
        return NULL;
    }

    (void) lxb_dom_document_init(doc, document, lxb_dom_interface_create,
                                 lxb_dom_interface_clone, lxb_dom_interface_destroy,
                                 LXB_DOM_DOCUMENT_DTYPE_UNDEF, 0);

    return doc;
}

lxb_dom_document_t *
lxb_dom_document_interface_clone(lxb_dom_document_t *document,
                                 const lxb_dom_document_t *doc)
{
    lxb_dom_document_t *new;

    new = lxb_dom_document_interface_create(document);
    if (new == NULL) {
        return NULL;
    }

    new->doctype = doc->doctype;
    new->compat_mode = doc->compat_mode;
    new->type = doc->type;
    new->user = doc->user;

    return new;
}

lxb_dom_document_t *
lxb_dom_document_interface_destroy(lxb_dom_document_t *document)
{
    (void) lxb_dom_node_interface_destroy(lxb_dom_interface_node(document));

    return NULL;
}

lxb_dom_document_t *
lxb_dom_document_create(lxb_dom_document_t *owner)
{
    if (owner != NULL) {
        return lexbor_mraw_calloc(owner->mraw, sizeof(lxb_dom_document_t));
    }

    return lexbor_calloc(1, sizeof(lxb_dom_document_t));
}

lxb_status_t
lxb_dom_document_init(lxb_dom_document_t *document, lxb_dom_document_t *owner,
                      lxb_dom_interface_create_f create_interface,
                      lxb_dom_interface_clone_f clone_interface,
                      lxb_dom_interface_destroy_f destroy_interface,
                      lxb_dom_document_dtype_t type, unsigned int ns)
{
    lxb_status_t status;
    lxb_dom_node_t *node;

    if (document == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    document->type = type;
    document->create_interface = create_interface;
    document->clone_interface = clone_interface;
    document->destroy_interface = destroy_interface;

    document->ev_insert = NULL;
    document->ev_remove = NULL;
    document->ev_destroy = NULL;

    node = lxb_dom_interface_node(document);

    node->type = LXB_DOM_NODE_TYPE_DOCUMENT;
    node->local_name = LXB_TAG__DOCUMENT;
    node->ns = ns;

    if (owner != NULL) {
        document->mraw = owner->mraw;
        document->text = owner->text;
        document->tags = owner->tags;
        document->ns = owner->ns;
        document->prefix = owner->prefix;
        document->attrs = owner->attrs;
        document->parser = owner->parser;
        document->user = owner->user;
        document->scripting = owner->scripting;
        document->compat_mode = owner->compat_mode;

        document->tags_inherited = true;
        document->ns_inherited = true;

        node->owner_document = owner;

        return LXB_STATUS_OK;
    }

    /* For nodes */
    document->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(document->mraw, (4096 * 8));

    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* For text */
    document->text = lexbor_mraw_create();
    status = lexbor_mraw_init(document->text, (4096 * 12));

    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->tags = lexbor_hash_create();
    status = lexbor_hash_init(document->tags, 128, sizeof(lxb_tag_data_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->ns = lexbor_hash_create();
    status = lexbor_hash_init(document->ns, 128, sizeof(lxb_ns_data_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->prefix = lexbor_hash_create();
    status = lexbor_hash_init(document->prefix, 128,
                              sizeof(lxb_dom_attr_data_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    document->attrs = lexbor_hash_create();
    status = lexbor_hash_init(document->attrs, 128,
                              sizeof(lxb_dom_attr_data_t));
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    node->owner_document = document;

    return LXB_STATUS_OK;

failed:

    lexbor_mraw_destroy(document->mraw, true);
    lexbor_mraw_destroy(document->text, true);
    lexbor_hash_destroy(document->tags, true);
    lexbor_hash_destroy(document->ns, true);
    lexbor_hash_destroy(document->attrs, true);
    lexbor_hash_destroy(document->prefix, true);

    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_dom_document_clean(lxb_dom_document_t *document)
{
    if (lxb_dom_interface_node(document)->owner_document == document) {
        lexbor_mraw_clean(document->mraw);
        lexbor_mraw_clean(document->text);
        lexbor_hash_clean(document->tags);
        lexbor_hash_clean(document->ns);
        lexbor_hash_clean(document->attrs);
        lexbor_hash_clean(document->prefix);
    }

    document->node.first_child = NULL;
    document->node.last_child = NULL;
    document->element = NULL;
    document->doctype = NULL;

    return LXB_STATUS_OK;
}

lxb_dom_document_t *
lxb_dom_document_destroy(lxb_dom_document_t *document)
{
    if (document == NULL) {
        return NULL;
    }

    if (lxb_dom_interface_node(document)->owner_document != document) {
        lxb_dom_document_t *owner;

        owner = lxb_dom_interface_node(document)->owner_document;

        return lexbor_mraw_free(owner->mraw, document);
    }

    lexbor_mraw_destroy(document->text, true);
    lexbor_mraw_destroy(document->mraw, true);
    lexbor_hash_destroy(document->tags, true);
    lexbor_hash_destroy(document->ns, true);
    lexbor_hash_destroy(document->attrs, true);
    lexbor_hash_destroy(document->prefix, true);

    return lexbor_free(document);
}

void
lxb_dom_document_attach_doctype(lxb_dom_document_t *document,
                                lxb_dom_document_type_t *doctype)
{
    document->doctype = doctype;
}

void
lxb_dom_document_attach_element(lxb_dom_document_t *document,
                                lxb_dom_element_t *element)
{
    document->element = element;
}

lxb_dom_element_t *
lxb_dom_document_create_element(lxb_dom_document_t *document,
                                const lxb_char_t *local_name, size_t lname_len,
                                void *reserved_for_opt)
{
    /* TODO: If localName does not match the Name production... */

    const lxb_char_t *ns_link;
    size_t ns_len;

    if (document->type == LXB_DOM_DOCUMENT_DTYPE_HTML) {
        ns_link = (const lxb_char_t *) "http://www.w3.org/1999/xhtml";

        /* FIXME: he will get len at the compilation stage?!? */
        ns_len = strlen((const char *) ns_link);
    }
    else {
        ns_link = NULL;
        ns_len = 0;
    }

    return lxb_dom_element_create(document, local_name, lname_len,
                                  ns_link, ns_len, NULL, 0, NULL, 0, true);
}

lxb_dom_element_t *
lxb_dom_document_destroy_element(lxb_dom_element_t *element)
{
    return lxb_dom_element_destroy(element);
}

lxb_dom_document_fragment_t *
lxb_dom_document_create_document_fragment(lxb_dom_document_t *document)
{
    return lxb_dom_document_fragment_interface_create(document);
}

lxb_dom_text_t *
lxb_dom_document_create_text_node(lxb_dom_document_t *document,
                                  const lxb_char_t *data, size_t len)
{
    lxb_dom_text_t *text;

    text = lxb_dom_document_create_interface(document,
                                             LXB_TAG__TEXT, LXB_NS_HTML);
    if (text == NULL) {
        return NULL;
    }

    lexbor_str_init(&text->char_data.data, document->text, len);
    if (text->char_data.data.data == NULL) {
        return lxb_dom_document_destroy_interface(text);
    }

    lexbor_str_append(&text->char_data.data, document->text, data, len);

    return text;
}

lxb_dom_cdata_section_t *
lxb_dom_document_create_cdata_section(lxb_dom_document_t *document,
                                      const lxb_char_t *data, size_t len)
{
    if (document->type != LXB_DOM_DOCUMENT_DTYPE_HTML) {
        return NULL;
    }

    const lxb_char_t *end = data + len;
    const lxb_char_t *ch = memchr(data, ']', sizeof(lxb_char_t) * len);

    while (ch != NULL) {
        if ((end - ch) < 3) {
            break;
        }

        if(memcmp(ch, "]]>", 3) == 0) {
            return NULL;
        }

        ch++;
        ch = memchr(ch, ']', sizeof(lxb_char_t) * (end - ch));
    }

    lxb_dom_cdata_section_t *cdata;

    cdata = lxb_dom_cdata_section_interface_create(document);
    if (cdata == NULL) {
        return NULL;
    }

    lexbor_str_init(&cdata->text.char_data.data, document->text, len);
    if (cdata->text.char_data.data.data == NULL) {
        return lxb_dom_cdata_section_interface_destroy(cdata);
    }

    lexbor_str_append(&cdata->text.char_data.data, document->text, data, len);

    return cdata;
}

lxb_dom_processing_instruction_t *
lxb_dom_document_create_processing_instruction(lxb_dom_document_t *document,
                                               const lxb_char_t *target, size_t target_len,
                                               const lxb_char_t *data, size_t data_len)
{
    /*
     * TODO: If target does not match the Name production,
     * then throw an "InvalidCharacterError" DOMException.
     */

    const lxb_char_t *end = data + data_len;
    const lxb_char_t *ch = memchr(data, '?', sizeof(lxb_char_t) * data_len);

    while (ch != NULL) {
        if ((end - ch) < 2) {
            break;
        }

        if(memcmp(ch, "?>", 2) == 0) {
            return NULL;
        }

        ch++;
        ch = memchr(ch, '?', sizeof(lxb_char_t) * (end - ch));
    }

    lxb_dom_processing_instruction_t *pi;

    pi = lxb_dom_processing_instruction_interface_create(document);
    if (pi == NULL) {
        return NULL;
    }

    lexbor_str_init(&pi->char_data.data, document->text, data_len);
    if (pi->char_data.data.data == NULL) {
        return lxb_dom_processing_instruction_interface_destroy(pi);
    }

    lexbor_str_init(&pi->target, document->text, target_len);
    if (pi->target.data == NULL) {
        lexbor_str_destroy(&pi->char_data.data, document->text, false);

        return lxb_dom_processing_instruction_interface_destroy(pi);
    }

    lexbor_str_append(&pi->char_data.data, document->text, data, data_len);
    lexbor_str_append(&pi->target, document->text, target, target_len);

    return pi;
}


lxb_dom_comment_t *
lxb_dom_document_create_comment(lxb_dom_document_t *document,
                                const lxb_char_t *data, size_t len)
{
    lxb_dom_comment_t *comment;

    comment = lxb_dom_document_create_interface(document, LXB_TAG__EM_COMMENT,
                                                LXB_NS_HTML);
    if (comment == NULL) {
        return NULL;
    }

    lexbor_str_init(&comment->char_data.data, document->text, len);
    if (comment->char_data.data.data == NULL) {
        return lxb_dom_document_destroy_interface(comment);
    }

    lexbor_str_append(&comment->char_data.data, document->text, data, len);

    return comment;
}

lxb_dom_node_t *
lxb_dom_document_root(lxb_dom_document_t *document)
{
    lxb_dom_node_t *node;

    if (document->type == LXB_DOM_DOCUMENT_DTYPE_HTML) {
        node = document->node.first_child;

        while (node != NULL) {
            if (node->local_name == LXB_TAG_HTML) {
                return node;
            }

            node = node->next;
        }
    }

    return document->node.first_child;
}

lxb_dom_node_t *
lxb_dom_document_import_node(lxb_dom_document_t *doc, lxb_dom_node_t *node,
                             bool deep)
{
    lxb_dom_node_t *new, *curr, *cnode, *root;

    new = doc->clone_interface(doc, node);
    if (new == NULL) {
        return NULL;
    }

    if (!deep) {
        return new;
    }

    curr = new;
    root = node;
    node = node->first_child;

    while (node != NULL) {
        cnode = doc->clone_interface(doc, node);
        if (cnode == NULL) {
            return NULL;
        }

        lxb_dom_node_insert_child(curr, cnode);

        if (node->first_child != NULL) {
            node = node->first_child;
            curr = cnode;
        }
        else {
            while (node->next == NULL && node != root) {
                node = node->parent;
                curr = curr->parent;
            }

            if (node == root) {
                break;
            }

            node = node->next;
        }
    }

    return new;
}


/*
 * No inline functions for ABI.
 */
lxb_dom_interface_t *
lxb_dom_document_create_interface_noi(lxb_dom_document_t *document,
                                      lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    return lxb_dom_document_create_interface(document, tag_id, ns);
}

lxb_dom_interface_t *
lxb_dom_document_destroy_interface_noi(lxb_dom_interface_t *intrfc)
{
    return lxb_dom_document_destroy_interface(intrfc);
}

void *
lxb_dom_document_create_struct_noi(lxb_dom_document_t *document,
                                   size_t struct_size)
{
    return lxb_dom_document_create_struct(document, struct_size);
}

void *
lxb_dom_document_destroy_struct_noi(lxb_dom_document_t *document,
                                    void *structure)
{
    return lxb_dom_document_destroy_struct(document, structure);
}

lxb_char_t *
lxb_dom_document_create_text_noi(lxb_dom_document_t *document, size_t len)
{
    return lxb_dom_document_create_text(document, len);
}

void *
lxb_dom_document_destroy_text_noi(lxb_dom_document_t *document,
                                  lxb_char_t *text)
{
    return lxb_dom_document_destroy_text(document, text);
}

lxb_dom_element_t *
lxb_dom_document_element_noi(lxb_dom_document_t *document)
{
    return lxb_dom_document_element(document);
}

bool
lxb_dom_document_scripting_noi(lxb_dom_document_t *document)
{
    return lxb_dom_document_scripting(document);
}

void
lxb_dom_document_scripting_set_noi(lxb_dom_document_t *document,
                                   bool scripting)
{
    lxb_dom_document_scripting_set(document, scripting);
}
