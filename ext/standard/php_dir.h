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

#ifndef PHP_DIR_H
#define PHP_DIR_H

/* directory functions */
PHP_MINIT_FUNCTION(dir);
PHP_RINIT_FUNCTION(dir);

#define PHP_SCANDIR_SORT_ASCENDING 0
#define PHP_SCANDIR_SORT_DESCENDING 1
#define PHP_SCANDIR_SORT_NONE 2

#endif /* PHP_DIR_H */
