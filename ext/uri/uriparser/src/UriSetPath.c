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
#  include "UriSetPath.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetPath.c"
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



#define URI_SET_HEX_LETTER_UPPER \
	     _UT('A'): \
	case _UT('B'): \
	case _UT('C'): \
	case _UT('D'): \
	case _UT('E'): \
	case _UT('F')



#define URI_SET_HEX_LETTER_LOWER \
	     _UT('a'): \
	case _UT('b'): \
	case _UT('c'): \
	case _UT('d'): \
	case _UT('e'): \
	case _UT('f')



#define URI_SET_HEXDIG \
	URI_SET_DIGIT: \
	case URI_SET_HEX_LETTER_UPPER: \
	case URI_SET_HEX_LETTER_LOWER



#define URI_SET_ALPHA \
	URI_SET_HEX_LETTER_UPPER: \
	case URI_SET_HEX_LETTER_LOWER: \
	case _UT('g'): \
	case _UT('G'): \
	case _UT('h'): \
	case _UT('H'): \
	case _UT('i'): \
	case _UT('I'): \
	case _UT('j'): \
	case _UT('J'): \
	case _UT('k'): \
	case _UT('K'): \
	case _UT('l'): \
	case _UT('L'): \
	case _UT('m'): \
	case _UT('M'): \
	case _UT('n'): \
	case _UT('N'): \
	case _UT('o'): \
	case _UT('O'): \
	case _UT('p'): \
	case _UT('P'): \
	case _UT('q'): \
	case _UT('Q'): \
	case _UT('r'): \
	case _UT('R'): \
	case _UT('s'): \
	case _UT('S'): \
	case _UT('t'): \
	case _UT('T'): \
	case _UT('u'): \
	case _UT('U'): \
	case _UT('v'): \
	case _UT('V'): \
	case _UT('w'): \
	case _UT('W'): \
	case _UT('x'): \
	case _UT('X'): \
	case _UT('y'): \
	case _UT('Y'): \
	case _UT('z'): \
	case _UT('Z')



#define URI_SET_SUB_DELIMS \
	     _UT('!'): \
	case _UT('$'): \
	case _UT('&'): \
	case _UT('\''): \
	case _UT('('): \
	case _UT(')'): \
	case _UT('*'): \
	case _UT('+'): \
	case _UT(','): \
	case _UT(';'): \
	case _UT('=')



#define URI_SET_UNRESERVED \
	URI_SET_ALPHA: \
	case URI_SET_DIGIT: \
	case _UT('-'): \
	case _UT('.'): \
	case _UT('_'): \
	case _UT('~')



UriBool URI_FUNC(IsWellFormedPath)(const URI_CHAR * first, const URI_CHAR * afterLast, UriBool hasHost) {
	if ((first == NULL) || (afterLast == NULL)) {
		return URI_FALSE;
	}

	if ((hasHost == URI_TRUE) && ((first >= afterLast) || (first[0] != _UT('/')))) {
		return URI_FALSE;
	}

	/* The related part of the grammar in RFC 3986 (section 3.3) reads:
	 *
	 *   path          = path-abempty    ; begins with "/" or is empty
	 *                 / path-absolute   ; begins with "/" but not "//"
	 *                 / path-noscheme   ; begins with a non-colon segment
	 *                 / path-rootless   ; begins with a segment
	 *                 / path-empty      ; zero characters
	 *
	 *   path-abempty  = *( "/" segment )
	 *   path-absolute = "/" [ segment-nz *( "/" segment ) ]
	 *   path-noscheme = segment-nz-nc *( "/" segment )
	 *   path-rootless = segment-nz *( "/" segment )
	 *   path-empty    = 0<pchar>
	 *
	 *   segment       = *pchar
	 *   segment-nz    = 1*pchar
	 *   segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
	 *                 ; non-zero-length segment without any colon ":"
	 *
	 *   pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
	 *
	 * The check below simplifies this to ..
	 *
	 *   path          = *( unreserved / pct-encoded / sub-delims / ":" / "@" / "/" )
	 *
	 * .. and leaves the rest to pre-return removal of ambiguity
	 * from cases like "path1:/path2" and "//path1/path2" inside SetPath.
	 */
	while (first < afterLast) {
		switch (first[0]) {
			case URI_SET_UNRESERVED:
				break;

			/* pct-encoded */
			case _UT('%'):
				if (afterLast - first < 3) {
					return URI_FALSE;
				}
				switch (first[1]) {
					case URI_SET_HEXDIG:
						break;
					default:
						return URI_FALSE;
				}
				switch (first[2]) {
					case URI_SET_HEXDIG:
						break;
					default:
						return URI_FALSE;
				}
				first += 2;
				break;

			case URI_SET_SUB_DELIMS:
				break;

			/* ":" / "@" and "/" */
			case _UT(':'):
			case _UT('@'):
			case _UT('/'):
				break;

			default:
				return URI_FALSE;
		}

		first++;
	}
	return URI_TRUE;
}



