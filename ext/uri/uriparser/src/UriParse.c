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
 * @file UriParse.c
 * Holds the RFC 3986 %URI parsing implementation.
 * NOTE: This source file includes itself twice.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriParse.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriParse.c"
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
# include "UriMemory.h"
# include "UriParseBase.h"
#endif



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



static const URI_CHAR * URI_FUNC(ParseAuthority)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseAuthorityTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseHexZero)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseHierPart)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseIpFutLoop)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseIpFutStopGo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseIpLit2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseIPv6address2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseMustBeSegmentNzNc)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnHost)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfoNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseOwnUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePartHelperTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePathAbsEmpty)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePathAbsNoLeadSlash)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePathRootless)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePchar)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePctEncoded)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePctSubUnres)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParsePort)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseQueryFrag)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseSegment)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseSegmentNz)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseUriReference)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseUriTail)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseUriTailTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);
static const URI_CHAR * URI_FUNC(ParseZeroMoreSlashSegs)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast, UriMemoryManager * memory);

static UriBool URI_FUNC(OnExitOwnHost2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, UriMemoryManager * memory);
static UriBool URI_FUNC(OnExitOwnHostUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, UriMemoryManager * memory);
static UriBool URI_FUNC(OnExitOwnPortUserInfo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, UriMemoryManager * memory);
static UriBool URI_FUNC(OnExitSegmentNzNcOrScheme2)(URI_TYPE(ParserState) * state, const URI_CHAR * first, UriMemoryManager * memory);
static void URI_FUNC(OnExitPartHelperTwo)(URI_TYPE(ParserState) * state);

static void URI_FUNC(ResetParserStateExceptUri)(URI_TYPE(ParserState) * state);

static UriBool URI_FUNC(PushPathSegment)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory);

static void URI_FUNC(StopSyntax)(URI_TYPE(ParserState) * state, const URI_CHAR * errorPos, UriMemoryManager * memory);
static void URI_FUNC(StopMalloc)(URI_TYPE(ParserState) * state, UriMemoryManager * memory);

static int URI_FUNC(ParseUriExMm)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory);



static URI_INLINE void URI_FUNC(StopSyntax)(URI_TYPE(ParserState) * state,
		const URI_CHAR * errorPos, UriMemoryManager * memory) {
	URI_FUNC(FreeUriMembersMm)(state->uri, memory);
	state->errorPos = errorPos;
	state->errorCode = URI_ERROR_SYNTAX;
}



static URI_INLINE void URI_FUNC(StopMalloc)(URI_TYPE(ParserState) * state, UriMemoryManager * memory) {
	URI_FUNC(FreeUriMembersMm)(state->uri, memory);
	state->errorPos = NULL;
	state->errorCode = URI_ERROR_MALLOC;
}



/*
 * [authority]-><[>[ipLit2][authorityTwo]
 * [authority]->[ownHostUserInfoNz]
 * [authority]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthority)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		/* "" regname host */
		state->uri->hostText.first = URI_FUNC(SafeToPointTo);
		state->uri->hostText.afterLast = URI_FUNC(SafeToPointTo);
		return afterLast;
	}

	switch (*first) {
	case _UT('['):
		{
			const URI_CHAR * const afterIpLit2
					= URI_FUNC(ParseIpLit2)(state, first + 1, afterLast, memory);
			if (afterIpLit2 == NULL) {
				return NULL;
			}
			state->uri->hostText.first = first + 1; /* HOST BEGIN */
			return URI_FUNC(ParseAuthorityTwo)(state, afterIpLit2, afterLast);
		}

	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		state->uri->userInfo.first = first; /* USERINFO BEGIN */
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast, memory);

	default:
		/* "" regname host */
		state->uri->hostText.first = URI_FUNC(SafeToPointTo);
		state->uri->hostText.afterLast = URI_FUNC(SafeToPointTo);
		return first;
	}
}



/*
 * [authorityTwo]-><:>[port]
 * [authorityTwo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseAuthorityTwo)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT(':'):
		{
			const URI_CHAR * const afterPort = URI_FUNC(ParsePort)(state, first + 1, afterLast);
			if (afterPort == NULL) {
				return NULL;
			}
			state->uri->portText.first = first + 1; /* PORT BEGIN */
			state->uri->portText.afterLast = afterPort; /* PORT END */
			return afterPort;
		}

	default:
		return first;
	}
}



/*
 * [hexZero]->[HEXDIG][hexZero]
 * [hexZero]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseHexZero)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_HEXDIG:
		return URI_FUNC(ParseHexZero)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [hierPart]->[pathRootless]
 * [hierPart]-></>[partHelperTwo]
 * [hierPart]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseHierPart)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParsePathRootless)(state, first, afterLast, memory);

	case _UT('/'):
		return URI_FUNC(ParsePartHelperTwo)(state, first + 1, afterLast, memory);

	default:
		return first;
	}
}



/*
 * [ipFutLoop]->[subDelims][ipFutStopGo]
 * [ipFutLoop]->[unreserved][ipFutStopGo]
 * [ipFutLoop]-><:>[ipFutStopGo]
 */
