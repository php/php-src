/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 *
 * The URL Standard.
 * By specification: https://url.spec.whatwg.org/
 */

#ifndef LEXBOR_URL_H
#define LEXBOR_URL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/url/base.h"
#include "lexbor/core/mraw.h"
#include "lexbor/core/plog.h"
#include "lexbor/encoding/encoding.h"
#include "lexbor/unicode/unicode.h"

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
    LXB_URL_MAP_USERINFO|LXB_URL_MAP_PATH|LXB_URL_MAP_COMPONENT|LXB_URL_MAP_X_WWW_FORM, /* 0x5e (^) */
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

typedef enum {
    LXB_URL_ERROR_TYPE_DOMAIN_TO_ASCII = 0x00,
    LXB_URL_ERROR_TYPE_DOMAIN_TO_UNICODE,
    LXB_URL_ERROR_TYPE_DOMAIN_INVALID_CODE_POINT,
    LXB_URL_ERROR_TYPE_HOST_INVALID_CODE_POINT,
    LXB_URL_ERROR_TYPE_IPV4_EMPTY_PART,
    LXB_URL_ERROR_TYPE_IPV4_TOO_MANY_PARTS,
    LXB_URL_ERROR_TYPE_IPV4_NON_NUMERIC_PART,
    LXB_URL_ERROR_TYPE_IPV4_NON_DECIMAL_PART,
    LXB_URL_ERROR_TYPE_IPV4_OUT_OF_RANGE_PART,
    LXB_URL_ERROR_TYPE_IPV6_UNCLOSED,
    LXB_URL_ERROR_TYPE_IPV6_INVALID_COMPRESSION,
    LXB_URL_ERROR_TYPE_IPV6_TOO_MANY_PIECES,
    LXB_URL_ERROR_TYPE_IPV6_MULTIPLE_COMPRESSION,
    LXB_URL_ERROR_TYPE_IPV6_INVALID_CODE_POINT,
    LXB_URL_ERROR_TYPE_IPV6_TOO_FEW_PIECES,
    LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_MANY_PIECES,
    LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_INVALID_CODE_POINT,
    LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_OUT_OF_RANGE_PART,
    LXB_URL_ERROR_TYPE_IPV4_IN_IPV6_TOO_FEW_PARTS,
    LXB_URL_ERROR_TYPE_INVALID_URL_UNIT,
    LXB_URL_ERROR_TYPE_SPECIAL_SCHEME_MISSING_FOLLOWING_SOLIDUS,
    LXB_URL_ERROR_TYPE_MISSING_SCHEME_NON_RELATIVE_URL,
    LXB_URL_ERROR_TYPE_INVALID_REVERSE_SOLIDUS,
    LXB_URL_ERROR_TYPE_INVALID_CREDENTIALS,
    LXB_URL_ERROR_TYPE_HOST_MISSING,
    LXB_URL_ERROR_TYPE_PORT_OUT_OF_RANGE,
    LXB_URL_ERROR_TYPE_PORT_INVALID,
    LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER,
    LXB_URL_ERROR_TYPE_FILE_INVALID_WINDOWS_DRIVE_LETTER_HOST,
    LXB_URL_ERROR_TYPE__LAST_ENTRY
}
lxb_url_error_type_t;

typedef enum {
    LXB_URL_STATE__UNDEF = 0x00,
    LXB_URL_STATE_SCHEME_START_STATE,
    LXB_URL_STATE_SCHEME_STATE,
    LXB_URL_STATE_NO_SCHEME_STATE,
    LXB_URL_STATE_SPECIAL_RELATIVE_OR_AUTHORITY_STATE,
    LXB_URL_STATE_PATH_OR_AUTHORITY_STATE,
    LXB_URL_STATE_RELATIVE_STATE,
    LXB_URL_STATE_RELATIVE_SLASH_STATE,
    LXB_URL_STATE_SPECIAL_AUTHORITY_SLASHES_STATE,
    LXB_URL_STATE_SPECIAL_AUTHORITY_IGNORE_SLASHES_STATE,
    LXB_URL_STATE_AUTHORITY_STATE,
    LXB_URL_STATE_HOST_STATE,
    LXB_URL_STATE_HOSTNAME_STATE,
    LXB_URL_STATE_PORT_STATE,
    LXB_URL_STATE_FILE_STATE,
    LXB_URL_STATE_FILE_SLASH_STATE,
    LXB_URL_STATE_FILE_HOST_STATE,
    LXB_URL_STATE_PATH_START_STATE,
    LXB_URL_STATE_PATH_STATE,
    LXB_URL_STATE_OPAQUE_PATH_STATE,
    LXB_URL_STATE_QUERY_STATE,
    LXB_URL_STATE_FRAGMENT_STATE
}
lxb_url_state_t;

