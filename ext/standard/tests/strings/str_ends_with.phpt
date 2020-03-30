--TEST--
str_ends_with() function - unit tests for str_ends_with()
--FILE--
<?php
/* Prototype: bool str_ends_with (string $haystack, string $needle);
   Description: Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ends_with($testStr, "End"));
var_dump(str_ends_with($testStr, "end"));
var_dump(str_ends_with($testStr, "en"));
var_dump(str_ends_with($testStr, $testStr."a"));
var_dump(str_ends_with($testStr, ""));
var_dump(str_ends_with("", ""));
var_dump(str_ends_with("", " "));
var_dump(str_ends_with("\x00", ""));
var_dump(str_ends_with("\x00", "\x00"));
var_dump(str_ends_with("str", "\x00"));
var_dump(str_ends_with("cat\x00rat", "\x00"));
var_dump(str_ends_with("cat\x00rat", "rat"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)

