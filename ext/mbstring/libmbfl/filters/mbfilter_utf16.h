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
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#ifndef MBFL_MBFILTER_UTF16_H
#define MBFL_MBFILTER_UTF16_H

extern const mbfl_encoding mbfl_encoding_utf16;
extern const mbfl_encoding mbfl_encoding_utf16be;
extern const mbfl_encoding mbfl_encoding_utf16le;

extern const struct mbfl_convert_vtbl vtbl_utf16_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf16;
extern const struct mbfl_convert_vtbl vtbl_utf16be_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf16be;
extern const struct mbfl_convert_vtbl vtbl_utf16le_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_utf16le;

int mbfl_filt_conv_utf16_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_utf16be_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_utf16be(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_utf16le_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_utf16le(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_UTF16_H */
