/**********************************************************************
  iso8859_1.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2006  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define ENC_IS_ISO_8859_1_CTYPE(code,ctype) \
  ((EncISO_8859_1_CtypeTable[code] & ctype) != 0)

static const unsigned short EncISO_8859_1_CtypeTable[256] = {
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
  0x0284, 0x01a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10e2, 0x01a0, 0x00a0, 0x01a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10a0, 0x10a0, 0x00a0, 0x10e2, 0x00a0, 0x01a0,
  0x00a0, 0x10a0, 0x10e2, 0x01a0, 0x10a0, 0x10a0, 0x10a0, 0x01a0,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x00a0,
  0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x14a2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x00a0,
  0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2, 0x10e2
};

static int
iso_8859_1_mbc_to_normalize(OnigAmbigType flag, const UChar** pp, const UChar* end, UChar* lower)
{
  const UChar* p = *pp;

  if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
       ONIGENC_IS_MBC_ASCII(p)) ||
      ((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
       !ONIGENC_IS_MBC_ASCII(p))) {
    *lower = ONIGENC_ISO_8859_1_TO_LOWER_CASE(*p);
  }
  else {
    *lower = *p;
  }
  (*pp)++;
  return 1; /* return byte length of converted char to lower */
}

static int
iso_8859_1_is_mbc_ambiguous(OnigAmbigType flag,
			    const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp)++;
  if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
       ONIGENC_IS_MBC_ASCII(p)) ||
      ((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
       !ONIGENC_IS_MBC_ASCII(p))) {
    int v = (EncISO_8859_1_CtypeTable[*p] &
             (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER));

    if ((v | ONIGENC_CTYPE_LOWER) != 0) {
      /* 0xdf, 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
      if (*p == 0xdf || (*p >= 0xaa && *p <= 0xba))
        return FALSE;
      else
        return TRUE;
    }

    return (v != 0 ? TRUE : FALSE);
  }
  return FALSE;
}

static int
iso_8859_1_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256)
    return ENC_IS_ISO_8859_1_CTYPE(code, ctype);
  else
    return FALSE;
}

OnigEncodingType OnigEncodingISO_8859_1 = {
  onigenc_single_byte_mbc_enc_len,
  "ISO-8859-1",  /* name */
  1,             /* max enc length */
  1,             /* min enc length */
  (ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE ),
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
  iso_8859_1_mbc_to_normalize,
  iso_8859_1_is_mbc_ambiguous,
  onigenc_iso_8859_1_get_all_pair_ambig_codes,
  onigenc_ess_tsett_get_all_comp_ambig_codes,
  iso_8859_1_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  onigenc_single_byte_left_adjust_char_head,
  onigenc_always_true_is_allowed_reverse_match
};
