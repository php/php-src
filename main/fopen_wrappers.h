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
   | Author: Jim Winstead <jimw@php.net>                                  |
   +----------------------------------------------------------------------+
 */

#ifndef FOPEN_WRAPPERS_H
#define FOPEN_WRAPPERS_H

BEGIN_EXTERN_C()
#include "php_globals.h"
#include "php_ini.h"

PHPAPI int php_fopen_primary_script(zend_file_handle *file_handle);
PHPAPI char *expand_filepath(const char *filepath, char *real_path);
PHPAPI char *expand_filepath_ex(const char *filepath, char *real_path, const char *relative_to, size_t relative_to_len);
PHPAPI char *expand_filepath_with_mode(const char *filepath, char *real_path, const char *relative_to, size_t relative_to_len, int use_realpath);

PHPAPI int php_check_open_basedir(const char *path);
PHPAPI int php_check_open_basedir_ex(const char *path, int warn);
PHPAPI int php_check_specific_open_basedir(const char *basedir, const char *path);

/* OPENBASEDIR_CHECKPATH(filename) to ease merge between 6.x and 5.x */
#define OPENBASEDIR_CHECKPATH(filename) php_check_open_basedir(filename)

PHPAPI zend_string *php_resolve_path(const char *filename, size_t filename_len, const char *path);

PHPAPI FILE *php_fopen_with_path(const char *filename, const char *mode, const char *path, zend_string **opened_path);

PHPAPI char *php_strip_url_passwd(char *path);

PHPAPI ZEND_INI_MH(OnUpdateBaseDir);
END_EXTERN_C()

#endif
