/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

/* Synced with php 3.0 revision 1.30 1999-06-16 [ssb] */

#ifndef FILE_H
#define FILE_H

extern PHP_MINIT_FUNCTION(file);

PHP_FUNCTION(tempnam);
PHP_NAMED_FUNCTION(php_if_tmpfile);
PHP_NAMED_FUNCTION(php_if_fopen);
PHP_FUNCTION(fclose);
PHP_FUNCTION(popen);
PHP_FUNCTION(pclose);
PHP_FUNCTION(feof);
PHP_FUNCTION(fread);
PHP_FUNCTION(fgetc);
PHP_FUNCTION(fgets);
PHP_FUNCTION(fscanf);
PHP_FUNCTION(fgetss);
PHP_FUNCTION(fgetcsv);
PHP_FUNCTION(fwrite);
PHP_FUNCTION(fflush);
PHP_FUNCTION(rewind);
PHP_FUNCTION(ftell);
PHP_FUNCTION(fseek);
PHP_FUNCTION(mkdir);
PHP_FUNCTION(rmdir);
PHP_FUNCTION(fpassthru);
PHP_FUNCTION(readfile);
PHP_FUNCTION(umask);
PHP_FUNCTION(rename);
PHP_FUNCTION(unlink);
PHP_FUNCTION(copy);
PHP_FUNCTION(file);
PHP_FUNCTION(set_socket_blocking); /* deprecated */
PHP_FUNCTION(socket_set_blocking);
PHP_FUNCTION(socket_set_timeout);
PHP_FUNCTION(socket_get_status);
PHP_FUNCTION(set_file_buffer);
PHP_FUNCTION(get_meta_tags);
PHP_FUNCTION(flock);
PHP_FUNCTION(fd_set);
PHP_FUNCTION(fd_isset);
PHP_FUNCTION(select);
PHP_FUNCTION(realpath);
PHP_NAMED_FUNCTION(php_if_ftruncate);
PHP_NAMED_FUNCTION(php_if_fstat);

PHPAPI int php_set_sock_blocking(int socketd, int block);
PHPAPI int php_file_le_fopen(void);
PHPAPI int php_file_le_popen(void);
PHPAPI int php_file_le_socket(void);
PHPAPI int php_copy_file(char *src, char *dest);

typedef struct {
	int fgetss_state;
	int pclose_ret;
	HashTable ht_fsock_keys;
	HashTable ht_fsock_socks;
	struct php_sockbuf *phpsockbuf;
	size_t def_chunk_size;
} php_file_globals;

#ifdef ZTS
#define FLS_D php_file_globals *file_globals
#define FLS_DC , FLS_D
#define FLS_C file_globals
#define FLS_CC , FLS_C
#define FG(v) (file_globals->v)
#define FLS_FETCH() php_file_globals *file_globals = ts_resource(file_globals_id)
extern int file_globals_id;
#else
#define FLS_D	void
#define FLS_DC
#define FLS_C
#define FLS_CC
#define FG(v) (file_globals.v)
#define FLS_FETCH()
extern php_file_globals file_globals;
#endif


#endif /* FILE_H */
