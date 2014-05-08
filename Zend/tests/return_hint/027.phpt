--TEST--
Inheritance Hinting Compile Checking Failure
--SKIPIF--
<?php if (!extension_loaded("Zend OPcache")) die("Skip: opcache required"); ?>
--FILE--
<?php
class Foo {
	public static function test() : Traversable {
		return new ArrayIterator([1, 2]);
	}
}

class Bar extends Foo {
	public static function test() : ArrayObject {
		return new ArrayObject([1, 2]);
	}
}

var_dump(Bar::test());
var_dump(Foo::test());
--EXPECTF--
object(ArrayIterator)#%d (0) {
}
object(ArrayObject)#%d (0) {
}

