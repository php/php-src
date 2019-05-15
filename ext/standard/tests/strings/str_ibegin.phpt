--TEST--
str_ibegin() function - unit tests for str_ibegin()
--FILE--
<?php
/* Prototype: boolean str_ibegin (string $haystack string $needle);
   Description: Performs case insensitive check to determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ibegin($testStr, "beginning"));
var_dump(str_ibegin($testStr, "Beginning"));
var_dump(str_ibegin($testStr, "eginning"));
var_dump(str_begin($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
