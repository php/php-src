--TEST--
functional interfaces: static implementation of non static interface
--FILE--
<?php
interface IFoo {
	public function method();
}

$cb = static function () implements IFoo {};
--EXPECTF--
Fatal error: cannot create static implementation of non static functional interface IFoo in %s on line 6

