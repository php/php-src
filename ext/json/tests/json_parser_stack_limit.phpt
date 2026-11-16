--TEST--
JSON: nesting beyond the parser stack is reported as a depth error, not a syntax error
--FILE--
<?php
$deep = str_repeat('[', 100000) . str_repeat(']', 100000);
var_dump(json_decode($deep, true, 2147483647));
var_dump(json_last_error() === JSON_ERROR_DEPTH);
var_dump(json_validate($deep, 2147483647));
var_dump(json_last_error() === JSON_ERROR_DEPTH);

// Invalid input is still a syntax error
var_dump(json_validate('[1,]', 2147483647));
var_dump(json_last_error() === JSON_ERROR_SYNTAX);
?>
--EXPECT--
NULL
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
