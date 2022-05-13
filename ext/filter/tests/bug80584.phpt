--TEST--
Bug #80584: "0x" and "0X" are considered valid hex numbers by filter_var()
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var('0x', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('0X', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
var_dump(filter_var('0', FILTER_VALIDATE_INT, FILTER_FLAG_ALLOW_HEX));
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
int(0)