static void URI_FUNC(DropEmptyFirstPathSegment)(URI_TYPE(Uri) * uri, UriMemoryManager * memory) {
	assert(uri != NULL);
	assert(memory != NULL);
	assert(uri->pathHead != NULL);
	assert(uri->pathHead->text.first == uri->pathHead->text.afterLast);

	{
		URI_TYPE(PathSegment) * const originalHead = uri->pathHead;

		uri->pathHead = uri->pathHead->next;

		originalHead->text.first = NULL;
		originalHead->text.afterLast = NULL;
		memory->free(memory, originalHead);
	}
}



/* URIs without a host encode a leading slash in the path as .absolutePath == URI_TRUE.
 * This function checks for a leading empty path segment (that would have the "visual effect"
 * of a leading slash during stringification) and transforms it into .absolutePath == URI_TRUE
 * instead, if present. */
static void URI_FUNC(TransformEmptyLeadPathSegments)(URI_TYPE(Uri) * uri, UriMemoryManager * memory) {
	assert(uri != NULL);
	assert(memory != NULL);

	if ((URI_FUNC(HasHost)(uri) == URI_TRUE)
			|| (uri->pathHead == NULL)
			|| (uri->pathHead->text.first != uri->pathHead->text.afterLast)) {
		return;  /* i.e. nothing to do */
	}

	assert(uri->absolutePath == URI_FALSE);

	URI_FUNC(DropEmptyFirstPathSegment)(uri, memory);

	uri->absolutePath = URI_TRUE;
}



