/*
   +----------------------------------------------------------------------+
   | Zend OPcache                                                         |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2018 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <zeev@php.net>                                 |
   |          Stanislav Malyshev <stas@zend.com>                          |
   |          Dmitry Stogov <dmitry@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_ACCELERATOR_DEBUG_H
#define ZEND_ACCELERATOR_DEBUG_H

#define ACCEL_LOG_FATAL					0
#define ACCEL_LOG_ERROR					1
#define ACCEL_LOG_WARNING				2
#define ACCEL_LOG_INFO					3
#define ACCEL_LOG_DEBUG					4

void zend_accel_error(int type, const char *format, ...) ZEND_ATTRIBUTE_FORMAT(printf, 2, 3);

#endif /* _ZEND_ACCELERATOR_DEBUG_H */
