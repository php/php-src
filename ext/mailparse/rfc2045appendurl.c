/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors:                                                             |
   | Wez Furlong (wez@thebrainroom.com)                                   |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

/*
** Copyright 2000 Double Precision, Inc.  See COPYING for
** distribution information.
*/

#include "php.h"
#include "php_mailparse.h"

/*
** ---------------------------------------------------------------------
** Attempt to parse Content-Base: and Content-Location:, and return the
** "base" of all the relative URLs in the section.
** ---------------------------------------------------------------------
*/

/* {{{ get_method_path
 */
static void get_method_path(const char *p,
	const char **method,
	unsigned *methodl,
	const char **path)
{
	unsigned	i;

	for (i=0; p && p[i]; i++)
	{
		if (p[i] == ':')
		{
			*method=p;
			*methodl= ++i;
			*path=p+i;
			return;
		}

		if (!isalpha( (int)(unsigned char)p[i]))
			break;
	}

	*method=0;
	*methodl=0;
	*path=p;
}
/* }}} */

/* {{{ rfc2045_append_url
 */
char *rfc2045_append_url(const char *base, const char *loc)
{
	const char *base_method;
	unsigned base_method_l;
	const char *base_path;

	const char *loc_method;
	unsigned loc_method_l;
	const char *loc_path;
	char *buf, *q;

	get_method_path(base, &base_method, &base_method_l, &base_path);
	get_method_path(loc, &loc_method, &loc_method_l, &loc_path);

	if (loc_method_l)
	{
		buf = emalloc(strlen(loc)+1);
		strcpy(buf, loc);
		return (buf);
	}

	loc_method = base_method;
	loc_method_l = base_method_l;

	if (!base_path)	base_path = "";
	if (!loc_path)	loc_path = "";

	buf = emalloc(loc_method_l + strlen(base_path)+strlen(loc_path) + 3);

	if (loc_method_l)
		memcpy(buf, loc_method, loc_method_l);
	buf[loc_method_l] = 0;

	q=buf + loc_method_l;

	strcat(strcpy(q, base_path), "/");

	if ( loc_path[0] == '/')
	{
		char *r;

		if (loc_path[1] == '/')	{
			*q=0; /* Location is absolute */
		}
		else if ( q[0] == '/' && q[1] == '/' && (r=strchr(q+2, '/')) != 0)	{
			*r=0; /* Relative to top of base */
		}
		else	{
			*q=0;	/* No sys in base, just start with / */
		}
	}

	strcat(q, loc_path);

	return (buf);
}
/* }}} */

char *rfc2045_content_base(struct rfc2045 *p)
{
    return (rfc2045_append_url(p->content_base, p->content_location));
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
