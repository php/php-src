--TEST--
hex2bin(); function test
--CREDITS--
edgarsandi - <edgar.r.sandi@gmail.com>
--FILE--
<?php

var_dump(bin2hex(hex2bin('012345')) == '012345');
var_dump(bin2hex(hex2bin('abc123')) == 'abc123');
var_dump(bin2hex(hex2bin('123abc')) == '123abc');
var_dump(bin2hex(hex2bin('FFFFFF')) == 'ffffff');

?>
--EXPECTF--
bool(true)
bool(true)
bool(true)
bool(true)
