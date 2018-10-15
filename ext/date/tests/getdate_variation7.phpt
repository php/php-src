--TEST--
Test getdate() function : usage variation - Passing high positive and negative float values to timestamp.
--SKIPIF--
<?php if (PHP_INT_SIZE != 4) echo "skip this test is for 32-bit only"; ?>
--FILE--
<?php
/* Prototype  : array getdate([int timestamp])
 * Description: Get date/time information
 * Source code: ext/date/php_date.c
 * Alias to functions:
 */

echo "*** Testing getdate() : usage variation ***\n";
date_default_timezone_set("Asia/Calcutta");

echo "\n-- Testing getdate() function by passing float 12.3456789000e10 value to timestamp --\n";
$timestamp = 12.3456789000e10;
var_dump( getdate($timestamp) );

echo "\n-- Testing getdate() function by passing float -12.3456789000e10 value to timestamp --\n";
$timestamp = -12.3456789000e10;
var_dump( getdate($timestamp) );
?>
===DONE===
--EXPECTF--
*** Testing getdate() : usage variation ***

-- Testing getdate() function by passing float 12.3456789000e10 value to timestamp --

Warning: getdate() expects parameter 1 to be integer, float given in %s on line %d
bool(false)

-- Testing getdate() function by passing float -12.3456789000e10 value to timestamp --

Warning: getdate() expects parameter 1 to be integer, float given in %s on line %d
bool(false)
===DONE===
