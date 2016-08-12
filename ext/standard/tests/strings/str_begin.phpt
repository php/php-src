--TEST--
str_begin() function - unit tests for str_begin()
--FILE--
<?php
/* Prototype: boolean str_begin (string $search_value string $str);
   Description: Determine if $str begins with $search_value
*/
$testStr = "beginningMiddleEnd";
var_dump(str_begin("beginning", $testStr));
var_dump(str_begin("Beginning", $testStr));
var_dump(str_begin("eginning", $testStr));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
