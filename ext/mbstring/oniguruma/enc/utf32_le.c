/**********************************************************************
  utf32_le.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2008  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

static int
utf32le_mbc_enc_len(const UChar* p)
{
  return 4;
}

static int
utf32le_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 3 < end) {
    if (*p == 0x0a && *(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*p == 0x0d || *p == 0x85) && *(p+1) == 0x00
	&& (p+2) == 0x00 && *(p+3) == 0x00)
      return 1;
    if (*(p+1) == 0x20 && (*p == 0x29 || *p == 0x28)
	&& *(p+2) == 0x00 && *(p+3) == 0x00)
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf32le_mbc_to_code(const UChar* p, const UChar* end)
{
  return (OnigCodePoint )(((p[3] * 256 + p[2]) * 256 + p[1]) * 256 + p[0]);
}

static int
utf32le_code_to_mbclen(OnigCodePoint code)
{
  return 4;
}

static int
utf32le_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar* p = buf;

  *p++ = (UChar ) (code & 0xff);
  *p++ = (UChar )((code & 0xff00)     >> 8);
  *p++ = (UChar )((code & 0xff0000)   >>16);
  *p++ = (UChar )((code & 0xff000000) >>24);
  return 4;
}

static int
utf32le_mbc_to_normalize(OnigAmbigType flag, const UChar** pp, const UChar* end,
                         UChar* lower)
{
  const UChar* p = *pp;

  if (*(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0) {
    if (end > p + 7 &&
	(flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0 &&
	((*p == 's' && *(p+4) == 's') ||
	 ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	  (*p == 'S' && *(p+4) == 'S'))) &&
        *(p+5) == 0 && *(p+6) == 0 && *(p+7) == 0) {
      *lower++ = 0xdf;
      *lower++ = '\0';
      *lower++ = '\0';
      *lower   = '\0';
      (*pp) += 8;
      return 4;
    }

    if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 ONIGENC_IS_MBC_ASCII(p)) ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
	 !ONIGENC_IS_MBC_ASCII(p))) {
      *lower++ = ONIGENC_ISO_8859_1_TO_LOWER_CASE(*p);
    }
    else {
      *lower++ = *p;
    }
    *lower++ = '\0';
    *lower++ = '\0';
    *lower   = '\0';

    (*pp) += 4;
    return 4;  /* return byte length of converted char to lower */
  }
  else {
    int len = 4;
    if (lower != p) {
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    (*pp) += len;
    return len; /* return byte length of converted char to lower */
  }
}

static int
utf32le_is_mbc_ambiguous(OnigAmbigType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += 4;

  if (*(p+1) == 0 && *(p+2) == 0 && *(p+3) == 0) {
    int c, v;

    if ((flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0) {
      if (end > p + 7 &&
	  ((*p == 's' && *(p+4) == 's') ||
	   ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	    (*p == 'S' && *(p+4) == 'S'))) &&
          *(p+5) == 0 && *(p+6) == 0 && *(p+7) == 0) {
        (*pp) += 4;
        return TRUE;
      }
      else if (*p == 0xdf) {
        return TRUE;
      }
    }

    if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 ONIGENC_IS_MBC_ASCII(p)) ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
	 !ONIGENC_IS_MBC_ASCII(p))) {
      c = *p;
      v = ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(c,
                       (ONIGENC_CTYPE_UPPER | ONIGENC_CTYPE_LOWER));
      if ((v | ONIGENC_CTYPE_LOWER) != 0) {
        /* 0xaa, 0xb5, 0xba are lower case letter, but can't convert. */
        if (c >= 0xaa && c <= 0xba)
          return FALSE;
        else
          return TRUE;
      }
      return (v != 0 ? TRUE : FALSE);
    }
  }

  return FALSE;
}

static UChar*
utf32le_left_adjust_char_head(const UChar* start, const UChar* s)
{
  int rem;

  if (s <= start) return (UChar* )s;

  rem = (s - start) % 4;
  return (UChar* )(s - rem);
}

OnigEncodingType OnigEncodingUTF32_LE = {
  utf32le_mbc_enc_len,
  "UTF-32LE",   /* name */
  4,            /* max byte length */
  4,            /* min byte length */
  (ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE |
   ONIGENC_AMBIGUOUS_MATCH_COMPOUND),
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  },
  utf32le_is_mbc_newline,
  utf32le_mbc_to_code,
  utf32le_code_to_mbclen,
  utf32le_code_to_mbc,
  utf32le_mbc_to_normalize,
  utf32le_is_mbc_ambiguous,
  onigenc_iso_8859_1_get_all_pair_ambig_codes,
  onigenc_ess_tsett_get_all_comp_ambig_codes,
  onigenc_unicode_is_code_ctype,
  onigenc_unicode_get_ctype_code_range,
  utf32le_left_adjust_char_head,
  onigenc_always_false_is_allowed_reverse_match
};
