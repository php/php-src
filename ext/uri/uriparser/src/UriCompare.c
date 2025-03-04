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

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriCompare.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriCompare.c"
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
# include <uriparser/UriIp4.h>
# include "UriCommon.h"
#endif



UriBool URI_FUNC(EqualsUri)(const URI_TYPE(Uri) * a,
		const URI_TYPE(Uri) * b) {
	/* NOTE: Both NULL means equal! */
	if ((a == NULL) || (b == NULL)) {
		return ((a == NULL) && (b == NULL)) ? URI_TRUE : URI_FALSE;
	}

	/* scheme */
	if (URI_FUNC(CompareRange)(&(a->scheme), &(b->scheme))) {
		return URI_FALSE;
	}

	/* absolutePath */
	if ((a->scheme.first == NULL)&& (a->absolutePath != b->absolutePath)) {
		return URI_FALSE;
	}

	/* userInfo */
	if (URI_FUNC(CompareRange)(&(a->userInfo), &(b->userInfo))) {
		return URI_FALSE;
	}

	/* Host */
	if (((a->hostData.ip4 == NULL) != (b->hostData.ip4 == NULL))
			|| ((a->hostData.ip6 == NULL) != (b->hostData.ip6 == NULL))
			|| ((a->hostData.ipFuture.first == NULL)
				!= (b->hostData.ipFuture.first == NULL))) {
		return URI_FALSE;
	}

	if (a->hostData.ip4 != NULL) {
		if (memcmp(a->hostData.ip4->data, b->hostData.ip4->data, 4)) {
			return URI_FALSE;
		}
	}

	if (a->hostData.ip6 != NULL) {
		if (memcmp(a->hostData.ip6->data, b->hostData.ip6->data, 16)) {
			return URI_FALSE;
		}
	}

	if (a->hostData.ipFuture.first != NULL) {
		if (URI_FUNC(CompareRange)(&(a->hostData.ipFuture), &(b->hostData.ipFuture))) {
			return URI_FALSE;
		}
	}

	if ((a->hostData.ip4 == NULL)
			&& (a->hostData.ip6 == NULL)
			&& (a->hostData.ipFuture.first == NULL)) {
		if (URI_FUNC(CompareRange)(&(a->hostText), &(b->hostText))) {
			return URI_FALSE;
		}
	}

	/* portText */
	if (URI_FUNC(CompareRange)(&(a->portText), &(b->portText))) {
		return URI_FALSE;
	}

	/* Path */
	if ((a->pathHead == NULL) != (b->pathHead == NULL)) {
		return URI_FALSE;
	}

	if (a->pathHead != NULL) {
		URI_TYPE(PathSegment) * walkA = a->pathHead;
		URI_TYPE(PathSegment) * walkB = b->pathHead;
		do {
			if (URI_FUNC(CompareRange)(&(walkA->text), &(walkB->text))) {
				return URI_FALSE;
			}
			if ((walkA->next == NULL) != (walkB->next == NULL)) {
				return URI_FALSE;
			}
			walkA = walkA->next;
			walkB = walkB->next;
		} while (walkA != NULL);
	}

	/* query */
	if (URI_FUNC(CompareRange)(&(a->query), &(b->query))) {
		return URI_FALSE;
	}

	/* fragment */
	if (URI_FUNC(CompareRange)(&(a->fragment), &(b->fragment))) {
		return URI_FALSE;
	}

	return URI_TRUE; /* Equal*/
}



#endif
