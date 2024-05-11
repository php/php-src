/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/encoding/encode.h"
#include "lexbor/encoding/single.h"
#include "lexbor/encoding/multi.h"
#include "lexbor/encoding/range.h"


#define LXB_ENCODING_ENCODE_APPEND(ctx, cp)                                    \
    do {                                                                       \
        if ((ctx)->buffer_used == (ctx)->buffer_length) {                      \
            return LXB_STATUS_SMALL_BUFFER;                                    \
        }                                                                      \
                                                                               \
        (ctx)->buffer_out[(ctx)->buffer_used++] = (lxb_char_t) cp;             \
    }                                                                          \
    while (0)

#define LXB_ENCODING_ENCODE_APPEND_P(ctx, cp)                                  \
    do {                                                                       \
        if ((ctx)->buffer_used == (ctx)->buffer_length) {                      \
            *cps = p;                                                          \
            return LXB_STATUS_SMALL_BUFFER;                                    \
        }                                                                      \
                                                                               \
        (ctx)->buffer_out[(ctx)->buffer_used++] = (lxb_char_t) cp;             \
    }                                                                          \
    while (0)

#define LXB_ENCODING_ENCODE_ERROR(ctx)                                         \
    do {                                                                       \
        if (ctx->replace_to == NULL) {                                         \
            return LXB_STATUS_ERROR;                                           \
        }                                                                      \
                                                                               \
        if ((ctx->buffer_used + ctx->replace_len) > ctx->buffer_length) {      \
            return LXB_STATUS_SMALL_BUFFER;                                    \
        }                                                                      \
                                                                               \
        memcpy(&ctx->buffer_out[ctx->buffer_used], ctx->replace_to,            \
               ctx->replace_len);                                              \
                                                                               \
        ctx->buffer_used += ctx->replace_len;                                  \
    }                                                                          \
    while (0)

#define LXB_ENCODING_ENCODE_ERROR_P(ctx)                                       \
    do {                                                                       \
        if (ctx->replace_to == NULL) {                                         \
            *cps = p;                                                          \
            return LXB_STATUS_ERROR;                                           \
        }                                                                      \
                                                                               \
        if ((ctx->buffer_used + ctx->replace_len) > ctx->buffer_length) {      \
            *cps = p;                                                          \
            return LXB_STATUS_SMALL_BUFFER;                                    \
        }                                                                      \
                                                                               \
        memcpy(&ctx->buffer_out[ctx->buffer_used], ctx->replace_to,            \
               ctx->replace_len);                                              \
                                                                               \
        ctx->buffer_used += ctx->replace_len;                                  \
    }                                                                          \
    while (0)

#define LXB_ENCODING_ENCODE_SINGLE_BYTE(table, table_size)                     \
    do {                                                                       \
        lxb_codepoint_t cp;                                                    \
        const lxb_codepoint_t *p = *cps;                                       \
        const lexbor_shs_hash_t *hash;                                         \
                                                                               \
        for (; p < end; p++) {                                                 \
            cp = *p;                                                           \
                                                                               \
            if (cp < 0x80) {                                                   \
                LXB_ENCODING_ENCODE_APPEND_P(ctx, cp);                         \
                continue;                                                      \
            }                                                                  \
                                                                               \
            hash = lexbor_shs_hash_get_static(table, table_size, cp);          \
            if (hash == NULL) {                                                \
                LXB_ENCODING_ENCODE_ERROR_P(ctx);                              \
                continue;                                                      \
            }                                                                  \
                                                                               \
            LXB_ENCODING_ENCODE_APPEND_P(ctx, (uintptr_t) hash->value);        \
        }                                                                      \
                                                                               \
        return LXB_STATUS_OK;                                                  \
    }                                                                          \
    while (0)

#define LXB_ENCODING_ENCODE_BYTE_SINGLE(table, table_size)                     \
    const lexbor_shs_hash_t *hash;                                             \
                                                                               \
    if (cp < 0x80) {                                                           \
        *(*data)++ = (lxb_char_t) cp;                                          \
        return 1;                                                              \
    }                                                                          \
                                                                               \
    hash = lexbor_shs_hash_get_static(table, table_size, cp);                  \
    if (hash == NULL) {                                                        \
        return LXB_ENCODING_ENCODE_ERROR;                                      \
    }                                                                          \
                                                                               \
    *(*data)++ = (lxb_char_t) (uintptr_t) hash->value;                         \
    return 1


lxb_status_t
lxb_encoding_encode_default(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                            const lxb_codepoint_t *end)
{
    return lxb_encoding_encode_utf_8(ctx, cps, end);
}

