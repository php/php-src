--TEST--
Test str_suffix_replace() function
--FILE--
<?php

echo "*** Testing str_suffix_replace() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Ends with suffix - should be replaced
var_dump(str_suffix_replace("End", "Finish", $testStr));

// Does not end with suffix - should return original
var_dump(str_suffix_replace("Middle", "Center", $testStr));

// Replace with empty string (essentially remove)
var_dump(str_suffix_replace("End", "", $testStr));

// Replace empty suffix - should add suffix
var_dump(str_suffix_replace("", "New", $testStr));

// Replace with longer string
var_dump(str_suffix_replace(".txt", ".backup.txt", "file.txt"));

// Replace with shorter string
var_dump(str_suffix_replace(".html", ".md", "document.html"));

// Suffix longer than source - should return original
var_dump(str_suffix_replace("abcd", "xyz", "ab"));

// Source equals suffix - replace entire string
var_dump(str_suffix_replace("test", "replaced", "test"));

// Empty source
var_dump(str_suffix_replace("suffix", "new", ""));

// Both suffix and replace empty
var_dump(str_suffix_replace("", "", $testStr));

// Null byte handling
var_dump(str_suffix_replace("\x00", "b", "a\x00"));
var_dump(str_suffix_replace("\x00", "\x00", "a\x00"));
var_dump(str_suffix_replace("\x00c", "d", "ab\x00c"));
var_dump(str_suffix_replace("\x00c", "\x00d", "ab\x00c"));
var_dump(str_suffix_replace("c\x00b", "d", "a\x00b"));
var_dump(str_suffix_replace("st\x00", "new", "test\x00"));

?>
--EXPECTF--
*** Testing str_suffix_replace() : various strings ***
string(21) "BeginningMiddleFinish"
string(18) "BeginningMiddleEnd"
string(15) "BeginningMiddle"
string(21) "BeginningMiddleEndNew"
string(15) "file.backup.txt"
string(11) "document.md"
string(2) "ab"
string(8) "replaced"
string(0) ""
string(18) "BeginningMiddleEnd"
string(2) "ab"
string(2) "a%0"
string(3) "abd"
string(4) "ab%0d"
string(3) "a%0b"
string(5) "tenew"