/*
 * New values can only be added downwards.
 * Before LXB_URL_SCHEMEL_TYPE__LAST_ENTRY.
 *
 * Please, see lxb_url_scheme_res in /lexbor/url/url.c.
 */
typedef enum {
    LXB_URL_SCHEMEL_TYPE__UNDEF      = 0x00,
    LXB_URL_SCHEMEL_TYPE__UNKNOWN    = 0x01,
    LXB_URL_SCHEMEL_TYPE_HTTP        = 0x02,
    LXB_URL_SCHEMEL_TYPE_HTTPS       = 0x03,
    LXB_URL_SCHEMEL_TYPE_WS          = 0x04,
    LXB_URL_SCHEMEL_TYPE_WSS         = 0x05,
    LXB_URL_SCHEMEL_TYPE_FTP         = 0x06,
    LXB_URL_SCHEMEL_TYPE_FILE        = 0x07,
    LXB_URL_SCHEMEL_TYPE__LAST_ENTRY
}
lxb_url_scheme_type_t;

typedef struct {
    const lexbor_str_t    name;
    uint16_t              port;
    lxb_url_scheme_type_t type;
}
lxb_url_scheme_data_t;

typedef struct {
    lexbor_str_t          name;
    lxb_url_scheme_type_t type;
}
lxb_url_scheme_t;

typedef enum {
    LXB_URL_HOST_TYPE__UNDEF = 0x00,
    LXB_URL_HOST_TYPE_DOMAIN = 0x01,
    LXB_URL_HOST_TYPE_OPAQUE = 0x02,
    LXB_URL_HOST_TYPE_IPV4   = 0x03,
    LXB_URL_HOST_TYPE_IPV6   = 0x04,
    LXB_URL_HOST_TYPE_EMPTY  = 0x05
}
lxb_url_host_type_t;

typedef struct {
    lxb_url_host_type_t type;

    union {
        uint16_t     ipv6[8];
        uint32_t     ipv4;
        lexbor_str_t opaque;
        lexbor_str_t domain;
    } u;
}
lxb_url_host_t;

typedef struct {
    lexbor_str_t str;
    size_t       length;
    bool         opaque;
}
lxb_url_path_t;

typedef struct {
    lxb_url_scheme_t   scheme;

    lxb_url_host_t     host;

    lexbor_str_t       username;
    lexbor_str_t       password;

    uint16_t           port;
    bool               has_port;

    lxb_url_path_t     path;

    lexbor_str_t       query;
    lexbor_str_t       fragment;

    lexbor_mraw_t      *mraw;
}
lxb_url_t;

typedef struct {
    lxb_url_t          *url;
    lexbor_mraw_t      *mraw;
    lexbor_plog_t      *log;

    lxb_unicode_idna_t *idna;

    lxb_char_t         *buffer;
}
lxb_url_parser_t;

/* URLSearchParams */

typedef struct lxb_url_search_entry lxb_url_search_entry_t;

struct lxb_url_search_entry {
    lexbor_str_t           name;
    lexbor_str_t           value;

    lxb_url_search_entry_t *next;
    lxb_url_search_entry_t *prev;
};

typedef struct {
    lxb_url_search_entry_t *first;
    lxb_url_search_entry_t *last;
    lexbor_mraw_t          *mraw;
    size_t                 length;
}
lxb_url_search_params_t;

