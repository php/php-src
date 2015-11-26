--TEST--
Test wrong number of arguments for min()
--FILE--
<?php
/* 
 * proto mixed min(mixed arg1 [, mixed arg2 [, mixed ...]])
 * Function is implemented in ext/standard/array.c
*/ 


echo "\n*** Testing Error Conditions ***\n";

var_dump(max());
var_dump(max(1));
var_dump(max(array()));
var_dump(max(new stdclass));

?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: max() expects at least 1 parameter, 0 given in %s on line 10
NULL

Warning: max(): When only one parameter is given, it must be an array in %s on line 11
NULL

Warning: max(): Array must contain at least one element in %s on line 12
bool(false)

Warning: max(): When only one parameter is given, it must be an array in %s on line 13
NULL
