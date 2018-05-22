--TEST--
64bit pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
	die("skip 64bit test only");
}
?>
--FILE--
<?php
print_r(unpack("Q", pack("Q", 0xfffffffffffe)));
print_r(unpack("Q", pack("Q", 0)));
print_r(unpack("Q", pack("Q", 0x8000000000000002)));
print_r(unpack("Q", pack("Q", -1)));
print_r(unpack("Q", pack("Q", 0x8000000000000000)));

print_r(unpack("J", pack("J", 0xfffffffffffe)));
print_r(unpack("J", pack("J", 0)));
print_r(unpack("J", pack("J", 0x8000000000000002)));
print_r(unpack("J", pack("J", -1)));
print_r(unpack("J", pack("J", 0x8000000000000000)));

print_r(unpack("P", pack("P", 0xfffffffffffe)));
print_r(unpack("P", pack("P", 0)));
print_r(unpack("P", pack("P", 0x8000000000000002)));
print_r(unpack("P", pack("P", -1)));
print_r(unpack("P", pack("P", 0x8000000000000000)));

print_r(unpack("q", pack("q", 0xfffffffffffe)));
print_r(unpack("q", pack("q", 0)));
print_r(unpack("q", pack("q", 0x8000000000000002)));
print_r(unpack("q", pack("q", -1)));
print_r(unpack("q", pack("q", 0x8000000000000000)));
?>
--EXPECT--
Array
(
    [1] => 281474976710654
)
Array
(
    [1] => 0
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => -1
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => 281474976710654
)
Array
(
    [1] => 0
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => -1
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => 281474976710654
)
Array
(
    [1] => 0
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => -1
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => 281474976710654
)
Array
(
    [1] => 0
)
Array
(
    [1] => -9223372036854775808
)
Array
(
    [1] => -1
)
Array
(
    [1] => -9223372036854775808
)
