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
*/

#ifndef ZEND_TEST_OBSERVER_H
#define ZEND_TEST_OBSERVER_H

void zend_test_observer_init(INIT_FUNC_ARGS);
void zend_test_observer_shutdown(SHUTDOWN_FUNC_ARGS);
void zend_test_observer_ginit(zend_zend_test_globals *zend_test_globals);
void zend_test_observer_gshutdown(zend_zend_test_globals *zend_test_globals);

#endif
