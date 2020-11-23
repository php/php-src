--TEST--
str_starts_with() function - unit tests for str_starts_with()
--FILE--
<?php
$testStr = "beginningMiddleEnd";
var_dump(str_starts_with($testStr, "beginning"));
var_dump(str_starts_with($testStr, "Beginning"));
var_dump(str_starts_with($testStr, "eginning"));
var_dump(str_starts_with($testStr, $testStr));
var_dump(str_starts_with($testStr, $testStr.$testStr));
var_dump(str_starts_with($testStr, ""));
var_dump(str_starts_with("", ""));
var_dump(str_starts_with("", " "));
var_dump(str_starts_with($testStr, "\x00"));
var_dump(str_starts_with("\x00", ""));
var_dump(str_starts_with("\x00", "\x00"));
var_dump(str_starts_with("\x00a", "\x00"));
var_dump(str_starts_with("a\x00bc", "a\x00b"));
var_dump(str_starts_with("a\x00b", "a\x00d"));
var_dump(str_starts_with("a\x00b", "z\x00b"));
var_dump(str_starts_with("a", "a\x00"));
var_dump(str_starts_with("a", "\x00a"));
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
