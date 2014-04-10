--TEST--
test null passed in arrayof arg
--FILE--
<?php
function test(array[] $arrays) {
	return $arrays;
}

var_dump(test([null]));
?>
--EXPECTF--
Catchable fatal error: Argument 1 passed to test() must be an array of arrays, null at index 0, called in %s on line %d and defined in %s on line %d



