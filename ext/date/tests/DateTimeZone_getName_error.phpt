--TEST--
Test DateTimeZone::getName() function : error conditions 
--FILE--
<?php
/* Prototype  : public string DateTimeZone::getName  ( void  )
 * Description: Returns the name of the timezone
 * Source code: ext/date/php_date.c
 * Alias to functions: timezone_name_get()
 */
 
//Set the default time zone 
date_default_timezone_set("GMT");

$tz = new DateTimeZone("Europe/London");
 
echo "*** Testing DateTimeZone::getName() : error conditions ***\n";

echo "\n-- Testing DateTimeZone::getName() function with more than expected no. of arguments --\n";
$extra_arg = 99;
var_dump( $tz->getName($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTimeZone::getName() : error conditions ***

-- Testing DateTimeZone::getName() function with more than expected no. of arguments --

Warning: DateTimeZone::getName() expects exactly 0 parameters, 1 given in %s on line %d
bool(false)
===DONE===
