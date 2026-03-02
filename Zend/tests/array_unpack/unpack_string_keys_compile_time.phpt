--TEST--
Unpacking of string keys is supported at compile-time
--FILE--
<?php

var_dump([...['a' => 'b']]);
var_dump(['a' => 'X', ...['a' => 'b']]);
var_dump([...['a' => 'b'], 'a' => 'X']);

?>
--EXPECT--
array(1) {
  ["a"]=>
  string(1) "b"
}
array(1) {
  ["a"]=>
  string(1) "b"
}
array(1) {
  ["a"]=>
  string(1) "X"
}
