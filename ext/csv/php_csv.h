/*
   +----------------------------------------------------------------------+
   | Copyright © 2020-2026, Gina Peter Banyard and Contributors.          |
   +----------------------------------------------------------------------+
   | This source file is subject to the Modified BSD License that is      |
   | bundled with this package in the file LICENSE, and is available      |
   | through the World Wide Web at <https://www.php.net/license/>.        |
   |                                                                      |
   | SPDX-License-Identifier: BSD-3-Clause                                |
   +----------------------------------------------------------------------+
   | Authors: Gina Peter Banyard <girgias@php.net>                        |
   |          Damian Jóźwiak <damian.jozwiak.lodz@gmail.com>              |
   +----------------------------------------------------------------------+
*/
/* csv extension for PHP */

#ifndef PHP_CSV_H
# define PHP_CSV_H

extern zend_module_entry csv_module_entry;
# define phpext_csv_ptr &csv_module_entry

# if defined(ZTS) && defined(COMPILE_DL_CSV)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_CSV_H */
