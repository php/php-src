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
 * @file UriIp4.c
 * Holds the IPv4 parser implementation.
 * NOTE: This source file includes itself twice.
 */

/* What encodings are enabled? */
#include <uriparser/UriDefsConfig.h>
#if (!defined(URI_PASS_ANSI) && !defined(URI_PASS_UNICODE))
/* Include SELF twice */
# ifdef URI_ENABLE_ANSI
#  define URI_PASS_ANSI 1
#  include "UriIp4.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriIp4.c"
#  undef URI_PASS_UNICODE
# endif
#else
# ifdef URI_PASS_ANSI
#  include <uriparser/UriDefsAnsi.h>
# else
#  include <uriparser/UriDefsUnicode.h>
# endif



#ifndef URI_DOXYGEN
# include <uriparser/UriIp4.h>
# include "UriIp4Base.h"
# include <uriparser/UriBase.h>
#endif



/* Prototypes */
static const URI_CHAR * URI_FUNC(ParseDecOctet)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseDecOctetOne)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseDecOctetTwo)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseDecOctetThree)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast);
static const URI_CHAR * URI_FUNC(ParseDecOctetFour)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast);



/*
 * [ipFourAddress]->[decOctet]<.>[decOctet]<.>[decOctet]<.>[decOctet]
 */
int URI_FUNC(ParseIpFourAddress)(unsigned char * octetOutput,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	const URI_CHAR * after;
	UriIp4Parser parser;

	/* Essential checks */
	if ((octetOutput == NULL) || (first == NULL)
			|| (afterLast <= first)) {
		return URI_ERROR_SYNTAX;
	}

	/* Reset parser */
	parser.stackCount = 0;

	/* Octet #1 */
	after = URI_FUNC(ParseDecOctet)(&parser, first, afterLast);
	if ((after == NULL) || (after >= afterLast) || (*after != _UT('.'))) {
		return URI_ERROR_SYNTAX;
	}
	uriStackToOctet(&parser, octetOutput);

	/* Octet #2 */
	after = URI_FUNC(ParseDecOctet)(&parser, after + 1, afterLast);
	if ((after == NULL) || (after >= afterLast) || (*after != _UT('.'))) {
		return URI_ERROR_SYNTAX;
	}
	uriStackToOctet(&parser, octetOutput + 1);

	/* Octet #3 */
	after = URI_FUNC(ParseDecOctet)(&parser, after + 1, afterLast);
	if ((after == NULL) || (after >= afterLast) || (*after != _UT('.'))) {
		return URI_ERROR_SYNTAX;
	}
	uriStackToOctet(&parser, octetOutput + 2);

	/* Octet #4 */
	after = URI_FUNC(ParseDecOctet)(&parser, after + 1, afterLast);
	if (after != afterLast) {
		return URI_ERROR_SYNTAX;
	}
	uriStackToOctet(&parser, octetOutput + 3);

	return URI_SUCCESS;
}



/*
 * [decOctet]-><0>
 * [decOctet]-><1>[decOctetOne]
 * [decOctet]-><2>[decOctetTwo]
 * [decOctet]-><3>[decOctetThree]
 * [decOctet]-><4>[decOctetThree]
 * [decOctet]-><5>[decOctetThree]
 * [decOctet]-><6>[decOctetThree]
 * [decOctet]-><7>[decOctetThree]
 * [decOctet]-><8>[decOctetThree]
 * [decOctet]-><9>[decOctetThree]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseDecOctet)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return NULL;
	}

	switch (*first) {
	case _UT('0'):
		uriPushToStack(parser, 0);
		return first + 1;

	case _UT('1'):
		uriPushToStack(parser, 1);
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetOne)(parser, first + 1, afterLast);

	case _UT('2'):
		uriPushToStack(parser, 2);
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetTwo)(parser, first + 1, afterLast);

	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetThree)(parser, first + 1, afterLast);

	default:
		return NULL;
	}
}



/*
 * [decOctetOne]-><NULL>
 * [decOctetOne]->[DIGIT][decOctetThree]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseDecOctetOne)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetThree)(parser, first + 1, afterLast);

	default:
		return first;
	}
}



/*
 * [decOctetTwo]-><NULL>
 * [decOctetTwo]-><0>[decOctetThree]
 * [decOctetTwo]-><1>[decOctetThree]
 * [decOctetTwo]-><2>[decOctetThree]
 * [decOctetTwo]-><3>[decOctetThree]
 * [decOctetTwo]-><4>[decOctetThree]
 * [decOctetTwo]-><5>[decOctetFour]
 * [decOctetTwo]-><6>
 * [decOctetTwo]-><7>
 * [decOctetTwo]-><8>
 * [decOctetTwo]-><9>
*/
static URI_INLINE const URI_CHAR * URI_FUNC(ParseDecOctetTwo)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetThree)(parser, first + 1, afterLast);

	case _UT('5'):
		uriPushToStack(parser, 5);
		return (const URI_CHAR *)URI_FUNC(ParseDecOctetFour)(parser, first + 1, afterLast);

	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return first + 1;

	default:
		return first;
	}
}



/*
 * [decOctetThree]-><NULL>
 * [decOctetThree]->[DIGIT]
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseDecOctetThree)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
	case _UT('6'):
	case _UT('7'):
	case _UT('8'):
	case _UT('9'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return first + 1;

	default:
		return first;
	}
}



/*
 * [decOctetFour]-><NULL>
 * [decOctetFour]-><0>
 * [decOctetFour]-><1>
 * [decOctetFour]-><2>
 * [decOctetFour]-><3>
 * [decOctetFour]-><4>
 * [decOctetFour]-><5>
 */
static URI_INLINE const URI_CHAR * URI_FUNC(ParseDecOctetFour)(UriIp4Parser * parser,
		const URI_CHAR * first, const URI_CHAR * afterLast) {
	if (first >= afterLast) {
		return afterLast;
	}

	switch (*first) {
	case _UT('0'):
	case _UT('1'):
	case _UT('2'):
	case _UT('3'):
	case _UT('4'):
	case _UT('5'):
		uriPushToStack(parser, (unsigned char)(9 + *first - _UT('9')));
		return first + 1;

	default:
		return first;
	}
}



#endif
