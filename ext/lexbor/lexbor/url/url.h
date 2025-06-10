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
}
lxb_url_parser_t;


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
lxb_url_clone(lexbor_mraw_t *mraw, lxb_url_t *url);

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
