--TEST--
Basic return hints at execution mismatched basic type
--FILE--
<?php
function test1() : array {
	return "hello";
}

test1()
?>
--EXPECTF--
Catchable fatal error: the function test1 was expected to return an array and returned a string in %s on line %d
