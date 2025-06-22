--TEST--
GH-18901 (integer overflow mb_split)
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
if (!function_exists('mb_split')) die('skip mb_split() not available');
?>
--FILE--
<?php
$vals = [PHP_INT_MIN, PHP_INT_MAX, -1, 0, 1];
foreach ($vals as $val) {
    var_dump(mb_split('\d', '123', $val));
}
?>
--EXPECT--
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
}
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
}
array(4) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(3) "123"
}
array(1) {
  [0]=>
  string(3) "123"
}
