/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/unicode/idna.h"
#include "lexbor/unicode/unicode.h"
#include "lexbor/punycode/punycode.h"
#include "lexbor/encoding/encoding.h"


typedef struct {
    lxb_unicode_idna_cb_f   cb;
    void                    *context;
    lxb_unicode_idna_flag_t flags;
}
lxb_unicode_idna_ctx_t;

typedef struct {
    lxb_char_t              buffer[4096];
    lxb_char_t              *p;
    lxb_char_t              *buf;
    const lxb_char_t        *end;
    lxb_unicode_idna_flag_t flags;
}
lxb_unicode_idna_ascii_ctx_t;


static lxb_status_t
lxb_unicode_idna_processing_body(lxb_unicode_idna_t *idna, const void *data,
                                 size_t len, lxb_unicode_idna_cb_f cb, void *ctx,
                                 lxb_unicode_idna_flag_t flags, bool is_cp);

static lxb_status_t
lxb_unicode_idna_norm_c_cb(const lxb_codepoint_t *cps, size_t len, void *ctx);

static lxb_status_t
lxb_unicode_idna_norm_c_send(const lxb_codepoint_t *cps,
                             const lxb_codepoint_t *p,
                             lxb_unicode_idna_ctx_t *context);

static lxb_status_t
lxb_unicode_idna_punycode_cb(const lxb_codepoint_t *cps, size_t len, void *ctx);

static lxb_status_t
lxb_unicode_idna_to_ascii_cb(const lxb_codepoint_t *part, size_t len,
                             void *ctx, lxb_status_t status);

static lxb_status_t
lxb_unicode_idna_to_ascii_body(lxb_unicode_idna_t *idna, const void *data,
                               size_t length, lexbor_serialize_cb_f cb, void *ctx,
                               lxb_unicode_idna_flag_t flags, bool is_cp);

static lxb_status_t
lxb_unicode_idna_ascii_puny_cb(const lxb_char_t *data, size_t length, void *ctx,
                               bool unchanged);

static lxb_status_t
lxb_unicode_idna_to_unicode_cb(const lxb_codepoint_t *part, size_t len,
                               void *ctx, lxb_status_t status);

static lxb_status_t
lxb_unicode_idna_to_unicode_body(lxb_unicode_idna_t *idna, const void *data,
                                 size_t length, lexbor_serialize_cb_f cb,
                                 void *ctx, lxb_unicode_idna_flag_t flags,
                                 bool is_cp);

static bool
lxb_unicode_idna_validity_criteria_h(const void *data, size_t length,
                                     lxb_unicode_idna_flag_t flags, bool is_cp);

lxb_unicode_idna_t *
lxb_unicode_idna_create(void)
{
    return lexbor_malloc(sizeof(lxb_unicode_idna_t));
}

