--TEST--
Test array_diff_assoc() function : basic functionality
--FILE--
<?php
/* Prototype  : array array_diff_assoc(array $arr1, array $arr2 [, array ...])
 * Description: Returns the entries of $arr1 that have values which are not 
 * present in any of the others arguments but do additional checks whether the keys are equal 
 * Source code: ext/standard/array.c
 */

/*
 * Test basic functionality of array_diff_assoc
 */

echo "*** Testing array_diff_assoc() : basic functionality ***\n";
$array_default_key = array('one',       2,            'three', '4');
$array_numeric_key = array(1 => 'one',  2=> 'two',    3 => 4);
$array_string_key  = array('one' => 1, 'two' => '2', '3' => 'three');



echo "-- Compare Default keys to numeric keys --\n";
var_dump(array_diff_assoc($array_default_key, $array_numeric_key));
var_dump(array_diff_assoc($array_numeric_key, $array_default_key));


echo "\n-- Compare Default keys to string keys --\n";
var_dump(array_diff_assoc($array_default_key, $array_numeric_key));
var_dump(array_diff_assoc($array_numeric_key, $array_default_key));


echo "\n-- Compare numeric keys to string keys --\n";
var_dump(array_diff_assoc($array_numeric_key, $array_string_key));
var_dump(array_diff_assoc($array_string_key, $array_numeric_key));


echo "Done";
?>
--EXPECTF--
*** Testing array_diff_assoc() : basic functionality ***
-- Compare Default keys to numeric keys --
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  int(2)
  [2]=>
  string(5) "three"
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}

-- Compare Default keys to string keys --
array(3) {
  [0]=>
  string(3) "one"
  [1]=>
  int(2)
  [2]=>
  string(5) "three"
}
array(2) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
}

-- Compare numeric keys to string keys --
array(3) {
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  int(4)
}
array(3) {
  ["one"]=>
  int(1)
  ["two"]=>
  string(1) "2"
  [3]=>
  string(5) "three"
}
Done
