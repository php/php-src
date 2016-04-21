--TEST--
basic union test inheritance
--FILE--
<?php
class Foo {
	public function method(array or Traversable $arg) {}
}

class Bar extends Foo {
	public function method($arg) {}
}
?>
--EXPECTF--
Warning: Declaration of Bar::method($arg) should be compatible with Foo::method(Traversable or array $arg) in %s on line 8
