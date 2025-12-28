/*
 * Copyright (C) 2023-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_URL_BASE_H
#define LEXBOR_URL_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"
#include "lexbor/core/mraw.h"
#include "lexbor/core/str.h"


#define LXB_URL_VERSION_MAJOR 0
#define LXB_URL_VERSION_MINOR 3
#define LXB_URL_VERSION_PATCH 0

#define LXB_URL_VERSION_STRING LEXBOR_STRINGIZE(LXB_URL_VERSION_MAJOR) "."    \
                               LEXBOR_STRINGIZE(LXB_URL_VERSION_MINOR) "."    \
                               LEXBOR_STRINGIZE(LXB_URL_VERSION_PATCH)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_URL_BASE_H */
