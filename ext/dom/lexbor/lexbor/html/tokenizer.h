/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_TOKENIZER_H
#define LEXBOR_HTML_TOKENIZER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/sbst.h"
#include "lexbor/core/array_obj.h"

#include "lexbor/html/base.h"
#include "lexbor/html/token.h"

#include "lexbor/tag/tag.h"
#include "lexbor/ns/ns.h"


/* State */
typedef const lxb_char_t *
(*lxb_html_tokenizer_state_f)(lxb_html_tokenizer_t *tkz,
                              const lxb_char_t *data, const lxb_char_t *end);

typedef lxb_html_token_t *
(*lxb_html_tokenizer_token_f)(lxb_html_tokenizer_t *tkz,
                              lxb_html_token_t *token, void *ctx);


struct lxb_html_tokenizer {
    lxb_html_tokenizer_state_f       state;
    lxb_html_tokenizer_state_f       state_return;

    lxb_html_tokenizer_token_f       callback_token_done;
    void                             *callback_token_ctx;

    lexbor_hash_t                    *tags;
    lexbor_hash_t                    *attrs;
    lexbor_mraw_t                    *attrs_mraw;

    /* For a temp strings and other templary data */
    lexbor_mraw_t                    *mraw;

    /* Current process token */
    lxb_html_token_t                 *token;

    /* Memory for token and attr */
    lexbor_dobject_t                 *dobj_token;
    lexbor_dobject_t                 *dobj_token_attr;

    /* Parse error */
    lexbor_array_obj_t               *parse_errors;

    /*
     * Leak abstractions.
     * The only place where the specification causes mixing Tree Builder
     * and Tokenizer. We kill all beauty.
     * Current Tree parser. This is not ref (not ref count).
     */
    lxb_html_tree_t                  *tree;

    /* Temp */
    const lxb_char_t                 *markup;
    const lxb_char_t                 *temp;
    lxb_tag_id_t                     tmp_tag_id;

    lxb_char_t                       *start;
    lxb_char_t                       *pos;
    const lxb_char_t                 *end;
    const lxb_char_t                 *begin;
    const lxb_char_t                 *last;
    size_t                            current_line;
    size_t                            current_column;

    /* Entities */
    const lexbor_sbst_entry_static_t *entity;
    const lexbor_sbst_entry_static_t *entity_match;
    uintptr_t                        entity_start;
    uintptr_t                        entity_end;
    uint32_t                         entity_length;
    uint32_t                         entity_number;
    bool                             is_attribute;

    /* Process */
    lxb_html_tokenizer_opt_t         opt;
    lxb_status_t                     status;
    bool                             is_eof;

    lxb_html_tokenizer_t             *base;
    size_t                           ref_count;
};


#include "lexbor/html/tokenizer/error.h"


extern const lxb_char_t *lxb_html_tokenizer_eof;

LXB_API lxb_html_tokenizer_t *
lxb_html_tokenizer_create(void);

