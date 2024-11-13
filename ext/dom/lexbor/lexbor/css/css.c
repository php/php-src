/*
 * Copyright (C) 2021-2022 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/css/css.h"


typedef struct {
    lexbor_str_t  *str;
    lexbor_mraw_t *mraw;
}
lxb_css_str_ctx_t;


static lxb_status_t
lxb_css_str_cb(const lxb_char_t *data, size_t len, void *cb_ctx);


lxb_css_memory_t *
lxb_css_memory_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_css_memory_t));
}

lxb_status_t
lxb_css_memory_init(lxb_css_memory_t *memory, size_t prepare_count)
{
    lxb_status_t status;

    static const size_t size_mem = lexbor_max(sizeof(lxb_css_selector_t),
                                              sizeof(lxb_css_selector_list_t));

    if (memory == NULL) {
        return LXB_STATUS_ERROR_INCOMPLETE_OBJECT;
    }

    if (prepare_count < 64) {
        prepare_count = 64;
    }

    if (memory->objs == NULL) {
        memory->objs = lexbor_dobject_create();
        status = lexbor_dobject_init(memory->objs, prepare_count, size_mem);
        if (status != LXB_STATUS_OK) {
            goto failed;
        }
    }

    if (memory->tree == NULL) {
        prepare_count = prepare_count * 96;

        memory->tree = lexbor_mraw_create();
        status = lexbor_mraw_init(memory->tree, prepare_count);
        if (status != LXB_STATUS_OK) {
            goto failed;
        }
    }

    if (memory->mraw == NULL) {
        memory->mraw = lexbor_mraw_create();
        status = lexbor_mraw_init(memory->mraw, 4096);
        if (status != LXB_STATUS_OK) {
            goto failed;
        }
    }

    memory->ref_count = 1;

    return LXB_STATUS_OK;

failed:

    (void) lxb_css_memory_destroy(memory, false);

    return status;
}

void
lxb_css_memory_clean(lxb_css_memory_t *memory)
{
    if (memory->objs != NULL) {
        lexbor_dobject_clean(memory->objs);
    }

    if (memory->mraw != NULL) {
        lexbor_mraw_clean(memory->mraw);
    }

    if (memory->tree != NULL) {
        lexbor_mraw_clean(memory->tree);
    }
}

lxb_css_memory_t *
lxb_css_memory_destroy(lxb_css_memory_t *memory, bool self_destroy)
{
    if (memory == NULL) {
        return NULL;
    }

    if (memory->objs != NULL) {
        memory->objs = lexbor_dobject_destroy(memory->objs, true);
    }

    if (memory->mraw != NULL) {
        memory->mraw = lexbor_mraw_destroy(memory->mraw, true);
    }

    if (memory->tree != NULL) {
        memory->tree = lexbor_mraw_destroy(memory->tree, true);
    }

    if (self_destroy) {
        return lexbor_free(memory);
    }

    return memory;
}

lxb_css_memory_t *
lxb_css_memory_ref_inc(lxb_css_memory_t *memory)
{
    if (SIZE_MAX - memory->ref_count == 0) {
        return NULL;
    }

    memory->ref_count++;

    return memory;
}

void
lxb_css_memory_ref_dec(lxb_css_memory_t *memory)
{
    if (memory->ref_count > 0) {
        memory->ref_count--;
    }
}

lxb_css_memory_t *
lxb_css_memory_ref_dec_destroy(lxb_css_memory_t *memory)
{
    if (memory->ref_count > 0) {
        memory->ref_count--;
    }

    if (memory->ref_count == 0) {
        return lxb_css_memory_destroy(memory, true);
    }

    return memory;
}

lxb_status_t
lxb_css_make_data(lxb_css_parser_t *parser, lexbor_str_t *str,
                  uintptr_t begin, uintptr_t end)
{
    size_t length, offlen, len;
    const lxb_char_t *pos;
    const lexbor_str_t *tmp;

    tmp = &parser->str;

    offlen = begin - parser->offset;
    length = end - begin;

    if (str->data == NULL) {
        (void) lexbor_str_init(str, parser->memory->mraw, length);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    if (tmp->length > offlen) {
        len = tmp->length - offlen;

        if (len >= length) {
            memcpy(str->data + str->length, tmp->data + offlen, length);
            goto done;
        }
        else {
            memcpy(str->data + str->length, tmp->data + offlen, len);
        }

        str->length += len;

        pos = parser->pos;
        length -= len;
    }
    else {
        pos = parser->pos + (offlen - tmp->length);
    }

    memcpy(str->data + str->length, pos, length);

done:

    str->length += length;
    str->data[str->length] = '\0';

    return LXB_STATUS_OK;
}

lxb_char_t *
lxb_css_serialize_char_handler(const void *style, lxb_css_style_serialize_f cb,
                               size_t *out_length)
{
    size_t length = 0;
    lxb_status_t status;
    lexbor_str_t str;

    status = cb(style, lexbor_serialize_length_cb, &length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    /* + 1 == '\0' */
    str.data = lexbor_malloc(length + 1);
    if (str.data == NULL) {
        goto failed;
    }

    str.length = 0;

    status = cb(style, lexbor_serialize_copy_cb, &str);
    if (status != LXB_STATUS_OK) {
        lexbor_free(str.data);
        goto failed;
    }

    str.data[str.length] = '\0';

    if (out_length != NULL) {
        *out_length = str.length;
    }

    return str.data;

failed:

    if (out_length != NULL) {
        *out_length = 0;
    }

    return NULL;
}

lxb_status_t
lxb_css_serialize_str_handler(const void *style, lexbor_str_t *str,
                              lexbor_mraw_t *mraw,
                              lxb_css_style_serialize_f cb)
{
    lxb_css_str_ctx_t ctx;

    ctx.str = str;
    ctx.mraw = mraw;

    if (str->data == NULL) {
        lexbor_str_init(str, mraw, 1);
        if (str->data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    return cb(style, lxb_css_str_cb, &ctx);
}

static lxb_status_t
lxb_css_str_cb(const lxb_char_t *data, size_t len, void *cb_ctx)
{
    lxb_char_t *ptr;
    lxb_css_str_ctx_t *ctx = (lxb_css_str_ctx_t *) cb_ctx;

    ptr = lexbor_str_append(ctx->str, ctx->mraw, data, len);

    return (ptr != NULL) ? LXB_STATUS_OK : LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}
