/* $Id$ */

#ifndef	_INCLUDED_FTP_H
#define	_INCLUDED_FTP_H

#if COMPILE_DL
#undef HAVE_FTP
#define HAVE_FTP 1
#endif

#if HAVE_FTP

extern php3_module_entry php3_ftp_module_entry;
#define php3_ftp_module_ptr &php3_ftp_module_entry

#ifdef ZEND_VERSION
PHP_MINIT_FUNCTION(ftp);
#else
int PHP_MINIT_FUNCTION(INIT_FUNC_ARGS);
#endif

PHP_FUNCTION(ftp_connect);
PHP_FUNCTION(ftp_login);
PHP_FUNCTION(ftp_pwd);
PHP_FUNCTION(ftp_cdup);
PHP_FUNCTION(ftp_chdir);
PHP_FUNCTION(ftp_mkdir);
PHP_FUNCTION(ftp_rmdir);
PHP_FUNCTION(ftp_nlist);
PHP_FUNCTION(ftp_rawlist);
PHP_FUNCTION(ftp_systype);
PHP_FUNCTION(ftp_get);
PHP_FUNCTION(ftp_put);
PHP_FUNCTION(ftp_quit);

#define phpext_ftp_ptr php3_ftp_module_ptr

#else
#define php3_ftp_module_ptr NULL
#endif	/* HAVE_FTP */

#endif
