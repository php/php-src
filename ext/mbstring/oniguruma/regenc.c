/**********************************************************************

  regenc.c -  Oniguruma (regular expression library)

  Copyright (C) 2003-2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regenc.h"

OnigEncoding OnigEncDefaultCharEncoding = ONIG_ENCODING_INIT_DEFAULT;

extern int
onigenc_init()
{
  return 0;
}

extern OnigEncoding
onigenc_get_default_encoding()
{
  return OnigEncDefaultCharEncoding;
}

extern int
onigenc_set_default_encoding(OnigEncoding enc)
{
  OnigEncDefaultCharEncoding = enc;
  return 0;
}

extern UChar*
onigenc_get_right_adjust_char_head(OnigEncoding enc, UChar* start, UChar* s)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);
  if (p < s) {
    p += enc_len(enc, *p);
  }
  return p;
}

extern UChar*
onigenc_get_right_adjust_char_head_with_prev(OnigEncoding enc,
				   UChar* start, UChar* s, UChar** prev)
{
  UChar* p = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);

  if (p < s) {
    if (prev) *prev = p;
    p += enc_len(enc, *p);
  }
  else {
    if (prev) *prev = (UChar* )NULL; /* Sorry */
  }
  return p;
}

extern UChar*
onigenc_get_prev_char_head(OnigEncoding enc, UChar* start, UChar* s)
{
  if (s <= start)
    return (UChar* )NULL;

  return ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1);
}

extern UChar*
onigenc_step_back(OnigEncoding enc, UChar* start, UChar* s, int n)
{
  while (ONIG_IS_NOT_NULL(s) && n-- > 0) {
    if (s <= start)
      return (UChar* )NULL;

    s = ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s - 1);
  }
  return s;
}


#ifndef ONIG_RUBY_M17N

#ifndef NOT_RUBY
#define USE_APPLICATION_TO_LOWER_CASE_TABLE
#endif

UChar* OnigEncAsciiToLowerCaseTable = (UChar* )0;

#ifndef USE_APPLICATION_TO_LOWER_CASE_TABLE
static UChar BuiltInAsciiToLowerCaseTable[] = {
  '\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
  '\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
  '\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
  '\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
  '\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
  '\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
  '\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
  '\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
  '\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
  '\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
  '\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
  '\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
  '\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
  '\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
  '\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
  '\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
  '\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
  '\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
  '\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
  '\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
  '\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
  '\300', '\301', '\302', '\303', '\304', '\305', '\306', '\307',
  '\310', '\311', '\312', '\313', '\314', '\315', '\316', '\317',
  '\320', '\321', '\322', '\323', '\324', '\325', '\326', '\327',
  '\330', '\331', '\332', '\333', '\334', '\335', '\336', '\337',
  '\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
  '\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
  '\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
  '\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};
#endif /* not USE_APPLICATION_TO_LOWER_CASE_TABLE */

unsigned short OnigEncAsciiCtypeTable[256] = {
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1106, 0x1104, 0x1104, 0x1104, 0x1104, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004, 0x1004,
  0x1142, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0,
  0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0,
  0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58, 0x1c58,
  0x1c58, 0x1c58, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x10d0,
  0x10d0, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1e51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51, 0x1a51,
  0x1a51, 0x1a51, 0x1a51, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x18d0,
  0x10d0, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1c71, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871, 0x1871,
  0x1871, 0x1871, 0x1871, 0x10d0, 0x10d0, 0x10d0, 0x10d0, 0x1004,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

extern void
onigenc_set_default_caseconv_table(UChar* table)
{
  if (table == (UChar* )0) {
#ifndef USE_APPLICATION_TO_LOWER_CASE_TABLE
    table = BuiltInAsciiToLowerCaseTable;
#else
    return ;
#endif
  }

  if (table != OnigEncAsciiToLowerCaseTable) {
    OnigEncAsciiToLowerCaseTable = table;
  }
}

extern UChar*
onigenc_get_left_adjust_char_head(OnigEncoding enc, UChar* start, UChar* s)
{
  return ONIGENC_LEFT_ADJUST_CHAR_HEAD(enc, start, s);
}

extern int
onigenc_nothing_get_all_fold_match_code(OnigCodePoint** codes)
{
  return 0;
}

extern int
onigenc_nothing_get_fold_match_info(UChar* p, UChar* end,
				OnigEncFoldMatchInfo** info)
{
  return -1;
}

extern int
onigenc_nothing_get_ctype_code_range(int ctype, int* nsb, int* nmb,
			 OnigCodePointRange* sbr[], OnigCodePointRange* mbr[])
{
  return -1;
}

/* for single byte encodings */
extern int
onigenc_ascii_mbc_to_lower(UChar* p, UChar* lower)
{
  *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
  return 1; /* return byte length of converted char to lower */
}

extern int
onigenc_ascii_mbc_is_case_ambig(UChar* p)
{
  return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);
}

extern OnigCodePoint
onigenc_single_byte_mbc_to_code(UChar* p, UChar* end)
{
  return (OnigCodePoint )(*p);
}

extern int
onigenc_single_byte_code_to_mbclen(OnigCodePoint code)
{
  return 1;
}

extern int
onigenc_single_byte_code_to_mbc_first(OnigCodePoint code)
{
  return (code & 0xff);
}

extern int
onigenc_single_byte_code_to_mbc(OnigCodePoint code, UChar *buf)
{
  *buf = (UChar )(code & 0xff);
  return 1;
}

extern UChar*
onigenc_single_byte_left_adjust_char_head(UChar* start, UChar* s)
{
  return s;
}

extern int
onigenc_single_byte_is_allowed_reverse_match(UChar* s, UChar* end)
{
  return TRUE;
}

extern OnigCodePoint
onigenc_mbn_mbc_to_code(OnigEncoding enc, UChar* p, UChar* end)
{
  int c, i, len;
  OnigCodePoint n;

  c = *p++;
  len = enc_len(enc, c);
  n = c;
  if (len == 1) return n;

  for (i = 1; i < len; i++) {
    if (p >= end) break;
    c = *p++;
    n <<= 8;  n += c;
  }
  return n;
}

extern int
onigenc_mbn_mbc_to_lower(OnigEncoding enc, UChar* p, UChar* lower)
{
  int len;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    return 1;
  }
  else {
    len = enc_len(enc, *p);
    if (lower != p) {
      /* memcpy(lower, p, len); */
      int i;
      for (i = 0; i < len; i++) {
	*lower++ = *p++;
      }
    }
    return len; /* return byte length of converted to lower char */
  }
}

