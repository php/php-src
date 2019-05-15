--TEST--
str_iend() function - unit tests for str_iend()
--FILE--
<?php
/* Prototype: boolean str_iend (string $haystack string $needle );
   Description: Performs case insensitive check to Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_iend($testStr, "End"));
var_dump(str_iend($testStr, "end"));
var_dump(str_iend($testStr, "en"));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
