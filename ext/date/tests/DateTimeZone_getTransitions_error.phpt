--TEST--
Test DateTimeZone::getTransitions() function : error conditions 
--FILE--
<?php
/* Prototype  : array DateTimeZone::getTransitions ()
 * Description: Returns all transitions for the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_transitions_get()
 */
 
//Set the default time zone 
date_default_timezone_set("GMT");

$tz = new DateTimeZone("Europe/London");
 
echo "*** Testing DateTimeZone::getTransitions() : error conditions ***\n";

echo "\n-- Testing DateTimeZone::getTransitions() function with more than expected no. of arguments --\n";
$extra_arg = 99;
var_dump( $tz->getTransitions($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone::getTransitions() : error conditions ***

-- Testing DateTimeZone::getTransitions() function with more than expected no. of arguments --

Warning: DateTimeZone::getTransitions() expects exactly 0 parameters, 1 given in %s on line %d
bool(false)
===DONE===
