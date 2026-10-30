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
  int(1)
  [1]=>
  int(3)
  [2]=>
  int(5)
}