static const URI_CHAR * URI_FUNC(ParseIpFutLoop)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParseIpFutStopGo)(state, first + 1, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



/*
 * [ipFutStopGo]->[ipFutLoop]
 * [ipFutStopGo]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseIpFutStopGo)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParseIpFutLoop)(state, first, afterLast, memory);

	default:
		return first;
	}
}



/*
 * [ipFuture]-><v>[HEXDIG][hexZero]<.>[ipFutLoop]
 */
static const URI_CHAR * URI_FUNC(ParseIpFuture)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	/*
	First character has already been
	checked before entering this rule.

	switch (*first) {
	case _UT('v'):
	case _UT('V'):
	*/
		if (first + 1 >= afterLast) {
			URI_FUNC(StopSyntax)(state, afterLast, memory);
			return NULL;
		}

		switch (first[1]) {
		case URI_SET_HEXDIG:
			{
				const URI_CHAR * afterIpFutLoop;
				const URI_CHAR * const afterHexZero
						= URI_FUNC(ParseHexZero)(state, first + 2, afterLast);
				if (afterHexZero == NULL) {
					return NULL;
				}
				if (afterHexZero >= afterLast) {
					URI_FUNC(StopSyntax)(state, afterLast, memory);
					return NULL;
				}
				if (*afterHexZero != _UT('.')) {
					URI_FUNC(StopSyntax)(state, afterHexZero, memory);
					return NULL;
				}
				state->uri->hostText.first = first; /* HOST BEGIN */
				state->uri->hostData.ipFuture.first = first; /* IPFUTURE BEGIN */
				afterIpFutLoop = URI_FUNC(ParseIpFutLoop)(state, afterHexZero + 1, afterLast, memory);
				if (afterIpFutLoop == NULL) {
					return NULL;
				}
				state->uri->hostText.afterLast = afterIpFutLoop; /* HOST END */
				state->uri->hostData.ipFuture.afterLast = afterIpFutLoop; /* IPFUTURE END */
				return afterIpFutLoop;
			}

		default:
			URI_FUNC(StopSyntax)(state, first + 1, memory);
			return NULL;
		}

	/*
	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
	*/
}



