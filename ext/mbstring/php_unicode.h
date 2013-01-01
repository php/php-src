/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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

#if HAVE_MBSTRING
/*
 * Values that can appear in the `mask1' parameter of the php_unicode_is_prop()
 * function.
 */
#define UC_MN 0x00000001 /* Mark, Non-Spacing          */
#define UC_MC 0x00000002 /* Mark, Spacing Combining    */
#define UC_ME 0x00000004 /* Mark, Enclosing            */
#define UC_ND 0x00000008 /* Number, Decimal Digit      */
#define UC_NL 0x00000010 /* Number, Letter             */
#define UC_NO 0x00000020 /* Number, Other              */
#define UC_ZS 0x00000040 /* Separator, Space           */
#define UC_ZL 0x00000080 /* Separator, Line            */
#define UC_ZP 0x00000100 /* Separator, Paragraph       */
#define UC_CC 0x00000200 /* Other, Control             */
#define UC_CF 0x00000400 /* Other, Format              */
#define UC_OS 0x00000800 /* Other, Surrogate           */
#define UC_CO 0x00001000 /* Other, Private Use         */
#define UC_CN 0x00002000 /* Other, Not Assigned        */
#define UC_LU 0x00004000 /* Letter, Uppercase          */
#define UC_LL 0x00008000 /* Letter, Lowercase          */
#define UC_LT 0x00010000 /* Letter, Titlecase          */
#define UC_LM 0x00020000 /* Letter, Modifier           */
#define UC_LO 0x00040000 /* Letter, Other              */
#define UC_PC 0x00080000 /* Punctuation, Connector     */
#define UC_PD 0x00100000 /* Punctuation, Dash          */
#define UC_PS 0x00200000 /* Punctuation, Open          */
#define UC_PE 0x00400000 /* Punctuation, Close         */
#define UC_PO 0x00800000 /* Punctuation, Other         */
#define UC_SM 0x01000000 /* Symbol, Math               */
#define UC_SC 0x02000000 /* Symbol, Currency           */
#define UC_SK 0x04000000 /* Symbol, Modifier           */
#define UC_SO 0x08000000 /* Symbol, Other              */
#define UC_L  0x10000000 /* Left-To-Right              */
#define UC_R  0x20000000 /* Right-To-Left              */
#define UC_EN 0x40000000 /* European Number            */
#define UC_ES 0x80000000 /* European Number Separator  */

/*
 * Values that can appear in the `mask2' parameter of the php_unicode_is_prop()
 * function.
 */
#define UC_ET 0x00000001 /* European Number Terminator */
#define UC_AN 0x00000002 /* Arabic Number              */
#define UC_CS 0x00000004 /* Common Number Separator    */
#define UC_B  0x00000008 /* Block Separator            */
#define UC_S  0x00000010 /* Segment Separator          */
#define UC_WS 0x00000020 /* Whitespace                 */
#define UC_ON 0x00000040 /* Other Neutrals             */
/*
 * Implementation specific character properties.
 */
#define UC_CM 0x00000080 /* Composite                  */
#define UC_NB 0x00000100 /* Non-Breaking               */
#define UC_SY 0x00000200 /* Symmetric                  */
#define UC_HD 0x00000400 /* Hex Digit                  */
#define UC_QM 0x00000800 /* Quote Mark                 */
#define UC_MR 0x00001000 /* Mirroring                  */
#define UC_SS 0x00002000 /* Space, other               */

#define UC_CP 0x00004000 /* Defined                    */

/*
 * Added for UnicodeData-2.1.3.
 */
#define UC_PI 0x00008000 /* Punctuation, Initial       */
#define UC_PF 0x00010000 /* Punctuation, Final         */

MBSTRING_API int php_unicode_is_prop(unsigned long code, unsigned long mask1,
		unsigned long mask2);
MBSTRING_API char *php_unicode_convert_case(int case_mode, const char *srcstr, size_t srclen, size_t *retlen,
		const char *src_encoding TSRMLS_DC);

#define PHP_UNICODE_CASE_UPPER	0
#define PHP_UNICODE_CASE_LOWER	1
#define PHP_UNICODE_CASE_TITLE	2

#define php_unicode_is_alpha(cc) php_unicode_is_prop(cc, UC_LU|UC_LL|UC_LM|UC_LO|UC_LT, 0)
#define php_unicode_is_digit(cc) php_unicode_is_prop(cc, UC_ND, 0)
#define php_unicode_is_alnum(cc) php_unicode_is_prop(cc, UC_LU|UC_LL|UC_LM|UC_LO|UC_LT|UC_ND, 0)
#define php_unicode_is_cntrl(cc) php_unicode_is_prop(cc, UC_CC|UC_CF, 0)
#define php_unicode_is_space(cc) php_unicode_is_prop(cc, UC_ZS|UC_SS, 0)
#define php_unicode_is_blank(cc) php_unicode_is_prop(cc, UC_ZS, 0)
#define php_unicode_is_punct(cc) php_unicode_is_prop(cc, UC_PD|UC_PS|UC_PE|UC_PO, UC_PI|UC_PF)
#define php_unicode_is_graph(cc) php_unicode_is_prop(cc, UC_MN|UC_MC|UC_ME|UC_ND|UC_NL|UC_NO|\
                               UC_LU|UC_LL|UC_LT|UC_LM|UC_LO|UC_PC|UC_PD|\
                               UC_PS|UC_PE|UC_PO|UC_SM|UC_SM|UC_SC|UC_SK|\
                               UC_SO, UC_PI|UC_PF)
