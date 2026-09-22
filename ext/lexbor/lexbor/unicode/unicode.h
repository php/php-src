/*
 * Copyright (C) 2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_UNICODE_H
#define LEXBOR_UNICODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/unicode/base.h"
#include "lexbor/unicode/idna.h"
#include "lexbor/core/array_obj.h"


typedef enum {
    LXB_UNICODE_NFC  = 0x00, /* Normalization Form C (NFC).   */
    LXB_UNICODE_NFD  = 0x01, /* Normalization Form D (NFD).   */
    LXB_UNICODE_NFKC = 0x02, /* Normalization Form KC (NFKC). */
    LXB_UNICODE_NFKD = 0x03  /* Normalization Form KD (NFKD). */
}
lxb_unicode_form_t;


/*
 * Create lxb_unicode_normalizer_t object.
 *
 * @return lxb_unicode_normalizer_t * if successful, otherwise NULL.
 */
LXB_API lxb_unicode_normalizer_t *
lxb_unicode_normalizer_create(void);

/*
 * Initialization of lxb_unicode_normalizer_t object.
 *
 * Support normalization forms:
 *     Normalization Form D (NFD):   LXB_UNICODE_NFD
 *     Normalization Form C (NFC):   LXB_UNICODE_NFC
 *     Normalization Form KD (NFKD): LXB_UNICODE_NFKD
 *     Normalization Form KC (NFKC): LXB_UNICODE_NFKC
 *
 * https://www.unicode.org/reports/tr15/
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Normalization form.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalizer_init(lxb_unicode_normalizer_t *uc,
                            lxb_unicode_form_t form);

/*
 * Initialization of lxb_unicode_normalizer_t object.
 *
 * Clears the object.  Returns to states as after initialization.
 *
 * @param[in] lxb_unicode_normalizer_t *
 */
LXB_API void
lxb_unicode_normalizer_clean(lxb_unicode_normalizer_t *uc);

/*
 * Destroy lxb_unicode_normalizer_t object.
 *
 * Release of occupied resources.
 *
 * @param[in] lxb_unicode_normalizer_t *. Can be NULL.
 * @param[in] if false: only destroys internal buffers.
 * if true: destroys the lxb_unicode_normalizer_t object and all internal buffers.
 *
 * @return lxb_unicode_normalizer_t * if self_destroy = false, otherwise NULL.
 */
LXB_API lxb_unicode_normalizer_t *
lxb_unicode_normalizer_destroy(lxb_unicode_normalizer_t *uc, bool self_destroy);

