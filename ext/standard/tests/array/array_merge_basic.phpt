--TEST--
Test array_merge() function : basic functionality
--FILE--
<?php
/*
 * Test basic functionality of array_merge()
 */

echo "*** Testing array_merge() : basic functionality ***\n";

//indexed array
$array1 = array ('zero', 'one', 'two');
//associative array
$array2 = array ('a' => 1, 'b' => 2, 'c' => 3);

var_dump(array_merge($array1, $array2));

var_dump(array_merge($array2, $array1));

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : basic functionality ***
array(6) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
}
array(6) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(2)
  ["c"]=>
  int(3)
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}
Done
