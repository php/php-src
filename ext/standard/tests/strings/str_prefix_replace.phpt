--TEST--
Test str_prefix_replace() function
--FILE--
<?php

echo "*** Testing str_prefix_replace() : various strings ***\n";

$testStr = "BeginningMiddleEnd";

// Starts with prefix - should be replaced
var_dump(str_prefix_replace("Beginning", "Start", $testStr));

// Does not start with prefix - should return original
var_dump(str_prefix_replace("Middle", "Center", $testStr));

// Replace with empty string (essentially remove)
var_dump(str_prefix_replace("Beginning", "", $testStr));

// Replace empty prefix - should add prefix
var_dump(str_prefix_replace("", "New", $testStr));

// Replace with longer string
var_dump(str_prefix_replace("http://", "https://", "http://example.com"));

// Replace with shorter string
var_dump(str_prefix_replace("/usr/local/", "/opt/", "/usr/local/bin"));

// Prefix longer than source - should return original
var_dump(str_prefix_replace("abcd", "xyz", "ab"));

// Source equals prefix - replace entire string
var_dump(str_prefix_replace("test", "replaced", "test"));

// Empty source
var_dump(str_prefix_replace("prefix", "new", ""));

// Both prefix and replace empty
var_dump(str_prefix_replace("", "", $testStr));

// Null byte handling
var_dump(str_prefix_replace("\x00", "b", "\x00a"));
var_dump(str_prefix_replace("\x00", "\x00", "\x00a"));
var_dump(str_prefix_replace("\x00c", "d", "\x00cab"));
var_dump(str_prefix_replace("\x00c", "\x00d", "\x00cab"));
var_dump(str_prefix_replace("c\x00a", "d", "b\x00a"));
var_dump(str_prefix_replace("\x00te", "new", "\x00test"));

?>
--EXPECTF--
*** Testing str_prefix_replace() : various strings ***
string(14) "StartMiddleEnd"
string(18) "BeginningMiddleEnd"
string(9) "MiddleEnd"
string(21) "NewBeginningMiddleEnd"
string(19) "https://example.com"
string(8) "/opt/bin"
string(2) "ab"
string(8) "replaced"
string(0) ""
string(18) "BeginningMiddleEnd"
string(2) "ba"
string(2) "%0a"
string(3) "dab"
string(4) "%0dab"
string(3) "b%0a"
string(5) "newst"
