/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_CSS_SELECTORS_BASE_H
#define LEXBOR_CSS_SELECTORS_BASE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "lexbor/core/base.h"


#define LXB_CSS_SELECTORS_VERSION_MAJOR 1
#define LXB_CSS_SELECTORS_VERSION_MINOR 0
#define LXB_CSS_SELECTORS_VERSION_PATCH 0

#define LXB_CSS_SELECTORS_VERSION_STRING                                       \
    LEXBOR_STRINGIZE(LXB_CSS_SELECTORS_VERSION_MAJOR) "."                      \
    LEXBOR_STRINGIZE(LXB_CSS_SELECTORS_VERSION_MINOR) "."                      \
    LEXBOR_STRINGIZE(LXB_CSS_SELECTORS_VERSION_PATCH)


#define lxb_css_selector_serialize_write(data, length)                        \
    do {                                                                      \
        (status) = cb((lxb_char_t *) (data), (length), (ctx));                \
        if ((status) != LXB_STATUS_OK) {                                      \
            return (status);                                                  \
        }                                                                     \
    }                                                                         \
    while (false)


typedef struct lxb_css_selectors lxb_css_selectors_t;
typedef struct lxb_css_selector lxb_css_selector_t;
typedef struct lxb_css_selector_list lxb_css_selector_list_t;


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_CSS_SELECTORS_BASE_H */
