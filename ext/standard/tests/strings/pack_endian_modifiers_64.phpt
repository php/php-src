--TEST--
pack()/unpack() endianness modifiers on 64-bit format codes
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64bit test only");
?>
--FILE--
<?php

var_dump(bin2hex(pack("q<", 0x0102030405060708)));
var_dump(bin2hex(pack("Q<", 0x0102030405060708)));

var_dump(bin2hex(pack("q>", 0x0102030405060708)));
var_dump(bin2hex(pack("Q>", 0x0102030405060708)));

var_dump(pack("q<", 0x0102030405060708) === pack("P", 0x0102030405060708));
var_dump(pack("Q<", 0x0102030405060708) === pack("P", 0x0102030405060708));

var_dump(pack("q>", 0x0102030405060708) === pack("J", 0x0102030405060708));
var_dump(pack("Q>", 0x0102030405060708) === pack("J", 0x0102030405060708));

var_dump(unpack("q<", "\x08\x07\x06\x05\x04\x03\x02\x01"));
var_dump(unpack("Q<", "\x08\x07\x06\x05\x04\x03\x02\x01"));

var_dump(unpack("q>", "\x01\x02\x03\x04\x05\x06\x07\x08"));
var_dump(unpack("Q>", "\x01\x02\x03\x04\x05\x06\x07\x08"));

var_dump(unpack("q<", "\xfe\xff\xff\xff\xff\xff\xff\xff"));  // -2 in little-endian
var_dump(unpack("q>", "\xff\xff\xff\xff\xff\xff\xff\xfe"));  // -2 in big-endian
?>
--EXPECT--
string(16) "0807060504030201"
string(16) "0807060504030201"
string(16) "0102030405060708"
string(16) "0102030405060708"
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [1]=>
  int(72623859790382856)
}
array(1) {
  [1]=>
  int(72623859790382856)
}
array(1) {
  [1]=>
  int(72623859790382856)
}
array(1) {
  [1]=>
  int(72623859790382856)
}
array(1) {
  [1]=>
  int(-2)
}
array(1) {
  [1]=>
  int(-2)
}
