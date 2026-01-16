--TEST--
Test str_suffix_remove() function
--FILE--
<?php

echo "*** Testing str_suffix_remove() : various strings ***\n";

$testStr = "beginningMiddleEnd";

// Matching suffix - should be removed
var_dump(str_suffix_remove("End", $testStr));

// Non-matching suffix (case sensitive) - should return original
var_dump(str_suffix_remove("end", $testStr));

// Partial match but not at end - should return original
var_dump(str_suffix_remove("en", $testStr));

// Remove entire string
var_dump(str_suffix_remove($testStr, $testStr));

// Suffix longer than source - should return original
var_dump(str_suffix_remove($testStr.$testStr, $testStr));

// Empty suffix - should return original
var_dump(str_suffix_remove("", $testStr));

// Both empty strings
var_dump(str_suffix_remove("", ""));

// Empty source, non-empty suffix
var_dump(str_suffix_remove(" ", ""));

// Null byte handling
var_dump(str_suffix_remove("\x00", $testStr));
var_dump(str_suffix_remove("", "\x00"));
var_dump(str_suffix_remove("\x00", "\x00"));
var_dump(str_suffix_remove("\x00", "a\x00"));
var_dump(str_suffix_remove("b\x00c", "ab\x00c"));
var_dump(str_suffix_remove("d\x00b", "a\x00b"));
var_dump(str_suffix_remove("a\x00z", "a\x00b"));
var_dump(str_suffix_remove("\x00a", "a"));
var_dump(str_suffix_remove("a\x00", "a"));

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
