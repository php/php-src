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
 * the source code included in this files was separated from mbfilter_sjis_open.c
 * by Rui Hirokawa <hirokawa@php.net> on 25 July 2011.
 *
 */

#ifndef MBFL_MBFILTER_SJIS_MAC_H
#define MBFL_MBFILTER_SJIS_MAC_H

#include "mbfilter.h"

extern const mbfl_encoding mbfl_encoding_sjis_mac;

extern const struct mbfl_identify_vtbl vtbl_identify_sjis_mac;
extern const struct mbfl_convert_vtbl vtbl_sjis_mac_wchar;
extern const struct mbfl_convert_vtbl vtbl_wchar_sjis_mac;

int mbfl_filt_conv_sjis_mac_wchar(int c, mbfl_convert_filter *filter);
int mbfl_filt_conv_wchar_sjis_mac(int c, mbfl_convert_filter *filter);

#endif /* MBFL_MBFILTER_SJIS_MAC_H */
