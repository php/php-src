/**********************************************************************

  utf8.c -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

#define utf8_islead(c)     ((UChar )((c) & 0xc0) != 0x80)

#define ENC_IS_ISO_8859_1_CTYPE(code,ctype) \
  ((EncUnicode_ISO_8859_1_CtypeTable[code] & ctype) != 0)

static unsigned short EncUnicode_ISO_8859_1_CtypeTable[256] = {
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1106, 0x1104, 0x1104, 0x1104, 0x1104, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1142, 0x10d0, 0x10d0, 0x10d0, 0x1050, 0x10d0, 0x10d0, 0x10d0,
  0x10d0, 0x10d0, 0x10d0, 0x1050, 0x10d0, 0x10d0, 0x10d0, 0x10d0,
  0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58,
  0x1c58, 0x1c58, 0x10d0, 0x10d0, 0x1050, 0x1050, 0x1050, 0x10d0,
  0x10d0, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x10d0, 0x10d0, 0x10d0, 0x1050, 0x18d0,
  0x1050, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x10d0, 0x1050, 0x10d0, 0x1050, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1142, 0x10d0, 0x1050, 0x1050, 0x1050, 0x1050, 0x1050, 0x1050,
  0x1050, 0x1050, 0x1871, 0x10d0, 0x1050, 0x10d0, 0x1050, 0x1050,
  0x1050, 0x1050, 0x1850, 0x1850, 0x1050, 0x1871, 0x1050, 0x10d0,
  0x1050, 0x1850, 0x1871, 0x10d0, 0x1850, 0x1850, 0x1850, 0x10d0,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1050,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1050,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871
};

static OnigCodePoint
utf8_mbc_to_code(UChar* p, UChar* end)
{
  int c, len;
  OnigCodePoint n;

  c = *p++;
  len = enc_len(ONIG_ENCODING_UTF8, c);
  if (len > 1) {
    len--;
    n = c & ((1 << (6 - len)) - 1);
    while (len--) {
      c = *p++;
      n = (n << 6) | (c & ((1 << 6) - 1));
    }
    return n;
  }
  else
    return (OnigCodePoint )c;
}

static int
utf8_code_to_mbclen(OnigCodePoint code)
{
  if      ((code & 0xffffff80) == 0) return 1;
  else if ((code & 0xfffff800) == 0) {
    if (code <= 0xff && code >= 0xfe)
      return 1;
    return 2;
  }
  else if ((code & 0xffff0000) == 0) return 3;
  else if ((code & 0xffe00000) == 0) return 4;
  else if ((code & 0xfc000000) == 0) return 5;
  else if ((code & 0x80000000) == 0) return 6;
  else
    return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
}

#if 0
static int
utf8_code_to_mbc_first(OnigCodePoint code)
{
  if ((code & 0xffffff80) == 0)
    return code;
  else {
    if ((code & 0xfffff800) == 0)
      return ((code>>6)& 0x1f) | 0xc0;
    else if ((code & 0xffff0000) == 0)
      return ((code>>12) & 0x0f) | 0xe0;
    else if ((code & 0xffe00000) == 0)
      return ((code>>18) & 0x07) | 0xf0;
    else if ((code & 0xfc000000) == 0)
      return ((code>>24) & 0x03) | 0xf8;
    else if ((code & 0x80000000) == 0)
      return ((code>>30) & 0x01) | 0xfc;
    else {
      return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
    }
  }
}
#endif

static int
utf8_code_to_mbc(OnigCodePoint code, UChar *buf)
{
#define UTF8_TRAILS(code, shift) (UChar )((((code) >> (shift)) & 0x3f) | 0x80)
#define UTF8_TRAIL0(code)        (UChar )(((code) & 0x3f) | 0x80)

  if ((code & 0xffffff80) == 0) {
    *buf = (UChar )code;
    return 1;
  }
  else {
    UChar *p = buf;

    if ((code & 0xfffff800) == 0) {
      *p++ = (UChar )(((code>>6)& 0x1f) | 0xc0);
    }
    else if ((code & 0xffff0000) == 0) {
      *p++ = (UChar )(((code>>12) & 0x0f) | 0xe0);
      *p++ = UTF8_TRAILS(code, 6);
    }
    else if ((code & 0xffe00000) == 0) {
      *p++ = (UChar )(((code>>18) & 0x07) | 0xf0);
      *p++ = UTF8_TRAILS(code, 12);
      *p++ = UTF8_TRAILS(code,  6);
    }
    else if ((code & 0xfc000000) == 0) {
      *p++ = (UChar )(((code>>24) & 0x03) | 0xf8);
      *p++ = UTF8_TRAILS(code, 18);
      *p++ = UTF8_TRAILS(code, 12);
      *p++ = UTF8_TRAILS(code,  6);
    }
    else if ((code & 0x80000000) == 0) {
      *p++ = (UChar )(((code>>30) & 0x01) | 0xfc);
      *p++ = UTF8_TRAILS(code, 24);
      *p++ = UTF8_TRAILS(code, 18);
      *p++ = UTF8_TRAILS(code, 12);
      *p++ = UTF8_TRAILS(code,  6);
    }
    else {
      return ONIGERR_TOO_BIG_WIDE_CHAR_VALUE;
    }

    *p++ = UTF8_TRAIL0(code);
    return p - buf;
  }
}

static int
utf8_mbc_to_lower(UChar* p, UChar* lower)
{
  int len;

  /* !!! U+0080 - U+00ff is treated by fold match. !!! */
  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    return 1; /* return byte length of converted char to lower */
  }
  else {
    len = enc_len(ONIG_ENCODING_UTF8, *p);
    if (lower != p) {
      /* memcpy(lower, p, len); */
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    return len; /* return byte length of converted char to lower */
  }
}

