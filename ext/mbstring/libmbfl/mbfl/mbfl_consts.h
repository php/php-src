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
 * The source code included in this files was separated from mbfilter.h
 * by Moriyoshi Koizumi <moriyoshi@php.net> on 20 Dec 2002. The file
 * mbfilter.h is included in this package .
 *
 */

#ifndef MBFL_CONSTS_H
#define MBFL_CONSTS_H

#define MBFL_ENCTYPE_SBCS		0x00000001 /* single-byte encoding */
#define MBFL_ENCTYPE_WCS2		0x00000002 /* 2 bytes/char */
#define MBFL_ENCTYPE_WCS4		0x00000004 /* 4 bytes/char */
#define MBFL_ENCTYPE_GL_UNSAFE	0x00004000

#define MBFL_WCSPLANE_UCS2MAX		0x00010000
#define MBFL_WCSPLANE_UTF32MAX		0x00110000
#define MBFL_WCSPLANE_SUPMIN		0x00010000
#define MBFL_WCSPLANE_SUPMAX		0x00200000

/* Marker for an erroneous input byte (or sequence of bytes) */
#define MBFL_BAD_INPUT (-1)

#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_NONE 0
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_CHAR 1
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_LONG 2
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_ENTITY 3
#define MBFL_OUTPUTFILTER_ILLEGAL_MODE_BADUTF8 4 /* For internal use only; deliberately uses invalid UTF-8 byte sequence as error marker */

#endif /* MBFL_CONSTS_H */
