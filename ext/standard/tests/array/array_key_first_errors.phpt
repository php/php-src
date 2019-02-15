--TEST--
Test array_key_first() function (errors)
--FILE--
<?php

$empty_array = array();
$number = 5;
$str = "abc";

/* Various combinations of arrays to be used for the test */
$mixed_array = array(
    array( 1,2,3,4,5,6,7,8,9 ),
    array( "One", "_Two", "Three", "Four", "Five" )
);

/* Testing Error Conditions */
echo "\n*** Testing Error Conditions ***\n";

/* Zero argument  */
var_dump( array_key_first() );

/* Scalar argument */
var_dump( array_key_first($number) );

/* String argument */
var_dump( array_key_first($str) );

/* Invalid Number of arguments */
var_dump( array_key_first($mixed_array[0],$mixed_array[1]) );

/* Empty Array as argument */
var_dump( array_key_first($empty_array) );

echo"\nDone";
?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: array_key_first() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: array_key_first() expects parameter 1 to be array, int given in %s on line %d
NULL

Warning: array_key_first() expects parameter 1 to be array, string given in %s on line %d
NULL

Warning: array_key_first() expects exactly 1 parameter, 2 given in %s on line %d
NULL
NULL

Done
