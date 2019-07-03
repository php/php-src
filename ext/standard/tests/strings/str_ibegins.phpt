--TEST--
str_begins_with_ci() function - unit tests for str_begins_with_ci()
--FILE--
<?php
/* Prototype: boolean str_begins_with_ci (string $haystack string $needle);
   Description: Performs case insensitive check to determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_begins_with_ci($testStr, "beginning"));
var_dump(str_begins_with_ci($testStr, "Beginning"));
var_dump(str_begins_with_ci($testStr, "eginning"));
var_dump(str_begins_with_ci($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