static int
utf8_mbc_is_case_ambig(UChar* p)
{
  /* !!! U+0080 - U+00ff ( 0x80[0xc2,0x80] - 0xff[0xc3,0xbf] )
     is treated by fold match. !!! */

  if (ONIGENC_IS_MBC_ASCII(p))
    return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);

  return FALSE;
}

static int
utf8_code_is_ctype(OnigCodePoint code, unsigned int ctype)
{
  if (code < 256) {
    return ENC_IS_ISO_8859_1_CTYPE(code, ctype);
  }

  if ((ctype & ONIGENC_CTYPE_WORD) != 0) {
    return TRUE;
  }

  return FALSE;
}

static int
utf8_get_ctype_code_range(int ctype, int* nsb, int* nmb,
			  OnigCodePointRange* sbr[], OnigCodePointRange* mbr[])
{
#define CR_SET(sbl,mbl) do { \
  *nsb = sizeof(sbl) / sizeof(OnigCodePointRange); \
  *nmb = sizeof(mbl) / sizeof(OnigCodePointRange); \
  *sbr = sbl; \
  *mbr = mbl; \
} while (0)

#define CR_SB_SET(sbl) do { \
  *nsb = sizeof(sbl) / sizeof(OnigCodePointRange); \
  *nmb = 0; \
  *sbr = sbl; \
} while (0)

  static OnigCodePointRange SBAlpha[] = {
    { 0x41, 0x5a },
    { 0x61, 0x7a }
  };

  static OnigCodePointRange MBAlpha[] = {
    { 0xaa, 0xaa },
    { 0xb5, 0xb5 },
    { 0xba, 0xba },
    { 0xc0, 0xd6 },
    { 0xd8, 0xf6 },
    { 0xf8, 0x220 }
  };

  static OnigCodePointRange SBBlank[] = {
    { 0x09, 0x09 },
    { 0x20, 0x20 }
  };

  static OnigCodePointRange MBBlank[] = {
    { 0xa0, 0xa0 }
  };

  static OnigCodePointRange SBCntrl[] = {
    { 0x00, 0x1f },
    { 0x7f, 0x7f }
  };

  static OnigCodePointRange MBCntrl[] = {
    { 0x80, 0x9f }
  };

  static OnigCodePointRange SBDigit[] = {
    { 0x30, 0x39 }
  };

  static OnigCodePointRange SBGraph[] = {
    { 0x21, 0x7e }
  };

  static OnigCodePointRange MBGraph[] = {
    { 0xa1, 0x220 }
  };

  static OnigCodePointRange SBLower[] = {
    { 0x61, 0x7a }
  };

  static OnigCodePointRange MBLower[] = {
    { 0xaa, 0xaa },
    { 0xb5, 0xb5 },
    { 0xba, 0xba },
    { 0xdf, 0xf6 },
    { 0xf8, 0xff }
  };

  static OnigCodePointRange SBPrint[] = {
    { 0x20, 0x7e }
  };

  static OnigCodePointRange MBPrint[] = {
    { 0xa0, 0x220 }
  };

  static OnigCodePointRange SBPunct[] = {
    { 0x21, 0x23 },
    { 0x25, 0x2a },
    { 0x2c, 0x2f },
    { 0x3a, 0x3b },
    { 0x3f, 0x40 },
    { 0x5b, 0x5d },
    { 0x5f, 0x5f },
    { 0x7b, 0x7b },
    { 0x7d, 0x7d }
  };

  static OnigCodePointRange MBPunct[] = {
    { 0xa1, 0xa1 },
    { 0xab, 0xab },
    { 0xad, 0xad },
    { 0xb7, 0xb7 },
    { 0xbb, 0xbb },
    { 0xbf, 0xbf }
  };

  static OnigCodePointRange SBSpace[] = {
    { 0x09, 0x0d },
    { 0x20, 0x20 }
  };

  static OnigCodePointRange MBSpace[] = {
    { 0xa0, 0xa0 }
  };

  static OnigCodePointRange SBUpper[] = {
    { 0x41, 0x5a }
  };

  static OnigCodePointRange MBUpper[] = {
    { 0xc0, 0xd6 },
    { 0xd8, 0xde }
  };

  static OnigCodePointRange SBXDigit[] = {
    { 0x30, 0x39 },
    { 0x41, 0x46 },
    { 0x61, 0x66 }
  };

  static OnigCodePointRange SBWord[] = {
    { 0x30, 0x39 },
    { 0x41, 0x5a },
    { 0x5f, 0x5f },
    { 0x61, 0x7a }
  };

  static OnigCodePointRange MBWord[] = {
    { 0xaa, 0xaa },
    { 0xb2, 0xb3 },
    { 0xb5, 0xb5 },
    { 0xb9, 0xba },
    { 0xbc, 0xbe },
    { 0xc0, 0xd6 },
    { 0xd8, 0xf6 },
#if 0
    { 0xf8, 0x220 }
#else
    { 0xf8, 0x7fffffff } /* all multibyte code as word */
#endif
  };

  static OnigCodePointRange SBAscii[] = {
    { 0x00, 0x7f }
  };

  static OnigCodePointRange SBAlnum[] = {
    { 0x30, 0x39 },
    { 0x41, 0x5a },
    { 0x61, 0x7a }
  };

  static OnigCodePointRange MBAlnum[] = {
    { 0xaa, 0xaa },
    { 0xb5, 0xb5 },
    { 0xba, 0xba },
    { 0xc0, 0xd6 },
    { 0xd8, 0xf6 },
    { 0xf8, 0x220 }
  };

  switch (ctype) {
  case ONIGENC_CTYPE_ALPHA:
    CR_SET(SBAlpha, MBAlpha);
    break;
  case ONIGENC_CTYPE_BLANK:
    CR_SET(SBBlank, MBBlank);
    break;
  case ONIGENC_CTYPE_CNTRL:
    CR_SET(SBCntrl, MBCntrl);
    break;
  case ONIGENC_CTYPE_DIGIT:
    CR_SB_SET(SBDigit);
    break;
  case ONIGENC_CTYPE_GRAPH:
    CR_SET(SBGraph, MBGraph);
    break;
  case ONIGENC_CTYPE_LOWER:
    CR_SET(SBLower, MBLower);
    break;
  case ONIGENC_CTYPE_PRINT:
    CR_SET(SBPrint, MBPrint);
    break;
  case ONIGENC_CTYPE_PUNCT:
    CR_SET(SBPunct, MBPunct);
    break;
  case ONIGENC_CTYPE_SPACE:
    CR_SET(SBSpace, MBSpace);
    break;
  case ONIGENC_CTYPE_UPPER:
    CR_SET(SBUpper, MBUpper);
    break;
  case ONIGENC_CTYPE_XDIGIT:
    CR_SB_SET(SBXDigit);
    break;
  case ONIGENC_CTYPE_WORD:
    CR_SET(SBWord, MBWord);
    break;
  case ONIGENC_CTYPE_ASCII:
    CR_SB_SET(SBAscii);
    break;
  case ONIGENC_CTYPE_ALNUM:
    CR_SET(SBAlnum, MBAlnum);
    break;

  default:
    return ONIGERR_TYPE_BUG;
    break;
  }

  return 0;
}

