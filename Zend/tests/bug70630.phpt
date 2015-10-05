--TEST--
Bug #70630 (Closure::call/bind() crash with ReflectionFunction->getClosure())
--FILE--
<?php

class a {}
function foo() { print "ok\n"; }

foreach (["substr", "foo"] as $fn) {
	$x = (new ReflectionFunction($fn))->getClosure();
	$x->call(new a);
	Closure::bind($x, new a, "a");
}

?>
--EXPECTF--

Warning: substr() expects at least 2 parameters, 0 given in %s on line %d

Warning: Cannot bind function substr to a class scope in %s on line %d
ok

Warning: Cannot bind function foo to a class scope in %s on line %d
