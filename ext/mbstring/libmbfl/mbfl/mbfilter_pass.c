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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "mbfilter.h"
#include "mbfilter_pass.h"

static const char *mbfl_encoding_pass_aliases[] = {"none", NULL};

const mbfl_encoding mbfl_encoding_pass = {
	mbfl_no_encoding_pass,
	"pass",
	NULL,
	(const char *(*)[])&mbfl_encoding_pass_aliases,
	NULL,
	0
};

const struct mbfl_convert_vtbl vtbl_pass = {
	mbfl_no_encoding_pass,
	mbfl_no_encoding_pass,
	mbfl_filt_conv_common_ctor,
	mbfl_filt_conv_common_dtor,
	mbfl_filt_conv_pass,
	mbfl_filt_conv_common_flush
};

int mbfl_filt_conv_pass(int c, mbfl_convert_filter *filter)
{
	return (*filter->output_function)(c, filter->data);
}