/*
 * [ipLit2]->[ipFuture]<]>
 * [ipLit2]->[IPv6address2]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseIpLit2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	/* The leading "v" of IPvFuture is case-insensitive. */
	case _UT('v'):
	case _UT('V'):
		{
			const URI_CHAR * const afterIpFuture
					= URI_FUNC(ParseIpFuture)(state, first, afterLast, memory);
			if (afterIpFuture == NULL) {
				return NULL;
			}
			if (afterIpFuture >= afterLast) {
				URI_FUNC(StopSyntax)(state, afterLast, memory);
				return NULL;
			}
			if (*afterIpFuture != _UT(']')) {
				URI_FUNC(StopSyntax)(state, afterIpFuture, memory);
				return NULL;
			}
			return afterIpFuture + 1;
		}

	case _UT(':'):
	case _UT(']'):
	case URI_SET_HEXDIG:
		state->uri->hostData.ip6 = memory->malloc(memory, 1 * sizeof(UriIp6)); /* Freed when stopping on parse error */
		if (state->uri->hostData.ip6 == NULL) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return URI_FUNC(ParseIPv6address2)(state, first, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



/*
 * [IPv6address2]->..<]>
 */
static const URI_CHAR * URI_FUNC(ParseIPv6address2)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	int zipperEver = 0;
	int quadsDone = 0;
	int digitCount = 0;
	unsigned char digitHistory[4];
	int ip4OctetsDone = 0;

	unsigned char quadsAfterZipper[14];
	int quadsAfterZipperCount = 0;


	for (;;) {
		if (first >= afterLast) {
			URI_FUNC(StopSyntax)(state, afterLast, memory);
			return NULL;
		}

		/* Inside IPv4 part? */
		if (ip4OctetsDone > 0) {
			/* Eat rest of IPv4 address */
			for (;;) {
				switch (*first) {
				case URI_SET_DIGIT:
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount++] = (unsigned char)(9 + *first - _UT('9'));
					break;

				case _UT('.'):
					if ((ip4OctetsDone == 4) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					/* Copy IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + ip4OctetsDone] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;
					ip4OctetsDone++;
					break;

				case _UT(']'):
					if ((ip4OctetsDone != 3) /* NOTE! */
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid digit or octet count */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					state->uri->hostText.afterLast = first; /* HOST END */

					/* Copy missing quads right before IPv4 */
					memcpy(state->uri->hostData.ip6->data + 16 - 4 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					/* Copy last IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4 + 3] = uriGetOctetValue(digitHistory, digitCount);

					return first + 1;

				default:
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL;
				}
				first++;

				if (first >= afterLast) {
					URI_FUNC(StopSyntax)(state, afterLast, memory);
					return NULL;
				}
			}
		} else {
			/* Eat while no dot in sight */
			int letterAmong = 0;
			int walking = 1;
			do {
				switch (*first) {
				case URI_SET_HEX_LETTER_LOWER:
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('f'));
					digitCount++;
					break;

				case URI_SET_HEX_LETTER_UPPER:
					letterAmong = 1;
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(15 + *first - _UT('F'));
					digitCount++;
					break;

				case URI_SET_DIGIT:
					if (digitCount == 4) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}
					digitHistory[digitCount] = (unsigned char)(9 + *first - _UT('9'));
					digitCount++;
					break;

				case _UT(':'):
					{
						int setZipper = 0;

						if (digitCount > 0) {
							if (zipperEver) {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
								quadsAfterZipperCount++;
							} else {
								uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
							}
							quadsDone++;
							digitCount = 0;
						}
						letterAmong = 0;

						/* Too many quads? */
						if (quadsDone >= 8 - zipperEver) {
							URI_FUNC(StopSyntax)(state, first, memory);
							return NULL;
						}

						/* "::"? */
						if (first + 1 >= afterLast) {
							URI_FUNC(StopSyntax)(state, afterLast, memory);
							return NULL;
						}
						if (first[1] == _UT(':')) {
							const int resetOffset = 2 * (quadsDone + (digitCount > 0));

							first++;
							if (zipperEver) {
								URI_FUNC(StopSyntax)(state, first, memory);
								return NULL; /* "::.+::" */
							}

							/* Zero everything after zipper */
							memset(state->uri->hostData.ip6->data + resetOffset, 0, 16 - resetOffset);
							setZipper = 1;

							/* ":::+"? */
							if (first + 1 >= afterLast) {
								URI_FUNC(StopSyntax)(state, afterLast, memory);
								return NULL; /* No ']' yet */
							}
							if (first[1] == _UT(':')) {
								URI_FUNC(StopSyntax)(state, first + 1, memory);
								return NULL; /* ":::+ "*/
							}
						} else if (quadsDone == 0 || first[1] == _UT(']')) {
							/* Single leading or trailing ":" */
							URI_FUNC(StopSyntax)(state, first, memory);
							return NULL;
						}

						if (setZipper) {
							zipperEver = 1;
						}
					}
					break;

				case _UT('.'):
					if ((quadsDone + zipperEver > 6) /* NOTE */
							|| (!zipperEver && (quadsDone < 6))
							|| letterAmong
							|| (digitCount == 0)
							|| (digitCount == 4)) {
						/* Invalid octet before */
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					} else if ((digitCount > 1)
							&& (digitHistory[0] == 0)) {
						/* Leading zero */
						URI_FUNC(StopSyntax)(state, first - digitCount, memory);
						return NULL;
					} else if ((digitCount == 3)
							&& (100 * digitHistory[0]
								+ 10 * digitHistory[1]
								+ digitHistory[2] > 255)) {
						/* Octet value too large */
						if (digitHistory[0] > 2) {
							URI_FUNC(StopSyntax)(state, first - 3, memory);
						} else if (digitHistory[1] > 5) {
							URI_FUNC(StopSyntax)(state, first - 2, memory);
						} else {
							URI_FUNC(StopSyntax)(state, first - 1, memory);
						}
						return NULL;
					}

					/* Copy first IPv4 octet */
					state->uri->hostData.ip6->data[16 - 4] = uriGetOctetValue(digitHistory, digitCount);
					digitCount = 0;

					/* Switch over to IPv4 loop */
					ip4OctetsDone = 1;
					walking = 0;
					break;

				case _UT(']'):
					/* Too little quads? */
					if (!zipperEver && !((quadsDone == 7) && (digitCount > 0))) {
						URI_FUNC(StopSyntax)(state, first, memory);
						return NULL;
					}

					if (digitCount > 0) {
						if (zipperEver) {
							/* Too many quads? */
							if (quadsDone >= 7) {
								URI_FUNC(StopSyntax)(state, first, memory);
								return NULL;
							}
							uriWriteQuadToDoubleByte(digitHistory, digitCount, quadsAfterZipper + 2 * quadsAfterZipperCount);
							quadsAfterZipperCount++;
						} else {
							uriWriteQuadToDoubleByte(digitHistory, digitCount, state->uri->hostData.ip6->data + 2 * quadsDone);
						}
						/*
						quadsDone++;
						digitCount = 0;
						*/
					}

					/* Copy missing quads to the end */
					memcpy(state->uri->hostData.ip6->data + 16 - 2 * quadsAfterZipperCount,
								quadsAfterZipper, 2 * quadsAfterZipperCount);

					state->uri->hostText.afterLast = first; /* HOST END */
					return first + 1; /* Fine */

				default:
					URI_FUNC(StopSyntax)(state, first, memory);
					return NULL;
				}
				first++;

				if (first >= afterLast) {
					URI_FUNC(StopSyntax)(state, afterLast, memory);
					return NULL; /* No ']' yet */
				}
			} while (walking);
		}
	}
}



/*
 * [mustBeSegmentNzNc]->[pctEncoded][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[subDelims][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[unreserved][mustBeSegmentNzNc]
 * [mustBeSegmentNzNc]->[uriTail] // can take <NULL>
 * [mustBeSegmentNzNc]-></>[segment][zeroMoreSlashSegs][uriTail]
 * [mustBeSegmentNzNc]-><@>[mustBeSegmentNzNc]
 */
static const URI_CHAR * URI_FUNC(ParseMustBeSegmentNzNc)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		state->uri->scheme.first = NULL; /* Not a scheme, reset */
		return afterLast;
	}

	switch (*first) {
	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast, memory);
		}

	case _UT('@'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast, memory);

	case _UT('/'):
		{
			const URI_CHAR * afterZeroMoreSlashSegs;
			const URI_CHAR * afterSegment;
			if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			state->uri->scheme.first = NULL; /* Not a scheme, reset */
			afterSegment = URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			afterZeroMoreSlashSegs
					= URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast, memory);
			if (afterZeroMoreSlashSegs == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterZeroMoreSlashSegs, afterLast, memory);
		}

	default:
		if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		state->uri->scheme.first = NULL; /* Not a scheme, reset */
		return URI_FUNC(ParseUriTail)(state, first, afterLast, memory);
	}
}



/*
 * [ownHost]-><[>[ipLit2][authorityTwo]
 * [ownHost]->[ownHost2] // can take <NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseOwnHost)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		state->uri->hostText.afterLast = afterLast; /* HOST END */
		return afterLast;
	}

	switch (*first) {
	case _UT('['):
		{
			const URI_CHAR * const afterIpLit2
					= URI_FUNC(ParseIpLit2)(state, first + 1, afterLast, memory);
			if (afterIpLit2 == NULL) {
				return NULL;
			}
			state->uri->hostText.first = first + 1; /* HOST BEGIN */
			return URI_FUNC(ParseAuthorityTwo)(state, afterIpLit2, afterLast);
		}

	default:
		return URI_FUNC(ParseOwnHost2)(state, first, afterLast, memory);
	}
}



static URI_INLINE UriBool URI_FUNC(OnExitOwnHost2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	state->uri->hostText.afterLast = first; /* HOST END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = memory->malloc(memory, 1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (state->uri->hostData.ip4 == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		memory->free(memory, state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
	return URI_TRUE; /* Success */
}



