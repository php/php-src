/**********************************************************************
  regext.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2017  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#include "regint.h"

static void
conv_ext0be32(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = '\0';
    *conv++ = '\0';
    *conv++ = '\0';
    *conv++ = *s++;
  }
}

static void
conv_ext0le32(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = *s++;
    *conv++ = '\0';
    *conv++ = '\0';
    *conv++ = '\0';
  }
}

static void
conv_ext0be(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = '\0';
    *conv++ = *s++;
  }
}

static void
conv_ext0le(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = *s++;
    *conv++ = '\0';
  }
}

static void
conv_swap4bytes(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = s[3];
    *conv++ = s[2];
    *conv++ = s[1];
    *conv++ = s[0];
    s += 4;
  }
}

static void
conv_swap2bytes(const UChar* s, const UChar* end, UChar* conv)
{
  while (s < end) {
    *conv++ = s[1];
    *conv++ = s[0];
    s += 2;
  }
}

static int
conv_encoding(OnigEncoding from, OnigEncoding to, const UChar* s, const UChar* end,
              UChar** conv, UChar** conv_end)
{
  int len = (int )(end - s);

  if (to == ONIG_ENCODING_UTF16_BE) {
    if (from == ONIG_ENCODING_ASCII || from == ONIG_ENCODING_ISO_8859_1) {
      *conv = (UChar* )xmalloc(len * 2);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + (len * 2);
      conv_ext0be(s, end, *conv);
      return 0;
    }
    else if (from == ONIG_ENCODING_UTF16_LE) {
    swap16:
      *conv = (UChar* )xmalloc(len);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + len;
      conv_swap2bytes(s, end, *conv);
      return 0;
    }
  }
  else if (to == ONIG_ENCODING_UTF16_LE) {
    if (from == ONIG_ENCODING_ASCII || from == ONIG_ENCODING_ISO_8859_1) {
      *conv = (UChar* )xmalloc(len * 2);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + (len * 2);
      conv_ext0le(s, end, *conv);
      return 0;
    }
    else if (from == ONIG_ENCODING_UTF16_BE) {
      goto swap16;
    }
  }
  if (to == ONIG_ENCODING_UTF32_BE) {
    if (from == ONIG_ENCODING_ASCII || from == ONIG_ENCODING_ISO_8859_1) {
      *conv = (UChar* )xmalloc(len * 4);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + (len * 4);
      conv_ext0be32(s, end, *conv);
      return 0;
    }
    else if (from == ONIG_ENCODING_UTF32_LE) {
    swap32:
      *conv = (UChar* )xmalloc(len);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + len;
      conv_swap4bytes(s, end, *conv);
      return 0;
    }
  }
  else if (to == ONIG_ENCODING_UTF32_LE) {
    if (from == ONIG_ENCODING_ASCII || from == ONIG_ENCODING_ISO_8859_1) {
      *conv = (UChar* )xmalloc(len * 4);
      CHECK_NULL_RETURN_MEMERR(*conv);
      *conv_end = *conv + (len * 4);
      conv_ext0le32(s, end, *conv);
      return 0;
    }
    else if (from == ONIG_ENCODING_UTF32_BE) {
      goto swap32;
    }
  }

  return ONIGERR_NOT_SUPPORTED_ENCODING_COMBINATION;
}

extern int
onig_new_deluxe(regex_t** reg, const UChar* pattern, const UChar* pattern_end,
                OnigCompileInfo* ci, OnigErrorInfo* einfo)
{
  int r;
  UChar *cpat, *cpat_end;

  if (IS_NOT_NULL(einfo)) einfo->par = (UChar* )NULL;

  if (ci->pattern_enc != ci->target_enc) {
    r = conv_encoding(ci->pattern_enc, ci->target_enc, pattern, pattern_end,
                      &cpat, &cpat_end);
    if (r != 0) return r;
  }
  else {
    cpat     = (UChar* )pattern;
    cpat_end = (UChar* )pattern_end;
  }

  *reg = (regex_t* )xmalloc(sizeof(regex_t));
  if (IS_NULL(*reg)) {
    r = ONIGERR_MEMORY;
    goto err2;
  }

  r = onig_reg_init(*reg, ci->option, ci->case_fold_flag, ci->target_enc,
		    ci->syntax);
  if (r != 0) goto err;

  r = onig_compile(*reg, cpat, cpat_end, einfo);
  if (r != 0) {
  err:
    onig_free(*reg);
    *reg = NULL;
  }

 err2:
  if (cpat != pattern) xfree(cpat);

  return r;
}
