--TEST--
str_iends() function - unit tests for str_iends()
--FILE--
<?php
/* Prototype: boolean str_iends (string $haystack string $needle );
   Description: Performs case insensitive check to Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_iends($testStr, "End"));
var_dump(str_iends($testStr, "end"));
var_dump(str_iends($testStr, "en"));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