static int URI_FUNC(InternalSetPath)(URI_TYPE(Uri) * destUri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	assert(destUri != NULL);
	assert(first != NULL);
	assert(afterLast != NULL);
	assert(memory != NULL);
	assert(destUri->pathHead == NULL);  /* set by SetPathMm right before */
	assert(destUri->pathTail == NULL);  /* set by SetPathMm right before */
	assert(destUri->absolutePath == URI_FALSE);  /* set by SetPathMm right before */

	/* Skip the leading slash from target URIs with a host (so that we can
	 * transfer the path 1:1 further down) */
	if (URI_FUNC(HasHost)(destUri) == URI_TRUE) {
		/* NOTE: This is because SetPathMm called IsWellFormedPath earlier: */
		assert((afterLast - first >= 1) && (first[0] == _UT('/')));
		first++;
	} else if (first == afterLast) {
	    /* This avoids (1) all the expensive but unnecessary work below
	     * and also (2) mis-encoding as single empty path segment
	     * that would need (detection and) repair further down otherwise */
	    return URI_SUCCESS;
	}

	/* Assemble "///.." input wrap for upcoming parse as a URI */
	{
		const size_t inputLenChars = (afterLast - first);
		const size_t MAX_SIZE_T = (size_t)-1;

		/* Detect overflow */
		if (MAX_SIZE_T - inputLenChars < 3 + 1) {
			return URI_ERROR_MALLOC;
		}

		{
			const size_t candidateLenChars = 3 + inputLenChars;

			/* Detect overflow */
			if (MAX_SIZE_T / sizeof(URI_CHAR) < candidateLenChars + 1) {
				return URI_ERROR_MALLOC;
			}

			{
				URI_CHAR * const candidate = memory->malloc(memory, (candidateLenChars + 1) * sizeof(URI_CHAR));

				if (candidate == NULL) {
					return URI_ERROR_MALLOC;
				}

				memcpy(candidate, _UT("///"), 3 * sizeof(URI_CHAR));
				memcpy(candidate + 3, first, inputLenChars * sizeof(URI_CHAR));
				candidate[3 + inputLenChars] = _UT('\0');

				/* Parse as an RFC 3986 URI */
				{
					URI_TYPE(Uri) tempUri;
					const int res = URI_FUNC(ParseSingleUriExMm)(&tempUri,
							candidate,
							candidate + candidateLenChars,
							NULL,
							memory);
					assert((res == URI_SUCCESS) || (res == URI_ERROR_SYNTAX) || (res == URI_ERROR_MALLOC));
					if (res != URI_SUCCESS) {
						memory->free(memory, candidate);
						return res;
					}

					/* Nothing but path and host is supposed to be set by the parse, in particular not: */
					assert(tempUri.query.first == NULL);
					assert(tempUri.fragment.first == NULL);

					/* Ensure that the strings in the path segments are all owned by `tempUri`
					 * because we want to (1) rip out and keep the full path list further down
					 * and (2) be able to free the parsed string (`candidate`) also. */
					{
						const int res = URI_FUNC(MakeOwnerMm)(&tempUri, memory);
						assert((res == URI_SUCCESS) || (res == URI_ERROR_MALLOC));
						if (res != URI_SUCCESS) {
							URI_FUNC(FreeUriMembersMm)(&tempUri, memory);
							memory->free(memory, candidate);
							return res;
						}
						assert(tempUri.owner == URI_TRUE);
					}

					/* Move path to destination URI */
					assert(tempUri.absolutePath == URI_FALSE);  /* always URI_FALSE for URIs with host */
					destUri->pathHead = tempUri.pathHead;
					destUri->pathTail = tempUri.pathTail;
					destUri->absolutePath = URI_FALSE;

					tempUri.pathHead = NULL;
					tempUri.pathTail = NULL;

					/* Free the rest of the temp URI */
					URI_FUNC(FreeUriMembersMm)(&tempUri, memory);
					memory->free(memory, candidate);

					/* Restore use of .absolutePath as needed */
					URI_FUNC(TransformEmptyLeadPathSegments)(destUri, memory);

					/* Disambiguate as needed */
					{
						const UriBool success = URI_FUNC(FixPathNoScheme)(destUri, memory);
						if (success == URI_FALSE) {
							return URI_ERROR_MALLOC;
						}
					}
					{
						const UriBool success = URI_FUNC(EnsureThatPathIsNotMistakenForHost)(destUri, memory);
						if (success == URI_FALSE) {
							return URI_ERROR_MALLOC;
						}
					}
				}
			}
		}
	}

	return URI_SUCCESS;
}



int URI_FUNC(SetPathMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	/* Input validation (before making any changes) */
	if ((uri == NULL) || ((first == NULL) != (afterLast == NULL))) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	if ((first != NULL) && (URI_FUNC(IsWellFormedPath)(first, afterLast, URI_FUNC(HasHost)(uri)) == URI_FALSE)) {
		return URI_ERROR_SYNTAX;
	}

	/* Clear old value */
	{
		const int res = URI_FUNC(FreeUriPath)(uri, memory);
		if (res != URI_SUCCESS) {
			return res;
		}
		uri->absolutePath = URI_FALSE;
	}

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
		const int res = URI_FUNC(InternalSetPath)(uri, first, afterLast, memory);
		assert((res == URI_SUCCESS) || (res == URI_ERROR_SYNTAX) || (res == URI_ERROR_MALLOC));
		return res;
	}
}



int URI_FUNC(SetPath)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetPathMm)(uri, first, afterLast, NULL);
}



#endif
