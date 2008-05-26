--TEST--
Test array_sum() function : usage variations - array with reference variables
--FILE--
<?php
/* Prototype  : mixed array_sum(array $input)
 * Description: Returns the sum of the array entries 
 * Source code: ext/standard/array.c
*/

/*
* Testing array_sum() with 'input' containing different reference variables 
*/

echo "*** Testing array_sum() : array with elements as reference ***\n";

// different variables which are used as elements of 'array_arg'
$value1 = -5;
$value2 = 100;
$value3 = 0;
$value4 = &$value1;

// input an array containing elements with reference variables
$input = array(
  0 => 10,
  1 => &$value4,
  2 => &$value2,
  3 => 200,
  4 => &$value3,
);

var_dump( array_sum($input) );

echo "Done"
?>
--EXPECT--
*** Testing array_sum() : array with elements as reference ***
int(305)
Done
