--TEST--
Test typed properties disallow void
--FILE--
<?php
class Foo {
    public void $int;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Property Foo::$int cannot be of type void in %s on line %d
