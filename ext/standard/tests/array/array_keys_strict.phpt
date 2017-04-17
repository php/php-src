--TEST--
array_keys() in strict mode 
--FILE--
<?php

$arr = array(1, "1", "", NULL, 0, false, true, array());

$s = 1;
var_dump(array_keys($arr, $s, true));

$s = "1";
var_dump(array_keys($arr, $s, true));

$s = "";
var_dump(array_keys($arr, $s, true));

$s = NULL;
var_dump(array_keys($arr, $s, true));

$s = 0;
var_dump(array_keys($arr, $s, true));

$s = false;
var_dump(array_keys($arr, $s, true));

$s = true;
var_dump(array_keys($arr, $s, true));

$s = array();
var_dump(array_keys($arr, $s, true));

?>
--EXPECTF--
array(1) {
  [0]=>
  int(0)
}
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  int(4)
}
array(1) {
  [0]=>
  int(5)
}
array(1) {
  [0]=>
  int(6)
}
array(1) {
  [0]=>
  int(7)
}
