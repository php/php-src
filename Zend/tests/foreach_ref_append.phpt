--TEST--
Foreach by reference to array append
--FILE--
<?php

$array1 = [1, 2, 3];
$array2 = [];
$array3 = [];
foreach ($array1 as &$array2[]);
foreach ($array1 as &$array3[]['x']);
var_dump($array1, $array2, $array3);

?>
--EXPECT--
array(3) {
  [0]=>
  &int(1)
  [1]=>
  &int(2)
  [2]=>
  &int(3)
}
array(3) {
  [0]=>
  &int(1)
  [1]=>
  &int(2)
  [2]=>
  &int(3)
}
array(3) {
  [0]=>
  array(1) {
    ["x"]=>
    &int(1)
  }
  [1]=>
  array(1) {
    ["x"]=>
    &int(2)
  }
  [2]=>
  array(1) {
    ["x"]=>
    &int(3)
  }
}
