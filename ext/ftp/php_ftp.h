/*
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
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
#include "php_network.h"
#define PHP_FTP_VERSION PHP_VERSION

#define PHP_FTP_OPT_TIMEOUT_SEC	0
#define PHP_FTP_OPT_AUTOSEEK	1
#define PHP_FTP_OPT_USEPASVADDRESS	2
#define PHP_FTP_AUTORESUME		-1

PHP_MINIT_FUNCTION(ftp);
PHP_MINFO_FUNCTION(ftp);

#ifdef PHP_WIN32
#define PHP_FTP_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#define PHP_FTP_API __attribute__ ((visibility("default")))
#else
#define PHP_FTP_API
#endif

/* expose this interface to enable coroutine support for FTP in other extensions
 */
PHP_FTP_API extern int(*php_ftp_pollfd_for_ms)(php_socket_t, int, int);

#endif
