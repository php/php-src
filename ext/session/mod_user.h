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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */

#ifndef MOD_USER_H
#define MOD_USER_H

typedef union {
	zval *names[6];
	struct {
		zval *ps_open;
		zval *ps_close;
		zval *ps_read;
		zval *ps_write;
		zval *ps_destroy;
		zval *ps_gc;
	} name;
} ps_user;

extern ps_module ps_mod_user;
#define ps_user_ptr &ps_mod_user

PS_FUNCS(user);

#endif
