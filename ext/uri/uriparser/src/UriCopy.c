/*
 * uriparser - RFC 3986 URI parsing library
 *
 * Copyright (C) 2007, Weijia Song <songweijia@gmail.com>
 * Copyright (C) 2007, Sebastian Pipping <sebastian@pipping.org>
 * Copyright (C) 2025, Máté Kocsis <kocsismate@php.net>
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
 * @file UriCopy.c
 * Holds the RFC 3986 %URI normalization implementation.
 * NOTE: This source file includes itself twice.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriCopy.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriCopy.c"
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
# include "UriNormalize.h"
# include "UriCopy.h"
#endif



static void URI_FUNC(PreventLeakageAfterCopy)(URI_TYPE(Uri) * uri,
		unsigned int revertMask, UriMemoryManager * memory) {
	URI_FUNC(PreventLeakage)(uri, revertMask, memory);

	if (uri->hostData.ip4 != NULL) {
		memory->free(memory, uri->hostData.ip4);
		uri->hostData.ip4 = NULL;
	} else if (uri->hostData.ip6 != NULL) {
		memory->free(memory, uri->hostData.ip6);
		uri->hostData.ip6 = NULL;
	}

	if (revertMask & URI_NORMALIZE_PORT) {
		if (uri->portText.first != uri->portText.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->portText.first);
		}
		uri->portText.first = NULL;
		uri->portText.afterLast = NULL;
	}
}



int URI_FUNC(CopyUriMm)(URI_TYPE(Uri) * destUri,
		 const URI_TYPE(Uri) * sourceUri, UriMemoryManager * memory) {
	unsigned int revertMask = URI_NORMALIZED;

	if (sourceUri == NULL || destUri == NULL) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory); /* may return */

	URI_FUNC(ResetUri)(destUri);

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->scheme, &sourceUri->scheme, memory) == URI_FALSE) {
		return URI_ERROR_MALLOC;
	}

	revertMask |= URI_NORMALIZE_SCHEME;

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->userInfo, &sourceUri->userInfo, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	revertMask |= URI_NORMALIZE_USER_INFO;

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->hostText, &sourceUri->hostText, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	revertMask |= URI_NORMALIZE_HOST;

	if (sourceUri->hostData.ip4 == NULL) {
		destUri->hostData.ip4 = NULL;
	} else {
		destUri->hostData.ip4 = memory->malloc(memory, sizeof(UriIp4));
		if (destUri->hostData.ip4 == NULL) {
			URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
			return URI_ERROR_MALLOC;
		}
		*(destUri->hostData.ip4) = *(sourceUri->hostData.ip4);
	}

	if (sourceUri->hostData.ip6 == NULL) {
		destUri->hostData.ip6 = NULL;
	} else {
		destUri->hostData.ip6 = memory->malloc(memory, sizeof(UriIp6));
		if (destUri->hostData.ip6 == NULL) {
			URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
			return URI_ERROR_MALLOC;
		}
		*(destUri->hostData.ip6) = *(sourceUri->hostData.ip6);
	}

	if (sourceUri->hostData.ipFuture.first != NULL) {
		destUri->hostData.ipFuture.first = destUri->hostText.first;
		destUri->hostData.ipFuture.afterLast = destUri->hostText.afterLast;
	} else if (URI_FUNC(CopyRangeAsNeeded)(&destUri->hostData.ipFuture, &sourceUri->hostData.ipFuture, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->portText, &sourceUri->portText, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	revertMask |= URI_NORMALIZE_PORT;

	destUri->pathHead = NULL;
	destUri->pathTail = NULL;

	if (sourceUri->pathHead != NULL) {
		URI_TYPE(PathSegment) * sourceWalker = sourceUri->pathHead;
		URI_TYPE(PathSegment) * destPrev = NULL;

		while (sourceWalker != NULL) {
			URI_TYPE(PathSegment) * destWalker = memory->malloc(memory, sizeof(URI_TYPE(PathSegment)));
			if (destWalker == NULL) {
				URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
				return URI_ERROR_MALLOC;
			}

			destWalker->text.first = NULL;
			destWalker->text.afterLast = NULL;
			destWalker->next = NULL;
			destWalker->reserved = NULL;

			if (destUri->pathHead == NULL) {
				destUri->pathHead = destWalker;
				revertMask |= URI_NORMALIZE_PATH;
			}

			if (URI_FUNC(CopyRangeAsNeeded)(&destWalker->text, &sourceWalker->text, memory) == URI_FALSE) {
				URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
				return URI_ERROR_MALLOC;
			}

			if (destPrev != NULL) {
				destPrev->next = destWalker;
			}

			destPrev = destWalker;
			sourceWalker = sourceWalker->next;

			destUri->pathTail = destWalker;
		}
	}

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->query, &sourceUri->query, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	revertMask |= URI_NORMALIZE_QUERY;

	if (URI_FUNC(CopyRangeAsNeeded)(&destUri->fragment, &sourceUri->fragment, memory) == URI_FALSE) {
		URI_FUNC(PreventLeakageAfterCopy)(destUri, revertMask, memory);
		return URI_ERROR_MALLOC;
	}

	destUri->absolutePath = sourceUri->absolutePath;
	destUri->owner = URI_TRUE;
	destUri->reserved = NULL;

	return URI_SUCCESS;
}



int URI_FUNC(CopyUri)(URI_TYPE(Uri) * destUri,
		const URI_TYPE(Uri) * sourceUri) {
	return URI_FUNC(CopyUriMm)(destUri, sourceUri, NULL);
}

#endif
