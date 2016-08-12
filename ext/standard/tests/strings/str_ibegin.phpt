--TEST--
str_ibegin() function - unit tests for str_ibegin()
--FILE--
<?php
/* Prototype: boolean str_ibegin (string $search_value string $str);
   Description: Performs case insensitive check to determine if $str begins with $search_value
*/
$testStr = "beginningMiddleEnd";
var_dump(str_ibegin("beginning", $testStr));
var_dump(str_ibegin("Beginning", $testStr));
var_dump(str_ibegin("eginning", $testStr));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
