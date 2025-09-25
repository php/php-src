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
#  include "UriSetHostRegName.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriSetHostRegName.c"
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



UriBool URI_FUNC(IsWellFormedHostRegName)(const URI_CHAR * first, const URI_CHAR * afterLast) {
	if ((first == NULL) || (afterLast == NULL)) {
		return URI_FALSE;
	}

	/* reg-name = *( unreserved / pct-encoded / sub-delims ) */
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

			default:
				return URI_FALSE;
		}

		first++;
	}
	return URI_TRUE;
}



int URI_FUNC(SetHostRegNameMm)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast,
		UriMemoryManager * memory) {
	return URI_FUNC(InternalSetHostMm)(uri, URI_HOST_TYPE_REGNAME, first, afterLast, memory);
}



int URI_FUNC(SetHostRegName)(URI_TYPE(Uri) * uri,
		const URI_CHAR * first,
		const URI_CHAR * afterLast) {
	return URI_FUNC(SetHostRegNameMm)(uri, first, afterLast, NULL);
}



#endif
