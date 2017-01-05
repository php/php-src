/*
   +----------------------------------------------------------------------+
   | PHP Version 7                                                        |
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

#ifndef TIMEZONE_METHODS_H
#define TIMEZONE_METHODS_H

#include <php.h>

PHP_METHOD(IntlTimeZone, __construct);

PHP_FUNCTION(intltz_create_time_zone);

PHP_FUNCTION(intltz_from_date_time_zone);

PHP_FUNCTION(intltz_create_default);

PHP_FUNCTION(intltz_get_id);

PHP_FUNCTION(intltz_get_gmt);

PHP_FUNCTION(intltz_get_unknown);

PHP_FUNCTION(intltz_create_enumeration);

PHP_FUNCTION(intltz_count_equivalent_ids);

PHP_FUNCTION(intltz_create_time_zone_id_enumeration);

PHP_FUNCTION(intltz_get_canonical_id);

PHP_FUNCTION(intltz_get_region);

PHP_FUNCTION(intltz_get_tz_data_version);

PHP_FUNCTION(intltz_get_equivalent_id);

PHP_FUNCTION(intltz_use_daylight_time);

PHP_FUNCTION(intltz_get_offset);

PHP_FUNCTION(intltz_get_raw_offset);

PHP_FUNCTION(intltz_has_same_rules);

PHP_FUNCTION(intltz_get_display_name);

PHP_FUNCTION(intltz_get_dst_savings);

PHP_FUNCTION(intltz_to_date_time_zone);

PHP_FUNCTION(intltz_get_error_code);

PHP_FUNCTION(intltz_get_error_message);

#endif /* #ifndef TIMEZONE_METHODS_H */
