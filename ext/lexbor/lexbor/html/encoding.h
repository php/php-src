/*
 * Copyright (C) 2019-2026 Alexander Borisov
 *
 * Functions for detecting encoding in an HTML byte stream.
 *
 * The HTML parser accepts only UTF-8 input. The detected encoding can be used
 * with the Encoding module to convert the original byte stream from its
 * encoding to UTF-8 before passing it to the parser.
 *
 * By specification:
 *     https://html.spec.whatwg.org/#determining-the-character-encoding
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_HTML_ENCODING_H
#define LEXBOR_HTML_ENCODING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/html/base.h"
#include "lexbor/core/array_obj.h"


typedef struct {
    /* Pointer to the beginning of the encoding name in the original data. */
    const lxb_char_t *name;
    /* Pointer to the end of the encoding name in the original data. */
    const lxb_char_t *end;
}
lxb_html_encoding_entry_t;

typedef struct {
    lexbor_array_obj_t cache;
    lexbor_array_obj_t result;
}
lxb_html_encoding_t;


/*
 * Initialization of the lxb_html_encoding_t object.
 *
 * The object can be allocated on the stack or created using
 * lxb_html_encoding_create() function.
 *
 * @param[in] lxb_html_encoding_t *
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_html_encoding_init(lxb_html_encoding_t *em);

/*
 * Destruction of the lxb_html_encoding_t object.
 *
 * Releases internal resources of the object.
 * If the object was created using lxb_html_encoding_create(), set self_destroy
 * to true.
 *
 * @param[in] lxb_html_encoding_t *
 * @param[in] If true, the object itself will be freed.
 *
 * @return NULL if self_destroy is true. Pointer to the object if false.
 */
LXB_API lxb_html_encoding_t *
lxb_html_encoding_destroy(lxb_html_encoding_t *em, bool self_destroy);

/*
 * Prescan a byte stream to determine its encoding.
 *
 * By specification:
 *     https://html.spec.whatwg.org/#prescan-a-byte-stream-to-determine-its-encoding
 *
 * Returns the validated encoding name if found, or NULL otherwise.
 *
 * Important:
 * The returned pointer is not guaranteed to point into the input data stream.
 * It may reference an internal static string (e.g. when the found encoding name
 * is matched against a known alias). Do not assume the returned pointer lies
 * within the [data, end) range.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 * @param[in] Pointer to the beginning of the data. Not NULL.
 * @param[in] Pointer to the end of the data. Not NULL.
 * @param[out] Length of the returned encoding name. Not NULL.
 *
 * @return Pointer to the encoding name, or NULL if no encoding was found.
 */
LXB_API const lxb_char_t *
lxb_html_encoding_prescan(lxb_html_encoding_t *em, const lxb_char_t *data,
                          const lxb_char_t *end, size_t *out_length);

/*
 * Prescan a byte stream to determine its encoding.
 *
 * Implementation of the HTML specification algorithm for extracting encoding
 * from <meta> tags. Scans raw HTML bytes looking for <meta charset="..."> and
 * <meta http-equiv="content-type" content="...;charset=..."> declarations.
 *
 * Results can be retrieved using lxb_html_encoding_meta_entry() and
 * lxb_html_encoding_meta_length() functions.
 *
 * Important:
 * The HTML specification requires that if the determined charset is
 * UTF-16BE/LE, it must be replaced with UTF-8, and if it is x-user-defined,
 * it must be replaced with windows-1252. This function does not perform these
 * replacements because it returns pointers to the original data (start and end
 * of the found encoding name), which can be useful for developers. To get the
 * validated encoding according to the specification, use the
 * lxb_encoding_data_prescan_validate()/lxb_encoding_prescan_validate()
 * functions from the Encoding module, or handle these cases manually.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 * @param[in] Pointer to the beginning of the data. Not NULL.
 * @param[in] Pointer to the end of the data. Not NULL.
 *
 * @return LXB_STATUS_OK if successful, otherwise an error status value.
 */
LXB_API lxb_status_t
lxb_html_encoding_determine(lxb_html_encoding_t *em,
                            const lxb_char_t *data, const lxb_char_t *end);

/*
 * Extract encoding name from a Content-Type string.
 *
 * Parses the value of a content attribute looking for "charset=<encoding>"
 * pattern. For example, from "text/html; charset=utf-8" extracts "utf-8".
 *
 * @param[in] Pointer to the beginning of the content string. Not NULL.
 * @param[in] Pointer to the end of the content string. Not NULL.
 * @param[out] Pointer to the end of the encoding name. Not NULL.
 *
 * @return Pointer to the beginning of the encoding name, or NULL if not found.
 */
LXB_API const lxb_char_t *
lxb_html_encoding_content(const lxb_char_t *data, const lxb_char_t *end,
                          const lxb_char_t **name_end);


/*
 * Inline functions
 */

/*
 * Create an lxb_html_encoding_t object on the heap.
 *
 * The object must be initialized using lxb_html_encoding_init() after creation.
 *
 * @return Pointer to a new object, or NULL if memory allocation failed.
 */
lxb_inline lxb_html_encoding_t *
lxb_html_encoding_create(void)
{
    return (lxb_html_encoding_t *) lexbor_calloc(1,
                                                 sizeof(lxb_html_encoding_t));
}

/*
 * Clean the lxb_html_encoding_t object for reuse.
 *
 * Resets internal state so the object can be used for a new prescan without
 * reallocation. Does not free memory.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 */
lxb_inline void
lxb_html_encoding_clean(lxb_html_encoding_t *em)
{
    lexbor_array_obj_clean(&em->cache);
    lexbor_array_obj_clean(&em->result);
}

/*
 * Get an encoding entry from the results by index.
 *
 * Returns a pointer to the lxb_html_encoding_entry_t which contains pointers
 * to the beginning and end of the encoding name in the original data.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 * @param[in] Index of the entry.
 *
 * @return Pointer to the entry, or NULL if the index is out of bounds.
 */
lxb_inline lxb_html_encoding_entry_t *
lxb_html_encoding_meta_entry(lxb_html_encoding_t *em, size_t idx)
{
    return (lxb_html_encoding_entry_t *) lexbor_array_obj_get(&em->result, idx);
}

/*
 * Get the number of encoding entries found.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 *
 * @return Number of entries in the result.
 */
lxb_inline size_t
lxb_html_encoding_meta_length(lxb_html_encoding_t *em)
{
    return lexbor_array_obj_length(&em->result);
}

/*
 * Get the result array object directly.
 *
 * Returns the internal array of lxb_html_encoding_entry_t entries.
 *
 * @param[in] lxb_html_encoding_t *. Not NULL.
 *
 * @return Pointer to the lexbor_array_obj_t with results.
 */
lxb_inline lexbor_array_obj_t *
lxb_html_encoding_meta_result(lxb_html_encoding_t *em)
{
    return &em->result;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_html_encoding_t *
lxb_html_encoding_create_noi(void);

LXB_API void
lxb_html_encoding_clean_noi(lxb_html_encoding_t *em);

LXB_API lxb_html_encoding_entry_t *
lxb_html_encoding_meta_entry_noi(lxb_html_encoding_t *em, size_t idx);

LXB_API size_t
lxb_html_encoding_meta_length_noi(lxb_html_encoding_t *em);

LXB_API lexbor_array_obj_t *
lxb_html_encoding_meta_result_noi(lxb_html_encoding_t *em);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_HTML_ENCODING_H */
