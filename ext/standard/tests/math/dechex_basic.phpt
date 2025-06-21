--TEST--
Test dechex() - basic function dechex()
--FILE--
<?php
$values = [
    10,
    035,
    0x5F,
    0xFFF,
    245,
];

foreach ($values as $value) {
    var_dump(dechex($value));
}

?>
--EXPECT--
string(1) "a"
string(2) "1d"
string(2) "5f"
string(3) "fff"
string(2) "f5"
