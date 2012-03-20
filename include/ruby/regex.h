/**********************************************************************

  regex.h -

  $Author$

  Copyright (C) 1993-2007 Yukihiro Matsumoto

**********************************************************************/

#ifndef ONIGURUMA_REGEX_H
#define ONIGURUMA_REGEX_H 1

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

#ifdef RUBY
#include "ruby/oniguruma.h"
#else
#include "oniguruma.h"
#endif

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility push(default)
#endif

#ifndef ONIG_RUBY_M17N

ONIG_EXTERN OnigEncoding    OnigEncDefaultCharEncoding;

#define mbclen(p,e,enc)  rb_enc_mbclen((p),(e),(enc))

#endif /* ifndef ONIG_RUBY_M17N */

#if defined __GNUC__ && __GNUC__ >= 4
#pragma GCC visibility pop
#endif

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif /* ONIGURUMA_REGEX_H */
