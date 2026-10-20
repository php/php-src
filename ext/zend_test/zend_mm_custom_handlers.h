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
  | Author: Florian Engelhardt <florian@engelhardt.tc>                   |
  +----------------------------------------------------------------------+
*/

#ifndef ZEND_TEST_MM_CUSTOM_HANDLERS_H
#define ZEND_TEST_MM_CUSTOM_HANDLERS_H

void zend_test_mm_custom_handlers_minit(INIT_FUNC_ARGS);
void zend_test_mm_custom_handlers_rinit(void);
void zend_test_mm_custom_handlers_rshutdown(void);

#endif
