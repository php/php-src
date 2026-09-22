--TEST--
GH-18480 (array_splice overflow with large offset / length values)
--FILE--
<?php

foreach ([PHP_INT_MIN, PHP_INT_MAX] as $length) {
	$a = [PHP_INT_MAX];
	$offset = PHP_INT_MAX;
	var_dump(array_splice($a,$offset, $length));
	$a = [PHP_INT_MAX];
	$offset = PHP_INT_MIN;
	var_dump(array_splice($a,$offset, $length));
	$a = ["a" => PHP_INT_MAX];
	$offset = PHP_INT_MAX;
	var_dump(array_splice($a,$offset, $length));
	$a = ["a" => PHP_INT_MAX];
	$offset = PHP_INT_MIN;
	var_dump(array_splice($a,$offset, $length));
}
--EXPECTF--
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  [0]=>
  int(%d)
}
array(0) {
}
array(1) {
  ["a"]=>
  int(%d)
}
