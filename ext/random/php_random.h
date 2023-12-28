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
   | Authors: Rasmus Lerdorf <rasmus@php.net>                             |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Sascha Schumann <sascha@schumann.cx>                        |
   |          Pedro Melo <melo@ip.pt>                                     |
   |          Sterling Hughes <sterling@php.net>                          |
   |          Sammy Kaye Powers <me@sammyk.me>                            |
   |          Go Kudo <zeriyoshi@php.net>                                 |
   |                                                                      |
   | Based on code from: Richard J. Wagner <rjwagner@writeme.com>         |
   |                     Makoto Matsumoto <matumoto@math.keio.ac.jp>      |
   |                     Takuji Nishimura                                 |
   |                     Shawn Cokus <Cokus@math.washington.edu>          |
   |                     David Blackman                                   |
   |                     Sebastiano Vigna <vigna@acm.org>                 |
   |                     Melissa O'Neill <oneill@pcg-random.org>          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_RANDOM_H
# define PHP_RANDOM_H

# include "php.h"

PHPAPI double php_combined_lcg(void);

# ifdef PHP_WIN32
#  define GENERATE_SEED() (((zend_long) ((zend_ulong) time(NULL) * (zend_ulong) GetCurrentProcessId())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
# else
#  define GENERATE_SEED() (((zend_long) ((zend_ulong) time(NULL) * (zend_ulong) getpid())) ^ ((zend_long) (1000000.0 * php_combined_lcg())))
# endif

# define PHP_MT_RAND_MAX ((zend_long) (0x7FFFFFFF)) /* (1<<31) - 1 */

# define MT_RAND_MT19937 0
# define MT_RAND_PHP 1

# define MT_N (624)

#define PHP_RANDOM_RANGE_ATTEMPTS (50)

PHPAPI void php_mt_srand(uint32_t seed);
PHPAPI uint32_t php_mt_rand(void);
PHPAPI zend_long php_mt_rand_range(zend_long min, zend_long max);
PHPAPI zend_long php_mt_rand_common(zend_long min, zend_long max);

# ifndef RAND_MAX
#  define RAND_MAX PHP_MT_RAND_MAX
# endif

# define PHP_RAND_MAX PHP_MT_RAND_MAX

PHPAPI void php_srand(zend_long seed);
PHPAPI zend_long php_rand(void);

# if !defined(__SIZEOF_INT128__) || defined(PHP_RANDOM_FORCE_EMULATE_128)
typedef struct _php_random_uint128_t {
	uint64_t hi;
	uint64_t lo;
} php_random_uint128_t;

static inline uint64_t php_random_uint128_hi(php_random_uint128_t num)
{
	return num.hi;
}

static inline uint64_t php_random_uint128_lo(php_random_uint128_t num)
{
	return num.lo;
}

static inline php_random_uint128_t php_random_uint128_constant(uint64_t hi, uint64_t lo)
{
	php_random_uint128_t r;

	r.hi = hi;
	r.lo = lo;

	return r;
}

static inline php_random_uint128_t php_random_uint128_add(php_random_uint128_t num1, php_random_uint128_t num2)
{
	php_random_uint128_t r;

	r.lo = (num1.lo + num2.lo);
	r.hi = (num1.hi + num2.hi + (r.lo < num1.lo));

	return r;
}

static inline php_random_uint128_t php_random_uint128_multiply(php_random_uint128_t num1, php_random_uint128_t num2)
{
	php_random_uint128_t r;
	const uint64_t
		x0 = num1.lo & 0xffffffffULL,
		x1 = num1.lo >> 32,
		y0 = num2.lo & 0xffffffffULL,
		y1 = num2.lo >> 32,
		z0 = (((x1 * y0) + (x0 * y0 >> 32)) & 0xffffffffULL) + x0 * y1;

	r.hi = num1.hi * num2.lo + num1.lo * num2.hi;
	r.lo = num1.lo * num2.lo;
	r.hi += x1 * y1 + ((x1 * y0 + (x0 * y0 >> 32)) >> 32) + (z0 >> 32);

	return r;
}

static inline uint64_t php_random_pcgoneseq128xslrr64_rotr64(php_random_uint128_t num)
{
	const uint64_t
		v = (num.hi ^ num.lo),
		s = num.hi >> 58U;

	return (v >> s) | (v << ((-s) & 63));
}
# else
typedef __uint128_t php_random_uint128_t;

