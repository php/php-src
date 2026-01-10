/*
 * Copyright (C) 2018-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_STR_H
#define LEXBOR_STR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/mraw.h"
#include "lexbor/core/utils.h"


#define lexbor_str_get(str, attr) str->attr
#define lexbor_str_set(str, attr) lexbor_str_get(str, attr)
#define lexbor_str_len(str) lexbor_str_get(str, length)
#define lexbor_str(p) {.data = (lxb_char_t *) (p), sizeof(p) - 1}


#define lexbor_str_check_size_arg_m(str, size, mraw, plus_len, return_fail)    \
    do {                                                                       \
        void *tmp;                                                             \
                                                                               \
        if (str->length > (SIZE_MAX - (plus_len)))                             \
            return (return_fail);                                              \
                                                                               \
        if ((str->length + (plus_len)) > (size)) {                             \
            tmp = lexbor_mraw_realloc(mraw, str->data,                         \
                                      (str->length + plus_len));               \
                                                                               \
            if (tmp == NULL) {                                                 \
                return (return_fail);                                          \
            }                                                                  \
                                                                               \
            str->data = (lxb_char_t *) tmp;                                    \
        }                                                                      \
    }                                                                          \
    while (0)


typedef struct {
    lxb_char_t *data;
    size_t     length;
}
lexbor_str_t;


LXB_API lexbor_str_t *
lexbor_str_create(void);

LXB_API lxb_char_t *
lexbor_str_init(lexbor_str_t *str, lexbor_mraw_t *mraw, size_t size);

LXB_API lxb_char_t *
lexbor_str_init_append(lexbor_str_t *str, lexbor_mraw_t *mraw,
                       const lxb_char_t *data, size_t length);

LXB_API void
lexbor_str_clean(lexbor_str_t *str);

LXB_API void
lexbor_str_clean_all(lexbor_str_t *str);

LXB_API lexbor_str_t *
lexbor_str_destroy(lexbor_str_t *str, lexbor_mraw_t *mraw, bool destroy_obj);


LXB_API lxb_char_t *
lexbor_str_realloc(lexbor_str_t *str, lexbor_mraw_t *mraw, size_t new_size);

LXB_API lxb_char_t *
lexbor_str_check_size(lexbor_str_t *str, lexbor_mraw_t *mraw, size_t plus_len);

/* Append */
LXB_API lxb_char_t *
lexbor_str_append(lexbor_str_t *str, lexbor_mraw_t *mraw,
                  const lxb_char_t *data, size_t length);

LXB_API lxb_char_t *
lexbor_str_append_before(lexbor_str_t *str, lexbor_mraw_t *mraw,
                         const lxb_char_t *buff, size_t length);

LXB_API lxb_char_t *
lexbor_str_append_one(lexbor_str_t *str, lexbor_mraw_t *mraw,
                      const lxb_char_t data);

LXB_API lxb_char_t *
lexbor_str_append_lowercase(lexbor_str_t *str, lexbor_mraw_t *mraw,
                            const lxb_char_t *data, size_t length);

LXB_API lxb_char_t *
lexbor_str_append_with_rep_null_chars(lexbor_str_t *str, lexbor_mraw_t *mraw,
                                      const lxb_char_t *buff, size_t length);

/* Other functions */
LXB_API lxb_char_t *
lexbor_str_copy(lexbor_str_t *dest, const lexbor_str_t *target,
                lexbor_mraw_t *mraw);

LXB_API void
lexbor_str_stay_only_whitespace(lexbor_str_t *target);

LXB_API void
lexbor_str_strip_collapse_whitespace(lexbor_str_t *target);

LXB_API size_t
lexbor_str_crop_whitespace_from_begin(lexbor_str_t *target);

LXB_API size_t
lexbor_str_whitespace_from_begin(lexbor_str_t *target);

LXB_API size_t
lexbor_str_whitespace_from_end(lexbor_str_t *target);


