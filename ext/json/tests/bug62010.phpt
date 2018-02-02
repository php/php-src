--TEST--
Bug #62010 (json_decode produces invalid byte-sequences)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

var_dump(json_decode('"\ud834"'));
var_dump(json_last_error() === JSON_ERROR_UTF16);
var_dump(json_last_error_msg());
?>
--EXPECTF--
NULL
bool(true)
string(50) "Single unpaired UTF-16 surrogate in unicode escape"
