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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_SYSLOG_H
#define PHP_SYSLOG_H

#ifdef PHP_WIN32
#include "win32/syslog.h"
#elif defined(NETWARE)
# include "config.nw.h"
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif
#else
#include "php_config.h"
#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif
#endif

/* 
 * The SCO OpenServer 5 Development System (not the UDK)
 * defines syslog to std_syslog.
 */

#ifdef syslog

#ifdef HAVE_STD_SYSLOG
#define php_syslog std_syslog
#endif

#undef syslog

#endif

#ifndef php_syslog
#define php_syslog syslog
#endif

#endif