lxb_status_t
lxb_encoding_encode_auto(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                         const lxb_codepoint_t *end)
{
    *cps = end;
    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_encoding_encode_undefined(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                              const lxb_codepoint_t *end)
{
    *cps = end;
    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_encoding_encode_big5(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                         const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_big5,
                                          LXB_ENCODING_MULTI_HASH_BIG5_SIZE, cp);
        if (hash == NULL) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        ctx->buffer_out[ ctx->buffer_used++ ] = ((uint32_t) (uintptr_t) hash->value) / 157 + 0x81;

        if ((((uint32_t) (uintptr_t) hash->value) % 157) < 0x3F) {
            ctx->buffer_out[ ctx->buffer_used++ ] = (((uint32_t) (uintptr_t) hash->value) % 157) + 0x40;
        }
        else {
            ctx->buffer_out[ ctx->buffer_used++ ] = (((uint32_t) (uintptr_t) hash->value) % 157) + 0x62;
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_euc_jp(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                           const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        if (cp == 0x00A5) {
            LXB_ENCODING_ENCODE_APPEND(ctx, 0x5C);
            continue;
        }

        if (cp == 0x203E) {
            LXB_ENCODING_ENCODE_APPEND(ctx, 0x7E);
            continue;
        }

        if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
            if ((ctx->buffer_used + 2) > ctx->buffer_length) {
                return LXB_STATUS_SMALL_BUFFER;
            }

            ctx->buffer_out[ ctx->buffer_used++ ] = 0x8E;
            ctx->buffer_out[ ctx->buffer_used++ ] = cp - 0xFF61 + 0xA1;

            continue;
        }

        if (cp == 0x2212) {
            cp = 0xFF0D;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_jis0208,
                                          LXB_ENCODING_MULTI_HASH_JIS0208_SIZE, cp);
        if (hash == NULL) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value / 94 + 0xA1;
        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value % 94 + 0xA1;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_euc_kr(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                           const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_euc_kr,
                                          LXB_ENCODING_MULTI_HASH_EUC_KR_SIZE, cp);
        if (hash == NULL) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value / 190 + 0x81;
        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value % 190 + 0x41;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_gbk(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                        const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        if (cp == 0xE5E5) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if (cp == 0x20AC) {
            LXB_ENCODING_ENCODE_APPEND(ctx, 0x80);
            continue;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_gb18030,
                                          LXB_ENCODING_MULTI_HASH_GB18030_SIZE, cp);
        if (hash == NULL) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (uintptr_t) hash->value / 190 + 0x81;

        if (((lxb_char_t) (uintptr_t) hash->value % 190) < 0x3F) {
            ctx->buffer_out[ ctx->buffer_used++ ] = ((lxb_char_t) (uintptr_t) hash->value % 190) + 0x40;
        }
        else {
            ctx->buffer_out[ ctx->buffer_used++ ] = ((lxb_char_t) (uintptr_t) hash->value % 190) + 0x41;
        }
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_ibm866(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                           const lxb_codepoint_t *end)
{

    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_ibm866,
                                    LXB_ENCODING_SINGLE_HASH_IBM866_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_2022_jp(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    int8_t size;
    unsigned state;
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    size = 0;
    state = ctx->state;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

    begin:

        switch (ctx->state) {
            case LXB_ENCODING_ENCODE_2022_JP_ASCII:
                if (cp == 0x000E || cp == 0x000F || cp == 0x001B) {
                    goto failed;
                }

                if (cp < 0x80) {
                    LXB_ENCODING_ENCODE_APPEND(ctx, cp);
                    continue;
                }

                if (cp == 0x00A5 || cp == 0x203E) {
                    /*
                     * Do not switch to the ROMAN stage with prepend code point
                     * to stream, add it immediately.
                     */
                    if ((ctx->buffer_used + 4) > ctx->buffer_length) {
                        goto small_buffer;
                    }

                    ctx->state = LXB_ENCODING_ENCODE_2022_JP_ROMAN;

                    if (cp == 0x00A5) {
                        memcpy(&ctx->buffer_out[ctx->buffer_used],
                               "\x1B\x28\x4A\x5C", 4);
                        ctx->buffer_used += 4;

                        continue;
                    }

                    memcpy(&ctx->buffer_out[ctx->buffer_used],
                           "\x1B\x28\x4A\x7E", 4);
                    ctx->buffer_used += 4;

                    continue;
                }

                break;

            case LXB_ENCODING_ENCODE_2022_JP_ROMAN:
                if (cp == 0x000E || cp == 0x000F || cp == 0x001B) {
                    goto failed;
                }

                if (cp < 0x80) {
                    switch (cp) {
                        case 0x005C:
                        case 0x007E:
                            break;

                        case 0x00A5:
                            LXB_ENCODING_ENCODE_APPEND(ctx, 0x5C);
                            continue;

                        case 0x203E:
                            LXB_ENCODING_ENCODE_APPEND(ctx, 0x7E);
                            continue;

                        default:
                            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
                            continue;
                    }

                    /*
                     * Do not switch to the ANSI stage with prepend code point
                     * to stream, add it immediately.
                     */
                    if ((ctx->buffer_used + 4) > ctx->buffer_length) {
                        goto small_buffer;
                    }

                    ctx->state = LXB_ENCODING_ENCODE_2022_JP_ASCII;

                    memcpy(&ctx->buffer_out[ctx->buffer_used], "\x1B\x28\x42", 3);
                    ctx->buffer_used += 3;

                    ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) cp;
                    continue;
                }

                break;

            case LXB_ENCODING_ENCODE_2022_JP_JIS0208:
                if (cp < 0x80) {
                    if ((ctx->buffer_used + 4) > ctx->buffer_length) {
                        goto small_buffer;
                    }

                    ctx->state = LXB_ENCODING_ENCODE_2022_JP_ASCII;

                    memcpy(&ctx->buffer_out[ctx->buffer_used], "\x1B\x28\x42", 3);
                    ctx->buffer_used += 3;

                    ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) cp;
                    continue;
                }

                if (cp == 0x00A5 || cp == 0x203E) {
                    if ((ctx->buffer_used + 4) > ctx->buffer_length) {
                        goto small_buffer;
                    }

                    ctx->state = LXB_ENCODING_ENCODE_2022_JP_ROMAN;

                    if (cp == 0x00A5) {
                        memcpy(&ctx->buffer_out[ctx->buffer_used],
                               "\x1B\x28\x4A\x5C", 4);
                        ctx->buffer_used += 4;

                        continue;
                    }

                    memcpy(&ctx->buffer_out[ctx->buffer_used],
                           "\x1B\x28\x4A\x7E", 4);
                    ctx->buffer_used += 4;

                    continue;
                }

                break;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            goto small_buffer;
        }

        if (cp == 0x2212) {
            cp = 0xFF0D;
        }

        if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
            cp = lxb_encoding_multi_index_iso_2022_jp_katakana[cp - 0xFF61].codepoint;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_jis0208,
                                          LXB_ENCODING_MULTI_HASH_JIS0208_SIZE, cp);
        if (hash == NULL) {
            goto failed;
        }

        if (ctx->state != LXB_ENCODING_ENCODE_2022_JP_JIS0208) {
            if ((ctx->buffer_used + 3) > ctx->buffer_length) {
                goto small_buffer;
            }

            memcpy(&ctx->buffer_out[ctx->buffer_used], "\x1B\x24\x42", 3);
            ctx->buffer_used += 3;

            ctx->state = LXB_ENCODING_ENCODE_2022_JP_JIS0208;
            size += 3;

            goto begin;
        }

        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value / 94 + 0x21;
        ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value % 94 + 0x21;

        continue;

    small_buffer:

        ctx->state = state;
        ctx->buffer_used -= size;

        return LXB_STATUS_SMALL_BUFFER;

    failed:

        ctx->buffer_used -= size;
        LXB_ENCODING_ENCODE_ERROR(ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_iso_2022_jp_eof(lxb_encoding_encode_t *ctx)
{
    if (ctx->state != LXB_ENCODING_ENCODE_2022_JP_ASCII) {
        if ((ctx->buffer_used + 3) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        memcpy(&ctx->buffer_out[ctx->buffer_used], "\x1B\x28\x42", 3);
        ctx->buffer_used += 3;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_iso_8859_10(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_10,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_10_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_13(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_13,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_13_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_14(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_14,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_14_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_15(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_15,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_15_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_16(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_16,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_16_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_2(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_2,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_2_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_3(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_3,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_3_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_4(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_4,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_4_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_5(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_5,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_5_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_6(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_6,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_6_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_7(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_7,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_7_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_8(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                               const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_8,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_8_SIZE);
}

lxb_status_t
lxb_encoding_encode_iso_8859_8_i(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_iso_8859_8,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_8_SIZE);
}

lxb_status_t
lxb_encoding_encode_koi8_r(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                           const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_koi8_r,
                                    LXB_ENCODING_SINGLE_HASH_KOI8_R_SIZE);
}

lxb_status_t
lxb_encoding_encode_koi8_u(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                           const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_koi8_u,
                                    LXB_ENCODING_SINGLE_HASH_KOI8_U_SIZE);
}

lxb_inline const lexbor_shs_hash_t *
lxb_encoding_encode_shift_jis_index(lxb_codepoint_t cp)
{
    const lexbor_shs_hash_t *entry;

    entry = &lxb_encoding_multi_hash_jis0208[ (cp % LXB_ENCODING_MULTI_HASH_JIS0208_SIZE) + 1 ];

    do {
        if (entry->key == cp) {
            if ((unsigned) ((uint32_t) (uintptr_t) entry->value - 8272) > (8835 - 8272)) {
                return entry;
            }
        }

        entry = &lxb_encoding_multi_hash_jis0208[entry->next];
    }
    while (entry != lxb_encoding_multi_hash_jis0208);

    return NULL;
}

lxb_status_t
lxb_encoding_encode_shift_jis(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                              const lxb_codepoint_t *end)
{
    uint32_t lead, trail;
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp <= 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp - 0xFF61 + 0xA1);
            continue;
        }

        switch (cp) {
            case 0x00A5:
                LXB_ENCODING_ENCODE_APPEND(ctx, 0x5C);
                continue;

            case 0x203E:
                LXB_ENCODING_ENCODE_APPEND(ctx, 0x7E);
                continue;

            case 0x2212:
                cp = 0xFF0D;
                break;
        }

        hash = lxb_encoding_encode_shift_jis_index(cp);
        if (hash == NULL) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        if ((ctx->buffer_used + 2) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        lead = (uint32_t) (uintptr_t) hash->value / 188;
        trail = (uint32_t) (uintptr_t) hash->value % 188;

        ctx->buffer_out[ctx->buffer_used++ ] = lead + ((lead < 0x1F) ? 0x81 : 0xC1);
        ctx->buffer_out[ctx->buffer_used++ ] = trail + ((trail < 0x3F) ? 0x40 : 0x41);
    }

    return LXB_STATUS_OK;
}

lxb_inline void
lxb_encoding_encode_utf_16_write(lxb_encoding_encode_t *ctx, bool is_be,
                                 lxb_codepoint_t cp)
{
    if (is_be) {
        ctx->buffer_out[ctx->buffer_used++] = cp >> 8;
        ctx->buffer_out[ctx->buffer_used++] = cp & 0x00FF;

        return;
    }

    ctx->buffer_out[ctx->buffer_used++] = cp & 0x00FF;
    ctx->buffer_out[ctx->buffer_used++] = cp >> 8;
}

lxb_inline int8_t
lxb_encoding_encode_utf_16(lxb_encoding_encode_t *ctx, bool is_be,
                        const lxb_codepoint_t **cps, const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x10000) {
            if ((ctx->buffer_used + 2) > ctx->buffer_length) {
                return LXB_STATUS_SMALL_BUFFER;
            }

            lxb_encoding_encode_utf_16_write(ctx, is_be, cp);

            continue;
        }

        if ((ctx->buffer_used + 4) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        cp -= 0x10000;

        lxb_encoding_encode_utf_16_write(ctx, is_be, (0xD800 | (cp >> 0x0A)));
        lxb_encoding_encode_utf_16_write(ctx, is_be, (0xDC00 | (cp & 0x03FF)));
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_utf_16be(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                             const lxb_codepoint_t *end)
{
    return lxb_encoding_encode_utf_16(ctx, true, cps, end);
}

lxb_status_t
lxb_encoding_encode_utf_16le(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                             const lxb_codepoint_t *end)
{
    return lxb_encoding_encode_utf_16(ctx, false, cps, end);
}

lxb_status_t
lxb_encoding_encode_utf_8(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                          const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;
    const lxb_codepoint_t *p = *cps;

    for (; p < end; p++) {
        cp = *p;

        if (cp < 0x80) {
            if ((ctx->buffer_used + 1) > ctx->buffer_length) {
                *cps = p;

                return LXB_STATUS_SMALL_BUFFER;
            }

            /* 0xxxxxxx */
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) cp;
        }
        else if (cp < 0x800) {
            if ((ctx->buffer_used + 2) > ctx->buffer_length) {
                *cps = p;

                return LXB_STATUS_SMALL_BUFFER;
            }

            /* 110xxxxx 10xxxxxx */
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0xC0 | (cp >> 6  ));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | (cp & 0x3F));
        }
        else if (cp < 0x10000) {
            if ((ctx->buffer_used + 3) > ctx->buffer_length) {
                *cps = p;

                return LXB_STATUS_SMALL_BUFFER;
            }

            /* 1110xxxx 10xxxxxx 10xxxxxx */
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0xE0 | ((cp >> 12)));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | ((cp >> 6 ) & 0x3F));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | ( cp        & 0x3F));
        }
        else if (cp < 0x110000) {
            if ((ctx->buffer_used + 4) > ctx->buffer_length) {
                *cps = p;

                return LXB_STATUS_SMALL_BUFFER;
            }

            /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0xF0 | ( cp >> 18));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | ((cp >> 12) & 0x3F));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | ((cp >> 6 ) & 0x3F));
            ctx->buffer_out[ ctx->buffer_used++ ] = (lxb_char_t) (0x80 | ( cp        & 0x3F));
        }
        else {
            *cps = p;
            LXB_ENCODING_ENCODE_ERROR(ctx);
        }
    }

    *cps = p;

    return LXB_STATUS_OK;
}

lxb_inline uint32_t
lxb_encoding_encode_gb18030_range(lxb_codepoint_t cp)
{
    size_t mid, left, right;
    const lxb_encoding_range_index_t *range;

    if (cp == 0xE7C7) {
        return 7457;
    }

    left = 0;
    right = LXB_ENCODING_RANGE_INDEX_GB18030_SIZE;
    range = lxb_encoding_range_index_gb18030;

    /* Some compilers say about uninitialized mid */
    mid = 0;

    while (left < right) {
        mid = left + (right - left) / 2;

        if (range[mid].codepoint < cp) {
            left = mid + 1;

            if (left < right && range[left].codepoint > cp) {
                break;
            }
        }
        else if (range[mid].codepoint > cp) {
            right = mid - 1;

            if (right > 0 && range[right].codepoint <= cp) {
                mid = right;
                break;
            }
        }
        else {
            break;
        }
    }

    return range[mid].index + cp - range[mid].codepoint;
}

lxb_status_t
lxb_encoding_encode_gb18030(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                            const lxb_codepoint_t *end)
{
    uint32_t index;
    lxb_codepoint_t cp;
    const lexbor_shs_hash_t *hash;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
            continue;
        }

        if (cp == 0xE5E5) {
            LXB_ENCODING_ENCODE_ERROR(ctx);
            continue;
        }

        hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_gb18030,
                                          LXB_ENCODING_MULTI_HASH_GB18030_SIZE, cp);
        if (hash != NULL) {
            if ((ctx->buffer_used + 2) > ctx->buffer_length) {
                return LXB_STATUS_SMALL_BUFFER;
            }

            ctx->buffer_out[ ctx->buffer_used++ ] = (uint32_t) (uintptr_t) hash->value / 190 + 0x81;

            if (((uint32_t) (uintptr_t) hash->value % 190) < 0x3F) {
                ctx->buffer_out[ ctx->buffer_used++ ] = ((uint32_t) (uintptr_t) hash->value % 190) + 0x40;
            }
            else {
                ctx->buffer_out[ ctx->buffer_used++ ] = ((uint32_t) (uintptr_t) hash->value % 190) + 0x41;
            }

            continue;
        }

        if ((ctx->buffer_used + 4) > ctx->buffer_length) {
            return LXB_STATUS_SMALL_BUFFER;
        }

        index = lxb_encoding_encode_gb18030_range(cp);

        ctx->buffer_out[ ctx->buffer_used++ ] = (index / (10 * 126 * 10)) + 0x81;
        ctx->buffer_out[ ctx->buffer_used++ ] = ((index % (10 * 126 * 10)) / (10 * 126)) + 0x30;

        index = (index % (10 * 126 * 10)) % (10 * 126);

        ctx->buffer_out[ ctx->buffer_used++ ] = (index / 10) + 0x81;
        ctx->buffer_out[ ctx->buffer_used++ ] = (index % 10) + 0x30;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_encoding_encode_macintosh(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                              const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_macintosh,
                                    LXB_ENCODING_SINGLE_HASH_MACINTOSH_SIZE);
}

lxb_status_t
lxb_encoding_encode_replacement(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    *cps = end;
    return LXB_STATUS_ERROR;
}

lxb_status_t
lxb_encoding_encode_windows_1250(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1250,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1250_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1251(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1251,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1251_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1252(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1252,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1252_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1253(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1253,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1253_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1254(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1254,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1254_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1255(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1255,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1255_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1256(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1256,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1256_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1257(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1257,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1257_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_1258(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                 const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_1258,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1258_SIZE);
}

lxb_status_t
lxb_encoding_encode_windows_874(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_windows_874,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_874_SIZE);
}

lxb_status_t
lxb_encoding_encode_x_mac_cyrillic(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                   const lxb_codepoint_t *end)
{
    LXB_ENCODING_ENCODE_SINGLE_BYTE(lxb_encoding_single_hash_x_mac_cyrillic,
                                  LXB_ENCODING_SINGLE_HASH_X_MAC_CYRILLIC_SIZE);
}

lxb_status_t
lxb_encoding_encode_x_user_defined(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cps,
                                   const lxb_codepoint_t *end)
{
    lxb_codepoint_t cp;

    for (; *cps < end; (*cps)++) {
        cp = **cps;

        if (cp < 0x80) {
            LXB_ENCODING_ENCODE_APPEND(ctx, cp);
        }
        else if (cp >= 0xF780 && cp <= 0xF7FF) {
            LXB_ENCODING_ENCODE_APPEND(ctx, (cp - 0xF780 + 0x80));
        }
        else {
            LXB_ENCODING_ENCODE_ERROR(ctx);
        }
    }

    return LXB_STATUS_OK;
}

/*
 * Single
 */
int8_t
lxb_encoding_encode_default_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                   const lxb_char_t *end, lxb_codepoint_t cp)
{
    return lxb_encoding_encode_utf_8_single(ctx, data, end, cp);
}

int8_t
lxb_encoding_encode_auto_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                const lxb_char_t *end, lxb_codepoint_t cp)
{
    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_undefined_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                     const lxb_char_t *end, lxb_codepoint_t cp)
{
    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_big5_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                const lxb_char_t *end, lxb_codepoint_t cp)
{
    const lexbor_shs_hash_t *hash;

    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_big5,
                                      LXB_ENCODING_MULTI_HASH_BIG5_SIZE, cp);
    if (hash == NULL) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    if ((*data + 2) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    *(*data)++ = ((uint32_t) (uintptr_t) hash->value) / 157 + 0x81;

    if ((((uint32_t) (uintptr_t) hash->value) % 157) < 0x3F) {
        *(*data)++ = (((uint32_t) (uintptr_t) hash->value) % 157) + 0x40;
    }
    else {
        *(*data)++ = (((uint32_t) (uintptr_t) hash->value) % 157) + 0x62;
    }

    return 2;
}

int8_t
lxb_encoding_encode_euc_jp_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                  const lxb_char_t *end, lxb_codepoint_t cp)
{
    const lexbor_shs_hash_t *hash;

    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if (cp == 0x00A5) {
        *(*data)++ = 0x5C;

        return 1;
    }

    if (cp == 0x203E) {
        *(*data)++ = 0x7E;

        return 1;
    }

    if ((*data + 2) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
        *(*data)++ = 0x8E;
        *(*data)++ = cp - 0xFF61 + 0xA1;

        return 2;
    }

    if (cp == 0x2212) {
        cp = 0xFF0D;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_jis0208,
                                      LXB_ENCODING_MULTI_HASH_JIS0208_SIZE, cp);
    if (hash == NULL) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    *(*data)++ = (uint32_t) (uintptr_t) hash->value / 94 + 0xA1;
    *(*data)++ = (uint32_t) (uintptr_t) hash->value % 94 + 0xA1;

    return 2;
}

int8_t
lxb_encoding_encode_euc_kr_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                  const lxb_char_t *end, lxb_codepoint_t cp)
{
    const lexbor_shs_hash_t *hash;

    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if ((*data + 2) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_euc_kr,
                                      LXB_ENCODING_MULTI_HASH_EUC_KR_SIZE, cp);
    if (hash == NULL) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    *(*data)++ = (uint32_t) (uintptr_t) hash->value / 190 + 0x81;
    *(*data)++ = (uint32_t) (uintptr_t) hash->value % 190 + 0x41;

    return 2;
}

int8_t
lxb_encoding_encode_gbk_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                               const lxb_char_t *end, lxb_codepoint_t cp)
{
    const lexbor_shs_hash_t *hash;

    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if (cp == 0xE5E5) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    if (cp == 0x20AC) {
        *(*data)++ = 0x80;

        return 1;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_gb18030,
                                      LXB_ENCODING_MULTI_HASH_GB18030_SIZE, cp);
    if (hash != NULL) {
        if ((*data + 2) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        *(*data)++ = (lxb_char_t) (uintptr_t) hash->value / 190 + 0x81;

        if (((lxb_char_t) (uintptr_t) hash->value % 190) < 0x3F) {
            *(*data)++ = ((lxb_char_t) (uintptr_t) hash->value % 190) + 0x40;
        }
        else {
            *(*data)++ = ((lxb_char_t) (uintptr_t) hash->value % 190) + 0x41;
        }

        return 2;
    }

    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_ibm866_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                  const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_ibm866,
                                    LXB_ENCODING_SINGLE_HASH_IBM866_SIZE);
}

int8_t
lxb_encoding_encode_iso_2022_jp_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    int8_t size;
    unsigned state;
    const lexbor_shs_hash_t *hash;

    size = 0;
    state = ctx->state;

begin:

    switch (ctx->state) {
        case LXB_ENCODING_ENCODE_2022_JP_ASCII:
            if (cp == 0x000E || cp == 0x000F || cp == 0x001B) {
                goto failed;
            }

            if (cp < 0x80) {
                *(*data)++ = (lxb_char_t) cp;

                return size + 1;
            }

            if (cp == 0x00A5 || cp == 0x203E) {
                /*
                 * Do not switch to the ROMAN stage with prepend code point
                 * to stream, add it immediately.
                 */
                if ((*data + 4) > end) {
                    goto small_buffer;
                }

                ctx->state = LXB_ENCODING_ENCODE_2022_JP_ROMAN;

                if (cp == 0x00A5) {
                    memcpy(*data, "\x1B\x28\x4A\x5C", 4);
                    *data = *data + 4;

                    return size + 4;
                }

                memcpy(*data, "\x1B\x28\x4A\x7E", 4);
                *data = *data + 4;

                return size + 4;
            }

            break;

        case LXB_ENCODING_ENCODE_2022_JP_ROMAN:
            if (cp == 0x000E || cp == 0x000F || cp == 0x001B) {
                goto failed;
            }

            if (cp < 0x80) {
                switch (cp) {
                    case 0x005C:
                    case 0x007E:
                        break;

                    case 0x00A5:
                        *(*data)++ = 0x5C;
                        return size + 1;

                    case 0x203E:
                        *(*data)++ = 0x7E;
                        return size + 1;

                    default:
                        *(*data)++ = (lxb_char_t) cp;
                        return size + 1;
                }

                /*
                 * Do not switch to the ANSI stage with prepend code point
                 * to stream, add it immediately.
                 */
                if ((*data + 4) > end) {
                    goto small_buffer;
                }

                ctx->state = LXB_ENCODING_ENCODE_2022_JP_ASCII;

                memcpy(*data, "\x1B\x28\x42", 3);
                *data = *data + 3;

                *(*data)++ = (lxb_char_t) cp;

                return size + 4;
            }

            break;

        case LXB_ENCODING_ENCODE_2022_JP_JIS0208:
            if (cp < 0x80) {
                if ((*data + 4) > end) {
                    goto small_buffer;
                }

                ctx->state = LXB_ENCODING_ENCODE_2022_JP_ASCII;

                memcpy(*data, "\x1B\x28\x42", 3);
                *data = *data + 3;

                *(*data)++ = (lxb_char_t) cp;

                return size + 4;
            }

            if (cp == 0x00A5 || cp == 0x203E) {
                if ((*data + 4) > end) {
                    goto small_buffer;
                }

                ctx->state = LXB_ENCODING_ENCODE_2022_JP_ROMAN;

                if (cp == 0x00A5) {
                    memcpy(*data, "\x1B\x28\x4A\x5C", 4);
                    *data = *data + 4;

                    return size + 4;
                }

                memcpy(*data, "\x1B\x28\x4A\x7E", 4);
                *data = *data + 4;

                return size + 4;
            }

            break;
    }

    if ((*data + 2) > end) {
        goto small_buffer;
    }

    if (cp == 0x2212) {
        cp = 0xFF0D;
    }

    if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
        cp = lxb_encoding_multi_index_iso_2022_jp_katakana[cp - 0xFF61].codepoint;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_jis0208,
                                      LXB_ENCODING_MULTI_HASH_JIS0208_SIZE, cp);
    if (hash == NULL) {
        goto failed;
    }

    if (ctx->state != LXB_ENCODING_ENCODE_2022_JP_JIS0208) {
        if ((*data + 3) > end) {
            goto small_buffer;
        }

        memcpy(*data, "\x1B\x24\x42", 3);
        *data = *data + 3;

        ctx->state = LXB_ENCODING_ENCODE_2022_JP_JIS0208;
        size += 3;

        goto begin;
    }

    *(*data)++ = (uint32_t) (uintptr_t) hash->value / 94 + 0x21;
    *(*data)++ = (uint32_t) (uintptr_t) hash->value % 94 + 0x21;

    return size + 2;

small_buffer:

    ctx->state = state;
    *data = *data - size;

    return LXB_ENCODING_ENCODE_SMALL_BUFFER;

failed:

    *data = *data - size;

    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_iso_2022_jp_eof_single(lxb_encoding_encode_t *ctx,
                                       lxb_char_t **data, const lxb_char_t *end)
{
    if (ctx->state != LXB_ENCODING_ENCODE_2022_JP_ASCII) {
        if ((*data + 3) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        memcpy(*data, "\x1B\x28\x42", 3);
        *data = *data + 3;

        ctx->state = LXB_ENCODING_ENCODE_2022_JP_ASCII;

        return 3;
    }

    return 0;
}

int8_t
lxb_encoding_encode_iso_8859_10_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_10,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_10_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_13_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_13,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_13_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_14_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_14,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_14_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_15_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_15,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_15_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_16_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_16,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_16_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_2_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_2,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_2_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_3_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_3,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_3_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_4_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_4,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_4_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_5_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_5,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_5_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_6_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_6,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_6_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_7_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_7,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_7_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_8_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                      const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_8,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_8_SIZE);
}

int8_t
lxb_encoding_encode_iso_8859_8_i_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_iso_8859_8,
                                    LXB_ENCODING_SINGLE_HASH_ISO_8859_8_SIZE);
}

