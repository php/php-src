--TEST--
Test array_unique() function : usage variations - array with duplicate keys
--FILE--
<?php
/* Prototype  : array array_unique(array $input)
 * Description: Removes duplicate values from array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the functionality of array_unique() by passing
 * array having duplicate keys as values.
*/

echo "*** Testing array_unique() : array with duplicate keys for \$input argument ***\n";

// initialize the array having duplicate keys
$input = array( 1 => "one", 2 => "two", 2 => "2", 3 => "three", 1 => "1", "1", "2");
var_dump( array_unique($input) );

echo "Done";
?>
--EXPECT--
*** Testing array_unique() : array with duplicate keys for $input argument ***
array(3) {
  [1]=>
  string(1) "1"
  [2]=>
  string(1) "2"
  [3]=>
  string(5) "three"
}
Done
