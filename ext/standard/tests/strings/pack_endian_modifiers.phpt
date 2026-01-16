--TEST--
pack()/unpack() endianness modifiers
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) die("skip 64bit test only");
?>
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

var_dump(bin2hex(pack("q<", 0x0102030405060708)));
var_dump(bin2hex(pack("Q<", 0x0102030405060708)));

var_dump(bin2hex(pack("q>", 0x0102030405060708)));
var_dump(bin2hex(pack("Q>", 0x0102030405060708)));

var_dump(pack("q<", 0x0102030405060708) === pack("P", 0x0102030405060708));
var_dump(pack("Q<", 0x0102030405060708) === pack("P", 0x0102030405060708));

var_dump(pack("q>", 0x0102030405060708) === pack("J", 0x0102030405060708));
var_dump(pack("Q>", 0x0102030405060708) === pack("J", 0x0102030405060708));

print_r(unpack("s<", "\x02\x01"));
print_r(unpack("S<", "\x02\x01"));

print_r(unpack("s>", "\x01\x02"));
print_r(unpack("S>", "\x01\x02"));

print_r(unpack("s<", "\xfe\xff"));  // -2 in little-endian
print_r(unpack("s>", "\xff\xfe"));  // -2 in big-endian

print_r(unpack("l<", "\x04\x03\x02\x01"));
print_r(unpack("L<", "\x04\x03\x02\x01"));

print_r(unpack("l>", "\x01\x02\x03\x04"));
print_r(unpack("L>", "\x01\x02\x03\x04"));

print_r(unpack("l<", "\xfe\xff\xff\xff"));  // -2 in little-endian
print_r(unpack("l>", "\xff\xff\xff\xfe"));  // -2 in big-endian

print_r(unpack("q<", "\x08\x07\x06\x05\x04\x03\x02\x01"));
print_r(unpack("Q<", "\x08\x07\x06\x05\x04\x03\x02\x01"));

print_r(unpack("q>", "\x01\x02\x03\x04\x05\x06\x07\x08"));
print_r(unpack("Q>", "\x01\x02\x03\x04\x05\x06\x07\x08"));

print_r(unpack("q<", "\xfe\xff\xff\xff\xff\xff\xff\xff"));  // -2 in little-endian
print_r(unpack("q>", "\xff\xff\xff\xff\xff\xff\xff\xfe"));  // -2 in big-endian

var_dump(bin2hex(pack("s<2", 0x0102, 0x0304)));
var_dump(bin2hex(pack("s>2", 0x0102, 0x0304)));

print_r(unpack("s<2", "\x02\x01\x04\x03"));
print_r(unpack("s>2", "\x01\x02\x03\x04"));

print_r(unpack("s<value/L>count", "\x02\x01\x00\x00\x00\x05"));

// modifiers on codes with inherent endianness
$error_formats = ['n<', 'v>', 'N<', 'V>', 'J<', 'P>'];
foreach ($error_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

$unsupported_formats = ['c<', 'C>', 'a<', 'A>', 'i<', 'I>', 'f<', 'd>'];
foreach ($unsupported_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['n<', 'v>', 'N<', 'V>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}
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
string(16) "0807060504030201"
string(16) "0807060504030201"
string(16) "0102030405060708"
string(16) "0102030405060708"
bool(true)
bool(true)
bool(true)
bool(true)
Array
(
    [1] => 258
)
Array
(
    [1] => 258
)
Array
(
    [1] => 258
)
Array
(
    [1] => 258
)
Array
(
    [1] => -2
)
Array
(
    [1] => -2
)
Array
(
    [1] => 16909060
)
Array
(
    [1] => 16909060
)
Array
(
    [1] => 16909060
)
Array
(
    [1] => 16909060
)
Array
(
    [1] => -2
)
Array
(
    [1] => -2
)
Array
(
    [1] => 72623859790382856
)
Array
(
    [1] => 72623859790382856
)
Array
(
    [1] => 72623859790382856
)
Array
(
    [1] => 72623859790382856
)
Array
(
    [1] => -2
)
Array
(
    [1] => -2
)
string(8) "02010403"
string(8) "01020304"
Array
(
    [1] => 258
    [2] => 772
)
Array
(
    [1] => 258
    [2] => 772
)
Array
(
    [value] => 258
    [count] => 5
)
pack('n<'): Endianness modifier not allowed for this format code
pack('v>'): Endianness modifier not allowed for this format code
pack('N<'): Endianness modifier not allowed for this format code
pack('V>'): Endianness modifier not allowed for this format code
pack('J<'): Endianness modifier not allowed for this format code
pack('P>'): Endianness modifier not allowed for this format code
pack('c<'): Endianness modifier not allowed for this format code
pack('C>'): Endianness modifier not allowed for this format code
pack('a<'): Endianness modifier not allowed for this format code
pack('A>'): Endianness modifier not allowed for this format code
pack('i<'): Endianness modifier not allowed for this format code
pack('I>'): Endianness modifier not allowed for this format code
pack('f<'): Endianness modifier not allowed for this format code
pack('d>'): Endianness modifier not allowed for this format code
unpack('n<'): Endianness modifier not allowed for this format code
unpack('v>'): Endianness modifier not allowed for this format code
unpack('N<'): Endianness modifier not allowed for this format code
unpack('V>'): Endianness modifier not allowed for this format code
