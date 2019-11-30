/**********************************************************************
  unicode.c -  Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2019  K.Kosako
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

struct PoolPropertyNameCtype {
  short int name;
  short int ctype;
};

#define ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(code,ctype) \
  ((EncUNICODE_ISO_8859_1_CtypeTable[code] & CTYPE_TO_BIT(ctype)) != 0)

static const unsigned short EncUNICODE_ISO_8859_1_CtypeTable[256] = {
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4008, 0x428c, 0x4289, 0x4288, 0x4288, 0x4288, 0x4008, 0x4008,
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008, 0x4008,
  0x4284, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0, 0x78b0,
  0x78b0, 0x78b0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x41a0,
  0x41a0, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x7ca2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2, 0x74a2,
  0x74a2, 0x74a2, 0x74a2, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x51a0,
  0x41a0, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x78e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2, 0x70e2,
  0x70e2, 0x70e2, 0x70e2, 0x41a0, 0x41a0, 0x41a0, 0x41a0, 0x4008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0288, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008, 0x0008,
  0x0284, 0x01a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x30e2, 0x01a0, 0x00a0, 0x00a8, 0x00a0, 0x00a0,
  0x00a0, 0x00a0, 0x10a0, 0x10a0, 0x00a0, 0x30e2, 0x00a0, 0x01a0,
  0x00a0, 0x10a0, 0x30e2, 0x01a0, 0x10a0, 0x10a0, 0x10a0, 0x01a0,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x00a0,
  0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x34a2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x00a0,
  0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2, 0x30e2
};

#include "st.h"

#include "unicode_fold_data.c"

extern int
onigenc_unicode_mbc_case_fold(OnigEncoding enc,
    OnigCaseFoldType flag ARG_UNUSED, const UChar** pp, const UChar* end,
    UChar* fold)
{
  const struct ByUnfoldKey* buk;

  OnigCodePoint code;
  int i, len, rlen;
  const UChar *p = *pp;

  code = ONIGENC_MBC_TO_CODE(enc, p, end);
  len = enclen(enc, p);
  *pp += len;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    if (code == 0x0130) {
      return ONIGENC_CODE_TO_MBC(enc, 0x0069, fold);
    }
#if 0
    if (code == 0x0049) {
      return ONIGENC_CODE_TO_MBC(enc, 0x0131, fold);
    }
#endif
  }
#endif

  buk = onigenc_unicode_unfold_key(code);
  if (buk != 0) {
    if (buk->fold_len == 1) {
      return ONIGENC_CODE_TO_MBC(enc, *FOLDS1_FOLD(buk->index), fold);
    }
    else {
      OnigCodePoint* addr;

      FOLDS_FOLD_ADDR_BUK(buk, addr);
      rlen = 0;
      for (i = 0; i < buk->fold_len; i++) {
        OnigCodePoint c = addr[i];
        len = ONIGENC_CODE_TO_MBC(enc, c, fold);
        fold += len;
        rlen += len;
      }
      return rlen;
    }
  }

  for (i = 0; i < len; i++) {
    *fold++ = *p++;
  }
  return len;
}

static int
apply_case_fold1(int from, int to, OnigApplyAllCaseFoldFunc f, void* arg)
{
  int i, j, k, n, r;

  for (i = from; i < to; ) {
    OnigCodePoint fold = *FOLDS1_FOLD(i);
    n = FOLDS1_UNFOLDS_NUM(i);
    for (j = 0; j < n; j++) {
      OnigCodePoint unfold = FOLDS1_UNFOLDS(i)[j];

      r = (*f)(fold, &unfold, 1, arg);
      if (r != 0) return r;
      r = (*f)(unfold, &fold, 1, arg);
      if (r != 0) return r;

      for (k = 0; k < j; k++) {
        OnigCodePoint unfold2 = FOLDS1_UNFOLDS(i)[k];
        r = (*f)(unfold, &unfold2, 1, arg);
        if (r != 0) return r;
        r = (*f)(unfold2, &unfold, 1, arg);
        if (r != 0) return r;
      }
    }

    i = FOLDS1_NEXT_INDEX(i);
  }

  return 0;
}

static int
apply_case_fold2(int from, int to, OnigApplyAllCaseFoldFunc f, void* arg)
{
  int i, j, k, n, r;

  for (i = from; i < to; ) {
    OnigCodePoint* fold = FOLDS2_FOLD(i);
    n = FOLDS2_UNFOLDS_NUM(i);
    for (j = 0; j < n; j++) {
      OnigCodePoint unfold = FOLDS2_UNFOLDS(i)[j];

      r = (*f)(unfold, fold, 2, arg);
      if (r != 0) return r;

      for (k = 0; k < j; k++) {
        OnigCodePoint unfold2 = FOLDS2_UNFOLDS(i)[k];
        r = (*f)(unfold, &unfold2, 1, arg);
        if (r != 0) return r;
        r = (*f)(unfold2, &unfold, 1, arg);
        if (r != 0) return r;
      }
    }

    i = FOLDS2_NEXT_INDEX(i);
  }

  return 0;
}

static int
apply_case_fold3(int from, int to, OnigApplyAllCaseFoldFunc f, void* arg)
{
  int i, j, k, n, r;

  for (i = from; i < to; ) {
    OnigCodePoint* fold = FOLDS3_FOLD(i);
    n = FOLDS3_UNFOLDS_NUM(i);
    for (j = 0; j < n; j++) {
      OnigCodePoint unfold = FOLDS3_UNFOLDS(i)[j];

      r = (*f)(unfold, fold, 3, arg);
      if (r != 0) return r;

      for (k = 0; k < j; k++) {
        OnigCodePoint unfold2 = FOLDS3_UNFOLDS(i)[k];
        r = (*f)(unfold, &unfold2, 1, arg);
        if (r != 0) return r;
        r = (*f)(unfold2, &unfold, 1, arg);
        if (r != 0) return r;
      }
    }

    i = FOLDS3_NEXT_INDEX(i);
  }

  return 0;
}

extern int
onigenc_unicode_apply_all_case_fold(OnigCaseFoldType flag,
                                    OnigApplyAllCaseFoldFunc f, void* arg)
{
  int r;

  r = apply_case_fold1(0, FOLDS1_NORMAL_END_INDEX, f, arg);
  if (r != 0) return r;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    code = 0x0131;
    r = (*f)(0x0049, &code, 1, arg);
    if (r != 0) return r;
    code = 0x0049;
    r = (*f)(0x0131, &code, 1, arg);
    if (r != 0) return r;

    code = 0x0130;
    r = (*f)(0x0069, &code, 1, arg);
    if (r != 0) return r;
    code = 0x0069;
    r = (*f)(0x0130, &code, 1, arg);
    if (r != 0) return r;
  }
  else {
#endif
    r = apply_case_fold1(FOLDS1_NORMAL_END_INDEX, FOLDS1_END_INDEX, f, arg);
    if (r != 0) return r;
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  }
#endif

  if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) == 0)
    return 0;

  r = apply_case_fold2(0, FOLDS2_NORMAL_END_INDEX, f, arg);
  if (r != 0) return r;

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) == 0) {
#endif
    r = apply_case_fold2(FOLDS2_NORMAL_END_INDEX, FOLDS2_END_INDEX, f, arg);
    if (r != 0) return r;
#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  }
#endif

  r = apply_case_fold3(0, FOLDS3_NORMAL_END_INDEX, f, arg);
  if (r != 0) return r;

  return 0;
}

extern int
onigenc_unicode_get_case_fold_codes_by_str(OnigEncoding enc,
    OnigCaseFoldType flag, const OnigUChar* p, const OnigUChar* end,
    OnigCaseFoldCodeItem items[])
{
  int n, m, i, j, k, len;
  OnigCodePoint code, codes[3];
  const struct ByUnfoldKey* buk;

  n = 0;

  code = ONIGENC_MBC_TO_CODE(enc, p, end);
  len = enclen(enc, p);

#ifdef USE_UNICODE_CASE_FOLD_TURKISH_AZERI
  if ((flag & ONIGENC_CASE_FOLD_TURKISH_AZERI) != 0) {
    if (code == 0x0049) {
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 0x0131;
      return 1;
    }
    else if (code == 0x0130) {
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 0x0069;
      return 1;
    }
    else if (code == 0x0131) {
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 0x0049;
      return 1;
    }
    else if (code == 0x0069) {
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = 0x0130;
      return 1;
    }
  }
#endif

  buk = onigenc_unicode_unfold_key(code);
  if (buk != 0) {
    if (buk->fold_len == 1) {
      int un;
      items[0].byte_len = len;
      items[0].code_len = 1;
      items[0].code[0]  = *FOLDS1_FOLD(buk->index);
      n++;

      un = FOLDS1_UNFOLDS_NUM(buk->index);
      for (i = 0; i < un; i++) {
        OnigCodePoint unfold = FOLDS1_UNFOLDS(buk->index)[i];
        if (unfold != code) {
          items[n].byte_len = len;
          items[n].code_len = 1;
          items[n].code[0]  = unfold;
          n++;
        }
      }
      code = items[0].code[0]; /* for multi-code to unfold search. */
    }
    else if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
      OnigCodePoint cs[3][4];
      int fn, ncs[3];

      if (buk->fold_len == 2) {
        m = FOLDS2_UNFOLDS_NUM(buk->index);
        for (i = 0; i < m; i++) {
          OnigCodePoint unfold = FOLDS2_UNFOLDS(buk->index)[i];
          if (unfold == code) continue;

          items[n].byte_len = len;
          items[n].code_len = 1;
          items[n].code[0]  = unfold;
          n++;
        }

        for (fn = 0; fn < 2; fn++) {
          int index;
          cs[fn][0] = FOLDS2_FOLD(buk->index)[fn];
          ncs[fn] = 1;
          index = onigenc_unicode_fold1_key(&cs[fn][0]);
          if (index >= 0) {
            int m = FOLDS1_UNFOLDS_NUM(index);
            for (i = 0; i < m; i++) {
              cs[fn][i+1] = FOLDS1_UNFOLDS(index)[i];
            }
            ncs[fn] += m;
          }
        }

        for (i = 0; i < ncs[0]; i++) {
          for (j = 0; j < ncs[1]; j++) {
            items[n].byte_len = len;
            items[n].code_len = 2;
            items[n].code[0]  = cs[0][i];
            items[n].code[1]  = cs[1][j];
            n++;
          }
        }
      }
      else { /* fold_len == 3 */
        m = FOLDS3_UNFOLDS_NUM(buk->index);
        for (i = 0; i < m; i++) {
          OnigCodePoint unfold = FOLDS3_UNFOLDS(buk->index)[i];
          if (unfold == code) continue;

          items[n].byte_len = len;
          items[n].code_len = 1;
          items[n].code[0]  = unfold;
          n++;
        }

        for (fn = 0; fn < 3; fn++) {
          int index;
          cs[fn][0] = FOLDS3_FOLD(buk->index)[fn];
          ncs[fn] = 1;
          index = onigenc_unicode_fold1_key(&cs[fn][0]);
          if (index >= 0) {
            int m = FOLDS1_UNFOLDS_NUM(index);
            for (i = 0; i < m; i++) {
              cs[fn][i+1] = FOLDS1_UNFOLDS(index)[i];
            }
            ncs[fn] += m;
          }
        }

        for (i = 0; i < ncs[0]; i++) {
          for (j = 0; j < ncs[1]; j++) {
            for (k = 0; k < ncs[2]; k++) {
              items[n].byte_len = len;
              items[n].code_len = 3;
              items[n].code[0]  = cs[0][i];
              items[n].code[1]  = cs[1][j];
              items[n].code[2]  = cs[2][k];
              n++;
            }
          }
        }
      }

      /* multi char folded code is not head of another folded multi char */
      return n;
    }
  }
  else {
    int index = onigenc_unicode_fold1_key(&code);
    if (index >= 0) {
      int m = FOLDS1_UNFOLDS_NUM(index);
      for (i = 0; i < m; i++) {
        items[n].byte_len = len;
        items[n].code_len = 1;
        items[n].code[0]  = FOLDS1_UNFOLDS(index)[i];
        n++;
      }
    }
  }

  if ((flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) == 0)
    return n;

  p += len;
  if (p < end) {
    int clen;
    int index;

    codes[0] = code;
    code = ONIGENC_MBC_TO_CODE(enc, p, end);

    buk = onigenc_unicode_unfold_key(code);
    if (buk != 0 && buk->fold_len == 1) {
      codes[1] = *FOLDS1_FOLD(buk->index);
    }
    else
      codes[1] = code;

    clen = enclen(enc, p);
    len += clen;

    index = onigenc_unicode_fold2_key(codes);
    if (index >= 0) {
      m = FOLDS2_UNFOLDS_NUM(index);
      for (i = 0; i < m; i++) {
        items[n].byte_len = len;
        items[n].code_len = 1;
        items[n].code[0]  = FOLDS2_UNFOLDS(index)[i];
        n++;
      }
    }

    p += clen;
    if (p < end) {
      code = ONIGENC_MBC_TO_CODE(enc, p, end);
      buk = onigenc_unicode_unfold_key(code);
      if (buk != 0 && buk->fold_len == 1) {
        codes[2] = *FOLDS1_FOLD(buk->index);
      }
      else
        codes[2] = code;

      clen = enclen(enc, p);
      len += clen;

      index = onigenc_unicode_fold3_key(codes);
      if (index >= 0) {
        m = FOLDS3_UNFOLDS_NUM(index);
        for (i = 0; i < m; i++) {
          items[n].byte_len = len;
          items[n].code_len = 1;
          items[n].code[0]  = FOLDS3_UNFOLDS(index)[i];
          n++;
        }
      }
    }
  }

  return n;
}

