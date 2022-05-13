--TEST--
Surprising result with integer literals (hex/octal)
--EXTENSIONS--
filter
--FILE--
<?php
echo 'Hex', \PHP_EOL;
var_dump(filter_var('0x', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('0xg', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('0X', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('0Xg', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
echo 'Octal', \PHP_EOL;
var_dump(filter_var('0o', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('0og', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('0O', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('0Og', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('O', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('Og', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
var_dump(filter_var('', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_OCTAL));
?>
--EXPECT--
Hex
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
Octal
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