lxb_status_t
lxb_unicode_idna_init(lxb_unicode_idna_t *idna)
{
    if (idna == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    return lxb_unicode_normalizer_init(&idna->normalizer, LXB_UNICODE_NFC);
}

void
lxb_unicode_idna_clean(lxb_unicode_idna_t *idna)
{
    lxb_unicode_normalizer_clean(&idna->normalizer);
}

lxb_unicode_idna_t *
lxb_unicode_idna_destroy(lxb_unicode_idna_t *idna, bool self_destroy)
{
    if (idna == NULL) {
        return NULL;
    }

    (void) lxb_unicode_normalizer_destroy(&idna->normalizer, false);

    if (self_destroy) {
        return lexbor_free(idna);
    }

    return idna;
}

lxb_codepoint_t *
lxb_unicode_idna_realloc(lxb_codepoint_t *buf, const lxb_codepoint_t *buffer,
                         lxb_codepoint_t **buf_p, lxb_codepoint_t **buf_end,
                         size_t len)
{
    size_t nlen;
    lxb_codepoint_t *tmp;

    nlen = ((*buf_end - buf) * 4) + len;
 
    if (buf == buffer) {
        tmp = lexbor_malloc(nlen * sizeof(lxb_codepoint_t));
        if (tmp == NULL) {
            return NULL;
        }
    }
    else {
        tmp = lexbor_realloc(buf, nlen * sizeof(lxb_codepoint_t));
        if (tmp == NULL) {
            return lexbor_free(buf);
        }
    }

    *buf_p = tmp + (*buf_p - buf);
    *buf_end = tmp + nlen;

    return tmp;
}

lxb_status_t
lxb_unicode_idna_processing(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                            size_t length, lxb_unicode_idna_cb_f cb, void *ctx,
                            lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_processing_body(idna, data, length, cb, ctx,
                                            flags, false);
}

lxb_status_t
lxb_unicode_idna_processing_cp(lxb_unicode_idna_t *idna,
                               const lxb_codepoint_t *cps, size_t length,
                               lxb_unicode_idna_cb_f cb, void *ctx,
                               lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_processing_body(idna, cps, length, cb, ctx,
                                            flags, true);
}

static lxb_status_t
lxb_unicode_idna_processing_body(lxb_unicode_idna_t *idna, const void *data,
                                 size_t len, lxb_unicode_idna_cb_f cb, void *ctx,
                                 lxb_unicode_idna_flag_t flags, bool is_cp)
{
    bool need;
    size_t i, length;
    lxb_status_t status;
    lxb_codepoint_t cp, *buf, *buf_p, *buf_end;
    const lxb_char_t *end, *p;
    lxb_unicode_idna_type_t type;
    const lxb_unicode_idna_entry_t *udata;
    const lxb_codepoint_t *maps;
    lxb_unicode_idna_ctx_t context;
    lxb_codepoint_t buffer[4096];

    buf = buffer;
    buf_p = buffer;
    buf_end = buffer + (sizeof(buffer) / sizeof(lxb_codepoint_t));

    p = data;
    len *= (is_cp) ? sizeof(lxb_codepoint_t) : 1;
    end = (const lxb_char_t *) data + len;

    while (p < end) {
        if (is_cp) {
            cp = *((const lxb_codepoint_t *) p);
            p = (const lxb_char_t *) ((const lxb_codepoint_t *) p + 1);
        }
        else {
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);
            if (cp > LXB_ENCODING_DECODE_MAX_CODEPOINT) {
                status = LXB_STATUS_ERROR_UNEXPECTED_DATA;
                goto done;
            }
        }

        type = lxb_unicode_idna_type(cp);

    again:

        switch (type) {
            case LXB_UNICODE_IDNA_IGNORED:
                break;

            case LXB_UNICODE_IDNA_MAPPED:
                udata = lxb_unicode_idna_entry_by_cp(cp);
                maps = lxb_unicode_idna_map(udata, &length);

                if (buf_p + length > buf_end) {
                    buf = lxb_unicode_idna_realloc(buf, buffer, &buf_p,
                                                   &buf_end, length);
                    if (buf == NULL) {
                        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                    }
                }

                for (i = 0; i < length; i++) {
                    *buf_p++ = maps[i];
                }

                break;

            case LXB_UNICODE_IDNA_DEVIATION:
                if ((flags & LXB_UNICODE_IDNA_FLAG_TRANSITIONAL_PROCESSING)) {
                    type = LXB_UNICODE_IDNA_MAPPED;
                    goto again;
                }

                /* Fall through. */

            case LXB_UNICODE_IDNA_DISALLOWED:
                /* Fall through. */

            case LXB_UNICODE_IDNA_VALID:
            default:
                if (buf_p >= buf_end) {
                    buf = lxb_unicode_idna_realloc(buf, buffer, &buf_p,
                                                   &buf_end, 1);
                    if (buf == NULL) {
                        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                    }
                }

                *buf_p++ = cp;
                break;
        }
    }

    context.cb = cb;
    context.context = ctx;
    context.flags = flags;


    need = lxb_unicode_quick_check_cp(&idna->normalizer, buf, buf_p - buf,
                                      true);
    if (need) {
        lxb_unicode_flush_count_set(&idna->normalizer, UINT32_MAX);

        status = lxb_unicode_normalize_cp(&idna->normalizer, buf, buf_p - buf,
                                          lxb_unicode_idna_norm_c_cb,
                                          &context, true);
    }
    else {
        status = lxb_unicode_idna_norm_c_cb(buf, buf_p - buf, &context);
    }

done:

    if (buf != buffer) {
        (void) lexbor_free(buf);
    }

    return status;
}

