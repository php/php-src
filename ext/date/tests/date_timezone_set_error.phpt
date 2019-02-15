--TEST--
Test date_timezone_set() function : error conditions
--FILE--
<?php
/* Prototype  : DateTime date_timezone_set  ( DateTime $object  , DateTimeZone $timezone  )
 * Description: Sets the time zone for the DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::setTimezone
 */

date_default_timezone_set("UTC");

echo "*** Testing date_timezone_set() : error conditions ***\n";

echo "\n-- Testing date_timezone_set() function with zero arguments --\n";
var_dump( date_timezone_set() );

echo "\n-- Testing date_timezone_set() function with less than expected no. of arguments --\n";
$datetime = date_create("2009-01-30 17:57:32");
var_dump( date_timezone_set($datetime) );

echo "\n-- Testing date_timezone_set() function with more than expected no. of arguments --\n";
$timezone  = timezone_open("GMT");
$extra_arg = 99;
var_dump( date_timezone_set($datetime, $timezone, $extra_arg) );

echo "\n-- Testing date_timezone_set() function with an invalid values for \$object argument --\n";
$invalid_obj = new stdClass();
var_dump( date_timezone_set($invalid_obj, $timezone) );
$invalid_obj = 10;
var_dump( date_timezone_set($invalid_obj, $timezone) );
$invalid_obj = null;
var_dump( date_timezone_set($invalid_obj, $timezone) );
?>
===DONE===
--EXPECTF--
*** Testing date_timezone_set() : error conditions ***

-- Testing date_timezone_set() function with zero arguments --

Warning: date_timezone_set() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

-- Testing date_timezone_set() function with less than expected no. of arguments --

Warning: date_timezone_set() expects exactly 2 parameters, 1 given in %s on line %d
bool(false)

-- Testing date_timezone_set() function with more than expected no. of arguments --

Warning: date_timezone_set() expects exactly 2 parameters, 3 given in %s on line %d
bool(false)

-- Testing date_timezone_set() function with an invalid values for $object argument --

Warning: date_timezone_set() expects parameter 1 to be DateTime, object given in %s on line %d
bool(false)

Warning: date_timezone_set() expects parameter 1 to be DateTime, int given in %s on line %d
bool(false)

Warning: date_timezone_set() expects parameter 1 to be DateTime, null given in %s on line %d
bool(false)
===DONE===
