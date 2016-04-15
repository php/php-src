--TEST--
functional interfaces: non static implementation of static interface
--FILE--
<?php
interface IFoo {
	public static function method();
}

$cb = function () implements IFoo {};
--EXPECTF--
Fatal error: cannot create non static implementation of static functional interface IFoo in %s on line 6

