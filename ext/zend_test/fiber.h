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
  | Authors: Aaron Piotrowski <aaron@trowski.com>                        |
  +----------------------------------------------------------------------+
*/

#ifndef ZEND_TEST_FIBER_H
#define ZEND_TEST_FIBER_H

#include "zend_fibers.h"

typedef struct _zend_test_fiber zend_test_fiber;

struct _zend_test_fiber {
	zend_object std;
	uint8_t flags;
	zend_fiber_context context;
	zend_fiber_context *caller;
	zend_fiber_context *previous;
	zend_test_fiber *target;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	zval result;
};

void zend_test_fiber_init(void);

#endif
