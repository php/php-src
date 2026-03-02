/*
 * Copyright (C) 2018-2020 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/html/tokenizer.h"
#include "lexbor/html/tokenizer/state.h"
#include "lexbor/html/tokenizer/state_rcdata.h"
#include "lexbor/html/tokenizer/state_rawtext.h"
#include "lexbor/html/tokenizer/state_script.h"
#include "lexbor/html/tree.h"

#define LXB_HTML_TAG_RES_DATA
#define LXB_HTML_TAG_RES_SHS_DATA
#include "lexbor/html/tag_res.h"


#define LXB_HTML_TKZ_TEMP_SIZE (4096 * 4)


enum {
    LXB_HTML_TOKENIZER_OPT_UNDEF           = 0x00,
    LXB_HTML_TOKENIZER_OPT_TAGS_SELF       = 0x01,
    LXB_HTML_TOKENIZER_OPT_ATTRS_SELF      = 0x02,
    LXB_HTML_TOKENIZER_OPT_ATTRS_MRAW_SELF = 0x04
};


const lxb_char_t *lxb_html_tokenizer_eof = (const lxb_char_t *) "\x00";


static lxb_html_token_t *
lxb_html_tokenizer_token_done(lxb_html_tokenizer_t *tkz,
                              lxb_html_token_t *token, void *ctx);


lxb_html_tokenizer_t *
lxb_html_tokenizer_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_html_tokenizer_t));
}

lxb_status_t
lxb_html_tokenizer_init(lxb_html_tokenizer_t *tkz)
{
    lxb_status_t status;

    if (tkz == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    /* mraw for templary strings or structures */
    tkz->mraw = lexbor_mraw_create();
    status = lexbor_mraw_init(tkz->mraw, 1024);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Init Token */
    tkz->token = NULL;

    tkz->dobj_token = lexbor_dobject_create();
    status = lexbor_dobject_init(tkz->dobj_token,
                                 4096, sizeof(lxb_html_token_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Init Token Attributes */
    tkz->dobj_token_attr = lexbor_dobject_create();
    status = lexbor_dobject_init(tkz->dobj_token_attr, 4096,
                                 sizeof(lxb_html_token_attr_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Parse errors */
    tkz->parse_errors = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tkz->parse_errors, 16,
                                   sizeof(lxb_html_tokenizer_error_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    /* Temporary memory for tag name and attributes. */
    tkz->start = lexbor_malloc(LXB_HTML_TKZ_TEMP_SIZE * sizeof(lxb_char_t));
    if (tkz->start == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    tkz->pos = tkz->start;
    tkz->end = tkz->start + LXB_HTML_TKZ_TEMP_SIZE;
    /* current_line & current_column already initialized by calloc (zero-based) */

    tkz->tree = NULL;
    tkz->tags = NULL;
    tkz->attrs = NULL;
    tkz->attrs_mraw = NULL;

    tkz->state = lxb_html_tokenizer_state_data_before;
    tkz->state_return = NULL;

    tkz->callback_token_done = lxb_html_tokenizer_token_done;
    tkz->callback_token_ctx = NULL;

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;

    tkz->base = NULL;
    tkz->ref_count = 1;

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tokenizer_inherit(lxb_html_tokenizer_t *tkz_to,
                           lxb_html_tokenizer_t *tkz_from)
{
    lxb_status_t status;

    tkz_to->tags = tkz_from->tags;
    tkz_to->attrs = tkz_from->attrs;
    tkz_to->attrs_mraw = tkz_from->attrs_mraw;
    tkz_to->mraw = tkz_from->mraw;

    /* Token and Attributes */
    tkz_to->token = NULL;

    tkz_to->dobj_token = tkz_from->dobj_token;
    tkz_to->dobj_token_attr = tkz_from->dobj_token_attr;

    /* Parse errors */
    tkz_to->parse_errors = lexbor_array_obj_create();
    status = lexbor_array_obj_init(tkz_to->parse_errors, 16,
                                   sizeof(lxb_html_tokenizer_error_t));
    if (status != LXB_STATUS_OK) {
        return status;
    }

    tkz_to->state = lxb_html_tokenizer_state_data_before;
    tkz_to->state_return = NULL;

    tkz_to->callback_token_done = lxb_html_tokenizer_token_done;
    tkz_to->callback_token_ctx = NULL;

    tkz_to->is_eof = false;
    tkz_to->status = LXB_STATUS_OK;

    tkz_to->base = tkz_from;
    tkz_to->ref_count = 1;

    tkz_to->start = tkz_from->start;
    tkz_to->end = tkz_from->end;
    tkz_to->pos = tkz_to->start;
    tkz_to->current_line = tkz_from->current_line;
    tkz_to->current_column = tkz_from->current_column;

    return LXB_STATUS_OK;
}

lxb_html_tokenizer_t *
lxb_html_tokenizer_ref(lxb_html_tokenizer_t *tkz)
{
    if (tkz == NULL) {
        return NULL;
    }

    if (tkz->base != NULL) {
        return lxb_html_tokenizer_ref(tkz->base);
    }

    tkz->ref_count++;

    return tkz;
}

lxb_html_tokenizer_t *
lxb_html_tokenizer_unref(lxb_html_tokenizer_t *tkz)
{
    if (tkz == NULL || tkz->ref_count == 0) {
        return NULL;
    }

    if (tkz->base != NULL) {
        tkz->base = lxb_html_tokenizer_unref(tkz->base);
    }

    tkz->ref_count--;

    if (tkz->ref_count == 0) {
        lxb_html_tokenizer_destroy(tkz);
    }

    return NULL;
}

void
lxb_html_tokenizer_clean(lxb_html_tokenizer_t *tkz)
{
    tkz->tree = NULL;

    tkz->state = lxb_html_tokenizer_state_data_before;
    tkz->state_return = NULL;

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;

    tkz->pos = tkz->start;

    lexbor_mraw_clean(tkz->mraw);
    lexbor_dobject_clean(tkz->dobj_token);
    lexbor_dobject_clean(tkz->dobj_token_attr);

    lexbor_array_obj_clean(tkz->parse_errors);
}

lxb_html_tokenizer_t *
lxb_html_tokenizer_destroy(lxb_html_tokenizer_t *tkz)
{
    if (tkz == NULL) {
        return NULL;
    }

    if (tkz->base == NULL) {
        if (tkz->opt & LXB_HTML_TOKENIZER_OPT_TAGS_SELF) {
            lxb_html_tokenizer_tags_destroy(tkz);
        }

        if (tkz->opt & LXB_HTML_TOKENIZER_OPT_ATTRS_SELF) {
            lxb_html_tokenizer_attrs_destroy(tkz);
        }

        lexbor_mraw_destroy(tkz->mraw, true);
        lexbor_dobject_destroy(tkz->dobj_token, true);
        lexbor_dobject_destroy(tkz->dobj_token_attr, true);
        lexbor_free(tkz->start);
    }

    tkz->parse_errors = lexbor_array_obj_destroy(tkz->parse_errors, true);

    return lexbor_free(tkz);
}

lxb_status_t
lxb_html_tokenizer_tags_make(lxb_html_tokenizer_t *tkz, size_t table_size)
{
    tkz->tags = lexbor_hash_create();
    return lexbor_hash_init(tkz->tags, table_size, sizeof(lxb_tag_data_t));
}

void
lxb_html_tokenizer_tags_destroy(lxb_html_tokenizer_t *tkz)
{
    tkz->tags = lexbor_hash_destroy(tkz->tags, true);
}

lxb_status_t
lxb_html_tokenizer_attrs_make(lxb_html_tokenizer_t *tkz, size_t table_size)
{
    tkz->attrs = lexbor_hash_create();
    return lexbor_hash_init(tkz->attrs, table_size,
                            sizeof(lxb_dom_attr_data_t));
}

void
lxb_html_tokenizer_attrs_destroy(lxb_html_tokenizer_t *tkz)
{
    tkz->attrs = lexbor_hash_destroy(tkz->attrs, true);
}

lxb_status_t
lxb_html_tokenizer_begin(lxb_html_tokenizer_t *tkz)
{
    if (tkz->tags == NULL) {
        tkz->status = lxb_html_tokenizer_tags_make(tkz, 256);
        if (tkz->status != LXB_STATUS_OK) {
            return tkz->status;
        }

        tkz->opt |= LXB_HTML_TOKENIZER_OPT_TAGS_SELF;
    }

    if (tkz->attrs == NULL) {
        tkz->status = lxb_html_tokenizer_attrs_make(tkz, 256);
        if (tkz->status != LXB_STATUS_OK) {
            return tkz->status;
        }

        tkz->opt |= LXB_HTML_TOKENIZER_OPT_ATTRS_SELF;
    }

    if (tkz->attrs_mraw == NULL) {
        tkz->attrs_mraw = tkz->mraw;

        tkz->opt |= LXB_HTML_TOKENIZER_OPT_ATTRS_MRAW_SELF;
    }

    tkz->token = lxb_html_token_create(tkz->dobj_token);
    if (tkz->token == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_html_tokenizer_chunk(lxb_html_tokenizer_t *tkz, const lxb_char_t *data,
                         size_t size)
{
    const lxb_char_t *end = data + size;

    tkz->is_eof = false;
    tkz->status = LXB_STATUS_OK;
    tkz->last = end;

    while (data < end) {
        size_t current_column = tkz->current_column;
        const lxb_char_t *new_data = tkz->state(tkz, data, end);
        while (data < new_data) {
            /* Codepoints < 0x80 are encoded the same as their ASCII counterpart, so '\n' will uniquely identify a newline. */
            if (*data == '\n') {
                tkz->current_line++;
                current_column = 0;
            } else {
                /* Other characters can be mapped back to the unicode codepoint offset because UTF-8 is a prefix code.
                 * Continuation bytes start with 0b10XXXXXX so we can skip those to only get the start of an encoded code point. */
                if ((*data & 0b11000000) == 0b10000000) {
                    /* Continuation byte, do nothing */
                } else {
                    /* First byte for a codepoint */
                    current_column++;
                }
            }
            data++;
        }
        tkz->current_column = current_column;
    }

    return tkz->status;
}

lxb_status_t
lxb_html_tokenizer_end(lxb_html_tokenizer_t *tkz)
{
    const lxb_char_t *data, *end;

    tkz->status = LXB_STATUS_OK;

    /* Send a fake EOF data. */
    data = lxb_html_tokenizer_eof;
    end = lxb_html_tokenizer_eof + 1UL;

    tkz->is_eof = true;

    while (tkz->state(tkz, data, end) < end) {
        /* empty loop */
    }

    tkz->is_eof = false;

    if (tkz->status != LXB_STATUS_OK) {
        return tkz->status;
    }

    /* Emit fake token: END OF FILE */
    lxb_html_token_clean(tkz->token);

    tkz->token->tag_id = LXB_TAG__END_OF_FILE;

    tkz->token = tkz->callback_token_done(tkz, tkz->token,
                                          tkz->callback_token_ctx);

    if (tkz->token == NULL && tkz->status == LXB_STATUS_OK) {
        tkz->status = LXB_STATUS_ERROR;
    }

    return tkz->status;
}

static lxb_html_token_t *
lxb_html_tokenizer_token_done(lxb_html_tokenizer_t *tkz,
                              lxb_html_token_t *token, void *ctx)
{
    return token;
}

lxb_ns_id_t
lxb_html_tokenizer_current_namespace(lxb_html_tokenizer_t *tkz)
{
    if (tkz->tree == NULL) {
        return LXB_NS__UNDEF;
    }

    lxb_dom_node_t *node = lxb_html_tree_adjusted_current_node(tkz->tree);

    if (node == NULL) {
        return LXB_NS__UNDEF;
    }

    return node->ns;
}

void
lxb_html_tokenizer_set_state_by_tag(lxb_html_tokenizer_t *tkz, bool scripting,
                                    lxb_tag_id_t tag_id, lxb_ns_id_t ns)
{
    if (ns != LXB_NS_HTML) {
        tkz->state = lxb_html_tokenizer_state_data_before;

        return;
    }

    switch (tag_id) {
        case LXB_TAG_TITLE:
        case LXB_TAG_TEXTAREA:
            tkz->tmp_tag_id = tag_id;
            tkz->state = lxb_html_tokenizer_state_rcdata_before;

            break;

        case LXB_TAG_STYLE:
        case LXB_TAG_XMP:
        case LXB_TAG_IFRAME:
        case LXB_TAG_NOEMBED:
        case LXB_TAG_NOFRAMES:
            tkz->tmp_tag_id = tag_id;
            tkz->state = lxb_html_tokenizer_state_rawtext_before;

            break;

        case LXB_TAG_SCRIPT:
            tkz->tmp_tag_id = tag_id;
            tkz->state = lxb_html_tokenizer_state_script_data_before;

            break;

        case LXB_TAG_NOSCRIPT:
            if (scripting) {
                tkz->tmp_tag_id = tag_id;
                tkz->state = lxb_html_tokenizer_state_rawtext_before;

                return;
            }

            tkz->state = lxb_html_tokenizer_state_data_before;

            break;

        case LXB_TAG_PLAINTEXT:
            tkz->state = lxb_html_tokenizer_state_plaintext_before;

            break;

        default:
            break;
    }
}

/*
 * No inline functions for ABI.
 */
void
lxb_html_tokenizer_status_set_noi(lxb_html_tokenizer_t *tkz,
                                  lxb_status_t status)
{
    lxb_html_tokenizer_status_set(tkz, status);
}

void
lxb_html_tokenizer_callback_token_done_set_noi(lxb_html_tokenizer_t *tkz,
                                               lxb_html_tokenizer_token_f call_func,
                                               void *ctx)
{
    lxb_html_tokenizer_callback_token_done_set(tkz, call_func, ctx);
}

void *
lxb_html_tokenizer_callback_token_done_ctx_noi(lxb_html_tokenizer_t *tkz)
{
    return lxb_html_tokenizer_callback_token_done_ctx(tkz);
}

void
lxb_html_tokenizer_state_set_noi(lxb_html_tokenizer_t *tkz,
                                 lxb_html_tokenizer_state_f state)
{
    lxb_html_tokenizer_state_set(tkz, state);
}

void
lxb_html_tokenizer_tmp_tag_id_set_noi(lxb_html_tokenizer_t *tkz,
                                      lxb_tag_id_t tag_id)
{
    lxb_html_tokenizer_tmp_tag_id_set(tkz, tag_id);
}

lxb_html_tree_t *
lxb_html_tokenizer_tree_noi(lxb_html_tokenizer_t *tkz)
{
    return lxb_html_tokenizer_tree(tkz);
}

void
lxb_html_tokenizer_tree_set_noi(lxb_html_tokenizer_t *tkz,
                                lxb_html_tree_t *tree)
{
    lxb_html_tokenizer_tree_set(tkz, tree);
}

lexbor_mraw_t *
lxb_html_tokenizer_mraw_noi(lxb_html_tokenizer_t *tkz)
{
    return lxb_html_tokenizer_mraw(tkz);
}

lexbor_hash_t *
lxb_html_tokenizer_tags_noi(lxb_html_tokenizer_t *tkz)
{
    return lxb_html_tokenizer_tags(tkz);
}