LXB_API lxb_status_t
lxb_html_tokenizer_init(lxb_html_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_html_tokenizer_inherit(lxb_html_tokenizer_t *tkz_to,
                           lxb_html_tokenizer_t *tkz_from);

LXB_API lxb_html_tokenizer_t *
lxb_html_tokenizer_ref(lxb_html_tokenizer_t *tkz);

LXB_API lxb_html_tokenizer_t *
lxb_html_tokenizer_unref(lxb_html_tokenizer_t *tkz);

LXB_API void
lxb_html_tokenizer_clean(lxb_html_tokenizer_t *tkz);

LXB_API lxb_html_tokenizer_t *
lxb_html_tokenizer_destroy(lxb_html_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_html_tokenizer_tags_make(lxb_html_tokenizer_t *tkz, size_t table_size);

LXB_API void
lxb_html_tokenizer_tags_destroy(lxb_html_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_html_tokenizer_attrs_make(lxb_html_tokenizer_t *tkz, size_t table_size);

LXB_API void
lxb_html_tokenizer_attrs_destroy(lxb_html_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_html_tokenizer_begin(lxb_html_tokenizer_t *tkz);

LXB_API lxb_status_t
lxb_html_tokenizer_chunk(lxb_html_tokenizer_t *tkz,
                         const lxb_char_t *data, size_t size);

LXB_API lxb_status_t
lxb_html_tokenizer_end(lxb_html_tokenizer_t *tkz);


LXB_API const lxb_char_t *
lxb_html_tokenizer_change_incoming(lxb_html_tokenizer_t *tkz,
                                   const lxb_char_t *pos);

LXB_API lxb_ns_id_t
lxb_html_tokenizer_current_namespace(lxb_html_tokenizer_t *tkz);

LXB_API void
lxb_html_tokenizer_set_state_by_tag(lxb_html_tokenizer_t *tkz, bool scripting,
                                    lxb_tag_id_t tag_id, lxb_ns_id_t ns);


/*
 * Inline functions
 */
lxb_inline void
lxb_html_tokenizer_status_set(lxb_html_tokenizer_t *tkz, lxb_status_t status)
{
    tkz->status = status;
}

lxb_inline void
lxb_html_tokenizer_tags_set(lxb_html_tokenizer_t *tkz, lexbor_hash_t *tags)
{
    tkz->tags = tags;
}

lxb_inline lexbor_hash_t *
lxb_html_tokenizer_tags(lxb_html_tokenizer_t *tkz)
{
    return tkz->tags;
}

lxb_inline void
lxb_html_tokenizer_attrs_set(lxb_html_tokenizer_t *tkz, lexbor_hash_t *attrs)
{
    tkz->attrs = attrs;
}

lxb_inline lexbor_hash_t *
lxb_html_tokenizer_attrs(lxb_html_tokenizer_t *tkz)
{
    return tkz->attrs;
}

lxb_inline void
lxb_html_tokenizer_attrs_mraw_set(lxb_html_tokenizer_t *tkz,
                                  lexbor_mraw_t *mraw)
{
    tkz->attrs_mraw = mraw;
}

lxb_inline lexbor_mraw_t *
lxb_html_tokenizer_attrs_mraw(lxb_html_tokenizer_t *tkz)
{
    return tkz->attrs_mraw;
}

lxb_inline void
lxb_html_tokenizer_callback_token_done_set(lxb_html_tokenizer_t *tkz,
                                           lxb_html_tokenizer_token_f call_func,
                                           void *ctx)
{
    tkz->callback_token_done = call_func;
    tkz->callback_token_ctx = ctx;
}

lxb_inline void *
lxb_html_tokenizer_callback_token_done_ctx(lxb_html_tokenizer_t *tkz)
{
    return tkz->callback_token_ctx;
}

lxb_inline void
lxb_html_tokenizer_state_set(lxb_html_tokenizer_t *tkz,
                             lxb_html_tokenizer_state_f state)
{
    tkz->state = state;
}

lxb_inline void
lxb_html_tokenizer_tmp_tag_id_set(lxb_html_tokenizer_t *tkz,
                                  lxb_tag_id_t tag_id)
{
    tkz->tmp_tag_id = tag_id;
}

lxb_inline lxb_html_tree_t *
lxb_html_tokenizer_tree(lxb_html_tokenizer_t *tkz)
{
    return tkz->tree;
}

lxb_inline void
lxb_html_tokenizer_tree_set(lxb_html_tokenizer_t *tkz, lxb_html_tree_t *tree)
{
    tkz->tree = tree;
}

lxb_inline lexbor_mraw_t *
lxb_html_tokenizer_mraw(lxb_html_tokenizer_t *tkz)
{
    return tkz->mraw;
}

lxb_inline lxb_status_t
lxb_html_tokenizer_temp_realloc(lxb_html_tokenizer_t *tkz, size_t size)
{
    size_t length = tkz->pos - tkz->start;
    size_t new_size = (tkz->end - tkz->start) + size + 4096;

    tkz->start = (lxb_char_t *)lexbor_realloc(tkz->start, new_size);
    if (tkz->start == NULL) {
        tkz->status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return tkz->status;
    }

    tkz->pos = tkz->start + length;
    tkz->end = tkz->start + new_size;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_html_tokenizer_temp_append_data(lxb_html_tokenizer_t *tkz,
                                    const lxb_char_t *data)
{
    size_t size = data - tkz->begin;

    if ((tkz->pos + size) > tkz->end) {
        if(lxb_html_tokenizer_temp_realloc(tkz, size)) {
            return tkz->status;
        }
    }

    tkz->pos = (lxb_char_t *) memcpy(tkz->pos, tkz->begin, size) + size;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_html_tokenizer_temp_append(lxb_html_tokenizer_t *tkz,
                               const lxb_char_t *data, size_t size)
{
    if ((tkz->pos + size) > tkz->end) {
        if(lxb_html_tokenizer_temp_realloc(tkz, size)) {
            return tkz->status;
        }
    }

    tkz->pos = (lxb_char_t *) memcpy(tkz->pos, data, size) + size;

    return LXB_STATUS_OK;
}


/*
 * No inline functions for ABI.
 */
LXB_API void
lxb_html_tokenizer_status_set_noi(lxb_html_tokenizer_t *tkz,
                                  lxb_status_t status);

LXB_API void
lxb_html_tokenizer_callback_token_done_set_noi(lxb_html_tokenizer_t *tkz,
                                               lxb_html_tokenizer_token_f call_func,
                                               void *ctx);

LXB_API void *
lxb_html_tokenizer_callback_token_done_ctx_noi(lxb_html_tokenizer_t *tkz);

LXB_API void
lxb_html_tokenizer_state_set_noi(lxb_html_tokenizer_t *tkz,
                                 lxb_html_tokenizer_state_f state);

LXB_API void
lxb_html_tokenizer_tmp_tag_id_set_noi(lxb_html_tokenizer_t *tkz,
                                      lxb_tag_id_t tag_id);

LXB_API lxb_html_tree_t *
lxb_html_tokenizer_tree_noi(lxb_html_tokenizer_t *tkz);

LXB_API void
lxb_html_tokenizer_tree_set_noi(lxb_html_tokenizer_t *tkz,
                                lxb_html_tree_t *tree);

LXB_API lexbor_mraw_t *
lxb_html_tokenizer_mraw_noi(lxb_html_tokenizer_t *tkz);

LXB_API lexbor_hash_t *
lxb_html_tokenizer_tags_noi(lxb_html_tokenizer_t *tkz);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_TOKENIZER_H */
