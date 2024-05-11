/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef LEXBOR_PERF_H
#define LEXBOR_PERF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


#ifdef LEXBOR_WITH_PERF


LXB_API void *
lexbor_perf_create(void);

LXB_API void
lexbor_perf_clean(void *perf);

LXB_API void
lexbor_perf_destroy(void *perf);

LXB_API lxb_status_t
lexbor_perf_begin(void *perf);

LXB_API lxb_status_t
lexbor_perf_end(void *perf);

LXB_API double
lexbor_perf_in_sec(void *perf);


#endif /* LEXBOR_WITH_PERF */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_PERF_H */
