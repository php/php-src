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
Fatal error: Property Bar::$qux must be of type Whatever to be compatible with overridden property Foo::$qux in %s on line %d
