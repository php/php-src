--TEST--
Test DateTime::setISODate () function : error conditions 
--FILE--
<?php

/* Prototype  : public DateTime DateTime::setISODate  ( int $year  , int $week  [, int $day  ] )
 * Description: Set a date according to the ISO 8601 standard - using weeks and day offsets rather than specific dates. 
 * Source code: ext/date/php_date.c
 * Alias to functions: date_isodate_set
 */
 
 //Set the default time zone 
date_default_timezone_set("Europe/London");

$datetime = new DateTime("2009-01-30 19:34:10");

echo "*** Testing DateTime::setISODate () : error conditions ***\n";

echo "\n-- Testing DateTime::setISODate() function with zero arguments --\n";
var_dump( $datetime->setISODate() );

$year = 2009;
echo "\n-- Testing DateTime::setISODate() function with less than expected no. of arguments --\n";
var_dump( $datetime->setISODate($year) );

echo "\n-- Testing date_isodate_set() function with more than expected no. of arguments --\n";
$week = 30; 
$day = 7;
$extra_arg = 30;
var_dump(  $datetime->setISODate($year, $week, $day, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::setISODate () : error conditions ***

-- Testing DateTime::setISODate() function with zero arguments --

Warning: DateTime::setISODate() expects at least 2 parameters, 0 given in %s on line %d
bool(false)

-- Testing DateTime::setISODate() function with less than expected no. of arguments --

Warning: DateTime::setISODate() expects at least 2 parameters, 1 given in %s on line %d
bool(false)

-- Testing date_isodate_set() function with more than expected no. of arguments --

Warning: DateTime::setISODate() expects at most 3 parameters, 4 given in %s on line %d
bool(false)
===DONE===
