--TEST--
Test DateTime::setDate() function : error conditions 
--FILE--
<?php
/* Prototype  : public DateTime DateTime::setDate  ( int $year  , int $month  , int $day  )
 * Description: Resets the current date of the DateTime object to a different date. 
 * Source code: ext/date/php_date.c
 * Alias to functions: date_date_set()
 */
 
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::setDate() : error conditions ***\n";

$datetime = new DateTime("2009-01-30 19:34:10");

echo "\n-- Testing DateTime::setDate() function with zero arguments --\n";
var_dump( $datetime->setDate() );

echo "\n-- Testing DateTime::setDate() function with less than expected no. of arguments --\n";
$year = 2009;
$month = 1;
$day = 30;
var_dump( $datetime->setDate($year) );
var_dump( $datetime->setDate($year, $month) );

echo "\n-- Testing DateTime::setDate() function with more than expected no. of arguments --\n";
$extra_arg = 10;
var_dump( $datetime->setDate($year, $month, $day, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setDate() : error conditions ***

-- Testing DateTime::setDate() function with zero arguments --

Warning: DateTime::setDate() expects exactly 3 parameters, 0 given in %s on line %d
bool(false)

-- Testing DateTime::setDate() function with less than expected no. of arguments --

Warning: DateTime::setDate() expects exactly 3 parameters, 1 given in %s on line %d
bool(false)

Warning: DateTime::setDate() expects exactly 3 parameters, 2 given in %s on line %d
bool(false)

-- Testing DateTime::setDate() function with more than expected no. of arguments --

Warning: DateTime::setDate() expects exactly 3 parameters, 4 given in %s on line %d
bool(false)
===DONE===
