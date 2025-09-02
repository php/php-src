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
#  include "UriEscape.c"
#  undef URI_PASS_ANSI
# endif
# ifdef URI_ENABLE_UNICODE
#  define URI_PASS_UNICODE 1
#  include "UriEscape.c"
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
#endif



URI_CHAR * URI_FUNC(Escape)(const URI_CHAR * in, URI_CHAR * out,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	return URI_FUNC(EscapeEx)(in, NULL, out, spaceToPlus, normalizeBreaks);
}



URI_CHAR * URI_FUNC(EscapeEx)(const URI_CHAR * inFirst,
		const URI_CHAR * inAfterLast, URI_CHAR * out,
		UriBool spaceToPlus, UriBool normalizeBreaks) {
	const URI_CHAR * read = inFirst;
	URI_CHAR * write = out;
	UriBool prevWasCr = URI_FALSE;
	if ((out == NULL) || (inFirst == out)) {
		return NULL;
	} else if (inFirst == NULL) {
		if (out != NULL) {
			out[0] = _UT('\0');
		}
		return out;
	}

	for (;;) {
		if ((inAfterLast != NULL) && (read >= inAfterLast)) {
			write[0] = _UT('\0');
			return write;
		}

		switch (read[0]) {
		case _UT('\0'):
			write[0] = _UT('\0');
			return write;

		case _UT(' '):
			if (spaceToPlus) {
				write[0] = _UT('+');
				write++;
			} else {
				write[0] = _UT('%');
				write[1] = _UT('2');
				write[2] = _UT('0');
				write += 3;
			}
			prevWasCr = URI_FALSE;
			break;

		case _UT('a'): /* ALPHA */
		case _UT('A'):
		case _UT('b'):
		case _UT('B'):
		case _UT('c'):
		case _UT('C'):
		case _UT('d'):
		case _UT('D'):
		case _UT('e'):
		case _UT('E'):
		case _UT('f'):
		case _UT('F'):
		case _UT('g'):
		case _UT('G'):
		case _UT('h'):
		case _UT('H'):
		case _UT('i'):
		case _UT('I'):
		case _UT('j'):
		case _UT('J'):
		case _UT('k'):
		case _UT('K'):
		case _UT('l'):
		case _UT('L'):
		case _UT('m'):
		case _UT('M'):
		case _UT('n'):
		case _UT('N'):
		case _UT('o'):
		case _UT('O'):
		case _UT('p'):
		case _UT('P'):
		case _UT('q'):
		case _UT('Q'):
		case _UT('r'):
		case _UT('R'):
		case _UT('s'):
		case _UT('S'):
		case _UT('t'):
		case _UT('T'):
		case _UT('u'):
		case _UT('U'):
		case _UT('v'):
		case _UT('V'):
		case _UT('w'):
		case _UT('W'):
		case _UT('x'):
		case _UT('X'):
		case _UT('y'):
		case _UT('Y'):
		case _UT('z'):
		case _UT('Z'):
		case _UT('0'): /* DIGIT */
		case _UT('1'):
		case _UT('2'):
		case _UT('3'):
		case _UT('4'):
		case _UT('5'):
		case _UT('6'):
		case _UT('7'):
		case _UT('8'):
		case _UT('9'):
		case _UT('-'): /* "-" / "." / "_" / "~" */
		case _UT('.'):
		case _UT('_'):
		case _UT('~'):
			/* Copy unmodified */
			write[0] = read[0];
			write++;

			prevWasCr = URI_FALSE;
			break;

		case _UT('\x0a'):
			if (normalizeBreaks) {
				if (!prevWasCr) {
					write[0] = _UT('%');
					write[1] = _UT('0');
					write[2] = _UT('D');
					write[3] = _UT('%');
					write[4] = _UT('0');
					write[5] = _UT('A');
					write += 6;
				}
			} else {
				write[0] = _UT('%');
				write[1] = _UT('0');
				write[2] = _UT('A');
				write += 3;
			}
			prevWasCr = URI_FALSE;
			break;

		case _UT('\x0d'):
			if (normalizeBreaks) {
				write[0] = _UT('%');
				write[1] = _UT('0');
				write[2] = _UT('D');
				write[3] = _UT('%');
				write[4] = _UT('0');
				write[5] = _UT('A');
				write += 6;
			} else {
				write[0] = _UT('%');
				write[1] = _UT('0');
				write[2] = _UT('D');
				write += 3;
			}
			prevWasCr = URI_TRUE;
			break;

		default:
			/* Percent encode */
			{
				const unsigned char code = (unsigned char)read[0];
				write[0] = _UT('%');
				write[1] = URI_FUNC(HexToLetter)(code >> 4);
				write[2] = URI_FUNC(HexToLetter)(code & 0x0f);
				write += 3;
			}
			prevWasCr = URI_FALSE;
			break;
		}

		read++;
	}
}



