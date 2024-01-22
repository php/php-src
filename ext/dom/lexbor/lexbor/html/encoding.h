/*
 * Copyright (C) 2019 Alexander Borisov
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
    const lxb_char_t *name;
    const lxb_char_t *end;
}
lxb_html_encoding_entry_t;

typedef struct {
    lexbor_array_obj_t cache;
    lexbor_array_obj_t result;
}
lxb_html_encoding_t;


LXB_API lxb_status_t
lxb_html_encoding_init(lxb_html_encoding_t *em);

LXB_API lxb_html_encoding_t *
lxb_html_encoding_destroy(lxb_html_encoding_t *em, bool self_destroy);


LXB_API lxb_status_t
lxb_html_encoding_determine(lxb_html_encoding_t *em,
                            const lxb_char_t *data, const lxb_char_t *end);

LXB_API const lxb_char_t *
lxb_html_encoding_content(const lxb_char_t *data, const lxb_char_t *end,
                          const lxb_char_t **name_end);


/*
 * Inline functions
 */
lxb_inline lxb_html_encoding_t *
lxb_html_encoding_create(void)
{
    return (lxb_html_encoding_t *) lexbor_calloc(1,
                                                 sizeof(lxb_html_encoding_t));
}

lxb_inline void
lxb_html_encoding_clean(lxb_html_encoding_t *em)
{
    lexbor_array_obj_clean(&em->cache);
    lexbor_array_obj_clean(&em->result);
}

lxb_inline lxb_html_encoding_entry_t *
lxb_html_encoding_meta_entry(lxb_html_encoding_t *em, size_t idx)
{
    return (lxb_html_encoding_entry_t *) lexbor_array_obj_get(&em->result, idx);
}

lxb_inline size_t
lxb_html_encoding_meta_length(lxb_html_encoding_t *em)
{
    return lexbor_array_obj_length(&em->result);
}

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
