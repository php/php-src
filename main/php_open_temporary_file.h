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
   | Author: Zeev Suraski <zeev@php.net>                                  |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_OPEN_TEMPORARY_FILE_H
#define PHP_OPEN_TEMPORARY_FILE_H

#define PHP_TMP_FILE_DEFAULT 0
#define PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_FALLBACK (1<<0)
#define PHP_TMP_FILE_SILENT (1<<1)
#define PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_EXPLICIT_DIR (1<<2)
#define PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ALWAYS \
    (PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_FALLBACK | PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_EXPLICIT_DIR)

/* for compatibility purpose */
#define PHP_TMP_FILE_OPEN_BASEDIR_CHECK PHP_TMP_FILE_OPEN_BASEDIR_CHECK_ON_FALLBACK


BEGIN_EXTERN_C()
PHPAPI FILE *php_open_temporary_file(const char *dir, const char *pfx, zend_string **opened_path_p);
PHPAPI int php_open_temporary_fd_ex(const char *dir, const char *pfx, zend_string **opened_path_p, uint32_t flags);
PHPAPI int php_open_temporary_fd(const char *dir, const char *pfx, zend_string **opened_path_p);
PHPAPI const char *php_get_temporary_directory(void);
END_EXTERN_C()

#endif /* PHP_OPEN_TEMPORARY_FILE_H */
