--TEST--
Basic return hints at execution
--FILE--
<?php
function test1() : array {
	return null;
}

test1();
?>
--EXPECTF--
Catchable fatal error: the function test1 was expected to return an array and returned null in %s on line %d


