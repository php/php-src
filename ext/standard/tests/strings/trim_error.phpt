--TEST--
Test trim() function : error conditions
--FILE--
<?php

echo "*** Testing trim() : error conditions ***\n";

$hello = "  Hello World\n";
echo "\n-- Test trim function with various invalid charlists --\n";
var_dump(trim($hello, "..a"));
var_dump(trim($hello, "a.."));
var_dump(trim($hello, "z..a"));
var_dump(trim($hello, "a..b..c"));

?>
--EXPECTF--
*** Testing trim() : error conditions ***

-- Test trim function with various invalid charlists --

Warning: trim(): Invalid '..'-range, no character to the left of '..' in %s on line %d
string(14) "  Hello World
"

Warning: trim(): Invalid '..'-range, no character to the right of '..' in %s on line %d
string(14) "  Hello World
"

Warning: trim(): Invalid '..'-range, '..'-range needs to be incrementing in %s on line %d
string(14) "  Hello World
"

Warning: trim(): Invalid '..'-range in %s on line %d
string(14) "  Hello World
"
