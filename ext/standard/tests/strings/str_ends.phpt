--TEST--
str_begins() function - unit tests for str_begins()
--FILE--
<?php
/* Prototype: boolean str_ends (string $str, string $search_value [, boolean $case_sensitive = true] );
   Description: Determine if $str ends with $search_value
*/
// First check with case sensitivity
$testStr1 = "beginningMiddleEnd";
var_dump(str_begins($testStr1, "End"));
var_dump(str_begins($testStr1, "end"));
var_dump(str_begins($testStr1, "nd"));

// Now check with case insensitivity
var_dump(str_begins($testStr1, "End", false));
var_dump(str_begins($testStr1, "end", false));
var_dump(str_begins($testStr1, "nd", false));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
