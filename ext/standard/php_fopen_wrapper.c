/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
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
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>

#include "php.h"
#include "php_globals.h"
#include "php_standard.h"
#include "php_fopen_wrappers.h"



FILE *php_fopen_url_wrap_php(char *path, char *mode, int options, int *issock, int *socketd, char **opened_path)
{
	const char *res = path + 6;

	*issock = 0;
	
	if (!strcasecmp(res, "stdin")) {
		return fdopen(STDIN_FILENO, mode);
	} else if (!strcasecmp(res, "stdout")) {
		return fdopen(STDOUT_FILENO, mode);
	} else if (!strcasecmp(res, "stderr")) {
		return fdopen(STDERR_FILENO, mode);
	}
	
	return NULL;
}
