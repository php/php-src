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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 4 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifndef MBFL_MBFILTER_BYTE4_H
#define MBFL_MBFILTER_BYTE4_H

extern const mbfl_encoding mbfl_encoding_byte4be;
extern const mbfl_encoding mbfl_encoding_byte4le;
extern const struct mbfl_convert_vtbl vtbl_byte4be_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_byte4be;
extern const struct mbfl_convert_vtbl vtbl_byte4le_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_byte4le;

int mbfl_filt_conv_wchar_byte4be(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_byte4be_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_byte4le(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_byte4le_wchar(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_BYTE4_H */
