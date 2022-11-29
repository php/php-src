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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

#ifndef ZEND_TEST_OBSERVER_H
#define ZEND_TEST_OBSERVER_H

void zend_test_observer_init(INIT_FUNC_ARGS);
void zend_test_observer_shutdown(SHUTDOWN_FUNC_ARGS);
void zend_test_observer_ginit(zend_zend_test_globals *zend_test_globals);
void zend_test_observer_gshutdown(zend_zend_test_globals *zend_test_globals);

#endif
