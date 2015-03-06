/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2015 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Nikita Popov <nikic@php.net>                                |
   |          Bob Weinand <bobwei9@hotmail.com>                           |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef ZEND_GENERATORS_H
#define ZEND_GENERATORS_H

BEGIN_EXTERN_C()

extern ZEND_API zend_class_entry *zend_ce_generator;
extern ZEND_API zend_class_entry *zend_ce_ClosedGeneratorException;

typedef struct _zend_generator_node zend_generator_node;
typedef struct _zend_generator zend_generator;

struct _zend_generator_node {
	zend_generator *parent; /* NULL for root */
	uint32_t children;
	union {
		HashTable ht; /* if > 4 children */
		struct {
			zend_generator *leaf;
			zend_generator *child;
		} array[4]; /* if <= 4 children */
	} child;
	union {
		zend_generator *leaf; /* if > 0 children */
		zend_generator *root; /* if 0 children */
	} ptr;
};

struct _zend_generator {
	zend_object std;

	zend_object_iterator *iterator;

	/* The suspended execution context. */
	zend_execute_data *execute_data;

	/* The separate stack used by generator */
	zend_vm_stack stack;

	/* Current value */
	zval value;
	/* Current key */
	zval key;
	/* Return value */
	zval retval;

	/* Variable to put sent value into */
	zval *send_target;
	/* Largest used integer key for auto-incrementing keys */
	zend_long largest_used_integer_key;

	/* Values specified by "yield from" to yield from this generator.
	 * This is only used for arrays or non-generator Traversables.
	 * This zval also uses the u2 structure in the same way as
	 * by-value foreach. */
	zval values;

	/* Node of waiting generators when multiple "yield *" expressions
	 * are nested. */
	zend_generator_node node;

	/* Fake execute_data for stacktraces */
	zend_execute_data execute_fake;

	/* ZEND_GENERATOR_* flags */
	zend_uchar flags;
};

static const zend_uchar ZEND_GENERATOR_CURRENTLY_RUNNING = 0x1;
static const zend_uchar ZEND_GENERATOR_FORCED_CLOSE      = 0x2;
static const zend_uchar ZEND_GENERATOR_AT_FIRST_YIELD    = 0x4;
static const zend_uchar ZEND_GENERATOR_DO_INIT           = 0x8;

void zend_register_generator_ce(void);
ZEND_API void zend_generator_create_zval(zend_execute_data *call, zend_op_array *op_array, zval *return_value);
ZEND_API void zend_generator_close(zend_generator *generator, zend_bool finished_execution);
ZEND_API void zend_generator_resume(zend_generator *generator);

void zend_generator_yield_from(zend_generator *this, zend_generator *from);
ZEND_API zend_generator *zend_generator_get_current(zend_generator *generator);
ZEND_API zend_execute_data *zend_generator_check_placeholder_frame(zend_execute_data *ptr);

END_EXTERN_C()

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