int8_t
lxb_encoding_encode_koi8_r_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                  const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_koi8_r,
                                    LXB_ENCODING_SINGLE_HASH_KOI8_R_SIZE);
}

int8_t
lxb_encoding_encode_koi8_u_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                  const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_koi8_u,
                                    LXB_ENCODING_SINGLE_HASH_KOI8_U_SIZE);
}

int8_t
lxb_encoding_encode_shift_jis_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                     const lxb_char_t *end, lxb_codepoint_t cp)
{
    uint32_t lead, trail;
    const lexbor_shs_hash_t *hash;

    if (cp <= 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if ((unsigned) (cp - 0xFF61) <= (0xFF9F - 0xFF61)) {
        *(*data)++ = cp - 0xFF61 + 0xA1;

        return 1;
    }

    switch (cp) {
        case 0x00A5:
            *(*data)++ = 0x5C;
            return 1;

        case 0x203E:
            *(*data)++ = 0x7E;
            return 1;

        case 0x2212:
            cp = 0xFF0D;
            break;
    }

    hash = lxb_encoding_encode_shift_jis_index(cp);
    if (hash == NULL) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    if ((*data + 2) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    lead = (uint32_t) (uintptr_t) hash->value / 188;
    trail = (uint32_t) (uintptr_t) hash->value % 188;

    *(*data)++ = lead + ((lead < 0x1F) ? 0x81 : 0xC1);
    *(*data)++ = trail + ((trail < 0x3F) ? 0x40 : 0x41);

    return 2;
}

lxb_inline void
lxb_encoding_encode_utf_16_write_single(bool is_be, lxb_char_t **data,
                                        lxb_codepoint_t cp)
{
    if (is_be) {
        *(*data)++ = cp >> 8;
        *(*data)++ = cp & 0x00FF;

        return;
    }

    *(*data)++ = cp & 0x00FF;
    *(*data)++ = cp >> 8;
}

lxb_inline int8_t
lxb_encoding_encode_utf_16_single(lxb_encoding_encode_t *ctx, bool is_be,
                   lxb_char_t **data, const lxb_char_t *end, lxb_codepoint_t cp)
{
    if ((*data + 2) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    if (cp < 0x10000) {
        lxb_encoding_encode_utf_16_write_single(is_be, data, cp);

        return 2;
    }

    if ((*data + 4) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    cp -= 0x10000;

    lxb_encoding_encode_utf_16_write_single(is_be, data, (0xD800 | (cp >> 0x0A)));
    lxb_encoding_encode_utf_16_write_single(is_be, data, (0xDC00 | (cp & 0x03FF)));

    return 4;
}

int8_t
lxb_encoding_encode_utf_16be_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                    const lxb_char_t *end, lxb_codepoint_t cp)
{
    return lxb_encoding_encode_utf_16_single(ctx, true, data, end, cp);
}

int8_t
lxb_encoding_encode_utf_16le_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                    const lxb_char_t *end, lxb_codepoint_t cp)
{
    return lxb_encoding_encode_utf_16_single(ctx, false, data, end, cp);
}

int8_t
lxb_encoding_encode_utf_8_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                 const lxb_char_t *end, lxb_codepoint_t cp)
{
    if (cp < 0x80) {
        /* 0xxxxxxx */
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if (cp < 0x800) {
        if ((*data + 2) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        /* 110xxxxx 10xxxxxx */
        *(*data)++ = (lxb_char_t) (0xC0 | (cp >> 6  ));
        *(*data)++ = (lxb_char_t) (0x80 | (cp & 0x3F));

        return 2;
    }

    if (cp < 0x10000) {
        if ((*data + 3) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        /* 1110xxxx 10xxxxxx 10xxxxxx */
        *(*data)++ = (lxb_char_t) (0xE0 | ((cp >> 12)));
        *(*data)++ = (lxb_char_t) (0x80 | ((cp >> 6 ) & 0x3F));
        *(*data)++ = (lxb_char_t) (0x80 | ( cp        & 0x3F));

        return 3;
    }

    if (cp < 0x110000) {
        if ((*data + 4) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        *(*data)++ = (lxb_char_t) (0xF0 | ( cp >> 18));
        *(*data)++ = (lxb_char_t) (0x80 | ((cp >> 12) & 0x3F));
        *(*data)++ = (lxb_char_t) (0x80 | ((cp >> 6 ) & 0x3F));
        *(*data)++ = (lxb_char_t) (0x80 | ( cp        & 0x3F));

        return 4;
    }

    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_utf_8_length(lxb_codepoint_t cp)
{
    if (cp < 0x80) {
        return 1;
    }
    else if (cp < 0x800) {
        return 2;
    }
    else if (cp < 0x10000) {
        return 3;
    }
    else if (cp < 0x110000) {
        return 4;
    }

    return 0;
}

int8_t
lxb_encoding_encode_gb18030_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                   const lxb_char_t *end, lxb_codepoint_t cp)
{
    uint32_t index;
    const lexbor_shs_hash_t *hash;

    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if (cp == 0xE5E5) {
        return LXB_ENCODING_ENCODE_ERROR;
    }

    hash = lexbor_shs_hash_get_static(lxb_encoding_multi_hash_gb18030,
                                      LXB_ENCODING_MULTI_HASH_GB18030_SIZE, cp);
    if (hash != NULL) {
        if ((*data + 2) > end) {
            return LXB_ENCODING_ENCODE_SMALL_BUFFER;
        }

        *(*data)++ = (uint32_t) (uintptr_t) hash->value / 190 + 0x81;

        if (((uint32_t) (uintptr_t) hash->value % 190) < 0x3F) {
            *(*data)++ = ((uint32_t) (uintptr_t) hash->value % 190) + 0x40;
        }
        else {
            *(*data)++ = ((uint32_t) (uintptr_t) hash->value % 190) + 0x41;
        }

        return 2;
    }

    if ((*data + 4) > end) {
        return LXB_ENCODING_ENCODE_SMALL_BUFFER;
    }

    index = lxb_encoding_encode_gb18030_range(cp);

    *(*data)++ = (index / (10 * 126 * 10)) + 0x81;
    *(*data)++ = ((index % (10 * 126 * 10)) / (10 * 126)) + 0x30;

    index = (index % (10 * 126 * 10)) % (10 * 126);

    *(*data)++ = (index / 10) + 0x81;
    *(*data)++ = (index % 10) + 0x30;

    return 4;
}

int8_t
lxb_encoding_encode_macintosh_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                     const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_macintosh,
                                    LXB_ENCODING_SINGLE_HASH_MACINTOSH_SIZE);
}

int8_t
lxb_encoding_encode_replacement_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    (*data)++;
    return LXB_ENCODING_ENCODE_ERROR;
}

int8_t
lxb_encoding_encode_windows_1250_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1250,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1250_SIZE);
}