/*
 * [ownHost2]->[authorityTwo] // can take <NULL>
 * [ownHost2]->[pctSubUnres][ownHost2]
 */
static const URI_CHAR * URI_FUNC(ParseOwnHost2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnHost2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast, memory);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnHost2)(state, afterPctSubUnres, afterLast, memory);
		}

	default:
		if (!URI_FUNC(OnExitOwnHost2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return URI_FUNC(ParseAuthorityTwo)(state, first, afterLast);
	}
}



static URI_INLINE UriBool URI_FUNC(OnExitOwnHostUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	state->uri->hostText.first = state->uri->userInfo.first; /* Host instead of userInfo, update */
	state->uri->userInfo.first = NULL; /* Not a userInfo, reset */
	state->uri->hostText.afterLast = first; /* HOST END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = memory->malloc(memory, 1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (state->uri->hostData.ip4 == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		memory->free(memory, state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
	return URI_TRUE; /* Success */
}



/*
 * [ownHostUserInfo]->[ownHostUserInfoNz]
 * [ownHostUserInfo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseOwnHostUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnHostUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return URI_FUNC(ParseOwnHostUserInfoNz)(state, first, afterLast, memory);

	default:
		if (!URI_FUNC(OnExitOwnHostUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return first;
	}
}



/*
 * [ownHostUserInfoNz]->[pctSubUnres][ownHostUserInfo]
 * [ownHostUserInfoNz]-><:>[ownPortUserInfo]
 * [ownHostUserInfoNz]-><@>[ownHost]
 */
static const URI_CHAR * URI_FUNC(ParseOwnHostUserInfoNz)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast, memory);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnHostUserInfo)(state, afterPctSubUnres, afterLast, memory);
		}

	case _UT(':'):
		state->uri->hostText.afterLast = first; /* HOST END */
		state->uri->portText.first = first + 1; /* PORT BEGIN */
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast, memory);

	case _UT('@'):
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



static URI_INLINE UriBool URI_FUNC(OnExitOwnPortUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	state->uri->hostText.first = state->uri->userInfo.first; /* Host instead of userInfo, update */
	state->uri->userInfo.first = NULL; /* Not a userInfo, reset */
	state->uri->portText.afterLast = first; /* PORT END */

	/* Valid IPv4 or just a regname? */
	state->uri->hostData.ip4 = memory->malloc(memory, 1 * sizeof(UriIp4)); /* Freed when stopping on parse error */
	if (state->uri->hostData.ip4 == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (URI_FUNC(ParseIpFourAddress)(state->uri->hostData.ip4->data,
			state->uri->hostText.first, state->uri->hostText.afterLast)) {
		/* Not IPv4 */
		memory->free(memory, state->uri->hostData.ip4);
		state->uri->hostData.ip4 = NULL;
	}
	return URI_TRUE; /* Success */
}



/*
 * [ownPortUserInfo]->[ALPHA][ownUserInfo]
 * [ownPortUserInfo]->[DIGIT][ownPortUserInfo]
 * [ownPortUserInfo]-><.>[ownUserInfo]
 * [ownPortUserInfo]-><_>[ownUserInfo]
 * [ownPortUserInfo]-><~>[ownUserInfo]
 * [ownPortUserInfo]-><->[ownUserInfo]
 * [ownPortUserInfo]->[subDelims][ownUserInfo]
 * [ownPortUserInfo]->[pctEncoded][ownUserInfo]
 * [ownPortUserInfo]-><:>[ownUserInfo]
 * [ownPortUserInfo]-><@>[ownHost]
 * [ownPortUserInfo]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseOwnPortUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitOwnPortUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return afterLast;
	}

	switch (*first) {
	/* begin sub-delims */
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('\''):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT('+'):
	case _UT(','):
	case _UT(';'):
	case _UT('='):
	/* end sub-delims */
	/* begin unreserved (except alpha and digit) */
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	/* end unreserved (except alpha and digit) */
	case _UT(':'):
	case URI_SET_ALPHA:
		state->uri->hostText.afterLast = NULL; /* Not a host, reset */
		state->uri->portText.first = NULL; /* Not a port, reset */
		return URI_FUNC(ParseOwnUserInfo)(state, first + 1, afterLast, memory);

	case URI_SET_DIGIT:
		return URI_FUNC(ParseOwnPortUserInfo)(state, first + 1, afterLast, memory);

	case _UT('%'):
		state->uri->portText.first = NULL; /* Not a port, reset */
		{
			const URI_CHAR * const afterPct
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPct == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnUserInfo)(state, afterPct, afterLast, memory);
		}

	case _UT('@'):
		state->uri->hostText.afterLast = NULL; /* Not a host, reset */
		state->uri->portText.first = NULL; /* Not a port, reset */
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast, memory);

	default:
		if (!URI_FUNC(OnExitOwnPortUserInfo)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return first;
	}
}



/*
 * [ownUserInfo]->[pctSubUnres][ownUserInfo]
 * [ownUserInfo]-><:>[ownUserInfo]
 * [ownUserInfo]-><@>[ownHost]
 */
