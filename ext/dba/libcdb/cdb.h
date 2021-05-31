/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* incorporated from D.J.Bernstein's cdb-0.75 (http://cr.yp.to/cdb.html)*/

#ifndef CDB_H
#define CDB_H

#include "uint32.h"

#define CDB_HASHSTART 5381

struct cdb {
	php_stream *fp;
	uint32 loop; /* number of hash slots searched under this key */
	uint32 khash; /* initialized if loop is nonzero */
	uint32 kpos; /* initialized if loop is nonzero */
	uint32 hpos; /* initialized if loop is nonzero */
	uint32 hslots; /* initialized if loop is nonzero */
	uint32 dpos; /* initialized if cdb_findnext() returns 1 */
	uint32 dlen; /* initialized if cdb_findnext() returns 1 */
};

uint32 cdb_hash(char *, unsigned int);

void cdb_free(struct cdb *);
void cdb_init(struct cdb *, php_stream *fp);

int cdb_read(struct cdb *, char *, unsigned int, uint32);

void cdb_findstart(struct cdb *);
int cdb_findnext(struct cdb *, char *, unsigned int);
int cdb_find(struct cdb *, char *, unsigned int);

#define cdb_datapos(c) ((c)->dpos)
#define cdb_datalen(c) ((c)->dlen)

char *cdb_version(void);

#endif
