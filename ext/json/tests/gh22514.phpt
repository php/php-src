--TEST--
GH-22514: Incorrect error column in JSON parser after escape sequences
--FILE--
<?php

// The error is on the '_' on line 2. The ÿ escape occupies six source
// characters, so the column must count each of them: '_' is at column 10.
var_dump(json_decode("\n{\"\\u00FF\"_"));
var_dump(json_last_error_msg());

// A real multibyte UTF-8 character counts as a single column, so the same
// logical value reached via raw UTF-8 yields a smaller column than the escape.
var_dump(json_decode("\n{\"\xC3\xBF\"_"));
var_dump(json_last_error_msg());

?>
--EXPECT--
NULL
string(31) "Syntax error near location 2:10"
NULL
string(30) "Syntax error near location 2:5"
