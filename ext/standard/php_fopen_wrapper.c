/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Jim Winstead <jimw@php.net>                                 |
   |          Hartmut Holzgraefe <hholzgra@php.net>                       |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#include <stdio.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "php.h"
#include "php_globals.h"
#include "php_standard.h"
#include "php_fopen_wrappers.h"

php_stream * php_stream_url_wrap_php(char * path, char * mode, int options, char ** opened_path STREAMS_DC TSRMLS_DC)
{
	FILE * fp = NULL;
	php_stream * stream = NULL;
	
	if (!strcasecmp(path, "stdin")) {
		fp = fdopen(dup(STDIN_FILENO), mode);
	} else if (!strcasecmp(path, "stdout")) {
		fp = fdopen(dup(STDOUT_FILENO), mode);
	} else if (!strcasecmp(path, "stderr")) {
		fp = fdopen(dup(STDERR_FILENO), mode);
	}
	/* TODO: implement php://output as a stream to write to the current output buffer ? */

	if (fp)	{
		stream = php_stream_fopen_from_file(fp, mode);
		if (stream == NULL)
			fclose(fp);
	}
	return stream;
}

php_stream_wrapper php_stream_php_wrapper =	{
	php_stream_url_wrap_php,
	NULL
};


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
