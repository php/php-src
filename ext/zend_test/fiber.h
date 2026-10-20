/*
  +----------------------------------------------------------------------+
  | Copyright © The PHP Group and Contributors.                          |
  +----------------------------------------------------------------------+
  | This source file is subject to the Modified BSD License that is      |
  | bundled with this package in the file LICENSE, and is available      |
  | through the World Wide Web at <https://www.php.net/license/>.        |
  |                                                                      |
  | SPDX-License-Identifier: BSD-3-Clause                                |
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