static lxb_status_t
lxb_unicode_idna_norm_c_cb(const lxb_codepoint_t *cps, size_t len, void *ctx)
{
    lxb_status_t status;
    lxb_unicode_idna_ctx_t *context = ctx;
    const lxb_codepoint_t *p, *end;

    p = cps;
    end = cps + len;

    while (p < end) {
        /* U+002E ( . ) FULL STOP. */

        if (*p == 0x002E) {
            status = lxb_unicode_idna_norm_c_send(cps, p, context);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            cps = p + 1;
        }

        p += 1;
    }

    /*
     * We need to call a zero-length callback if the last codepoint was a
     * U+002E ( . ) FULL STOP.
     *
     * For example, "muuuu." will call for two callbacks.
     * First: "muuuu".
     * Second: "" -- empty string with length = 0.
     */

    if (p > cps || (len >= 1 && p[-1] == '.')) {
        return lxb_unicode_idna_norm_c_send(cps, p, context);
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_unicode_idna_norm_c_send(const lxb_codepoint_t *cps,
                             const lxb_codepoint_t *p,
                             lxb_unicode_idna_ctx_t *context)
{
    bool cr;
    lxb_status_t status;

    /* xn-- or Xn-- or xN-- or XN-- */

    if (p - cps >= 4
        && (cps[0] == 0x0078 || cps[0] == 0x0058)
        && (cps[1] == 0x006E || cps[1] == 0x004E)
        && cps[2] == 0x002D && cps[3] == 0x002D)
    {
        cps += 4;
        status = lxb_punycode_decode_cp(cps, p - cps,
                                        lxb_unicode_idna_punycode_cb,
                                        context);
        if (status == LXB_STATUS_OK) {
            return LXB_STATUS_OK;
        }

        cps -= 4;
    }
    else {
        status = LXB_STATUS_OK;
    }

    cr = lxb_unicode_idna_validity_criteria_cp(cps, p - cps, context->flags);
    if (!cr) {
        return LXB_STATUS_ERROR_UNEXPECTED_RESULT;
    }

    return context->cb(cps, p - cps, context->context, status);
}

static lxb_status_t
lxb_unicode_idna_punycode_cb(const lxb_codepoint_t *cps, size_t len, void *ctx)
{
    bool cr;
    lxb_unicode_idna_ctx_t *context = ctx;
    lxb_unicode_idna_ascii_ctx_t *asc = context->context;

    cr = lxb_unicode_idna_validity_criteria_cp(cps, len, asc->flags);
    if (!cr) {
        return LXB_STATUS_ERROR_UNEXPECTED_RESULT;
    }

    return context->cb(cps, len, context->context, LXB_STATUS_OK);
}

lxb_status_t
lxb_unicode_idna_to_ascii(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                          size_t length, lexbor_serialize_cb_f cb, void *ctx,
                          lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_to_ascii_body(idna, data, length, cb, ctx,
                                          flags, false);
}

lxb_status_t
lxb_unicode_idna_to_ascii_cp(lxb_unicode_idna_t *idna, const lxb_codepoint_t *cps,
                             size_t length, lexbor_serialize_cb_f cb, void *ctx,
                             lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_to_ascii_body(idna, cps, length, cb, ctx,
                                          flags, true);
}

static lxb_status_t
lxb_unicode_idna_to_ascii_body(lxb_unicode_idna_t *idna, const void *data,
                               size_t length, lexbor_serialize_cb_f cb, void *ctx,
                               lxb_unicode_idna_flag_t flags, bool is_cp)
{
    size_t len;
    lxb_status_t status;
    lxb_unicode_idna_ascii_ctx_t context;

    context.p = context.buffer;
    context.buf = context.buffer;
    context.end = context.buf + sizeof(context.buffer);
    context.flags = flags;

    if (!is_cp) {
        status = lxb_unicode_idna_processing(idna, data, length,
                                             lxb_unicode_idna_to_ascii_cb,
                                             &context, flags);
    }
    else {
        status = lxb_unicode_idna_processing_cp(idna, data, length,
                                                lxb_unicode_idna_to_ascii_cb,
                                                &context, flags);
    }

    if (status != LXB_STATUS_OK) {
        goto done;
    }

    /* Remove last U+002E ( . ) FULL STOP. */

    if (context.p > context.buf) {
        context.p -= 1;
    }

    len = context.p - context.buf;

    status = cb(context.buf, len, ctx);

done:

    if (context.buf != context.buffer) {
        (void) lexbor_free(context.buf);
    }

    return status;
}

static lxb_status_t
lxb_unicode_idna_to_ascii_cb(const lxb_codepoint_t *part, size_t len,
                             void *ctx, lxb_status_t status)
{
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return lxb_punycode_encode_cp(part, len, lxb_unicode_idna_ascii_puny_cb,
                                  ctx);
}

static lxb_status_t
lxb_unicode_idna_ascii_puny_cb(const lxb_char_t *data, size_t length, void *ctx,
                               bool unchanged)
{
    size_t nlen;
    lxb_char_t *tmp;
    lxb_unicode_idna_ascii_ctx_t *asc = ctx;

    static const lexbor_str_t prefix = lexbor_str("xn--");

    if (asc->p + length + 6 > asc->end) {
        nlen = ((asc->end - asc->buf) * 4) + length + 6;

        if (asc->buf == asc->buffer) {
            tmp = lexbor_malloc(nlen);
        }
        else {
            tmp = lexbor_realloc(asc->buf, nlen);
        }

        if (tmp == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        asc->p = tmp + (asc->p - asc->buf);
        asc->buf = tmp;
        asc->end = tmp + nlen;
    }

    if (!unchanged) {
        memcpy(asc->p, prefix.data, prefix.length);
        asc->p += 4;
    }

    memcpy(asc->p, data, length);

    asc->p += length;
    *asc->p++ = '.';
    *asc->p = 0x00;

    return LXB_STATUS_OK;
}

bool
lxb_unicode_idna_validity_criteria(const lxb_char_t *data, size_t length,
                                   lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_validity_criteria_h(data, length, flags, false);
}

bool
lxb_unicode_idna_validity_criteria_cp(const lxb_codepoint_t *data, size_t length,
                                      lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_validity_criteria_h(data, length, flags, true);
}

static bool
lxb_unicode_idna_validity_criteria_h(const void *data, size_t length,
                                     lxb_unicode_idna_flag_t flags, bool is_cp)
{
    size_t len;
    lxb_codepoint_t cp;
    const lxb_codepoint_t *cps;
    const lxb_char_t *p, *end;
    lxb_unicode_idna_type_t type;

    p = data;
    len = length * ((is_cp) ? sizeof(lxb_codepoint_t) : 1);
    end = (const lxb_char_t *) data + len;

    if (flags & LXB_UNICODE_IDNA_FLAG_CHECK_HYPHENS) {
        /* U+002D HYPHEN-MINUS */

        if (is_cp) {
            cps = data;

            if (length > 4) {
                if (cps[3] == 0x002D || cps[4] == 0x002D) {
                    return false;
                }
            }

            if (length >= 1) {
                if (cps[0] == 0x002D || cps[length - 1] == 0x002D) {
                    return false;
                }
            }
        }
        else {
            if (length > 4) {
                if (p[3] == 0x002D || p[4] == 0x002D) {
                    return false;
                }
            }

            if (length >= 1) {
                if (p[0] == 0x002D || p[-1] == 0x002D) {
                    return false;
                }
            }
        }
    }
    else if (length >= 4) {
        if (is_cp) {
            cps = data;

            if (   (cps[0] == 0x0078 || cps[0] == 0x0058)
                && (cps[1] == 0x006E || cps[1] == 0x004E)
                &&  cps[2] == 0x002D && cps[3] == 0x002D)
            {
                return false;
            }
        }
        else {
            if (   (p[0] == 0x0078 || p[0] == 0x0058)
                && (p[1] == 0x006E || p[1] == 0x004E)
                &&  p[2] == 0x002D && p[3] == 0x002D)
            {
                return false;
            }
        }
    }

    while (p < end) {
        if (!is_cp) {
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);
            if (cp == LXB_ENCODING_DECODE_ERROR) {
                return false;
            }
        }
        else {
            cp = *((const lxb_codepoint_t *) p);
            p = (const lxb_char_t *) ((const lxb_codepoint_t *) p + 1);
        }

        /* U+002E ( . ) FULL STOP */

        if (cp == 0x002E) {
            return false;
        }

        type = lxb_unicode_idna_type(cp);

        switch (type) {
            case LXB_UNICODE_IDNA_VALID:
                break;

            case LXB_UNICODE_IDNA_DEVIATION:
                if (!(flags & LXB_UNICODE_IDNA_FLAG_TRANSITIONAL_PROCESSING)) {
                    break;
                }

                /* Fall through. */

            case LXB_UNICODE_IDNA_DISALLOWED:
            case LXB_UNICODE_IDNA_IGNORED:
            case LXB_UNICODE_IDNA_MAPPED:
            default:
                return false;
        }
    }

    return true;
}

lxb_status_t
lxb_unicode_idna_to_unicode(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                            size_t length, lexbor_serialize_cb_f cb,
                            void *ctx, lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_to_unicode_body(idna, data, length, cb, ctx,
                                            flags, false);
}

lxb_status_t
lxb_unicode_idna_to_unicode_cp(lxb_unicode_idna_t *idna,
                               const lxb_codepoint_t *cps,
                               size_t length, lexbor_serialize_cb_f cb,
                               void *ctx, lxb_unicode_idna_flag_t flags)
{
    return lxb_unicode_idna_to_unicode_body(idna, cps, length, cb, ctx,
                                            flags, true);
}

static lxb_status_t
lxb_unicode_idna_to_unicode_body(lxb_unicode_idna_t *idna, const void *data,
                                 size_t length, lexbor_serialize_cb_f cb,
                                 void *ctx, lxb_unicode_idna_flag_t flags,
                                 bool is_cp)
{
    size_t len;
    lxb_status_t status;
    lxb_unicode_idna_ascii_ctx_t context;

    context.p = context.buffer;
    context.buf = context.buffer;
    context.end = context.buf + sizeof(context.buffer);
    context.flags = flags;

    if (!is_cp) {
        status = lxb_unicode_idna_processing(idna, data, length,
                                             lxb_unicode_idna_to_unicode_cb,
                                             &context, flags);
    }
    else {
        status = lxb_unicode_idna_processing_cp(idna, data, length,
                                                lxb_unicode_idna_to_unicode_cb,
                                                &context, flags);
    }

    if (status != LXB_STATUS_OK) {
        goto done;
    }

    /* Remove last U+002E ( . ) FULL STOP. */

    if (context.p > context.buf) {
        context.p -= 1;
    }

    len = context.p - context.buf;

    status = cb(context.buf, len, ctx);

done:

    if (context.buf != context.buffer) {
        (void) lexbor_free(context.buf);
    }

    return status;
}


static lxb_status_t
lxb_unicode_idna_to_unicode_cb(const lxb_codepoint_t *part, size_t len,
                               void *ctx, lxb_status_t status)
{
    int8_t res;
    size_t length, nlen;
    lxb_char_t *tmp;
    const lxb_codepoint_t *p, *end;
    lxb_unicode_idna_ascii_ctx_t *asc = ctx;

    if (status != LXB_STATUS_OK) {
        return status;
    }

    p = part;
    end = part + len;

    length = 0;

    while (p < end) {
        res = lxb_encoding_encode_utf_8_length(*p++);
        if (res == 0) {
            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        length += res;
    }

    if (asc->p + length + 2 > asc->end) {
        nlen = ((asc->end - asc->buf) * 4) + length + 2;

        if (asc->buf == asc->buffer) {
            tmp = lexbor_malloc(nlen);
        }
        else {
            tmp = lexbor_realloc(asc->buf, nlen);
        }

        if (tmp == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        asc->p = tmp + (asc->p - asc->buf);
        asc->buf = tmp;
        asc->end = tmp + nlen;
    }

    p = part;

    while (p < end) {
        (void) lxb_encoding_encode_utf_8_single(NULL, &asc->p, asc->end, *p++);
    }

    *asc->p++ = '.';
    *asc->p = 0x00;

    return LXB_STATUS_OK;
}
