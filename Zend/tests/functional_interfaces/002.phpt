--TEST--
functional interfaces: verify fail
--FILE--
<?php
interface IFoo {
	public function method(string $arg) : void;
}

$foo = function () implements IFoo {};
--EXPECTF--
Fatal error: Declaration of {IFoo}::method() must be compatible with IFoo::method(string $arg): void in %s on line 6

