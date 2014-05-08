--TEST--
Inheritance Hinting Compile Checking Failure Same Class
--SKIPIF--
<?php if (!extension_loaded("Zend OPcache")) die("Skip: opcache required"); ?>
--FILE--
<?php
class Foo {
	public static function test() : self {
		return new Foo;
	}
}

class Bar extends Foo {
	public static function test() : self {
		return new Bar;
	}
}

var_dump(Bar::test());
var_dump(Foo::test());
--EXPECTF--
object(Bar)#%d (0) {
}
object(Foo)#%d (0) {
}

