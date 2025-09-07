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
#  include "UriSetHostIpFuture.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetHostIpFuture.c"
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
# include "UriMemory.h"
# include "UriSetHostBase.h"
# include "UriSetHostCommon.h"
#endif



#include <assert.h>
#include <string.h>  /* for memcpy */



int URI_FUNC(IsWellFormedHostIpFutureMm)(const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if ((first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	/* Are we dealing with potential IPv6 input? */
	if (first < afterLast) {
		switch (first[0]) {
			case _UT('v'):
			case _UT('V'):
				break;
			default:
				return URI_ERROR_SYNTAX;
		}
	}

	/* Are we dealing with IPvFuture input? */
	{
		/* Assemble "//[..]" input wrap for upcoming parse as a URI
		 * NOTE: If the input contains closing "]" on its own, the resulting
		 *       string will not be valid URI syntax, and hence there is
		 *       no risk of false positives from "bracket injection". */
		const size_t inputLenChars = (afterLast - first);
		const size_t MAX_SIZE_T = (size_t)-1;

		/* Detect overflow */
		if (MAX_SIZE_T - inputLenChars < 3 + 1 + 1) {
		    return URI_ERROR_MALLOC;
		}

		{
			const size_t candidateLenChars = 3 + inputLenChars + 1;

			/* Detect overflow */
			if (MAX_SIZE_T / sizeof(URI_CHAR) < candidateLenChars + 1) {
				return URI_ERROR_MALLOC;
			}

			{
				URI_CHAR * const candidate = memory->malloc(memory, (candidateLenChars + 1) * sizeof(URI_CHAR));

				if (candidate == NULL) {
					return URI_ERROR_MALLOC;
				}

				memcpy(candidate, _UT("//["), 3 * sizeof(URI_CHAR));
				memcpy(candidate + 3, first, inputLenChars * sizeof(URI_CHAR));
				memcpy(candidate + 3 + inputLenChars, _UT("]"), 2 * sizeof(URI_CHAR));  /* includes zero terminator */

				/* Parse as an RFC 3986 URI */
				{
					URI_TYPE(Uri) uri;
					const int res = URI_FUNC(ParseSingleUriExMm)(&uri, candidate, candidate + candidateLenChars, NULL, memory);

					assert((res == URI_SUCCESS) || (res == URI_ERROR_SYNTAX) || (res == URI_ERROR_MALLOC));

					if (res == URI_SUCCESS) {
						assert(uri.hostData.ipFuture.first != NULL);
						URI_FUNC(FreeUriMembersMm)(&uri, memory);
					}

					memory->free(memory, candidate);

					return res;
				}
			}
		}
	}
}



int URI_FUNC(IsWellFormedHostIpFuture)(const URI_CHAR * first, const URI_CHAR * afterLast) {
	return URI_FUNC(IsWellFormedHostIpFutureMm)(first, afterLast, NULL);
}



int URI_FUNC(SetHostIpFutureMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	return URI_FUNC(InternalSetHostMm)(uri, URI_HOST_TYPE_IPFUTURE, first, afterLast, memory);
}



int URI_FUNC(SetHostIpFuture)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetHostIpFutureMm)(uri, first, afterLast, NULL);
}



#endif
