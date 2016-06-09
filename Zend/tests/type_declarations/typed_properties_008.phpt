--TEST--
Test typed properties inheritance (missing info)
--FILE--
<?php
class Foo {
	public int $qux;
}

class Bar extends Foo {
	public $qux;
}
?>
--EXPECTF--
Fatal error: Type of Bar::$qux must be integer (as in class Foo) in %s on line 8


