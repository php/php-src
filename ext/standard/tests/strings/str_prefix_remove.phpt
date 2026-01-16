--TEST--
Test str_prefix_remove() function
--FILE--
<?php

echo "*** Testing str_prefix_remove() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Matching prefix - should be removed
var_dump(str_prefix_remove("Beginning", $testStr));

// Non-matching prefix (case sensitive) - should return original
var_dump(str_prefix_remove("beginning", $testStr));

// Partial match but not at start - should return original
var_dump(str_prefix_remove("Middle", $testStr));

// Remove entire string
var_dump(str_prefix_remove($testStr, $testStr));

// Prefix longer than source - should return original
var_dump(str_prefix_remove($testStr.$testStr, $testStr));

// Empty prefix - should return original
var_dump(str_prefix_remove("", $testStr));

// Both empty strings
var_dump(str_prefix_remove("", ""));

// Empty source, non-empty prefix
var_dump(str_prefix_remove(" ", ""));

// Null byte handling
var_dump(str_prefix_remove("\x00", $testStr));
var_dump(str_prefix_remove("", "\x00"));
var_dump(str_prefix_remove("\x00", "\x00"));
var_dump(str_prefix_remove("\x00", "\x00a"));
var_dump(str_prefix_remove("\x00bc", "\x00bca"));
var_dump(str_prefix_remove("d\x00a", "b\x00a"));
var_dump(str_prefix_remove("z\x00a", "b\x00a"));
var_dump(str_prefix_remove("a\x00", "a"));
var_dump(str_prefix_remove("\x00a", "a"));

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
