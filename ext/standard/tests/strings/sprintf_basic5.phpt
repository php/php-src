--TEST--
Test sprintf() function : basic functionality - char format
--FILE--
<?php
echo "*** Testing sprintf() : basic functionality - using char format ***\n";


// Initialise all required variables
$format = "format";
$format1 = "%c";
$format2 = "%c %c";
$format3 = "%c %c %c";
$arg1 = 65;
$arg2 = 66;
$arg3 = 67;

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
--EXPECT--
*** Testing sprintf() : basic functionality - using char format ***
string(6) "format"
string(1) "A"
string(3) "A B"
string(5) "A B C"
Done
