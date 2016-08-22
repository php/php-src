/*
   +----------------------------------------------------------------------+
   | Zend JIT                                                             |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Dmitry Stogov <dmitry@zend.com>                             |
   +----------------------------------------------------------------------+
*/

#define HAVE_PERFTOOLS 1

#include <stdio.h>
#include <unistd.h>

static void zend_jit_perf_dump(const char *name, void *start, size_t size)
{
	static FILE *fp = NULL;

	if (!fp) {
		char filename[64];

		sprintf(filename, "/tmp/perf-%d.map", getpid());
		fp = fopen(filename, "w");
		if (!fp) {
			return;
		}
	    setlinebuf(fp);
	}
	fprintf(fp, "%lx %lx %s\n", (size_t)start, size, name);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