typedef lexbor_action_t
(*lxb_url_search_params_match_f)(lxb_url_search_params_t *sp,
                                 lxb_url_search_entry_t *entry, void *ctx);


/*
 * Create lxb_url_parser_t object.
 *
 * @return lxb_url_parser_t * if successful, otherwise NULL.
 */
LXB_API lxb_url_parser_t *
lxb_url_parser_create(void);

/*
 * Initialization of lxb_url_parser_t object.
 *
 * The parser is not bound to the received URLs in any way. That is, after
 * parsing the lxb_url_parser_t object can be destroyed and we can continue
 * working with the received URLs.
 *
 * Memory for created URLs is taken from lexbor_mraw_t object, which you can
 * pass during initialization of lxb_url_parser_t object, or a new lexbor_mraw_t
 * object will be created during initialization if NULL is passed.
 *
 * Each created URL will have a pointer to the lexbor_mraw_t object.
 *
 * By destroying the lexbor_mraw_t object you destroy all the URL objects
 * created by the parser. Use the lxb_url_destroy() function to destroy a
 * specific URL.
 *
 * Destroying the lxb_url_parser_t object with lxb_url_parser_destroy() does
 * not destroy the lexbor_mraw_t memory object.
 *
 * Please, see functions lxb_url_parser_memory_destroy(), lxb_url_destroy(),
 * lxb_url_memory_destroy().
 *
 * @param[in] lxb_url_parser_t *
 * @param[in] lexbor_mraw_t *. Can be NULL. If pass NULL, it will create its own
 * memory object inside parser and it will be bound to all created URLs.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_url_parser_init(lxb_url_parser_t *parser, lexbor_mraw_t *mraw);

/*
 * Clears the object. Returns object to states as after initialization.
 *
 * This function must be called before the parsing functions can be reused.
 *
 * For example:
 *     lxb_url_parse()
 *     lxb_url_parser_clean()
 *     lxb_url_parse()
 *     lxb_url_destroy()
 *
 * @param[in] lxb_url_parser_t *
 */
LXB_API void
lxb_url_parser_clean(lxb_url_parser_t *parser);

/*
 * Destroy lxb_url_parser_t object.
 *
 * Release of occupied resources.
 * The lexbor_mraw_t memory object is not destroyed in this function.
 *
 * @param[in] lxb_url_parser_t *. Can be NULL.
 * @param[in] if false: only destroys internal buffers.
 * if true: destroys the lxb_url_parser_t object and all internal buffers.
 *
 * @return lxb_url_parser_t * if self_destroy = false, otherwise NULL.
 */
LXB_API lxb_url_parser_t *
lxb_url_parser_destroy(lxb_url_parser_t *parser, bool destroy_self);

/*
 * Destroys the lexbor_mraw_t object, and thus all associated URLs.
 *
 * After that, new URLs cannot be parsed until a new lexbor_mraw_t object is
 * assigned to the lxb_url_parser_t object.
 *
 * @param[in] lxb_url_parser_t *.
 */
LXB_API void
lxb_url_parser_memory_destroy(lxb_url_parser_t *parser);

lxb_status_t
lxb_url_percent_encode_after_utf_8_ex(const lxb_char_t *data,
                                   const lxb_char_t *end, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw,
                                   lxb_url_map_type_t enmap,
                                   bool space_as_plus,
                                   const uint8_t *url_map);

lxb_status_t
lxb_url_percent_encode_after_utf_8(const lxb_char_t *data,
                                   const lxb_char_t *end, lexbor_str_t *str,
                                   lexbor_mraw_t *mraw,
                                   lxb_url_map_type_t enmap,
                                   bool space_as_plus);

/*
 * URL parser.
 *
 * This functional an implementation of URL parsing according to the WHATWG
 * specification.
 *
 * @param[in] lxb_url_parser_t *.
 * @param[in] const lxb_url_t *. Base URL, can be NULL.
 * @param[in] Input characters. Not NULL.
 * @param[in] Length of characters. Can be 0.
 *
 * @return lxb_url_t * if successful, otherwise NULL.
 */
