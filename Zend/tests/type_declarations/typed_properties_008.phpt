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
Fatal error: Property Bar::$qux must be of type int to be compatible with overridden property Foo::$qux in %s on line %d
