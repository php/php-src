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
   +----------------------------------------------------------------------+
 */
#ifndef HEAD_H
#define HEAD_H


/* 
   We are still using a PHP2-style Push/Pop list here as opposed
   to the PHP built-in list functionality because of the nature
   of this particular list.  It is just used as a structured
   buffer.  Doing this with the built-in list code would require
   some changes to allow a search for the first item with a
   certain type.  This type of search would not be optimal.
   Private list management makes more sense here
*/
typedef struct CookieList {
	char *name;
	char *value;
	time_t expires;
	char *path;
	char *domain;
	int secure;
	struct CookieList *next;
} CookieList;

extern PHP_RINIT_FUNCTION(head);
PHP_FUNCTION(Header);
PHP_FUNCTION(setcookie);
PHP_FUNCTION(headers_sent);

PHPAPI int php_header(void);
int php_headers_unsent(void);

#endif
