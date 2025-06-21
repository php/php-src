--TEST--
Test decoct() - basic function test decoct()
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
    var_dump(decoct($value));
}

?>
--EXPECT--
string(2) "12"
string(2) "35"
string(3) "137"
string(4) "7777"
string(3) "365"
