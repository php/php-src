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
   | Authors:                                                             |
   |                                                                      |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

#include "php.h"
#include "php3_filestat.h"
#include "php_globals.h"

#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <string.h>
#if HAVE_PWD_H
#if MSVC5
#include "win32/pwd.h"
#else
#include <pwd.h>
#endif
#endif
#if HAVE_GRP_H
#if MSVC5
#include "win32/grp.h"
#else
#include <grp.h>
#endif
#endif
#include <errno.h>
#include <ctype.h>

#include "safe_mode.h"
#include "php3_link.h"

/* {{{ proto string readlink(string filename)
   Return the target of a symbolic link */
PHP_FUNCTION(readlink)
{
#if HAVE_SYMLINK
	pval *filename;
	char buff[256];
	int ret;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	ret = readlink(filename->value.str.val, buff, 255);
	if (ret == -1) {
		php3_error(E_WARNING, "readlink failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	/* Append NULL to the end of the string */
	buff[ret] = '\0';
	RETURN_STRING(buff,1);
#endif
}
/* }}} */

/* {{{ proto int linkinfo(string filename)
   Returns the st_dev field of the UNIX C stat structure describing the link */
PHP_FUNCTION(linkinfo)
{
#if HAVE_SYMLINK
	pval *filename;
	struct stat sb;
	int ret;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	ret = lstat(filename->value.str.val, &sb);
	if (ret == -1) {
		php3_error(E_WARNING, "LinkInfo failed (%s)", strerror(errno));
		RETURN_LONG(-1L);
	}
	RETURN_LONG((long) sb.st_dev);
#endif
}
/* }}} */

/* {{{ proto int symlink(string target, string link)
   Create a symbolic link */
PHP_FUNCTION(symlink)
{
#if HAVE_SYMLINK
	pval *topath, *frompath;
	int ret;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &topath, &frompath) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(topath);
	convert_to_string(frompath);

	if (PG(safe_mode) && !_php3_checkuid(topath->value.str.val, 2)) {
		RETURN_FALSE;
	}

	ret = symlink(topath->value.str.val, frompath->value.str.val);
	if (ret == -1) {
		php3_error(E_WARNING, "SymLink failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#endif
}
/* }}} */

/* {{{ proto int link(string target, string link)
   Create a hard link */
PHP_FUNCTION(link)
{
#if HAVE_LINK
	pval *topath, *frompath;
	int ret;

	if (ARG_COUNT(ht) != 2 || getParameters(ht, 2, &topath, &frompath) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(topath);
	convert_to_string(frompath);

	if (PG(safe_mode) && !_php3_checkuid(topath->value.str.val, 2)) {
		RETURN_FALSE;
	}

	ret = link(topath->value.str.val, frompath->value.str.val);
	if (ret == -1) {
		php3_error(E_WARNING, "Link failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	RETURN_TRUE;
#endif
}
/* }}} */

/* {{{ proto int unlink(string filename)
   Delete a file */
PHP_FUNCTION(unlink)
{
	pval *filename;
	int ret;
	PLS_FETCH();
	
	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &filename) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string(filename);

	if (PG(safe_mode) && !_php3_checkuid(filename->value.str.val, 2)) {
		RETURN_FALSE;
	}

	ret = unlink(filename->value.str.val);
	if (ret == -1) {
		php3_error(E_WARNING, "Unlink failed (%s)", strerror(errno));
		RETURN_FALSE;
	}
	/* Clear stat cache */
	php3_clearstatcache(INTERNAL_FUNCTION_PARAM_PASSTHRU);
	RETURN_TRUE;
}
/* }}} */

function_entry link_functions[] = {
	{"readlink",		php3_readlink,		NULL},
	{"linkinfo",		php3_linkinfo,		NULL},
	{"symlink",			php3_symlink,		NULL},
	{"link",			php3_link,			NULL},
	{"unlink",			php3_unlink,		NULL},
	{NULL, NULL, NULL}
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
