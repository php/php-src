--TEST--
str_iend() function - unit tests for str_iend()
--FILE--
<?php
/* Prototype: boolean str_iend (string $search_value string $str );
   Description: Performs case insensitive check to Determine if $str ends with $search_value
*/
$testStr = "beginningMiddleEnd";
var_dump(str_iend("End", $testStr));
var_dump(str_iend("end", $testStr));
var_dump(str_iend("en", $testStr));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
