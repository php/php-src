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
   | Author: Go Kudo <zeriyoshi@gmail.com>                                |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_RANDOM_EXT_H
#define PHP_RANDOM_EXT_H

extern zend_module_entry random_module_entry;
#define phpext_random_ext_ptr &random_module_entry

extern PHPAPI zend_class_entry *random_ce_Random_NumberGenerator;
extern PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_XorShift128Plus;
extern PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_MT19937;
extern PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_Secure;
extern PHPAPI zend_class_entry *random_ce_Random;

typedef struct _php_random_rng_algo {
	const size_t generate_size;
	const size_t state_size;
	uint64_t (*generate)(void *state);
	void (*seed)(void *state, const zend_long seed); /* nullable */
	int (*serialize)(void *state, HashTable *data); /* nullable */
	int (*unserialize)(void *state, HashTable *data); /* nullable */
} php_random_rng_algo;

typedef struct _php_random_rng {
	const php_random_rng_algo *algo;
  void *state;
	zend_object std;
} php_random_rng;

typedef struct _php_random {
	php_random_rng *rng;
	zend_object std;
} php_random;

static inline php_random_rng *php_random_rng_from_obj(zend_object *obj) {
	return (php_random_rng *)((char *)(obj) - XtOffsetOf(php_random_rng, std));
}

static inline php_random *php_random_from_obj(zend_object *obj) {
	return (php_random *)((char *)(obj) - XtOffsetOf(php_random, std));
}

#define Z_RANDOM_RNG_P(zval) php_random_rng_from_obj(Z_OBJ_P(zval))

#define Z_RANDOM_P(zval) php_random_from_obj(Z_OBJ_P(zval))

PHPAPI uint64_t php_random_next(php_random *random);
PHPAPI zend_long php_random_range(php_random *random, zend_long min, zend_long max);
PHPAPI void php_random_array_data_shuffle(php_random *random, zval *array);
PHPAPI void php_random_string_shuffle(php_random *random, char *str, zend_long len);

#endif	/* PHP_RANDOM_EXT_H */
