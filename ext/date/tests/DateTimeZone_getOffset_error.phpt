--TEST--
Test DateTimeZone::getOffset() function : error conditions
--FILE--
<?php
/* Prototype  : int DateTimeZone::getOffset  ( DateTime $datetime  )
 * Description: Returns the timezone offset from GMT
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_offset_get()
 */

//Set the default time zone
date_default_timezone_set("GMT");

$tz = new DateTimeZone("Europe/London");
$date = date_create("GMT");

echo "*** Testing DateTimeZone::getOffset() : error conditions ***\n";

echo "\n-- Testing DateTimeZone::getOffset() function with zero arguments --\n";
var_dump( $tz->getOffset() );

echo "\n-- Testing DateTimeZone::getOffset() function with more than expected no. of arguments --\n";
$extra_arg = 99;
var_dump( $tz->getOffset($date, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone::getOffset() : error conditions ***

-- Testing DateTimeZone::getOffset() function with zero arguments --

Warning: DateTimeZone::getOffset() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing DateTimeZone::getOffset() function with more than expected no. of arguments --

Warning: DateTimeZone::getOffset() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)
===DONE===
