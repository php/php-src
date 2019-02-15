--TEST--
ZE2 class type hinting
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
Fatal error: Uncaught TypeError: Argument 1 passed to FooBar::a() must implement interface Foo, instance of Blort given, called in %s on line 27 and defined in %s:12
Stack trace:
#0 %s(%d): FooBar->a(Object(Blort))
#1 {main}
  thrown in %s on line 12
