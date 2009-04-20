/**********************************************************************
  euc_jp.c -  Oniguruma (regular expression library)
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

#define eucjp_islead(c)    ((UChar )((c) - 0xa1) > 0xfe - 0xa1)

static const int EncLen_EUCJP[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

static int
eucjp_mbc_enc_len(const UChar* p)
{
  return EncLen_EUCJP[*p];
}

static OnigCodePoint
eucjp_mbc_to_code(const UChar* p, const UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  len = enc_len(ONIG_ENCODING_EUC_JP, p);
  n = (OnigCodePoint )*p++;
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

static int
eucjp_code_to_mbclen(OnigCodePoint code)
{
  if (ONIGENC_IS_CODE_ASCII(code)) return 1;
  else if ((code & 0xff0000) != 0) return 3;
  else if ((code &   0xff00) != 0) return 2;
  else return 0;
}

#if 0
static int
eucjp_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff0000) != 0) {
    first = (code >> 16) & 0xff;
  }
  else if ((code & 0xff00) != 0) {
    first = (code >> 8) & 0xff;
  }
  else {
    return (int )code;
  }
  return first;
}
#endif

static int
eucjp_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff0000) != 0) *p++ = (UChar )(((code >> 16) & 0xff));
  if ((code &   0xff00) != 0) *p++ = (UChar )(((code >>  8) & 0xff));
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(ONIG_ENCODING_EUC_JP, buf) != (p - buf))
    return ONIGENCERR_INVALID_WIDE_CHAR_VALUE;
#endif  
  return p - buf;
}

static int
eucjp_mbc_to_normalize(OnigAmbigType flag,
		       const UChar** pp, const UChar* end, UChar* lower)
{
  int len;
  const UChar* p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    if ((flag & ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE) != 0) {
      *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    }
    else {
      *lower = *p;
    }

    (*pp)++;
    return 1;
  }
  else {
    len = enc_len(ONIG_ENCODING_EUC_JP, p);
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
eucjp_is_mbc_ambiguous(OnigAmbigType flag, const UChar** pp, const UChar* end)
{
  return onigenc_mbn_is_mbc_ambiguous(ONIG_ENCODING_EUC_JP, flag, pp, end);
}

static int
eucjp_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else {
    if ((ctype & (ONIGENC_CTYPE_WORD |
                  ONIGENC_CTYPE_GRAPH | ONIGENC_CTYPE_PRINT)) != 0) {
      return (eucjp_code_to_mbclen(code) > 1 ? TRUE : FALSE);
    }
  }

  return FALSE;
}

static UChar*
eucjp_left_adjust_char_head(const UChar* start, const UChar* s)
{
  /* In this encoding
     mb-trail bytes doesn't mix with single bytes.
  */
  const UChar *p;
  int len;

  if (s <= start) return (UChar* )s;
  p = s;

  while (!eucjp_islead(*p) && p > start) p--;
  len = enc_len(ONIG_ENCODING_EUC_JP, p);
  if (p + len > s) return (UChar* )p;
  p += len;
  return (UChar* )(p + ((s - p) & ~1));
}

static int
eucjp_is_allowed_reverse_match(const UChar* s, const UChar* end)
{
  const UChar c = *s;
  if (c <= 0x7e || c == 0x8e || c == 0x8f)
    return TRUE;
  else
    return FALSE;
}

OnigEncodingType OnigEncodingEUC_JP = {
  eucjp_mbc_enc_len,
  "EUC-JP",   /* name */
  3,          /* max enc length */
  1,          /* min enc length */
  ONIGENC_AMBIGUOUS_MATCH_ASCII_CASE,
  {
      (OnigCodePoint )'\\'                       /* esc */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar '.'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anytime '*'  */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* zero or one time '?' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* one or more time '+' */
    , (OnigCodePoint )ONIG_INEFFECTIVE_META_CHAR /* anychar anytime */
  },
  onigenc_is_mbc_newline_0x0a,
  eucjp_mbc_to_code,
  eucjp_code_to_mbclen,
  eucjp_code_to_mbc,
  eucjp_mbc_to_normalize,
  eucjp_is_mbc_ambiguous,
  onigenc_ascii_get_all_pair_ambig_codes,
  onigenc_nothing_get_all_comp_ambig_codes,
  eucjp_is_code_ctype,
  onigenc_not_support_get_ctype_code_range,
  eucjp_left_adjust_char_head,
  eucjp_is_allowed_reverse_match
};