static int
utf8_get_all_fold_match_code(OnigCodePoint** codes)
{
  static OnigCodePoint list[] = {
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
    0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6,
    0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,

    0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
    0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe,
  };

  *codes = list;
  return sizeof(list) / sizeof(OnigCodePoint);
}

static int
utf8_get_fold_match_info(UChar* p, UChar* end, OnigEncFoldMatchInfo** info)
{
  
  static OnigEncFoldMatchInfo xc[] = {
    { 2, { 2, 2 }, { "\303\200", "\303\240" } }, /* CodePoint 0xc0 */
    { 2, { 2, 2 }, { "\303\201", "\303\241" } },
    { 2, { 2, 2 }, { "\303\202", "\303\242" } },
    { 2, { 2, 2 }, { "\303\203", "\303\243" } },
    { 2, { 2, 2 }, { "\303\204", "\303\244" } },
    { 2, { 2, 2 }, { "\303\205", "\303\245" } },
    { 2, { 2, 2 }, { "\303\206", "\303\246" } },
    { 2, { 2, 2 }, { "\303\207", "\303\247" } },
    { 2, { 2, 2 }, { "\303\210", "\303\250" } },
    { 2, { 2, 2 }, { "\303\211", "\303\251" } },
    { 2, { 2, 2 }, { "\303\212", "\303\252" } },
    { 2, { 2, 2 }, { "\303\213", "\303\253" } },
    { 2, { 2, 2 }, { "\303\214", "\303\254" } },
    { 2, { 2, 2 }, { "\303\215", "\303\255" } },
    { 2, { 2, 2 }, { "\303\216", "\303\256" } },
    { 2, { 2, 2 }, { "\303\217", "\303\257" } },
    { 2, { 2, 2 }, { "\303\220", "\303\260" } }, /* CodePoint 0xd0 */
    { 2, { 2, 2 }, { "\303\221", "\303\261" } },
    { 2, { 2, 2 }, { "\303\222", "\303\262" } },
    { 2, { 2, 2 }, { "\303\223", "\303\263" } },
    { 2, { 2, 2 }, { "\303\224", "\303\264" } },
    { 2, { 2, 2 }, { "\303\225", "\303\265" } },
    { 2, { 2, 2 }, { "\303\226", "\303\266" } },
    { 0, { 0 }, { "" } },
    { 2, { 2, 2 }, { "\303\230", "\303\270" } },
    { 2, { 2, 2 }, { "\303\231", "\303\271" } },
    { 2, { 2, 2 }, { "\303\232", "\303\272" } },
    { 2, { 2, 2 }, { "\303\233", "\303\273" } },
    { 2, { 2, 2 }, { "\303\234", "\303\274" } },
    { 2, { 2, 2 }, { "\303\235", "\303\275" } },
    { 2, { 2, 2 }, { "\303\236", "\303\276" } },
    { 3, { 2, 2, 2 }, { "\303\237", "ss", "SS" }} /* ess-tsett(U+00DF) */
  };

  if (p + 1 >= end)  return -1;
  if (*p < 0x80) {
    if ((*p == 'S' && *(p+1) == 'S') ||
	(*p == 's' && *(p+1) == 's')) {
      *info = &(xc[0xdf - 0xc0]);
      return 2;
    }
  }
  else if (*p == 195) { /* 195 == '\303' */
    int c = *(p+1);
    if (c >= 128) {
      if (c <= 159) { /* upper */
	if (c == 151) return -1; /* 0xd7 */
	*info = &(xc[c - 128]);
	return 2;
      }
      else { /* lower */
	if (c == 183) return -1; /* 0xf7 */
	*info = &(xc[c - 160]);
	return 2;
      }
    }
  }

  return -1; /* is not a fold string. */
}


static UChar*
utf8_left_adjust_char_head(UChar* start, UChar* s)
{
  UChar *p;

  if (s <= start) return s;
  p = s;

  while (!utf8_islead(*p) && p > start) p--;
  return p;
}

static int
utf8_is_allowed_reverse_match(UChar* s, UChar* end)
{
  return TRUE;
}

OnigEncodingType OnigEncodingUTF8 = {
  {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
  },
  "UTF-8",     /* name */
  6,           /* max byte length */
  TRUE,        /* is_fold_match */
  ONIGENC_CTYPE_SUPPORT_LEVEL_FULL,  /* ctype_support_level */
  TRUE,                             /* is continuous sb mb codepoint */
  utf8_mbc_to_code,
  utf8_code_to_mbclen,
  utf8_code_to_mbc,
  utf8_mbc_to_lower,
  utf8_mbc_is_case_ambig,
  utf8_code_is_ctype,
  utf8_get_ctype_code_range,
  utf8_left_adjust_char_head,
  utf8_is_allowed_reverse_match,
  utf8_get_all_fold_match_code,
  utf8_get_fold_match_info
};
