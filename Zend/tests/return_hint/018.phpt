--TEST--
Fully qualified classes in trait return types
--FILE--
<?php
class Test {}

function test() : &Test {
	return new Test();
}

test();
--EXPECTF--
Notice: Only variable references should be returned by reference in %s on line %d

