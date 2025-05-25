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

#if (defined(URI_PASS_ANSI) && !defined(URI_COMMON_H_ANSI)) \
	|| (defined(URI_PASS_UNICODE) && !defined(URI_COMMON_H_UNICODE)) \
	|| (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriCommon.h"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriCommon.h"
#  undef URI_PASS_UNICODE
# endif
/* Only one pass for each encoding */
#elif (defined(URI_PASS_ANSI) && !defined(URI_COMMON_H_ANSI) \
	&& defined(URI_ENABLE_ANSI)) || (defined(URI_PASS_UNICODE) \
	&& !defined(URI_COMMON_H_UNICODE) && defined(URI_ENABLE_UNICODE))
# ifdef URI_PASS_ANSI
#  define URI_COMMON_H_ANSI 1
#  include <uriparser/UriDefsAnsi.h>
# else
#  define URI_COMMON_H_UNICODE 1
#  include <uriparser/UriDefsUnicode.h>
# endif



/* Used to point to from empty path segments.
 * X.first and X.afterLast must be the same non-NULL value then. */
extern const URI_CHAR * const URI_FUNC(SafeToPointTo);
extern const URI_CHAR * const URI_FUNC(ConstPwd);
extern const URI_CHAR * const URI_FUNC(ConstParent);



void URI_FUNC(ResetUri)(URI_TYPE(Uri) * uri);

int URI_FUNC(CompareRange)(
		const URI_TYPE(TextRange) * a,
		const URI_TYPE(TextRange) * b);

UriBool URI_FUNC(RemoveDotSegmentsAbsolute)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory);
UriBool URI_FUNC(RemoveDotSegmentsEx)(URI_TYPE(Uri) * uri,
		UriBool relative, UriBool pathOwned, UriMemoryManager * memory);

unsigned char URI_FUNC(HexdigToInt)(URI_CHAR hexdig);
URI_CHAR URI_FUNC(HexToLetter)(unsigned int value);
URI_CHAR URI_FUNC(HexToLetterEx)(unsigned int value, UriBool uppercase);

UriBool URI_FUNC(IsHostSet)(const URI_TYPE(Uri) * uri);

UriBool URI_FUNC(CopyPath)(URI_TYPE(Uri) * dest, const URI_TYPE(Uri) * source,
		UriMemoryManager * memory);
UriBool URI_FUNC(CopyAuthority)(URI_TYPE(Uri) * dest,
		const URI_TYPE(Uri) * source, UriMemoryManager * memory);

UriBool URI_FUNC(FixAmbiguity)(URI_TYPE(Uri) * uri, UriMemoryManager * memory);
void URI_FUNC(FixEmptyTrailSegment)(URI_TYPE(Uri) * uri,
		UriMemoryManager * memory);


#endif
#endif
