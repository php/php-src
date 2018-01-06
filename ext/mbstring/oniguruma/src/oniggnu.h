#ifndef ONIGGNU_H
#define ONIGGNU_H
/**********************************************************************
  oniggnu.h - Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2005  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#include "oniguruma.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RE_MBCTYPE_ASCII         0
#define RE_MBCTYPE_EUC           1
#define RE_MBCTYPE_SJIS          2
#define RE_MBCTYPE_UTF8          3

/* GNU regex options */
#ifndef RE_NREGS
#define RE_NREGS                ONIG_NREGION
#endif

#define RE_OPTION_IGNORECASE         ONIG_OPTION_IGNORECASE
#define RE_OPTION_EXTENDED           ONIG_OPTION_EXTEND
#define RE_OPTION_MULTILINE          ONIG_OPTION_MULTILINE
#define RE_OPTION_SINGLELINE         ONIG_OPTION_SINGLELINE
#define RE_OPTION_LONGEST            ONIG_OPTION_FIND_LONGEST
#define RE_OPTION_POSIXLINE         (RE_OPTION_MULTILINE|RE_OPTION_SINGLELINE)
#define RE_OPTION_FIND_NOT_EMPTY     ONIG_OPTION_FIND_NOT_EMPTY
#define RE_OPTION_NEGATE_SINGLELINE  ONIG_OPTION_NEGATE_SINGLELINE
#define RE_OPTION_DONT_CAPTURE_GROUP ONIG_OPTION_DONT_CAPTURE_GROUP
#define RE_OPTION_CAPTURE_GROUP      ONIG_OPTION_CAPTURE_GROUP


ONIG_EXTERN
void re_mbcinit P_((int));
ONIG_EXTERN
int re_compile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
ONIG_EXTERN
int re_recompile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
ONIG_EXTERN
void re_free_pattern P_((struct re_pattern_buffer*));
ONIG_EXTERN
int re_adjust_startpos P_((struct re_pattern_buffer*, const char*, int, int, int));
ONIG_EXTERN
int re_search P_((struct re_pattern_buffer*, const char*, int, int, int, struct re_registers*));
ONIG_EXTERN
int re_match P_((struct re_pattern_buffer*, const char *, int, int, struct re_registers*));
ONIG_EXTERN
void re_set_casetable P_((const char*));
ONIG_EXTERN
void re_free_registers P_((struct re_registers*));
ONIG_EXTERN
int re_alloc_pattern P_((struct re_pattern_buffer**));  /* added */

#ifdef __cplusplus
}
#endif

#endif /* ONIGGNU_H */
