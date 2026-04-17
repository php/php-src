/*
 * Copyright (C) 2015-2019 NGINX, Inc.
 * Copyright (C) 2019-2025 F5, Inc.
 * Copyright (C) 2015-2021 Igor Sysoev
 * Copyright (C) 2017-2025 Dmitry Volyntsev
 * Copyright (C) 2019-2022 Alexander Borisov
 * Copyright (C) 2022-2025 Vadim Zhestikov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (C) Alexander Borisov
 */

#ifndef LEXBOR_DTOA_H
#define LEXBOR_DTOA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lexbor/core/base.h"


LXB_API size_t
lexbor_dtoa(double value, lxb_char_t *begin, size_t len);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LEXBOR_DTOA_H */
