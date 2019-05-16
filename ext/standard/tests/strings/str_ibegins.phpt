--TEST--
str_ibegins() function - unit tests for str_ibegins()
--FILE--
<?php
/* Prototype: boolean str_ibegins (string $haystack string $needle);
   Description: Performs case insensitive check to determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ibegins($testStr, "beginning"));
var_dump(str_ibegins($testStr, "Beginning"));
var_dump(str_ibegins($testStr, "eginning"));
var_dump(str_ibegins($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
