--TEST--
functional interfaces: non interface
--FILE--
<?php
class IFoo {
	public function method(string $arg) : void {}
}

$foo = function (string $arg) implements IFoo : void {};
--EXPECTF--
Fatal error: cannot implement non interface IFoo in %s on line 6



