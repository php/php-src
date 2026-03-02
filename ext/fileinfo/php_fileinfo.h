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
  | Author: Ilia Alshanetsky <ilia@php.net>                              |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_FILEINFO_H
#define PHP_FILEINFO_H

extern zend_module_entry fileinfo_module_entry;
#define phpext_fileinfo_ptr &fileinfo_module_entry

#define PHP_FILEINFO_VERSION PHP_VERSION

#ifdef PHP_WIN32
#define PHP_FILEINFO_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_FILEINFO_API __attribute__((visibility("default")))
#else
#define PHP_FILEINFO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINFO_FUNCTION(fileinfo);

#endif	/* PHP_FILEINFO_H */
