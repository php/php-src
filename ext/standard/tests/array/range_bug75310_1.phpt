--TEST--
Bug #75310 (another incorrect truncation due to floating point precision issue)
--FILE--
<?php
var_dump(range(-0.57, -0.49, 0.01));
?>
--EXPECT--
array(9) {
  [0]=>
  float(-0.57)
  [1]=>
  float(-0.56)
  [2]=>
  float(-0.55)
  [3]=>
  float(-0.54)
  [4]=>
  float(-0.53)
  [5]=>
  float(-0.52)
  [6]=>
  float(-0.51)
  [7]=>
  float(-0.5)
  [8]=>
  float(-0.49)
}
