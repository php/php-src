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
#include "php_random.h"
#include "php_rng.h"

#include "zend_exceptions.h"

#include "rng_os.h"
#include "rng_os_arginfo.h"

PHPAPI zend_class_entry *rng_ce_RNG_OS;

static zend_object_handlers OS_handlers;

static uint32_t next(php_rng *rng)
{
	uint32_t buf;
	php_random_bytes_silent(&buf, sizeof(uint32_t));
	return buf;
}

static uint64_t next64(php_rng *rng)
{
	uint64_t buf;
	php_random_bytes_silent(&buf, sizeof(uint64_t));
	return buf;
}

static zend_object *rng_object_new(zend_class_entry *ce)
{
	php_rng *rng = php_rng_initialize(next, next64);
	zend_object_std_init(&rng->std, ce);
	object_properties_init(&rng->std, ce);
	rng->std.handlers = &OS_handlers;
	
	return &rng->std;
}

static void free_object_storage(zend_object *object)
{
	php_rng *rng = rng_from_obj(object);
	zend_object_std_dtor(&rng->std);
}

PHP_METHOD(RNG_OS, next)
{	
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG((zend_long) rng->next(rng));
}

PHP_METHOD(RNG_OS, next64)
{
	php_rng *rng = Z_RNG_P(ZEND_THIS);

	ZEND_PARSE_PARAMETERS_NONE();

	RETURN_LONG((zend_long) rng->next64(rng));
}

PHP_MINIT_FUNCTION(rng_os)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, RNG_NAMESPACE "OS", class_RNG_OS_methods);
	rng_ce_RNG_OS = zend_register_internal_class(&ce);
	zend_class_implements(rng_ce_RNG_OS, 1, rng_ce_RNG_RNGInterface);
	rng_ce_RNG_OS->create_object = rng_object_new;
	memcpy(&OS_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	OS_handlers.offset = XtOffsetOf(php_rng, std);
	OS_handlers.free_obj = free_object_storage;

	return SUCCESS;
}
