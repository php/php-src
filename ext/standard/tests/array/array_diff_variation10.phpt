--TEST--
Test array_diff() function : usage variations - binary safe checking
--FILE--
<?php
/* Prototype  : array array_diff(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are 
 * not present in any of the others arguments. 
 * Source code: ext/standard/array.c
 */

/*
 * Test behaviour of array_diff() function with binary input
 */

echo "*** Testing array_diff() : usage variations ***\n";


$array1 = array( b"1", 
                 b"hello", 
                 "world", 
                 "str1" => "hello", 
                 "str2" => "world");

$array2 = array( b"1" => 'hello',
                 b"world",
                 "hello", 
                 'test');

var_dump(array_diff($array1, $array2));
var_dump(array_diff($array2, $array1));

echo "Done";
?>
--EXPECTF--
*** Testing array_diff() : usage variations ***
array(1) {
  [0]=>
  string(1) "1"
}
array(1) {
  [4]=>
  string(4) "test"
}
Done
