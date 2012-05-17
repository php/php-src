/*
   +----------------------------------------------------------------------+
   | PHP Version 5                                                        |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@netcabo.pt>                       |
   +----------------------------------------------------------------------+
 */

#ifndef CALENDAR_METHODS_H
#define CALENDAR_METHODS_H

#include <php.h>

PHP_METHOD(IntlCalendar, __construct);

PHP_FUNCTION(intlcal_create_instance);

PHP_FUNCTION(intlcal_get_keyword_values_for_locale);

PHP_FUNCTION(intlcal_get_now);

PHP_FUNCTION(intlcal_get_available_locales);

PHP_FUNCTION(intlcal_get);

PHP_FUNCTION(intlcal_get_time);

PHP_FUNCTION(intlcal_set_time);

PHP_FUNCTION(intlcal_add);

PHP_FUNCTION(intlcal_set_time_zone);

PHP_FUNCTION(intlcal_after);

PHP_FUNCTION(intlcal_before);

PHP_FUNCTION(intlcal_set);

PHP_FUNCTION(intlcal_roll);

PHP_FUNCTION(intlcal_clear);

PHP_FUNCTION(intlcal_field_difference);

PHP_FUNCTION(intlcal_get_actual_maximum);

PHP_FUNCTION(intlcal_get_actual_minimum);

PHP_FUNCTION(intlcal_get_day_of_week_type);

PHP_FUNCTION(intlcal_get_first_day_of_week);

PHP_FUNCTION(intlcal_get_greatest_minimum);

PHP_FUNCTION(intlcal_get_least_maximum);

PHP_FUNCTION(intlcal_get_locale);

PHP_FUNCTION(intlcal_get_maximum);

PHP_FUNCTION(intlcal_get_minimal_days_in_first_week);

PHP_FUNCTION(intlcal_get_minimum);

PHP_FUNCTION(intlcal_get_time_zone);

PHP_FUNCTION(intlcal_get_type);

PHP_FUNCTION(intlcal_get_weekend_transition);

PHP_FUNCTION(intlcal_in_daylight_time);

PHP_FUNCTION(intlcal_is_equivalent_to);

PHP_FUNCTION(intlcal_is_lenient);

PHP_FUNCTION(intlcal_is_set);

PHP_FUNCTION(intlcal_is_weekend);

PHP_FUNCTION(intlcal_set_first_day_of_week);

PHP_FUNCTION(intlcal_set_lenient);

PHP_FUNCTION(intlcal_equals);

PHP_FUNCTION(intlcal_get_repeated_wall_time_option);

PHP_FUNCTION(intlcal_get_skipped_wall_time_option);

PHP_FUNCTION(intlcal_set_repeated_wall_time_option);

PHP_FUNCTION(intlcal_set_skipped_wall_time_option);

PHP_FUNCTION(intlcal_from_date_time);

PHP_FUNCTION(intlcal_to_date_time);

PHP_FUNCTION(intlcal_get_error_code);

PHP_FUNCTION(intlcal_get_error_message);

#endif /* #ifndef CALENDAR_METHODS_H */