#ifdef USE_UNICODE_PROPERTIES
#include "unicode_property_data.c"
#else
#include "unicode_property_data_posix.c"
#endif


#ifdef USE_UNICODE_WORD_BREAK

enum WB_TYPE {
  WB_Any = 0,
  WB_ALetter,
  WB_CR,
  WB_Double_Quote,
  WB_Extend,
  WB_ExtendNumLet,
  WB_Format,
  WB_Hebrew_Letter,
  WB_Katakana,
  WB_LF,
  WB_MidLetter,
  WB_MidNum,
  WB_MidNumLet,
  WB_Newline,
  WB_Numeric,
  WB_Regional_Indicator,
  WB_Single_Quote,
  WB_WSegSpace,
  WB_ZWJ,
};

typedef struct {
  OnigCodePoint start;
  OnigCodePoint end;
  enum WB_TYPE  type;
} WB_RANGE_TYPE;

#include "unicode_wb_data.c"

static enum WB_TYPE
wb_get_type(OnigCodePoint code)
{
  OnigCodePoint low, high, x;
  enum WB_TYPE type;

  for (low = 0, high = (OnigCodePoint )WB_RANGE_NUM; low < high; ) {
    x = (low + high) >> 1;
    if (code > WB_RANGES[x].end)
      low = x + 1;
    else
      high = x;
  }

  type = (low < (OnigCodePoint )WB_RANGE_NUM &&
          code >= WB_RANGES[low].start) ?
    WB_RANGES[low].type : WB_Any;

  return type;
}

