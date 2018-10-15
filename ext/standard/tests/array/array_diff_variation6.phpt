--TEST--
Test array_diff() function : usage variations - array containing duplicate keys and values
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are not
 * present in any of the others arguments.
 * Source code: ext/standard/array.c
 */

/*
 * Test that array_diff behaves as expected for comparing:
 * 1. the order of the array
 * 2. duplicate values
 * 3. duplicate key names
 */

echo "*** Testing array_diff() : usage variations ***\n";

$array_index = array('a', 'b', 'c', 0 => 'd', 'b');   //duplicate key (0), duplicate value (b)
$array_assoc = array ('2' => 'c',   //same key=>value pair, different order
                      '1' => 'b',
                      '0' => 'a',
                      'b' => '3',   //key and value from array_index swapped
                      'c' => 2);    //same as above, using integer

var_dump(array_diff($array_index, $array_assoc));
var_dump(array_diff($array_assoc, $array_index));

echo "Done";
?>
--EXPECTF--
*** Testing array_diff() : usage variations ***
array(1) {
  [0]=>
  string(1) "d"
}
array(3) {
  [0]=>
  string(1) "a"
  ["b"]=>
  string(1) "3"
  ["c"]=>
  int(2)
}
Done
