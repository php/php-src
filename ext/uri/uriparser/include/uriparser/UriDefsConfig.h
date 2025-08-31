/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <sebastian@pipping.org>
 * All rights reserved.
 *
 * Redistribution and use in source  and binary forms, with or without
 * modification, are permitted provided  that the following conditions
 * are met:
 *
 *     1. Redistributions  of  source  code   must  retain  the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer.
 *
 *     2. Redistributions  in binary  form  must  reproduce the  above
 *        copyright notice, this list  of conditions and the following
 *        disclaimer  in  the  documentation  and/or  other  materials
 *        provided with the distribution.
 *
 *     3. Neither the  name of the  copyright holder nor the  names of
 *        its contributors may be used  to endorse or promote products
 *        derived from  this software  without specific  prior written
 *        permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY EXPRESS OR IMPLIED WARRANTIES,  INCLUDING, BUT NOT
 * LIMITED TO,  THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS
 * FOR  A  PARTICULAR  PURPOSE  ARE  DISCLAIMED.  IN  NO  EVENT  SHALL
 * THE  COPYRIGHT HOLDER  OR CONTRIBUTORS  BE LIABLE  FOR ANY  DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO,  PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA,  OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT  LIABILITY,  OR  TORT (INCLUDING  NEGLIGENCE  OR  OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file UriDefsConfig.h
 * Adjusts the internal configuration after processing external definitions.
 */

#ifndef URI_DEFS_CONFIG_H
#define URI_DEFS_CONFIG_H 1



/* Deny external overriding */
#undef URI_ENABLE_ANSI      /* Internal for !URI_NO_ANSI */
#undef URI_ENABLE_UNICODE   /* Internal for !URI_NO_UNICODE */



/* Encoding */
#ifdef URI_NO_ANSI
# ifdef URI_NO_UNICODE
/* No encoding at all */
#  error URI_NO_ANSI and URI_NO_UNICODE cannot go together.
# else
/* Wide strings only */
#  define URI_ENABLE_UNICODE  1
# endif
#else
# ifdef URI_NO_UNICODE
/* Narrow strings only */
#  define URI_ENABLE_ANSI     1
# else
/* Both narrow and wide strings */
#  define URI_ENABLE_ANSI     1
#  define URI_ENABLE_UNICODE  1
# endif
#endif



/* Function inlining, not ANSI/ISO C! */
#if defined(URI_DOXYGEN)
# define URI_INLINE
#elif defined(__INTEL_COMPILER)
/* Intel C/C++ */
/* http://predef.sourceforge.net/precomp.html#sec20 */
/* http://www.intel.com/support/performancetools/c/windows/sb/CS-007751.htm#2 */
# define URI_INLINE __forceinline
#elif defined(_MSC_VER)
/* Microsoft Visual C++ */
/* http://predef.sourceforge.net/precomp.html#sec32 */
/* http://msdn2.microsoft.com/en-us/library/ms882281.aspx */
# define URI_INLINE __forceinline
#elif (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))
/* C99, "inline" is a keyword */
# define URI_INLINE inline
#else
/* No inlining */
# define URI_INLINE
#endif



#endif /* URI_DEFS_CONFIG_H */
