/*
 * Copyright (C) 2018-2021 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_DOM_DOCUMENT_H
#define LEXBOR_DOM_DOCUMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/mraw.h"
#include "lexbor/core/hash.h"

#include "lexbor/dom/interface.h"
#include "lexbor/dom/interfaces/node.h"


typedef enum {
    LXB_DOM_DOCUMENT_CMODE_NO_QUIRKS       = 0x00,
    LXB_DOM_DOCUMENT_CMODE_QUIRKS          = 0x01,
    LXB_DOM_DOCUMENT_CMODE_LIMITED_QUIRKS  = 0x02
}
lxb_dom_document_cmode_t;

typedef enum {
    LXB_DOM_DOCUMENT_DTYPE_UNDEF = 0x00,
    LXB_DOM_DOCUMENT_DTYPE_HTML  = 0x01,
    LXB_DOM_DOCUMENT_DTYPE_XML   = 0x02
}
lxb_dom_document_dtype_t;

struct lxb_dom_document {
    lxb_dom_node_t              node;

    lxb_dom_document_cmode_t    compat_mode;
    lxb_dom_document_dtype_t    type;

    lxb_dom_document_type_t     *doctype;
    lxb_dom_element_t           *element;

    lxb_dom_interface_create_f  create_interface;
    lxb_dom_interface_clone_f   clone_interface;
    lxb_dom_interface_destroy_f destroy_interface;

    lxb_dom_event_insert_f      ev_insert;
    lxb_dom_event_remove_f      ev_remove;
    lxb_dom_event_destroy_f     ev_destroy;
    lxb_dom_event_set_value_f   ev_set_value;

    lexbor_mraw_t               *mraw;
    lexbor_mraw_t               *text;
    lexbor_hash_t               *tags;
    lexbor_hash_t               *attrs;
    lexbor_hash_t               *prefix;
    lexbor_hash_t               *ns;
    void                        *parser;
    void                        *user;

    bool                        tags_inherited;
    bool                        ns_inherited;

    bool                        scripting;
};


LXB_API lxb_dom_document_t *
lxb_dom_document_interface_create(lxb_dom_document_t *document);

LXB_API lxb_dom_document_t *
lxb_dom_document_interface_clone(lxb_dom_document_t *document,
                                 const lxb_dom_document_t *doc);

LXB_API lxb_dom_document_t *
lxb_dom_document_interface_destroy(lxb_dom_document_t *document);

LXB_API lxb_dom_document_t *
lxb_dom_document_create(lxb_dom_document_t *owner);

LXB_API lxb_status_t
lxb_dom_document_init(lxb_dom_document_t *document, lxb_dom_document_t *owner,
                      lxb_dom_interface_create_f create_interface,
                      lxb_dom_interface_clone_f clone_interface,
                      lxb_dom_interface_destroy_f destroy_interface,
                      lxb_dom_document_dtype_t type, unsigned int ns);

LXB_API lxb_status_t
lxb_dom_document_clean(lxb_dom_document_t *document);

LXB_API lxb_dom_document_t *
lxb_dom_document_destroy(lxb_dom_document_t *document);

LXB_API void
lxb_dom_document_attach_doctype(lxb_dom_document_t *document,
                                lxb_dom_document_type_t *doctype);

LXB_API void
lxb_dom_document_attach_element(lxb_dom_document_t *document,
                                lxb_dom_element_t *element);

LXB_API lxb_dom_element_t *
lxb_dom_document_create_element(lxb_dom_document_t *document,
                                const lxb_char_t *local_name, size_t lname_len,
                                void *reserved_for_opt);

LXB_API lxb_dom_element_t *
lxb_dom_document_destroy_element(lxb_dom_element_t *element);

LXB_API lxb_dom_document_fragment_t *
lxb_dom_document_create_document_fragment(lxb_dom_document_t *document);

LXB_API lxb_dom_text_t *
lxb_dom_document_create_text_node(lxb_dom_document_t *document,
                                  const lxb_char_t *data, size_t len);

LXB_API lxb_dom_cdata_section_t *
lxb_dom_document_create_cdata_section(lxb_dom_document_t *document,
                                      const lxb_char_t *data, size_t len);

LXB_API lxb_dom_processing_instruction_t *
lxb_dom_document_create_processing_instruction(lxb_dom_document_t *document,
                                               const lxb_char_t *target, size_t target_len,
                                               const lxb_char_t *data, size_t data_len);

LXB_API lxb_dom_comment_t *
lxb_dom_document_create_comment(lxb_dom_document_t *document,
                                const lxb_char_t *data, size_t len);

LXB_API lxb_dom_node_t *
lxb_dom_document_root(lxb_dom_document_t *document);

LXB_API lxb_dom_node_t *
lxb_dom_document_import_node(lxb_dom_document_t *doc, lxb_dom_node_t *node,
                             bool deep);

/*
 * Inline functions
 */
