/*
   +----------------------------------------------------------------------+
   | Copyright © The PHP Group and Contributors.                          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_LIB_FLATFILE_H
#define PHP_LIB_FLATFILE_H

typedef struct {
	char *dptr;
	size_t dsize;
} datum;

typedef struct {
	char *lockfn;
	int lockfd;
	php_stream *fp;
	size_t CurrentFlatFilePos;
	datum nextkey;
} flatfile;

#define FLATFILE_INSERT 1
#define FLATFILE_REPLACE 0

int flatfile_store(flatfile *dba, datum key_datum, datum value_datum, int mode);
datum flatfile_fetch(flatfile *dba, datum key_datum);
int flatfile_delete(flatfile *dba, datum key_datum);
int flatfile_findkey(flatfile *dba, datum key_datum);
datum flatfile_firstkey(flatfile *dba);
datum flatfile_nextkey(flatfile *dba);
const char *flatfile_version(void);

#endif
