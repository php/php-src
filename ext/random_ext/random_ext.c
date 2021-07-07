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

/*
	The following functions are based on a C++ class MTRand by
	Richard J. Wagner. For more information see the web page at
	http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/MersenneTwister.h
	Mersenne Twister random number generator -- a C++ class MTRand
	Based on code by Makoto Matsumoto, Takuji Nishimura, and Shawn Cokus
	Richard J. Wagner  v1.0  15 May 2003  rjwagner@writeme.com
	The Mersenne Twister is an algorithm for generating random numbers.  It
	was designed with consideration of the flaws in various other generators.
	The period, 2^19937-1, and the order of equidistribution, 623 dimensions,
	are far greater.  The generator is also fast; it avoids multiplication and
	division, and it benefits from caches and pipelines.  For more information
	see the inventors' web page at http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
	Reference
	M. Matsumoto and T. Nishimura, "Mersenne Twister: A 623-Dimensionally
	Equidistributed Uniform Pseudo-Random Number Generator", ACM Transactions on
	Modeling and Computer Simulation, Vol. 8, No. 1, January 1998, pp 3-30.
	Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
	Copyright (C) 2000 - 2003, Richard J. Wagner
	All rights reserved.
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The names of its contributors may not be used to endorse or promote
	   products derived from this software without specific prior written
	   permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "ext/standard/php_random.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

#include "php_random_ext.h"
#include "random_ext_arginfo.h"

PHPAPI zend_class_entry *random_ce_Random_NumberGenerator;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_XorShift128Plus;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_MT19937;
PHPAPI zend_class_entry *random_ce_Random_NumberGenerator_Secure;
PHPAPI zend_class_entry *random_ce_Random;

static zend_object_handlers random_rng_xorshift128plus_object_handlers;
static zend_object_handlers random_rng_mt19937_object_handlers;
static zend_object_handlers random_rng_secure_object_handlers;
static zend_object_handlers random_php_random_object_handlers;

static inline uint64_t php_random_rng_next(php_random_rng *rng)
{
	return rng->algo->generate(rng->state);
}

PHPAPI uint64_t php_random_next(php_random *random)
{
	zval retval;
	zend_object *rng;
	zend_string *rng_fname;
	zend_function *rng_func;

	if (random->rng) {
		return php_random_rng_next(random->rng);
	}

	rng = Z_OBJ_P(zend_read_property(random->std.ce, &random->std, "rng", sizeof("rng") - 1, 0, NULL));
	rng_fname = zend_string_init("generate", sizeof("generate") - 1, 0);
	rng_func = zend_hash_find_ptr(&rng->ce->function_table, rng_fname);
	zend_string_release(rng_fname);

	zend_call_known_instance_method_with_0_params(rng_func, rng, &retval);

	return ((uint64_t) Z_LVAL(retval));
}

static uint32_t range32(php_random *random, uint32_t umax) {
	uint32_t result, limit;

	result = php_random_next(random);

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT32_MAX)) {
		return result;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT32_MAX % max == 0 */
	limit = UINT32_MAX - (UINT32_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		result = php_random_next(random);
	}

	return result % umax;
}

#if ZEND_ULONG_MAX > UINT32_MAX
static uint64_t range64(php_random *random, uint64_t umax) {
	uint64_t result, limit;

	result = php_random_next(random);
	if (random->rng && random->rng->algo->generate_size == sizeof(uint32_t)) {
		result = (result << sizeof(uint32_t)) | php_random_next(random);
	}

	/* Special case where no modulus is required */
	if (UNEXPECTED(umax == UINT64_MAX)) {
		return result;
	}

	/* Increment the max so the range is inclusive of max */
	umax++;

	/* Powers of two are not biased */
	if ((umax & (umax - 1)) == 0) {
		return result & (umax - 1);
	}

	/* Ceiling under which UINT64_MAX % max == 0 */
	limit = UINT64_MAX - (UINT64_MAX % umax) - 1;

	/* Discard numbers over the limit to avoid modulo bias */
	while (UNEXPECTED(result > limit)) {
		result = php_random_next(random);
		if (random->rng && random->rng->algo->generate_size == sizeof(uint32_t)) {
			result = (result << sizeof(uint32_t)) | php_random_next(random);
		}
	}

	return result % umax;
}
#endif

