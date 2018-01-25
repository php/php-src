--TEST--
Test sprintf() function : basic functionality - hexadecimal format
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
 */

echo "*** Testing sprintf() : basic functionality - using hexadecimal format ***\n";

// Initialise all required variables

// Initialising different format strings
$format = "format";
$format1 = "%x";
$format2 = "%x %x";
$format3 = "%x %x %x";

$format11 = "%X";
$format22 = "%X %X";
$format33 = "%X %X %X";

$arg1 = 11;
$arg2 = 132;
$arg3 = 177;

// Calling sprintf() with default arguments
var_dump( sprintf($format) );

// Calling sprintf() with two arguments
var_dump( sprintf($format1, $arg1) );
var_dump( sprintf($format11, $arg1) );

// Calling sprintf() with three arguments
var_dump( sprintf($format2, $arg1, $arg2) );
var_dump( sprintf($format22, $arg1, $arg2) );

// Calling sprintf() with four arguments
var_dump( sprintf($format3, $arg1, $arg2, $arg3) );
var_dump( sprintf($format33, $arg1, $arg2, $arg3) );

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : basic functionality - using hexadecimal format ***
string(6) "format"
string(1) "b"
string(1) "B"
string(4) "b 84"
string(4) "B 84"
string(7) "b 84 b1"
string(7) "B 84 B1"
Done
