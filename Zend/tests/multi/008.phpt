--TEST--
basic union test inheritance
--FILE--
<?php
class Foo {
	public function method(array | Traversable $arg) {}
}

class Bar extends Foo {
	public function method($arg) {}
}
?>
--EXPECTF--
Warning: Declaration of Bar::method($arg) should be compatible with Foo::method(Traversable | array $arg) in %s on line 8