const URI_CHAR * URI_FUNC(UnescapeInPlace)(URI_CHAR * inout) {
	return URI_FUNC(UnescapeInPlaceEx)(inout, URI_FALSE, URI_BR_DONT_TOUCH);
}



const URI_CHAR * URI_FUNC(UnescapeInPlaceEx)(URI_CHAR * inout,
		UriBool plusToSpace, UriBreakConversion breakConversion) {
	URI_CHAR * read = inout;
	URI_CHAR * write = inout;
	UriBool prevWasCr = URI_FALSE;

	if (inout == NULL) {
		return NULL;
	}

	for (;;) {
		switch (read[0]) {
		case _UT('\0'):
			if (read > write) {
				write[0] = _UT('\0');
			}
			return write;

		case _UT('%'):
			switch (read[1]) {
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
			case _UT('a'):
			case _UT('b'):
			case _UT('c'):
			case _UT('d'):
			case _UT('e'):
			case _UT('f'):
			case _UT('A'):
			case _UT('B'):
			case _UT('C'):
			case _UT('D'):
			case _UT('E'):
			case _UT('F'):
				switch (read[2]) {
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
				case _UT('a'):
				case _UT('b'):
				case _UT('c'):
				case _UT('d'):
				case _UT('e'):
				case _UT('f'):
				case _UT('A'):
				case _UT('B'):
				case _UT('C'):
				case _UT('D'):
				case _UT('E'):
				case _UT('F'):
					{
						/* Percent group found */
						const unsigned char left = URI_FUNC(HexdigToInt)(read[1]);
						const unsigned char right = URI_FUNC(HexdigToInt)(read[2]);
						const int code = 16 * left + right;
						switch (code) {
						case 10:
							switch (breakConversion) {
							case URI_BR_TO_LF:
								if (!prevWasCr) {
									write[0] = (URI_CHAR)10;
									write++;
								}
								break;

							case URI_BR_TO_CRLF:
								if (!prevWasCr) {
									write[0] = (URI_CHAR)13;
									write[1] = (URI_CHAR)10;
									write += 2;
								}
								break;

							case URI_BR_TO_CR:
								if (!prevWasCr) {
									write[0] = (URI_CHAR)13;
									write++;
								}
								break;

							case URI_BR_DONT_TOUCH:
							default:
								write[0] = (URI_CHAR)10;
								write++;

							}
							prevWasCr = URI_FALSE;
							break;

						case 13:
							switch (breakConversion) {
							case URI_BR_TO_LF:
								write[0] = (URI_CHAR)10;
								write++;
								break;

							case URI_BR_TO_CRLF:
								write[0] = (URI_CHAR)13;
								write[1] = (URI_CHAR)10;
								write += 2;
								break;

							case URI_BR_TO_CR:
								write[0] = (URI_CHAR)13;
								write++;
								break;

							case URI_BR_DONT_TOUCH:
							default:
								write[0] = (URI_CHAR)13;
								write++;

							}
							prevWasCr = URI_TRUE;
							break;

						default:
							write[0] = (URI_CHAR)(code);
							write++;

							prevWasCr = URI_FALSE;

						}
						read += 3;
					}
					break;

				default:
					/* Copy two chars unmodified and */
					/* look at this char again */
					if (read > write) {
						write[0] = read[0];
						write[1] = read[1];
					}
					read += 2;
					write += 2;

					prevWasCr = URI_FALSE;
				}
				break;

			default:
				/* Copy one char unmodified and */
				/* look at this char again */
				if (read > write) {
					write[0] = read[0];
				}
				read++;
				write++;

				prevWasCr = URI_FALSE;
			}
			break;

		case _UT('+'):
			if (plusToSpace) {
				/* Convert '+' to ' ' */
				write[0] = _UT(' ');
			} else {
				/* Copy one char unmodified */
				if (read > write) {
					write[0] = read[0];
				}
			}
			read++;
			write++;

			prevWasCr = URI_FALSE;
			break;

		default:
			/* Copy one char unmodified */
			if (read > write) {
				write[0] = read[0];
			}
			read++;
			write++;

			prevWasCr = URI_FALSE;
		}
	}
}



#endif
