--TEST--
range() allows $step parameter to be negative for decreasing ranges
--INI--
precision=14
--FILE--
<?php
var_dump(range('c', 'a', -1));
var_dump(range(3, 1, -1));
var_dump(range(3.5, 1.5, -1.5));
?>
--EXPECT--
array(3) {
  [0]=>
  string(1) "c"
  [1]=>
  string(1) "b"
  [2]=>
  string(1) "a"
}
array(3) {
  [0]=>
  int(3)
  [1]=>
  int(2)
  [2]=>
  int(1)
}
array(2) {
  [0]=>
  float(3.5)
  [1]=>
  float(2)
}
