/**********************************************************************
  regposix.c - Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2018  K.Kosako  <sndgk393 AT ybb DOT ne DOT jp>
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

#define regex_t   onig_regex_t
#include "regint.h"
#undef regex_t
#include "onigposix.h"

#define ONIG_C(reg)    ((onig_regex_t* )((reg)->onig))
#define PONIG_C(reg)   ((onig_regex_t** )(&(reg)->onig))

/* #define ENC_STRING_LEN(enc,s,len)    len = strlen(s) */
#define ENC_STRING_LEN(enc,s,len) do { \
  if (ONIGENC_MBC_MINLEN(enc) == 1) { \
    UChar* tmps = (UChar* )(s); \
    while (*tmps != 0) tmps++; \
    len = (int )(tmps - (UChar* )(s));\
  } \
  else { \
    len = onigenc_str_bytelen_null(enc, (UChar* )s); \
  } \
} while(0)

typedef struct {
  int onig_err;
  int posix_err;
} O2PERR;

static int
onig2posix_error_code(int code)
{
  static const O2PERR o2p[] = {
    { ONIG_MISMATCH,                                      REG_NOMATCH },
    { ONIG_NO_SUPPORT_CONFIG,                             REG_EONIG_INTERNAL },
    { ONIG_ABORT,                                         REG_EONIG_INTERNAL },
    { ONIGERR_MEMORY,                                     REG_ESPACE  },
    { ONIGERR_MATCH_STACK_LIMIT_OVER,                     REG_EONIG_INTERNAL },
    { ONIGERR_RETRY_LIMIT_IN_MATCH_OVER,                  REG_EONIG_INTERNAL },
    { ONIGERR_TYPE_BUG,                                   REG_EONIG_INTERNAL },
    { ONIGERR_PARSER_BUG,                                 REG_EONIG_INTERNAL },
    { ONIGERR_STACK_BUG,                                  REG_EONIG_INTERNAL },
    { ONIGERR_UNDEFINED_BYTECODE,                         REG_EONIG_INTERNAL },
    { ONIGERR_UNEXPECTED_BYTECODE,                        REG_EONIG_INTERNAL },
    { ONIGERR_DEFAULT_ENCODING_IS_NOT_SETTED,             REG_EONIG_BADARG },
    { ONIGERR_SPECIFIED_ENCODING_CANT_CONVERT_TO_WIDE_CHAR, REG_EONIG_BADARG },
    { ONIGERR_FAIL_TO_INITIALIZE,                         REG_EONIG_INTERNAL },
    { ONIGERR_INVALID_ARGUMENT,                           REG_EONIG_BADARG },
    { ONIGERR_END_PATTERN_AT_LEFT_BRACE,                  REG_EBRACE  },
    { ONIGERR_END_PATTERN_AT_LEFT_BRACKET,                REG_EBRACK  },
    { ONIGERR_EMPTY_CHAR_CLASS,                           REG_ECTYPE  },
    { ONIGERR_PREMATURE_END_OF_CHAR_CLASS,                REG_ECTYPE  },
    { ONIGERR_END_PATTERN_AT_ESCAPE,                      REG_EESCAPE },
    { ONIGERR_END_PATTERN_AT_META,                        REG_EESCAPE },
    { ONIGERR_END_PATTERN_AT_CONTROL,                     REG_EESCAPE },
    { ONIGERR_META_CODE_SYNTAX,                           REG_BADPAT  },
    { ONIGERR_CONTROL_CODE_SYNTAX,                        REG_BADPAT  },
    { ONIGERR_CHAR_CLASS_VALUE_AT_END_OF_RANGE,           REG_ECTYPE  },
    { ONIGERR_CHAR_CLASS_VALUE_AT_START_OF_RANGE,         REG_ECTYPE  },
    { ONIGERR_UNMATCHED_RANGE_SPECIFIER_IN_CHAR_CLASS,    REG_ECTYPE  },
    { ONIGERR_TARGET_OF_REPEAT_OPERATOR_NOT_SPECIFIED,    REG_BADRPT  },
    { ONIGERR_TARGET_OF_REPEAT_OPERATOR_INVALID,          REG_BADRPT  },
    { ONIGERR_NESTED_REPEAT_OPERATOR,                     REG_BADRPT  },
    { ONIGERR_UNMATCHED_CLOSE_PARENTHESIS,                REG_EPAREN  },
    { ONIGERR_END_PATTERN_WITH_UNMATCHED_PARENTHESIS,     REG_EPAREN  },
    { ONIGERR_END_PATTERN_IN_GROUP,                       REG_BADPAT  },
    { ONIGERR_UNDEFINED_GROUP_OPTION,                     REG_BADPAT  },
    { ONIGERR_INVALID_POSIX_BRACKET_TYPE,                 REG_BADPAT  },
    { ONIGERR_INVALID_LOOK_BEHIND_PATTERN,                REG_BADPAT  },
    { ONIGERR_INVALID_REPEAT_RANGE_PATTERN,               REG_BADPAT  },
    { ONIGERR_TOO_BIG_NUMBER,                             REG_BADPAT  },
    { ONIGERR_TOO_BIG_NUMBER_FOR_REPEAT_RANGE,            REG_BADBR   },
    { ONIGERR_UPPER_SMALLER_THAN_LOWER_IN_REPEAT_RANGE,   REG_BADBR   },
    { ONIGERR_EMPTY_RANGE_IN_CHAR_CLASS,                  REG_ECTYPE  },
    { ONIGERR_MISMATCH_CODE_LENGTH_IN_CLASS_RANGE,        REG_ECTYPE  },
    { ONIGERR_TOO_MANY_MULTI_BYTE_RANGES,                 REG_ECTYPE  },
    { ONIGERR_TOO_SHORT_MULTI_BYTE_STRING,                REG_BADPAT  },
    { ONIGERR_TOO_BIG_BACKREF_NUMBER,                     REG_ESUBREG },
    { ONIGERR_INVALID_BACKREF,                            REG_ESUBREG },
    { ONIGERR_NUMBERED_BACKREF_OR_CALL_NOT_ALLOWED,       REG_BADPAT  },
    { ONIGERR_TOO_BIG_WIDE_CHAR_VALUE,                    REG_EONIG_BADWC },
    { ONIGERR_TOO_LONG_WIDE_CHAR_VALUE,                   REG_EONIG_BADWC },
    { ONIGERR_INVALID_CODE_POINT_VALUE,                   REG_EONIG_BADWC },
    { ONIGERR_EMPTY_GROUP_NAME,                           REG_BADPAT },
    { ONIGERR_INVALID_GROUP_NAME,                         REG_BADPAT },
    { ONIGERR_INVALID_CHAR_IN_GROUP_NAME,                 REG_BADPAT },
    { ONIGERR_UNDEFINED_NAME_REFERENCE,                   REG_BADPAT },
    { ONIGERR_UNDEFINED_GROUP_REFERENCE,                  REG_BADPAT },
    { ONIGERR_MULTIPLEX_DEFINED_NAME,                     REG_BADPAT },
    { ONIGERR_MULTIPLEX_DEFINITION_NAME_CALL,             REG_BADPAT },
    { ONIGERR_NEVER_ENDING_RECURSION,                     REG_BADPAT },
    { ONIGERR_GROUP_NUMBER_OVER_FOR_CAPTURE_HISTORY,      REG_BADPAT },
    { ONIGERR_INVALID_CHAR_PROPERTY_NAME,                 REG_BADPAT },
    { ONIGERR_INVALID_IF_ELSE_SYNTAX,                     REG_BADPAT },
    { ONIGERR_INVALID_ABSENT_GROUP_PATTERN,               REG_BADPAT },
    { ONIGERR_INVALID_ABSENT_GROUP_GENERATOR_PATTERN,     REG_BADPAT },
    { ONIGERR_INVALID_CALLOUT_PATTERN,                    REG_BADPAT },
    { ONIGERR_INVALID_CALLOUT_NAME,                       REG_BADPAT },
    { ONIGERR_UNDEFINED_CALLOUT_NAME,                     REG_BADPAT },
    { ONIGERR_INVALID_CALLOUT_BODY,                       REG_BADPAT },
    { ONIGERR_INVALID_CALLOUT_TAG_NAME,                   REG_BADPAT },
    { ONIGERR_INVALID_CALLOUT_ARG,                        REG_BADPAT },
    { ONIGERR_NOT_SUPPORTED_ENCODING_COMBINATION,         REG_EONIG_BADARG },
    { ONIGERR_LIBRARY_IS_NOT_INITIALIZED,                 REG_EONIG_INTERNAL }
  };

  int i;

  if (code >= 0) return 0;

  for (i = 0; i < (int )(sizeof(o2p) / sizeof(o2p[0])); i++) {
    if (code == o2p[i].onig_err)
      return o2p[i].posix_err;
  }

  return REG_EONIG_INTERNAL;  /* but, unknown error code */
}

