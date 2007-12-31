/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2008 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_THTTPD_H
#define PHP_THTTPD_H

#include <sys/types.h>
#include <sys/stat.h>
#include <libhttpd.h>

void	 thttpd_php_shutdown(void);
void	 thttpd_php_init(void);
off_t	 thttpd_php_request(httpd_conn *hc, int show_source);

void	 thttpd_register_on_close(void (*)(int));
void	 thttpd_closed_conn(int fd);
int		 thttpd_get_fd(void);
void	 thttpd_set_dont_close(void);

#endif
