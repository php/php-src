--TEST--
Bug #68546 (json_decode() Fatal error: Cannot access property started with '\0')
--SKIPIF--
<?php

if (!extension_loaded('json')) die('skip');
?>
--FILE--
<?php

var_dump(json_decode('{"key": {"\u0000": "aa"}}'));
var_dump(json_last_error() === JSON_ERROR_INVALID_PROPERTY_NAME);
var_dump(json_decode('[{"key1": 0, "\u0000": 1}]'));
var_dump(json_last_error() === JSON_ERROR_INVALID_PROPERTY_NAME);
var_dump(json_last_error_msg());

echo "Done\n";
?>
--EXPECT--
NULL
bool(true)
NULL
bool(true)
string(36) "The decoded property name is invalid"
Done
