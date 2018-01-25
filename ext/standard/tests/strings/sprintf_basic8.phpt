--TEST--
Test sprintf() function : basic functionality - octal format
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) {
	    die("skip this test is for 32bit platform only");
}
?>
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing sprintf() : basic functionality - using octal format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%o";
$format2 = "%o %o";
$format3 = "%o %o %o";
$arg1 = 021;
$arg2 = -0347;
$arg3 = 0567;

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
*** Testing sprintf() : basic functionality - using octal format ***
string(6) "format"
string(2) "21"
string(14) "21 37777777431"
string(18) "21 37777777431 567"
Done
