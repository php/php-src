--TEST--
Bug #61038: unpack("a5", "str\0\0") does not work as expected
--FILE--
<?php
var_dump(unpack("a4", "str\0\0"));
var_dump(unpack("a5", "str\0\0"));
var_dump(unpack("a6", "str\0\0"));
var_dump(unpack("a*", "str\0\0"));
?>
--EXPECTF--
array(1) {
  [1]=>
  string(4) "str%c"
}
array(1) {
  [1]=>
  string(5) "str%c%c"
}

Warning: unpack(): Type a: not enough input, need 6, have 5 in %s on line %d
bool(false)
array(1) {
  [1]=>
  string(5) "str%c%c"
}
