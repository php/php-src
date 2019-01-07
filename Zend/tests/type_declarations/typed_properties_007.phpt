--TEST--
Test typed properties inheritance
--FILE--
<?php
class Whatever {}
class Thing extends Whatever {}

class Foo {
	public Whatever $qux;
}

class Bar extends Foo {
	public Thing $qux;
}
?>
--EXPECTF--
Fatal error: Type of Bar::$qux must be Whatever (as in class Foo) in %s on line 11
