/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_ENCODING_ENCODING_H
#define LEXBOR_ENCODING_ENCODING_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/encoding/base.h"
#include "lexbor/encoding/res.h"
#include "lexbor/encoding/encode.h"
#include "lexbor/encoding/decode.h"

#include "lexbor/core/shs.h"


/*
 * Before searching will be removed any leading and trailing
 * ASCII whitespace in name.
 */
LXB_API const lxb_encoding_data_t *
lxb_encoding_data_by_pre_name(const lxb_char_t *name, size_t length);


/*
 * Inline functions
 */

/*
 * Encode
 */
lxb_inline lxb_status_t
lxb_encoding_encode_init(lxb_encoding_encode_t *encode,
                         const lxb_encoding_data_t *encoding_data,
                         lxb_char_t *buffer_out, size_t buffer_length)
{
    if (encoding_data == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    memset(encode, 0, sizeof(lxb_encoding_encode_t));

    encode->buffer_out = buffer_out;
    encode->buffer_length = buffer_length;
    encode->encoding_data = encoding_data;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_encoding_encode_finish(lxb_encoding_encode_t *encode)
{
    if (encode->encoding_data->encoding == LXB_ENCODING_ISO_2022_JP) {
        return lxb_encoding_encode_iso_2022_jp_eof(encode);
    }

    return LXB_STATUS_OK;
}

lxb_inline lxb_char_t *
lxb_encoding_encode_buf(lxb_encoding_encode_t *encode)
{
    return encode->buffer_out;
}

lxb_inline void
lxb_encoding_encode_buf_set(lxb_encoding_encode_t *encode,
                            lxb_char_t *buffer_out, size_t buffer_length)
{
    encode->buffer_out = buffer_out;
    encode->buffer_length = buffer_length;
    encode->buffer_used = 0;
}

lxb_inline void
lxb_encoding_encode_buf_used_set(lxb_encoding_encode_t *encode,
                                 size_t buffer_used)
{
    encode->buffer_used = buffer_used;
}

lxb_inline size_t
lxb_encoding_encode_buf_used(lxb_encoding_encode_t *encode)
{
    return encode->buffer_used;
}

lxb_inline lxb_status_t
lxb_encoding_encode_replace_set(lxb_encoding_encode_t *encode,
                                const lxb_char_t *replace, size_t length)
{
    if (encode->buffer_out == NULL || encode->buffer_length < length) {
        return LXB_STATUS_SMALL_BUFFER;
    }

    encode->replace_to = replace;
    encode->replace_len = length;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_encoding_encode_buf_add_to(lxb_encoding_encode_t *encode,
                               lxb_char_t *data, size_t length)
{
    if ((encode->buffer_used + length) > encode->buffer_length) {
        return LXB_STATUS_SMALL_BUFFER;
    }

    memcpy(&encode->buffer_out[encode->buffer_used], data, length);

    encode->buffer_used += length;

    return LXB_STATUS_OK;
}

/*
 * Decode
 */
lxb_inline lxb_status_t
lxb_encoding_decode_buf_add_to(lxb_encoding_decode_t *decode,
                               const lxb_codepoint_t *data, size_t length)
{
    if ((decode->buffer_used + length) > decode->buffer_length) {
        return LXB_STATUS_SMALL_BUFFER;
    }

    memcpy(&decode->buffer_out[decode->buffer_used], data,
           sizeof(lxb_codepoint_t) * length);

    decode->buffer_used += length;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_encoding_decode_init(lxb_encoding_decode_t *decode,
                         const lxb_encoding_data_t *encoding_data,
                         lxb_codepoint_t *buffer_out, size_t buffer_length)
{
    if (encoding_data == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    memset(decode, 0, sizeof(lxb_encoding_decode_t));

    decode->buffer_out = buffer_out;
    decode->buffer_length = buffer_length;
    decode->encoding_data = encoding_data;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_encoding_decode_finish(lxb_encoding_decode_t *decode)
{
    lxb_status_t status;

    if (decode->status != LXB_STATUS_OK) {

        if (decode->encoding_data->encoding == LXB_ENCODING_ISO_2022_JP
            && decode->u.iso_2022_jp.state == LXB_ENCODING_DECODE_2022_JP_ASCII)
        {
            return LXB_STATUS_OK;
        }

        if (decode->replace_to == NULL) {
            return LXB_STATUS_ERROR;
        }

        status = lxb_encoding_decode_buf_add_to(decode, decode->replace_to,
                                                decode->replace_len);
        if (status == LXB_STATUS_SMALL_BUFFER) {
            return status;
        }
    }

    return LXB_STATUS_OK;
}

lxb_inline lxb_codepoint_t *
lxb_encoding_decode_buf(lxb_encoding_decode_t *decode)
{
    return decode->buffer_out;
}

lxb_inline void
lxb_encoding_decode_buf_set(lxb_encoding_decode_t *decode,
                            lxb_codepoint_t *buffer_out, size_t buffer_length)
{
    decode->buffer_out = buffer_out;
    decode->buffer_length = buffer_length;
    decode->buffer_used = 0;
}

lxb_inline void
lxb_encoding_decode_buf_used_set(lxb_encoding_decode_t *decode,
                                 size_t buffer_used)
{
    decode->buffer_used = buffer_used;
}

lxb_inline size_t
lxb_encoding_decode_buf_used(lxb_encoding_decode_t *decode)
{
    return decode->buffer_used;
}

lxb_inline lxb_status_t
lxb_encoding_decode_replace_set(lxb_encoding_decode_t *decode,
                                const lxb_codepoint_t *replace, size_t length)
{
    if (decode->buffer_out == NULL || decode->buffer_length < length) {
        return LXB_STATUS_SMALL_BUFFER;
    }

    decode->replace_to = replace;
    decode->replace_len = length;

    return LXB_STATUS_OK;
}

/*
 * Single encode.
 */
lxb_inline lxb_status_t
lxb_encoding_encode_init_single(lxb_encoding_encode_t *encode,
                                const lxb_encoding_data_t *encoding_data)
{
    if (encoding_data == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    memset(encode, 0, sizeof(lxb_encoding_encode_t));

    encode->encoding_data = encoding_data;

    return LXB_STATUS_OK;
}

lxb_inline int8_t
lxb_encoding_encode_finish_single(lxb_encoding_encode_t *encode,
                                  lxb_char_t **data, const lxb_char_t *end)
{
    if (encode->encoding_data->encoding == LXB_ENCODING_ISO_2022_JP) {
        return lxb_encoding_encode_iso_2022_jp_eof_single(encode, data, end);
    }

    return 0;
}

/*
 * Single decode.
 */
lxb_inline lxb_status_t
lxb_encoding_decode_init_single(lxb_encoding_decode_t *decode,
                                const lxb_encoding_data_t *encoding_data)
{
    if (encoding_data == NULL) {
        return LXB_STATUS_ERROR_WRONG_ARGS;
    }

    memset(decode, 0, sizeof(lxb_encoding_decode_t));

    decode->encoding_data = encoding_data;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_encoding_decode_finish_single(lxb_encoding_decode_t *decode)
{
    if (decode->status != LXB_STATUS_OK) {

        if (decode->encoding_data->encoding == LXB_ENCODING_ISO_2022_JP
            && decode->u.iso_2022_jp.state == LXB_ENCODING_DECODE_2022_JP_ASCII)
        {
            return LXB_STATUS_OK;
        }

        return LXB_STATUS_ERROR;
    }

    return LXB_STATUS_OK;
}

/*
 * Encoding data.
 */
lxb_inline const lxb_encoding_data_t *
lxb_encoding_data_by_name(const lxb_char_t *name, size_t length)
{
    const lexbor_shs_entry_t *entry;

    if (length == 0) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_encoding_res_shs_entities,
                                              name, length);
    if (entry == NULL) {
        return NULL;
    }

    return (const lxb_encoding_data_t *) entry->value;
}

lxb_inline const lxb_encoding_data_t *
lxb_encoding_data(lxb_encoding_t encoding)
{
    if (encoding >= LXB_ENCODING_LAST_ENTRY) {
        return NULL;
    }

    return &lxb_encoding_res_map[encoding];
}

lxb_inline lxb_encoding_encode_f
lxb_encoding_encode_function(lxb_encoding_t encoding)
{
    if (encoding >= LXB_ENCODING_LAST_ENTRY) {
        return NULL;
    }

    return lxb_encoding_res_map[encoding].encode;
}

lxb_inline lxb_encoding_decode_f
lxb_encoding_decode_function(lxb_encoding_t encoding)
{
    if (encoding >= LXB_ENCODING_LAST_ENTRY) {
        return NULL;
    }

    return lxb_encoding_res_map[encoding].decode;
}

lxb_inline lxb_status_t
lxb_encoding_data_call_encode(lxb_encoding_data_t *encoding_data, lxb_encoding_encode_t *ctx,
                              const lxb_codepoint_t **cp, const lxb_codepoint_t *end)
{
    return encoding_data->encode(ctx, cp, end);
}

lxb_inline lxb_status_t
lxb_encoding_data_call_decode(lxb_encoding_data_t *encoding_data, lxb_encoding_decode_t *ctx,
                              const lxb_char_t **data, const lxb_char_t *end)
{
    return encoding_data->decode(ctx, data, end);
}

lxb_inline lxb_encoding_t
lxb_encoding_data_encoding(lxb_encoding_data_t *data)
{
    return data->encoding;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_status_t
lxb_encoding_encode_init_noi(lxb_encoding_encode_t *encode,
                             const lxb_encoding_data_t *encoding_data,
                             lxb_char_t *buffer_out, size_t buffer_length);

LXB_API lxb_status_t
lxb_encoding_encode_finish_noi(lxb_encoding_encode_t *encode);

LXB_API lxb_char_t *
lxb_encoding_encode_buf_noi(lxb_encoding_encode_t *encode);

LXB_API void
lxb_encoding_encode_buf_set_noi(lxb_encoding_encode_t *encode,
                                lxb_char_t *buffer_out, size_t buffer_length);

LXB_API void
lxb_encoding_encode_buf_used_set_noi(lxb_encoding_encode_t *encode,
                                     size_t buffer_used);

LXB_API size_t
lxb_encoding_encode_buf_used_noi(lxb_encoding_encode_t *encode);

LXB_API lxb_status_t
lxb_encoding_encode_replace_set_noi(lxb_encoding_encode_t *encode,
                               const lxb_char_t *replace, size_t buffer_length);

LXB_API lxb_status_t
lxb_encoding_encode_buf_add_to_noi(lxb_encoding_encode_t *encode,
                                   lxb_char_t *data, size_t length);

LXB_API lxb_status_t
lxb_encoding_decode_init_noi(lxb_encoding_decode_t *decode,
                             const lxb_encoding_data_t *encoding_data,
                             lxb_codepoint_t *buffer_out, size_t buffer_length);

LXB_API lxb_status_t
lxb_encoding_decode_finish_noi(lxb_encoding_decode_t *decode);

LXB_API lxb_codepoint_t *
lxb_encoding_decode_buf_noi(lxb_encoding_decode_t *decode);

LXB_API void
lxb_encoding_decode_buf_set_noi(lxb_encoding_decode_t *decode,
                             lxb_codepoint_t *buffer_out, size_t buffer_length);

LXB_API void
lxb_encoding_decode_buf_used_set_noi(lxb_encoding_decode_t *decode,
                                     size_t buffer_used);

LXB_API size_t
lxb_encoding_decode_buf_used_noi(lxb_encoding_decode_t *decode);

LXB_API lxb_status_t
lxb_encoding_decode_replace_set_noi(lxb_encoding_decode_t *decode,
                                 const lxb_codepoint_t *replace, size_t length);

LXB_API lxb_status_t
lxb_encoding_decode_buf_add_to_noi(lxb_encoding_decode_t *decode,
                                   const lxb_codepoint_t *data, size_t length);

LXB_API lxb_status_t
lxb_encoding_encode_init_single_noi(lxb_encoding_encode_t *encode,
                                    const lxb_encoding_data_t *encoding_data);

LXB_API int8_t
lxb_encoding_encode_finish_single_noi(lxb_encoding_encode_t *encode,
                                      lxb_char_t **data, const lxb_char_t *end);

LXB_API lxb_status_t
lxb_encoding_decode_init_single_noi(lxb_encoding_decode_t *decode,
                                    const lxb_encoding_data_t *encoding_data);

LXB_API lxb_status_t
lxb_encoding_decode_finish_single_noi(lxb_encoding_decode_t *decode);

LXB_API const lxb_encoding_data_t *
lxb_encoding_data_by_name_noi(const lxb_char_t *name, size_t length);

LXB_API const lxb_encoding_data_t *
lxb_encoding_data_noi(lxb_encoding_t encoding);

LXB_API lxb_encoding_encode_f
lxb_encoding_encode_function_noi(lxb_encoding_t encoding);

LXB_API lxb_encoding_decode_f
lxb_encoding_decode_function_noi(lxb_encoding_t encoding);

LXB_API lxb_status_t
lxb_encoding_data_call_encode_noi(lxb_encoding_data_t *encoding_data, lxb_encoding_encode_t *ctx,
                                  const lxb_codepoint_t **cp, const lxb_codepoint_t *end);
LXB_API lxb_status_t
lxb_encoding_data_call_decode_noi(lxb_encoding_data_t *encoding_data, lxb_encoding_decode_t *ctx,
                                  const lxb_char_t **data, const lxb_char_t *end);

LXB_API lxb_encoding_t
lxb_encoding_data_encoding_noi(lxb_encoding_data_t *data);

LXB_API size_t
lxb_encoding_encode_t_sizeof(void);

LXB_API size_t
lxb_encoding_decode_t_sizeof(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_ENCODING_ENCODING_H */
