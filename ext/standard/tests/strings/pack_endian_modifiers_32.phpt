--TEST--
pack()/unpack() endianness modifiers on 32-bit systems
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) die("skip 32bit test only");
?>
--FILE--
<?php

var_dump(bin2hex(pack("s<", 0x0102)));
var_dump(bin2hex(pack("S<", 0x0102)));
var_dump(bin2hex(pack("s>", 0x0102)));
var_dump(bin2hex(pack("S>", 0x0102)));

print_r(unpack("s<", "\x02\x01"));
print_r(unpack("S>", "\x01\x02"));

var_dump(bin2hex(pack("l<", 0x01020304)));
var_dump(bin2hex(pack("L<", 0x01020304)));
var_dump(bin2hex(pack("l>", 0x01020304)));
var_dump(bin2hex(pack("L>", 0x01020304)));

print_r(unpack("l<", "\x04\x03\x02\x01"));
print_r(unpack("L>", "\x01\x02\x03\x04"));

$formats = ['q<', 'q>', 'Q<', 'Q>'];
foreach ($formats as $fmt) {
    try {
        pack($fmt, 0);
        echo "FAIL: Expected ValueError for pack('$fmt', 0)\n";
    } catch (ValueError $e) {
        echo "pack('$fmt'): " . $e->getMessage() . "\n";
    }
}

foreach ($formats as $fmt) {
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
Array
(
    [1] => 258
)
Array
(
    [1] => 258
)

string(8) "04030201"
string(8) "04030201"
string(8) "01020304"
string(8) "01020304"
Array
(
    [1] => 16909060
)
Array
(
    [1] => 16909060
)

pack('q<'): 64-bit format codes are not available for 32-bit versions of PHP
pack('q>'): 64-bit format codes are not available for 32-bit versions of PHP
pack('Q<'): 64-bit format codes are not available for 32-bit versions of PHP
pack('Q>'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('q<'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('q>'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('Q<'): 64-bit format codes are not available for 32-bit versions of PHP
unpack('Q>'): 64-bit format codes are not available for 32-bit versions of PHP