static const URI_CHAR * URI_FUNC(ParseOwnUserInfo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(';'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPctSubUnres
					= URI_FUNC(ParsePctSubUnres)(state, first, afterLast, memory);
			if (afterPctSubUnres == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseOwnUserInfo)(state, afterPctSubUnres, afterLast, memory);
		}

	case _UT(':'):
		return URI_FUNC(ParseOwnUserInfo)(state, first + 1, afterLast, memory);

	case _UT('@'):
		/* SURE */
		state->uri->userInfo.afterLast = first; /* USERINFO END */
		state->uri->hostText.first = first + 1; /* HOST BEGIN */
		return URI_FUNC(ParseOwnHost)(state, first + 1, afterLast, memory);

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



static URI_INLINE void URI_FUNC(OnExitPartHelperTwo)(URI_TYPE(ParserState) * state) {
	state->uri->absolutePath = URI_TRUE;
}



/*
 * [partHelperTwo]->[pathAbsNoLeadSlash] // can take <NULL>
 * [partHelperTwo]-></>[authority][pathAbsEmpty]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePartHelperTwo)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(OnExitPartHelperTwo)(state);
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterAuthority
					= URI_FUNC(ParseAuthority)(state, first + 1, afterLast, memory);
			const URI_CHAR * afterPathAbsEmpty;
			if (afterAuthority == NULL) {
				return NULL;
			}
			afterPathAbsEmpty = URI_FUNC(ParsePathAbsEmpty)(state, afterAuthority, afterLast, memory);

			URI_FUNC(FixEmptyTrailSegment)(state->uri, memory);

			return afterPathAbsEmpty;
		}

	default:
		URI_FUNC(OnExitPartHelperTwo)(state);
		return URI_FUNC(ParsePathAbsNoLeadSlash)(state, first, afterLast, memory);
	}
}



/*
 * [pathAbsEmpty]-></>[segment][pathAbsEmpty]
 * [pathAbsEmpty]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParsePathAbsEmpty)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			return URI_FUNC(ParsePathAbsEmpty)(state, afterSegment, afterLast, memory);
		}

	default:
		return first;
	}
}



/*
 * [pathAbsNoLeadSlash]->[segmentNz][zeroMoreSlashSegs]
 * [pathAbsNoLeadSlash]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathAbsNoLeadSlash)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterSegmentNz
					= URI_FUNC(ParseSegmentNz)(state, first, afterLast, memory);
			if (afterSegmentNz == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first, afterSegmentNz, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast, memory);
		}

	default:
		return first;
	}
}



/*
 * [pathRootless]->[segmentNz][zeroMoreSlashSegs]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParsePathRootless)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	const URI_CHAR * const afterSegmentNz
			= URI_FUNC(ParseSegmentNz)(state, first, afterLast, memory);
	if (afterSegmentNz == NULL) {
		return NULL;
	} else {
		if (!URI_FUNC(PushPathSegment)(state, first, afterSegmentNz, memory)) { /* SEGMENT BOTH */
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
	}
	return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegmentNz, afterLast, memory);
}



/*
 * [pchar]->[pctEncoded]
 * [pchar]->[subDelims]
 * [pchar]->[unreserved]
 * [pchar]-><:>
 * [pchar]-><@>
 */
static const URI_CHAR * URI_FUNC(ParsePchar)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('%'):
		return URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);

	case _UT(':'):
	case _UT('@'):
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return first + 1;

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



/*
 * [pctEncoded]-><%>[HEXDIG][HEXDIG]
 */
static const URI_CHAR * URI_FUNC(ParsePctEncoded)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	/*
	First character has already been
	checked before entering this rule.

	switch (*first) {
	case _UT('%'):
	*/
		if (first + 1 >= afterLast) {
			URI_FUNC(StopSyntax)(state, afterLast, memory);
			return NULL;
		}

		switch (first[1]) {
		case URI_SET_HEXDIG:
			if (first + 2 >= afterLast) {
				URI_FUNC(StopSyntax)(state, afterLast, memory);
				return NULL;
			}

			switch (first[2]) {
			case URI_SET_HEXDIG:
				return first + 3;

			default:
				URI_FUNC(StopSyntax)(state, first + 2, memory);
				return NULL;
			}

		default:
			URI_FUNC(StopSyntax)(state, first + 1, memory);
			return NULL;
		}

	/*
	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
	*/
}



/*
 * [pctSubUnres]->[pctEncoded]
 * [pctSubUnres]->[subDelims]
 * [pctSubUnres]->[unreserved]
 */
static const URI_CHAR * URI_FUNC(ParsePctSubUnres)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		URI_FUNC(StopSyntax)(state, afterLast, memory);
		return NULL;
	}

	switch (*first) {
	case _UT('%'):
		return URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);

	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('-'):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		return first + 1;

	default:
		URI_FUNC(StopSyntax)(state, first, memory);
		return NULL;
	}
}



/*
 * [port]->[DIGIT][port]
 * [port]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParsePort)(URI_TYPE(ParserState) * state, const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_DIGIT:
		return URI_FUNC(ParsePort)(state, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [queryFrag]->[pchar][queryFrag]
 * [queryFrag]-></>[queryFrag]
 * [queryFrag]-><?>[queryFrag]
 * [queryFrag]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseQueryFrag)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPchar
					= URI_FUNC(ParsePchar)(state, first, afterLast, memory);
			if (afterPchar == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseQueryFrag)(state, afterPchar, afterLast, memory);
		}

	case _UT('/'):
	case _UT('?'):
		return URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);

	default:
		return first;
	}
}



/*
 * [segment]->[pchar][segment]
 * [segment]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseSegment)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('!'):
	case _UT('$'):
	case _UT('%'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('-'):
	case _UT('*'):
	case _UT(','):
	case _UT('.'):
	case _UT(':'):
	case _UT(';'):
	case _UT('@'):
	case _UT('\''):
	case _UT('_'):
	case _UT('~'):
	case _UT('+'):
	case _UT('='):
	case URI_SET_DIGIT:
	case URI_SET_ALPHA:
		{
			const URI_CHAR * const afterPchar
					= URI_FUNC(ParsePchar)(state, first, afterLast, memory);
			if (afterPchar == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseSegment)(state, afterPchar, afterLast, memory);
		}

	default:
		return first;
	}
}



/*
 * [segmentNz]->[pchar][segment]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseSegmentNz)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	const URI_CHAR * const afterPchar
			= URI_FUNC(ParsePchar)(state, first, afterLast, memory);
	if (afterPchar == NULL) {
		return NULL;
	}
	return URI_FUNC(ParseSegment)(state, afterPchar, afterLast, memory);
}



static URI_INLINE UriBool URI_FUNC(OnExitSegmentNzNcOrScheme2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		UriMemoryManager * memory) {
	if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
		return URI_FALSE; /* Raises malloc error*/
	}
	state->uri->scheme.first = NULL; /* Not a scheme, reset */
	return URI_TRUE; /* Success */
}



