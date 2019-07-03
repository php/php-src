--TEST--
str_begins_with() function - unit tests for str_begins_with()
--FILE--
<?php
/* Prototype: boolean str_begins_with (string $haystack string $needle);
   Description: Determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_begins_with($testStr, "beginning"));                                                                             
var_dump(str_begins_with($testStr, "Beginning"));                                                                             
var_dump(str_begins_with($testStr, "eginning"));  
var_dump(str_begins_with($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
