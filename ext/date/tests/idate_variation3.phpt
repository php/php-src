--TEST--
Test idate() function : usage variation - Passing higher positive and negetive float values to timestamp.
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
/* Prototype  : int idate(string format [, int timestamp])
 * Description: Format a local time/date as integer
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing idate() : usage variation ***\n";

// Initialise function arguments not being substituted (if any)
$format = 'Y';
date_default_timezone_set("Asia/Calcutta");

echo "\n-- Testing idate() function with float 12.3456789000e10 to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( idate($format, $timestamp) );

echo "\n-- Testing idate() function with float -12.3456789000e10 to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( idate($format, $timestamp) );

?>
===DONE===
--EXPECTF--
*** Testing idate() : usage variation ***

-- Testing idate() function with float 12.3456789000e10 to timestamp --

Warning: idate() expects parameter 2 to be integer, float given in %s on line %d
bool(false)

-- Testing idate() function with float -12.3456789000e10 to timestamp --

Warning: idate() expects parameter 2 to be integer, float given in %s on line %d
bool(false)
===DONE===
