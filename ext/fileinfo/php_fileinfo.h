/*
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.0 of the PHP license,       |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_0.txt.                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
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
#else
#define PHP_FILEINFO_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINFO_FUNCTION(fileinfo);

#endif	/* PHP_FILEINFO_H */
