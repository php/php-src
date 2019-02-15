--TEST--
Test array_keys() function (variation - 2)
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

echo "\n*** Testing array_keys() on range of values ***\n";
$arr_range = array(
  2147483647 => 1,
  2147483648 => 2,
  -2147483647 => 3,
  -2147483648 => 4,
  -2147483649 => 5,
  -0 => 6,
  0 => 7
);
var_dump(array_keys($arr_range));

echo "\n*** Testing array_keys() on an array created on the fly ***\n";
var_dump(array_keys(array("a" => 1, "b" => 2, "c" => 3)));
var_dump(array_keys(array()));  // null array

echo "Done\n";
?>
--EXPECT--
*** Testing array_keys() on range of values ***
array(4) {
  [0]=>
  int(2147483647)
  [1]=>
  int(-2147483648)
  [2]=>
  int(-2147483647)
  [3]=>
  int(0)
}

*** Testing array_keys() on an array created on the fly ***
array(3) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "c"
}
array(0) {
}
Done
