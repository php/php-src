/**********************************************************************
  iso8859_6.c -  Oniguruma (regular expression library)
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

#include "regenc.h"

#define ENC_IS_ISO_8859_6_CTYPE(code,ctype) \
  ((EncISO_8859_6_CtypeTable[code] & ctype) != 0)

static const unsigned short EncISO_8859_6_CtypeTable[256] = {
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x220c, 0x2209, 0x2208, 0x2208, 0x2208, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008, 0x2008,
  0x2284, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0, 0x38b0,
  0x38b0, 0x38b0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x21a0,
  0x21a0, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x3ca2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x31a0,
  0x21a0, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x38e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x21a0, 0x21a0, 0x21a0, 0x21a0, 0x2008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0284, 0x0000, 0x0000, 0x0000, 0x00a0, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x01a0, 0x01a0, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x01a0, 0x0000, 0x0000, 0x0000, 0x01a0,
  0x0000, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
  0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
  0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
  0x10a2, 0x10a2, 0x10a2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
  0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2, 0x10a2,
  0x10a2, 0x10a2, 0x10a2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

static int
iso_8859_6_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256)
    return ENC_IS_ISO_8859_6_CTYPE(code, ctype);
  else
    return FALSE;
}

OnigEncodingType OnigEncodingISO_8859_6 = {
  onigenc_single_byte_mbc_enc_len,
  "ISO-8859-6",  /* name */
  1,             /* max enc length */
  1,             /* min enc length */
  ( ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE ),
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  },
  onigenc_is_mbc_newline_0x0a,
  onigenc_single_byte_mbc_to_code,
  onigenc_single_byte_code_to_mbclen,
  onigenc_single_byte_code_to_mbc,
  onigenc_ascii_mbc_to_normalize,
  onigenc_ascii_is_mbc_ambiguous,
  onigenc_ascii_get_all_pair_ambig_codes,
  onigenc_nothing_get_all_comp_ambig_codes,
  iso_8859_6_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match
};
