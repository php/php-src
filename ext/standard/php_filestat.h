/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2002 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author:  Jim Winstead <jimw@php.net>                                 |
   +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FILESTAT_H
#define PHP_FILESTAT_H

PHP_RINIT_FUNCTION(filestat);
PHP_RSHUTDOWN_FUNCTION(filestat);

PHP_FUNCTION(clearstatcache);
PHP_FUNCTION(fileatime);
PHP_FUNCTION(filectime);
PHP_FUNCTION(filegroup);
PHP_FUNCTION(fileinode);
PHP_FUNCTION(filemtime);
PHP_FUNCTION(fileowner);
PHP_FUNCTION(fileperms);
PHP_FUNCTION(filesize);
PHP_FUNCTION(filetype);
PHP_FUNCTION(is_writable);
PHP_FUNCTION(is_readable);
#ifndef PHP_WIN32
PHP_FUNCTION(is_executable);
#endif
PHP_FUNCTION(is_file);
PHP_FUNCTION(is_dir);
PHP_FUNCTION(is_link);
PHP_FUNCTION(file_exists);
PHP_NAMED_FUNCTION(php_if_stat);
PHP_NAMED_FUNCTION(php_if_lstat);
PHP_FUNCTION(disk_total_space);
PHP_FUNCTION(disk_free_space);
PHP_FUNCTION(chown);
PHP_FUNCTION(chgrp);
PHP_FUNCTION(chmod);
#if HAVE_UTIME
PHP_FUNCTION(touch);
#endif
PHP_FUNCTION(clearstatcache);

#define MAKE_LONG_ZVAL_INCREF(name, val)\
	MAKE_STD_ZVAL(name); \
	ZVAL_LONG(name, val); \
	name->refcount++; 

#ifdef PHP_WIN32
#define S_IRUSR S_IREAD
#define S_IWUSR S_IWRITE
#define S_IXUSR S_IEXEC
#define S_IRGRP S_IREAD
#define S_IWGRP S_IWRITE
#define S_IXGRP S_IEXEC
#define S_IROTH S_IREAD
#define S_IWOTH S_IWRITE
#define S_IXOTH S_IEXEC

#undef getgid
#define getgroups(a, b) 0
#define getgid() 1
#define getuid() 1
#endif

#endif /* PHP_FILESTAT_H */
