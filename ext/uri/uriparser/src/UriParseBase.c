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
# include "UriParseBase.h"
#endif



void uriWriteQuadToDoubleByte(const unsigned char * hexDigits, int digitCount, unsigned char * output) {
	switch (digitCount) {
	case 1:
		/* 0x___? -> \x00 \x0? */
		output[0] = 0;
		output[1] = hexDigits[0];
		break;

	case 2:
		/* 0x__?? -> \0xx \x?? */
		output[0] = 0;
		output[1] = 16 * hexDigits[0] + hexDigits[1];
		break;

	case 3:
		/* 0x_??? -> \0x? \x?? */
		output[0] = hexDigits[0];
		output[1] = 16 * hexDigits[1] + hexDigits[2];
		break;

	case 4:
		/* 0x???? -> \0?? \x?? */
		output[0] = 16 * hexDigits[0] + hexDigits[1];
		output[1] = 16 * hexDigits[2] + hexDigits[3];
		break;

	}
}



unsigned char uriGetOctetValue(const unsigned char * digits, int digitCount) {
	switch (digitCount) {
	case 1:
		return digits[0];

	case 2:
		return 10 * digits[0] + digits[1];

	case 3:
	default:
		return 100 * digits[0] + 10 * digits[1] + digits[2];

	}
}
