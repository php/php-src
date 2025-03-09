--TEST--
Bug #62010 (json_decode produces invalid byte-sequences)
--FILE--
<?php

var_dump(json_decode('"\ud834"'));
var_dump(json_last_error() === JSON_ERROR_UTF16);
var_dump(json_last_error_msg());
?>
--EXPECT--
NULL
bool(true)
string(89) "Single unpaired UTF-16 surrogate in unicode escape, at character 0 near content: "\ud834""
