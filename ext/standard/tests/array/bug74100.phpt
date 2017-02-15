--TEST--
Bug #74100 (array_fill with negative $start_index works incorrectly)
--FILE--
<?php
var_dump(array_fill_keys(range(-2,1), true));
var_dump(array_fill(-2, 4, true));
?>
--EXPECT--
array(4) {
  [-2]=>
  bool(true)
  [-1]=>
  bool(true)
  [0]=>
  bool(true)
  [1]=>
  bool(true)
}
array(4) {
  [-2]=>
  bool(true)
  [-1]=>
  bool(true)
  [0]=>
  bool(true)
  [1]=>
  bool(true)
}

