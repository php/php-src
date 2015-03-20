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
Fatal error: "bar\int" cannot be used as a type declaration in %s on line %d