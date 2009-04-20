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

#ifndef MBFL_LANGUAGE_H
#define MBFL_LANGUAGE_H

#include "mbfl_defs.h"
#include "mbfl_encoding.h"

enum mbfl_no_language {
	mbfl_no_language_invalid = -1,
	mbfl_no_language_neutral,
	mbfl_no_language_uni,
	mbfl_no_language_min,
	mbfl_no_language_catalan,		/* ca */
	mbfl_no_language_danish,		/* da */
	mbfl_no_language_german,		/* de */
	mbfl_no_language_english,		/* en */
	mbfl_no_language_estonian,		/* et */
	mbfl_no_language_greek,			/* el */
	mbfl_no_language_spanish,		/* es */
	mbfl_no_language_french,		/* fr */
	mbfl_no_language_italian,		/* it */
	mbfl_no_language_japanese,		/* ja */
	mbfl_no_language_korean,		/* ko */
	mbfl_no_language_dutch,			/* nl */
	mbfl_no_language_polish,		/* pl */
	mbfl_no_language_portuguese,	        /* pt */
	mbfl_no_language_swedish,		/* sv */
	mbfl_no_language_simplified_chinese,		/* zh-cn */
	mbfl_no_language_traditional_chinese,		/* zh-tw */
	mbfl_no_language_russian,		/* ru */
	mbfl_no_language_ukrainian,		/* ua */
	mbfl_no_language_armenian,		/* hy */
	mbfl_no_language_turkish,		/* tr */
	mbfl_no_language_max
};

typedef enum mbfl_no_language mbfl_language_id;

/*
 * language
 */
typedef struct _mbfl_language {
	enum mbfl_no_language no_language;
	const char *name;
	const char *short_name;
	const char *(*aliases)[];
	enum mbfl_no_encoding mail_charset;
	enum mbfl_no_encoding mail_header_encoding;
	enum mbfl_no_encoding mail_body_encoding;
} mbfl_language;

MBFLAPI extern const mbfl_language * mbfl_name2language(const char *name);
MBFLAPI extern const mbfl_language * mbfl_no2language(enum mbfl_no_language no_language);
MBFLAPI extern enum mbfl_no_language mbfl_name2no_language(const char *name);
MBFLAPI extern const char * mbfl_no_language2name(enum mbfl_no_language no_language);


#endif /* MBFL_LANGUAGE_H */
