--TEST--
GH-13094 (range(9.9, '0') causes segmentation fault)
--FILE--
<?php
var_dump(range(9.9, '0'));
?>
--EXPECT--
array(10) {
  [0]=>
  float(9.9)
  [1]=>
  float(8.9)
  [2]=>
  float(7.9)
  [3]=>
  float(6.9)
  [4]=>
  float(5.9)
  [5]=>
  float(4.9)
  [6]=>
  float(3.9000000000000004)
  [7]=>
  float(2.9000000000000004)
  [8]=>
  float(1.9000000000000004)
  [9]=>
  float(0.9000000000000004)
}
