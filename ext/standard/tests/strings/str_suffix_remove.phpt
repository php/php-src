--TEST--
Test str_suffix_remove() function
--FILE--
<?php

echo "*** Testing str_suffix_remove() : various strings ***\n";

$testStr = "beginningMiddleEnd";

// Matching suffix - should be removed
var_dump(str_suffix_remove($testStr, "End"));

// Non-matching suffix (case sensitive) - should return original
var_dump(str_suffix_remove($testStr, "end"));

// Partial match but not at end - should return original
var_dump(str_suffix_remove($testStr, "en"));

// Remove entire string
var_dump(str_suffix_remove($testStr, $testStr));

// Suffix longer than source - should return original
var_dump(str_suffix_remove($testStr, $testStr.$testStr));

// Empty suffix - should return original
var_dump(str_suffix_remove($testStr, ""));

// Both empty strings
var_dump(str_suffix_remove("", ""));

// Empty source, non-empty suffix
var_dump(str_suffix_remove("", " "));

// Null byte handling
var_dump(str_suffix_remove($testStr, "\x00"));
var_dump(str_suffix_remove("\x00", ""));
var_dump(str_suffix_remove("\x00", "\x00"));
var_dump(str_suffix_remove("a\x00", "\x00"));
var_dump(str_suffix_remove("ab\x00c", "b\x00c"));
var_dump(str_suffix_remove("a\x00b", "d\x00b"));
var_dump(str_suffix_remove("a\x00b", "a\x00z"));
var_dump(str_suffix_remove("a", "\x00a"));
var_dump(str_suffix_remove("a", "a\x00"));

?>
--EXPECTF--
*** Testing str_suffix_remove() : various strings ***
string(15) "beginningMiddle"
string(18) "beginningMiddleEnd"
string(18) "beginningMiddleEnd"
string(0) ""
string(18) "beginningMiddleEnd"
string(18) "beginningMiddleEnd"
string(0) ""
string(0) ""
string(18) "beginningMiddleEnd"
string(1) "%0"
string(0) ""
string(1) "a"
string(1) "a"
string(3) "a%0b"
string(3) "a%0b"
string(1) "a"
string(1) "a"
