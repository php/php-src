/*
 * "streamable kanji code filter and converter"
 * Copyright (c) 1998-2002 HappySize, Inc. All rights reserved.
 *
 * LICENSE NOTICES
 *
 * This file is part of "streamable kanji code filter and converter",
 * which is distributed under the terms of GNU Lesser General Public 
 * License (version 2) as published by the Free Software Foundation.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with "streamable kanji code filter and converter";
 * if not, write to the Free Software Foundation, Inc., 59 Temple Place,
 * Suite 330, Boston, MA  02111-1307  USA
 *
 * The author of this file:
 *
 */
/*
 * The source code included in this files was separated from mbfilter.c
 * by rui hrokawa <hirokawa@php.net> on 8 aug 2011.
 * 
 */

#ifndef MBFL_MBFILTER_UTF8_MOBILE_H
#define MBFL_MBFILTER_UTF8_MOBILE_H

extern const mbfl_encoding mbfl_encoding_utf8_docomo;
extern const mbfl_encoding mbfl_encoding_utf8_kddi_a;
extern const mbfl_encoding mbfl_encoding_utf8_kddi_b;
extern const mbfl_encoding mbfl_encoding_utf8_sb;

extern const struct mbfl_identify_vtbl vtbl_identify_utf8_docomo;
extern const struct mbfl_identify_vtbl vtbl_identify_utf8_kddi_a;
extern const struct mbfl_identify_vtbl vtbl_identify_utf8_kddi_b;
extern const struct mbfl_identify_vtbl vtbl_identify_utf8_sb;

extern const struct mbfl_convert_vtbl vtbl_utf8_docomo_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf8_docomo;
extern const struct mbfl_convert_vtbl vtbl_utf8_kddi_a_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf8_kddi_a;
extern const struct mbfl_convert_vtbl vtbl_utf8_kddi_b_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf8_kddi_b;
extern const struct mbfl_convert_vtbl vtbl_utf8_sb_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf8_sb;

int mbfl_filt_conv_utf8_mobile_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_utf8_mobile(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_UTF8_MOBILE_H */
