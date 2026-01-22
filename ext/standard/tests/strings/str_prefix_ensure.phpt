--TEST--
Test str_prefix_ensure() function
--FILE--
<?php

echo "*** Testing str_prefix_ensure() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Does not start with prefix - should be added
var_dump(str_prefix_ensure($testStr, "The"));

// Already starts with prefix - should return original
var_dump(str_prefix_ensure($testStr, "Beginning"));

// Empty prefix - should return original
var_dump(str_prefix_ensure($testStr, ""));

// Empty source - should add prefix
var_dump(str_prefix_ensure("", "prefix"));

// Both empty strings
var_dump(str_prefix_ensure("", ""));

// Add prefix to empty string with space
var_dump(str_prefix_ensure("", " "));

// Source shorter than prefix but doesn't start with it
var_dump(str_prefix_ensure("a", "abc"));

// Source equals prefix - should return original
var_dump(str_prefix_ensure("test", "test"));

// Null byte handling
var_dump(str_prefix_ensure($testStr, "\x00"));
var_dump(str_prefix_ensure("\x00", ""));
var_dump(str_prefix_ensure("\x00", "\x00"));
var_dump(str_prefix_ensure("\x00a", "\x00"));
var_dump(str_prefix_ensure("a", "\x00"));
var_dump(str_prefix_ensure("c\x00ab", "\x00ab"));
var_dump(str_prefix_ensure("\x00ab", "c\x00"));
var_dump(str_prefix_ensure("a\x00b", "a\x00d"));
var_dump(str_prefix_ensure("a\x00b", "a\x00z"));

?>
--EXPECTF--
*** Testing str_prefix_ensure() : various strings ***
string(21) "TheBeginningMiddleEnd"
string(18) "BeginningMiddleEnd"
string(18) "BeginningMiddleEnd"
string(6) "prefix"
string(0) ""
string(1) " "
string(4) "abca"
string(4) "test"
string(19) "%0BeginningMiddleEnd"
string(1) "%0"
string(1) "%0"
string(2) "%0a"
string(2) "%0a"
string(7) "%0abc%0ab"
string(5) "c%0%0ab"
string(6) "a%0da%0b"
string(6) "a%0za%0b"
