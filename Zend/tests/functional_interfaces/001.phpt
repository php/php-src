--TEST--
functional interfaces: basic test
--FILE--
<?php
interface IFoo {
	public function method();
}

$foo = function () implements IFoo {};

var_dump($foo instanceof IFoo,
		 $foo instanceof Closure);
--EXPECT--
bool(true)
bool(true)
