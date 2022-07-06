--TEST--
Bug #72017 (incorrect truncation due to floating point precision issue)
--FILE--
<?php
var_dump(range(4.5, 4.2, 0.1));
?>
--EXPECT--
array(4) {
  [0]=>
  float(4.5)
  [1]=>
  float(4.4)
  [2]=>
  float(4.3)
  [3]=>
  float(4.2)
}
