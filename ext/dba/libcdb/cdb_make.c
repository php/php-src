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

/* $Id$ */

/* incorporated from D.J.Bernstein's cdb-0.75 (http://cr.yp.to/cdb.html)*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "cdb.h"
#include "cdb_make.h"
#include "uint32.h"

/* {{{ cdb_make_write */
static int cdb_make_write(struct cdb_make *c, char *buf, uint32 sz) {
	return php_stream_write(c->fp, buf, sz) == sz ? 0 : -1;
}

/* {{{ cdb_posplus */
static int cdb_posplus(struct cdb_make *c, uint32 len)
{
	uint32 newpos = c->pos + len;
	if (newpos < len) {
		errno = ENOMEM;
		return -1;
	}
	c->pos = newpos;
	return 0;
}
/* }}} */

/* {{{ cdb_make_start */
int cdb_make_start(struct cdb_make *c, php_stream * f)
{
	c->head = 0;
	c->split = 0;
	c->hash = 0;
	c->numentries = 0;
	c->fp = f;
	c->pos = sizeof(c->final);
	if (php_stream_seek(f, c->pos, SEEK_SET) == -1) {
		php_error_docref(NULL, E_NOTICE, "Fseek failed");
		return -1;
	}
	return php_stream_tell(c->fp);
}
/* }}} */

/* {{{ cdb_make_addend */
int cdb_make_addend(struct cdb_make *c, unsigned int keylen, unsigned int datalen, uint32 h)
{
	struct cdb_hplist *head;

	head = c->head;
	if (!head || (head->num >= CDB_HPLIST)) {
		head = (struct cdb_hplist *) emalloc(sizeof(struct cdb_hplist));
		if (!head)
			return -1;
		head->num = 0;
		head->next = c->head;
		c->head = head;
	}
	head->hp[head->num].h = h;
	head->hp[head->num].p = c->pos;
	++head->num;
	++c->numentries;
	if (cdb_posplus(c,8) == -1)
		return -1;
	if (cdb_posplus(c, keylen) == -1)
		return -1;
	if (cdb_posplus(c, datalen) == -1)
		return -1;
	return 0;
}
/* }}} */

/* {{{ cdb_make_addbegin */
int cdb_make_addbegin(struct cdb_make *c, unsigned int keylen, unsigned int datalen)
{
	char buf[8];

	if (keylen > 0xffffffff) {
		errno = ENOMEM;
		return -1;
	}
	if (datalen > 0xffffffff) {
		errno = ENOMEM;
		return -1;
	}

	uint32_pack(buf, keylen);
	uint32_pack(buf + 4, datalen);
	if (cdb_make_write(c, buf, 8) != 0)
		return -1;
	return 0;
}

/* {{{ cdb_make_add */
int cdb_make_add(struct cdb_make *c,char *key,unsigned int keylen,char *data,unsigned int datalen)
{
	if (cdb_make_addbegin(c, keylen, datalen) == -1)
		return -1;
	if (cdb_make_write(c, key, keylen) != 0)
		return -1;
	if (cdb_make_write(c, data, datalen) != 0)
		return -1;
	return cdb_make_addend(c, keylen, datalen, cdb_hash(key, keylen));
}
/* }}} */

/* {{{ cdb_make_finish */
int cdb_make_finish(struct cdb_make *c)
{
	char buf[8];
	int i;
	uint32 len;
	uint32 u;
	uint32 memsize;
	uint32 count;
	uint32 where;
	struct cdb_hplist *x;
	struct cdb_hp *hp;

	for (i = 0;i < 256;++i)
	c->count[i] = 0;

	for (x = c->head; x; x = x->next) {
		i = x->num;
		while (i--)
		++c->count[255 & x->hp[i].h];
	}

	memsize = 1;
	for (i = 0;i < 256;++i) {
		u = c->count[i] * 2;
		if (u > memsize)
		memsize = u;
	}

	memsize += c->numentries; /* no overflow possible up to now */
	u = (uint32) 0 - (uint32) 1;
	u /= sizeof(struct cdb_hp);
	if (memsize > u) {
		errno = ENOMEM;
		return -1;
	}

	c->split = (struct cdb_hp *) safe_emalloc(memsize, sizeof(struct cdb_hp), 0);
	if (!c->split)
		return -1;

	c->hash = c->split + c->numentries;

	u = 0;
	for (i = 0;i < 256;++i) {
		u += c->count[i]; /* bounded by numentries, so no overflow */
		c->start[i] = u;
	}

	for (x = c->head; x; x = x->next) {
		i = x->num;
		while (i--)
		c->split[--c->start[255 & x->hp[i].h]] = x->hp[i];
	}

	for (i = 0;i < 256;++i) {
		count = c->count[i];

		len = count + count; /* no overflow possible */
		uint32_pack(c->final + 8 * i,c->pos);
		uint32_pack(c->final + 8 * i + 4,len);

		for (u = 0;u < len;++u)
			c->hash[u].h = c->hash[u].p = 0;

		hp = c->split + c->start[i];
		for (u = 0;u < count;++u) {
			where = (hp->h >> 8) % len;
			while (c->hash[where].p)
				if (++where == len)
					where = 0;
			c->hash[where] = *hp++;
		}

		for (u = 0;u < len;++u) {
			uint32_pack(buf, c->hash[u].h);
			uint32_pack(buf + 4, c->hash[u].p);
			if (cdb_make_write(c, buf, 8) != 0)
				return -1;
			if (cdb_posplus(c, 8) == -1)
				return -1;
		}
	}

	if (c->split)
		efree(c->split);

	for (x = c->head; x; c->head = x) {
		x = x->next;
		efree(c->head);
	}

	if (php_stream_flush(c->fp) != 0)
		return -1;
	php_stream_rewind(c->fp);
	if (php_stream_tell(c->fp) != 0)
		return -1;
	if (cdb_make_write(c, c->final, sizeof(c->final)) != 0)
		return -1;
	return php_stream_flush(c->fp);
}
/* }}} */

/* {{{ cdb_make_version */
char *cdb_make_version(void)
{
	return "0.75, $Id$";
}
