/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

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
