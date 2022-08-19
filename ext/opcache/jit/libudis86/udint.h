/* udis86 - libudis86/udint.h -- definitions for internal use only
 * 
 * Copyright (c) 2002-2009 Vivek Thampi
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, 
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, 
 *       this list of conditions and the following disclaimer in the documentation 
 *       and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _UDINT_H_
#define _UDINT_H_

#include "types.h"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */

#if defined(UD_DEBUG) && HAVE_ASSERT_H
# include <assert.h>
# define UD_ASSERT(_x) assert(_x)
#else
# define UD_ASSERT(_x)
#endif /* !HAVE_ASSERT_H */

#if defined(UD_DEBUG)
  #define UDERR(u, msg) \
    do { \
      (u)->error = 1; \
      fprintf(stderr, "decode-error: %s:%d: %s", \
              __FILE__, __LINE__, (msg)); \
    } while (0)
#else
  #define UDERR(u, m) \
    do { \
      (u)->error = 1; \
    } while (0)
#endif /* !LOGERR */

#define UD_RETURN_ON_ERROR(u) \
  do { \
    if ((u)->error != 0) { \
      return (u)->error; \
    } \
  } while (0)

#define UD_RETURN_WITH_ERROR(u, m) \
  do { \
    UDERR(u, m); \
    return (u)->error; \
  } while (0)

#ifndef __UD_STANDALONE__
# define UD_NON_STANDALONE(x) x
#else
# define UD_NON_STANDALONE(x)
#endif

/* printf formatting int64 specifier */
#ifdef FMT64
# undef FMT64
#endif
#if defined(_MSC_VER) || defined(__BORLANDC__)
# define FMT64 "I64"
#else
# if defined(__APPLE__) || defined(__OpenBSD__)
#  define FMT64 "ll"
# elif defined(__amd64__) || defined(__x86_64__)
#  define FMT64 "l"
# else 
#  define FMT64 "ll"
# endif /* !x64 */
#endif

/* define an inline macro */
#if defined(_MSC_VER) || defined(__BORLANDC__)
# define UD_INLINE __inline /* MS Visual Studio requires __inline
                               instead of inline for C code */
#else
# define UD_INLINE inline
#endif

#endif /* _UDINT_H_ */
