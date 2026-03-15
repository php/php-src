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

/**
 * @file UriSets.h
 * Holds character set definitions.
 */

// NOTE: We cannot use a regular include-once guard here because the
//       file must support being included twice, e.g. from file UriParse.c.
#if !defined(URI_SET_DIGIT)

// clang-format off
#  define URI_SET_DIGIT(ut) \
       ut('0'):  \
  case ut('1'): \
  /* clang-format on */ \
  case ut('2'): \
  case ut('3'): \
  case ut('4'): \
  case ut('5'): \
  case ut('6'): \
  case ut('7'): \
  case ut('8'): \
  case ut('9')

// clang-format off
#  define URI_SET_HEX_LETTER_LOWER(ut) \
       ut('a'): \
  case ut('b'): \
  /* clang-format on */ \
  case ut('c'): \
  case ut('d'): \
  case ut('e'): \
  case ut('f')

// clang-format off
#  define URI_SET_HEX_LETTER_UPPER(ut) \
       ut('A'): \
  case ut('B'): \
  /* clang-format on */ \
  case ut('C'): \
  case ut('D'): \
  case ut('E'): \
  case ut('F')

// clang-format off
#  define URI_SET_HEXDIG(ut) \
       URI_SET_DIGIT(ut): \
  case URI_SET_HEX_LETTER_LOWER(ut): \
  /* clang-format on */ \
  case URI_SET_HEX_LETTER_UPPER(ut)

// clang-format off
#  define URI_SET_ALPHA(ut) \
       URI_SET_HEX_LETTER_UPPER(ut): \
  case URI_SET_HEX_LETTER_LOWER(ut): \
  /* clang-format on */ \
  case ut('g'): \
  case ut('G'): \
  case ut('h'): \
  case ut('H'): \
  case ut('i'): \
  case ut('I'): \
  case ut('j'): \
  case ut('J'): \
  case ut('k'): \
  case ut('K'): \
  case ut('l'): \
  case ut('L'): \
  case ut('m'): \
  case ut('M'): \
  case ut('n'): \
  case ut('N'): \
  case ut('o'): \
  case ut('O'): \
  case ut('p'): \
  case ut('P'): \
  case ut('q'): \
  case ut('Q'): \
  case ut('r'): \
  case ut('R'): \
  case ut('s'): \
  case ut('S'): \
  case ut('t'): \
  case ut('T'): \
  case ut('u'): \
  case ut('U'): \
  case ut('v'): \
  case ut('V'): \
  case ut('w'): \
  case ut('W'): \
  case ut('x'): \
  case ut('X'): \
  case ut('y'): \
  case ut('Y'): \
  case ut('z'): \
  case ut('Z')

// clang-format off
#  define URI_SET_SUB_DELIMS(ut) \
       ut('!'): \
  case ut('$'): \
  /* clang-format on */ \
  case ut('&'): \
  case ut('\''): \
  case ut('('): \
  case ut(')'): \
  case ut('*'): \
  case ut('+'): \
  case ut(','): \
  case ut(';'): \
  case ut('=')

// clang-format off
#  define URI_SET_UNRESERVED(ut) \
       URI_SET_ALPHA(ut): \
  case URI_SET_DIGIT(ut): \
  /* clang-format on */ \
  case ut('-'): \
  case ut('.'): \
  case ut('_'): \
  case ut('~')

// clang-format off
#  define URI_SET_PCHAR_WITHOUT_PERCENT(ut) \
       URI_SET_UNRESERVED(ut): \
  case URI_SET_SUB_DELIMS(ut): \
  /* clang-format on */ \
  case ut(':'): \
  case ut('@')

// clang-format off
#  define URI_SET_PCHAR(ut) \
       URI_SET_PCHAR_WITHOUT_PERCENT(ut): \
  case ut('%')
/* clang-format on */

#endif  // ! defined(URI_SET_DIGIT)
