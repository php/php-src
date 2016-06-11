--TEST--
Test gmmktime() function : basic functionality 
--FILE--
<?php
/* Prototype  : int gmmktime([int hour [, int min [, int sec [, int mon [, int day [, int year]]]]]])
 * Description: Get UNIX timestamp for a GMT date 
 * Source code: ext/date/php_date.c
 * Alias to functions: 
 */

echo "*** Testing gmmktime() : basic functionality ***\n";

// Initialise all required variables
$hour = 8;
$min = 8;
$sec = 8;
$mon = 8;
$day = 8;
$year = 2008;

// Calling gmmktime() with all possible arguments
var_dump( gmmktime($hour, $min, $sec, $mon, $day, $year) );

// Calling gmmktime() with mandatory arguments
var_dump( gmmktime() );

?>
===DONE===
--EXPECTF--
*** Testing gmmktime() : basic functionality ***
int(1218182888)

Deprecated: gmmktime(): You should be using the time() function instead in %s on line %d
int(%d)
===DONE===
