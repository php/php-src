/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 *
 * UNICODE IDNA COMPATIBILITY PROCESSING
 * https://www.unicode.org/reports/tr46/
 */

#ifndef LEXBOR_UNICODE_IDNA_H
#define LEXBOR_UNICODE_IDNA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/unicode/base.h"


typedef lxb_status_t
(*lxb_unicode_idna_cb_f)(const lxb_codepoint_t *part, size_t len,
                         void *ctx, lxb_status_t status);

typedef enum {
    LXB_UNICODE_IDNA_FLAG_UNDEF                   = 0x00,
    LXB_UNICODE_IDNA_FLAG_USE_STD3ASCII_RULES     = 1 << 1,
    LXB_UNICODE_IDNA_FLAG_CHECK_HYPHENS           = 1 << 2,
    LXB_UNICODE_IDNA_FLAG_CHECK_BIDI              = 1 << 3, /* Not implemented. */
    LXB_UNICODE_IDNA_FLAG_CHECK_JOINERS           = 1 << 4, /* Not implemented. */
    LXB_UNICODE_IDNA_FLAG_TRANSITIONAL_PROCESSING = 1 << 5,
    LXB_UNICODE_IDNA_FLAG_VERIFY_DNS_LENGTH       = 1 << 6
}
lxb_unicode_idna_flag_t;

typedef struct {
    lxb_unicode_normalizer_t normalizer;
}
lxb_unicode_idna_t;


/*
 * Create lxb_unicode_idna_t object.
 *
 * @return lxb_unicode_idna_t * if successful, otherwise NULL.
 */
LXB_API lxb_unicode_idna_t *
lxb_unicode_idna_create(void);

/*
 * Initialization of lxb_unicode_idna_t object.
 *
 * @param[in] lxb_unicode_idna_t *.  May be NULL,
 * LXB_STATUS_ERROR_OBJECT_IS_NULL status will be returned.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_init(lxb_unicode_idna_t *idna);

/*
 * Clears the object.  Returns to states as after initialization.
 *
 * @param[in] lxb_unicode_idna_t *
 */
LXB_API void
lxb_unicode_idna_clean(lxb_unicode_idna_t *idna);

/*
 * Destroy lxb_unicode_idna_t object.
 *
 * Release of occupied resources.
 *
 * @param[in] lxb_unicode_idna_t *. Can be NULL.
 * @param[in] if false: only destroys internal buffers.
 * if true: destroys the lxb_unicode_idna_t object and all internal buffers.
 *
 * @return lxb_unicode_idna_t * if self_destroy = false, otherwise NULL.
 */
LXB_API lxb_unicode_idna_t *
lxb_unicode_idna_destroy(lxb_unicode_idna_t *idna, bool self_destroy);

/*
 * Domain name processing.
 *
 * Mapping, Normalization (NFC), Converting, Validating.
 *
 * Callback will be invoked at each level of the domain name.
 *
 * For example:
 *     lexbor.com -- there will be two callbacks, for "lexbor" and "com".
 *
 * https://www.unicode.org/reports/tr46/#Processing
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_processing(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                            size_t length, lxb_unicode_idna_cb_f cb, void *ctx,
                            lxb_unicode_idna_flag_t flags);

/*
 * Domain name processing for code points.
 *
 * This function is exactly the same as lxb_unicode_idna_processing() only it
 * takes code points instead of characters as input.
 *
 * * Please, see lxb_unicode_idna_processing() function.
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input code points for processing. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_processing_cp(lxb_unicode_idna_t *idna,
                               const lxb_codepoint_t *cps, size_t length,
                               lxb_unicode_idna_cb_f cb, void *ctx,
                               lxb_unicode_idna_flag_t flags);

/*
 * Processing and converting domain name to ASCII.
 *
 * Does the same thing as lxb_unicode_idna_processing() + converts each part
 * domain name to Punycode.
 *
 * Callback will be invoked only once in at end of processing.
 *
 * https://www.unicode.org/reports/tr46/#ToASCII
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_to_ascii(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                          size_t length, lexbor_serialize_cb_f cb, void *ctx,
                          lxb_unicode_idna_flag_t flags);

/*
 * Processing and converting domain name to ASCII for code points.
 *
 * This function is exactly the same as lxb_unicode_idna_to_ascii() only it
 * takes code points instead of characters as input.
 *
 * Please, see lxb_unicode_idna_to_ascii() function.
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_to_ascii_cp(lxb_unicode_idna_t *idna, const lxb_codepoint_t *cps,
                             size_t length, lexbor_serialize_cb_f cb, void *ctx,
                             lxb_unicode_idna_flag_t flags);

/*
 * Processing and converting domain name to Unicode.
 *
 * Does the same thing as lxb_unicode_idna_processing().
 *
 * Callback will be invoked only once in at end of processing.
 *
 * https://www.unicode.org/reports/tr46/#ToUnicode
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_to_unicode(lxb_unicode_idna_t *idna, const lxb_char_t *data,
                            size_t length, lexbor_serialize_cb_f cb, void *ctx,
                            lxb_unicode_idna_flag_t flags);

/*
 * Processing and converting domain name to Unicode for code points.
 *
 * This function is exactly the same as lxb_unicode_idna_to_unicode() only it
 * takes code points instead of characters as input.
 *
 * Please, see lxb_unicode_idna_to_unicode() function.
 *
 * @param[in] lxb_unicode_idna_t *.
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of processing.
 * @param[in] Context for callback.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_idna_to_unicode_cp(lxb_unicode_idna_t *idna, const lxb_codepoint_t *cps,
                               size_t length, lexbor_serialize_cb_f cb, void *ctx,
                               lxb_unicode_idna_flag_t flags);

/*
 * Validity Criteria.
 *
 * The function checks the domain name for validity according to a number of
 * criteria.
 *
 * LXB_UNICODE_IDNA_FLAG_CHECK_BIDI and LXB_UNICODE_IDNA_FLAG_CHECK_JOINERS
 * not implemented.
 *
 * https://www.unicode.org/reports/tr46/#Validity_Criteria
 *
 * @param[in] Input characters for processing. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return true if valid, otherwise false.
 */
LXB_API bool
lxb_unicode_idna_validity_criteria(const lxb_char_t *data, size_t length,
                                   lxb_unicode_idna_flag_t flags);

/*
 * Validity Criteria.
 *
 * Same as lxb_unicode_idna_validity_criteria() only it takes codepoints as
 * input.
 *
 * @param[in] Input codepoints for processing. Not NULL.
 * @param[in] Length of codepoints. Can be 0.
 * @param[in] Bitmap of IDNA flags (LXB_UNICODE_IDNA_FLAG_*).
 *
 * @return true if valid, otherwise false.
 */
LXB_API bool
lxb_unicode_idna_validity_criteria_cp(const lxb_codepoint_t *data, size_t length,
                                      lxb_unicode_idna_flag_t flags);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_UNICODE_IDNA_H */