/*
 * Unicode normalization forms.
 *
 * This is a function with an implementation of the unicode normalization
 * algorithm.
 *
 * The function is designed to work with a stream (chunks).
 *
 * Please, see examples for this function in examples/lexbor/unicode directory.
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Input characters for normalization. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Callback for results of normalization.
 * @param[in] Context for callback.
 * @param[in] Set to true if the last chunk or the only one chunk is processed.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalize(lxb_unicode_normalizer_t *uc, const lxb_char_t *data,
                      size_t length, lexbor_serialize_cb_f cb, void *ctx,
                      bool is_last);

/*
 * Unicode normalization end.
 *
 * The function is used to complete a normalization.
 * Same as calling the lxb_unicode_normalize() function with is_last = true.
 *
 * Use this function only if you do not set is_last = true in
 * the lxb_unicode_normalize() function.
 *
 * For example:
 *     status = lxb_unicode_normalize(uc, data, length, cb, NULL, false);
 *     status = lxb_unicode_normalize(uc, data, length, cb, NULL, false);
 *     lxb_unicode_normalize_end(uc);
 *
 *     The same as:
 *     status = lxb_unicode_normalize(uc, data, length, cb, NULL, false);
 *     status = lxb_unicode_normalize(uc, data, length, cb, NULL, true);
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Callback for results of normalization.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalize_end(lxb_unicode_normalizer_t *uc, lexbor_serialize_cb_f cb,
                          void *ctx);

/*
 * Unicode normalization forms for code points.
 *
 * This function is exactly the same as lxb_unicode_normalize() only it takes
 * code points instead of characters as input.
 *
 * Also, unlike the lxb_unicode_normalize() function, a callback will be called
 * to return a code points, not characters.
 *
 * The function is designed to work with a stream (chunks).
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Input code points for normalization. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Callback for results of normalization.
 * @param[in] Context for callback.
 * @param[in] Set to true if the last chunk or the only one chunk is processed.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalize_cp(lxb_unicode_normalizer_t *uc, const lxb_codepoint_t *cps,
                         size_t length, lexbor_serialize_cb_cp_f cb, void *ctx,
                         bool is_last);

/*
 * Unicode normalization end for code points.
 *
 * This function is completely similar to lxb_unicode_normalize_end(),
 * only it takes a function with code points as a callback function.
 *
 * Same as calling the lxb_unicode_normalize_cp() function with is_last = true.
 *
 * Use this function only if you do not set is_last = true in
 * the lxb_unicode_normalize_cp() function.
 *
 * For example:
 *     status = lxb_unicode_normalize_cp(uc, cps, length, cb, NULL, false);
 *     status = lxb_unicode_normalize_cp(uc, cps, length, cb, NULL, false);
 *     lxb_unicode_normalize_cp_end(uc);
 *
 *     The same as:
 *     status = lxb_unicode_normalize_cp(uc, cps, length, cb, NULL, false);
 *     status = lxb_unicode_normalize_cp(uc, cps, length, cb, NULL, true);
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Callback for results of normalization.
 * @param[in] Context for callback.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalize_cp_end(lxb_unicode_normalizer_t *uc,
                             lexbor_serialize_cb_cp_f cb, void *ctx);

/*
 * Quick Check.
 *
 * The basic normalization algorithm is not simple and requires time
 * and resources.
 * This function checks relatively quickly if the text needs to be normalized.
 *
 * The function is designed to work with a stream (chunks).
 *
 * https://www.unicode.org/reports/tr15/#Detecting_Normalization_Forms
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Input characters for checks. Not NULL.
 * @param[in] Length of characters. Can be 0.
 * @param[in] Set to true if the last chunk or the only one chunk is processed.
 *
 * @return true if it needs to be normalized, otherwise false.
 */
LXB_API bool
lxb_unicode_quick_check(lxb_unicode_normalizer_t *uc, const lxb_char_t *data,
                        size_t length, bool is_last);

/*
 * Quick Check End.
 *
 * The function is used to complete a quick check.
 * Same as calling the lxb_unicode_quick_check() function with is_last = true.
 *
 * Use this function only if you do not set is_last = true in
 * the lxb_unicode_quick_check() function.
 *
 * For example:
 *     is = lxb_unicode_quick_check(uc, data, length, false);
 *     is = lxb_unicode_quick_check(uc, data, length, false);
 *     is = lxb_unicode_quick_check_end(uc);
 *
 *     The same as:
 *     is = lxb_unicode_quick_check(uc, data, length, false);
 *     is = lxb_unicode_quick_check(uc, data, length, true);
 *
 * @param[in] lxb_unicode_normalizer_t *
 *
 * @return true if it needs to be normalized, otherwise false.
 */
LXB_API bool
lxb_unicode_quick_check_end(lxb_unicode_normalizer_t *uc);

/*
 * Quick Check for code points.
 *
 * Same as lxb_unicode_quick_check() only it takes code points as input.
 *
 * @param[in] lxb_unicode_normalizer_t *
 * @param[in] Input code points for checks. Not NULL.
 * @param[in] Length of code points. Can be 0.
 * @param[in] Set to true if the last chunk or the only one chunk is processed.
 *
 * @return true if it needs to be normalized, otherwise false.
 */
LXB_API bool
lxb_unicode_quick_check_cp(lxb_unicode_normalizer_t *uc,
                           const lxb_codepoint_t *cps, size_t length,
                           bool is_last);

/*
 * Quick Check End for code points.
 *
 * Same as lxb_unicode_quick_check_end().
 *
 * For example:
 *     is = lxb_unicode_quick_check_cp(uc, cps, length, false);
 *     is = lxb_unicode_quick_check_cp(uc, cps, length, false);
 *     is = lxb_unicode_quick_check_cp_end(uc);
 *
 *     The same as:
 *     is = lxb_unicode_quick_check_cp(uc, cps, length, false);
 *     is = lxb_unicode_quick_check_cp(uc, cps, length, true);
 *
 * @param[in] lxb_unicode_normalizer_t *
 *
 * @return true if it needs to be normalized, otherwise false.
 */
