/*
 * Copyright (C) 2019 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_ENCODING_BASE_H
#define LEXBOR_ENCODING_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/encoding/const.h"


#define LXB_ENCODING_VERSION_MAJOR 2
#define LXB_ENCODING_VERSION_MINOR 0
#define LXB_ENCODING_VERSION_PATCH 1

#define LXB_ENCODING_VERSION_STRING                                            \
        LEXBOR_STRINGIZE(LXB_ENCODING_VERSION_MAJOR) "."                       \
        LEXBOR_STRINGIZE(LXB_ENCODING_VERSION_MINOR) "."                       \
        LEXBOR_STRINGIZE(LXB_ENCODING_VERSION_PATCH)


#define LXB_ENCODING_REPLACEMENT_BYTES ((lxb_char_t *) "\xEF\xBF\xBD")

#define LXB_ENCODING_REPLACEMENT_BUFFER_LEN 1
#define LXB_ENCODING_REPLACEMENT_BUFFER                                        \
    (&(const lxb_codepoint_t) {LXB_ENCODING_REPLACEMENT_CODEPOINT})


/*
 * In UTF-8 0x10FFFF value is maximum (inclusive)
 */
enum {
    LXB_ENCODING_REPLACEMENT_SIZE      = 0x03,
    LXB_ENCODING_REPLACEMENT_CODEPOINT = 0xFFFD,
    LXB_ENCODING_MAX_CODEPOINT         = 0x10FFFF,
    LXB_ENCODING_ERROR_CODEPOINT       = 0x1FFFFF
};

enum {
    LXB_ENCODING_ENCODE_OK           =  0x00,
    LXB_ENCODING_ENCODE_ERROR        = -0x01,
    LXB_ENCODING_ENCODE_SMALL_BUFFER = -0x02
};

enum {
    LXB_ENCODING_DECODE_MAX_CODEPOINT = LXB_ENCODING_MAX_CODEPOINT,
    LXB_ENCODING_DECODE_ERROR         = LXB_ENCODING_ERROR_CODEPOINT,
    LXB_ENCODING_DECODE_CONTINUE      = 0x2FFFFF
};

enum {
    LXB_ENCODING_DECODE_2022_JP_ASCII = 0x00,
    LXB_ENCODING_DECODE_2022_JP_ROMAN,
    LXB_ENCODING_DECODE_2022_JP_KATAKANA,
    LXB_ENCODING_DECODE_2022_JP_LEAD,
    LXB_ENCODING_DECODE_2022_JP_TRAIL,
    LXB_ENCODING_DECODE_2022_JP_ESCAPE_START,
    LXB_ENCODING_DECODE_2022_JP_ESCAPE,
    LXB_ENCODING_DECODE_2022_JP_UNSET
};

enum {
    LXB_ENCODING_ENCODE_2022_JP_ASCII = 0x00,
    LXB_ENCODING_ENCODE_2022_JP_ROMAN,
    LXB_ENCODING_ENCODE_2022_JP_JIS0208
};

typedef struct {
    unsigned   need;
    lxb_char_t lower;
    lxb_char_t upper;
}
lxb_encoding_ctx_utf_8_t;

typedef struct {
    lxb_char_t first;
    lxb_char_t second;
    lxb_char_t third;
}
lxb_encoding_ctx_gb18030_t;

typedef struct {
    lxb_char_t lead;
    bool       is_jis0212;
}
lxb_encoding_ctx_euc_jp_t;

typedef struct {
    lxb_char_t lead;
    lxb_char_t prepand;
    unsigned   state;
    unsigned   out_state;
    bool       out_flag;
}
lxb_encoding_ctx_2022_jp_t;

typedef struct lxb_encoding_data lxb_encoding_data_t;

typedef struct {
    const lxb_encoding_data_t *encoding_data;

    /* Out buffer */
    lxb_codepoint_t           *buffer_out;
    size_t                    buffer_length;
    size_t                    buffer_used;

    /*
     * Bad code points will be replaced to user code point.
     * If replace_to == 0 stop parsing and return error ot user.
     */
    const lxb_codepoint_t     *replace_to;
    size_t                    replace_len;

    /* Not for users */
    lxb_codepoint_t           codepoint;
    lxb_codepoint_t           second_codepoint;
    bool                      prepend;
    bool                      have_error;

    lxb_status_t              status;

    union {
        lxb_encoding_ctx_utf_8_t   utf_8;
        lxb_encoding_ctx_gb18030_t gb18030;
        unsigned                   lead;
        lxb_encoding_ctx_euc_jp_t  euc_jp;
        lxb_encoding_ctx_2022_jp_t iso_2022_jp;
    } u;
}
lxb_encoding_decode_t;

typedef struct {
    const lxb_encoding_data_t *encoding_data;

    /* Out buffer */
    lxb_char_t                *buffer_out;
    size_t                    buffer_length;
    size_t                    buffer_used;

    /*
     * Bad code points will be replaced to user bytes.
     * If replace_to == NULL stop parsing and return error ot user.
     */
    const lxb_char_t          *replace_to;
    size_t                    replace_len;

    unsigned                  state;
}
lxb_encoding_encode_t;

/*
* Why can't I pass a char ** to a function which expects a const char **?
* http://c-faq.com/ansi/constmismatch.html
*
* Short answer: use cast (const char **).
*
* For example:
*     lxb_encoding_ctx_t ctx = {0};
*     const lxb_encoding_data_t *enc;
*
*     lxb_char_t *data = (lxb_char_t *) "\x81\x30\x84\x36";
*
*     enc = lxb_encoding_data(LXB_ENCODING_GB18030);
*
*     enc->decode(&ctx, (const lxb_char_t **) &data, data + 4);
*/
typedef lxb_status_t
(*lxb_encoding_encode_f)(lxb_encoding_encode_t *ctx, const lxb_codepoint_t **cp,
                         const lxb_codepoint_t *end);

typedef lxb_status_t
(*lxb_encoding_decode_f)(lxb_encoding_decode_t *ctx,
                         const lxb_char_t **data, const lxb_char_t *end);

typedef int8_t
(*lxb_encoding_encode_single_f)(lxb_encoding_encode_t *ctx, lxb_char_t **data,
                                const lxb_char_t *end, lxb_codepoint_t cp);

typedef lxb_codepoint_t
(*lxb_encoding_decode_single_f)(lxb_encoding_decode_t *ctx,
                                const lxb_char_t **data, const lxb_char_t *end);

struct lxb_encoding_data {
    lxb_encoding_t               encoding;
    lxb_encoding_encode_f        encode;
    lxb_encoding_decode_f        decode;
    lxb_encoding_encode_single_f encode_single;
    lxb_encoding_decode_single_f decode_single;
    lxb_char_t                   *name;
};

typedef struct {
    lxb_char_t      name[4];
    unsigned        size;
    lxb_codepoint_t codepoint;
}
lxb_encoding_single_index_t;

typedef lxb_encoding_single_index_t lxb_encoding_multi_index_t;

typedef struct {
    unsigned        index;
    lxb_codepoint_t codepoint;
}
lxb_encoding_range_index_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_ENCODING_BASE_H */