int8_t
lxb_encoding_encode_windows_1251_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1251,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1251_SIZE);
}

int8_t
lxb_encoding_encode_windows_1252_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1252,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1252_SIZE);
}

int8_t
lxb_encoding_encode_windows_1253_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1253,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1253_SIZE);
}

int8_t
lxb_encoding_encode_windows_1254_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1254,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1254_SIZE);
}

int8_t
lxb_encoding_encode_windows_1255_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1255,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1255_SIZE);
}

int8_t
lxb_encoding_encode_windows_1256_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1256,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1256_SIZE);
}

int8_t
lxb_encoding_encode_windows_1257_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1257,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1257_SIZE);
}

int8_t
lxb_encoding_encode_windows_1258_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                        const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_1258,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_1258_SIZE);
}

int8_t
lxb_encoding_encode_windows_874_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                       const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_windows_874,
                                    LXB_ENCODING_SINGLE_HASH_WINDOWS_874_SIZE);
}

int8_t
lxb_encoding_encode_x_mac_cyrillic_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                          const lxb_char_t *end, lxb_codepoint_t cp)
{
    LXB_ENCODING_ENCODE_BYTE_SINGLE(lxb_encoding_single_hash_x_mac_cyrillic,
                                  LXB_ENCODING_SINGLE_HASH_X_MAC_CYRILLIC_SIZE);
}

int8_t
lxb_encoding_encode_x_user_defined_single(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                          const lxb_char_t *end, lxb_codepoint_t cp)
{
    if (cp < 0x80) {
        *(*data)++ = (lxb_char_t) cp;

        return 1;
    }

    if (cp >= 0xF780 && cp <= 0xF7FF) {
        *(*data)++ = (lxb_char_t) (cp - 0xF780 + 0x80);

        return 1;
    }

    return LXB_ENCODING_ENCODE_ERROR;
}
