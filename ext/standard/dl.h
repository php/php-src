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
   | Authors: Brian Schaffner <brian@tool.net>                            |
   |          Shane Caraveo <shane@caraveo.com>                           |
   |          Zeev Suraski <zeev@php.net>                                 |
   +----------------------------------------------------------------------+
*/

#ifndef DL_H
#define DL_H

PHPAPI int php_load_extension(const char *filename, int type, int start_now);
PHPAPI void php_dl(const char *file, int type, zval *return_value, int start_now);
PHPAPI void *php_load_shlib(const char *path, char **errp);

/* dynamic loading functions */
PHPAPI PHP_FUNCTION(dl);

PHP_MINFO_FUNCTION(dl);

#endif /* DL_H */
