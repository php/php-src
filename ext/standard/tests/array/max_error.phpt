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

Warning: max(): At least one %s on line %d
NULL

Warning: Wrong parameter count for max() in %s on line %d
NULL

Warning: max(): Array must contain at least one element in %s on line %d
bool(false)

Warning: Wrong parameter count for max() in %s on line %d
NULL
