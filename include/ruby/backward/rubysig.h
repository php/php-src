/**********************************************************************

  rubysig.h -

  $Author$
  $Date$
  created at: Wed Aug 16 01:15:38 JST 1995

  Copyright (C) 1993-2008 Yukihiro Matsumoto

**********************************************************************/

#if   defined __GNUC__
#warning rubysig.h is obsolete
#elif defined _MSC_VER || defined __BORLANDC__
#pragma message("warning: rubysig.h is obsolete")
#endif

#ifndef RUBYSIG_H
#define RUBYSIG_H
#include "ruby/ruby.h"

#if defined(__cplusplus)
extern "C" {
#if 0
} /* satisfy cc-mode */
#endif
#endif

struct rb_blocking_region_buffer;
DEPRECATED(RUBY_EXTERN struct rb_blocking_region_buffer *rb_thread_blocking_region_begin(void));
DEPRECATED(RUBY_EXTERN void rb_thread_blocking_region_end(struct rb_blocking_region_buffer *));
#define TRAP_BEG do {struct rb_blocking_region_buffer *__region = rb_thread_blocking_region_begin();
#define TRAP_END rb_thread_blocking_region_end(__region);} while (0)
#define RUBY_CRITICAL(statements) do {statements;} while (0)
#define DEFER_INTS (0)
#define ENABLE_INTS (1)
#define ALLOW_INTS do {CHECK_INTS;} while (0)
#define CHECK_INTS rb_thread_check_ints()

#if defined(__cplusplus)
#if 0
{ /* satisfy cc-mode */
#endif
}  /* extern "C" { */
#endif

#endif