static inline uint64_t php_random_uint128_hi(php_random_uint128_t num)
{
	return (uint64_t) (num >> 64);
}

static inline uint64_t php_random_uint128_lo(php_random_uint128_t num)
{
	return (uint64_t) num;
}

static inline php_random_uint128_t php_random_uint128_constant(uint64_t hi, uint64_t lo)
{
	php_random_uint128_t r;

	r = ((php_random_uint128_t) hi << 64) + lo;

	return r;
}

static inline php_random_uint128_t php_random_uint128_add(php_random_uint128_t num1, php_random_uint128_t num2)
{
	return num1 + num2;
}

static inline php_random_uint128_t php_random_uint128_multiply(php_random_uint128_t num1, php_random_uint128_t num2)
{
	return num1 * num2;
}

static inline uint64_t php_random_pcgoneseq128xslrr64_rotr64(php_random_uint128_t num)
{
	const uint64_t
		v = ((uint64_t) (num >> 64U)) ^ (uint64_t) num,
		s = num >> 122U;

	return (v >> s) | (v << ((-s) & 63));
}
# endif

PHPAPI zend_result php_random_bytes(void *bytes, size_t size, bool should_throw);
PHPAPI zend_result php_random_int(zend_long min, zend_long max, zend_long *result, bool should_throw);

static inline zend_result php_random_bytes_throw(void *bytes, size_t size)
{
	return php_random_bytes(bytes, size, true);
}

static inline zend_result php_random_bytes_silent(void *bytes, size_t size)
{
	return php_random_bytes(bytes, size, false);
}

static inline zend_result php_random_int_throw(zend_long min, zend_long max, zend_long *result)
{
	return php_random_int(min, max, result, true);
}

static inline zend_result php_random_int_silent(zend_long min, zend_long max, zend_long *result)
{
	return php_random_int(min, max, result, false);
}

typedef struct _php_random_status_ {
	void *state;
} php_random_status;

typedef struct _php_random_status_state_combinedlcg {
	int32_t state[2];
} php_random_status_state_combinedlcg;

typedef struct _php_random_status_state_mt19937 {
	uint32_t state[MT_N];
	uint32_t count;
	uint8_t mode;
} php_random_status_state_mt19937;

typedef struct _php_random_status_state_pcgoneseq128xslrr64 {
	php_random_uint128_t state;
} php_random_status_state_pcgoneseq128xslrr64;

typedef struct _php_random_status_state_xoshiro256starstar {
	uint64_t state[4];
} php_random_status_state_xoshiro256starstar;

typedef struct _php_random_status_state_user {
	zend_object *object;
	zend_function *generate_method;
} php_random_status_state_user;

typedef struct _php_random_result {
	const uint64_t result;
	const size_t size;
} php_random_result;

typedef struct _php_random_algo {
	const size_t state_size;
	void (*seed)(php_random_status *status, uint64_t seed);
	php_random_result (*generate)(php_random_status *status);
	zend_long (*range)(php_random_status *status, zend_long min, zend_long max);
	bool (*serialize)(php_random_status *status, HashTable *data);
	bool (*unserialize)(php_random_status *status, HashTable *data);
} php_random_algo;

extern PHPAPI const php_random_algo php_random_algo_combinedlcg;
extern PHPAPI const php_random_algo php_random_algo_mt19937;
extern PHPAPI const php_random_algo php_random_algo_pcgoneseq128xslrr64;
extern PHPAPI const php_random_algo php_random_algo_xoshiro256starstar;
extern PHPAPI const php_random_algo php_random_algo_secure;
extern PHPAPI const php_random_algo php_random_algo_user;

typedef struct _php_random_engine {
	const php_random_algo *algo;
	php_random_status *status;
	zend_object std;
} php_random_engine;

typedef struct _php_random_randomizer {
	const php_random_algo *algo;
	php_random_status *status;
	bool is_userland_algo;
	zend_object std;
} php_random_randomizer;

extern PHPAPI zend_class_entry *random_ce_Random_Engine;
extern PHPAPI zend_class_entry *random_ce_Random_CryptoSafeEngine;

extern PHPAPI zend_class_entry *random_ce_Random_RandomError;
extern PHPAPI zend_class_entry *random_ce_Random_BrokenRandomEngineError;
extern PHPAPI zend_class_entry *random_ce_Random_RandomException;

