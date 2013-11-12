/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2013 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Felipe Pena <felipe@php.net>                                |
   | Authors: Joe Watkins <joe.watkins@live.co.uk>                        |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "phpdbg_list.h"

void phpdbg_list_file(const char *filename, long count, long offset) /* {{{ */
{
	unsigned char *mem, *pos, *last_pos, *end_pos;
	struct stat st;
	int fd, all_content = (count == 0);
	unsigned int line = 0, displayed = 0;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		printf("[Failed to open file %s to list]\n", filename);
		return;
	}

	if (fstat(fd, &st) == -1) {
		printf("[Failed to stat file %s]\n", filename);
		goto out;
	}

	last_pos = mem = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	end_pos = mem + st.st_size;

	while (1) {
		pos = memchr(last_pos, '\n', end_pos - last_pos);

		if (!pos) {
			/* No more line breaks */
			break;
		}

		++line;

		if (!offset || offset <= line) {
			/* Without offset, or offset reached */
			printf("%05u: %.*s\n", line, (int)(pos - last_pos), last_pos);
			++displayed;
		}

		last_pos = pos + 1;

		if (!all_content && displayed == count) {
			/* Reached max line to display */
			break;
		}
	}

	munmap(mem, st.st_size);
out:
	close(fd);
} /* }}} */
