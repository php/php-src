--TEST--
Bug #70668 (array_keys() doesn't respect references when $strict is true)
--FILE--
<?php
$arr = array(1, "1", "", NULL, 0, false, true, array());

$s = &$arr[0];
var_dump(array_keys($arr, $s, true));

$s = &$arr[1];
var_dump(array_keys($arr, $s, true));

$s = &$arr[2];
var_dump(array_keys($arr, $s, true));

$s = &$arr[3];
var_dump(array_keys($arr, $s, true));

$s = &$arr[4];
var_dump(array_keys($arr, $s, true));

$s = &$arr[5];
var_dump(array_keys($arr, $s, true));

$s = &$arr[6];
var_dump(array_keys($arr, $s, true));

$s = &$arr[7];
var_dump(array_keys($arr, $s, true));
?>
--EXPECT--
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
