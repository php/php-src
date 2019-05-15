--TEST--
str_begin() function - unit tests for str_begin()
--FILE--
<?php
/* Prototype: boolean str_begin (string $haystack string $needle);
   Description: Determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_begin($testStr, "beginning"));                                                                             
var_dump(str_begin($testStr, "Beginning"));                                                                             
var_dump(str_begin($testStr, "eginning"));  
var_dump(str_begin($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
