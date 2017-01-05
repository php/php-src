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
 * The author of this part: Marcus Boerger <helly@php.net>
 *
 */
/*
 * The source code included in this files was separated from mbfilter.h
 * by moriyoshi koizumi <moriyoshi@php.net> on 4 dec 2002.
 *
 */

#ifndef MBFL_HTML_ENTITIES_H
#define MBFL_HTML_ENTITIES_H

typedef struct _mbfl_html_entity_entry {
	char *  name;
	int     code;
} mbfl_html_entity_entry;

extern const mbfl_html_entity_entry mbfl_html_entity_list[];

#endif /* MBFL_HTML_ENTITIES_H */

