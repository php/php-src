/**********************************************************************
  ascii.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2018  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#include "regint.h"   /* for USE_CALLOUT */

static int
init(void)
{
#ifdef USE_CALLOUT

    int id;
    OnigEncoding enc;
    char* name;
    unsigned int args[4];
    OnigValue    opts[4];

    enc = ONIG_ENCODING_ASCII;

    name = "FAIL";        BC0_P(name, fail);
    name = "MISMATCH";    BC0_P(name, mismatch);

    name = "MAX";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_CHAR;
    opts[0].c = 'X';
    BC_B_O(name, max, 2, args, 1, opts);

    name = "ERROR";
    args[0] = ONIG_TYPE_LONG; opts[0].l = ONIG_ABORT;
    BC_P_O(name, error, 1, args, 1, opts);

    name = "COUNT";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, count, 1, args, 1, opts);

    name = "TOTAL_COUNT";
    args[0] = ONIG_TYPE_CHAR; opts[0].c = '>';
    BC_B_O(name, total_count, 1, args, 1, opts);

    name = "CMP";
    args[0] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    args[1] = ONIG_TYPE_STRING;
    args[2] = ONIG_TYPE_TAG | ONIG_TYPE_LONG;
    BC_P(name, cmp, 3, args);

#endif /* USE_CALLOUT */

  return ONIG_NORMAL;
}

#if 0
static int
is_initialized(void)
{
  /* Don't use this function */
  /* can't answer, because builtin callout entries removed in onig_end() */
  return 0;
}
#endif

static int
ascii_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}

OnigEncodingType OnigEncodingASCII = {
  onigenc_single_byte_mbc_enc_len,
  "US-ASCII",  /* name */
  1,           /* max enc length */
  1,           /* min enc length */
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  onigenc_ascii_mbc_case_fold,
  onigenc_ascii_apply_all_case_fold,
  onigenc_ascii_get_case_fold_codes_by_str,
  onigenc_minimum_property_name_to_ctype,
  ascii_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match,
  init,
  0, /* is_initialized */
  onigenc_always_true_is_valid_mbc_string,
  ENC_FLAG_ASCII_COMPATIBLE|ENC_FLAG_SKIP_OFFSET_1,
  0, 0
};