LXB_API bool
lxb_unicode_quick_check_cp_end(lxb_unicode_normalizer_t *uc);

/*
 * Flush.
 *
 * Force flush the buffer to the user's callback if it possible.
 *
 * Please, see lxb_unicode_flush_count_set() function.
 *
 * @param[in] lxb_unicode_normalizer_t *.
 * @param[in] Callback.
 * @param[in] Callback context.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_flush(lxb_unicode_normalizer_t *uc, lexbor_serialize_cb_f cb,
                  void *ctx);

/*
 * Flush for code points.
 *
 * Same as lxb_unicode_flush(), but it takes a callback with code points as
 * input.
 *
 * @param[in] lxb_unicode_normalizer_t *.
 * @param[in] Callback.
 * @param[in] Callback context.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_flush_cp(lxb_unicode_normalizer_t *uc, lexbor_serialize_cb_cp_f cb,
                     void *ctx);

/*
 * Change normalization form.
 *
 * You should only apply this function after one of the following actions:
 *     1. The lxb_unicode_normalize() function was called with is_last = true.
 *        That is, the processing of the previous type was successfully
 *        completed.
 *  OR
 *     2. The end of normalization function was called:
          lxb_unicode_normalize_end().
 *  OR
 *     3. The lxb_unicode_normalizer_t object cleanup function was called:
 *        lxb_unicode_normalizer_clean().
 *
 *
 * All this is to be able to normalize or quickly check text with different
 * types without creating new objects.
 *
 * @param[in] lxb_unicode_normalizer_t *.
 * @param[in] Normalization form.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_unicode_normalization_form_set(lxb_unicode_normalizer_t *uc,
                                   lxb_unicode_form_t form);

LXB_API const lxb_unicode_entry_t *
lxb_unicode_entry(lxb_codepoint_t cp);

LXB_API const lxb_unicode_composition_cp_t *
lxb_unicode_compose_entry(lxb_codepoint_t first, lxb_codepoint_t second);

LXB_API lxb_unicode_idna_type_t
lxb_unicode_idna_type(lxb_codepoint_t cp);

LXB_API const lxb_unicode_composition_cp_t *
lxb_unicode_composition_cp(lxb_codepoint_t first, lxb_codepoint_t second);

LXB_API const lxb_unicode_normalization_entry_t *
lxb_unicode_normalization_entry(const lxb_unicode_entry_t *entry);

LXB_API const lxb_unicode_normalization_entry_t *
lxb_unicode_normalization_entry_by_cp(lxb_codepoint_t cp);

LXB_API const lxb_unicode_normalization_entry_t *
lxb_unicode_normalization_entry_by_index(uint16_t index);

LXB_API bool
lxb_unicode_normalization_is_null(const lxb_unicode_normalization_entry_t *entry);

LXB_API const lxb_codepoint_t *
lxb_unicode_full_canonical(const lxb_unicode_normalization_entry_t *entry,
                           size_t *out_length);

LXB_API const lxb_codepoint_t *
lxb_unicode_full_compatibility(const lxb_unicode_normalization_entry_t *entry,
                               size_t *out_length);

LXB_API const lxb_unicode_idna_entry_t *
lxb_unicode_idna_entry(const lxb_unicode_entry_t *entry);

LXB_API const lxb_unicode_idna_entry_t *
lxb_unicode_idna_entry_by_cp(lxb_codepoint_t cp);

LXB_API const lxb_unicode_idna_entry_t *
lxb_unicode_idna_entry_by_index(uint16_t index);

LXB_API const lxb_codepoint_t *
lxb_unicode_idna_map(const lxb_unicode_idna_entry_t *entry,
                     size_t *out_length);

/*
 * Inline functions.
 */

/*
 * Sets the buffer size for codepoints.
 *
 * By default, 4096 processed codepoints are accumulated before converting them
 * to lxb_char_t and returning the result to the user via callback.
 *
 * If set the count to 0, the user callback will be called for every codepoint
 * processed.  That is, it will be streaming without accumulation in
 * the intermediate buffer.
 *
 * @param[in] lxb_unicode_normalizer_t *.
 * @param[in] Count of codepoints in the buffer.
 */
lxb_inline void
lxb_unicode_flush_count_set(lxb_unicode_normalizer_t *uc, size_t count)
{
    uc->flush_cp = count;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_UNICODE_H */
