--TEST--
Test typed properties inheritance (scalar)
--FILE--
<?php
class Foo {
    public int $qux;
}

class Bar extends Foo {
    public string $qux;
}
?>
--EXPECTF--
Fatal error: Property Bar::$qux must be of type int to be compatible with overridden property Foo::$qux in %s on line %d
