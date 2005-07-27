#ifndef PHP_SYSLOG_H
#define PHP_SYSLOG_H

#ifdef PHP_WIN32
#include "win32/syslog.h"
#else
#include <php_config.h>
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
