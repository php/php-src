--TEST--
Bug #62010 (json_decode produces invalid byte-sequences)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_decode('"\ud834"'));
var_dump(json_last_error() === JSON_ERROR_UTF16);
var_dump(json_last_error_msg());
?>
--EXPECT--
NULL
bool(true)
string(50) "Single unpaired UTF-16 surrogate in unicode escape"
