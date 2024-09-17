--TEST--
Int size-dependent input pack()/unpack() tests
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) {
    die("skip 32bit test only");
}
?>
--FILE--
<?php
echo "I\n";
print_r(unpack("I", pack("I", -1000)));
print_r(unpack("I", pack("I", -64434)));

echo "L\n";
print_r(unpack("L", asLittleEndian(hex2bin('02000080'))));
print_r(unpack("L", asLittleEndian(hex2bin('ffffffff'))));
print_r(unpack("L", asLittleEndian(hex2bin('00000080'))));

echo "N\n";
print_r(unpack("N", hex2bin('80000002')));
print_r(unpack("N", hex2bin('80000000')));
print_r(unpack("N", hex2bin('ffff8ad0')));

echo "V\n";
print_r(unpack("V", hex2bin('02000080')));
print_r(unpack("V", hex2bin('00000080')));

function asLittleEndian(string $bin): string
{
    static $isBe;
    if (!isset($isBe)) {
        $isBe = unpack('s', "\x00\x01")[1] == 1;
    }
    if ($isBe) {
        $bin = implode('', array_reverse(str_split($bin, 1)));
    }
    return $bin;
}
?>
--EXPECT--
I
Array
(
    [1] => -1000
)
Array
(
    [1] => -64434
)
L
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
N
Array
(
    [1] => -2147483646
)
Array
(
    [1] => -2147483648
)
Array
(
    [1] => -30000
)
V
Array
(
    [1] => -2147483646
)
Array
(
    [1] => -2147483648
)
