--TEST--
Bug #40685 (mb_decode_numericentity() removes '&' in the string)
--EXTENSIONS--
mbstring
--FILE--
<?php
$map = array(0, 0x10FFFF, 0, 0xFFFFFF);
var_dump(mb_decode_numericentity('&', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&&&', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#x', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#61', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#x3d', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#61;', $map, 'UTF-8'));
var_dump(mb_decode_numericentity('&#x3d;', $map, 'UTF-8'));
?>
--EXPECT--
string(1) "&"
string(3) "&&&"
string(2) "&#"
string(3) "&#x"
string(1) "="
string(1) "="
string(1) "="
string(1) "="
