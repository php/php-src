--TEST--
Test rtrim() function : error conditions
--FILE--
<?php

echo "*** Testing rtrim() : error conditions ***\n";

$hello = "  Hello World\n";
echo "\n-- Test rtrim function with various invalid charlists\n";
var_dump(rtrim($hello, "..a"));
var_dump(rtrim($hello, "a.."));
var_dump(rtrim($hello, "z..a"));
var_dump(rtrim($hello, "a..b..c"));

?>
--EXPECTF--
*** Testing rtrim() : error conditions ***

-- Test rtrim function with various invalid charlists

Warning: rtrim(): Invalid '..'-range, no character to the left of '..' in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range, no character to the right of '..' in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range, '..'-range needs to be incrementing in %s on line %d
string(14) "  Hello World
"

Warning: rtrim(): Invalid '..'-range in %s on line %d
string(14) "  Hello World
"
