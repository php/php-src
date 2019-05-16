--TEST--
str_begins() function - unit tests for str_begins()
--FILE--
<?php
/* Prototype: boolean str_begins (string $haystack string $needle);
   Description: Determine if $haystack begins with $needle
*/
$testStr = "beginningMiddleEnd";
var_dump(str_begins($testStr, "beginning"));                                                                             
var_dump(str_begins($testStr, "Beginning"));                                                                             
var_dump(str_begins($testStr, "eginning"));  
var_dump(str_begins($testStr, $testStr."a"));  
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(false)
