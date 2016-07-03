/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2016 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Alex Waugh <alex@alexwaugh.com>                              |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_WEBJAMES_H
#define PHP_WEBJAMES_H

#include "webjames.h"

void webjames_php_shutdown(void);
int webjames_php_init(void);
void webjames_php_request(struct connection *conn);

#endif
