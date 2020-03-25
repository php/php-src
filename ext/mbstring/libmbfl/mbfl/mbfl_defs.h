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

#ifndef MBFL_DEFS_H
#define MBFL_DEFS_H

#ifndef NULL
#ifdef __cplusplus
#define NULL (0L)
#else
#define NULL (void *)(0L)
#endif
#endif

#ifndef SIZE_MAX
#define SIZE_MAX	((size_t)~0)
#endif

#ifdef WIN32
#ifdef MBFL_DLL_EXPORT
#define MBFLAPI __declspec(dllexport)
#else
#define MBFLAPI __declspec(dllimport)
#endif /* MBFL_DLL_EXPORT */
#else
#if defined(__GNUC__) && __GNUC__ >= 4
#define MBFLAPI __attribute__((visibility("default")))
#else
#define MBFLAPI
#endif /* defined(__GNUC__) && __GNUC__ >= 4 */
#endif /* WIN32 */

#endif /* MBFL_DEFS_H */
