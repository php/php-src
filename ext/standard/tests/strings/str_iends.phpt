--TEST--
str_ends_with_ci() function - unit tests for str_ends_with_ci()
--FILE--
<?php
/* Prototype: boolean str_ends_with_ci (string $haystack string $needle );
   Description: Performs case insensitive check to Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ends_with_ci($testStr, "End"));
var_dump(str_ends_with_ci($testStr, "end"));
var_dump(str_ends_with_ci($testStr, "en"));
var_dump(str_ends_with_ci($testStr, $testStr."a"));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
