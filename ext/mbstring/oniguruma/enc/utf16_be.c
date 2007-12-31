/**********************************************************************
  utf16_be.c -  Oniguruma (regular expression library)
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

#define UTF16_IS_SURROGATE_FIRST(c)    (c >= 0xd8 && c <= 0xdb)
#define UTF16_IS_SURROGATE_SECOND(c)   (c >= 0xdc && c <= 0xdf)

static const int EncLen_UTF16[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

static int
utf16be_mbc_enc_len(const UChar* p)
{
  return EncLen_UTF16[*p];
}

static int
utf16be_is_mbc_newline(const UChar* p, const UChar* end)
{
  if (p + 1 < end) {
    if (*(p+1) == 0x0a && *p == 0x00)
      return 1;
#ifdef USE_UNICODE_ALL_LINE_TERMINATORS
    if ((*(p+1) == 0x0d || *(p+1) == 0x85) && *p == 0x00)
      return 1;
    if (*p == 0x20 && (*(p+1) == 0x29 || *(p+1) == 0x28))
      return 1;
#endif
  }
  return 0;
}

static OnigCodePoint
utf16be_mbc_to_code(const UChar* p, const UChar* end)
{
  OnigCodePoint code;

  if (UTF16_IS_SURROGATE_FIRST(*p)) {
    code = ((((p[0] - 0xd8) << 2) + ((p[1] & 0xc0) >> 6) + 1) << 16)
         + ((((p[1] & 0x3f) << 2) + (p[2] - 0xdc)) << 8)
         + p[3];
  }
  else {
    code = p[0] * 256 + p[1];
  }
  return code;
}

static int
utf16be_code_to_mbclen(OnigCodePoint code)
{
  return (code > 0xffff ? 4 : 2);
}

static int
utf16be_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar* p = buf;

  if (code > 0xffff) {
    unsigned int plane, high;

    plane = code >> 16;
    *p++ = (plane >> 2) + 0xd8;
    high = (code & 0xff00) >> 8;
    *p++ = ((plane & 0x03) << 6) + (high >> 2);
    *p++ = (high & 0x02) + 0xdc;
    *p   = (UChar )(code & 0xff);
    return 4;
  }
  else {
    *p++ = (UChar )((code & 0xff00) >> 8);
    *p++ = (UChar )(code & 0xff);
    return 2;
  }
}

static int
utf16be_mbc_to_normalize(OnigAmbigType flag, const UChar** pp, const UChar* end,
                         UChar* lower)
{
  const UChar* p = *pp;

  if (*p == 0) {
    p++;
    if (end > p + 2 &&
	(flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0 &&
	((*p == 's' && *(p+2) == 's') ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 (*p == 'S' && *(p+2) == 'S'))) &&
        *(p+1) == 0) {
      *lower++ = '\0';
      *lower   = 0xdf;
      (*pp) += 4;
      return 2;
    }

    *lower++ = '\0';
    if (((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	 ONIGENC_IS_MBC_ASCII(p)) ||
	((flag & ONIGENC_AMBIGUOUS_MATCH_NONASCII_CASE) != 0 &&
	 !ONIGENC_IS_MBC_ASCII(p))) {
      *lower = ONIGENC_ISO_8859_1_TO_LOWER_CASE(*p);
    }
    else {
      *lower = *p;
    }

    (*pp) += 2;
    return 2;  /* return byte length of converted char to lower */
  }
  else {
    int len;
    len = EncLen_UTF16[*p];
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
utf16be_is_mbc_ambiguous(OnigAmbigType flag, const UChar** pp, const UChar* end)
{
  const UChar* p = *pp;

  (*pp) += EncLen_UTF16[*p];

  if (*p == 0) {
    int c, v;

    p++;
    if ((flag & ONIGENC_AMBIGUOUS_MATCH_COMPOUND) != 0) {
      if (end > p + 2 &&
	  ((*p == 's' && *(p+2) == 's') ||
	   ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0 &&
	    (*p == 'S' && *(p+2) == 'S'))) &&
          *(p+1) == 0) {
        (*pp) += 2;
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
utf16be_left_adjust_char_head(const UChar* start, const UChar* s)
{
  if (s <= start) return (UChar* )s;

  if ((s - start) % 2 == 1) {
    s--;
  }

  if (UTF16_IS_SURROGATE_SECOND(*s) && s > start + 1)
    s -= 2;

  return (UChar* )s;
}

OnigEncodingType OnigEncodingUTF16_BE = {
  utf16be_mbc_enc_len,
  "UTF-16BE",   /* name */
  4,            /* max byte length */
  2,            /* min byte length */
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
  utf16be_is_mbc_newline,
  utf16be_mbc_to_code,
  utf16be_code_to_mbclen,
  utf16be_code_to_mbc,
  utf16be_mbc_to_normalize,
  utf16be_is_mbc_ambiguous,
  onigenc_iso_8859_1_get_all_pair_ambig_codes,
  onigenc_ess_tsett_get_all_comp_ambig_codes,
  onigenc_unicode_is_code_ctype,
  onigenc_unicode_get_ctype_code_range,
  utf16be_left_adjust_char_head,
  onigenc_always_false_is_allowed_reverse_match
};
