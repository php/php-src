/**********************************************************************

  regposix.c - Oniguruma (regular expression library)

  Copyright (C) 2003  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/

#define regex_t   onig_regex_t
#include "regint.h"
#undef regex_t
#include "onigposix.h"

#define ONIG_C(reg)    ((onig_regex_t* )((reg)->onig))
#define PONIG_C(reg)   ((onig_regex_t** )(&(reg)->onig))

#if 1
#define ENC_STRING_LEN(enc,s,len) do { \
  UChar* tmps = (UChar* )(s); \
  /* while (*tmps != 0) tmps += mblen(enc,*tmps); */ \
  while (*tmps != 0) tmps++; /* OK for UTF-8, EUC-JP, Shift_JIS */ \
  len = tmps - (UChar* )(s); \
} while(0)
#else
#define ENC_STRING_LEN(enc,s,len)    len = strlen(s)
#endif

typedef struct {
  int onig_err;
  int posix_err;
} O2PERR;

static int
onig2posix_error_code(int code)
{
  static O2PERR o2p[] = {
    { REG_MISMATCH,                                      REG_NOMATCH },
    { REG_NO_SUPPORT_CONFIG,                             REG_EONIG_INTERNAL },
    { REGERR_MEMORY,                                     REG_ESPACE  },
    { REGERR_MATCH_STACK_LIMIT_OVER,                     REG_EONIG_INTERNAL },
    { REGERR_TYPE_BUG,                                   REG_EONIG_INTERNAL },
    { REGERR_PARSER_BUG,                                 REG_EONIG_INTERNAL },
    { REGERR_STACK_BUG,                                  REG_EONIG_INTERNAL },
    { REGERR_UNDEFINED_BYTECODE,                         REG_EONIG_INTERNAL },
    { REGERR_UNEXPECTED_BYTECODE,                        REG_EONIG_INTERNAL },
    { REGERR_DEFAULT_ENCODING_IS_NOT_SETTED,             REG_EONIG_BADARG },
    { REGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR, REG_EONIG_BADARG },
    { REGERR_END_PATTERN_AT_LEFT_BRACE,                  REG_EBRACE  },
    { REGERR_END_PATTERN_AT_LEFT_BRACKET,                REG_EBRACK  },
    { REGERR_EMPTY_CHAR_CLASS,                           REG_ECTYPE  },
    { REGERR_PREMATURE_END_OF_CHAR_CLASS,                REG_ECTYPE  },
    { REGERR_END_PATTERN_AT_BACKSLASH,                   REG_EESCAPE },
    { REGERR_END_PATTERN_AT_META,                        REG_EESCAPE },
    { REGERR_END_PATTERN_AT_CONTROL,                     REG_EESCAPE },
    { REGERR_META_CODE_SYNTAX,                           REG_BADPAT  },
    { REGERR_CONTROL_CODE_SYNTAX,                        REG_BADPAT  },
    { REGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE,           REG_ECTYPE  },
    { REGERR_CHAR_CLASS_VALUE_AT_START_OF_RANGE,         REG_ECTYPE  },
    { REGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS,    REG_ECTYPE  },
    { REGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED,    REG_BADRPT  },
    { REGERR_TARGET_OF_REPEAT_OPERATOR_INVALID,          REG_BADRPT  },
    { REGERR_NESTED_REPEAT_OPERATOR,                     REG_BADRPT  },
    { REGERR_UNMATCHED_CLOSE_PARENTHESIS,                REG_EPAREN  },
    { REGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS,     REG_EPAREN  },
    { REGERR_END_PATTERN_IN_GROUP,                       REG_BADPAT  },
    { REGERR_UNDEFINED_GROUP_OPTION,                     REG_BADPAT  },
    { REGERR_INVALID_POSIX_BRACKET_TYPE,                 REG_BADPAT  },
    { REGERR_INVALID_LOOK_BEHIND_PATTERN,                REG_BADPAT  },
    { REGERR_INVALID_REPEAT_RANGE_PATTERN,               REG_BADPAT  },
    { REGERR_TOO_BIG_NUMBER,                             REG_BADPAT  },
    { REGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE,            REG_BADBR   },
    { REGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE,   REG_BADBR   },
    { REGERR_EMPTY_RANGE_IN_CHAR_CLASS,                  REG_ECTYPE  },
    { REGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE,        REG_ECTYPE  },
    { REGERR_TOO_MANY_MULTI_BYTE_RANGES,                 REG_ECTYPE  },
    { REGERR_TOO_SHORT_MULTI_BYTE_STRING,                REG_BADPAT  },
    { REGERR_TOO_BIG_BACKREF_NUMBER,                     REG_ESUBREG },
    { REGERR_INVALID_BACKREF,                            REG_ESUBREG },
    { REGERR_TOO_BIG_WIDE_CHAR_VALUE,                    REG_EONIG_BADWC },
    { REGERR_TOO_LONG_WIDE_CHAR_VALUE,                   REG_EONIG_BADWC },
    { REGERR_INVALID_WIDE_CHAR_VALUE,                    REG_EONIG_BADWC },
    { REGERR_INVALID_SUBEXP_NAME,                        REG_BADPAT },
    { REGERR_UNDEFINED_NAME_REFERENCE,                   REG_BADPAT },
    { REGERR_UNDEFINED_GROUP_REFERENCE,                  REG_BADPAT },
    { REGERR_MULTIPLEX_DEFINITION_NAME_CALL,             REG_BADPAT },
    { REGERR_NEVER_ENDING_RECURSION,                     REG_BADPAT },
    { REGERR_OVER_THREAD_PASS_LIMIT_COUNT,               REG_EONIG_THREAD }
  };

  int i;

  if (code >= 0) return 0;

  for (i = 0; i < sizeof(o2p) / sizeof(o2p[0]); i++) {
    if (code == o2p[i].onig_err)
      return o2p[i].posix_err;
  }

  return REG_EONIG_INTERNAL;  /* but, unknown error code */
}