extern int
onigenc_mbn_mbc_is_case_ambig(UChar* p)
{
  if (ONIGENC_IS_MBC_ASCII(p))
    return ONIGENC_IS_ASCII_CODE_CASE_AMBIG(*p);

  return FALSE;
}

extern int
onigenc_mb2_code_to_mbclen(OnigCodePoint code)
{
  if ((code & 0xff00) != 0) return 2;
  else return 1;
}

extern int
onigenc_mb4_code_to_mbclen(OnigCodePoint code)
{
       if ((code & 0xff000000) != 0) return 4;
  else if ((code & 0xff0000) != 0) return 3;
  else if ((code & 0xff00) != 0) return 2;
  else return 1;
}

extern int
onigenc_mb2_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff00) != 0) {
    first = (code >> 8) & 0xff;
  }
  else {
    return (int )code;
  }
  return first;
}

extern int
onigenc_mb4_code_to_mbc_first(OnigCodePoint code)
{
  int first;

  if ((code & 0xff000000) != 0) {
    first = (code >> 24) & 0xff;
  }
  else if ((code & 0xff0000) != 0) {
    first = (code >> 16) & 0xff;
  }
  else if ((code & 0xff00) != 0) {
    first = (code >>  8) & 0xff;
  }
  else {
    return (int )code;
  }
  return first;
}

extern int
onigenc_mb2_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff00) != 0) {
    *p++ = (UChar )((code >>  8) & 0xff);
  }
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(enc, buf[0]) != (p - buf))
    return ONIGERR_INVALID_WIDE_CHAR_VALUE;
#endif
  return p - buf;
}

extern int
onigenc_mb4_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  UChar *p = buf;

  if ((code & 0xff000000) != 0) {
    *p++ = (UChar )((code >> 24) & 0xff);
  }
  if ((code & 0xff0000) != 0) {
    *p++ = (UChar )((code >> 16) & 0xff);
  }
  if ((code & 0xff00) != 0) {
    *p++ = (UChar )((code >> 8) & 0xff);
  }
  *p++ = (UChar )(code & 0xff);

#if 1
  if (enc_len(enc, buf[0]) != (p - buf))
    return ONIGERR_INVALID_WIDE_CHAR_VALUE;
#endif
  return p - buf;
}

extern int
onigenc_mb2_code_is_ctype(OnigEncoding enc, OnigCodePoint code,
			  unsigned int ctype)
{
  if ((ctype & ONIGENC_CTYPE_WORD) != 0) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      int first = onigenc_mb2_code_to_mbc_first(code);
      return (enc_len(enc, first) > 1 ? TRUE : FALSE);
    }

    ctype &= ~ONIGENC_CTYPE_WORD;
    if (ctype == 0) return FALSE;
  }

  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}

