--TEST--
Int size-dependent input pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_SIZE < 8) {
    die("skip 64bit test only");
}
?>
--FILE--
<?php
echo "I\n";
print_r(unpack("I", hex2bin('18fcffff')));
print_r(unpack("I", hex2bin('4e04ffff')));

echo "L\n";
print_r(unpack("L", hex2bin('02000080')));
print_r(unpack("L", hex2bin('ffffffff')));
print_r(unpack("L", hex2bin('00000080')));

echo "N\n";
print_r(unpack("N", hex2bin('80000002')));
print_r(unpack("N", hex2bin('80000000')));
print_r(unpack("N", hex2bin('ffff8ad0')));

echo "V\n";
print_r(unpack("V", hex2bin('02000080')));
print_r(unpack("V", hex2bin('00000080')));
?>
--EXPECT--
I
Array
(
    [1] => 4294966296
)
Array
(
    [1] => 4294902862
)
L
Array
(
    [1] => 2147483650
)
Array
(
    [1] => 4294967295
)
Array
(
    [1] => 2147483648
)
N
Array
(
    [1] => 2147483650
)
Array
(
    [1] => 2147483648
)
Array
(
    [1] => 4294937296
)
V
Array
(
    [1] => 2147483650
)
Array
(
    [1] => 2147483648
)
