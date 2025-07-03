--TEST--
Test decbin() - basic function test
--FILE--
<?php

$values = [
    10,
    0o35,
    0x5F,
    0xFFF,
    245,
    0b1010101,
];

foreach ($values as $value) {
    var_dump(decbin($value));
}

?>
--EXPECT--
string(4) "1010"
string(5) "11101"
string(7) "1011111"
string(12) "111111111111"
string(8) "11110101"
string(7) "1010101"
