--TEST--
Array unpacking with string keys
--FILE--
<?php

// Works with both arrays and Traversables.
$array = [1, 2, "foo" => 3, 4];
var_dump([...$array]);

$iterator = new ArrayIterator([1, 2, "foo" => 3, 4]);
var_dump([...$iterator]);

// Test overwriting behavior.
$array1 = ["foo" => 1];
$array2 = ["foo" => 2];
var_dump(["foo" => 0, ...$array1, ...$array2]);
var_dump(["foo" => 0, ...$array1, ...$array2, "foo" => 3]);

?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["foo"]=>
  int(3)
  [2]=>
  int(4)
}
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["foo"]=>
  int(3)
  [2]=>
  int(4)
}
array(1) {
  ["foo"]=>
  int(2)
}
array(1) {
  ["foo"]=>
  int(3)
}
