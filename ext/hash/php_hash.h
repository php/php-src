/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2005 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Sara Golemon <pollita@php.net>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_HASH_H
#define PHP_HASH_H

#include "php.h"

#define PHP_HASH_EXTNAME	"hash"
#define PHP_HASH_EXTVER		"0.1"
#define PHP_HASH_RESNAME	"Hash Context"

#define PHP_HASH_HMAC		0x0001

typedef struct _php_hash_ops {
	int (*hash_init)(void *context);
	int (*hash_update)(void *context, const unsigned char *buf, unsigned int count);
	int (*hash_final)(unsigned char *digest, void *context);

	int digest_size;
	int block_size;
	int context_size;
} php_hash_ops;

typedef struct _php_hash_data {
	php_hash_ops *ops;
	void *context;

	long options;
	unsigned char *key;
} php_hash_data;

extern php_hash_ops php_hash_md5_ops;
extern php_hash_ops php_hash_sha1_ops;
extern php_hash_ops php_hash_sha256_ops;
extern php_hash_ops php_hash_sha384_ops;
extern php_hash_ops php_hash_sha512_ops;
extern php_hash_ops php_hash_ripemd128_ops;
extern php_hash_ops php_hash_ripemd160_ops;

extern zend_module_entry hash_module_entry;
#define phpext_hash_ptr &hash_module_entry

#ifdef PHP_WIN32
#define PHP_HASH_API __declspec(dllexport)
#else
#define PHP_HASH_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#endif	/* PHP_HASH_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
