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
   | Author: Stig Sæther Bakken <ssb@php.net>                             |
   +----------------------------------------------------------------------+
*/

#ifndef PHP_EXT_SYSLOG_H
#define PHP_EXT_SYSLOG_H

#ifdef HAVE_SYSLOG_H

#include "php_syslog.h"

PHP_MINIT_FUNCTION(syslog);
PHP_RSHUTDOWN_FUNCTION(syslog);

#endif

#endif /* PHP_EXT_SYSLOG_H */
