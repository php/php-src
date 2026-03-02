--TEST--
Test unserialize() with extra data at the end of a valid value
--FILE--
<?php

var_dump(unserialize('i:5;i:6;'));
var_dump(unserialize('N;i:6;'));
var_dump(unserialize('b:1;i:6;'));
var_dump(unserialize('a:1:{s:3:"foo";b:1;}i:6;'));

?>
--EXPECTF--
Warning: unserialize(): Extra data starting at offset 4 of 8 bytes in %s on line %d
int(5)

Warning: unserialize(): Extra data starting at offset 2 of 6 bytes in %s on line %d
NULL

Warning: unserialize(): Extra data starting at offset 4 of 8 bytes in %s on line %d
bool(true)

Warning: unserialize(): Extra data starting at offset 20 of 24 bytes in %s on line %d
array(1) {
  ["foo"]=>
  bool(true)
}
