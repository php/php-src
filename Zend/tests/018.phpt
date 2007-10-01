--TEST--
constant() tests
--FILE--
<?php

var_dump(constant());
var_dump(constant("", ""));
var_dump(constant(""));

var_dump(constant(array()));

define("TEST_CONST", 1);
var_dump(constant("TEST_CONST"));

define("TEST_CONST2", "test");
var_dump(constant("TEST_CONST2"));

echo "Done\n";
?>
--EXPECTF--	
Warning: constant() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: constant() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: constant(): Couldn't find constant  in %s on line %d
NULL

Warning: constant() expects parameter 1 to be string, array given in %s on line %d
NULL
int(1)
string(4) "test"
Done
