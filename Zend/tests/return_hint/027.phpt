--TEST--
Inheritance Hinting Compile Checking Failure External Classes With OpCache
--SKIPIF--
<?php if (!extension_loaded("Zend OPcache")) die("Skip: opcache required"); ?>
--INI--
opcache.enable_cli=1
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

