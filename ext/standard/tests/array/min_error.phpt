--TEST--
Test wrong number of arguments for min()
--FILE--
<?php
/* 
 * proto mixed min(mixed arg1 [, mixed arg2 [, mixed ...]])
 * Function is implemented in ext/standard/array.c
*/ 


echo "\n*** Testing Error Conditions ***\n";

var_dump(min());
var_dump(min(1));
var_dump(min(array()));
var_dump(min(new stdclass));

?>
--EXPECTF--
*** Testing Error Conditions ***

Warning: min() expects at least 1 parameter, 0 given in %s on line 10
NULL

Warning: min(): When only one parameter is given, it must be an array in %s on line 11
NULL

Warning: min(): Array must contain at least one element in %s on line 12
bool(false)

Warning: min(): When only one parameter is given, it must be an array in %s on line 13
NULL