extern int
regcomp(regex_t* reg, const char* pattern, int posix_options)
{
  int r, len;
  RegSyntaxType* syntax = RegDefaultSyntax;
  RegOptionType options;

  if ((posix_options & REG_EXTENDED) == 0)
    syntax = REG_SYNTAX_POSIX_BASIC;

  options = syntax->options;
  if ((posix_options & REG_ICASE)   != 0)
    REG_OPTION_ON(options, REG_OPTION_IGNORECASE);
  if ((posix_options & REG_NEWLINE) != 0) {
    REG_OPTION_ON( options, REG_OPTION_NEGATE_SINGLELINE);
    REG_OPTION_OFF(options, REG_OPTION_SINGLELINE);
  }

  reg->comp_options = posix_options;

  ENC_STRING_LEN(RegDefaultCharEncoding, pattern, len);
  r = regex_new(PONIG_C(reg), (UChar* )pattern, (UChar* )(pattern + len),
		options, RegDefaultCharEncoding, syntax, (RegErrorInfo* )NULL);
  if (r != REG_NORMAL) {
    return onig2posix_error_code(r);
  }

  reg->re_nsub = ONIG_C(reg)->num_mem;
  return 0;
}

extern int
regexec(regex_t* reg, const char* str, size_t nmatch,
	regmatch_t pmatch[], int posix_options)
{
  int r, i, len;
  UChar* end;
  RegOptionType options;

  options = REG_OPTION_POSIX_REGION;
  if ((posix_options & REG_NOTBOL) != 0) options |= REG_OPTION_NOTBOL;
  if ((posix_options & REG_NOTEOL) != 0) options |= REG_OPTION_NOTEOL;

  if ((reg->comp_options & REG_NOSUB) != 0) {
    pmatch = (regmatch_t* )NULL;
    nmatch = 0;
  }

  ENC_STRING_LEN(ONIG_C(reg)->code,str,len);
  end = (UChar* )(str + len);
  r = regex_search(ONIG_C(reg), (UChar* )str, end, (UChar* )str, end,
		   (RegRegion* )pmatch, options);

  if (r >= 0) {
    r = 0; /* Match */
  }
  else if (r == REG_MISMATCH) {
    r = REG_NOMATCH;
    for (i = 0; i < nmatch; i++)
      pmatch[i].rm_so = pmatch[i].rm_eo = REG_REGION_NOTPOS;
  }
  else {
    r = onig2posix_error_code(r);
  }

  return r;
}

extern void
regfree(regex_t* reg)
{
  regex_free(ONIG_C(reg));
}


extern void
reg_set_encoding(int mb_code)
{
  RegDefaultCharEncoding = REG_MBLEN_TABLE[mb_code];
}

extern int
reg_name_to_group_numbers(regex_t* reg,
		  unsigned char* name, unsigned char* name_end, int** nums)
{
  return regex_name_to_group_numbers(ONIG_C(reg), name, name_end, nums);
}

extern int
reg_foreach_name(regex_t* reg, int (*func)(unsigned char*,int,int*,void*),
		 void* arg)
{
  return regex_foreach_name(ONIG_C(reg), func, arg);
}