#define IS_WB_IGNORE_TAIL(t)  ((t) == WB_Extend || (t) == WB_Format || (t) == WB_ZWJ)
#define IS_WB_AHLetter(t)     ((t) == WB_ALetter || (t) == WB_Hebrew_Letter)
#define IS_WB_MidNumLetQ(t)   ((t) == WB_MidNumLet || (t) == WB_Single_Quote)

static int
wb_get_next_main_code(OnigEncoding enc, UChar* p, const UChar* end,
                      OnigCodePoint* rcode, enum WB_TYPE* rtype)
{
  OnigCodePoint code;
  enum WB_TYPE type;

  while (TRUE) {
    p += enclen(enc, p);
    if (p >= end) break;

    code = ONIGENC_MBC_TO_CODE(enc, p, end);
    type = wb_get_type(code);
    if (! IS_WB_IGNORE_TAIL(type)) {
      *rcode = code;
      *rtype = type;
      return 1;
    }
  }

  return 0;
}

extern int
onigenc_wb_is_break_position(OnigEncoding enc, UChar* p, UChar* prev,
                             const UChar* start, const UChar* end)
{
  int r;
  UChar* pp;
  OnigCodePoint cfrom;
  OnigCodePoint cfrom2;
  OnigCodePoint cto;
  OnigCodePoint cto2;
  enum WB_TYPE from;
  enum WB_TYPE from2;
  enum WB_TYPE to;
  enum WB_TYPE to2;

  /* WB1: sot / Any */
  if (p == start) return TRUE;
  /* WB2: Any / eot */
  if (p == end)   return TRUE;

  if (IS_NULL(prev)) {
    prev = onigenc_get_prev_char_head(enc, start, p);
    if (IS_NULL(prev)) return TRUE;
  }

  cfrom = ONIGENC_MBC_TO_CODE(enc, prev, end);
  cto   = ONIGENC_MBC_TO_CODE(enc, p, end);

  from = wb_get_type(cfrom);
  to   = wb_get_type(cto);

  /* short cut */
  if (from == 0 && to == 0) goto WB999;

  /* WB3: CR + LF */
  if (from == WB_CR && to == WB_LF) return FALSE;

  /* WB3a: (Newline|CR|LF) /  */
  if (from == WB_Newline || from == WB_CR || from == WB_LF) return TRUE;
  /* WB3b: / (Newline|CR|LF) */
  if (to == WB_Newline || to == WB_CR || to == WB_LF) return TRUE;

  /* WB3c: ZWJ + {Extended_Pictographic} */
  if (from == WB_ZWJ) {
    if (onigenc_unicode_is_code_ctype(cto, PROP_INDEX_EXTENDEDPICTOGRAPHIC))
      return FALSE;
  }

  /* WB3d: WSegSpace + WSegSpace */
  if (from == WB_WSegSpace && to == WB_WSegSpace) return FALSE;

  /* WB4:  X (Extend|Format|ZWJ)* -> X */
  if (IS_WB_IGNORE_TAIL(to)) return FALSE;
  if (IS_WB_IGNORE_TAIL(from)) {
    while ((pp = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
      prev = pp;
      cfrom = ONIGENC_MBC_TO_CODE(enc, prev, end);
      from = wb_get_type(cfrom);
      if (! IS_WB_IGNORE_TAIL(from))
        break;
    }
  }

  if (IS_WB_AHLetter(from)) {
    /* WB5: AHLetter + AHLetter */
    if (IS_WB_AHLetter(to)) return FALSE;

    /* WB6: AHLetter + (MidLetter | MidNumLetQ) AHLetter */
    if (to == WB_MidLetter || IS_WB_MidNumLetQ(to)) {
      r = wb_get_next_main_code(enc, p, end, &cto2, &to2);
      if (r == 1) {
        if (IS_WB_AHLetter(to2)) return FALSE;
      }
    }
  }

  /* WB7: AHLetter (MidLetter | MidNumLetQ) + AHLetter */
  if (from == WB_MidLetter || IS_WB_MidNumLetQ(from)) {
    if (IS_WB_AHLetter(to)) {
      from2 = WB_Any;
      while ((pp = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
        prev = pp;
        cfrom2 = ONIGENC_MBC_TO_CODE(enc, prev, end);
        from2 = wb_get_type(cfrom2);
        if (! IS_WB_IGNORE_TAIL(from2))
          break;
      }

      if (IS_WB_AHLetter(from2)) return FALSE;
    }
  }

  if (from == WB_Hebrew_Letter) {
    /* WB7a: Hebrew_Letter + Single_Quote */
    if (to == WB_Single_Quote) return FALSE;

    /* WB7b: Hebrew_Letter + Double_Quote Hebrew_Letter */
    if (to == WB_Double_Quote) {
      r = wb_get_next_main_code(enc, p, end, &cto2, &to2);
      if (r == 1) {
        if (to2 == WB_Hebrew_Letter) return FALSE;
      }
    }
  }

  /* WB7c: Hebrew_Letter Double_Quote + Hebrew_Letter */
  if (from == WB_Double_Quote) {
    if (to == WB_Hebrew_Letter) {
      from2 = WB_Any;
      while ((pp = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
        prev = pp;
        cfrom2 = ONIGENC_MBC_TO_CODE(enc, prev, end);
        from2 = wb_get_type(cfrom2);
        if (! IS_WB_IGNORE_TAIL(from2))
          break;
      }

      if (from2 == WB_Hebrew_Letter) return FALSE;
    }
  }

  if (to == WB_Numeric) {
    /* WB8: Numeric + Numeric */
    if (from == WB_Numeric) return FALSE;

    /* WB9: AHLetter + Numeric */
    if (IS_WB_AHLetter(from)) return FALSE;

    /* WB11: Numeric (MidNum | MidNumLetQ) + Numeric */
    if (from == WB_MidNum || IS_WB_MidNumLetQ(from)) {
      from2 = WB_Any;
      while ((pp = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
        prev = pp;
        cfrom2 = ONIGENC_MBC_TO_CODE(enc, prev, end);
        from2 = wb_get_type(cfrom2);
        if (! IS_WB_IGNORE_TAIL(from2))
          break;
      }

      if (from2 == WB_Numeric) return FALSE;
    }
  }

  if (from == WB_Numeric) {
    /* WB10: Numeric + AHLetter */
    if (IS_WB_AHLetter(to)) return FALSE;

    /* WB12: Numeric + (MidNum | MidNumLetQ) Numeric */
    if (to == WB_MidNum || IS_WB_MidNumLetQ(to)) {
      r = wb_get_next_main_code(enc, p, end, &cto2, &to2);
      if (r == 1) {
        if (to2 == WB_Numeric) return FALSE;
      }
    }
  }

  /* WB13: Katakana + Katakana */
  if (from == WB_Katakana && to == WB_Katakana) return FALSE;

  /* WB13a: (AHLetter | Numeric | Katakana | ExtendNumLet) + ExtendNumLet */
  if (IS_WB_AHLetter(from) || from == WB_Numeric || from == WB_Katakana
      || from == WB_ExtendNumLet) {
    if (to == WB_ExtendNumLet) return FALSE;
  }

  /* WB13b: ExtendNumLet + (AHLetter | Numeric | Katakana) */
  if (from == WB_ExtendNumLet) {
    if (IS_WB_AHLetter(to) || to == WB_Numeric || to == WB_Katakana)
      return FALSE;
  }


  /* WB15:   sot (RI RI)* RI + RI */
  /* WB16: [^RI] (RI RI)* RI + RI */
  if (from == WB_Regional_Indicator && to == WB_Regional_Indicator) {
    int n = 0;
    while ((prev = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
      cfrom2 = ONIGENC_MBC_TO_CODE(enc, prev, end);
      from2  = wb_get_type(cfrom2);
      if (from2 != WB_Regional_Indicator)
        break;

      n++;
    }
    if ((n % 2) == 0) return FALSE;
  }

 WB999:
  /* WB999: Any / Any */
  return TRUE;
}

#endif /* USE_UNICODE_WORD_BREAK */


#ifdef USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER

enum EGCB_BREAK_TYPE {
  EGCB_NOT_BREAK = 0,
  EGCB_BREAK     = 1,
  EGCB_BREAK_UNDEF_GB11  = 2,
  EGCB_BREAK_UNDEF_RI_RI = 3
};

enum EGCB_TYPE {
  EGCB_Other   = 0,
  EGCB_CR      = 1,
  EGCB_LF      = 2,
  EGCB_Control = 3,
  EGCB_Extend  = 4,
  EGCB_Prepend = 5,
  EGCB_Regional_Indicator = 6,
  EGCB_SpacingMark = 7,
  EGCB_ZWJ         = 8,
#if 0
  /* obsoleted */
  EGCB_E_Base         = 9,
  EGCB_E_Base_GAZ     = 10,
  EGCB_E_Modifier     = 11,
  EGCB_Glue_After_Zwj = 12,
#endif
  EGCB_L   = 13,
  EGCB_LV  = 14,
  EGCB_LVT = 15,
  EGCB_T   = 16,
  EGCB_V   = 17
};

typedef struct {
  OnigCodePoint  start;
  OnigCodePoint  end;
  enum EGCB_TYPE type;
} EGCB_RANGE_TYPE;

#include "unicode_egcb_data.c"

static enum EGCB_TYPE
egcb_get_type(OnigCodePoint code)
{
  OnigCodePoint low, high, x;
  enum EGCB_TYPE type;

  for (low = 0, high = (OnigCodePoint )EGCB_RANGE_NUM; low < high; ) {
    x = (low + high) >> 1;
    if (code > EGCB_RANGES[x].end)
      low = x + 1;
    else
      high = x;
  }

  type = (low < (OnigCodePoint )EGCB_RANGE_NUM &&
          code >= EGCB_RANGES[low].start) ?
    EGCB_RANGES[low].type : EGCB_Other;

  return type;
}

#define IS_CONTROL_CR_LF(code)   ((code) <= EGCB_Control && (code) >= EGCB_CR)
#define IS_HANGUL(code)          ((code) >= EGCB_L)

/* GB1 and GB2 are outside of this function. */
static enum EGCB_BREAK_TYPE
unicode_egcb_is_break_2code(OnigCodePoint from_code, OnigCodePoint to_code)
{
  enum EGCB_TYPE from;
  enum EGCB_TYPE to;

  from = egcb_get_type(from_code);
  to   = egcb_get_type(to_code);

  /* short cut */
  if (from == 0 && to == 0) goto GB999;

  /* GB3 */
  if (from == EGCB_CR && to == EGCB_LF) return EGCB_NOT_BREAK;
  /* GB4 */
  if (IS_CONTROL_CR_LF(from)) return EGCB_BREAK;
  /* GB5 */
  if (IS_CONTROL_CR_LF(to)) return EGCB_BREAK;

  if (IS_HANGUL(from) && IS_HANGUL(to)) {
    /* GB6 */
    if (from == EGCB_L && to != EGCB_T) return EGCB_NOT_BREAK;
    /* GB7 */
    if ((from == EGCB_LV || from == EGCB_V)
        && (to == EGCB_V || to == EGCB_T)) return EGCB_NOT_BREAK;

    /* GB8 */
    if ((to == EGCB_T) && (from == EGCB_LVT || from == EGCB_T))
      return EGCB_NOT_BREAK;

    goto GB999;
  }

  /* GB9 */
  if (to == EGCB_Extend || to == EGCB_ZWJ) return EGCB_NOT_BREAK;

  /* GB9a */
  if (to == EGCB_SpacingMark) return EGCB_NOT_BREAK;
  /* GB9b */
  if (from == EGCB_Prepend) return EGCB_NOT_BREAK;

  /* GB10 removed */

  /* GB11 */
  if (from == EGCB_ZWJ) {
    if (onigenc_unicode_is_code_ctype(to_code, PROP_INDEX_EXTENDEDPICTOGRAPHIC))
      return EGCB_BREAK_UNDEF_GB11;

    goto GB999;
  }

  /* GB12, GB13 */
  if (from == EGCB_Regional_Indicator && to == EGCB_Regional_Indicator) {
    return EGCB_BREAK_UNDEF_RI_RI;
  }

 GB999:
  return EGCB_BREAK;
}

#endif /* USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER */

extern int
onigenc_egcb_is_break_position(OnigEncoding enc, UChar* p, UChar* prev,
                               const UChar* start, const UChar* end)
{
  OnigCodePoint from;
  OnigCodePoint to;
#ifdef USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER
  enum EGCB_BREAK_TYPE btype;
  enum EGCB_TYPE type;
#endif

  /* GB1 and GB2 */
  if (p == start) return 1;
  if (p == end)   return 1;

  if (IS_NULL(prev)) {
    prev = onigenc_get_prev_char_head(enc, start, p);
    if (IS_NULL(prev)) return 1;
  }

  from = ONIGENC_MBC_TO_CODE(enc, prev, end);
  to   = ONIGENC_MBC_TO_CODE(enc, p, end);

#ifdef USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER
  if (! ONIGENC_IS_UNICODE_ENCODING(enc)) {
    return from != 0x000d || to != 0x000a;
  }

  btype = unicode_egcb_is_break_2code(from, to);
  switch (btype) {
  case EGCB_NOT_BREAK:
    return 0;
    break;
  case EGCB_BREAK:
    return 1;
    break;

  case EGCB_BREAK_UNDEF_GB11:
    while ((prev = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
      from = ONIGENC_MBC_TO_CODE(enc, prev, end);
      if (onigenc_unicode_is_code_ctype(from, PROP_INDEX_EXTENDEDPICTOGRAPHIC))
        return 0;

      type = egcb_get_type(from);
      if (type != EGCB_Extend)
        break;
    }
    break;

  case EGCB_BREAK_UNDEF_RI_RI:
    {
      int n = 0;
      while ((prev = onigenc_get_prev_char_head(enc, start, prev)) != NULL) {
        from = ONIGENC_MBC_TO_CODE(enc, prev, end);
        type = egcb_get_type(from);
        if (type != EGCB_Regional_Indicator)
          break;

        n++;
      }
      if ((n % 2) == 0) return 0;
    }
    break;
  }

  return 1;

#else
  return from != 0x000d || to != 0x000a;
#endif /* USE_UNICODE_EXTENDED_GRAPHEME_CLUSTER */
}


#define USER_DEFINED_PROPERTY_MAX_NUM  20

typedef struct {
  int ctype;
  OnigCodePoint* ranges;
} UserDefinedPropertyValue;

static int UserDefinedPropertyNum;
static UserDefinedPropertyValue
UserDefinedPropertyRanges[USER_DEFINED_PROPERTY_MAX_NUM];
static st_table* UserDefinedPropertyTable;

extern int
onig_unicode_define_user_property(const char* name, OnigCodePoint* ranges)
{
  UserDefinedPropertyValue* e;
  int r;
  int i;
  int n;
  int len;
  int c;
  char* s;
  UChar* uname;

  if (UserDefinedPropertyNum >= USER_DEFINED_PROPERTY_MAX_NUM)
    return ONIGERR_TOO_MANY_USER_DEFINED_OBJECTS;

  len = (int )strlen(name);
  if (len >= PROPERTY_NAME_MAX_SIZE)
    return ONIGERR_TOO_LONG_PROPERTY_NAME;

  s = (char* )xmalloc(len + 1);
  if (s == 0)
    return ONIGERR_MEMORY;

  uname = (UChar* )name;
  n = 0;
  for (i = 0; i < len; i++) {
    c = uname[i];
    if (c < 0x20 || c >= 0x80) {
      xfree(s);
      return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
    }

    if (c != ' ' && c != '-' && c != '_') {
      s[n] = c;
      n++;
    }
  }
  s[n] = '\0';

  if (UserDefinedPropertyTable == 0) {
    UserDefinedPropertyTable = onig_st_init_strend_table_with_size(10);
    if (IS_NULL(UserDefinedPropertyTable)) {
      xfree(s);
      return ONIGERR_MEMORY;
    }
  }

  e = UserDefinedPropertyRanges + UserDefinedPropertyNum;
  e->ctype = CODE_RANGES_NUM + UserDefinedPropertyNum;
  e->ranges = ranges;
  r = onig_st_insert_strend(UserDefinedPropertyTable,
                            (const UChar* )s, (const UChar* )s + n,
                            (hash_data_type )((void* )e));
  if (r < 0) return r;

  UserDefinedPropertyNum++;
  return 0;
}

extern int
onigenc_unicode_is_code_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (
#ifdef USE_UNICODE_PROPERTIES
      ctype <= ONIGENC_MAX_STD_CTYPE &&
#endif
      code < 256) {
    return ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(code, ctype);
  }

  if (ctype >= CODE_RANGES_NUM) {
    int index = ctype - CODE_RANGES_NUM;
    if (index < UserDefinedPropertyNum)
      return onig_is_in_code_range((UChar* )UserDefinedPropertyRanges[index].ranges, code);
    else
      return ONIGERR_TYPE_BUG;
  }

  return onig_is_in_code_range((UChar* )CodeRanges[ctype], code);
}


extern int
onigenc_unicode_ctype_code_range(OnigCtype ctype, const OnigCodePoint* ranges[])
{
  if (ctype >= CODE_RANGES_NUM) {
    int index = ctype - CODE_RANGES_NUM;
    if (index < UserDefinedPropertyNum) {
      *ranges = UserDefinedPropertyRanges[index].ranges;
      return 0;
    }
    else
      return ONIGERR_TYPE_BUG;
  }

  *ranges = CodeRanges[ctype];
  return 0;
}

extern int
onigenc_utf16_32_get_ctype_code_range(OnigCtype ctype, OnigCodePoint* sb_out,
                                      const OnigCodePoint* ranges[])
{
  *sb_out = 0x00;
  return onigenc_unicode_ctype_code_range(ctype, ranges);
}

extern int
onigenc_unicode_property_name_to_ctype(OnigEncoding enc, UChar* name, UChar* end)
{
  int len;
  UChar *p;
  OnigCodePoint code;
  const struct PoolPropertyNameCtype* pc;
  char buf[PROPERTY_NAME_MAX_SIZE];

  p = name;
  len = 0;
  while (p < end) {
    code = ONIGENC_MBC_TO_CODE(enc, p, end);
    if (code >= 0x80)
      return ONIGERR_INVALID_CHAR_PROPERTY_NAME;

    if (code != ' ' && code != '-' && code != '_') {
      buf[len++] = (char )code;
      if (len >= PROPERTY_NAME_MAX_SIZE)
        return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
    }

    p += enclen(enc, p);
  }

  buf[len] = 0;

  if (UserDefinedPropertyTable != 0) {
    UserDefinedPropertyValue* e;
    e = (UserDefinedPropertyValue* )NULL;
    onig_st_lookup_strend(UserDefinedPropertyTable,
                          (const UChar* )buf, (const UChar* )buf + len,
                          (hash_data_type* )((void* )(&e)));
    if (e != 0) {
      return e->ctype;
    }
  }

  pc = unicode_lookup_property_name(buf, len);
  if (pc != 0) {
    /* fprintf(stderr, "LOOKUP: %s: %d\n", buf, pc->ctype); */
#ifndef USE_UNICODE_PROPERTIES
    if (pc->ctype > ONIGENC_MAX_STD_CTYPE)
      return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
#endif

    return (int )pc->ctype;
  }

  return ONIGERR_INVALID_CHAR_PROPERTY_NAME;
}
