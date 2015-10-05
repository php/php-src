--TEST--
Bug #70630 (Closure::call/bind() crash with ReflectionFunction->getClosure())
--FILE--
<?php

class a {}
function foo() {}

foreach (["substr", "foo"] as $fn) {
	$x = (new ReflectionFunction($fn))->getClosure();
	$x->call(new a);
	Closure::bind($x, new a, "a");
}

?>
--EXPECTF--

Warning: Cannot bind function substr to an object in %s on line %d

Warning: Cannot bind function substr to an object or class in %s on line %d

Warning: Cannot bind function foo to an object in %s on line %d

Warning: Cannot bind function foo to an object or class in %s on line %d
