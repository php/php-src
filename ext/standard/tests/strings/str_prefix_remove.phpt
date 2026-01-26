--TEST--
Test str_prefix_remove() function
--FILE--
<?php

echo "*** Testing str_prefix_remove() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Matching prefix - should be removed
var_dump(str_prefix_remove($testStr, "Beginning"));

// Non-matching prefix (case sensitive) - should return original
var_dump(str_prefix_remove($testStr, "beginning"));

// Partial match but not at start - should return original
var_dump(str_prefix_remove($testStr, "Middle"));

// Remove entire string
var_dump(str_prefix_remove($testStr, $testStr));

// Prefix longer than source - should return original
var_dump(str_prefix_remove($testStr, $testStr.$testStr));

// Empty prefix - should return original
var_dump(str_prefix_remove($testStr, ""));

// Both empty strings
var_dump(str_prefix_remove("", ""));

// Empty source, non-empty prefix
var_dump(str_prefix_remove("", " "));

// Null byte handling
var_dump(str_prefix_remove($testStr, "\x00"));
var_dump(str_prefix_remove("\x00", ""));
var_dump(str_prefix_remove("\x00", "\x00"));
var_dump(str_prefix_remove("\x00a", "\x00"));
var_dump(str_prefix_remove("\x00bca", "\x00bc"));
var_dump(str_prefix_remove("b\x00a", "d\x00a"));
var_dump(str_prefix_remove("b\x00a", "z\x00a"));
var_dump(str_prefix_remove("a", "a\x00"));
var_dump(str_prefix_remove("a", "\x00a"));

?>
--EXPECTF--
*** Testing str_prefix_remove() : various strings ***
string(9) "MiddleEnd"
string(18) "BeginningMiddleEnd"
string(18) "BeginningMiddleEnd"
string(0) ""
string(18) "BeginningMiddleEnd"
string(18) "BeginningMiddleEnd"
string(0) ""
string(0) ""
string(18) "BeginningMiddleEnd"
string(1) "%0"
string(0) ""
string(1) "a"
string(1) "a"
string(3) "b%0a"
string(3) "b%0a"
string(1) "a"
string(1) "a"
