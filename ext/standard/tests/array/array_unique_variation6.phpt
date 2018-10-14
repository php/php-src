--TEST--
Test array_unique() function : usage variations - array with reference variables
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unique() by passing
 * array having reference variables as values.
*/

echo "*** Testing array_unique() : array with reference variables for \$input argument ***\n";

$value1 = 10;
$value2 = "hello";
$value3 = 0;
$value4 = &$value2;

// input array containing elements as reference variables
$input = array(
  0 => 0,
  1 => &$value4,
  2 => &$value2,
  3 => "hello",
  4 => &$value3,
  5 => $value4
);

var_dump( array_unique($input, SORT_STRING) );

echo "Done";
?>
--EXPECTF--
*** Testing array_unique() : array with reference variables for $input argument ***
array(2) {
  [0]=>
  int(0)
  [1]=>
  &string(5) "hello"
}
Done
