/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Marcus Boerger <helly@php.net>                               |
   +----------------------------------------------------------------------+
 */

/* $Id$ */

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

void cdb_free(struct cdb * TSRMLS_DC);
void cdb_init(struct cdb *, php_stream *fp TSRMLS_DC);

int cdb_read(struct cdb *, char *, unsigned int, uint32 TSRMLS_DC);

void cdb_findstart(struct cdb * TSRMLS_DC);
int cdb_findnext(struct cdb *, char *, unsigned int TSRMLS_DC);
int cdb_find(struct cdb *, char *, unsigned int TSRMLS_DC);

#define cdb_datapos(c) ((c)->dpos)
#define cdb_datalen(c) ((c)->dlen)

char *cdb_version();

#endif
