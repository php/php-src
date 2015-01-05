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
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.c is included in this package .
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "mbfl_convert.h"
#include "mbfl_filter_output.h"

int mbfl_filter_output_pipe(int c, void* data)
{
	mbfl_convert_filter *filter = (mbfl_convert_filter*)data;
	return (*filter->filter_function)(c, filter);
}

int mbfl_filter_output_pipe_flush(void *data)
{
	mbfl_convert_filter *filter = (mbfl_convert_filter*)data;
	if (filter->filter_flush != NULL) {
		return (*filter->filter_flush)(filter);
	}

	return 0;
}

int mbfl_filter_output_null(int c, void* data)
{
	return c;
}
