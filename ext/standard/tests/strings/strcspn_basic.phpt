--TEST--
Test strcspn() function : basic functionality
--FILE--
<?php
/*
* Testing strcspn() : basic functionality
*/

echo "*** Testing strcspn() : basic functionality ***\n";


// Initialise all required variables
$str = "this is the test string";
$mask = "es";
$start = 15;
$len = 3;

// Calling strcspn() with all possible arguments
var_dump( strcspn($str, $mask, $start, $len) );

// Calling strcspn() with three arguments
var_dump( strcspn($str, $mask, $start) );

// Calling strcspn() with default arguments
var_dump( strcspn($str, $mask) );

echo "Done"
?>
--EXPECT--
*** Testing strcspn() : basic functionality ***
int(2)
int(2)
int(3)
Done
