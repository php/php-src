--TEST--
Generic pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_MAX > 2147483647) {
	die("skip 32bit test only");
}
?>
--FILE--
<?php
print_r(unpack("A", pack("A", "hello world")));
print_r(unpack("A*", pack("A*", "hello world")));
echo '"'.pack("A9", "hello").'"';
echo "\n";

print_r(unpack("C", pack("C", -127)));
print_r(unpack("C", pack("C", 127)));
print_r(unpack("C", pack("C", 255)));
print_r(unpack("C", pack("C", -129)));

print_r(unpack("H", pack("H", 0x04)));

print_r(unpack("I", pack("I", 65534)));
print_r(unpack("I", pack("I", 0)));
print_r(unpack("I", pack("I", -1000)));
print_r(unpack("I", pack("I", -64434)));
print_r(unpack("I", pack("I", 4294967296)));
print_r(unpack("I", pack("I", -4294967296)));

print_r(unpack("L", pack("L", 65534)));
print_r(unpack("L", pack("L", 0)));
print_r(unpack("L", pack("L", 2147483650)));
print_r(unpack("L", pack("L", 4294967295)));
print_r(unpack("L", pack("L", -2147483648)));

print_r(unpack("N", pack("N", 65534)));
print_r(unpack("N", pack("N", 0)));
print_r(unpack("N", pack("N", 2147483650)));
print_r(unpack("N", pack("N", 4294967296)));
print_r(unpack("N", pack("N", -2147483648)));
print_r(unpack("N", pack("N", -30000)));

print_r(unpack("S", pack("S", 65534)));
print_r(unpack("S", pack("S", 65537)));
print_r(unpack("S", pack("S", 0)));
print_r(unpack("S", pack("S", -1000)));
print_r(unpack("S", pack("S", -64434)));
print_r(unpack("S", pack("S", -65535)));

print_r(unpack("V", pack("V", 65534)));
print_r(unpack("V", pack("V", 0)));
print_r(unpack("V", pack("V", 2147483650)));
print_r(unpack("V", pack("V", 4294967296)));
print_r(unpack("V", pack("V", -2147483648)));

print_r(unpack("a", pack("a", "hello world")));
print_r(unpack("a*", pack("a*", "hello world")));

print_r(unpack("c", pack("c", -127)));
print_r(unpack("c", pack("c", 127)));
print_r(unpack("c", pack("c", 255)));
print_r(unpack("c", pack("c", -129)));

print_r(unpack("h", pack("h", 3000000)));

print_r(unpack("i", pack("i", 65534)));
print_r(unpack("i", pack("i", 0)));
print_r(unpack("i", pack("i", -1000)));
print_r(unpack("i", pack("i", -64434)));
print_r(unpack("i", pack("i", -65535)));
print_r(unpack("i", pack("i", -2147483647)));

print_r(unpack("l", pack("l", 65534)));
print_r(unpack("l", pack("l", 0)));
print_r(unpack("l", pack("l", 2147483650)));
print_r(unpack("l", pack("l", 4294967296)));
print_r(unpack("l", pack("l", -2147483648)));

print_r(unpack("n", pack("n", 65534)));
print_r(unpack("n", pack("n", 65537)));
print_r(unpack("n", pack("n", 0)));
print_r(unpack("n", pack("n", -1000)));
print_r(unpack("n", pack("n", -64434)));
print_r(unpack("n", pack("n", -65535)));

print_r(unpack("s", pack("s", 32767)));
print_r(unpack("s", pack("s", 65535)));
print_r(unpack("s", pack("s", 0)));
print_r(unpack("s", pack("s", -1000)));
print_r(unpack("s", pack("s", -64434)));
print_r(unpack("s", pack("s", -65535)));

print_r(unpack("v", pack("v", 65534)));
print_r(unpack("v", pack("v", 65537)));
print_r(unpack("v", pack("v", 0)));
print_r(unpack("v", pack("v", -1000)));
print_r(unpack("v", pack("v", -64434)));
print_r(unpack("v", pack("v", -65535)));
?>
--EXPECTF--
Array
(
    [1] => h
)
Array
(
    [1] => hello world
)
"hello    "
Array
(
    [1] => 129
)
Array
(
    [1] => 127
)
Array
(
    [1] => 255
)
Array
(
    [1] => 127
)
Array
(
    [1] => 4
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -1000
)
Array
(
    [1] => -64434
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483646
)
Array
(
    [1] => -1
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483646
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => -30000
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 1
)
Array
(
    [1] => 0
)
Array
(
    [1] => 64536
)
Array
(
    [1] => 1102
)
Array
(
    [1] => 1
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483646
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => h
)
Array
(
    [1] => hello world
)
Array
(
    [1] => -127
)
Array
(
    [1] => 127
)
Array
(
    [1] => -1
)
Array
(
    [1] => 127
)
Array
(
    [1] => 3
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -1000
)
Array
(
    [1] => -64434
)
Array
(
    [1] => -65535
)
Array
(
    [1] => -2147483647
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483646
)
Array
(
    [1] => 0
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 1
)
Array
(
    [1] => 0
)
Array
(
    [1] => 64536
)
Array
(
    [1] => 1102
)
Array
(
    [1] => 1
)
Array
(
    [1] => 32767
)
Array
(
    [1] => -1
)
Array
(
    [1] => 0
)
Array
(
    [1] => -1000
)
Array
(
    [1] => 1102
)
Array
(
    [1] => 1
)
Array
(
    [1] => 65534
)
Array
(
    [1] => 1
)
Array
(
    [1] => 0
)
Array
(
    [1] => 64536
)
Array
(
    [1] => 1102
)
Array
(
    [1] => 1
)
