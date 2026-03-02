/*
 * Copyright (C) 2023-2024 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_PUNYCODE_BASE_H
#define LEXBOR_PUNYCODE_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


#define LXB_PUNYCODE_VERSION_MAJOR 1
#define LXB_PUNYCODE_VERSION_MINOR 1
#define LXB_PUNYCODE_VERSION_PATCH 0

#define LEXBOR_PUNYCODE_VERSION_STRING LEXBOR_STRINGIZE(LXB_PUNYCODE_VERSION_MAJOR) "." \
                                       LEXBOR_STRINGIZE(LXB_PUNYCODE_VERSION_MINOR) "." \
                                       LEXBOR_STRINGIZE(LXB_PUNYCODE_VERSION_PATCH)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_PUNYCODE_BASE_H */
