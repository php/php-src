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

#include "php.h"
#include "php_rng.h"

#include "zend_exceptions.h"

#include "rng_xorshift128plus.h"
#include "rng_xorshift128plus_arginfo.h"

PHPAPI zend_class_entry *rng_ce_RNG_XorShift128Plus;

static zend_object_handlers XorShift128Plus_handlers;

static uint64_t splitmix64(uint64_t *seed)
{
	uint64_t r;
	r = (*seed += UINT64_C(0x9e3779b97f4a7c15));
	r = (r ^ (r >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
	r = (r ^ (r >> 27)) * UINT64_C(0x94d049bb133111eb);
	return (r ^ (r >> 31));
}

static uint32_t next(php_rng *rng)
{
	return (uint32_t) (rng->next64(rng));
}

static uint64_t next64(php_rng *rng)
{
	uint64_t s0, s1, r;

	rng_xorshift128plus_state *state = (rng_xorshift128plus_state*) rng->state;

	s1 = state->s[0];
	s0 = state->s[1];
	r = s0 + s1;
	state->s[0] = s0;
	s1 ^= s1 << 23;
	state->s[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);

	return r;
}

static zend_object *rng_object_new(zend_class_entry *ce)
{
	php_rng *rng = php_rng_initialize(next, next64);
	rng->state = (rng_xorshift128plus_state*)ecalloc(1, sizeof(rng_xorshift128plus_state));
	zend_object_std_init(&rng->std, ce);
	object_properties_init(&rng->std, ce);
	rng->std.handlers = &XorShift128Plus_handlers;
	
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

	memcpy(new->state, old->state, sizeof(rng_xorshift128plus_state));

	return new_obj;
}

PHP_METHOD(RNG_XorShift128Plus, __construct)
{
	zend_long seed;
	uint64_t s;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(seed);
	ZEND_PARSE_PARAMETERS_END();

	php_rng *rng = Z_RNG_P(ZEND_THIS);
	rng_xorshift128plus_state *state = rng->state;

	s = (uint64_t) seed;
	state->s[0] = splitmix64(&s);
	state->s[1] = splitmix64(&s);
}

PHP_METHOD(RNG_XorShift128Plus, next)
{
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG((zend_long) rng->next(rng));
}

PHP_METHOD(RNG_XorShift128Plus, next64)
{
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

#if UINT32_MAX >= ZEND_ULONG_MAX
	zend_value_error("Method doesn't supported 32bit integer range.");
	RETURN_THROWS();
#endif

	RETURN_LONG((zend_long) rng->next64(rng));
}

PHP_METHOD(RNG_XorShift128Plus, __serialize)
{
	php_rng *intern = Z_RNG_P(ZEND_THIS);
	rng_xorshift128plus_state *state = (rng_xorshift128plus_state*) intern->state;
	zval tmp;
	int i;

	ZEND_PARSE_PARAMETERS_NONE();
	
	array_init(return_value);
	
	/* state */
	for (i = 0; i < XORSHIFT128PLUS_N; i++) {
		ZVAL_STR(&tmp, zend_strpprintf(0, "%" PRIu64, state->s[i]));
		zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
	}

	/* members */
	ZVAL_ARR(&tmp, zend_std_get_properties(&intern->std));
	Z_TRY_ADDREF(tmp);
	zend_hash_next_index_insert(Z_ARRVAL_P(return_value), &tmp);
}

PHP_METHOD(RNG_XorShift128Plus, __unserialize)
{
	php_rng *intern = Z_RNG_P(ZEND_THIS);
	rng_xorshift128plus_state *state = (rng_xorshift128plus_state*) intern->state;
	HashTable *data;
	zval *tmp, *members_zv;
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "h", &data) == FAILURE) {
		RETURN_THROWS();
	}

	/* state */
	for (i = 0; i < XORSHIFT128PLUS_N; i++) {
		tmp = zend_hash_index_find(data, i);
		if (Z_TYPE_P(tmp) != IS_STRING) {
			zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
			RETURN_THROWS();
		}

		state->s[i] = strtoull(ZSTR_VAL(Z_STR_P(tmp)), NULL, 10);
	}

	/* members */
	members_zv = zend_hash_index_find(data, XORSHIFT128PLUS_N);
	if (!members_zv || Z_TYPE_P(members_zv) != IS_ARRAY || (state->s[0] == 0 && state->s[1] == 0)) {
		zend_throw_exception(NULL, "Incomplete or ill-formed serialization data", 0);
		RETURN_THROWS();
	}

	object_properties_load(&intern->std, Z_ARRVAL_P(members_zv));
}

PHP_MINIT_FUNCTION(rng_xorshift128plus)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, RNG_NAMESPACE "XorShift128Plus", class_RNG_XorShift128Plus_methods);
	rng_ce_RNG_XorShift128Plus = zend_register_internal_class(&ce);
	zend_class_implements(rng_ce_RNG_XorShift128Plus, 1, rng_ce_RNG_RNG64Interface);
	rng_ce_RNG_XorShift128Plus->create_object = rng_object_new;
	memcpy(&XorShift128Plus_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	XorShift128Plus_handlers.offset = XtOffsetOf(php_rng, std);
	XorShift128Plus_handlers.free_obj = free_object_storage;
	XorShift128Plus_handlers.clone_obj = rng_clone_obj;

	return SUCCESS;
}
