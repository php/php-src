--TEST--
Scalar type hint - disallow relative typehints
--FILE--
<?php

function foo(bar\int $a): int {
	return $a;
}

foo(10);

?>
--EXPECTF--
Fatal error: Argument 1 passed to foo() must be an instance of bar\int, integer given, called in %s on line %d and defined in %s on line %d