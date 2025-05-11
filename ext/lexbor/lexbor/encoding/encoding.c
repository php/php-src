/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include "lexbor/encoding/encoding.h"


const lxb_encoding_data_t *
lxb_encoding_data_by_pre_name(const lxb_char_t *name, size_t length)
{
    const lxb_char_t *end;
    const lexbor_shs_entry_t *entry;

    if (length == 0) {
        return NULL;
    }

    end = name + length;

    /* Remove any leading */
    do {
        switch (*name) {
            case 0x09: case 0x0A: case 0x0C: case 0x0D: case 0x20:
                name++;
                continue;
        }

        break;
    }
    while (name < end);

    /* Remove any trailing */
    while (name < end) {
        switch (*(end - 1)) {
            case 0x09: case 0x0A: case 0x0C: case 0x0D: case 0x20:
                end--;
                continue;
        }

        break;
    }

    if (name == end) {
        return NULL;
    }

    entry = lexbor_shs_entry_get_lower_static(lxb_encoding_res_shs_entities,
                                              name, (end - name));
    if (entry == NULL) {
        return NULL;
    }

    return entry->value;
}

/*
 * No inline functions for ABI.
 */
lxb_status_t
lxb_encoding_encode_init_noi(lxb_encoding_encode_t *encode,
                             const lxb_encoding_data_t *encoding_data,
                             lxb_char_t *buffer_out, size_t buffer_length)
{
    return lxb_encoding_encode_init(encode, encoding_data,
                                    buffer_out, buffer_length);
}

lxb_status_t
lxb_encoding_encode_finish_noi(lxb_encoding_encode_t *encode)
{
    return lxb_encoding_encode_finish(encode);
}

LXB_API lxb_char_t *
lxb_encoding_encode_buf_noi(lxb_encoding_encode_t *encode)
{
    return lxb_encoding_encode_buf(encode);
}

void
lxb_encoding_encode_buf_set_noi(lxb_encoding_encode_t *encode,
                                lxb_char_t *buffer_out, size_t buffer_length)
{
    lxb_encoding_encode_buf_set(encode, buffer_out, buffer_length);
}

void
lxb_encoding_encode_buf_used_set_noi(lxb_encoding_encode_t *encode,
                                     size_t buffer_used)
{
    lxb_encoding_encode_buf_used_set(encode, buffer_used);
}

size_t
lxb_encoding_encode_buf_used_noi(lxb_encoding_encode_t *encode)
{
    return lxb_encoding_encode_buf_used(encode);
}

lxb_status_t
lxb_encoding_encode_replace_set_noi(lxb_encoding_encode_t *encode,
                                    const lxb_char_t *replace, size_t length)
{
    return lxb_encoding_encode_replace_set(encode, replace, length);
}

lxb_status_t
lxb_encoding_encode_buf_add_to_noi(lxb_encoding_encode_t *encode,
                                   lxb_char_t *data, size_t length)
{
    return lxb_encoding_encode_buf_add_to(encode, data, length);
}

lxb_status_t
lxb_encoding_decode_init_noi(lxb_encoding_decode_t *decode,
                             const lxb_encoding_data_t *encoding_data,
                             lxb_codepoint_t *buffer_out, size_t buffer_length)
{
    return lxb_encoding_decode_init(decode, encoding_data,
                                    buffer_out, buffer_length);
}

lxb_status_t
lxb_encoding_decode_finish_noi(lxb_encoding_decode_t *decode)
{
    return lxb_encoding_decode_finish(decode);
}

lxb_codepoint_t *
lxb_encoding_decode_buf_noi(lxb_encoding_decode_t *decode)
{
    return lxb_encoding_decode_buf(decode);
}

void
lxb_encoding_decode_buf_set_noi(lxb_encoding_decode_t *decode,
                              lxb_codepoint_t *buffer_out, size_t buffer_length)
{
    lxb_encoding_decode_buf_set(decode, buffer_out, buffer_length);
}

void
lxb_encoding_decode_buf_used_set_noi(lxb_encoding_decode_t *decode,
                                     size_t buffer_used)
{
    lxb_encoding_decode_buf_used_set(decode, buffer_used);
}

size_t
lxb_encoding_decode_buf_used_noi(lxb_encoding_decode_t *decode)
{
    return lxb_encoding_decode_buf_used(decode);
}

lxb_status_t
lxb_encoding_decode_replace_set_noi(lxb_encoding_decode_t *decode,
                                  const lxb_codepoint_t *replace, size_t length)
{
    return lxb_encoding_decode_replace_set(decode, replace, length);
}

lxb_status_t
lxb_encoding_decode_buf_add_to_noi(lxb_encoding_decode_t *decode,
                                   const lxb_codepoint_t *data, size_t length)
{
    return lxb_encoding_decode_buf_add_to(decode, data, length);
}

lxb_status_t
lxb_encoding_encode_init_single_noi(lxb_encoding_encode_t *encode,
                                    const lxb_encoding_data_t *encoding_data)
{
    return lxb_encoding_encode_init_single(encode, encoding_data);
}

int8_t
lxb_encoding_encode_finish_single_noi(lxb_encoding_encode_t *encode,
                                      lxb_char_t **data, const lxb_char_t *end)
{
    return lxb_encoding_encode_finish_single(encode, data, end);
}

lxb_status_t
lxb_encoding_decode_init_single_noi(lxb_encoding_decode_t *decode,
                                    const lxb_encoding_data_t *encoding_data)
{
    return lxb_encoding_decode_init_single(decode, encoding_data);
}

lxb_status_t
lxb_encoding_decode_finish_single_noi(lxb_encoding_decode_t *decode)
{
    return lxb_encoding_decode_finish_single(decode);
}

const lxb_encoding_data_t *
lxb_encoding_data_by_name_noi(const lxb_char_t *name, size_t length)
{
    return lxb_encoding_data_by_name(name, length);
}

const lxb_encoding_data_t *
lxb_encoding_data_noi(lxb_encoding_t encoding)
{
    return lxb_encoding_data(encoding);
}

lxb_encoding_encode_f
lxb_encoding_encode_function_noi(lxb_encoding_t encoding)
{
    return lxb_encoding_encode_function(encoding);
}

lxb_encoding_decode_f
lxb_encoding_decode_function_noi(lxb_encoding_t encoding)
{
    return lxb_encoding_decode_function(encoding);
}

lxb_status_t
lxb_encoding_data_call_encode_noi(lxb_encoding_data_t *encoding_data, lxb_encoding_encode_t *ctx,
                                  const lxb_codepoint_t **cp, const lxb_codepoint_t *end)
{
    return lxb_encoding_data_call_encode(encoding_data, ctx, cp, end);
}

lxb_status_t
lxb_encoding_data_call_decode_noi(lxb_encoding_data_t *encoding_data, lxb_encoding_decode_t *ctx,
                                  const lxb_char_t **data, const lxb_char_t *end)
{
    return lxb_encoding_data_call_decode(encoding_data, ctx, data, end);
}

lxb_encoding_t
lxb_encoding_data_encoding_noi(lxb_encoding_data_t *data)
{
    return lxb_encoding_data_encoding(data);
}

size_t
lxb_encoding_encode_t_sizeof(void)
{
    return sizeof(lxb_encoding_encode_t);
}

size_t
lxb_encoding_decode_t_sizeof(void)
{
    return sizeof(lxb_encoding_decode_t);
}
