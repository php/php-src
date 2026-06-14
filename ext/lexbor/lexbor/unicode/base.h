/*
 * Copyright (C) 2023-2024 Alexander Borisov
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
#define LXB_UNICODE_VERSION_MINOR 4
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
#define LXB_UNICODE_CANONICAL_SEPARATELY        (1 << 7)
#define LXB_UNICODE_IS_CANONICAL_SEPARATELY(a)  ((a) >> 7)
#define LXB_UNICODE_DECOMPOSITION_TYPE(a)       ((a) & ~(1 << 7))
typedef uint8_t lxb_unicode_decomposition_type_t;

enum {
    LXB_UNICODE_QUICK__UNDEF     = 0x00,
    LXB_UNICODE_QUICK_NFC_MAYBE  = 1 << 0,
    LXB_UNICODE_QUICK_NFC_NO     = 1 << 1,
    LXB_UNICODE_QUICK_NFD_NO     = 1 << 2,
    LXB_UNICODE_QUICK_NFKC_MAYBE = 1 << 3,
    LXB_UNICODE_QUICK_NFKC_NO    = 1 << 4,
    LXB_UNICODE_QUICK_NFKD_NO    = 1 << 5
};
typedef uint8_t lxb_unicode_quick_type_t;

enum {
    LXB_UNICODE_IDNA__UNDEF = 0x00,
    LXB_UNICODE_IDNA_DEVIATION,
    LXB_UNICODE_IDNA_DISALLOWED,
    LXB_UNICODE_IDNA_IGNORED,
    LXB_UNICODE_IDNA_MAPPED,
    LXB_UNICODE_IDNA_VALID
};
typedef uint8_t lxb_unicode_idna_type_t;

typedef struct lxb_unicode_normalizer lxb_unicode_normalizer_t;

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
    uint16_t normalization; /* lxb_unicode_normalization_t */
    uint16_t idna;          /* lxb_unicode_idna_t */
}
lxb_unicode_entry_t;

typedef struct {
    lxb_unicode_decomposition_type_t type;
    lxb_unicode_quick_type_t         quick;         /* Quick Check.               */
    uint8_t                          ccc;           /* Canonical Combining Class. */
    uint8_t                          length;
    uint16_t                         decomposition; /* lxb_codepoint_t */
    uint16_t                         composition;   /* lxb_unicode_composition_entry_t */
}
lxb_unicode_normalization_entry_t;

typedef struct {
    lxb_unicode_idna_type_t type;
    uint8_t                 length;
    uint16_t                index;
}
lxb_unicode_idna_entry_t;

typedef struct {
    uint8_t         length;  /* Length in lxb_unicode_composition_cps_t */
    uint16_t        index;   /* lxb_unicode_composition_cps_t */
    lxb_codepoint_t cp;      /* Begin code point in lxb_unicode_composition_cps_t */
}
lxb_unicode_composition_entry_t;

typedef struct {
    lxb_codepoint_t cp;
    bool            exclusion;
}
lxb_unicode_composition_cp_t;

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
