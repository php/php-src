/**********************************************************************

  oniggnu.h - Oniguruma (regular expression library)

  Copyright (C) 2004  K.Kosako (kosako@sofnec.co.jp)

**********************************************************************/
#ifndef ONIGGNU_H
#define ONIGGNU_H

#include "oniguruma.h"

#define MBCTYPE_ASCII         0
#define MBCTYPE_EUC           1
#define MBCTYPE_SJIS          2
#define MBCTYPE_UTF8          3

/* GNU regex options */
#ifndef RE_NREGS
#define RE_NREGS                ONIG_NREGION
#endif
#define RE_OPTION_IGNORECASE    ONIG_OPTION_IGNORECASE
#define RE_OPTION_EXTENDED      ONIG_OPTION_EXTEND
#define RE_OPTION_MULTILINE     ONIG_OPTION_MULTILINE
#define RE_OPTION_SINGLELINE    ONIG_OPTION_SINGLELINE
#define RE_OPTION_LONGEST       ONIG_OPTION_FIND_LONGEST
#define RE_OPTION_POSIXLINE    (RE_OPTION_MULTILINE|RE_OPTION_SINGLELINE)

#ifdef RUBY_PLATFORM
#define re_mbcinit              ruby_re_mbcinit
#define re_compile_pattern      ruby_re_compile_pattern
#define re_recompile_pattern    ruby_re_recompile_pattern
#define re_free_pattern         ruby_re_free_pattern
#define re_adjust_startpos      ruby_re_adjust_startpos
#define re_search               ruby_re_search
#define re_match                ruby_re_match
#define re_set_casetable        ruby_re_set_casetable
#define re_copy_registers       ruby_re_copy_registers
#define re_free_registers       ruby_re_free_registers
#define register_info_type      ruby_register_info_type
#define re_error_code_to_str    ruby_error_code_to_str

#define ruby_error_code_to_str  onig_error_code_to_str
#define ruby_re_copy_registers  onig_region_copy
#else
#define re_error_code_to_str    onig_error_code_to_str
#define re_copy_registers       onig_region_copy
#endif

#ifdef ONIG_RUBY_M17N
ONIG_EXTERN
void re_mbcinit P_((OnigEncoding));
#else
ONIG_EXTERN
void re_mbcinit P_((int));
#endif

ONIG_EXTERN
int re_compile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
ONIG_EXTERN
int re_recompile_pattern P_((const char*, int, struct re_pattern_buffer*, char* err_buf));
ONIG_EXTERN
void re_free_pattern P_((struct re_pattern_buffer*));
ONIG_EXTERN
int re_adjust_startpos P_((struct re_pattern_buffer*, const char*, int, int, int));
ONIG_EXTERN
int re_search P_((struct re_pattern_buffer*, const char*, int, int, int, struct re_registers*));
ONIG_EXTERN
int re_match P_((struct re_pattern_buffer*, const char *, int, int, struct re_registers*));
ONIG_EXTERN
void re_set_casetable P_((const char*));
ONIG_EXTERN
void re_free_registers P_((struct re_registers*));
ONIG_EXTERN
int re_alloc_pattern P_((struct re_pattern_buffer**));  /* added */

#endif /* ONIGGNU_H */
