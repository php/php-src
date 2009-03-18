--TEST--
Test DateTime::getOffset() function : error conditions 
--FILE--
<?php

/* Prototype  : public int DateTime::getOffset  ( void  )
 * Description: Returns the daylight saving time offset
 * Source code: ext/date/php_date.c
 * Alias to functions:  date_offset_get
 */
 
 //Set the default time zone 
date_default_timezone_set("Europe/London");

echo "*** Testing DateTime::getOffset() : error conditions ***\n";

echo "\n-- Testing DateTime::getOffset() function with more than expected no. of arguments --\n";
$datetime = new DateTime("2009-01-30 19:34:10");
$extra_arg = 30;

var_dump( $datetime->getOffset($extra_arg) );

?>
===DONE===
--EXPECTF--
*** Testing DateTime::getOffset() : error conditions ***

-- Testing DateTime::getOffset() function with more than expected no. of arguments --

Warning: DateTime::getOffset() expects exactly 0 parameters, 1 given in %s on line %d
bool(false)
===DONE===

