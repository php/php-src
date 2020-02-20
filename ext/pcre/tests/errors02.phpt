--TEST--
Test preg_split() function : error conditions - Malformed UTF-8
--FILE--
<?php

var_dump(preg_split('/a/u', "a\xff"));
var_dump(preg_last_error_msg() === 'Malformed UTF-8 characters, possibly incorrectly encoded');

?>
--EXPECT--
bool(false)
bool(true)