PHPAPI zend_long php_random_range(php_random *random, zend_long min, zend_long max)
{
	zend_ulong umax = max - min;

#if ZEND_ULONG_MAX > UINT32_MAX
	if (umax > UINT32_MAX) {
		return (zend_long) (range64(random, umax) + min);
	}
#endif

	return (zend_long) (range32(random, umax) + min);
}

PHPAPI void php_random_array_data_shuffle(php_random *random, zval *array)
{
	uint32_t idx, j, n_elems, n_left;
	Bucket *p, temp;
	HashTable *hash;
	zend_long rnd_idx;

	n_elems = zend_hash_num_elements(Z_ARRVAL_P(array));

	if (n_elems < 1) {
		return;
	}

	hash = Z_ARRVAL_P(array);
	n_left = n_elems;

	if (EXPECTED(!HT_HAS_ITERATORS(hash))) {
		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_random_range(random, 0, n_left);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
			}
		}
	} else {
		uint32_t iter_pos = zend_hash_iterators_lower_pos(hash, 0);

		if (hash->nNumUsed != hash->nNumOfElements) {
			for (j = 0, idx = 0; idx < hash->nNumUsed; idx++) {
				p = hash->arData + idx;
				if (Z_TYPE(p->val) == IS_UNDEF) continue;
				if (j != idx) {
					hash->arData[j] = *p;
					if (idx == iter_pos) {
						zend_hash_iterators_update(hash, idx, j);
						iter_pos = zend_hash_iterators_lower_pos(hash, iter_pos + 1);
					}
				}
				j++;
			}
		}
		while (--n_left) {
			rnd_idx = php_random_range(random, 0, n_left);
			if (rnd_idx != n_left) {
				temp = hash->arData[n_left];
				hash->arData[n_left] = hash->arData[rnd_idx];
				hash->arData[rnd_idx] = temp;
				zend_hash_iterators_update(hash, (uint32_t)rnd_idx, n_left);
			}
		}
	}
	hash->nNumUsed = n_elems;
	hash->nInternalPointer = 0;

	for (j = 0; j < n_elems; j++) {
		p = hash->arData + j;
		if (p->key) {
			zend_string_release_ex(p->key, 0);
		}
		p->h = j;
		p->key = NULL;
	}
	hash->nNextFreeElement = n_elems;
	if (!(HT_FLAGS(hash) & HASH_FLAG_PACKED)) {
		zend_hash_to_packed(hash);
	}
}

PHPAPI void php_random_string_shuffle(php_random *random, char *str, zend_long len)
{
	zend_long n_elems, rnd_idx, n_left;
	char temp;
	n_elems = len;

	if (n_elems <= 1) {
		return;
	}

	n_left = n_elems;

	while (--n_left) {
		rnd_idx = php_random_range(random, 0, n_left);
		if (rnd_idx != n_left) {
			temp = str[n_left];
			str[n_left] = str[rnd_idx];
			str[rnd_idx] = temp;
		}
	}
}

static inline zend_object *php_random_rng_common_init(zend_class_entry *ce, const php_random_rng_algo *algo, const zend_object_handlers *handlers) {
	php_random_rng *rng = zend_object_alloc(sizeof(php_random_rng), ce);

	zend_object_std_init(&rng->std, ce);
	object_properties_init(&rng->std, ce);
	
	rng->algo = algo;
	if (rng->algo->state_size > 0) {
		rng->state = ecalloc(1, algo->state_size);
	}
	rng->std.handlers = handlers;

	return &rng->std;
}

static void php_random_rng_common_free_obj(zend_object *object) {
	php_random_rng *rng = php_random_rng_from_obj(object);

	if (rng->state) {
		efree(rng->state);
	}

	zend_object_std_dtor(object);
}

static zend_object *php_random_rng_common_clone_obj(zend_object *old_object) {
	php_random_rng *old_rng = php_random_rng_from_obj(old_object);
	php_random_rng *new_rng = php_random_rng_from_obj(old_object->ce->create_object(old_object->ce));

	zend_objects_clone_members(&new_rng->std, &old_rng->std);

	new_rng->algo = old_rng->algo;
	if (old_rng->state) {
		memcpy(new_rng->state, old_rng->state, old_rng->algo->state_size);
	}

	return &new_rng->std;
}

