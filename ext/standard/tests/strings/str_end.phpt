--TEST--
str_end() function - unit tests for str_end()
--FILE--
<?php
/* Prototype: boolean str_end (string $haystack string $needle );
   Description: Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_end($testStr, "End"));
var_dump(str_end($testStr, "end"));
var_dump(str_end($testStr, "en"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
