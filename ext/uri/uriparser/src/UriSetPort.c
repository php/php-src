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
#  include "UriSetPort.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetPort.c"
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
# include "UriCommon.h"
# include "UriMemory.h"
#endif



#include <assert.h>



#define URI_SET_DIGIT \
	     _UT('0'): \
	case _UT('1'): \
	case _UT('2'): \
	case _UT('3'): \
	case _UT('4'): \
	case _UT('5'): \
	case _UT('6'): \
	case _UT('7'): \
	case _UT('8'): \
	case _UT('9')



UriBool URI_FUNC(IsWellFormedPort)(const URI_CHAR * first, const URI_CHAR * afterLast) {
	if ((first == NULL) || (afterLast == NULL)) {
		return URI_FALSE;
	}

	/* NOTE: Grammar reads "port = *DIGIT" which includes the empty string. */
	while (first < afterLast) {
		switch (first[0]) {
			case URI_SET_DIGIT:
				break;
			default:
				return URI_FALSE;
		}
		first++;
	}
	return URI_TRUE;
}



int URI_FUNC(SetPortTextMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	/* Input validation (before making any changes) */
	if ((uri == NULL) || ((first == NULL) != (afterLast == NULL))) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	/* The RFC 3986 grammar reads:
	 *   authority = [ userinfo "@" ] host [ ":" port ]
	 * So no port without a host. */
	if ((first != NULL) && (URI_FUNC(HasHost)(uri) == URI_FALSE)) {
		return URI_ERROR_SETPORT_HOST_NOT_SET;
	}

	if ((first != NULL) && (URI_FUNC(IsWellFormedPort)(first, afterLast) == URI_FALSE)) {
		return URI_ERROR_SYNTAX;
	}

	/* Clear old value */
	if ((uri->owner == URI_TRUE) && (uri->portText.first != uri->portText.afterLast)) {
		memory->free(memory, (URI_CHAR *)uri->portText.first);
	}
	uri->portText.first = NULL;
	uri->portText.afterLast = NULL;

	/* Already done? */
	if (first == NULL) {
		return URI_SUCCESS;
	}

	assert(first != NULL);

	/* Ensure owned */
	if (uri->owner == URI_FALSE) {
		const int res = URI_FUNC(MakeOwnerMm)(uri, memory);
		if (res != URI_SUCCESS) {
			return res;
		}
	}

	assert(uri->owner == URI_TRUE);

	/* Apply new value */
	{
		URI_TYPE(TextRange) sourceRange;
		sourceRange.first = first;
		sourceRange.afterLast = afterLast;

		if (URI_FUNC(CopyRangeAsNeeded)(&uri->portText, &sourceRange, memory) == URI_FALSE) {
			return URI_ERROR_MALLOC;
		}
	}
	
	return URI_SUCCESS;
}



int URI_FUNC(SetPortText)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetPortTextMm)(uri, first, afterLast, NULL);
}



#endif
