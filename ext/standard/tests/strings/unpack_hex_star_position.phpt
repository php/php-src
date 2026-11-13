--TEST--
unpack() with "h*" and "H*" advances the position to the end of the input
--FILE--
<?php
var_dump(unpack("Ca/H*b/X/Cc", "\x01\xab\xcd"));
var_dump(unpack("Ca/h*b/X/Cc", "\x01\xab\xcd"));
var_dump(unpack("H*a/H*b", "\xab\xcd"));
var_dump(unpack("H*a/Cb", "\xab\xcd"));
var_dump(unpack("H3a/Cb", "\xab\xcd\xef"));
?>
--EXPECTF--
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(4) "abcd"
  ["c"]=>
  int(205)
}
array(3) {
  ["a"]=>
  int(1)
  ["b"]=>
  string(4) "badc"
  ["c"]=>
  int(205)
}
array(2) {
  ["a"]=>
  string(4) "abcd"
  ["b"]=>
  string(0) ""
}

Warning: unpack(): Type C: not enough input values, need 1 values but only 0 were provided in %s on line %d
bool(false)
array(2) {
  ["a"]=>
  string(3) "abc"
  ["b"]=>
  int(239)
}