LXB_API lxb_url_t *
lxb_url_parse(lxb_url_parser_t *parser, const lxb_url_t *base_url,
              const lxb_char_t *data, size_t length);

/*
 * URL basic parser.
 *
 * This functional an implementation of URL parsing according to the WHATWG
 * specification.
 *
 * Use the lxb_url_get() function to get the URL object.
 *
 * @param[in] lxb_url_parser_t *.
 * @param[in] lxb_url_t *. Can be NULL.
 * @param[in] const lxb_url_t *. Base URL, can be NULL.
 * @param[in] Input characters. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] lxb_url_state_t, for default set to LXB_URL_STATE__UNDEF.
 * @param[in] lxb_encoding_t, default (LXB_ENCODING_DEFAULT) LXB_ENCODING_UTF_8.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_url_parse_basic(lxb_url_parser_t *parser, lxb_url_t *url,
                    const lxb_url_t *base_url,
                    const lxb_char_t *data, size_t length,
                    lxb_url_state_t override_state, lxb_encoding_t encoding);

/*
 * Erase URL.
 *
 * Frees all internal memory occupied by the URL object, but does not destroy
 * the object.
 *
 * @param[in] lxb_url_t *.
 *
 * @return NULL.
 */
LXB_API void
lxb_url_erase(lxb_url_t *url);

/*
 * Destroys URL.
 *
 * @param[in] lxb_url_t *.
 *
 * @return NULL.
 */
LXB_API lxb_url_t *
lxb_url_destroy(lxb_url_t *url);

/*
 * Destroys the lexbor_mraw_t memory object.
 *
 * The function will destroy all URLs associated with the lexbor_mraw_t memory
 * object, including the passed one.
 *
 * Keep in mind, if you have a live lxb_url_parser_t parsing object, you will
 * have a pointer to garbage after calling this function instead of a pointer
 * to the lexbor_mraw_t object.
 * In this case you need to assign a new memory object lexbor_mraw_t for the
 * parser. Use the lxb_url_mraw_set() function.
 *
 * @param[in] lxb_url_t *.
 */
LXB_API void
lxb_url_memory_destroy(lxb_url_t *url);


/*
 * Below is an API for modifying the URL object according to the
 * https://url.spec.whatwg.org/#api specification.
 *
 * It is not necessary to pass the lxb_url_parser_t object to API functions.
 * You need to pass the parser if you want to have logs of parsing.
 *
 * All API functions can be passed NULL as "const lxb_char_t *" data.
 */

LXB_API lxb_status_t
lxb_url_api_href_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *href, size_t length);

LXB_API lxb_status_t
lxb_url_api_protocol_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *protocol, size_t length);

LXB_API lxb_status_t
lxb_url_api_username_set(lxb_url_t *url,
                         const lxb_char_t *username, size_t length);

LXB_API lxb_status_t
lxb_url_api_password_set(lxb_url_t *url,
                         const lxb_char_t *password, size_t length);

LXB_API lxb_status_t
lxb_url_api_host_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *host, size_t length);

LXB_API lxb_status_t
lxb_url_api_hostname_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *hostname, size_t length);

LXB_API lxb_status_t
lxb_url_api_port_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *port, size_t length);

LXB_API lxb_status_t
lxb_url_api_pathname_set(lxb_url_t *url, lxb_url_parser_t *parser,
                         const lxb_char_t *pathname, size_t length);

LXB_API lxb_status_t
lxb_url_api_search_set(lxb_url_t *url, lxb_url_parser_t *parser,
                       const lxb_char_t *search, size_t length);

LXB_API lxb_status_t
lxb_url_api_hash_set(lxb_url_t *url, lxb_url_parser_t *parser,
                     const lxb_char_t *hash, size_t length);


/*
 * Below are functions for serializing a URL object and its individual
 * parameters.
 *
 * Note that the callback may be called more than once.
 * For example, the lxb_url_serialize() function will callback multiple times:
 * 1. http
 * 2. ://
 * 3. example.com
 * and so on.
 */

