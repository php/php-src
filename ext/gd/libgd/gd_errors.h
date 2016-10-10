#ifndef GD_ERRORS_H
#define GD_ERRORS_H

#ifndef _WIN32
# include <syslog.h>
#else
/*
 * priorities/facilities are encoded into a single 32-bit quantity, where the
 * bottom 3 bits are the priority (0-7) and the top 28 bits are the facility
 * (0-big number).  Both the priorities and the facilities map roughly
 * one-to-one to strings in the syslogd(8) source code.  This mapping is
 * included in this file.
 *
 * priorities (these are ordered)
 */
# define LOG_EMERG       0       /* system is unusable */
# define LOG_ALERT       1       /* action must be taken immediately */
# define LOG_CRIT        2       /* critical conditions */
# define LOG_ERR         3       /* error conditions */
# define LOG_WARNING     4       /* warning conditions */
# define LOG_NOTICE      5       /* normal but significant condition */
# define LOG_INFO        6       /* informational */
# define LOG_DEBUG       7       /* debug-level messages */
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
