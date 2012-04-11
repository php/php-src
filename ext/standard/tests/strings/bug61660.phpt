--TEST--
Bug #61660: bin2hex(hex2bin($data)) != $data
--FILE--
<?php

var_dump(hex2bin('123'));
var_dump(hex2bin('123', HEX2BIN_PARTIAL_OUTPUT_ON_ERROR));

?>
--EXPECTF--
Warning: hex2bin(): Hexadecimal input string must have an even length and cannot be empty in %s on line %d
bool(false)
string(1) ""
