/*
 * Copyright (C) 2023-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#include <math.h>
#include <float.h>

#include "lexbor/url/url.h"
#include "lexbor/core/conv.h"
#include "lexbor/core/utils.h"
#include "lexbor/core/serialize.h"
#include "lexbor/core/swar.h"
#include "lexbor/unicode/idna.h"

#define LEXBOR_STR_RES_MAP_LOWERCASE
#define LEXBOR_STR_RES_ALPHANUMERIC_CHARACTER
#define LEXBOR_STR_RES_ALPHA_CHARACTER
#define LEXBOR_STR_RES_CHAR_TO_TWO_HEX_VALUE
#define LEXBOR_STR_RES_MAP_HEX
#define LEXBOR_STR_RES_MAP_NUM
#include "lexbor/core/str_res.h"


#define LXB_URL_BUFFER_SIZE 4096
#define LXB_URL_BUFFER_NUM_SIZE 128


typedef enum {
    LXB_URL_MAP_UNDEF         = 0x00,
    LXB_URL_MAP_C0            = 0x01,
    LXB_URL_MAP_FRAGMENT      = 0x02,
    LXB_URL_MAP_QUERY         = 0x04,
    LXB_URL_MAP_SPECIAL_QUERY = 0x08,
    LXB_URL_MAP_PATH          = 0x10,
    LXB_URL_MAP_USERINFO      = 0x20,
    LXB_URL_MAP_COMPONENT     = 0x40,
    LXB_URL_MAP_X_WWW_FORM    = 0x80,
    LXB_URL_MAP_ALL           = 0xff
}
lxb_url_map_type_t;

typedef enum {
    LXB_URL_HOST_OPT_UNDEF       = 0 << 0,
    LXB_URL_HOST_OPT_NOT_SPECIAL = 1 << 0,
    LXB_URL_HOST_OPT_DECODE      = 1 << 1,
    LXB_URL_HOST_OPT_IDNA        = 1 << 2
}
lxb_url_host_opt_t;

typedef struct {
    lexbor_mraw_t *mraw;
    lexbor_str_t  *str;
}
lxb_url_idna_ctx_t;

typedef struct {
    lexbor_str_t **out_buf;
    size_t out_length;
    size_t out_size;
}
lxb_url_search_params_ctx_t;


static const uint8_t lxb_url_map[256] =
{
    LXB_URL_MAP_ALL, /* 0x00 */
    LXB_URL_MAP_ALL, /* 0x01 */
    LXB_URL_MAP_ALL, /* 0x02 */
    LXB_URL_MAP_ALL, /* 0x03 */
    LXB_URL_MAP_ALL, /* 0x04 */
    LXB_URL_MAP_ALL, /* 0x05 */
    LXB_URL_MAP_ALL, /* 0x06 */
    LXB_URL_MAP_ALL, /* 0x07 */
    LXB_URL_MAP_ALL, /* 0x08 */
    LXB_URL_MAP_ALL, /* 0x09 */
    LXB_URL_MAP_ALL, /* 0x0a */
    LXB_URL_MAP_ALL, /* 0x0b */
    LXB_URL_MAP_ALL, /* 0x0c */
    LXB_URL_MAP_ALL, /* 0x0d */
    LXB_URL_MAP_ALL, /* 0x0e */
    LXB_URL_MAP_ALL, /* 0x0f */
    LXB_URL_MAP_ALL, /* 0x10 */
    LXB_URL_MAP_ALL, /* 0x11 */
    LXB_URL_MAP_ALL, /* 0x12 */
    LXB_URL_MAP_ALL, /* 0x13 */
    LXB_URL_MAP_ALL, /* 0x14 */
    LXB_URL_MAP_ALL, /* 0x15 */
    LXB_URL_MAP_ALL, /* 0x16 */
    LXB_URL_MAP_ALL, /* 0x17 */
    LXB_URL_MAP_ALL, /* 0x18 */
    LXB_URL_MAP_ALL, /* 0x19 */
    LXB_URL_MAP_ALL, /* 0x1a */
    LXB_URL_MAP_ALL, /* 0x1b */
    LXB_URL_MAP_ALL, /* 0x1c */
    LXB_URL_MAP_ALL, /* 0x1d */
    LXB_URL_MAP_ALL, /* 0x1e */
    LXB_URL_MAP_ALL, /* 0x1f */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_FRAGMENT|LXB_URL_MAP_QUERY|LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x20 ( ) */
    LXB_URL_MAP_X_WWW_FORM, /* 0x21 (!) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_FRAGMENT|LXB_URL_MAP_QUERY|LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x22 (") */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_QUERY|LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x23 (#) */
    LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x24 ($) */
    LXB_URL_MAP_X_WWW_FORM, /* 0x25 (%) */
    LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x26 (&) */
    LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_X_WWW_FORM, /* 0x27 (') */
    LXB_URL_MAP_X_WWW_FORM, /* 0x28 (() */
    LXB_URL_MAP_X_WWW_FORM, /* 0x29 ()) */
    LXB_URL_MAP_UNDEF, /* 0x2a (*) */
    LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x2b (+) */
    LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x2c (,) */
    LXB_URL_MAP_UNDEF, /* 0x2d (-) */
    LXB_URL_MAP_UNDEF, /* 0x2e (.) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x2f (/) */
    LXB_URL_MAP_UNDEF, /* 0x30 (0) */
    LXB_URL_MAP_UNDEF, /* 0x31 (1) */
    LXB_URL_MAP_UNDEF, /* 0x32 (2) */
    LXB_URL_MAP_UNDEF, /* 0x33 (3) */
    LXB_URL_MAP_UNDEF, /* 0x34 (4) */
    LXB_URL_MAP_UNDEF, /* 0x35 (5) */
    LXB_URL_MAP_UNDEF, /* 0x36 (6) */
    LXB_URL_MAP_UNDEF, /* 0x37 (7) */
    LXB_URL_MAP_UNDEF, /* 0x38 (8) */
    LXB_URL_MAP_UNDEF, /* 0x39 (9) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3a (:) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3b (;) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_FRAGMENT|LXB_URL_MAP_QUERY|LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3c (<) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3d (=) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_FRAGMENT|LXB_URL_MAP_QUERY|LXB_URL_MAP_SPECIAL_QUERY|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3e (>) */
    LXB_URL_MAP_PATH|LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x3f (?) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x40 (@) */
    LXB_URL_MAP_UNDEF, /* 0x41 (A) */
    LXB_URL_MAP_UNDEF, /* 0x42 (B) */
    LXB_URL_MAP_UNDEF, /* 0x43 (C) */
    LXB_URL_MAP_UNDEF, /* 0x44 (D) */
    LXB_URL_MAP_UNDEF, /* 0x45 (E) */
    LXB_URL_MAP_UNDEF, /* 0x46 (F) */
    LXB_URL_MAP_UNDEF, /* 0x47 (G) */
    LXB_URL_MAP_UNDEF, /* 0x48 (H) */
    LXB_URL_MAP_UNDEF, /* 0x49 (I) */
    LXB_URL_MAP_UNDEF, /* 0x4a (J) */
    LXB_URL_MAP_UNDEF, /* 0x4b (K) */
    LXB_URL_MAP_UNDEF, /* 0x4c (L) */
    LXB_URL_MAP_UNDEF, /* 0x4d (M) */
    LXB_URL_MAP_UNDEF, /* 0x4e (N) */
    LXB_URL_MAP_UNDEF, /* 0x4f (O) */
    LXB_URL_MAP_UNDEF, /* 0x50 (P) */
    LXB_URL_MAP_UNDEF, /* 0x51 (Q) */
    LXB_URL_MAP_UNDEF, /* 0x52 (R) */
    LXB_URL_MAP_UNDEF, /* 0x53 (S) */
    LXB_URL_MAP_UNDEF, /* 0x54 (T) */
    LXB_URL_MAP_UNDEF, /* 0x55 (U) */
    LXB_URL_MAP_UNDEF, /* 0x56 (V) */
    LXB_URL_MAP_UNDEF, /* 0x57 (W) */
    LXB_URL_MAP_UNDEF, /* 0x58 (X) */
    LXB_URL_MAP_UNDEF, /* 0x59 (Y) */
    LXB_URL_MAP_UNDEF, /* 0x5a (Z) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x5b ([) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x5c (\) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x5d (]) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x5e (^) */
    LXB_URL_MAP_UNDEF, /* 0x5f (_) */
    LXB_URL_MAP_PATH|LXB_URL_MAP_FRAGMENT|LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x60 (`) */
    LXB_URL_MAP_UNDEF, /* 0x61 (a) */
    LXB_URL_MAP_UNDEF, /* 0x62 (b) */
    LXB_URL_MAP_UNDEF, /* 0x63 (c) */
    LXB_URL_MAP_UNDEF, /* 0x64 (d) */
    LXB_URL_MAP_UNDEF, /* 0x65 (e) */
    LXB_URL_MAP_UNDEF, /* 0x66 (f) */
    LXB_URL_MAP_UNDEF, /* 0x67 (g) */
    LXB_URL_MAP_UNDEF, /* 0x68 (h) */
    LXB_URL_MAP_UNDEF, /* 0x69 (i) */
    LXB_URL_MAP_UNDEF, /* 0x6a (j) */
    LXB_URL_MAP_UNDEF, /* 0x6b (k) */
    LXB_URL_MAP_UNDEF, /* 0x6c (l) */
    LXB_URL_MAP_UNDEF, /* 0x6d (m) */
    LXB_URL_MAP_UNDEF, /* 0x6e (n) */
    LXB_URL_MAP_UNDEF, /* 0x6f (o) */
    LXB_URL_MAP_UNDEF, /* 0x70 (p) */
    LXB_URL_MAP_UNDEF, /* 0x71 (q) */
    LXB_URL_MAP_UNDEF, /* 0x72 (r) */
    LXB_URL_MAP_UNDEF, /* 0x73 (s) */
    LXB_URL_MAP_UNDEF, /* 0x74 (t) */
    LXB_URL_MAP_UNDEF, /* 0x75 (u) */
    LXB_URL_MAP_UNDEF, /* 0x76 (v) */
    LXB_URL_MAP_UNDEF, /* 0x77 (w) */
    LXB_URL_MAP_UNDEF, /* 0x78 (x) */
    LXB_URL_MAP_UNDEF, /* 0x79 (y) */
    LXB_URL_MAP_UNDEF, /* 0x7a (z) */
    LXB_URL_MAP_PATH|LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x7b ({) */
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x7c (|) */
    LXB_URL_MAP_PATH|LXB_URL_MAP_USERINFO|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x7d (}) */
    LXB_URL_MAP_X_WWW_FORM, /* 0x7e (~) */
    LXB_URL_MAP_ALL, /* 0x7f */
    LXB_URL_MAP_ALL, /* 0x80 */
    LXB_URL_MAP_ALL, /* 0x81 */
    LXB_URL_MAP_ALL, /* 0x82 */
    LXB_URL_MAP_ALL, /* 0x83 */
    LXB_URL_MAP_ALL, /* 0x84 */
    LXB_URL_MAP_ALL, /* 0x85 */
    LXB_URL_MAP_ALL, /* 0x86 */
    LXB_URL_MAP_ALL, /* 0x87 */
    LXB_URL_MAP_ALL, /* 0x88 */
    LXB_URL_MAP_ALL, /* 0x89 */
    LXB_URL_MAP_ALL, /* 0x8a */
    LXB_URL_MAP_ALL, /* 0x8b */
    LXB_URL_MAP_ALL, /* 0x8c */
    LXB_URL_MAP_ALL, /* 0x8d */
    LXB_URL_MAP_ALL, /* 0x8e */
    LXB_URL_MAP_ALL, /* 0x8f */
    LXB_URL_MAP_ALL, /* 0x90 */
    LXB_URL_MAP_ALL, /* 0x91 */
    LXB_URL_MAP_ALL, /* 0x92 */
    LXB_URL_MAP_ALL, /* 0x93 */
    LXB_URL_MAP_ALL, /* 0x94 */
    LXB_URL_MAP_ALL, /* 0x95 */
    LXB_URL_MAP_ALL, /* 0x96 */
    LXB_URL_MAP_ALL, /* 0x97 */
    LXB_URL_MAP_ALL, /* 0x98 */
    LXB_URL_MAP_ALL, /* 0x99 */
    LXB_URL_MAP_ALL, /* 0x9a */
    LXB_URL_MAP_ALL, /* 0x9b */
    LXB_URL_MAP_ALL, /* 0x9c */
    LXB_URL_MAP_ALL, /* 0x9d */
    LXB_URL_MAP_ALL, /* 0x9e */
    LXB_URL_MAP_ALL, /* 0x9f */
    LXB_URL_MAP_ALL, /* 0xa0 */
    LXB_URL_MAP_ALL, /* 0xa1 */
    LXB_URL_MAP_ALL, /* 0xa2 */
    LXB_URL_MAP_ALL, /* 0xa3 */
    LXB_URL_MAP_ALL, /* 0xa4 */
    LXB_URL_MAP_ALL, /* 0xa5 */
    LXB_URL_MAP_ALL, /* 0xa6 */
    LXB_URL_MAP_ALL, /* 0xa7 */
    LXB_URL_MAP_ALL, /* 0xa8 */
    LXB_URL_MAP_ALL, /* 0xa9 */
    LXB_URL_MAP_ALL, /* 0xaa */
    LXB_URL_MAP_ALL, /* 0xab */
    LXB_URL_MAP_ALL, /* 0xac */
    LXB_URL_MAP_ALL, /* 0xad */
    LXB_URL_MAP_ALL, /* 0xae */
    LXB_URL_MAP_ALL, /* 0xaf */
    LXB_URL_MAP_ALL, /* 0xb0 */
    LXB_URL_MAP_ALL, /* 0xb1 */
    LXB_URL_MAP_ALL, /* 0xb2 */
    LXB_URL_MAP_ALL, /* 0xb3 */
    LXB_URL_MAP_ALL, /* 0xb4 */
    LXB_URL_MAP_ALL, /* 0xb5 */
    LXB_URL_MAP_ALL, /* 0xb6 */
    LXB_URL_MAP_ALL, /* 0xb7 */
    LXB_URL_MAP_ALL, /* 0xb8 */
    LXB_URL_MAP_ALL, /* 0xb9 */
    LXB_URL_MAP_ALL, /* 0xba */
    LXB_URL_MAP_ALL, /* 0xbb */
    LXB_URL_MAP_ALL, /* 0xbc */
    LXB_URL_MAP_ALL, /* 0xbd */
    LXB_URL_MAP_ALL, /* 0xbe */
    LXB_URL_MAP_ALL, /* 0xbf */
    LXB_URL_MAP_ALL, /* 0xc0 */
    LXB_URL_MAP_ALL, /* 0xc1 */
    LXB_URL_MAP_ALL, /* 0xc2 */
    LXB_URL_MAP_ALL, /* 0xc3 */
    LXB_URL_MAP_ALL, /* 0xc4 */
    LXB_URL_MAP_ALL, /* 0xc5 */
    LXB_URL_MAP_ALL, /* 0xc6 */
    LXB_URL_MAP_ALL, /* 0xc7 */
    LXB_URL_MAP_ALL, /* 0xc8 */
    LXB_URL_MAP_ALL, /* 0xc9 */
    LXB_URL_MAP_ALL, /* 0xca */
    LXB_URL_MAP_ALL, /* 0xcb */
    LXB_URL_MAP_ALL, /* 0xcc */
    LXB_URL_MAP_ALL, /* 0xcd */
    LXB_URL_MAP_ALL, /* 0xce */
    LXB_URL_MAP_ALL, /* 0xcf */
    LXB_URL_MAP_ALL, /* 0xd0 */
    LXB_URL_MAP_ALL, /* 0xd1 */
    LXB_URL_MAP_ALL, /* 0xd2 */
    LXB_URL_MAP_ALL, /* 0xd3 */
    LXB_URL_MAP_ALL, /* 0xd4 */
    LXB_URL_MAP_ALL, /* 0xd5 */
    LXB_URL_MAP_ALL, /* 0xd6 */
    LXB_URL_MAP_ALL, /* 0xd7 */
    LXB_URL_MAP_ALL, /* 0xd8 */
    LXB_URL_MAP_ALL, /* 0xd9 */
    LXB_URL_MAP_ALL, /* 0xda */
    LXB_URL_MAP_ALL, /* 0xdb */
    LXB_URL_MAP_ALL, /* 0xdc */
    LXB_URL_MAP_ALL, /* 0xdd */
    LXB_URL_MAP_ALL, /* 0xde */
    LXB_URL_MAP_ALL, /* 0xdf */
    LXB_URL_MAP_ALL, /* 0xe0 */
    LXB_URL_MAP_ALL, /* 0xe1 */
    LXB_URL_MAP_ALL, /* 0xe2 */
    LXB_URL_MAP_ALL, /* 0xe3 */
    LXB_URL_MAP_ALL, /* 0xe4 */
    LXB_URL_MAP_ALL, /* 0xe5 */
    LXB_URL_MAP_ALL, /* 0xe6 */
    LXB_URL_MAP_ALL, /* 0xe7 */
    LXB_URL_MAP_ALL, /* 0xe8 */
    LXB_URL_MAP_ALL, /* 0xe9 */
    LXB_URL_MAP_ALL, /* 0xea */
    LXB_URL_MAP_ALL, /* 0xeb */
    LXB_URL_MAP_ALL, /* 0xec */
    LXB_URL_MAP_ALL, /* 0xed */
    LXB_URL_MAP_ALL, /* 0xee */
    LXB_URL_MAP_ALL, /* 0xef */
    LXB_URL_MAP_ALL, /* 0xf0 */
    LXB_URL_MAP_ALL, /* 0xf1 */
    LXB_URL_MAP_ALL, /* 0xf2 */
    LXB_URL_MAP_ALL, /* 0xf3 */
    LXB_URL_MAP_ALL, /* 0xf4 */
    LXB_URL_MAP_ALL, /* 0xf5 */
    LXB_URL_MAP_ALL, /* 0xf6 */
    LXB_URL_MAP_ALL, /* 0xf7 */
    LXB_URL_MAP_ALL, /* 0xf8 */
    LXB_URL_MAP_ALL, /* 0xf9 */
    LXB_URL_MAP_ALL, /* 0xfa */
    LXB_URL_MAP_ALL, /* 0xfb */
    LXB_URL_MAP_ALL, /* 0xfc */
    LXB_URL_MAP_ALL, /* 0xfd */
    LXB_URL_MAP_ALL, /* 0xfe */
    LXB_URL_MAP_ALL, /* 0xff */
};

/*
 * U+0000 NULL, U+0009 TAB, U+000A LF, U+000D CR, U+0020 SPACE, U+0023 (#),
 * U+002F (/), U+003A (:), U+003C (<), U+003E (>), U+003F (?), U+0040 (@),
 * U+005B ([), U+005C (\), U+005D (]), U+005E (^), or U+007C (|).
 * U+0000 NULL to U+001F, U+0025 (%), or U+007F DELETE.
 */
static const lxb_char_t lxb_url_map_forbidden_domain_cp[128] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d,
    0x1e, 0x1f, 0x20, 0xff, 0xff, 0x23, 0xff, 0x25, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3a, 0xff,
    0x3c, 0xff, 0x3e, 0x3f, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x5b, 0x5c, 0x5d, 0x5e, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x7c, 0xff, 0xff, 0x7f
};

/*
 * U+0000 NULL, U+0009 TAB, U+000A LF, U+000D CR, U+0020 SPACE, U+0023 (#),
 * U+002F (/), U+003A (:), U+003C (<), U+003E (>), U+003F (?), U+0040 (@),
 * U+005B ([), U+005C (\), U+005D (]), U+005E (^), or U+007C (|).
 */
static const lxb_char_t lxb_url_map_forbidden_host_cp[128] =
{
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x09,
    0x0a, 0xff, 0xff, 0x0d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0x20, 0xff, 0xff, 0x23, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x2f, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3a, 0xff,
    0x3c, 0xff, 0x3e, 0x3f, 0x40, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x5b, 0x5c, 0x5d, 0x5e, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0x7c, 0xff, 0xff, 0xff
};

static const lxb_char_t lxb_url_map_num_8[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05,
    0x06, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff
};

static const lxb_char_t lxb_url_codepoint_alphanumeric[0xA0] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x21, 0xff, 0xff, 0x24, 0xff, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3a, 0x3b, 0xff, 0x3d, 0xff, 0x3f,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5a, 0xff, 0xff, 0xff, 0xff, 0x5f,
    0xff, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0xff, 0xff, 0xff, 0x7e, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const lxb_char_t lxb_url_path_map[256] =
{
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x01, 0x00, 0x01, 0x01, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x01, 0x02, 0x02, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x01, 0x00, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02
};

static const lxb_url_scheme_data_t
lxb_url_scheme_res[LXB_URL_SCHEMEL_TYPE__LAST_ENTRY] =
{
    {.name = lexbor_str("#undef"),   0,   LXB_URL_SCHEMEL_TYPE__UNDEF  },
    {.name = lexbor_str("#unknown"), 0,   LXB_URL_SCHEMEL_TYPE__UNKNOWN},
    {.name = lexbor_str("http"),     80,  LXB_URL_SCHEMEL_TYPE_HTTP    },
    {.name = lexbor_str("https"),    443, LXB_URL_SCHEMEL_TYPE_HTTPS   },
    {.name = lexbor_str("ws"),       80,  LXB_URL_SCHEMEL_TYPE_WS      },
    {.name = lexbor_str("wss"),      443, LXB_URL_SCHEMEL_TYPE_WSS     },
    {.name = lexbor_str("ftp"),      21,  LXB_URL_SCHEMEL_TYPE_FTP     },
    {.name = lexbor_str("file"),     0,   LXB_URL_SCHEMEL_TYPE_FILE    }
};

static const size_t
lxb_url_scheme_length = sizeof(lxb_url_scheme_res) / sizeof(lxb_url_scheme_data_t);


#define lxb_url_parse_return(data, buf, status)                               \
    do {                                                                      \
        if ((buf) != (data)) {                                                \
            parser->buffer = (lxb_char_t *) (buf);                            \
        }                                                                     \
        return (status);                                                      \
    }                                                                         \
    while (false)

#define LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last)       \
    do {                                                                      \
        size_t new_len, offset, lst;                                          \
        lxb_char_t *tmp;                                                      \
                                                                              \
        lst = (last) - (sbuf_begin);                                          \
        offset = (sbuf) - (sbuf_begin);                                       \
        new_len = offset << 1;                                                \
                                                                              \
        if ((sbuf_begin) == (sbuffer)) {                                      \
            tmp = lexbor_malloc(new_len);                                     \
            if (tmp == NULL) {                                                \
                return NULL;                                                  \
            }                                                                 \
        }                                                                     \
        else {                                                                \
            tmp = lexbor_realloc((sbuf_begin), new_len);                      \
            if (tmp == NULL) {                                                \
                lexbor_free(sbuf_begin);                                      \
                return NULL;                                                  \
            }                                                                 \
        }                                                                     \
                                                                              \
        (sbuf) = tmp + offset;                                                \
        (last) = sbuf + lst;                                                  \
        (sbuf_begin) = tmp;                                                   \
        (sbuf_end) = tmp + new_len;                                           \
    }                                                                         \
    while (false)

#define lxb_url_is_windows_letter(data)                                       \
    (((data) >= 'a' && (data) <= 'z') || ((data) >= 'A' && (data) <= 'Z'))


static lxb_status_t
lxb_url_leading_trailing(lxb_url_parser_t *parser,
                         const lxb_char_t **data, size_t *length);

static const lxb_char_t *
lxb_url_remove_tab_newline(lxb_url_parser_t *parser,
                           const lxb_char_t *data, size_t *length);

static const lxb_url_scheme_data_t *
lxb_url_scheme_find(const lxb_char_t *data, size_t length);

static lxb_status_t
lxb_url_parse_basic_h(lxb_url_parser_t *parser, lxb_url_t *url,
                      const lxb_url_t *base_url,
                      const lxb_char_t *data, size_t length,
                      lxb_url_state_t override_state, lxb_encoding_t encoding);

static const lxb_char_t *
lxb_url_path_fast_path(lxb_url_parser_t *parser, lxb_url_t *url,
                       const lxb_char_t *data, const lxb_char_t *end, bool bqs);

const lxb_char_t *
lxb_url_path_slow_path(lxb_url_parser_t *parser, lxb_url_t *url,
                       const lxb_char_t *data, const lxb_char_t *end, bool bqs);

static lxb_status_t
lxb_url_path_try_dot(lxb_url_t *url, const lxb_char_t **begin,
                     const lxb_char_t **last, const lxb_char_t **start,
                     const lxb_char_t *end, bool bqs);

static const lxb_char_t *
lxb_url_path_dot_count(lxb_url_t *url, const lxb_char_t *p,
                       const lxb_char_t *end, const lxb_char_t *sbuf_begin,
                       lxb_char_t **sbuf, lxb_char_t **last, size_t *path_count,
                       bool bqs);

static void
lxb_url_path_fix_windows_drive(lxb_url_t *url, lxb_char_t *sbuf,
                               const lxb_char_t *last, size_t count);

static lxb_status_t
lxb_url_percent_encode_after_encoding(const lxb_char_t *data,
                                      const lxb_char_t *end, lexbor_str_t *str,
                                      lexbor_mraw_t *mraw,
                                      const lxb_encoding_data_t *encoding,
                                      lxb_url_map_type_t enmap,
                                      bool space_as_plus);

static lxb_status_t
lxb_url_percent_encode_after_utf_8(const lxb_char_t *data,
                                   const lxb_char_t *end, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw,
                                   lxb_url_map_type_t enmap,
                                   bool space_as_plus);

static lxb_status_t
lxb_url_host_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, lxb_url_host_t *host,
                   lexbor_mraw_t *mraw, lxb_url_host_opt_t opt);

static lxb_status_t
lxb_url_host_idna_cb(const lxb_char_t *data, size_t len, void *ctx);

static lxb_status_t
lxb_url_ipv4_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, uint32_t *ipv6);

static lxb_status_t
lxb_url_ipv4_number_parse(const lxb_char_t *data,
                          const lxb_char_t *end, uint64_t *num);

static bool
lxb_url_is_ipv4(lxb_url_parser_t *parser, const lxb_char_t *data,
                const lxb_char_t *end);

static lxb_status_t
lxb_url_ipv6_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, uint16_t *ipv6);

static lxb_status_t
lxb_url_ipv4_in_ipv6_parse(lxb_url_parser_t *parser, const lxb_char_t **data,
                           const lxb_char_t *end, uint16_t **pieces);

static lxb_status_t
lxb_url_opaque_host_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                          const lxb_char_t *end, lxb_url_host_t *host,
                          lexbor_mraw_t *mraw);

static lxb_status_t
lxb_url_percent_decode(const lxb_char_t *data, const lxb_char_t *end,
                       lexbor_str_t *str, lexbor_mraw_t *mraw,
                       lxb_url_host_opt_t *opt);

static lxb_status_t
lxb_url_percent_decode_plus(const lxb_char_t *data, const lxb_char_t *end,
                            lexbor_str_t *str, lexbor_mraw_t *mraw);

static const lxb_char_t *
lxb_url_path_part_by_index(const lxb_url_t *url, size_t index,
                           size_t *out_length);

static lxb_status_t
lxb_url_host_set_h(lxb_url_t *url, lxb_url_parser_t *parser,
                   const lxb_char_t *host, size_t length,
                   lxb_url_state_t override_state);

static lxb_status_t
lxb_url_search_params_parse(lxb_url_search_params_t *search_params,
                            const lxb_char_t *query, size_t length);


lxb_url_parser_t *
lxb_url_parser_create(void)
{
    return lexbor_calloc(1, sizeof(lxb_url_parser_t));
}

lxb_status_t
lxb_url_parser_init(lxb_url_parser_t *parser, lexbor_mraw_t *mraw)
{
    bool itmy;
    lxb_status_t status;

    if (parser == NULL) {
        return LXB_STATUS_ERROR_OBJECT_IS_NULL;
    }

    itmy = false;

    if (mraw == NULL) {
        mraw = lexbor_mraw_create();
        status = lexbor_mraw_init(mraw, LXB_URL_BUFFER_SIZE);
        if (status != LXB_STATUS_OK) {
            goto failed;
        }

        itmy = true;
    }

    parser->mraw = mraw;
    parser->log = NULL;
    parser->idna = NULL;
    parser->buffer = NULL;

    return LXB_STATUS_OK;

failed:

    if (itmy) {
        (void) lexbor_mraw_destroy(mraw, true);
    }

    memset(parser, 0x00, sizeof(lxb_url_parser_t));

    return status;
}

void
lxb_url_parser_clean(lxb_url_parser_t *parser)
{
    parser->url = NULL;

    if (parser->log != NULL) {
        lexbor_plog_clean(parser->log);
    }

    if (parser->buffer != NULL) {
        parser->buffer = lexbor_free(parser->buffer);
    }
}

lxb_url_parser_t *
lxb_url_parser_destroy(lxb_url_parser_t *parser, bool destroy_self)
{
    if (parser == NULL) {
        return NULL;
    }

    parser->log = lexbor_plog_destroy(parser->log, true);
    parser->idna = lxb_unicode_idna_destroy(parser->idna, true);

    if (parser->buffer != NULL) {
        parser->buffer = lexbor_free(parser->buffer);
    }

    if (destroy_self) {
        return lexbor_free(parser);
    }

    return parser;
}

void
lxb_url_parser_memory_destroy(lxb_url_parser_t *parser)
{
    parser->mraw = lexbor_mraw_destroy(parser->mraw, true);
}

static lxb_status_t
lxb_url_log_append(lxb_url_parser_t *parser, const lxb_char_t *pos,
                   lxb_url_error_type_t type)
{
    void *entry;
    lxb_status_t status;

    if (parser->log == NULL) {
        parser->log = lexbor_plog_create();
        status = lexbor_plog_init(parser->log, 5, sizeof(lexbor_plog_entry_t));
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    entry = lexbor_plog_push(parser->log, pos, NULL, type);
    if (entry == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_str_init(lexbor_str_t *str, lexbor_mraw_t *mraw, size_t length)
{
    size_t size;
    const lxb_char_t *p;

    if (str->data == NULL) {
        p = lexbor_str_init(str, mraw, length);
        if (p == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }
    else {
        size = str->length + length;

        if (size > lexbor_str_size(str)) {
            p = lexbor_str_realloc(str, mraw, size);
            if (p == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }
        }
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_str_copy(const lexbor_str_t *src, lexbor_str_t *dst,
                 lexbor_mraw_t *dst_mraw)
{
    if (src->data == NULL) {
        *dst = *src;
        return LXB_STATUS_OK;
    }

    if (dst->data != NULL) {
        if (dst->length >= src->length) {

            /* +1 == '\0' */
            memcpy(dst->data, src->data, src->length + 1);

            return LXB_STATUS_OK;
        }

        (void) lexbor_str_destroy(dst, dst_mraw, false);
    }

    (void) lexbor_str_init_append(dst, dst_mraw,
                                  src->data, src->length);

    return (dst->data != NULL) ? LXB_STATUS_OK
    : LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}

lxb_inline bool
lxb_url_is_noncharacter(lxb_codepoint_t cp)
{
    if (cp >= 0xFDD0 && cp <= 0xFDEF) {
        return true;
    }

    switch (cp) {
        case 0xFFFE:
        case 0xFFFF:
        case 0x1FFFE:
        case 0x1FFFF:
        case 0x2FFFE:
        case 0x2FFFF:
        case 0x3FFFE:
        case 0x3FFFF:
        case 0x4FFFE:
        case 0x4FFFF:
        case 0x5FFFE:
        case 0x5FFFF:
        case 0x6FFFE:
        case 0x6FFFF:
        case 0x7FFFE:
        case 0x7FFFF:
        case 0x8FFFE:
        case 0x8FFFF:
        case 0x9FFFE:
        case 0x9FFFF:
        case 0xAFFFE:
        case 0xAFFFF:
        case 0xBFFFE:
        case 0xBFFFF:
        case 0xCFFFE:
        case 0xCFFFF:
        case 0xDFFFE:
        case 0xDFFFF:
        case 0xEFFFE:
        case 0xEFFFF:
        case 0xFFFFE:
        case 0xFFFFF:
        case 0x10FFFE:
        case 0x10FFFF:
            return true;

        default:
            break;
    }

    return false;
}

lxb_inline bool
lxb_url_is_url_codepoint(lxb_codepoint_t cp)
{
    if (cp >= 0x00A0 && cp <= 0x1FFFFF) {
        /* Leading and trailing surrogate. */
        if ((cp >= 0xD800 && cp <= 0xDFFF)) {
            return false;
        }

        return !(cp > 0x10FFFF || lxb_url_is_noncharacter(cp));
    }

    return lxb_url_codepoint_alphanumeric[(lxb_char_t) cp] != 0xFF;
}

lxb_inline bool
lxb_url_is_special(const lxb_url_t *url)
{
    return url->scheme.type != LXB_URL_SCHEMEL_TYPE__UNKNOWN;
}

lxb_inline const lxb_url_scheme_data_t *
lxb_url_scheme_by_type(lxb_url_scheme_type_t type)
{
    return &lxb_url_scheme_res[type];
}

lxb_inline bool
lxb_url_scheme_is_special(const lxb_url_scheme_data_t *scheme)
{
    return scheme->type != LXB_URL_SCHEMEL_TYPE__UNKNOWN;
}

lxb_inline bool
lxb_url_scheme_is_equal(const lxb_url_scheme_t *first,
                        const lxb_url_scheme_t *second)
{
    if (first->type != second->type) {
        return false;
    }

    if (first->type == LXB_URL_SCHEMEL_TYPE__UNKNOWN) {
        if (first->name.length != second->name.length) {
            return false;
        }

        return memcmp(first->name.data, second->name.data,
                      first->name.length) == 0;
    }

    return true;
}

lxb_inline bool
lxb_url_scheme_equal_port(lxb_url_scheme_type_t type, uint16_t port)
{
    return lxb_url_scheme_res[type].port == port;
}

static lxb_status_t
lxb_url_scheme_copy(const lxb_url_scheme_t *src, lxb_url_scheme_t *dst,
                    lexbor_mraw_t *dst_mraw)
{
    dst->type = src->type;

    return lxb_url_str_copy(&src->name, &dst->name, dst_mraw);
}

static lxb_status_t
lxb_url_scheme_copy_special(const lxb_url_scheme_data_t *src,
                            lxb_url_scheme_t *dst, lexbor_mraw_t *dst_mraw)
{
    dst->type = src->type;

    return lxb_url_str_copy(&src->name, &dst->name, dst_mraw);
}

static void
lxb_url_path_set_null(lxb_url_t *url)
{
    if (url->path.str.data == NULL) {
        return;
    }

    (void) lexbor_str_destroy(&url->path.str, url->mraw, false);

    url->path.str.length = 0;
    url->path.length = 0;
    url->path.opaque = false;
}

static lxb_status_t
lxb_url_path_copy(const lxb_url_t *src, lxb_url_t *dst)
{
    lexbor_str_t *to;
    const lexbor_str_t *str;

    if (dst->path.str.data != NULL) {
        lxb_url_path_set_null(dst);
    }

    dst->path.opaque = src->path.opaque;

    if (src->path.str.data == NULL) {
        return LXB_STATUS_OK;
    }

    dst->path.length = src->path.length;

    str = &src->path.str;
    to = &dst->path.str;

    to->data = lexbor_mraw_alloc(dst->mraw, (str->length + 1));
    if (to->data == NULL) {
        return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
    }

    /* +1 == '\0' */
    memcpy(to->data, str->data, str->length + 1);

    to->length = str->length;

    return LXB_STATUS_OK;
}

static void
lxb_url_path_shorten(lxb_url_t *url)
{
    lexbor_str_t *str;
    lxb_char_t *p, *begin;

    str = &url->path.str;

    if (url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE
        && url->path.length == 1)
    {
        if (str->length >= 3
            && lxb_url_is_windows_letter(str->data[1])
            && str->data[2] == ':')
        {
            return;
        }
    }

    if (url->path.str.data != NULL) {
        url->path.length -= 1;

        begin = str->data;
        p = begin + str->length;

        while (p > begin) {
            p -= 1;

            if (*p == '/') {
                *p = '\0';
                break;
            }
        }

        str->length = p - begin;
    }
}

static lxb_status_t
lxb_url_path_append_wo_slash(lxb_url_t *url,
                             const lxb_char_t *data, size_t length)
{
    lxb_char_t *p;

    if (url->path.str.data == NULL) {
        p = lexbor_str_init(&url->path.str, url->mraw, length);
        if (p == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    p = lexbor_str_append(&url->path.str, url->mraw, data, length);

    return (p != NULL) ? LXB_STATUS_OK : LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}

static lxb_status_t
lxb_url_path_append(lxb_url_t *url, const lxb_char_t *data, size_t length)
{
    size_t len;
    lxb_char_t *p;
    lexbor_str_t *str;

    str = &url->path.str;

    if (str->data == NULL) {
        p = lexbor_str_init(str, url->mraw, length + 1);
        if (p == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    len = str->length;
    str->length += 1;

    p = lexbor_str_append(&url->path.str, url->mraw, data, length);

    str->data[len] = '/';

    return (p != NULL) ? LXB_STATUS_OK : LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}

static lxb_status_t
lxb_url_path_list_push(lxb_url_t *url, lexbor_str_t *data)
{
    return lxb_url_path_append_wo_slash(url, data->data, data->length);
}

lxb_inline lxb_status_t
lxb_url_query_copy(const lexbor_str_t *src, lexbor_str_t *dst,
                   lexbor_mraw_t *dst_mraw)
{
    return lxb_url_str_copy(src, dst, dst_mraw);
}

lxb_inline lxb_status_t
lxb_url_username_copy(const lexbor_str_t *src, lexbor_str_t *dst,
                      lexbor_mraw_t *dst_mraw)
{
    return lxb_url_str_copy(src, dst, dst_mraw);
}

lxb_inline lxb_status_t
lxb_url_password_copy(const lexbor_str_t *src, lexbor_str_t *dst,
                      lexbor_mraw_t *dst_mraw)
{
    return lxb_url_str_copy(src, dst, dst_mraw);
}

static lxb_status_t
lxb_url_host_copy(const lxb_url_host_t *src, lxb_url_host_t *dst,
                  lexbor_mraw_t *dst_mraw)
{
    if (dst->type != LXB_URL_HOST_TYPE__UNDEF) {
        if (src->type == LXB_URL_HOST_TYPE__UNDEF) {
            if (dst->type <= LXB_URL_HOST_TYPE_OPAQUE) {
                (void) lexbor_str_destroy(&dst->u.domain, dst_mraw, false);
            }

            dst->type = LXB_URL_HOST_TYPE__UNDEF;

            return LXB_STATUS_OK;
        }

        if (dst->type <= LXB_URL_HOST_TYPE_OPAQUE) {
            if (src->type <= LXB_URL_HOST_TYPE_OPAQUE) {
                dst->type = src->type;

                return lxb_url_str_copy(&src->u.domain,
                                        &dst->u.domain, dst_mraw);
            }

            (void) lexbor_str_destroy(&dst->u.domain, dst_mraw, false);
        }
    }

    if (src->type <= LXB_URL_HOST_TYPE_OPAQUE) {
        dst->type = src->type;

        if (src->type == LXB_URL_HOST_TYPE__UNDEF) {
            return LXB_STATUS_OK;
        }

        return lxb_url_str_copy(&src->u.domain,
                                &dst->u.domain, dst_mraw);
    }

    if (src->type == LXB_URL_HOST_TYPE_IPV6) {
        memcpy(dst->u.ipv6, src->u.ipv6, sizeof(src->u.ipv6));
    }
    else {
        dst->u.ipv4 = src->u.ipv4;
    }

    return LXB_STATUS_OK;
}

lxb_inline void
lxb_url_host_destroy(lxb_url_host_t *host, lexbor_mraw_t *mraw)
{
    if (host->type != LXB_URL_HOST_TYPE__UNDEF) {
        if (host->type <= LXB_URL_HOST_TYPE_OPAQUE) {
            (void) lexbor_str_destroy(&host->u.domain, mraw, false);
        }
    }
}

static void
lxb_url_host_set_empty(lxb_url_host_t *host, lexbor_mraw_t *mraw)
{
    lxb_url_host_destroy(host, mraw);

    host->type = LXB_URL_HOST_TYPE_EMPTY;
}

static bool
lxb_url_host_eq(lxb_url_host_t *host, const lxb_char_t *data, size_t length)
{
    lexbor_str_t *str;

    if (host->type != LXB_URL_HOST_TYPE__UNDEF) {
        if (host->type <= LXB_URL_HOST_TYPE_OPAQUE) {
            str = &host->u.domain;

            return str->length == length
                   && memcmp(data, str->data, length) == 0;
        }
    }

    return false;
}

lxb_inline void
lxb_url_port_set(lxb_url_t *url, uint16_t port)
{
    url->port = port;
    url->has_port = true;
}

static void
lxb_url_fragment_set_null(lxb_url_t *url)
{
    if (url->fragment.data != NULL) {
        (void) lexbor_str_destroy(&url->fragment, url->mraw, false);
    }
}

lxb_inline bool
lxb_url_includes_credentials(const lxb_url_t *url)
{
    return url->username.length != 0 || url->password.length != 0;
}

lxb_inline void
lxb_url_encoding_init(const lxb_encoding_data_t *encoding,
                      lxb_encoding_encode_t *encode)
{
    (void) lxb_encoding_encode_init_single(encode, encoding);
}

static bool
lxb_url_start_windows_drive_letter(const lxb_char_t *data,
                                   const lxb_char_t *end)
{
    size_t length = end - data;

    if (length < 2) {
        return false;
    }

    if (!lxb_url_is_windows_letter(data[0])
        || (data[1] != ':' && data[1] != '|'))
    {
        return false;
    }

    if (length > 2 && !(   data[2] == '/' || data[2] == '\\'
                        || data[2] == '?' || data[2] == '#'))
    {
        return false;
    }

    return true;
}

static bool
lxb_url_windows_drive_letter(const lxb_char_t *data, const lxb_char_t *end)
{
    size_t length = end - data;

    if (length < 2) {
        return false;
    }

    return lxb_url_is_windows_letter(data[0])
           && (data[1] == ':' || data[1] == '|');
}

static bool
lxb_url_normalized_windows_drive_letter(const lxb_char_t *data,
                                        const lxb_char_t *end)
{
    size_t length = end - data;

    if (length < 2) {
        return false;
    }

    return lxb_url_is_windows_letter(data[0]) && data[1] == ':';
}

static bool
lxb_url_cannot_have_user_pass_port(lxb_url_t *url)
{
    return url->host.type == LXB_URL_HOST_TYPE_EMPTY
    || url->host.type == LXB_URL_HOST_TYPE__UNDEF
    || url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE;
}

lxb_url_t *
lxb_url_parse(lxb_url_parser_t *parser, const lxb_url_t *base_url,
              const lxb_char_t *data, size_t length)
{
    (void) lxb_url_parse_basic(parser, NULL, base_url, data, length,
                               LXB_URL_STATE__UNDEF, LXB_ENCODING_AUTO);
    return parser->url;
}

lxb_status_t
lxb_url_parse_basic(lxb_url_parser_t *parser, lxb_url_t *url,
                    const lxb_url_t *base_url,
                    const lxb_char_t *data, size_t length,
                    lxb_url_state_t override_state, lxb_encoding_t encoding)
{
    lxb_status_t status;

    status = lxb_url_parse_basic_h(parser, url, base_url, data,
                                   length, override_state, encoding);
    if (status != LXB_STATUS_OK) {
        if (parser->url != url) {
            parser->url = lxb_url_destroy(parser->url);
        }

        return status;
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_parse_basic_h(lxb_url_parser_t *parser, lxb_url_t *url,
                      const lxb_url_t *base_url,
                      const lxb_char_t *data, size_t length,
                      lxb_url_state_t override_state, lxb_encoding_t encoding)
{
    bool at_sign, inside_bracket;
    size_t len;
    uint32_t port;
    lxb_status_t status;
    lexbor_str_t tmp_str;
    lxb_url_state_t state;
    const lxb_char_t *p, *begin, *end, *tmp, *pswd, *buf, *orig_data;
    lxb_char_t c;
    lxb_codepoint_t cp;
    lxb_url_map_type_t map_type;
    const lxb_url_scheme_data_t *schm;
    const lxb_encoding_data_t *enc;
    lxb_url_host_opt_t opt;

    static const lexbor_str_t mp_str = lexbor_str("");
    static const lexbor_str_t lh_str = lexbor_str("localhost");

    if (url == NULL) {
        url = lexbor_mraw_calloc(parser->mraw, sizeof(lxb_url_t));
        if (url == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        url->mraw = parser->mraw;

        status = lxb_url_leading_trailing(parser, &data, &length);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    parser->url = url;
    orig_data = data;

    buf = lxb_url_remove_tab_newline(parser, data, &length);
    if (buf != data) {
        if (buf == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }

        data = buf;
    }

    state = LXB_URL_STATE_SCHEME_START_STATE;

    if (override_state != LXB_URL_STATE__UNDEF) {
        state = override_state;
    }

    if (encoding <= LXB_ENCODING_UNDEFINED
        || encoding == LXB_ENCODING_UTF_16BE
        || encoding == LXB_ENCODING_UTF_16LE)
    {
        encoding = LXB_ENCODING_UTF_8;
    }

    enc = lxb_encoding_data(encoding);
    if (enc == NULL) {
        lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_WRONG_ARGS);
    }

    inside_bracket = false;

    p = data;
    end = data + length;

    /* And go. */

    schm = lxb_url_scheme_by_type(LXB_URL_SCHEMEL_TYPE__UNDEF);

again:

    switch (state) {
    case LXB_URL_STATE_SCHEME_START_STATE:
        if (p >= end || lexbor_str_res_alpha_character[*p] == 0xff) {
            if (override_state == LXB_URL_STATE__UNDEF) {
                state = LXB_URL_STATE_NO_SCHEME_STATE;
                goto again;
            }

            lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);
        }

        /* Fall through. */

    case LXB_URL_STATE_SCHEME_STATE:
        do {
            p++;
        }
        while (p < end && (lexbor_str_res_alphanumeric_character[*p] != 0xff
                           || *p == '+' || *p == '-' || *p == '.'));

        if (p >= end || *p != ':') {
            if (override_state == LXB_URL_STATE__UNDEF) {
                p = data;

                state = LXB_URL_STATE_NO_SCHEME_STATE;
                goto again;
            }
            else if (p < end || override_state != LXB_URL_STATE_SCHEME_START_STATE) {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);
            }
        }

        schm = lxb_url_scheme_find(data, p - data);

        if (override_state != LXB_URL_STATE__UNDEF) {
            if (lxb_url_is_special(url) != lxb_url_scheme_is_special(schm)) {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
            }

            if (url->has_port || lxb_url_includes_credentials(url)) {
                if (schm->type == LXB_URL_SCHEMEL_TYPE_FILE) {
                    lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
                }
            }

            if (url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE
                && url->host.type == LXB_URL_HOST_TYPE_EMPTY)
            {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
            }
        }

        url->scheme.type = schm->type;
        url->scheme.name.length = 0;

        status = lxb_url_str_init(&url->scheme.name, url->mraw, p - data);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_MEMORY_ALLOCATION);
        }

        (void) lexbor_str_append_lowercase(&url->scheme.name, url->mraw,
                                           data, p - data);

        p += 1;

        if (override_state != LXB_URL_STATE__UNDEF) {
            if (url->has_port && url->port == schm->port) {
                url->port = 0;
                url->has_port = false;
            }

            lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
        }

        if (schm->type == LXB_URL_SCHEMEL_TYPE_FILE) {
            if (end - p < 2 || p[0] != '/' || p[1] != '/') {
                status = lxb_url_log_append(parser, p,
                  LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }

            state = LXB_URL_STATE_FILE_STATE;
            goto again;
        }

        if (lxb_url_scheme_is_special(schm)) {
            if (base_url != NULL
                && lxb_url_scheme_is_equal(&url->scheme, &base_url->scheme))
            {
                state = LXB_URL_STATE_SPECIAL_RELATIVE_OR_AUTHORITY_STATE;
            }
            else {
                state = LXB_URL_STATE_SPECIAL_AUTHORITY_SLASHES_STATE;
            }

            goto again;
        }

        if (p < end && *p == '/') {
            p += 1;
            state = LXB_URL_STATE_PATH_OR_AUTHORITY_STATE;
            goto again;
        }

        lxb_url_path_set_null(url);

        state = LXB_URL_STATE_OPAQUE_PATH_STATE;
        goto again;

    case LXB_URL_STATE_NO_SCHEME_STATE:
        if (base_url == NULL) {
            goto failed_non_relative_url;
        }

        if (base_url->path.opaque) {
            if (p >= end || *p != '#') {
                goto failed_non_relative_url;
            }

            p += 1;

            status = lxb_url_scheme_copy(&base_url->scheme,
                                         &url->scheme, url->mraw);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            status = lxb_url_path_copy(base_url, url);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            status = lxb_url_query_copy(&base_url->query, &url->query,
                                        url->mraw);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            lxb_url_fragment_set_null(url);

            state = LXB_URL_STATE_FRAGMENT_STATE;
            goto again;
        }

        if (base_url->scheme.type != LXB_URL_SCHEMEL_TYPE_FILE) {
            state = LXB_URL_STATE_RELATIVE_STATE;
            goto again;
        }

        state = LXB_URL_STATE_FILE_STATE;
        goto again;

    case LXB_URL_STATE_SPECIAL_RELATIVE_OR_AUTHORITY_STATE:
        if (end - p > 1 && p[0] == '/' && p[1] == '/') {
            p += 2;
            state = LXB_URL_STATE_SPECIAL_AUTHORITY_IGNORE_SLASHES_STATE;
            goto again;
        }

        status = lxb_url_log_append(parser, p,
                  LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        state = LXB_URL_STATE_RELATIVE_STATE;
        goto again;

    case LXB_URL_STATE_PATH_OR_AUTHORITY_STATE:
        if (p < end && *p == '/') {
            p += 1;
            state = LXB_URL_STATE_AUTHORITY_STATE;
        }
        else {
            state = LXB_URL_STATE_PATH_STATE;
        }

        goto again;

    case LXB_URL_STATE_RELATIVE_STATE:
        status = lxb_url_scheme_copy(&base_url->scheme, &url->scheme, url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        if (end - p >= 1) {
            if (*p == '/') {
                p += 1;
                state = LXB_URL_STATE_RELATIVE_SLASH_STATE;
                goto again;
            }

            if (lxb_url_is_special(url) && *p == '\\') {
                p += 1;

                status = lxb_url_log_append(parser, p,
                                   LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                state = LXB_URL_STATE_RELATIVE_SLASH_STATE;
                goto again;
            }
        }

        status = lxb_url_username_copy(&base_url->username, &url->username,
                                       url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        status = lxb_url_password_copy(&base_url->password, &url->password,
                                       url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        status = lxb_url_host_copy(&base_url->host, &url->host, url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        if (base_url->has_port) {
            lxb_url_port_set(url, base_url->port);
        }

        status = lxb_url_path_copy(base_url, url);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        status = lxb_url_query_copy(&base_url->query, &url->query, url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        if (end - p == 0) {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
        }

        if (*p == '?') {
            p += 1;

            (void) lexbor_str_destroy(&url->query, url->mraw, false);

            state = LXB_URL_STATE_QUERY_STATE;
            goto again;
        }

        if (*p == '#') {
            p += 1;

            (void) lexbor_str_destroy(&url->fragment, url->mraw, false);

            state = LXB_URL_STATE_FRAGMENT_STATE;
            goto again;
        }

        (void) lexbor_str_destroy(&url->query, url->mraw, false);

        lxb_url_path_shorten(url);

        state = LXB_URL_STATE_PATH_STATE;
        goto again;

    case LXB_URL_STATE_RELATIVE_SLASH_STATE:
        c = (end - p >= 1) ? *p : '\0';

        if (lxb_url_is_special(url) && (c == '/' || c == '\\')) {
            if (c == '\\') {
                status = lxb_url_log_append(parser, p,
                                   LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }

            p += 1;
            state = LXB_URL_STATE_SPECIAL_AUTHORITY_IGNORE_SLASHES_STATE;
            goto again;
        }

        if (c == '/') {
            p += 1;
            state = LXB_URL_STATE_AUTHORITY_STATE;
            goto again;
        }

        status = lxb_url_username_copy(&base_url->username, &url->username,
                                       url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        status = lxb_url_password_copy(&base_url->password, &url->password,
                                       url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        status = lxb_url_host_copy(&base_url->host, &url->host, url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        if (base_url->has_port) {
            lxb_url_port_set(url, base_url->port);
        }

        state = LXB_URL_STATE_PATH_STATE;

        goto again;

    case LXB_URL_STATE_SPECIAL_AUTHORITY_SLASHES_STATE:
        state = LXB_URL_STATE_SPECIAL_AUTHORITY_IGNORE_SLASHES_STATE;

        if (end - p > 1 && p[0] == '/' && p[1] == '/') {
            p += 2;
        }
        else {
            status = lxb_url_log_append(parser, p,
                  LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }
        }

        goto again;

    case LXB_URL_STATE_SPECIAL_AUTHORITY_IGNORE_SLASHES_STATE:
        if (p >= end || (*p != '/' && *p != '\\')) {
            state = LXB_URL_STATE_AUTHORITY_STATE;
            goto again;
        }

        status = lxb_url_log_append(parser, p,
                  LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        p += 1;
        goto again;

    case LXB_URL_STATE_AUTHORITY_STATE:
        begin = p;
        pswd = NULL;
        at_sign = false;

        while (p < end) {
            c = *p;

            switch (c) {
                case '@':
                    status = lxb_url_log_append(parser, p,
                                                LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS);
                    if (status != LXB_STATUS_OK) {
                        lxb_url_parse_return(orig_data, buf, status);
                    }

                    if (p == begin) {
                        at_sign = true;
                        break;
                    }

                    if (pswd == NULL || !at_sign) {
                        tmp = (pswd != NULL) ? pswd - 1 : p;

                        if (tmp > begin) {
                            status = lxb_url_percent_encode_after_utf_8(begin, tmp,
                                                        &url->username, url->mraw,
                                                        LXB_URL_MAP_USERINFO, false);
                            if (status != LXB_STATUS_OK) {
                                lxb_url_parse_return(orig_data, buf, status);
                            }
                        }
                    }

                    if (pswd != NULL && p > pswd) {
                        status = lxb_url_percent_encode_after_utf_8(pswd, p,
                                                    &url->password, url->mraw,
                                                    LXB_URL_MAP_USERINFO, false);
                        if (status != LXB_STATUS_OK) {
                            lxb_url_parse_return(orig_data, buf, status);
                        }

                        pswd = p;
                    }

                    at_sign = true;
                    begin = p;

                    break;

                case ':':
                    if (pswd == NULL) {
                        pswd = p + 1;
                    }

                    break;

                case '/':
                case '\\':
                case '?':
                case '#':
                    if (c == '\\') {
                        if (!lxb_url_is_special(url)) {
                            p += 1;
                            continue;
                        }
                    }

                    goto authority_done;

                default:
                    break;
            }

            p += 1;
        }

    authority_done:

        if (at_sign) {
            if (begin == p || begin == p - 1) {
                status = lxb_url_log_append(parser, p,
                                            LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);
            }

            /* Skip '@'. */
            begin += 1;
        }

        p = begin;

        state = LXB_URL_STATE_HOST_STATE;
        goto again;

    case LXB_URL_STATE_HOST_STATE:
    case LXB_URL_STATE_HOSTNAME_STATE:
        begin = p;

        if (override_state != LXB_URL_STATE__UNDEF
            && url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE)
        {
            p -= 1;
            state = LXB_URL_STATE_FILE_HOST_STATE;
            goto again;
        }

        opt = !lxb_url_is_special(url);

        for (; p < end; p++) {
            c = *p;

            switch (c) {
                case '/':
                case '?':
                case '#':
                    goto host_done;

                case '\\':
                    if (!lxb_url_is_special(url)) {
                        break;
                    }

                    goto host_done;

                case ':':
                    if (inside_bracket) {
                        break;
                    }

                    if (p == begin) {
                        goto failed_host;
                    }

                    if (override_state == LXB_URL_STATE_HOSTNAME_STATE) {
                        lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR);
                    }

                    status = lxb_url_host_parse(parser, begin, p, &url->host,
                                                url->mraw, opt);
                    if (status != LXB_STATUS_OK) {
                        lxb_url_parse_return(orig_data, buf, status);
                    }

                    p += 1;

                    state = LXB_URL_STATE_PORT_STATE;
                    goto again;

                case '[':
                    inside_bracket = true;
                    break;

                case ']':
                    inside_bracket = false;
                    break;

                case '%':
                    opt |= LXB_URL_HOST_OPT_DECODE;
                    break;

                case 'X':
                case 'x':
                    if (p + 4 <= end && (p[1] == 'n' || p[1] == 'N')
                        && p[2] == '-' && p[3] == '-')
                    {
                        opt |= LXB_URL_HOST_OPT_IDNA;
                        p += 3;
                    }

                    break;

                default:
                    if (c >= 0x80) {
                        opt |= LXB_URL_HOST_OPT_IDNA;
                    }

                    break;
            }
        }

        host_done:

        if (begin == p && lxb_url_is_special(url)) {
            goto failed_host;
        }

        if (override_state != LXB_URL_STATE__UNDEF && begin == p
            && (lxb_url_includes_credentials(url) || url->has_port))
        {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
        }

        status = lxb_url_host_parse(parser, begin, p, &url->host,
                                    url->mraw, opt);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        if (override_state != LXB_URL_STATE__UNDEF) {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
        }

        state = LXB_URL_STATE_PATH_START_STATE;
        goto again;

    case LXB_URL_STATE_PORT_STATE:
        begin = p;

        while (true) {
            c = (p < end) ? *p : '\0';

            if (c >= '0' && c <= '9') {
                p += 1;
                continue;
            }

            if (p >= end || c == '/' || c == '?' || c == '#'
                || (lxb_url_is_special(url) && c == '\\')
                || override_state != LXB_URL_STATE__UNDEF)
            {
                if (begin == p) {
                    if (override_state != LXB_URL_STATE__UNDEF) {
                        lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR);
                    }

                    state = LXB_URL_STATE_PATH_START_STATE;
                    goto again;
                }

                port = 0;

                while (begin < p) {
                    port = lexbor_str_res_map_num[*begin++] + port * 10;

                    if (port > 65535) {
                        status = lxb_url_log_append(parser, p,
                                          LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE);
                        if (status != LXB_STATUS_OK) {
                            lxb_url_parse_return(orig_data, buf, status);
                        }

                        lxb_url_parse_return(orig_data, buf,
                                             LXB_STATUS_ERROR_UNEXPECTED_DATA);
                    }
                }

                if (!lxb_url_is_special(url)
                    || !lxb_url_scheme_equal_port(url->scheme.type, port))
                {
                    url->port = port;
                    url->has_port = true;
                }
                else {
                    url->port = 0;
                    url->has_port = false;
                }

                if (override_state != LXB_URL_STATE__UNDEF) {
                    lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
                }

                state = LXB_URL_STATE_PATH_START_STATE;
                goto again;
            }

            status = lxb_url_log_append(parser, p,
                                        LXB_URL_ERROR_TYPE_PORT_INVALID);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);
        }

        break;

    case LXB_URL_STATE_FILE_STATE:
        schm = lxb_url_scheme_by_type(LXB_URL_SCHEMEL_TYPE_FILE);

        status = lxb_url_scheme_copy_special(schm, &url->scheme, url->mraw);
        if (status != LXB_STATUS_OK) {
            lxb_url_parse_return(orig_data, buf, status);
        }

        lxb_url_host_set_empty(&url->host, url->mraw);

        c = (p < end) ? *p : '\0';

        if (c == '/' || c == '\\') {
            if (c == '\\') {
                status = lxb_url_log_append(parser, p,
                                   LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }

            p += 1;

            state = LXB_URL_STATE_FILE_SLASH_STATE;
            goto again;
        }

        if (base_url != NULL
            && base_url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE)
        {
            status = lxb_url_host_copy(&base_url->host, &url->host, url->mraw);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            status = lxb_url_path_copy(base_url, url);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            status = lxb_url_query_copy(&base_url->query, &url->query, url->mraw);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            if (c == '?') {
                p += 1;

                (void) lexbor_str_destroy(&url->query, url->mraw, false);

                state = LXB_URL_STATE_QUERY_STATE;
                goto again;
            }

            if (c == '#') {
                p += 1;

                (void) lexbor_str_destroy(&url->fragment, url->mraw, false);

                state = LXB_URL_STATE_FRAGMENT_STATE;
                goto again;
            }

            if (p >= end) {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
            }

            (void) lexbor_str_destroy(&url->query, url->mraw, false);

            if (!lxb_url_start_windows_drive_letter(p, end)) {
                lxb_url_path_shorten(url);
            }
            else {
                status = lxb_url_log_append(parser, p,
                          LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                lxb_url_path_set_null(url);
                url->path.opaque = true;
            }
        }

        state = LXB_URL_STATE_PATH_STATE;
        goto again;

    case LXB_URL_STATE_FILE_SLASH_STATE:
        c = (p < end) ? *p : '\0';

        if (c == '/' || c == '\\') {
            if (c == '\\') {
                status = lxb_url_log_append(parser, p,
                                   LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }

            p += 1;

            state = LXB_URL_STATE_FILE_HOST_STATE;
            goto again;
        }

        if (base_url != NULL
            && base_url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE)
        {
            status = lxb_url_host_copy(&base_url->host, &url->host, url->mraw);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            if (!lxb_url_start_windows_drive_letter(p, end)
                && !base_url->path.opaque && base_url->path.length >= 1)
            {
                tmp = lxb_url_path_part_by_index(base_url,
                                       base_url->path.str.data[0] == '/', &len);

                if (tmp != NULL
                    && lxb_url_normalized_windows_drive_letter(tmp, tmp + len))
                {
                    len = (tmp + len) - base_url->path.str.data;

                    status = lxb_url_path_append_wo_slash(url,
                                                  base_url->path.str.data, len);
                    if (status != LXB_STATUS_OK) {
                        lxb_url_parse_return(orig_data, buf, status);
                    }
                }
            }
        }

        state = LXB_URL_STATE_PATH_STATE;
        goto again;

    case LXB_URL_STATE_FILE_HOST_STATE:
        begin = p;
        opt = !lxb_url_is_special(url);

        while (true) {
            if (p < end) {
                switch (*p) {
                    case '/':
                    case '\\':
                    case '?':
                    case '#':
                        break;

                    case '%':
                        p += 1;
                        opt |= LXB_URL_HOST_OPT_DECODE;
                        continue;

                    case 'X':
                    case 'x':
                        if (p + 4 <= end && (p[1] == 'n' || p[1] == 'N')
                            && p[2] == '-' && p[3] == '-')
                        {
                            opt |= LXB_URL_HOST_OPT_IDNA;
                            p += 3;
                        }

                        p += 1;
                        continue;

                    default:
                        if (*p >= 0x80) {
                            opt |= LXB_URL_HOST_OPT_IDNA;
                        }

                        p += 1;
                        continue;
                }
            }

            if (override_state == LXB_URL_STATE__UNDEF && p - begin == 2
                && lxb_url_windows_drive_letter(begin, p))
            {
                status = lxb_url_log_append(parser, begin,
                    LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                p = begin;

                state = LXB_URL_STATE_PATH_STATE;
                goto again;
            }

            if (p == begin) {
                lxb_url_host_set_empty(&url->host, url->mraw);

                if (override_state != LXB_URL_STATE__UNDEF) {
                    lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
                }

                state = LXB_URL_STATE_PATH_START_STATE;
                goto again;
            }

            status = lxb_url_host_parse(parser, begin, p, &url->host,
                                        url->mraw, opt);
            if (status != LXB_STATUS_OK) {
                lxb_url_parse_return(orig_data, buf, status);
            }

            if (lxb_url_host_eq(&url->host, lh_str.data, lh_str.length)) {
                lxb_url_host_set_empty(&url->host, url->mraw);
            }

            if (override_state != LXB_URL_STATE__UNDEF) {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
            }

            state = LXB_URL_STATE_PATH_START_STATE;
            goto again;
        }

        break;

    case LXB_URL_STATE_PATH_START_STATE:
        c = (p < end) ? *p : '\0';

        if (lxb_url_is_special(url)) {
            if (c == '\\') {
                status = lxb_url_log_append(parser, p,
                                   LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                p += 1;
            }
            else if (c == '/') {
                p += 1;
            }

            state = LXB_URL_STATE_PATH_STATE;
            goto again;
        }

        if (override_state == LXB_URL_STATE__UNDEF) {
            if (c == '?') {
                p += 1;
                state = LXB_URL_STATE_QUERY_STATE;
                goto again;
            }

            if (c == '#') {
                p += 1;
                state = LXB_URL_STATE_FRAGMENT_STATE;
                goto again;
            }
        }

        if (p < end) {
            if (c == '/') {
                p += 1;
            }

            state = LXB_URL_STATE_PATH_STATE;
            goto again;
        }

        if (override_state != LXB_URL_STATE__UNDEF
            && url->host.type == LXB_URL_HOST_TYPE__UNDEF)
        {
            status = lxb_url_path_append(url, mp_str.data, mp_str.length);
            lxb_url_parse_return(orig_data, buf, status);
        }

        lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);

    case LXB_URL_STATE_PATH_STATE:
        p = lxb_url_path_fast_path(parser, url, p, end,
                                   override_state == LXB_URL_STATE__UNDEF);
        if (p == NULL) {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_MEMORY_ALLOCATION);
        }

        if (p >= end) {
            lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
        }

        switch (*p) {
            case '?':
                p += 1;
                state = LXB_URL_STATE_QUERY_STATE;
                break;
            case '#':
                p += 1;
                state = LXB_URL_STATE_FRAGMENT_STATE;
                break;
        }

        goto again;

    case LXB_URL_STATE_OPAQUE_PATH_STATE:
        begin = p;
        url->path.opaque = true;

        while (true) {
            if (p >= end) {
                tmp_str.data = NULL;

                status = lxb_url_percent_encode_after_utf_8(begin, p,
                                                            &tmp_str, url->mraw,
                                                            LXB_URL_MAP_C0, false);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                status = lxb_url_path_list_push(url, &tmp_str);

                lxb_url_parse_return(orig_data, buf, status);
            }

            c = *p;

            if (c == '#' || c == '?') {
                tmp_str.data = NULL;

                status = lxb_url_percent_encode_after_utf_8(begin, p,
                                                            &tmp_str, url->mraw,
                                                            LXB_URL_MAP_C0, false);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                status = lxb_url_path_list_push(url, &tmp_str);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                p += 1;

                if (c == '#') {
                    state = LXB_URL_STATE_FRAGMENT_STATE;
                }
                else {
                    state = LXB_URL_STATE_QUERY_STATE;
                }

                goto again;
            }

            tmp = p;
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);

            if ((!lxb_url_is_url_codepoint(cp) && cp != '%')
                || (cp == '%' && (end - p < 2
                                  || lexbor_str_res_map_hex[p[0]] == 0xff
                                  || lexbor_str_res_map_hex[p[1]] == 0xff)))
            {
                status = lxb_url_log_append(parser, tmp,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }
        }

        break;

    case LXB_URL_STATE_QUERY_STATE:
        if (encoding != LXB_ENCODING_UTF_8
            && (!lxb_url_is_special(url)
                || schm->type == LXB_URL_SCHEMEL_TYPE_WS
                || schm->type == LXB_URL_SCHEMEL_TYPE_WSS))
        {
            encoding = LXB_ENCODING_UTF_8;

            enc = lxb_encoding_data(encoding);
            if (enc == NULL) {
                lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_WRONG_ARGS);
            }
        }

        begin = p;

        while (true) {
            c = (p < end) ? *p : '\0';

            if (p >= end || (override_state == LXB_URL_STATE__UNDEF && *p == '#')) {
                if (lxb_url_is_special(url)) {
                    map_type = LXB_URL_MAP_SPECIAL_QUERY;
                }
                else {
                    map_type = LXB_URL_MAP_QUERY;
                }

                status = lxb_url_percent_encode_after_encoding(begin, p,
                                                               &url->query,
                                                               url->mraw, enc,
                                                               map_type, false);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }

                if (p < end) {
                    p += 1;
                    state = LXB_URL_STATE_FRAGMENT_STATE;
                    goto again;
                }

                lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);
            }

            tmp = p;
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);

            if ((!lxb_url_is_url_codepoint(cp) && cp != '%')
                || (cp == '%' && (end - p < 2
                                  || lexbor_str_res_map_hex[p[0]] == 0xff
                                  || lexbor_str_res_map_hex[p[1]] == 0xff)))
            {
                status = lxb_url_log_append(parser, tmp,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }
        }

        break;

    case LXB_URL_STATE_FRAGMENT_STATE:
        begin = p;

        while (p < end) {
            tmp = p;
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);

            if ((!lxb_url_is_url_codepoint(cp) && cp != '%')
                || (cp == '%' && (end - p < 2
                                  || lexbor_str_res_map_hex[p[0]] == 0xff
                                  || lexbor_str_res_map_hex[p[1]] == 0xff)))
            {
                status = lxb_url_log_append(parser, tmp,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    lxb_url_parse_return(orig_data, buf, status);
                }
            }
        }

        status = lxb_url_percent_encode_after_utf_8(begin, p, &url->fragment,
                                                    url->mraw,
                                                    LXB_URL_MAP_FRAGMENT, false);
        lxb_url_parse_return(orig_data, buf, status);

    default:
        lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR);
    }

    lxb_url_parse_return(orig_data, buf, LXB_STATUS_OK);

failed_non_relative_url:

    status = lxb_url_log_append(parser, p,
                           LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL);
    if (status != LXB_STATUS_OK) {
        lxb_url_parse_return(orig_data, buf, status);
    }

    lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);

failed_host:

    status = lxb_url_log_append(parser, p,
                                LXB_URL_ERROR_TYPE_HOST_MISSING);
    if (status != LXB_STATUS_OK) {
        lxb_url_parse_return(orig_data, buf, status);
    }

    lxb_url_parse_return(orig_data, buf, LXB_STATUS_ERROR_UNEXPECTED_DATA);
}

static const lxb_char_t *
lxb_url_path_fast_path(lxb_url_parser_t *parser, lxb_url_t *url,
                       const lxb_char_t *p, const lxb_char_t *end, bool bqs)
{
    size_t count;
    lxb_char_t x, c;
    lxb_status_t status;
    const lxb_char_t *begin, *last;

    begin = p;
    last = p;
    count = url->path.length;

    for (; p < end; p++) {
        c = *p;
        x = lxb_url_path_map[c];

        if (x != 0x00) {
            if (c == '/') {
                count += 1;
                last = p + 1;
            }
            else if (c == '%') {
                if (end - p < 3
                    || lexbor_str_res_map_hex[p[1]] == 0xff
                    || lexbor_str_res_map_hex[p[2]] == 0xff)
                {
                    status = lxb_url_log_append(parser, p,
                                                LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }

                    p = (end - p < 3) ? end - 1 : p + 2;
                }
                else if (p[1] == '2' && (p[2] == 'e' || p[2] == 'E')
                         && (p == begin
                             || p[-1] == '/'
                             || (p[-1] == '\\' && lxb_url_is_special(url))))
                {
                    url->path.length = count;

                    status = lxb_url_path_try_dot(url, &begin, &last,
                                                  &p, end, bqs);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }

                    count = url->path.length;
                }
                else {
                    p += 2;
                }
            }
            else if ((c == '?' || c == '#') && bqs) {
                break;
            }
            else if (c == '\\' && lxb_url_is_special(url)) {
                count += 1;

                status = lxb_url_log_append(parser, p,
                                            LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
                if (status != LXB_STATUS_OK) {
                    return NULL;
                }

                status = lxb_url_path_append(url, begin, p - begin);
                if (status != LXB_STATUS_OK) {
                    return NULL;
                }

                begin = p + 1;
                last = p + 1;
            }
            else if (c == '.') {
                if (p == begin
                    || p[-1] == '/'
                    || (p[-1] == '\\' && lxb_url_is_special(url)))
                {
                    url->path.length = count;

                    status = lxb_url_path_try_dot(url, &begin, &last,
                                                  &p, end, bqs);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }

                    count = url->path.length;
                }
            }
            else {
                url->path.length = count;

                if (last - 1 > begin) {
                    status = lxb_url_path_append(url, begin,
                                                 (last - 1) - begin);
                    if (status != LXB_STATUS_OK) {
                        return NULL;
                    }
                }

                return lxb_url_path_slow_path(parser, url, last, end, bqs);
            }
        }
    }

    status = lxb_url_path_append(url, begin, p - begin);
    if (status != LXB_STATUS_OK) {
        return NULL;
    }

    if (count == 0 || p != begin) {
        count += 1;
    }

    url->path.length = count;

    return p;
}

/*
 * The lxb_url_path_slow_path() function should not be static.  Otherwise, the
 * compiler will inline it, which will lead to cache problems and slower code
 * execution.
 */
const lxb_char_t *
lxb_url_path_slow_path(lxb_url_parser_t *parser, lxb_url_t *url,
                       const lxb_char_t *data, const lxb_char_t *end, bool bqs)
{
    size_t len, count;
    lxb_char_t c, *last, *sbuf, *sbuf_begin;
    lexbor_str_t *str;
    lxb_status_t status;
    lxb_codepoint_t cp;
    const lxb_char_t *p, *tmp, *sbuf_end;
    lxb_char_t sbuffer[1024];

    p = data;
    sbuf = sbuffer;
    sbuf_begin = sbuffer;
    sbuf_end = sbuffer + sizeof(sbuffer);

    c = '\0';

    if (url->path.str.length != 0) {
        str = &url->path.str;

        /* "+ 2" == "/\0" */
        if (sbuf + (str->length + 2) > sbuf_end) {
            len = str->length + sizeof(sbuffer);

            sbuf_begin = lexbor_malloc(len);
            if (sbuf_begin == NULL) {
                return NULL;
            }

            sbuf = sbuf_begin;
            sbuf_end = sbuf + len;
        }

        memcpy(sbuf, str->data, str->length);
        sbuf += str->length;

        str->length = 0;
    }

    *sbuf++ = '/';

    last = sbuf;
    count = url->path.length;

    while (p < end) {
        c = *p;

        if (c >= 0x80) {
            tmp = p;
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);

            if (!lxb_url_is_url_codepoint(cp)) {
                status = lxb_url_log_append(parser, tmp,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    goto failed;
                }
            }

            len = (p - tmp) * 3;

            if (sbuf + len + 1 >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last);
            }

            while (tmp < p) {
                c = *tmp++;

                *sbuf++ = '%';
                *sbuf++ = lexbor_str_res_char_to_two_hex_value[c][0];
                *sbuf++ = lexbor_str_res_char_to_two_hex_value[c][1];
            }

            continue;
        }

        if (c == '/') {
            *sbuf++ = '/';
            if (sbuf >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last);
            }

            lxb_url_path_fix_windows_drive(url, last, sbuf, count);

            count += 1;
            last = sbuf;

            if (p + 1 >= end) {
                count += 1;
            }
        }
        else if (c == '\\' && lxb_url_is_special(url)) {
            status = lxb_url_log_append(parser, p,
                                        LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS);
            if (status != LXB_STATUS_OK) {
                goto failed;
            }

            *sbuf++ = '/';
            if (sbuf >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last);
            }

            lxb_url_path_fix_windows_drive(url, last, sbuf, count);

            count += 1;
            last = sbuf;

            if (p + 1 >= end) {
                count += 1;
            }
        }
        else if ((c == '?' || c == '#') && bqs) {
            lxb_url_path_fix_windows_drive(url, last, sbuf, count);

            count += 1;
            last = sbuf;
            break;
        }
        else if (lxb_url_map[c] & LXB_URL_MAP_PATH) {
            if (sbuf + 4 >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last);
            }

            *sbuf++ = '%';
            *sbuf++ = lexbor_str_res_char_to_two_hex_value[c][0];
            *sbuf++ = lexbor_str_res_char_to_two_hex_value[c][1];

            status = lxb_url_log_append(parser, p,
                                        LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
            if (status != LXB_STATUS_OK) {
                goto failed;
            }
        }
        else if (c == '.') {
            if (last == sbuf) {
                tmp = lxb_url_path_dot_count(url, p, end, sbuf_begin,
                                             &sbuf, &last, &count, bqs);

                if (tmp != p) {
                    p = tmp + 1;
                    continue;
                }
            }

            *sbuf++ = '.';
            if (sbuf >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin,
                                     sbuf_end, sbuffer, last);
            }
        }
        else if (c == '%') {
            if (end - p < 3
                || lexbor_str_res_map_hex[p[1]] == 0xff
                || lexbor_str_res_map_hex[p[2]] == 0xff)
            {
                status = lxb_url_log_append(parser, p,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    goto failed;
                }
            }
            else if (p[1] == '2' && (p[2] == 'e' || p[2] == 'E')
                     && last == sbuf)
            {
                tmp = lxb_url_path_dot_count(url, p, end, sbuf_begin,
                                             &sbuf, &last, &count, bqs);

                if (tmp != p) {
                    p = tmp + 1;
                    continue;
                }
            }

            *sbuf++ = '%';
            if (sbuf >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin,
                                     sbuf_end, sbuffer, last);
            }
        }
        else {
            if (lxb_url_codepoint_alphanumeric[c] == 0xFF) {
                status = lxb_url_log_append(parser, p,
                                            LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
                if (status != LXB_STATUS_OK) {
                    goto failed;
                }
            }

            *sbuf++ = c;
            if (sbuf >= sbuf_end) {
                LXB_URL_SBUF_REALLOC(sbuf, sbuf_begin, sbuf_end, sbuffer, last);
            }
        }

        p += 1;
    }

    if (count == 0 || last < sbuf) {
        lxb_url_path_fix_windows_drive(url, last, sbuf, count);
        count += 1;
    }

    url->path.length = count;

    status = lxb_url_path_append_wo_slash(url, sbuf_begin, sbuf - sbuf_begin);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    if (sbuf_begin != sbuffer) {
        lexbor_free(sbuf_begin);
    }

    return p;

failed:

    if (sbuf_begin != sbuffer) {
        lexbor_free(sbuf_begin);
    }

    return NULL;
}

static lxb_status_t
lxb_url_path_try_dot(lxb_url_t *url, const lxb_char_t **begin,
                     const lxb_char_t **last, const lxb_char_t **start,
                     const lxb_char_t *end, bool bqs)
{
    unsigned count;
    lxb_char_t c;
    lexbor_str_t *str;
    lxb_status_t status;
    const lxb_char_t *p;

    p = *start;
    count = 0;

    for (; p < end; p++) {
        c = *p;

        if (c == '/'
            || (c == '\\' && lxb_url_is_special(url))
            || ((c == '?' || c == '#') && bqs))
        {
            break;
        }
        else if (c == '.') {
            count += 1;
        }
        else if (c == '%') {
            if (p + 3 <= end && p[1] == '2' && (p[2] == 'e' || p[2] == 'E')) {
                count += 1;
            }
            else {
                return LXB_STATUS_OK;
            }

            p += 2;
        }
        else {
            return LXB_STATUS_OK;
        }
    }

    if (count == 0 || count > 2) {
        return LXB_STATUS_OK;
    }

    if (*start > *begin) {
        status = lxb_url_path_append(url, *begin, (*start - *begin) - 1);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (p < end) {
        *start = p;
        *begin = p + 1;
        *last = *begin;
    }
    else {
        *start = end - 1;
        *begin = end;
        *last = end;
    }

    if (count == 2) {
        lxb_url_path_shorten(url);
    }
    else if (count == 1) {
        str = &url->path.str;

        if (str->length > 0 && str->data[str->length - 1] == '/') {
            str->length -= 1;
            str->data[str->length] = '\0';
        }
    }

    return LXB_STATUS_OK;
}

static const lxb_char_t *
lxb_url_path_dot_count(lxb_url_t *url, const lxb_char_t *p,
                       const lxb_char_t *end, const lxb_char_t *sbuf_begin,
                       lxb_char_t **sbuf, lxb_char_t **last, size_t *path_count,
                       bool bqs)
{
    unsigned count;
    lxb_char_t c, *last_p;
    const lxb_char_t *begin;

    count = 0;
    begin = p;

    for (; p < end; p++) {
        c = *p;

        if (c == '/'
            || (c == '\\' && lxb_url_is_special(url))
            || ((c == '?' || c == '#') && bqs))
        {
            break;
        }
        else if (c == '.') {
            count += 1;
        }
        else if (c == '%') {
            if (p + 3 <= end && p[1] == '2' && (p[2] == 'e' || p[2] == 'E')) {
                count += 1;
            }
            else {
                return begin;
            }

            p += 2;
        }
        else {
            return begin;
        }
    }

    if (count == 0 || count > 2) {
        return begin;
    }

    if (url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE
        && *path_count == 1
        && lxb_url_normalized_windows_drive_letter(sbuf_begin + 1, *last - 1))
    {
        return p;
    }

    if (count == 2) {
        if (*path_count > 0) {
            *path_count -= 1;
            last_p = *last - 1;

            while (last_p > sbuf_begin) {
                last_p -= 1;

                if (*last_p == '/') {
                    *sbuf = last_p + 1;
                    break;
                }
            }

            *last = *sbuf;
        }
    }

    return p;
}

static void
lxb_url_path_fix_windows_drive(lxb_url_t *url, lxb_char_t *sbuf,
                               const lxb_char_t *last, size_t count)
{
    if (url->scheme.type == LXB_URL_SCHEMEL_TYPE_FILE
        && count == 0
        && ((last - sbuf == 3 && (last[-1] == '/')) || last - sbuf == 2)
        && lxb_url_windows_drive_letter(sbuf, last))
    {
        sbuf[1] = ':';
    }
}

static lxb_status_t
lxb_url_leading_trailing(lxb_url_parser_t *parser,
                         const lxb_char_t **data, size_t *length)
{
    lxb_char_t c;
    lxb_status_t status;
    lxb_codepoint_t cp;
    const lxb_char_t *p, *end, *tmp;

    p = *data;
    end = p + *length;

    while (p < end) {
        c = *p;

        if (c > 0x1F && c < 0x80 && c != 0x20) {
            break;
        }
        else if (c >= 0x80) {
            tmp = p;
            cp = lxb_encoding_decode_valid_utf_8_single(&p, end);
            if (cp > 0x1F && cp != LXB_ENCODING_DECODE_ERROR) {
                p = tmp;
                break;
            }

            continue;
        }

        p += 1;
    }

    if (p != *data) {
        status = lxb_url_log_append(parser, *data,
                                    LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    while (end > p) {
        tmp = end;
        cp = lxb_encoding_decode_valid_utf_8_single_reverse(&end, p);

        if (cp > 0x1F && cp != 0x20 && cp != LXB_ENCODING_DECODE_ERROR) {
            end = tmp;
            break;
        }
    }

    if (end != *data + *length) {
        status = lxb_url_log_append(parser, end,
                                    LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    *data = p;
    *length = end - p;

    return LXB_STATUS_OK;
}

static const lxb_char_t *
lxb_url_remove_tab_newline(lxb_url_parser_t *parser,
                           const lxb_char_t *data, size_t *length)
{
    size_t len;
    lxb_char_t c, *buf, *p_buf;
    lxb_status_t status;
    const lxb_char_t *p, *end;

    p = data;
    end = data + *length;

    /* Fast path. */

    p = lexbor_swar_seek3(p, end, '\n', '\r', '\t');

    while (p < end) {
        c = *p;

        if (c == '\n' || c == '\r' || c == '\t') {
            /* Slow path. */
            goto oh_my;
        }

        p += 1;
    }

    return data;

oh_my:

    status = lxb_url_log_append(parser, p,
                                LXB_URL_ERROR_TYPE_INVALID_URL_UNIT);
    if (status != LXB_STATUS_OK) {
        return NULL;
    }

    buf = lexbor_malloc(*length + 1);
    if (buf == NULL) {
        return NULL;
    }

    p_buf = buf;
    len = p - data;
    p_buf += len;

    memcpy(buf, data, len);

    p += 1;

    while (p < end) {
        c = *p;

        if (c == '\n' || c == '\r' || c == '\t') {
            p += 1;
            continue;
        }

        *p_buf++ = c;
        p += 1;
    }

    *length = p_buf - buf;

    buf[*length] = '\0';

    return buf;
}


static const lxb_url_scheme_data_t *
lxb_url_scheme_find(const lxb_char_t *data, size_t length)
{
    const lxb_url_scheme_data_t *schm;

    for (size_t i = LXB_URL_SCHEMEL_TYPE__UNKNOWN + 1;
         i < lxb_url_scheme_length; i++)
    {
        schm = &lxb_url_scheme_res[i];

        if (schm->name.length == length) {
            if (lexbor_str_data_ncasecmp(schm->name.data, data, length)) {
                return schm;
            }
        }
    }

    return &lxb_url_scheme_res[LXB_URL_SCHEMEL_TYPE__UNKNOWN];
}

static lxb_status_t
lxb_url_percent_encode_after_encoding(const lxb_char_t *data,
                                      const lxb_char_t *end, lexbor_str_t *str,
                                      lexbor_mraw_t *mraw,
                                      const lxb_encoding_data_t *encoding,
                                      lxb_url_map_type_t enmap,
                                      bool space_as_plus)
{
    int8_t len;
    size_t length, size;
    lxb_status_t status;
    lxb_codepoint_t cp;
    const lxb_char_t *p, *pb;
    lxb_encoding_encode_t encode;
    lxb_char_t c, buffer[128], percent[3];
    lxb_char_t *buf = buffer;
    const lxb_char_t *buf_end = buf + sizeof(buffer);
    static const lexbor_str_t esc_str = lexbor_str("%26%23");

    if (encoding->encoding == LXB_ENCODING_UTF_8) {
        return lxb_url_percent_encode_after_utf_8(data, end, str, mraw,
                                                  enmap, space_as_plus);
    }

    lxb_url_encoding_init(encoding, &encode);

    p = data;
    length = end - p;

    /* Only valid for UTF-8. */

    while (p < end) {
        if (lxb_url_map[*p++] & enmap) {
            length += 2;
        }
    }

    status = lxb_url_str_init(str, mraw, length + 1);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    p = data;
    percent[0] = '%';

    while (p < end) {
        cp = lxb_encoding_decode_valid_utf_8_single(&p, end);
        if (cp > LXB_ENCODING_DECODE_MAX_CODEPOINT) {
            continue;
        }

        len = encoding->encode_single(&encode, &buf, buf_end, cp);

        if (len < LXB_ENCODING_ENCODE_OK) {
            size = lexbor_conv_int64_to_data((int64_t) cp, buf, buf_end - buf);

            if (lexbor_str_append(str, mraw, esc_str.data, esc_str.length) == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            if (lexbor_str_append(str, mraw, buf, size) == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            percent[1] = '3';
            percent[2] = 'B';

            if (lexbor_str_append(str, mraw, percent, 3) == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            continue;
        }

    iso_2022_jp:

        pb = buffer;

        while (pb < buf) {
            c = *pb;

            if (space_as_plus && c == ' ') {
                pb += 1;

                if (lexbor_str_append_one(str, mraw, '+') == NULL) {
                    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                }
            }
            else if (lxb_url_map[c] & enmap) {
                percent[1] = lexbor_str_res_char_to_two_hex_value[c][0];
                percent[2] = lexbor_str_res_char_to_two_hex_value[c][1];

                if (lexbor_str_append(str, mraw, percent, 3) == NULL) {
                    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                }
            }
            else {
                if (lexbor_str_append_one(str, mraw, c) == NULL) {
                    return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
                }
            }

            pb++;
        }

        buf = buffer;
    }

    if (encoding->encoding == LXB_ENCODING_ISO_2022_JP) {
        len = lxb_encoding_encode_iso_2022_jp_eof_single(&encode,
                                                         &buf, buf_end);
        if (len != 0) {
            goto iso_2022_jp;
        }
    }

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_percent_encode_after_utf_8(const lxb_char_t *data,
                                   const lxb_char_t *end, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw,
                                   lxb_url_map_type_t enmap,
                                   bool space_as_plus)
{
    size_t length;
    lxb_status_t status;
    const lxb_char_t *p;
    lxb_char_t c, *pd;

    p = data;
    length = end - p;

    /* Only valid for UTF-8. */

    while (p < end) {
        if (lxb_url_map[*p++] & enmap) {
            length += 2;
        }
    }

    status = lxb_url_str_init(str, mraw, length + 1);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    p = data;
    pd = &str->data[str->length];

    while (p < end) {
        c = *p;

        if (space_as_plus && c == ' ') {
            *pd++ = '+';
        }
        else if (lxb_url_map[c] & enmap) {
            *pd++ = '%';
            *pd++ = lexbor_str_res_char_to_two_hex_value[c][0];
            *pd++ = lexbor_str_res_char_to_two_hex_value[c][1];
        }
        else {
            *pd++ = c;
        }

        p += 1;
    }

    *pd = '\0';
    str->length += pd - &str->data[str->length];

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_host_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, lxb_url_host_t *host,
                   lexbor_mraw_t *mraw, lxb_url_host_opt_t opt)
{
    uint32_t ipv4;
    lxb_char_t c;
    lxb_status_t status;
    lexbor_str_t *domain;
    const lxb_char_t *p;
    lxb_url_idna_ctx_t context;

    if (data < end && *data == '[') {
        if (end[-1] != ']') {
            status = lxb_url_log_append(parser, &end[-1],
                                        LXB_URL_ERROR_TYPE_IPV6_UNCLOSED);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }

        data += 1;
        end -= 1;

        host->type = LXB_URL_HOST_TYPE_IPV6;

        return lxb_url_ipv6_parse(parser, data, end, host->u.ipv6);
    }

    if (opt & LXB_URL_HOST_OPT_NOT_SPECIAL) {
        return lxb_url_opaque_host_parse(parser, data, end, host, mraw);
    }

    domain = &host->u.domain;

    if (opt & LXB_URL_HOST_OPT_DECODE) {
        status = lxb_url_percent_decode(data, end, domain, mraw, &opt);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }
    else {
        status = lxb_url_str_init(domain, mraw, (end - data) + 1);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (opt & LXB_URL_HOST_OPT_IDNA) {
            domain->length = end - data;

            memcpy(domain->data, data, domain->length);
            domain->data[domain->length] = '\0';
        }
        else {
            (void) lexbor_str_append_lowercase(domain, mraw, data, end - data);
        }
    }

    if (opt & LXB_URL_HOST_OPT_IDNA) {
        if (parser->idna == NULL) {
            parser->idna = lxb_unicode_idna_create();
            status = lxb_unicode_idna_init(parser->idna);
            if (status != LXB_STATUS_OK) {
                return status;
            }
        }

        context.str = domain;
        context.mraw = mraw;

        status = lxb_unicode_idna_to_ascii(parser->idna,
                                           domain->data, domain->length,
                                           lxb_url_host_idna_cb, &context,
                                           LXB_UNICODE_IDNA_FLAG_CHECK_BIDI
                                           | LXB_UNICODE_IDNA_FLAG_CHECK_JOINERS);

        lxb_unicode_idna_clean(parser->idna);

        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (domain->length == 0) {
        return LXB_STATUS_ERROR_UNEXPECTED_DATA;
    }

    p = domain->data;
    end = p + domain->length;

    while (p < end) {
        c = *p++;

        if (c < 128 && lxb_url_map_forbidden_domain_cp[c] != 0xff) {
            status = lxb_url_log_append(parser, p - 1,
                                 LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }
    }

    if (lxb_url_is_ipv4(parser, domain->data, end)) {
        status = lxb_url_ipv4_parse(parser, domain->data, end, &ipv4);

        (void) lexbor_str_destroy(domain, mraw, false);

        if (status != LXB_STATUS_OK) {
            return status;
        }

        host->u.ipv4 = ipv4;
        host->type = LXB_URL_HOST_TYPE_IPV4;

        return status;
    }

    host->type = LXB_URL_HOST_TYPE_DOMAIN;

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_host_idna_cb(const lxb_char_t *data, size_t len, void *ctx)
{
    lxb_char_t *p;
    lxb_url_idna_ctx_t *idna = ctx;

    if (lexbor_str_size(idna->str) < len + 1) {
        p = lexbor_str_realloc(idna->str, idna->mraw, len + 1);
        if (p == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    memcpy(idna->str->data, data, len);

    idna->str->length = len;
    idna->str->data[len] = 0x00;

    return LXB_STATUS_OK;
}

lxb_inline lxb_status_t
lxb_url_ipv4_append(lxb_url_parser_t *parser, const lxb_char_t *data,
                    const lxb_char_t *end, uint64_t *ipv,
                    int *out_of, unsigned i)
{
    lxb_status_t status;
    lxb_url_error_type_t type;

    status = lxb_url_ipv4_number_parse(data, end, &ipv[i]);

    if (status != LXB_STATUS_OK) {
        if (status == LXB_STATUS_ERROR) {
            type = LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART;
            goto failed;
        }

        status = lxb_url_log_append(parser, data,
                                    LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (ipv[i] > 255) {
        status = lxb_url_log_append(parser, data,
                                    LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (*out_of != -1) {
            *out_of = (int) i;
        }
    }

    return LXB_STATUS_OK;

failed:

    status = lxb_url_log_append(parser, data, type);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

static lxb_status_t
lxb_url_ipv4_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, uint32_t *ipv4)
{
    int out_of;
    uint32_t ip;
    uint64_t parts[5];
    unsigned i;
    lxb_status_t status;
    const lxb_char_t *p, *begin;
    lxb_url_error_type_t type;

    static const uint64_t st[] = {0, 256, 65536, 16777216, 4294967296, 0};

    if (data >= end) {
        return LXB_STATUS_ERROR;
    }

    i = 0;
    p = data;
    begin = data;
    out_of = -1;

    /* Let's make the compiler happy. */

    parts[0] = 0;

    while (p < end) {
        if (*p == '.') {
            if (i == 4) {
                type = LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS;
                goto failed;
            }

            status = lxb_url_ipv4_append(parser, begin, p, parts, &out_of, ++i);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            begin = p + 1;
        }

        p += 1;
    }

    if (begin < p) {
        if (i == 4) {
            type = LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS;
            goto failed;
        }

        status = lxb_url_ipv4_append(parser, begin, p, parts, &out_of, ++i);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }
    else if (p[-1] == '.') {
        status = lxb_url_log_append(parser, begin,
                                    LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART);
        if (status != LXB_STATUS_OK) {
            return status;
        }
    }

    if (out_of != -1 && out_of != (int) i) {
        return LXB_STATUS_ERROR_OVERFLOW;
    }

    if (parts[i] >= st[5 - i]) {
        return LXB_STATUS_ERROR_OVERFLOW;
    }

    ip = (uint32_t) parts[i--];

    for (unsigned j = 1; j <= i; j++)  {
        if (parts[j] > 255) {
            return LXB_STATUS_ERROR_OVERFLOW;
        }

        ip += parts[j] * st[3 - (j - 1)];
    }

    *ipv4 = ip;

    return LXB_STATUS_OK;

failed:

    status = lxb_url_log_append(parser, begin, type);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

static lxb_status_t
lxb_url_ipv4_number_parse(const lxb_char_t *data,
                          const lxb_char_t *end, uint64_t *num)
{
    uint64_t n;
    unsigned r;
    lxb_char_t c;
    lxb_status_t status;
    const lxb_char_t *str_map;

    if (data >= end) {
        goto failed;
    }

    r = 10;
    str_map = lexbor_str_res_map_num;
    status = LXB_STATUS_OK;

    if (data + 1 < end) {
        if (data[0] == '0') {
            if ((data[1] == 'x' || data[1] == 'X')) {
                data += 2;
                r = 16;
                str_map = lexbor_str_res_map_hex;
            }
            else {
                data += 1;
                r = 8;
                str_map = lxb_url_map_num_8;
            }

            if (data >= end) {
                *num = 0;
                return LXB_STATUS_WARNING;
            }

            status = LXB_STATUS_WARNING;
        }
    }

    n = 0;

    while (data < end) {
        c = *data++;

        if (str_map[c] == 0xff) {
            goto failed;
        }

        n = str_map[c] + n * r;

        if (n > UINT32_MAX) {
            break;
        }
    }

    *num = n;

    return status;

failed:

    *num = 0;

    return LXB_STATUS_ERROR;
}

static bool
lxb_url_is_ipv4(lxb_url_parser_t *parser, const lxb_char_t *data,
                const lxb_char_t *end)
{
    bool isit, first;
    uint64_t num;
    lxb_char_t c;
    lxb_status_t status;
    const lxb_char_t *p = end;

    if (data >= end) {
        return false;
    }

    isit = true;
    first = true;

    do {
        p -= 1;
        c = *p;

        if (lexbor_str_res_map_hex[c] == 0xff) {
            if (c == '.') {
                if (p == end - 1) {
                    if (first) {
                        end = p;
                        first = false;
                        continue;
                    }

                    return false;
                }

                p += 1;
                break;
            }
            else if (c != 'x' && c != 'X') {
                return false;
            }
        }

        if (c < '0' || c > '9') {
            isit = false;
        }
    }
    while (p > data);

    if (p == end) {
        return false;
    }

    if (isit) {
        return true;
    }

    status = lxb_url_ipv4_number_parse(p, end, &num);

    return status != LXB_STATUS_ERROR;
}

static lxb_status_t
lxb_url_ipv6_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                   const lxb_char_t *end, uint16_t *ipv6)
{
    size_t i, idx;
    uint16_t *piece, *compress, num, swap;
    lxb_char_t c;
    lxb_status_t status;
    const lxb_char_t *p;
    lxb_url_error_type_t err_type;

    piece = ipv6;
    compress = NULL;
    p = data;

    if (p >= end) {
        goto done;
    }

    if (*p == ':') {
        if (p + 1 >= end || p[1] != ':') {
            p = (p + 1 >= end) ? p : &p[1];

            err_type = LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION;
            goto failed;
        }

        p += 2;

        piece += 1;
        compress = piece;
    }

    while (p < end) {
        if (piece == &ipv6[8]) {
            err_type = LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES;
            goto failed;
        }

        if (*p == ':') {
            if (compress != NULL) {
                err_type = LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION;
                goto failed;
            }

            p += 1;

            piece += 1;
            compress = piece;

            continue;
        }

        num = 0;
        i = 0;

        while (i < 4 && p < end) {
            c = lexbor_str_res_map_hex[*p];
            if (c == 0xff) {
                break;
            }

            num = num << 4 | c;

            p += 1;
            i += 1;
        }

        if (p >= end) {
            *piece++ = num;
            break;
        }

        if (*p == '.') {
            if (i == 0) {
                err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT;
                goto failed;
            }

            p -= i;

            if (piece > &ipv6[6]) {
                err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES;
                goto failed;
            }

            status = lxb_url_ipv4_in_ipv6_parse(parser, &p, end, &piece);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            break;
        }

        if (*p == ':') {
            p += 1;

            if (p >= end) {
                err_type = LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT;
                goto failed;
            }
        }
        else if (p < end) {
            err_type = LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT;
            goto failed;
        }

        *piece++ = num;
    }

done:

    if (compress != NULL) {
        num = piece - compress;
        i = 7;

        while (i != 0 && num > 0) {
            idx = (compress - ipv6) + num - 1;
            swap = ipv6[idx];

            ipv6[idx] = ipv6[i];
            ipv6[i] = swap;

            i -= 1;
            num -= 1;
        }
    }
    else if (piece - ipv6 != 8) {
        err_type = LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES;
        goto failed;
    }

    return LXB_STATUS_OK;

failed:

    status = lxb_url_log_append(parser, p, err_type);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

static lxb_status_t
lxb_url_ipv4_in_ipv6_parse(lxb_url_parser_t *parser, const lxb_char_t **data,
                           const lxb_char_t *end, uint16_t **pieces)
{
    int16_t ipv4;
    uint16_t *piece;
    lxb_char_t c;
    lxb_status_t status;
    unsigned int seen;
    const lxb_char_t *p;
    lxb_url_error_type_t err_type;

    piece = *pieces;
    seen = 0;
    p = *data;

    while (p < end) {
        ipv4 = -1;

        if (seen > 0) {
            if (*p == '.' && seen < 4) {
                p += 1;

                if (p >= end) {
                    break;
                }
            }
            else {
                err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT;
                goto failed;
            }
        }

        do {
            c = *p;

            if (c < '0' || c > '9') {
                if (ipv4 == -1) {
                    err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT;
                    goto failed;
                }

                break;
            }

            if (ipv4 == -1) {
                ipv4 = lexbor_str_res_map_num[c];
            }
            else if (ipv4 == 0) {
                err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT;
                goto failed;
            }
            else {
                ipv4 = ipv4 * 10 + lexbor_str_res_map_num[c];
            }

            if (ipv4 > 255) {
                err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART;
                goto failed;
            }

            p += 1;
        }
        while (p < end);

        *piece = *piece * 0x100 + ipv4;

        seen += 1;

        if (seen == 2 || seen == 4) {
            piece += 1;
        }
    }

    if (seen != 4) {
        err_type = LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS;
        goto failed;
    }

    *pieces = piece;
    *data = p;

    return LXB_STATUS_OK;

failed:

    status = lxb_url_log_append(parser, p, err_type);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    return LXB_STATUS_ERROR_UNEXPECTED_DATA;
}

static lxb_status_t
lxb_url_opaque_host_parse(lxb_url_parser_t *parser, const lxb_char_t *data,
                          const lxb_char_t *end, lxb_url_host_t *host,
                          lexbor_mraw_t *mraw)
{
    lxb_char_t c;
    lxb_status_t status;
    const lxb_char_t *p;

    p = data;

    while (p < end) {
        c = *p++;

        if (c < 128 && lxb_url_map_forbidden_host_cp[c] != 0xff) {
            status = lxb_url_log_append(parser, p - 1,
                                   LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return LXB_STATUS_ERROR_UNEXPECTED_DATA;
        }
    }

    host->type = LXB_URL_HOST_TYPE_OPAQUE;

    return lxb_url_percent_encode_after_utf_8(data, end, &host->u.opaque, mraw,
                                              LXB_URL_MAP_C0, false);
}

static lxb_status_t
lxb_url_percent_decode(const lxb_char_t *data, const lxb_char_t *end,
                       lexbor_str_t *str, lexbor_mraw_t *mraw,
                       lxb_url_host_opt_t *opt)
{
    lxb_char_t c, *dp;
    lxb_status_t status;
    const lxb_char_t *p;

    status = lxb_url_str_init(str, mraw, (end - data) + 1);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    p = data;
    dp = str->data;

    while (p < end) {
        c = *p++;

        if (c != '%') {
            *dp++ = c;
            continue;
        }

        if (p + 2 <= end && lexbor_str_res_map_hex[p[0]] != 0xff
            && lexbor_str_res_map_hex[p[1]] != 0xff)
        {
            c = lexbor_str_res_map_hex[p[0]] << 4 | lexbor_str_res_map_hex[p[1]];
            p += 2;

            if (c >= 0x80) {
                *opt |= LXB_URL_HOST_OPT_IDNA;
            }
        }

        *dp++ = c;
    }

    *dp = '\0';
    str->length = dp - str->data;

    return LXB_STATUS_OK;
}

static lxb_status_t
lxb_url_percent_decode_plus(const lxb_char_t *data, const lxb_char_t *end,
                            lexbor_str_t *str, lexbor_mraw_t *mraw)
{
    lxb_char_t c, *dp;
    lxb_status_t status;
    const lxb_char_t *p;

    status = lxb_url_str_init(str, mraw, (end - data) + 1);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    p = data;
    dp = str->data;

    while (p < end) {
        c = *p++;

        if (c != '%') {
            if (c == '+') {
                c = ' ';
            }

            *dp++ = c;
            continue;
        }

        if (p + 2 <= end && lexbor_str_res_map_hex[p[0]] != 0xff
            && lexbor_str_res_map_hex[p[1]] != 0xff)
        {
            c = lexbor_str_res_map_hex[p[0]] << 4 | lexbor_str_res_map_hex[p[1]];
            p += 2;
        }

        *dp++ = c;
    }

    *dp = '\0';
    str->length = dp - str->data;

    return LXB_STATUS_OK;
}

void
lxb_url_erase(lxb_url_t *url)
{
    if (url == NULL) {
        return;
    }

    if (url->scheme.name.data != NULL) {
        lexbor_str_destroy(&url->scheme.name, url->mraw, false);
    }

    switch (url->host.type) {
        case LXB_URL_HOST_TYPE_DOMAIN:
        case LXB_URL_HOST_TYPE_OPAQUE:
            lexbor_str_destroy(&url->host.u.domain, url->mraw, false);
            break;

        default:
            break;
    }

    if (url->username.data != NULL) {
        lexbor_str_destroy(&url->username, url->mraw, false);
    }

    if (url->password.data != NULL) {
        lexbor_str_destroy(&url->password, url->mraw, false);
    }

    if (url->path.str.data != NULL) {
        lexbor_str_destroy(&url->path.str, url->mraw, false);
    }

    if (url->query.data != NULL) {
        lexbor_str_destroy(&url->query, url->mraw, false);
    }

    if (url->fragment.data != NULL) {
        lexbor_str_destroy(&url->fragment, url->mraw, false);
    }
}

lxb_url_t *
lxb_url_destroy(lxb_url_t *url)
{
    if (url == NULL) {
        return NULL;
    }

    lxb_url_erase(url);

    return lexbor_mraw_free(url->mraw, url);
}

void
lxb_url_memory_destroy(lxb_url_t *url)
{
    (void) lexbor_mraw_destroy(url->mraw, true);
}

static const lxb_char_t *
lxb_url_path_part_by_index(const lxb_url_t *url, size_t index,
                           size_t *out_length)
{
    size_t i, length, begin;
    const lxb_char_t *data;

    data = url->path.str.data;
    length = url->path.str.length;

    i = 0;
    begin = 0;

    while (i < length) {
        if (data[i] == '/') {
            if (index == 0) {
                *out_length = i - begin;
                return &data[begin];
            }

            index -= 1;
            begin = i + 1;
        }

        i += 1;
    }

    *out_length = 0;

    return NULL;
}

lxb_status_t
lxb_url_api_href_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *href, size_t length)
{
    lxb_status_t status;
    lexbor_mraw_t *origin_mraw;
    lxb_url_parser_t self_parser;
    const lxb_char_t tmp[1] = "";

    if (href == NULL) {
        href = tmp;
        length = 0;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    origin_mraw = parser->mraw;
    parser->mraw = url->mraw;

    status = lxb_url_parse_basic_h(parser, NULL, NULL, href, length,
                                   LXB_URL_STATE__UNDEF, LXB_ENCODING_AUTO);

    parser->mraw = origin_mraw;

    if (status != LXB_STATUS_OK) {
        parser->url = lxb_url_destroy(parser->url);
    }
    else {
        lxb_url_erase(url);

        *url = *parser->url;
    }

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

lxb_status_t
lxb_url_api_protocol_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *protocol, size_t length)
{
    lxb_status_t status;
    lxb_url_parser_t self_parser;
    const lxb_char_t tmp[1] = "";

    if (protocol == NULL) {
        protocol = tmp;
        length = 0;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    status = lxb_url_parse_basic_h(parser, url, NULL, protocol, length,
                                   LXB_URL_STATE_SCHEME_START_STATE,
                                   LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

lxb_status_t
lxb_url_api_username_set(lxb_url_t *url,
                         const lxb_char_t *username, size_t length)
{
    if (lxb_url_cannot_have_user_pass_port(url)) {
        return LXB_STATUS_OK;
    }

    url->username.length = 0;

    if (username == NULL || length == 0) {
        lexbor_str_destroy(&url->username, url->mraw, false);
        return LXB_STATUS_OK;
    }

    return lxb_url_percent_encode_after_utf_8(username, username + length,
                                              &url->username, url->mraw,
                                              LXB_URL_MAP_USERINFO, false);
}

lxb_status_t
lxb_url_api_password_set(lxb_url_t *url,
                         const lxb_char_t *password, size_t length)
{
    if (lxb_url_cannot_have_user_pass_port(url)) {
        return LXB_STATUS_OK;
    }

    url->password.length = 0;

    if (password == NULL || length == 0) {
        lexbor_str_destroy(&url->password, url->mraw, false);
        return LXB_STATUS_OK;
    }

    return lxb_url_percent_encode_after_utf_8(password, password + length,
                                              &url->password, url->mraw,
                                              LXB_URL_MAP_USERINFO, false);
}

lxb_status_t
lxb_url_api_host_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *host, size_t length)
{
    return lxb_url_host_set_h(url, parser, host, length,
                              LXB_URL_STATE_HOST_STATE);
}

lxb_status_t
lxb_url_api_hostname_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *hostname, size_t length)
{
    return lxb_url_host_set_h(url, parser, hostname, length,
                              LXB_URL_STATE_HOSTNAME_STATE);
}

static lxb_status_t
lxb_url_host_set_h(lxb_url_t *url, lxb_url_parser_t *parser,
                   const lxb_char_t *host, size_t length,
                   lxb_url_state_t override_state)
{
    lxb_status_t status;
    lxb_url_host_t old;
    lxb_url_parser_t self_parser;
    const lxb_char_t tmp[1] = "";

    /* If this’s URL has an opaque path, then return. */
    if (url->path.opaque) {
        return LXB_STATUS_OK;
    }

    if (host == NULL) {
        host = tmp;
        length = 0;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    old = url->host;

    memset(&url->host, 0x00, sizeof(lxb_url_host_t));

    status = lxb_url_parse_basic_h(parser, url, NULL, host, length,
                                   override_state, LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    if (status != LXB_STATUS_OK) {
        lxb_url_host_destroy(&url->host, url->mraw);
        url->host = old;
    }
    else {
        if (override_state == LXB_URL_STATE_HOSTNAME_STATE
            && url->host.type == LXB_URL_HOST_TYPE__UNDEF)
        {
            url->host = old;
        }
        else {
            lxb_url_host_destroy(&old, url->mraw);
        }
    }

    return status;
}

lxb_status_t
lxb_url_api_port_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *port, size_t length)
{
    lxb_status_t status;
    lxb_url_parser_t self_parser;

    if (lxb_url_cannot_have_user_pass_port(url)) {
        return LXB_STATUS_OK;
    }

    if (port == NULL || length == 0) {
        url->port = 0;
        url->has_port = false;

        return LXB_STATUS_OK;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    status = lxb_url_parse_basic_h(parser, url, NULL, port, length,
                                   LXB_URL_STATE_PORT_STATE, LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

lxb_status_t
lxb_url_api_pathname_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *pathname, size_t length)
{
    lxb_status_t status;
    lxb_url_parser_t self_parser;
    const lxb_char_t tmp[1] = "";

    if (url->path.opaque) {
        return LXB_STATUS_OK;
    }

    if (pathname == NULL) {
        pathname = tmp;
        length = 0;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    url->path.length = 0;
    url->path.str.length = 0;

    status = lxb_url_parse_basic_h(parser, url, NULL, pathname, length,
                                   LXB_URL_STATE_PATH_START_STATE,
                                   LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

lxb_status_t
lxb_url_api_search_set(lxb_url_t *url, lxb_url_parser_t *parser,
                       const lxb_char_t *search, size_t length)
{
    lxb_status_t status;
    lxb_url_parser_t self_parser;

    lexbor_str_destroy(&url->query, url->mraw, false);

    url->query.length = 0;

    if (search == NULL || length == 0) {
        lexbor_str_destroy(&url->query, url->mraw, false);
        return LXB_STATUS_OK;
    }

    if (*search == '?') {
        search += 1;
        length -= 1;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    status = lxb_url_parse_basic_h(parser, url, NULL, search, length,
                                   LXB_URL_STATE_QUERY_STATE,
                                   LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

lxb_status_t
lxb_url_api_hash_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *hash, size_t length)
{
    lxb_status_t status;
    lxb_url_parser_t self_parser;

    lexbor_str_destroy(&url->fragment, url->mraw, false);

    url->fragment.length = 0;

    if (hash == NULL || length == 0) {
        lexbor_str_destroy(&url->fragment, url->mraw, false);
        return LXB_STATUS_OK;
    }

    if (*hash == '#') {
        hash += 1;
        length -= 1;
    }

    if (parser == NULL) {
        parser = &self_parser;

        parser->log = NULL;
        parser->idna = NULL;
        parser->buffer = NULL;
    }

    status = lxb_url_parse_basic_h(parser, url, NULL, hash, length,
                                   LXB_URL_STATE_FRAGMENT_STATE,
                                   LXB_ENCODING_AUTO);

    if (parser == &self_parser) {
        lxb_url_parser_destroy(parser, false);
    }

    return status;
}

static lxb_status_t
lxb_url_serialize_body(lxb_unicode_idna_t *idna, const lxb_url_t *url, lexbor_serialize_cb_f cb,
                     void *ctx, bool exclude_fragment)
{
    lxb_status_t status;
    const lexbor_str_t *str;
    lxb_char_t *p;
    lxb_char_t buf[LXB_URL_BUFFER_NUM_SIZE];

    static const lexbor_str_t colon_str = lexbor_str(":");
    static const lexbor_str_t dsol_str = lexbor_str("//");
    static const lexbor_str_t at_str = lexbor_str("@");
    static const lexbor_str_t dt_str = lexbor_str("/.");
    static const lexbor_str_t qm_str = lexbor_str("?");
    static const lexbor_str_t hs_str = lexbor_str("#");

    /* Scheme. */

    str = &url->scheme.name;

    lexbor_serialize_write(cb, str->data, str->length, ctx, status);
    lexbor_serialize_write(cb, colon_str.data, colon_str.length, ctx, status);

    /* Host. */

    if (url->host.type != LXB_URL_HOST_TYPE__UNDEF) {
        lexbor_serialize_write(cb, dsol_str.data, dsol_str.length, ctx, status);

        if (lxb_url_includes_credentials(url)) {
            lexbor_serialize_write(cb, url->username.data, url->username.length,
                                   ctx, status);

            if (url->password.length != 0) {
                lexbor_serialize_write(cb, colon_str.data, colon_str.length,
                                       ctx, status);
                lexbor_serialize_write(cb, url->password.data,
                                       url->password.length, ctx, status);
            }

            lexbor_serialize_write(cb, at_str.data, at_str.length, ctx, status);
        }

        if (idna != NULL) {
            status = lxb_url_serialize_host_unicode(idna, &url->host, cb, ctx);
        } else {
            status = lxb_url_serialize_host(&url->host, cb, ctx);
        }

        if (status != LXB_STATUS_OK) {
            return status;
        }

        if (url->has_port) {
            lexbor_serialize_write(cb, colon_str.data, colon_str.length,
                                   ctx, status);

            p = buf + lexbor_conv_int64_to_data((int64_t) url->port,
                                                buf, LXB_URL_BUFFER_NUM_SIZE);

            lexbor_serialize_write(cb, buf, p - buf, ctx, status);
        }
    }
    else if (!url->path.opaque && url->path.str.length > 1) {
        str = &url->path.str;

        if (str->data[0] == '/' && str->data[1] == '/') {
            lexbor_serialize_write(cb, dt_str.data, dt_str.length, ctx, status);
        }
    }

    status = lxb_url_serialize_path(&url->path, cb, ctx);
    if (status != LXB_STATUS_OK) {
        return status;
    }

    if (url->query.data != NULL) {
        lexbor_serialize_write(cb, qm_str.data, qm_str.length,
                               ctx, status);
        lexbor_serialize_write(cb, url->query.data, url->query.length,
                               ctx, status);
    }

    if (!exclude_fragment && url->fragment.data != NULL) {
        lexbor_serialize_write(cb, hs_str.data, hs_str.length,
                               ctx, status);
        lexbor_serialize_write(cb, url->fragment.data, url->fragment.length,
                               ctx, status);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize(const lxb_url_t *url, lexbor_serialize_cb_f cb, void *ctx,
                  bool exclude_fragment)
{
    return lxb_url_serialize_body(NULL, url, cb, ctx, exclude_fragment);
}

lxb_status_t
lxb_url_serialize_idna(lxb_unicode_idna_t *idna, const lxb_url_t *url, lexbor_serialize_cb_f cb,
                       void *ctx, bool exclude_fragment)
{
    return lxb_url_serialize_body(idna, url, cb, ctx, exclude_fragment);
}

lxb_status_t
lxb_url_serialize_scheme(const lxb_url_t *url,
                         lexbor_serialize_cb_f cb, void *ctx)
{
    const lexbor_str_t *str = &url->scheme.name;

    return cb(str->data, str->length, ctx);
}

lxb_status_t
lxb_url_serialize_username(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    if (lxb_url_includes_credentials(url)) {
        return cb(url->username.data, url->username.length, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize_password(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    if (lxb_url_includes_credentials(url)) {
        return cb(url->password.data, url->password.length, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize_host(const lxb_url_host_t *host,
                       lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_status_t status;

    static const lexbor_str_t ob_str = lexbor_str("[");
    static const lexbor_str_t cb_str = lexbor_str("]");

    switch (host->type) {
        case LXB_URL_HOST_TYPE_DOMAIN:
        case LXB_URL_HOST_TYPE_OPAQUE:
            return cb(host->u.domain.data, host->u.domain.length, ctx);

        case LXB_URL_HOST_TYPE_IPV4:
            return lxb_url_serialize_host_ipv4(host->u.ipv4, cb, ctx);

        case LXB_URL_HOST_TYPE_IPV6:
            lexbor_serialize_write(cb, ob_str.data, ob_str.length,
                                   ctx, status);

            status = lxb_url_serialize_host_ipv6(host->u.ipv6, cb, ctx);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            return cb(cb_str.data, cb_str.length, ctx);

        default:
            break;
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize_host_unicode(lxb_unicode_idna_t *idna,
                               const lxb_url_host_t *host,
                               lexbor_serialize_cb_f cb, void *ctx)
{
    const lexbor_str_t *str;

    if (host->type != LXB_URL_HOST_TYPE_DOMAIN
        && host->type != LXB_URL_HOST_TYPE_OPAQUE)
    {
        return lxb_url_serialize_host(host, cb, ctx);
    }

    str = &host->u.domain;

    return lxb_unicode_idna_to_unicode(idna, str->data, str->length, cb, ctx,
                                       LXB_UNICODE_IDNA_FLAG_CHECK_BIDI
                                       | LXB_UNICODE_IDNA_FLAG_CHECK_JOINERS);
}

lxb_status_t
lxb_url_serialize_host_ipv4(uint32_t ipv4,
                            lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t *p, *end;
    lxb_char_t buf[LXB_URL_BUFFER_NUM_SIZE];

    p = buf;
    end = p + LXB_URL_BUFFER_NUM_SIZE;

    p += lexbor_conv_int64_to_data((uint8_t) (ipv4 >> 24), p, end - p);
    *p++ = '.';
    p += lexbor_conv_int64_to_data((uint8_t) (ipv4 >> 16), p, end - p);
    *p++ = '.';
    p += lexbor_conv_int64_to_data((uint8_t) (ipv4 >> 8), p, end - p);
    *p++ = '.';
    p += lexbor_conv_int64_to_data((uint8_t) (ipv4), p, end - p);


    /* By specification. */
    /*
    uint32_t n;

    for (size_t i = 0; i < 4; i++) {
        n = ipv4 % 256;

        p += lexbor_conv_int64_to_data((int64_t) n, p, end - p);

        if (i != 3) {
            *p++ = '.';
        }

        ipv4 = (uint32_t) floor((double) ipv4 / 256.0f);
    }
     */

    *p = '\0';

    return cb(buf, p - buf, ctx);
}

lxb_status_t
lxb_url_serialize_host_ipv6(const uint16_t *ipv6,
                            lexbor_serialize_cb_f cb, void *ctx)
{
    bool ignore;
    size_t i, count, tmp_count;
    const uint16_t *compress, *tmp_compress;
    lxb_char_t *p, *end;
    lxb_char_t buf[LXB_URL_BUFFER_NUM_SIZE];

    p = buf;
    end = p + LXB_URL_BUFFER_NUM_SIZE;

    count = 0;
    tmp_count = (size_t) (ipv6[0] == 0);
    compress = NULL;
    tmp_compress = ipv6;

    for (i = 1; i < 8; i++) {
        if (ipv6[i] == 0) {
            if (ipv6[i - 1] == 0) {
                tmp_count += 1;
            }
            else {
                tmp_count = 1;
                tmp_compress = &ipv6[i];
            }
        }
        else if (tmp_count > count) {
            compress = tmp_compress;
            count = tmp_count;
        }
    }

    if (tmp_count > count) {
        compress = tmp_compress;
        count = tmp_count;
    }

    if (compress == &ipv6[1] && ipv6[0] == 0) {
        compress = ipv6;
    }

    if (count < 2) {
        compress = NULL;
    }

    ignore = false;

    for (i = 0; i < 8; i++) {
        if (ignore) {
            if (ipv6[i] == 0) {
                continue;
            }

            ignore = false;
        }

        if (compress == &ipv6[i]) {
            *p++ = ':';

            if (i == 0) {
                *p++ = ':';
            }

            i += count - 1;
            ignore = true;

            continue;
        }

        p += lexbor_conv_dec_to_hex(ipv6[i], p, end - p, false);

        if (i != 7) {
            *p++ = ':';
        }
    }

    *p = '\0';

    return cb(buf, p - buf, ctx);
}

lxb_status_t
lxb_url_serialize_port(const lxb_url_t *url,
                       lexbor_serialize_cb_f cb, void *ctx)
{
    lxb_char_t *p;
    lxb_char_t buf[LXB_URL_BUFFER_NUM_SIZE];

    if (url->has_port) {
        p = buf + lexbor_conv_int64_to_data((int64_t) url->port,
                                            buf, LXB_URL_BUFFER_NUM_SIZE);
        return cb(buf, p - buf, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize_path(const lxb_url_path_t *path,
                       lexbor_serialize_cb_f cb, void *ctx)
{
    return cb(path->str.data, path->str.length, ctx);
}

lxb_status_t
lxb_url_serialize_query(const lxb_url_t *url,
                        lexbor_serialize_cb_f cb, void *ctx)
{
    if (url->query.data != NULL) {
        return cb(url->query.data, url->query.length, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_status_t
lxb_url_serialize_fragment(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx)
{
    if (url->query.data != NULL) {
        return cb(url->fragment.data, url->fragment.length, ctx);
    }

    return LXB_STATUS_OK;
}

lxb_url_t *
lxb_url_clone(lexbor_mraw_t *mraw, const lxb_url_t *url)
{
	lxb_status_t status;
	lxb_url_t *new_url;

	new_url = lexbor_mraw_calloc(mraw, sizeof(lxb_url_t));
	if (new_url == NULL) {
		return NULL;
	}

	new_url->mraw = mraw;

	status = lxb_url_scheme_copy(&url->scheme, &new_url->scheme, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	status = lxb_url_username_copy(&url->username, &new_url->username, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	status = lxb_url_password_copy(&url->password, &new_url->password, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	status = lxb_url_host_copy(&url->host, &new_url->host, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	new_url->port = url->port;
	new_url->has_port = url->has_port;

	status = lxb_url_path_copy(url, new_url);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	status = lxb_url_query_copy(&url->query, &new_url->query, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	status = lxb_url_str_copy(&url->fragment, &new_url->fragment, mraw);
	if (status != LXB_STATUS_OK) {
		goto failed;
	}

	return new_url;

failed:

	lxb_url_destroy(new_url);

	return NULL;
}

lxb_url_search_params_t *
lxb_url_search_params_init(lexbor_mraw_t *mraw,
                           const lxb_char_t *query, size_t length)
{
    lxb_status_t status;
    lxb_url_search_params_t *sp;

    sp = lexbor_mraw_calloc(mraw, sizeof(lxb_url_search_params_t));
    if (sp == NULL) {
        return NULL;
    }

    sp->mraw = mraw;
    sp->length = 0;

    status = lxb_url_search_params_parse(sp, query, length);
    if (status != LXB_STATUS_OK) {
        return lxb_url_search_params_destroy(sp);
    }

    return sp;
}

lxb_inline lxb_url_search_entry_t *
lxb_url_search_params_entry(lxb_url_search_params_t *search_params,
                            lxb_url_search_entry_t *last, lexbor_mraw_t *mraw,
                            const lxb_char_t *query, const lxb_char_t *p,
                            lxb_status_t *status)
{
    lxb_url_search_entry_t *entry;

    entry = lexbor_mraw_calloc(mraw,
                               sizeof(lxb_url_search_entry_t));
    if (entry == NULL) {
        *status = LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        return NULL;
    }

    last->next = entry;
    entry->prev = last;
    entry->next = NULL;

    search_params->length += 1;

    *status = lxb_url_percent_decode_plus(query, p, &entry->name, mraw);

    return entry;
}

static lxb_status_t
lxb_url_search_params_parse(lxb_url_search_params_t *search_params,
                            const lxb_char_t *query, size_t length)
{
    lxb_char_t c;
    lxb_status_t status;
    const lxb_char_t *p, *end;
    lexbor_mraw_t *mraw;
    lxb_url_search_entry_t first, *last, *entry;

    if (query == NULL || length == 0) {
        return LXB_STATUS_OK;
    }

    if (*query == '?') {
        query += 1;
        length -= 1;
    }

    p = query;
    end = p + length;
    mraw = search_params->mraw;
    last = &first;
    last->next = NULL;
    last->prev = NULL;

    while (p < end) {
        c = *p++;

        if (c == '=') {
            entry = lxb_url_search_params_entry(search_params, last, mraw,
                                                query, p - 1, &status);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            last = entry;
            query = p;

            while (p < end) {
                c = *p;

                if (c == '&') {
                    break;
                }

                p += 1;
            }

            status = lxb_url_percent_decode_plus(query, p,
                                                 &entry->value, mraw);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            p += 1;
            query = p;
        }
        else if (c == '&') {
            entry = lxb_url_search_params_entry(search_params, last, mraw,
                                                query, p - 1, &status);
            if (status != LXB_STATUS_OK) {
                return status;
            }

            last = entry;

            lexbor_str_init(&entry->value, mraw, 0);
            if (entry->value.data == NULL) {
                return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
            }

            query = p;
        }
    }

    if (query < p) {
        entry = lxb_url_search_params_entry(search_params, last, mraw,
                                            query, p, &status);
        if (status != LXB_STATUS_OK) {
            return status;
        }

        lexbor_str_init(&entry->value, mraw, 0);
        if (entry->value.data == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    search_params->first = first.next;

    if (first.next != NULL) {
        search_params->last = last;
        search_params->first->prev = NULL;
    }
    else {
        search_params->last = NULL;
    }

    return LXB_STATUS_OK;
}

lxb_url_search_params_t *
lxb_url_search_params_destroy(lxb_url_search_params_t *search_params)
{
    lxb_url_search_entry_t *entry, *next;
    lexbor_mraw_t *mraw;

    if (search_params == NULL) {
        return NULL;
    }

    entry = search_params->first;
    mraw = search_params->mraw;

    while (entry != NULL) {
        next = entry->next;

        lexbor_str_destroy(&entry->name, mraw, false);
        lexbor_str_destroy(&entry->value, mraw, false);
        lexbor_mraw_free(mraw, entry);

        entry = next;
    }

    return lexbor_mraw_free(mraw, search_params);
}

static void
lxb_url_search_params_entry_destroy(lxb_url_search_params_t *search_params,
                                    lxb_url_search_entry_t *entry)
{
    lexbor_mraw_t *mraw = search_params->mraw;

    lexbor_str_destroy(&entry->name, mraw, false);
    lexbor_str_destroy(&entry->value, mraw, false);
    lexbor_mraw_free(mraw, entry);
}

static lxb_status_t
lxb_url_search_params_to_str(lexbor_str_t *str, lexbor_mraw_t *mraw,
                             const lxb_char_t *data, size_t length)
{
    if (data == NULL || length == 0) {
        lexbor_str_init(str, mraw, 0);
    }
    else {
        lexbor_str_init_append(str, mraw, data, length);
    }

    return str->data != NULL ? LXB_STATUS_OK
                             : LXB_STATUS_ERROR_MEMORY_ALLOCATION;
}

lxb_url_search_entry_t *
lxb_url_search_params_append(lxb_url_search_params_t *search_params,
                             const lxb_char_t *name, size_t name_length,
                             const lxb_char_t *value, size_t value_length)
{
    lxb_status_t status;
    lxb_url_search_entry_t *entry;

    entry = lexbor_mraw_calloc(search_params->mraw,
                               sizeof(lxb_url_search_entry_t));
    if (entry == NULL) {
        return NULL;
    }

    status = lxb_url_search_params_to_str(&entry->name, search_params->mraw,
                                          name, name_length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    status = lxb_url_search_params_to_str(&entry->value, search_params->mraw,
                                          value, value_length);
    if (status != LXB_STATUS_OK) {
        goto failed;
    }

    if (search_params->first == NULL) {
        search_params->first = entry;
    }

    if (search_params->last != NULL) {
        search_params->last->next = entry;
    }

    entry->prev = search_params->last;
    entry->next = NULL;
    search_params->last = entry;

    search_params->length += 1;

    return entry;

failed:

    if (entry->name.data != NULL) {
        lexbor_mraw_free(search_params->mraw, entry->name.data);
    }

    if (entry->value.data != NULL) {
        lexbor_mraw_free(search_params->mraw, entry->value.data);
    }

    lexbor_mraw_free(search_params->mraw, entry);

    return NULL;
}

static lexbor_action_t
lxb_url_search_params_delete_cb(lxb_url_search_params_t *sp,
                                lxb_url_search_entry_t *entry, void *ctx)
{
    if (sp->first == entry) {
        sp->first = entry->next;
    }

    if (sp->last == entry) {
        sp->last = entry->prev;
    }

    if (entry->next != NULL) {
        entry->next->prev = entry->prev;
    }

    if (entry->prev != NULL) {
        entry->prev->next = entry->next;
    }

    lxb_url_search_params_entry_destroy(sp, entry);

    sp->length -= 1;

    return LEXBOR_ACTION_OK;
}

void
lxb_url_search_params_delete(lxb_url_search_params_t *search_params,
                             const lxb_char_t *name, size_t name_length,
                             const lxb_char_t *value, size_t value_length)
{
    lxb_url_search_params_match(search_params, name, name_length,
                                value, value_length,
                                lxb_url_search_params_delete_cb, NULL);
}

lxb_url_search_entry_t *
lxb_url_search_params_get_entry(lxb_url_search_params_t *search_params,
                                const lxb_char_t *name, size_t length)
{
    lexbor_str_t *str;
    lxb_url_search_entry_t *entry;

    entry = search_params->first;

    while (entry != NULL) {
        str = &entry->name;

        if (str->length == length
            && memcmp(str->data, name, length) == 0)
        {
            return entry;
        }

        entry = entry->next;
    }

    return NULL;
}

lexbor_str_t *
lxb_url_search_params_get(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t length)
{
    lxb_url_search_entry_t *entry;

    entry = lxb_url_search_params_get_entry(search_params, name, length);
    if (entry == NULL) {
        return NULL;
    }

    return &entry->value;
}

static lexbor_action_t
lxb_url_search_params_get_all_cb(lxb_url_search_params_t *sp,
                                 lxb_url_search_entry_t *entry, void *context)
{
    lxb_url_search_params_ctx_t *ctx = context;

    if (ctx->out_length >= ctx->out_size) {
        return LEXBOR_ACTION_STOP;
    }

    ctx->out_buf[ctx->out_length] = &entry->value;
    ctx->out_length += 1;

    return LEXBOR_ACTION_OK;
}

size_t
lxb_url_search_params_get_all(lxb_url_search_params_t *search_params,
                              const lxb_char_t *name, size_t length,
                              lexbor_str_t **out_buf, size_t out_size)
{
    lxb_url_search_params_ctx_t ctx;

    ctx.out_buf = out_buf;
    ctx.out_length = 0;
    ctx.out_size = out_size;

    lxb_url_search_params_match(search_params, name, length, NULL, 0,
                                lxb_url_search_params_get_all_cb, &ctx);
    return ctx.out_length;
}

static lexbor_action_t
lxb_url_search_params_get_count_cb(lxb_url_search_params_t *sp,
                                   lxb_url_search_entry_t *entry, void *context)
{
    size_t *count = context;

    *count += 1;

    return LEXBOR_ACTION_OK;
}

size_t
lxb_url_search_params_get_count(lxb_url_search_params_t *search_params,
                                const lxb_char_t *name, size_t length)
{
    size_t count = 0;

    lxb_url_search_params_match(search_params, name, length, NULL, 0,
                                lxb_url_search_params_get_count_cb, &count);
    return count;
}

lxb_url_search_entry_t *
lxb_url_search_params_match_entry(lxb_url_search_params_t *search_params,
                                  const lxb_char_t *name, size_t name_length,
                                  const lxb_char_t *value, size_t value_length,
                                  lxb_url_search_entry_t *entry)
{
    lexbor_str_t *str;

    if (entry == NULL) {
        entry = search_params->first;
    }

    while (entry != NULL) {
        str = &entry->name;

        if (str->length == name_length
            && memcmp(str->data, name, name_length) == 0)
        {
            if (value != NULL) {
                str = &entry->value;

                if (str->length == value_length
                    && memcmp(str->data, value, value_length) == 0)
                {
                    return entry;
                }
            }
            else {
                return entry;
            }
        }

        entry = entry->next;
    }

    return NULL;
}

void
lxb_url_search_params_match(lxb_url_search_params_t *search_params,
                            const lxb_char_t *name, size_t name_length,
                            const lxb_char_t *value, size_t value_length,
                            lxb_url_search_params_match_f cb, void *ctx)
{
    lexbor_str_t *str;
    lexbor_action_t action;
    lxb_url_search_entry_t *entry, *next;

    entry = search_params->first;

    while (entry != NULL) {
        str = &entry->name;
        next = entry->next;

        if (str->length == name_length
            && memcmp(str->data, name, name_length) == 0)
        {
            if (value != NULL) {
                str = &entry->value;

                if (str->length == value_length
                    && memcmp(str->data, value, value_length) == 0)
                {
                    action = cb(search_params, entry, ctx);
                    if (action == LEXBOR_ACTION_STOP) {
                        return;
                    }
                }
            }
            else {
                action = cb(search_params, entry, ctx);
                if (action == LEXBOR_ACTION_STOP) {
                    return;
                }
            }
        }

        entry = next;
    }
}

static lexbor_action_t
lxb_url_search_params_has_cb(lxb_url_search_params_t *sp,
                             lxb_url_search_entry_t *entry, void *ctx)
{
    bool *is = ctx;

    *is = true;

    return LEXBOR_ACTION_STOP;
}

bool
lxb_url_search_params_has(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t name_length,
                          const lxb_char_t *value, size_t value_length)
{
    bool is = false;

    lxb_url_search_params_match(search_params, name, name_length,
                                value, value_length,
                                lxb_url_search_params_has_cb, &is);
    return is;
}

lxb_url_search_entry_t *
lxb_url_search_params_set(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t name_length,
                          const lxb_char_t *value, size_t value_length)
{
    bool changed;
    lxb_status_t status;
    lexbor_str_t *str, str_name, str_value;
    lexbor_mraw_t *mraw;
    lxb_url_search_entry_t *entry, *next, *root;

    mraw = search_params->mraw;
    entry = search_params->first;
    changed = false;
    root = NULL;

    while (entry != NULL) {
        str = &entry->name;
        next = entry->next;

        if (str->length == name_length
            && memcmp(str->data, name, name_length) == 0)
        {
            if (changed) {
                lxb_url_search_params_delete_cb(search_params, entry, NULL);
                entry = next;
                continue;
            }

            str_name = entry->name;
            str_value = entry->value;

            status = lxb_url_search_params_to_str(&entry->name, mraw,
                                                  name, name_length);
            if (status != LXB_STATUS_OK) {
                goto failed;
            }

            status = lxb_url_search_params_to_str(&entry->value, mraw,
                                                  value, value_length);
            if (status != LXB_STATUS_OK) {
                lexbor_str_destroy(&entry->name, mraw, false);
                goto failed;
            }

            lexbor_str_destroy(&str_name, mraw, false);
            lexbor_str_destroy(&str_value, mraw, false);

            changed = true;
            root = entry;
        }

        entry = next;
    }

    if (!changed) {
        return lxb_url_search_params_append(search_params, name, name_length,
                                            value, value_length);
    }

    return root;

failed:

    entry->name = str_name;
    entry->value = str_value;

    return NULL;
}

lxb_inline int
lxb_url_search_params_compare(lxb_url_search_entry_t *first,
                              lxb_url_search_entry_t *second)
{
    return strcmp((const char *) first->name.data,
                  (const char *) second->name.data);
}

void
lxb_url_search_params_sort(lxb_url_search_params_t *search_params)
{
    lxb_url_search_entry_t *root, *new_root, *node, *current;

    root = search_params->first;

    if (root == NULL) {
        return;
    }

    node = root;
    root = root->next;

    node->next = NULL;
    new_root = node;

    while (root != NULL) {
        node = root;
        root = root->next;

        if (lxb_url_search_params_compare(node, new_root) < 0) {
            node->next = new_root;
            new_root->prev = node;
            new_root = node;
        }
        else {
            current = new_root;

            while (current->next != NULL
                   && lxb_url_search_params_compare(node, current->next) >= 0)
            {
                current = current->next;
            }

            node->next = current->next;

            if (current->next != NULL) {
                current->next->prev = node;
            }

            current->next = node;
            node->prev = current;
        }
    }

    new_root->prev = NULL;

    search_params->first = new_root;
    search_params->last = node->next != NULL ? node->next : node;
}

static size_t
lxb_url_search_params_length(const lexbor_str_t *str)
{
    size_t length;
    const lxb_char_t *p, *end;

    p = str->data;
    end = p + str->length;
    length = str->length;

    while (p < end) {
        if (lxb_url_map[*p++] & LXB_URL_MAP_X_WWW_FORM) {
            length += 2;
        }
    }

    return length;
}

static lxb_char_t *
lxb_url_search_params_to_buf(const lexbor_str_t *str, lxb_char_t *buf)
{
    lxb_char_t c;
    const lxb_char_t *p, *end;

    p = str->data;
    end = p + str->length;

    while (p < end) {
        c = *p;

        if (c == ' ') {
            *buf++ = '+';
        }
        else if (lxb_url_map[c] & LXB_URL_MAP_X_WWW_FORM) {
            *buf++ = '%';
            *buf++ = lexbor_str_res_char_to_two_hex_value[c][0];
            *buf++ = lexbor_str_res_char_to_two_hex_value[c][1];
        }
        else {
            *buf++ = c;
        }

        p += 1;
    }

    return buf;
}

lxb_status_t
lxb_url_search_params_serialize(lxb_url_search_params_t *search_params,
                                lexbor_callback_f cb, void *ctx)
{
    size_t length;
    lxb_status_t status;
    lxb_char_t *p, *begin;
    lxb_url_search_entry_t *entry;
    lxb_char_t buf[1024];

    entry = search_params->first;

    if (entry == NULL) {
        buf[0] = '\0';
        return cb(buf, 0, ctx);
    }

    /* Calc length. */

    length = 0;

    while (entry != NULL) {
        length += lxb_url_search_params_length(&entry->name);
        length += lxb_url_search_params_length(&entry->value);
        length += 2; /* = and & */

        entry = entry->next;
    }

    if (length < sizeof(buf)) {
        p = buf;
    }
    else {
        p = lexbor_malloc(length);
        if (p == NULL) {
            return LXB_STATUS_ERROR_MEMORY_ALLOCATION;
        }
    }

    /* To buffer. */

    begin = p;
    entry = search_params->first;

    p = lxb_url_search_params_to_buf(&entry->name, p);
    *p++ = '=';
    p = lxb_url_search_params_to_buf(&entry->value, p);

    entry = entry->next;

    while (entry != NULL) {
        *p++ = '&';

        p = lxb_url_search_params_to_buf(&entry->name, p);
        *p++ = '=';
        p = lxb_url_search_params_to_buf(&entry->value, p);

        entry = entry->next;
    }

    *p = '\0';

    status = cb(begin, p - begin, ctx);

    if (buf != begin) {
        lexbor_free(begin);
    }

    return status;
}
