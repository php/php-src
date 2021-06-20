--TEST--
Test typed properties disallow never
--FILE--
<?php
class Foo {
    public never $int;
}

$foo = new Foo();
?>
--EXPECTF--
Fatal error: Property Foo::$int cannot have type never in %s on line 3