static inline uint64_t xorshift128plus_splitmix64(uint64_t *seed) {
	uint64_t r;

	r = (*seed += UINT64_C(0x9e3779b97f4a7c15));
	r = (r ^ (r >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	r = (r ^ (r >> 27)) * UINT64_C(0x94d049bb133111eb);
	return (r ^ (r >> 31));
}

typedef struct _xorshift128plus_state {
	uint64_t s[2];
} xorshift128plus_state;

static uint64_t xorshift128plus_generate(void *state) {
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	uint64_t s0, s1, r;

	s1 = s->s[0];
	s0 = s->s[1];
	r = s0 + s1;
	s->s[0] = s0;
	s1 ^= s1 << 23;
	s->s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

	return r;
}

static void xorshift128plus_seed(void *state, const zend_long seed) {
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	uint64_t sd = (uint64_t) seed;

	s->s[0] = xorshift128plus_splitmix64(&sd);
	s->s[1] = xorshift128plus_splitmix64(&sd);
}

static int xorshift128plus_serialize(void *state, HashTable *data) {
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	zval tmp;
	int i;

	for (i = 0; i < 2; i++) {
		ZVAL_STR(&tmp, zend_strpprintf(0, "%" PRIu64, s->s[i]));
		zend_hash_next_index_insert(data, &tmp);
	}

	return FAILURE;
}

static int xorshift128plus_unserialize(void *state, HashTable *data) {
	xorshift128plus_state *s = (xorshift128plus_state *)state;
	zval *tmp;
	int i;

	for (i = 0; i < 2; i++) {
		tmp = zend_hash_index_find(data, i);
		if (!tmp || Z_TYPE_P(tmp) != IS_STRING) {
			return FAILURE;
		}

		s->s[i] = strtoull(ZSTR_VAL(Z_STR_P(tmp)), NULL, 10);
	}

	return FAILURE;
}

const php_random_rng_algo php_random_rng_algo_xorshift128plus = {
	sizeof(uint64_t),
	sizeof(xorshift128plus_state),
	xorshift128plus_generate,
	xorshift128plus_seed,
	xorshift128plus_serialize,
	xorshift128plus_unserialize
};

static zend_object *php_random_rng_xorshift128plus_new(zend_class_entry *ce) {
	return php_random_rng_common_init(ce, &php_random_rng_algo_xorshift128plus, &random_rng_xorshift128plus_object_handlers);
}

#define MT19937_N             (624)
#define MT19937_M             (397)

#define MT19937_hiBit(u)      ((u) & 0x80000000U)
#define MT19937_loBit(u)      ((u) & 0x00000001U)
#define MT19937_loBits(u)     ((u) & 0x7FFFFFFFU)
#define MT19937_MixBits(u, v) (MT19937_hiBit(u)|MT19937_loBits(v))
#define MT19937_twist(m,u,v)  (m ^ (MT19937_MixBits(u,v)>>1) ^ ((uint32_t)(-(int32_t)(MT19937_loBit(v))) & 0x9908b0dfU))

typedef struct _mt19937_state {
	uint32_t s[MT19937_N + 1];
	int i;
} mt19937_state;

static inline void mt19937_reload(mt19937_state *state) {
	uint32_t *p = state->s;
	int i;

	for (i = MT19937_N - MT19937_M; i--; ++p) {
		*p = MT19937_twist(p[MT19937_M], p[0], p[1]);
	}
	for (i = MT19937_M; --i; ++p) {
		*p = MT19937_twist(p[MT19937_M - MT19937_N], p[0], p[1]);
	}
	*p = MT19937_twist(p[MT19937_M - MT19937_N], p[0], state->s[0]);

	state->i = 0;
}

static uint64_t mt19937_generate(void *state) {
	mt19937_state *s = (mt19937_state *)state;
	uint32_t s1;

	if (s->i >= MT19937_N) {
		mt19937_reload(s);
	}

	s1 = s->s[s->i++];
	s1 ^= (s1 >> 11);
	s1 ^= (s1 << 7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}

static void mt19937_seed(void *state, const zend_long seed) {
	mt19937_state *s = (mt19937_state *)state;

	s->s[0] = seed & 0xffffffffU;
	for (s->i = 1; s->i < MT19937_N; s->i++) {
		s->s[s->i] = (1812433253U * (s->s[s->i - 1] ^ (s->s[s->i - 1] >> 30)) + s->i) & 0xffffffffU;
	}
	mt19937_reload(s);
}

static int mt19937_serialize(void *state, HashTable *data) {
	mt19937_state *s = (mt19937_state *)state;
	zval tmp;
	int i;

	for (i = 0; i< MT19937_N + 1; i++) {
		ZVAL_LONG(&tmp, s->s[i]);
		zend_hash_next_index_insert(data, &tmp);
	}
	ZVAL_LONG(&tmp, s->i);
	zend_hash_next_index_insert(data, &tmp);

	return SUCCESS;
}

static int mt19937_unserialize(void *state, HashTable *data) {
	mt19937_state *s = (mt19937_state *)state;
	zval *tmp;
	int i;

	for (i = 0; i < MT19937_N + 1; i++) {
		tmp = zend_hash_index_find(data, i);
		if (!tmp || Z_TYPE_P(tmp) != IS_LONG) {
			return FAILURE;
		}

		s->s[i] = Z_LVAL_P(tmp);
	}
	tmp = zend_hash_index_find(data, MT19937_N + 1);
	if (Z_TYPE_P(tmp) != IS_LONG) {
		return FAILURE;
	}
	s->i = Z_LVAL_P(tmp);

	return SUCCESS;
}

const php_random_rng_algo php_random_rng_algo_mt19937 = {
	sizeof(uint32_t),
	sizeof(mt19937_state),
	mt19937_generate,
	mt19937_seed,
	mt19937_serialize,
	mt19937_unserialize
};

static zend_object *php_random_rng_mt19937_new(zend_class_entry *ce) {
	return php_random_rng_common_init(ce, &php_random_rng_algo_mt19937, &random_rng_mt19937_object_handlers);
}

static uint64_t secure_generate(void *state) {
	uint64_t ret;

	php_random_bytes_silent(&ret, sizeof(uint64_t));

	return ret;
}

const php_random_rng_algo php_random_rng_algo_secure = {
	sizeof(uint64_t),
	0,
	secure_generate,
	NULL,
	NULL,
	NULL
};

static zend_object *php_random_rng_secure_new(zend_class_entry *ce) {
	return php_random_rng_common_init(ce, &php_random_rng_algo_secure, &random_rng_secure_object_handlers);
}

static zend_object *php_random_new(zend_class_entry *ce) {
	php_random *random = zend_object_alloc(sizeof(php_random), ce);

	zend_object_std_init(&random->std, ce);
	object_properties_init(&random->std, ce);
	random->std.handlers = &random_php_random_object_handlers;

	return &random->std;
}

static void php_random_free_obj(zend_object *object) {
	php_random *random = php_random_from_obj(object);

	zend_object_std_dtor(&random->std);
}

PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __construct)
{
	php_random_rng *rng = Z_RANDOM_RNG_P(ZEND_THIS);
	zend_long seed;
	bool is_null = true;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG_OR_NULL(seed, is_null)
	ZEND_PARSE_PARAMETERS_END();

	if (is_null) {
		if (php_random_bytes_throw(&seed, sizeof(zend_long)) == FAILURE) {
			RETURN_THROWS();
		}
	}

	if (rng->algo->seed) {
		rng->algo->seed(rng->state, seed);
	}
}

PHP_METHOD(Random_NumberGenerator_XorShift128Plus, generate)
{
	php_random_rng *rng = Z_RANDOM_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();
	
	RETURN_LONG((zend_long) php_random_rng_next(rng));
}

PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __serialize)
{
	php_random_rng *rng = Z_RANDOM_RNG_P(ZEND_THIS);
	zval tmp;

	ZEND_PARSE_PARAMETERS_NONE();

	if (!rng->algo->serialize) {
		zend_throw_exception(NULL, "Algorithm does not support serialize", 0);
		RETURN_THROWS();
	}

	array_init(return_value);

	/* members */
	ZVAL_ARR(&tmp, zend_std_get_properties(&rng->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* state */
	array_init(&tmp);
	rng->algo->serialize(rng->state, Z_ARRVAL(tmp));
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}

PHP_METHOD(Random_NumberGenerator_XorShift128Plus, __unserialize)
{
	php_random_rng *rng = Z_RANDOM_RNG_P(ZEND_THIS);
	HashTable *data;
	zval *tmp;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data);
	ZEND_PARSE_PARAMETERS_END();

	if (!rng->algo->unserialize) {
		zend_throw_exception(NULL, "Algorithm does not support unserialize", 0);
		RETURN_THROWS();
	}

	/* members */
	tmp = zend_hash_index_find(data, 0);
	if (!tmp || Z_TYPE_P(tmp) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	object_properties_load(&rng->std, Z_ARRVAL_P(tmp));

	/* state */
	tmp = zend_hash_index_find(data, 1);
	if (!tmp || Z_TYPE_P(tmp) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	rng->algo->unserialize(rng->state, Z_ARRVAL_P(tmp));
}

PHP_METHOD(Random_NumberGenerator_Secure, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

PHP_METHOD(Random, __construct)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_object *rng = 0;
	zval zrng;
	zend_string *pname;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_OBJ_OF_CLASS_OR_NULL(rng, random_ce_Random_NumberGenerator);
	ZEND_PARSE_PARAMETERS_END();

	if (!rng) {
		rng = php_random_rng_xorshift128plus_new(random_ce_Random_NumberGenerator_XorShift128Plus);
		zend_call_known_instance_method_with_0_params(
			random_ce_Random_NumberGenerator_XorShift128Plus->constructor,
			rng,
			NULL
		);
		/* No needed self-refcount */
		GC_DELREF(rng);
	}

	ZVAL_OBJ(&zrng, rng);

	pname = zend_string_init("rng", sizeof("rng") - 1, 0);
	zend_std_write_property(&random->std, pname, &zrng, NULL);
	zend_string_release(pname);

	/* Internal class always php_random_rng */
	if (rng->ce->type == ZEND_INTERNAL_CLASS) {
		random->rng = php_random_rng_from_obj(rng);
	}
}

PHP_METHOD(Random, getNumberGenerator)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zval *zrng;

	ZEND_PARSE_PARAMETERS_NONE();

	if (random->rng) {
		RETURN_OBJ_COPY(&random->rng->std);
	}

	zrng = zend_read_property(random->std.ce, &random->std, "rng", sizeof("rng") - 1, 0, NULL);
	RETURN_OBJ(Z_OBJ_P(zrng));
}

PHP_METHOD(Random, getInt)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_long min, max;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(min)
		Z_PARAM_LONG(max)
	ZEND_PARSE_PARAMETERS_END();

	if (UNEXPECTED(max < min)) {
		zend_argument_value_error(2, "must be greater than or equal to argument #1 ($min)");
		RETURN_THROWS();
	}

	RETURN_LONG(php_random_range(random, min, max));
}

PHP_METHOD(Random, getBytes)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_string *ret;
	zend_long size;
	uint64_t buf;
	uint8_t *bytes;
	size_t generated_bytes = 0;
	int i;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(size)
	ZEND_PARSE_PARAMETERS_END();

	if (size < 1) {
		zend_argument_value_error(1, "must be greater than 0");
		RETURN_THROWS();
	}

	ret = zend_string_alloc(size, 0);

	while (generated_bytes <= size) {
		buf = php_random_next(random);
		if (random->rng && random->rng->algo->generate_size == sizeof(uint32_t)) {
			buf = (buf << 32) | php_random_next(random);
		}
		bytes = (uint8_t *) &buf;
		for (i = 0; i < (sizeof(uint64_t) / sizeof(uint8_t)); i ++) {
			ZSTR_VAL(ret)[generated_bytes + i] = bytes[i];

			if ((generated_bytes + i) >= size) {
				ZSTR_VAL(ret)[size] = '\0';
				RETURN_STR(ret);
			}
		}
		generated_bytes += (sizeof(uint64_t) / sizeof(uint8_t));
	}
}

PHP_METHOD(Random, shuffleArray)
{
	php_random *random =  Z_RANDOM_P(ZEND_THIS);
	zval *array;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY(array)
	ZEND_PARSE_PARAMETERS_END();

	ZVAL_DUP(return_value, array);
	php_random_array_data_shuffle(random, return_value);
}

PHP_METHOD(Random, shuffleString)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zend_string *string;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STR(string)
	ZEND_PARSE_PARAMETERS_END();

	if (ZSTR_LEN(string) < 2) {
		RETURN_STR_COPY(string);
	}

	RETVAL_STRINGL(ZSTR_VAL(string), ZSTR_LEN(string));
	php_random_string_shuffle(random, Z_STRVAL_P(return_value), (zend_long) Z_STRLEN_P(return_value));
}