lxb_inline lxb_dom_interface_t *
lxb_dom_document_create_interface(lxb_dom_document_t *document,
                                  lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    return document->create_interface(document, tag_id, ns);
}

lxb_inline lxb_dom_interface_t *
lxb_dom_document_destroy_interface(lxb_dom_interface_t *intrfc)
{
    return lxb_dom_interface_node(intrfc)->owner_document->destroy_interface(intrfc);
}

lxb_inline void *
lxb_dom_document_create_struct(lxb_dom_document_t *document, size_t struct_size)
{
    return lexbor_mraw_calloc(document->mraw, struct_size);
}

lxb_inline void *
lxb_dom_document_destroy_struct(lxb_dom_document_t *document, void *structure)
{
    return lexbor_mraw_free(document->mraw, structure);
}

lxb_inline lxb_char_t *
lxb_dom_document_create_text(lxb_dom_document_t *document, size_t len)
{
    return (lxb_char_t *) lexbor_mraw_alloc(document->text,
                                            sizeof(lxb_char_t) * len);
}

lxb_inline void *
lxb_dom_document_destroy_text(lxb_dom_document_t *document, lxb_char_t *text)
{
    return lexbor_mraw_free(document->text, text);
}

lxb_inline lxb_dom_element_t *
lxb_dom_document_element(lxb_dom_document_t *document)
{
    return document->element;
}

lxb_inline bool
lxb_dom_document_scripting(lxb_dom_document_t *document)
{
    return document->scripting;
}

lxb_inline void
lxb_dom_document_scripting_set(lxb_dom_document_t *document, bool scripting)
{
    document->scripting = scripting;
}

lxb_inline lxb_dom_document_t *
lxb_dom_document_owner(lxb_dom_document_t *document)
{
    return lxb_dom_interface_node(document)->owner_document;
}

lxb_inline bool
lxb_dom_document_is_original(lxb_dom_document_t *document)
{
    return lxb_dom_interface_node(document)->owner_document == document;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_dom_interface_t *
lxb_dom_document_create_interface_noi(lxb_dom_document_t *document,
                                      lxb_tag_id_t tag_id, lxb_ns_id_t ns);

LXB_API lxb_dom_interface_t *
lxb_dom_document_destroy_interface_noi(lxb_dom_interface_t *intrfc);

LXB_API void *
lxb_dom_document_create_struct_noi(lxb_dom_document_t *document,
                                   size_t struct_size);

LXB_API void *
lxb_dom_document_destroy_struct_noi(lxb_dom_document_t *document,
                                    void *structure);

LXB_API lxb_char_t *
lxb_dom_document_create_text_noi(lxb_dom_document_t *document, size_t len);

LXB_API void *
lxb_dom_document_destroy_text_noi(lxb_dom_document_t *document,
                                  lxb_char_t *text);

LXB_API lxb_dom_element_t *
lxb_dom_document_element_noi(lxb_dom_document_t *document);

LXB_API bool
lxb_dom_document_scripting_noi(lxb_dom_document_t *document);

LXB_API void
lxb_dom_document_scripting_set_noi(lxb_dom_document_t *document,
                                   bool scripting);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DOM_DOCUMENT_H */