#define php_unicode_is_print(cc) php_unicode_is_prop(cc, UC_MN|UC_MC|UC_ME|UC_ND|UC_NL|UC_NO|\
                               UC_LU|UC_LL|UC_LT|UC_LM|UC_LO|UC_PC|UC_PD|\
                               UC_PS|UC_PE|UC_PO|UC_SM|UC_SM|UC_SC|UC_SK|\
                               UC_SO|UC_ZS, UC_PI|UC_PF)
#define php_unicode_is_upper(cc) php_unicode_is_prop(cc, UC_LU, 0)
#define php_unicode_is_lower(cc) php_unicode_is_prop(cc, UC_LL, 0)
#define php_unicode_is_title(cc) php_unicode_is_prop(cc, UC_LT, 0)
#define php_unicode_is_xdigit(cc) php_unicode_is_prop(cc, 0, UC_HD)

#define php_unicode_is_isocntrl(cc) php_unicode_is_prop(cc, UC_CC, 0)
#define php_unicode_is_fmtcntrl(cc) php_unicode_is_prop(cc, UC_CF, 0)

#define php_unicode_is_symbol(cc) php_unicode_is_prop(cc, UC_SM|UC_SC|UC_SO|UC_SK, 0)
#define php_unicode_is_number(cc) php_unicode_is_prop(cc, UC_ND|UC_NO|UC_NL, 0)
#define php_unicode_is_nonspacing(cc) php_unicode_is_prop(cc, UC_MN, 0)
#define php_unicode_is_openpunct(cc) php_unicode_is_prop(cc, UC_PS, 0)
#define php_unicode_is_closepunct(cc) php_unicode_is_prop(cc, UC_PE, 0)
#define php_unicode_is_initialpunct(cc) php_unicode_is_prop(cc, 0, UC_PI)
#define php_unicode_is_finalpunct(cc) php_unicode_is_prop(cc, 0, UC_PF)

#define php_unicode_is_composite(cc) php_unicode_is_prop(cc, 0, UC_CM)
#define php_unicode_is_hex(cc) php_unicode_is_prop(cc, 0, UC_HD)
#define php_unicode_is_quote(cc) php_unicode_is_prop(cc, 0, UC_QM)
#define php_unicode_is_symmetric(cc) php_unicode_is_prop(cc, 0, UC_SY)
#define php_unicode_is_mirroring(cc) php_unicode_is_prop(cc, 0, UC_MR)
#define php_unicode_is_nonbreaking(cc) php_unicode_is_prop(cc, 0, UC_NB)

/*
 * Directionality macros.
 */
#define php_unicode_is_rtl(cc) php_unicode_is_prop(cc, UC_R, 0)
#define php_unicode_is_ltr(cc) php_unicode_is_prop(cc, UC_L, 0)
#define php_unicode_is_strong(cc) php_unicode_is_prop(cc, UC_L|UC_R, 0)
#define php_unicode_is_weak(cc) php_unicode_is_prop(cc, UC_EN|UC_ES, UC_ET|UC_AN|UC_CS)
#define php_unicode_is_neutral(cc) php_unicode_is_prop(cc, 0, UC_B|UC_S|UC_WS|UC_ON)
#define php_unicode_is_separator(cc) php_unicode_is_prop(cc, 0, UC_B|UC_S)

/*
 * Other macros inspired by John Cowan.
 */
#define php_unicode_is_mark(cc) php_unicode_is_prop(cc, UC_MN|UC_MC|UC_ME, 0)
#define php_unicode_is_modif(cc) php_unicode_is_prop(cc, UC_LM, 0)
#define php_unicode_is_letnum(cc) php_unicode_is_prop(cc, UC_NL, 0)
#define php_unicode_is_connect(cc) php_unicode_is_prop(cc, UC_PC, 0)
#define php_unicode_is_dash(cc) php_unicode_is_prop(cc, UC_PD, 0)
#define php_unicode_is_math(cc) php_unicode_is_prop(cc, UC_SM, 0)
#define php_unicode_is_currency(cc) php_unicode_is_prop(cc, UC_SC, 0)
#define php_unicode_is_modifsymbol(cc) php_unicode_is_prop(cc, UC_SK, 0)
#define php_unicode_is_nsmark(cc) php_unicode_is_prop(cc, UC_MN, 0)
#define php_unicode_is_spmark(cc) php_unicode_is_prop(cc, UC_MC, 0)
#define php_unicode_is_enclosing(cc) php_unicode_is_prop(cc, UC_ME, 0)
#define php_unicode_is_private(cc) php_unicode_is_prop(cc, UC_CO, 0)
#define php_unicode_is_surrogate(cc) php_unicode_is_prop(cc, UC_OS, 0)
#define php_unicode_is_lsep(cc) php_unicode_is_prop(cc, UC_ZL, 0)
#define php_unicode_is_psep(cc) php_unicode_is_prop(cc, UC_ZP, 0)

#define php_unicode_is_identstart(cc) php_unicode_is_prop(cc, UC_LU|UC_LL|UC_LT|UC_LO|UC_NL, 0)
#define php_unicode_is_identpart(cc) php_unicode_is_prop(cc, UC_LU|UC_LL|UC_LT|UC_LO|UC_NL|\
                                   UC_MN|UC_MC|UC_ND|UC_PC|UC_CF, 0)

#define php_unicode_is_defined(cc) php_unicode_is_prop(cc, 0, UC_CP)
#define php_unicode_is_undefined(cc) !php_unicode_is_prop(cc, 0, UC_CP)

/*
 * Other miscellaneous character property macros.
 */
#define php_unicode_is_han(cc) (((cc) >= 0x4e00 && (cc) <= 0x9fff) ||\
                     ((cc) >= 0xf900 && (cc) <= 0xfaff))
#define php_unicode_is_hangul(cc) ((cc) >= 0xac00 && (cc) <= 0xd7ff)


#endif


#endif /* PHP_UNICODE_H */



