--TEST--
Basic arrayof test (simple types, error)
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test([array(), 1]));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays , member of type integer given, called in %s on line %d and defined in %s on line %d


