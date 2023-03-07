--TEST--
Test range() function with integer inputs and float step
--INI--
serialize_precision=14
--FILE--
<?php
var_dump( range(1, 5, 2.0) );
?>
--EXPECT--
array(3) {
  [0]=>
  float(1)
  [1]=>
  float(3)
  [2]=>
  float(5)
}
