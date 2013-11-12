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
#ifndef _WIN32
#	include <sys/mman.h>
#	include <unistd.h>
#endif
#include <fcntl.h>
#include "phpdbg.h"
#include "phpdbg_list.h"

void phpdbg_list_file(const char *filename, long count, long offset TSRMLS_DC) /* {{{ */
{
	unsigned char *mem, *pos, *last_pos, *end_pos;
	struct stat st;
#ifndef _WIN32
	int fd;
#else
	HANDLE fd, map;
#endif
	int all_content = (count == 0);
	unsigned int line = 0, displayed = 0;

    if (VCWD_STAT(filename, &st) == -1) {
		phpdbg_error("Failed to stat file %s", filename);
		return;
	}
#ifndef _WIN32
	if ((fd = VCWD_OPEN(filename, O_RDONLY)) == -1) {
		phpdbg_error("Failed to open file %s to list", filename);
		return;
	}

	last_pos = mem = mmap(0, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
	end_pos = mem + st.st_size;
#else
	fd = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (fd == INVALID_HANDLE_VALUE) {
		phpdbg_error("Failed to open file!");
		return;
	}

	map = CreateFileMapping(fd, NULL, PAGE_EXECUTE_READ, 0, 0, 0);
	if (map == NULL) {
		phpdbg_error("Failed to map file!");
		CloseHandle(fd);
		return;
	}

	last_pos = mem = (char*) MapViewOfFile(map, FILE_MAP_READ, 0, 0, 0);
	if (mem == NULL) {
		phpdbg_error("Failed to map file in memory");
		CloseHandle(map);
		CloseHandle(fd);
		return;
	}
	end_pos = mem + st.st_size;
#endif
	while (1) {
		pos = memchr(last_pos, '\n', end_pos - last_pos);

		if (!pos) {
			/* No more line breaks */
			break;
		}

		++line;

		if (!offset || offset <= line) {
			/* Without offset, or offset reached */
			phpdbg_writeln("%05u: %.*s", line, (int)(pos - last_pos), last_pos);
			++displayed;
		}

		last_pos = pos + 1;

		if (!all_content && displayed == count) {
			/* Reached max line to display */
			break;
		}
	}

#ifndef _WIN32
	munmap(mem, st.st_size);
	close(fd);
#else
	UnmapViewOfFile(mem);
	CloseHandle(map);
	CloseHandle(fd);
#endif
} /* }}} */

void phpdbg_list_function(const zend_function *fbc TSRMLS_DC) /* {{{ */
{
	const zend_op_array *ops;

	if (fbc->type != ZEND_USER_FUNCTION) {
		return;
	}

	ops = (zend_op_array*)fbc;

	phpdbg_list_file(ops->filename,
		ops->line_end - ops->line_start + 1, ops->line_start TSRMLS_CC);
} /* }}} */

