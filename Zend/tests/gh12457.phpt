--TEST--
GH-12458 (Fix GH-12457: Fixed a bug in zend_memnistr)
--FILE--
<?php
echo "Test case to ensure the issue is fixed.\n";
var_dump(stripos('aaBBBBBb', 'b'));
var_dump(stripos('aaBBBBBbb', 'b'));
var_dump(stripos('aaBBBBBbbb', 'b'));
var_dump(stristr('aaBBBBBb', 'b'));
var_dump(stristr('aaBBBBBbb', 'b'));
var_dump(stristr('aaBBBBBbbb', 'b'));

echo "\n";
echo "Test cases to ensure the original functionality is not broken.\n";
var_dump(stripos('aaBBBBBbc', 'c'));
var_dump(stripos('aaBBBBBbC', 'c'));
var_dump(stristr('aaBBBBBbc', 'c'));
var_dump(stristr('aaBBBBBbC', 'c'));
?>
--EXPECTF--
Test case to ensure the issue is fixed.
int(2)
int(2)
int(2)
string(6) "BBBBBb"
string(7) "BBBBBbb"
string(8) "BBBBBbbb"

Test cases to ensure the original functionality is not broken.
int(8)
int(8)
string(1) "c"
string(1) "C"