LXB_API lxb_status_t
lxb_url_serialize(const lxb_url_t *url, lexbor_serialize_cb_f cb, void *ctx,
                  bool exclude_fragment);

LXB_API lxb_status_t
lxb_url_serialize_idna(lxb_unicode_idna_t *idna, const lxb_url_t *url, lexbor_serialize_cb_f cb,
                       void *ctx, bool exclude_fragment);

LXB_API lxb_status_t
lxb_url_serialize_scheme(const lxb_url_t *url,
                         lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_username(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_password(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_host(const lxb_url_host_t *host,
                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_host_unicode(lxb_unicode_idna_t *idna,
                               const lxb_url_host_t *host,
                               lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_host_ipv4(uint32_t ipv4,
                            lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_host_ipv6(const uint16_t *ipv6,
                            lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_port(const lxb_url_t *url,
                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_path(const lxb_url_path_t *path,
                       lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_query(const lxb_url_t *url,
                        lexbor_serialize_cb_f cb, void *ctx);

LXB_API lxb_status_t
lxb_url_serialize_fragment(const lxb_url_t *url,
                           lexbor_serialize_cb_f cb, void *ctx);

/*
 * Creates a clone of the object's URL.
 *
 * For lexbor_mraw_t *, use url->mraw or another lexbor_mraw_t * object.
 *
 * @param[in] lexbor_mraw_t *.
 * @param[in] lxb_url_t *.
 *
 * @return a new URL object if successful, otherwise NULL value.
 */
LXB_API lxb_url_t *
lxb_url_clone(lexbor_mraw_t *mraw, const lxb_url_t *url);

/* URLSearchParams */

/*
 * Initialization of lxb_url_search_params_t object.
 *
 * Corresponds to the URLSearchParams interface constructor.
 * https://url.spec.whatwg.org/#interface-urlsearchparams
 *
 * @param[in] lexbor_mraw_t *. Not NULL. Can be taken from lxb_url_t.
 * @param[in] const lxb_char_t *. Can be NULL. Query.
 * @param[in] size_t. Can be 0. Length of query.
 *
 * @return lxb_url_search_params_t * if successful, otherwise NULL value.
 */
LXB_API lxb_url_search_params_t *
lxb_url_search_params_init(lexbor_mraw_t *mraw,
                           const lxb_char_t *params, size_t length);

/*
 * Destroy lxb_url_search_params_t object.
 *
 * Corresponds to the URLSearchParams interface constructor.
 * https://url.spec.whatwg.org/#interface-urlsearchparams
 *
 * @param[in] lxb_url_search_params_t *. Not NULL.
 *
 * @return NULL value.
 */
LXB_API lxb_url_search_params_t *
lxb_url_search_params_destroy(lxb_url_search_params_t *search_params);

/*
 * Append a specified key/value pair as a new search parameter.
 *
 * Adds a new key-value pair to the end of the search parameters list.
 * If a parameter with this name already exists, creates a duplicate.
 * Equivalent to URLSearchParams.prototype.append().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-append
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] name Parameter. Can be NULL.
 * @param[in] name_length Length of parameter name. Can be 0.
 * @param[in] value Parameter. Must not be NULL. Can be NULL.
 * @param[in] Length of parameter value. Can be 0.
 *
 * @return lxb_url_search_entry_t * if successful, otherwise NULL value.
 */
LXB_API lxb_url_search_entry_t *
lxb_url_search_params_append(lxb_url_search_params_t *search_params,
                             const lxb_char_t *name, size_t name_length,
                             const lxb_char_t *value, size_t value_length);

/*
 * Delete search parameters that match a name and value.
 *
 * Removes all search parameters that match the specified name and value.
 * If only name is specified (value == NULL), removes all parameters with
 * that name.
 * Equivalent to URLSearchParams.prototype.delete().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-delete
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] name Parameter. Must not be NULL.
 * @param[in] name_length Length of parameter name.
 * @param[in] value Parameter. Can be NULL.
 * @param[in] Length of parameter value. Can be 0.
 *
 * @return void.
 */
LXB_API void
lxb_url_search_params_delete(lxb_url_search_params_t *search_params,
                             const lxb_char_t *name, size_t name_length,
                             const lxb_char_t *value, size_t value_length);

/*
 * Get the first entry object associated with the given search parameter name.
 *
 * Returns the first search parameter entry with the specified name.
 * Useful for obtaining both name and value of the parameter.
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 *
 * @return Pointer to found parameter entry or NULL if parameter not found.
 */
LXB_API lxb_url_search_entry_t *
lxb_url_search_params_get_entry(lxb_url_search_params_t *search_params,
                                const lxb_char_t *name, size_t length);

/*
 * Get the first value associated with the given search parameter name.
 *
 * Returns the first value associated with the specified search parameter name.
 * Equivalent to URLSearchParams.prototype.get().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-get
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 *
 * @return Pointer to string with parameter value or NULL if parameter not found.
 */
LXB_API lexbor_str_t *
lxb_url_search_params_get(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t length);

/*
 * Get all values associated with the given search parameter name.
 *
 * Returns all values associated with the specified search parameter name.
 * Fills the provided buffer with pointers to strings.
 * Equivalent to URLSearchParams.prototype.getAll().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-getall
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 * @param[out] Buffer for writing pointers to found strings. Must not be NULL.
 * @param[in] Size of output buffer.
 *
 * @return Number of found values or 0 if parameter not found.
 */
LXB_API size_t
lxb_url_search_params_get_all(lxb_url_search_params_t *search_params,
                              const lxb_char_t *name, size_t length,
                              lexbor_str_t **out_buf, size_t out_size);

/*
 * Get the count of values associated with the given search parameter name.
 *
 * Returns the number of values associated with the specified search parameter
 * name. Useful for determining buffer size before calling
 * lxb_url_search_params_get_all.
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 *
 * @return Number of found values or 0 if parameter not found.
 */
LXB_API size_t
lxb_url_search_params_get_count(lxb_url_search_params_t *search_params,
                                const lxb_char_t *name, size_t length);

/*
 * Find an entry that matches with the specified name and, optionally, value,
 * starting from a given entry.
 *
 * Searches for a parameter entry that matches the specified name and,
 * optionally, value, starting the search from the specified position.
 * Useful for iterating through duplicate parameters.
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 * @param[in] Value of the parameter to find. Can be NULL.
 * @param[in] Length of parameter value. Can be 0.
 * @param[in] Entry to start search from. May be NULL to search from beginning.
 *
 * @return Pointer to found entry or NULL if no match found.
 */
LXB_API lxb_url_search_entry_t *
lxb_url_search_params_match_entry(lxb_url_search_params_t *search_params,
                                  const lxb_char_t *name, size_t name_length,
                                  const lxb_char_t *value, size_t value_length,
                                  lxb_url_search_entry_t *entry);

/*
 * Check if a search parameter exists.
 *
 * Checks if a search parameter with the specified name and, optionally, value
 * exists.
 * If value is specified, checks for existence of specific name-value pair.
 * Equivalent to URLSearchParams.prototype.has().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-has
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 * @param[in] Value to check for. May be NULL to check only name.
 * @param[in] Length of parameter value. Can be 0.
 *
 * @return true if parameter exists, false otherwise.
 */
LXB_API bool
lxb_url_search_params_has(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t name_length,
                          const lxb_char_t *value, size_t value_length);

/*
 * Set the value associated with a given search parameter.
 *
 * Sets the value for a search parameter with the specified name.
 * If parameters with this name already exist, removes them all and creates
 * a new one.
 * If parameter doesn't exist, creates it.
 * Equivalent to URLSearchParams.prototype.set().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-set
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Parameter name to set. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 * @param[in] Parameter value to set. Can be NULL.
 * @param[in] Length of parameter value. Can be 0.
 *
 * @return Pointer to created or updated parameter entry or NULL on error.
 */
LXB_API lxb_url_search_entry_t *
lxb_url_search_params_set(lxb_url_search_params_t *search_params,
                          const lxb_char_t *name, size_t name_length,
                          const lxb_char_t *value, size_t value_length);

/*
 * Iterate through all search parameters that match a name and, optionally,
 * value.
 *
 * Iterates through all search parameters that match the specified name and,
 * optionally, value, calling the callback function for each found parameter.
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Name of the parameter to find. Must not be NULL.
 * @param[in] Length of parameter name. Can be 0.
 * @param[in] Value to search for. May be NULL to search by name only.
 * @param[in] Length of parameter value. Can be 0.
 * @param[in] Callback function to process found parameters. Must not be NULL.
 * @param[in] User context passed to callback function. Can be NULL.
 *
 * @return void.
 */
LXB_API void
lxb_url_search_params_match(lxb_url_search_params_t *search_params,
                            const lxb_char_t *name, size_t name_length,
                            const lxb_char_t *value, size_t value_length,
                            lxb_url_search_params_match_f cb, void *ctx);

/*
 * Sort all key/value pairs by their keys.
 *
 * Sorts all key-value pairs in alphabetical order by keys (parameter names).
 * Equivalent to URLSearchParams.prototype.sort().
 * https://url.spec.whatwg.org/#dom-urlsearchparams-sort
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 *
 * @return void.
 */
LXB_API void
lxb_url_search_params_sort(lxb_url_search_params_t *search_params);

/*
 * Serialize search parameters into a URL query string format.
 *
 * Converts all search parameters into a properly encoded query string format
 * according to the application/x-www-form-urlencoded specification.
 * The serialized output is passed to the provided callback function.
 * Equivalent to URLSearchParams.prototype.toString().
 * https://url.spec.whatwg.org/#concept-urlencoded-serializer
 *
 * The callback will be called only once. It will be passed a fully prepared
 * string.
 *
 * @param[in] lxb_url_search_params_t *. Must not be NULL.
 * @param[in] Callback function to receive serialized data chunks. Not NULL.
 * @param[in] User context passed to callback function. May be NULL.
 *
 * @return LXB_STATUS_OK on success, error status on failure.
 */
LXB_API lxb_status_t
lxb_url_search_params_serialize(lxb_url_search_params_t *search_params,
                                lexbor_callback_f cb, void *ctx);

/**
 * Returns whether the URL is special.
 *
 * @param[in] lxb_url_t *. Cannot be NULL.
 * @return true if URL is special, false otherwise.
 */
LXB_API bool
lxb_url_is_special(const lxb_url_t *url);

/*
 * Inline functions.
 */

lxb_inline const lexbor_str_t *
lxb_url_scheme(const lxb_url_t *url)
{
    return &url->scheme.name;
}

lxb_inline const lexbor_str_t *
lxb_url_username(const lxb_url_t *url)
{
    return &url->username;
}

lxb_inline const lexbor_str_t *
lxb_url_password(const lxb_url_t *url)
{
    return &url->password;
}

lxb_inline const lxb_url_host_t *
lxb_url_host(const lxb_url_t *url)
{
    return &url->host;
}

lxb_inline uint16_t
lxb_url_port(const lxb_url_t *url)
{
    return url->port;
}

lxb_inline bool
lxb_url_has_port(const lxb_url_t *url)
{
    return url->has_port;
}

lxb_inline const lxb_url_path_t *
lxb_url_path(const lxb_url_t *url)
{
    return &url->path;
}

lxb_inline const lexbor_str_t *
lxb_url_path_str(const lxb_url_t *url)
{
    return &url->path.str;
}

lxb_inline const lexbor_str_t *
lxb_url_query(const lxb_url_t *url)
{
    return &url->query;
}

lxb_inline const lexbor_str_t *
lxb_url_fragment(const lxb_url_t *url)
{
    return &url->fragment;
}

lxb_inline lexbor_mraw_t *
lxb_url_mraw(lxb_url_parser_t *parser)
{
    return parser->mraw;
}

lxb_inline void
lxb_url_mraw_set(lxb_url_parser_t *parser, lexbor_mraw_t *mraw)
{
    parser->mraw = mraw;
}

lxb_inline lxb_url_t *
lxb_url_get(lxb_url_parser_t *parser)
{
    return parser->url;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_URL_H */
