--TEST--
Test DateTime::setTime() function : error conditions 
--FILE--
<?php
/* Prototype  : public DateTime DateTime::setTime  ( int $hour  , int $minute  [, int $second  ] )
 * Description: Resets the current time of the DateTime object to a different time. 
 * Source code: ext/date/php_date.c
 * Alias to functions: date_time_set
 */
 
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::setTime() : error conditions ***\n";

$datetime = date_create("2009-01-31 15:34:10");

echo "\n-- Testing DateTime::setTime() function with zero arguments --\n";
var_dump( $datetime->setTime() );

echo "\n-- Testing DateTime::setTime() function with less than expected no. of arguments --\n";
$hour = 18;
var_dump( $datetime->setTime($hour) );

echo "\n-- Testing DateTime::setTime() function with more than expected no. of arguments --\n";
$min = 15;
$sec = 30;
$extra_arg = 10;
var_dump( $datetime->setTime($hour, $min, $sec, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setTime() : error conditions ***

-- Testing DateTime::setTime() function with zero arguments --

Warning: DateTime::setTime() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

-- Testing DateTime::setTime() function with less than expected no. of arguments --

Warning: DateTime::setTime() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

-- Testing DateTime::setTime() function with more than expected no. of arguments --

Warning: DateTime::setTime() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
===DONE===
