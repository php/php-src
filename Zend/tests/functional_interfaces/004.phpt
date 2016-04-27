--TEST--
functional interfaces: not functional interface
--FILE--
<?php
interface IFoo {
	public function method(string $arg) : void;
	public function other();
}

$foo = function (string $arg) implements IFoo : void {};
--EXPECTF--
Fatal error: cannot implement non functional interface IFoo in %s on line 7



