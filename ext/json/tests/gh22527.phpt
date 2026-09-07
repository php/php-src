--TEST--
GH-22527: Unterminated JSON strings are misleadingly reported as “Control character error”
--FILE--
<?php

var_dump(json_decode('"'));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_decode('"123'));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_decode('{"123": "'));
var_dump(json_last_error(), json_last_error_msg());

var_dump(json_decode('"' . chr(0)));
var_dump(json_last_error(), json_last_error_msg());

?>
--EXPECT--
NULL
int(4)
string(30) "Syntax error near location 1:1"
NULL
int(4)
string(30) "Syntax error near location 1:1"
NULL
int(4)
string(30) "Syntax error near location 1:9"
NULL
int(3)
string(71) "Control character error, possibly incorrectly encoded near location 1:1"
