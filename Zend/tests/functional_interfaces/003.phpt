--TEST--
functional interfaces: verify pass
--FILE--
<?php
interface IFoo {
	public function method(string $arg) : void;
}

$foo = function (string $arg) implements IFoo : void {};

var_dump($foo instanceof IFoo,
		 $foo instanceof Closure);
--EXPECTF--
bool(true)
bool(true)

