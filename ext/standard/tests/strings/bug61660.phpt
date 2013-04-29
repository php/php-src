--TEST--
Bug #61660: bin2hex(hex2bin($data)) != $data
--FILE--
<?php

var_dump(hex2bin('123'));

?>
--EXPECTF--
Warning: hex2bin(): Hexadecimal input string must have an even length in %s on line %d
bool(false)
