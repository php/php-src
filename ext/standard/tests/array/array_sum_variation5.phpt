--TEST--
Test array_sum() function : usage variations - array with reference variables as elements
--FILE--
<?php
/* Prototype  : mixed array_sum(array $input)
 * Description: Returns the sum of the array entries
 * Source code: ext/standard/array.c
*/

/*
* Testing array_sum() with 'input' having reference variables as elements
*/

echo "*** Testing array_sum() : array with elements as reference ***\n";

$value1 = -5;
$value2 = 100;
$value3 = 0;
$value4 = &$value1;

// input array containing elements as reference variables
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
--EXPECTF--
*** Testing array_sum() : array with elements as reference ***
int(305)
Done
