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
 * The source code included in this files was separated from mbfilter_jis.c
 * by rui hirokawa <hirokawa@php.net> on 18 aug 2011.
 *
 */

#include "mbfilter.h"
#include "mbfilter_iso2022jp_2004.h"
#include "mbfilter_sjis_2004.h"

#include "unicode_table_jis.h"

extern int mbfl_filt_conv_any_jis_flush(mbfl_convert_filter *filter);

const mbfl_encoding mbfl_encoding_2022jp_2004 = {
	mbfl_no_encoding_2022jp_2004,
	"ISO-2022-JP-2004",
	"ISO-2022-JP-2004",
	NULL,
	NULL,
	MBFL_ENCTYPE_MBCS | MBFL_ENCTYPE_GL_UNSAFE,
	&vtbl_2022jp_2004_wchar,
	&vtbl_wchar_2022jp_2004
};

const struct mbfl_convert_vtbl vtbl_2022jp_2004_wchar = {
	mbfl_no_encoding_2022jp_2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_common_flush,
	NULL,
};

const struct mbfl_convert_vtbl vtbl_wchar_2022jp_2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_2022jp_2004,
	mbfl_filt_conv_common_ctor,
	NULL,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_wchar_jis2004_flush,
	NULL,
};
