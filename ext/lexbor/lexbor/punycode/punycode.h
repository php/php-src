/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_PUNYCODE_H
#define LEXBOR_PUNYCODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/punycode/base.h"


typedef lxb_status_t
(*lxb_punycode_encode_cb_f)(const lxb_char_t *data, size_t len, void *ctx,
                            bool unchanged);


/*
 * Punycode: A Bootstring encoding of Unicode
 * for Internationalized Domain Names in Applications (IDNA).
 *
 * https://www.rfc-editor.org/rfc/inline-errata/rfc3492.html
 */

/*
 * Encoding from characters to characters.
 *
 * @param[in] Input characters for encoding. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results. Сalled only once when encoding is complete.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_punycode_encode(const lxb_char_t *data, size_t length,
                    lxb_punycode_encode_cb_f cb, void *ctx);

/*
 * Encoding from code points to characters.
 *
 * Same as lxb_punycode_encode() only the input is code points.
 *
 * @param[in] Input code points for encoding. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Callback for results. Сalled only once when encoding is complete.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_punycode_encode_cp(const lxb_codepoint_t *cps, size_t length,
                       lxb_punycode_encode_cb_f cb, void *ctx);

/*
 * Decoding from characters to characters.
 *
 * @param[in] Input characters for encoding. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results. Сalled only once when encoding is complete.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_punycode_decode(const lxb_char_t *data, size_t length,
                    lexbor_serialize_cb_f cb, void *ctx);

/*
 * Decoding from code points to code points.
 *
 * Same as lxb_punycode_decode() only the input/output is code points.
 *
 * @param[in] Input code points for encoding. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Callback for results. Сalled only once when encoding is complete.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_punycode_decode_cp(const lxb_codepoint_t *data, size_t length,
                       lexbor_serialize_cb_cp_f cb, void *ctx);

/*
 * Decoding from characters to code points.
 *
 * Same as lxb_punycode_decode() only the output is code points.
 *
 * @param[in] Input code points for encoding. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Callback for results. Сalled only once when encoding is complete.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_punycode_decode_cb_cp(const lxb_char_t *data, size_t length,
                          lexbor_serialize_cb_cp_f cb, void *ctx);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_PUNYCODE_H */
