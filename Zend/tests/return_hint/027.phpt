--TEST--
Inheritance Hinting Compile Checking Failure External Classes
--SKIPIF--
<?php if (!extension_loaded("Zend OPcache")) die("Skip: opcache required"); ?>
--FILE--
<?php
require __DIR__ . "/classes.php";

class Foo {
	public static function test() : A {
		return new A;
	}
}

class Bar extends Foo {
	public static function test() : B {
		return new B;
	}
}

var_dump(Bar::test());
var_dump(Foo::test());
--EXPECTF--
object(B)#%d (0) {
}
object(A)#%d (0) {
}

