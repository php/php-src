--TEST--
Test strspn() function : basic functionality
--FILE--
<?php
/*
* Testing strspn() : basic functionality
*/

echo "*** Testing strspn() : basic functionality ***\n";


// Initialise all required variables
$str = "this is the test string";
$mask = "htes ";
$start = 8;
$len = 12;

// Calling strspn() with all possible arguments
var_dump( strspn($str, $mask, $start, $len) );

// Calling strspn() with three arguments and default len argument
var_dump( strspn($str, $mask, $start) );

// Calling strspn() with default arguments
var_dump( strspn($str, $mask) );

echo "Done"
?>
--EXPECT--
*** Testing strspn() : basic functionality ***
int(11)
int(11)
int(2)
Done