extern int
onigenc_mb4_code_is_ctype(OnigEncoding enc, OnigCodePoint code,
			  unsigned int ctype)
{
  if ((ctype & ONIGENC_CTYPE_WORD) != 0) {
    if (code < 128)
      return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
    else {
      int first = onigenc_mb4_code_to_mbc_first(code);
      return (enc_len(enc, first) > 1 ? TRUE : FALSE);
    }

    ctype &= ~ONIGENC_CTYPE_WORD;
    if (ctype == 0) return FALSE;
  }

  if (code < 128)
    return ONIGENC_IS_ASCII_CODE_CTYPE(code, ctype);
  else
    return FALSE;
}

extern int
onigenc_get_all_fold_match_code_ss_0xdf(OnigCodePoint** codes)
{
  static OnigCodePoint list[] = { 0xdf };
  *codes = list;
  return 1;
}

extern int
onigenc_get_fold_match_info_ss_0xdf(UChar* p, UChar* end,
				    OnigEncFoldMatchInfo** info)
{
  /* German alphabet ess-tsett(U+00DF) */
  static OnigEncFoldMatchInfo ss = {
    3,
    { 1, 2, 2 },
    { "\337", "ss", "SS" } /* 0337: 0xdf */
  };

  if (p >= end) return -1;

  if (*p == 0xdf) {
    *info = &ss;
    return 1;
  }
  else if (p + 1 < end) {
    if ((*p == 'S' && *(p+1) == 'S') ||
	(*p == 's' && *(p+1) == 's')) {
      *info = &ss;
      return 2;
    }
  }

  return -1; /* is not a fold string. */
}

#else /* ONIG_RUBY_M17N */

extern int
onigenc_is_code_ctype(OnigEncoding enc, OnigCodePoint code, int ctype)
{
  switch (ctype) {
  case ONIGENC_CTYPE_ALPHA:
    return m17n_isalpha(enc, code);
    break;
  case ONIGENC_CTYPE_BLANK:
    return ONIGENC_IS_CODE_BLANK(enc, (int )(code));
    break;
  case ONIGENC_CTYPE_CNTRL:
    return m17n_iscntrl(enc, code);
    break;
  case ONIGENC_CTYPE_DIGIT:
    return m17n_isdigit(enc, code);
    break;
  case ONIGENC_CTYPE_GRAPH:
    return ONIGENC_IS_CODE_GRAPH(enc, (int )(code));
    break;
  case ONIGENC_CTYPE_LOWER:
    return m17n_islower(enc, code);
    break;
  case ONIGENC_CTYPE_PRINT:
    return m17n_isprint(enc, code);
    break;
  case ONIGENC_CTYPE_PUNCT:
    return m17n_ispunct(enc, code);
    break;
  case ONIGENC_CTYPE_SPACE:
    return m17n_isspace(enc, code);
    break;
  case ONIGENC_CTYPE_UPPER:
    return m17n_isupper(enc, code);
    break;
  case ONIGENC_CTYPE_XDIGIT:
    return m17n_isxdigit(enc, code);
    break;
  case ONIGENC_CTYPE_WORD:
    return m17n_iswchar(enc, code);
    break;
  case ONIGENC_CTYPE_ASCII:
    return (code < 128 ? TRUE : FALSE);
    break;
  case ONIGENC_CTYPE_ALNUM:
    return m17n_isalnum(enc, code);
    break;
  default:
    break;
  }

  return 0;
}

extern int
onigenc_code_to_mbc(OnigEncoding enc, OnigCodePoint code, UChar *buf)
{
  int c, len;

  m17n_mbcput(enc, code, buf);
  c = m17n_firstbyte(enc, code);
  len = enc_len(enc, c);
  return len;
}

extern int
onigenc_mbc_to_lower(OnigEncoding enc, UChar* p, UChar* buf)
{
  unsigned int c, low;

  c   = m17n_codepoint(enc, p, p + enc_len(enc, *p));
  low = m17n_tolower(enc, c);
  m17n_mbcput(enc, low, buf);

  return m17n_codelen(enc, low);
}

extern int
onigenc_mbc_is_case_ambig(OnigEncoding enc, UChar* p)
{
  unsigned int c = m17n_codepoint(enc, p, p + enc_len(enc, *p));

  if (m17n_isupper(enc, c) || m17n_islower(enc, c))
    return TRUE;
  return FALSE;
}

extern UChar*
onigenc_get_left_adjust_char_head(OnigEncoding enc, UChar* start, UChar* s)
{
  UChar *p;
  int len;

  if (s <= start) return s;
  p = s;

  while (!m17n_islead(enc, *p) && p > start) p--;
  while (p + (len = enc_len(enc, *p)) < s) {
    p += len;
  }
  if (p + len == s) return s;
  return p;
}

extern int
onigenc_is_allowed_reverse_match(OnigEncoding enc, UChar* s, UChar* end)
{
  return ONIGENC_IS_SINGLEBYTE(enc);
}

extern void
onigenc_set_default_caseconv_table(UChar* table) { }

#endif /* ONIG_RUBY_M17N */
