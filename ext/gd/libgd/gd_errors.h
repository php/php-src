#ifndef GD_ERRORS_H
#define GD_ERRORS_H

#ifndef _WIN32
# include <syslog.h>
#else
# include "win32/syslog.h"
#endif

/*
LOG_EMERG      system is unusable
LOG_ALERT      action must be taken immediately
LOG_CRIT       critical conditions
LOG_ERR        error conditions
LOG_WARNING    warning conditions
LOG_NOTICE     normal, but significant, condition
LOG_INFO       informational message
LOG_DEBUG      debug-level message
*/

#define GD_ERROR LOG_ERR
#define GD_WARNING LOG_WARNING
#define GD_NOTICE LOG_NOTICE
#define GD_INFO LOG_INFO
#define GD_DEBUG LOG_DEBUG

void gd_error(const char *format, ...);
void gd_error_ex(int priority, const char *format, ...);

#endif
