--TEST--
Test array_merge() function : usage variations - string keys
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Pass array_merge arrays with string keys to test behaviour.
 * $arr2 has a duplicate key to $arr1
 */

echo "*** Testing array_merge() : usage variations ***\n";

//string keys
$arr1 = array('zero' => 'zero', 'one' => 'un', 'two' => 'deux');
$arr2 = array('zero' => 'zero', 'un' => 'eins', 'deux' => 'zwei');

var_dump(array_merge($arr1, $arr2));
var_dump(array_merge($arr2, $arr1));

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***
array(5) {
  ["zero"]=>
  string(4) "zero"
  ["one"]=>
  string(2) "un"
  ["two"]=>
  string(4) "deux"
  ["un"]=>
  string(4) "eins"
  ["deux"]=>
  string(4) "zwei"
}
array(5) {
  ["zero"]=>
  string(4) "zero"
  ["un"]=>
  string(4) "eins"
  ["deux"]=>
  string(4) "zwei"
  ["one"]=>
  string(2) "un"
  ["two"]=>
  string(4) "deux"
}
Done
