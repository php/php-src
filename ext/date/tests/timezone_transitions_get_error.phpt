--TEST--
Test timezone_transitions_get() function : error conditions
--FILE--
<?php
/* Prototype  : array timezone_transitions_get  ( DateTimeZone $object, [ int $timestamp_begin  [, int $timestamp_end  ]]  )
 * Description: Returns all transitions for the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTimeZone::getTransitions()
 */

//Set the default time zone
date_default_timezone_set("GMT");
$tz = timezone_open("Europe/London");

echo "*** Testing timezone_transitions_get() : error conditions ***\n";

echo "\n-- Testing timezone_transitions_get() function with zero arguments --\n";
var_dump( timezone_transitions_get() );

echo "\n-- Testing timezone_transitions_get() function with more than expected no. of arguments --\n";
$timestamp_begin = mktime(0, 0, 0, 1, 1, 1972);
$timestamp_end = mktime(0, 0, 0, 1, 1, 1975);
$extra_arg = 99;
var_dump( timezone_transitions_get($tz, $timestamp_begin, $timestamp_end, $extra_arg) );

echo "\n-- Testing timezone_transitions_get() function with an invalid values for \$object argument --\n";
$invalid_obj = new stdClass();
var_dump( timezone_transitions_get($invalid_obj) );
$invalid_obj = 10;
var_dump( timezone_transitions_get($invalid_obj) );
$invalid_obj = null;
var_dump( timezone_transitions_get($invalid_obj) );
?>
===DONE===
--EXPECTF--
*** Testing timezone_transitions_get() : error conditions ***

-- Testing timezone_transitions_get() function with zero arguments --

Warning: timezone_transitions_get() expects at least 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing timezone_transitions_get() function with more than expected no. of arguments --

Warning: timezone_transitions_get() expects at most 3 parameters, 4 given in %s on line %d
bool(false)

-- Testing timezone_transitions_get() function with an invalid values for $object argument --

Warning: timezone_transitions_get() expects parameter 1 to be DateTimeZone, object given in %s on line %d
bool(false)

Warning: timezone_transitions_get() expects parameter 1 to be DateTimeZone, integer given in %s on line %d
bool(false)

Warning: timezone_transitions_get() expects parameter 1 to be DateTimeZone, null given in %s on line %d
bool(false)
===DONE===
