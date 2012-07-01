--TEST--
Bug #61537 (json_encode() incorrectly truncates/discards information)
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php
$invalid_utf8 = "\x9f";
var_dump(json_encode($invalid_utf8), json_last_error());
var_dump(json_encode($invalid_utf8, JSON_PARTIAL_OUTPUT_ON_ERROR), json_last_error());

$invalid_utf8 = "an invalid sequen\xce in the middle of a string";
var_dump(json_encode($invalid_utf8), json_last_error());
var_dump(json_encode($invalid_utf8, JSON_PARTIAL_OUTPUT_ON_ERROR), json_last_error());
?>
--EXPECTF--
Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
bool(false)
int(5)

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
string(4) "null"
int(5)

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
bool(false)
int(5)

Warning: json_encode(): Invalid UTF-8 sequence in argument in %s on line %d
string(4) "null"
int(5)
