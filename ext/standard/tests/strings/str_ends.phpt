--TEST--
str_ends() function - unit tests for str_ends()
--FILE--
<?php
/* Prototype: boolean str_ends (string $haystack string $needle );
   Description: Determine if $haystack ends with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ends($testStr, "End"));
var_dump(str_ends($testStr, "end"));
var_dump(str_ends($testStr, "en"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
