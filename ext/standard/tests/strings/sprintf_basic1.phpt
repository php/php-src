--TEST--
Test sprintf() function : basic functionality - string format
--FILE--
<?php
/* Prototype  : string sprintf(string $format [, mixed $arg1 [, mixed ...]])
 * Description: Return a formatted string
 * Source code: ext/standard/formatted_print.c
*/

echo "*** Testing sprintf() : basic functionality - using string format ***\n";

// Initialise all required variables
$format = "format";
$format1 = "%s";
$format2 = "%s %s";
$format3 = "%s %s %s";
$arg1 = "arg1 argument";
$arg2 = "arg2 argument";
$arg3 = "arg3 argument";

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
*** Testing sprintf() : basic functionality - using string format ***
string(6) "format"
string(13) "arg1 argument"
string(27) "arg1 argument arg2 argument"
string(41) "arg1 argument arg2 argument arg3 argument"
Done
