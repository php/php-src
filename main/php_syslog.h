#ifndef PHP_SYSLOG_H
#define PHP_SYSLOG_H

#ifdef PHP_WIN32
#include "win32/syslog.h"
#include "win32/php_registry.h"
#else
#include <syslog.h>
#endif

/*
 * SCO OpenServer 5 defines syslog to var_syslog/sys_syslog which
 * causes trouble with our use of syslog. We define php_syslog
 * to be the system function syslog.
 */

#ifdef syslog

#if defined(var_syslog) && var_syslog == syslog
#define php_syslog var_syslog
#elif defined(sys_syslog) && sys_syslog == syslog
#define php_syslog sys_syslog
#endif

#endif

#ifndef php_syslog
#define php_syslog syslog
#undef syslog
#endif


#endif