extern PHPAPI zend_class_entry *random_ce_Random_Engine_PcgOneseq128XslRr64;
extern PHPAPI zend_class_entry *random_ce_Random_Engine_Mt19937;
extern PHPAPI zend_class_entry *random_ce_Random_Engine_Xoshiro256StarStar;
extern PHPAPI zend_class_entry *random_ce_Random_Engine_Secure;

extern PHPAPI zend_class_entry *random_ce_Random_Randomizer;

extern PHPAPI zend_class_entry *random_ce_Random_IntervalBoundary;

static inline php_random_engine *php_random_engine_from_obj(zend_object *object) {
	return (php_random_engine *)((char *)(object) - XtOffsetOf(php_random_engine, std));
}

static inline php_random_randomizer *php_random_randomizer_from_obj(zend_object *object) {
	return (php_random_randomizer *)((char *)(object) - XtOffsetOf(php_random_randomizer, std));
}

# define Z_RANDOM_ENGINE_P(zval) php_random_engine_from_obj(Z_OBJ_P(zval))

# define Z_RANDOM_RANDOMIZER_P(zval) php_random_randomizer_from_obj(Z_OBJ_P(zval));

PHPAPI php_random_status *php_random_status_alloc(const php_random_algo *algo, const bool persistent);
PHPAPI php_random_status *php_random_status_copy(const php_random_algo *algo, php_random_status *old_status, php_random_status *new_status);
PHPAPI void php_random_status_free(php_random_status *status, const bool persistent);
PHPAPI php_random_engine *php_random_engine_common_init(zend_class_entry *ce, zend_object_handlers *handlers, const php_random_algo *algo);
PHPAPI void php_random_engine_common_free_object(zend_object *object);
PHPAPI zend_object *php_random_engine_common_clone_object(zend_object *object);
PHPAPI uint32_t php_random_range32(const php_random_algo *algo, php_random_status *status, uint32_t umax);
PHPAPI uint64_t php_random_range64(const php_random_algo *algo, php_random_status *status, uint64_t umax);
PHPAPI zend_long php_random_range(const php_random_algo *algo, php_random_status *status, zend_long min, zend_long max);
PHPAPI const php_random_algo *php_random_default_algo(void);
PHPAPI php_random_status *php_random_default_status(void);

PHPAPI zend_string *php_random_bin2hex_le(const void *ptr, const size_t len);
PHPAPI bool php_random_hex2bin_le(zend_string *hexstr, void *dest);

PHPAPI void php_random_combinedlcg_seed_default(php_random_status_state_combinedlcg *state);

PHPAPI void php_random_mt19937_seed_default(php_random_status_state_mt19937 *state);

PHPAPI void php_random_pcgoneseq128xslrr64_advance(php_random_status_state_pcgoneseq128xslrr64 *state, uint64_t advance);

PHPAPI void php_random_xoshiro256starstar_jump(php_random_status_state_xoshiro256starstar *state);
PHPAPI void php_random_xoshiro256starstar_jump_long(php_random_status_state_xoshiro256starstar *state);

PHPAPI double php_random_gammasection_closed_open(const php_random_algo *algo, php_random_status *status, double min, double max);
PHPAPI double php_random_gammasection_closed_closed(const php_random_algo *algo, php_random_status *status, double min, double max);
PHPAPI double php_random_gammasection_open_closed(const php_random_algo *algo, php_random_status *status, double min, double max);
PHPAPI double php_random_gammasection_open_open(const php_random_algo *algo, php_random_status *status, double min, double max);

extern zend_module_entry random_module_entry;
# define phpext_random_ptr &random_module_entry

PHP_MINIT_FUNCTION(random);
PHP_MSHUTDOWN_FUNCTION(random);
PHP_RINIT_FUNCTION(random);

ZEND_BEGIN_MODULE_GLOBALS(random)
	php_random_status *combined_lcg;
	bool combined_lcg_seeded;
	php_random_status *mt19937;
	bool mt19937_seeded;
	int random_fd;
ZEND_END_MODULE_GLOBALS(random)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(random)

# define RANDOM_G(v)	ZEND_MODULE_GLOBALS_ACCESSOR(random, v)

#endif	/* PHP_RANDOM_H */
