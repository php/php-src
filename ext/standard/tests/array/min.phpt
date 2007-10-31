--TEST--
min() tests
--INI--
precision=14
--FILE--
<?php

var_dump(min());
var_dump(min(1));
var_dump(min(array()));
var_dump(min(new stdclass));
var_dump(min(2,1,2));
var_dump(min(2.1,2.11,2.09));
var_dump(min("", "t", "b"));
var_dump(min(false, true, false));
var_dump(min(true, false, true));
var_dump(min(1, true, false, true));
var_dump(min(0, true, false, true));

echo "Done\n";
?>
--EXPECTF--	
Warning: min(): At least one value should be passed in %s on line %d
NULL

Warning: Wrong parameter count for min() in %s on line %d
NULL

Warning: min(): Array must contain at least one element in %s on line %d
bool(false)

Warning: Wrong parameter count for min() in %s on line %d
NULL
int(1)
float(2.09)
string(0) ""
bool(false)
bool(false)
bool(false)
int(0)
Done
