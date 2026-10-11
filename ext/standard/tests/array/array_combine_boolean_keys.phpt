--TEST--
array_combine() with boolean keys compared to direct array keys
--FILE--
<?php

echo "Direct array keys:\n";
var_dump([true => 'true', false => 'false']);

echo "\narray_combine:\n";
var_dump(array_combine([true, false], ['true', 'false']));

echo "\nExplicit integer keys:\n";
var_dump(array_combine([1, 0], ['true', 'false']));

?>
--EXPECT--
Direct array keys:
array(2) {
  [1]=>
  string(4) "true"
  [0]=>
  string(5) "false"
}

array_combine:
array(2) {
  [1]=>
  string(4) "true"
  [0]=>
  string(5) "false"
}

Explicit integer keys:
array(2) {
  [1]=>
  string(4) "true"
  [0]=>
  string(5) "false"
}
