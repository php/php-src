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

PHP_FUNCTION(strlen);
PHP_FUNCTION(strcmp);
PHP_FUNCTION(strspn);
PHP_FUNCTION(strcspn);
PHP_FUNCTION(strcasecmp);
PHP_FUNCTION(str_replace);
PHP_FUNCTION(chop);
PHP_FUNCTION(trim);
PHP_FUNCTION(ltrim);
void soundex(INTERNAL_FUNCTION_PARAMETERS);

PHP_FUNCTION(explode);
PHP_FUNCTION(implode);
PHP_FUNCTION(strtok);
PHP_FUNCTION(strtoupper);
PHP_FUNCTION(strtolower);
PHP_FUNCTION(basename);
PHP_FUNCTION(dirname);
PHP_FUNCTION(strstr);
PHP_FUNCTION(strpos);
PHP_FUNCTION(strrpos);
PHP_FUNCTION(strrchr);
PHP_FUNCTION(substr);
PHP_FUNCTION(quotemeta);
PHP_FUNCTION(ucfirst);
PHP_FUNCTION(ucwords);
PHP_FUNCTION(strtr);
PHP_FUNCTION(strrev);
PHP_FUNCTION(hebrev);
PHP_FUNCTION(hebrev_with_conversion);
PHP_FUNCTION(user_sprintf);
PHP_FUNCTION(user_printf);
PHP_FUNCTION(addslashes);
PHP_FUNCTION(stripslashes);
PHP_FUNCTION(chr);
PHP_FUNCTION(ord);
PHP_FUNCTION(newline_to_br);
PHP_FUNCTION(setlocale);
PHP_FUNCTION(stristr);
PHP_FUNCTION(chunk_split);
PHP_FUNCTION(parsestr);
PHP_FUNCTION(bin2hex);

extern PHPAPI char *_php3_strtoupper(char *s);
extern PHPAPI char *_php3_strtolower(char *s);
extern char *_StrTr(char *string, char *str_from, char *str_to);
extern PHPAPI char *_php3_addslashes(char *string, int length, int *new_length, int freeit);
extern PHPAPI void _php3_stripslashes(char *string, int *len);
extern PHPAPI void _php3_dirname(char *str, int len);
extern PHPAPI char *php3i_stristr(unsigned char *s, unsigned char *t);

#endif /* _PHPSTRING_H */
