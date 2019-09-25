/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* incorporated from D.J.Bernstein's cdb-0.75 (http://cr.yp.to/cdb.html)*/

#ifndef CDB_MAKE_H
#define CDB_MAKE_H

#include <stdio.h>
#include "uint32.h"

#define CDB_HPLIST 1000

struct cdb_hp {
	uint32 h;
	uint32 p;
};

struct cdb_hplist {
	struct cdb_hp hp[CDB_HPLIST];
	struct cdb_hplist *next;
	int num;
} ;

struct cdb_make {
	/* char bspace[8192]; */
	char final[2048];
	uint32 count[256];
	uint32 start[256];
	struct cdb_hplist *head;
	struct cdb_hp *split; /* includes space for hash */
	struct cdb_hp *hash;
	uint32 numentries;
	/* buffer b; */
	uint32 pos;
	/* int fd; */
	php_stream * fp;
};

int cdb_make_start(struct cdb_make *, php_stream *);
int cdb_make_addbegin(struct cdb_make *, unsigned int, unsigned int);
int cdb_make_addend(struct cdb_make *, unsigned int, unsigned int, uint32);
int cdb_make_add(struct cdb_make *, char *, unsigned int, char *, unsigned int);
int cdb_make_finish(struct cdb_make *);
char *cdb_make_version();

#endif
