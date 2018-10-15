--TEST--
Test gmdate() function : usage variation - Passing high positive and negetive float values to timestamp.
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
/* Prototype  : string gmdate(string format [, long timestamp])
 * Description: Format a GMT date/time
 * Source code: ext/date/php_date.c
 */

echo "*** Testing gmdate() : usage variation ***\n";

// Initialise all required variables
date_default_timezone_set('UTC');
$format = DATE_ISO8601;

echo "\n-- Testing gmdate() function with float 12.3456789000e10 to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( gmdate($format, $timestamp) );

echo "\n-- Testing gmdate() function with float -12.3456789000e10 to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( gmdate($format, $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing gmdate() : usage variation ***

-- Testing gmdate() function with float 12.3456789000e10 to timestamp --

Warning: gmdate() expects parameter 2 to be int, float given in %s on line %d
bool(false)

-- Testing gmdate() function with float -12.3456789000e10 to timestamp --

Warning: gmdate() expects parameter 2 to be int, float given in %s on line %d
bool(false)
===DONE===
