--TEST--
Test strftime() function : usage variation - Checking large positive and negative float values to timestamp.
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
/* Prototype  : string strftime(string format [, int timestamp])
 * Description: Format a local time/date according to locale settings
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing strftime() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
setlocale(LC_ALL, "en_US");
date_default_timezone_set("UTC");
$format = '%b %d %Y %H:%M:%S';

echo "\n-- Testing strftime() function with float 12.3456789000e10 to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( strftime($format, $timestamp) );

echo "\n-- Testing strftime() function with float -12.3456789000e10 to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( strftime($format, $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing strftime() : usage variation ***

-- Testing strftime() function with float 12.3456789000e10 to timestamp --

Warning: strftime() expects parameter 2 to be int, float given in %s on line %d
bool(false)

-- Testing strftime() function with float -12.3456789000e10 to timestamp --

Warning: strftime() expects parameter 2 to be int, float given in %s on line %d
bool(false)
===DONE===
