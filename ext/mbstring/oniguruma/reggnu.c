/**********************************************************************

  reggnu.c -  Oniguruma (regular expression library)

  Copyright (C) 2002-2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#include "regint.h"

#if defined(RUBY_PLATFORM) || defined(RUBY)
#ifndef REG_RUBY_M17N
#define USE_COMPATIBILITY_FOR_RUBY_EXTENSION_LIBRARY
#endif
#endif

#ifndef NULL
#define NULL   ((void* )0)
#endif

extern void
re_free_registers(RegRegion* r)
{
  /* 0: don't free self */
  regex_region_free(r, 0);
}

extern int
re_adjust_startpos(regex_t* reg, const char* string, int size,
		   int startpos, int range)
{
  if (startpos > 0 && mbmaxlen(reg->enc) != 1 && startpos < size) {
    UChar *p;
    UChar *s = (UChar* )string + startpos;

    if (range > 0) {
      p = regex_get_right_adjust_char_head(reg->enc, (UChar* )string, s);
    }
    else {
      p = regex_get_left_adjust_char_head(reg->enc, (UChar* )string, s);
    }
    return p - (UChar* )string;
  }

  return startpos;
}

extern int
re_match(regex_t* reg, const char* str, int size, int pos,
	 struct re_registers* regs)
{
  return regex_match(reg, (UChar* )str, (UChar* )(str + size),
		     (UChar* )(str + pos), regs, REG_OPTION_NONE);
}

extern int
re_search(regex_t* bufp, const char* string, int size, int startpos, int range,
	  struct re_registers* regs)
{
  return regex_search(bufp, (UChar* )string, (UChar* )(string + size),
		  (UChar* )(string + startpos),
		  (UChar* )(string + startpos + range), regs, REG_OPTION_NONE);
}

extern int
re_compile_pattern(const char* pattern, int size, regex_t* reg, char* ebuf)
{
  int r;
  RegErrorInfo einfo;

  r = regex_compile(reg, (UChar* )pattern, (UChar* )(pattern + size), &einfo);
  if (r != 0) {
    if (IS_NOT_NULL(ebuf))
      (void )regex_error_code_to_str((UChar* )ebuf, r, &einfo);
  }

  return r;
}

extern int
re_recompile_pattern(const char* pattern, int size, regex_t* reg, char* ebuf)
{
  int r;
  RegErrorInfo einfo;
  RegCharEncoding enc;

  /* I think encoding and options should be arguments of this function.
     But this is adapted to present re.c. (2002/11/29)
   */
  enc = RegDefaultCharEncoding;

  r = regex_recompile(reg, (UChar* )pattern, (UChar* )(pattern + size),
		      reg->options, enc, RegDefaultSyntax, &einfo);
  if (r != 0) {
    if (IS_NOT_NULL(ebuf))
      (void )regex_error_code_to_str((UChar* )ebuf, r, &einfo);
  }
  return r;
}

extern void
re_free_pattern(regex_t* reg)
{
  regex_free(reg);
}

extern int
re_alloc_pattern(regex_t** reg)
{
  if (RegDefaultCharEncoding == REGCODE_UNDEF)
    return REGERR_DEFAULT_ENCODING_IS_NOT_SETTED;

  return regex_alloc_init(reg, REG_OPTION_DEFAULT, RegDefaultCharEncoding,
			  RegDefaultSyntax);
}

extern void
re_set_casetable(const char* table)
{
  regex_set_default_trans_table((UChar* )table);
}

#ifdef USE_COMPATIBILITY_FOR_RUBY_EXTENSION_LIBRARY
static const unsigned char mbctab_ascii[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

static const unsigned char mbctab_euc[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};

static const unsigned char mbctab_sjis[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0
};

static const unsigned char mbctab_utf8[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 0, 0,
};

const unsigned char *re_mbctab = mbctab_ascii;
#endif

extern void
#ifdef REG_RUBY_M17N
re_mbcinit(RegCharEncoding enc)
#else
re_mbcinit(int mb_code)
#endif
{
#ifdef REG_RUBY_M17N
  RegDefaultCharEncoding = enc;
#else
  RegDefaultCharEncoding = REG_MBLEN_TABLE[mb_code];
#endif

#ifdef USE_COMPATIBILITY_FOR_RUBY_EXTENSION_LIBRARY
  switch (mb_code) {
  case MBCTYPE_ASCII:
    re_mbctab = mbctab_ascii;
    break;
  case MBCTYPE_EUC:
    re_mbctab = mbctab_euc;
    break;
  case MBCTYPE_SJIS:
    re_mbctab = mbctab_sjis;
    break;
  case MBCTYPE_UTF8:
    re_mbctab = mbctab_utf8;
    break;
  }
#endif
}
