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

/* maximum number of files that will fit into a phar */
#define PHAR2_HASHSIZE_SMALL 997
#define PHAR2_HASHSIZE 4999

typedef struct _phar2_openhash_entry {
	char[20] filepart; /* last 10 characters of filename */
	long    offset;   /* offset of this file's info in the manifest */
	ulong    zhash;    /* value of zend_inline_hash_func() on the filename */
} phar2_openhash_entry;

typedef struct _phar2_openhash {
	char                 is_large; /* always 0 */
	int                  count;
	phar2_openhash_entry info[PHAR2_HASHSIZE_SMALL];
} phar2_openhash;

typedef struct _phar2_openhash {
	char                 is_large; /* always 1 */
	int                  count;
	phar2_openhash_entry info[PHAR2_HASHSIZE];
} phar2_openhash_large;

BEGIN_EXTERN_C()

phar2_openhash *phar2_openhash_init(int large);
int phar2_openhash_destroy(phar2_openhash *hash);
long phar2_openhash_getoffset(char *fname, int len);
int phar2_openhash_insert(phar2_openhash *hash, char* fname, int len, long offset);

END_EXTERN_C()

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
