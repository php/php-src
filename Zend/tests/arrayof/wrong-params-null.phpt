--TEST--
Basic arrayof test (null, error)
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test(null));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays, null given, called in %s on line %d and defined in %s on line %d



