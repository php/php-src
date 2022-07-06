#ifndef PHP_CALENDAR_H
#define PHP_CALENDAR_H

extern zend_module_entry calendar_module_entry;
#define	calendar_module_ptr &calendar_module_entry

#include "php_version.h"
#define PHP_CALENDAR_VERSION PHP_VERSION

/* Functions */

PHP_MINIT_FUNCTION(calendar);
PHP_MINFO_FUNCTION(calendar);

#define phpext_calendar_ptr calendar_module_ptr

/*
 * Specifying the easter calculation method
 *
 * DEFAULT is Anglican, ie. use Julian calendar before 1753
 * and Gregorian after that. With ROMAN, the cutoff year is 1582.
 * ALWAYS_GREGORIAN and ALWAYS_JULIAN force the calendar
 * regardless of date.
 *
 */

#define CAL_EASTER_DEFAULT			0
#define CAL_EASTER_ROMAN			1
#define CAL_EASTER_ALWAYS_GREGORIAN	2
#define CAL_EASTER_ALWAYS_JULIAN	3

#endif
