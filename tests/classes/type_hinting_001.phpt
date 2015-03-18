--TEST--
ZE2 class type hinting
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

interface Foo {
	function a(Foo $foo);
}

interface Bar {
	function b(Bar $bar);
}

class FooBar implements Foo, Bar {
	function a(Foo $foo) {
		// ...
	}

	function b(Bar $bar) {
		// ...
	}
}

class Blort {
}

$a = new FooBar;
$b = new Blort;

$a->a($b);
$a->b($b);

?>
--EXPECTF--

Fatal error: Argument 1 passed to FooBar::a() must implement interface Foo, instance of Blort given, called in %s on line 27 and defined in %s on line 12