/* Data utils */
/*
 * [in] first: must be null-terminated
 * [in] sec: no matter what data
 * [in] sec_size: size of the 'sec' buffer
 *
 * Function compare two lxb_char_t data until find '\0' in first arg.
 * Successfully if the function returned a pointer starting with '\0',
 * otherwise, if the data of the second buffer is insufficient function returned
 * position in first buffer.
 * If function returns NULL, the data are not equal.
 */
LXB_API const lxb_char_t *
lexbor_str_data_ncasecmp_first(const lxb_char_t *first, const lxb_char_t *sec,
                               size_t sec_size);
LXB_API bool
lexbor_str_data_ncasecmp_end(const lxb_char_t *first, const lxb_char_t *sec,
                             size_t size);
LXB_API bool
lexbor_str_data_ncasecmp_contain(const lxb_char_t *where, size_t where_size,
                                 const lxb_char_t *what, size_t what_size);
LXB_API bool
lexbor_str_data_ncasecmp(const lxb_char_t *first, const lxb_char_t *sec,
                         size_t size);
LXB_API bool
lexbor_str_data_nlocmp_right(const lxb_char_t *first, const lxb_char_t *sec,
                             size_t size);
LXB_API bool
lexbor_str_data_nupcmp_right(const lxb_char_t *first, const lxb_char_t *sec,
                             size_t size);
LXB_API bool
lexbor_str_data_casecmp(const lxb_char_t *first, const lxb_char_t *sec);

LXB_API bool
lexbor_str_data_ncmp_end(const lxb_char_t *first, const lxb_char_t *sec,
                         size_t size);
LXB_API bool
lexbor_str_data_ncmp_contain(const lxb_char_t *where, size_t where_size,
                             const lxb_char_t *what, size_t what_size);
LXB_API bool
lexbor_str_data_ncmp(const lxb_char_t *first, const lxb_char_t *sec,
                     size_t size);

LXB_API bool
lexbor_str_data_cmp(const lxb_char_t *first, const lxb_char_t *sec);

LXB_API bool
lexbor_str_data_cmp_ws(const lxb_char_t *first, const lxb_char_t *sec);

LXB_API void
lexbor_str_data_to_lowercase(lxb_char_t *to, const lxb_char_t *from, size_t len);

LXB_API void
lexbor_str_data_to_uppercase(lxb_char_t *to, const lxb_char_t *from, size_t len);

LXB_API const lxb_char_t *
lexbor_str_data_find_lowercase(const lxb_char_t *data, size_t len);

LXB_API const lxb_char_t *
lexbor_str_data_find_uppercase(const lxb_char_t *data, size_t len);


/*
 * Inline functions
 */
lxb_inline lxb_char_t *
lexbor_str_data(lexbor_str_t *str)
{
    return str->data;
}

lxb_inline size_t
lexbor_str_length(lexbor_str_t *str)
{
    return str->length;
}

lxb_inline size_t
lexbor_str_size(lexbor_str_t *str)
{
    return lexbor_mraw_data_size(str->data);
}

lxb_inline void
lexbor_str_data_set(lexbor_str_t *str, lxb_char_t *data)
{
    str->data = data;
}

lxb_inline lxb_char_t *
lexbor_str_length_set(lexbor_str_t *str, lexbor_mraw_t *mraw, size_t length)
{
    if (length >= lexbor_str_size(str)) {
        lxb_char_t *tmp;

        tmp = lexbor_str_realloc(str, mraw, length + 1);
        if (tmp == NULL) {
            return NULL;
        }
    }

    str->length = length;
    str->data[length] = 0x00;

    return str->data;
}

/*
 * No inline functions for ABI.
 */
LXB_API lxb_char_t *
lexbor_str_data_noi(lexbor_str_t *str);

LXB_API size_t
lexbor_str_length_noi(lexbor_str_t *str);

LXB_API size_t
lexbor_str_size_noi(lexbor_str_t *str);

LXB_API void
lexbor_str_data_set_noi(lexbor_str_t *str, lxb_char_t *data);

LXB_API lxb_char_t *
lexbor_str_length_set_noi(lexbor_str_t *str, lexbor_mraw_t *mraw,
                          size_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_STR_H */
