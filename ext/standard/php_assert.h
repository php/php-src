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
   | Author: Thies C. Arntzen <thies@thieso.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_ASSERT_H
#define PHP_ASSERT_H

PHP_MINIT_FUNCTION(assert);
PHP_MSHUTDOWN_FUNCTION(assert);
PHP_RINIT_FUNCTION(assert);
PHP_RSHUTDOWN_FUNCTION(assert);
PHP_MINFO_FUNCTION(assert);

enum {
	PHP_ASSERT_ACTIVE=1,
	PHP_ASSERT_CALLBACK,
	PHP_ASSERT_BAIL,
	PHP_ASSERT_WARNING,
	PHP_ASSERT_EXCEPTION
};

extern PHPAPI zend_class_entry *assertion_error_ce;

#endif /* PHP_ASSERT_H */
