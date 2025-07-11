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
 * @file UriIp4.h
 * Holds the IPv4 parser interface.
 * NOTE: This header includes itself twice.
 */

#if (defined(URI_PASS_ANSI) && !defined(URI_IP4_TWICE_H_ANSI)) \
	|| (defined(URI_PASS_UNICODE) && !defined(URI_IP4_TWICE_H_UNICODE)) \
	|| (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* What encodings are enabled? */
#include "UriDefsConfig.h"
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriIp4.h"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriIp4.h"
#  undef URI_PASS_UNICODE
# endif
/* Only one pass for each encoding */
#elif (defined(URI_PASS_ANSI) && !defined(URI_IP4_TWICE_H_ANSI) \
	&& defined(URI_ENABLE_ANSI)) || (defined(URI_PASS_UNICODE) \
	&& !defined(URI_IP4_TWICE_H_UNICODE) && defined(URI_ENABLE_UNICODE))
# ifdef URI_PASS_ANSI
#  define URI_IP4_TWICE_H_ANSI 1
#  include "UriDefsAnsi.h"
# else
#  define URI_IP4_TWICE_H_UNICODE 1
#  include "UriDefsUnicode.h"
#  include <wchar.h>
# endif



#ifdef __cplusplus
extern "C" {
#endif



#ifndef URI_DOXYGEN
# include "UriBase.h"
#endif



/**
 * Converts a IPv4 text representation into four bytes.
 *
 * @param octetOutput  Output destination
 * @param first        First character of IPv4 text to parse
 * @param afterLast    Position to stop parsing at
 * @return Error code or 0 on success
 */
URI_PUBLIC int URI_FUNC(ParseIpFourAddress)(unsigned char * octetOutput,
		const URI_CHAR * first, const URI_CHAR * afterLast);



#ifdef __cplusplus
}
#endif



#endif
#endif
