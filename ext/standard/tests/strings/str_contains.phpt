--TEST--
Test str_contains() function
--FILE--
<?php
var_dump(str_contains("test string", "test"));
var_dump(str_contains("test string", "string"));
var_dump(str_contains("test string", "strin"));
var_dump(str_contains("test string", "t s"));
var_dump(str_contains("test string", "g"));
var_dump(str_contains("te".chr(0)."st", chr(0)));
var_dump(str_contains("tEst", "test"));
var_dump(str_contains("teSt", "test"));
var_dump(str_contains("", ""));
var_dump(str_contains("a", ""));
var_dump(str_contains("", "a"));
var_dump(str_contains("\\\\a", "\\a"));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
