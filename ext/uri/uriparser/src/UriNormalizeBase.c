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

#ifndef URI_DOXYGEN
# include "UriNormalizeBase.h"
#endif



UriBool uriIsUnreserved(int code) {
	switch (code) {
	case L'a': /* ALPHA */
	case L'A':
	case L'b':
	case L'B':
	case L'c':
	case L'C':
	case L'd':
	case L'D':
	case L'e':
	case L'E':
	case L'f':
	case L'F':
	case L'g':
	case L'G':
	case L'h':
	case L'H':
	case L'i':
	case L'I':
	case L'j':
	case L'J':
	case L'k':
	case L'K':
	case L'l':
	case L'L':
	case L'm':
	case L'M':
	case L'n':
	case L'N':
	case L'o':
	case L'O':
	case L'p':
	case L'P':
	case L'q':
	case L'Q':
	case L'r':
	case L'R':
	case L's':
	case L'S':
	case L't':
	case L'T':
	case L'u':
	case L'U':
	case L'v':
	case L'V':
	case L'w':
	case L'W':
	case L'x':
	case L'X':
	case L'y':
	case L'Y':
	case L'z':
	case L'Z':
	case L'0': /* DIGIT */
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
	case L'-': /* "-" / "." / "_" / "~" */
	case L'.':
	case L'_':
	case L'~':
		return URI_TRUE;

	default:
		return URI_FALSE;
	}
}
