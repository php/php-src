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

// === Integer unpack with endianness modifiers ===

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

$inherent_formats = ['n<', 'v>', 'N<', 'V>', 'J<', 'P>'];
foreach ($inherent_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

$inherent_float_formats = ['g<', 'G>', 'e<', 'E>'];
foreach ($inherent_float_formats as $fmt) {
    try {
        pack($fmt, 1.0);
        echo "FAIL: Expected ValueError for pack('$fmt', 1.0)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

$unsupported_formats = ['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'];
foreach ($unsupported_formats as $fmt) {
    try {
        pack($fmt, 1);
        echo "FAIL: Expected ValueError for pack('$fmt', 1)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['n<', 'v>', 'N<', 'V>', 'J<', 'P>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['g<', 'G>', 'e<', 'E>'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
        echo "FAIL: Expected ValueError for unpack('$fmt', ...)\n";
    } catch (ValueError $e) {
        echo "unpack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach (['c<', 'C>', 'a<', 'A>', 'h<', 'H>', 'i<', 'I>', 'x<', 'X>', '@<'] as $fmt) {
    try {
        unpack($fmt, "\x00\x00\x00\x00\x00\x00\x00\x00");
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
pack('n<'): Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
pack('v>'): Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
pack('N<'): Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
pack('V>'): Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
pack('J<'): Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
pack('P>'): Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
pack('g<'): Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
pack('G>'): Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
pack('e<'): Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
pack('E>'): Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
pack('c<'): Endianness modifier is not supported for format code 'c'
pack('C>'): Endianness modifier is not supported for format code 'C'
pack('a<'): Endianness modifier is not supported for format code 'a'
pack('A>'): Endianness modifier is not supported for format code 'A'
pack('h<'): Endianness modifier is not supported for format code 'h'
pack('H>'): Endianness modifier is not supported for format code 'H'
pack('i<'): Endianness modifier is not supported for format code 'i'
pack('I>'): Endianness modifier is not supported for format code 'I'
pack('x<'): Endianness modifier is not supported for format code 'x'
pack('X>'): Endianness modifier is not supported for format code 'X'
pack('@<'): Endianness modifier is not supported for format code '@'
unpack('n<'): Endianness modifier '<' cannot be applied to format code 'n' which already has inherent endianness
unpack('v>'): Endianness modifier '>' cannot be applied to format code 'v' which already has inherent endianness
unpack('N<'): Endianness modifier '<' cannot be applied to format code 'N' which already has inherent endianness
unpack('V>'): Endianness modifier '>' cannot be applied to format code 'V' which already has inherent endianness
unpack('J<'): Endianness modifier '<' cannot be applied to format code 'J' which already has inherent endianness
unpack('P>'): Endianness modifier '>' cannot be applied to format code 'P' which already has inherent endianness
unpack('g<'): Endianness modifier '<' cannot be applied to format code 'g' which already has inherent endianness
unpack('G>'): Endianness modifier '>' cannot be applied to format code 'G' which already has inherent endianness
unpack('e<'): Endianness modifier '<' cannot be applied to format code 'e' which already has inherent endianness
unpack('E>'): Endianness modifier '>' cannot be applied to format code 'E' which already has inherent endianness
unpack('c<'): Endianness modifier is not supported for format code 'c'
unpack('C>'): Endianness modifier is not supported for format code 'C'
unpack('a<'): Endianness modifier is not supported for format code 'a'
unpack('A>'): Endianness modifier is not supported for format code 'A'
unpack('h<'): Endianness modifier is not supported for format code 'h'
unpack('H>'): Endianness modifier is not supported for format code 'H'
unpack('i<'): Endianness modifier is not supported for format code 'i'
unpack('I>'): Endianness modifier is not supported for format code 'I'
unpack('x<'): Endianness modifier is not supported for format code 'x'
unpack('X>'): Endianness modifier is not supported for format code 'X'
unpack('@<'): Endianness modifier is not supported for format code '@'
