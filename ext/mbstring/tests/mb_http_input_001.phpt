--TEST--
mb_http_input() - Returns FALSE for $type 'L' or 'l'
--EXTENSIONS--
mbstring
--INI--
input_encoding=-1
--FILE--
<?php
var_dump(mb_http_input('L'));
var_dump(mb_http_input('l'));
?>
--EXPECT--
Warning: PHP Startup: INI setting contains invalid encoding "-1" in Unknown on line 0

Warning: PHP Startup: INI setting contains invalid encoding "-1" in Unknown on line 0
bool(false)
bool(false)
