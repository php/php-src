/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2025, Sebastian Pipping <sebastian@pipping.org>
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

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriSetHostAuto.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetHostAuto.c"
#  undef URI_PASS_UNICODE
# endif
#else
# ifdef URI_PASS_ANSI
#  include <uriparser/UriDefsAnsi.h>
# else
#  include <uriparser/UriDefsUnicode.h>
#  include <wchar.h>
# endif



#ifndef URI_DOXYGEN
# include <uriparser/Uri.h>
# include "UriSetHostBase.h"
# include "UriSetHostCommon.h"
# include "UriMemory.h"
#endif



#include <assert.h>



int URI_FUNC(SetHostAutoMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if ((uri == NULL) || ((first == NULL) != (afterLast == NULL))) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	if ((first == NULL) || (first >= afterLast)) {
		return URI_FUNC(SetHostRegNameMm)(uri, first, afterLast, memory);
	}

	/* Auto-detect type and then apply */
	{
		UriHostType hostType;

		/* IPv6 or IPvFuture? */
		if (first[0] == _UT('[')) {
			if ((afterLast - first < 2) || (afterLast[-1] != _UT(']'))) {
				return URI_ERROR_SYNTAX;
			}

			/* Drop the bracket wrap (for InternalSetHostMm call below) */
			first++;
			afterLast--;

			if (first >= afterLast) {
			    return URI_ERROR_SYNTAX;
			}

			switch (first[0]) {
				case _UT('v'):
				case _UT('V'):
					hostType = URI_HOST_TYPE_IPFUTURE;
					break;
				default:
					hostType = URI_HOST_TYPE_IP6;
					break;
			}
		/* IPv4? */
		} else if (URI_FUNC(IsWellFormedHostIp4)(first, afterLast)) {
			hostType = URI_HOST_TYPE_IP4;
		} else {
			/* RegName! */
			hostType = URI_HOST_TYPE_REGNAME;
		}

		return URI_FUNC(InternalSetHostMm)(uri, hostType, first, afterLast, memory);
	}
}



int URI_FUNC(SetHostAuto)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetHostAutoMm)(uri, first, afterLast, NULL);
}



#endif
