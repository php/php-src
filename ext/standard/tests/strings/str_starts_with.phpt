--TEST--
str_starts_with() function - unit tests for str_starts_with()
--FILE--
<?php
/* Prototype: bool str_starts_with (string $haystack string $needle);
   Description: Determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_starts_with($testStr, "beginning"));                                                                             
var_dump(str_starts_with($testStr, "Beginning"));                                                                             
var_dump(str_starts_with($testStr, "eginning"));  
var_dump(str_starts_with($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)

