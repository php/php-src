--TEST--
Test array_merge() function : usage variations - numeric keys
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Pass array_merge() arrays with only numeric keys to test behaviour.
 * $arr2 contains a duplicate element to $arr1.
 */

echo "*** Testing array_merge() : usage variations ***\n";

//numeric keys
$arr1 = array('zero', 'one', 'two', 'three');
$arr2 = array(1 => 'one', 20 => 'twenty', 30 => 'thirty');

var_dump(array_merge($arr1, $arr2));
var_dump(array_merge($arr2, $arr1));

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***
array(7) {
  [0]=>
  string(4) "zero"
  [1]=>
  string(3) "one"
  [2]=>
  string(3) "two"
  [3]=>
  string(5) "three"
  [4]=>
  string(3) "one"
  [5]=>
  string(6) "twenty"
  [6]=>
  string(6) "thirty"
}
array(7) {
  [0]=>
  string(3) "one"
  [1]=>
  string(6) "twenty"
  [2]=>
  string(6) "thirty"
  [3]=>
  string(4) "zero"
  [4]=>
  string(3) "one"
  [5]=>
  string(3) "two"
  [6]=>
  string(5) "three"
}
Done
