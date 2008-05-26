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

Warning: min(): At least one %s on line %d
NULL

Warning: Wrong parameter count for min() in %s on line %d
NULL

Warning: min(): Array must contain at least one element in %s on line %d
bool(false)

Warning: Wrong parameter count for min() in %s on line %d
NULL
