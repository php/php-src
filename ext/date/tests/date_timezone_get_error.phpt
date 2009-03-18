--TEST--
Test date_timezone_get() function : error conditions 
--FILE--
<?php
/* Prototype  : DateTimeZone date_timezone_get  ( DateTime $object  )
 * Description: Return time zone relative to given DateTime
 * Source code: ext/date/php_date.c
 * Alias to functions: DateTime::getTimezone
 */
 
// Set timezone 
date_default_timezone_set("Europe/London");

echo "*** Testing date_timezone_get() : error conditions ***\n";

echo "\n-- Testing date_timezone_get() function with zero arguments --\n";
var_dump( date_timezone_get() ); 

echo "\n-- Testing date_timezone_get() function with more than expected no. of arguments --\n";
$datetime = date_create("2009-01-30 17:57:32");
$extra_arg = 99;
var_dump( date_timezone_get($datetime, $extra_arg) );

echo "\n-- Testing date_timezone_get() function with an invalid values for \$object argument --\n";
$invalid_obj = new stdClass();
var_dump( date_timezone_get($invalid_obj) );  
$invalid_obj = 10;
var_dump( date_timezone_get($invalid_obj) );
$invalid_obj = null;
var_dump( date_timezone_get($invalid_obj) ); 
?>
===DONE===
--EXPECTF--
*** Testing date_timezone_get() : error conditions ***

-- Testing date_timezone_get() function with zero arguments --

Warning: date_timezone_get() expects exactly 1 parameter, 0 given in %s on line %d
bool(false)

-- Testing date_timezone_get() function with more than expected no. of arguments --

Warning: date_timezone_get() expects exactly 1 parameter, 2 given in %s on line %d
bool(false)

-- Testing date_timezone_get() function with an invalid values for $object argument --

Warning: date_timezone_get() expects parameter 1 to be DateTime, object given in %s on line %d
bool(false)

Warning: date_timezone_get() expects parameter 1 to be DateTime, integer given in %s on line %d
bool(false)

Warning: date_timezone_get() expects parameter 1 to be DateTime, null given in %s on line %d
bool(false)
===DONE===
