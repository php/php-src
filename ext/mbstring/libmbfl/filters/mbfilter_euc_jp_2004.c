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
 * The source code included in this files was separated from mbfilter_ja.c
 * by rui hirokawa <hirokawa@php.net> on 16 aug 2011.
 * 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfilter.h"
#include "mbfilter_euc_jp_2004.h"
#include "mbfilter_sjis_2004.h"

#include "unicode_table_jis2004.h"

extern int mbfl_filt_ident_eucjp(int c, mbfl_identify_filter *filter);
extern const unsigned char mblen_table_eucjp[];

static const char *mbfl_encoding_eucjp2004_aliases[] = {"EUC_JP-2004", NULL};

const mbfl_encoding mbfl_encoding_eucjp2004 = {
	mbfl_no_encoding_eucjp2004,
	"EUC-JP-2004",
	"EUC-JP",
	(const char *(*)[])&mbfl_encoding_eucjp2004_aliases,
	mblen_table_eucjp,
	MBFL_ENCTYPE_MBCS
};

const struct mbfl_identify_vtbl vtbl_identify_eucjp2004 = {
	mbfl_no_encoding_eucjp2004,
	mbfl_filt_ident_common_ctor,
	mbfl_filt_ident_common_dtor,
	mbfl_filt_ident_eucjp
};

const struct mbfl_convert_vtbl vtbl_eucjp2004_wchar = {
	mbfl_no_encoding_eucjp2004,
	mbfl_no_encoding_wchar,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_jis2004_wchar,
	mbfl_filt_conv_common_flush
};

const struct mbfl_convert_vtbl vtbl_wchar_eucjp2004 = {
	mbfl_no_encoding_wchar,
	mbfl_no_encoding_eucjp2004,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_wchar_jis2004,
	mbfl_filt_conv_jis2004_flush
};

