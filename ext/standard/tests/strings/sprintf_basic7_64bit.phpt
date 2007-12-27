--TEST--
Test sprintf() function : basic functionality - unsigned format
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string 
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing sprintf() : basic functionality - using unsigned format ***\n";


// Initialise all required variables
$format = "format";
$format1 = "%u";
$format2 = "%u %u";
$format3 = "%u %u %u";
$arg1 = -1111;
$arg2 = -1234567;
$arg3 = +2345432;

// Calling sprintf() with default arguments
var_dump( sprintf($format) );

// Calling sprintf() with two arguments
var_dump( sprintf($format1, $arg1) );

// Calling sprintf() with three arguments
var_dump( sprintf($format2, $arg1, $arg2) );

// Calling sprintf() with four arguments
var_dump( sprintf($format3, $arg1, $arg2, $arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : basic functionality - using unsigned format ***
string(6) "format"
string(20) "18446744073709550505"
string(41) "18446744073709550505 18446744073708317049"
string(49) "18446744073709550505 18446744073708317049 2345432"
Done
