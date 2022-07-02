--TEST--
array_diff() with single array argument
--FILE--
<?php

$array = ["a" => 42];
$cmp = function($a, $b) { return $a <=> $b; };
var_dump(array_diff($array));
var_dump(array_diff_key($array));
var_dump(array_diff_ukey($array, $cmp));
var_dump(array_diff_assoc($array));
var_dump(array_diff_uassoc($array, $cmp));
var_dump(array_udiff($array, $cmp));
var_dump(array_udiff_assoc($array, $cmp));
var_dump(array_udiff_uassoc($array, $cmp, $cmp));

?>
--EXPECT--
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
array(1) {
  ["a"]=>
  int(42)
}
