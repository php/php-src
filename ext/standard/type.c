/*
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of one of the following licenses:                 |
   |                                                                      |
   |  A) the GNU General Public License as published by the Free Software |
   |     Foundation; either version 2 of the License, or (at your option) |
   |     any later version.                                               |
   |                                                                      |
   |  B) the PHP License as published by the PHP Development Team and     |
   |     included in the distribution in the file: LICENSE                |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of both licenses referred to here.   |
   | If you did not, or have any questions about PHP licensing, please    |
   | contact core@php.net.                                                |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf                                              |
   |                                                                      |
   +----------------------------------------------------------------------+
 */
/* $Id$ */
#include "php.h"
#include "type.h"

/*
 * Determines if 'str' is an integer (long), real number or a string
 *
 * Note that leading zeroes automatically force a STRING type
 */
int php3_check_type(char *str)
{
	char *s;
	int type = IS_LONG;

	s = str;
	if (*s == '0' && *(s + 1) && *(s + 1) != '.')
		return (IS_STRING);
	if (*s == '+' || *s == '-' || (*s >= '0' && *s <= '9') || *s == '.') {
		if (*s == '.')
			type = IS_DOUBLE;
		s++;
		while (*s) {
			if (*s >= '0' && *s <= '9') {
				s++;
				continue;
			} else if (*s == '.' && type == IS_LONG) {
				type = IS_DOUBLE;
				s++;
				continue;
			} else
				return (IS_STRING);
		}
	} else
		return (IS_STRING);

	return (type);
}								/* php3_check_type */

/*
 * 0 - simple variable
 * 1 - non-index array
 * 2 - index array
 */
int php3_check_ident_type(char *str)
{
	char *s;

	if (!(s = (char *) strchr(str, '[')))
		return (GPC_REGULAR);
	s++;
	while (*s == ' ' || *s == '\t' || *s == '\n') {
		s++;
	}
	if (*s == ']') {
		return (GPC_NON_INDEXED_ARRAY);
	}
	return (GPC_INDEXED_ARRAY);
}

char *php3_get_ident_index(char *str)
{
	char *temp;
	char *s, *t;
	char o;

	temp = emalloc(strlen(str));
	temp[0] = '\0';
	s = (char *) strchr(str, '[');
	if (s) {
		t = (char *) strrchr(str, ']');
		if (t) {
			o = *t;
			*t = '\0';
			strcpy(temp, s + 1);
			*t = o;
		}
	}
	return (temp);
}
