--TEST--
max() tests
--INI--
precision=14
--FILE--
<?php

var_dump(max());
var_dump(max(1));
var_dump(max(array()));
var_dump(max(new stdclass));
var_dump(max(2,1,2));
var_dump(max(2.1,2.11,2.09));
var_dump(max("", "t", "b"));
var_dump(max(false, true, false));
var_dump(max(true, false, true));
var_dump(max(1, true, false, true));
var_dump(max(0, true, false, true));

echo "Done\n";
?>
--EXPECTF--
Warning: max() expects at least 1 parameter, 0 given in %s on line 3
NULL

Warning: max(): When only one parameter is given, it must be an array in %s on line 4
NULL

Warning: max(): Array must contain at least one element in %s on line 5
bool(false)

Warning: max(): When only one parameter is given, it must be an array in %s on line 6
NULL
int(2)
float(2.11)
string(1) "t"
bool(true)
bool(true)
int(1)
bool(true)
Done
