/*
   +----------------------------------------------------------------------+
   | PHP Version 4                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2003 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Wez Furlong <wez@thebrainroom.com>                           |
   +----------------------------------------------------------------------+
 */
/* $Id$ */

#ifdef PHP_WIN32
typedef HANDLE php_file_descriptor_t;
typedef HANDLE php_process_id_t;
#else
typedef int php_file_descriptor_t;
typedef pid_t php_process_id_t;
#endif

#define PHP_PROC_OPEN_MAX_DESCRIPTORS	16

struct php_process_handle {
	php_process_id_t	child;
	int npipes;
	long pipes[PHP_PROC_OPEN_MAX_DESCRIPTORS];
	char *command;
	int is_persistent;
};

