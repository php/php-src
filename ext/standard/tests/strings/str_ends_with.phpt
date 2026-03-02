--TEST--
str_ends_with() function - unit tests for str_ends_with()
--FILE--
<?php
$testStr = "beginningMiddleEnd";
var_dump(str_ends_with($testStr, "End"));
var_dump(str_ends_with($testStr, "end"));
var_dump(str_ends_with($testStr, "en"));
var_dump(str_ends_with($testStr, $testStr));
var_dump(str_ends_with($testStr, $testStr.$testStr));
var_dump(str_ends_with($testStr, ""));
var_dump(str_ends_with("", ""));
var_dump(str_ends_with("", " "));
var_dump(str_ends_with($testStr, "\x00"));
var_dump(str_ends_with("\x00", ""));
var_dump(str_ends_with("\x00", "\x00"));
var_dump(str_ends_with("a\x00", "\x00"));
var_dump(str_ends_with("ab\x00c", "b\x00c"));
var_dump(str_ends_with("a\x00b", "d\x00b"));
var_dump(str_ends_with("a\x00b", "a\x00z"));
var_dump(str_ends_with("a", "\x00a"));
var_dump(str_ends_with("a", "a\x00"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
