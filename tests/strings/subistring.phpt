--TEST--
subistring() function - unit tests for subistring()
--FILE--
<?php
$testStr = "The CAT sat on the Mat";
var_dump(subistring($testStr, "sat on the", ""));
var_dump(subistring($testStr, "The", "sat"));
var_dump(subistring($testStr, "The", "dog"));
var_dump(subistring($testStr, "T", "e"));
var_dump(subistring($testStr, "mat", "The"));
var_dump(subistring($testStr, "the", "sat"));
var_dump(subistring($testStr, " cat ", " on "));
var_dump(subistring($testStr, "", ""));
var_dump(subistring($testStr, "", "SAT"));
?>
--EXPECT--
string(3) "Mat"
string(3) "CAT"
string(0) ""
string(1) "h"
string(0) ""
string(3) "CAT"
string(3) "sat"
string(22) "The CAT sat on the Mat"
string(7) "The CAT"
