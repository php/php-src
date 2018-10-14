--TEST--
Test DateTime::setTimezone () function : error conditions
--FILE--
<?php
/* Prototype  : public DateTime DateTime::setTimezone  ( DateTimeZone $timezone  )
 * Description: Sets the time zone for the DateTime object
 * Source code: ext/date/php_date.c
 * Alias to functions: date_timezone_set
 */

date_default_timezone_set("UTC");

echo "*** Testing DateTime::setTimezone () : error conditions ***\n";

$datetime = new DateTime("2009-01-30 17:57:32");

echo "\n-- Testing DateTime::setTimezone () function with zero arguments --\n";
var_dump( $datetime->setTimezone() );

echo "\n-- Testing DateTime::setTimezone () function with more than expected no. of arguments --\n";
$timezone  = new DateTimezone("GMT");
$extra_arg = 99;
var_dump( $datetime->setTimezone($timezone, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setTimezone () : error conditions ***

-- Testing DateTime::setTimezone () function with zero arguments --

Warning: DateTime::setTimezone() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing DateTime::setTimezone () function with more than expected no. of arguments --

Warning: DateTime::setTimezone() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===
