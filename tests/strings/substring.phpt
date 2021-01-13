--TEST--
substring() function - unit tests for substring()
--FILE--
<?php
$testStr = "The cat sat on the mat";
var_dump(substring($testStr, "sat on the", ""));
var_dump(substring($testStr, "The", "sat"));
var_dump(substring($testStr, "The", "dog"));
var_dump(substring($testStr, "T", "e"));
var_dump(substring($testStr, "mat", "The"));
var_dump(substring($testStr, "the", "sat"));
var_dump(substring($testStr, " cat ", " on "));
var_dump(substring($testStr, "", ""));
var_dump(substring($testStr, "", "sat"));
?>
--EXPECT--
string(3) "mat"
string(3) "cat"
string(0) ""
string(1) "h"
string(0) ""
string(0) ""
string(3) "sat"
string(22) "The cat sat on the mat"
string(7) "The cat"
