--TEST--
GH-22420: json_encode() errors don't clear the error position
--FILE--
<?php

var_dump(json_decode('{132'));
var_dump(json_last_error_msg());
var_dump(json_encode("\xFE\xD0"));
var_dump(json_last_error_msg());

?>
--EXPECTF--
NULL
string(30) "Syntax error near location 1:2"
bool(false)
string(56) "Malformed UTF-8 characters, possibly incorrectly encoded"
