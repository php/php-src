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
   | Authors: Go Kudo <zeriyoshi@gmail.com>                               |
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

#include "php.h"
#include "php_rng.h"

#include "zend_exceptions.h"

#include "rng_mt19937.h"
#include "rng_mt19937_arginfo.h"

PHPAPI zend_class_entry *rng_ce_RNG_MT19937;

static zend_object_handlers MT19937_handlers;

static inline void reload(rng_mt19937_state *state)
{
	uint32_t *p = state->s;
	int i;

	for (i = MT19937_N - MT19937_M; i--; ++p) {
		*p = MT19937_twist(p[MT19937_M], p[0], p[1]);
	}
	for (i = MT19937_M; --i; ++p) {
		*p = MT19937_twist(p[MT19937_M-MT19937_N], p[0], p[1]);
	}
	*p = MT19937_twist(p[MT19937_M-MT19937_N], p[0], state->s[0]);

	state->i = 0;
}

static uint32_t next(php_rng *rng)
{
	rng_mt19937_state *state = rng->state;
	uint32_t s1;
	
	if (state->i >= MT19937_N) {
		reload(state);
	}

	s1 = state->s[state->i++];
	s1 ^= (s1 >> 11);
	s1 ^= (s1 << 7) & 0x9d2c5680U;
	s1 ^= (s1 << 15) & 0xefc60000U;
	return ( s1 ^ (s1 >> 18) );
}

static zend_object *rng_object_new(zend_class_entry *ce)
{
	php_rng *rng = php_rng_initialize(next, NULL);
	rng->state = (rng_mt19937_state*)ecalloc(1, sizeof(rng_mt19937_state));
	zend_object_std_init(&rng->std, ce);
	rng->std.handlers = &MT19937_handlers;

	return &rng->std;
}

static void free_object_storage(zend_object *object)
{
	php_rng *rng = rng_from_obj(object);
	zend_object_std_dtor(&rng->std);
	if (rng->state != NULL) {
		efree(rng->state);
	}
}

static zend_object *rng_clone_obj(zend_object *object)
{
	zend_object *new_obj;
	php_rng *new, *old;

	new_obj = rng_object_new(object->ce);

	zend_objects_clone_members(new_obj, object);

	new = rng_from_obj(new_obj);
	old = rng_from_obj(object);

	memcpy(new->state, old->state, sizeof(rng_mt19937_state));

	return new_obj;
}

PHP_METHOD(RNG_MT19937, __construct)
{
	zend_long seed;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(seed);
	ZEND_PARSE_PARAMETERS_END();

	php_rng *rng = Z_RNG_P(ZEND_THIS);
	rng_mt19937_state *state = rng->state;

	state->s[0] = seed & 0xffffffffU;
	for (state->i = 1; state->i < MT19937_N; state->i++) {
		state->s[state->i] = (1812433253U * (state->s[state->i - 1] ^ (state->s[state->i - 1] >> 30)) + state->i) & 0xffffffffU;
	}
	reload(state);
}

PHP_METHOD(RNG_MT19937, next)
{	
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();
	
	RETURN_LONG((zend_long) rng->next(rng));
}

PHP_METHOD(RNG_MT19937, next64)
{
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

#if UINT32_MAX >= ZEND_ULONG_MAX
	zend_value_error("Method doesn't supported 32bit integer range.");
	RETURN_THROWS();
#endif

	uint64_t result = rng->next(rng);
	result = (result << 32) | rng->next(rng);
	
	RETURN_LONG((zend_long) result);
}

PHP_METHOD(RNG_MT19937, __serialize)
{
	php_rng *intern = Z_RNG_P(ZEND_THIS);
	rng_mt19937_state *state = (rng_mt19937_state*) intern->state;
	zval tmp;
	int i;

	ZEND_PARSE_PARAMETERS_NONE();

	array_init(return_value);
	
	/* state */
	for (i = 0; i < MT19937_N + 1; i++) {
		ZVAL_LONG(&tmp,state->s[i]);
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
	}
	ZVAL_LONG(&tmp, state->i);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);

	/* members */
	ZVAL_ARR(&tmp, zend_std_get_properties(&intern->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}

PHP_METHOD(RNG_MT19937, __unserialize)
{
	php_rng *intern = Z_RNG_P(ZEND_THIS);
	rng_mt19937_state *state = (rng_mt19937_state*) intern->state;
	HashTable *data;
	zval *tmp, *members_zv;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	/* state */
	for (i = 0; i < MT19937_N + 1; i++) {
		tmp = zend_hash_index_find(data, i);
		if (Z_TYPE_P(tmp) != IS_LONG) {
			zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
			RETURN_THROWS();
		}

		state->s[i] = Z_LVAL_P(tmp);
	}
	tmp = zend_hash_index_find(data, MT19937_N + 1);
	if (Z_TYPE_P(tmp) != IS_LONG) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}
	state->i = Z_LVAL_P(tmp);

	/* members */
	members_zv = zend_hash_index_find(data, MT19937_N + 2);
	if (!members_zv || Z_TYPE_P(members_zv) != IS_ARRAY || state->i < 0) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}

	object_properties_load(&intern->std, Z_ARRVAL_P(members_zv));
}

PHP_MINIT_FUNCTION(rng_mt19937)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, RNG_NAMESPACE "MT19937", class_RNG_MT19937_methods);
	rng_ce_RNG_MT19937 = zend_register_internal_class(&ce);
	zend_class_implements(rng_ce_RNG_MT19937, 1, rng_ce_RNG_RNG64Interface);
	rng_ce_RNG_MT19937->create_object = rng_object_new;
	memcpy(&MT19937_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	MT19937_handlers.offset = XtOffsetOf(php_rng, std);
	MT19937_handlers.free_obj = free_object_storage;
	MT19937_handlers.clone_obj = rng_clone_obj;

	return SUCCESS;
}
