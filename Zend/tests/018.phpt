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
Warning: Wrong parameter count for constant() in %s on line %d
NULL

Warning: Wrong parameter count for constant() in %s on line %d
NULL

Warning: constant(): Couldn't find constant  in %s on line %d
NULL

Notice: Array to string conversion in %s on line %d

Warning: constant(): Couldn't find constant Array in %s on line %d
NULL
int(1)
string(4) "test"
Done
