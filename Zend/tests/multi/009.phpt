--TEST--
basic union test inheritance with class
--FILE--
<?php
class Foo {
	public function method(IFoo & IBar $arg) {}
}

class Bar extends Foo {
	public function method($arg) {}
}
?>
--EXPECTF--
Warning: Declaration of Bar::method($arg) should be compatible with Foo::method(IFoo & IBar $arg) in %s on line 8
