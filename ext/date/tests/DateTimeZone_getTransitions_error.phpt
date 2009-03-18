--TEST--
Test DateTimeZone::getTransitions() function : error conditions 
--FILE--
<?php
/* Prototype  : array DateTimeZone::getTransitions  ([ int $timestamp_begin  [, int $timestamp_end  ]] )
 * Description: Returns all transitions for the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_transitions_get()
 */
 
//Set the default time zone 
date_default_timezone_set("GMT");

$tz = new DateTimeZone("Europe/London");
 
echo "*** Testing DateTimeZone::getTransitions() : error conditions ***\n";

echo "\n-- Testing DateTimeZone::getTransitions() function with more than expected no. of arguments --\n";
$timestamp_begin = mktime(0, 0, 0, 1, 1, 1972);
$timestamp_end = mktime(0, 0, 0, 1, 1, 1975);
$extra_arg = 99;

var_dump( $tz->getTransitions($timestamp_begin, $timestamp_end, $extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone::getTransitions() : error conditions ***

-- Testing DateTimeZone::getTransitions() function with more than expected no. of arguments --

Warning: DateTimeZone::getTransitions() expects at most 2 parameters, 3 given in %s on line %d
bool(false)
===DONE===