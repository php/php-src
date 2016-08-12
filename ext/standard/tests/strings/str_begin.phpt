--TEST--
str_begin() function - unit tests for str_begin()
--FILE--
<?php
/* Prototype: boolean str_begin (string $search_value string $str);
   Description: Determine if $str begins with $search_value
*/
$testStr1 = "beginningMiddleEnd";
var_dump(str_begins($testStr1, "beginning"));
var_dump(str_begins($testStr1, "Beginning"));
var_dump(str_begins($testStr1, "eginning"));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
