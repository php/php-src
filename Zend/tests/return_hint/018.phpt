--TEST--
Raise error when return type information is not grouped
--FILE--
<?php
class Test {}

function & test() : Test {
	return new Test();
}

test();
--EXPECTF--
Fatal error: the function test returns by reference and declares a return type; the & must be placed immediately before the return type and not before the function name. in %s on line %d


