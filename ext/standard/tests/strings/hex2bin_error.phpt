--TEST--
hex2bin(); function test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php
var_dump(hex2bin('AH'));
var_dump(hex2bin('HA'));
?>
--EXPECTF--
Warning: hex2bin(): Input string must be hexadecimal string in %s on line %d
bool(false)

Warning: hex2bin(): Input string must be hexadecimal string in %s on line %d
bool(false)
