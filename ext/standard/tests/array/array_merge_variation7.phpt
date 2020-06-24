--TEST--
Test array_merge() function : usage variations - Mixed keys
--FILE--
<?php
/*
 * Pass array_merge() arrays with mixed keys to test how it attaches them to
 * existing arrays
 */

echo "*** Testing array_merge() : usage variations ***\n";

//mixed keys
$arr1 = array('zero', 20 => 'twenty', 'thirty' => 30, true => 'bool');
$arr2 = array(0, 1, 2, null => 'null', 1.234E-10 => 'float');

var_dump(array_merge($arr1, $arr2));
var_dump(array_merge($arr2, $arr1));

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : usage variations ***
array(8) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(6) "twenty"
  ["thirty"]=>
  int(30)
  [2]=>
  string(4) "bool"
  [3]=>
  string(5) "float"
  [4]=>
  int(1)
  [5]=>
  int(2)
  [""]=>
  string(4) "null"
}
array(8) {
  [0]=>
  string(5) "float"
  [1]=>
  int(1)
  [2]=>
  int(2)
  [""]=>
  string(4) "null"
  [3]=>
  string(4) "zero"
  [4]=>
  string(6) "twenty"
  ["thirty"]=>
  int(30)
  [5]=>
  string(4) "bool"
}
Done
