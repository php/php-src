--TEST--
Test sprintf() function : basic functionality - float format
--FILE--
<?php
echo "*** Testing sprintf() : basic functionality - using float format ***\n";


// Initialise all required variables

$format = "format";
$format1 = "%f";
$format2 = "%f %f";
$format3 = "%f %f %f";

$format11 = "%F";
$format22 = "%F %F";
$format33 = "%F %F %F";
$arg1 = 11.11;
$arg2 = 22.22;
$arg3 = 33.33;

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
--EXPECT--
*** Testing sprintf() : basic functionality - using float format ***
string(6) "format"
string(9) "11.110000"
string(9) "11.110000"
string(19) "11.110000 22.220000"
string(19) "11.110000 22.220000"
string(29) "11.110000 22.220000 33.330000"
string(29) "11.110000 22.220000 33.330000"
Done
