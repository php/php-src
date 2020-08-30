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

#include "libmbfl/config.h"

#include <stddef.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "mbfl_encoding.h"
#include "mbfl_language.h"

#include "nls/nls_ja.h"
#include "nls/nls_kr.h"
#include "nls/nls_zh.h"
#include "nls/nls_uni.h"
#include "nls/nls_de.h"
#include "nls/nls_ru.h"
#include "nls/nls_ua.h"
#include "nls/nls_en.h"
#include "nls/nls_hy.h"
#include "nls/nls_tr.h"
#include "nls/nls_neutral.h"

#ifndef HAVE_STRCASECMP
#ifdef HAVE_STRICMP
#define strcasecmp stricmp
#endif
#endif

static const mbfl_language *mbfl_language_ptr_table[] = {
	&mbfl_language_uni,
	&mbfl_language_japanese,
	&mbfl_language_korean,
	&mbfl_language_simplified_chinese,
	&mbfl_language_traditional_chinese,
	&mbfl_language_english,
	&mbfl_language_german,
	&mbfl_language_russian,
	&mbfl_language_ukrainian,
	&mbfl_language_armenian,
	&mbfl_language_turkish,
	&mbfl_language_neutral,
	NULL
};

/* language resolver */
const mbfl_language *mbfl_name2language(const char *name)
{
	const mbfl_language **language;

	if (name == NULL) {
		return NULL;
	}

	for (language = mbfl_language_ptr_table; *language; language++) {
		if (strcasecmp((*language)->name, name) == 0) {
			return *language;
		}
	}

	for (language = mbfl_language_ptr_table; *language; language++) {
		if (strcasecmp((*language)->short_name, name) == 0) {
			return *language;
		}
	}

	/* search aliases */
	for (language = mbfl_language_ptr_table; *language; language++) {
		if ((*language)->aliases) {
			for (const char **alias = (*language)->aliases; *alias; alias++) {
				if (strcasecmp(*alias, name) == 0) {
					return *language;
				}
			}
		}
	}

	return NULL;
}

const mbfl_language *mbfl_no2language(enum mbfl_no_language no_language)
{
	const mbfl_language **language;

	for (language = mbfl_language_ptr_table; *language; language++) {
		if ((*language)->no_language == no_language) {
			return *language;
		}
	}

	return NULL;
}

enum mbfl_no_language mbfl_name2no_language(const char *name)
{
	const mbfl_language *language = mbfl_name2language(name);
	return (language == NULL) ? mbfl_no_language_invalid : language->no_language;
}

const char *mbfl_no_language2name(enum mbfl_no_language no_language)
{
	const mbfl_language *language = mbfl_no2language(no_language);
	return (language == NULL) ? "" : language->name;
}
