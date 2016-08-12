--TEST--
str_end() function - unit tests for str_end()
--FILE--
<?php
/* Prototype: boolean str_end (string $search_value string $str );
   Description: Determine if $str ends with $search_value
*/
$testStr = "beginningMiddleEnd";
var_dump(str_end("End", $testStr));
var_dump(str_end("end", $testStr));
var_dump(str_end("en", $testStr));
?>
--EXPECT--
bool(true)
bool(false)
bool(false)
