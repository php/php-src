--TEST--
basic union test inheritance with class
--FILE--
<?php
class Foo {
	public function method() : IFoo & IBar {}
}

class Bar extends Foo {
	public function method() {}
}
?>
--EXPECTF--
Fatal error: Declaration of Bar::method() must be compatible with Foo::method(): IFoo & IBar in %s on line 8