extern int
regcomp(regex_t* reg, const char* pattern, int posix_options)
{
  int r, len;
  OnigSyntaxType* syntax = OnigDefaultSyntax;
  OnigOptionType options;

  if ((posix_options & REG_EXTENDED) == 0)
    syntax = ONIG_SYNTAX_POSIX_BASIC;

  options = syntax->options;
  if ((posix_options & REG_ICASE)   != 0)
    ONIG_OPTION_ON(options, ONIG_OPTION_IGNORECASE);
  if ((posix_options & REG_NEWLINE) != 0) {
    ONIG_OPTION_ON( options, ONIG_OPTION_NEGATE_SINGLELINE);
    ONIG_OPTION_OFF(options, ONIG_OPTION_SINGLELINE);
  }

  reg->comp_options = posix_options;

  ENC_STRING_LEN(OnigEncDefaultCharEncoding, pattern, len);
  r = onig_new(PONIG_C(reg), (UChar* )pattern, (UChar* )(pattern + len),
	       options, OnigEncDefaultCharEncoding, syntax,
	       (OnigErrorInfo* )NULL);
  if (r != ONIG_NORMAL) {
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
  regmatch_t* pm;
  OnigOptionType options;

  options = ONIG_OPTION_POSIX_REGION;
  if ((posix_options & REG_NOTBOL) != 0) options |= ONIG_OPTION_NOTBOL;
  if ((posix_options & REG_NOTEOL) != 0) options |= ONIG_OPTION_NOTEOL;

  if (nmatch == 0 || (reg->comp_options & REG_NOSUB) != 0) {
    pm = (regmatch_t* )NULL;
    nmatch = 0;
  }
  else if ((int )nmatch < ONIG_C(reg)->num_mem + 1) {
    pm = (regmatch_t* )xmalloc(sizeof(regmatch_t)
                               * (ONIG_C(reg)->num_mem + 1));
    if (pm == NULL)
      return REG_ESPACE;
  }
  else {
    pm = pmatch;
  }

  ENC_STRING_LEN(ONIG_C(reg)->enc, str, len);
  end = (UChar* )(str + len);
  r = onig_search(ONIG_C(reg), (UChar* )str, end, (UChar* )str, end,
		  (OnigRegion* )pm, options);

  if (r >= 0) {
    r = 0; /* Match */
    if (pm != pmatch && pm != NULL) {
      xmemcpy(pmatch, pm, sizeof(regmatch_t) * nmatch);
    }
  }
  else if (r == ONIG_MISMATCH) {
    r = REG_NOMATCH;
    for (i = 0; i < (int )nmatch; i++)
      pmatch[i].rm_so = pmatch[i].rm_eo = ONIG_REGION_NOTPOS;
  }
  else {
    r = onig2posix_error_code(r);
  }

  if (pm != pmatch && pm != NULL)
    xfree(pm);

#if 0
  if (reg->re_nsub > nmatch - 1)
    reg->re_nsub = (nmatch <= 1 ? 0 : nmatch - 1);
#endif

  return r;
}

extern void
regfree(regex_t* reg)
{
  onig_free(ONIG_C(reg));
}


extern void
reg_set_encoding(int mb_code)
{
  OnigEncoding enc;

  switch (mb_code) {
  case REG_POSIX_ENCODING_ASCII:
    enc = ONIG_ENCODING_ASCII;
    break;
  case REG_POSIX_ENCODING_EUC_JP:
    enc = ONIG_ENCODING_EUC_JP;
    break;
  case REG_POSIX_ENCODING_SJIS:
    enc = ONIG_ENCODING_SJIS;
    break;
  case REG_POSIX_ENCODING_UTF8:
    enc = ONIG_ENCODING_UTF8;
    break;
  case REG_POSIX_ENCODING_UTF16_BE:
    enc = ONIG_ENCODING_UTF16_BE;
    break;
  case REG_POSIX_ENCODING_UTF16_LE:
    enc = ONIG_ENCODING_UTF16_LE;
    break;

  default:
    return ;
    break;
  }

  onig_initialize(&enc, 1);

  onigenc_set_default_encoding(enc);
}

extern int
reg_name_to_group_numbers(regex_t* reg,
  const unsigned char* name, const unsigned char* name_end, int** nums)
{
  return onig_name_to_group_numbers(ONIG_C(reg), name, name_end, nums);
}

typedef struct {
  int (*func)(const unsigned char*, const unsigned char*,int,int*,regex_t*,void*);
  regex_t* reg;
  void* arg;
} i_wrap;

static int
i_wrapper(const UChar* name, const UChar* name_end, int ng, int* gs,
	  onig_regex_t* reg ARG_UNUSED, void* arg)
{
  i_wrap* warg = (i_wrap* )arg;

  return (*warg->func)(name, name_end, ng, gs, warg->reg, warg->arg);
}

extern int
reg_foreach_name(regex_t* reg,
 int (*func)(const unsigned char*, const unsigned char*,int,int*,regex_t*,void*),
 void* arg)
{
  i_wrap warg;

  warg.func = func;
  warg.reg  = reg;
  warg.arg  = arg;

  return onig_foreach_name(ONIG_C(reg), i_wrapper, &warg);
}

extern int
reg_number_of_names(regex_t* reg)
{
  return onig_number_of_names(ONIG_C(reg));
}
