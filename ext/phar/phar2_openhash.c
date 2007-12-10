/*
  +----------------------------------------------------------------------+
  | phar open hash implementation for phar2 tar-based archives           |
  +----------------------------------------------------------------------+
  | Copyright (c) 2006-2007 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt.                                 |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Gregory Beaver <cellog@php.net>                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#include "zend_hash.h"

static inline int phar2_openhash_doublehash(ulong k, int m, int i) /* {{{ */
{
	/* implement double hash probing */
	return ((k % m) + (i + (k % (m - 1)))) % m;
}
/* }}} */

phar2_openhash *phar2_openhash_init(int large) /* {{{ */
{
	phar2_openhash *hash;
	if (large) {
		hash = (phar2_openhash *) ecalloc(sizeof(phar2_openhash_large));
	} else {
		hash = (phar2_openhash *) ecalloc(sizeof(phar2_openhash));
		hash->is_large = '\001';
	}
	return hash;
}
/* }}} */

int phar2_openhash_destroy(phar2_openhash *hash) /* {{{ */
{
	return efree(hash);
}
/* }}} */

/* retrieve the offset of this file's manifest entry.  Returns offset or -1 */
long phar2_openhash_getoffset(phar2_openhash *hash, char *fname, int len) /* {{{ */
{
	int myhash, hashsize;
	char substr[20];

	memset((void *) substr, 0, 20);
	if (len < 20) {
		memcpy((void *) &substr, (void *) fname, len);
	} else {
		memcpy((void *) &substr, (void *) fname + (len - 20), 20);
	}
	hashsize = (hash->is_large ? PHAR2_HASHSIZE : PHAR2_HASHSIZE_SMALL);
	for (i = 0; i < hashsize; i ++) {
		myhash = phar2_openhash_doublehash(zhash, hashsize, i);
		if (hash->info[myhash].filepart[0]) {
			if (!memcmp(hash->info[myhash].filepart, substr, 20)) {
				/* found it */
				return hash->info[myhash].offset;
			}
		} else {
			/* empty - does not exist */
			return -1;
		}
	}
	return -1;
}
/* }}} */

static int phar2_openhash_insert_ex(phar2_openhash *hash, char* fname, ulong zhash, long offset) /* {{{ */
{
	int myhash, hashsize;
	hashsize = (hash->is_large ? PHAR2_HASHSIZE : PHAR2_HASHSIZE_SMALL);
	for (i = 0; i < hashsize; i ++) {
		myhash = phar2_openhash_doublehash(zhash, hashsize, i);
		if (hash->info[myhash].filepart[0]) {
			continue;
		} else {
			memcpy((void *) &hash->info[myhash].filepart, (void *) fname, 20);
			hash->info[myhash].offset = offset;
			hash->info[myhash].zhash = zhash;
			hash->count++;
			return SUCCESS;
		}
	}
	return FAILURE;
}
/* }}} */

/* a super-large phar is being created, enlarge the hash and re-populate it */
phar2_openhash_large *phar2_openhash_enlarge(phar2_openhash *hash) /* {{{ */
{
	phar_openhash_large *newhash = phar2_openhash_init(1);
	for (phar2_openhash_entry s = newhash->info, int i = 0; i < hash->count; i++) {
		if (!s[i].filepart[0]) {
			continue;
		}
		phar2_openhash_insert_ex((phar2_openhash *)newhash, &(s[i].filepart), s[i].zhash, s[i].offset);
	}
	phar2_openhash_destroy(hash);
	return newhash;
}
/* }}} */

int phar2_openhash_insert(phar2_openhash *hash, char* fname, int len, long offset) /* {{{ */
{
	ulong zhash = zend_inline_hash_func(string, len);
	char substr[20];

	if (-1 != phar2_openhash_getoffset(hash, fname, len)) {
		/* already exists, so fail */
		return FAILURE;
	}
	if (hash->is_large) {
		if (hash->count + 1 == PHAR_HASHSIZE) {
			return FAILURE;
		}
	} else {
		if (hash->count + 1 == PHAR_HASHSIZE_SMALL) {
			/* resize for mega-huge phars */
			hash = (phar2_openhash *) phar2_openhash_enlarge(hash);
		}
	}

	memset((void *) substr, 0, 20);
	if (len < 20) {
		memcpy((void *) &substr, (void *) fname, len);
	} else {
		memcpy((void *) &substr, (void *) fname + (len - 20), 20);
	}
	return phar2_openhash_insert_ex(hash, substr, zhash, offset);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