/*
 * [segmentNzNcOrScheme2]->[ALPHA][segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]->[DIGIT][segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]->[pctEncoded][mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]->[uriTail] // can take <NULL>
 * [segmentNzNcOrScheme2]-><!>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><$>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><&>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><(>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><)>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><*>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><,>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><.>[segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]-></>[segment][zeroMoreSlashSegs][uriTail]
 * [segmentNzNcOrScheme2]-><:>[hierPart][uriTail]
 * [segmentNzNcOrScheme2]-><;>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><@>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><_>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><~>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><+>[segmentNzNcOrScheme2]
 * [segmentNzNcOrScheme2]-><=>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><'>[mustBeSegmentNzNc]
 * [segmentNzNcOrScheme2]-><->[segmentNzNcOrScheme2]
 */
static const URI_CHAR * URI_FUNC(ParseSegmentNzNcOrScheme2)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		if (!URI_FUNC(OnExitSegmentNzNcOrScheme2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return afterLast;
	}

	switch (*first) {
	case _UT('.'):
	case _UT('+'):
	case _UT('-'):
	case URI_SET_ALPHA:
	case URI_SET_DIGIT:
		return URI_FUNC(ParseSegmentNzNcOrScheme2)(state, first + 1, afterLast, memory);

	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast, memory);
		}

	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('@'):
	case _UT('_'):
	case _UT('~'):
	case _UT('='):
	case _UT('\''):
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast, memory);

	case _UT('/'):
		{
			const URI_CHAR * afterZeroMoreSlashSegs;
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, state->uri->scheme.first, first, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			state->uri->scheme.first = NULL; /* Not a scheme, reset */
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			afterZeroMoreSlashSegs
					= URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast, memory);
			if (afterZeroMoreSlashSegs == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterZeroMoreSlashSegs, afterLast, memory);
		}

	case _UT(':'):
		{
			const URI_CHAR * const afterHierPart
					= URI_FUNC(ParseHierPart)(state, first + 1, afterLast, memory);
			state->uri->scheme.afterLast = first; /* SCHEME END */
			if (afterHierPart == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterHierPart, afterLast, memory);
		}

	default:
		if (!URI_FUNC(OnExitSegmentNzNcOrScheme2)(state, first, memory)) {
			URI_FUNC(StopMalloc)(state, memory);
			return NULL;
		}
		return URI_FUNC(ParseUriTail)(state, first, afterLast, memory);
	}
}



/*
 * [uriReference]->[ALPHA][segmentNzNcOrScheme2]
 * [uriReference]->[DIGIT][mustBeSegmentNzNc]
 * [uriReference]->[pctEncoded][mustBeSegmentNzNc]
 * [uriReference]->[subDelims][mustBeSegmentNzNc]
 * [uriReference]->[uriTail] // can take <NULL>
 * [uriReference]-><.>[mustBeSegmentNzNc]
 * [uriReference]-></>[partHelperTwo][uriTail]
 * [uriReference]-><@>[mustBeSegmentNzNc]
 * [uriReference]-><_>[mustBeSegmentNzNc]
 * [uriReference]-><~>[mustBeSegmentNzNc]
 * [uriReference]-><->[mustBeSegmentNzNc]
 */
static const URI_CHAR * URI_FUNC(ParseUriReference)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case URI_SET_ALPHA:
		state->uri->scheme.first = first; /* SCHEME BEGIN */
		return URI_FUNC(ParseSegmentNzNcOrScheme2)(state, first + 1, afterLast, memory);

	case URI_SET_DIGIT:
	case _UT('!'):
	case _UT('$'):
	case _UT('&'):
	case _UT('('):
	case _UT(')'):
	case _UT('*'):
	case _UT(','):
	case _UT(';'):
	case _UT('\''):
	case _UT('+'):
	case _UT('='):
	case _UT('.'):
	case _UT('_'):
	case _UT('~'):
	case _UT('-'):
	case _UT('@'):
		state->uri->scheme.first = first; /* SEGMENT BEGIN, ABUSE SCHEME POINTER */
		return URI_FUNC(ParseMustBeSegmentNzNc)(state, first + 1, afterLast, memory);

	case _UT('%'):
		{
			const URI_CHAR * const afterPctEncoded
					= URI_FUNC(ParsePctEncoded)(state, first, afterLast, memory);
			if (afterPctEncoded == NULL) {
				return NULL;
			}
			state->uri->scheme.first = first; /* SEGMENT BEGIN, ABUSE SCHEME POINTER */
			return URI_FUNC(ParseMustBeSegmentNzNc)(state, afterPctEncoded, afterLast, memory);
		}

	case _UT('/'):
		{
			const URI_CHAR * const afterPartHelperTwo
					= URI_FUNC(ParsePartHelperTwo)(state, first + 1, afterLast, memory);
			if (afterPartHelperTwo == NULL) {
				return NULL;
			}
			return URI_FUNC(ParseUriTail)(state, afterPartHelperTwo, afterLast, memory);
		}

	default:
		return URI_FUNC(ParseUriTail)(state, first, afterLast, memory);
	}
}



