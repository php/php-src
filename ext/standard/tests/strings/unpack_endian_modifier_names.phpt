--TEST--
unpack() endianness modifiers and element names
--FILE--
<?php

var_dump(unpack("s<value", "\x02\x01"));
var_dump(unpack("s>value", "\x01\x02"));

var_dump(unpack("s<2name", "\x02\x01\x04\x03"));
var_dump(unpack("s<*name", "\x02\x01\x04\x03"));

var_dump(unpack("v1<value", "\x02\x01"));
var_dump(unpack("n2>name", "\x01\x02\x03\x04"));
var_dump(unpack("C1>name", "\x01"));

?>
--EXPECT--
array(1) {
  ["value"]=>
  int(258)
}
array(1) {
  ["value"]=>
  int(258)
}
array(2) {
  ["name1"]=>
  int(258)
  ["name2"]=>
  int(772)
}
array(2) {
  ["name1"]=>
  int(258)
  ["name2"]=>
  int(772)
}
array(1) {
  ["<value"]=>
  int(258)
}
array(2) {
  [">name1"]=>
  int(258)
  [">name2"]=>
  int(772)
}
array(1) {
  [">name"]=>
  int(1)
}
