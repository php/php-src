--TEST--
test unpack() to array with named keys
--FILE--
<?php
$str = pack('VVV', 0x00010203, 0x04050607, 0x08090a0b);
print_r(unpack('Vaa/Vbb/Vcc', $str));
print_r(unpack('V2aa/Vcc', $str));
print_r(unpack('V3aa', $str));
print_r(unpack('V*aa', $str));
print_r(unpack('V*', $str));
?>
--EXPECT--
Array
(
    [aa] => 66051
    [bb] => 67438087
    [cc] => 134810123
)
Array
(
    [aa1] => 66051
    [aa2] => 67438087
    [cc] => 134810123
)
Array
(
    [aa1] => 66051
    [aa2] => 67438087
    [aa3] => 134810123
)
Array
(
    [aa1] => 66051
    [aa2] => 67438087
    [aa3] => 134810123
)
Array
(
    [1] => 66051
    [2] => 67438087
    [3] => 134810123
)
