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
   | Authors: Andrew Skalski <askalski@chek.com>                          |
   |          Stefan Esser <sesser@php.net> (resume functions)            |
   +----------------------------------------------------------------------+
 */

#ifndef	_INCLUDED_FTP_H
#define	_INCLUDED_FTP_H

extern zend_module_entry php_ftp_module_entry;
#define phpext_ftp_ptr &php_ftp_module_entry

#include "php_version.h"
#define PHP_FTP_VERSION PHP_VERSION

#define PHP_FTP_OPT_TIMEOUT_SEC	0
#define PHP_FTP_OPT_AUTOSEEK	1
#define PHP_FTP_OPT_USEPASVADDRESS	2
#define PHP_FTP_AUTORESUME		-1

PHP_MINIT_FUNCTION(ftp);
PHP_MINFO_FUNCTION(ftp);

#endif
