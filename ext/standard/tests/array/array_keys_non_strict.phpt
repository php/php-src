--TEST--
array_keys() in non-strict mode 
--FILE--
<?php

$arr = array(1, "1", "", NULL, 0, false, true, array());

$s = 1;
var_dump(array_keys($arr, $s));

$s = "1";
var_dump(array_keys($arr, $s));

$s = "";
var_dump(array_keys($arr, $s));

$s = NULL;
var_dump(array_keys($arr, $s));

$s = 0;
var_dump(array_keys($arr, $s));

$s = false;
var_dump(array_keys($arr, $s));

$s = true;
var_dump(array_keys($arr, $s));

$s = array();
var_dump(array_keys($arr, $s));

?>
--EXPECTF--
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(6)
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(6)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
array(5) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(7)
}
array(4) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
}
array(5) {
  [0]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(4)
  [3]=>
  int(5)
  [4]=>
  int(7)
}
array(3) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(6)
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(5)
  [2]=>
  int(7)
}
