/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) The PHP Group                                          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Author: Philip Prindeville <philipp@redfish-solutions.com>           |
   +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "php.h"
#include "php_syslog.h"

#include "zend.h"
#include "zend_smart_string.h"

/*
 * The SCO OpenServer 5 Development System (not the UDK)
 * defines syslog to std_syslog.
 */

#ifdef HAVE_STD_SYSLOG
#define syslog std_syslog
#endif

#ifdef PHP_WIN32
PHPAPI void php_syslog(int priority, const char *format, ...) /* {{{ */
{
	va_list args;

	/*
	 * don't rely on openlog() being called by syslog() if it's
	 * not already been done; call it ourselves and pass the
	 * correct parameters!
	 */
	if (!PG(have_called_openlog)) {
		php_openlog(PG(syslog_ident), 0, PG(syslog_facility));
	}

	va_start(args, format);
	vsyslog(priority, format, args);
	va_end(args);
}
/* }}} */
#else
PHPAPI void php_syslog(int priority, const char *format, ...) /* {{{ */
{
	const char *ptr;
	unsigned char c;
	smart_string fbuf = {0};
	smart_string sbuf = {0};
	va_list args;

	/*
	 * don't rely on openlog() being called by syslog() if it's
	 * not already been done; call it ourselves and pass the
	 * correct parameters!
	 */
	if (!PG(have_called_openlog)) {
		php_openlog(PG(syslog_ident), 0, PG(syslog_facility));
	}

	va_start(args, format);
	zend_printf_to_smart_string(&fbuf, format, args);
	smart_string_0(&fbuf);
	va_end(args);

	if (PG(syslog_filter) == PHP_SYSLOG_FILTER_RAW) {
		/* Just send it directly to the syslog */
		syslog(priority, "%.*s", (int)fbuf.len, fbuf.c);
		smart_string_free(&fbuf);
		return;
	}

	for (ptr = fbuf.c; ; ++ptr) {
		c = *ptr;
		if (c == '\0') {
			syslog(priority, "%.*s", (int)sbuf.len, sbuf.c);
			break;
		}

		/* check for NVT ASCII only unless test disabled */
		if (((0x20 <= c) && (c <= 0x7e)))
			smart_string_appendc(&sbuf, c);
		else if ((c >= 0x80) && (PG(syslog_filter) != PHP_SYSLOG_FILTER_ASCII))
			smart_string_appendc(&sbuf, c);
		else if (c == '\n') {
			syslog(priority, "%.*s", (int)sbuf.len, sbuf.c);
			smart_string_reset(&sbuf);
		} else if ((c < 0x20) && (PG(syslog_filter) == PHP_SYSLOG_FILTER_ALL))
			smart_string_appendc(&sbuf, c);
		else {
			const char xdigits[] = "0123456789abcdef";

			smart_string_appendl(&sbuf, "\\x", 2);
			smart_string_appendc(&sbuf, xdigits[(c / 0x10)]);
			c &= 0x0f;
			smart_string_appendc(&sbuf, xdigits[c]);
		}
	}

	smart_string_free(&fbuf);
	smart_string_free(&sbuf);
}
/* }}} */
#endif
