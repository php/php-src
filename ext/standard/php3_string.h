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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   |          Stig Sæther Bakken <ssb@guardian.no>                        |
   +----------------------------------------------------------------------+
 */


/* $Id$ */

#ifndef _PHPSTRING_H
#define _PHPSTRING_H

#ifndef THREAD_SAFE
extern char *strtok_string;
#endif

extern void php3_strlen(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcmp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strspn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcspn(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strcasecmp(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_str_replace(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chop(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_trim(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ltrim(INTERNAL_FUNCTION_PARAMETERS);
extern void soundex(INTERNAL_FUNCTION_PARAMETERS);

extern void php3_explode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_implode(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtok(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtoupper(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtolower(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_basename(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_dirname(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strstr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strpos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrpos(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrchr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_substr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_quotemeta(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ucfirst(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ucwords(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strtr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_strrev(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hebrev(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_hebrev_with_conversion(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_sprintf(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_user_printf(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_addslashes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_stripslashes(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_ord(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_newline_to_br(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_setlocale(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_stristr(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_chunk_split(INTERNAL_FUNCTION_PARAMETERS);
extern void php3_parsestr(INTERNAL_FUNCTION_PARAMETERS);

extern PHPAPI char *_php3_strtoupper(char *s);
extern PHPAPI char *_php3_strtolower(char *s);
extern char *_StrTr(char *string, char *str_from, char *str_to);
extern PHPAPI char *_php3_addslashes(char *string, int length, int *new_length, int freeit);
extern PHPAPI void _php3_stripslashes(char *string, int *len);
extern PHPAPI void _php3_dirname(char *str, int len);
extern PHPAPI char *php3i_stristr(unsigned char *s, unsigned char *t);

#endif /* _PHPSTRING_H */