/*
 * [uriTail]-><#>[queryFrag]
 * [uriTail]-><?>[queryFrag][uriTailTwo]
 * [uriTail]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseUriTail)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('#'):
		{
			const URI_CHAR * const afterQueryFrag = URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->fragment.first = first + 1; /* FRAGMENT BEGIN */
			state->uri->fragment.afterLast = afterQueryFrag; /* FRAGMENT END */
			return afterQueryFrag;
		}

	case _UT('?'):
		{
			const URI_CHAR * const afterQueryFrag
					= URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->query.first = first + 1; /* QUERY BEGIN */
			state->uri->query.afterLast = afterQueryFrag; /* QUERY END */
			return URI_FUNC(ParseUriTailTwo)(state, afterQueryFrag, afterLast, memory);
		}

	default:
		return first;
	}
}



/*
 * [uriTailTwo]-><#>[queryFrag]
 * [uriTailTwo]-><NULL>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseUriTailTwo)(
		URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('#'):
		{
			const URI_CHAR * const afterQueryFrag = URI_FUNC(ParseQueryFrag)(state, first + 1, afterLast, memory);
			if (afterQueryFrag == NULL) {
				return NULL;
			}
			state->uri->fragment.first = first + 1; /* FRAGMENT BEGIN */
			state->uri->fragment.afterLast = afterQueryFrag; /* FRAGMENT END */
			return afterQueryFrag;
		}

	default:
		return first;
	}
}



/*
 * [zeroMoreSlashSegs]-></>[segment][zeroMoreSlashSegs]
 * [zeroMoreSlashSegs]-><NULL>
 */
static const URI_CHAR * URI_FUNC(ParseZeroMoreSlashSegs)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('/'):
		{
			const URI_CHAR * const afterSegment
					= URI_FUNC(ParseSegment)(state, first + 1, afterLast, memory);
			if (afterSegment == NULL) {
				return NULL;
			}
			if (!URI_FUNC(PushPathSegment)(state, first + 1, afterSegment, memory)) { /* SEGMENT BOTH */
				URI_FUNC(StopMalloc)(state, memory);
				return NULL;
			}
			return URI_FUNC(ParseZeroMoreSlashSegs)(state, afterSegment, afterLast, memory);
		}

	default:
		return first;
	}
}



static URI_INLINE void URI_FUNC(ResetParserStateExceptUri)(URI_TYPE(ParserState) * state) {
	URI_TYPE(Uri) * const uriBackup = state->uri;
	memset(state, 0, sizeof(URI_TYPE(ParserState)));
	state->uri = uriBackup;
}



static URI_INLINE UriBool URI_FUNC(PushPathSegment)(
		URI_TYPE(ParserState) * state, const URI_CHAR * first,
		const URI_CHAR * afterLast, UriMemoryManager * memory) {
	URI_TYPE(PathSegment) * segment = memory->calloc(memory, 1, sizeof(URI_TYPE(PathSegment)));
	if (segment == NULL) {
		return URI_FALSE; /* Raises malloc error */
	}
	if (first == afterLast) {
		segment->text.first = URI_FUNC(SafeToPointTo);
		segment->text.afterLast = URI_FUNC(SafeToPointTo);
	} else {
		segment->text.first = first;
		segment->text.afterLast = afterLast;
	}

	/* First segment ever? */
	if (state->uri->pathHead == NULL) {
		/* First segment ever, set head and tail */
		state->uri->pathHead = segment;
		state->uri->pathTail = segment;
	} else {
		/* Append, update tail */
		state->uri->pathTail->next = segment;
		state->uri->pathTail = segment;
	}

	return URI_TRUE; /* Success */
}



int URI_FUNC(ParseUriEx)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	return URI_FUNC(ParseUriExMm)(state, first, afterLast, NULL);
}



