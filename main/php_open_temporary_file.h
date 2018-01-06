/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_OPEN_TEMPORARY_FILE_H
#define PHP_OPEN_TEMPORARY_FILE_H

#define PHP_TMP_FILE_OPEN_BASEDIR_CHECK (1<<0)
#define PHP_TMP_FILE_SILENT (1<<1)

BEGIN_EXTERN_C()
PHPAPI FILE *php_open_temporary_file(const char *dir, const char *pfx, zend_string **opened_path_p);
PHPAPI int php_open_temporary_fd_ex(const char *dir, const char *pfx, zend_string **opened_path_p, uint32_t flags);
PHPAPI int php_open_temporary_fd(const char *dir, const char *pfx, zend_string **opened_path_p);
PHPAPI const char *php_get_temporary_directory(void);
END_EXTERN_C()

#endif /* PHP_OPEN_TEMPORARY_FILE_H */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: sw=4 ts=4 fdm=marker
 * vim<600: sw=4 ts=4
 */
