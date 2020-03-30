--TEST--
str_ends_with() function - unit tests for str_ends_with()
--FILE--
<?php
/* Prototype: bool str_ends_with (string $haystack string $needle );
   Description: Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ends_with($testStr, "End"));
var_dump(str_ends_with($testStr, "end"));
var_dump(str_ends_with($testStr, "en"));
var_dump(str_ends_with($testStr, $testStr."a"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)

