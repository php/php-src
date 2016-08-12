--TEST--
str_ends() function - unit tests for str_ends()
--FILE--
<?php
/* Prototype: boolean str_ends (string $str, string $search_value [, boolean $case_sensitive = true] );
   Description: Determine if $str ends with $search_value
*/
// First check with case sensitivity
$testStr1 = "beginningMiddleEnd";
var_dump(str_ends($testStr1, "End"));
var_dump(str_ends($testStr1, "end"));
var_dump(str_ends($testStr1, "en"));

// Now check with case insensitivity
var_dump(str_ends($testStr1, "End", false));
var_dump(str_ends($testStr1, "end", false));
var_dump(str_ends($testStr1, "en", false));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