static int URI_FUNC(ParseUriExMm)(URI_TYPE(ParserState) * state,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	const URI_CHAR * afterUriReference;
	URI_TYPE(Uri) * uri;

	/* Check params */
	if ((state == NULL) || (first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	uri = state->uri;

	/* Init parser */
	URI_FUNC(ResetParserStateExceptUri)(state);
	URI_FUNC(ResetUri)(uri);

	/* Parse */
	afterUriReference = URI_FUNC(ParseUriReference)(state, first, afterLast, memory);
	if (afterUriReference == NULL) {
		/* Waterproof errorPos <= afterLast */
		if (state->errorPos && (state->errorPos > afterLast)) {
			state->errorPos = afterLast;
		}
		return state->errorCode;
	}
	if (afterUriReference != afterLast) {
		if (afterUriReference < afterLast) {
			URI_FUNC(StopSyntax)(state, afterUriReference, memory);
		} else {
			URI_FUNC(StopSyntax)(state, afterLast, memory);
		}
		return state->errorCode;
	}
	return URI_SUCCESS;
}



int URI_FUNC(ParseUri)(URI_TYPE(ParserState) * state, const URI_CHAR * text) {
	if ((state == NULL) || (text == NULL)) {
		return URI_ERROR_NULL;
	}
	return URI_FUNC(ParseUriEx)(state, text, text + URI_STRLEN(text));
}



int URI_FUNC(ParseSingleUri)(URI_TYPE(Uri) * uri, const URI_CHAR * text,
		const URI_CHAR ** errorPos) {
	return URI_FUNC(ParseSingleUriEx)(uri, text, NULL, errorPos);
}



int URI_FUNC(ParseSingleUriEx)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		const URI_CHAR ** errorPos) {
    if ((afterLast == NULL) && (first != NULL)) {
		afterLast = first + URI_STRLEN(first);
	}
	return URI_FUNC(ParseSingleUriExMm)(uri, first, afterLast, errorPos, NULL);
}



int URI_FUNC(ParseSingleUriExMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first, const URI_CHAR * afterLast,
		const URI_CHAR ** errorPos, UriMemoryManager * memory) {
	URI_TYPE(ParserState) state;
	int res;

	/* Check params */
	if ((uri == NULL) || (first == NULL) || (afterLast == NULL)) {
		return URI_ERROR_NULL;
	}
	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	state.uri = uri;

	res = URI_FUNC(ParseUriExMm)(&state, first, afterLast, memory);

	if (res != URI_SUCCESS) {
		if (errorPos != NULL) {
			*errorPos = state.errorPos;
		}
		URI_FUNC(FreeUriMembersMm)(uri, memory);
	}

	return res;
}



void URI_FUNC(FreeUriMembers)(URI_TYPE(Uri) * uri) {
	URI_FUNC(FreeUriMembersMm)(uri, NULL);
}



int URI_FUNC(FreeUriMembersMm)(URI_TYPE(Uri) * uri, UriMemoryManager * memory) {
	if (uri == NULL) {
		return URI_ERROR_NULL;
	}

	URI_CHECK_MEMORY_MANAGER(memory);  /* may return */

	if (uri->owner) {
		/* Scheme */
		if (uri->scheme.first != NULL) {
			if (uri->scheme.first != uri->scheme.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->scheme.first);
			}
			uri->scheme.first = NULL;
			uri->scheme.afterLast = NULL;
		}

		/* User info */
		if (uri->userInfo.first != NULL) {
			if (uri->userInfo.first != uri->userInfo.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->userInfo.first);
			}
			uri->userInfo.first = NULL;
			uri->userInfo.afterLast = NULL;
		}

		/* Host data - IPvFuture (may affect host text) */
		if (uri->hostData.ipFuture.first != NULL) {
			/* NOTE: .hostData.ipFuture may hold the very same range pointers
			 *       as .hostText; then we need to prevent freeing memory twice. */
			if (uri->hostText.first == uri->hostData.ipFuture.first) {
				uri->hostText.first = NULL;
				uri->hostText.afterLast = NULL;
			}

			if (uri->hostData.ipFuture.first != uri->hostData.ipFuture.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->hostData.ipFuture.first);
			}
			uri->hostData.ipFuture.first = NULL;
			uri->hostData.ipFuture.afterLast = NULL;
		}

		/* Host text (after IPvFuture, see above) */
		if (uri->hostText.first != NULL) {
			if (uri->hostText.first != uri->hostText.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->hostText.first);
			}
			uri->hostText.first = NULL;
			uri->hostText.afterLast = NULL;
		}
	}

	/* Host data - IPv4 */
	if (uri->hostData.ip4 != NULL) {
		memory->free(memory, uri->hostData.ip4);
		uri->hostData.ip4 = NULL;
	}

	/* Host data - IPv6 */
	if (uri->hostData.ip6 != NULL) {
		memory->free(memory, uri->hostData.ip6);
		uri->hostData.ip6 = NULL;
	}

	/* Port text */
	if (uri->owner && (uri->portText.first != NULL)) {
		if (uri->portText.first != uri->portText.afterLast) {
			memory->free(memory, (URI_CHAR *)uri->portText.first);
		}
		uri->portText.first = NULL;
		uri->portText.afterLast = NULL;
	}

	/* Path */
	if (uri->pathHead != NULL) {
		URI_TYPE(PathSegment) * segWalk = uri->pathHead;
		while (segWalk != NULL) {
			URI_TYPE(PathSegment) * const next = segWalk->next;
			if (uri->owner && (segWalk->text.first != NULL)
					&& (segWalk->text.first < segWalk->text.afterLast)) {
				memory->free(memory, (URI_CHAR *)segWalk->text.first);
			}
			memory->free(memory, segWalk);
			segWalk = next;
		}
		uri->pathHead = NULL;
		uri->pathTail = NULL;
	}

	if (uri->owner) {
		/* Query */
		if (uri->query.first != NULL) {
			if (uri->query.first != uri->query.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->query.first);
			}
			uri->query.first = NULL;
			uri->query.afterLast = NULL;
		}

		/* Fragment */
		if (uri->fragment.first != NULL) {
			if (uri->fragment.first != uri->fragment.afterLast) {
				memory->free(memory, (URI_CHAR *)uri->fragment.first);
			}
			uri->fragment.first = NULL;
			uri->fragment.afterLast = NULL;
		}
	}

	return URI_SUCCESS;
}



UriBool URI_FUNC(_TESTING_ONLY_ParseIpSix)(const URI_CHAR * text) {
	UriMemoryManager * const memory = &defaultMemoryManager;
	URI_TYPE(Uri) uri;
	URI_TYPE(ParserState) parser;
	const URI_CHAR * const afterIpSix = text + URI_STRLEN(text);
	const URI_CHAR * res;

	URI_FUNC(ResetUri)(&uri);
	parser.uri = &uri;
	URI_FUNC(ResetParserStateExceptUri)(&parser);
	parser.uri->hostData.ip6 = memory->malloc(memory, 1 * sizeof(UriIp6));
	res = URI_FUNC(ParseIPv6address2)(&parser, text, afterIpSix, memory);
	URI_FUNC(FreeUriMembersMm)(&uri, memory);
	return res == afterIpSix ? URI_TRUE : URI_FALSE;
}



UriBool URI_FUNC(_TESTING_ONLY_ParseIpFour)(const URI_CHAR * text) {
	unsigned char octets[4];
	int res = URI_FUNC(ParseIpFourAddress)(octets, text, text + URI_STRLEN(text));
	return (res == URI_SUCCESS) ? URI_TRUE : URI_FALSE;
}



#undef URI_SET_DIGIT
#undef URI_SET_HEX_LETTER_UPPER
#undef URI_SET_HEX_LETTER_LOWER
#undef URI_SET_HEXDIG
#undef URI_SET_ALPHA



#endif