PHP_METHOD(Random, __serialize)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	zval tmp;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);

	ZVAL_ARR(&tmp, zend_std_get_properties(&random->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}

PHP_METHOD(Random, __unserialize)
{
	php_random *random = Z_RANDOM_P(ZEND_THIS);
	HashTable *data;
	zval *members_zv;
	zval *prng;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(data);
	ZEND_PARSE_PARAMETERS_END();

	members_zv = zend_hash_index_find(data, 0);
	if (!members_zv || Z_TYPE_P(members_zv) != IS_ARRAY) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	object_properties_load(&random->std, Z_ARRVAL_P(members_zv));

	prng = zend_read_property(random->std.ce, &random->std, "rng", sizeof("rng") - 1, 0, NULL);
	if (Z_TYPE_P(prng) != IS_OBJECT || !instanceof_function(Z_OBJCE_P(prng), random_ce_Random_NumberGenerator)) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}

	/* Internal class always php_random_rng */
	if (Z_OBJCE_P(prng)->type == ZEND_INTERNAL_CLASS) {
		random->rng = Z_RANDOM_RNG_P(prng);
	}
}

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(random_ext)
{
	/* Random\NumberGenerator */
	random_ce_Random_NumberGenerator = register_class_Random_NumberGenerator();

	/* Random\NumberGenerator\XorShift128Plus */
	random_ce_Random_NumberGenerator_XorShift128Plus = register_class_Random_NumberGenerator_XorShift128Plus(random_ce_Random_NumberGenerator);
	random_ce_Random_NumberGenerator_XorShift128Plus->create_object = php_random_rng_xorshift128plus_new;
	memcpy(&random_rng_xorshift128plus_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_rng_xorshift128plus_object_handlers.offset = XtOffsetOf(php_random_rng, std);
	random_rng_xorshift128plus_object_handlers.free_obj = php_random_rng_common_free_obj;
	random_rng_xorshift128plus_object_handlers.clone_obj = php_random_rng_common_clone_obj;

	/* Random\NumberGenerator\MT19937 */
	random_ce_Random_NumberGenerator_MT19937 = register_class_Random_NumberGenerator_MT19937(random_ce_Random_NumberGenerator);
	random_ce_Random_NumberGenerator_MT19937->create_object = php_random_rng_mt19937_new;
	memcpy(&random_rng_mt19937_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_rng_mt19937_object_handlers.offset = XtOffsetOf(php_random_rng, std);
	random_rng_mt19937_object_handlers.free_obj = php_random_rng_common_free_obj;
	random_rng_mt19937_object_handlers.clone_obj = php_random_rng_common_clone_obj;

	/* Random\NumberGenerator\Secure */
	random_ce_Random_NumberGenerator_Secure = register_class_Random_NumberGenerator_Secure(random_ce_Random_NumberGenerator);
	random_ce_Random_NumberGenerator_Secure->create_object = php_random_rng_secure_new;
	random_ce_Random_NumberGenerator_Secure->serialize = zend_class_serialize_deny;
	random_ce_Random_NumberGenerator_Secure->unserialize = zend_class_unserialize_deny;
	memcpy(&random_rng_secure_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_rng_secure_object_handlers.offset = XtOffsetOf(php_random_rng, std);
	random_rng_secure_object_handlers.free_obj = php_random_rng_common_free_obj;
	random_rng_secure_object_handlers.clone_obj = NULL;

	/* Random */
	random_ce_Random = register_class_Random();
	random_ce_Random->create_object = php_random_new;
	memcpy(&random_php_random_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	random_php_random_object_handlers.offset = XtOffsetOf(php_random, std);
	random_php_random_object_handlers.free_obj = php_random_free_obj;
	random_php_random_object_handlers.clone_obj = NULL;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(random_ext)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "Random support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ random_module_entry */
zend_module_entry random_module_entry = {
	STANDARD_MODULE_HEADER,
	"random",				/* Extension name */
	NULL,					/* zend_function_entry */
	PHP_MINIT(random_ext),	/* PHP_MINIT - Module initialization */
	NULL,					/* PHP_MSHUTDOWN - Module shutdown */
	NULL,					/* PHP_RINIT - Request initialization */
	NULL,					/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(random_ext),	/* PHP_MINFO - Module info */
	PHP_VERSION,			/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_RANDOM
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(random_ext)
#endif
