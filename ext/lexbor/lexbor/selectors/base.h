/*
 * Copyright (C) 2021-2023 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_SELECTORS_BASE_H
#define LEXBOR_SELECTORS_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


#define LXB_SELECTORS_VERSION_MAJOR 0
#define LXB_SELECTORS_VERSION_MINOR 3
#define LXB_SELECTORS_VERSION_PATCH 0

#define LXB_SELECTORS_VERSION_STRING LEXBOR_STRINGIZE(LXB_SELECTORS_VERSION_MAJOR) "."   \
                                     LEXBOR_STRINGIZE(LXB_SELECTORS_VERSION_MINOR) "."   \
                                     LEXBOR_STRINGIZE(LXB_SELECTORS_VERSION_PATCH)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_SELECTORS_BASE_H */
