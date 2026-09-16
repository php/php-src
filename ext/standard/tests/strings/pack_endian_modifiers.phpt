--TEST--
pack()/unpack() endianness modifiers
--FILE--
<?php

var_dump(bin2hex(pack("s<", 0x0102)));
var_dump(bin2hex(pack("S<", 0x0102)));

var_dump(bin2hex(pack("s>", 0x0102)));
var_dump(bin2hex(pack("S>", 0x0102)));

var_dump(pack("s<", 0x0102) === pack("v", 0x0102));
var_dump(pack("S<", 0x0102) === pack("v", 0x0102));

var_dump(pack("s>", 0x0102) === pack("n", 0x0102));
var_dump(pack("S>", 0x0102) === pack("n", 0x0102));

var_dump(bin2hex(pack("l<", 0x01020304)));
var_dump(bin2hex(pack("L<", 0x01020304)));

var_dump(bin2hex(pack("l>", 0x01020304)));
var_dump(bin2hex(pack("L>", 0x01020304)));

var_dump(pack("l<", 0x01020304) === pack("V", 0x01020304));
var_dump(pack("L<", 0x01020304) === pack("V", 0x01020304));

var_dump(pack("l>", 0x01020304) === pack("N", 0x01020304));
var_dump(pack("L>", 0x01020304) === pack("N", 0x01020304));

// === Integer unpack with endianness modifiers ===

var_dump(unpack("s<", "\x02\x01"));
var_dump(unpack("S<", "\x02\x01"));

var_dump(unpack("s>", "\x01\x02"));
var_dump(unpack("S>", "\x01\x02"));

var_dump(unpack("s<", "\xfe\xff"));  // -2 in little-endian
var_dump(unpack("s>", "\xff\xfe"));  // -2 in big-endian

var_dump(unpack("l<", "\x04\x03\x02\x01"));
var_dump(unpack("L<", "\x04\x03\x02\x01"));

var_dump(unpack("l>", "\x01\x02\x03\x04"));
var_dump(unpack("L>", "\x01\x02\x03\x04"));

var_dump(unpack("l<", "\xfe\xff\xff\xff"));  // -2 in little-endian
var_dump(unpack("l>", "\xff\xff\xff\xfe"));  // -2 in big-endian

var_dump(bin2hex(pack("s<2", 0x0102, 0x0304)));
var_dump(bin2hex(pack("s>2", 0x0102, 0x0304)));

var_dump(unpack("s<2", "\x02\x01\x04\x03"));
var_dump(unpack("s>2", "\x01\x02\x03\x04"));

var_dump(unpack("s<value/L>count", "\x02\x01\x00\x00\x00\x05"));

var_dump(pack("f<", 3.14) === pack("g", 3.14));
var_dump(pack("f>", 3.14) === pack("G", 3.14));

var_dump(pack("d<", 3.14) === pack("e", 3.14));
var_dump(pack("d>", 3.14) === pack("E", 3.14));

$packed_le = pack("g", 3.14);
$packed_be = pack("G", 3.14);
$unpacked_le = unpack("f<", $packed_le);
$unpacked_be = unpack("f>", $packed_be);
$unpacked_g = unpack("g", $packed_le);
$unpacked_G = unpack("G", $packed_be);
var_dump($unpacked_le[1] === $unpacked_g[1]);
var_dump($unpacked_be[1] === $unpacked_G[1]);

$packed_le = pack("e", 3.14);
$packed_be = pack("E", 3.14);
$unpacked_le = unpack("d<", $packed_le);
$unpacked_be = unpack("d>", $packed_be);
$unpacked_e = unpack("e", $packed_le);
$unpacked_E = unpack("E", $packed_be);
var_dump($unpacked_le[1] === $unpacked_e[1]);
var_dump($unpacked_be[1] === $unpacked_E[1]);

$machine_float = pack("f", 1.5);
var_dump(unpack("f", $machine_float)[1] === 1.5);

$machine_double = pack("d", 1.5);
var_dump(unpack("d", $machine_double)[1] === 1.5);
?>
--EXPECT--
string(4) "0201"
string(4) "0201"
string(4) "0102"
string(4) "0102"
bool(true)
bool(true)
bool(true)
bool(true)
string(8) "04030201"
string(8) "04030201"
string(8) "01020304"
string(8) "01020304"
bool(true)
bool(true)
bool(true)
bool(true)
array(1) {
  [1]=>
  int(258)
}
array(1) {
  [1]=>
  int(258)
}
array(1) {
  [1]=>
  int(258)
}
array(1) {
  [1]=>
  int(258)
}
array(1) {
  [1]=>
  int(-2)
}
array(1) {
  [1]=>
  int(-2)
}
array(1) {
  [1]=>
  int(16909060)
}
array(1) {
  [1]=>
  int(16909060)
}
array(1) {
  [1]=>
  int(16909060)
}
array(1) {
  [1]=>
  int(16909060)
}
array(1) {
  [1]=>
  int(-2)
}
array(1) {
  [1]=>
  int(-2)
}
string(8) "02010403"
string(8) "01020304"
array(2) {
  [1]=>
  int(258)
  [2]=>
  int(772)
}
array(2) {
  [1]=>
  int(258)
  [2]=>
  int(772)
}
array(2) {
  ["value"]=>
  int(258)
  ["count"]=>
  int(5)
}
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
