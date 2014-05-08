--TEST--
Inheritance Hinting Compile Checking Failure External Classes
--SKIPIF--
<?php if (!extension_loaded("Zend OPcache")) die("Skip: opcache required"); ?>
--FILE--
<?php
class A {}
class B extends A {}

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
object(A)#%d (0) {
}
object(B)#%d (0) {
}

