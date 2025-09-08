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
#  include "UriSetHostIp6.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetHostIp6.c"
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



#define URI_MAX_IP6_LEN (8 * 4 + 7 * 1)  /* i.e. 8 full quads plus 7 colon separators */



int URI_FUNC(ParseIpSixAddressMm)(UriIp6 * output,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	/* NOTE: output is allowed to be NULL */
	if ((first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	/* Are we dealing with potential IPvFuture input? */
	if (first < afterLast) {
		switch (first[0]) {
			case _UT('v'):
			case _UT('V'):
				return URI_ERROR_SYNTAX;
			default:
				break;
		}
	}

	/* Are we dealing with IPv6 input? */
	{
		/* Assemble "//[..]" input wrap for upcoming parse as a URI
		 * NOTE: If the input contains closing "]" on its own, the resulting
		 *       string will not be valid URI syntax, and hence there is
		 *       no risk of false positives from "bracket injection". */
		URI_CHAR candidate[3 + URI_MAX_IP6_LEN + 1 + 1] = _UT("//[");
		const size_t inputLenChars = (afterLast - first);

		/* Detect overflow */
		if (inputLenChars > URI_MAX_IP6_LEN) {
			return URI_ERROR_SYNTAX;
		}

		memcpy(candidate + 3, first, inputLenChars * sizeof(URI_CHAR));
		memcpy(candidate + 3 + inputLenChars, _UT("]"), 2 * sizeof(URI_CHAR));  /* includes zero terminator */

		/* Parse as an RFC 3986 URI */
		{
			const size_t candidateLenChars = 3 + inputLenChars + 1;
			URI_TYPE(Uri) uri;
			const int res = URI_FUNC(ParseSingleUriExMm)(&uri, candidate, candidate + candidateLenChars, NULL, memory);

			assert((res == URI_SUCCESS) || (res == URI_ERROR_SYNTAX) || (res == URI_ERROR_MALLOC));

			if (res == URI_SUCCESS) {
				assert(uri.hostData.ip6 != NULL);

				if (output != NULL) {
					memcpy(output->data, uri.hostData.ip6->data, sizeof(output->data));
				}

				URI_FUNC(FreeUriMembersMm)(&uri, memory);
			}

			return res;
		}
	}
}



int URI_FUNC(ParseIpSixAddress)(UriIp6 * output,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(ParseIpSixAddressMm)(output, first, afterLast, NULL);
}



int URI_FUNC(IsWellFormedHostIp6Mm)(const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory) {
    return URI_FUNC(ParseIpSixAddressMm)(NULL, first, afterLast, memory);
}



int URI_FUNC(IsWellFormedHostIp6)(const URI_CHAR * first, const URI_CHAR * afterLast) {
	return URI_FUNC(IsWellFormedHostIp6Mm)(first, afterLast, NULL);
}



int URI_FUNC(SetHostIp6Mm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	return URI_FUNC(InternalSetHostMm)(uri, URI_HOST_TYPE_IP6, first, afterLast, memory);
}



int URI_FUNC(SetHostIp6)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetHostIp6Mm)(uri, first, afterLast, NULL);
}



#endif
