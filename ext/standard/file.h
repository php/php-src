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
PHP_FUNCTION(tmpfile);
PHP_FUNCTION(fopen);
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
PHP_FUNCTION(ftruncate);
PHP_FUNCTION(fstat);

PHPAPI int php_set_sock_blocking(int socketd, int block);
PHPAPI int php_file_le_fopen(void);
PHPAPI int php_file_le_popen(void);
PHPAPI int php_file_le_socket(void);
PHPAPI int php_copy_file(char *src, char *dest);
#if 0
PHPAPI FILE *php_open_temporary_file(const char *dir, const char *pfx, char **opened_path_p);
#endif

#endif /* FILE_H */
