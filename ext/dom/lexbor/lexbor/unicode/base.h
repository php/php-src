/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_UNICODE_BASE_H
#define LEXBOR_UNICODE_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/str.h"


#define LXB_UNICODE_VERSION_MAJOR 0
#define LXB_UNICODE_VERSION_MINOR 1
#define LXB_UNICODE_VERSION_PATCH 0

#define LXB_UNICODE_VERSION_STRING LEXBOR_STRINGIZE(LXB_UNICODE_VERSION_MAJOR) "." \
                                   LEXBOR_STRINGIZE(LXB_UNICODE_VERSION_MINOR) "." \
                                   LEXBOR_STRINGIZE(LXB_UNICODE_VERSION_PATCH)


enum {
    LXB_UNICODE_DECOMPOSITION_TYPE__UNDEF = 0x00,
    LXB_UNICODE_DECOMPOSITION_TYPE_CIRCLE,
    LXB_UNICODE_DECOMPOSITION_TYPE_COMPAT,
    LXB_UNICODE_DECOMPOSITION_TYPE_FINAL,
    LXB_UNICODE_DECOMPOSITION_TYPE_FONT,
    LXB_UNICODE_DECOMPOSITION_TYPE_FRACTION,
    LXB_UNICODE_DECOMPOSITION_TYPE_INITIAL,
    LXB_UNICODE_DECOMPOSITION_TYPE_ISOLATED,
    LXB_UNICODE_DECOMPOSITION_TYPE_MEDIAL,
    LXB_UNICODE_DECOMPOSITION_TYPE_NARROW,
    LXB_UNICODE_DECOMPOSITION_TYPE_NOBREAK,
    LXB_UNICODE_DECOMPOSITION_TYPE_SMALL,
    LXB_UNICODE_DECOMPOSITION_TYPE_SQUARE,
    LXB_UNICODE_DECOMPOSITION_TYPE_SUB,
    LXB_UNICODE_DECOMPOSITION_TYPE_SUPER,
    LXB_UNICODE_DECOMPOSITION_TYPE_VERTICAL,
    LXB_UNICODE_DECOMPOSITION_TYPE_WIDE,
    LXB_UNICODE_DECOMPOSITION_TYPE__LAST_ENTRY
};
typedef uint8_t lxb_unicode_decomposition_type_t;

enum {
    LXB_UNICODE_QUICK_UNDEF      = 0x00,
    LXB_UNICODE_NFC_QUICK_NO     = 1 << 1,
    LXB_UNICODE_NFC_QUICK_MAYBE  = 1 << 2,
    LXB_UNICODE_NFD_QUICK_NO     = 1 << 3,
    LXB_UNICODE_NFKC_QUICK_NO    = 1 << 4,
    LXB_UNICODE_NFKC_QUICK_MAYBE = 1 << 5,
    LXB_UNICODE_NFKD_QUICK_NO    = 1 << 6
};
typedef uint16_t lxb_unicode_quick_type_t;

enum {
    LXB_UNICODE_IDNA_UNDEF                  = 0x00,
    LXB_UNICODE_IDNA_DISALLOWED_STD3_VALID  = 0x01,
    LXB_UNICODE_IDNA_VALID                  = 0x02,
    LXB_UNICODE_IDNA_MAPPED                 = 0x03,
    LXB_UNICODE_IDNA_DISALLOWED             = 0x04,
    LXB_UNICODE_IDNA_DISALLOWED_STD3_MAPPED = 0x05,
    LXB_UNICODE_IDNA_IGNORED                = 0x06,
    LXB_UNICODE_IDNA_DEVIATION              = 0x07
};
typedef uint8_t lxb_unicode_idna_type_t;

typedef struct lxb_unicode_normalizer lxb_unicode_normalizer_t;
typedef struct lxb_unicode_compose_table lxb_unicode_compose_table_t;

typedef struct {
    lxb_codepoint_t cp;
    uint8_t         ccc;
}
lxb_unicode_buffer_t;

typedef lxb_status_t
(*lxb_unicode_nf_handler_f)(lxb_unicode_normalizer_t *uc, const lxb_char_t *data,
                            size_t length, lexbor_serialize_cb_f cb, void *ctx,
                            bool is_last);

typedef lxb_unicode_buffer_t *
(*lxb_unicode_de_handler_f)(lxb_unicode_normalizer_t *uc, lxb_codepoint_t cp,
                            lxb_unicode_buffer_t **buf,
                            const lxb_unicode_buffer_t **end);

typedef void
(*lxb_unicode_co_handler_f)(lxb_unicode_buffer_t *starter,
                            lxb_unicode_buffer_t *op, lxb_unicode_buffer_t *p);

typedef struct {
    lxb_codepoint_t idx;
    lxb_codepoint_t cp;
    bool            exclusion;
}
lxb_unicode_compose_entry_t;

typedef struct {
    const lxb_unicode_compose_entry_t *entry;
    const lxb_unicode_compose_table_t *table;
}
lxb_unicode_compose_node_t;

struct lxb_unicode_compose_table {
    uint16_t                         length;
    const lxb_unicode_compose_node_t *nodes;
};

typedef struct {
    lxb_unicode_decomposition_type_t type;
    const lxb_codepoint_t            *mapping;
    uint8_t                          length;
}
lxb_unicode_decomposition_t;

typedef struct {
    lxb_codepoint_t                   cp;    /* Codepoint.                        */
    uint8_t                           ccc;   /* Canonical Combining Class.        */
    lxb_unicode_quick_type_t          quick; /* Quick Check.                      */
    const lxb_unicode_decomposition_t *de;   /* Canonical Decomposition.          */
    const lxb_unicode_decomposition_t *cde;  /* Full Canonical Decomposition.     */
    const lxb_unicode_decomposition_t *kde;  /* Full Compatibility Decomposition. */
}
lxb_unicode_entry_t;

typedef struct {
    const lxb_codepoint_t *cps;
    uint8_t               length;
}
lxb_unicode_idna_map_t;

typedef struct {
    const lxb_unicode_entry_t    *entry;
    const lxb_unicode_idna_map_t *idna;
}
lxb_unicode_data_t;

struct lxb_unicode_normalizer {
    lxb_unicode_de_handler_f   decomposition;
    lxb_unicode_co_handler_f   composition;

    lxb_unicode_buffer_t       *starter;

    lxb_unicode_buffer_t       *buf;
    const lxb_unicode_buffer_t *end;
    lxb_unicode_buffer_t       *p;
    lxb_unicode_buffer_t       *ican;

    lxb_char_t                 tmp[4];
    uint8_t                    tmp_lenght;

    uint8_t                    quick_ccc;
    lxb_unicode_quick_type_t   quick_type;

    size_t                     flush_cp;
};


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_UNICODE_BASE_H */
