--TEST--
test no argument passed for array args
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test());
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays, none given, called in %s on line %d and defined in %s on line %d



