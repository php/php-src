--TEST--
Test str_suffix_ensure() function
--FILE--
<?php

echo "*** Testing str_suffix_ensure() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Does not end with suffix - should be added
var_dump(str_suffix_ensure("ing", $testStr));

// Already ends with suffix - should return original
var_dump(str_suffix_ensure("End", $testStr));

// Empty suffix - should return original
var_dump(str_suffix_ensure("", $testStr));

// Empty source - should add suffix
var_dump(str_suffix_ensure("suffix", ""));

// Both empty strings
var_dump(str_suffix_ensure("", ""));

// Add suffix to empty string with space
var_dump(str_suffix_ensure(" ", ""));

// Source shorter than suffix but doesn't end with it
var_dump(str_suffix_ensure("abc", "a"));

// Source equals suffix - should return original
var_dump(str_suffix_ensure("test", "test"));

// Null byte handling
var_dump(str_suffix_ensure("\x00", $testStr));
var_dump(str_suffix_ensure("", "\x00"));
var_dump(str_suffix_ensure("\x00", "\x00"));
var_dump(str_suffix_ensure("\x00", "a\x00"));
var_dump(str_suffix_ensure("\x00", "a"));
var_dump(str_suffix_ensure("\x00c", "ab\x00c"));
var_dump(str_suffix_ensure("\x00c", "ab\x00"));
var_dump(str_suffix_ensure("d\x00a", "b\x00a"));
var_dump(str_suffix_ensure("z\x00a", "b\x00a"));

?>
--EXPECTF--
*** Testing str_suffix_ensure() : various strings ***
string(21) "BeginningMiddleEnding"
string(18) "BeginningMiddleEnd"
string(18) "BeginningMiddleEnd"
string(6) "suffix"
string(0) ""
string(1) " "
string(4) "aabc"
string(4) "test"
string(19) "BeginningMiddleEnd%0"
string(1) "%0"
string(1) "%0"
string(2) "a%0"
string(2) "a%0"
string(4) "ab%0c"
string(5) "ab%0%0c"
string(6) "b%0ad%0a"
string(6) "b%0az%0a"
