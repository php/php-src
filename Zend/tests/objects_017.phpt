--TEST--
Testing visibility of object returned by function
--FILE--
<?php

class foo {
	private $test = 1;
}

function test() {
	return new foo;
}

test()->test = 2;

?>
--EXPECTF--
Fatal error: Cannot access private property foo::$test in %s on line %d
