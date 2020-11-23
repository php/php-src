--TEST--
Test preg_split() function : error conditions - Recursion limit exhausted
--INI--
pcre.recursion_limit=1
--FILE--
<?php

var_dump(preg_last_error_msg() === 'No error');
preg_split('/(\d*)/', 'ab2c3u');
var_dump(preg_last_error_msg() === 'Recursion limit exhausted');

?>
--EXPECT--
bool(true)
bool(true)
