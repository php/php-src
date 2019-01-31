/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#if HAVE_FTP

extern zend_module_entry php_ftp_module_entry;
#define php_ftp_module_ptr &php_ftp_module_entry

#include "php_version.h"
#define PHP_FTP_VERSION PHP_VERSION

#define PHP_FTP_OPT_TIMEOUT_SEC	0
#define PHP_FTP_OPT_AUTOSEEK	1
#define PHP_FTP_OPT_USEPASVADDRESS	2
#define PHP_FTP_AUTORESUME		-1

PHP_MINIT_FUNCTION(ftp);
PHP_MINFO_FUNCTION(ftp);

PHP_FUNCTION(ftp_connect);
#ifdef HAVE_FTP_SSL
PHP_FUNCTION(ftp_ssl_connect);
#endif
PHP_FUNCTION(ftp_login);
PHP_FUNCTION(ftp_pwd);
PHP_FUNCTION(ftp_cdup);
PHP_FUNCTION(ftp_chdir);
PHP_FUNCTION(ftp_exec);
PHP_FUNCTION(ftp_raw);
PHP_FUNCTION(ftp_mkdir);
PHP_FUNCTION(ftp_rmdir);
PHP_FUNCTION(ftp_chmod);
PHP_FUNCTION(ftp_alloc);
PHP_FUNCTION(ftp_nlist);
PHP_FUNCTION(ftp_rawlist);
PHP_FUNCTION(ftp_mlsd);
PHP_FUNCTION(ftp_systype);
PHP_FUNCTION(ftp_pasv);
PHP_FUNCTION(ftp_get);
PHP_FUNCTION(ftp_fget);
PHP_FUNCTION(ftp_put);
PHP_FUNCTION(ftp_append);
PHP_FUNCTION(ftp_fput);
PHP_FUNCTION(ftp_size);
PHP_FUNCTION(ftp_mdtm);
PHP_FUNCTION(ftp_rename);
PHP_FUNCTION(ftp_delete);
PHP_FUNCTION(ftp_site);
PHP_FUNCTION(ftp_close);
PHP_FUNCTION(ftp_set_option);
PHP_FUNCTION(ftp_get_option);
PHP_FUNCTION(ftp_nb_get);
PHP_FUNCTION(ftp_nb_fget);
PHP_FUNCTION(ftp_nb_put);
PHP_FUNCTION(ftp_nb_fput);
PHP_FUNCTION(ftp_nb_continue);

#define phpext_ftp_ptr php_ftp_module_ptr

#else
#define php_ftp_module_ptr NULL
#endif	/* HAVE_FTP */

#endif
