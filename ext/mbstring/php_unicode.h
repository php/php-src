/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong (wez@thebrainroom.com)                           |
   +----------------------------------------------------------------------+

	Based on code from ucdata-2.5, which has the following Copyright:

	Copyright 2001 Computing Research Labs, New Mexico State University

	Permission is hereby granted, free of charge, to any person obtaining a
	copy of this software and associated documentation files (the "Software"),
	to deal in the Software without restriction, including without limitation
	the rights to use, copy, modify, merge, publish, distribute, sublicense,
	and/or sell copies of the Software, and to permit persons to whom the
	Software is furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.
*/

#ifndef PHP_UNICODE_H
#define PHP_UNICODE_H

#define UC_MN  0 /* Mark, Non-Spacing          */
#define UC_MC  1 /* Mark, Spacing Combining    */
#define UC_ME  2 /* Mark, Enclosing            */
#define UC_ND  3 /* Number, Decimal Digit      */
#define UC_NL  4 /* Number, Letter             */
#define UC_NO  5 /* Number, Other              */
#define UC_ZS  6 /* Separator, Space           */
#define UC_ZL  7 /* Separator, Line            */
#define UC_ZP  8 /* Separator, Paragraph       */
#define UC_OS  9 /* Other, Surrogate           */
#define UC_CO 10 /* Other, Private Use         */
#define UC_CN 11 /* Other, Not Assigned        */
#define UC_LU 12 /* Letter, Uppercase          */
#define UC_LL 13 /* Letter, Lowercase          */
#define UC_LT 14 /* Letter, Titlecase          */
#define UC_LM 15 /* Letter, Modifier           */
#define UC_LO 16 /* Letter, Other              */
#define UC_SM 17 /* Symbol, Math               */
#define UC_SC 18 /* Symbol, Currency           */
#define UC_SK 19 /* Symbol, Modifier           */
#define UC_SO 20 /* Symbol, Other              */
#define UC_L  21 /* Left-To-Right              */
#define UC_R  22 /* Right-To-Left              */
#define UC_EN 23 /* European Number            */
#define UC_ES 24 /* European Number Separator  */
#define UC_ET 25 /* European Number Terminator */
#define UC_AN 26 /* Arabic Number              */
#define UC_CS 27 /* Common Number Separator    */
#define UC_B  28 /* Block Separator            */
#define UC_S  29 /* Segment Separator          */
#define UC_WS 30 /* Whitespace                 */
#define UC_ON 31 /* Other Neutrals             */
#define UC_AL 32 /* Arabic Letter              */

/* Merged property categories */
#define UC_C 33 /* Control */
#define UC_P 34 /* Punctuation */

/* Derived properties from DerivedCoreProperties.txt */
#define UC_CASED          35
#define UC_CASE_IGNORABLE 36


MBSTRING_API bool php_unicode_is_prop1(unsigned long code, int prop);

typedef enum {
	PHP_UNICODE_CASE_UPPER = 0,
	PHP_UNICODE_CASE_LOWER,
	PHP_UNICODE_CASE_TITLE,
	PHP_UNICODE_CASE_FOLD,
	PHP_UNICODE_CASE_UPPER_SIMPLE,
	PHP_UNICODE_CASE_LOWER_SIMPLE,
	PHP_UNICODE_CASE_TITLE_SIMPLE,
	PHP_UNICODE_CASE_FOLD_SIMPLE,
	PHP_UNICODE_CASE_MODE_MAX
} php_case_mode;

MBSTRING_API zend_string *php_unicode_convert_case(
		php_case_mode case_mode, const char *srcstr, size_t srclen,
		const mbfl_encoding *src_encoding, const mbfl_encoding *dst_encoding, int illegal_mode, uint32_t illegal_substchar);

/*
 * Derived core properties.
 */

#define php_unicode_is_cased(cc) php_unicode_is_prop1(cc, UC_CASED)
#define php_unicode_is_case_ignorable(cc) php_unicode_is_prop1(cc, UC_CASE_IGNORABLE)

#endif /* PHP_UNICODE_H */
