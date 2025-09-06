#ifndef ONIGPOSIX_H
#define ONIGPOSIX_H
/**********************************************************************
  onigposix.h - Oniguruma (regular expression library)
**********************************************************************/
/*-
 * Copyright (c) 2002-2020  K.Kosako
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
#ifndef ONIG_NO_STANDARD_C_HEADERS
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* options */
#define REG_ICASE          (1<<0)
#define REG_NEWLINE        (1<<1)
#define REG_NOTBOL         (1<<2)
#define REG_NOTEOL         (1<<3)
#define REG_EXTENDED       (1<<4) /* if not set, Basic Onigular Expression */
#define REG_NOSUB          (1<<5)

/* POSIX error codes */
#define REG_NOMATCH          1
#define REG_BADPAT           2
#define REG_ECOLLATE         3
#define REG_ECTYPE           4
#define REG_EESCAPE          5
#define REG_ESUBREG          6
#define REG_EBRACK           7
#define REG_EPAREN           8
#define REG_EBRACE           9
#define REG_BADBR           10
#define REG_ERANGE          11
#define REG_ESPACE          12
#define REG_BADRPT          13

/* extended error codes */
#define REG_EONIG_INTERNAL  14
#define REG_EONIG_BADWC     15
#define REG_EONIG_BADARG    16
/* #define REG_EONIG_THREAD    17 */

/* character encodings (for reg_set_encoding()) */
#define REG_POSIX_ENCODING_ASCII     0
#define REG_POSIX_ENCODING_EUC_JP    1
#define REG_POSIX_ENCODING_SJIS      2
#define REG_POSIX_ENCODING_UTF8      3
#define REG_POSIX_ENCODING_UTF16_BE  4
#define REG_POSIX_ENCODING_UTF16_LE  5


typedef int onig_posix_regoff_t;

typedef struct {
  onig_posix_regoff_t  rm_so;
  onig_posix_regoff_t  rm_eo;
} onig_posix_regmatch_t;

/* POSIX regex_t */
typedef struct {
  void*   onig;          /* Oniguruma regex_t*  */
  size_t  re_nsub;
  int     comp_options;
} onig_posix_regex_t;


#ifndef P_
#if defined(__STDC__) || defined(_WIN32)
# define P_(args) args
#else
# define P_(args) ()
#endif
#endif

#ifndef ONIG_STATIC
#ifndef ONIG_EXTERN
#if defined(_WIN32) && !defined(__GNUC__)
#if defined(ONIGURUMA_EXPORT)
#define ONIG_EXTERN   extern __declspec(dllexport)
#else
#define ONIG_EXTERN   extern __declspec(dllimport)
#endif
#endif
#endif

#ifndef ONIG_EXTERN
#define ONIG_EXTERN   extern
#endif
#else
#define ONIG_EXTERN   extern
#endif

#ifndef ONIGURUMA_H
typedef unsigned int        OnigOptionType;

/* syntax */
typedef struct {
  unsigned int op;
  unsigned int op2;
  unsigned int behavior;
  OnigOptionType options;    /* default option */
} OnigSyntaxType;

ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixBasic;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPosixExtended;
ONIG_EXTERN OnigSyntaxType OnigSyntaxEmacs;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGrep;
ONIG_EXTERN OnigSyntaxType OnigSyntaxGnuRegex;
ONIG_EXTERN OnigSyntaxType OnigSyntaxJava;
ONIG_EXTERN OnigSyntaxType OnigSyntaxPerl;
ONIG_EXTERN OnigSyntaxType OnigSyntaxRuby;
ONIG_EXTERN OnigSyntaxType OnigSyntaxOniguruma;

/* predefined syntaxes (see regsyntax.c) */
#define ONIG_SYNTAX_POSIX_BASIC        (&OnigSyntaxPosixBasic)
#define ONIG_SYNTAX_POSIX_EXTENDED     (&OnigSyntaxPosixExtended)
#define ONIG_SYNTAX_EMACS              (&OnigSyntaxEmacs)
#define ONIG_SYNTAX_GREP               (&OnigSyntaxGrep)
#define ONIG_SYNTAX_GNU_REGEX          (&OnigSyntaxGnuRegex)
#define ONIG_SYNTAX_JAVA               (&OnigSyntaxJava)
#define ONIG_SYNTAX_PERL               (&OnigSyntaxPerl)
#define ONIG_SYNTAX_RUBY               (&OnigSyntaxRuby)
#define ONIG_SYNTAX_ONIGURUMA          (&OnigSyntaxOniguruma)
/* default syntax */
#define ONIG_SYNTAX_DEFAULT             OnigDefaultSyntax

ONIG_EXTERN OnigSyntaxType*  OnigDefaultSyntax;

ONIG_EXTERN int  onig_set_default_syntax P_((OnigSyntaxType* syntax));
ONIG_EXTERN void onig_copy_syntax P_((OnigSyntaxType* to, OnigSyntaxType* from));
ONIG_EXTERN const char* onig_version P_((void));
ONIG_EXTERN const char* onig_copyright P_((void));
ONIG_EXTERN int onig_end P_((void));

#endif /* ONIGURUMA_H */


ONIG_EXTERN int    onig_posix_regcomp P_((onig_posix_regex_t* reg, const char* pat, int options));
ONIG_EXTERN int    onig_posix_regexec P_((onig_posix_regex_t* reg, const char* str, size_t nmatch, onig_posix_regmatch_t* matches, int options));
ONIG_EXTERN void   onig_posix_regfree P_((onig_posix_regex_t* reg));
ONIG_EXTERN size_t onig_posix_regerror P_((int code, const onig_posix_regex_t* reg, char* buf, size_t size));

/* extended API */
ONIG_EXTERN void onig_posix_reg_set_encoding P_((int enc));
ONIG_EXTERN int  onig_posix_reg_name_to_group_numbers P_((onig_posix_regex_t* reg, const unsigned char* name, const unsigned char* name_end, int** nums));
ONIG_EXTERN int  onig_posix_reg_foreach_name P_((onig_posix_regex_t* reg, int (*func)(const unsigned char*, const unsigned char*,int,int*,onig_posix_regex_t*,void*), void* arg));
ONIG_EXTERN int  onig_posix_reg_number_of_names P_((onig_posix_regex_t* reg));


/* aliases */
#define regex_t    onig_posix_regex_t
#define regmatch_t onig_posix_regmatch_t
#define regoff_t   onig_posix_regoff_t

#define regcomp  onig_posix_regcomp
#define regexec  onig_posix_regexec
#define regfree  onig_posix_regfree
#define regerror onig_posix_regerror
#define reg_set_encoding          onig_posix_reg_set_encoding
#define reg_name_to_group_numbers onig_posix_reg_name_to_group_numbers
#define reg_foreach_name          onig_posix_reg_foreach_name
#define reg_number_of_names       onig_posix_reg_number_of_names

#ifdef __cplusplus
}
#endif

#endif /* ONIGPOSIX_H */
